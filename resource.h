//! @file resource.h
//! @brief ResourceManager긏깋긚궻먬뙻

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

#ifndef RESOURCE_H
#define RESOURCE_H

#define TOTAL_UPMODE 6				//!< 뤵뵾릆궻뜃똶긾긢깑
#define TOTAL_ARMMODE 4				//!< 팔 모델 수
#define TOTAL_WALKMODE 8			//!< 뫉걁뺖궖걂궻뜃똶긾긢깑
#define TOTAL_RUNMODE 12			//!< 뫉걁몪귟걂궻뜃똶긾긢깑
#define TOTAL_SITWALKMODE 4			// 앉은 상태 걷기 다리 모델 수

#define MAX_LOADHUMANTEXTURE 15		//!< 릐궻긡긏긚?긿귩벶귒뜛귔띍묈뻼릶

#define ENABLE_HUMANMODEL_MORPHING	//!< 릐긾긢깑궻긾?긲귻깛긐룉뿚뾎뚼돸걁긓긽깛긣돸궳??뼰뚼걂

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 2		//!< Select include file.
#endif
#include "main.h"

#ifndef RELOAD_SOUND_ID_DEFINED
#define RELOAD_SOUND_ID_DEFINED

enum ReloadSoundID {
	RELOAD_NONE = -1,

	RELOAD_PISTOL_EMPTY = 0,
	RELOAD_PISTOL_TACTICAL,

	RELOAD_HEAVY_PISTOL_EMPTY,
	RELOAD_HEAVY_PISTOL_TACTICAL,

	RELOAD_SMG_EMPTY,
	RELOAD_SMG_TACTICAL,
	RELOAD_CLASSIC_SMG,

	RELOAD_AR_EMPTY,
	RELOAD_AR_TACTICAL,
	RELOAD_AK_EMPTY,
	RELOAD_AK_TACTICAL,
	RELOAD_BULLPUP_EMPTY,
	RELOAD_BULLPUP_TACTICAL,

	RELOAD_BAR,

	RELOAD_P90_EMPTY,
	RELOAD_P90_TACTICAL,
	RELOAD_BIZON_EMPTY,
	RELOAD_BIZON_TACTICAL,

	RELOAD_LMG_BOX,
	RELOAD_LMG_BELT,
	RELOAD_DP28,
	RELOAD_MINIGUN,
	RELOAD_MINIGUN_SPIN_START, // 플레이어 미니건 예열 시작음
	RELOAD_MINIGUN_SPIN,       // 미니건 예열/회전 유지음
	RELOAD_MINIGUN_SPIN_END,   // 플레이어 미니건 예열 종료음

	RELOAD_DMR_EMPTY,
	RELOAD_DMR_TACTICAL,

	RELOAD_SNIPER_EMPTY,
	RELOAD_SNIPER_TACTICAL,

	RELOAD_RELOAD_START,
	RELOAD_SHOTGUN_SHELL_INSERT,
	RELOAD_LEVER_SHELL_INSERT,

	RELOAD_BOLT_RELOAD_START,
	RELOAD_SNIPER_SHELL_INSERT,
	RELOAD_BOLT_RELOAD_END,

	RELOAD_SHOTGUN_MAG_EMPTY,
	RELOAD_SHOTGUN_MAG_TACTICAL,
	RELOAD_DRUM_SHOTGUN,

	RELOAD_REVOLVER,
	RELOAD_KAR98_CLIP,
	RELOAD_GARAND_CLIP,
	RELOAD_DOUBLE_BARREL,

	RELOAD_AWP_BOLT,
	RELOAD_KAR98_SNIPER_BOLT,

	RELOAD_SHOTGUN_ACTION,
	RELOAD_WINCHESTER_LEVER,

	RELOAD_DRY_FIRE,   // 빈 탄창 딸깍 소리

	ZOMBIE_ATTACK_1,
	ZOMBIE_ATTACK_2,

	WEAPONMODE_PISTOL_SUPPRESSOR_ATTACH,
	WEAPONMODE_PISTOL_SUPPRESSOR_DETACH,
	WEAPONMODE_LONGGUN_SUPPRESSOR_ATTACH,
	WEAPONMODE_LONGGUN_SUPPRESSOR_DETACH,
	WEAPONMODE_FIREMODE_CHANGE,

	TOTAL_RELOAD_SOUND
};

#endif

#ifndef SKILL_SOUND_ID_DEFINED
#define SKILL_SOUND_ID_DEFINED

// 플레이어 스킬 공용 효과음
enum SkillSoundID {
	// 공용 스킬 사운드
	SKILL_SOUND_COMMON_START = 0,
	SKILL_SOUND_COMMON_READY,
	SKILL_SOUND_COMMON_FAIL,
	SKILL_SOUND_COMMON_END,
	// 테러리스트: 교란 신호
	SKILL_SOUND_DISTRACTION_BEEP,
	// 로봇 처형 프로토콜: 순차 처형 총소리
	SKILL_SOUND_ROBOT_EXECUTE_SHOT,
	// 경찰 방패: 총알 방어 피격음
	SKILL_SOUND_SHIELD_BLOCK,
	// 테러리스트 지뢰: 감지음
	SKILL_SOUND_TERRORIST_MINE_TRIGGER,
	// 테러리스트 지뢰: 활성화 완료음
	SKILL_SOUND_TERRORIST_MINE_ARMED,

