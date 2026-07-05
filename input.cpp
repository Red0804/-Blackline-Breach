//! @file input.cpp
//! @brief InputControl긏깋긚궻믦?

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

#include "input.h"

//! @brief 긓깛긚긣깋긏?
InputControl::InputControl()
{
	hWnd = NULL;
#if INPUT_INTERFACE == 2
	pDI = NULL;
	pDIDevice = NULL;
	pMouse = NULL;
#else
	InitFlag = false;
#endif
	MouseLimitFlag = true;

	//긌???긤먠믦뭠룊딖돸
	for(int i=0; i<256; i++){
		keys[i] = 0;
	}
	memcpy(keys_lt, keys, sizeof(char)*256);

	//?긂긚궻먠믦뭠룊딖돸
	mx = 0;
	my = 0;
#if INPUT_INTERFACE == 0
	point_lt.x = 0;
	point_lt.y = 0;
#endif
	mbl = false;
	mbr = false;
	mbl_lt = mbl;
	mbr_lt = mbr;

#if INPUT_INTERFACE == 1
	memcpy(keys_proc, keys, sizeof(char)*256);
	mx_proc = 0;
	my_proc = 0;
	mbl_proc = mbl;
	mbr_proc = mbr;
#endif
}

//! @brief 긢귻긚긣깋긏?
InputControl::~InputControl()
{
	DestroyInput();
}

//! @brief 룊딖돸
//! @param WindowCtrl WindowControl긏깋긚궻?귽깛?
//! @return 맟뚻갌0?렪봲갌1
int InputControl::InitInput(WindowControl *WindowCtrl)
{
	if( WindowCtrl == NULL ){ return 1; }

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
 #if INPUT_INTERFACE == 0
	OutputLog.WriteLog(LOG_INIT, "Input", "WindowsInput");
 #elif INPUT_INTERFACE == 1
	OutputLog.WriteLog(LOG_INIT, "Input", "RawInput");
 #elif INPUT_INTERFACE == 2
	OutputLog.WriteLog(LOG_INIT, "Input", "DirectInput");
 #endif
#endif

	//긂귻깛긤긂긪깛긤깑귩먠믦
	hWnd = WindowCtrl->GethWnd();

#if INPUT_INTERFACE == 1
	RAWINPUTDEVICE Rid[2];

	//?긂긚
	Rid[0].usUsagePage = 0x01; 
	Rid[0].usUsage = 0x02; 
	Rid[0].dwFlags = NULL;//RIDEV_NOLEGACY;
	Rid[0].hwndTarget = 0;

	//긌???긤
	Rid[1].usUsagePage = 0x01; 
	Rid[1].usUsage = 0x06; 
	Rid[1].dwFlags = NULL;//RIDEV_NOLEGACY;
	Rid[1].hwndTarget = 0;

	//WM_INPUT궳롦벦궥귡긢긫귽긚귩뱋?
	if( RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE ){
		return 1;
	}

	//긵깓긘?긙긿뱋?
	SetWindowSubclass(hWnd, SubClassProc, 1, (DWORD_PTR)this);
#elif INPUT_INTERFACE == 2
	//DirectInput룊딖돸
	if( FAILED( DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDI, NULL) ) ){
		return 1;
	}

	//긌???긤룊딖돸
	if( FAILED( pDI->CreateDevice(GUID_SysKeyboard, &pDIDevice, NULL) ) ){
		return 1;
	}
	pDIDevice->SetDataFormat(&c_dfDIKeyboard);
	pDIDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	//?긂긚룊딖돸
	if( FAILED( pDI->CreateDevice(GUID_SysMouse, &pMouse, NULL) ) ){
		return 1;
	}
	pMouse->SetDataFormat(&c_dfDIMouse2);
	pMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	pMouse->Acquire();
#endif

	//긇??깑귩뷄?렑
	ShowCursor(false);

#if INPUT_INTERFACE != 2
	InitFlag = true;
#endif

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	return 0;
}

