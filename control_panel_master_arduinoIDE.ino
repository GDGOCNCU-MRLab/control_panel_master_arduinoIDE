#include <WiFi.h>
#include <esp_now.h>

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Replace it with your master ESP32's MAC Address.

typedef struct struct_message{
  char a[32];
} message;

message myData;

esp_now_peer_info_t peerInfo;

// Call message for checking if the data is sent succesfully or not.
void OnDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Succeed" : "Failed");

}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK){
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // Pairing with slaves.
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Peer is not add successfully.");
    return;
  }

}

void loop() {
  // put your main code here, to run repeatedly:

  strcpy(myData.a, "Test message.");

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Message sent successfully.");
  } 
  else{
    Serial.println("Message sent failed");
  }
  delay(2000);
}
