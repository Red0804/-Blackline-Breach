//! @file config.cpp
//! @brief config긏깋긚궻믦? 

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

#include "config.h"

//! @brief 긓깛긚긣깋긏?
Config::Config()
{
	//둫롰긽깛긫?빾릶룊딖돸
	for(int i=0; i<TOTAL_ControlKey; i++){
		Keycode[i] = 0;
	}
	MouseSensitivity = 0;
	FullscreenFlag = false;
	SoundFlag = false;
	BloodFlag = false;
	Brightness = 0;
	InvertMouseFlag = false;
	FrameskipFlag = false;
	AnotherGunsightFlag = false;
	strcpy(PlayerName, "");

	Language = 0;
	ShowArmFlag = false;
	ScreenWidth = 0;
	ScreenHeight = 0;
	MouseLimitFlag = true;
	mastervolume = 1.0f;
	DamageMultiplier = 1.0f;
	HealthRegenAmount = 10;
	RenderFpsLimit = RENDERFPS_DEFAULT;

	AIReactionSpeedMultiplier = 1.0f;
	AIFireRateMultiplier = 1.0f;
	AIAccuracyMultiplier = 1.0f;
	AISearchRangeMultiplier = 1.0f;

	RecoilHipMultiplier = 1.0f;
	RecoilAimMultiplier = 1.0f;
	RecoilSideMultiplier = 0.0f;

	// [✨ 여기에 크로스헤어 기본값 초기화 코드를 추가합니다]
	CrosshairStyle = 0;        // 0: 기본 십자(CROSS)
	CrosshairColor = 0;        // 0: 기본 초록색(GREEN)
	CrosshairOutline = false;   // true: 테두리 켬 (끄고 싶다면 false)

	// 기본값: 스킬 모드 활성화
	SkillModeFlag = true;

	// 기본값: 이벤트 대상 체력 증가 활성화
	EventTargetHPBoostFlag = true;

	// 기본값: 플레이어/전체 지도 조작 안내 활성화
	ControlGuideFlag = true;

	// 기본값: 레이더와 레이더 정보 UI는 활성화
	RadarEnabledFlag = true;
	RadarInfoHudFlag = true;

	// 기본값: 기존처럼 벽 너머 적도 레이더에 표시
	RadarSeeThroughWallsFlag = true;

	// 기본값: 기존 조작감 유지
// true: TOGGLE, false: HOLD
	GrenadeTrajectoryToggleFlag = true;
	ScopeAimToggleFlag = true;

	// RUN / CROUCH는 기존 조작감이 HOLD이므로 기본값 false
	RunToggleFlag = false;
	CrouchToggleFlag = false;

	// 화면 렌더링 제한 기본값
	RenderFpsLimit = RENDERFPS_DEFAULT;

}

//! @brief 긢귻긚긣깋긏?
Config::~Config()
{}

//! @brief 먠믦긲?귽깑귩벶귒뜛귔
//! @param fname 긲?귽깑뼹
//! @return 맟뚻갌0?렪봲갌1
int Config::LoadFile(const char *fname)
{
	if( fname == NULL ){ return 1; }

	FILE *fp;
	char buf;

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_LOAD, "Config File", fname);
#endif

#ifdef ENABLE_PATH_DELIMITER_SLASH
	//긬긚뗦먛귟빒럻귩빾듂
	fname = ChangePathDelimiter(fname);
