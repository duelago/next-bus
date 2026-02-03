// https://oebb.macistry.com/api/stops/1350662/departures?bus=true&direction=596051

// ===================== TIMETABLE DATA IN PROGMEM =====================
// Store timetables in flash memory instead of RAM to save precious heap space

// Zwölferkogelbahn to Hochalmbahnen timetable
const char ZWOELFERKOGEL_STATION_ID[] PROGMEM = "1350662";
const int ZWOELFERKOGEL_TIMETABLE_SIZE = 57;
const char zwoelferkogel_time_00[] PROGMEM = "07:53";
const char zwoelferkogel_time_01[] PROGMEM = "08:23";
const char zwoelferkogel_time_02[] PROGMEM = "08:35";
const char zwoelferkogel_time_03[] PROGMEM = "08:45";
const char zwoelferkogel_time_04[] PROGMEM = "08:50";
const char zwoelferkogel_time_05[] PROGMEM = "09:00";
const char zwoelferkogel_time_06[] PROGMEM = "09:11";
const char zwoelferkogel_time_07[] PROGMEM = "09:16";
const char zwoelferkogel_time_08[] PROGMEM = "09:21";
const char zwoelferkogel_time_09[] PROGMEM = "09:26";
const char zwoelferkogel_time_10[] PROGMEM = "09:31";
const char zwoelferkogel_time_11[] PROGMEM = "09:36";
const char zwoelferkogel_time_12[] PROGMEM = "09:46";
const char zwoelferkogel_time_13[] PROGMEM = "09:56";
const char zwoelferkogel_time_14[] PROGMEM = "10:01";
const char zwoelferkogel_time_15[] PROGMEM = "10:06";
const char zwoelferkogel_time_16[] PROGMEM = "10:11";
const char zwoelferkogel_time_17[] PROGMEM = "10:16";
const char zwoelferkogel_time_18[] PROGMEM = "10:26";
const char zwoelferkogel_time_19[] PROGMEM = "10:36";
const char zwoelferkogel_time_20[] PROGMEM = "10:46";
const char zwoelferkogel_time_21[] PROGMEM = "10:51";
const char zwoelferkogel_time_22[] PROGMEM = "10:56";
const char zwoelferkogel_time_23[] PROGMEM = "11:11";
const char zwoelferkogel_time_24[] PROGMEM = "11:16";
const char zwoelferkogel_time_25[] PROGMEM = "11:36";
const char zwoelferkogel_time_26[] PROGMEM = "11:51";
const char zwoelferkogel_time_27[] PROGMEM = "11:56";
const char zwoelferkogel_time_28[] PROGMEM = "12:16";
const char zwoelferkogel_time_29[] PROGMEM = "12:31";
const char zwoelferkogel_time_30[] PROGMEM = "12:36";
const char zwoelferkogel_time_31[] PROGMEM = "12:56";
const char zwoelferkogel_time_32[] PROGMEM = "13:16";
const char zwoelferkogel_time_33[] PROGMEM = "13:36";
const char zwoelferkogel_time_34[] PROGMEM = "13:51";
const char zwoelferkogel_time_35[] PROGMEM = "13:56";
const char zwoelferkogel_time_36[] PROGMEM = "14:16";
const char zwoelferkogel_time_37[] PROGMEM = "14:26";
const char zwoelferkogel_time_38[] PROGMEM = "14:31";
const char zwoelferkogel_time_39[] PROGMEM = "14:36";
const char zwoelferkogel_time_40[] PROGMEM = "14:56";
const char zwoelferkogel_time_41[] PROGMEM = "15:01";
const char zwoelferkogel_time_42[] PROGMEM = "15:11";
const char zwoelferkogel_time_43[] PROGMEM = "15:16";
const char zwoelferkogel_time_44[] PROGMEM = "15:26";
const char zwoelferkogel_time_45[] PROGMEM = "15:31";
const char zwoelferkogel_time_46[] PROGMEM = "15:36";
const char zwoelferkogel_time_47[] PROGMEM = "15:46";
const char zwoelferkogel_time_48[] PROGMEM = "15:51";
const char zwoelferkogel_time_49[] PROGMEM = "15:56";
const char zwoelferkogel_time_50[] PROGMEM = "16:11";
const char zwoelferkogel_time_51[] PROGMEM = "16:16";
const char zwoelferkogel_time_52[] PROGMEM = "16:26";
const char zwoelferkogel_time_53[] PROGMEM = "16:36";
const char zwoelferkogel_time_54[] PROGMEM = "16:56";
const char zwoelferkogel_time_55[] PROGMEM = "17:03";
const char zwoelferkogel_time_56[] PROGMEM = "17:16";

