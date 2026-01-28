// https://oebb.macistry.com/api/stops/1350662/departures?bus=true&direction=596051&results=10

#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>
#include <time.h>

// ===================== FÄRGER =====================
#define DARK_PINK 0x4B00
#define DELAY_RED 0xF800

#define TFT_BL 5
TFT_eSPI tft = TFT_eSPI();
ESP8266WebServer server(80);

// ===================== DATA =====================
struct BusData {
    String departureTime;
    String secondDepartureTime;
    String destination;
    String lineNumber;
    int delayMinutes;
    bool isDelayed;
    bool farAway;
} busInfo;

String debugMsg = "Starting...";
int httpStatus = 0;

#define EEPROM_SIZE 256

struct Config {
    char stop_id[12];
    char destination_id[12];
    bool night_mode_enabled;
    uint8_t night_start_hour;
    uint8_t night_start_minute;
    uint8_t night_end_hour;
    uint8_t night_end_minute;
    uint8_t bri;
    uint8_t duration_mode;
} config;

// ===================== LJUS =====================
void setBrightness(uint8_t val) {
    analogWrite(TFT_BL, 255 - val);
}

// ===================== BOOT INFO =====================
void displayBootInfo(const String &msg) {
    Serial.println(msg);
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(msg, 120, 120, 2);
    delay(500);
}

// ===================== TID =====================
bool isNightMode() {
    if (!config.night_mode_enabled) return false;
    
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    
    int currentMinutes = t->tm_hour * 60 + t->tm_min;
    int startMinutes = config.night_start_hour * 60 + config.night_start_minute;
    int endMinutes = config.night_end_hour * 60 + config.night_end_minute;
    
    if (startMinutes < endMinutes) {
        return currentMinutes >= startMinutes && currentMinutes < endMinutes;
    } else {
        return currentMinutes >= startMinutes || currentMinutes < endMinutes;
    }
}

int minutesUntil(const char* whenStr) {
    int y, mo, d, h, mi, s;
    if (sscanf(whenStr, "%d-%d-%dT%d:%d:%d", &y, &mo, &d, &h, &mi, &s) != 6) 
        return 9999; 

    struct tm t = {};
    t.tm_year = y - 1900;
    t.tm_mon  = mo - 1;
    t.tm_mday = d;
    t.tm_hour = h;
    t.tm_min  = mi;
    t.tm_sec  = s;

    time_t dep = mktime(&t);
    time_t now = time(nullptr);
    return (dep - now) / 60;
}

// ===================== PROCESS JSON =====================
void processDepartures(JsonArray departures) {
    JsonObject firstSkiBus, secondSkiBus;
    bool foundFirst = false, foundSecond = false;

    for (JsonObject dep : departures) {
        String lineName = dep["line"]["name"] | "";
        if (lineName == "Bus SKI") {
            if (!foundFirst) { firstSkiBus = dep; foundFirst = true; }
            else if (!foundSecond) { secondSkiBus = dep; foundSecond = true; break; }
        }
    }

    if (!foundFirst) {
        debugMsg = "No Bus SKI";
        busInfo.departureTime = "No Bus SKI";
        busInfo.secondDepartureTime = "--:--";
        busInfo.farAway = false;
        return;
    }

    const char* whenStr = firstSkiBus["when"] | "";
    busInfo.departureTime = String(whenStr).substring(11, 16);
    busInfo.destination = firstSkiBus["direction"] | "";
    busInfo.lineNumber = firstSkiBus["line"]["name"] | "";
    int delaySec = firstSkiBus["delay"] | 0;
    busInfo.delayMinutes = delaySec / 60;
    busInfo.isDelayed = delaySec > 60;

    int minsUntilDep = minutesUntil(whenStr);
    busInfo.farAway = minsUntilDep > 360;

    if (foundSecond) {
        const char* whenStr2 = secondSkiBus["when"] | "";
        busInfo.secondDepartureTime = String(whenStr2).substring(11,16);
    } else {
        busInfo.secondDepartureTime = "--:--";
    }

    debugMsg = busInfo.farAway ? "Next >6h" : "OK";
}

