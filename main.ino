#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== 腳位與變數宣告 =====
const int BUTTON_PIN = 16;  
const int BUZZER_PIN = 15;
bool isMedicationTaken = true; 
bool isAlerting = false;       
unsigned long lastCheckTime = 0;
const unsigned long REMINDER_INTERVAL = 60000; // 60秒檢查一次

unsigned long alertStartTime = 0; // 記錄提醒響起的起點

Adafruit_SSD1306 display(128, 64, &Wire, -1);

// ===== 網路與 API 設定 =====
const char* WIFI_SSID = "peggy";
const char* WIFI_PASS = "asdfghjkl";
const char* LINE_TOKEN = "yj+em1c6XKsXudkLDxJUNcDXDvyLh/BIQku27CoozWAJgKbXF+Vo2gWOJWtmFnZ9ZDkECh3PNV/b7ICPw1fG8pYpWSTDmSY1WCrBjoBVOzXPTPVhp6ZLHPz+hfm99PnT9r1f2lTeaEH44SkJ1iFaIAdB04t89/1O/w1cDnyilFU=";

// ！！！請在這裡貼上您剛剛重新部署複製的完整網址！！！
const char* GAS_URL = "https://docs.google.com/spreadsheets/d/e/2PACX-1vRZHDnKUzrl5WVoW0d6Ui6d6SfuQ3NrXFR94qLZzDvKHGHd2tkVKWDY5P4JSNVnwSBNW8FSPJ2MUQtN/pub?gid=0&single=true&output=csv"; 
const String MED_NAME = "維他命C"; // 完整藥名

void sendLineMessage(String text);
void sendToGoogleSheets(String status, unsigned long duration, String medName);
void displayStatus(String title, String msg);

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void loop() {
  // 1. 偵測按鈕按下
  if (digitalRead(BUTTON_PIN) == LOW && isAlerting) {
    isMedicationTaken = true;
    isAlerting = false;
    digitalWrite(BUZZER_PIN, LOW); 
    
    displayStatus("TAKEN", "Great Job!");
    sendLineMessage("您已完成服藥，紀錄已更新。");
    
    // 計算從開始響到按下按鈕，隔了幾秒
    unsigned long durationSec = (millis() - alertStartTime) / 1000;
    
    // 發送至 Google Sheets
    sendToGoogleSheets("已吃藥", durationSec, MED_NAME);
    
    delay(500); 
  }

  // 2. 定時提醒邏輯
  if (millis() - lastCheckTime > REMINDER_INTERVAL) {
    if (isMedicationTaken) {
      isMedicationTaken = false;
      isAlerting = true;
      alertStartTime = millis(); 
    }
    lastCheckTime = millis();
  }

  // 3. 若處於提醒狀態
  if (isAlerting) {
    digitalWrite(BUZZER_PIN, HIGH); 
    displayStatus("REMINDER", "Please Take Med!");
    
    static unsigned long lastNotify = 0;
    if(millis() - lastNotify > 30000) { 
        sendLineMessage("提醒：請記得吃藥！");
        lastNotify = millis();
    }
  }
}

void displayStatus(String title, String msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(title);
  display.setCursor(0, 20);
  display.println(msg);
  display.display();
}

void sendLineMessage(String text) {
  if (WiFi.status() != WL_CONNECTED) return;
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  if (http.begin(client, "https://api.line.me/v2/bot/message/broadcast")) {
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", String("Bearer ") + LINE_TOKEN);
    String body = "{\"messages\":[{\"type\":\"text\",\"text\":\"" + text + "\"}]}";
    http.POST(body);
    http.end();
  }
}

// ===== 改用 GET 網址直接傳值，最不容易漏資料 =====
void sendToGoogleSheets(String status, unsigned long duration, String medName) {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure(); 
  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  // 把資料直接用 ? 與 & 接在網址後面
  String fullURL = String(GAS_URL) + "?status=" + status + "&duration=" + String(duration) + "&medName=" + medName;
  
  // 由於網址內有中文（維他命C、已吃藥），需要進行網路 URL 編碼轉換
  fullURL.replace(" ", "%20");
  
  Serial.println("正在傳送至 Google Sheets...");
  if (http.begin(client, fullURL)) {
    int httpResponseCode = http.GET(); // 改用 GET 請求
    Serial.print("回應代碼: ");
    Serial.println(httpResponseCode);
    http.end();
  } else {
    Serial.println("網址連接失敗");
  }
}

