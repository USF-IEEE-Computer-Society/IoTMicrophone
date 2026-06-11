#include <driver/i2s.h>
#include <esp_now.h>
#include <WiFi.h>

// Amp I2S pins (output)
#define AMP_DOUT 25
#define AMP_BCLK 26
#define AMP_LRC  32
#define SAMPLE_RATE 16000

void setupAmp() {
  i2s_config_t cfg = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 128,
    .use_apll = false
  };
  i2s_pin_config_t pins = {
    .bck_io_num = AMP_BCLK,
    .ws_io_num = AMP_LRC,
    .data_out_num = AMP_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);   // NUM_1 to avoid conflicts
  i2s_set_pin(I2S_NUM_0, &pins);
  i2s_start(I2S_NUM_0);
}

volatile int packetCount = 0;

void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  packetCount++;
  size_t written = 0;
  i2s_write(I2S_NUM_0, data, len, &written, portMAX_DELAY);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (true) delay(100);
  }
  esp_now_register_recv_cb(onReceive);

  setupAmp();
  Serial.println("Receiver ready.");
}

void loop() {
  Serial.printf("Packets received: %d\n", packetCount);
  delay(1000);
}