#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <arduinoFFT.h>
#include "pagina.h"

// ====================================================================
// WIFI (mismos datos que tu boot.py). Si no conecta, crea su propia red.
// ====================================================================
const char* WIFI_SSID = "te";
const char* WIFI_PASS = "Eliza123";
const char* AP_SSID   = "Vibracion-ESP32";   // red propia (clave: 12345678)

// ====================================================================
// PINES (ESP32-S3) Y REGISTROS
// ====================================================================
#define I2C_SDA_PIN       8
#define I2C_SCL_PIN       9
#define MPU_INT_PIN       10
#define I2C_FREQUENCY     400000L
#define MPU6050_ADDR      0x68

#define MPU6050_SMPLRT_DIV    0x19
#define MPU6050_CONFIG        0x1A
#define MPU6050_INT_ENABLE    0x38
#define MPU6050_PWR_MGMT_1    0x6B
#define MPU6050_ACCEL_XOUT_H  0x3B
#define ESCALA_ACCEL_LSB      16384.0f

// DLPF: 0x03 = ~44 Hz (atenua el 2X a 50 Hz). 0x02 = ~94 Hz, 0x01 = ~184 Hz.
#define MPU_DLPF              0x02

// ====================================================================
// FFT (1024 muestras @ 500 Hz)
// ====================================================================
#define SAMPLES        1024
#define SAMPLING_FREQ  500
#define NBINS          (SAMPLES / 2)
#define VENTANA_GAIN   0.54f      // ganancia coherente de la ventana Hamming

float vReal[SAMPLES];
float vImag[SAMPLES];
uint16_t sampleIndex = 0;

ArduinoFFT<float> FFT = ArduinoFFT<float>(vReal, vImag, SAMPLES, SAMPLING_FREQ);

// Resultados compartidos con el servidor web
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
uint16_t espectro_mg[NBINS];      // amplitud de cada bin en mg
float    r_pico = 0, r_vel = 0, r_arms = 0;
uint32_t r_bloques = 0;

TaskHandle_t tareaH = NULL;
WebServer server(80);

// ====================================================================
// INTERRUPCION E I2C
// ====================================================================
void IRAM_ATTR mpuISR() {
  BaseType_t despertar = pdFALSE;
  if (tareaH) vTaskNotifyGiveFromISR(tareaH, &despertar);
  if (despertar) portYIELD_FROM_ISR();
}

void writeRegister(uint8_t regAddr, uint8_t value) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(regAddr);
  Wire.write(value);
  Wire.endTransmission();
}

int16_t readAccelX() {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(MPU6050_ACCEL_XOUT_H);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)MPU6050_ADDR, (uint8_t)2);
  if (Wire.available() == 2) {
    uint8_t hi = Wire.read();
    uint8_t lo = Wire.read();
    return (int16_t)((hi << 8) | lo);
  }
  return 0;
}

// ====================================================================
// PROCESAMIENTO DE LA FFT
// ====================================================================
void procesarFFT() {
  // Quitar la componente continua (gravedad) para que no tape el espectro
  float media = 0;
  for (uint16_t i = 0; i < SAMPLES; i++) media += vReal[i];
  media /= SAMPLES;
  for (uint16_t i = 0; i < SAMPLES; i++) vReal[i] -= media;

  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();
  float pico = FFT.majorPeak();

  const float deltaF = (float)SAMPLING_FREQ / SAMPLES;
  const float k = 2.0f / (SAMPLES * VENTANA_GAIN);   // magnitud -> amplitud en g

  uint16_t tmp[NBINS];
  float vel2 = 0, a2 = 0;
  tmp[0] = 0;
  for (uint16_t i = 1; i < NBINS; i++) {
    float amp = vReal[i] * k;                          // g (pico)
    float mg = amp * 1000.0f;
    tmp[i] = mg > 65535 ? 65535 : (uint16_t)mg;
    a2 += amp * amp * 0.5f;                            // RMS^2 en g^2
    float f = i * deltaF;
    if (f >= 10.0f) {                                  // ISO: desde 10 Hz
      float a = amp * 0.7071f * 9.80665f;              // m/s2 RMS
      float v = a / (6.2832f * f) * 1000.0f;           // mm/s RMS
      vel2 += v * v;
    }
  }

  portENTER_CRITICAL(&mux);
  memcpy(espectro_mg, tmp, sizeof(tmp));
  r_pico = pico;
  r_vel = sqrtf(vel2);
  r_arms = sqrtf(a2);
  r_bloques++;
  portEXIT_CRITICAL(&mux);

  Serial.printf("Bloque %lu | pico %.2f Hz | vel %.2f mm/s | acc RMS %.4f g\n",
                (unsigned long)r_bloques, pico, r_vel, r_arms);
}

