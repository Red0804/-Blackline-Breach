//! @file main.h
//! @brief 띍뤵댧궻긶긞??긲?귽깑

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

#ifndef MAIN_H
#define MAIN_H

#define MAINICON 101		//!< Icon

#ifdef _DEBUG
 #define _CRTDBG_MAP_ALLOC
 #include <crtdbg.h>
#endif

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 3		//!< Select include file.
#endif

#pragma warning(disable:4996)		//VC++똸뜍뻞?
//#pragma warning(disable:4244)

//믦릶
#define GAMENAME "Blackline Breach"				//!< 게임명
#define GAMEVERSION "EX6"						//!< 게임의 버전
#define GAMEFRAMEMS 30					//!< 기존 게임 내부 기준 프레임 시간(ms)
#define GAMEFPS (1000.0f/GAMEFRAMEMS)	//!< 기존 게임 내부 기준 FPS. 1000 / 30 = 33.333FPS

// 실제 메인 루프에서 로직을 실행할 간격.
// 30이면 원래 의도된 속도.
// 예전 ControlFps 체감에 맞추고 싶으면 32~33을 테스트한다.
// 기존 게임 체감에 맞춘 실제 로직 실행 FPS.
// 게임 내부 시간 표시가 (int)GAMEFPS == 33 기준으로 되어 있으므로,
// 실제 로직도 평균 33FPS에 맞춘다.
#define LOGICFPS_LEGACY 33.0
#define LOGICFRAMEMS_D (1000.0 / LOGICFPS_LEGACY)

// 화면 렌더링 제한.
// 게임 로직 속도와는 별개이다.
#define RENDERFPS_DEFAULT 144
#define RENDERFPS_MIN 30
#define RENDERFPS_MAX 240

// 렉이 걸렸을 때 한 루프에서 로직을 너무 많이 따라잡지 않도록 제한
#define MAX_LOGIC_STEPS_PER_LOOP 5
#define DEFAULT_SCREEN_WIDTH 1920		//!< 긚긏깏?깛궻븴
#define DEFAULT_SCREEN_HEIGHT 1080		//!< 긚긏깏?깛궻뛼궠

#define ADDONDIR "addon"		//!< ADDON귩볺귢귡긢귻깒긏긣깏뼹
#define MAX_ADDONLIST 128		//!< ADDON귩벶귒뜛귔띍묈릶

#define TOTAL_HAVEWEAPON 2		//!< 보유 무기 슬롯 수

//긘긚긡?궻귽깛긏깑?긤긲?귽깑
#define _USE_MATH_DEFINES	 //!< math.h 'M_PI' enable
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <direct.h>

#define ENABLE_BUG_HUMANWEAPON			//!< 붝댪둖궻릐갋븧딇롰쀞붥뜂귩뾭궋궫긫긐귩띋뙸궥귡걁긓긽깛긣돸궳뼰뚼걂
#define ENABLE_BUG_TEAMID				//!< ???붥뜂귩븠릶궸먠믦궢궫띧궻긫긐귩띋뙸궥귡걁긓긽깛긣돸궳뼰뚼걂
#define ENABLE_WEAPONNAME_SCALING		//!< 븧딇뼹궕뮮궋뤾뜃궸?렑귩뢫룷궥귡걁긓긽깛긣돸궳뼰뚼걂
#define ENABLE_ADDOBJ_PARAM8BIT			//!< 믁돿룷븿궻먠믦뭠귩char? 8bit궸궥귡걁긓긽깛긣돸궳뼰뚼걂
#define ENABLE_DEBUGCONSOLE				//!< 긢긫긞긏뾭긓깛??깑궻뾎뚼돸걁긓긽깛긣돸궳??뼰뚼걂
#define ENABLE_DEBUGLOG					//!< 긢긫긞긏뾭깓긐뢯쀍궻뾎뚼돸걁긓긽깛긣돸궳??뼰뚼걂
#define ENABLE_CHECKOPENXOPSEVENT		//!< OpenXOPS벍띿뵽믦뾭귽긹깛긣궻뾎뚼돸걁긓긽깛긣돸궳??뼰뚼걂
#define ENABLE_MENUOPTIONS				//!< 긽긦깄?궸긆긵긘깈깛됪뽋믁돿걁긓긽깛긣돸궳??뼰뚼걂
//#define ENABLE_AUTOCREATECONFIG		//!< config.dat궕뙥궰궔귞궶궋뤾뜃갂벏긲?귽깑귩렔벍맯맟궥귡갃
//#define ENABLE_PATH_DELIMITER_SLASH	//!< 긬긚뗦먛귟빒럻귩갂'\'궔귞갻/갽귉빾듂궥귡갃
#define ENABLE_AUTOLOADMIF				//!< .mif궕덙릶궸럚믦궠귢궫뤾뜃갂렔벍밒궸깓?긤궥귡걁긓긽깛긣돸궳??뼰뚼걂
#define ENABLE_ADDCONFIG				//!< ini긲?귽깑궸귝귡믁돿먠믦귩롦벦궥귡걁긓긽깛긣돸궳??뼰뚼걂
#define ENABLE_CHECKDATADIR				//!< 딳벍렄궸data긲긅깑??궻뾎뼰귩?긃긞긏궥귡걁긓긽깛긣돸궳??뼰뚼걂

