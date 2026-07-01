//! @file config.h
//! @brief config긏깋긚궻먬뙻

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

#ifndef CONFING_H
#define CONFING_H

#define TOTAL_ControlKey 24		//!< 뜃똶?띿긌?릶
#define MAX_PLAYERNAME 21		//!< 긵깒귽깂?뼹궻띍묈빒럻릶

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 1		//!< Select include file.
#endif
#include "main.h"

//! GetKeycode()뾭 믦릶
enum ControlKey
{
	KEY_TURNUP = 0,
	KEY_TURNDOWN,
	KEY_TURNLEFT,
	KEY_TURNRIGHT,
	KEY_MOVEFORWARD,
	KEY_MOVEBACKWARD,
	KEY_MOVELEFT,
	KEY_MOVERIGHT,
	KEY_WALK,
	KEY_JUMP,
	KEY_RELOAD,
	KEY_DROPWEAPON,
	KEY_ZOOMSCOPE,
	KEY_SHOTMODE,
	KEY_SWITCHWEAPON,
	KEY_WEAPON1,
	KEY_WEAPON2,
	KEY_SHOT,
	KEY_RUN,
	KEY_CROUCH,
	KEY_NIGHTVISION,
	KEY_MAP,
	KEY_SKILL,
	KEY_SKILL2
};

//! @brief 먠믦긲?귽깑귩벶귒뜛귔긏깋긚
//! @details 긲?귽깑궻랷뤖궴뭠궻듖뿚귩뛱궋귏궥갃
//! @details 랷뛩럱뿿갌config.dat긲?귽깑됶먏륃뺪?http://openxops.net/filesystem-config.php
class Config
{
	int Keycode[TOTAL_ControlKey];		//!< 긆깏긙긥깑긌?긓?긤
	int MouseSensitivity;				//!< ?긂긚뒾뱗
	bool FullscreenFlag;				//!< 긲깑긚긏깏?깛뾎뚼
	bool SoundFlag;						//!< 뚼됈돶뾎뚼
	bool BloodFlag;						//!< 뢯뙆뾎뚼
	int Brightness;						//!< 됪뽋궻뼻귡궠
	bool InvertMouseFlag;				//!< ?긂긚뵿?
	bool FrameskipFlag;					//!< 긲깒??긚긌긞긵
	bool AnotherGunsightFlag;			//!< 빶궻뤖?귩럊뾭
	char PlayerName[MAX_PLAYERNAME];	//!< 긵깒귽깂?뼹

	int Language;						//!< ?렑뙻뚭
	bool ShowArmFlag;					//!< 쁱?렑긲깋긐
	int ScreenWidth;					//!< 됪뽋됶몴뱗걁븴걂
	int ScreenHeight;					//!< 됪뽋됶몴뱗걁뛼궠걂
	bool MouseLimitFlag;				//!< ?긂긚댷벍쀊맕뙽긲깋긐
	int RenderFpsLimit;					//!< 화면 렌더링 FPS 제한. 0이면 제한 없음
	float mastervolume;					//!< 돶쀊

	float DamageMultiplier;				//!< プレイヤ?の被ダメージ倍率
	int HealthRegenAmount;				//!< プレイヤ?の体力回復量




public:
	Config();
	~Config();
	int LoadFile(const char *fname);
	int SaveFile(const char *fname);
	void SetDefaultConfig();
	int GetKeycode(int id);
	void SetKeycode(int id, int key);
	int GetMouseSensitivity();
	void SetMouseSensitivity(int param);
	bool GetFullscreenFlag();
	void SetFullscreenFlag(bool flag);
	bool GetSoundFlag();
	void SetSoundFlag(bool flag);
	bool GetBloodFlag();
	void SetBloodFlag(bool flag);
	int GetBrightness();
	void SetBrightness(int param);
	bool GetInvertMouseFlag();
	void SetInvertMouseFlag(bool flag);
	bool GetFrameskipFlag();
	void SetFrameskipFlag(bool flag);
	bool GetAnotherGunsightFlag();
	void SetAnotherGunsightFlag(bool flag);
	int GetPlayerName(char *out_str);
	void SetPlayerName(char *in_str);
	const char* GetOriginalkeycodeString(int code);

	bool CreateExtConfig(const char *fname);
	int LoadExtFile(const char *fname);
	void SetDefaultExtConfig();
	int GetLanguage();
	bool GetShowArmFlag();
	int GetScreenWidth();
	int GetScreenHeight();
	bool GetMouseLimitFlag();

	int GetRenderFpsLimit();
	void SetRenderFpsLimit(int fps);

	float GetVolume();
	float GetDamageMultiplier();

	// [✨ 추가됨 ✨] AI 설정 변수
	float AIReactionSpeedMultiplier;
	float AIFireRateMultiplier;
	float AIAccuracyMultiplier;
	float AISearchRangeMultiplier;

	float RecoilHipMultiplier;	//!< 지향 사격 반동 배율
	float RecoilAimMultiplier;	//!< 조준 사격 반동 배율
	float RecoilSideMultiplier;	//!< 좌우 반동 배율

