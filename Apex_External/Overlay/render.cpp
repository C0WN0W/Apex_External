#include "Render.hpp"

Render* gRender = nullptr;

Render::Render() : overlayWindow(nullptr), hdc(nullptr), brush(nullptr), pen(nullptr) {}

Render::~Render() {
    if (brush) DeleteObject(brush);
    if (pen) DeleteObject(pen);
    if (hdc) ReleaseDC(overlayWindow, hdc);
    if (overlayWindow) DestroyWindow(overlayWindow);
}

bool Render::InitializeOverlay() {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = DefWindowProc;
    wc.lpszClassName = L"Chrome";
    RegisterClassEx(&wc);

    overlayWindow = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        L"Chrome",
        L"Overlay",
        WS_POPUP,
        0, 0, (int)ScreenSize.x, (int)ScreenSize.y,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );

    if (!overlayWindow) return false;

    SetLayeredWindowAttributes(overlayWindow, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(overlayWindow, SW_SHOW);

    hdc = GetDC(overlayWindow);
    SetBkMode(hdc, TRANSPARENT);

    return true;
}

void Render::BeginDraw() {
    if (!overlayWindow || !hdc) return;
    // Clear the overlay window with a transparent background
    RECT rect = { 0, 0, (LONG)ScreenSize.x, (LONG)ScreenSize.y };
    HBRUSH transparentBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdc, &rect, transparentBrush);
    DeleteObject(transparentBrush);
}

void Render::EndDraw() {
    if (!overlayWindow || !hdc) return;
    // Update the overlay window
    InvalidateRect(overlayWindow, nullptr, FALSE);
}

void Render::DrawLine(float x1, float y1, float x2, float y2, COLORREF color, int Width) {
    if (!hdc) return;
    pen = CreatePen(PS_SOLID, Width, color);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    MoveToEx(hdc, (int)x1, (int)y1, nullptr);
    LineTo(hdc, (int)x2, (int)y2);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void Render::DrawBox(float x, float y, float width, float height, COLORREF color) {
    DrawLine(x, y, x + width, y, color);
    DrawLine(x + width, y, x + width, y + height, color);
    DrawLine(x + width, y + height, x, y + height, color);
    DrawLine(x, y + height, x, y, color);
}