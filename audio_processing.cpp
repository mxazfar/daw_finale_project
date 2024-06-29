#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <cmath>

#include "audio_processing.hpp"

#define RAW_INTENSITY_MAX 32767

#pragma comment(lib, "winmm.lib")

const int SAMPLE_RATE = 44100;
const int BITS_PER_SAMPLE = 16;
const int CHANNELS = 1;
const int DURATION_MS = 1000;   // 1 second

double getRawIntensity(double intensity) {
    return (intensity / 100) * RAW_INTENSITY_MAX;
}

int playSineWave(double frequency, double intensity) {
    // Prepare the WAVEFORMATEX structure
    WAVEFORMATEX wfx = {};
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = CHANNELS;
    wfx.nSamplesPerSec = SAMPLE_RATE;
    wfx.wBitsPerSample = BITS_PER_SAMPLE;
    wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    // Open the wave out device
    HWAVEOUT hWaveOut;
    if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open wave out device" << std::endl;
        return 1;
    }

    // Prepare the audio data
    int bufferSize = SAMPLE_RATE * DURATION_MS / 1000 * wfx.nBlockAlign;
    short* buffer = new short[bufferSize / sizeof(short)];

    for (int i = 0; i < bufferSize / sizeof(short); i++) {
        double t = static_cast<double>(i) / SAMPLE_RATE;
        buffer[i] = static_cast<short>(getRawIntensity(intensity) * sin(2 * 3.141592654 * frequency * t));
    }

    // Prepare the WAVEHDR structure
    WAVEHDR header = {};
    header.lpData = reinterpret_cast<LPSTR>(buffer);
    header.dwBufferLength = bufferSize;

    // Prepare the header
    if (waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
        std::cerr << "Failed to prepare wave out header" << std::endl;
        waveOutClose(hWaveOut);
        delete[] buffer;
        return 1;
    }

    // Play the sound
    if (waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
        std::cerr << "Failed to write wave out" << std::endl;
        waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
        waveOutClose(hWaveOut);
        delete[] buffer;
        return 1;
    }

    // Wait for the sound to finish playing
    while (!(header.dwFlags & WHDR_DONE)) {
        Sleep(100);
    }

    // Clean up
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
    delete[] buffer;

    return 0;
}