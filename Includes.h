#pragma once
#include <Windows.h>
#include <iostream>
#include <psapi.h>
#include <tlhelp32.h>
#include <vector>
#include <format>
#include <stdio.h>
#include <cstdint>
#include <sstream>
#pragma comment(lib, "winmm.lib")

#include <d3d11.h>
#include <D3Dcompiler.h>
#pragma comment(lib, "D3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "winmm.lib")

#include "Core/ImGui/imgui.h"
#include "Core/ImGui/imgui_impl_win32.h"
#include "Core/ImGui/imgui_internal.h"
#include "Core/ImGui/imgui_impl_dx11.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "Detours/Include/detours.h"
#pragma comment (lib, "detours.lib")
#pragma comment(lib, "winmm.lib")

#include "Core/Hooks/Hooks.h"