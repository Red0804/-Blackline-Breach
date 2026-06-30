//! @file gui-object.cpp
//! @brief gui-object関連クラスの定?

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

#include "gui-object.h"

//! @brief コンストラク?
ScrollbarObject::ScrollbarObject()
{
	pos_x = 0;
	pos_y = 0;
	width = 0;
	height = 0;
	totalitems = 0;
	showtems = 0;
	scrollitem = 0;
	knob_height = 0.0f;
	knob_scale = 0.0f;
	knob_y = 0;
	mouse_mode = 0;
	color_body = 0x00000000;
	color1_default = 0x00000000;
	color2_default = 0x00000000;
	color1_cursor = 0x00000000;
	color2_cursor = 0x00000000;
	color1_select = 0x00000000;
	color2_select = 0x00000000;
}

//! @brief ディストラク?
ScrollbarObject::~ScrollbarObject()
{
	//
}

//! @brief スクロ?ルバ?初期化
//! @param x x座標
//! @param y y座標
//! @param w 幅
//! @param h 高さ
//! @param in_totalitems 総アイテ?数
//! @param in_showtems 画面に?示するアイテ?数
//! @param in_scrollitem スクロ?ルしたアイテ?数
//! @warning サイズは幅・高さそれぞれ 8以上 に設定してください。
//! @attention SetColor()関数でスクロ?ルバ?の色を設定してください。
void ScrollbarObject::Create(int x, int y, int w, int h, int in_totalitems, int in_showtems, int in_scrollitem)
{
	pos_x = x;
	pos_y = y;
	width = w;
	height = h;
	totalitems = in_totalitems;
	showtems = in_showtems;
	scrollitem = in_scrollitem;
	if( totalitems < showtems ){
		showtems = totalitems;
		knob_height = (float)h;
		knob_scale = 1.0f;
	}
	else{
		knob_height = (float)h / totalitems * showtems;
		knob_scale = ((float)h - knob_height) / (totalitems - showtems);
	}
	knob_y = (int)Round(knob_scale * scrollitem);
	mouse_mode = 0;
}

//! @brief スクロ?ルバ?の色を設定
//! @param body ?体（後ろ側）の色
//! @param default1 通常時のつまみの色（外側）
//! @param default2 通常時のつまみの色（内側）
//! @param cursor1 カ??ル選択時のつまみの色（外側）
//! @param cursor2 カ??ル選択時のつまみの色（内側）
//! @param select1 クリック時のつまみの色（外側）
//! @param select2 クリック時のつまみの色（内側）
void ScrollbarObject::SetColor(int body, int default1, int default2, int cursor1, int cursor2, int select1, int select2)
{
	color_body = body;
	color1_default = default1;
	color2_default = default2;
	color1_cursor = cursor1;
	color2_cursor = cursor2;
	color1_select = select1;
	color2_select = select2;
}

//! @brief スクロ?ルしたアイテ?数設定
//! @param id アイテ?数
void ScrollbarObject::SetScrollItem(int id)
{
	if( (id < 0)||(totalitems <= id) ){ return; }

	scrollitem = id;
}

//! @brief スクロ?ルしたアイテ?数取得
//! @return アイテ?数
int ScrollbarObject::GetScrollItem()
{
	return scrollitem;
}

//! @brief 上方向へスクロ?ル
void ScrollbarObject::ScrollUP()
{
	if( scrollitem > 0 ){
		scrollitem -= 1;
	}
}

//! @brief 下方向へスクロ?ル
void ScrollbarObject::ScrollDOWN()
{
	if( scrollitem < (totalitems - showtems) ){
		scrollitem += 1;
	}
}

//! @brief スクロ?ル中か判定
//! @return スクロ?ル中である：true?スクロ?ル中でない：false
bool ScrollbarObject::CheckScrolling()
{
	if( mouse_mode == 2 ){ return true; }
	return false;
}

