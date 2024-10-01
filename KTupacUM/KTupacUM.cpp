// KTupacUM.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <mutex>
#include <Windows.h>
#include <iostream>
#include <vector>

#include "communication.h"
#include "helpers.h"
#include "menu.h"
#include "offsets.h"


int GetDefaults(Driver driver)
{
    {
        driver.ReadMemory<uintptr_t>((driver.moduleBase + Offsets::UWorld), sizeof(uintptr_t), Cheat::uworld);
        if (Cheat::uworld == NULL)
        {
            Print("UWorld is null!");
            return 0;
        }

        driver.ReadMemory<uintptr_t>((Cheat::uworld + Offsets::GameInstance), sizeof(uintptr_t), Cheat::gameInstance);
        if (Cheat::gameInstance == NULL)
        {
            Print("GameInstance is null!");
            return 0;
        }

        driver.ReadMemory<uintptr_t>((Cheat::gameInstance + Offsets::LocalPlayers), sizeof(uintptr_t), Cheat::localPlayers);
        if (Cheat::localPlayers == NULL)
        {
            Print("LocalPlayer is null!");
            return 0;
        }

        driver.ReadMemory<uintptr_t>((Cheat::localPlayers), sizeof(uintptr_t), Cheat::ulocalPlayer);
        if (Cheat::ulocalPlayer == NULL)
        {
            Print("LocalPlayer is null!");
            return 0;
        }
        std::cout << "[+] LocalPlayer found at: 0x" << std::hex << Cheat::ulocalPlayer << std::endl;


        driver.ReadMemory<uintptr_t>((Cheat::ulocalPlayer + Offsets::PlayerController), sizeof(uintptr_t), Cheat::localPlayerController);
        if (Cheat::localPlayerController == NULL)
        {
            Print("PlayerController is null!");
            return 0;
        }
        std::cout << "[+] PlayerController found at: 0x" << std::hex << Cheat::localPlayerController << std::endl;

        driver.ReadMemory<uintptr_t>((Cheat::localRootComponent + Offsets::RootComponent), sizeof(uintptr_t), Cheat::localRootComponent);
        if (Cheat::localPlayerController == NULL)
        {
            Print("PlayerController is null!");
            return 0;
        }


        driver.ReadMemory<uintptr_t>((Cheat::localPlayerController + Offsets::CameraManager), sizeof(uintptr_t), Cheat::playerCamera);
        if (Cheat::playerCamera == NULL)
        {
            Print("PlayerCamera is null!");
            return 0;
        }
        std::cout << "[+] PlayerCamera found at: 0x" << std::hex << Cheat::playerCamera << std::endl;

        driver.ReadMemory<uintptr_t>(Cheat::uworld + Offsets::PersistentLevel, sizeof(uintptr_t), Cheat::PersistentLevel);
        if (Cheat::PersistentLevel == NULL)
        {
            Print("ULevel is null!");
            return 0;
        }
    }
}



