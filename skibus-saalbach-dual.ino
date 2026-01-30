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
#define DARK_BLUE 0x001F
#define DELAY_RED 0xF800
#define YELLOW 0xCD80
#define PURPLE 0xF81F

#define TFT_BL 5
TFT_eSPI tft = TFT_eSPI();
ESP8266WebServer server(80);

// ===================== DATA =====================
struct BusData {
    String departureTime;
    String destination;
    String lineNumber;
    int delayMinutes;
    bool isDelayed;
    bool farAway;
    bool isSki;
} busInfo1, busInfo2;

String debugMsg = "Starting...";
int httpStatus = 0;

#define EEPROM_SIZE 256

struct Config {
    char stop_id_1[12];
    char destination_name_1[50];
    char stop_id_2[12];
    char destination_name_2[50];
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
void processDepartures(JsonArray departures, BusData &busInfo, const char* destinationFilter) {
    if (departures.isNull() || departures.size() == 0) {
        busInfo.departureTime = "No buses";
        busInfo.lineNumber = "";
        busInfo.farAway = false;
        busInfo.isSki = false;
        return;
    }

    // Filter departures by destination name (partial match)
    JsonObject selectedBus;
    bool found = false;
    
    for (JsonObject bus : departures) {
        String direction = bus["direction"] | "";
        
        // Check if the direction contains the destination filter (case-insensitive partial match)
        if (destinationFilter[0] == '\0' || direction.indexOf(destinationFilter) >= 0) {
            selectedBus = bus;
            found = true;
            break;
        }
    }
    
    if (!found) {
        busInfo.departureTime = "No match";
        busInfo.lineNumber = "";
        busInfo.farAway = false;
        busInfo.isSki = false;
        return;
    }

    const char* whenStr = selectedBus["when"] | "";
    busInfo.departureTime = String(whenStr).substring(11, 16);
    busInfo.destination = selectedBus["direction"] | "";
    busInfo.lineNumber = selectedBus["line"]["name"] | "";
    
    // Check if it's a ski bus
    busInfo.isSki = (busInfo.lineNumber == "Bus SKI");
    
    int delaySec = selectedBus["delay"] | 0;
    busInfo.delayMinutes = delaySec / 60;
    busInfo.isDelayed = delaySec > 60;

    int minsUntilDep = minutesUntil(whenStr);
    busInfo.farAway = minsUntilDep > 360;
}

// ===================== FETCH API =====================
bool fetchBus(const char* stopId, const char* destinationName, BusData &busInfo, int retries = 2) {
    for (int attempt = 0; attempt < retries; attempt++) {
        WiFiClientSecure client;
        client.setInsecure();
        client.setTimeout(15000);

        HTTPClient http;
        http.setTimeout(15000);
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

        String url =
            "https://oebb.macistry.com/api/stops/" + String(stopId) +
            "/departures?bus=true&results=10";

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
        processDepartures(departures, busInfo, destinationName);
        return true;
    }
    return false;
}

// ===================== DISPLAY =====================
void displayBus() {
    tft.fillScreen(TFT_BLACK);
    
    // ===== UPPER HALF - Bus 1 =====
    uint32_t bg1;
    if (busInfo1.departureTime == "No match" || busInfo1.departureTime == "No buses") {
        // No match or no buses: purple
        bg1 = PURPLE;
    } else if (busInfo1.isDelayed) {
        // Any delayed bus: red
        bg1 = DELAY_RED;
    } else if (busInfo1.isSki) {
        // Ski bus on time: blue
        bg1 = DARK_BLUE;
    } else {
        // Non-ski bus on time: yellow
        bg1 = YELLOW;
    }
    
    tft.fillRect(0, 0, 240, 120, bg1);
    tft.setTextColor(TFT_WHITE, bg1);  // White text on colored background
    tft.setTextDatum(MC_DATUM);

    if (busInfo1.farAway) {
        tft.drawString("Next > 6h", 120, 68, 4);  // Moved down 8 pixels
    } else {
        tft.setFreeFont(&FreeSansBold24pt7b);
        tft.drawString(busInfo1.departureTime, 120, 60);  // Using free font
        // Display bus type
        tft.setTextFont(2);  // Reset to default font
        String busType1 = busInfo1.isSki ? "Bus SKI" : busInfo1.lineNumber;
        tft.drawString(busType1, 120, 98, 2);  // Moved down 8 pixels
    }

    // ===== LOWER HALF - Bus 2 =====
    uint32_t bg2;
    if (busInfo2.departureTime == "No match" || busInfo2.departureTime == "No buses") {
        // No match or no buses: purple
        bg2 = PURPLE;
    } else if (busInfo2.isDelayed) {
        // Any delayed bus: red
        bg2 = DELAY_RED;
    } else if (busInfo2.isSki) {
        // Ski bus on time: blue
        bg2 = DARK_BLUE;
    } else {
        // Non-ski bus on time: yellow
        bg2 = YELLOW;
    }
    
    tft.fillRect(0, 120, 240, 120, bg2);
    tft.setTextColor(TFT_WHITE, bg2);  // White text on colored background
    tft.setTextDatum(MC_DATUM);

    if (busInfo2.farAway) {
        tft.drawString("Next > 6h", 120, 188, 4);  // Moved down 8 pixels 188
    } else {
        tft.setFreeFont(&FreeSansBold24pt7b);
        tft.drawString(busInfo2.departureTime, 120, 170);  // Using free font 180
        // Display bus type
        tft.setTextFont(2);  // Reset to default font
        String busType2 = busInfo2.isSki ? "Bus SKI" : busInfo2.lineNumber;
        tft.drawString(busType2, 120, 210, 2);  // Moved down 8 pixels 218
    }
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
body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh;padding:20px;display:flex;justify-content:center;align-items:center;}
.container{background:white;border-radius:16px;box-shadow:0 20px 60px rgba(0,0,0,0.3);padding:30px;max-width:600px;width:100%;}
h2{color:#333;margin-bottom:10px;font-size:28px;}
h3{color:#555;margin-top:25px;margin-bottom:15px;font-size:20px;border-bottom:2px solid #667eea;padding-bottom:8px;}
.status{background:#f0f4ff;padding:15px;border-radius:8px;margin:20px 0;border-left:4px solid #667eea;}
.status p{margin:5px 0;color:#555;font-size:14px;}
.status strong{color:#667eea;font-size:20px;display:block;margin-top:5px;}
.bus-section{background:#f9f9f9;padding:15px;border-radius:8px;margin:10px 0;}
.bus-section h4{color:#333;margin-bottom:10px;font-size:16px;}
.delay-badge{display:inline-block;background:#ff4444;color:white;padding:4px 12px;border-radius:12px;font-size:12px;font-weight:bold;margin-left:10px;}
.night-badge{display:inline-block;background:#333;color:white;padding:4px 12px;border-radius:12px;font-size:12px;font-weight:bold;margin-left:10px;}
.ski-badge{display:inline-block;background:#4CAF50;color:white;padding:4px 12px;border-radius:12px;font-size:12px;font-weight:bold;margin-left:10px;}
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
<h2>🚌 Glemmtal buses</h2>
<div class="status">
<p>IP Address</p><strong>%IP%</strong>%NIGHT_BADGE%
</div>

<div class="bus-section">
<h4>🔵 Upper Display - Station 1%DELAY_BADGE_1%%SKI_BADGE_1%</h4>
<p><strong>Next Bus:</strong> %TIME1%</p>
<p><strong>Line:</strong> %LINE1%</p>
</div>

<div class="bus-section">
<h4>🔵 Lower Display - Station 2%DELAY_BADGE_2%%SKI_BADGE_2%</h4>
<p><strong>Next Bus:</strong> %TIME2%</p>
<p><strong>Line:</strong> %LINE2%</p>
</div>

<form action="/save">
<h3>🚏 Station 1 (Upper Display)</h3>
<label>Station ID</label><input type="text" name="stopid1" value="%STOPID1%" required>
<a href="https://oebb.macistry.com/api/locations?query=Saalbach" target="_blank" class="helper-link">📍 Find station ID here</a>

<label>Destination Name (partial match, e.g., "Saalbach" is ok)</label><input type="text" name="destname1" value="%DESTNAME1%" required>
<p style="font-size:12px;color:#666;margin-top:5px;">💡 Enter part of the destination name to filter buses</p>

<h3>🚏 Station 2 (Lower Display)</h3>
<label>Station ID</label><input type="text" name="stopid2" value="%STOPID2%" required>
<a href="https://oebb.macistry.com/api/locations?query=Hinterglemm" target="_blank" class="helper-link">📍 Find station ID here</a>

<label>Destination Name (partial match, e.g., "Hinterglemm" is ok)</label><input type="text" name="destname2" value="%DESTNAME2%" required>
<p style="font-size:12px;color:#666;margin-top:5px;">💡 Enter part of the destination name to filter buses</p>

<h3>⚙️ Display Settings</h3>
<label>Brightness (20-255)</label><input type="number" name="bri" value="%BRI%" min="20" max="255" required>

<label>🌙 Night Mode</label>
<div class="checkbox-wrapper">
<input type="checkbox" name="night" id="night" value="1" %NIGHT_CHECKED%>
<label for="night">Enable night mode (turn off display & API calls) Recommended!</label>
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

<a href="/update" class="link">⚙️ OTA Firmware Update</a>
</div>
</body>
</html>
)";

// ===================== HANDLERS =====================
void handleRoot() {
    String page = FPSTR(html);
    page.replace("%IP%", WiFi.localIP().toString());
    
    // Bus 1 info
    page.replace("%TIME1%", busInfo1.departureTime);
    page.replace("%LINE1%", busInfo1.lineNumber.length() > 0 ? busInfo1.lineNumber : "N/A");
    page.replace("%STOPID1%", config.stop_id_1);
    page.replace("%DESTNAME1%", config.destination_name_1);
    
    // Bus 2 info
    page.replace("%TIME2%", busInfo2.departureTime);
    page.replace("%LINE2%", busInfo2.lineNumber.length() > 0 ? busInfo2.lineNumber : "N/A");
    page.replace("%STOPID2%", config.stop_id_2);
    page.replace("%DESTNAME2%", config.destination_name_2);
    
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

    // Delay badges
    if (busInfo1.isDelayed) {
        page.replace("%DELAY_BADGE_1%", "<span class=\"delay-badge\">⚠️ DELAYED</span>");
    } else {
        page.replace("%DELAY_BADGE_1%", "");
    }
    
    if (busInfo2.isDelayed) {
        page.replace("%DELAY_BADGE_2%", "<span class=\"delay-badge\">⚠️ DELAYED</span>");
    } else {
        page.replace("%DELAY_BADGE_2%", "");
    }

    // Ski badges
    if (busInfo1.isSki) {
        page.replace("%SKI_BADGE_1%", "<span class=\"ski-badge\">⛷️ SKI</span>");
    } else {
        page.replace("%SKI_BADGE_1%", "");
    }
    
    if (busInfo2.isSki) {
        page.replace("%SKI_BADGE_2%", "<span class=\"ski-badge\">⛷️ SKI</span>");
    } else {
        page.replace("%SKI_BADGE_2%", "");
    }

    server.send(200, "text/html", page);
}

void handleSave() {
    strlcpy(config.stop_id_1, server.arg("stopid1").c_str(), sizeof(config.stop_id_1));
    strlcpy(config.destination_name_1, server.arg("destname1").c_str(), sizeof(config.destination_name_1));
    strlcpy(config.stop_id_2, server.arg("stopid2").c_str(), sizeof(config.stop_id_2));
    strlcpy(config.destination_name_2, server.arg("destname2").c_str(), sizeof(config.destination_name_2));
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
        strlcpy(config.stop_id_1, "1350662", sizeof(config.stop_id_1));
        strlcpy(config.destination_name_1, "Hochalm", sizeof(config.destination_name_1));
        strlcpy(config.stop_id_2, "596060", sizeof(config.stop_id_2));
        strlcpy(config.destination_name_2, "Mitterlengau", sizeof(config.destination_name_2));
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

    configTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org", "time.nist.gov");

    displayBootInfo("Syncing Time...");

    // Check time sync with visual feedback
    for (int i = 0; i < 6; i++) {
        delay(500);
        yield();
        time_t now = time(nullptr);
        if (now > 1700000000) {
            debugMsg = "Time Synced";
            Serial.println("Time synced! Timestamp: " + String(now));
            struct tm* timeinfo = localtime(&now);
            char buffer[30];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
            Serial.println("Current time: " + String(buffer));
            displayBootInfo("Time Synced!");
            delay(1000);
            break;
        }
        
        String dots = "Syncing Time";
        for (int j = 0; j <= (i % 4); j++) {
            dots += ".";
        }
        
        tft.fillScreen(TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(dots, 120, 120, 2);
    }

    time_t finalCheck = time(nullptr);
    if (finalCheck < 1700000000) {
        config.night_mode_enabled = false; 
        debugMsg = "Time Sync Failed";
        Serial.println("Time sync FAILED! Will retry in background");
        displayBootInfo("Time Sync Failed");
        delay(1000);
    }

    displayBootInfo("Starting Server...");

    server.on("/", handleRoot);
    server.on("/save", handleSave);
    ElegantOTA.begin(&server);
    server.begin();

    displayBootInfo("Fetching Buses...");
    delay(500);

    if (!isNightMode()) {
        fetchBus(config.stop_id_1, config.destination_name_1, busInfo1);
        fetchBus(config.stop_id_2, config.destination_name_2, busInfo2);
        displayBus();
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
        if (nightNow) {
            displayNightMode();
        } else { 
            setBrightness(config.bri); 
            fetchBus(config.stop_id_1, config.destination_name_1, busInfo1);
            fetchBus(config.stop_id_2, config.destination_name_2, busInfo2);
            displayBus();
        }
    }

    if (!nightNow && millis() - last > 120000) {
        last = millis();
        fetchBus(config.stop_id_1, config.destination_name_1, busInfo1);
        fetchBus(config.stop_id_2, config.destination_name_2, busInfo2);
        displayBus();
    }
}