const char* const zwoelferkogel_timetable[] PROGMEM = {
    zwoelferkogel_time_00, zwoelferkogel_time_01, zwoelferkogel_time_02, zwoelferkogel_time_03,
    zwoelferkogel_time_04, zwoelferkogel_time_05, zwoelferkogel_time_06, zwoelferkogel_time_07,
    zwoelferkogel_time_08, zwoelferkogel_time_09, zwoelferkogel_time_10, zwoelferkogel_time_11,
    zwoelferkogel_time_12, zwoelferkogel_time_13, zwoelferkogel_time_14, zwoelferkogel_time_15,
    zwoelferkogel_time_16, zwoelferkogel_time_17, zwoelferkogel_time_18, zwoelferkogel_time_19,
    zwoelferkogel_time_20, zwoelferkogel_time_21, zwoelferkogel_time_22, zwoelferkogel_time_23,
    zwoelferkogel_time_24, zwoelferkogel_time_25, zwoelferkogel_time_26, zwoelferkogel_time_27,
    zwoelferkogel_time_28, zwoelferkogel_time_29, zwoelferkogel_time_30, zwoelferkogel_time_31,
    zwoelferkogel_time_32, zwoelferkogel_time_33, zwoelferkogel_time_34, zwoelferkogel_time_35,
    zwoelferkogel_time_36, zwoelferkogel_time_37, zwoelferkogel_time_38, zwoelferkogel_time_39,
    zwoelferkogel_time_40, zwoelferkogel_time_41, zwoelferkogel_time_42, zwoelferkogel_time_43,
    zwoelferkogel_time_44, zwoelferkogel_time_45, zwoelferkogel_time_46, zwoelferkogel_time_47,
    zwoelferkogel_time_48, zwoelferkogel_time_49, zwoelferkogel_time_50, zwoelferkogel_time_51,
    zwoelferkogel_time_52, zwoelferkogel_time_53, zwoelferkogel_time_54, zwoelferkogel_time_55,
    zwoelferkogel_time_56
};

// Wiesern to Saalbach timetable
const char WIESERN_STATION_ID[] PROGMEM = "596057";
const int WIESERN_TIMETABLE_SIZE = 52;
const char wiesern_time_00[] PROGMEM = "08:33";
const char wiesern_time_01[] PROGMEM = "08:53";
const char wiesern_time_02[] PROGMEM = "08:58";
const char wiesern_time_03[] PROGMEM = "09:03";
const char wiesern_time_04[] PROGMEM = "09:23";
const char wiesern_time_05[] PROGMEM = "09:33";
const char wiesern_time_06[] PROGMEM = "09:38";
const char wiesern_time_07[] PROGMEM = "09:43";
const char wiesern_time_08[] PROGMEM = "09:48";
const char wiesern_time_09[] PROGMEM = "09:53";
const char wiesern_time_10[] PROGMEM = "10:03";
const char wiesern_time_11[] PROGMEM = "10:13";
const char wiesern_time_12[] PROGMEM = "10:28";
const char wiesern_time_13[] PROGMEM = "10:33";
const char wiesern_time_14[] PROGMEM = "10:43";
const char wiesern_time_15[] PROGMEM = "10:48";
const char wiesern_time_16[] PROGMEM = "10:53";
const char wiesern_time_17[] PROGMEM = "11:08";
const char wiesern_time_18[] PROGMEM = "11:13";
const char wiesern_time_19[] PROGMEM = "11:28";
const char wiesern_time_20[] PROGMEM = "11:33";
const char wiesern_time_21[] PROGMEM = "11:53";
const char wiesern_time_22[] PROGMEM = "12:08";
const char wiesern_time_23[] PROGMEM = "12:13";
const char wiesern_time_24[] PROGMEM = "12:33";
const char wiesern_time_25[] PROGMEM = "12:53";
const char wiesern_time_26[] PROGMEM = "13:13";
const char wiesern_time_27[] PROGMEM = "13:28";
const char wiesern_time_28[] PROGMEM = "13:33";
const char wiesern_time_29[] PROGMEM = "13:53";
const char wiesern_time_30[] PROGMEM = "14:03";
const char wiesern_time_31[] PROGMEM = "14:08";
const char wiesern_time_32[] PROGMEM = "14:13";
const char wiesern_time_33[] PROGMEM = "14:33";
const char wiesern_time_34[] PROGMEM = "14:38";
const char wiesern_time_35[] PROGMEM = "14:48";
const char wiesern_time_36[] PROGMEM = "14:53";
const char wiesern_time_37[] PROGMEM = "15:03";
const char wiesern_time_38[] PROGMEM = "15:13";
const char wiesern_time_39[] PROGMEM = "15:23";
const char wiesern_time_40[] PROGMEM = "15:28";
const char wiesern_time_41[] PROGMEM = "15:33";
const char wiesern_time_42[] PROGMEM = "15:43";
const char wiesern_time_43[] PROGMEM = "15:48";
const char wiesern_time_44[] PROGMEM = "15:53";
const char wiesern_time_45[] PROGMEM = "16:03";
const char wiesern_time_46[] PROGMEM = "16:13";
const char wiesern_time_47[] PROGMEM = "16:28";
const char wiesern_time_48[] PROGMEM = "16:33";
const char wiesern_time_49[] PROGMEM = "16:43";
const char wiesern_time_50[] PROGMEM = "16:53";
const char wiesern_time_51[] PROGMEM = "17:00";

