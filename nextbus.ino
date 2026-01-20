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
    client.setInsecure();               // Tillåt alla certifikat
    client.setTimeout(15000);

    HTTPClient http;
    http.setTimeout(15000);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    auto fetchOnce = [&](int durationMinutes) -> bool {
        String url =
            "https://api.resrobot.se/v2.1/departureBoard?"
            "id=" + String(config.stop_id) +
            "&format=json"
            "&accessId=" + String(config.api_key) +
            "&maxJourneys=10"
            "&duration=" + String(durationMinutes);

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

        WiFiClient* stream = http.getStreamPtr();
        DynamicJsonDocument doc(4096); // max 4 KB, justerat för större hållplatser
        DeserializationError err = deserializeJson(doc, *stream);
        http.end();

        if (err) {
            debugMsg = "JSON fel: " + String(err.c_str());
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
            debugMsg = "Inga matchande avgångar";
            return false;
        } else {
            debugMsg = "Inga avgångar inom " + String(durationMinutes) + " min";
            return false;
        }
    };

    if (fetchOnce(60)) return true;
    if (fetchOnce(720)) return true;

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
<!DOCTYPE html><html><body>
<h2>Bus Display</h2>
<p>IP: %IP%</p>
<p>Nästa buss: %TIME%</p>
<form action="/save">
API-key:<br><input name="apikey" value="%APIKEY%"><br>
Stop ID:<br><input name="stopid" value="%STOPID%"><br>
Destination:<br><input name="dir" value="%DIR%"><br>
Ljusstyrka:<br><input name="bri" value="%BRI%"><br>
<input type="submit" value="Spara">
</form>
<a href="/update">OTA</a>
</body></html>
)";

void handleRoot() {
    String page = FPSTR(html);
    page.replace("%IP%", WiFi.localIP().toString());
    page.replace("%TIME%", busInfo.departureTime);
    page.replace("%APIKEY%", config.api_key);
    page.replace("%STOPID%", config.stop_id);
    page.replace("%DIR%", config.direction);
    page.replace("%BRI%", String(config.bri));
    server.send(200, "text/html", page);
}

void handleSave() {
    strlcpy(config.api_key, server.arg("apikey").c_str(), sizeof(config.api_key));
    strlcpy(config.stop_id, server.arg("stopid").c_str(), sizeof(config.stop_id));
    strlcpy(config.direction, server.arg("dir").c_str(), sizeof(config.direction));
    config.bri = server.arg("bri").toInt();
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

    if (millis() - last > 600000) {
        last = millis();
        if (fetchBus()) {
            displayBus();
        }
    }
}
