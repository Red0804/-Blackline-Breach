//! @file gamemain.h
//! @brief 긒??긽귽깛룉뿚궻긶긞??

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

#ifndef GAMEMAIN_H
#define GAMEMAIN_H

// 타이틀 메인 화면 UI 스케일.
// 기존 640x480~720p에서는 1.0배를 유지하고,
// 고해상도에서는 최대 1.5배까지만 커지도록 제한한다.
#define MAINMENU_SCALE_RAW ((float)GameConfig.GetScreenHeight() / 720.0f)
#define MAINMENU_SCALE ((MAINMENU_SCALE_RAW < 1.0f) ? 1.0f : ((MAINMENU_SCALE_RAW > 1.5f) ? 1.5f : MAINMENU_SCALE_RAW))

#define MAX_FULLMAP_FLOORS 32

#define TOTAL_MENUITEMS 8								//!< 긽긦깄?1됪뽋궸?렑궥귡?긞긘깈깛릶

#define MAINMENU_ROW_H ((int)(30 * MAINMENU_SCALE))
#define MAINMENU_SWITCH_H ((int)(25 * MAINMENU_SCALE))
#define MAINMENU_W ((int)(360 * MAINMENU_SCALE))
#define MAINMENU_LIST_W ((int)(340 * MAINMENU_SCALE))
#define MAINMENU_SCROLLBAR_W ((int)(19 * MAINMENU_SCALE))
#define MAINMENU_H ((TOTAL_MENUITEMS + 2) * MAINMENU_ROW_H + MAINMENU_SWITCH_H)

#define MAINMENU_X (GameConfig.GetScreenWidth() - MAINMENU_W)	//!< 긽긦깄?궻?렑 X띆뷭걁뜺뤵딈?걂
#define MAINMENU_Y (GameConfig.GetScreenHeight() - MAINMENU_H - (int)(15 * MAINMENU_SCALE))	//!< 긽긦깄?궻?렑 Y띆뷭걁갫걂

#define MAINMENU_TITLE_FONT_W ((int)(25 * MAINMENU_SCALE))
#define MAINMENU_TITLE_FONT_H ((int)(26 * MAINMENU_SCALE))
#define MAINMENU_MISSION_FONT_W ((int)(20 * MAINMENU_SCALE))
#define MAINMENU_MISSION_FONT_H ((int)(26 * MAINMENU_SCALE))
// 미션 목록 전환 버튼용 글자 크기.
// STANDARD / TRAINING / ADD-ON이 3칸 안에 들어가도록 폭을 줄인다.
#define MAINMENU_SWITCH_FONT_W ((int)(12 * MAINMENU_SCALE))
#define MAINMENU_SWITCH_FONT_H ((int)(22 * MAINMENU_SCALE))

// 메인 메뉴 미션 목록 모드
#define MISSION_LIST_STANDARD 0
#define MISSION_LIST_BASIC    1
#define MISSION_LIST_ADDON    2

// TRAINING 내부 탭
#define TRAINING_TAB_TUTORIAL 0
#define TRAINING_TAB_SKILL    1
#define TRAINING_TAB_DATAS    2

// TRAINING 탭 버튼 위치
// 메인 미션 목록 바로 위에 표시한다.
#define TRAINING_TAB_H MAINMENU_SWITCH_H
#define TRAINING_TAB_Y (MAINMENU_Y - TRAINING_TAB_H - (int)(4 * MAINMENU_SCALE))

// 타이틀 화면 왼쪽 아래의 < OPTIONS > 버튼.
// 타이틀 메인 메뉴와 같은 스케일을 사용한다.
#define MAINMENU_OPTION_TEXT "< OPTIONS >"
#define MAINMENU_OPTION_TEXT_LEN 11
#define MAINMENU_OPTION_FONT_W ((int)(14 * MAINMENU_SCALE))
#define MAINMENU_OPTION_FONT_H ((int)(20 * MAINMENU_SCALE))
#define MAINMENU_OPTION_PAD_X ((int)(8 * MAINMENU_SCALE))
#define MAINMENU_OPTION_PAD_Y ((int)(4 * MAINMENU_SCALE))
#define MAINMENU_OPTION_W (MAINMENU_OPTION_TEXT_LEN * MAINMENU_OPTION_FONT_W + MAINMENU_OPTION_PAD_X * 2)
#define MAINMENU_OPTION_H (MAINMENU_OPTION_FONT_H + MAINMENU_OPTION_PAD_Y * 2)
#define MAINMENU_OPTION_X ((int)(10 * MAINMENU_SCALE))
#define MAINMENU_OPTION_Y (GameConfig.GetScreenHeight() - MAINMENU_OPTION_H - (int)(10 * MAINMENU_SCALE))

// 타이틀 화면 왼쪽 아래의 < SKILLS > 버튼.
// OPTIONS 버튼 오른쪽에 배치한다.
#define MAINMENU_SKILLS_TEXT "< SKILLS >"
#define MAINMENU_SKILLS_TEXT_LEN 10
#define MAINMENU_SKILLS_W (MAINMENU_SKILLS_TEXT_LEN * MAINMENU_OPTION_FONT_W + MAINMENU_OPTION_PAD_X * 2)
#define MAINMENU_SKILLS_H MAINMENU_OPTION_H
#define MAINMENU_SKILLS_X (MAINMENU_OPTION_X + MAINMENU_OPTION_W + (int)(8 * MAINMENU_SCALE))
#define MAINMENU_SKILLS_Y MAINMENU_OPTION_Y

#define INPUT_ARROWKEYS_ANGLE DegreeToRadian(4)		//!< 뺴뛀긌?궳궻됷?둷뱗
#define INPUT_F1NUMKEYS_ANGLE DegreeToRadian(2)		//!< 랳릐뤝럨?궳궻긡깛긌?궻됷?둷뱗

#define HUDA_WEAPON_POSX (GameConfig.GetScreenWidth() - 255)	//!< 븧딇륃뺪귩?됪궥귡쀌덃갋X띆뷭
#define HUDA_WEAPON_POSY (GameConfig.GetScreenHeight() - 98)	//!< 븧딇륃뺪귩?됪궥귡쀌덃갋Y띆뷭
#define HUDA_WEAPON_SIZEW 8										//!< 븧딇륃뺪귩?됪궥귡쀌덃갋돘긖귽긛걁32긯긏긜깑궻봹뭫뙿릶걂
#define HUDA_WEAPON_SIZEH 3										//!< 븧딇륃뺪귩?됪궥귡쀌덃갋뢢긖귽긛걁32긯긏긜깑궻봹뭫뙿릶걂

#define VIEW_HEIGHT 19.0f							//!< 럨?궻뛼궠
#define VIEW_DIST 0.1f								//!< 뭷륲궔귞럨?귏궳궻떁뿣
#define VIEW_F1MODE_ANGLE DegreeToRadian(-22.5f)	//!< 랳릐뤝럨?궳궻럨?둷뱗걁룊딖걂
#define VIEW_F1MODE_DIST 14.0f						//!< 랳릐뤝럨?궳궻럨?떁뿣걁띍묈걂
#define VIEW_FREECAMERA_SCALE 1.5f					//!< 긲깏?긇긽깋궳궻댷벍뫊뱗걁뛼뫊긾?긤궼?2걂
#define VIEWANGLE_NORMAL DegreeToRadian(65)			//!< 뷭?밒궶럨뽰둷

