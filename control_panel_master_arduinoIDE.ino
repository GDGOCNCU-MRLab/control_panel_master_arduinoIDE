#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// 填入你的車輛端 MAC 地址
uint8_t receiverMAC[] = {0x18, 0x8b, 0x0e, 0x92, 0x4f, 0x98};

#define BUTTON_PIN 13 

typedef struct struct_message {
  bool led;
} struct_message;

struct_message sendData;
bool ledState = false;      // 紀錄目前的燈號狀態
bool lastButtonState = HIGH; // 紀錄上一次按鈕的狀態

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // 使用內建上拉電阻

  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE); // 固定頻道 1

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW 初始化失敗");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 1; 
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("新增配對設備失敗");
    return;
  }
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);

  // 關鍵邏輯：偵測「按下」的那一瞬間 (從 HIGH 變成 LOW)
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    delay(50); // 硬體防彈跳：等待訊號穩定
    if (digitalRead(BUTTON_PIN) == LOW) { // 再次確認真的有按住
      
      // 翻轉燈號狀態
      ledState = !ledState; 
      sendData.led = ledState;

      Serial.print("切換狀態至: ");
      Serial.println(ledState ? "開" : "關");

      // 送出指令
      esp_now_send(receiverMAC, (uint8_t *)&sendData, sizeof(sendData));
    }
  }

  // 儲存這一次的狀態，給下一次 loop 比較用
  lastButtonState = currentButtonState;
}
