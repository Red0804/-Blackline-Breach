//! @file gui-object.h
//! @brief gui-object関連クラスの宣言

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

#ifndef OBJECT2D_H
#define OBJECT2D_H

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 2		//!< Select include file.
#endif
#include "main.h"

#define MAX_INPUTBOXLEN 127				//!< 入力?ックス文字列初期化時の最大文字数

//! @brief スクロ?ルバ?の管理?画クラス
//! @details スクロ?ルバ?の処理や?画を行います。
//! @attention 縦/Y軸方向のスクロ?ルしか想定していません。
class ScrollbarObject
{
	int pos_x;				//!< x座標
	int pos_y;				//!< y座標
	int width;				//!< 幅
	int height;				//!< 高さ
	int totalitems;			//!< 合計アイテ?数
	int showtems;			//!< ?示するアイテ?数
	int scrollitem;			//!< スクロ?ルしたアイテ?数
	float knob_height;		//!< ノブの高さ
	float knob_scale;		//!< ノブの目盛
	int knob_y;				//!< ノブのY座標
	int mouse_mode;			//!< ?ウスによる選択状況
	int color_body;			//!< ?体の色
	int color1_default;		//!< 通常時の色（外側）
	int color2_default;		//!< 通常時の色（内側）
	int color1_cursor;		//!< カ??ル選択時の色（外側）
	int color2_cursor;		//!< カ??ル選択時の色（内側）
	int color1_select;		//!< クリック時の色（外側）
	int color2_select;		//!< クリック時の色（内側）

public:
	ScrollbarObject();
	~ScrollbarObject();
	void Create(int x, int y, int w, int h, int in_totalitems, int in_showtems, int in_scrollitem);
	void SetColor(int body, int default1, int default2, int cursor1, int cursor2, int select1, int select2);
	void SetScrollItem(int id);
	int GetScrollItem();
	void ScrollUP();
	void ScrollDOWN();
	bool CheckScrolling();
	void Input(int mouse_x, int mouse_y, bool mouse_click, int scroll_y);
	void Draw(class D3DGraphics *d3dg);
};

//! @brief 入力?ックスの文字管理クラス
//! @details 入力?ックスの文字入力を管理します。
//! @attention ?画??は含まれていません。
class TextBoxObject
{
	int mode;							//!< 入力モ?ド
	char inputstr[MAX_INPUTBOXLEN+1];	//!< 入力?ックスの文字列
	int maxlen;							//!< 入力?ックスの最大文字数

	void InputChar(char inchar);
	void DeleteChar();

public:
	TextBoxObject();
	~TextBoxObject();
	void InitTextBox(const char *str, int in_maxlen, int in_mode);
	void DestroyTextBox();
	void SetTextBoxStr(const char *str);
	const char* GetTextBoxStr();
	bool ProcessTextBox(InputControl *inputCtrl);
};

#endif