	// [추가/수정] 변수 이름에 맞게 함수 작성
	void SetMasterVolume(float val) { mastervolume = val; }
	float GetMasterVolume() { return mastervolume; }

	// [✨ 새로 추가할 부분 ✨] 해상도 값을 변경하는 함수
	void SetScreenWidth(int w) { ScreenWidth = w; }
	void SetScreenHeight(int h) { ScreenHeight = h; }

	void SetPlayerDamageMultiplier(float val) { DamageMultiplier = val; } // 변수명 DamageMultiplier 주의
	float GetPlayerDamageMultiplier() { return DamageMultiplier; }

	void SetPlayerHealthRegenAmount(int val) { HealthRegenAmount = val; }
	int GetHealthRegenAmount() { return HealthRegenAmount; }

	void SetAIReactionSpeedMultiplier(float val) { AIReactionSpeedMultiplier = val; }
	float GetAIReactionSpeedMultiplier() { return AIReactionSpeedMultiplier; }

	void SetAIFireRateMultiplier(float val) { AIFireRateMultiplier = val; }
	float GetAIFireRateMultiplier() { return AIFireRateMultiplier; }

	void SetAIAccuracyMultiplier(float val) { AIAccuracyMultiplier = val; }
	float GetAIAccuracyMultiplier() { return AIAccuracyMultiplier; }

	void SetAISearchRangeMultiplier(float val) { AISearchRangeMultiplier = val; }
	float GetAISearchRangeMultiplier() { return AISearchRangeMultiplier; }

	void SetRecoilHipMultiplier(float val) { RecoilHipMultiplier = val; }
	float GetRecoilHipMultiplier() { return RecoilHipMultiplier; }

	void SetRecoilAimMultiplier(float val) { RecoilAimMultiplier = val; }
	float GetRecoilAimMultiplier() { return RecoilAimMultiplier; }

	void SetRecoilSideMultiplier(float val) { RecoilSideMultiplier = val; }
	float GetRecoilSideMultiplier() { return RecoilSideMultiplier; }

	int CrosshairStyle;		//!< 조준선 모양 (0: 기본 십자, 1: 원형, 2: 점, 3: 십자+원)
	int CrosshairColor;		//!< 조준선 색상 (0: 연두색, 1: 빨간색, 2: 노란색, 3: 하얀색)
	bool CrosshairOutline; // [✨ 추가] 테두리 유무 (true/false)

	bool SkillModeFlag;				//!< 플레이어 스킬 모드 사용 여부
	bool RadarEnabledFlag;			//!< 레이더 / 전체 지도 사용 여부
	bool RadarInfoHudFlag;			//!< 레이더 아래 TIME / KILL / ENEMY 정보 표시 여부
	bool RadarSeeThroughWallsFlag;	//!< 레이더가 벽 너머 적을 표시할지 여부

	bool GrenadeTrajectoryToggleFlag;	//!< 수류탄 궤적 입력 방식. true: TOGGLE, false: HOLD
	bool ScopeAimToggleFlag;			//!< 스코프 입력 방식. true: TOGGLE, false: HOLD
	bool RunToggleFlag;					//!< 달리기 입력 방식. true: TOGGLE, false: HOLD
	bool CrouchToggleFlag;				//!< 앉기 입력 방식. true: TOGGLE, false: HOLD

	// Getter & Setter
	void SetSkillModeFlag(bool val) { SkillModeFlag = val; }
	bool GetSkillModeFlag() { return SkillModeFlag; }

	void SetRadarEnabledFlag(bool val) { RadarEnabledFlag = val; }
	bool GetRadarEnabledFlag() { return RadarEnabledFlag; }

	void SetRadarInfoHudFlag(bool val) { RadarInfoHudFlag = val; }
	bool GetRadarInfoHudFlag() { return RadarInfoHudFlag; }

	void SetRadarSeeThroughWallsFlag(bool val) { RadarSeeThroughWallsFlag = val; }
	bool GetRadarSeeThroughWallsFlag() { return RadarSeeThroughWallsFlag; }

	void SetGrenadeTrajectoryToggleFlag(bool val) { GrenadeTrajectoryToggleFlag = val; }
	bool GetGrenadeTrajectoryToggleFlag() { return GrenadeTrajectoryToggleFlag; }

	void SetScopeAimToggleFlag(bool val) { ScopeAimToggleFlag = val; }
	bool GetScopeAimToggleFlag() { return ScopeAimToggleFlag; }

	void SetRunToggleFlag(bool val) { RunToggleFlag = val; }
	bool GetRunToggleFlag() { return RunToggleFlag; }

	void SetCrouchToggleFlag(bool val) { CrouchToggleFlag = val; }
	bool GetCrouchToggleFlag() { return CrouchToggleFlag; }

	void SetCrosshairStyle(int val) { CrosshairStyle = val; }
	int GetCrosshairStyle() { return CrosshairStyle; }
	void SetCrosshairColor(int val) { CrosshairColor = val; }
	int GetCrosshairColor() { return CrosshairColor; }
	void SetCrosshairOutline(bool val) { CrosshairOutline = val; }
	bool GetCrosshairOutline() { return CrosshairOutline; }

};

#endif