int HackLoop(Driver driver)
{
    uintptr_t level;
    driver.ReadMemory<uintptr_t>(Cheat::uworld + Offsets::Levels, sizeof(uintptr_t), Cheat::Levels);
    driver.ReadMemory<uintptr_t>(Cheat::Levels + 0 * 0x8, sizeof(uintptr_t), level);

    while (true)
    {
        uintptr_t actorList;
        driver.ReadMemory<uintptr_t>(Cheat::PersistentLevel + Offsets::ActorArray, sizeof(uintptr_t), actorList);
        if (!actorList)
            return 0;

        auto start = std::chrono::high_resolution_clock::now();

        std::vector<PlayerInfo> playersBuffer;
        {
            driver.ReadMemory<uintptr_t>(Cheat::localPlayerController + Offsets::AcknowledgedPawn, sizeof(uintptr_t), Cheat::localAcknowledgedPawn);
            int actorCount;
            driver.ReadMemory<int>(level + Offsets::ActorCount, sizeof(int), actorCount);
            Vector3 localActorLocation;
       
            ActorPlayerState localPlayerState;
            driver.ReadMemory<ActorPlayerState>(Cheat::localAcknowledgedPawn + Offsets::PlayerState, sizeof(ActorPlayerState), localPlayerState);

            int optimizedCount = actorCount > 400 ? actorCount - 120 : actorCount / 2; // We do this because im dumb and havent figured out a better way to optimize
            for (int i = optimizedCount; i < actorCount; i++)
            {

                uintptr_t actor;
                driver.ReadMemory<uintptr_t>(actorList + (i * 0x8), sizeof(uintptr_t), actor);
                if (actor == Cheat::localAcknowledgedPawn)
                {
                    ActorRootStruct localActorRootStruct;
                    driver.ReadMemory<ActorRootStruct>(Cheat::localRootComponent, sizeof(ActorRootStruct), localActorRootStruct);
                    localActorLocation = localActorRootStruct.RelativeLocation;

                    continue;
                }
      
                ActorStruct actorStruct;
                driver.ReadMemory<ActorStruct>(actor, sizeof(ActorStruct), actorStruct);
                if (actorStruct.actorRoot == 0)
                    continue;

         
                ActorRootStruct actorRootStruct;
                driver.ReadMemory<ActorRootStruct>(actorStruct.actorRoot, sizeof(ActorRootStruct), actorRootStruct);
                FBoxSphereBounds actorBounds = actorRootStruct.actorBounds;
                if (actorRootStruct.actorBounds.SphereRadius > 100 || actorRootStruct.actorBounds.SphereRadius < -100 || actorRootStruct.actorBounds.SphereRadius == 0)
                    continue;

                uintptr_t actorRoot = actorStruct.actorRoot;
                if (!actorRoot)
                    continue;

                std::string actorName;
                actorName = driver.GetNameFromId(actorStruct.ActorID);

                if (actorName.find("Character") == std::string::npos) continue;

                uintptr_t playerState;
                driver.ReadMemory<uintptr_t>(actor + Offsets::PlayerState, sizeof(uintptr_t), playerState);
            
                ActorPlayerState fixedPlayerState;
                driver.ReadMemory<ActorPlayerState>(playerState, sizeof(ActorPlayerState), fixedPlayerState);
                
                wchar_t* playerName = new wchar_t[200];
                driver.ReadMemory(fixedPlayerState.playerNamePtr, sizeof(wchar_t)*200, *playerName);

             /*   if (fixedPlayerState.teamId != 7)
                    continue;
                std::wcout << "PlayerName" << playerName << std::endl;

                std::cout << "pstate :    " << playerState << std::endl;*/

               // std::cout << "Me" << std::hex << localPlayerState << std::endl;

                //std::cout << "Allusive" << std::hex << playerState << std::endl;
                /*    if (playerState.r_Team == localPlayerState.r_Team)
                    continue;*/
               
                //static int characterId = -1;
                //if (characterId == -1)
                //{
                //    actorName = driver.GetNameFromId(actorStruct.ActorID);
                //    if (actorName.find("Character") == std::string::npos) continue;
                //        characterId = actorStruct.ActorID;
                //}

                //if (characterId != actorStruct.ActorID) continue;
                //std::cout << actorName << "  " << 0 << std::endl;

                //driver.ReadMemory<FBoxSphereBounds>(actorRoot + Offsets::ActorBounds, sizeof(FBoxSphereBounds), actorBounds);

                //std::cout << actorBounds.Origin.x << " " << actorBounds.Origin.y << " " << actorBounds.Origin.z << " " << std::endl;
                /*      uintptr_t cameraComponent;
                      driver.ReadMemory<uintptr_t>(Cheat::playerCamera + Offsets::CameraComponent, sizeof(uintptr_t), cameraComponent);
                      if (!cameraComponent)
                          continue;*/
                /*           
                driver.ReadMemory<ActorPlayerState>(actor + Offsets::PlayerState, sizeof(ActorPlayerState), playerState);

                AKSTeamState teamState;
                driver.ReadMemory<AKSTeamState>(playerState.r_Team, sizeof(AKSTeamState), teamState);
                
                if (teamState.r_TeamNum == localTeam)
                    continue;*/
                
                FCameraCacheEntry cameraCache;
                driver.ReadMemory<FCameraCacheEntry>(Cheat::playerCamera + Offsets::CameraCache, sizeof(FCameraCacheEntry), cameraCache);

                Cheat::fov = cameraCache.POV.FOV * 1.638;//;
                auto cameraRotation = cameraCache.POV.Rotation;
                auto cameraLocation = cameraCache.POV.Location;

                Cheat::CameraLocation = cameraLocation;
                Cheat::CameraRotation = cameraRotation;

                Vector3 actorLocation = actorRootStruct.RelativeLocation;
                std::vector<Vector2> lines = Render3DBox(actorBounds.Origin, actorBounds.BoxExtent, cameraRotation, cameraLocation);
                PlayerInfo player;

                uintptr_t actorMesh;
                driver.ReadMemory<uintptr_t>(actor + Offsets::Mesh, sizeof(uintptr_t), actorMesh);
                auto headPosition = GetBoneWithRotation(actorMesh, 165, driver);

                player.name = actorName;
                player.Location = headPosition;//actorBounds.Origin;
                player.ActorID = i;

                for (int z = 0; z < lines.size(); z++)
                {
                    auto line = lines[z];
                    Vector2 buff;
                    buff.x = line.x;
                    buff.y = line.y;
                    player.Bounds.push_back(buff);
                }
                playersBuffer.push_back(player);
            }
            if (GetAsyncKeyState(VK_XBUTTON2) & 0x8001)
            {
                if (!Cheat::closestPlayerId)
                    Cheat::closestPlayerId = GetClosestLOSPlayer(true, Cheat::CameraLocation, Cheat::CameraRotation);

                for (int v = 0; v < playersBuffer.size(); v++)
                {
                    auto player = playersBuffer[v];
                    if (player.ActorID == Cheat::closestPlayerId)
                    {
                        Vector3 point;
                        point.x = player.Bounds[0].x;
                        point.y = player.Bounds[0].y;
                        auto location = player.Location;
                        location.z -= 10;
                        auto aimRotation = FindLookAtRotation(Cheat::CameraLocation, location);

                        if (!driver.WriteMemory(Cheat::localPlayerController + 0x0288, aimRotation, sizeof(Vector3)))
                            std::cout << "Write Failed" << std::endl;

                    }
                }
            }
            else
                Cheat::closestPlayerId = 0;

        }
        NoStam::bufferMutex.lock();
        NoStam::players = playersBuffer;
        NoStam::bufferMutex.unlock();
    }
}
void InitMenu()
{
    Menu menu{};
    menu.InitializeMenu();
}

int main() {
   
    Driver driver = Driver();
    const wchar_t* name = L"RogueCompany.exe";

    Print("Driver Handle: " + std::to_string((long)driver.handle));
    if (driver.IsLoaded())
        Print("Connected!");
    else
    {
        Print("Failed to connect to the driver (is it loaded?)! Error Code:" + std::to_string(GetLastError()) + "\n", true);
        return 0;
    }
    if (driver.GetProcId(name))
        Print("Process ID: " + std::to_string(driver.procId));
    else
    {
        Print("Failed to find the process!");
        return 0;
    }
    if (driver.GetModuleBase(name))
        std::wcout << "[+] BaseName: " << name << "\n" << "[+] ModuleBase: " << std::hex << driver.moduleBase << std::endl;
    else
        Print("Failed to get Module Base!", true);

    std::thread menuThread(InitMenu);

    GetDefaults(driver);

    HackLoop(driver);

    return 0;
}