const char* const wiesern_timetable[] PROGMEM = {
    wiesern_time_00, wiesern_time_01, wiesern_time_02, wiesern_time_03,
    wiesern_time_04, wiesern_time_05, wiesern_time_06, wiesern_time_07,
    wiesern_time_08, wiesern_time_09, wiesern_time_10, wiesern_time_11,
    wiesern_time_12, wiesern_time_13, wiesern_time_14, wiesern_time_15,
    wiesern_time_16, wiesern_time_17, wiesern_time_18, wiesern_time_19,
    wiesern_time_20, wiesern_time_21, wiesern_time_22, wiesern_time_23,
    wiesern_time_24, wiesern_time_25, wiesern_time_26, wiesern_time_27,
    wiesern_time_28, wiesern_time_29, wiesern_time_30, wiesern_time_31,
    wiesern_time_32, wiesern_time_33, wiesern_time_34, wiesern_time_35,
    wiesern_time_36, wiesern_time_37, wiesern_time_38, wiesern_time_39,
    wiesern_time_40, wiesern_time_41, wiesern_time_42, wiesern_time_43,
    wiesern_time_44, wiesern_time_45, wiesern_time_46, wiesern_time_47,
    wiesern_time_48, wiesern_time_49, wiesern_time_50, wiesern_time_51
};

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
    bool isScheduled;  // true if from timetable, false if from live API
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
    bool single_station_mode;  // true = 1 station, false = 2 stations
} config;

// ===================== TIMETABLE FUNCTIONS =====================
String readTimeFromProgmem(const char* const* table, int index) {
    char buffer[6];
    const char* timePtr = (const char*)pgm_read_ptr(&table[index]);
    strcpy_P(buffer, timePtr);
    return String(buffer);
}

String getNextBusFromTimetable(const char* stationId, int skip = 0) {
    time_t now = time(nullptr);
    if (now < 1700000000) return "";
    
    struct tm* t = localtime(&now);
    int currentMinutes = t->tm_hour * 60 + t->tm_min;
    
    const char* const* timetable = nullptr;
    int timetableSize = 0;
    
    char stationIdBuffer[12];
    strcpy_P(stationIdBuffer, ZWOELFERKOGEL_STATION_ID);
    
    if (strcmp(stationId, stationIdBuffer) == 0) {
        timetable = zwoelferkogel_timetable;
        timetableSize = ZWOELFERKOGEL_TIMETABLE_SIZE;
    } else {
        strcpy_P(stationIdBuffer, WIESERN_STATION_ID);
        if (strcmp(stationId, stationIdBuffer) == 0) {
            timetable = wiesern_timetable;
            timetableSize = WIESERN_TIMETABLE_SIZE;
        } else {
            return "";
        }
    }
    
    int foundCount = 0;
    for (int i = 0; i < timetableSize; i++) {
        String timeStr = readTimeFromProgmem(timetable, i);
        int hour = 0, minute = 0;
        if (sscanf(timeStr.c_str(), "%d:%d", &hour, &minute) == 2) {
            int busMinutes = hour * 60 + minute;
            if (busMinutes > currentMinutes) {
                if (foundCount == skip) return timeStr;
                foundCount++;
            }
        }
    }
    return "";
}

void applyTimetableFallback(BusData &busInfo, const char* stationId, int skip = 0) {
    Serial.print("Timetable fallback check - departureTime: ");
    Serial.print(busInfo.departureTime);
    Serial.print(", isScheduled: ");
    Serial.println(busInfo.isScheduled);
    
    if (busInfo.departureTime != "No buses" && busInfo.departureTime != "No match") {
        Serial.println("Skipping timetable - already has valid data");
        return;
    }
    
    String nextBus = getNextBusFromTimetable(stationId, skip);
    
    if (nextBus.length() > 0) {
        Serial.print("Applying timetable bus: ");
        Serial.println(nextBus);
        busInfo.departureTime = nextBus;
        busInfo.lineNumber = "Bus SKI";
        busInfo.isSki = true;
        busInfo.isDelayed = false;
        busInfo.delayMinutes = 0;
        busInfo.farAway = false;
        busInfo.destination = "";
        busInfo.isScheduled = true;
    } else {
        Serial.println("No timetable bus found for this time");
    }
}

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

// ===================== HELPER: pick background colour =====================
// Shared logic for both layouts.
// isScheduled (timetable fallback) -> black.
// Otherwise the exact same chain as the old working code.
uint32_t pickBgColor(const BusData &bus) {
    if (bus.isScheduled) {
        return TFT_BLACK;
    }
    if (bus.departureTime == "No match" || bus.departureTime == "No buses") {
        return PURPLE;
    }
    if (bus.isDelayed && bus.isSki) {
        return ORANGE;
    }
    if (bus.isDelayed) {
        return DELAY_RED;
    }
    if (bus.isSki) {
        return DARK_BLUE;
    }
    return YELLOW;
}