#endif

	// 파일을 읽기 모드로 열기 시도
	fp = fopen(fname, "rb");

	// ★ [수정] 파일이 없으면 기본값을 설정하고 새로 저장한 뒤 성공(0)으로 처리 ★
	if (fp == NULL) {
		SetDefaultConfig(); // 기본 설정값 로드
		SaveFile(fname);    // 파일 새로 만들기 (config.dat 생성)
		return 0;           // 성공으로 리턴
	}

	//긌?긓?긤
	for(int i=0; i<TOTAL_ControlKey; i++){
		fread(&buf, 1, 1, fp);
		Keycode[i] = (int)buf;
	}

	//?긂긚뒾뱗
	fread(&buf, 1, 1, fp);
	MouseSensitivity = (int)buf;

	//긲깑긚긏깏?깛뾎뚼
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ FullscreenFlag = false; }
	else{ FullscreenFlag = true; }

	//뚼됈돶뾎뚼
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ SoundFlag = false; }
	else{ SoundFlag = true; }

	//뢯뙆뾎뚼
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ BloodFlag = false; }
	else{ BloodFlag = true; }

	//됪뽋궻뼻귡궠
	fread(&buf, 1, 1, fp);
	Brightness = (int)buf;

	//?긂긚뵿?
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ InvertMouseFlag = false; }
	else{ InvertMouseFlag = true; }

	//긲깒??긚긌긞긵
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ FrameskipFlag = false; }
	else{ FrameskipFlag = true; }

	//빶궻뤖?귩럊뾭
	fread(&buf, 1, 1, fp);
	if( buf == 0x00 ){ AnotherGunsightFlag = false; }
	else{ AnotherGunsightFlag = true; }

	//긵깒귽깂?뼹
	fread(PlayerName, 1, MAX_PLAYERNAME, fp);

	// [✨ 새로 추가 ✨] config.dat 파일에서 설정값을 불러옵니다!
	fread(&mastervolume, sizeof(float), 1, fp);
	fread(&DamageMultiplier, sizeof(float), 1, fp);
	fread(&HealthRegenAmount, sizeof(int), 1, fp);
	fread(&AIReactionSpeedMultiplier, sizeof(float), 1, fp);
	fread(&AIFireRateMultiplier, sizeof(float), 1, fp);
	fread(&AIAccuracyMultiplier, sizeof(float), 1, fp);
	fread(&AISearchRangeMultiplier, sizeof(float), 1, fp);

	if (DamageMultiplier < 0.0f) DamageMultiplier = 0.0f;
	if (DamageMultiplier > 3.0f) DamageMultiplier = 3.0f;

	if (AIReactionSpeedMultiplier < 0.0f) AIReactionSpeedMultiplier = 0.0f;
	if (AIFireRateMultiplier < 0.0f) AIFireRateMultiplier = 0.0f;
	if (AIAccuracyMultiplier < 0.0f) AIAccuracyMultiplier = 0.0f;
	if (AISearchRangeMultiplier < 0.0f) AISearchRangeMultiplier = 0.0f;

	if (AIReactionSpeedMultiplier > 2.0f) AIReactionSpeedMultiplier = 2.0f;
	if (AIFireRateMultiplier > 4.0f) AIFireRateMultiplier = 4.0f;
	if (AIAccuracyMultiplier > 3.0f) AIAccuracyMultiplier = 3.0f;
	if (AISearchRangeMultiplier > 2.5f) AISearchRangeMultiplier = 2.5f;

	fread(&RecoilHipMultiplier, sizeof(float), 1, fp);
	fread(&RecoilAimMultiplier, sizeof(float), 1, fp);

	fread(&CrosshairStyle, sizeof(int), 1, fp);
	fread(&CrosshairColor, sizeof(int), 1, fp);
	fread(&CrosshairOutline, sizeof(bool), 1, fp);

	// 추가: 구버전 config.dat 대응
	if (fread(&RecoilSideMultiplier, sizeof(float), 1, fp) != 1) {
		RecoilSideMultiplier = 0.0f;
	}

	if (RecoilSideMultiplier < 0.0f) RecoilSideMultiplier = 0.0f;
	if (RecoilSideMultiplier > 1.0f) RecoilSideMultiplier = 1.0f;

	// 추가: 구버전 config.dat 대응
	if (fread(&SkillModeFlag, sizeof(bool), 1, fp) != 1) {
		SkillModeFlag = true;
	}

	// 레이더 사용 여부
	// 구버전 config.dat에는 값이 없으므로 기본값 true
	if (fread(&RadarEnabledFlag, sizeof(bool), 1, fp) != 1) {
		RadarEnabledFlag = true;
	}

	// 레이더 아래 TIME / KILL / ENEMY 정보 표시 여부
// 구버전 config.dat에는 값이 없으므로 기본값 true
	if (fread(&RadarInfoHudFlag, sizeof(bool), 1, fp) != 1) {
		RadarInfoHudFlag = true;
	}

	// 레이더 벽 너머 적 표시 여부
// 구버전 config.dat에는 값이 없으므로 기본값 true
	if (fread(&RadarSeeThroughWallsFlag, sizeof(bool), 1, fp) != 1) {
		RadarSeeThroughWallsFlag = true;
	}

	// 수류탄 궤적 입력 방식
	// 구버전 config.dat에는 값이 없으므로 기존 조작감인 TOGGLE
	if (fread(&GrenadeTrajectoryToggleFlag, sizeof(bool), 1, fp) != 1) {
		GrenadeTrajectoryToggleFlag = true;
	}

	// 스코프 입력 방식
// 구버전 config.dat에는 값이 없으므로 기존 조작감인 TOGGLE
	if (fread(&ScopeAimToggleFlag, sizeof(bool), 1, fp) != 1) {
		ScopeAimToggleFlag = true;
	}

	// 달리기 입력 방식
	// 구버전 config.dat에는 값이 없으므로 기존 조작감인 HOLD
	if (fread(&RunToggleFlag, sizeof(bool), 1, fp) != 1) {
		RunToggleFlag = false;
	}

	// 앉기 입력 방식
