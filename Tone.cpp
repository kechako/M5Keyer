#include "Tone.h"

Tone::Tone(int freq, int unitMs, float gain) : frequency(freq),
                                                unitMs(unitMs),
                                                dashBuffer(NULL),
                                                dashBufferLength(0),
                                                dotBuffer(NULL),
                                                dotBufferLength(0),
                                                gain(gain),
                                                endTime(0) {
  this->initBuffer();
}

void Tone::setup() {
    M5.Axp.SetSpkEnable(true);
    this->initI2SSpeakOrMic(MODE_SPK);
}

bool Tone::initI2SSpeakOrMic(int mode) {
  esp_err_t err = ESP_OK;

  i2s_driver_uninstall(SPEAKER_I2S_NUMBER);
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
      .communication_format = I2S_COMM_FORMAT_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 2,
      .dma_buf_len = 128,
  };
  if (mode == MODE_MIC) {
    i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM);
  } else {
    i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
    i2s_config.use_apll = false;
    i2s_config.tx_desc_auto_clear = true;
  }
  err += i2s_driver_install(SPEAKER_I2S_NUMBER, &i2s_config, 0, NULL);
  i2s_pin_config_t tx_pin_config;

  tx_pin_config.bck_io_num = CONFIG_I2S_BCK_PIN;
  tx_pin_config.ws_io_num = CONFIG_I2S_LRCK_PIN;
  tx_pin_config.data_out_num = CONFIG_I2S_DATA_PIN;
  tx_pin_config.data_in_num = CONFIG_I2S_DATA_IN_PIN;
  err += i2s_set_pin(Speak_I2S_NUMBER, &tx_pin_config);
  err += i2s_set_clk(Speak_I2S_NUMBER, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);

  return true;
}

void Tone::setUnit(int unitMs) {
    if (unitMs == this->unitMs) {
        return;
    }
    this->unitMs = unitMs;
    this->initBuffer();
}

void Tone::setGain(float gain) {
    if (gain == this->gain) {
        return;
    }
    this->gain = gain;
    this->initBuffer();
}

bool Tone::getPlaying() {
    return this->endTime > 0;
}

void Tone::update() {
    if (this->endTime == 0) {
        return;
    }

    if (millis() < this->endTime) {
        return;
    }

    this->endTime = 0;
}

void Tone::playDash() {
  if (this->endTime > 0) {
      return;
  }

  size_t bytes_written = 0;
  i2s_write(Speak_I2S_NUMBER, this->dashBuffer, this->dashBufferLength, &bytes_written, portMAX_DELAY);

  this->endTime = millis() + this->unitMs * 4;
}

void Tone::playDot() {
  if (this->endTime > 0) {
      return;
  }

  size_t bytes_written = 0;
  i2s_write(Speak_I2S_NUMBER, this->dotBuffer, this->dotBufferLength, &bytes_written, portMAX_DELAY);

  this->endTime = millis() + this->unitMs * 2;
}

void Tone::initBuffer() {
    this->initDashBuffer();
    this->initDotBuffer();
}

void Tone::initDashBuffer() {
    if (this->dashBuffer != NULL) {
        delete this->dashBuffer;
    }

    int fadeSamples = this->unitMs2Samples() / 4;

    int samples = this->unitMs2Samples() * 3;
    this->dashBufferLength = samples * 2;
    this->dashBuffer = new int16_t[samples];

    float freq = static_cast<float>(this->frequency);
    for (int i = 0; i < samples; i++) {
        float t = static_cast<float>(i) / static_cast<float>(SAMPLE_RATE);
        float amp = sinf(2.0f * M_PI * freq * t) * this->gain;
        if (i >= samples - fadeSamples - 1) {
            amp *= static_cast<float>(samples - i + 1) / static_cast<float>(fadeSamples);
        }
        this->dashBuffer[i] = this->float2Uint16(amp);
    }
}

void Tone::initDotBuffer() {
    if (this->dotBuffer != NULL) {
        delete this->dotBuffer;
    }

    int fadeSamples = this->unitMs2Samples() / 4;

    int samples = this->unitMs2Samples();
    this->dotBufferLength = samples * 2;
    this->dotBuffer = new int16_t[samples];

    float freq = static_cast<float>(this->frequency);
    for (int i = 0; i < samples; i++) {
        float t = static_cast<float>(i) / static_cast<float>(SAMPLE_RATE);
        float amp = sinf(2.0f * M_PI * freq * t) * this->gain;
        if (i >= samples - fadeSamples - 1) {
            amp *= static_cast<float>(samples - i + 1) / static_cast<float>(fadeSamples);
        }
        this->dotBuffer[i] = this->float2Uint16(amp);
    }
}

int Tone::unitMs2Samples() {
    return this->unitMs * SAMPLE_RATE / 1000;
}

int16_t Tone::float2Uint16(float f) {
    int32_t n = static_cast<int32_t>(f * static_cast<float>(MAX_INT16));
    if (n > MAX_INT16) {
        return MAX_INT16;
    } else if (n < MIN_INT16) {
        return MIN_INT16;
    }

    return static_cast<int16_t>(n);
}