	TOTAL_SKILL_SOUND
};

#endif

#ifndef SKILL_ICON_ID_DEFINED
#define SKILL_ICON_ID_DEFINED

enum SkillIconID {
	SKILL_ICON_STEALTH = 0,
	SKILL_ICON_AMMOBOOST,
	SKILL_ICON_SCAN,
	SKILL_ICON_HEAL,
	SKILL_ICON_SURVIVAL,
	SKILL_ICON_DODGE,
	SKILL_ICON_ABSORB,
	SKILL_ICON_MARKSHOT,
	SKILL_ICON_ROBOT_EXECUTE,
	SKILL_ICON_DISTRACTION,
	SKILL_ICON_SUMMON,
	SKILL_ICON_SUPPRESS,
	SKILL_ICON_HOOK,
	SKILL_ICON_IRONBODY,
	SKILL_ICON_RAGE,
	SKILL_ICON_TELEPORT,
	SKILL_ICON_IMPACT_GRENADE,
	SKILL_ICON_SHIELD,
	SKILL_ICON_DP28,
	SKILL_ICON_TERRORIST_MINE,
	SKILL_ICON_BIOTIC_FIELD,
	SKILL_ICON_ARTILLERY_STRIKE,
	SKILL_ICON_SHADOW_DECOY,
	SKILL_ICON_AIMHACK,

	TOTAL_SKILL_ICON
};

#endif

//! @brief 깏??긚듖뿚긏깋긚
//! @details 롥궸럊뾭궠귢귡긾긢깑귘긡긏긚?긿궓귝귂긖긂깛긤귩듖뿚궢귏궥갃?걁덇븫귩룣궘걂
class ResourceManager
{
	class ParameterInfo *ParamInfo;				//!< 듖뿚궥귡먠믦뭠륃뺪긏깋긚
	class D3DGraphics *d3dg;					//!< 듖뿚궥귡?됪긏깋긚
	class SoundControl *SoundCtrl;				//!< 듖뿚궥귡긖긂깛긤긏깋긚

	int human_upmodel[TOTAL_UPMODE];			//!< 뤵뵾릆궻긾긢깑
	int human_armmodel[TOTAL_ARMMODE];			//!< 쁱궻긾긢깑
	int human_legmodel;							//!< 뫉걁먄?륉뫴걂궻긾긢깑
	int human_walkmodel[TOTAL_WALKMODE];		//!< 뫉걁뺖궖걂궻긾긢깑
	int human_runmodel[TOTAL_RUNMODE];			//!< 뫉걁몪귟걂궻긾긢깑

	int human_sitlegmodel;						// 앉기 정지 다리 모델
	int human_sitwalkmodel[TOTAL_SITWALKMODE];	// 앉기 이동 다리 모델
	int human_texture_Param[MAX_LOADHUMANTEXTURE];		//!< 릐궻긡긏긚?긿궻뫮돒붥뜂
	int human_texture_d3dg[MAX_LOADHUMANTEXTURE];		//!< 릐궻긡긏긚?긿궻긢??붥뜂
	int weapon_model[TOTAL_PARAMETERINFO_WEAPON];		//!< 븧딇궻긾긢깑궻긢??붥뜂
	int weapon_texture[TOTAL_PARAMETERINFO_WEAPON];		//!< 븧딇궻긡긏긚?긿궻긢??붥뜂
	int weapon_sound[TOTAL_PARAMETERINFO_WEAPON];		//!< 븧딇궻긖긂깛긤궻긢??붥뜂
	int weapon_reloadsound_group[TOTAL_RELOAD_SOUND];
	int weapon_reloadsound;								//!< 븧딇궻깏깓?긤긖긂깛긤궻긢??붥뜂
	int smallobject_model[TOTAL_PARAMETERINFO_SMALLOBJECT + MAX_ADDSMALLOBJECT];		//!< 룷븿궻긾긢깑궻긢??붥뜂
	int smallobject_texture[TOTAL_PARAMETERINFO_SMALLOBJECT + MAX_ADDSMALLOBJECT];		//!< 룷븿궻긡긏긚?긿궻긢??붥뜂
	int smallobject_sound[TOTAL_PARAMETERINFO_SMALLOBJECT + MAX_ADDSMALLOBJECT];		//!< 룷븿궻긖긂깛긤궻긢??붥뜂
	int bullet_model[TOTAL_PARAMETERINFO_BULLET];		//!< 뭙궻긾긢깑
	int bullet_texture[TOTAL_PARAMETERINFO_BULLET];		//!< 뭙궻긡긏긚?긿

