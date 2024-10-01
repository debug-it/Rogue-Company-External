#pragma once
#include "includes.h"
#include <dwmapi.h>
#include <dxgi.h>
#include <d3d11.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#pragma comment (lib, "Dwmapi")
#pragma comment (lib, "d3d11.lib")

class NoStam
{
public:
	static std::vector<PlayerInfo> players;
	static std::mutex bufferMutex;
};
class Menu
{
private:
	void CleanupRenderTarget();
	void CreateRenderTarget();
	void CleanupDeviceD3D();
	bool CreateDeviceD3D(HWND hWnd);
	void ChangeClickability(bool canClick, HWND hwnd);
public:
	int InitializeMenu();
};