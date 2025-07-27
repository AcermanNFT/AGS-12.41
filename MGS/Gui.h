#pragma once

//#include "./kiero/kiero.h"
#include "./Imgui/imgui.h"
//#include "./Imgui/imgui_impl_dx11.h"
#include "./Imgui/imgui_impl_dx9.h"
#include "./Imgui/imgui_impl_win32.h"
#include <d3d9.h>
#include <wchar.h>
#include <tchar.h>

#pragma comment(lib, "D3d9.lib") // fixes a error for some reason

// Data
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


void InitImGui() noexcept
{
    ImGui::CreateContext();
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);

    colors[ImGuiCol_Border] = ImColor(0.05f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImColor(0.05f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);

    colors[ImGuiCol_CheckMark] = ImColor(51, 214, 255, 255);
    colors[ImGuiCol_SliderGrab] = ImColor(51, 214, 255, 255);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Separator] = ImColor(51, 214, 255, 255);
    colors[ImGuiCol_SeparatorHovered] = ImColor(51, 214, 255, 255);
    colors[ImGuiCol_SeparatorActive] = ImColor(51, 214, 255, 255);

    colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_TabActive] = ImColor(0.05f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);

    style.WindowBorderSize = 3;
    style.ChildBorderSize = 2;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 2;

    style.WindowRounding = 3;
    style.FrameRounding = 3;
    style.TabRounding = 3;
    style.GrabRounding = 3;

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
}

// Main code
// COPIED FROM LUNA IMGUI
DWORD WINAPI ImGuiThread(LPVOID)
{
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Luna", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Andreu GameServer", WS_OVERLAPPEDWINDOW, 100, 100, 700, 700, nullptr, nullptr, wc.hInstance, nullptr);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    InitImGui();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool done = false;

    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Andreu GameServer 12.41");
        static int tab = 0;
        static bool ShowPlayerDetails = false;

        if (ImGui::BeginTabBar("Tabs"))
        {
            if (IsLoaded)
            {
                if (ImGui::BeginTabItem("Server"))
                {
                    ImGui::InputText("Playlist Name", PlaylistBuffer, 1024);
                    ImGui::Checkbox("LateGame", &bLateGame);

                    if (ImGui::Button("Execute console command"))
                    {
                        static std::string ConsoleCommand;
                        auto wstr = std::wstring(ConsoleCommand.begin(), ConsoleCommand.end());

                        auto aa = wstr.c_str();
                        FString cmd = aa;

                        UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), cmd, nullptr);
                    }

                    ImGui::SliderFloat("Duration", &Duration, 0.0f, 100.0f, "%.1f");
                    ImGui::Checkbox("Siphon", &bEnableSiphon);

                    if (ImGui::Button("Start Safe Zone"))
                        UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"startsafezone", nullptr);
                    if (ImGui::Button("Pause Safe Zone"))
                        UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"pausesafezone", nullptr);
                    if (ImGui::Button("Skip Zone"))
                        UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"skipsafezone", nullptr);
                    if (ImGui::Button("Start Shrink Safe Zone"))
                        UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"startshrinksafezone", nullptr);

                    if (ImGui::Button("Start Aircraft"))
                    {
                        GetGameState()->GamePhase = EAthenaGamePhase::Warmup;
                        GetGameState()->OnRep_GamePhase(EAthenaGamePhase::None);
                        float now = ((UGameplayStatics*)UGameplayStatics::StaticClass()->DefaultObject)->GetTimeSeconds(UWorld::GetWorld());
                        GetGameState()->WarmupCountdownEndTime = now + 11;
                        GetGameState()->AircraftStartTime = now + 11;
                    }

                    if (ImGui::Button("Start Event"))
                    {
                        UObject* JerkyLoader = UObject::FindObject<UObject>("BP_Jerky_Loader_C JerkyLoaderLevel.JerkyLoaderLevel.PersistentLevel.BP_Jerky_Loader_2");

                        UFunction* StartEventFunc = StaticFindObject<UFunction>("/CycloneJerky/Gameplay/BP_Jerky_Loader.BP_Jerky_Loader_C.StartEvent");
                        float idk = 0;
                        ProcessEventOG(JerkyLoader, StartEventFunc, &idk);
                    }

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Players"))
                {
                    auto GameState = reinterpret_cast<AFortGameStateAthena*>(UWorld::GetWorld()->GameState);
                    if (GameState)
                    {
                        static int selectedPlayerIndex = -1;
                        auto PlayerArray = GameState->PlayerArray;

                        ImGui::Text("Max Players = %d", GetGameState()->CurrentPlaylistInfo.BasePlaylist->MaxPlayers);
                        ImGui::BeginListBox("Players", ImVec2(-1, 300));
                        for (int i = 0; i < PlayerArray.Num(); ++i)
                        {
                            if (PlayerArray[i]->bIsABot) continue;
                            bool isSelected = selectedPlayerIndex == i;
                            if (ImGui::Selectable(PlayerArray[i]->GetPlayerName().ToString().c_str(), isSelected))
                                selectedPlayerIndex = i;
                        }
                        ImGui::EndListBox();

                        if (selectedPlayerIndex != -1 && ImGui::Button("View Details"))
                            ShowPlayerDetails = true;

                        if (ShowPlayerDetails)
                        {
                            auto PlayerInfo = PlayerArray[selectedPlayerIndex];
                            ImGui::Begin("Player Details");
                            ImGui::Text("Username: %s", PlayerInfo->PlayerNamePrivate.ToString().c_str());
                            ImGui::Text("Ping: %s", PlayerInfo->Ping);
                            ImGui::End();
                        }
                    }
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Settings"))
                {
                    ImGui::Checkbox("LateGame", &bLateGame);
                    ImGui::Checkbox("Arsenal", &bArsenal);
                    ImGui::Checkbox("Siphon", &bEnableSiphon);
                    ImGui::EndTabItem();
                }
            }
            else
            {
                ImGui::Text("Server is still loading <3");
            }

            ImGui::EndTabBar();
        }

        ImGui::End();

        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}




bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

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
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
