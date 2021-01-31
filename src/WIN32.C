#include <windows.h>
#include <mmsystem.h>

#include "NOKIA.C"
#include "GAME.C"

void WINAPI WinFatalError (const char* msg)
{
    MessageBoxA(NULL, msg, "WINERROR", MB_OK|MB_ICONERROR);
    ExitProcess(1); // Failure
}

void WINAPI WinUpdateDisplay (HWND hwnd, HDC hdc, nkCONTEXT* nokia)
{
    RECT client;
    GetClientRect(hwnd, &client);

    INT windowWidth = client.right-client.left;
    INT windowHeight = client.bottom-client.top;

    HDC backDC = CreateCompatibleDC(hdc);
    HBITMAP backbuffer = CreateCompatibleBitmap(hdc, windowWidth, windowHeight);

    INT savedDC = SaveDC(backDC);
    SelectObject(backDC, backbuffer);
    HBRUSH brush = CreateSolidBrush(RGB(0,0,0));
    FillRect(backDC, &client, brush);
    DeleteObject(brush);

    INT xScale = windowWidth / NK_SCREEN_W;
    INT yScale = windowHeight / NK_SCREEN_H;

    INT scale = NK_MIN(xScale,yScale);

    INT dstX = (windowWidth - (NK_SCREEN_W*scale)) / 2;
    INT dstY = (windowHeight - (NK_SCREEN_H*scale)) / 2;
    INT dstW = (NK_SCREEN_W*scale);
    INT dstH = (NK_SCREEN_H*scale);
    INT srcX = 0;
    INT srcY = 0;
    INT srcW = NK_SCREEN_W;
    INT srcH = NK_SCREEN_H;

    BITMAPINFO bitmapInfo;
    ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = NK_SCREEN_W;
    bitmapInfo.bmiHeader.biHeight = -NK_SCREEN_H;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = nokia->screen.bits;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    StretchDIBits(backDC, dstX,dstY,dstW,dstH, srcX,srcY,srcW,srcH,
        nokia->screen.pixels, &bitmapInfo, DIB_RGB_COLORS,SRCCOPY);

    BitBlt(hdc, 0,0,windowWidth,windowHeight, backDC, 0,0, SRCCOPY);
    RestoreDC(backDC,savedDC);

    DeleteObject(backbuffer);
    DeleteDC(backDC);

    // Update the viewport values.
    nokia->viewportX = dstX;
    nokia->viewportY = dstY;
    nokia->viewportW = dstW;
    nokia->viewportH = dstH;
}

