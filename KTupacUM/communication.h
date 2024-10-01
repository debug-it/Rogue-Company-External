#pragma once

#include "includes.h"

class Driver
{
private:

public:
	HANDLE handle;
	ULONG procId;
	uintptr_t moduleBase;
	HMODULE win32Base;
	//

	Driver();

	bool Load();

	bool IsLoaded();

	bool GetModuleBase(LPCWSTR targetModule);

	bool GetProcId(std::wstring exeName);

	template<typename T>
	bool WriteMemory(uintptr_t writeAddress, T buff, SIZE_T Size)
	{
		HookMemory mem{};
		mem.Secret = 69696969;
		mem.Pid = (HANDLE)procId;
		mem.Write = TRUE;
		mem.Address = (PVOID)writeAddress;
		mem.Size = Size;
		mem.Buff = &buff;

		//std::cout << "Trying to com" << std::endl;
		PVOID result = 0;// Redacted

		DWORD Bytes;

		if (mem.Output != 0)
		{
			//std::cout << request.Size << std::endl;

			return true;
		}
	

		return false;
	}

	std::string GetNameFromId(int ID);
	//Vector3 Driver::GetBoneIndex(DWORD_PTR Mesh, int index);
	template<typename T>
	bool ReadMemory(uintptr_t ReadAddress, SIZE_T Size, T& temp)
	{

		HookMemory mem{};
		mem.Secret = 69696969;
		mem.Pid = (HANDLE)procId;
		mem.Read = TRUE;
		mem.Address = (PVOID)ReadAddress;
		mem.Size = Size;
		mem.Buff = &temp;

		

		//std::cout << "Trying to com" << std::endl;
		PVOID result = 0; // Redacted

		DWORD Bytes;

		temp = *(T*)mem.Buff;
	
		if (mem.Buff != nullptr)
		{
			//std::cout << request.Size << std::endl;
			
			return true;
		}
		//else
			return false;
	}


};

