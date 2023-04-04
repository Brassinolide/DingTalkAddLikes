#include <iostream>
#include <d3d9.h>
#include <vector>
#include <TlHelp32.h>
#include <WinInet.h>

#include "imgui\imgui.h"
#include "imgui\imgui_impl_dx9.h"
#include "imgui\imgui_impl_win32.h"

#pragma comment(lib, "wininet.lib")

#pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup")

using namespace std;

static LPDIRECT3D9 g_pD3D = NULL;
static LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS g_d3dpp = {};
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

vector<unsigned __int64> __ResultArray;
char uuid[50];

#define BLOCKMAXSIZE 409600
BYTE * MemoryData;
short Next[260];
WORD GetTzmArray(char* Tzm, WORD* TzmArray) {
    int len = 0;
    WORD TzmLength = strlen(Tzm) / 3 + 1;
    for (int i = 0; i < strlen(Tzm); ) {
        char num[2];
        num[0] = Tzm[i++];
        num[1] = Tzm[i++];
        i++;
        if (num[0] != '?' && num[1] != '?') {
            int sum = 0;
            WORD a[2];
            for (int i = 0; i < 2; i++) {
                if (num[i] >= '0' && num[i] <= '9') {
                    a[i] = num[i] - '0';
                }
                else if (num[i] >= 'a' && num[i] <= 'z') {
                    a[i] = num[i] - 87;
                }
                else if (num[i] >= 'A' && num[i] <= 'Z') {
                    a[i] = num[i] - 55;
                }

            }
            sum = a[0] * 16 + a[1];
            TzmArray[len++] = sum;
        }
        else {
            TzmArray[len++] = 256;
        }
    }
    return TzmLength;
}

void GetNext(short* next, WORD* Tzm, WORD TzmLength) {
    for (int i = 0; i < 260; i++)
        next[i] = -1;
    for (int i = 0; i < TzmLength; i++)
        next[Tzm[i]] = i;
}

void SearchMemoryBlock(HANDLE hProcess, WORD* Tzm, WORD TzmLength, unsigned __int64 StartAddress, unsigned long size, vector<unsigned __int64>& ResultArray) {
    if (!ReadProcessMemory(hProcess, (LPCVOID)StartAddress, MemoryData, size, NULL)) {
        return;
    }
    for (int i = 0, j, k; i < size;) {
        j = i; k = 0;
        for (; k < TzmLength && j < size && (Tzm[k] == MemoryData[j] || Tzm[k] == 256); k++, j++);
        if (k == TzmLength) {
            ResultArray.push_back(StartAddress + i);
        }
        if ((i + TzmLength) >= size) {
            return;
        }
        int num = Next[MemoryData[i + TzmLength]];
        if (num == -1)
            i += (TzmLength - Next[256]);
        else
            i += (TzmLength - num);
    }
}

int SearchMemory(HANDLE hProcess, char* Tzm, unsigned __int64 StartAddress, unsigned __int64 EndAddress, int InitSize, vector<unsigned __int64>& ResultArray) {
    int i = 0;
    unsigned long BlockSize;
    MEMORY_BASIC_INFORMATION mbi;
    WORD TzmLength = strlen(Tzm) / 3 + 1;
    WORD* TzmArray = new WORD[TzmLength];
    GetTzmArray(Tzm, TzmArray);
    GetNext(Next, TzmArray, TzmLength);
    ResultArray.clear();
    ResultArray.reserve(InitSize);
    while (VirtualQueryEx(hProcess, (LPCVOID)StartAddress, &mbi, sizeof(mbi)) != 0) {
        if (mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_EXECUTE_READWRITE) {
            i = 0;
            BlockSize = mbi.RegionSize;
            while (BlockSize >= BLOCKMAXSIZE) {
                SearchMemoryBlock(hProcess, TzmArray, TzmLength, StartAddress + (BLOCKMAXSIZE * i), BLOCKMAXSIZE, ResultArray);
                BlockSize -= BLOCKMAXSIZE; i++;
            }
            SearchMemoryBlock(hProcess, TzmArray, TzmLength, StartAddress + (BLOCKMAXSIZE * i), BlockSize, ResultArray);
        }
        StartAddress += mbi.RegionSize;
        if (EndAddress != 0 && StartAddress > EndAddress) {
            return ResultArray.size();
        }
    }
    free(TzmArray);
    return ResultArray.size();
}

