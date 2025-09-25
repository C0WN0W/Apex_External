#pragma once
#include "../utils/defs.hpp"
#include "../utils/structs.hpp"
#include <Windows.h>

class Render {
private:
    HWND overlayWindow;
    HDC hdc;
    HBRUSH brush;
    HPEN pen;

public:
    Render();
    ~Render();

    bool InitializeOverlay();
    void BeginDraw();
    void EndDraw();

    // Basic drawing functions
    void DrawLine(float x1, float y1, float x2, float y2, COLORREF color, int Width = 3);
    void DrawBox(float x, float y, float width, float height, COLORREF color);
};

extern Render* gRender;