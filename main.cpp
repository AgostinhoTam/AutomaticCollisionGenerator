/*===================================================================================

メイン関数(main.cpp)

====================================================================================*/
#include "main.h"
#include <thread>
#include "Manager/sceneManager.h"
#include "Manager/debuggerImGuiManager.h"
const char* CLASS_NAME = "AppClass";
const char* WINDOW_NAME = "AutoCollision";

constexpr float FPS = 60.0f;
constexpr float FRAME_PER_SEC = 1 / FPS;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


HWND g_Window;

//	=========================ウインドウズ取得=========================
HWND GetWindow()
{
	return g_Window;
}

//	=========================メイン処理=========================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	//	ウインドウズ初期設定
	WNDCLASSEX wcex;
	{
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = 0;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = nullptr;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = nullptr;
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = CLASS_NAME;
		wcex.hIconSm = nullptr;

		RegisterClassEx(&wcex);


		RECT rc = { 0, 0, (LONG)SCREEN_WIDTH, (LONG)SCREEN_HEIGHT };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	
		g_Window = CreateWindowEx(0, CLASS_NAME,WINDOW_NAME, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
	}

	(void)CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	
	ShowWindow(g_Window, nCmdShow);
	UpdateWindow(g_Window);
	
	DWORD dwExecLastTime;
	DWORD dwCurrentTime;
	timeBeginPeriod(1);
	dwExecLastTime = timeGetTime();
	dwCurrentTime = 0;

	//	シーン作成
	SceneManager* sceneManager = SceneManager::GetInstance();
	MSG msg;

	//	作成失敗したら処理
	if (!sceneManager)
	{
		assert("Scene Manager Failed");
		return 0;
	}
	//	シーンマネージャー初期化
	sceneManager->Init();

	//	デバッグモードオン
	sceneManager->SetIsDebugMode(true);

	//	ゲームループ開始
	while(true)
	{
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();
			float deltaTime = static_cast<float>((dwCurrentTime - dwExecLastTime) / 1000.0f);
			if((deltaTime) >= FRAME_PER_SEC)
			{
				dwExecLastTime = dwCurrentTime;
				if (!sceneManager)return (int)msg.wParam;
				//	シーン更新
				sceneManager->Update(deltaTime);
				//	シーン描画
				sceneManager->Draw();

			}
		}
	}
	//	シーンUninit
	sceneManager->Uninit();
	timeEndPeriod(1);

	UnregisterClass(CLASS_NAME, wcex.hInstance);


	CoUninitialize();

	return (int)msg.wParam;
}


extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM IParam);

//	=========================ウインドウズコールバック関数==========================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

{	//	もしImGuiのUIにクリックしたら、処理はImGuiに向く
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))return true;

	switch(uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