#define TOTAL_EVENTLINE 3				//!< 귽긹깛긣궻깋귽깛릶
#define TOTAL_EVENTENTRYPOINT_0 -100	//!< 깋귽깛 0 궻둎럑렞빶붥뜂
#define TOTAL_EVENTENTRYPOINT_1 -110	//!< 깋귽깛 1 궻둎럑렞빶붥뜂
#define TOTAL_EVENTENTRYPOINT_2 -120	//!< 깋귽깛 2 궻둎럑렞빶붥뜂

#define TOTAL_EVENTENT_SHOWMESSEC 5.0f		//!< 귽긹깛긣긽긞긜?긙귩?렑궥귡뷳릶

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 3		//!< Select include file.
#endif
#include "main.h"
#include "playerskill.h"

#ifdef ENABLE_MENUOPTIONS
 #define OPTIONS_P1_W 340															//!< 긆긵긘깈깛됪뽋 No.1?븴
 #define OPTIONS_P1_H (30*7)														//!< 긆긵긘깈깛됪뽋 No.1?뛼궠
 #define OPTIONS_P1_X (GameConfig.GetScreenWidth() - 360)							//!< 긆긵긘깈깛됪뽋 No.1?X띆뷭
 #define OPTIONS_P1_Y (MAINMENU_Y+MAINMENU_H - OPTIONS_P1_H - 24)					//!< 긆긵긘깈깛됪뽋 No.1?Y띆뷭
#define OPTIONS_P1_DATAS 7															//!< 옵션 메인 항목 개수
 #define OPTIONS_P2_W 600															//!< 긆긵긘깈깛됪뽋 No.2?븴
 #define OPTIONS_P2_H 360															//!< 긆긵긘깈깛됪뽋 No.2?뛼궠
 #define OPTIONS_P2_X ((GameConfig.GetScreenWidth() - OPTIONS_P2_W)/2)				//!< 긆긵긘깈깛됪뽋 No.2?X띆뷭
 #define OPTIONS_P2_Y (105 + (GameConfig.GetScreenHeight()-105 - OPTIONS_P2_H)/2)	//!< 긆긵긘깈깛됪뽋 No.2?Y띆뷭
 #define OPTIONS_P2_DATAS 32														//!< 긆긵긘깈깛됪뽋 No.2?깏깛긏긡긌긚긣궻릶
#define OPTIONS_P3_W 760
#define OPTIONS_P3_H 390
 #define OPTIONS_P3_X ((GameConfig.GetScreenWidth() - OPTIONS_P3_W)/2)				//!< 긆긵긘깈깛됪뽋 No.3?X띆뷭
 #define OPTIONS_P3_Y (105 + (GameConfig.GetScreenHeight()-105 - OPTIONS_P3_H)/2)	//!< 긆긵긘깈깛됪뽋 No.3?Y띆뷭
 #define OPTIONS_P3_DATAS 1															//!< 긆긵긘깈깛됪뽋 No.3?깏깛긏긡긌긚긣궻릶
#define OPTIONS_ADVANCED_DATAS 22  // Gameplay Setting 항목 개수
#define OPTIONS_HUD_DATAS 7         // HUD Setting 항목 개수
#define OPTIONS_TOGGLE_DATAS 7      // Toggle Setting 항목 개수
#define OPTIONS_CROSSHAIR_DATAS 23  // style10 + color10 + outline + reset + back

// SKILLS 화면
#define SKILLINFO_GROUP_DATAS 12
#define SKILLINFO_GROUP_LINK_DATAS (SKILLINFO_GROUP_DATAS + 1) // 12개 그룹 + BACK

// 화면이 너무 작을 때는 화면 안에 들어오도록 줄이고,
// 일반 해상도에서는 기존보다 크게 표시한다.
#define SKILLINFO_W ((GameConfig.GetScreenWidth() < 1000) ? (GameConfig.GetScreenWidth() - 80) : 900)
#define SKILLINFO_H ((GameConfig.GetScreenHeight() < 720) ? (GameConfig.GetScreenHeight() - 130) : 600)
#define SKILLINFO_X ((GameConfig.GetScreenWidth() - SKILLINFO_W) / 2)
#define SKILLINFO_Y (105 + (GameConfig.GetScreenHeight() - 105 - SKILLINFO_H) / 2)

#define SKILLINFO_LINE_H 26
#define SKILLINFO_DETAIL_VISIBLE_LINES 14
#define SKILLINFO_DETAIL_TOTAL_LINES 24

// SKILLS 언어 선택
#define SKILLINFO_LANG_DATAS 3
#define SKILLINFO_LANG_KO 0
#define SKILLINFO_LANG_EN 1
#define SKILLINFO_LANG_JP 2
 #define OPTIONS_ADVANCED_X     120
 #define SLIDER_WIDTH           200 // 슬라이더 바 길이

 #if (GRAPHIC_ENGINE == 0)||(GRAPHIC_ENGINE == 1)
 #define OPTIONS_LICENSELINE 26											//!< 깋귽긜깛긚빒궻뛱릶
#elif GRAPHIC_ENGINE == 8
 #define OPTIONS_LICENSELINE 6											//!< 깋귽긜깛긚빒궻뛱릶
#endif	//GRAPHIC_ENGINE
#endif	//ENABLE_MENUOPTIONS

#ifdef ENABLE_DEBUGCONSOLE
 #define MAX_CONSOLELEN 78		//!< 긢긫긞긏뾭긓깛??깑궻빒럻릶걁뛱걂
 #define MAX_CONSOLELINES 12	//!< 긢긫긞긏뾭긓깛??깑궻뛱릶
 #define CONSOLE_PROMPT ">"		//!< 긢긫긞긏뾭긓깛??깑궻긵깓깛긵긣
#endif

int InitGame(WindowControl *WindowCtrl, int mode, char *MIFpath);
void CleanupGame();

//! 긒??궻륉뫴귩롷궚뱊궢궥귡?몾뫬
typedef struct
{
	int selectmission_id;	//!< 멗묖궠귢궫?긞긘깈깛
	char mifpath[MAX_PATH];	//!< MIF긲?귽깑궻긬긚
	bool missioncomplete;	//!< ?긞긘깈깛뒶뿹
	unsigned int framecnt;	//!< 긲깒??릶
	int fire;		//!< 롅똼됷릶
	float ontarget;	//!< 뼺뭷릶
	int kill;		//!< ?궢궫밎궻릶	
	int headshot;	//!< 밎궻벆븫궸뼺뭷궢궫릶
} GameInfo;

#ifdef ENABLE_MENUOPTIONS
//! 긽긦깄?깏깛긏긡긌긚긣?몾뫬
typedef struct
{
	bool enable;	//!< 깏깛긏긡긌긚긣궴궢궲궻뾎뚼돸긲깋긐	
	int pos_x;		//!< X띆뷭
	int pos_y;		//!< Y띆뷭
	int text_w;		//!< 빒럻궻븴
	int text_h;		//!< 빒럻궻뛼궠
	int text_defaultcolor;		//!< 뷭?궻빒럻륡
	int text_cursorcolor;		//!< 멗묖렄궻빒럻륡
	char text[32];				//!< 긡긌긚긣
} MenuLinkTextData;
#endif	//ENABLE_MENUOPTIONS