//windows.h귩럊뾭궢궶궋궶귞궽
#ifndef _MAX_PATH
 #define _MAX_PATH 260		//!< _MAX_PATH is 260
#endif
#ifndef MAX_PATH
 #define MAX_PATH 260		//!< MAX_PATH is 260
#endif

//BorlandC++ Compiler뾭궻룉뿚
#ifdef __BORLANDC__
 //#define sqrtf(df) sqrt(df)
 int _matherr(struct _exception  *__e) { return 1; }
#endif

//뼟럊뾭덙릶궻뫮랉뾭?긏깓
#ifndef MAIN_H
 #define UNREFERENCED_PARAMETER(P) ((P)=(P))
#endif

//float?궻럁뢱듫릶뾭댰
#ifndef _MSC_VER
 #define sinf(x) (float)sin(x)				//!< sinf(x)
 #define cosf(x) (float)cos(x)				//!< cosf(x)
 #define asinf(x) (float)asin(x)			//!< asinf(x)
 #define acosf(x) (float)acos(x)			//!< acosf(x)
 #define atan2f(y,x) (float)atan2(y,x)		//!< atan2f(y,x)
 #define sqrtf(x) (float)sqrt(x)			//!< sqrtf(x)
 #define floorf(x) (float)floor(x)			//!< floorf(x)
 #define ceilf(x) (float)ceil(x)			//!< ceilf(x)
 #define fabsf(x) (float)fabs(x)			//!< fabsf(x)
#endif

#define SetFlag(value, bit) value = value | bit		//!< 긮긞긣궸귝귡긲깋긐 먠믦
#define DelFlag(value, bit) value = value & (~bit)	//!< 긮긞긣궸귝귡긲깋긐 됶룣
#define GetFlag(value, bit) (value & bit)			//!< 긮긞긣궸귝귡긲깋긐 롦벦

#define DegreeToRadian(value) ((float)M_PI/180*value)	//!< 뱗궔귞깋긙귺깛귉궻빾듂

//믟깒귽깂?
#if H_LAYERLEVEL >= 1
 #include "debug.h"
 #include "window.h"
 #include "config.h"
 #include "datafile.h"
 #include "d3dgraphics.h"
 #include "input.h"
 #include "sound.h"
 #include "parameter.h"
#endif

//뭷듩깒귽깂?
#if H_LAYERLEVEL >= 2
 #include "resource.h"
 #include "collision.h"
 #include "object.h"
 #include "soundmanager.h"
 #include "gui-object.h"
#endif

//뛼깒귽깂?
#if H_LAYERLEVEL >= 3
 #include "objectmanager.h"
 #include "scene.h"
 #include "gamemain.h"
 #include "ai.h"
 #include "statemachine.h"
 #include "event.h"
#endif

//믁돿깋귽긳깋깏
// not .lib

#ifdef ENABLE_DEBUGLOG
 extern class DebugLog OutputLog;
#endif
extern class StateMachine GameState;
extern class Config GameConfig;

bool CheckCommandParameter(const char *param, const char *cmd);
float AngleNormalization(float r);
unsigned int PointParamToInt(signed short int value);
void Set2ByteLittleEndian(unsigned char *outdata, unsigned int value);
void Set4ByteLittleEndian(unsigned char *outdata, unsigned int value);
void Set2ByteBigEndian(unsigned char *outdata, unsigned int value);
void Set4ByteBigEndian(unsigned char *outdata, unsigned int value);
unsigned int Get2ByteLittleEndian(unsigned char *indata);
unsigned int Get4ByteLittleEndian(unsigned char *indata);
unsigned int Get2ByteBigEndian(unsigned char *indata);
unsigned int Get4ByteBigEndian(unsigned char *indata);

#endif