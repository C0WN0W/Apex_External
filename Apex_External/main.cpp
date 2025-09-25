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
}