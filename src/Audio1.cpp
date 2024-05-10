#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include <iostream>

#include "kissfft/kiss_fft.c"
#include "kissfft/kiss_fft.h"
#include "kissfft/kiss_fftr.c"
#include "kissfft/kiss_fftr.h"
#include "raygui.h"

using namespace std;

// Time-domain representation of Chebyshev window
// From: http://practicalcryptography.com/miscellaneous/machine-learning/implementing-dolph-chebyshev-window/

/**************************************************************************
This function computes the chebyshev polyomial T_n(x)
***************************************************************************/
double cheby_poly(int n, double x) {
  double res;
  if (fabs(x) <= 1)
    res = cos(n * acos(x));
  else
    res = cosh(n * acosh(x));
  return res;
}

/***************************************************************************
 calculate a chebyshev window of size N, store coeffs in out as in Antoniou
-out should be array of size N
-atten is the required sidelobe attenuation (e.g. if you want -60dB atten, use '60')
***************************************************************************/
void cheby_win(float *out, int N, float atten) {
  int nn, i;
  double M, n, sum = 0, max = 0;
  double tg = pow(10, atten / 20); /* 1/r term [2], 10^gamma [2] */
  double x0 = cosh((1.0 / (N - 1)) * acosh(tg));
  M = (N - 1) / 2;
  if (N % 2 == 0) M = M + 0.5; /* handle even length windows */
  for (nn = 0; nn < (N / 2 + 1); nn++) {
    n = nn - M;
    sum = 0;
    for (i = 1; i <= M; i++) {
      sum += cheby_poly(N - 1, x0 * cos(PI * i / N)) * cos(2.0 * n * PI * i / N);
    }
    out[nn] = tg + 2 * sum;
    out[N - nn - 1] = out[nn];
    if (out[nn] > max) max = out[nn];
  }
  for (nn = 0; nn < N; nn++) out[nn] /= max; /* normalise everything */
  return;
}

struct peakbar {
  int value;
  int timestamp;
};