#ifdef ENABLE_DEBUGCONSOLE
//! 긓깛??깑귩듖뿚궥귡?몾뫬
typedef struct
{
	int color;							//!< 륡
	char textdata[MAX_CONSOLELEN+1];	//!< 빒럻쀱
} ConsoleData;
#endif

//! @brief 긆?긵긦깛긐됪뽋듖뿚긏깋긚
//! @details 긆?긵긦깛긐됪뽋귩듖뿚궢귏궥갃
class opening : public D3Dscene
{
	float add_camera_x;			//!< 긇긽깋띆뷭 X궻몵뙵쀊
	float add_camera_y;			//!< 긇긽깋띆뷭 Y궻몵뙵쀊
	float add_camera_z;			//!< 긇긽깋띆뷭 Z궻몵뙵쀊
	float add_camera_rx;		//!< 긇긽깋됷?둷뱗 RX궻몵뙵쀊
	float add_camera_ry;		//!< 긇긽깋됷?둷뱗 RY궻몵뙵쀊
	//int opening_banner;		//!< 긆?긵긦깛긐궳?렑궥귡긡긏긚?긿ID
	void Render3D();
	void Render2D();

public:
	opening();
	~opening();
	int Create();
	int Recovery();
	void Input();
	void Process();
	void Destroy();
};

//! @brief 긽긦깄?됪뽋듖뿚긏깋긚
//! @details 긽긦깄?됪뽋귩듖뿚궢귏궥갃
class mainmenu : public D3Dscene
{
	char demopath[MAX_PATH];		//!< 긢긾긲?귽깑궻긬긚
	int modescreen;					//!< ?렑됪뽋
	int mainmenu_mouseX;			//!< 긽긦깄?됪뽋?긂긚X띆뷭
	int mainmenu_mouseY;			//!< 긽긦깄?됪뽋?긂긚Y띆뷭
	int mainmenu_mouseY_old;		//!< 긽긦깄?됪뽋?긂긚Y띆뷭걁멟됷궻긲깒??걂
	class ScrollbarObject scrollbar_official;	//!< 뷭??긞긘깈깛궻긚긏깓?깑긫?
	class ScrollbarObject scrollbar_training;	//!< TRAINING 미션 스크롤바
	class ScrollbarObject scrollbar_addon;		//!< addon궻긚긏깓?깑긫?
	int mission_list_mode;			//!< 현재 표시 중인 미션 목록 모드
	int training_list_tab;			//!< TRAINING 내부 탭. 0: tutorial, 1: skill
	int gametitle;					//!< 긒???귽긣깑됪몴
#ifdef ENABLE_MENUOPTIONS
	// [추가] 고급 설정 메뉴용 변수
	MenuLinkTextData Options_AdvancedLinkData[OPTIONS_ADVANCED_DATAS];
	int Options_AdvancedID;    // 현재 선택된 항목 ID
	bool IsDraggingSlider;     // 슬라이더 드래그 중인지 여부
	int DraggingID;            // 드래그 중인 항목 번호

	MenuLinkTextData Options_HudLinkData[OPTIONS_HUD_DATAS];
	int Options_HudID;         // HUD Setting 현재 선택 항목 ID

	MenuLinkTextData Options_ToggleLinkData[OPTIONS_TOGGLE_DATAS];
	int Options_ToggleID;      // Toggle Setting 현재 선택 항목 ID

	MenuLinkTextData Options_p1LinkTextData[OPTIONS_P1_DATAS];
	MenuLinkTextData Options_p2LinkTextData[OPTIONS_P2_DATAS];		//!< 긆긵긘깈깛됪뽋 No.2?깏깛긏긡긌긚긣긢??
	MenuLinkTextData Options_p3LinkTextData[OPTIONS_P3_DATAS];		//!< 긆긵긘깈깛됪뽋 No.3?깏깛긏긡긌긚긣긢??
	class ScrollbarObject scrollbar_license;		//!< 깋귽긜깛긚빒궻긚긏깓?깑긫?

	// SKILLS 설명 화면
	MenuLinkTextData SkillInfo_GroupLinkData[SKILLINFO_GROUP_LINK_DATAS];
	MenuLinkTextData SkillInfo_LangLinkData[SKILLINFO_LANG_DATAS];
	class ScrollbarObject scrollbar_skillinfo;
	int SkillInfo_SelectedGroup;
	int SkillInfo_Language;
	int Options_p2LinkTextID;						//!< 긆긵긘깈깛됪뽋 No.2?돓궠귢궫깏깛긏긡긌긚긣궻ID
	bool Options_FullscreenFlag_old;				//!< 긆긵긘깈깛됪뽋 No.2?긲깑긚긏깏?깛긲깋긐
	int Options_KeyConfigID;						//!< 긆긵긘깈깛됪뽋 No.2?긌?먠믦ID
	int Options_KeyConfigCnt;						//!< 긆긵긘깈깛됪뽋 No.2?긌?먠믦긇긂깛긣걁긲깒??릶걂
	const char *licenseinfo[OPTIONS_LICENSELINE];	//!< 깋귽긜깛긚빒
	char *licenseinfobuf;							//!< ?렑궥귡깋귽긜깛긚빒귩뺎렃궥귡긫긞긲??
#endif	//ENABLE_MENUOPTIONS

	void Render3D();
	void Render2D();
#ifdef ENABLE_MENUOPTIONS
	void CreateOptions();
	void InputOptions();
	void ProcessOptions();
	void RenderOptions();

	bool IsDuplicateControlKey(int targetKeyID, int keycode);
	bool IsBlockedControlKey(int targetKeyID, int keycode);
	bool TryApplyControlKey(int targetKeyID, int keycode);
#endif	//ENABLE_MENUOPTIONS

public:
	mainmenu();
	~mainmenu();
	int Create();
	int Recovery();
	void Input();
	void Process();
	void Destroy();
};

//! @brief 긳깏?긲귻깛긐됪뽋듖뿚긏깋긚
//! @details 긳깏?긲귻깛긐됪뽋귩듖뿚궢귏궥갃
class briefing : public D2Dscene
{
	bool TwoTexture;	//!< 긳깏?긲귻깛긐됪몴귩2뻼럊뾭
	int TextureA;		//!< 긳깏?긲귻깛긐됪몴A
	int TextureB;		//!< 긳깏?긲귻깛긐됪몴B
	void Render2D();

public:
	briefing();
	~briefing();
	int Create();
	int Recovery();
	void Destroy();
};