//! @brief 됶뺳
//! @attention ?듫릶궼렔벍밒궸뚁귂뢯궠귢귏궥궕갂뼻렑밒궸뚁귂뢯궥궞궴귖됀?궳궥갃
void InputControl::DestroyInput()
{
#if INPUT_INTERFACE != 2
	if( InitFlag == false ){ return; }
#else
	if( (pDI == NULL)&&(pDIDevice == NULL)&&(pMouse == NULL) ){ return; }
#endif

#if INPUT_INTERFACE == 0
	InitFlag = false;
#elif INPUT_INTERFACE == 1
	InitFlag = false;

	//긵깓긘?긙긿됶뺳
	RemoveWindowSubclass(hWnd, SubClassProc, 1);
#elif INPUT_INTERFACE == 2
	//긌???긤긢긫귽긚귩됶뺳
	if( pDIDevice != NULL ){
		pDIDevice->Unacquire();
		pDIDevice->Release();
		pDIDevice = NULL;
	}

	//?긂긚긢긫귽긚귩됶뺳
	if( pMouse != NULL ){
		pMouse->Unacquire();
		pMouse->Release();
		pMouse = NULL;
	}

	//DirectInput귩됶뺳
	if( pDI != NULL){
		pDI->Release();
		pDI = NULL;
	}
#endif

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
 #if INPUT_INTERFACE == 0
	OutputLog.WriteLog(LOG_CLEANUP, "Input", "WindowsInput");
 #elif INPUT_INTERFACE == 1
	OutputLog.WriteLog(LOG_CLEANUP, "Input", "RawInput");
 #elif INPUT_INTERFACE == 2
	OutputLog.WriteLog(LOG_CLEANUP, "Input", "DirectInput");
 #endif
#endif
}