// 구버전 config.dat에는 값이 없으므로 기존 조작감인 HOLD
	if (fread(&CrouchToggleFlag, sizeof(bool), 1, fp) != 1) {
		CrouchToggleFlag = false;
	}

	// 화면 렌더링 FPS 제한
	// 구버전 config.dat에는 값이 없으므로 144FPS 기본값 사용
	if (fread(&RenderFpsLimit, sizeof(int), 1, fp) != 1) {
		RenderFpsLimit = RENDERFPS_DEFAULT;
	}
	SetRenderFpsLimit(RenderFpsLimit);

	// 이벤트 대상 체력 증가 여부
	// 구버전 config.dat에는 값이 없으므로 ON
	if (fread(&EventTargetHPBoostFlag, sizeof(bool), 1, fp) != 1) {
		EventTargetHPBoostFlag = true;
	}

	// 플레이어/전체 지도 조작 안내 표시 여부
	// 구버전 config.dat에는 값이 없으므로 ON
	if (fread(&ControlGuideFlag, sizeof(bool), 1, fp) != 1) {
		ControlGuideFlag = true;
	}

	//긲?귽깑긪깛긤깑귩빧궣귡
	fclose(fp);

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif
	return 0;
}

//! @brief 먠믦긲?귽깑궸룕궖뜛귔
//! @param fname 긲?귽깑뼹
//! @return 맟뚻갌0?렪봲갌1
int Config::SaveFile(const char *fname)
{
	if( fname == NULL ){ return 1; }

	FILE *fp;
	char buf;

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_LOAD, "Config File (Save)", fname);
#endif

#ifdef ENABLE_PATH_DELIMITER_SLASH
	//긬긚뗦먛귟빒럻귩빾듂
	fname = ChangePathDelimiter(fname);
#endif

	//긲?귽깑귩둎궘
	fp = fopen(fname, "wb");
	if( fp == NULL ){ return 1; }

	//긌?긓?긤
	for(int i=0; i<TOTAL_ControlKey; i++){
		buf = (char)Keycode[i];
		fwrite(&buf, 1, 1, fp);
	}

	//?긂긚뒾뱗
	buf = (char)MouseSensitivity;
	fwrite(&buf, 1, 1, fp);

	//긲깑긚긏깏?깛뾎뚼
	if( FullscreenFlag == false ){ buf = 0x00; }
	else{ buf = 0x01; }
	fwrite(&buf, 1, 1, fp);

	//뚼됈돶뾎뚼
	if( SoundFlag == false ){ buf = 0x00; }
	else{ buf = 0x01; }
	fwrite(&buf, 1, 1, fp);

	//뢯뙆뾎뚼
	if( BloodFlag == false ){ buf = 0x00; }
	else{ buf = 0x01; }
	fwrite(&buf, 1, 1, fp);

	//됪뽋궻뼻귡궠
	buf = (char)Brightness;
	fwrite(&buf, 1, 1, fp);

	//?긂긚뵿?
	if( InvertMouseFlag == false ){ buf = 0x00; }
	else{ buf = 0x01; }
	fwrite(&buf, 1, 1, fp);

	//긲깒??긚긌긞긵
	if( FrameskipFlag == false ){ buf = 0x00; }
	else{ buf = 0x01; }
	fwrite(&buf, 1, 1, fp);

	//빶궻뤖?귩럊뾭
	if( AnotherGunsightFlag == false ){ buf = 0x00; }
	else{ buf = 0x01; }
	fwrite(&buf, 1, 1, fp);

	//긵깒귽깂?뼹
	fwrite(PlayerName, 1, MAX_PLAYERNAME, fp);

	// [✨ 새로 추가 ✨] INI 대신 config.dat 파일에 영구 저장합니다!
	fwrite(&mastervolume, sizeof(float), 1, fp);
	fwrite(&DamageMultiplier, sizeof(float), 1, fp);
	fwrite(&HealthRegenAmount, sizeof(int), 1, fp);
	fwrite(&AIReactionSpeedMultiplier, sizeof(float), 1, fp);
	fwrite(&AIFireRateMultiplier, sizeof(float), 1, fp);
	fwrite(&AIAccuracyMultiplier, sizeof(float), 1, fp);
	fwrite(&AISearchRangeMultiplier, sizeof(float), 1, fp);

	fwrite(&RecoilHipMultiplier, sizeof(float), 1, fp);
	fwrite(&RecoilAimMultiplier, sizeof(float), 1, fp);

	fwrite(&CrosshairStyle, sizeof(int), 1, fp);
	fwrite(&CrosshairColor, sizeof(int), 1, fp);
	fwrite(&CrosshairOutline, sizeof(bool), 1, fp);
	// 추가
	fwrite(&RecoilSideMultiplier, sizeof(float), 1, fp);

	// 플레이어 스킬 모드 사용 여부
	fwrite(&SkillModeFlag, sizeof(bool), 1, fp);

	// 레이더 사용 여부
	fwrite(&RadarEnabledFlag, sizeof(bool), 1, fp);

	// 레이더 아래 TIME / KILL / ENEMY 정보 표시 여부
	fwrite(&RadarInfoHudFlag, sizeof(bool), 1, fp);

	// 레이더 벽 너머 적 표시 여부
	fwrite(&RadarSeeThroughWallsFlag, sizeof(bool), 1, fp);

	// 수류탄 궤적 입력 방식
	fwrite(&GrenadeTrajectoryToggleFlag, sizeof(bool), 1, fp);

	// 스코프 입력 방식
	fwrite(&ScopeAimToggleFlag, sizeof(bool), 1, fp);

	// 달리기 입력 방식
	fwrite(&RunToggleFlag, sizeof(bool), 1, fp);

	// 앉기 입력 방식
	fwrite(&CrouchToggleFlag, sizeof(bool), 1, fp);

	// 화면 렌더링 FPS 제한
	fwrite(&RenderFpsLimit, sizeof(int), 1, fp);

	// 이벤트 대상 체력 증가 여부
	fwrite(&EventTargetHPBoostFlag, sizeof(bool), 1, fp);

	// 플레이어/전체 지도 조작 안내 표시 여부
	fwrite(&ControlGuideFlag, sizeof(bool), 1, fp);


	//긲?귽깑긪깛긤깑귩빧궣귡
	fclose(fp);

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif
	return 0;
}

