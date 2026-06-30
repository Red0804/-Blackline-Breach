//! @file window.cpp
//! @brief 긽귽깛긂귻깛긤긂궻띿맟갋맕뚥

//--------------------------------------------------------------------------------
// 
// OpenXOPS
// Copyright (c) 2014-2023, OpenXOPS Project / [-_-;](mikan) All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// * Neither the name of the OpenXOPS Project nor the names of its contributors 
//   may be used to endorse or promote products derived from this software 
//   without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL OpenXOPS Project BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//--------------------------------------------------------------------------------

#include "window.h"

extern InputControl inputCtrl;

//! @brief 긓깛긚긣깋긏?
WindowControl::WindowControl()
{
	hInstance = 0;
	nCmdShow = 0;
	hWnd = NULL;

	timeBeginPeriod(1);		//timeEndPeriod궴뫮
}

//! @brief 긢귻긚긣깋긏?
WindowControl::~WindowControl()
{
	timeEndPeriod(1);		//timeBeginPeriod궴뫮
}

//! @brief 귺긵깏긑?긘깈깛궻륃뺪귩먠믦
//! @param in_hInstance 귽깛긚?깛긚 긪깛긤깑
//! @param in_nCmdShow 긂귻깛긤긂궻?렑륉뫴
void WindowControl::SetParam(HINSTANCE in_hInstance, int in_nCmdShow)
{
	hInstance = in_hInstance;
	nCmdShow = in_nCmdShow;
}

//! @brief 긽귽깛긂귻깛긤긂띿맟
//! @param title 긂귻깛긤긂?귽긣깑
//! @param width 긂귻깛긤긂궻븴
//! @param height 긂귻깛긤긂궻뛼궠
//! @param fullscreen false갌긂귻깛긤긂?렑?true갌긲깑긚긏깏?깛뾭?렑
//! @return 맟뚻갌true?렪봲갌false
//! @warning 먩궸SetParam()듫릶궳먠믦궢궲궓궘뷠뾴궕궇귟귏궥갃
bool WindowControl::InitWindow(const char* title, int width, int height, bool fullscreen)
{
	if( title == NULL ){ return false; }
	if( (width <= 0)||(height <= 0) ){ return false; }

	WNDCLASS wc;
	int x, y;
	RECT Rect;
	DWORD dwStyle;

	//긂귻깛긤긂긏깋긚궻뱋?
	wc.style		= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	= WindowProc;
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.hInstance	= hInstance;
	wc.hIcon		= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(MAINICON));
	wc.hCursor		= NULL;
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName	= NULL;
	wc.lpszClassName	= "MainWindow";
	if( RegisterClass(&wc) == 0 ){
		return false;
	}

	if (fullscreen == false) {
		dwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;

		// ウィンドウサイズを計算
		Rect.left = 0;
		Rect.top = 0;
		Rect.right = width;
		Rect.bottom = height;
		AdjustWindowRect(&Rect, dwStyle, FALSE);
		width = Rect.right - Rect.left;
		height = Rect.bottom - Rect.top;

		// ウィンドウを作業領域の中心に配置
		SystemParametersInfo(SPI_GETWORKAREA, 0, &Rect, 0);
		x = (Rect.right - width) / 2;
		y = (Rect.bottom - height) / 2;
	}
	else {
		// Borderless fullscreen.
		// 화면 모드는 바꾸지 않고, 현재 모니터 전체를 덮는 창으로 만든다.
		dwStyle = WS_POPUP;

		HMONITOR hMonitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO mi;
		ZeroMemory(&mi, sizeof(mi));
		mi.cbSize = sizeof(mi);

		if (GetMonitorInfo(hMonitor, &mi) != 0) {
			x = mi.rcMonitor.left;
			y = mi.rcMonitor.top;
			width = mi.rcMonitor.right - mi.rcMonitor.left;
			height = mi.rcMonitor.bottom - mi.rcMonitor.top;

			// 중요:
			// 테두리 없는 전체화면에서는 실제 창 크기와 게임 내부 해상도를 일치시킨다.
			// 그렇지 않으면 낮은 해상도 선택 시 화면 안에 화면처럼 보인다.
			GameConfig.SetScreenWidth(width);
			GameConfig.SetScreenHeight(height);
		}
		else {
			x = 0;
			y = 0;

			// 실패 시에는 기존 설정값 유지
		}
	}
	

	//긂귻깛긤긂띿맟
	hWnd = CreateWindow( "MainWindow", title, dwStyle, x, y, width, height, NULL, NULL, hInstance, NULL );
	if( hWnd == NULL ){
		return false;
	}

	//?렑
	ShowWindow( hWnd, nCmdShow );

	return true;
}

