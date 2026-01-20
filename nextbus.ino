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

struct BusData {
    String departureTime;
    String rtDepartureTime;
    String destination;
    int delayMinutes;
    bool isDelayed;
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

void setBrightness(uint8_t val) {
    analogWrite(TFT_BL, 255 - val); 
    brightness = val; 
}

bool isNightMode(uint8_t currentHour, uint8_t currentMinute) {
    if (!config.night_mode_enabled) return false;
    int currentMinutes = currentHour * 60 + currentMinute;
    int startMinutes = config.night_start_hour * 60 + config.night_start_minute;
    int endMinutes = config.night_end_hour * 60 + config.night_end_minute;
    if (startMinutes > endMinutes) return (currentMinutes >= startMinutes || currentMinutes < endMinutes);
    else return (currentMinutes >= startMinutes && currentMinutes < endMinutes);
}

// --- HTML / Web Server ---
const char html[] PROGMEM = R"(
<!DOCTYPE html><html><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Bus Config</title><style>
body{font-family:Arial;max-width:600px;margin:20px auto;padding:10px;background:#f0f0f0}
.box{background:#fff;padding:15px;margin:10px 0;border-radius:5px}
input[type="text"],input[type="number"]{width:100%;padding:8px;margin:5px 0;box-sizing:border-box}
input[type="submit"],.btn{background:#9333ea;color:#fff;padding:10px;border:none;border-radius:5px;width:100%;cursor:pointer;text-decoration:none;display:block;text-align:center;margin:5px 0}
.btn{background:#059669}
label{font-weight:bold;display:block;margin-top:10px}
</style></head><body>
<div class="box"><h2>Bus Display</h2>
<p>IP: %IP%<br>Nästa buss: %TIME% %DELAY%</p></div>
<form action="/save" method="GET">
<div class="box"><h3>API Inställningar</h3>
<label>ResRobot API-nyckel:</label>
<input type="text" name="apikey" value="%APIKEY%">
<label>Station ID:</label>
<input type="text" name="stopid" value="%STOPID%">
<label>Destination (filter):</label>
<input type="text" name="dir" value="%DIR%">
</div>
<div class="box"><h3>Display</h3>
<label>Ljusstyrka (20-255):</label>
<input type="number" name="bri" value="%BRI%">
<label>Nattläge:</label>
<select name="night_en" style="width:100%;padding:8px"><option value="1"%NE1%>På</option><option value="0"%NE0%>Av</option></select>
<div style="margin-top:10px">
<label>Start:</label>
<input type="number" name="night_sh" value="%NSH%" style="width:45%;display:inline"> : 
<input type="number" name="night_sm" value="%NSM%" style="width:45%;display:inline"><br>
<label>Slut:</label>
<input type="number" name="night_eh" value="%NEH%" style="width:45%;display:inline"> : 
<input type="number" name="night_em" value="%NEM%" style="width:45%;display:inline">
</div>
</div>
<input type="submit" value="Spara och Starta om">
</form>
<a href="/update" class="btn">OTA Uppdatering</a>
</body></html>
)";

void handleRoot() {
    String page = FPSTR(html);
    page.replace("%IP%", WiFi.localIP().toString());
    page.replace("%TIME%", busInfo.departureTime != "" ? busInfo.departureTime : "Inga fler");
    page.replace("%DELAY%", busInfo.isDelayed ? "(FÖRSENAD)" : "");
    
    String fullDebug = debugMsg + "<br>Key len: " + String(strlen(config.api_key)) + 
                       "<br>Stop len: " + String(strlen(config.stop_id)) +
                       "<br>Dir len: " + String(strlen(config.direction));
    
    page.replace("%DEBUG%", fullDebug);
    page.replace("%HTTPSTATUS%", String(httpStatus));
    page.replace("%APIKEY%", String(config.api_key));
    page.replace("%STOPID%", String(config.stop_id));
    page.replace("%DIR%", String(config.direction));
    page.replace("%BRI%", String(config.bri));
    page.replace("%NE1%", config.night_mode_enabled ? " selected" : "");
    page.replace("%NE0%", config.night_mode_enabled ? "" : " selected");
    page.replace("%NSH%", String(config.night_start_hour));
    page.replace("%NSM%", String(config.night_start_minute));
    page.replace("%NEH%", String(config.night_end_hour));
    page.replace("%NEM%", String(config.night_end_minute));
    server.send(200, "text/html", page);
}

void handleSave() {
    strlcpy(config.api_key, server.arg("apikey").c_str(), sizeof(config.api_key));
    strlcpy(config.stop_id, server.arg("stopid").c_str(), sizeof(config.stop_id));
    strlcpy(config.direction, server.arg("dir").c_str(), sizeof(config.direction));
    config.bri = server.arg("bri").toInt();
    config.night_mode_enabled = (server.arg("night_en") == "1");
    config.night_start_hour = server.arg("night_sh").toInt();
    config.night_start_minute = server.arg("night_sm").toInt();
    config.night_end_hour = server.arg("night_eh").toInt();
    config.night_end_minute = server.arg("night_em").toInt();
    
    EEPROM.put(0, config);
    EEPROM.commit();
    
    server.send(200, "text/html", "Sparat! Startar om...");
    delay(2000);
    ESP.restart();
}

String fetchBusData() {
    if (strlen(config.api_key) < 5) {
        debugMsg = "API nyckel for kort";
        httpStatus = 0;
        return "";
    }
    
    debugMsg = "Ansluter...";
    
    WiFiClientSecure client;
    client.setInsecure();
    
    HTTPClient http;
    
    // 2 bussar men längre duration för landsbygd
    String url = "https://api.resrobot.se/v2.1/departureBoard?id=" + String(config.stop_id) + 
                 "&format=json&accessId=" + String(config.api_key) + 
                 "&maxJourneys=2&duration=720"; // 12 timmar framåt
    
    http.setTimeout(20000); // Längre timeout
    
    if (!http.begin(client, url)) {
        debugMsg = "Begin misslyckades";
        httpStatus = -1;
        return "";
    }
    
    debugMsg = "Skickar...";
    httpStatus = http.GET();
    
    debugMsg = "Svar: " + String(httpStatus);
    
    if (httpStatus == 200) {
        debugMsg = "Läser data...";
        
        // Kontrollera content length
        int len = http.getSize();
        debugMsg = "Size: " + String(len);
        
        if (len <= 0 && len != -1) {
            http.end();
            debugMsg = "Content length 0";
            return "";
        }
        
        // Läs med WiFiClient stream istället för getString()
        WiFiClient * stream = http.getStreamPtr();
        String payload = "";
        payload.reserve(4096); // Pre-allokera minne
        
        unsigned long timeout = millis();
        while (http.connected() && (len > 0 || len == -1)) {
            size_t size = stream->available();
            
            if (size) {
                // Läs i mindre bitar
                char buff[128];
                int c = stream->readBytes(buff, min((size_t)sizeof(buff), size));
                payload.concat(buff, c);
                
                if (len > 0) {
                    len -= c;
                }
                timeout = millis();
            }
            
            // Timeout efter 10 sekunder utan data
            if (millis() - timeout > 10000) {
                debugMsg = "Timeout vid läsning";
                http.end();
                return "";
            }
            
            // Avbryt om vi redan har tillräckligt med data (first 2 busses)
            if (payload.length() > 3000 && payload.indexOf("\"Departure\":[") > 0) {
                // Försök hitta slutet av andra bussen
                int firstBracket = payload.indexOf("{\"JourneyDetailRef\"");
                if (firstBracket > 0) {
                    int secondBracket = payload.indexOf("{\"JourneyDetailRef\"", firstBracket + 20);
                    if (secondBracket > 0) {
                        // Hitta slutet av andra objektet
                        int endBracket = payload.indexOf("},", secondBracket);
                        if (endBracket > secondBracket + 200) {
                            break; // Vi har tillräckligt
                        }
                    }
                }
            }
            
            yield();
        }
        
        http.end();
        
        if (payload.length() == 0) {
            debugMsg = "Tom efter läsning!";
            return "";
        }
        
        debugMsg = "OK! " + String(payload.length()) + "b";
        return payload;
        
    } else if (httpStatus == 301 || httpStatus == 302) {
        debugMsg = "Redirect fel";
        
    } else if (httpStatus == 401) {
        debugMsg = "401 - Fel API-nyckel";
    } else if (httpStatus == 404) {
        debugMsg = "404 - Fel Station ID";
    } else if (httpStatus == -1) {
        debugMsg = "Anslutning misslyckades";
    } else if (httpStatus == -2) {
        debugMsg = "Stream write fel";
    } else if (httpStatus == -3) {
        debugMsg = "Lost connection";
    } else if (httpStatus == -4) {
        debugMsg = "No stream";
    } else if (httpStatus == -5) {
        debugMsg = "Connection timeout";
    } else if (httpStatus == -11) {
        debugMsg = "Read timeout";
    } else if (httpStatus < 0) {
        debugMsg = "Fel: " + String(httpStatus);
    } else {
        debugMsg = "HTTP: " + String(httpStatus);
    }
    
    http.end();
    return "";
}

void displayBus() {
    uint32_t bgColor = busInfo.isDelayed ? DELAY_RED : DARK_PINK;
    tft.fillScreen(bgColor);
    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(TFT_WHITE, bgColor);
    tft.setTextSize(1);
    tft.drawString("Nasta buss", 120, 30, 4);
    tft.drawString("-> " + String(config.direction), 120, 65, 2);

    if (busInfo.departureTime != "") {
        String displayTime = busInfo.isDelayed ? busInfo.rtDepartureTime : busInfo.departureTime;
        displayTime = displayTime.substring(0, 5);
        
        tft.setTextColor(TFT_BLACK, bgColor);
        tft.setTextSize(1); 
        tft.drawString(displayTime, 120, 125, 6); 
        
        if (busInfo.isDelayed) {
            tft.setTextColor(TFT_WHITE, bgColor);
            tft.setTextSize(1);
            tft.drawString("FORSENAD +" + String(busInfo.delayMinutes) + "m", 120, 195, 4);
        }
    } else {
        tft.setTextColor(TFT_WHITE, bgColor);
        tft.drawString("Inga bussar", 120, 125, 4);
    }
    
    // Debug info längst ner
    tft.setTextColor(TFT_YELLOW, bgColor);
    tft.setTextSize(1);
    String shortDebug = debugMsg;
    if (shortDebug.length() > 30) shortDebug = shortDebug.substring(0, 30);
    tft.drawString(shortDebug, 120, 220, 1);
}

void processJSON(String json) {
    DynamicJsonDocument doc(16384); // Ökat från 8192 till 16384 bytes
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
        debugMsg = "JSON fel: " + String(error.c_str());
        
        tft.fillScreen(DARK_PINK);
        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(TFT_WHITE, DARK_PINK);
        tft.drawString("JSON Parse Fel", 120, 100, 2);
        tft.drawString(String(error.c_str()), 120, 130, 1);
        return;
    }

    struct tm timeinfo;
    time_t now = time(nullptr);
    localtime_r(&now, &timeinfo);
    
    if (isNightMode(timeinfo.tm_hour, timeinfo.tm_min)) {
        debugMsg = "Nattlage aktivt";
        setBrightness(0); 
        tft.fillScreen(TFT_BLACK); 
        return;
    } else {
        setBrightness(config.bri);
    }

    JsonArray departures = doc["Departure"];
    busInfo.departureTime = "";
    busInfo.isDelayed = false;

    if (departures.isNull()) {
        debugMsg = "Inga avgångar i JSON";
        displayBus();
        return;
    }

    int depCount = departures.size();
    debugMsg = String(depCount) + " avgångar, soker: " + String(config.direction);
    
    bool foundMatch = false;
    for (JsonObject dep : departures) {
        String dir = dep["direction"].as<String>();
        
        if (dir.indexOf(config.direction) >= 0) {
            busInfo.departureTime = dep["time"].as<String>();
            foundMatch = true;
            
            if (dep.containsKey("rtTime")) {
                busInfo.rtDepartureTime = dep["rtTime"].as<String>();
                int h1, m1, h2, m2;
                sscanf(busInfo.departureTime.c_str(), "%d:%d", &h1, &m1);
                sscanf(busInfo.rtDepartureTime.c_str(), "%d:%d", &h2, &m2);
                busInfo.delayMinutes = (h2 * 60 + m2) - (h1 * 60 + m1);
                if (busInfo.delayMinutes > 0) busInfo.isDelayed = true;
            }
            debugMsg = "Match! Tid: " + busInfo.departureTime;
            break; 
        }
    }
    
    if (!foundMatch) {
        debugMsg = "Ingen match. Forsta dest: " + departures[0]["direction"].as<String>();
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
    EEPROM.get(0, config);
    
    // Första gången - sätt defaultvärden
    if (config.bri < 20 || config.bri > 255) {
        strlcpy(config.api_key, "9e601a57-4232-4e2a-8fc2-a0ccaa3f44be", sizeof(config.api_key));
        strlcpy(config.stop_id, "740001149", sizeof(config.stop_id));
        strlcpy(config.direction, "Ystad", sizeof(config.direction));
        config.bri = 200;
        config.night_mode_enabled = false;
        config.night_start_hour = 22;
        config.night_start_minute = 0;
        config.night_end_hour = 6;
        config.night_end_minute = 0;
        EEPROM.put(0, config);
        EEPROM.commit();
    }
    
    setBrightness(config.bri);

    tft.fillScreen(DARK_PINK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Ansluter...", 120, 120, 4);

    WiFiManager wm;
    wm.autoConnect("BusDisplay_Setup");

    configTime(3600, 3600, "pool.ntp.org");
    
    server.on("/", handleRoot);
    server.on("/save", handleSave);
    ElegantOTA.begin(&server);
    server.begin();
    
    tft.fillScreen(DARK_PINK);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Hamtar data...", 120, 120, 2);
    // Låt loop() hantera första hämtningen
}

void loop() {
    static unsigned long lastUpdate = 0;
    static bool firstRun = true;
    static bool useHTTP = false; // Fallback till HTTP om HTTPS misslyckas
    
    server.handleClient();
    ElegantOTA.loop();

    if (firstRun || millis() - lastUpdate >= 600000) {
        firstRun = false;
        lastUpdate = millis();
        
        if (WiFi.status() == WL_CONNECTED) {
            // Visa WiFi-styrka
            int rssi = WiFi.RSSI();
            debugMsg = "WiFi: " + String(rssi) + "dBm";
            
            String data = fetchBusData();
            if (data != "") {
                processJSON(data);
            } else {
                // Om HTTPS misslyckades, försök HTTP nästa gång
                if (!useHTTP && httpStatus < 0) {
                    useHTTP = true;
                    debugMsg += " Provar HTTP...";
                }
                
                tft.fillScreen(DARK_PINK);
                tft.setTextDatum(MC_DATUM);
                tft.setTextColor(TFT_WHITE, DARK_PINK);
                tft.drawString("API Fel", 120, 80, 4);
                tft.setTextColor(TFT_YELLOW, DARK_PINK);
                tft.setTextSize(1);
                
                String shortDebug = debugMsg;
                if (shortDebug.length() > 35) shortDebug = shortDebug.substring(0, 35);
                tft.drawString(shortDebug, 120, 120, 1);
                tft.drawString("HTTP: " + String(httpStatus), 120, 140, 1);
                tft.drawString("Forsoker om 10 min", 120, 160, 1);
            }
        } else {
            debugMsg = "WiFi ej ansluten";
            tft.fillScreen(DARK_PINK);
            tft.setTextDatum(MC_DATUM);
            tft.setTextColor(TFT_WHITE, DARK_PINK);
            tft.drawString("WiFi Fel", 120, 120, 4);
        }
    }
    yield();
}