//! @brief スクロ?ルバ?入力処理
//! @param mouse_x ?ウスx座標
//! @param mouse_y ?ウスy座標
//! @param mouse_click ?ウスクリックフラグ
//! @param scroll_y y方向のスクロ?ル量
void ScrollbarObject::Input(int mouse_x, int mouse_y, bool mouse_click, int scroll_y)
{
	//mouse_mode = 0 : ?ウスカ??ルがスクロ?ルバ?のエリア外
	//mouse_mode = 1 : ?ウスカ??ルがスクロ?ルバ?に乗っている
	//mouse_mode = 2 : ?ウスカ??ルがスクロ?ルバ?をクリックしている

	//スクロ?ルバ?の?作判定
	if( mouse_mode == 2 ){
		if( mouse_click == false ){
			mouse_mode = 0;	//or 1
		}
	}
	if( mouse_mode != 2 ){
		if( (pos_x < mouse_x)&&(mouse_x < (pos_x+width))
			&&(pos_y + (int)(knob_scale * scrollitem) < mouse_y)&&(mouse_y < pos_y + (int)(knob_scale * scrollitem + knob_height))
		){
			if( mouse_click == true ){
				mouse_mode = 2;
			}
			else{
				mouse_mode = 1;
			}
		}
		else{
			mouse_mode = 0;
		}
	}

	//スクロ?ルバ?の移動
	if( mouse_mode == 2 ){
		knob_y += scroll_y;
		if( knob_y < 0 ){ knob_y = 0; }
		if( knob_y > (height - (int)knob_height) ){ knob_y = (height - (int)knob_height); }

		scrollitem = (int)Round((float)knob_y / knob_scale);
		if( scrollitem < 0 ){ scrollitem = 0; }
		if( scrollitem > (totalitems - showtems) ){ scrollitem = (totalitems - showtems); }
	}
	else{
		knob_y = (int)Round(knob_scale * scrollitem);
	}
}

//! @brief スクロ?ルバ??画処理
//! @param d3dg D3DGraphicsの?イン?
//! @attention 「合計アイテ?数＜?示するアイテ?数」の場合、ノブは?示されません。
void ScrollbarObject::Draw(class D3DGraphics *d3dg)
{
	if( d3dg == NULL ){ return; }

	int color, color2;

	//エリア?画
	d3dg->Draw2DBox(pos_x, pos_y, pos_x+width, pos_y+height, color_body);

	//ノブ?画
	if( totalitems > showtems ){
		//色を設定
		if( mouse_mode == 2 ){
			color = color1_select;
			color2 = color2_select;
		}
		else if( mouse_mode == 1 ){
			color = color1_cursor;
			color2 = color2_cursor;
		}
		else{
			color = color1_default;
			color2 = color2_default;
		}

		//?画
		d3dg->Draw2DBox(pos_x, pos_y+knob_y, pos_x+width, pos_y+knob_y + (int)knob_height, color);
		d3dg->Draw2DBox(pos_x+3, pos_y+knob_y+3, pos_x+width-3, pos_y+knob_y + (int)knob_height -3, color2);
	}
}

//! @brief コンストラク?
TextBoxObject::TextBoxObject()
{
	mode = 0;
	inputstr[0] = '\0';
	maxlen = MAX_INPUTBOXLEN;
}

//! @brief ディストラク?
TextBoxObject::~TextBoxObject()
{}

//! @brief 入力?ックス初期化
//! @param str デフォルト文字列
//! @param in_maxlen 最大文字数（1?127）
//! @param in_mode 入力モ?ド
//! @note 入力モ?ドは、以下の値を組み合わせて指定します。
//! @note 0x01：数字、0x02：数字/-/.、0x04：アルフ?ベット小文字、0x08：アルフ?ベット大文字、0x20：スペ?ス
void TextBoxObject::InitTextBox(const char *str, int in_maxlen, int in_mode)
{
	if( in_maxlen < 1 ){ in_maxlen = 1; }
	if( in_maxlen > MAX_INPUTBOXLEN ){ in_maxlen = MAX_INPUTBOXLEN; }

	mode = in_mode;
	SetTextBoxStr(str);
	maxlen = in_maxlen;
}

