# About

Project seeks to have a stable / reliable method for ALPC exploit originally disclosed by Sandbox Escaper.

Attempts to clean up itself after loading the DLL.

Screenshots :

  
  ![](https://github.com/realoriginal/alpc-diaghub/blob/master/Image%20Pasted%20at%202018-11-2%2020-06.png.jpg)
  ![](https://github.com/realoriginal/alpc-diaghub/blob/master/Image%20Pasted%20at%202018-11-2%2020-09.png)

Example Payload : 

```
#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD dwReason, LPVOID lpReserved)
{
        switch(dwReason)
        {
                case DLL_PROCESS_ATTACH:
                        WinExec("C:\\Windows\\System32\\notepad.exe", 0);
                        break;
                case DLL_PROCESS_DETACH:
                        break;
                case DLL_THREAD_ATTACH:
                        break;
                case DLL_THREAD_DETACH:
                        break;
        }

        return 0;
}
```
