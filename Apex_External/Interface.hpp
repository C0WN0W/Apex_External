#pragma once
#include "Windows.h"
#include "comms.hpp"
#include <cstdint>
#include <cstddef>
#include <string>
#include <memory>
#include <vector>
#include <TlHelp32.h>

class Interface {
private:
	HANDLE hDriver = INVALID_HANDLE_VALUE;

public:
	Interface(const char* kernelFunc);

	bool ConnectToDriver();
	bool ReadPhysical(const int pid, const std::uintptr_t address, void* buffer, const std::size_t size);

	template <typename t>
	t ReadPhysical(const int pid, const std::uintptr_t address)
	{
		t response{ };
		ReadPhysical(pid, address, &response, sizeof(t));
		return response;
	}

	bool WritePhysical(const int pid, const std::uintptr_t address, void* buffer, const std::size_t size);

	template <typename t>
	bool WritePhysical(const int pid, const std::uintptr_t address, t value)
	{
		return WritePhysical(pid, address, &value, sizeof(t));
	}
	uintptr_t BaseAddress(int pid, const char* module);
	bool GetCR3(int pid);
	bool Unload();
	void* AllocateMemory(const std::size_t size, std::uintptr_t* mdl);
	bool __fastcall SendRequest(void* data, request_codes code);

	void ProectProcess(int pid);

	uint64_t AllocateVirtualMemory(ULONGLONG targetPid, size_t size, uint32_t allocationType, uint32_t protect, uintptr_t sourceAddress);
	NTSTATUS CopyVirtualMemory(ULONGLONG srcPid, uintptr_t srcAddr, ULONGLONG targetPid, uintptr_t targetAddr, SIZE_T size);
	NTSTATUS ProtectVirtualMemory(ULONGLONG targetPid, size_t size, uint32_t protect, uintptr_t sourceAddress);
	NTSTATUS FreeVirtualMemory(ULONGLONG targetPid, uintptr_t address);

	bool CallEntryPoint(ULONGLONG targetPid, uintptr_t address, uintptr_t stub);
};

inline Interface* kInterface;
inline int localPid;

inline uint64_t AllocateMemory(size_t size, uint32_t allocation_type, uint32_t protect)
{
	uint64_t address = 0;
	return kInterface->AllocateVirtualMemory(localPid, size, allocation_type, protect, address);
}
inline NTSTATUS ReadBuffer(uint64_t address, LPVOID lpBuffer, SIZE_T nSize)
{
	if (lpBuffer == 0)
		return STATUS_INVALID_PARAMETER;

	return kInterface->CopyVirtualMemory(localPid, address, GetCurrentProcessId(), uintptr_t(lpBuffer), nSize);
}
inline uint64_t GetModuleBase(const char* moduleName)
{
	return kInterface->BaseAddress(localPid, moduleName);
}
inline NTSTATUS WriteMemory(uint64_t address, uintptr_t dstAddress, SIZE_T nSize)
{
	if (dstAddress == 0)
		return STATUS_INVALID_PARAMETER;

	return kInterface->CopyVirtualMemory(GetCurrentProcessId(), dstAddress, localPid, address, nSize);
}
inline NTSTATUS ProtectMemory(uint64_t address, size_t size, uint32_t protect)
{
	return kInterface->ProtectVirtualMemory(localPid, size, protect, address);
}
inline NTSTATUS FreeMemory(uint64_t address)
{
	return kInterface->FreeVirtualMemory(localPid, address);
}
template<typename T>
inline T Read(uint64_t address)
{
	T buffer{ };
	if (address == NULL || address < 10000) {
		return buffer;
	}
	kInterface->ReadPhysical(localPid, address, &buffer, sizeof(T));
	return buffer;
}
template<typename T>
inline bool Write(uint64_t address, T value)
{
	kInterface->WritePhysical<T>(localPid, address, value);
	return true;
}
inline std::string ReadString(uint64_t Address)
{
	std::unique_ptr<char[]> buffer(new char[64]);
	kInterface->ReadPhysical(localPid, Address, buffer.get(), 64);
	return buffer.get();
}
inline std::string read_wstr(uintptr_t address)
{
	wchar_t buffer[1024 * sizeof(wchar_t)];
	kInterface->ReadPhysical(localPid, address, &buffer, 1024 * sizeof(wchar_t));

	std::wstring new_buffer = std::wstring(buffer);

	return std::string(new_buffer.begin(), new_buffer.end());
}

template <typename T>
inline T Read_chain(std::uint64_t address, std::vector<std::uint64_t> chain)
{
	uint64_t cur_read = address;

	for (size_t r = 0; r < chain.size() - 1; ++r)
		cur_read = Read<std::uint64_t>(cur_read + chain[r]);

	return Read<T>(cur_read + chain[chain.size() - 1]);
}

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