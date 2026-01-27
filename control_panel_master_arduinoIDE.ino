#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// 接收端 MAC 地址
uint8_t receiverMAC[] = {0x18, 0x8b, 0x0e, 0x92, 0x4f, 0x98};

#define BUTTON_PIN 13 
#define MOTOR_SPEED_CTRL 34 // 改用 GPIO 34，燒錄更穩定

typedef struct struct_message {
  bool led;
  int motor;
} struct_message;

struct_message sendData;
bool ledState = false;
bool lastButtonState = HIGH;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

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
  // 按鈕邏輯
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      ledState = !ledState;
      Serial.print("切換狀態: ");
      Serial.println(ledState ? "開" : "關");
    }
  }

  // 讀取速度
  int motorSpeed = analogRead(MOTOR_SPEED_CTRL);
  sendData.led = ledState;
  sendData.motor = motorSpeed;

  // 發送
  esp_now_send(receiverMAC, (uint8_t *)&sendData, sizeof(sendData));
  
  // 顯示在 Serial 讓我們確認發送端有讀到數值
  Serial.print("目前發送速度: ");
  Serial.println(motorSpeed);

  lastButtonState = currentButtonState;
  delay(50); // 稍微放慢速度
}