//! @brief 入力?ックス解放
void TextBoxObject::DestroyTextBox()
{}

//! @brief 入力?ックス文字列を設定
//! @param str 文字列
void TextBoxObject::SetTextBoxStr(const char *str)
{
	if( str == NULL ){ str = ""; }
	if( (int)strlen((char*)str) > maxlen ){ return; }

	strcpy(inputstr, (char*)str);
}

//! @brief 入力?ックス文字列を取得
//! @return 文字列の?イン?
const char* TextBoxObject::GetTextBoxStr()
{
	return inputstr;
}

//! @brief 文字を一文字追加
//! @param inchar 追加する文字
void TextBoxObject::InputChar(char inchar)
{
	for(int i=0; i<maxlen; i++){
		if( inputstr[i] == '\0' ){
			inputstr[i] = inchar;
			inputstr[i+1] = '\0';
			return;
		}
	}
}

//! @brief 文字を一文字削除
void TextBoxObject::DeleteChar()
{
	int s = (int)strlen(inputstr);
	if( s == 0 ){ return; }
	inputstr[ s-1 ] = '\0';
}

//! @brief 入力?ックス文字列処理
//! @param inputCtrl InputControlクラスの?イン?
//! @return 入力済み状態でENTERキ?：true?それ以外（エラ?含む）：false
//! @attention 事前にInputControlクラスを適切に処理しておく必要があります。?※GetInputState()関数など。
bool TextBoxObject::ProcessTextBox(InputControl *inputCtrl)
{
	if( inputCtrl == NULL ){ return false; }

	if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x0F)) ){		// [ENTER]
		//何か入力されていれば?
		if( inputstr[0] != '\0' ){
			return true;
		}
	}
	else{
		//一文字削除
		if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x0E)) ){		// [BACK]
			DeleteChar();
		}

		if( (mode & 0x01)||(mode & 0x02) ){
			//数字
			for(char key=0x16; key<=0x1F; key++){
				if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(key)) ){
					InputChar(key + 0x1A);
				}
			}
			for(char key=0x04; key<=0x0D; key++){
				if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(key)) ){
					InputChar(key + 0x2C);
				}
			}

			if( mode & 0x02 ){
				//if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x48)) ){ InputChar('+'); }
				if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x3D)) ){ InputChar('-'); }
				if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x49)) ){ InputChar('-'); }
				if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x44)) ){ InputChar('.'); }
				if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x4A)) ){ InputChar('.'); }
			}
		}

		if( (mode & 0x04)&&(mode & 0x08) ){
			if( inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x14)) ){	// [SHIFT]
				//アルフ?ベット大文字
				for(char key=0x20; key<=0x39; key++){
					if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(key)) ){
						InputChar(key + 0x21);
					}
				}
			}
			else{
				//アルフ?ベット小文字
				for(char key=0x20; key<=0x39; key++){
					if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(key)) ){
						InputChar(key + 0x41);
					}
				}
			}
		}
		else if( mode & 0x04 ){
			//アルフ?ベット小文字
			for(char key=0x20; key<=0x39; key++){
				if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(key)) ){
					InputChar(key + 0x41);
				}
			}
		}
		else if( mode & 0x08 ){
			//アルフ?ベット大文字
			for(char key=0x20; key<=0x39; key++){
				if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(key)) ){
					InputChar(key + 0x21);
				}
			}
		}

		//if( mode & 0x10 ){
		//	if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x43)) ){ InputChar(','); }
		//}

		if( mode & 0x20 ){
			//スペ?ス
			if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x11)) ){		// [SPACE]
				InputChar(' ');
			}
		}
	}

	return false;
}