#if INPUT_INTERFACE == 1
//! @brief RowInput귩롦벦궥귡긵깓긘?긙긿
LRESULT CALLBACK InputControl::SubClassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	//뼟럊뾭덙릶뫮랉
	UNREFERENCED_PARAMETER(uIdSubclass);

	if( uMsg == WM_INPUT ){
		InputControl* input = (InputControl*)dwRefData;
		input->GetRowInput(wParam, lParam);
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

//! @brief 긵깓긘?긙긿궔귞RowInput롦벦
void InputControl::GetRowInput(WPARAM wParam, LPARAM lParam)
{
	//뼟럊뾭덙릶뫮랉
	UNREFERENCED_PARAMETER(wParam);

	UINT dwSize;

	//RowInput긖귽긛롦벦
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	LPBYTE lpb = new BYTE[dwSize];
	if( lpb == NULL ){ return; }

	//RowInput렳긢??롦벦
	if( GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize ){
		return;
	}

	RAWINPUT* raw = (RAWINPUT*)lpb;

	//긌???긤
	if( raw->header.dwType == RIM_TYPEKEYBOARD ){
		if( (raw->data.keyboard.Flags&0x01) == RI_KEY_MAKE ){		//긌?궕돓궠귢궫
			int Vkey = raw->data.keyboard.VKey;
			keys_proc[Vkey] = (char)0x80;
		}
		if( (raw->data.keyboard.Flags&0x01) == RI_KEY_BREAK ){		//긌?궕뿣궠귢궫
			int Vkey = raw->data.keyboard.VKey;
			keys_proc[Vkey] = (char)0x00;
		}
	}

	//?긂긚
	if( raw->header.dwType == RIM_TYPEMOUSE ){
		if( (raw->data.mouse.usButtonFlags&RI_MOUSE_LEFT_BUTTON_DOWN) != 0 ){	//뜺??깛궕돓궠귢궫
			mbl_proc = true;
		}
		if( (raw->data.mouse.usButtonFlags&RI_MOUSE_LEFT_BUTTON_UP) != 0 ){		//뜺??깛궕뿣궠귢궫
			mbl_proc = false;
		}
		if( (raw->data.mouse.usButtonFlags&RI_MOUSE_RIGHT_BUTTON_DOWN) != 0 ){	//덭??깛궕돓궠귢궫
			mbr_proc = true;
		}
		if( (raw->data.mouse.usButtonFlags&RI_MOUSE_RIGHT_BUTTON_UP) != 0 ){	//덭??깛궕뿣궠귢궫
			mbr_proc = false;
		}

		//?긂긚띆뷭돿럁
		mx_proc += (int)raw->data.mouse.lLastX;
		my_proc += (int)raw->data.mouse.lLastY;
	}

	delete[] lpb;
}
#endif

//! @brief ?긂긚댷벍쀊귩맕뙽궥귡긲깋긐먠믦
//! @param flag 뾎뚼갌true?뼰뚼갌false
//! @attention ?긂긚볺쀍쀊궕char? 8bit몜뱰궸궶귟귏궥갃
void InputControl::SetMouseLimitFlag(bool flag)
{
	MouseLimitFlag = flag;
}

//! @brief 볺쀍긢긫귽긚궻륉뫴귩뛛륷
//! @param mousemode ?긂긚궻띆뷭롦벦?먥뫮뭠걁띆뷭걂갌false?몜뫮뭠걁댷벍쀊걂갌true
void InputControl::GetInputState(bool mousemode)
{

	// 게임 창이 포그라운드가 아닐 때는 입력을 갱신하지 않는다.
// 단, window.cpp의 메인 루프는 계속 돌 수 있으므로 Discord 캡처 안정성은 유지된다.
	if (GetForegroundWindow() != hWnd) {
		ClipCursor(NULL);
		ReleaseCapture();

		memcpy(keys_lt, keys, sizeof(char) * 256);
		for (int i = 0; i < 256; i++) {
			keys[i] = 0;
		}

		mx = 0;
		my = 0;

		mbl_lt = mbl;
		mbr_lt = mbr;
		mbl = false;
		mbr = false;

#if INPUT_INTERFACE == 1
		mx_proc = 0;
		my_proc = 0;
		mbl_proc = false;
		mbr_proc = false;
#endif

		return;
	}
#if INPUT_INTERFACE == 0
	//롦벦뮳멟궻긌?륃뺪귩갂멟긲깒??륃뺪궴궢궲딯?
	memcpy(keys_lt, keys, sizeof(char)*256);

	//뙸띪궻긌???긤볺쀍귩롦벦
	if( GetKeyboardState((PBYTE)&keys) == 0 ){ return; }
#elif INPUT_INTERFACE == 1
	//롦벦뮳멟궻긌?륃뺪귩갂멟긲깒??륃뺪궴궢궲딯?
	memcpy(keys_lt, keys, sizeof(char)*256);

	//뙸띪궻긌???긤볺쀍귩롦벦
	memcpy(keys, keys_proc, sizeof(char)*256);
#elif INPUT_INTERFACE == 2
	//긌???긤긢긫귽긚궕맫궢궘럊뾭궳궖귢궽
	if( pDIDevice != NULL ){
		HRESULT hr = pDIDevice->Acquire();
		if( (hr==DI_OK) || (hr==S_FALSE) ){
			//롦벦뮳멟궻긌?륃뺪귩갂멟긲깒??륃뺪궴궢궲딯?
			memcpy(keys_lt, keys, sizeof(char)*256);

			//뙸띪궻긌???긤볺쀍귩롦벦
			pDIDevice->GetDeviceState(sizeof(keys), &keys);
		}
	}
#endif

	POINT point;

	//댧뭫귩긚긏깏?깛띆뷭궳롦벦걁WinAPI걂
	GetCursorPos(&point);
	ScreenToClient(hWnd, &point);

#if INPUT_INTERFACE == 0
	//?긂긚띆뷭귩뢯쀍
	if( mousemode == false ){
		mx = point.x;
		my = point.y;
	}
	else{
		mx = point.x - point_lt.x;
		my = point.y - point_lt.y;
	}

	//?긂긚띆뷭귩멟긲깒??륃뺪궴궢궲딯?
	point_lt = point;


	//롦벦뮳멟궻??깛륃뺪귩갂멟긲깒??륃뺪궴궢궲딯?
	mbl_lt = mbl;
	mbr_lt = mbr;

	//?긂긚궻??깛귩롦벦
	if( GetKeyState(VK_LBUTTON) < 0 ){ mbl = true; }
	else{ mbl = false; }
	if( GetKeyState(VK_RBUTTON) < 0 ){ mbr = true; }
	else{ mbr = false; }
#elif INPUT_INTERFACE == 1
	//?긂긚띆뷭귩뢯쀍
	if( mousemode == false ){
		mx = point.x;
		my = point.y;
	}
	else{
		mx = mx_proc;
		my = my_proc;
	}

	mx_proc = 0;
	my_proc = 0;


	//롦벦뮳멟궻??깛륃뺪귩갂멟긲깒??륃뺪궴궢궲딯?
	mbl_lt = mbl;
	mbr_lt = mbr;

	//?긂긚궻??깛귩롦벦
	mbl = mbl_proc;
	mbr = mbr_proc;
#elif INPUT_INTERFACE == 2
	//?긂긚긢긫귽긚궕맫궢궘룊딖돸궠귢궲궋귢궽
	if( pMouse != NULL ){
		//?긂긚륃뺪귩롦벦걁DirectInput걂
		DIMOUSESTATE2 dIMouseState={0};
		if( FAILED(pMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &dIMouseState ) ) ){
			pMouse->Acquire();
		}

		//?긂긚띆뷭귩뢯쀍
		if( mousemode == false ){
			mx = point.x;
			my = point.y;
		}
		else{
			mx = dIMouseState.lX;
			my = dIMouseState.lY;
		}

		//롦벦뮳멟궻??깛륃뺪귩갂멟긲깒??륃뺪궴궢궲딯?
		mbl_lt = mbl;
		mbr_lt = mbr;

		//?긂긚궻??깛귩롦벦
		if( dIMouseState.rgbButtons[0]&0x80 ){ mbl = true; }
		else{ mbl = false; }
		if( dIMouseState.rgbButtons[1]&0x80 ){ mbr = true; }
		else{ mbr = false; }
	}
#endif

	if( MouseLimitFlag == true ){
		//?긂긚궕몜뫮뭠걁댷벍쀊걂롦벦긾?긤궶귞궽갂char? 8bit몜뱰궸궥귡갃
		if( mousemode == true ){
			if( mx > 127 ) { mx = 127; }
			if( mx < -128 ){ mx = -128; }
			if( my > 127 ) { my = 127; }
			if( my < -128 ){ my = -128; }
		}
	}
}

