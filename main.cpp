#ifndef UNICODE
#define UNICODE
#endif 

#define _WIN32_WINNT 0x0600 

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <iostream>
#include <Windows.h>
#include <WinUser.h>
#include <commctrl.h>
#include <stdlib.h>

#include "audio_processing.hpp"

#define WM_LBUTTONDOWN 0x0201

#define PLAY_SOUND_BUTTON_ID 1

using namespace std;

bool startThread = false;

AudioThread thread1(startThread);
AudioThread* thread1Ptr = &thread1;

int waveCmdParams[3] = {440, 50, 500};
waveCommand_t cmd = {
    SINE_WAVE_CMD_ID,
    &waveCmdParams[0]
};
waveCommand_t* cmdPtr = &cmd;

void* playbackCommand[2] = {(void*)thread1Ptr, (void*)cmdPtr};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            EndPaint(hwnd, &ps);
        }
        return 0;
    case WM_COMMAND:
        {
            if(LOWORD(wParam) == PLAY_SOUND_BUTTON_ID && HIWORD(wParam) == BN_CLICKED) {
                printf("BUTTON JUST CLICKED AYOO!!!!!\n");
                return 0;
            }
            break;
        }

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ButtonSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
        printf("Button pressed\n");
        thread1.startThread();
        thread1.startAudioPlayback(&playbackCommand);
        break;
    case WM_LBUTTONUP:
        printf("Button released\n");
        thread1.stopThread();
        break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"Sample Window Class";
    
    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = &CLASS_NAME[0];

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Learning to make a window BRUH!!!",
        WS_OVERLAPPEDWINDOW,

        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,
        NULL,
        hInstance,
        NULL
    );

    HWND hwndButton = CreateWindow(
        L"BUTTON",
        L"Play sound",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10,
        10,
        100,
        100,
        hwnd,
        (HMENU)PLAY_SOUND_BUTTON_ID,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL
    );

    SetWindowSubclass(hwndButton, ButtonSubclassProc, 0, 0);

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while(GetMessage(&msg, NULL, 0 , 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}