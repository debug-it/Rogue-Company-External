#pragma once

#include "includes.h"

typedef struct HookMemory
{
    ULONG Secret;
    SIZE_T Size;
    PVOID Buff;
    PVOID Address;
    HANDLE Pid;
    BOOLEAN Write;
    BOOLEAN Read;
    BOOLEAN ReqBase;
    PVOID Output;
    PCSTR ModuleName;
    PVOID BaseAddress;

}HookMemory;

struct Vector3 {
    float x, y, z;

    Vector3() {}

    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    Vector3 operator^(const Vector3& other) const {
        return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    float Dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    float Distance(const Vector3& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        float dz = z - other.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }
};


struct Vector2
{
    float x;
    float y;
    Vector2 operator-(const Vector2& other) const
    {
        return { x - other.x, y - other.y };
    }
};
struct PlayerInfo
{

    std::string name;
    int ActorID;
    uintptr_t RootComponent;
    Vector3 Location;
    std::vector<Vector2> Bounds;
    std::vector<Vector2> BonePositions;
};

struct FMinimalViewInfo
{
    Vector3 Location; // 0x00(0x0c)
    Vector3 Rotation; // 0x0c(0x0c)
    float FOV; // 0x18(0x04)
    float DesiredFOV; // 0x1c(0x04)
    float OrthoWidth; // 0x20(0x04)
    float OrthoNearClipPlane; // 0x24(0x04)
    float OrthoFarClipPlane; // 0x28(0x04)
    float AspectRatio; // 0x2c(0x04)
};

struct FBoxSphereBounds
{

    Vector3 Origin;                                                  // 0x0000(0x000C) Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
    Vector3 BoxExtent;                                               // 0x000C(0x000C) Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
    float SphereRadius;                                            // 0x0018(0x0004) Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic
};
struct FCameraCacheEntry
{
    float Timestamp; // 0x00(0x04)
    char pad_4[0xc]; // 0x04(0x0c)
    FMinimalViewInfo POV; // 0x10(0x5e0)
};

typedef struct _KERNEL_READ_REQUEST
{
    ULONG ProcessId;
    PVOID Address;
    PVOID Buffer;
    ULONGLONG Size;

} KERNEL_READ_REQUEST, * PKERNEL_READ_REQUEST;

typedef struct _KERNEL_WRITE_REQUEST
{
    ULONG ProcessId;
    PVOID Address;
    PVOID Buffer;
    ULONG Size;

} KERNEL_WRITE_REQUEST, * PKERNEL_WRITE_REQUEST;

typedef struct _KERNEL_PROCESS_INFO
{
    ULONG ProcessId;
    //WCHAR FullPath[256];
    PVOID PebAddress;
    BOOLEAN IsWow64;

} KERNEL_PROCESS_INFO, * PKERNEL_PROCESS_INFO;

struct PEB
{
    BYTE InheritedAddressSpace;
    BYTE ReadImageFileExecOptions;
    BYTE BeingDebugged;
    BYTE BitField;
    PVOID Mutant;
    PVOID ImageBaseAddress;
    PVOID Ldr; //PPEB_LDR_DATA32
    PVOID ProcessParameters;
    PVOID SubSystemData;
    PVOID ProcessHeap;
    PVOID FastPebLock;
    PVOID AtlThunkSListPtr;
    PVOID IFEOKey;
    PVOID CrossProcessFlags;
    PVOID UserSharedInfoPtr;
    UINT SystemReserved;
    UINT AtlThunkSListPtr32;
    PVOID ApiSetMap;
};

struct PEB_LDR_DATA
{
    UINT Length;
    BOOLEAN Initialized;
    PVOID SsHandle; //ULONG SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
};
struct UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PVOID Buffer;
};

struct LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    UINT SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    UINT Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    LIST_ENTRY HashLinks;
    UINT TimeDateStamp;
};
struct LevelStruct
{
    uintptr_t level;
};
struct ActorStruct
{
    char pad[0x17];
    int ActorID;
    char pad2[0x114];
    uintptr_t actorRoot;
    char pad3[0xF4];
    uintptr_t playerState;
    //char pad4[0x54];
    //uintptr_t Mesh;

};
struct ActorMesh
{
    char pad0[0x1BC];
    uintptr_t compToWorld;
    char pad[0x2B4];
    uintptr_t boneArray;
};
struct ActorRootStruct
{
    char pad[0xFF];
    FBoxSphereBounds actorBounds;
    Vector3 RelativeLocation;
};
struct ActorPlayerState
{
    char pad1[0x1B0];
    int teamId;
    char pad[0x14C];
    uintptr_t playerNamePtr;

    //char pad[0x2FC];
    //uintptr_t playerNamePtr;
    //char pad1[0x14F];
    //int teamId;

    //0x44F

};
struct AKSTeamState
{
    int r_TeamNum;
};
struct NameIdStruct
{
    uint16_t flags;
    char name[512];
};