//! @brief 긽귽깛긂귻깛긤긂궻긂귻깛긤긂긵깓긘?긙긿
LRESULT WINAPI WindowControl::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg ){
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_SYSKEYDOWN:
			return 0;

		case WM_ACTIVATEAPP:
			// Alt+Tab 등으로 게임 창이 비활성화될 때 커서 고정 해제
			if (wParam == FALSE) {
				ClipCursor(NULL);
				ReleaseCapture();
			}
			return 0;

		case WM_KILLFOCUS:
			// 포커스를 잃으면 커서 고정 해제
			ClipCursor(NULL);
			ReleaseCapture();
			return 0;

		case WM_CANCELMODE:
			// 시스템에 의해 마우스 캡처/모드가 취소될 때도 안전하게 해제
			ClipCursor(NULL);
			ReleaseCapture();
			return 0;

		case WM_MOUSEWHEEL:
			// 휠의 변화량(GET_WHEEL_DELTA_WPARAM)을 가져와 inputCtrl에 전달합니다.
			inputCtrl.SetMouseWheel((int)GET_WHEEL_DELTA_WPARAM(wParam));
			return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//! @brief 긂귻깛긤긂궻?렑긾?긤먛귟뫶궑
//! @param fullscreen false갌긂귻깛긤긂?렑?true갌긲깑긚긏깏?깛뾭?렑
//! @return 맟뚻갌true?렪봲갌false
bool WindowControl::ChangeWindowMode(bool fullscreen)
{
	if (hWnd == NULL) { return false; }

	DWORD dwStyle;
	RECT Rect;
	int x, y;
	int width, height;

	if (fullscreen == false) {
		// 念のため、古い排他フルスクリーンから戻る処理を残す
		ChangeDisplaySettings(NULL, 0);

		dwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;

		// クライアント領域はゲーム設定の解像度に戻す
		Rect.left = 0;
		Rect.top = 0;
		Rect.right = GameConfig.GetScreenWidth();
		Rect.bottom = GameConfig.GetScreenHeight();
		AdjustWindowRect(&Rect, dwStyle, FALSE);

		width = Rect.right - Rect.left;
		height = Rect.bottom - Rect.top;

		SystemParametersInfo(SPI_GETWORKAREA, 0, &Rect, 0);
		x = (Rect.right - width) / 2;
		y = (Rect.bottom - height) / 2;
	}
	else {
		// Borderless fullscreen.
		// 디스플레이 해상도는 변경하지 않고, 창만 모니터 전체로 확장한다.
		ChangeDisplaySettings(NULL, 0);

		dwStyle = WS_POPUP;

		HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		ZeroMemory(&mi, sizeof(mi));
		mi.cbSize = sizeof(mi);

		if (GetMonitorInfo(hMonitor, &mi) != 0) {
			x = mi.rcMonitor.left;
			y = mi.rcMonitor.top;
			width = mi.rcMonitor.right - mi.rcMonitor.left;
			height = mi.rcMonitor.bottom - mi.rcMonitor.top;

			// 중요:
			// 창 크기와 GameConfig 해상도를 맞춰야 UI/마우스/조준선 좌표가 깨지지 않는다.
			GameConfig.SetScreenWidth(width);
			GameConfig.SetScreenHeight(height);
		}
		else {
			x = 0;
			y = 0;
			width = GameConfig.GetScreenWidth();
			height = GameConfig.GetScreenHeight();
		}
	}

	// 反映
	SetWindowLong(hWnd, GWL_STYLE, dwStyle);
	SetWindowPos(
		hWnd,
		HWND_TOP,
		x,
		y,
		width,
		height,
		SWP_FRAMECHANGED | SWP_SHOWWINDOW
	);

	return true;
}

//! @brief 긂귻깛긤긂긪깛긤깑귩롦벦
//! @return 긂귻깛긤긂긪깛긤깑
HWND WindowControl::GethWnd()
{
	return hWnd;
}

//! @brief 긂귻깛긤긂긽긞긜?긙귩룉뿚걁긽귽깛깑?긵뵽믦걂
//! @return 귺귽긤깑륉뫴갌0?긒??궻긽귽깛룉뿚렳뛱갌1?긂귻깛긤긂궕빧궣궫갌-1
//! @attention ?듫릶궔귞걏1걐궕뺅궠귢궫뤾뜃궸뙽귟갂긒??궻긽귽깛룉뿚귩렳뛱궢궲궘궬궠궋갃
//! @attention ?듫릶궔귞걏-1걐궕뺅궠귢궫뤾뜃갂둫깏??긚귘귽깛??긲긃?긚귩됶뺳궢갂긒??귩뢎뿹궢궲궘궬궠궋갃
int WindowControl::CheckMainLoop()
{
	MSG msg = { 0 };

	if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT) {
			return -1;
		}
		return 0;
	}

	if (GetActiveWindow() == hWnd) {
		return 1;
	}

	// 비활성화 상태에서도 루프를 계속 돌린다.
	// Alt+Tab 중 Discord 캡처가 시작되어도 D3D 복구 루틴이 멈추지 않도록 한다.
	Sleep(10);
	return 1;
}