void http(const char* URL) {
    HINTERNET hIntSession = InternetOpenA("Mozilla/5.0", 1, 0, 0, 0);
    HINTERNET hHttpSession = InternetOpenUrlA(hIntSession, URL, NULL, NULL, NULL, NULL);
    HINTERNET hHttpRequest = HttpOpenRequestA(hHttpSession, "GET", URL, 0, 0, 0, INTERNET_FLAG_RELOAD, 0);
    const char* szHeaders = ("");
    char szRequest[1024] = { 0 };
    HttpSendRequestA(hHttpRequest, szHeaders, strlen(szHeaders), szRequest, strlen(szRequest));
    InternetCloseHandle(hIntSession);
    InternetCloseHandle(hHttpSession);
    InternetCloseHandle(hHttpRequest);
}

HANDLE hThread = 0;
static int d = 100;
int counter = 0;
static char* title = (char*)u8"开刷！！！";
static bool b = false;
void Thread() {
    string uuid_tmp = uuid;
    string url = "https://lv.dingtalk.com/interaction/createLike?uuid=" + uuid_tmp + "&count=100";
    while (1) {
        http(url.c_str());
        counter = counter + 1;
        if (counter >= d) {
            title = (char*)u8"开刷！！！";
            counter = 0;
            b = false;
            return;
        }
        Sleep(300);
    }
}

void runAgain() {
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    WinExec(szPath,SW_SHOW);
    exit(0);
}

int main() {

    DWORD PID;
    GetWindowThreadProcessId(FindWindow("StandardFrame", "钉钉"), &PID);
    HANDLE hDing = OpenProcess(PROCESS_ALL_ACCESS, 0, PID);
    if (hDing) {
        MemoryData = new BYTE[BLOCKMAXSIZE];
        SearchMemory(hDing, (char*)"68 74 74 70 3A 2F 2F 64 74 6C 69 76 65 2D ?? ?? 2E 64 69 6E 67 74 61 6C 6B 2E 63 6F 6D 2F 6C 69 76 65 5F 68 70 2F ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 2E 66 6C 76 3F 61 75 74 68 5F 6B 65 79 3D", 0x0CAAAAAA, 0x5FFFFFFF, 30, __ResultArray);
        delete[] MemoryData;
        if (__ResultArray.size() == 0) {
            MessageBox(0, "Grabber Failed", "错误", 0);
            return 0;
        }
        else {
            int address = (*__ResultArray.begin()) + 38;
            ReadProcessMemory(hDing, (void*)address, uuid, 36, 0);
        }
    }
    else {
        MessageBox(0,"初始化失败，请先进入直播间再启动此程序", "错误", 0);
        return 0;
    }

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "wtf", NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, "钉钉刷赞器", WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, 100, 100, 520, 225, NULL, NULL, wc.hInstance, NULL);
    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = 0; 
    io.LogFilename = 0;
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc", 18.0f, 0, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool done = false;
    while (!done) {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("钉钉刷赞器", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);
            ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

            ImGui::PushFont(font);
            ImGui::Text(u8"直播间uuid(自动获取):");
            ImGui::SameLine();
            ImGui::Text(uuid);
            ImGui::SameLine();
            if (ImGui::Button(u8"复制")) {
                ImGui::SetClipboardText(uuid);
            }
            ImGui::Text(u8"数量:");
            ImGui::SameLine();
            
            ImGui::InputInt(u8" x 100个赞", &d);
            if (ImGui::Button(title,ImVec2(100,50))) {
                b = !b;
                if (b) {
                    hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Thread, 0, 0, 0);
                    title = (char*)u8"停止";
                }
                else {
                    if (!TerminateThread(hThread, 0)) {
                        exit(0);
                    }
                    title = (char*)u8"开刷！！！";
                }
            }

            ImGui::Text(u8"每次直播结束后请重启钉钉和此刷赞器");
            ImGui::SameLine();
            if (ImGui::Button(u8"重启刷赞器")) {
                runAgain();
            }

            ImGui::PopFont();
            ImGui::End();
        }

        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}

bool CreateDeviceD3D(HWND hWnd) {
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;
    return true;
}

void CleanupDeviceD3D() {
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    switch (msg) {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
