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
#define DARK_PINK 0xC318
#define DELAY_RED 0xF800

#define TFT_BL 5
TFT_eSPI tft = TFT_eSPI();
ESP8266WebServer server(80);

// ===================== DATA =====================
struct BusData {
    String departureTime;
    String rtDepartureTime;
    String destination;
    int delayMinutes;
    bool isDelayed;
    bool farAway;
} busInfo;

String debugMsg = "Startar...";
int httpStatus = 0;

#define EEPROM_SIZE 256

struct Config {
    char api_key[40];
    char stop_id[12];
    char direction[20];
    bool night_mode_enabled;
    uint8_t night_start_hour;
    uint8_t night_start_minute;
    uint8_t night_end_hour;
    uint8_t night_end_minute;
    uint8_t bri;
    uint8_t duration_mode; // 0=mycket få, 1=få, 2=många, 3=väldigt många
} config;

// ===================== LJUS =====================
void setBrightness(uint8_t val) {
    analogWrite(TFT_BL, 255 - val);
}

// ===================== TID =====================
int minutesUntil(const char* date, const char* timeStr) {
    struct tm t = {};
    t.tm_year = atoi(date) - 1900;
    t.tm_mon  = atoi(date + 5) - 1;
    t.tm_mday = atoi(date + 8);
    t.tm_hour = atoi(timeStr);
    t.tm_min  = atoi(timeStr + 3);
    time_t dep = mktime(&t);
    time_t now = time(nullptr);
    return (dep - now) / 60;
}

// ===================== API =====================
bool fetchBus() {
    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(15000);

    HTTPClient http;
    http.setTimeout(15000);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    // Durations baserat på användarvald nivå
    int durations[] = {5, 20, 60, 180}; // mycket få, få, många, väldigt många
    uint8_t mode = config.duration_mode > 3 ? 3 : config.duration_mode;
    int primaryDuration = durations[mode];
    int fallbackDuration = 720; // alltid 12h som fallback

    auto fetchOnce = [&](int durationMinutes) -> bool {
        String url =
            "https://api.resrobot.se/v2.1/departureBoard?"
            "id=" + String(config.stop_id) +
            "&format=json"
            "&accessId=" + String(config.api_key) +
            "&maxJourneys=15"
            "&duration=" + String(durationMinutes) +
            "&passlist=0"; // VIKTIGT: Minskar datamängd rejält!

        if (!http.begin(client, url)) {
            debugMsg = "HTTP begin fail";
            return false;
        }

        int status = http.GET();
        httpStatus = status;

        if (status != 200) {
            debugMsg = "HTTP " + String(status);
            http.end();
            return false;
        }

        // ========== STREAMING MED FILTER ==========
        // Vi filtrerar bort allt utom det vi behöver INNAN parsing
        WiFiClient* stream = http.getStreamPtr();
        
        StaticJsonDocument<200> filter;
        filter["Departure"][0]["date"] = true;
        filter["Departure"][0]["time"] = true;
        filter["Departure"][0]["rtTime"] = true;
        filter["Departure"][0]["direction"] = true;

        DynamicJsonDocument doc(3072); // 3KB räcker nu med filter
        DeserializationError err = deserializeJson(doc, *stream, DeserializationOption::Filter(filter));
        http.end();

        if (err) {
            debugMsg = "JSON: " + String(err.c_str());
            return false;
        }

        JsonArray departures = doc["Departure"];
        if (!departures.isNull() && departures.size() > 0) {
            for (JsonObject dep : departures) {
                String dir = dep["direction"].as<String>();
                String search = String(config.direction);
                dir.toLowerCase();
                search.toLowerCase();

                if (dir.indexOf(search) >= 0) {
                    const char* date    = dep["date"];
                    const char* timeStr = dep["time"];

                    busInfo.departureTime = timeStr;
                    busInfo.destination  = dep["direction"].as<String>();
                    busInfo.isDelayed    = dep.containsKey("rtTime");
                    busInfo.farAway      = minutesUntil(date, timeStr) > 360;

                    debugMsg = busInfo.farAway ? "Nästa >6h" : "OK";
                    return true;
                }
            }
            debugMsg = "Ingen buss mot " + String(config.direction);
            return false;
        } else {
            debugMsg = "Inga avg inom " + String(durationMinutes) + "min";
            return false;
        }
    };

    // Försök först med användarens val, sen fallback till 12h
    if (fetchOnce(primaryDuration)) return true;
    if (fetchOnce(fallbackDuration)) return true;

    busInfo.departureTime = "Inga bussar";
    busInfo.farAway = false;
    return true;
}

