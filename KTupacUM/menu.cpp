#include "menu.h"

std::vector<PlayerInfo> NoStam::players;
std::mutex NoStam::bufferMutex;

static ID3D11Device* g_pd3dDevice;
static ID3D11DeviceContext* g_pd3dDeviceContext;
static IDXGISwapChain* g_pSwapChain;
static UINT g_ResizeWidth, g_ResizeHeight;
static ID3D11RenderTargetView* g_mainRenderTargetView;
static ID3D11BlendState* g_pBlendState;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef BOOL(WINAPI* pfnSetWindowCompositionAttribute)(HWND, void*);

void Menu::ChangeClickability(bool canClick, HWND hwnd) {
    long style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

    if (canClick) {
        style |= WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW;
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, style);
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        SetForegroundWindow(hwnd);
    }
    else {
        style &= ~WS_EX_LAYERED;
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, style | WS_POPUP | WS_EX_TOOLWINDOW);
        SetForegroundWindow(hwnd);

    }
}
void rainbowGradient(int index, Vector3& colors) {
    float frequency = 0.02;  // Controls the frequency of color change

    // Calculate RGB values using sine function
    colors.x = static_cast<int>(sin(frequency * index + 0) * 127 + 128);
    colors.y = static_cast<int>(sin(frequency * index + 2 * 3.1415 / 3) * 127 + 128);
    colors.z = static_cast<int>(sin(frequency * index + 4 * 3.1415 / 3) * 127 + 128);
}
int Menu::InitializeMenu()
{
    WNDCLASSEX wc{ };
    wc.cbSize = 80;
    wc.style = 64;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hIcon = nullptr;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = L"Razer";
    wc.hIconSm = nullptr;
    const ATOM register_class_atom = RegisterClassEx(&wc);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Razer", 0xC0000000 | WS_EX_TOPMOST, 0, 0, screenWidth, screenHeight, nullptr, nullptr, wc.hInstance, nullptr);

    auto windowHwnd = hwnd;

    MARGINS p_mar_inset = { -1 };
    if (!SetProcessDPIAware()) {
        std::cerr << "Failed to set DPI awareness!" << std::endl;
        return 1;
    }

    if (DwmExtendFrameIntoClientArea(hwnd, &p_mar_inset))
    {
        std::cerr << "Failed DwmExtendFrameIntoClientArea. | GetLastError: " << GetLastError() << '\n';
        return -1;
    }

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    //TODO HOOK AND RETURN 0 JUST IN CASE
    DWORD affin;
    //GetWindowDisplayAffinity(hwnd, &affin);
    //std::cout << affin << std::endl;

    //SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE);

    //GetWindowDisplayAffinity(hwnd, &affin);
    //std::cout << "Affin: " << affin << std::endl;
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);



    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


    ImGuiStyle* style = &ImGui::GetStyle();

    style->WindowPadding = ImVec2(15, 15);
    style->WindowRounding = 5.0f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 4.0f;
    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 3.0f;

    style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    //style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    //style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.25f, 0.25f, 0.80f);  // Bright Red

    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    //style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    //style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    //style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    //style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
    //style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
   // style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
    //style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
    //ImGui::StyleColorsLight();

    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //io.Fonts->AddFontFromFileTTF("D:\\Backups2023Dec\\BackupsReposDec2023\\RazerRewrite\\x64\\Release\\NovaSquare-Regular.ttf", 16.0f);
    // ImGui::PushFont();
     // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.00f);

    // Main loop
    bool done = false;
    bool canClick = false;

    int index = 0;
    while (!done)
    {
        //std::cout << canClick << std::endl;

        if (GetAsyncKeyState(VK_INSERT) & 0x8001) {

            Sleep(100);
            if (!((GetAsyncKeyState(VK_INSERT) & 0x8000)))
            {
                std::cout << "Clicked" << std::endl;
                canClick = !canClick; // Toggle canClick
                ChangeClickability(canClick, hwnd);
            }

        }
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (!canClick) // Show or hide window
        {
            if (show_demo_window)
                ImGui::ShowDemoWindow(&show_demo_window);

            static float f = 0.0f;
            static int counter = 0;
            ImGui::SetNextWindowSize(ImVec2(500, 700), ImGuiCond_FirstUseEver);
            ImGui::Begin("Dark NoStam");

            if (ImGui::BeginTabBar("MyTabs"))
            {
                if (ImGui::BeginTabItem("GENERAL"))
                {
                    //ImGui::Checkbox("Demo Window", &show_demo_window);     
                    //ImGui::Checkbox("Another Window", &show_another_window);

                    //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                    //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

               /*     if (ImGui::Button("Save Config"))
                        SaveConfig(configData, "r6.dat");
                    ImGui::SameLine();
                    if (ImGui::Button("Load Config"))
                        LoadConfig(configData, "r6.dat");*/

                        //ImGui::Text("counter = %d", counter);

                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("ESP"))
                {

                    // Mob ESP Checkbox and Color Picker


                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("WEAPON"))
                {
                    // Contents of Tab 2
                    ImGui::Text("COMING SOON");
                    ImGui::EndTabItem();
                }

                //if (ImGui::BeginTabItem("Tab 3"))
                //{
                //    // Contents of Tab 3
                //    ImGui::Text("Tab 3 Contents");
                //    ImGui::EndTabItem();
                //}

                ImGui::EndTabBar();
            }
            ImGui::End();
            if (show_another_window)
            {
                ImGui::Begin("Another Window", &show_another_window);   
                ImGui::Text("Hello from another window!");
                if (ImGui::Button("Close Me"))
                    show_another_window = false;
                ImGui::End();
            }
        } 

        NoStam::bufferMutex.lock();
        std::vector<PlayerInfo> playersBuffer = NoStam::players;
        NoStam::bufferMutex.unlock();

      

        Vector3 colors = Vector3();
        rainbowGradient(index, colors);
        index++;
        if (index >= 255)
            index = 0;
        for (int i = 0; i < playersBuffer.size(); i++) { 


            auto playerBounds = playersBuffer[i].Bounds;

          /*  auto bonePositions = playersBuffer[i].BonePositions;
            for(int l = 0; l < bonePositions.size(); l ++)
                ImGui::GetBackgroundDrawList()->AddText(ImVec2(bonePositions[l].x, bonePositions[l].y), IM_COL32(255, 255, 255, 255), std::to_string(l).c_str());*/

           

            ImVec2 bottomRight = ImVec2(playerBounds[0].x, playerBounds[0].y);
            ImVec2 bottomLeft = ImVec2(playerBounds[2].x, playerBounds[2].y);
            ImVec2 topLeft = ImVec2(playerBounds[1].x, playerBounds[1].y);
            ImVec2 topRight = ImVec2(playerBounds[7].x, playerBounds[7].y);

            ImVec2 backBottomRight = ImVec2(playerBounds[6].x, playerBounds[6].y);
            ImVec2 backBottomLeft = ImVec2(playerBounds[4].x, playerBounds[4].y);
            ImVec2 backTopLeft = ImVec2(playerBounds[3].x, playerBounds[3].y);
            ImVec2 backTopRight = ImVec2(playerBounds[5].x, playerBounds[5].y);

            ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y/2),150.0f, IM_COL32(255, 255, 255, 255), 100, 1.0f);

            //ImGui::GetBackgroundDrawList()->AddText(topLeft, IM_COL32(255, 255, 255, 255), std::to_string(playersBuffer[i].ActorID).c_str());
            ImGui::GetBackgroundDrawList()->AddLine(topLeft, topRight, IM_COL32(colors.x, colors.y, colors.z, 255), 2);
            ImGui::GetBackgroundDrawList()->AddLine(topRight, bottomRight, IM_COL32(colors.x, colors.y, colors.z, 255), 2);
            ImGui::GetBackgroundDrawList()->AddLine(bottomRight, bottomLeft, IM_COL32(colors.x, colors.y, colors.z, 255), 2);
            ImGui::GetBackgroundDrawList()->AddLine(bottomLeft, topLeft, IM_COL32(colors.x, colors.y, colors.z, 255), 2);
            ImGui::GetBackgroundDrawList()->AddLine(topLeft, backTopLeft, IM_COL32(colors.x, colors.y, colors.z, 255), 2);
            ImGui::GetBackgroundDrawList()->AddLine(topRight, backTopRight, IM_COL32(colors.x, colors.y, colors.z, 255), 2);

            ImGui::GetBackgroundDrawList()->AddLine(bottomRight, backBottomRight, IM_COL32(colors.x, colors.y, colors.z, 255), 2);
            ImGui::GetBackgroundDrawList()->AddLine(bottomLeft, backBottomLeft, IM_COL32(colors.x, colors.y, colors.z, 255), 2);
            ImGui::GetBackgroundDrawList()->AddLine(backTopRight, backBottomRight, IM_COL32(colors.x, colors.y, colors.z, 255), 2);
            ImGui::GetBackgroundDrawList()->AddLine(backTopLeft, backBottomLeft, IM_COL32(colors.x, colors.y, colors.z, 255), 2);
            ImGui::GetBackgroundDrawList()->AddLine(backBottomLeft, backBottomRight, IM_COL32(colors.x, colors.y, colors.z, 255), 2);
            ImGui::GetBackgroundDrawList()->AddLine(backTopRight, backTopLeft, IM_COL32(colors.x, colors.y, colors.z, 255), 2);

        }
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        float clearColor[4] = { 0.0f,0.0f,0.0f,0 };

        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clearColor);
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool Menu::CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC  sd{ };
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferCount = 2;
    sd.Flags = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 120; // 60 fps
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = 32;
    sd.OutputWindow = hWnd;
    sd.SampleDesc = DXGI_SAMPLE_DESC{ 1, 0 };
    sd.Windowed = true;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void Menu::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void Menu::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void Menu::CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

