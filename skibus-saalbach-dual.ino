// https://oebb.macistry.com/api/stops/1350662/departures?bus=true&direction=596051

// ===================== TIMETABLE DATA =====================
// Zwölferkogelbahn to Hochalmbahnen timetable
// const char* ZWOELFERKOGEL_STATION_ID = "1350662";
// const int ZWOELFERKOGEL_TIMETABLE_SIZE = 57;
// const char* zwoelferkogel_timetable[ZWOELFERKOGEL_TIMETABLE_SIZE] = {
//     "07:53", "08:23", "08:35", "08:45", "08:50", "09:00", "09:11", "09:16", 
//     "09:21", "09:26", "09:31", "09:36", "09:46", "09:56", "10:01", "10:06",
//     "10:11", "10:16", "10:26", "10:36", "10:46", "10:51", "10:56", "11:11",
//     "11:16", "11:36", "11:51", "11:56", "12:16", "12:31", "12:36", "12:56",
//     "13:16", "13:36", "13:51", "13:56", "14:16", "14:26", "14:31", "14:36",
//     "14:56", "15:01", "15:11", "15:16", "15:26", "15:31", "15:36", "15:46",
//     "15:51", "15:56", "16:11", "16:16", "16:26", "16:36", "16:56", "17:03"
//  };

// Wiesern to Saalbach timetable
// const char* WIESERN_STATION_ID = "596057";
// const int WIESERN_TIMETABLE_SIZE = 52;
// const char* wiesern_timetable[WIESERN_TIMETABLE_SIZE] = {
//     "08:33", "08:53", "08:58", "09:03", "09:23", "09:33", "09:38", "09:43",
//     "09:48", "09:53", "10:03", "10:13", "10:28", "10:33", "10:43", "10:48",
//     "10:53", "11:08", "11:13", "11:28", "11:33", "11:53", "12:08", "12:13",
//     "12:33", "12:53", "13:13", "13:28", "13:33", "13:53", "14:03", "14:08",
//     "14:13", "14:33", "14:38", "14:48", "14:53", "15:03", "15:13", "15:23",
//     "15:28", "15:33", "15:43", "15:48", "15:53", "16:03", "16:13", "16:28",
//     "16:33", "16:43", "16:53", "17:00"
//   };

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
#define ORANGE 0xFD20    // Orange färg för försenade SKI-bussar

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
    bool single_station_mode;  // NEW: true = 1 station, false = 2 stations
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

// NEW: Process departures and get multiple buses for single station mode
void processDeparturesMultiple(JsonArray departures, BusData &bus1, BusData &bus2, const char* destinationFilter) {
    if (departures.isNull() || departures.size() == 0) {
        bus1.departureTime = "No buses";
        bus1.lineNumber = "";
        bus1.farAway = false;
        bus1.isSki = false;
        bus2.departureTime = "No buses";
        bus2.lineNumber = "";
        bus2.farAway = false;
        bus2.isSki = false;
        return;
    }

    // Filter departures by destination name and get first two matches
    int foundCount = 0;
    
    for (JsonObject bus : departures) {
        String direction = bus["direction"] | "";
        
        // Check if the direction contains the destination filter
        if (destinationFilter[0] == '\0' || direction.indexOf(destinationFilter) >= 0) {
            BusData* targetBus = (foundCount == 0) ? &bus1 : &bus2;
            
            const char* whenStr = bus["when"] | "";
            targetBus->departureTime = String(whenStr).substring(11, 16);
            targetBus->destination = bus["direction"] | "";
            targetBus->lineNumber = bus["line"]["name"] | "";
            targetBus->isSki = (targetBus->lineNumber == "Bus SKI");
            
            int delaySec = bus["delay"] | 0;
            targetBus->delayMinutes = delaySec / 60;
            targetBus->isDelayed = delaySec > 60;

            int minsUntilDep = minutesUntil(whenStr);
            targetBus->farAway = minsUntilDep > 360;
            
            foundCount++;
            if (foundCount >= 2) break;
        }
    }
    
    // If only one bus found, set second bus to "No buses"
    if (foundCount == 1) {
        bus2.departureTime = "No buses";
        bus2.lineNumber = "";
        bus2.farAway = false;
        bus2.isSki = false;
    }
    
    // If no buses found
    if (foundCount == 0) {
        bus1.departureTime = "No match";
        bus1.lineNumber = "";
        bus1.farAway = false;
        bus1.isSki = false;
        bus2.departureTime = "No match";
        bus2.lineNumber = "";
        bus2.farAway = false;
        bus2.isSki = false;
    }
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

// NEW: Fetch multiple buses from single station
bool fetchBusMultiple(const char* stopId, const char* destinationName, BusData &bus1, BusData &bus2, int retries = 2) {
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
        processDeparturesMultiple(departures, bus1, bus2, destinationName);
        return true;
    }
    return false;
}