//! @brief ?긂긚귩뭷륲궸댷벍
void InputControl::MoveMouseCenter()
{
	POINT point;

	// 게임 창이 실제 포그라운드가 아닐 때는 마우스를 중앙으로 되돌리지 않는다.
// Alt+Tab, Discord 화면 공유 창 선택 중 커서가 다시 게임에 붙잡히는 문제 방지.
	if (GetForegroundWindow() != hWnd) {
		ClipCursor(NULL);
		ReleaseCapture();
		return;
	}

	//긂귻깛긤긂띆뷭궻뭷돍귩땫귕귡
	point.x = GameConfig.GetScreenWidth()/2;
	point.y = GameConfig.GetScreenHeight()/2;

#if INPUT_INTERFACE == 0
	//멟됷궻띆뷭귩룕궖듂궑
	point_lt = point;
#endif

	//긚긏깏?깛띆뷭궸빾듂궢갂띆뷭빾뛛
	ClientToScreen(hWnd, &point);
	SetCursorPos(point.x, point.y);
}

//! @brief 긌???긤궻볺쀍귩?긃긞긏걁깏귺깑?귽?걂
//! @return 돓궠귢궲궶궋갌false?돓궠귢궲궋귡갌true
bool InputControl::CheckKeyNow(int id)
{
	if( (id < 0)||(256 <= id) ){ return false; }

	//뙸띪돓궠귢궲궋귢궽
	if( keys[id]&0x80 ){ return true; }
	return false;
}

//! @brief 렌더 프레임용 실시간 키 입력 확인
//! @details keys/keys_lt를 변경하지 않으므로 33FPS 로직의 Down/Up 판정에 영향을 주지 않는다.
bool InputControl::CheckKeyNowRealtime(int id)
{
	if ((id < 0) || (256 <= id)) { return false; }
	if (GetForegroundWindow() != hWnd) { return false; }

#if INPUT_INTERFACE == 0
	return ((GetAsyncKeyState(id) & 0x8000) != 0);
#elif INPUT_INTERFACE == 1
	return ((keys_proc[id] & 0x80) != 0);
#elif INPUT_INTERFACE == 2
	if (pDIDevice == NULL) { return false; }

	char realtime_keys[256] = { 0 };
	HRESULT hr = pDIDevice->Acquire();
	if ((hr != DI_OK) && (hr != S_FALSE)) { return false; }
	if (FAILED(pDIDevice->GetDeviceState(sizeof(realtime_keys), realtime_keys))) {
		return false;
	}
	return ((realtime_keys[id] & 0x80) != 0);
#else
	return false;
#endif
}

//! @brief 긌???긤궻볺쀍귩?긃긞긏걁돓궠귢궫뢷듩걂
//! @return 돓궠귢궫뢷듩궳궶궋갌false?돓궠귢궫뢷듩궳궇귡갌true
bool InputControl::CheckKeyDown(int id)
{
	if( (id < 0)||(256 <= id) ){ return false; }

	//멟됷궼돓궠귢궲궓귞궦갂뙸띪돓궠귢궲궋귢궽
	if( ((keys_lt[id]&0x80) == 0)&&(keys[id]&0x80) ){ return true; }
	return false;
}

//! @brief 긌???긤궻볺쀍귩?긃긞긏걁뿣궠귢궫뢷듩걂
//! @return 뿣궠귢궫뢷듩궳궶궋갌false?뿣궠귢궫뢷듩궳궇귡갌true
bool InputControl::CheckKeyUp(int id)
{
	if( (id < 0)||(256 <= id) ){ return false; }

	//멟됷귩돓궠귢궲궓귟갂뙸띪돓궠귢궲궋궶궚귢궽
	if( (keys_lt[id]&0x80)&&((keys[id]&0x80) == 0) ){ return true; }
	return false;
}