// ===================== FETCH API =====================
bool fetchBus(int retries = 2) {
    for (int attempt = 0; attempt < retries; attempt++) {
        WiFiClientSecure client;
        client.setInsecure();
        client.setTimeout(15000);

        HTTPClient http;
        http.setTimeout(15000);
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

        String url =
            "https://oebb.macistry.com/api/stops/" + String(config.stop_id) +
            "/departures?bus=true&direction=" + String(config.destination_id) + "&results=10";

        if (!http.begin(client, url)) {
            debugMsg = "HTTP begin fail";
            delay(500);
            continue;
        }

        int status = http.GET();
        httpStatus = status;

        if (status != 200) {
            debugMsg = "HTTP " + String(status);
            http.end();
            delay(500);
            continue;
        }

        WiFiClient* stream = http.getStreamPtr();

        DynamicJsonDocument doc(8192);
        DeserializationError err = deserializeJson(doc, *stream);
        http.end();

        if (err) {
            debugMsg = "JSON: " + String(err.c_str());
            delay(500);
            continue;
        }

        JsonArray departures = doc["departures"];
        if (!departures.isNull() && departures.size() > 0) {
            processDepartures(departures);
            return true;
        } else {
            debugMsg = "No departures";
            busInfo.departureTime = "No buses";
            busInfo.secondDepartureTime = "--:--";
            busInfo.farAway = false;
            delay(500);
        }
    }
    return false;
}

// ===================== DISPLAY =====================
void displayBus() {
    uint32_t bg = busInfo.isDelayed ? DELAY_RED : DARK_PINK;
    tft.fillScreen(bg);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE, bg);

    if (busInfo.farAway) {
        tft.drawString("Next > 6h", 120, 120, 4);
    } else {
        tft.drawString(busInfo.departureTime, 120, 100, 6);
        tft.setTextSize(1);
        tft.drawString(busInfo.secondDepartureTime, 120, 150, 4);
    }

    tft.setTextSize(1);
    tft.drawString(debugMsg, 120, 220, 1);
}

void displayNightMode() {
    tft.fillScreen(TFT_BLACK);
    setBrightness(0);
}