//! @brief 먠믦뭠귩긢긲긅깑긣궻뭠궳룊딖돸
void Config::SetDefaultConfig()
{
	Keycode[KEY_TURNUP]			= 0;
	Keycode[KEY_TURNDOWN]		= 1;
	Keycode[KEY_TURNLEFT]		= 2;
	Keycode[KEY_TURNRIGHT]		= 3;
	Keycode[KEY_MOVEFORWARD]	= 54;
	Keycode[KEY_MOVEBACKWARD]	= 50;
	Keycode[KEY_MOVELEFT]		= 32;
	Keycode[KEY_MOVERIGHT]		= 35;
	Keycode[KEY_WALK] = 0x10;	// TAB
	Keycode[KEY_JUMP] = 17;
	Keycode[KEY_RELOAD] = 49;
	Keycode[KEY_DROPWEAPON]		= 38;
	Keycode[KEY_ZOOMSCOPE]		= 19;
	Keycode[KEY_SHOTMODE]		= 55;
	Keycode[KEY_SWITCHWEAPON]	= 57;
	Keycode[KEY_WEAPON1]		= 23;
	Keycode[KEY_WEAPON2]		= 24;
	Keycode[KEY_SHOT]			= 18;
	Keycode[KEY_RUN] = 0x14;		// SHIFT
	Keycode[KEY_CROUCH] = 0x15;		// CTRL
	Keycode[KEY_NIGHTVISION] = 0x2D; // N
	Keycode[KEY_MAP] = 0x2C;         // M
	Keycode[KEY_SKILL] = 0x24; // E
	Keycode[KEY_SKILL2] = 0x25; // F
	MouseSensitivity = 10;
	FullscreenFlag = true;
	SoundFlag = true;
	BloodFlag = true;
	Brightness = 4;
	InvertMouseFlag = false;
	FrameskipFlag = false;
	AnotherGunsightFlag = false;
	strcpy(PlayerName, "xopsplayer");
	CrosshairStyle = 0; // 기본 모양
	CrosshairColor = 0; // 기본 색상 (연두색)
	CrosshairOutline = false;   // true: 테두리 켬 (끄고 싶다면 false)

	// 기본값: 스킬 모드 활성화
	SkillModeFlag = true;

	// 기본값: 이벤트 대상 체력 증가 활성화
	EventTargetHPBoostFlag = true;

	// 기본값: 플레이어/전체 지도 조작 안내 활성화
	ControlGuideFlag = true;

	// 기본값: 레이더와 레이더 정보 UI는 활성화
	RadarEnabledFlag = true;
	RadarInfoHudFlag = true;

	// 기본값: 기존처럼 벽 너머 적도 레이더에 표시
	RadarSeeThroughWallsFlag = true;

	// 기본값: 기존 조작감 유지
// true: TOGGLE, false: HOLD
	GrenadeTrajectoryToggleFlag = true;
	ScopeAimToggleFlag = true;

	// RUN / CROUCH는 기존 조작감이 HOLD이므로 기본값 false
	RunToggleFlag = false;
	CrouchToggleFlag = false;

}

//! @brief 긆깏긙긥깑긌?긓?긤귩롦벦
//! @param id 믦릶
//! @return 긆깏긙긥깑긌?긓?긤
int Config::GetKeycode(int id)
{
	if( (id < 0)||(TOTAL_ControlKey <= id) ){ return 0; }

	return Keycode[id];
}