//! @brief ?긂긚궻볺쀍귩?긃긞긏
//! @param x x렡귩롷궚롦귡맢릶뭠??귽깛?
//! @param y y렡귩롷궚롦귡맢릶뭠??귽깛?
//! @attention 뭠궼뮳멟궸렳뛱궢궫 GetInputState() 귉궻덙릶궸뎓떯궠귢귡갃
void InputControl::GetMouseMovement(int *x, int *y)
{
	if( (x == NULL)||(y == NULL) ){ return; }

	//?긂긚띆뷭귩묆볺
	*x = mx;
	*y = my;
}

// 렌더링 주기에서 마우스 이동량만 안전하게 읽는다.
// GetInputState()와 달리 키/버튼의 이전 상태(keys_lt, mbl_lt 등)는 변경하지 않는다.
void InputControl::GetMouseMovementRealtime(int *x, int *y)
{
	if ((x == NULL) || (y == NULL)) { return; }

	*x = 0;
	*y = 0;

	if (GetForegroundWindow() != hWnd) {
		return;
	}

#if INPUT_INTERFACE == 0
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(hWnd, &point);

	POINT center;
	center.x = GameConfig.GetScreenWidth() / 2;
	center.y = GameConfig.GetScreenHeight() / 2;

	*x = point.x - center.x;
	*y = point.y - center.y;

	// 다음 GetInputState(true)가 같은 이동을 다시 읽지 않도록 기준점을 중앙에 맞춘다.
	point_lt = center;

	if ((*x != 0) || (*y != 0)) {
		POINT screen_center = center;
		ClientToScreen(hWnd, &screen_center);
		SetCursorPos(screen_center.x, screen_center.y);
	}
#elif INPUT_INTERFACE == 1
	*x = mx_proc;
	*y = my_proc;
	mx_proc = 0;
	my_proc = 0;
#elif INPUT_INTERFACE == 2
	if (pMouse != NULL) {
		DIMOUSESTATE2 state = { 0 };
		if (FAILED(pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &state))) {
			pMouse->Acquire();
		}
		else {
			*x = state.lX;
			*y = state.lY;
		}
	}
#endif

	if (MouseLimitFlag == true) {
		if (*x > 127) { *x = 127; }
		if (*x < -128) { *x = -128; }
		if (*y > 127) { *y = 127; }
		if (*y < -128) { *y = -128; }
	}
}

//! @brief ?긂긚갋뜺??깛궻볺쀍귩?긃긞긏걁깏귺깑?귽?걂
//! @return 돓궠귢궲궶궋갌false?돓궠귢궲궋귡갌true
bool InputControl::CheckMouseButtonNowL()
{
	//뙸띪궻륃뺪귩뺅궥
	return mbl;
}

//! @brief ?긂긚갋뜺??깛궻볺쀍귩?긃긞긏걁돓궠귢궫뢷듩걂
//! @return 돓궠귢궫뢷듩궳궶궋갌false?돓궠귢궫뢷듩궳궇귡갌true
bool InputControl::CheckMouseButtonDownL()
{
	//멟됷궼돓궠귢궲궓귞궦갂뙸띪돓궠귢궲궋귢궽
	if( (mbl_lt == false)&&(mbl == true) ){ return true; }
	return false;
}

//! @brief ?긂긚갋뜺??깛궻볺쀍귩?긃긞긏걁뿣궠귢궫뢷듩걂
//! @return 뿣궠귢궫뢷듩궳궶궋갌false?뿣궠귢궫뢷듩궳궇귡갌true
bool InputControl::CheckMouseButtonUpL()
{
	//멟됷귩돓궠귢궲궓귟갂뙸띪돓궠귢궲궋궶궚귢궽
	if( (mbl_lt == true)&&(mbl == false) ){ return true; }
	return false;
}

//! @brief ?긂긚갋덭??깛궻볺쀍귩?긃긞긏걁깏귺깑?귽?걂
//! @return 돓궠귢궲궶궋갌false?돓궠귢궲궋귡갌true
bool InputControl::CheckMouseButtonNowR()
{
	//뙸띪궻륃뺪귩뺅궥
	return mbr;
}

//! @brief ?긂긚갋덭??깛궻볺쀍귩?긃긞긏걁돓궠귢궫뢷듩걂
//! @return 돓궠귢궫뢷듩궳궶궋갌false?돓궠귢궫뢷듩궳궇귡갌true
bool InputControl::CheckMouseButtonDownR()
{
	//멟됷궼돓궠귢궲궓귞궦갂뙸띪돓궠귢궲궋귢궽
	if( (mbr_lt == false)&&(mbr == true) ){ return true; }
	return false;
}

