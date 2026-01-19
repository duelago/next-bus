#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>

// Färger
#define DARK_PINK 0xC318  
#define DELAY_RED 0xF800  

#define TFT_BL 5 
TFT_eSPI tft = TFT_eSPI(); 
ESP8266WebServer server(80); 
uint8_t brightness = 200; 

// Struktur för att spara alla inställningar i EEPROM
struct DeviceConfig {
    char api_key[40];       // API-nyckel (ca 36 tecken)
    char stop_id[12];       // StationsID (9-10 siffror)
    char direction[20];     // Destination (t.ex. "Ystad")
    bool night_mode_enabled;
    uint8_t night_sh;
    uint8_t night_sm;
    uint8_t night_eh;
    uint8_t night_em;
    uint8_t bri;
} devConfig;

struct BusData {
    String departureTime;
    String rtDepartureTime;
    int delayMinutes;
    bool isDelayed;
} busInfo;

#define EEPROM_SIZE 512 // Ökad storlek för att rymma strängar

void setBrightness(uint8_t val) {
    analogWrite(TFT_BL, 255 - val); 
    brightness = val; 
}

// --- Webbgränssnitt ---
const char html[] PROGMEM = R"(
<!DOCTYPE html><html><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Bus Config</title><style>
body{font-family:Arial;max-width:600px;margin:20px auto;padding:10px;background:#f0f0f0}
.box{background:#fff;padding:15px;margin:10px 0;border-radius:5px}
input[type="text"], input[type="number"], select{width:100%;padding:8px;margin:5px 0;box-sizing:border-box}
input[type="submit"]{background:#9333ea;color:#fff;padding:10px;border:none;border-radius:5px;width:100%;cursor:pointer;font-size:16px}
label{font-weight:bold;display:block;margin-top:10px}
</style></head><body>
<div class="box"><h2>Inställningar</h2>
<form action="/save" method="GET">
<label>ResRobot API-nyckel:</label>
<input type="text" name="apikey" value="%APIKEY%" placeholder="Klistra in accessId">
<label>Från Station (ID):</label>
<input type="text" name="stopid" value="%STOPID%" placeholder="t.ex. 740001149">
<label>Mot Destination (Filter):</label>
<input type="text" name="dir" value="%DIR%" placeholder="t.ex. Ystad">
<label>Ljusstyrka (20-255):</label>
<input type="number" name="bri" value="%BRI%" min="20" max="255">
<hr>
<label>Nattläge:</label>
<select name="night_en"><option value="1"%NE1%>På</option><option value="0"%NE0%>Av</option></select>
<div style="display:flex; gap:10px; align-items:center">
Start: <input type="number" name="nsh" value="%NSH%" style="width:60px">:<input type="number" name="nsm" value="%NSM%" style="width:60px">
Slut: <input type="number" name="neh" value="%NEH%" style="width:60px">:<input type="number" name="nem" value="%NEM%" style="width:60px">
</div>
<input type="submit" value="Spara och Starta om">
</form></div>
<p style="text-align:center; font-size:12px">IP: %IP%</p>
</body></html>
)";

void handleRoot() {
    String page = FPSTR(html);
    page.replace("%APIKEY%", String(devConfig.api_key));
    page.replace("%STOPID%", String(devConfig.stop_id));
    page.replace("%DIR%", String(devConfig.direction));
    page.replace("%BRI%", String(devConfig.bri));
    page.replace("%NE1%", devConfig.night_mode_enabled ? " selected" : "");
    page.replace("%NE0%", devConfig.night_mode_enabled ? "" : " selected");
    page.replace("%NSH%", String(devConfig.night_sh));
    page.replace("%NSM%", String(devConfig.night_sm));
    page.replace("%NEH%", String(devConfig.night_eh));
    page.replace("%NEM%", String(devConfig.night_em));
    page.replace("%IP%", WiFi.localIP().toString());
    server.send(200, "text/html", page);
}

void handleSave() {
    strlcpy(devConfig.api_key, server.arg("apikey").c_str(), sizeof(devConfig.api_key));
    strlcpy(devConfig.stop_id, server.arg("stopid").c_str(), sizeof(devConfig.stop_id));
    strlcpy(devConfig.direction, server.arg("dir").c_str(), sizeof(devConfig.direction));
    devConfig.bri = server.arg("bri").toInt();
    devConfig.night_mode_enabled = (server.arg("night_en") == "1");
    devConfig.night_sh = server.arg("nsh").toInt();
    devConfig.night_sm = server.arg("nsm").toInt();
    devConfig.night_eh = server.arg("neh").toInt();
    devConfig.night_em = server.arg("nem").toInt();

    EEPROM.put(0, devConfig);
    EEPROM.commit();
    
    server.send(200, "text/html", "Sparat! Startar om...");
    delay(2000);
    ESP.restart();
}

// --- Logik ---

String fetchBusData() {
    if (strlen(devConfig.api_key) < 5) return ""; // Ingen nyckel sparad

    WiFiClientSecure client;
    client.setInsecure(); 
    HTTPClient http;
    
    String url = "https://api.resrobot.se/v2.1/departureBoard?id=" + String(devConfig.stop_id) + 
                 "&format=json&accessId=" + String(devConfig.api_key) + 
                 "&passlist=0&maxJourneys=10&duration=480";
    
    if (http.begin(client, url)) {
        int httpCode = http.GET();
        if (httpCode == 200) {
            String payload = http.getString();
            http.end();
            return payload;
        }
        http.end();
    }
    return "";
}

void displayBus() {
    uint32_t bgColor = busInfo.isDelayed ? DELAY_RED : DARK_PINK;
    tft.fillScreen(bgColor);
    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(TFT_WHITE, bgColor);
    tft.setTextSize(1);
    // Visa destinationen från inställningarna högst upp
    tft.drawString("Nasta buss", 120, 40, 4);
    tft.drawString("-> " + String(devConfig.direction), 120, 75, 2);

    if (busInfo.departureTime != "") {
        String displayTime = busInfo.isDelayed ? busInfo.rtDepartureTime : busInfo.departureTime;
        tft.setTextColor(TFT_BLACK, bgColor);
        tft.setTextSize(1);
        tft.drawString(displayTime.substring(0, 5), 120, 145, 6);
        
        if (busInfo.isDelayed) {
            tft.setTextColor(TFT_WHITE, bgColor);
            tft.drawString("FORSENAD +" + String(busInfo.delayMinutes) + "m", 120, 215, 4);
        }
    } else {
        tft.setTextColor(TFT_WHITE, bgColor);
        tft.drawString("Inga bussar", 120, 145, 4);
    }
}

void processJSON(String json) {
    DynamicJsonDocument doc(8192);
    if (deserializeJson(doc, json)) return;

    // Kolla nattläge
    struct tm timeinfo;
    time_t now = time(nullptr);
    localtime_r(&now, &timeinfo);
    
    int curMin = timeinfo.tm_hour * 60 + timeinfo.tm_min;
    int startMin = devConfig.night_sh * 60 + devConfig.night_sm;
    int endMin = devConfig.night_eh * 60 + devConfig.night_em;
    bool night;
    if (startMin > endMin) night = (curMin >= startMin || curMin < endMin);
    else night = (curMin >= startMin && curMin < endMin);

    if (devConfig.night_mode_enabled && night) {
        setBrightness(0); tft.fillScreen(TFT_BLACK); return;
    } else {
        setBrightness(devConfig.bri);
    }

    JsonArray departures = doc["Departure"];
    busInfo.departureTime = "";
    busInfo.isDelayed = false;

    if (!departures.isNull()) {
        for (JsonObject dep : departures) {
            String dir = dep["direction"].as<String>();
            // Använd det sparade filtret
            if (dir.indexOf(devConfig.direction) >= 0) {
                busInfo.departureTime = dep["time"].as<String>();
                if (dep.containsKey("rtTime")) {
                    busInfo.rtDepartureTime = dep["rtTime"].as<String>();
                    int h1, m1, h2, m2;
                    sscanf(busInfo.departureTime.c_str(), "%d:%d", &h1, &m1);
                    sscanf(busInfo.rtDepartureTime.c_str(), "%d:%d", &h2, &m2);
                    busInfo.delayMinutes = (h2 * 60 + m2) - (h1 * 60 + m1);
                    if (busInfo.delayMinutes > 0) busInfo.isDelayed = true;
                }
                break; 
            }
        }
    }
    displayBus();
}

void setup() {
    Serial.begin(115200);
    pinMode(TFT_BL, OUTPUT);
    tft.init();
    tft.setRotation(0);
    tft.invertDisplay(true);
    
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(0, devConfig);

    // Initialisera om EEPROM är tom (första körningen)
    if (devConfig.bri < 20 || devConfig.bri > 255) {
        strlcpy(devConfig.api_key, "", sizeof(devConfig.api_key));
        strlcpy(devConfig.stop_id, "740001149", sizeof(devConfig.stop_id));
        strlcpy(devConfig.direction, "Ystad", sizeof(devConfig.direction));
        devConfig.bri = 200;
        devConfig.night_mode_enabled = false;
    }
    
    setBrightness(devConfig.bri);

    WiFiManager wm;
    wm.autoConnect("BusDisplay_Setup");

    configTime(3600, 3600, "pool.ntp.org");
    
    server.on("/", handleRoot);
    server.on("/save", handleSave);
    ElegantOTA.begin(&server);
    server.begin();
    
    tft.fillScreen(DARK_PINK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Klar!", 120, 120, 4);
    delay(2000);
}

void loop() {
    static unsigned long lastUpdate = 0;
    server.handleClient();
    ElegantOTA.loop();

    if (millis() - lastUpdate >= 600000 || lastUpdate == 0) {
        lastUpdate = millis();
        if (WiFi.status() == WL_CONNECTED) {
            String data = fetchBusData();
            if (data != "") processJSON(data);
        }
    }
    yield();
}