//! @brief 긆깏긙긥깑긌?긓?긤귩먠믦
//! @param id 믦릶
//! @param key 긆깏긙긥깑긌?긓?긤
void Config::SetKeycode(int id, int key)
{
	if( (id < 0)||(TOTAL_ControlKey <= id) ){ return; }

	Keycode[id] = key;
}

//! @brief ?긂긚뒾뱗롦벦
//! @return 맯궻뭠
int Config::GetMouseSensitivity()
{
	return MouseSensitivity;
}

//! @brief ?긂긚뒾뱗먠믦
//! @param param 맯궻뭠
void Config::SetMouseSensitivity(int param)
{
	MouseSensitivity = param;
}

//! @brief 됪뽋?렑긾?긤롦벦
//! @return 긂귻깛긤긂갌false?긲깑긚긏깏?깛갌true
bool Config::GetFullscreenFlag()
{
	return FullscreenFlag;
}

//! @brief 됪뽋?렑긾?긤먠믦
//! @param flag 긲깋긐
void Config::SetFullscreenFlag(bool flag)
{
	FullscreenFlag = flag;
}

//! @brief 뚼됈돶먠믦롦벦
//! @return 뼰뚼갌false?뾎뚼갌true
bool Config::GetSoundFlag()
{
	return SoundFlag;
}

//! @brief 뚼됈돶먠믦
//! @param flag 긲깋긐
void Config::SetSoundFlag(bool flag)
{
	SoundFlag = flag;
}

//! @brief 뢯뙆먠믦롦벦
//! @return 뼰뚼갌false?뾎뚼갌true
bool Config::GetBloodFlag()
{
	return BloodFlag;
}

//! @brief 뢯뙆먠믦
//! @param flag 긲깋긐
void Config::SetBloodFlag(bool flag)
{
	BloodFlag = flag;
}

//! @brief 됪뽋궻뼻귡궠먠믦롦벦
//! @return 맯궻뭠
int Config::GetBrightness()
{
	return Brightness;
}

//! @brief 됪뽋궻뼻귡궠먠믦
//! @param param 맯궻뭠
void Config::SetBrightness(int param)
{
	Brightness = param;
}


//! @brief ?긂긚뵿?먠믦롦벦
//! @return 뼰뚼갌false?뾎뚼갌true
bool Config::GetInvertMouseFlag()
{
	return InvertMouseFlag;
}

//! @brief ?긂긚뵿?먠믦
//! @param flag 긲깋긐
void Config::SetInvertMouseFlag(bool flag)
{
	InvertMouseFlag = flag;
}

//! @brief 긲깒??긚긌긞긵먠믦롦벦
//! @return 뼰뚼갌false?뾎뚼갌true
bool Config::GetFrameskipFlag()
{
	return FrameskipFlag;
}

//! @brief 긲깒??긚긌긞긵먠믦
//! @param flag 긲깋긐
void Config::SetFrameskipFlag(bool flag)
{
	FrameskipFlag = flag;
}

//! @brief 빶궻뤖?귩럊뾭먠믦롦벦
//! @return 뼰뚼갌false?뾎뚼갌true
bool Config::GetAnotherGunsightFlag()
{
	return AnotherGunsightFlag;
}

//! @brief 빶궻뤖?귩럊뾭먠믦
//! @param flag 긲깋긐
void Config::SetAnotherGunsightFlag(bool flag)
{
	AnotherGunsightFlag = flag;
}

//! @brief 긵깒귽깂?뼹롦벦
//! @param out_str 롷궚롦귡빒럻쀱??귽깛?
//! @return 긵깒귽깂?뼹빒럻릶
int Config::GetPlayerName(char *out_str)
{
	if( out_str == NULL ){ return 0; }

	//?귽깛?궸긓긯?궥귡
	strcpy(out_str, PlayerName);

	//빒럻릶귩뺅궥
	return (int)strlen(PlayerName);
}

//! @brief 긵깒귽깂?뼹먠믦
//! @param in_str 긵깒귽깂?뼹
void Config::SetPlayerName(char *in_str)
{
	if( in_str == NULL ){ return; }

	if( strlen(in_str) > (MAX_PLAYERNAME - 1) ){
		in_str[MAX_PLAYERNAME - 1] = 0x00;
	}

	//빒럻쀱귩긓긯?궥귡
	strcpy(PlayerName, in_str);
}

