#pragma once
#include <windows.h>
#include <winternl.h>

#if _WIN64
#define PTRMAXVAL ((PVOID64)0x000F000000000000)
#define pVOID PVOID64
#else
#if _WIN32
#define PTRMAXVAL ((PVOID)0xFFF00000)
#define pVOID PVOID
#endif
#endif
#define IsAddressValid(ptr) (((ptr >= 0x40000) && ((pVOID)ptr < PTRMAXVAL) && ((pVOID)ptr != nullptr)) ? TRUE : FALSE)

 
#pragma comment(lib,"MD-x64.lib")
 

typedef unsigned long long u64;
typedef unsigned long u32;

typedef PVOID64 ptr;
typedef PVOID64 p;
typedef u32* dptr;
typedef u64* qptr;

class Driver
{
public:
	Driver();
	~Driver();
public:
	BOOL Loaddriver(char* key);
	BOOL IsInstall();
	BOOL proceint(u64 ProcessId);
	void Mouse_move(long x, long y, unsigned short button_flags);
	void Mouse_move2(long x, long y, unsigned short button_flags);
	//mode=0 default; mode=1 CR3fixMode
	BOOL ReadProcessMemory(u64 addr, ptr buffer, u64 size, u64 mode, u64* lpNumberOfBytesRead);
	BOOL WriteProcessMemory(u64 addr, ptr buffer, u64 nSize, u64 mode, u64* lpNumberOfBytesWritten);

	u64 GetMoudleBase();
	u64 GetMoudleBaseEx(const char* name);
	void QSDeleteFileEx(const char* name);
	// Anti ss
	u32 Protect_sprite_content(u64 handle, u32 attributes);
	
	
	u64 AllocateMemory(u64 size);
	
	void FreeMemory(u64 addrs);
	
	u32 ProtectMemory(u64 addrs, u64 size, u32 Protect);
	
	void QueryMemory(u64 addrs, PMEMORY_BASIC_INFORMATION pdata);
	
	void InitializeDecrypt(u64 base);

	u64 KernelDecrypt(u64 buff);

	BOOL ValidPtr(ULONG64 Ptr, ULONG a = 0)
	{
		return (BOOL)!IsAddressValid(Ptr); 
	}

	template<typename T>
	T RPM(unsigned long long Addr);

	template<typename T>
	T RPM(unsigned long long Addr, unsigned long Size);

	template<typename T>
	bool RPM(unsigned long long Addr, T OuterBuffer, unsigned long Size);

	template<typename T>
	bool WPM(unsigned long long Addr, T value);

	template<typename T>
	bool WPM(unsigned long long Addr, T value, unsigned long Size);
};


template<typename T> inline T Driver::RPM(unsigned long long Addr)
{
	T readBuffer{};
	u64 lpNumberOfBytesRead;
	ReadProcessMemory(Addr, &readBuffer, sizeof(T),0, &lpNumberOfBytesRead);
	return (T)readBuffer;
}

template<typename T> inline T Driver::RPM(unsigned long long Addr, unsigned long Size)
{
	T readBuffer = {};
	u64 lpNumberOfBytesRead;
	ReadProcessMemory(Addr, &readBuffer, Size,0, &lpNumberOfBytesRead);
	return (T)readBuffer;
}

template<typename T> inline bool Driver::RPM(unsigned long long Addr, T OuterBuffer, unsigned long Size)
{
	u64 lpNumberOfBytesRead;
	ReadProcessMemory(Addr, OuterBuffer, Size,0, &lpNumberOfBytesRead);
	return TRUE;
}


template<typename T> inline bool Driver::WPM(unsigned long long Addr, T value)
{
	u64 lpNumberOfBytesRead;
	WriteProcessMemory((ptr)Addr, &value, sizeof(T),0, &lpNumberOfBytesRead);
	return TRUE;
}

template<typename T> inline bool Driver::WPM(unsigned long long Addr, T value, unsigned long Size)
{
	u64 lpNumberOfBytesRead;
	WriteProcessMemory((ptr)Addr, &value, Size, 0,&lpNumberOfBytesRead);
	return TRUE;
}

extern Driver drv;