//! @brief 긄깋?긽긞긜?긙?렑
//! @param *str 긽긞긜?긙
void WindowControl::ErrorInfo(const char *str)
{
	if( str == NULL ){ return; }

	MessageBox(hWnd, str, "error", MB_OK);
}

//! @brief 긂귻깛긤긂귩빧궣귡귝궎궸뾴땫
void WindowControl::CloseWindow()
{
	PostMessage(hWnd, WM_CLOSE, 0L, 0L);
}

#ifdef ENABLE_DEBUGLOG
//! @brief 긢긫긞긏뾭깓긐궸깇?긗?듏떕귩뢯쀍
//! @note 뙸렄?궳궼OS륃뺪궴뙻뚭먠믦궻귒
void GetOperatingEnvironment()
{
	char str[256];
	char OSname[128];
	char OSver[128];
	char OSbit[128];
	int strsize;
	HKEY hKey = NULL;

	//깒긙긚긣깏궔귞OS륃뺪롦벦
	strcpy(OSname, "");
	strcpy(OSver, "");
	if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_EXECUTE | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS ){
		strsize = sizeof(OSname);
		RegQueryValueEx(hKey, "ProductName", 0, NULL, (LPBYTE)OSname, (LPDWORD)&strsize);
		strsize = sizeof(OSver);
		RegQueryValueEx(hKey, "DisplayVersion", 0, NULL, (LPBYTE)OSver, (LPDWORD)&strsize);
		RegCloseKey(hKey);
	}

	//OS긮긞긣릶롦벦
	SYSTEM_INFO SystemInfo = {0};
	GetNativeSystemInfo(&SystemInfo);
	switch(SystemInfo.wProcessorArchitecture){
		case PROCESSOR_ARCHITECTURE_AMD64: strcpy(OSbit, "64bit"); break;
		case PROCESSOR_ARCHITECTURE_IA64:  strcpy(OSbit, "64bit"); break;
		case PROCESSOR_ARCHITECTURE_INTEL: strcpy(OSbit, "32bit"); break;
		default: strcpy(OSbit, "unknown bit");
	}

	//OS륃뺪귩깓긐궸뢯쀍
	sprintf(str, "OS Name갌%s %s (%s)", OSname, OSver, OSbit);
	OutputLog.WriteLog(LOG_CHECK, "Environment", str);


	//뙻뚭륃뺪귩깓긐궸뢯쀍
	sprintf(str, "OS Language갌0x%04hx / 0x%04hx", GetSystemDefaultLangID(), GetUserDefaultLangID());
	OutputLog.WriteLog(LOG_CHECK, "Environment", str);
}
#endif

