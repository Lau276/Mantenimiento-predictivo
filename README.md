# Monitoreo de Vibraciones y Mantenimiento Predictivo en Motores Eléctricos

Este proyecto implementa un sistema embebido enfocado en el mantenimiento predictivo y monitoreo de condición de motores eléctricos, utilizando un microcontrolador ESP32-S3 y un acelerómetro MPU6050.

📌 Características Principales:
* Adquisición de Datos: Muestreo continuo a 500 Hz sincronizado por hardware mediante el pin de interrupción (`INT`).
* Análisis en Frecuencia (FFT): Procesamiento de señales con la Transformada Rápida de Fourier para convertir lecturas de aceleración en el tiempo al dominio de la frecuencia.
* Diagnóstico de Fallas: Detección de picos armónicos característicos asociados a problemas como desbalanceo mecánico , desalineación o solturas.
* Evaluación de Severidad: Clasificación de la condición operativa del motor basada en los criterios de la norma ISO 10816/20816.