// ===================== PROCESS JSON =====================
void processDepartures(JsonArray departures, BusData &busInfo, const char* destinationFilter) {
    Serial.print("Processing departures - Array size: ");
    Serial.println(departures.size());
    
    if (departures.isNull() || departures.size() == 0) {
        Serial.println("No departures in array");
        busInfo.departureTime = "No buses";
        busInfo.lineNumber = "";
        busInfo.farAway = false;
        busInfo.isSki = false;
        busInfo.isScheduled = false;
        return;
    }

    JsonObject selectedBus;
    bool found = false;
    
    for (JsonObject bus : departures) {
        if (bus.isNull()) continue;
        String direction = bus["direction"] | "";
        if (destinationFilter[0] == '\0' || direction.indexOf(destinationFilter) >= 0) {
            selectedBus = bus;
            found = true;
            Serial.print("Found matching bus to: ");
            Serial.println(direction);
            break;
        }
    }
    
    if (!found) {
        Serial.println("No matching destination found");
        busInfo.departureTime = "No match";
        busInfo.lineNumber = "";
        busInfo.farAway = false;
        busInfo.isSki = false;
        busInfo.isScheduled = false;
        return;
    }

    const char* whenStr = selectedBus["when"] | "";
    if (strlen(whenStr) < 16) {
        Serial.println("Invalid time format in API response");
        busInfo.departureTime = "No buses";
        busInfo.lineNumber = "";
        busInfo.farAway = false;
        busInfo.isSki = false;
        busInfo.isScheduled = false;
        return;
    }
    
    busInfo.departureTime = String(whenStr).substring(11, 16);
    busInfo.destination = selectedBus["direction"] | "";
    
    if (selectedBus.containsKey("line") && selectedBus["line"].is<JsonObject>()) {
        busInfo.lineNumber = selectedBus["line"]["name"] | "";
    } else {
        busInfo.lineNumber = "";
    }
    
    busInfo.isSki = (busInfo.lineNumber == "Bus SKI");
    
    int delaySec = selectedBus["delay"] | 0;
    busInfo.delayMinutes = delaySec / 60;
    busInfo.isDelayed = delaySec > 60;

    int minsUntilDep = minutesUntil(whenStr);
    busInfo.farAway = minsUntilDep > 360;
    busInfo.isScheduled = false;  // Live API data
    
    Serial.print("Live bus found - Time: ");
    Serial.print(busInfo.departureTime);
    Serial.print(", Line: ");
    Serial.print(busInfo.lineNumber);
    Serial.print(", isSki: ");
    Serial.print(busInfo.isSki);
    Serial.print(", isDelayed: ");
    Serial.println(busInfo.isDelayed);
}

void processDeparturesMultiple(JsonArray departures, BusData &bus1, BusData &bus2, const char* destinationFilter) {
    if (departures.isNull() || departures.size() == 0) {
        bus1.departureTime = "No buses";
        bus1.lineNumber = "";
        bus1.farAway = false;
        bus1.isSki = false;
        bus1.isScheduled = false;
        bus2.departureTime = "No buses";
        bus2.lineNumber = "";
        bus2.farAway = false;
        bus2.isSki = false;
        bus2.isScheduled = false;
        return;
    }

    int foundCount = 0;
    
    for (JsonObject bus : departures) {
        if (bus.isNull()) continue;
        String direction = bus["direction"] | "";
        
        if (destinationFilter[0] == '\0' || direction.indexOf(destinationFilter) >= 0) {
            BusData* targetBus = (foundCount == 0) ? &bus1 : &bus2;
            
            const char* whenStr = bus["when"] | "";
            if (strlen(whenStr) < 16) continue;
            
            targetBus->departureTime = String(whenStr).substring(11, 16);
            targetBus->destination = bus["direction"] | "";
            
            if (bus.containsKey("line") && bus["line"].is<JsonObject>()) {
                targetBus->lineNumber = bus["line"]["name"] | "";
            } else {
                targetBus->lineNumber = "";
            }
            
            targetBus->isSki = (targetBus->lineNumber == "Bus SKI");
            
            int delaySec = bus["delay"] | 0;
            targetBus->delayMinutes = delaySec / 60;
            targetBus->isDelayed = delaySec > 60;

            int minsUntilDep = minutesUntil(whenStr);
            targetBus->farAway = minsUntilDep > 360;
            targetBus->isScheduled = false;  // Live API data
            
            foundCount++;
            if (foundCount >= 2) break;
        }
    }
    
    if (foundCount == 1) {
        bus2.departureTime = "No buses";
        bus2.lineNumber = "";
        bus2.farAway = false;
        bus2.isSki = false;
        bus2.isScheduled = false;
    }
    
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

// ===================== FETCH BUS (single) =====================
void fetchBus(const char* stopId, const char* destFilter, BusData &busInfo) {
    Serial.println("=== FETCH BUS START ===");
    Serial.print("Free heap before fetch: ");
    Serial.println(ESP.getFreeHeap());
    Serial.print("Stop ID: ");
    Serial.println(stopId);
    Serial.print("Destination filter: ");
    Serial.println(destFilter);
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected, skipping fetch");
        busInfo.departureTime = "No buses";
        busInfo.lineNumber = "";
        busInfo.farAway = false;
        busInfo.isSki = false;
        busInfo.isScheduled = false;
        return;
    }
    
    Serial.println("WiFi connected, proceeding with fetch");
    yield();
    
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    http.setTimeout(10000);
    client.setTimeout(10000);

    String url = "https://oebb.macistry.com/api/stops/" + String(stopId) + "/departures?bus=true";
    Serial.print("URL: ");
    Serial.println(url);
    
    if (!http.begin(client, url)) {
        Serial.println("HTTP begin failed");
        busInfo.departureTime = "No buses";
        busInfo.lineNumber = "";
        busInfo.farAway = false;
        busInfo.isSki = false;
        busInfo.isScheduled = false;
        return;
    }
    
    Serial.println("Starting HTTP GET...");
    yield();
    httpStatus = http.GET();
    Serial.print("HTTP Status: ");
    Serial.println(httpStatus);

    if (httpStatus == 200) {
        int payloadSize = http.getSize();
        Serial.print("Payload size: ");
        Serial.println(payloadSize);
        
        if (payloadSize > 16384) {
            Serial.println("Payload too large, skipping");
            http.end();
            busInfo.departureTime = "No buses";
            busInfo.lineNumber = "";
            busInfo.farAway = false;
            busInfo.isSki = false;
            busInfo.isScheduled = false;
            return;
        }
        
        yield();
        Serial.print("Free heap before parse: ");
        Serial.println(ESP.getFreeHeap());

        // Stream directly from the HTTP response – no intermediate String copy.
        // DynamicJsonDocument allocates on the heap where the memory actually is.
        DynamicJsonDocument doc(4096);
        DeserializationError error = deserializeJson(doc, client);
        
        Serial.print("Free heap after JSON parse: ");
        Serial.println(ESP.getFreeHeap());

        if (!error) {
            yield();
            if (doc.containsKey("departures") && doc["departures"].is<JsonArray>()) {
                JsonArray departures = doc["departures"].as<JsonArray>();
                processDepartures(departures, busInfo, destFilter);
            } else {
                Serial.println("Invalid JSON structure - missing or invalid departures array");
                busInfo.departureTime = "No buses";
                busInfo.lineNumber = "";
                busInfo.farAway = false;
                busInfo.isSki = false;
                busInfo.isScheduled = false;
            }
        } else {
            Serial.print("JSON parse error: ");
            Serial.println(error.c_str());
            busInfo.departureTime = "No buses";
            busInfo.lineNumber = "";
            busInfo.farAway = false;
            busInfo.isSki = false;
            busInfo.isScheduled = false;
        }
    } else {
        Serial.print("HTTP error: ");
        Serial.println(httpStatus);
        busInfo.departureTime = "No buses";
        busInfo.lineNumber = "";
        busInfo.farAway = false;
        busInfo.isSki = false;
        busInfo.isScheduled = false;
    }
    http.end();
    yield();
    
    Serial.print("Free heap after http.end: ");
    Serial.println(ESP.getFreeHeap());
    
    // Timetable fallback – only fires when no live data arrived
    applyTimetableFallback(busInfo, stopId);
    
    delay(100);
    yield();
}