// ===================== DISPLAY =====================
void displayBus() {
    tft.fillScreen(TFT_BLACK);
    
    if (config.single_station_mode) {
        // ===== SINGLE STATION MODE - FULL SCREEN LAYOUT =====
        // Both buses displayed vertically on full screen
        
        // Determine background color based on most urgent bus (bus 1)
        uint32_t mainBg;
        if (busInfo1.departureTime == "No match" || busInfo1.departureTime == "No buses") {
            mainBg = PURPLE;
        } else if (busInfo1.isDelayed && busInfo1.isSki) {
            mainBg = ORANGE;
        } else if (busInfo1.isDelayed) {
            mainBg = DELAY_RED;
        } else if (busInfo1.isSki) {
            mainBg = DARK_BLUE;
        } else {
            mainBg = YELLOW;
        }
        
        // Fill entire screen with main background
        tft.fillScreen(mainBg);
        tft.setTextColor(TFT_WHITE, mainBg);
        tft.setTextDatum(MC_DATUM);
        
        // Display first bus (larger, top part)
        if (busInfo1.farAway) {
            tft.drawString("Next > 6h", 120, 80, 4);
        } else if (busInfo1.departureTime == "No match" || busInfo1.departureTime == "No buses") {
            tft.setTextFont(4);
            tft.drawString(busInfo1.departureTime, 120, 80, 4);
        } else {
            tft.setFreeFont(&FreeSansBold24pt7b);
            tft.drawString(busInfo1.departureTime, 120, 70);
            tft.setTextFont(2);
            String busType1 = busInfo1.isSki ? "Bus SKI" : busInfo1.lineNumber;
            tft.drawString(busType1, 120, 105, 2);
        }
        
        // Draw separator line
        tft.drawLine(20, 140, 220, 140, TFT_WHITE);
        tft.drawLine(20, 141, 220, 141, TFT_WHITE);
        
        // Display second bus (smaller, bottom part)
        tft.setTextFont(2);
        tft.drawString("Next:", 120, 160, 2);
        
        if (busInfo2.farAway) {
            tft.drawString("> 6h", 120, 185, 4);
        } else if (busInfo2.departureTime == "No match" || busInfo2.departureTime == "No buses") {
            tft.setTextFont(2);
            tft.drawString(busInfo2.departureTime, 120, 185, 2);
        } else {
            tft.setTextFont(4);
            tft.drawString(busInfo2.departureTime, 120, 185, 4);
            tft.setTextFont(2);
            String busType2 = busInfo2.isSki ? "Bus SKI" : busInfo2.lineNumber;
            tft.drawString(busType2, 120, 215, 2);
        }
        
    } else {
        // ===== TWO STATION MODE - SPLIT SCREEN =====
        
        // ===== UPPER HALF - Station 1 =====
        uint32_t bg1;
        if (busInfo1.departureTime == "No match" || busInfo1.departureTime == "No buses") {
            bg1 = PURPLE;
        } else if (busInfo1.isDelayed && busInfo1.isSki) {
            bg1 = ORANGE;
        } else if (busInfo1.isDelayed) {
            bg1 = DELAY_RED;
        } else if (busInfo1.isSki) {
            bg1 = DARK_BLUE;
        } else {
            bg1 = YELLOW;
        }
        
        tft.fillRect(0, 0, 240, 120, bg1);
        tft.setTextColor(TFT_WHITE, bg1);
        tft.setTextDatum(MC_DATUM);

        if (busInfo1.farAway) {
            tft.drawString("Next > 6h", 120, 68, 4);
        } else {
            tft.setFreeFont(&FreeSansBold24pt7b);
            tft.drawString(busInfo1.departureTime, 120, 60);
            tft.setTextFont(2);
            String busType1 = busInfo1.isSki ? "Bus SKI" : busInfo1.lineNumber;
            tft.drawString(busType1, 120, 98, 2);
        }

        // ===== LOWER HALF - Station 2 =====
        uint32_t bg2;
        if (busInfo2.departureTime == "No match" || busInfo2.departureTime == "No buses") {
            bg2 = PURPLE;
        } else if (busInfo2.isDelayed && busInfo2.isSki) {
            bg2 = ORANGE;
        } else if (busInfo2.isDelayed) {
            bg2 = DELAY_RED;
        } else if (busInfo2.isSki) {
            bg2 = DARK_BLUE;
        } else {
            bg2 = YELLOW;
        }
        
        tft.fillRect(0, 120, 240, 120, bg2);
        tft.setTextColor(TFT_WHITE, bg2);
        tft.setTextDatum(MC_DATUM);

        if (busInfo2.farAway) {
            tft.drawString("Next > 6h", 120, 188, 4);
        } else {
            tft.setFreeFont(&FreeSansBold24pt7b);
            tft.drawString(busInfo2.departureTime, 120, 170);
            tft.setTextFont(2);
            String busType2 = busInfo2.isSki ? "Bus SKI" : busInfo2.lineNumber;
            tft.drawString(busType2, 120, 210, 2);
        }
    }
}