//! @brief 긽귽깛긒??됪뽋듖뿚긏깋긚
//! @details 긽귽깛긒??됪뽋귩듖뿚궢귏궥갃
class maingame : public D3Dscene
{
	//class EventControl Event[TOTAL_EVENTLINE];	//!< 귽긹깛긣맕뚥긏깋긚
	int SkyNumber;			//!< 봶똧뗴붥뜂
	bool DarkScreenFlag;	//!< 됪뽋귩댠궘
	bool FogFlag;			//!< 긲긅긐뾎뚼
	float mouse_rx;				//!< ?긂긚궸귝귡릣빟렡둷뱗
	float mouse_ry;				//!< ?긂긚궸귝귡릠뮳렡둷뱗
	float view_rx;				//!< ?긂긚둷뱗궴긇긽깋둷뱗궻뜼걁릣빟렡걂
	float view_ry;				//!< ?긂긚둷뱗궴긇긽깋둷뱗궻뜼걁릠뮳렡걂
	float add_camera_rx;		//!< 긇긽깋됷?둷뱗 RX궻몵뙵쀊
	float add_camera_ry;		//!< 긇긽깋됷?둷뱗 RY궻몵뙵쀊


	// 이번 렌더 프레임에서 실제로 사용할 카메라
	float draw_camera_x;
	float draw_camera_y;
	float draw_camera_z;
	float draw_camera_rx;
	float draw_camera_ry;

	// 고정 33FPS 로직 사이의 플레이어 이동을 렌더링에서만 보간한다.
	float render_interpolation_alpha;
	bool render_interpolation_valid;
	class human* render_previous_player;
	float render_previous_player_x;
	float render_previous_player_y;
	float render_previous_player_z;

	// 이번 렌더에서 실제로 표시되는 플레이어 위치.
	// 캐릭터에 붙는 스킬 이펙트와 갈고리 시작점도 같은 위치를 사용한다.
	float render_visual_player_x;
	float render_visual_player_y;
	float render_visual_player_z;
	float render_visual_player_rx;
	float render_visual_player_ry;
	bool render_visual_player_valid;

	// 렌더 프레임에서 먼저 읽은 마우스 이동량.
	// 화면에는 즉시 반영하고 다음 로직 틱에서 실제 조준 방향에도 동일하게 반영한다.
	int render_pending_mouse_x;
	int render_pending_mouse_y;
	bool render_previous_crouch;
	bool render_previous_dead;
	bool render_previous_dead_motion;
	bool render_previous_f1mode;
	bool render_previous_debugmode;

	// 조준 확대의 렌더 전용 보간 상태.
	// 맵/포커스/플레이어/시점/생사 전환 때 이전 화면의 값을 이어받지 않는다.
	float render_smooth_zoom;
	int render_previous_scope;
	int render_previous_weapon;
	bool render_view_state_valid;
	class human* render_view_state_player;
	bool render_view_state_dead;
	bool render_view_state_dead_motion;
	bool render_view_state_f1mode;
	bool render_view_state_debugmode;

	bool ShowInfo_Debugmode;	//!< 띆뷭궶궵귩?렑궥귡긢긫긞긏긾?긤
	bool Camera_Debugmode;		//!< 긇긽깋긢긫긞긏긾?긤
	bool tag;					//!< 긆긳긙긃긏긣궻?긐귩?렑
	bool radar;
	bool ShowFullMap;
	float FullMapWorldR;   // 전체 지도 확대/축소 범위. 기본 500.0f, 최대 확대 300.0f

	// 전체 지도에서만 사용하는 수동 층 선택 상태.
	// AUTO에서는 안정화된 플레이어 바닥 기준을 사용하고, 수동 선택 시에만 선택 층 높이를 고정한다.
	// 수동 선택으로 현재 층까지 돌아오면 CURRENT 상태 없이 AUTO로 복귀한다.
	bool FullMapFloorManual;
	int FullMapFloorOffset;
	int FullMapFloorCurrentIndex;
	int FullMapFloorSelectedIndex;
	int FullMapFloorCount;
	float FullMapFloorHeight[MAX_FULLMAP_FLOORS];
	float FullMapFloorReferenceY;

	// 전체 지도를 연 채 이동할 때 층 목록을 다시 탐색하기 위한 마지막 기준 위치.
	// 수동으로 선택한 실제 층 높이는 재탐색 뒤 가장 가까운 층 후보에 다시 연결한다.
	bool FullMapFloorSyncValid;
	float FullMapFloorSyncX;
	float FullMapFloorSyncY;
	float FullMapFloorSyncZ;

	// 미니맵과 AUTO 전체 지도가 실제 카메라의 점프/시선 높이를 따라가지 않도록
	// 플레이어가 서 있는 바닥을 기준으로 유지하는 레이더 전용 높이.
	bool RadarFloorReferenceValid;
	float RadarFloorReferenceY;
	int RadarFloorReferencePlayerID;
	bool wireframe;
	bool nomodel;				//!< 긾긢깑?됪긲깋긐
	bool CenterLine;			//!< 3D뗴듩궸뭷륲멄귩?렑
	bool Camera_Blind;			//!< 뽞덨궢?됪
	bool Camera_F1mode;			//!< 긇긽깋F1긾?긤
	int Camera_F2mode;			//!< 긇긽깋F2긾?긤
	bool Camera_HOMEmode;		//!< 긇긽깋HOME긾?긤
	bool Cmd_F5;				//!< 뿞땆F5긾?긤
	int InvincibleID;			//!< 뼰밎궶릐궻뵽믦
	bool PlayerAI;				//!< 긵깒귽깂??띿귩AI궸댬궺귡
	bool AIstop;				//!< AI룉뿚귩믠?궥귡
	bool AINoFight;				//!< AI궕뷄먰벉돸궥귡걁먰귦궶궋걂
	int AIdebuginfoID;			//!< AI궻긢긫긞긏륃뺪?렑
	int start_framecnt;			//!< 긽귽깛긒??둎럑렄궻긇긂깛긣
	int end_framecnt;			//!< 긽귽깛긒??뢎뿹궻긇긂깛긣
	bool EventStop;				//!< 귽긹깛긣룉뿚귩믠?궥귡
	bool EventSkip[TOTAL_EVENTLINE];	//!< 귽긹깛긣룉뿚귩긚긌긞긵궥귡
	int GameSpeed;				//!< 긒??궻렳뛱뫊뱗걁1댥뤵궻맢릶뭠걂
	int message_id;				//!< ?렑뭷궻귽긹깛긣긽긞긜?긙붥뜂
	int message_cnt;			//!< ?렑뭷궻귽긹깛긣긽긞긜?긙긇긂깛긣
	bool redflash_flag;			//!< 깒긞긤긲깋긞긘깄?됪긲깋긐
	bool shieldflash_flag;		//!< 방패 방어 피격 하양 플래시 여부

	// 피격 방향 원호 표시
	int damage_arc_timer;
	float damage_arc_rx;
	bool damage_arc_shield_flag;

	// 수류탄 궤도 표시
	bool grenade_trajectory_flag;
	int grenade_trajectory_weapon_id;

	// RUN / CROUCH 토글 입력 상태
	bool player_run_toggle_state;
	bool player_crouch_toggle_state;

	// RUN / CROUCH가 동시에 입력되었을 때 마지막 입력 우선순위
	// true: CROUCH 우선, false: RUN 우선
	bool player_move_crouch_priority;

	// 군인 E: 충격 수류탄 임시 장착 상태
	bool player_skill_impact_grenade_ready;
	bool player_skill_impact_grenade_equip_pending;
	bool player_skill_impact_grenade_unequip_pending;
	bool player_skill_impact_grenade_fire_lock;
	int player_skill_impact_grenade_change_timer;
	int player_skill_impact_grenade_shot_delay_timer;
	int player_skill_impact_grenade_return_weapon_slot;
	int player_skill_impact_grenade_ammo;

