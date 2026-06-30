//! @file statemachine.h
//! @brief StateMachineクラスの宣言

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

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 3		//!< Select include file.
#endif
#include "main.h"

//! 状態を?す定数
enum State
{
	STATE_NULL = 0,			//!< ［エラ?］無効な状態
	STATE_CREATE_OPENING,	//!< オ?プニング画面 初期化・?備
	STATE_NOW_OPENING,		//!< オ?プニング画面 実行中
	STATE_DESTROY_OPENING,	//!< オ?プニング画面 終了・廃棄
	STATE_CREATE_MENU,		//!< メニュ?画面 初期化・?備
	STATE_NOW_MENU,			//!< メニュ?画面 実行中
	STATE_DESTROY_MENU,		//!< メニュ?画面 終了・廃棄
	STATE_CREATE_BRIEFING,	//!< ブリ?フィング画面 初期化・?備
	STATE_NOW_BRIEFING,		//!< ブリ?フィング画面 実行中
	STATE_DESTROY_BRIEFING,	//!< ブリ?フィング画面 終了・廃棄
	STATE_CREATE_MAINGAME,	//!< メイン画面 初期化・?備
	STATE_NOW_MAINGAME,		//!< メイン画面 実行中
	STATE_DESTROY_MAINGAME,	//!< メイン画面 終了・廃棄
	STATE_CREATE_RESULT,	//!< 結果?示画面 初期化・?備
	STATE_NOW_RESULT,		//!< 結果?示画面 実行中
	STATE_DESTROY_RESULT,	//!< 結果?示画面 終了・廃棄
	STATE_EXIT				//!< ゲ??終了
};

//! @brief 状態遷移クラス
//! @details ゲ??全体の画面遷移に関わる管理を行います。
class StateMachine
{
	int StateMode;	//!< 状態遷移モ?ド
	int NowState;	//!< 現在のステ?ト
	bool back;		//!< ESCキ?が押された
	bool f12;		//!< F12キ?が押された

public:
	StateMachine();
	~StateMachine();
	void SetStateMode(int mode);
	void NextState();
	void PushMouseButton();
	void PushBackSpaceKey();
	void PushF12Key();
	int GetState();
};

#endif