#include <WiFi.h>
#include <SimpleDHT.h>

// ============================================================
//                  WIFI CONFIGURATION
// ============================================================

const char* ssid = "GN_723,728-4G";
const char* password = "welcometoroyal";

// ============================================================
//                  SENSOR CONFIGURATION
// ============================================================

// DHT11 connected to GPIO 4
#define DHTPIN 4
SimpleDHT11 dht11(DHTPIN);

// LDR Digital Output connected to GPIO 34
#define LDR_PIN 34

// ============================================================
//                  WEB SERVER
// ============================================================

WiFiServer server(80);

// ============================================================
//                  SENSOR DATA
// ============================================================

byte temperature = 0;
byte humidity = 0;

int ldrState = 0;

bool dhtWorking = false;
bool ldrWorking = true;

// ============================================================
//                  TIMING
// ============================================================

unsigned long lastSensorRead = 0;
const unsigned long SENSOR_INTERVAL = 5000;

// ============================================================
//                  HISTORY
// ============================================================

#define HISTORY_SIZE 30

float temperatureHistory[HISTORY_SIZE];
float humidityHistory[HISTORY_SIZE];

unsigned long historyTime[HISTORY_SIZE];

int historyCount = 0;
int historyIndex = 0;

// ============================================================
//                  PROJECT START TIME
// ============================================================

unsigned long startTime;

// ============================================================
//                  ADD READING TO HISTORY
// ============================================================

void addHistory(float temp, float hum) {

  temperatureHistory[historyIndex] = temp;
  humidityHistory[historyIndex] = hum;

  historyTime[historyIndex] = millis();

  historyIndex++;

  if (historyIndex >= HISTORY_SIZE) {
    historyIndex = 0;
  }

  if (historyCount < HISTORY_SIZE) {
    historyCount++;
  }
}

// ============================================================
//                  READ SENSORS
// ============================================================

void readSensors() {

  byte temp = 0;
  byte hum = 0;

  int err = dht11.read(&temp, &hum, NULL);

  if (err == SimpleDHTErrSuccess) {

    temperature = temp;
    humidity = hum;

    dhtWorking = true;

    addHistory(temperature, humidity);

  } else {

    dhtWorking = false;

    Serial.print("DHT11 Error: ");
    Serial.println(err);
  }

  // Read LDR digital output
  ldrState = digitalRead(LDR_PIN);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("LDR State: ");
  Serial.println(ldrState);

  Serial.println("-------------------------");
}

// ============================================================
//                  SEND WEBPAGE
// ============================================================