	// 경찰 E: 방패 임시 장착 상태
	bool player_skill_shield_ready;
	bool player_skill_shield_equip_pending;
	bool player_skill_shield_unequip_pending;
	int player_skill_shield_change_timer;
	int player_skill_shield_return_weapon_slot;

	// 용병 E: DP28 스킬 무기 상태
// 실제 사격/재장전/조준경을 쓰기 위해 선택 슬롯에 DP28을 임시 삽입하고,
// 기존 슬롯 무기 정보는 ID/탄약만 저장했다가 복구한다.
	bool player_skill_dp28_ready;
	int player_skill_dp28_return_weapon_slot;
	int player_skill_dp28_stored_weapon_id;
	int player_skill_dp28_stored_lnbs;
	int player_skill_dp28_stored_nbs;

	// DP28 자체의 장탄/예비탄도 E로 껐다 켜도 유지한다.
	// -1이면 아직 DP28을 한 번도 저장한 적이 없는 상태로 보고 기본 탄약을 사용한다.
	int player_skill_dp28_saved_lnbs;
	int player_skill_dp28_saved_nbs;

	// 플레이어 스킬
	int player_skill_cooldown_timer[PLAYER_SKILL_SLOT_COUNT];
	int player_skill_cooldown_total_timer[PLAYER_SKILL_SLOT_COUNT];
	bool player_skill_ready_sound_played[PLAYER_SKILL_SLOT_COUNT];
	int player_skill_current_type[PLAYER_SKILL_SLOT_COUNT];

	// 현재 발동 처리가 진행 중인 스킬 슬롯
	int player_skill_using_slot;


	// 대장 전술 스캔
	int player_skill_scan_charge_timer;
	int player_skill_scan_timer;
	int player_skill_scan_charge_sound_timer;

	int player_skill_mark_prep_timer;
	int player_skill_mark_prep_slot;
	int player_skill_mark_timer;
	int player_skill_mark_target_id;

	// 로봇: 처형 프로토콜
	int player_skill_robot_target_count;
	int player_skill_robot_target_ids[10];

	// 로봇: 순차 처형 큐
	int player_skill_robot_fire_queue_count;
	int player_skill_robot_fire_queue_index;
	int player_skill_robot_fire_queue_timer;
	int player_skill_robot_fire_queue_ids[10];

	// 로봇: 처형 프로토콜 HUD 전용 가짜 탄약
	// 실제 무기 탄약/장전 상태는 건드리지 않는다.
	int player_skill_robot_execute_visual_ammo;
	int player_skill_robot_execute_visual_ammo_max;

	// 로봇: 이전 프레임에 마커가 떠 있던 대상 목록
// 새로 마커가 생긴 적을 감지해서 효과음을 재생하기 위해 사용한다.
	int player_skill_robot_prev_marker_count;
	int player_skill_robot_prev_marker_ids[10];

	// 로봇 F: 에임핵
	int player_skill_aimhack_timer;
	int player_skill_aimhack_target_id;

	// 경찰: 제압 명령 표시 타이머
	int player_skill_suppress_mark_timer[MAX_HUMAN];

	// 좀비: 갈고리
	int player_skill_hook_target_id;
	int player_skill_hook_prep_timer;
	int player_skill_hook_pull_timer;

	// 갈고리로 실제 끌어당기기 시작한 대상의 기존 AI 비활성 상태를 복구하기 위한 값
	bool player_skill_hook_target_prev_force_no_ai;
	bool player_skill_hook_force_no_ai_applied;

	// 테러리스트: 교란 신호
	int player_skill_distraction_timer;
	int player_skill_distraction_sound_timer;
	int player_skill_distraction_sound_count;
	float player_skill_distraction_x;
	float player_skill_distraction_y;
	float player_skill_distraction_z;

	// 교란 장치 설치 시도 후, 0.5초 뒤 실제 생성한다.
	bool player_skill_distraction_install_pending;
	int player_skill_distraction_install_timer;
	int player_skill_distraction_install_slot;
	float player_skill_distraction_install_x;
	float player_skill_distraction_install_y;
	float player_skill_distraction_install_z;

	// 테러리스트 F: 지뢰 설치 상태
	bool player_skill_terrorist_mine_active[MAX_TERRORIST_MINES];
	float player_skill_terrorist_mine_x[MAX_TERRORIST_MINES];
	float player_skill_terrorist_mine_y[MAX_TERRORIST_MINES];
	float player_skill_terrorist_mine_z[MAX_TERRORIST_MINES];
	float player_skill_terrorist_mine_rx[MAX_TERRORIST_MINES];
	int player_skill_terrorist_mine_owner_team[MAX_TERRORIST_MINES];
	int player_skill_terrorist_mine_owner_human_id[MAX_TERRORIST_MINES];
	int player_skill_terrorist_mine_arm_timer[MAX_TERRORIST_MINES];
	int player_skill_terrorist_mine_next_index;

	// 지뢰 설치 시도 후, 짧은 대기 시간이 지난 뒤 실제 설치한다.
	bool player_skill_terrorist_mine_install_pending;
	int player_skill_terrorist_mine_install_timer;
	int player_skill_terrorist_mine_install_slot;
	float player_skill_terrorist_mine_install_x;
	float player_skill_terrorist_mine_install_y;
	float player_skill_terrorist_mine_install_z;

	// 지뢰 감지음이 폭발음에 묻히지 않도록 감지 후 짧게 대기했다가 폭발시킨다.
	bool player_skill_terrorist_mine_pending_explosion;
	int player_skill_terrorist_mine_pending_timer;
	float player_skill_terrorist_mine_pending_x;
	float player_skill_terrorist_mine_pending_y;
	float player_skill_terrorist_mine_pending_z;
	int player_skill_terrorist_mine_pending_owner_team;
	int player_skill_terrorist_mine_pending_owner_human_id;

	// 시민A: 응급 치료 성공 이펙트
	int player_skill_heal_effect_timer;

	// 시민B: 생존 본능 성공 이펙트
	int player_skill_survival_effect_timer;

	// 시민B: 긴급 회피 잔상 효과
	int player_skill_dodge_trail_timer;
	float player_skill_dodge_trail_start_x;
	float player_skill_dodge_trail_start_y;
	float player_skill_dodge_trail_start_z;
	float player_skill_dodge_trail_end_x;
	float player_skill_dodge_trail_end_y;
	float player_skill_dodge_trail_end_z;

	// 조준 확정형 스킬 공용 상태
	int player_skill_targeting_type;
	int player_skill_targeting_slot;
	float player_skill_target_x;
	float player_skill_target_y;
	float player_skill_target_z;
	bool player_skill_target_valid;

	// 비밀 요원: 텔레포트 시전 대기 상태
	int player_skill_teleport_cast_timer;
	int player_skill_teleport_slot;
	float player_skill_teleport_x;
	float player_skill_teleport_y;
	float player_skill_teleport_z;

	// 비밀 요원: 텔레포트 시작/도착 이펙트
	int player_skill_teleport_start_effect_timer;
	float player_skill_teleport_start_effect_x;
	float player_skill_teleport_start_effect_y;
	float player_skill_teleport_start_effect_z;

