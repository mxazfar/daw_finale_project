#include <windows.h>
#include <mmsystem.h>
#include <atomic>
#include <iostream>
#include <cmath>

#include "audio_processing.hpp"

#define RAW_INTENSITY_MAX 32767
#define MAX_NOTES_PER_THREAD 10

#pragma comment(lib, "winmm.lib")

const int SAMPLE_RATE = 44100;
const int BITS_PER_SAMPLE = 16;
const int CHANNELS = 2;

AudioThread::AudioThread(bool startThread) : g_start(false) {
    if(startThread) {
        this->startThread();
    } else {
        this->stopThread();
    }
}

void AudioThread::startAudioPlayback(void* params)
{
    HANDLE hThread = CreateThread(
        NULL,                // default security attributes
        0,                   // use default stack size
        playAudioThread,     // thread function name
        params,              // argument to thread function
        0,                   // use default creation flags
        NULL);               // returns the thread identifier

    if (hThread == NULL)
    {
        std::cerr << "Failed to create thread" << std::endl;
    } 
    else
    {
        CloseHandle(hThread); // We're not waiting for the thread, so we can close the handle immediately
    }
}

void AudioThread::startThread() {
    g_start.store(true);
}

void AudioThread::stopThread() {
     printf("Entering stopThread()\n");
    g_start.store(false);
    printf("Thread stop flag set to false\n");
}

bool AudioThread::isThreadRunning() {
    bool running = g_start.load();
    printf("Thread running status: %s\n", running ? "true" : "false");
    return running;
}

double getRawIntensity(double intensity)
{
    return (intensity / 100) * RAW_INTENSITY_MAX;
}

// Expects a pointer to the current audio thread class along with a wavecommand.
DWORD WINAPI AudioThread::playAudioThread(LPVOID lpParam) 
{
    printf("Entered playAudioThread\n");
    
    try {
        void** typedLpParam = static_cast<void**>(lpParam);

        AudioThread *currentThread = (AudioThread*)typedLpParam[THREAD_INDEX];
        waveCommand_t *waveCommand = static_cast<waveCommand_t *>(typedLpParam[WAVE_COMMAND_INDEX]);

        int commandID = waveCommand->commandId;

        printf("Audio thread starting main loop\n");
        while(currentThread->isThreadRunning()) {
            printf("In audio thread loop\n");
            if (commandID == SINE_WAVE_CMD_ID)
            {
                double frequency = waveCommand->params[WAVE_CMD_FREQ_INDEX];
                double intensity = waveCommand->params[WAVE_CMD_POW_INDEX];
                double durationMs = waveCommand->params[WAVE_CMD_DURATION_INDEX];
                printf("Playing sine wave: freq=%f, intensity=%f, duration=%f\n", frequency, intensity, durationMs);
                if (playSineWave(frequency, intensity, durationMs, currentThread) != 0) {
                    printf("Error playing sine wave\n");
                    break;
                }
                printf("Finished playing sine wave\n");
            }
        }
        printf("Exited audio thread main loop\n");
    }
    catch (const std::exception& e) {
        printf("Caught exception in audio thread: %s\n", e.what());
    }
    catch (...) {
        printf("Caught unknown exception in audio thread\n");
    }
    printf("Exiting playAudioThread\n");
    return 0;
}

int playSineWave(double frequency, double intensity, double durationMs, AudioThread* currentThread)
{
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
    if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR)
    {
        std::cerr << "Failed to open wave out device" << std::endl;
        return 1;
    }

    int bufferSize;

    if(durationMs == PLAY_CONTINUOUS) {
        bufferSize = SAMPLE_RATE * 50 / 1000 * wfx.nBlockAlign;
    } else {
        bufferSize = SAMPLE_RATE * durationMs / 1000 * wfx.nBlockAlign;
    }

    short *buffer = new short[bufferSize / sizeof(short)];

    for (int i = 0; i < bufferSize / sizeof(short); i++)
    {
        double t = static_cast<double>(i) / SAMPLE_RATE;
        buffer[i] = static_cast<short>(getRawIntensity(intensity) * sin(2 * 3.141592654 * frequency * t));
    }

    // Prepare the WAVEHDR structure
    WAVEHDR header = {};
    header.lpData = reinterpret_cast<LPSTR>(buffer);
    header.dwBufferLength = bufferSize;

    // Prepare the header
    if (waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
    {
        std::cerr << "Failed to prepare wave out header" << std::endl;
        waveOutClose(hWaveOut);
        delete[] buffer;
        return 1;
    }

    if (durationMs > 0)
    {
        // Play the sound
        if (waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
        {
            std::cerr << "Failed to write wave out" << std::endl;
            waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
            waveOutClose(hWaveOut);
            delete[] buffer;
            return 1;
        }

        // Wait for the sound to finish playing
        while (!(header.dwFlags & WHDR_DONE))
        {
            Sleep(100);
        }
    }
    else
    {
        printf("Starting continuous playback\n");
        while (currentThread->isThreadRunning())
        {
            MMRESULT result = waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
            if (result != MMSYSERR_NOERROR)
            {
                printf("waveOutWrite failed with error code: %d\n", result);
                break;
            }
            Sleep(10);  // Add a small delay to prevent tight looping
        }
        printf("Continuous playback loop ended\n");
    }

    printf("Entering sinewave cleanup\n");

    // Clean up
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
    delete[] buffer;

    printf("Leaving sinwave cleanup\n");

    return 0;
}