//! @brief ?긂긚갋덭??깛궻볺쀍귩?긃긞긏걁뿣궠귢궫뢷듩걂
//! @return 뿣궠귢궫뢷듩궳궶궋갌false?뿣궠귢궫뢷듩궳궇귡갌true
bool InputControl::CheckMouseButtonUpR()
{
	//멟됷귩돓궠귢궲궓귟갂뙸띪돓궠귢궲궋궶궚귢궽
	if( (mbr_lt == true)&&(mbr == false) ){ return true; }
	return false;
}

//! @brief 긆깏긙긥깑긌?긓?긤궻볺쀍?긃긞긏
//! @return 돓궠귢궫긆깏긙긥깑긌?긓?긤
//! @attention 긆깏긙긥깑긌?긓?긤궸뫔띪궢궶궋긌?궼뵽믦궢궶궋갃
int InputControl::CheckInputAll()
{
	for(int i=0; i<=0x4A; i++){
		int KeyCode = OriginalkeycodeToDinputdef(i);

		if( KeyCode == -1 ){
			if( CheckMouseButtonUpL() == true ){
				return i;
			}
		}
		if( KeyCode == -2 ){
			if( CheckMouseButtonUpR() == true ){
				return i;
			}
		}
		if( KeyCode == -3 ){
			int CodeL, CodeR;
			GetDoubleKeyCode(0, &CodeL, &CodeR);
			if( CheckKeyUp(CodeL) == true ){
				return i;
			}
			if( CheckKeyUp(CodeR) == true ){
				return i;
			}
		}
		if( KeyCode == -4 ){
			int CodeL, CodeR;
			GetDoubleKeyCode(1, &CodeL, &CodeR);
			if( CheckKeyUp(CodeL) == true ){
				return i;
			}
			if( CheckKeyUp(CodeR) == true ){
				return i;
			}
		}

		if( CheckKeyUp(KeyCode) == true ){
			return i;
		}
	}

	return -1;
}