	int player_skill_teleport_end_effect_timer;
	float player_skill_teleport_end_effect_x;
	float player_skill_teleport_end_effect_y;
	float player_skill_teleport_end_effect_z;

	// 조준 확정형 스킬 발동 직후 입력 잠금
	int player_skill_input_lock_timer;

	// 로봇 처형 프로토콜: 좌클릭 처형 후 클릭을 누르고 있으면 일반 총 발사로 이어지지 않게 막는다.
	bool player_skill_block_shot_until_release;

	// 요원B: 동료 소환
	int player_skill_summon_ids[3];
	int player_skill_summon_remove_timer[3];

	bool player_skill_summon_install_pending;
	int player_skill_summon_install_timer;
	int player_skill_summon_install_slot;
	float player_skill_summon_install_x;
	float player_skill_summon_install_y;
	float player_skill_summon_install_z;

	// 요원B: 동료 소환 성공 이펙트
	int player_skill_summon_effect_timer;
	int player_skill_summon_effect_count;
	float player_skill_summon_effect_x[3];
	float player_skill_summon_effect_y[3];
	float player_skill_summon_effect_z[3];

	// 요원A: 그림자 잔상
	int player_skill_shadow_decoy_id;
	int player_skill_shadow_decoy_timer;

	bool player_skill_shadow_decoy_install_pending;
	int player_skill_shadow_decoy_install_timer;
	int player_skill_shadow_decoy_install_slot;
	float player_skill_shadow_decoy_install_x;
	float player_skill_shadow_decoy_install_y;
	float player_skill_shadow_decoy_install_z;

	int player_skill_shadow_decoy_ping_timer[MAX_HUMAN];

	// 요원B F: 생체장
	int player_skill_biotic_field_timer;
	int player_skill_biotic_field_heal_timer;
	float player_skill_biotic_field_x;
	float player_skill_biotic_field_y;
	float player_skill_biotic_field_z;

	// 생체장 설치 시도 후, 짧은 대기 시간이 지난 뒤 실제 생성한다.
	bool player_skill_biotic_field_install_pending;
	int player_skill_biotic_field_install_timer;
	int player_skill_biotic_field_install_slot;
	float player_skill_biotic_field_install_x;
	float player_skill_biotic_field_install_y;
	float player_skill_biotic_field_install_z;

	// 대장 F: 지점 지정 폭격
	int player_skill_artillery_strike_timer;
	int player_skill_artillery_strike_tick_timer;
	float player_skill_artillery_strike_x;
	float player_skill_artillery_strike_y;
	float player_skill_artillery_strike_z;
	int player_skill_artillery_strike_owner_team;
	int player_skill_artillery_strike_owner_human_id;

	// 지점 지정 폭격 호출 대기
	bool player_skill_artillery_strike_install_pending;
	int player_skill_artillery_strike_install_timer;
	int player_skill_artillery_strike_install_slot;
	float player_skill_artillery_strike_install_x;
	float player_skill_artillery_strike_install_y;
	float player_skill_artillery_strike_install_z;

	int player_skill_prev_active_timer;

	void ResetPlayerSkillState();

	int GetPlayerSkillType(human* myHuman);
	int GetPlayerSkillTypeBySlot(human* myHuman, int slot);
	bool HasPlayerSkill(human* myHuman);
	bool IsAnyPlayerSkillActive(human* myHuman);
	bool IsPlayerSkillBlockedByActiveSkill(human* myHuman, int skilltype);
	void StartPlayerSkillCooldown(int skilltype, int cooldown_frames);
	void StartPlayerSkillCooldownWithTotal(int skilltype, int cooldown_frames, int cooldown_total_frames);
	void StartPlayerSkillCooldownForSlot(int slot, int skilltype, int cooldown_frames);
	void StartPlayerSkillCooldownForSlotWithTotal(int slot, int skilltype, int cooldown_frames, int cooldown_total_frames);
	bool ActivateSimpleBuffPlayerSkill(human* myHuman, int skilltype);
	bool ActivateTacticalScanSkill(human* myHuman);
	bool ActivateHealSkill(human* myHuman);
	bool FindPlayerSkillDodgeDestination(human* myHuman, float* out_x, float* out_y, float* out_z, float* out_dist);
	bool ActivateDodgeSkill(human* myHuman);
	bool ActivateMarkShotSkill(human* myHuman);
	bool ActivateRobotExecuteSkill(human* myHuman);
	bool ActivateAimHackSkill(human* myHuman);
	bool ActivateSuppressSkill(human* myHuman);
	bool ActivateSpecialPlayerSkill(human* myHuman, int skilltype);
	bool IsPlayerTemporarySkillWeaponActive();
	bool IsPlayerTemporarySkillWeaponReady();
	bool IsPlayerTemporarySkillWeaponVisible();
	bool IsPlayerTemporarySkillWeaponChanging();
	bool IsPlayerTemporarySkillWeaponFireLocked();
	bool IsPlayerTemporarySkillWeaponShotDelayed();
	int GetPlayerTemporarySkillWeaponID();

	bool IsPlayerShieldWeaponEquipped(human* myHuman);
	bool IsPlayerCurrentWeaponReloading(human* myHuman);
	int GetPlayerTemporarySkillWeaponAmmo();
	int GetPlayerTemporarySkillWeaponStockAmmo(int weapon_id);
	int GetPlayerTemporarySkillWeaponReturnSlot();
	int GetPlayerTemporarySkillWeaponChangeTimer();
	void SetPlayerTemporarySkillWeaponFireLock(bool flag);
	void StartPlayerTemporarySkillWeaponShotDelay();
	int ConsumePlayerTemporarySkillWeaponAmmo(int amount);
	void KeepPlayerTemporarySkillWeaponReadyAfterFire();
	bool StartPlayerTemporarySkillWeaponEquip(human* myHuman, int weapon_id, int return_slot, int frames);
	void ProcessPlayerTemporarySkillWeaponState();
	void CancelPlayerTemporarySkillWeapon(human* myHuman);
	void StartPlayerTemporarySkillWeaponReturn(human* myHuman, int return_slot);
	void StartPlayerImpactGrenadeReturn(human* myHuman, int return_slot);

	bool IsPlayerDP28SkillWeaponActive();
	int GetPlayerDP28SkillWeaponReturnSlot();
	int GetPlayerDP28SkillStoredWeaponID();
	bool StartPlayerDP28SkillWeapon(human* myHuman);
	void ReturnPlayerDP28SkillWeapon(human* myHuman);

