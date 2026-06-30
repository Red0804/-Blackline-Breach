//! @file event.h
//! @brief EventControlクラスの宣言

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

#ifndef EVENT_H
#define EVENT_H

#define TOTAL_EVENTFRAMESTEP 6			//!< 1フレ??中に1ラインあたり実行するステップ数
#define DISTANCE_CHECKPOINT 25.0f		//!< 「到着待ち」「ケ?ス待ち」の判定距離

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 3		//!< Select include file.
#endif
#include "main.h"

//! イベントパラメ???用の?造体
typedef struct
{
	int endcnt;				//!< 終了カウント
	bool complete;			//!< ?ッション成功フラグ
	int MessageID;			//!< イベントメッセ?ジ番号
	bool SetMessageID;		//!< イベントメッセ?ジフラグ
} EventParameter;

//! @brief イベントを処理するクラス
//! @details ?ッションのイベント?イントを処理します。
//! @details このクラスは1つのイベント処理（流れ）に専念します。複数のイベントを並列に処理させる場合は、このクラスのオブジェクトを複数呼び出す必要があります。（例えば3つ同時に処理させる場合、このクラスのオブジェクトを3つ作成してください。）
class EventControl
{
	class PointDataInterface *Point;		//!< PointDataInterfaceクラスの?イン?
	class ObjectManager *ObjMgr;			//!< ObjectManagerクラスの?イン?
	signed short int nextp4;				//!< 次に処理するP4：識別番号
	int waitcnt;							//!< 時間待ち用カウント

	bool CheckDead(signed short int targetP4, bool reverse);
	bool CheckArrival(float pos_x, float pos_y, float pos_z, float dist_r, signed short int targetP4, bool reverse);
	bool CheckHaveWeapon(int WeaponID, signed short int targetP4, bool reverse);
	bool SetMovePathMode(signed short int targetP4, int newmode);
	bool SetTeamID(signed short int targetP4, int TeamID);
	bool CheckBreakSmallObject(signed short int targetP4, bool reverse);
	bool WaitSec(int sec);
	void SetMessage(int SetID, int *MessageID, bool *SetMessageID);

public:
	EventControl(PointDataInterface *in_Point = NULL, ObjectManager *in_ObjMgr = NULL);
	~EventControl();
	void SetClass(PointDataInterface *in_Point, ObjectManager *in_ObjMgr);
	void Reset(signed short int EntryP4);
	signed short int GetNextP4();
	int ProcessEventPoint(bool SkipFlag, EventParameter *Param);
};

#endif