//! @brief 긆깏긙긥깑긌?긓?긤궻뼹멟귩롦벦
//! @param code 긆깏긙긥깑긌?긓?긤
//! @return 긌?궻뼹멟
const char* Config::GetOriginalkeycodeString(int code)
{
	const char *out;

	switch(code){
		case 0x00: out = "UP"; break;
		case 0x01: out = "DOWN"; break;
		case 0x02: out = "LEFT"; break;
		case 0x03: out = "RIGHT"; break;
		case 0x04: out = "NUM0"; break;
		case 0x05: out = "NUM1"; break;
		case 0x06: out = "NUM2"; break;
		case 0x07: out = "NUM3"; break;
		case 0x08: out = "NUM4"; break;
		case 0x09: out = "NUM5"; break;
		case 0x0A: out = "NUM6"; break;
		case 0x0B: out = "NUM7"; break;
		case 0x0C: out = "NUM8"; break;
		case 0x0D: out = "NUM9"; break;
		case 0x0E: out = "BACKSPACE"; break;
		case 0x0F: out = "ENTER"; break;

		case 0x10: out = "TAB"; break;
		case 0x11: out = "SPACE"; break;
		case 0x12: out = "MOUSE L"; break;
		case 0x13: out = "MOUSE R"; break;
		case 0x14: out = "SHIFT"; break;
		case 0x15: out = "CTRL"; break;
		case 0x16: out = "0"; break;
		case 0x17: out = "1"; break;
		case 0x18: out = "2"; break;
		case 0x19: out = "3"; break;
		case 0x1A: out = "4"; break;
		case 0x1B: out = "5"; break;
		case 0x1C: out = "6"; break;
		case 0x1D: out = "7"; break;
		case 0x1E: out = "8"; break;
		case 0x1F: out = "9"; break;

		case 0x20: out = "A"; break;
		case 0x21: out = "B"; break;
		case 0x22: out = "C"; break;
		case 0x23: out = "D"; break;
		case 0x24: out = "E"; break;
		case 0x25: out = "F"; break;
		case 0x26: out = "G"; break;
		case 0x27: out = "H"; break;
		case 0x28: out = "I"; break;
		case 0x29: out = "J"; break;
		case 0x2A: out = "K"; break;
		case 0x2B: out = "L"; break;
		case 0x2C: out = "M"; break;
		case 0x2D: out = "N"; break;
		case 0x2E: out = "O"; break;
		case 0x2F: out = "P"; break;

		case 0x30: out = "Q"; break;
		case 0x31: out = "R"; break;
		case 0x32: out = "S"; break;
		case 0x33: out = "T"; break;
		case 0x34: out = "U"; break;
		case 0x35: out = "V"; break;
		case 0x36: out = "W"; break;
		case 0x37: out = "X"; break;
		case 0x38: out = "Y"; break;
		case 0x39: out = "Z"; break;
		case 0x3A: out = "/"; break;
		case 0x3B: out = ":"; break;
		case 0x3C: out = ";"; break;
		case 0x3D: out = "-"; break;
		case 0x3E: out = "@"; break;
		case 0x3F: out = "["; break;

		case 0x40: out = "]"; break;
		case 0x41: out = "BACKSLASH"; break;
		case 0x42: out = "\\"; break;
		case 0x43: out = ","; break;
		case 0x44: out = "."; break;
		case 0x45: out = "^"; break;
		case 0x46: out = "NUM*" ; break;
		case 0x47: out = "NUM/"; break;
		case 0x48: out = "NUM+"; break;
		case 0x49: out = "NUM-"; break;
		case 0x4A: out = "NUM."; break;

		default: out = "NULL";
	}

	return out;
}


