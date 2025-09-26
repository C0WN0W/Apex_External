#include <iostream>
#include <thread>
#include <chrono>
#include "utils/offsets.hpp"
#include "utils/structs.hpp"
#include "utils/defs.hpp"
#include "entity.hpp"
#include "Interface.hpp"
#include "Overlay/Overlay.hpp"

int main()
{
	SetConsoleTitle(L" ");

	kInterface = new Interface("NtGdiMakeInfoDC");

	if (!kInterface->ConnectToDriver())
	{
		std::cout << "[-] Failed To Call Driver\n";
		Sleep(2000);
		return EXIT_FAILURE;
	}

	localPid = PID(L"r5apex_dx12.exe");
	if (!localPid) {
		std::cout << "[-] Failed to get PID!";
		Sleep(2000);
		return EXIT_FAILURE;
	}
	std::cout << "[+] PID: " << localPid << "\n";
	DWORD64 BaseAddress = kInterface->BaseAddress(localPid, "r5apex_dx12.exe");
	if (!BaseAddress) {
		std::cout << "[-] Failed to get base address!\n";
		system("pause");
		exit(0);
	}
	std::cout << "[+] Base Address: " << std::hex << BaseAddress << "\n";

	gRender = new Render();
	if (!gRender->InitializeOverlay()) {
		std::cout << "[-] Failed to initialize overlay!\n";
		system("pause");
		exit(0);
	}

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		uint64_t viewRender = Read<uint64_t>(BaseAddress + OFF_VIEW_RENDER);
		uint64_t viewMatrix = Read<uint64_t>(viewRender + OFF_VIEW_MATRIX);
		Matrix m = Read<Matrix>(viewMatrix);

		gRender->BeginDraw();

		// loop 100 times for player, loop 20000 times for dummies
		for (int i = 0; i < 100; i++) {
			DWORD64 Entity = GetEntityById(i, BaseAddress);
			//  std::cout << Entity << "\n";
			if (Entity == 0)
				continue;

			Vector3 Position = Read<Vector3>(Entity + OFF_VecAbsOrigin);
			Vector3 ScreenPosition = _WorldToScreen(Position, m, ScreenSize);
			if (ScreenPosition.z <= 0.f)
				continue;

			// Color based on health
			int MaxHealth = Read<int>(Entity + OFF_iMaxHealth);
			float TargetHealth = Read<float>(Entity + OFF_iHealth) / (float)MaxHealth * 255.f;
			float r = 255.f - TargetHealth;
			float g = TargetHealth;
			float b = 0.f;

			// DrawLineESP({ 960,1080 }, { ScreenPosition.x, ScreenPosition.y }, { r,g,b });
			// Put your drawing functions here
		}

		gRender->EndDraw();
	}
}