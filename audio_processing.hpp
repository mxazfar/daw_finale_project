#ifndef AUDIO_PROCESSING_HPP
#define AUDIO_PROCESSING_HPP

#include <atomic>

#define SINE_WAVE_CMD_ID 1

#define WAVE_CMD_FREQ_INDEX 0
#define WAVE_CMD_POW_INDEX 1
#define WAVE_CMD_DURATION_INDEX 2

#define THREAD_INDEX 0
#define WAVE_COMMAND_INDEX 1

#define PLAY_CONTINUOUS -1

struct waveCommand_t {
    int commandId;
    int* params;
};

class AudioThread {
public:
    AudioThread(bool startThread);
    void startAudioPlayback(void* params);

    void startThread();
    void stopThread();
    bool isThreadRunning();

private:
    std::atomic<bool> g_start;
    static DWORD WINAPI playAudioThread(LPVOID lpParam);
};

int playSineWave(double frequency, double intensity, double durationMs, AudioThread* currentThread);

#endif 