//! @brief 긆깏긙긥깑긌?긓?긤궔귞롦벦뾭긌?긓?긤귉빾듂
//! @param code 긆깏긙긥깑긌?긓?긤
//! @return 1댥뤵갌롦벦뾭긌?긓?긤?-1댥돷갌벫롪?0갌렪봲
//! @attention 댥돷갂벫롪궶뽣귟뭠궻뤾뜃?<br>-1갌MOUSE L??-2갌MOUSE R??-3갌DIK_LSHIFT / DIK_RSHIFT??-4갌DIK_LCONTROL / DIK_RCONTROL
int OriginalkeycodeToDinputdef(int code)
{
	int out = 0;

#if INPUT_INTERFACE != 2
	switch(code){
		case 0x00: out = VK_UP; break;
		case 0x01: out = VK_DOWN; break;
		case 0x02: out = VK_LEFT; break;
		case 0x03: out = VK_RIGHT; break;
		case 0x04: out = VK_NUMPAD0; break;
		case 0x05: out = VK_NUMPAD1; break;
		case 0x06: out = VK_NUMPAD2; break;
		case 0x07: out = VK_NUMPAD3; break;
		case 0x08: out = VK_NUMPAD4; break;
		case 0x09: out = VK_NUMPAD5; break;
		case 0x0A: out = VK_NUMPAD6; break;
		case 0x0B: out = VK_NUMPAD7; break;
		case 0x0C: out = VK_NUMPAD8; break;
		case 0x0D: out = VK_NUMPAD9; break;
		case 0x0E: out = VK_BACK; break;
		case 0x0F: out = VK_RETURN; break;

		case 0x10: out = VK_TAB; break;
		case 0x11: out = VK_SPACE; break;
		case 0x12: out = -1; break;				//MOUSE L
		case 0x13: out = -2; break;				//MOUSE R
		case 0x14: out = VK_SHIFT; break;
		case 0x15: out = VK_CONTROL; break;
		case 0x16: out = '0'; break;
		case 0x17: out = '1'; break;
		case 0x18: out = '2'; break;
		case 0x19: out = '3'; break;
		case 0x1A: out = '4'; break;
		case 0x1B: out = '5'; break;
		case 0x1C: out = '6'; break;
		case 0x1D: out = '7'; break;
		case 0x1E: out = '8'; break;
		case 0x1F: out = '9'; break;

		case 0x20: out = 'A'; break;
		case 0x21: out = 'B'; break;
		case 0x22: out = 'C'; break;
		case 0x23: out = 'D'; break;
		case 0x24: out = 'E'; break;
		case 0x25: out = 'F'; break;
		case 0x26: out = 'G'; break;
		case 0x27: out = 'H'; break;
		case 0x28: out = 'I'; break;
		case 0x29: out = 'J'; break;
		case 0x2A: out = 'K'; break;
		case 0x2B: out = 'L'; break;
		case 0x2C: out = 'M'; break;
		case 0x2D: out = 'N'; break;
		case 0x2E: out = 'O'; break;
		case 0x2F: out = 'P'; break;

		case 0x30: out = 'Q'; break;
		case 0x31: out = 'R'; break;
		case 0x32: out = 'S'; break;
		case 0x33: out = 'T'; break;
		case 0x34: out = 'U'; break;
		case 0x35: out = 'V'; break;
		case 0x36: out = 'W'; break;
		case 0x37: out = 'X'; break;
		case 0x38: out = 'Y'; break;
		case 0x39: out = 'Z'; break;
		case 0x3A: out = VK_OEM_2; break;
		case 0x3B: out = VK_OEM_1; break;
		case 0x3C: out = VK_OEM_PLUS; break;
		case 0x3D: out = VK_OEM_MINUS; break;
		case 0x3E: out = VK_OEM_3; break;
		case 0x3F: out = VK_OEM_4; break;

		case 0x40: out = VK_OEM_6; break;
		case 0x41: out = VK_OEM_102; break;
		case 0x42: out = VK_OEM_5; break;
		case 0x43: out = VK_OEM_COMMA; break;
		case 0x44: out = VK_OEM_PERIOD; break;
		case 0x45: out = VK_OEM_7; break;
		case 0x46: out = VK_MULTIPLY ; break;
		case 0x47: out = VK_DIVIDE; break;
		case 0x48: out = VK_ADD; break;
		case 0x49: out = VK_SUBTRACT; break;
		case 0x4A: out = VK_DECIMAL; break;

		default : out = 0; 
	}
#else
	switch(code){
		case 0x00: out = DIK_UP; break;
		case 0x01: out = DIK_DOWN; break;
		case 0x02: out = DIK_LEFT; break;
		case 0x03: out = DIK_RIGHT; break;
		case 0x04: out = DIK_NUMPAD0; break;
		case 0x05: out = DIK_NUMPAD1; break;
		case 0x06: out = DIK_NUMPAD2; break;
		case 0x07: out = DIK_NUMPAD3; break;
		case 0x08: out = DIK_NUMPAD4; break;
		case 0x09: out = DIK_NUMPAD5; break;
		case 0x0A: out = DIK_NUMPAD6; break;
		case 0x0B: out = DIK_NUMPAD7; break;
		case 0x0C: out = DIK_NUMPAD8; break;
		case 0x0D: out = DIK_NUMPAD9; break;
		case 0x0E: out = DIK_BACK; break;
		case 0x0F: out = DIK_RETURN; break;

		case 0x10: out = DIK_TAB; break;
		case 0x11: out = DIK_SPACE; break;
		case 0x12: out = -1; break;				//MOUSE L
		case 0x13: out = -2; break;				//MOUSE R
		case 0x14: out = -3; break;				//DIK_LSHIFT / DIK_RSHIFT
		case 0x15: out = -4; break;				//DIK_LCONTROL / DIK_RCONTROL
		case 0x16: out = DIK_0; break;
		case 0x17: out = DIK_1; break;
		case 0x18: out = DIK_2; break;
		case 0x19: out = DIK_3; break;
		case 0x1A: out = DIK_4; break;
		case 0x1B: out = DIK_5; break;
		case 0x1C: out = DIK_6; break;
		case 0x1D: out = DIK_7; break;
		case 0x1E: out = DIK_8; break;
		case 0x1F: out = DIK_9; break;

		case 0x20: out = DIK_A; break;
		case 0x21: out = DIK_B; break;
		case 0x22: out = DIK_C; break;
		case 0x23: out = DIK_D; break;
		case 0x24: out = DIK_E; break;
		case 0x25: out = DIK_F; break;
		case 0x26: out = DIK_G; break;
		case 0x27: out = DIK_H; break;
		case 0x28: out = DIK_I; break;
		case 0x29: out = DIK_J; break;
		case 0x2A: out = DIK_K; break;
		case 0x2B: out = DIK_L; break;
		case 0x2C: out = DIK_M; break;
		case 0x2D: out = DIK_N; break;
		case 0x2E: out = DIK_O; break;
		case 0x2F: out = DIK_P; break;

		case 0x30: out = DIK_Q; break;
		case 0x31: out = DIK_R; break;
		case 0x32: out = DIK_S; break;
		case 0x33: out = DIK_T; break;
		case 0x34: out = DIK_U; break;
		case 0x35: out = DIK_V; break;
		case 0x36: out = DIK_W; break;
		case 0x37: out = DIK_X; break;
		case 0x38: out = DIK_Y; break;
		case 0x39: out = DIK_Z; break;
		case 0x3A: out = DIK_SLASH; break;
		case 0x3B: out = DIK_COLON; break;
		case 0x3C: out = DIK_SEMICOLON; break;
		case 0x3D: out = DIK_MINUS; break;
		case 0x3E: out = DIK_AT; break;
		case 0x3F: out = DIK_LBRACKET; break;

		case 0x40: out = DIK_RBRACKET; break;
		case 0x41: out = DIK_BACKSLASH; break;
		case 0x42: out = DIK_YEN; break;
		case 0x43: out = DIK_COMMA; break;
		case 0x44: out = DIK_PERIOD; break;
		case 0x45: out = DIK_EQUALS; break;
		case 0x46: out = DIK_NUMPADSTAR; break;
		case 0x47: out = DIK_NUMPADSLASH; break;
		case 0x48: out = DIK_NUMPADPLUS; break;
		case 0x49: out = DIK_NUMPADMINUS; break;
		case 0x4A: out = DIK_NUMPADPERIOD; break;

		default : out = 0; 
	}
#endif

	return out;
}

