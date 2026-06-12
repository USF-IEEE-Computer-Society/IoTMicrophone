#include <driver/i2s.h>
#include <math.h>

#define I2S_DOUT 25
#define I2S_BCLK 26
#define I2S_LRC  32
#define SAMPLE_RATE 16000

void setupAmp() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_LRC,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_start(I2S_NUM_0);
}

void setup() {
  Serial.begin(115200);
  setupAmp();
}

void loop() {
  // Generate a 440 Hz sine wave (musical note A)
  const int samples_per_cycle = SAMPLE_RATE / 440;
  int16_t buffer[samples_per_cycle];
  for (int i = 0; i < samples_per_cycle; i++) {
    buffer[i] = (int16_t)(sin(2.0 * PI * i / samples_per_cycle) * 10000);
  }
  
  size_t bytes_written;
  i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &bytes_written, portMAX_DELAY);
}