	bool TryUsePlayerSkill(human* myHuman);
	bool IsPlayerInstallSkillPending();
	void ProcessPlayerSkill(human* myHuman);
	void ProcessPlayerSkillCooldown();
	void ProcessTacticalScanChargeTimer();
	void ProcessSuppressMarkTimers();
	void ProcessDistractionSignalTimer();
	void ProcessTacticalScanActiveTimer();
	void ProcessMarkedShotTimer();
	void ProcessPlayerHealEffectTimer();
	void ProcessPlayerSummonEffectTimer();
	void ProcessPlayerBioticField(human* myHuman);
	void ProcessPlayerBioticFieldInstall(human* myHuman);
	void ProcessPlayerArtilleryStrike(human* myHuman);
	void ProcessPlayerArtilleryStrikeInstall(human* myHuman);
	void ApplyPlayerArtilleryStrikeTick(human* myHuman);
	void ProcessPlayerSurvivalEffectTimer();
	void ProcessPlayerDodgeTrailTimer();
	void ProcessPlayerSkillEndSound(human* myHuman);
	void ProcessRobotExecuteSkillState(human* myHuman);
	void ProcessAimHackSkill(human* myHuman);
	int FindAimHackTarget(human* myHuman, float* out_x, float* out_y, float* out_z);
	void ApplyAimHackAssist(human* myHuman, float target_x, float target_y, float target_z);
	int GetPlayerSkillSuppressActiveTimer();
	int GetPlayerSkillHudThemeColor(int skilltype, float alpha);
	bool IsPlayerSkillHudActive(human* myHuman, int skilltype);
	bool IsPlayerSkillUnavailableForHud(human* myHuman, int skilltype);
	bool GetPlayerSkillHudActiveGaugeInfo(human* myHuman, int skilltype, int* active_timer, int* active_total, int* active_color);
	void RenderPlayerSkillScreenEffect(human* myHuman);

	// 인게임 조작 안내 HUD
	const char* GetControlGuideKeyName(int key_id);
	bool CanWeaponUseFireModeGuide(int weapon_id);
	bool CanPlayerUseNightVisionGuide(human* myHuman);
	bool CanPlayerUseVariableZoomGuide(human* myHuman);
	void DrawControlGuideLine(int x, int y, const char* action, const char* key, int action_color, int key_color);
	void RenderPlayerControlGuide(human* myHuman, int display_weapon_id);
	void RenderMapControlGuide(int map_x, int map_y, int map_size);
	const char* GetPlayerInstallSkillCancelKeyName();
	void RenderInstallSkillCancelGuide();

	void RenderPlayerSkillHUD(human* myHuman);
	void RenderPlayerSkillHUDSlot(human* myHuman, int slot, int icon_x, int icon_y);
	void RenderTacticalScanPings(human* myHuman);

	int FindNearestEnemyForMarkShot(human* myHuman);
	bool HasEnemyInMarkShotRangeForHud(human* myHuman);
	void RenderMarkedShotPing(human* myHuman);

	bool IsEnemyInPlayerSuppressSight(human* myHuman, human* target);
	bool IsEnemySuppressableByPlayerSkill(human* myHuman, human* target);
	bool HasSuppressableEnemyInPlayerSightForHud(human* myHuman);
	int SuppressEnemiesInSight(human* myHuman);
	void RenderSuppressedEnemyPings(human* myHuman);

	int FindPlayerHookTargetByAimRay(human* myHuman);
	bool StartPlayerHookSkill(human* myHuman);
	void EndPlayerHookSkill(bool play_fail_sound);
	void ProcessPlayerHookSkill(human* myHuman);
	void RenderPlayerHookSkill(human* myHuman);
	void RenderPlayerHookAimPreview(human* myHuman);

	bool IsRobotExecuteInPrep(human* myHuman);
	float GetRobotExecutePrepRate(human* myHuman);
	float GetRobotExecuteActiveRate(human* myHuman);

	bool IsEnemyInRobotExecuteSight(human* myHuman, human* target, float* out_screen_order);
	int CollectRobotExecuteTargets(human* myHuman, int out_ids[], float out_orders[], int max_count);
	void StartRobotExecuteVisualWeapon(human* myHuman);
	void StopRobotExecuteVisualWeapon(human* myHuman);
	void FireRobotExecuteSkill(human* myHuman);
	void ProcessRobotExecuteFireQueue();
	void CancelRobotExecuteSkill(human* myHuman);
	bool HandleRobotExecuteSkillInput(human* myHuman, bool primary_skill_key_down, bool secondary_skill_key_down);

	// 스킬 사용 직전에 조준/수류탄 궤적을 해제한다.
	void ClearAimStateForPlayerSkill(human* myHuman);
	void RenderRobotExecutePings(human* myHuman);
	void RenderRobotExecuteScreenEffect(human* myHuman);

	void RenderPlayerRageAura(human* myHuman);
	void RenderPlayerBattleBoostEffect(human* myHuman);
	void RenderPlayerBattleAbsorbEffect(human* myHuman);
	void RenderPlayerHealEffect(human* myHuman);
	void RenderPlayerSummonEffect();
	void RenderPlayerBioticField();
	void RenderPlayerArtilleryStrike();
	void RenderPlayerSurvivalEffect(human* myHuman);
	void RenderPlayerDodgeTrail();
	void RenderPlayerTeleportCastPreview();
	void RenderPlayerTeleportEffect();
	void RenderDistractionSignalEffect();
	void RenderPlayerTerroristMines();

	bool IsPlayerSkillTargeting();
	void BeginPlayerSkillTargeting(int skilltype, int slot);
	bool BeginTargetingPlayerSkillIfNeeded(int skilltype, int slot);
	void CancelPlayerSkillTargeting();
	bool ActivateGroundTargetingSkill(int skilltype, float tx, float ty, float tz);
	bool StartPlayerTeleportSkill(human* myHuman, float tx, float ty, float tz);
	void ProcessPlayerTeleportSkill(human* myHuman);
	void ProcessPlayerTeleportEffectTimer();
	bool ConfirmPlayerSkillTargeting(human* myHuman);
	bool HandlePlayerSkillTargetingInput(human* myHuman, bool skill_key_down);
	bool UpdatePlayerSkillAimTarget(human* myHuman, float max_dist, bool need_ground, float* out_x, float* out_y, float* out_z);
	bool UpdatePlayerSkillAimTargetVisualRaw(human* myHuman, float max_dist, bool need_ground, float* out_x, float* out_y, float* out_z);
	bool IsPlayerSkillGroundTargetValid(int skilltype, float x, float y, float z);
	float GetPlayerSkillTargetPreviewHeight(int skilltype);
	void RenderPlayerSkillTargetPreview(human* myHuman);
	void RenderPlayerSkillEnemyPing(float x, float y, float z, float radius, float height);
	void RenderSkillTargetCylinder(float x, float y, float z, float radius, float height, int fill_color, int border_color);
	void RenderDistractionSignalMarker();
	void ApplyDistractionSignal();
	bool StartPlayerDistractionInstall(human* myHuman, int slot, float tx, float ty, float tz);
	void ProcessPlayerDistractionInstall(human* myHuman);

	int GetPlayerTerroristMineCount();
	void ClearPlayerTerroristMines();
	bool InstallPlayerTerroristMineAt(human* myHuman, float tx, float ty, float tz);
	bool StartPlayerTerroristMineInstall(human* myHuman, int slot, float tx, float ty, float tz);
	void ProcessPlayerTerroristMineInstall(human* myHuman);
	void ProcessPlayerTerroristMines();

	void CleanupPlayerSummonedZombies();
	void UpdatePlayerSummonedAllyFollow();
	void RemovePlayerSummonedZombies();
	int GetRandomSummonAllyTextureParamID(int player_team);
	void ApplySummonedAllyTexture(int human_id, int player_team);
	bool SummonPlayerZombies(float x, float y, float z);