//! @brief 뜺덭긌?궻긌?긓?긤롦벦
//! @param id Shift긌?갌0?Ctrl긌?갌1
//! @param *CodeL 뜺뫀긌?궻긌?긓?긤귩롷궚롦귡?귽깛?
//! @param *CodeR 덭뫀긌?궻긌?긓?긤귩롷궚롦귡?귽깛?
//! @return 맟뚻갌true?렪봲갌false?
bool GetDoubleKeyCode(int id, int *CodeL, int *CodeR)
{
	if( (CodeL == NULL)||(CodeR == NULL) ){ return false; }

#if INPUT_INTERFACE != 2
	//뼟럊뾭덙릶뫮랉
	UNREFERENCED_PARAMETER(id);
#else
	if( id == 0 ){
		*CodeL = DIK_LSHIFT;
		*CodeR = DIK_RSHIFT;
		return true;
	}
	if( id == 1 ){
		*CodeL = DIK_LCONTROL;
		*CodeR = DIK_RCONTROL;
		return true;
	}
#endif

	//긄깋?
	*CodeL = 0x00;
	*CodeR = 0x00;
	return false;
}

//! @brief Esc긌?궻긌?긓?긤롦벦
//! @return 긌?긓?긤
int GetEscKeycode()
{
#if INPUT_INTERFACE != 2
	return VK_ESCAPE;
#else
	return DIK_ESCAPE;
#endif
}

//! @brief Home긌?궻긌?긓?긤롦벦
//! @return 긌?긓?긤
int GetHomeKeycode()
{
#if INPUT_INTERFACE != 2
	return VK_HOME;
#else
	return DIK_HOME;
#endif
}

//! @brief 긲?깛긏긘깈깛긌?걁F1?F12걂궻긌?긓?긤귩롦벦
//! @param key 붥뜂걁1?12걂
//! @return 긌?긓?긤
int GetFunctionKeycode(int key)
{
	int out = 0;

#if INPUT_INTERFACE != 2
	switch(key){
		case 1: out = VK_F1; break;
		case 2: out = VK_F2; break;
		case 3: out = VK_F3; break;
		case 4: out = VK_F4; break;
		case 5: out = VK_F5; break;
		case 6: out = VK_F6; break;
		case 7: out = VK_F7; break;
		case 8: out = VK_F8; break;
		case 9: out = VK_F9; break;
		case 10: out = VK_F10; break;
		case 11: out = VK_F11; break;
		case 12: out = VK_F12; break;

		default : out = 0; 
	}
#else
	switch(key){
		case 1: out = DIK_F1; break;
		case 2: out = DIK_F2; break;
		case 3: out = DIK_F3; break;
		case 4: out = DIK_F4; break;
		case 5: out = DIK_F5; break;
		case 6: out = DIK_F6; break;
		case 7: out = DIK_F7; break;
		case 8: out = DIK_F8; break;
		case 9: out = DIK_F9; break;
		case 10: out = DIK_F10; break;
		case 11: out = DIK_F11; break;
		case 12: out = DIK_F12; break;

		default : out = 0; 
	}
#endif

	return out;
}