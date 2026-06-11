#include <driver/i2s.h>
#include <esp_now.h>
#include <WiFi.h>

// ---- SET THIS to your RECEIVER's MAC ----
uint8_t receiverMAC[] = {0x20, 0x9B, 0xA9, 0x61, 0xEC, 0xD4};


//20:9B:A9:61:EC:D4

// Mic I2S pins (input)
#define MIC_WS   25
#define MIC_SCK  26
#define MIC_SD   22
#define SAMPLE_RATE 16000
#define SAMPLES_PER_PACKET 125   // 125 * 2 bytes = 250 bytes (ESP-NOW max)

void setupMic() {
  i2s_config_t cfg = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 128,
    .use_apll = false
  };
  i2s_pin_config_t pins = {
    .bck_io_num = MIC_SCK,
    .ws_io_num = MIC_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = MIC_SD
  };
  i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pins);
  i2s_start(I2S_NUM_0);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (true) delay(100);
  }

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, receiverMAC, 6);
  peer.channel = 0;
  peer.encrypt = false;
  if (esp_now_add_peer(&peer) != ESP_OK) {
    Serial.println("Failed to add peer");
    while (true) delay(100);
  }

  setupMic();
  Serial.println("Sender ready.");
}

void loop() {
  int32_t raw[SAMPLES_PER_PACKET];
  size_t bytesRead = 0;
  i2s_read(I2S_NUM_0, raw, sizeof(raw), &bytesRead, portMAX_DELAY);
  int n = bytesRead / sizeof(int32_t);

  // Convert 32-bit mic samples to 16-bit for transmission
  int16_t out[SAMPLES_PER_PACKET];
  for (int i = 0; i < n; i++) {
    out[i] = (int16_t)(raw[i] >> 14);  // shift to usable 16-bit range
  }

  esp_now_send(receiverMAC, (uint8_t*)out, n * sizeof(int16_t));
}