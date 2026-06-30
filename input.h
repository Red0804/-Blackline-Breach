//! @file input.h
//! @brief InputControl긏깋긚궻먬뙻

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

#ifndef INPUT_H
#define INPUT_H

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 1		//!< Select include file.
#endif
#include "main.h"

#include <windows.h>

#define INPUT_INTERFACE 0	//!< @brief 볺쀍귩롦벦궥귡귽깛??긲긃?긚궻멗묖 @details WindowsInput갌0?RawInput갌1?DirectInput갌2

#if INPUT_INTERFACE == 0
 #define INPUT_CORE "WindowsInput"					//!< 긫?긙깈깛?렑뾭륃뺪
#elif INPUT_INTERFACE == 1
 #include <commctrl.h>
 #pragma comment (lib, "comctl32.lib")

 #define INPUT_CORE "RawInput"						//!< 긫?긙깈깛?렑뾭륃뺪
#elif INPUT_INTERFACE == 2
 #define DIRECTINPUT_VERSION 0x0800	//!< 똸뜍걏DIRECTINPUT_VERSION undefined. Defaulting to version 0x0800걐귩뻞?
 #include <dinput.h>

 #pragma comment(lib, "dinput8.lib")
 #pragma comment(lib, "dxguid.lib")

 #define INPUT_CORE "DirectInput"					//!< 긫?긙깈깛?렑뾭륃뺪
#endif

//! @brief 볺쀍긢긫귽긚귩듖뿚궥귡긏깋긚
//! @details ?긂긚귘긌???긤궶궵궻볺쀍긢긫귽긚귩듖뿚궢궲궋귏궥갃
class InputControl
{
	HWND hWnd;				//!< 긂귻깛긤긂긪깛긤깑
	int mwheel;  // [✨ 추가] 마우스 휠 변화량 저장
#if INPUT_INTERFACE == 2
	LPDIRECTINPUT8 pDI;					//!< DIRECTINPUT8궻?귽깛?
	LPDIRECTINPUTDEVICE8 pDIDevice;		//!< DIRECTINPUTDEVICE8궻?귽깛?갋긌???긤뾭
	LPDIRECTINPUTDEVICE8 pMouse;		//!< DIRECTINPUTDEVICE8궻?귽깛?갋?긂긚뾭
#else
	bool InitFlag;			//!< 룊딖돸긲깋긐
#endif
	bool MouseLimitFlag;	//!< ?긂긚댷벍쀊귩맕뙽궥귡긲깋긐
	char keys[256];			//!< 긌?륃뺪귩둰?궥귡봹쀱
	char keys_lt[256];		//!< 걁멟됷궻걂긌?륃뺪귩둰?궥귡봹쀱
	int mx;					//!< ?긂긚궻X띆뷭
	int my;					//!< ?긂긚궻Y띆뷭
#if INPUT_INTERFACE == 0
	POINT point_lt;			//!< 걁멟됷궻걂?긂긚띆뷭
#endif
	bool mbl;				//!< ?긂긚궻뜺??깛
	bool mbr;				//!< ?긂긚궻덭??깛
	bool mbl_lt;			//!< 걁멟됷궻걂?긂긚궻뜺??깛
	bool mbr_lt;			//!< 걁멟됷궻걂?긂긚궻덭??깛
#if INPUT_INTERFACE == 1
	char keys_proc[256];	//!< 긌?륃뺪귩둰?궥귡봹쀱걁긵깓긘?긙긿걂
	int mx_proc;			//!< ?긂긚궻X띆뷭걁긵깓긘?긙긿걂
	int my_proc;			//!< ?긂긚궻Y띆뷭걁긵깓긘?긙긿걂
	bool mbl_proc;			//!< ?긂긚궻뜺??깛걁긵깓긘?긙긿걂
	bool mbr_proc;			//!< ?긂긚궻덭??깛걁긵깓긘?긙긿걂

	static LRESULT CALLBACK SubClassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	void GetRowInput(WPARAM wParam, LPARAM lParam);
#endif

public:
	InputControl();
	~InputControl();
	int InitInput(WindowControl *WindowCtrl);
	void DestroyInput();
	void SetMouseLimitFlag(bool flag);
	void GetInputState(bool mousemode);
	void MoveMouseCenter();
	bool CheckKeyNow(int id);
	bool CheckKeyDown(int id);
	bool CheckKeyUp(int id);
	void GetMouseMovement(int *x, int *y);
	bool CheckMouseButtonNowL();
	bool CheckMouseButtonDownL();
	bool CheckMouseButtonUpL();
	bool CheckMouseButtonNowR();
	bool CheckMouseButtonDownR();
	bool CheckMouseButtonUpR();
	int CheckInputAll();
	void SetMouseWheel(int value) { mwheel = value; } // 값 설정
	int GetMouseWheel() { return mwheel; }           // 값 가져오기
};

int OriginalkeycodeToDinputdef(int code);
bool GetDoubleKeyCode(int id, int *CodeL, int *CodeR);
int GetEscKeycode();
int GetHomeKeycode();
int GetFunctionKeycode(int key);

#endif