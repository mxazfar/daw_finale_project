#ifndef UNICODE
#define UNICODE
#endif 

#include <iostream>
#include "audio_processing.hpp"
#include <Windows.h>
#include <WinUser.h>

#define WM_LBUTTONDOWN 0x0201

#define PLAY_SOUND_BUTTON_ID 1

using namespace std;

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

    case WM_NOTIFY:
        {
            NMHDR* pnmhdr = (NMHDR*)lParam;
            if(pnmhdr->idFrom == PLAY_SOUND_BUTTON_ID) {
                switch(pnmhdr->code) {
                    case BN_PUSHED:
                        printf("BUTTON BEING PUSHED!!!!");
                        return 0;
                    
                    case BN_UNPUSHED:
                        printf("BUTTON NOT BEING PUSHED :((");
                        return 0;
                }
            }
            break;
        }
    case WM_COMMAND:
        {
            if(LOWORD(wParam) == PLAY_SOUND_BUTTON_ID && HIWORD(wParam) == BN_CLICKED) {
                printf("BUTTON JUST CLICKED AYOO!!!!!");
                return 0;
            }
            break;
        }

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
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
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_NOTIFY,
        10,
        10,
        100,
        100,
        hwnd,
        (HMENU)PLAY_SOUND_BUTTON_ID,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL
    );

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