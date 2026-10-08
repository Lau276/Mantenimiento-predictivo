#pragma once
const char PAGINA[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="es"><head><meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>VibraMon · Monitor de vibración</title>
<style>
:root{--bg:#eef1f3;--pn:#fff;--ink:#14212b;--mut:#5b6b78;--ln:#d5dce1;--ac:#0e6b7a;--sp:#0e6b7a;--za:#2f9e6e;--zb:#8fb63a;--zc:#e0a21b;--zd:#d6452f}
@media(prefers-color-scheme:dark){:root{--bg:#0f171d;--pn:#16212a;--ink:#e6edf1;--mut:#93a4b0;--ln:#2a3945;--ac:#4fc1d3;--sp:#4fc1d3}}
*{margin:0;padding:0;box-sizing:border-box}
body{background:var(--bg);color:var(--ink);font:15px/1.55 system-ui,-apple-system,"Segoe UI",Roboto,sans-serif;-webkit-text-size-adjust:100%}
.w{max-width:960px;margin:0 auto;padding:0 16px}
header{display:flex;justify-content:space-between;align-items:center;gap:12px;padding:14px 0}
header b{font-size:18px;letter-spacing:-.01em}
.st{display:flex;align-items:center;gap:8px;font-size:13px;color:var(--mut)}
.dot{width:10px;height:10px;border-radius:50%;background:var(--mut)}
.dot.ok{background:var(--za)}.dot.err{background:var(--zd)}
.panel{background:var(--pn);border:1px solid var(--ln);border-radius:10px;padding:20px}
.hero{display:grid;grid-template-columns:1.5fr 1fr;gap:16px;margin-bottom:16px}
.zone h1{font-size:14px;font-weight:600;color:var(--mut)}
.big{font-size:clamp(44px,9vw,68px);font-weight:700;letter-spacing:-.03em;line-height:1.05;font-variant-numeric:tabular-nums;margin-top:4px}
.big small{font-size:.32em;font-weight:500;color:var(--mut);letter-spacing:0;margin-left:6px}
.zl{font-size:18px;font-weight:600;margin:6px 0 22px;min-height:28px}
.scale{position:relative;height:12px;border-radius:6px;display:flex;overflow:visible;margin-bottom:6px}
.scale i{flex:1;display:block}
.scale i:nth-child(1){background:var(--za);border-radius:6px 0 0 6px}.scale i:nth-child(2){background:var(--zb)}.scale i:nth-child(3){background:var(--zc)}.scale i:nth-child(4){background:var(--zd);border-radius:0 6px 6px 0}
#mk{position:absolute;top:-6px;left:0;width:4px;height:24px;margin-left:-2px;background:var(--ink);border-radius:2px;border:2px solid var(--pn);box-sizing:content-box;transition:left .6s ease}
.ticks{display:flex;font-size:11px;color:var(--mut)}
.ticks span{flex:1}.ticks span:last-child{text-align:right}
.ticks span:not(:first-child):not(:last-child){text-align:center}
.side{display:flex;flex-direction:column;gap:16px}
.side .panel{flex:1;padding:16px 20px}
.k{font-size:13px;color:var(--mut)}
.v{font-size:30px;font-weight:700;letter-spacing:-.02em;font-variant-numeric:tabular-nums}
.v small{font-size:14px;font-weight:500;color:var(--mut);margin-left:4px}
.h{display:flex;justify-content:space-between;align-items:flex-end;flex-wrap:wrap;gap:12px;margin-bottom:14px}
h2{font-size:18px;letter-spacing:-.01em}
.h p{font-size:13px;color:var(--mut);margin-top:2px}
label{font-size:13px;color:var(--mut);display:flex;align-items:center;gap:8px}
input{width:96px;font:inherit;font-size:14px;padding:6px 8px;border:1px solid var(--ln);border-radius:6px;background:var(--bg);color:var(--ink)}
input:focus-visible{outline:2px solid var(--ac);outline-offset:1px}
.cvw{position:relative;margin:0 -6px}
canvas{display:block;width:100%;height:320px;touch-action:pan-y}
.rd{font-size:14px;margin-top:12px;min-height:44px}
.rd b{font-variant-numeric:tabular-nums}
.f{margin:16px 0}
.f h2{margin-bottom:4px}
.f>p{color:var(--mut);font-size:13px;margin-bottom:12px}
table{width:100%;border-collapse:collapse;font-size:14px}
th,td{text-align:left;padding:10px 12px 10px 0;border-top:1px solid var(--ln);vertical-align:top}
th{font-weight:600;white-space:nowrap}
td:last-child{color:var(--mut)}
tr.on th,tr.on td{color:var(--ink)}
tr.on th::before{content:"";display:inline-block;width:8px;height:8px;border-radius:50%;background:var(--zc);margin-right:8px}
footer{font-size:12px;color:var(--mut);padding:8px 0 28px}
@media(max-width:700px){.hero{grid-template-columns:1fr}.side{flex-direction:row}canvas{height:240px}.panel{padding:16px}td:last-child{display:none}}
@media(max-width:420px){.side{flex-direction:column}}
</style></head><body>
<div class="w">
<header><b>VibraMon</b><div class="st"><span class="dot" id="dot"></span><span id="estado">Conectando…</span></div></header>

<div class="hero">
<div class="panel zone"><h1>Velocidad de vibración (RMS, 10–250 Hz)</h1>
<div class="big"><span id="vel">--</span><small>mm/s</small></div>
<div class="zl" id="zl">Midiendo el primer bloque…</div>
<div class="scale"><i></i><i></i><i></i><i></i><div id="mk"></div></div>
<div class="ticks"><span>0</span><span>0,71</span><span>1,8</span><span>4,5</span><span>7,1+</span></div>
</div>
<div class="side">
<div class="panel"><div class="k">Frecuencia pico</div><div class="v"><span id="fpk">--</span><small>Hz</small></div></div>
<div class="panel"><div class="k">RPM equivalente</div><div class="v"><span id="rpm">--</span><small>rpm</small></div></div>
</div></div>

<div class="panel">
<div class="h"><div><h2>Espectro de aceleración</h2><p>Eje X, 0–250 Hz. Tocá o pasá el cursor para leer un valor.</p></div>
<label>RPM nominales <input id="nom" type="number" inputmode="numeric" min="100" max="15000" step="10" placeholder="ej. 1450"></label></div>
<div class="cvw"><canvas id="cv"></canvas></div>
<div class="rd" id="rd">Esperando datos…</div>
</div>

<div class="panel f">
<h2>Qué buscar en el espectro</h2>
<p>Las frecuencias se expresan como múltiplos de 1X, la frecuencia de giro del motor.</p>
<table id="tb">
<tr id="f1"><th>Desbalanceo</th><td>Pico alto en 1X</td><td>Masa desigual en el rotor</td></tr>
<tr id="f2"><th>Desalineación</th><td>Pico en 2X, a veces 1X y 3X</td><td>Ejes del motor y la carga no coinciden</td></tr>
<tr id="f3"><th>Soltura</th><td>Muchos armónicos (3X, 4X…)</td><td>Bases o tornillos flojos</td></tr>
</table>
</div>
<footer>ESP32-S3 + MPU6050 · 500 Hz, 1024 muestras (0,49 Hz por barra) · Se actualiza cada ~2 s · Valores orientativos, ISO 10816 clase I</footer>
</div>

<script>
var $=function(i){return document.getElementById(i)},D=null,N=0,HV=null,hov=-1,
ZN=[["A","buena","--za"],["B","aceptable","--zb"],["C","alerta","--zc"],["D","peligro","--zd"]],
LM=[0.71,1.8,4.5,7.1];
function zi(v){return v<LM[0]?0:v<LM[1]?1:v<LM[2]?2:3}
function pos(v){var z=zi(v),lo=z?LM[z-1]:0,hi=LM[z];return Math.min(100,(z+Math.min(1,(v-lo)/(hi-lo)))*25)}
function cs(n){return getComputedStyle(document.documentElement).getPropertyValue(n).trim()}
function nom(){var n=parseFloat($("nom").value);return n>0?n/60:0}
function fmt(v,d){return v.toFixed(d).replace(".",",")}

var S=null,PS=null,t0=0;
function rgba(c,a){var n=parseInt(c.slice(1),16);return "rgba("+(n>>16)+","+(n>>8&255)+","+(n&255)+","+a+")"}
function pill(x,t,cx,cy,bg,fg){var w=x.measureText(t).width+16,h=22,r=11,l=Math.max(2,Math.min(cx-w/2,x.canvas.clientWidth-w-2)),u=cy-h/2;
x.fillStyle=bg;x.beginPath();x.moveTo(l+r,u);x.arcTo(l+w,u,l+w,u+h,r);x.arcTo(l+w,u+h,l,u+h,r);x.arcTo(l,u+h,l,u,r);x.arcTo(l,u,l+w,u,r);x.closePath();x.fill();
x.fillStyle=fg;x.textAlign="center";x.textBaseline="middle";x.fillText(t,l+w/2,cy+.5)}
function anim(){var k=Math.min(1,(performance.now()-t0)/700);k=1-Math.pow(1-k,3);
for(var i=0;i<S.length;i++)S[i]=PS[i]+(D.spec[i]-PS[i])*k;draw();if(k<1)requestAnimationFrame(anim)}
function draw(){
if(!D||!S)return;
var c=$("cv"),dpr=window.devicePixelRatio||1,W=c.clientWidth,H=c.clientHeight;
c.width=W*dpr;c.height=H*dpr;var x=c.getContext("2d");x.scale(dpr,dpr);
var s=S,n=s.length,L=46,B=28,T=24,R=12,pw=W-L-R,ph=H-B-T,m=50;
for(var i=1;i<n;i++)if(s[i]>m)m=s[i];
var st=m>200?100:m>100?50:20;m=Math.ceil(m*1.12/st)*st;
var ln=cs("--ln"),mu=cs("--mut"),sp=cs("--sp"),ink=cs("--ink"),pn=cs("--pn"),d=250/n,
X=function(f){return L+f/250*pw},Y=function(a){return T+ph-ph*a/m};
x.font="11px system-ui,sans-serif";x.lineJoin="round";
x.textAlign="right";x.textBaseline="middle";x.fillStyle=mu;x.fillText("mg",L-8,8);
x.setLineDash([2,5]);x.lineWidth=1;
for(var a=0;a<=m;a+=m/4){x.strokeStyle=ln;x.beginPath();x.moveTo(L,Math.round(Y(a))+.5);x.lineTo(W-R,Math.round(Y(a))+.5);x.stroke();x.fillStyle=mu;x.textAlign="right";x.fillText(Math.round(a),L-8,Y(a))}
x.setLineDash([]);
x.textAlign="center";x.textBaseline="top";x.fillStyle=mu;
for(var f=0;f<=250;f+=50){x.fillText(f+(f==250?" Hz":""),Math.min(X(f),W-16),H-B+9);x.strokeStyle=ln;x.beginPath();x.moveTo(X(f)+.5,T+ph);x.lineTo(X(f)+.5,T+ph+4);x.stroke()}
var f1=nom()||D.pico;
if(f1>=2){for(var k=1;k*f1<=250&&k<=6;k++){var hx=Math.round(X(k*f1))+.5;x.strokeStyle=rgba(mu.charAt(0)=="#"?mu:"#888888",.45);x.setLineDash([1,4]);x.beginPath();x.moveTo(hx,T-4);x.lineTo(hx,T+ph);x.stroke();x.setLineDash([]);x.fillStyle=mu;x.textBaseline="middle";x.textAlign="center";x.fillText(k+"X",hx,8)}}
function path(){x.beginPath();x.moveTo(X(d),Y(s[1]));
for(var i=2;i<n;i++){var px=X((i-1)*d),py=Y(s[i-1]);x.quadraticCurveTo(px,py,(px+X(i*d))/2,(py+Y(s[i]))/2)}
x.lineTo(X((n-1)*d),Y(s[n-1]))}
var g=x.createLinearGradient(0,T,0,T+ph);g.addColorStop(0,rgba(sp,.42));g.addColorStop(1,rgba(sp,0));
path();x.lineTo(X((n-1)*d),Y(0));x.lineTo(X(d),Y(0));x.closePath();x.fillStyle=g;x.fill();
path();x.strokeStyle=sp;x.lineWidth=2;x.stroke();
x.strokeStyle=ln;x.lineWidth=1;x.beginPath();x.moveTo(L,T+ph+.5);x.lineTo(W-R,T+ph+.5);x.stroke();
x.font="600 12px system-ui,sans-serif";
if(hov>0&&hov<n){var hx=X(hov*d),hy=Y(s[hov]);
x.strokeStyle=rgba(ink.charAt(0)=="#"?ink:"#888888",.35);x.beginPath();x.moveTo(hx,T);x.lineTo(hx,T+ph);x.stroke();
x.fillStyle=sp;x.beginPath();x.arc(hx,hy,4.5,0,7);x.fill();x.strokeStyle=pn;x.lineWidth=2;x.stroke();
pill(x,fmt(hov*d,1)+" Hz · "+Math.round(s[hov])+" mg",hx,Math.max(T+12,hy-24),ink,pn)}
else{var pi=Math.round(D.pico/d);
if(pi>0&&pi<n){var px=X(D.pico),py=Y(s[pi]);
x.fillStyle=rgba(sp,.22);x.beginPath();x.arc(px,py,10,0,7);x.fill();
x.fillStyle=sp;x.beginPath();x.arc(px,py,4.5,0,7);x.fill();x.strokeStyle=pn;x.lineWidth=2;x.stroke();
pill(x,fmt(D.pico,1)+" Hz",px,Math.max(T+12,py-26),ink,pn)}}
}
function lectura(){
var t="";
if(hov>0&&D){t="<b>"+fmt(hov*250/D.spec.length,1)+" Hz</b> · "+D.spec[hov]+" mg"}
else if(D){var u=nom();
t=u?"El pico está en <b>"+fmt(D.pico/u,2)+"X</b> la frecuencia nominal ("+fmt(u,1)+" Hz).":"Ingresá las rpm nominales para ver el pico como múltiplo de 1X."}
$("rd").innerHTML=t||"Esperando datos…"}
function diag(){
var u=nom(),r=u?D.pico/u:0;
["f1","f2","f3"].forEach(function(i){$(i).className=""});
if(!u||!D)return;
var on=Math.abs(r-1)<.08?"f1":Math.abs(r-2)<.1?"f2":r>2.9?"f3":"";
if(on)$(on).className="on"}
function show(d){
D=d;
var z=zi(d.vel),Z=ZN[z];
$("vel").textContent=fmt(d.vel,2);
$("zl").innerHTML='<span style="color:var('+Z[2]+')">●</span> Zona '+Z[0]+' · '+Z[1];
$("mk").style.left=pos(d.vel)+"%";
$("fpk").textContent=fmt(d.pico,1);
$("rpm").textContent=Math.round(d.pico*60);
$("estado").textContent="Bloque "+d.bloques+" · "+fmt(d.arms,3)+" g RMS";
$("dot").className="dot ok";
var rm=window.matchMedia&&matchMedia("(prefers-reduced-motion:reduce)").matches;
if(!S||rm||S.length!=d.spec.length){S=d.spec.slice();draw()}else{PS=S.slice();t0=performance.now();anim()}
lectura();diag()}
function upd(){
fetch("/data").then(function(r){return r.json()}).then(function(d){if(d.listo)show(d);else{$("estado").textContent="Midiendo el primer bloque…";$("dot").className="dot ok"}})
.catch(function(){$("dot").className="dot err";$("estado").textContent="Sin conexión con el ESP32"})
.then(function(){setTimeout(upd,2000)})}
function ptr(e){var c=$("cv"),r=c.getBoundingClientRect(),f=(e.clientX-r.left-40)/(r.width-48)*250;
hov=D&&f>0&&f<250?Math.round(f/(250/D.spec.length)):-1;draw();lectura()}
$("cv").addEventListener("pointermove",ptr);$("cv").addEventListener("pointerdown",ptr);
$("cv").addEventListener("pointerleave",function(){hov=-1;draw();lectura()});
try{var sv=localStorage.getItem("nom");if(sv)$("nom").value=sv}catch(e){}
$("nom").addEventListener("input",function(){try{localStorage.setItem("nom",this.value)}catch(e){}draw();lectura();if(D)diag()});
window.addEventListener("resize",draw);
upd();
</script></body></html>
)rawliteral";
