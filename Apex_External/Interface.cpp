#include "interface.hpp"
#include <iostream>

Interface::Interface(const char* kernelFunc)
{
    hDriver = CreateFileA("\\\\.\\ApexDriver",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hDriver == INVALID_HANDLE_VALUE)
    {
        return;
    }
}

bool __fastcall Interface::SendRequest(void* data, request_codes code)
{
    if (!data || !code || hDriver == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    request_data request{ 0 };
    request.unique = request_unique;
    request.data = data;
    request.code = code;

    DWORD bytesReturned = 0;
    BOOL success = DeviceIoControl(
        hDriver,
        static_cast<DWORD>(code),
        &request,
        sizeof(request_data),
        &request,
        sizeof(request_data),
        &bytesReturned,
        NULL
    );

    if (!success)
    {
        return false;
    }

    return true;
}

bool Interface::ConnectToDriver()
{
    driver_init data{ 0 };

    data.init = 0;

    SendRequest(&data, init_driver);

    return data.init;
}

bool Interface::Unload()
{
    unload_request data{ 0 };

    data.buffer = 0;

    SendRequest(&data, request_unload);

    return data.buffer;
}

void Interface::ProectProcess(int pid)
{
    process_request_t data{ 0 };

    data.process_id = pid;

    SendRequest(&data, protect_process);
}

bool Interface::ReadPhysical(const int pid, const std::uintptr_t address, void* buffer, const std::size_t size)
{
    read_request data{ 0 };

    data.pid = pid;
    data.address = address;
    data.buffer = buffer;
    data.size = size;

    SendRequest(&data, read_memory);
    return true;
}

uintptr_t Interface::BaseAddress(int pid, const char* module)
{
    base_request data{ 0 };

    data.pid = pid;
    data.name = module;

    SendRequest(&data, get_base);

    return data.handle;
}

bool Interface::GetCR3(int pid)
{
    cr3_request data{ 0 };
    data.pid = pid;

    SendRequest(&data, get_cr3);
    return data.buffer;
}

bool Interface::WritePhysical(const int pid, const std::uintptr_t address, void* buffer, const std::size_t size)
{
    write_request data{ 0 };

    data.pid = pid;
    data.address = address;
    data.buffer = buffer;
    data.size = size;

    return SendRequest(&data, write_memory);
}

NTSTATUS Interface::CopyVirtualMemory(ULONGLONG srcPid, uintptr_t srcAddr, ULONGLONG targetPid, uintptr_t targetAddr, SIZE_T size)
{
    copy_request request{ 0 };
    request.sourcePid = srcPid;
    request.sourceAddress = srcAddr;
    request.targetPid = targetPid;
    request.targetAddress = targetAddr;
    request.size = size;

    // Define variables for timing
    LARGE_INTEGER frequency;
    LARGE_INTEGER startTime;
    LARGE_INTEGER endTime;

    //// Get the frequency of the high-resolution performance counter
    //QueryPerformanceFrequency(&frequency);

    //// Get the start time
    //QueryPerformanceCounter(&startTime);

    // Call the SendRequest function
    SendRequest(&request, copy_memory);

    //// Get the end time
    //QueryPerformanceCounter(&endTime);

    //// Calculate the elapsed time
    //double elapsedTime = static_cast<double>(endTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;

    //// Output the elapsed time
    //std::cout << "Time taken for SendRequest: " << elapsedTime << " seconds" << std::endl;

    // Return the size as NTSTATUS (assuming this is what you want to return)
    return (NTSTATUS)request.size;
}

NTSTATUS Interface::FreeVirtualMemory(ULONGLONG targetPid, uintptr_t address)
{
    free_request serverRequest{ 0 };

    serverRequest.targetPid = targetPid;
    serverRequest.address = address;

    SendRequest(&serverRequest, free_memory);

    return NTSTATUS(serverRequest.code);
}

NTSTATUS Interface::ProtectVirtualMemory(ULONGLONG targetPid, size_t size, uint32_t protect, uintptr_t sourceAddress)
{
    protect_request serverRequest{ 0 };
    serverRequest.targetPid = targetPid;
    serverRequest.sourceAddress = sourceAddress;

    serverRequest.protect = protect;

    serverRequest.size = size;

    SendRequest(&serverRequest, protect_memory);

    return NTSTATUS(serverRequest.code);
}

uint64_t Interface::AllocateVirtualMemory(ULONGLONG targetPid, size_t size, uint32_t allocationType, uint32_t protect, uintptr_t sourceAddress)
{
    allocate_request serverRequest{ 0 };


    serverRequest.targetPid = targetPid;
    serverRequest.sourceAddress = sourceAddress;

    serverRequest.allocationType = allocationType;
    serverRequest.protect = protect;

    serverRequest.size = size;

    SendRequest(&serverRequest, alloc_memory);

    return serverRequest.targetAddress;
}

bool Interface::CallEntryPoint(ULONGLONG targetPid, uintptr_t address, uintptr_t stub)
{
    call_entry_request serverRequest{ 0 };

    serverRequest.process_id = (HANDLE)targetPid;
    serverRequest.address = address;
    serverRequest.shellcode = stub;

    SendRequest(&serverRequest, call_entry);

    return serverRequest.result;
}