// ===================== FETCH BUS (multiple from one station) =====================
void fetchBusMultiple(const char* stopId, const char* destFilter, BusData &bus1, BusData &bus2) {
    Serial.println("=== FETCH MULTIPLE BUSES START ===");
    Serial.print("Free heap before fetch: ");
    Serial.println(ESP.getFreeHeap());
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected, skipping fetch");
        bus1.departureTime = "No buses";
        bus1.lineNumber = "";
        bus1.farAway = false;
        bus1.isSki = false;
        bus1.isScheduled = false;
        bus2.departureTime = "No buses";
        bus2.lineNumber = "";
        bus2.farAway = false;
        bus2.isSki = false;
        bus2.isScheduled = false;
        return;
    }
    
    yield();
    
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    http.setTimeout(10000);
    client.setTimeout(10000);

    String url = "https://oebb.macistry.com/api/stops/" + String(stopId) + "/departures?bus=true";
    
    if (!http.begin(client, url)) {
        Serial.println("HTTP begin failed");
        bus1.departureTime = "No buses";
        bus1.lineNumber = "";
        bus1.farAway = false;
        bus1.isSki = false;
        bus1.isScheduled = false;
        bus2.departureTime = "No buses";
        bus2.lineNumber = "";
        bus2.farAway = false;
        bus2.isSki = false;
        bus2.isScheduled = false;
        return;
    }
    
    yield();
    httpStatus = http.GET();

    if (httpStatus == 200) {
        int payloadSize = http.getSize();
        Serial.print("Payload size: ");
        Serial.println(payloadSize);
        
        if (payloadSize > 16384) {
            Serial.println("Payload too large, skipping");
            http.end();
            bus1.departureTime = "No buses";
            bus1.lineNumber = "";
            bus1.farAway = false;
            bus1.isSki = false;
            bus1.isScheduled = false;
            bus2.departureTime = "No buses";
            bus2.lineNumber = "";
            bus2.farAway = false;
            bus2.isSki = false;
            bus2.isScheduled = false;
            return;
        }
        
        yield();
        Serial.print("Free heap before parse: ");
        Serial.println(ESP.getFreeHeap());

        // Stream directly – avoids holding payload String + doc buffer simultaneously.
        DynamicJsonDocument doc(4096);
        DeserializationError error = deserializeJson(doc, client);
        
        Serial.print("Free heap after JSON parse: ");
        Serial.println(ESP.getFreeHeap());

        if (!error) {
            yield();
            if (doc.containsKey("departures") && doc["departures"].is<JsonArray>()) {
                JsonArray departures = doc["departures"].as<JsonArray>();
                processDeparturesMultiple(departures, bus1, bus2, destFilter);
            } else {
                Serial.println("Invalid JSON structure - missing or invalid departures array");
                bus1.departureTime = "No buses";
                bus1.lineNumber = "";
                bus1.farAway = false;
                bus1.isSki = false;
                bus1.isScheduled = false;
                bus2.departureTime = "No buses";
                bus2.lineNumber = "";
                bus2.farAway = false;
                bus2.isSki = false;
                bus2.isScheduled = false;
            }
        } else {
            Serial.print("JSON parse error: ");
            Serial.println(error.c_str());
            bus1.departureTime = "No buses";
            bus1.lineNumber = "";
            bus1.farAway = false;
            bus1.isSki = false;
            bus1.isScheduled = false;
            bus2.departureTime = "No buses";
            bus2.lineNumber = "";
            bus2.farAway = false;
            bus2.isSki = false;
            bus2.isScheduled = false;
        }
    } else {
        Serial.print("HTTP error: ");
        Serial.println(httpStatus);
        bus1.departureTime = "No buses";
        bus1.lineNumber = "";
        bus1.farAway = false;
        bus1.isSki = false;
        bus1.isScheduled = false;
        bus2.departureTime = "No buses";
        bus2.lineNumber = "";
        bus2.farAway = false;
        bus2.isSki = false;
        bus2.isScheduled = false;
    }
    http.end();
    yield();
    
    Serial.print("Free heap after http.end: ");
    Serial.println(ESP.getFreeHeap());
    
    // Timetable fallback for both slots – only fires when no live data arrived
    applyTimetableFallback(bus1, stopId, 0);
    applyTimetableFallback(bus2, stopId, 1);
    
    delay(100);
    yield();
}

