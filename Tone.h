#ifndef _TONE_H_
#define _TONE_H_

#include <M5Core2.h>
#include <Math.h>
#include <driver/i2s.h>

#define CONFIG_I2S_BCK_PIN 12
#define CONFIG_I2S_LRCK_PIN 0
#define CONFIG_I2S_DATA_PIN 2
#define CONFIG_I2S_DATA_IN_PIN 34

#define SPEAKER_I2S_NUMBER I2S_NUM_0
#define SAMPLE_RATE 44100

#define MODE_MIC 0
#define MODE_SPK 1

#define MAX_INT16 32767
#define MIN_INT16 -32768

class Tone {
  public:
    Tone(int freq, int unitMs, float gain = 0.8);
    void setup();
    void setUnit(int unitMs);
    void setGain(float gain);
    bool getPlaying();
    void update();
    void playDash();
    void playDot();

  private:
    int frequency;
    int unitMs;
    int16_t *dashBuffer;
    int dashBufferLength;
    int16_t *dotBuffer;
    int dotBufferLength;
    float gain;
    long endTime;

    bool initI2SSpeakOrMic(int mode);
    void initBuffer();
    void initDashBuffer();
    void initDotBuffer();
    int unitMs2Samples();
    int16_t float2Uint16(float f);
};

#endif _TONE_H_