//! @brief 믁돿먠믦긲?귽깑귩맯맟궥귡
//! @param fname 긲?귽깑뼹
//! @return 맟뚻갌true?렪봲/븉뾴갌false
bool Config::CreateExtConfig(const char *fname)
{
#ifdef ENABLE_ADDCONFIG
	if( fname == NULL ){ return false; }

#ifdef ENABLE_PATH_DELIMITER_SLASH
	//긬긚뗦먛귟빒럻귩빾듂
	fname = ChangePathDelimiter(fname);
#endif

	FILE *fp;

	//긲?귽깑궕뫔띪궥귡궶귞렪봲걁긲?귽깑맯맟븉뾴걂
	fp = fopen(fname, "r");
	if( fp != NULL ){
		fclose(fp);
		return false;
	}

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_LOAD, "Config File (Create .ini)", fname);
#endif

	fp = fopen(fname, "w");
	if( fp == NULL ){ return false; }

	fprintf(fp, "[Game]\n");
	fprintf(fp, ";Game language.\n");
	fprintf(fp, ";  0: English + Japanese  (default)\n");
	fprintf(fp, ";  1: English + Korean\n");
	fprintf(fp, "Language = 0\n");
	fprintf(fp, "\n");
	fprintf(fp, ";Show player arm and weapon.\n");
	fprintf(fp, ";  0: OFF (default)\n");
	fprintf(fp, ";  1: ON\n");
	fprintf(fp, "ShowArm = 0\n");
	fprintf(fp, "\n");
	fprintf(fp, "\n");

	fprintf(fp, "[Graphics]\n");
	fprintf(fp, ";WindowSize\n");
	fprintf(fp, ";  0: 640x480  [4:3] (default)\n");
	fprintf(fp, ";  1: 800x600  [4:3]\n");
	fprintf(fp, ";  2:1024x768  [4:3]\n");
	fprintf(fp, ";  3:1280x960  [4:3]\n");
	fprintf(fp, ";  4:1600x1200 [4:3]\n");
	fprintf(fp, "; 10:1280x720  [16:9]\n");
	fprintf(fp, "; 11:1600x900  [16:9]\n");
	fprintf(fp, "; 12:1920x1080 [16:9]\n");
	//fprintf(fp, "; 13:2560x1440 [16:9]\n");
	//fprintf(fp, "; 14:3840x2160 [16:9]\n");
	fprintf(fp, "WindowSize = 0\n");
	fprintf(fp, "\n");
	fprintf(fp, "\n");

	fprintf(fp, "[Input]\n");
	fprintf(fp, ";Limit mouse input control.\n");
	fprintf(fp, ";  0: OFF\n");
	fprintf(fp, ";  1: ON (default)\n");
	fprintf(fp, "MouseLimit = 1\n");
	fprintf(fp, "\n");
	fprintf(fp, "\n");

	fprintf(fp, "[Sounds]\n");
	fprintf(fp, ";Game sound volume. (0.0 ~ 1.0)\n");
	fprintf(fp, "MasterVolume = 1.0\n");
	fprintf(fp, "\n");

	fprintf(fp, "\n");
	fprintf(fp, "[Gameplay]\n");
	fprintf(fp, ";Player damage taken multiplier. (1.0 = 100%%, 0.5 = 50%%)\n");
	fprintf(fp, "PlayerDamageMultiplier = 1.0\n");
	fprintf(fp, "\n");
	fprintf(fp, ";Player health regeneration amount. (Applied every 10 seconds)\n");
	fprintf(fp, "PlayerHealthRegenAmount = 10\n");
	fprintf(fp, "\n");

	fprintf(fp, "[AI]\n");
	fprintf(fp, "; AI Reaction speed multiplier. (1.0 = Default, 0.5 = Slow, 2.0 = Fast)\n");
	fprintf(fp, "AIReactionSpeedMultiplier = 1.0\n");
	fprintf(fp, "; AI Fire rate multiplier. (1.0 = Default, 2.0 = 2x Faster)\n");
	fprintf(fp, "AIFireRateMultiplier = 1.0\n");
	fprintf(fp, "\n");

	fclose(fp);

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif
	return true;
#else
	return false;
#endif
}

//! @brief 믁돿먠믦긲?귽깑귩벶귒뜛귔
//! @param fname 긲?귽깑뼹
//! @return 맟뚻갌0?렪봲갌1
int Config::LoadExtFile(const char *fname)
{
#ifdef ENABLE_ADDCONFIG
	if( fname == NULL ){ return 1; }

	INIFileInterface INIConfig;
	int mode;
	
#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_LOAD, "Config File (.ini)", fname);
#endif

#ifdef ENABLE_PATH_DELIMITER_SLASH
	//긬긚뗦먛귟빒럻귩빾듂
	fname = ChangePathDelimiter(fname);
#endif

	INIConfig.LoadINIFile(fname);

	//?렑뙻뚭롦벦
	Language = INIConfig.GetINIFileInt("Game", "Language", 0, NULL);

	//쁱?렑긲깋긐롦벦
	mode = INIConfig.GetINIFileInt("Game", "ShowArm", 0, NULL);
	if( mode == 0 ){ ShowArmFlag = false; }
	else{ ShowArmFlag = true; }

	//됶몴뱗롦벦
	mode = INIConfig.GetINIFileInt("Graphics", "WindowSize", 0, NULL);
	switch(mode){
		case  0: ScreenWidth =  640; ScreenHeight =  480; break;
		case  1: ScreenWidth =  800; ScreenHeight =  600; break;
		case  2: ScreenWidth = 1024; ScreenHeight =  768; break;
		case  3: ScreenWidth = 1280; ScreenHeight =  960; break;
		case  4: ScreenWidth = 1600; ScreenHeight = 1200; break;
		case 10: ScreenWidth = 1280; ScreenHeight =  720; break;
		case 11: ScreenWidth = 1600; ScreenHeight =  900; break;
		case 12: ScreenWidth = 1920; ScreenHeight = 1080; break;
		case 13: ScreenWidth = 2560; ScreenHeight = 1440; break;
		case 14: ScreenWidth = 3840; ScreenHeight = 2160; break;
		default: ScreenWidth = DEFAULT_SCREEN_WIDTH; ScreenHeight = DEFAULT_SCREEN_HEIGHT; break;
	}

	//?긂긚댷벍쀊맕뙽긲깋긐
	mode = INIConfig.GetINIFileInt("Input", "MouseLimit", 1, NULL);
	if( mode == 0 ){ MouseLimitFlag = false; }
	else{ MouseLimitFlag = true; }

	/*
	//돶쀊롦벦
	mastervolume = INIConfig.GetINIFileFloat("Sounds", "MasterVolume", 1.0f, NULL);
	if( mastervolume < 0.0f ){ mastervolume = 0.0f; }
	if( mastervolume > 1.0f ){ mastervolume = 1.0f; }

	//プレイヤ?の被ダメージ倍率取得
	DamageMultiplier = INIConfig.GetINIFileFloat("Gameplay", "PlayerDamageMultiplier", 1.0f, NULL);
	if (DamageMultiplier < 0.0f) { DamageMultiplier = 0.0f; }

	//プレイヤ?の体力回復量取得
	HealthRegenAmount = INIConfig.GetINIFileInt("Gameplay", "PlayerHealthRegenAmount", 10, NULL);
	if (HealthRegenAmount < 0) { HealthRegenAmount = 0; }


	AIReactionSpeedMultiplier = INIConfig.GetINIFileFloat("AI", "AIReactionSpeedMultiplier", 1.0f, NULL);
	if (AIReactionSpeedMultiplier < 0.1f) { AIReactionSpeedMultiplier = 0.1f; } // 최소값 안전장치

	AIFireRateMultiplier = INIConfig.GetINIFileFloat("AI", "AIFireRateMultiplier", 1.0f, NULL);
	if (AIFireRateMultiplier < 0.1f) { AIFireRateMultiplier = 0.1f; }
	*/

	INIConfig.ReleaseINIFile();

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

#else
	ShowArmFlag = false;
	ScreenWidth = DEFAULT_SCREEN_WIDTH;
	ScreenHeight = DEFAULT_SCREEN_HEIGHT;
	mastervolume = 1.0f;
#endif
	return 0;
}