// ===================== DISPLAY =====================
void displayBus() {
    uint32_t bg = busInfo.isDelayed ? DELAY_RED : DARK_PINK;
    tft.fillScreen(bg);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE, bg);

    if (busInfo.farAway) {
        tft.drawString("Nästa > 6h", 120, 120, 4);
    } else {
        tft.drawString(busInfo.departureTime.substring(0, 5), 120, 120, 6);
    }

    tft.setTextSize(1);
    tft.drawString(debugMsg, 120, 220, 1);
}

// ===================== WEB =====================
const char html[] PROGMEM = R"(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
* { margin: 0; padding: 0; box-sizing: border-box; }
body {
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
  background: linear-gradient(135deg, %GRADIENT%);
  min-height: 100vh;
  padding: 20px;
  display: flex;
  justify-content: center;
  align-items: center;
}
.container {
  background: white;
  border-radius: 16px;
  box-shadow: 0 20px 60px rgba(0,0,0,0.3);
  padding: 30px;
  max-width: 500px;
  width: 100%;
}
h2 {
  color: #333;
  margin-bottom: 10px;
  font-size: 28px;
}
.status {
  background: %STATUS_BG%;
  padding: 15px;
  border-radius: 8px;
  margin: 20px 0;
  border-left: 4px solid %STATUS_BORDER%;
}
.status p {
  margin: 5px 0;
  color: #555;
  font-size: 14px;
}
.status strong {
  color: %STATUS_COLOR%;
  font-size: 24px;
  display: block;
  margin-top: 5px;
}
.delay-badge {
  display: inline-block;
  background: #ff4444;
  color: white;
  padding: 4px 12px;
  border-radius: 12px;
  font-size: 12px;
  font-weight: bold;
  margin-left: 10px;
}
label {
  display: block;
  margin-top: 15px;
  margin-bottom: 5px;
  color: #333;
  font-weight: 600;
  font-size: 14px;
}
input[type="text"], input[type="number"] {
  width: 100%;
  padding: 12px;
  border: 2px solid #e0e0e0;
  border-radius: 8px;
  font-size: 16px;
  transition: border 0.3s;
}
input[type="text"]:focus, input[type="number"]:focus {
  outline: none;
  border-color: #667eea;
}
.radio-group {
  margin: 15px 0;
  padding: 15px;
  background: #f9f9f9;
  border-radius: 8px;
}
.radio-option {
  display: flex;
  align-items: center;
  padding: 10px;
  margin: 5px 0;
  border-radius: 6px;
  cursor: pointer;
  transition: background 0.2s;
}
.radio-option:hover {
  background: #fff;
}
input[type="radio"] {
  margin-right: 10px;
  width: 18px;
  height: 18px;
  cursor: pointer;
}
.radio-option label {
  margin: 0;
  cursor: pointer;
  font-weight: 500;
  font-size: 14px;
}
.btn {
  width: 100%;
  padding: 14px;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  border: none;
  border-radius: 8px;
  font-size: 16px;
  font-weight: 600;
  cursor: pointer;
  margin-top: 20px;
  transition: transform 0.2s, box-shadow 0.2s;
}
.btn:hover {
  transform: translateY(-2px);
  box-shadow: 0 10px 20px rgba(102, 126, 234, 0.3);
}
.btn:active {
  transform: translateY(0);
}
.link {
  display: block;
  text-align: center;
  margin-top: 15px;
  color: #667eea;
  text-decoration: none;
  font-weight: 600;
}
.link:hover {
  text-decoration: underline;
}
.info-text {
  font-size: 13px;
  color: #666;
  margin-top: 15px;
  padding: 10px;
  background: #f0f0f0;
  border-radius: 6px;
}
@media (max-width: 600px) {
  .container { padding: 20px; }
  h2 { font-size: 24px; }
}
</style>
</head>
<body>
<div class="container">
  <h2>🚌 Bus Display</h2>
  
  <div class="status">
    <p>IP-adress</p>
    <strong>%IP%</strong>
    <p style="margin-top:10px">Nästa buss%DELAY_BADGE%</p>
    <strong>%TIME%</strong>
  </div>

  <form action="/save">
    <label>API-nyckel</label>
    <input type="text" name="apikey" value="%APIKEY%" required>
    
    <label>Hållplats-ID</label>
    <input type="text" name="stopid" value="%STOPID%" required>
    
    <label>Destination (sökterm)</label>
    <input type="text" name="dir" value="%DIR%" required>
    
    <label>Ljusstyrka (20-255)</label>
    <input type="number" name="bri" value="%BRI%" min="20" max="255" required>
    
    <label>Hållplatstyp</label>
    <div class="radio-group">
      <div class="radio-option">
        <input type="radio" name="mode" value="0" id="m0" %M0%>
        <label for="m0">🏡 Mycket få avgångar (landsbygd)</label>
      </div>
      <div class="radio-option">
        <input type="radio" name="mode" value="1" id="m1" %M1%>
        <label for="m1">🏘️ Få avgångar (tätort)</label>
      </div>
      <div class="radio-option">
        <input type="radio" name="mode" value="2" id="m2" %M2%>
        <label for="m2">🏙️ Många avgångar (stad)</label>
      </div>
      <div class="radio-option">
        <input type="radio" name="mode" value="3" id="m3" %M3%>
        <label for="m3">🚇 Väldigt många avgångar (storstadsknutpunkt)</label>
      </div>
    </div>
    
    <button type="submit" class="btn">💾 Spara inställningar</button>
  </form>
  
  <div class="info-text">
    ℹ️ API:et kollas var 5:e minut. Senaste uppdatering visas på displayen.
  </div>
  
  <a href="/update" class="link">⚙️ OTA Firmware Update</a>
