#pragma once

#include <windows.h>
#include <vector>
#include <array>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

/* D3D9 */
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

/* ImGui */
#include "menu/ImGui/imgui.h"
#include "menu/ImGui/imgui_internal.h"
#include "menu/ImGui/impl/imgui_impl_dx9.h"
#include "menu/ImGui/impl/imgui_impl_win32.h"

inline const std::array< int , 2 > WINDOW_SIZE { 500, 300 };
static std::array< int , 2 > WINDOW_POS { 0, 0 };

static HWND g_pWindow = NULL;
static LPDIRECT3D9 g_pD3D = NULL;
static LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS g_d3dpp = {};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam );