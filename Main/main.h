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


#define SCREEN_WIDTH	(1280)
#define SCREEN_HEIGHT	(720)

//=============ÉIÉyÉåÅ[É^Å[=================
//// ë´ÇµéZ
//XMFLOAT3 operator+(const XMFLOAT3& a, const XMFLOAT3& b) noexcept {
//    return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
//}
//
//// å∏éZ
//XMFLOAT3 operator-(const XMFLOAT3& a, const XMFLOAT3& b) noexcept {
//    return XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
//}
//
//// ä|ÇØéZ
//XMFLOAT3 operator*(const XMFLOAT3& v, float scalar) noexcept {
//    return XMFLOAT3(v.x * scalar, v.y * scalar, v.z * scalar);
//}
//
//// äÑéZ
//XMFLOAT3 operator/(const XMFLOAT3& v, float scalar) {
//    if (scalar == 0) return { 0,0,0 };
//    return XMFLOAT3(v.x / scalar, v.y / scalar, v.z / scalar);
//}
//===============================

HWND GetWindow();
//void Invoke(std::function<void()> Function, int Time);