// Tarea de muestreo: se despierta con cada interrupcion del MPU6050
void tareaMuestreo(void*) {
  for (;;) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    vReal[sampleIndex] = (float)readAccelX() / ESCALA_ACCEL_LSB;
    vImag[sampleIndex] = 0.0f;
    sampleIndex++;
    if (sampleIndex >= SAMPLES) {
      procesarFFT();
      sampleIndex = 0;
      ulTaskNotifyTake(pdTRUE, 0);   // descarta interrupciones perdidas durante la FFT
    }
  }
}

// ====================================================================
// SERVIDOR WEB
// ====================================================================
void handleRoot() {
  server.send_P(200, "text/html; charset=utf-8", PAGINA);
}

void handleData() {
  uint16_t copia[NBINS];
  float pico, vel, arms;
  uint32_t bloques;
  portENTER_CRITICAL(&mux);
  memcpy(copia, espectro_mg, sizeof(copia));
  pico = r_pico; vel = r_vel; arms = r_arms; bloques = r_bloques;
  portEXIT_CRITICAL(&mux);

  String j;
  j.reserve(NBINS * 5 + 120);
  j += "{\"listo\":"; j += (bloques > 0 ? "true" : "false");
  j += ",\"bloques\":"; j += bloques;
  j += ",\"pico\":"; j += String(pico, 2);
  j += ",\"vel\":"; j += String(vel, 3);
  j += ",\"arms\":"; j += String(arms, 4);
  j += ",\"spec\":[";
  for (uint16_t i = 0; i < NBINS; i++) {
    if (i) j += ',';
    j += copia[i];
  }
  j += "]}";
  server.send(200, "application/json", j);
}

void iniciarWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Conectando al WiFi");
  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\nConectado. Abrir: http://");
    Serial.println(WiFi.localIP());
  } else {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, "12345678");
    Serial.print("\nSin WiFi. Red propia '");
    Serial.print(AP_SSID);
    Serial.print("'. Abrir: http://");
    Serial.println(WiFi.softAPIP());
  }
}

// ====================================================================
// SETUP Y LOOP
// ====================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(I2C_FREQUENCY);

  writeRegister(MPU6050_PWR_MGMT_1, 0x00);        // despertar
  delay(50);
  writeRegister(MPU6050_CONFIG, MPU_DLPF);
  writeRegister(MPU6050_SMPLRT_DIV, 1);           // 1000 / (1 + 1) = 500 Hz
  writeRegister(MPU6050_INT_ENABLE, 0x01);        // interrupcion de dato listo

  // Tarea de muestreo en el nucleo 1, con prioridad alta
  xTaskCreatePinnedToCore(tareaMuestreo, "muestreo", 8192, NULL, 5, &tareaH, 1);

  pinMode(MPU_INT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MPU_INT_PIN), mpuISR, RISING);

  iniciarWiFi();
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Servidor HTTP listo. Capturando bloques de 1024 muestras...");
}

void loop() {
  server.handleClient();
  delay(2);
}