int main() {
  InitWindow(1600, 800, "Title");
  SetWindowPosition(600, 50);

  Camera2D camera = {0};
  camera.target = {0, 0};
  camera.offset = {0, 0};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  SetTargetFPS(60);

  // Change "YourWaveFile.wav" to the filename of the wave file you wish to analyze
  Wave mywave = LoadWave("test-audio.wav");
  WaveFormat(&mywave, 44100, 16, 1);
  cout << "Sample channels=" << mywave.channels << endl;
  cout << "Sample Size=" << mywave.sampleSize << endl;
  cout << "Sizeof short=" << sizeof(short) << endl;

  float frequency = 440.0;  // Frequency for synthesizing a test wave
  float samplingrate = 44100.0;
  int SPU = 4096;  // Samples Per Update
  // int rawsize=samplingrate*120; // X seconds at samplingrate
  int rawsize = mywave.channels;
  const int FFTsamples = 1600;
  float Hz_per_band = samplingrate / FFTsamples;
  cout << (FFTsamples / 2) + 1 << " bands at Hz Per Band=" << Hz_per_band << endl;

  int readpos = 0;  // reader

  short *raw = new short[rawsize];
  short *buff = new short[SPU];
  float *fbuff = new float[FFTsamples];
  float *windowfunction = new float[SPU];

  cheby_win(windowfunction, FFTsamples, 240);  // Doplh-Chebyshev window, last variable is sidelobe attentuation in dB

  // fill raw with sound data for synthetic test wave.  Counter allows for frequency sweep functions
  float counter = 0.001;
  for (int i = 0; i < rawsize; i++) {
    raw[i] = (short)16000 * sin((float)i * 2 * PI * frequency / samplingrate);  // 2*PI*frequency/sample rate
  }

  // Overwrite synth wave with loaded wave data
  memcpy(raw, mywave.data, rawsize * 2);  // *2 because we want sampleSize/8

  // fill buff with first part of raw
  for (int i = 0; i < SPU; i++) {
    buff[i] = raw[i];
  }
  readpos += SPU;

  // fill FFT samples
  for (int i = 0; i < FFTsamples; i++) {
    fbuff[i] = (float)buff[i] * windowfunction[i];
  }

  InitAudioDevice();
  if (IsAudioDeviceReady()) {
    cout << "Audio ready" << endl;
  } else {
    cout << "Audio not initialized" << endl;
  }

  AudioStream myaudio = LoadAudioStream((unsigned)samplingrate, 16, 1);
  UpdateAudioStream(myaudio, buff, SPU);
  PlayAudioStream(myaudio);

  // set up FFT
  kiss_fftr_cfg cfg = kiss_fftr_alloc(FFTsamples, 0, NULL, NULL);

  kiss_fft_cpx freqs[FFTsamples / 2 + 1];
  int smoothbins[FFTsamples / 2 + 1] = {0};

  cout << "Hz per band=" << Hz_per_band << endl;

  kiss_fftr(cfg, fbuff, freqs);

  // Provide peak bars
  int framecounter = 0;
  int TTL = 30;
  peakbar zero = {0, 120};
  peakbar mbars[256] = {zero};
  peakbar dots[FFTsamples / 2] = {zero};

  while (!WindowShouldClose()) {
    framecounter++;

    if (IsAudioStreamProcessed(myaudio)) {
      if (readpos + SPU > rawsize) {  // we have to loop around on raw
        int tail = rawsize - readpos;
        // tail = tail < 0 ?0:tail;
        int j = 0;
        for (int i = readpos; i < rawsize; i++) {
          buff[j] = raw[i];
          j++;
        }
        int firstpart = SPU - tail;
        readpos = 0;
        for (int i = readpos; i < min(firstpart,0); i++) {
          buff[j] = raw[i];
          j++;
        }
        readpos += firstpart;
      } else if (readpos + SPU <= rawsize) {
        // cout<<"regular. Readpos in="<<readpos;
        int j = 0;
        for (int i = readpos; i < readpos + SPU; i++) {
          buff[j] = raw[i];
          j++;
        }
        readpos += SPU;
      }

      // create float buffer for FFT from updated buffer
      for (int i = 0; i < FFTsamples; i++) {
        fbuff[i] = (float)buff[i] * windowfunction[i];
      }

      UpdateAudioStream(myaudio, buff, SPU);
    }

    // Draw

    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode2D(camera);

    int mbins[256] = {0};  // resets bins to zero

    kiss_fftr(cfg, fbuff, freqs);  // performs the FFT

    // Draw full FFT and dots
    int barwidth = 1600 / (FFTsamples / 2) - 1;
    if (barwidth < 1) barwidth = 1;
    for (int i = 1; i < FFTsamples / 2; i++) {
      float height = 10 * log((freqs[i].r * freqs[i].r) / FFTsamples);  // 10* not 20* because no sqrt of magnitude calc
      height = (height < 0) ? 0 : height;                               // Clamp negative values to zero
      DrawRectangle(i * (barwidth + 1), 850 - height, barwidth, height, ORANGE);
      smoothbins[i] = height;
      if ((dots[i].value < smoothbins[i]) || (framecounter - dots[i].timestamp > TTL)) {
        dots[i].value = smoothbins[i];
        dots[i].timestamp = framecounter;
      }
      DrawLine(i * (barwidth + 1), 850 - dots[i].value, i * (barwidth + 1) + barwidth, 850 - dots[i].value, WHITE);
    }

    // Draw consolidated FFT: gather smooth FFT into logarithmically spaced bins.  TODO: Remove blank bins
    int rightbin = 1, counter = 1, oldbin = 1;
    for (int i = 1; i < FFTsamples / 2 + 1; i++) {
      // map frequency of FFT bin to correct consolidated bin, inverse of f(x)=440*2^(i-48)/12 to
      // get note frequencies on scale starting 4 octaves down from A=440Hz
      rightbin = trunc(12.0 * (log(i * Hz_per_band) - log(440.0)) / log(2.0) + 48);
      mbins[rightbin] += smoothbins[i];
      if (rightbin == oldbin) {
        counter++;
        oldbin = rightbin;
      } else {
        // If we have moved to a new bin, divide the previous one by counter to average it 
        mbins[rightbin - 1] /= counter;
        counter = 1;
        oldbin = rightbin;
      }
    }
    mbins[oldbin] /= counter;  // divide the last bin too

    // Draw consolidated bins and bars
    for (int i = 0; i < 128; i++) {
      DrawRectangle(i * 11, 600 - mbins[i], 10, mbins[i], BLUE);
      if ((mbars[i].value < mbins[i]) || (framecounter - mbars[i].timestamp > TTL)) {
        mbars[i].value = mbins[i];
        mbars[i].timestamp = framecounter;
      }
      DrawLine(i * 11, 600 - mbars[i].value, i * 11 + 10, 600 - mbars[i].value, WHITE);
    }

    // Plot wave data and read line.
    float xratio = SPU / 1600;  // Change SPU to rawsize/X to plot X part of whole wave...
    for (int i = 0; i < SPU; i++) {
      DrawPixel(i / xratio, 250 + 200 * buff[i] / 32000, RED);
    }
    DrawLine(readpos / xratio, 0, readpos / xratio, 800, GREEN);

    EndMode2D();
    DrawFPS(10, 10);
    EndDrawing();
  }

  kiss_fft_free(cfg);
  delete[] raw;
  delete[] buff;
  delete[] fbuff;
  delete[] windowfunction;
  UnloadWave(mywave);
  CloseAudioDevice();
  return 0;
}