// ===================== DISPLAY =====================
void displayBus() {
    tft.fillScreen(TFT_BLACK);

    if (config.single_station_mode) {
        // ===== SINGLE STATION MODE – FULL SCREEN =====
        // Background driven by bus1 (most urgent).
        uint32_t mainBg = pickBgColor(busInfo1);

        tft.fillScreen(mainBg);
        tft.setTextColor(TFT_WHITE, mainBg);
        tft.setTextDatum(MC_DATUM);

        // --- First bus (large, upper area) ---
        if (busInfo1.farAway) {
            tft.drawString("Next > 6h", 120, 80, 4);
        } else if (busInfo1.departureTime == "No match" || busInfo1.departureTime == "No buses") {
            tft.drawString(busInfo1.departureTime, 120, 80, 4);
        } else {
            tft.setFreeFont(&FreeSansBold24pt7b);
            tft.drawString(busInfo1.departureTime, 120, 70);
            tft.setTextFont(2);
            String busType1 = busInfo1.isSki ? "Bus SKI" : busInfo1.lineNumber;
            tft.drawString(busType1, 120, 105, 2);
        }

        // --- Separator ---
        tft.drawLine(20, 140, 220, 140, TFT_WHITE);
        tft.drawLine(20, 141, 220, 141, TFT_WHITE);

        // --- Second bus (smaller, lower area) ---
        // If bus2 is live and has a different colour, repaint below separator.
        uint32_t bg2 = pickBgColor(busInfo2);
        if (bg2 != mainBg) {
            tft.fillRect(0, 142, 240, 98, bg2);
            tft.setTextColor(TFT_WHITE, bg2);
        }

        tft.setTextDatum(MC_DATUM);
        tft.setTextFont(2);
        tft.drawString("Next:", 120, 160, 2);

        if (busInfo2.farAway) {
            tft.drawString("> 6h", 120, 185, 4);
        } else if (busInfo2.departureTime == "No match" || busInfo2.departureTime == "No buses") {
            tft.drawString(busInfo2.departureTime, 120, 185, 2);
        } else {
            tft.setTextFont(4);
            tft.drawString(busInfo2.departureTime, 120, 185, 4);
            tft.setTextFont(2);
            String busType2 = busInfo2.isSki ? "Bus SKI" : busInfo2.lineNumber;
            tft.drawString(busType2, 120, 215, 2);
        }

    } else {
        // ===== TWO STATION MODE – SPLIT SCREEN =====

        // --- Upper half – Station 1 ---
        uint32_t bg1 = pickBgColor(busInfo1);

        tft.fillRect(0, 0, 240, 120, bg1);
        tft.setTextColor(TFT_WHITE, bg1);
        tft.setTextDatum(MC_DATUM);

        if (busInfo1.farAway) {
            tft.drawString("Next > 6h", 120, 68, 4);
        } else if (busInfo1.departureTime == "No match" || busInfo1.departureTime == "No buses") {
            tft.drawString(busInfo1.departureTime, 120, 68, 2);
        } else {
            tft.setFreeFont(&FreeSansBold24pt7b);
            tft.drawString(busInfo1.departureTime, 120, 60);
            tft.setTextFont(2);
            String busType1 = busInfo1.isSki ? "Bus SKI" : busInfo1.lineNumber;
            tft.drawString(busType1, 120, 98, 2);
        }

        // --- Lower half – Station 2 ---
        uint32_t bg2 = pickBgColor(busInfo2);

        tft.fillRect(0, 120, 240, 120, bg2);
        tft.setTextColor(TFT_WHITE, bg2);
        tft.setTextDatum(MC_DATUM);

        if (busInfo2.farAway) {
            tft.drawString("Next > 6h", 120, 188, 4);
        } else if (busInfo2.departureTime == "No match" || busInfo2.departureTime == "No buses") {
            tft.drawString(busInfo2.departureTime, 120, 188, 2);
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
const char html_part1[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Bus Config</title><style>
body{font-family:Arial;max-width:800px;margin:20px auto;padding:20px;background:#667eea}
.container{background:#fff;border-radius:12px;padding:30px;box-shadow:0 10px 30px rgba(0,0,0,0.2)}
h1{color:#667eea;text-align:center;margin-bottom:30px}
.section{background:#f8f9fa;padding:20px;margin:20px 0;border-radius:8px;border-left:4px solid #667eea}
.section h2{margin-top:0;color:#495057}
.form-group{margin:15px 0}
label{display:block;margin-bottom:5px;color:#495057;font-weight:500}
input[type="text"],input[type="time"],select{width:100%;padding:10px;border:2px solid #e9ecef;
border-radius:6px;font-size:14px;box-sizing:border-box}
input[type="checkbox"]{width:20px;height:20px;margin-right:10px}
input[type="range"]{width:100%;margin:10px 0}
button{background:#667eea;color:#fff;padding:12px 30px;border:none;border-radius:6px;
cursor:pointer;font-size:16px;width:100%;margin-top:20px}
.status-card{background:#e7f5ff;border:1px solid #74c0fc;padding:15px;border-radius:8px;margin-bottom:20px}
.status-row{display:flex;justify-content:space-between;margin:8px 0;padding:8px;
background:#fff;border-radius:4px}
.brightness-value{text-align:center;font-size:20px;color:#667eea;font-weight:bold}
.checkbox-wrapper{display:flex;align-items:center;margin:15px 0}
.mode-selector{display:flex;gap:10px;margin:15px 0}
.mode-option{flex:1;padding:15px;border:2px solid #e9ecef;border-radius:8px;cursor:pointer;text-align:center}
.link{display:block;text-align:center;margin-top:15px;color:#667eea;text-decoration:none;font-weight:600}
</style>
<script>
function updateBrightness(val){document.getElementById('briVal').innerText=val}
function toggleStation2(){var m=document.querySelector('input[name="stationmode"]:checked').value;
var s=document.getElementById('station2section');if(m==='1'){s.style.display='none'}else{s.style.display='block'}}
window.onload=function(){toggleStation2()}
</script></head><body><div class="container">
<h1>🚌 Bus Display Configuration</h1>
)rawliteral";

void handleRoot() {
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    
    server.sendContent_P(html_part1);
    
    // Status card – also shows whether data is live or from timetable
    String statusCard = "<div class=\"status-card\"><h2>Current Status</h2>";
    statusCard += "<div class=\"status-row\"><span>🚏 Station 1:</span><span>";
    statusCard += busInfo1.departureTime + " " + busInfo1.lineNumber;
    if (busInfo1.isScheduled) statusCard += " (tidtabell)";
    statusCard += "</span></div>";
    
    if (!config.single_station_mode) {
        statusCard += "<div class=\"status-row\"><span>🚏 Station 2:</span><span>";
        statusCard += busInfo2.departureTime + " " + busInfo2.lineNumber;
        if (busInfo2.isScheduled) statusCard += " (tidtabell)";
        statusCard += "</span></div>";
    } else {
        statusCard += "<div class=\"status-row\"><span>🚏 Nästa:</span><span>";
        statusCard += busInfo2.departureTime + " " + busInfo2.lineNumber;
        if (busInfo2.isScheduled) statusCard += " (tidtabell)";
        statusCard += "</span></div>";
    }
    statusCard += "</div>";
    server.sendContent(statusCard);
    
    server.sendContent("<form action=\"/save\" method=\"POST\">");
    
    // Mode selector
    String modeSection = "<div class=\"section\"><h2>Display Mode</h2><div class=\"mode-selector\">";
    modeSection += "<div class=\"mode-option\"><input type=\"radio\" id=\"mode1\" name=\"stationmode\" value=\"1\"";
    if (config.single_station_mode) modeSection += " checked";
    modeSection += " onclick=\"toggleStation2()\"><label for=\"mode1\">Single Station</label></div>";
    modeSection += "<div class=\"mode-option\"><input type=\"radio\" id=\"mode2\" name=\"stationmode\" value=\"2\"";
    if (!config.single_station_mode) modeSection += " checked";
    modeSection += " onclick=\"toggleStation2()\"><label for=\"mode2\">Two Stations</label></div>";
    modeSection += "</div></div>";
    server.sendContent(modeSection);
    
    // Station 1
    String station1 = "<div class=\"section\"><h2>🚏 Station 1</h2>";
    station1 += "<div class=\"form-group\"><label>Stop ID:</label>";
    station1 += "<input type=\"text\" name=\"stopid1\" value=\"" + String(config.stop_id_1) + "\" required>";
    station1 += "<a href=\"https://oebb.macistry.com/api/locations?query=Saalbach\" target=\"_blank\" style=\"display:block;margin-top:5px;color:#667eea;font-size:13px;\">Find station ID here</a></div>";
    station1 += "<div class=\"form-group\"><label>Destination Filter:</label>";
    station1 += "<input type=\"text\" name=\"destname1\" value=\"" + String(config.destination_name_1) + "\"></div></div>";
    server.sendContent(station1);
    
    // Station 2
    String station2 = "<div class=\"section\" id=\"station2section\"";
    if (config.single_station_mode) station2 += " style=\"display:none;\"";
    station2 += "><h2>🚏 Station 2</h2>";
    station2 += "<div class=\"form-group\"><label>Stop ID:</label>";
    station2 += "<input type=\"text\" name=\"stopid2\" value=\"" + String(config.stop_id_2) + "\" required>";
    station2 += "<a href=\"https://oebb.macistry.com/api/locations?query=Hinterglemm\" target=\"_blank\" style=\"display:block;margin-top:5px;color:#667eea;font-size:13px;\">Find station ID here</a></div>";
    station2 += "<div class=\"form-group\"><label>Destination Filter:</label>";
    station2 += "<input type=\"text\" name=\"destname2\" value=\"" + String(config.destination_name_2) + "\"></div></div>";
    server.sendContent(station2);
    
    // Night mode
    char timeStr[6];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", config.night_start_hour, config.night_start_minute);
    String nightStart = String(timeStr);
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", config.night_end_hour, config.night_end_minute);
    String nightEnd = String(timeStr);
    
    String nightMode = "<div class=\"section\"><h2>🌙 Night Mode</h2>";
    nightMode += "<div class=\"checkbox-wrapper\"><input type=\"checkbox\" name=\"night\" id=\"night\"";
    if (config.night_mode_enabled) nightMode += " checked";
    nightMode += "><label for=\"night\">Enable Night Mode</label></div>";
    nightMode += "<div class=\"form-group\"><label>Start:</label>";
    nightMode += "<input type=\"time\" name=\"nightstart\" value=\"" + nightStart + "\"></div>";
    nightMode += "<div class=\"form-group\"><label>End:</label>";
    nightMode += "<input type=\"time\" name=\"nightend\" value=\"" + nightEnd + "\"></div></div>";
    server.sendContent(nightMode);
    
    // Brightness
    String brightness = "<div class=\"section\"><h2>💡 Brightness</h2>";
    brightness += "<input type=\"range\" name=\"bri\" min=\"20\" max=\"255\" value=\"" + String(config.bri) + "\" oninput=\"updateBrightness(this.value)\">";
    brightness += "<div class=\"brightness-value\" id=\"briVal\">" + String(config.bri) + "</div></div>";
    server.sendContent(brightness);
    
    // Footer
    String footer = "<button type=\"submit\">💾 Save & Restart</button></form>";
    footer += "<a href=\"/update\" class=\"link\">🔧 OTA Update</a>";
    footer += "</div></body></html>";
    server.sendContent(footer);
    server.sendContent("");
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
        strlcpy(config.destination_name_1, "Hinterglemm", sizeof(config.destination_name_1));
        strlcpy(config.stop_id_2, "596060", sizeof(config.stop_id_2));
        strlcpy(config.destination_name_2, "Mitterlengau", sizeof(config.destination_name_2));
        config.bri = 200;
        config.duration_mode = 0;
        config.night_mode_enabled = false;
        config.night_start_hour = 23;
        config.night_start_minute = 0;
        config.night_end_hour = 6;
        config.night_end_minute = 0;
        config.single_station_mode = false;
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
        for (int j = 0; j <= (i % 4); j++) dots += ".";
        
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

    Serial.print("Free heap before first fetch: ");
    Serial.println(ESP.getFreeHeap());

    if (!isNightMode()) {
        if (config.single_station_mode) {
            fetchBusMultiple(config.stop_id_1, config.destination_name_1, busInfo1, busInfo2);
        } else {
            fetchBus(config.stop_id_1, config.destination_name_1, busInfo1);
            delay(500);
            yield();
            fetchBus(config.stop_id_2, config.destination_name_2, busInfo2);
        }
        displayBus();
    } else {
        displayNightMode();
    }
    
    Serial.print("Free heap after setup: ");
    Serial.println(ESP.getFreeHeap());
}

// ===================== LOOP =====================
void loop() {
    static unsigned long last = 0;
    static bool wasNightMode = false;

    server.handleClient();
    yield();
    ElegantOTA.loop();
    yield();

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
                delay(500);
                yield();
                fetchBus(config.stop_id_2, config.destination_name_2, busInfo2);
            }
            yield();
            displayBus();
        }
    }

    if (!nightNow && millis() - last > 120000) {
        last = millis();
        Serial.println("Starting periodic bus fetch...");
        Serial.print("Free heap before fetch: ");
        Serial.println(ESP.getFreeHeap());
        
        if (config.single_station_mode) {
            fetchBusMultiple(config.stop_id_1, config.destination_name_1, busInfo1, busInfo2);
        } else {
            fetchBus(config.stop_id_1, config.destination_name_1, busInfo1);
            delay(500);
            yield();
            fetchBus(config.stop_id_2, config.destination_name_2, busInfo2);
        }
        
        yield();
        displayBus();
        
        Serial.print("Free heap after fetch: ");
        Serial.println(ESP.getFreeHeap());
    }
    
    delay(10);
    yield();
}