// ===================== WEB =====================
const char html[] PROGMEM = R"(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
*{margin:0;padding:0;box-sizing:border-box;}
body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;background:linear-gradient(135deg,%GRADIENT%);min-height:100vh;padding:20px;display:flex;justify-content:center;align-items:center;}
.container{background:white;border-radius:16px;box-shadow:0 20px 60px rgba(0,0,0,0.3);padding:30px;max-width:500px;width:100%;}
h2{color:#333;margin-bottom:10px;font-size:28px;}
.status{background:%STATUS_BG%;padding:15px;border-radius:8px;margin:20px 0;border-left:4px solid %STATUS_BORDER%;}
.status p{margin:5px 0;color:#555;font-size:14px;}
.status strong{color:%STATUS_COLOR%;font-size:24px;display:block;margin-top:5px;}
.delay-badge{display:inline-block;background:#ff4444;color:white;padding:4px 12px;border-radius:12px;font-size:12px;font-weight:bold;margin-left:10px;}
.night-badge{display:inline-block;background:#333;color:white;padding:4px 12px;border-radius:12px;font-size:12px;font-weight:bold;margin-left:10px;}
label{display:block;margin-top:15px;margin-bottom:5px;color:#333;font-weight:600;font-size:14px;}
input[type="text"],input[type="number"],input[type="time"]{width:100%;padding:12px;border:2px solid #e0e0e0;border-radius:8px;font-size:16px;transition:border 0.3s;}
input[type="text"]:focus,input[type="number"]:focus,input[type="time"]:focus{outline:none;border-color:#667eea;}
.checkbox-wrapper{display:flex;align-items:center;padding:12px;background:#f9f9f9;border-radius:8px;margin:10px 0;}
input[type="checkbox"]{width:20px;height:20px;margin-right:10px;cursor:pointer;}
.checkbox-wrapper label{margin:0;cursor:pointer;font-weight:500;}
.time-row{display:flex;gap:10px;}
.time-row>div{flex:1;}
.btn{width:100%;padding:14px;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:white;border:none;border-radius:8px;font-size:16px;font-weight:600;cursor:pointer;margin-top:20px;transition:transform 0.2s,box-shadow 0.2s;}
.btn:hover{transform:translateY(-2px);box-shadow:0 10px 20px rgba(102,126,234,0.3);}
.btn:active{transform:translateY(0);}
.link{display:block;text-align:center;margin-top:15px;color:#667eea;text-decoration:none;font-weight:600;}
.link:hover{text-decoration:underline;}
.helper-link{display:inline-block;margin-top:5px;margin-bottom:10px;color:#667eea;text-decoration:none;font-size:13px;font-weight:500;}
.helper-link:hover{text-decoration:underline;}
.info-text{font-size:13px;color:#666;margin-top:15px;padding:10px;background:#f0f0f0;border-radius:6px;}
@media(max-width:600px){.container{padding:20px;}h2{font-size:24px;}}
</style>
</head>
<body>
<div class="container">
<h2>🚌 Next Departure</h2>
<div class="status">
<p>IP Address</p><strong>%IP%</strong>
<p style="margin-top:10px">Next Bus%DELAY_BADGE%%NIGHT_BADGE%</p><strong>%TIME%</strong>
<p style="margin-top:10px">Following Bus</p><strong>%TIME2%</strong>
</div>

<form action="/save">
<label>Station ID</label><input type="text" name="stopid" value="%STOPID%" required>
<a href="https://oebb.macistry.com/api/locations?query=Saalbach" target="_blank" class="helper-link">📍 Find station ID here</a>

<label>Destination ID</label><input type="text" name="destid" value="%DESTID%" required>
<a href="https://oebb.macistry.com/api/locations?query=Hochalm" target="_blank" class="helper-link">🎯 Find destination ID here</a>

<label>Brightness (20-255)</label><input type="number" name="bri" value="%BRI%" min="20" max="255" required>

<label>🌙 Night Mode</label>
<div class="checkbox-wrapper">
<input type="checkbox" name="night" id="night" value="1" %NIGHT_CHECKED%>
<label for="night">Enable night mode (turn off display & API calls)</label>
</div>

<div class="time-row">
<div>
<label>Start Time</label>
<input type="time" name="nightstart" value="%NIGHT_START%" required>
</div>
<div>
<label>End Time</label>
<input type="time" name="nightend" value="%NIGHT_END%" required>
</div>
</div>

<button type="submit" class="btn">💾 Save Settings</button>
</form>

<div class="info-text">ℹ️ API checked every 2 minutes</div>
<a href="/update" class="link">⚙️ OTA Firmware Update</a>
</div>
</body>
</html>
)";

// ===================== HANDLERS =====================
void handleRoot() {
    String page = FPSTR(html);
    page.replace("%IP%", WiFi.localIP().toString());
    page.replace("%TIME%", busInfo.departureTime);
    page.replace("%TIME2%", busInfo.secondDepartureTime);
    page.replace("%STOPID%", config.stop_id);
    page.replace("%DESTID%", config.destination_id);
    page.replace("%BRI%", String(config.bri));

    page.replace("%NIGHT_CHECKED%", config.night_mode_enabled ? "checked" : "");

    char timeStr[6];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", config.night_start_hour, config.night_start_minute);
    page.replace("%NIGHT_START%", timeStr);
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", config.night_end_hour, config.night_end_minute);
    page.replace("%NIGHT_END%", timeStr);

    if (isNightMode()) {
        page.replace("%NIGHT_BADGE%", "<span class=\"night-badge\">🌙 NIGHT MODE</span>");
    } else {
        page.replace("%NIGHT_BADGE%", "");
    }

    if (busInfo.isDelayed) {
        page.replace("%GRADIENT%", "#ff4444 0%, #cc0000 100%");
        page.replace("%STATUS_BG%", "#ffe6e6");
        page.replace("%STATUS_BORDER%", "#ff4444");
        page.replace("%STATUS_COLOR%", "#cc0000");
        page.replace("%DELAY_BADGE%", "<span class=\"delay-badge\">⚠️ DELAYED</span>");
    } else {
        page.replace("%GRADIENT%", "#667eea 0%, #764ba2 100%");
        page.replace("%STATUS_BG%", "#f0f4ff");
        page.replace("%STATUS_BORDER%", "#667eea");
        page.replace("%STATUS_COLOR%", "#667eea");
        page.replace("%DELAY_BADGE%", "");
    }

    server.send(200, "text/html", page);
}

void handleSave() {
    strlcpy(config.stop_id, server.arg("stopid").c_str(), sizeof(config.stop_id));
    strlcpy(config.destination_id, server.arg("destid").c_str(), sizeof(config.destination_id));
    config.bri = server.arg("bri").toInt();
    config.night_mode_enabled = server.hasArg("night");

    String nightStart = server.arg("nightstart");
    config.night_start_hour = nightStart.substring(0, 2).toInt();
    config.night_start_minute = nightStart.substring(3, 5).toInt();

    String nightEnd = server.arg("nightend");
    config.night_end_hour = nightEnd.substring(0, 2).toInt();
    config.night_end_minute = nightEnd.substring(3, 5).toInt();

    EEPROM.put(0, config);
    EEPROM.commit();
    server.send(200, "text/plain", "Saved. Restarting...");
    delay(1500);
    ESP.restart();
}

// ===================== SETUP =====================
void setup() {
    Serial.begin(115200);
    pinMode(TFT_BL, OUTPUT);

    tft.init();
    tft.setRotation(0);
    tft.invertDisplay(true);

    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(0, config);

    if (config.bri < 20 || config.bri > 255) {
        strlcpy(config.stop_id, "1350662", sizeof(config.stop_id));
        strlcpy(config.destination_id, "596051", sizeof(config.destination_id));
        config.bri = 200;
        config.duration_mode = 0;
        config.night_mode_enabled = false;
        config.night_start_hour = 23;
        config.night_start_minute = 0;
        config.night_end_hour = 6;
        config.night_end_minute = 0;
        EEPROM.put(0, config);
        EEPROM.commit();
    }

    setBrightness(config.bri);

    displayBootInfo("Starting WiFi...");

    WiFiManager wm;
    wm.autoConnect("BusDisplay");

    displayBootInfo("Connected!");
    delay(1000);

    displayBootInfo(WiFi.localIP().toString());
    delay(3000);

    configTime(3600, 3600, "at.pool.ntp.org", "pool.ntp.org");

    server.on("/", handleRoot);
    server.on("/save", handleSave);
    ElegantOTA.begin(&server);
    server.begin();

    if (!isNightMode()) {
        if (fetchBus()) displayBus();
    } else {
        displayNightMode();
    }
}

// ===================== LOOP =====================
void loop() {
    static unsigned long last = 0;
    static bool wasNightMode = false;

    server.handleClient();
    ElegantOTA.loop();

    bool nightNow = isNightMode();

    if (nightNow != wasNightMode) {
        wasNightMode = nightNow;
        if (nightNow) displayNightMode();
        else { setBrightness(config.bri); if (fetchBus()) displayBus(); }
    }

    if (!nightNow && millis() - last > 120000) {
        last = millis();
        if (fetchBus()) displayBus();
    }
}