void sendWebPage(WiFiClient &client) {

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();

  client.print(R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<meta charset="UTF-8">

<meta name="viewport"
      content="width=device-width, initial-scale=1.0">

<title>Smart Environmental Weather Station</title>

<style>

* {
    box-sizing: border-box;
}

body {

    margin: 0;

    font-family:
    -apple-system,
    BlinkMacSystemFont,
    "Segoe UI",
    Roboto,
    Arial,
    sans-serif;

    background:
    linear-gradient(135deg,#07111f,#102a43);

    color: #f4f7fb;

    min-height: 100vh;
}


/* =========================================================
                     MAIN CONTAINER
   ========================================================= */

.container {

    width: 94%;

    max-width: 1250px;

    margin: auto;

    padding: 25px 0 50px 0;

}


/* =========================================================
                     HEADER
   ========================================================= */

.header {

    background:
    linear-gradient(135deg,#0d6efd,#00a6ff);

    border-radius: 22px;

    padding: 30px;

    margin-bottom: 25px;

    box-shadow:
    0 15px 40px rgba(0,0,0,0.25);

}

.header h1 {

    margin: 0;

    font-size: 32px;

}

.header p {

    margin: 8px 0 0;

    opacity: 0.9;

}

.live {

    display: inline-block;

    margin-top: 18px;

    padding: 7px 14px;

    border-radius: 50px;

    background: rgba(255,255,255,0.18);

    font-size: 13px;

}


/* =========================================================
                     SECTION
   ========================================================= */

.section-title {

    font-size: 22px;

    margin:

    30px 0 15px 5px;

}


/* =========================================================
                     SENSOR CARDS
   ========================================================= */

.sensor-grid {

    display: grid;

    grid-template-columns:
    repeat(auto-fit,minmax(250px,1fr));

    gap: 18px;

}

.card {

    background:
    rgba(255,255,255,0.08);

    border:
    1px solid rgba(255,255,255,0.10);

    border-radius: 20px;

    padding: 23px;

    backdrop-filter: blur(12px);

    box-shadow:
    0 12px 35px rgba(0,0,0,0.18);

}

.card-header {

    display: flex;

    justify-content: space-between;

    align-items: center;

}

.icon {

    font-size: 35px;

}

.status-dot {

    width: 10px;

    height: 10px;

    background: #28d17c;

    border-radius: 50%;

    display: inline-block;

    margin-right: 5px;

}

.value {

    font-size: 42px;

    font-weight: 700;

    margin: 18px 0 4px;

}

.unit {

    font-size: 17px;

    opacity: 0.7;

}

.label {

    opacity: 0.65;

}

.interpretation {

    margin-top: 15px;

    padding: 12px;

    border-radius: 12px;

    background: rgba(255,255,255,0.07);

    font-size: 14px;

    line-height: 1.5;

}


/* =========================================================
                     STATUS BADGE
   ========================================================= */

.badge {

    display: inline-block;

    padding: 6px 12px;

    border-radius: 30px;

    font-size: 12px;

    font-weight: 600;

    background: rgba(40,209,124,0.18);

    color: #58f0a3;

}


/* =========================================================
                     ANALYSIS
   ========================================================= */

.analysis {

    background:

    linear-gradient(
        135deg,
        rgba(13,110,253,0.22),
        rgba(0,166,255,0.08)
    );

    border:

    1px solid rgba(0,166,255,0.25);

    border-radius: 20px;

    padding: 25px;

}

.analysis h2 {

    margin-top: 0;

}

.analysis-main {

    font-size: 24px;

    font-weight: 700;

    margin-bottom: 15px;

}

.analysis-text {

    line-height: 1.7;

    opacity: 0.9;

}


/* =========================================================
                     GRAPHS
   ========================================================= */

.graph-grid {

    display: grid;

    grid-template-columns:
    repeat(auto-fit,minmax(300px,1fr));

    gap: 18px;

}

.graph-card {

    background:
    rgba(255,255,255,0.07);

    border-radius: 20px;

    padding: 20px;

}

canvas {

    width: 100%;

    height: 220px;

    background:
    rgba(0,0,0,0.12);

    border-radius: 12px;

}


/* =========================================================
                     MEANING CARDS
   ========================================================= */

.meaning-grid {

    display: grid;

    grid-template-columns:
    repeat(auto-fit,minmax(280px,1fr));

    gap: 18px;

}

.meaning-card {

    background:
    rgba(255,255,255,0.07);

    border-radius: 18px;

    padding: 22px;

}

.meaning-card h3 {

    margin-top: 0;

}

.meaning-card p {

    line-height: 1.65;

    opacity: 0.78;

}


/* =========================================================
                     APPLICATIONS
   ========================================================= */

.applications {

    display: grid;

    grid-template-columns:
    repeat(auto-fit,minmax(180px,1fr));

    gap: 15px;

}

.application {

    text-align: center;

    background:
    rgba(255,255,255,0.07);

    padding: 20px;

    border-radius: 18px;

    font-size: 16px;

}

.application-icon {

    font-size: 35px;

    margin-bottom: 8px;

}


/* =========================================================
                     SYSTEM STATUS
   ========================================================= */

.system-grid {

    display: grid;

    grid-template-columns:
    repeat(auto-fit,minmax(220px,1fr));

    gap: 15px;

}

.system-item {

    background:
    rgba(255,255,255,0.07);

    padding: 18px;

    border-radius: 15px;

}

.system-item span {

    display: block;

}

.system-value {

    margin-top: 7px;

    font-weight: 700;

}


/* =========================================================
                     ARCHITECTURE
   ========================================================= */

.architecture {

    background:
    rgba(255,255,255,0.06);

    border-radius: 20px;

    padding: 25px;

    text-align: center;

}

.architecture-flow {

    display: flex;

    justify-content: center;

    align-items: center;

    gap: 12px;

    flex-wrap: wrap;

    margin-top: 20px;

}

.arch-box {

    background:
    rgba(13,110,253,0.25);

    border:
    1px solid rgba(13,110,253,0.4);

    padding: 16px 20px;

    border-radius: 14px;

    font-weight: 600;

}

.arrow {

    font-size: 25px;

    opacity: 0.6;

}


/* =========================================================
                     WARNING
   ========================================================= */

.warning {

    background:
    rgba(255,193,7,0.12);

    border:
    1px solid rgba(255,193,7,0.25);

    color: #ffd86b;

    padding: 15px;

    border-radius: 14px;

    margin-top: 15px;

}


/* =========================================================
                     FOOTER
   ========================================================= */

.footer {

    margin-top: 40px;

    text-align: center;

    opacity: 0.5;

    font-size: 13px;

    line-height: 1.7;

}


/* =========================================================
                     MOBILE
   ========================================================= */

@media(max-width:600px) {

    .header h1 {

        font-size: 25px;

    }

    .value {

        font-size: 35px;

    }

    .container {

        width: 92%;

    }

}

</style>

</head>


<body>


<div class="container">


<!-- =====================================================
                         HEADER
====================================================== -->

<div class="header">

<h1>🌦️ Smart Environmental Weather Station</h1>

<p>
ESP32-Based Real-Time Environmental Monitoring System
</p>

<div class="live">

🟢 LIVE MONITORING

</div>

</div>


<!-- =====================================================
                     CURRENT CONDITIONS
====================================================== -->

<div class="section-title">

📡 Current Environmental Conditions

</div>


<div class="sensor-grid">


<!-- TEMPERATURE -->

<div class="card">

<div class="card-header">

<div>

<strong>Temperature</strong>

</div>

<div class="icon">🌡️</div>

</div>

<div class="value">

<span id="temperature">--</span>

<span class="unit">°C</span>

</div>

<div id="temperatureStatus" class="badge">

Reading...

</div>

<div class="interpretation">

<strong>What does this mean?</strong>

<br><br>

Temperature represents the surrounding
air temperature and helps us understand
the thermal condition of the environment.

</div>

</div>


<!-- HUMIDITY -->

<div class="card">

<div class="card-header">

<div>

<strong>Relative Humidity</strong>

</div>

<div class="icon">💧</div>

</div>

<div class="value">

<span id="humidity">--</span>

<span class="unit">%</span>

</div>

<div id="humidityStatus" class="badge">

Reading...

</div>

<div class="interpretation">

<strong>What does this mean?</strong>

<br><br>

Relative humidity represents the amount
of moisture present in the air compared
with the maximum moisture the air can
hold at that temperature.

</div>

</div>


<!-- LIGHT -->

<div class="card">

<div class="card-header">

<div>

<strong>Ambient Light</strong>

</div>

<div class="icon">☀️</div>

</div>

<div class="value" id="light">

--

</div>

<div id="lightStatus" class="badge">

Reading...

</div>

<div class="interpretation">

<strong>What does this mean?</strong>

<br><br>

The LDR detects changes in surrounding
light. This system uses digital detection,
so the reading represents Bright or Dark
rather than a calibrated lux value.

</div>

</div>


</div>


<!-- =====================================================
                     OVERALL ANALYSIS
====================================================== -->

<div class="section-title">

🧠 Environmental Analysis

</div>


<div class="analysis">

<div class="analysis-main" id="overallStatus">

Analyzing environment...

</div>

<div class="analysis-text" id="analysisText">

Waiting for sensor data...

</div>

<div id="warningArea"></div>

</div>


<!-- =====================================================
                         GRAPHS
====================================================== -->

<div class="section-title">

📊 Live Environmental Trends

</div>


<div class="graph-grid">


<div class="graph-card">

<h3>🌡️ Temperature History</h3>

<p style="opacity:.6">

Last 30 readings

</p>

<canvas id="temperatureChart"></canvas>

</div>


<div class="graph-card">

<h3>💧 Humidity History</h3>

<p style="opacity:.6">

Last 30 readings

</p>

<canvas id="humidityChart"></canvas>

</div>


</div>


<!-- =====================================================
                     APPLICATIONS
====================================================== -->

<div class="section-title">

🌍 Real-World Applications

</div>


<div class="applications">


<div class="application">

<div class="application-icon">🏠</div>

Smart Homes

</div>


<div class="application">

<div class="application-icon">🏫</div>

Smart Classrooms

</div>


<div class="application">

<div class="application-icon">🌱</div>

Agriculture

</div>


<div class="application">

<div class="application-icon">🏭</div>

Industrial Monitoring

</div>


<div class="application">

<div class="application-icon">💡</div>

Automatic Lighting

</div>


<div class="application">

<div class="application-icon">🌍</div>

Environmental Monitoring

</div>


</div>


<!-- =====================================================
                     SYSTEM STATUS
====================================================== -->

<div class="section-title">

⚙️ System Status

</div>


<div class="system-grid">


<div class="system-item">

<span>ESP32 Controller</span>

<span class="system-value">

🟢 ONLINE

</span>

</div>


<div class="system-item">

<span>Wi-Fi Connection</span>

<span class="system-value">

🟢 CONNECTED

</span>

</div>


<div class="system-item">

<span>DHT11 Sensor</span>

<span class="system-value" id="dhtStatus">

Checking...

</span>

</div>


<div class="system-item">

<span>LDR Sensor</span>

<span class="system-value">

🟢 ACTIVE

</span>

</div>


<div class="system-item">

<span>ESP32 IP Address</span>

<span class="system-value" id="ipAddress">

--

</span>

</div>


<div class="system-item">

<span>Last Update</span>

<span class="system-value" id="lastUpdate">

--

</span>

</div>


<div class="system-item">

<span>System Uptime</span>

<span class="system-value" id="uptime">

--

</span>

</div>


</div>


<!-- =====================================================
                     ARCHITECTURE
====================================================== -->

<div class="section-title">

🔬 System Architecture

</div>


<div class="architecture">

<p>

The system collects environmental data,
processes it using the ESP32 and provides
the results through a Wi-Fi web dashboard.

</p>


<div class="architecture-flow">


<div class="arch-box">

🌡️ DHT11

<br>

Temperature

<br>

Humidity

</div>


<div class="arrow">

→

</div>


<div class="arch-box">

☀️ LDR

<br>

Ambient Light

</div>


<div class="arrow">

→

</div>


<div class="arch-box">

🧠 ESP32

<br>

Data Processing

</div>


<div class="arrow">

→

</div>


<div class="arch-box">

📡 Wi-Fi

<br>

Local Network

</div>


<div class="arrow">

→

</div>


<div class="arch-box">

💻 Web Dashboard

<br>

Visualization

</div>


</div>

</div>


</div>


<script>


// ========================================================
//                    CHART VARIABLES
// ========================================================

let temperatureData = [];

let humidityData = [];


// ========================================================
//                    FETCH DATA
// ========================================================

async function updateData() {

    try {

        const response = await fetch("/data");

        const data = await response.json();


        // Temperature

        document.getElementById("temperature")
        .textContent = data.temperature;


        // Humidity

        document.getElementById("humidity")
        .textContent = data.humidity;


        // Light

        document.getElementById("light")
        .textContent =
            data.light ? "DARK" : "BRIGHT";


        // IP

        document.getElementById("ipAddress")
        .textContent = data.ip;


        // Last update

        document.getElementById("lastUpdate")
        .textContent =
            new Date().toLocaleTimeString();


        // DHT status

        document.getElementById("dhtStatus")
        .textContent =
            data.dht
            ? "🟢 ACTIVE"
            : "🔴 ERROR";


        // Store graph data

        temperatureData = data.tempHistory;

        humidityData = data.humHistory;


        updateInterpretation(data);

        drawChart(
            "temperatureChart",
            temperatureData,
            "Temperature"
        );

        drawChart(
            "humidityChart",
            humidityData,
            "Humidity"
        );

    }

    catch(error) {

        console.log(error);

    }

}


// ========================================================
//              ENVIRONMENT INTERPRETATION
// ========================================================

function updateInterpretation(data) {

    let temp = data.temperature;

    let hum = data.humidity;

    let light = data.light;


    // Temperature interpretation

    let tempText;

    if(temp < 15) {

        tempText = "❄️ Cold";

    }

    else if(temp < 20) {

        tempText = "🧥 Cool";

    }

    else if(temp <= 27) {

        tempText = "🟢 Comfortable";

    }

    else if(temp <= 32) {

        tempText = "🟡 Warm";

    }

    else {

        tempText = "🔴 Hot";

    }


    document.getElementById("temperatureStatus")
    .textContent = tempText;


    // Humidity interpretation

    let humText;

    if(hum < 30) {

        humText = "🏜️ Dry";

    }

    else if(hum < 40) {

        humText = "🟡 Slightly Dry";

    }

    else if(hum <= 60) {

        humText = "🟢 Comfortable";

    }

    else if(hum <= 70) {

        humText = "🟡 Moderately Humid";

    }

    else {

        humText = "💧 Humid";

    }


    document.getElementById("humidityStatus")
    .textContent = humText;


    // Light

    document.getElementById("lightStatus")
    .textContent =

        light

        ? "🌙 Low Light Environment"

        : "☀️ Bright Light Environment";


    // ====================================================
    //             OVERALL CONDITION
    // ====================================================

    let overall;

    let explanation;


    if(temp > 32 && hum > 70) {

        overall = "🔴 HOT & HUMID ENVIRONMENT";

        explanation =
        "The environment is currently warm/hot " +
        "with high humidity. This combination " +
        "may reduce thermal comfort.";

    }

    else if(temp > 32) {

        overall = "🟠 WARM ENVIRONMENT";

        explanation =
        "The measured temperature is relatively " +
        "high. Increased ventilation or cooling " +
        "may improve comfort.";

    }

    else if(temp < 15) {

        overall = "🔵 COLD ENVIRONMENT";

        explanation =
        "The measured temperature is relatively " +
        "low compared with typical indoor " +
        "comfort conditions.";

    }

    else if(hum > 70) {

        overall = "🟡 HIGH HUMIDITY";

        explanation =
        "Humidity is relatively high. The air " +
        "contains a high amount of moisture.";

    }

    else if(hum < 30) {

        overall = "🟡 DRY ENVIRONMENT";

        explanation =
        "Humidity is relatively low, indicating " +
        "a dry environment.";

    }

    else {

        overall = "🟢 GENERALLY COMFORTABLE";

        explanation =
        "Temperature and humidity are currently " +
        "within generally comfortable ranges. " +
        "The LDR indicates " +
        (light
        ? "adequate ambient illumination."
        : "low ambient illumination.");

    }


    document.getElementById("overallStatus")
    .textContent = overall;


    document.getElementById("analysisText")
    .textContent = explanation;


    // ====================================================
    //                    WARNINGS
    // ====================================================

    let warning = "";


    if(temp > 35) {

        warning +=
        "⚠️ Temperature is unusually high. ";

    }


    if(hum > 80) {

        warning +=
        "⚠️ Humidity is very high. ";

    }


    if(temp < 10) {

        warning +=
        "⚠️ Temperature is unusually low. ";

    }


    if(warning !== "") {

        document.getElementById("warningArea")
        .innerHTML =
        "<div class='warning'>" +
        warning +
        "</div>";

    }

    else {

        document.getElementById("warningArea")
        .innerHTML = "";

    }

}


// ========================================================
//                     DRAW CHART
// ========================================================

function drawChart(canvasId, values, label) {

    const canvas =
        document.getElementById(canvasId);

    const ctx =
        canvas.getContext("2d");


    const width =
        canvas.clientWidth;

    const height =
        canvas.clientHeight;


    const ratio =
        window.devicePixelRatio || 1;


    canvas.width =
        width * ratio;

    canvas.height =
        height * ratio;


    ctx.scale(ratio, ratio);


    ctx.clearRect(0,0,width,height);


    if(values.length < 2) {

        ctx.fillStyle = "rgba(255,255,255,.5)";

        ctx.font = "14px Arial";

        ctx.fillText(
            "Collecting data...",
            20,
            40
        );

        return;

    }


    let min =
        Math.min(...values);

    let max =
        Math.max(...values);


    if(min === max) {

        min -= 1;

        max += 1;

    }


    let padding = 30;


    // Grid

    ctx.strokeStyle =
        "rgba(255,255,255,.10)";

    ctx.lineWidth = 1;


    for(let i=0;i<5;i++) {

        let y =
            padding +
            i *
            ((height - padding*2) / 4);


        ctx.beginPath();

        ctx.moveTo(
            padding,
            y
        );

        ctx.lineTo(
            width-padding,
            y
        );

        ctx.stroke();

    }


    // Line

    ctx.beginPath();


    values.forEach((value,index) => {

        let x =
            padding +
            index *
            ((width-padding*2) /
            (values.length-1));


        let y =
            height-padding -
            ((value-min)/(max-min)) *
            (height-padding*2);


        if(index === 0)

            ctx.moveTo(x,y);

        else

            ctx.lineTo(x,y);

    });


    ctx.strokeStyle =
        "#32d6ff";

    ctx.lineWidth = 3;

    ctx.stroke();


    // Points

    values.forEach((value,index) => {

        let x =
            padding +
            index *
            ((width-padding*2) /
            (values.length-1));


        let y =
            height-padding -
            ((value-min)/(max-min)) *
            (height-padding*2);


        ctx.beginPath();

        ctx.arc(
            x,
            y,
            3,
            0,
            Math.PI*2
        );

        ctx.fillStyle =
            "#ffffff";

        ctx.fill();

    });


    // Current value

    ctx.fillStyle =
        "rgba(255,255,255,.65)";

    ctx.font =
        "12px Arial";

    ctx.fillText(
        "Latest: " +
        values[values.length-1],
        padding,
        18
    );

}


// ========================================================
//                     AUTO UPDATE
// ========================================================

updateData();

setInterval(
    updateData,
    5000
);


// Redraw when screen size changes

window.addEventListener(
    "resize",
    function() {

        drawChart(
            "temperatureChart",
            temperatureData,
            "Temperature"
        );

        drawChart(
            "humidityChart",
            humidityData,
            "Humidity"
        );

    }
);


</script>


</body>

</html>

)rawliteral");

}


// ============================================================
//                  SEND JSON DATA
// ============================================================

void sendJSON(WiFiClient &client) {

  client.println("HTTP/1.1 200 OK");

  client.println("Content-Type: application/json");

  client.println("Cache-Control: no-cache");

  client.println("Connection: close");

  client.println();


  client.print("{");


  // Temperature

  client.print("\"temperature\":");

  client.print(temperature);


  // Humidity

  client.print(",\"humidity\":");

  client.print(humidity);


  // LDR

  client.print(",\"light\":");

  /*
     IMPORTANT:
     This assumes:
       1 = BRIGHT
       0 = DARK

     If your LDR behaves opposite,
     change this to:

     ldrState == 0
  */

  client.print(ldrState == 1 ? "true" : "false");


  // DHT status

  client.print(",\"dht\":");

  client.print(dhtWorking ? "true" : "false");


  // IP

  client.print(",\"ip\":\"");

  client.print(WiFi.localIP());

  client.print("\"");


  // Temperature history

  client.print(",\"tempHistory\":[");


  for(int i = 0; i < historyCount; i++) {

    int index =
      (historyIndex - historyCount + i + HISTORY_SIZE)
      % HISTORY_SIZE;

    client.print(temperatureHistory[index]);

    if(i < historyCount - 1)

      client.print(",");

  }


  client.print("]");


  // Humidity history

  client.print(",\"humHistory\":[");


  for(int i = 0; i < historyCount; i++) {

    int index =
      (historyIndex - historyCount + i + HISTORY_SIZE)
      % HISTORY_SIZE;

    client.print(humidityHistory[index]);

    if(i < historyCount - 1)

      client.print(",");

  }


  client.print("]");


  client.print("}");

}


// ============================================================
//                  SETUP
// ============================================================

void setup() {

  Serial.begin(115200);

  delay(1000);


  // LDR input

  pinMode(
    LDR_PIN,
    INPUT
  );


  // Start timer

  startTime = millis();


  // ========================================================
  //                  WIFI
  // ========================================================

  WiFi.begin(
    ssid,
    password
  );


  Serial.print(
    "Connecting to WiFi"
  );


  while(
    WiFi.status() != WL_CONNECTED
  ) {

    delay(500);

    Serial.print(".");

  }


  Serial.println();

  Serial.println(
    "WiFi Connected!"
  );


  Serial.print(
    "IP Address: "
  );

  Serial.println(
    WiFi.localIP()
  );


  // ========================================================
  //                  SERVER
  // ========================================================

  server.begin();


  Serial.println(
    "Web server started!"
  );


  // Initial sensor reading

  readSensors();

}


// ============================================================
//                  MAIN LOOP
// ============================================================

void loop() {


  // ========================================================
  //                  READ SENSOR
  // ========================================================

  if(
    millis() - lastSensorRead
    >= SENSOR_INTERVAL
  ) {

    lastSensorRead = millis();

    readSensors();

  }


  // ========================================================
  //                  WEB CLIENT
  // ========================================================

  WiFiClient client =
    server.available();


  if(!client) {

    return;

  }


  Serial.println(
    "New Client Connected"
  );


  String request = "";

  unsigned long timeout =
    millis();


  while(
    client.connected() &&
    millis() - timeout < 1000
  ) {

    if(client.available()) {

      char c =
        client.read();


      request += c;


      if(c == '\n') {

        // First line contains request

        break;

      }

    }

  }


  // ========================================================
  //                  ROUTING
  // ========================================================

  if(
    request.indexOf("GET /data") >= 0
  ) {

    sendJSON(client);

  }

  else {

    sendWebPage(client);

  }


  delay(1);

  client.stop();


  Serial.println(
    "Client Disconnected"
  );

}
