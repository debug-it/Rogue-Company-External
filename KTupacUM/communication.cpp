#include "communication.h"
#include <tlhelp32.h>


Driver::Driver()
{
    Load();
}
bool Driver::Load()
{
    win32Base = LoadLibrary(L"win32u.dll");
    //redacted

    return true;
}
bool Driver::IsLoaded()
{
    constexpr uintptr_t invalidHandle = 0xFFFFFFFFFFFFFFFF;

    return (handle != INVALID_HANDLE_VALUE);
}


bool Driver::GetModuleBase(LPCWSTR targetModule)
{

    HookMemory mem{};
    mem.Secret = 69696969;
    mem.Pid = (HANDLE)procId;
    mem.ReqBase = TRUE;

    //std::cout << "Trying to com" << std::endl;
    PVOID result = 0; // Redacted


    {
        PVOID PebAddress = mem.Buff;
        PEB Peb;

        std::cout << "[+] Trying to read Peb at 0x" << PebAddress << std::endl;
        ReadMemory<PEB>((DWORD_PTR)PebAddress, sizeof(PEB), Peb);

        if (Peb.Ldr == 0)
            std::cerr << "Process LDR is not initialised!" << std::endl;
        std::cout << "[+] Process LDR found at 0x" << Peb.Ldr << std::endl;

        PEB_LDR_DATA Ldr;
        ReadMemory<PEB_LDR_DATA>((DWORD_PTR)Peb.Ldr, sizeof(PEB_LDR_DATA), Ldr);

        auto Head = Ldr.InLoadOrderModuleList.Flink;
        PVOID Next = Head;

        static bool first = true;
        while (Next != Head || first)
        {
            first = false;

            LDR_DATA_TABLE_ENTRY Entry;
            if (!ReadMemory<LDR_DATA_TABLE_ENTRY>((DWORD_PTR)Next, sizeof(LDR_DATA_TABLE_ENTRY), Entry))
                continue;
            Next = Entry.InLoadOrderLinks.Flink;

            if (Entry.SizeOfImage == 0 || Entry.DllBase == nullptr || Entry.EntryPoint == 0)
                continue;


            auto name = new wchar_t[Entry.FullDllName.Length / 2 + 1];
            ReadMemory((DWORD_PTR)Entry.BaseDllName.Buffer, Entry.FullDllName.Length, *name);

            //std::cout << "ModuleBase found at 0x" << Entry.DllBase << std::endl;
            //std::cout << "SizeOfImage: " << Entry.SizeOfImage << std::endl;
            //std::wcout << "Name: " << (name) << "  " << std::endl;
            ////std::wcout << "Address: " << &namePtr << "  " << std::endl;;
            //std::cout << "Name Len: " << Entry.FullDllName.Length <<  "  " << std::endl;
            //std::cout << std::endl;
            if (!lstrcmpW(name, targetModule))
                moduleBase = (uintptr_t)Entry.DllBase;

        }


        return true;
    }

}
//FTransform
//Vector3 Driver::GetBoneIndex(uintptr_t Mesh, int index)
//{
//    DWORD_PTR bonearray = ReadMemory(Mesh + Offsets);
//    if (!bonearray)
//        bonearray = read<DWORD_PTR>(mesh + GameOffset.offset_bone_array + 0x10);
//    return read<FTransform>(bonearray + (index * 0x30));
//}
//Vector3 GetBoneWithRotation(uintptr_t Mesh, int Id)
//{
//    FTransform bone = ();
//
//}

std::string Driver::GetNameFromId(int ID) {
    try {

        uintptr_t fNamePool = moduleBase + 0x70EA640;
        static constexpr int stride = 0x2;
        static constexpr int maxStride = stride * (1 << 16);

        int currentBlock;
        ReadMemory(fNamePool + 0x8, sizeof(int), currentBlock);

        int block = ID >> 16;
        uint16_t offset = (uint16_t)ID;
        if (ID < 0 || block > currentBlock + 1 || offset * stride > maxStride)
        {
            block = 0;
            offset = 0;
        }

        uintptr_t fNameBlock;
        ReadMemory(fNamePool + 0x10 + block * sizeof(uintptr_t), sizeof(uintptr_t), fNameBlock);

        uintptr_t fNameEntry = fNameBlock + offset * stride;
        
        NameIdStruct nameIdStruct;
        ReadMemory<NameIdStruct>(fNameEntry, sizeof(NameIdStruct), nameIdStruct);


        uint16_t length = nameIdStruct.flags >> 6;

        // bool wide = flags & 0x1;
        // if (wide)
        // {
        //     auto word = new wchar_t[length];
        //     ReadMemory(fNameEntry + sizeof(uint16_t), sizeof(wchar_t) * length, *word);
        //     return word;
        // }
    /*    auto word = new char[length + 1];
        word[length] = 0x00;*/


        
        
        //ReadMemory(fNameEntry + sizeof(uint16_t), sizeof(char) * length, *word);
        return std::string((const char*)nameIdStruct.name, length);


    }
    catch (int e)
    {
        return std::string("");
    }
}

bool Driver::GetProcId(std::wstring exeName)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(snapshot, &entry)) {
        CloseHandle(snapshot);
        return false;
    }

    do {
        if (_wcsicmp(entry.szExeFile, exeName.c_str()) == 0) {
            CloseHandle(snapshot);
            procId = entry.th32ProcessID;
            return true;
        }
    } while (Process32Next(snapshot, &entry));

    CloseHandle(snapshot);
    return false;

}
