#pragma once
#include "Render.hpp"

void DrawBoxESP(Vector3 foot, Vector3 head)
{
	float height = head.y - foot.y;
	float width = height / 1.2f;
	gRender->DrawBox(foot.x - width / 2, foot.y, head.x + width, head.y + height, RGB(0, 255, 0));
}

void DrawLineESP(Vector2 Start, Vector2 End, Vector3 Color)
{
	gRender->DrawLine(Start.x, Start.y, End.x, End.y, RGB(Color.x, Color.y, Color.z), 2);
}