void displayNightMode() {
    tft.fillScreen(TFT_BLACK);
    setBrightness(0);
}

// ===================== WEB =====================
const char html[] PROGMEM = R"=====(
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
.mode-badge{display:inline-block;background:#667eea;color:white;padding:4px 12px;border-radius:12px;font-size:12px;font-weight:bold;margin-left:10px;}
label{display:block;margin-top:15px;margin-bottom:5px;color:#333;font-weight:600;font-size:14px;}
input[type="text"],input[type="number"],input[type="time"],select{width:100%;padding:12px;border:2px solid #e0e0e0;border-radius:8px;font-size:16px;transition:border 0.3s;}
input[type="text"]:focus,input[type="number"]:focus,input[type="time"]:focus,select:focus{outline:none;border-color:#667eea;}
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
.station2-section{%STATION2_DISPLAY%}
@media(max-width:600px){.container{padding:20px;}h2{font-size:24px;}}
</style>
<script>
function toggleStationMode() {
    var mode = document.getElementById('stationmode').value;
    var station2 = document.getElementById('station2section');
    if (mode === '1') {
        station2.style.display = 'none';
    } else {
        station2.style.display = 'block';
    }
}
</script>
</head>
<body>
<div class="container">
<h2>Glemmtal buses</h2>
<div class="status">
<p>IP Address</p><strong>%IP%</strong>%NIGHT_BADGE%%MODE_BADGE%
</div>

<div class="bus-section">
<h4>%BUS1_LABEL%%DELAY_BADGE_1%%SKI_BADGE_1%</h4>
<p><strong>Next Bus:</strong> %TIME1%</p>
<p><strong>Line:</strong> %LINE1%</p>
</div>

<div class="bus-section">
<h4>%BUS2_LABEL%%DELAY_BADGE_2%%SKI_BADGE_2%</h4>
<p><strong>Next Bus:</strong> %TIME2%</p>
<p><strong>Line:</strong> %LINE2%</p>
</div>

<form action="/save">
<h3>Display Mode</h3>
<label>Station Mode</label>
<select name="stationmode" id="stationmode" onchange="toggleStationMode()" required>
<option value="2" %MODE2_SELECTED%>2 Stations (split screen)</option>
<option value="1" %MODE1_SELECTED%>1 Station </option>
</select>
<p style="font-size:12px;color:#666;margin-top:5px;">1 Station mode shows two consecutive buses from Station 1 on full screen</p>

<h3>Station 1 (Upper Display)</h3>
<label>Station ID</label><input type="text" name="stopid1" value="%STOPID1%" required>
<a href="https://oebb.macistry.com/api/locations?query=Saalbach" target="_blank" class="helper-link">Find station ID here</a>

<label>Destination Name (partial match, e.g., "Saalbach" is ok)</label><input type="text" name="destname1" value="%DESTNAME1%" required>
<p style="font-size:12px;color:#666;margin-top:5px;">Enter part of the destination name to filter buses</p>

<div id="station2section" class="station2-section">
<h3>Station 2 (Lower Display)</h3>
<label>Station ID</label><input type="text" name="stopid2" value="%STOPID2%" required>
<a href="https://oebb.macistry.com/api/locations?query=Hinterglemm" target="_blank" class="helper-link">Find station ID here</a>

<label>Destination Name (partial match, e.g., "Hinterglemm" is ok)</label><input type="text" name="destname2" value="%DESTNAME2%" required>
<p style="font-size:12px;color:#666;margin-top:5px;">Enter part of the destination name to filter buses</p>
</div>

<h3>Display Settings</h3>
<label>Brightness (20-255)</label><input type="number" name="bri" value="%BRI%" min="20" max="255" required>

<label>Night Mode</label>
<div class="checkbox-wrapper">
<input type="checkbox" name="night" id="night" value="1" %NIGHT_CHECKED%>
<label for="night">Enable night mode (turn off display and API calls) Recommended!</label>
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

<button type="submit" class="btn">Save Settings</button>
</form>

<a href="/update" class="link">OTA Firmware Update</a>
</div>
<script>toggleStationMode();</script>
</body>
</html>
)=====";

// ===================== HANDLERS =====================
void handleRoot() {
    String page = FPSTR(html);
    page.replace("%IP%", WiFi.localIP().toString());
    
    // Display mode badge and labels
    if (config.single_station_mode) {
        page.replace("%MODE_BADGE%", "<span class=\"mode-badge\">📺 1 STATION</span>");
        page.replace("%BUS1_LABEL%", "Next Bus (Station 1)");
        page.replace("%BUS2_LABEL%", "Following Bus (Station 1)");
        page.replace("%MODE1_SELECTED%", "selected");
        page.replace("%MODE2_SELECTED%", "");
    } else {
        page.replace("%MODE_BADGE%", "<span class=\"mode-badge\">📺 2 STATIONS</span>");
        page.replace("%BUS1_LABEL%", "Upper Display - Station 1");
        page.replace("%BUS2_LABEL%", "Lower Display - Station 2");
        page.replace("%MODE1_SELECTED%", "");
        page.replace("%MODE2_SELECTED%", "selected");
    }
    
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

    // Control Station 2 visibility
    if (config.single_station_mode) {
        page.replace("%STATION2_DISPLAY%", "display:none;");
    } else {
        page.replace("%STATION2_DISPLAY%", "");
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
    config.single_station_mode = (server.arg("stationmode") == "1");

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
        config.single_station_mode = false;  // Default to 2 station mode
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
        if (config.single_station_mode) {
            // Fetch two buses from station 1
            fetchBusMultiple(config.stop_id_1, config.destination_name_1, busInfo1, busInfo2);
        } else {
            // Fetch one bus from each station
            fetchBus(config.stop_id_1, config.destination_name_1, busInfo1);
            fetchBus(config.stop_id_2, config.destination_name_2, busInfo2);
        }
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
            if (config.single_station_mode) {
                fetchBusMultiple(config.stop_id_1, config.destination_name_1, busInfo1, busInfo2);
            } else {
                fetchBus(config.stop_id_1, config.destination_name_1, busInfo1);
                fetchBus(config.stop_id_2, config.destination_name_2, busInfo2);
            }
            displayBus();
        }
    }

    if (!nightNow && millis() - last > 120000) {
        last = millis();
        if (config.single_station_mode) {
            fetchBusMultiple(config.stop_id_1, config.destination_name_1, busInfo1, busInfo2);
        } else {
            fetchBus(config.stop_id_1, config.destination_name_1, busInfo1);
            fetchBus(config.stop_id_2, config.destination_name_2, busInfo2);
        }
        displayBus();
    }
}