//! @brief fps걁Frames Per Second갌긲깒??깒?긣걂똶럁
//! @param getcnt fps귩롦벦궥귡뢂딖걁긲깒??뭁댧걂
//! @return fps릶
float GetFps(int getcnt)
{
	if( getcnt <= 0 ){ return 0.0f; }

	static unsigned int ptimeg = 0;
	unsigned int nowtime;
	static float pfps = 0.0f;
	static int cnt = 0;

	if( cnt == 0 ){
		ptimeg = GetTimeMS();
	}
	if( cnt == getcnt ){
		nowtime = GetTimeMS();
		pfps = 1000.0f / ((nowtime - ptimeg)/getcnt);
		cnt = 0;
	}
	else{
		cnt += 1;
	}

	return pfps;
}

//! @brief fps걁Frames Per Second갌긲깒??깒?긣걂뮧맢
//! @return 뮧맢귩렳?갌true??뮧맢귩렳?궧궦갌false
//! @attention WindowControl긏깋긚귩룊딖돸궢궶궋궴갂?귽??궻맱뱗궕뢯귏궧귪갃
bool ControlFps()
{
	static unsigned int ptimec = 0;
	unsigned int nowtime;
	unsigned int waittime;

	nowtime = GetTimeMS();
	waittime = GAMEFRAMEMS - (nowtime - ptimec);
	if( (0 < waittime)&&(waittime <= GAMEFRAMEMS) ){
		//timeBeginPeriod(1);
		Sleep(waittime);
		//timeEndPeriod(1);
		
		ptimec = GetTimeMS();
		return true;
	}

	ptimec = nowtime;
	return false;
}

//! @brief ?깏뷳뭁댧귩뺅궥
//! @return ?깏뷳
//! @attention WindowControl긏깋긚귩룊딖돸궢궶궋궴갂?귽??궻맱뱗궕뢯귏궧귪갃
unsigned int GetTimeMS()
{
	unsigned int time;

	//timeBeginPeriod(1);
	time = timeGetTime();
	//timeEndPeriod(1);

	return time;
}

//! @brief 볷렄궸귝귡빒럻쀱귩롦벦
//! @param str 빒럻쀱귩롷궚롦귡?귽깛?
void GetTimeName(char *str)
{
	if( str == NULL ){ return; }

	time_t timer;
	struct tm *local;

	timer = time(NULL);
	local = localtime(&timer);

	sprintf(str, "%04d%02d%02d%02d%02d%02d", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
}

//! @brief 뿉릶귩룊딖돸
void InitRand()
{
	srand(GetTimeMS());
}

//! @brief 깋깛??궶맢릶뭠귩뺅궥
//! @param num 붝댪
//! @return 0?num-1
int GetRand(int num)
{
	if( num <= 0 ){ return 0; }

	return rand()%num;

	//return rand() / (RAND_MAX/num);

	//?됄XOPS궻귺깑긕깏긛?갎
	//static int memory = GetTimeMS();
	//int x;
	//memory = memory * 214013 + 2745024;
	//x = memory >> 16;
	//x = x & 0x00007FFF;
	//return x%num;
}

//! @brief 럏롆뚙볺
//! @param r 뭠
//! @return 뭠
float Round(float r)
{
	return (r > 0.0f) ? floorf(r + 0.5f) : ceilf(r - 0.5f);
}

#ifdef ENABLE_PATH_DELIMITER_SLASH
//! @brief 긬긚뗦먛귟빒럻귩빾듂
//! @param *str 긲?귽깑긬긚궻?귽깛?
//! @return 륷궢궋긲?귽깑긬긚궻?귽깛?
//! @note '\'귩'/'귉뭫궖듂궑귏궥갃
char* ChangePathDelimiter(char *str)
{
	if( str == NULL ){ return NULL; }

	static char newstr[MAX_PATH];
	strcpy(newstr, str);

	for(int i=0; i<(int)strlen(newstr); i++){
		if( newstr[i] == '\\' ){
			newstr[i] = '/';
		}
	}

	return newstr;
}
#endif