	bool StartPlayerSummonInstall(human* myHuman, int slot, float tx, float ty, float tz);
	void ProcessPlayerSummonInstall(human* myHuman);

	void RemovePlayerShadowDecoy();
	bool SummonPlayerShadowDecoy(human* myHuman, float x, float y, float z);
	bool StartPlayerShadowDecoyInstall(human* myHuman, int slot, float tx, float ty, float tz);
	void ProcessPlayerShadowDecoyInstall(human* myHuman);
	void ProcessPlayerShadowDecoy(human* myHuman);
	void ProcessPlayerShadowDecoyPings();
	void RenderShadowDecoyPings(human* myHuman);
	bool StartPlayerBioticField(human* myHuman, float tx, float ty, float tz);
	bool StartPlayerBioticFieldInstall(human* myHuman, int slot, float tx, float ty, float tz);
	bool StartPlayerArtilleryStrike(human* myHuman, int slot, float tx, float ty, float tz);
	bool StartPlayerArtilleryStrikeInstall(human* myHuman, int slot, float tx, float ty, float tz);

	void PlayPlayerSkillSound(int soundtype);
	void PlayPlayerSkill3DSound(int soundtype, float x, float y, float z);

	bool ThrowPlayerImpactGrenade(human* myHuman);
	bool IsPlayerHoldingGrenade(human* myHuman);
	int GetPlayerCurrentGrenadeTrajectoryWeaponID(human* myHuman);
	void ClearGrenadeTrajectoryToggle();
	void UpdateGrenadeTrajectoryAutoOff(human* myHuman);
	bool ShouldRenderGrenadeTrajectory(human* myHuman);
	void RenderGrenadeTrajectory();
	void RenderGrenadeTrajectoryDot(float x, float y, float z, float size, int color);
	void RenderGrenadeTrajectoryLine(float x1, float y1, float z1, float x2, float y2, float z2, int color);

	bool IsGrenadeTrajectoryPointVisibleFromCamera(float x, float y, float z);
	void RenderGrenadeTrajectoryVisibleDot(float x, float y, float z, float size, int color);
	void RenderGrenadeTrajectoryVisibleLine(float x1, float y1, float z1, float x2, float y2, float z2, int color);
	int time;		//!< timer
	int time_input;					//!< 볺쀍롦벦궻룉뿚렄듩
	int time_process_object;		//!< 딈?긆긳긙긃긏긣궻룉뿚렄듩
	int time_process_ai;			//!< AI궻룉뿚렄듩
	int time_process_event;			//!< 귽긹깛긣궻룉뿚렄듩
	int time_sound;					//!< 긖긂깛긤궻룉뿚렄듩
	int time_render;				//!< ?됪궻룉뿚렄듩
	GameInfo MainGameInfo;			//!< 깏긗깑긣뾭듖뿚긏깋긚
	bool CheckInputControl(int CheckKey, int mode);
	bool CheckInputControlRealtime(int CheckKey);
	void InputPlayer(human *myHuman, int mouse_x, int mouse_y, float MouseSensitivity);
	void InputViewCamera(int mouse_x, int mouse_y, float MouseSensitivity);
	void CleanupMissionRuntime();   // 추가
	void Render3D();
	void Render2D();
	bool GetRadarPos(float in_x, float in_y, float in_z, int RadarPosX, int RadarPosY, int RadarSize, float RadarWorldR, int* out_x, int* out_y, float* local_y, bool check);
	void ResetFullMapFloorSelection();
	bool GetRadarSupportFloorHeight(human* radar_player, float* floor_y);
	void UpdateRadarFloorReference(human* radar_player);
	void BuildFullMapFloorList(float player_x, float player_y, float player_z);
	void SyncFullMapFloorSelection(human* myHuman);
	bool ChangeFullMapFloor(int direction, human* myHuman);
	int radar_human_height_layer[MAX_HUMAN];
	int GetRadarHeightLayer(float local_y);
	int GetRadarHeightLayerWithHysteresis(float local_y, int previous_layer);
	void DrawRadarHeightMarker(int x, int y, int height_layer, int color, int marker_size);
	void DrawRadarPlayerDirectionMarker(int x, int y, float dir_x, float dir_y, int color);
	bool IsRadarTargetVisibleToPlayer(human* radar_player, float target_x, float target_y, float target_z);
	bool IsRadarTargetVisibleToTeam(int teamid, float target_x, float target_y, float target_z);
	bool GetMissionEventPoint(signed short int p4, pointdata* out_data);
	bool DoesMissionEventPathEndMission(signed short int start_p4);
	void SetupMissionEventTargets();
	bool IsMissionEventTargetActive(human* target);
	void RenderRadar();

#ifdef ENABLE_DEBUGCONSOLE
	bool Show_Console;						//!< 긢긫긞긏뾭긓깛??깑귩?렑
	ConsoleData *InfoConsoleData;			//!< 긢긫긞긏뾭긓깛??깑긢??걁?렑띙귒걂
	TextBoxObject ConsoleTextBoxControl;	//!< 긢긫긞긏뾭긓깛??깑긢??걁볺쀍뭷걂
	char NewCommand[MAX_CONSOLELEN];		//!< 륷궫궸볺쀍궠귢궫뼟룉뿚궻긓?깛긤
	int ScreenShot;		//!< SS귩랡뎓궥귡
	void AddInfoConsole(int color, const char *str);
	bool GetCommandNum(const char *cmd, int *num);
	void InputConsole();
	void ProcessConsole();
	void RenderConsole();
	bool IsDebugMuzzleAdjustInputActive();
	void ProcessDebugMuzzleInput();
	void RenderDebugWeaponOverlay();
#endif

public:
	maingame();
	~maingame();
	void SetShowInfoFlag(bool flag);
	void SetRenderInterpolationAlpha(float alpha);
	void PollRenderMouseInput();
	void CaptureRenderInterpolationState();
	void ResetRenderInterpolation();
	void HandleApplicationFocusChange(bool active);
	void InvalidatePlayerRenderInterpolation();
	void GetSkillVisualHumanPosition(human* target, float* x, float* y, float* z);
	void SyncDrawCamera();
	void ProcessVisualTimers();
	int GetGameSpeed();
	int Create();
	int Recovery();
	void Input();
	void Process();
	void Sound();
	void Destroy();
};

//! @brief 깏긗깑긣됪뽋듖뿚긏깋긚
//! @details 깏긗깑긣걁뙅됈?렑걂됪뽋귩듖뿚궢귏궥갃
class result : public D2Dscene
{
	void Render2D();

public:
	result();
	~result();
};

void InitScreen(WindowControl *WindowCtrl, opening *Opening, mainmenu *MainMenu, briefing *Briefing, maingame *MainGame, result *Result);
void ProcessScreen(
	WindowControl* WindowCtrl,
	opening* Opening,
	mainmenu* MainMenu,
	briefing* Briefing,
	maingame* MainGame,
	result* Result,
	unsigned int framecnt,
	bool do_logic,
	bool do_render
);
int ChangeWindowMode(WindowControl *WindowCtrl, D3DGraphics *d3dg, InputControl *inputCtrl, scene *RecoveryScene, bool fullscreen);

#endif