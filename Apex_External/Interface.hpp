#pragma once
#include "Windows.h"
#include <cstdint>
#include <string>
#include <TlHelp32.h>

inline int localPid;

inline int PID(std::wstring name)
{
	const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 entry{ };
	entry.dwSize = sizeof(PROCESSENTRY32);

	Process32First(snapshot, &entry);
	do
	{
		if (!name.compare(entry.szExeFile))
		{
			return entry.th32ProcessID;
		}

	} while (Process32Next(snapshot, &entry));

	return 0;
}