	int scopetexture;			//!< 긚긓?긵궻긡긏긚?긿
	int skymodel;				//!< 뗴봶똧궻긾긢깑
	int skytexture;				//!< 뗴봶똧궻긡긏긚?긿
	int bullet_hitsoundA;		//!< 뭙궻긭긞긣긖긂깛긤걁A걂
	int bullet_hitsoundB;		//!< 뭙궻긭긞긣긖긂깛긤걁B걂
	int bullet_humanhitsound;	//!< 뭙궻긭긞긣긖긂깛긤걁릐걂
	int bullet_passingsound;	//!< 뭙궻믅됡긖긂깛긤
	int grenade_bang;			//!< 롨왮뭙궻뵚뵯돶
	int grenade_cco;			//!< 롨왮뭙궻긫긂깛긤돶
	int effecttexture_blood;	//!< 긄긲긃긏긣뾭갋뙆긡긏긚?긿
	int effecttexture_mflash;	//!< 긄긲긃긏긣뾭갋긲깋긞긘깄긡긏긚?긿
	int effecttexture_smoke;	//!< 긄긲긃긏긣뾭갋뎹긡긏긚?긿
	int effecttexture_yakkyou;	//!< 긄긲긃긏긣뾭갋뽶娥긡긏긚?긿

	int sound_nvg;  // 야간 투시경 소리 핸들

	// 플레이어 스킬 사운드
	int skill_sound[TOTAL_SKILL_SOUND];

	// 플레이어 스킬 아이콘
int skillicon_texture[TOTAL_SKILL_ICON];

	int footstep_sound[10];		// [✨ 추가] 0:기본, 1:흙, 2:철, 3:물

public:
	ResourceManager();
	~ResourceManager();
	void SetParameterInfo(ParameterInfo *_ParamInfo);
	void SetD3DGraphics(D3DGraphics *_d3dg);
	void SetSoundControl(SoundControl *_SoundCtrl);
	void DestroyResource();

	int LoadHumanModel();
	void GetHumanModel(int out_upmodel[], int out_armmodel[], int* legmodel, int out_walkmodel[], int out_runmodel[]);
	void GetHumanSitModel(int* sitlegmodel, int out_sitwalkmodel[]);
	void CleanupHumanModel();
	int AddHumanTexture(int id);
	int GetHumanTextures();
	int GetHumanTexture(int id);
	int AddHumanTextureByTextureID(int texture_id);
	int GetHumanTextureByTextureID(int texture_id);
	bool GetHumanTextureInfo(int dataid, int* ParamID, int* TextureID);
	void CleanupHumanTexture();
	int LoadWeaponModelTexture();
	int GetWeaponModelTexture(int id, int *model, int *texture);

	int LoadNVGSound();   // 소리 로드 함수
	int GetNVGSound();    // 소리 핸들 반환 함수
	void CleanupNVGSound();     // [✨ 추가] 사운드 해제 함수 선언

	// 플레이어 스킬 사운드
	int LoadSkillSound();
	int GetSkillSound(int id);
	void CleanupSkillSound();

	// 플레이어 스킬 아이콘
	int LoadSkillIconTexture();
	int GetSkillIconTexture(int id);
	int GetSkillStealthIconTexture(); // 기존 코드 호환용
	void CleanupSkillIconTexture();

	// [✨ 추가] 발소리 관련 함수 선언
	int LoadFootstepSound();
	int GetFootstepSound(int type);
	void CleanupFootstepSound();

#ifdef ENABLE_BUG_HUMANWEAPON
	int GetBugWeaponModelTexture(int id, int *model, int *texture);
#endif
	void CleanupWeaponModelTexture();
	int LoadWeaponSound();
	int GetWeaponSound(int id);
	int GetReloadSound(int id);
	void CleanupWeaponSound();
	int LoadSmallObjectModelTexture();
	int GetSmallObjectModelTexture(int id, int *model, int *texture);
	void CleanupSmallObjectModelTexture();
	int LoadSmallObjectSound();
	int GetSmallObjectSound(int id);
	void CleanupSmallObjectSound();
	int LoadAddSmallObject(int id, char *modelpath, char *texturepath, char *soundpath);
	void CleanupAddSmallObject(int id);
	int LoadBulletModelTexture();
	int GetBulletModelTexture(int id, int *model, int *texture);
	void CleanupBulletModelTexture();

	int LoadScopeTexture();
	int GetScopeTexture();
	void CleanupScopeTexture();
	int LoadSkyModelTexture(int id);
	void GetSkyModelTexture(int *model, int *texture);
	void CleanupSkyModelTexture();
	int LoadBulletSound();
	void GetBulletSound(int *hitsoundA, int *hitsoundB, int *humanhitsound, int *passingsound, int *grenadebang, int *grenadecco);
	void CleanupBulletSound();
	int LoadEffectTexture();
	int GetEffectBloodTexture();
	int GetEffectMflashTexture();
	int GetEffectSmokeTexture();
	int GetEffectYakkyouTexture();
	void CleanupEffectTexture();
};

#endif