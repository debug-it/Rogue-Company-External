#pragma once
#include "Windows.h"


namespace Cheat
{
	//dynamic
	uintptr_t uworld;
	uintptr_t gameInstance;
	uintptr_t localPlayers;
	uintptr_t ulocalPlayer;
	uintptr_t localPlayerController;
	uintptr_t localRootComponent;
	uintptr_t localAcknowledgedPawn;
	uintptr_t playerCamera;
	uintptr_t PersistentLevel;
	uintptr_t Levels;

	Vector3 CameraRotation;
	Vector3 CameraLocation;

	static int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	static int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	float fov;
	bool aimlock;
	int closestPlayerId;


}
namespace Offsets
{
	//static
	uintptr_t UWorld = 0x72597A8;
	uintptr_t GNames = 0x70EA640;
	uintptr_t GameInstance = 0x188;

	uintptr_t LocalPlayers = 0x38;
	uintptr_t PlayerController = 0x30;
	uintptr_t AcknowledgedPawn = 0x2A0;

	uintptr_t CameraManager = 0x2B8;
	uintptr_t CameraComponent = 0x228;
	uintptr_t CameraCache = 0x1aa0;
	uintptr_t CameraCachePOV = CameraCache + 0x0010;
	uintptr_t CameraLocation = CameraCachePOV + 0x0;
	uintptr_t CameraRotation = CameraCachePOV + 0xC;

	uintptr_t ActorArray = 0x98;
	uintptr_t ActorCount = ActorArray + 0x8;

	uintptr_t RootComponent = 0x130;
	uintptr_t ActorBounds = 0x100;
	uintptr_t PersistentLevel = 0x30;
	uintptr_t Levels = 0x148;
	uintptr_t Mesh = 0x280;
	uintptr_t BoneArray = 0x478;
	uintptr_t ComponentToWorld = 0x1c0;
	uintptr_t PlayerState = 0x240;

	uintptr_t RelativeLocation = 0x11C;
	uintptr_t RelativeRotation = 0x128;
	uintptr_t RelativeRotationYaw = RelativeRotation + 0x4;

}