LRESULT CALLBACK WinProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // It is important to check that this is non-null before use as WinProc can be called before this is set.
    nkCONTEXT* nokia = NK_CAST(nkCONTEXT*, GetWindowLongPtr(hwnd,0));

    switch (uMsg)
    {
        case (WM_GETMINMAXINFO):
        {
            RECT rwWindow = { 0,0,NK_SCREEN_W,NK_SCREEN_H };
            AdjustWindowRect(&rwWindow, WS_OVERLAPPEDWINDOW, FALSE);
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI->ptMinTrackSize.x = rwWindow.right - rwWindow.left;
            lpMMI->ptMinTrackSize.y = rwWindow.bottom - rwWindow.top;
            return 0;
        }
        case (WM_SYSKEYDOWN):
        {
            if (wParam == VK_RETURN)
            {
                // Toggle Fullscreen

                static WINDOWPLACEMENT windowPlacement = { sizeof(windowPlacement) };

                DWORD windowStyle = GetWindowLongPtrA(hwnd, GWL_STYLE);
                if (windowStyle & WS_OVERLAPPEDWINDOW)
                {
                    MONITORINFO monitorInfo = { sizeof(monitorInfo) };
                    if (GetWindowPlacement(hwnd, &windowPlacement) && GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &monitorInfo))
                    {
                        SetWindowLongPtrA(hwnd, GWL_STYLE, windowStyle & ~WS_OVERLAPPEDWINDOW);
                        SetWindowPos(hwnd, HWND_TOP,
                            monitorInfo.rcMonitor.left,
                            monitorInfo.rcMonitor.top,
                            monitorInfo.rcMonitor.right-monitorInfo.rcMonitor.left,
                            monitorInfo.rcMonitor.bottom-monitorInfo.rcMonitor.top,
                            SWP_NOOWNERZORDER|SWP_FRAMECHANGED);
                        while (ShowCursor(FALSE) >= 0);
                    }
                }
                else
                {
                    SetWindowLongPtrA(hwnd, GWL_STYLE, windowStyle | WS_OVERLAPPEDWINDOW);
                    SetWindowPlacement(hwnd, &windowPlacement);
                    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_FRAMECHANGED);
                    while (ShowCursor(TRUE) < 0);
                }

                return 0;
            }
            // Important we don't return zero here and instead pass control to DefWindowProc!
            // This is so that other system keys (ALT+F4, etc.) still all work correctly.
        } break;
        case (WM_PAINT):
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            if (nokia) WinUpdateDisplay(hwnd, hdc, nokia);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case (WM_ERASEBKGND):
        {
            return 1;
        }
        case (WM_DESTROY):
        {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int APIENTRY WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
    // Attach to our parent's console if possible so that we can get console input and output.
    // If we don't have a parent console then we don't really care and don't create a console.
    #ifdef NK_DEBUG
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
        freopen("CONIN$",  "r", stdin );
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
    #endif

    WNDCLASSEXA windowClass;
    ZeroMemory(&windowClass, sizeof(windowClass));
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_HREDRAW|CS_VREDRAW;
    windowClass.lpfnWndProc = WinProc;
    windowClass.cbWndExtra = sizeof(nkCONTEXT*);
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = "nkWINCLASS";
    windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassExA(&windowClass))
    {
        WinFatalError("Failed to register window class!");
    }

    // Adjust the window so its client area is actually the size we specify without the border being included.
    RECT rwWindow = { 0,0,NK_SCREEN_W*8,NK_SCREEN_H*8 };
    AdjustWindowRect(&rwWindow, WS_OVERLAPPEDWINDOW, FALSE);

    LONG windowWidth = rwWindow.right - rwWindow.left;
    LONG windowHeight = rwWindow.bottom - rwWindow.top;

    HWND hwnd = CreateWindowExA(0, windowClass.lpszClassName, "NOKIA", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, 0, 0, windowClass.hInstance, 0);
    if (!hwnd)
    {
        WinFatalError("Failed to create the window!\n");
    }

    INT screenWidth = NK_SCREEN_W;
    INT screenHeight = NK_SCREEN_H;
    INT screenBits = 32;
    INT screenSize = (screenWidth*screenHeight*(screenBits/8));

    // We know the size of the screen at compile-time so we don't dynamically allocate it.
    // Instead we just have a static array that we point the screen's pixels pointer to.
    U32 screenPixels[NK_SCREEN_W*NK_SCREEN_H];
    ZeroMemory(screenPixels, sizeof(screenPixels));

    nkCONTEXT nokia;
    ZeroMemory(&nokia, sizeof(nokia));
    nokia.screen.width = screenWidth;
    nokia.screen.height = screenHeight;
    nokia.screen.bits = screenBits;
    nokia.screen.pixels = screenPixels;

    // Store the nkCONTEXT in our HWND so we can access it in our WinProc
    // function, and potentially anywhere else we might want to in the code.
    SetWindowLongPtrA(hwnd, 0, NK_CAST(LONG_PTR, &nokia));

    // Center window.
    RECT rwParent;
    RECT rcParent;

    HWND parent = GetDesktopWindow();

    GetWindowRect(parent, &rwParent);
    GetWindowRect(hwnd, &rwWindow);
    GetClientRect(parent, &rcParent);

    INT xPos = rwParent.left + (rcParent.right + rwWindow.left - rwWindow.right) / 2;
    INT yPos = rwParent.top + (rcParent.bottom + rwWindow.top - rwWindow.bottom) / 2;

    SetWindowPos(hwnd, NULL, xPos,yPos, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);

    LARGE_INTEGER perfFrequency;
    LARGE_INTEGER elapsedCounter;
    LARGE_INTEGER lastCounter;
    LARGE_INTEGER endCounter;

    FLOAT deltaTime = 1.0f / 15.0f;

    QueryPerformanceFrequency(&perfFrequency);
    QueryPerformanceCounter(&lastCounter);

    ShowWindow(hwnd, SW_SHOW);

    BOOL running = TRUE;
    while (running)
    {
        MSG message;
        while (PeekMessageA(&message, 0,0,0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
            if (message.message == WM_QUIT)
            {
                running = FALSE;
            }
        }

        CopyMemory(nokia.prevKeyState, nokia.currKeyState, sizeof(nokia.prevKeyState));
        nokia.currKeyState[NK_KEY_Q    ] = ((GetKeyState('Q'          )>>8) != 0);
        nokia.currKeyState[NK_KEY_W    ] = ((GetKeyState('W'          )>>8) != 0);
        nokia.currKeyState[NK_KEY_E    ] = ((GetKeyState('E'          )>>8) != 0);
        nokia.currKeyState[NK_KEY_A    ] = ((GetKeyState('A'          )>>8) != 0);
        nokia.currKeyState[NK_KEY_S    ] = ((GetKeyState('S'          )>>8) != 0);
        nokia.currKeyState[NK_KEY_D    ] = ((GetKeyState('D'          )>>8) != 0);
        nokia.currKeyState[NK_KEY_Z    ] = ((GetKeyState('Z'          )>>8) != 0);
        nokia.currKeyState[NK_KEY_X    ] = ((GetKeyState('X'          )>>8) != 0);
        nokia.currKeyState[NK_KEY_C    ] = ((GetKeyState('C'          )>>8) != 0);
        nokia.currKeyState[NK_KEY_SPACE] = ((GetKeyState(VK_SPACE     )>>8) != 0);
        nokia.currKeyState[NK_KEY_COMMA] = ((GetKeyState(VK_OEM_COMMA )>>8) != 0);
        nokia.currKeyState[NK_KEY_POINT] = ((GetKeyState(VK_OEM_PERIOD)>>8) != 0);

        nkBeginFrame(&nokia);
        nkGameUpdate(&nokia);
        nkEndFrame(&nokia);

        if (nokia.sound)
        {
            static const char* SOUNDS[NK_SND_TOTAL] =
            {
                NULL,
                "SNDBANK/BLIP01.WAV",
                "SNDBANK/BLIP02.WAV",
                "SNDBANK/BLIP03.WAV",
                "SNDBANK/BLIP04.WAV",
                "SNDBANK/BLIP05.WAV",
                "SNDBANK/BLIP06.WAV",
                "SNDBANK/BLIP07.WAV",
                "SNDBANK/BLIP08.WAV",
                "SNDBANK/BLIP09.WAV",
                "SNDBANK/BLIP10.WAV",
                "SNDBANK/BLIP11.WAV",
                "SNDBANK/BLIP12.WAV",
                "SNDBANK/BLIP13.WAV",
                "SNDBANK/BLIP14.WAV",
                "SNDBANK/C5.WAV",
                "SNDBANK/CRUST.WAV",
                "SNDBANK/GOOD01.WAV",
                "SNDBANK/GOOD02.WAV",
                "SNDBANK/GOOD03.WAV",
                "SNDBANK/HIT01.WAV",
                "SNDBANK/HIT02.WAV",
                "SNDBANK/HIT03.WAV",
                "SNDBANK/HIT04.WAV",
                "SNDBANK/HIT05.WAV",
                "SNDBANK/HIT06.WAV",
                "SNDBANK/JINGLE.WAV",
                "SNDBANK/MELODY.WAV",
                "SNDBANK/NEGTIV01.WAV",
                "SNDBANK/NEGTIV02.WAV",
                "SNDBANK/ODD01.WAV",
                "SNDBANK/ODD02.WAV",
                "SNDBANK/ODD03.WAV",
                "SNDBANK/ODD04.WAV",
                "SNDBANK/RING.WAV",
                "SNDBANK/SNDTEST.WAV",
                "SNDBANK/UNREAL.WAV",
            };

            PlaySound(SOUNDS[nokia.sound], NULL, SND_ASYNC|SND_FILENAME|SND_NODEFAULT);
            nokia.sound = NK_SND_NONE; // Reset the sound after playing it.
        }

        HDC hdc = GetDC(hwnd);
        WinUpdateDisplay(hwnd, hdc, &nokia);
        ReleaseDC(hwnd, hdc);

        QueryPerformanceCounter(&endCounter);
        elapsedCounter.QuadPart = endCounter.QuadPart - lastCounter.QuadPart;
        FLOAT elapsedTime = (FLOAT)(elapsedCounter.QuadPart) / (FLOAT)(perfFrequency.QuadPart);
        while (elapsedTime < deltaTime)
        {
            FLOAT timeLeft = deltaTime - elapsedTime;
            Sleep((DWORD)(1000.0f * timeLeft));
            QueryPerformanceCounter(&endCounter);
            elapsedCounter.QuadPart = endCounter.QuadPart - lastCounter.QuadPart;
            elapsedTime = (FLOAT)(elapsedCounter.QuadPart) / (FLOAT)(perfFrequency.QuadPart);
        }
        QueryPerformanceCounter(&lastCounter);
    }

    return 0;
}
