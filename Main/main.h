#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define NOMINMAX
#include <windows.h>
#include <assert.h>
#include <functional>

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")


#include <DirectXMath.h>
using namespace DirectX;

#include "External/DirectXTex.h"

#if _DEBUG
#pragma comment(lib,"DirectXTex_Debug.lib")
#else
#pragma comment(lib,"DirectXTex_Release.lib")
#endif

#pragma comment (lib, "winmm.lib")


#include "ImGui\\imgui.h"
#include "ImGui\\imgui_impl_win32.h"
#include "ImGui\\imgui_impl_dx11.h"

#define SCREEN_WIDTH	(1280)
#define SCREEN_HEIGHT	(720)


HWND GetWindow();