//! @brief 믁돿먠믦뭠귩긢긲긅깑긣궻뭠궳룊딖돸
void Config::SetDefaultExtConfig()
{
	Language = 0;
	ShowArmFlag = false;
	ScreenWidth = DEFAULT_SCREEN_WIDTH;
	ScreenHeight = DEFAULT_SCREEN_HEIGHT;
	MouseLimitFlag = true;

	// ★ [수정] 요청하신 기본값으로 설정
	mastervolume = 1.0f;				 // 마스터 볼륨 1.0
	DamageMultiplier = 0.1f;			 // 플레이어 데미지 0.1 (10%)
	HealthRegenAmount = 10;				 // 체력 회복 10
	AIReactionSpeedMultiplier = 1.0f;
	AIFireRateMultiplier = 1.0f;
	AIAccuracyMultiplier = 1.0f;
	AISearchRangeMultiplier = 1.0f;
	RecoilSideMultiplier = 0.0f;

}

//! @brief 뙻뚭먠믦귩롦벦
//! @return 뭠
int Config::GetLanguage()
{
	return Language;
}

//! @brief 쁱?렑긲깋긐귩롦벦
//! @return 긲깋긐
bool Config::GetShowArmFlag()
{
	return ShowArmFlag;
}

//! @brief 됪뽋됶몴뱗걁븴걂귩롦벦
//! @return 뭠
int Config::GetScreenWidth()
{
	return ScreenWidth;
}

//! @brief 됪뽋됶몴뱗걁뛼궠걂귩롦벦
//! @return 뭠
int Config::GetScreenHeight()
{
	return ScreenHeight;
}

//! @brief ?긂긚댷벍쀊맕뙽긲깋긐
//! @return 뭠
bool Config::GetMouseLimitFlag()
{
	return MouseLimitFlag;
}

int Config::GetRenderFpsLimit()
{
	return RenderFpsLimit;
}

void Config::SetRenderFpsLimit(int fps)
{
	// 0은 제한 없음
	if (fps == 0) {
		RenderFpsLimit = 0;
		return;
	}

	if (fps < RENDERFPS_MIN) fps = RENDERFPS_MIN;
	if (fps > RENDERFPS_MAX) fps = RENDERFPS_MAX;

	RenderFpsLimit = fps;
}

//! @brief 돶쀊먠믦귩롦벦
//! @return 뭠
float Config::GetVolume()
{
	return mastervolume;
}

//! @brief プレイヤ?の被ダメージ倍率を取得
//! @return 値
float Config::GetDamageMultiplier()
{
	return DamageMultiplier;
}

//! @brief プレイヤ?の体力回復量を取得
//! @return 値
/*int Config::GetHealthRegenAmount()
{
	return HealthRegenAmount;
}


float Config::GetAIReactionSpeedMultiplier()
{
	return AIReactionSpeedMultiplier;
}

float Config::GetAIFireRateMultiplier()
{
	return AIFireRateMultiplier;
}*/