</div>
</body>
</html>
)";

void handleRoot() {
    String page = FPSTR(html);
    page.replace("%IP%", WiFi.localIP().toString());
    page.replace("%TIME%", busInfo.departureTime);
    page.replace("%APIKEY%", config.api_key);
    page.replace("%STOPID%", config.stop_id);
    page.replace("%DIR%", config.direction);
    page.replace("%BRI%", String(config.bri));
    
    // Färgtema baserat på förseningsstatus
    if (busInfo.isDelayed) {
        page.replace("%GRADIENT%", "#ff4444 0%, #cc0000 100%");
        page.replace("%STATUS_BG%", "#ffe6e6");
        page.replace("%STATUS_BORDER%", "#ff4444");
        page.replace("%STATUS_COLOR%", "#cc0000");
        page.replace("%DELAY_BADGE%", "<span class=\"delay-badge\">⚠️ FÖRSENAD</span>");
    } else {
        page.replace("%GRADIENT%", "#667eea 0%, #764ba2 100%");
        page.replace("%STATUS_BG%", "#f0f4ff");
        page.replace("%STATUS_BORDER%", "#667eea");
        page.replace("%STATUS_COLOR%", "#667eea");
        page.replace("%DELAY_BADGE%", "");
    }
    
    // Radio buttons
    for (int i = 0; i < 4; i++) {
        String tag = "%M" + String(i) + "%";
        page.replace(tag, (config.duration_mode == i) ? "checked" : "");
    }
    
    server.send(200, "text/html", page);
}

void handleSave() {
    strlcpy(config.api_key, server.arg("apikey").c_str(), sizeof(config.api_key));
    strlcpy(config.stop_id, server.arg("stopid").c_str(), sizeof(config.stop_id));
    strlcpy(config.direction, server.arg("dir").c_str(), sizeof(config.direction));
    config.bri = server.arg("bri").toInt();
    config.duration_mode = server.arg("mode").toInt();
    
    EEPROM.put(0, config);
    EEPROM.commit();
    server.send(200, "text/plain", "Sparat. Startar om...");
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
        strlcpy(config.api_key, "DIN_API_KEY", sizeof(config.api_key));
        strlcpy(config.stop_id, "740001149", sizeof(config.stop_id));
        strlcpy(config.direction, "Ystad", sizeof(config.direction));
        config.bri = 200;
        config.duration_mode = 0; // Default: mycket få avgångar
        EEPROM.put(0, config);
        EEPROM.commit();
    }

    setBrightness(config.bri);

    WiFiManager wm;
    wm.autoConnect("BusDisplay");

    configTime(3600, 3600, "pool.ntp.org");

    server.on("/", handleRoot);
    server.on("/save", handleSave);
    ElegantOTA.begin(&server);
    server.begin();

    // 🔹 Gör första hämtningen direkt
    if (fetchBus()) displayBus();
}

// ===================== LOOP =====================
void loop() {
    static unsigned long last = 0;

    server.handleClient();
    ElegantOTA.loop();

    if (millis() - last > 300000) {
        last = millis();
        if (fetchBus()) {
            displayBus();
        }
    }
}
