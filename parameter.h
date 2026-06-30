//! @file parameter.h
//! @brief ParameterInfo긏깋긚궻먬뙻

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

// 릐갋븧딇갋룷븿 먠믦긢??궻둴봃띿떾
// ?Manda궠귪?걁2014봏12뙉걂

#ifndef PARAMETER_H
#define PARAMETER_H

#define TOTAL_PARAMETERINFO_HUMAN 46			//!< 릐궻먠믦릶
#define TOTAL_HUMANTEXTURE 31					//!< 릐궻긡긏긚?긿궻뜃똶뻼릶
#define TOTAL_PARAMETERINFO_WEAPON 95			//!< 무기 설정 수
#define TOTAL_PARAMETERINFO_SMALLOBJECT 12	//!< 룷븿궻먠믦릶
#define TOTAL_PARAMETERINFO_BULLET 2		//!< 뢤뭙긆긳긙긃긏긣궻먠믦릶
#define TOTAL_OFFICIALMISSION 58			//!< 뷭??긞긘깈깛궻릶
#define TOTAL_PARAMETERINFO_AILEVEL 6		//!< AI깒긹깑궻먠믦릶
#define TOTAL_PARAMETERINFO_SCOPE 8			//!< 긚긓?긵궻먠믦릶

#define ID_WEAPON_NONE 0		//!< 무기：없음 으로서 취급하는 무기번호
#define ID_WEAPON_GRENADE 13			//!< 일반 수류탄
#define ID_WEAPON_IMPACT_GRENADE 91		//!< 충격 수류탄
#define ID_WEAPON_SHIELD 92				//!< 경찰 방패
#define ID_WEAPON_DP28 93					//!< 용병 DP28
#define ID_WEAPON_MK14EBR 27				//!< 기존 Mk14 EBR
#define ID_WEAPON_ROBOT_EXECUTE_RIFLE 94	//!< 로봇 처형 프로토콜 전용 Mk14 EBR
#define ID_WEAPON_CASE 15					//!< 케이스

// WeaponP 팔 자세 타입
#define WEAPONP_TWOHAND 0				//!< 양손 무기 팔
#define WEAPONP_ONEHAND 1				//!< 한손 무기 팔
#define WEAPONP_NOWEAPON 2				//!< 맨손 팔
#define WEAPONP_SHIELD 3				//!< 방패 전용 팔

#define OFFICIALMISSION_BD1 "temp.bd1"		//!< 뷭??긞긘깈깛궻BD1긲?귽깑뼹

#define SMALLOBJECT_SCALE 5.0f				//!< 룷븿궻?됪?뿦

#define ID_HUMAN_WHITE_DUMMY 43			//더미
#define ID_HUMAN_WHITE_DUMMY_ALIAS 200

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 1		//!< Select include file.
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

//! 릐먠믦뾭궻?몾뫬
typedef struct
{
	int texture;		//!< 긡긏긚?긿붥뜂
	int model;			//!< 긾긢깑붥뜂
	int hp;				//!< 뫬쀍
	int AIlevel;		//!< AI깒긹깑
	int AItrackability;	//!< AI뛘똼렄궻믁?맜걁?깛긮궻귒걂
	int Weapon[TOTAL_HAVEWEAPON];	//!< 븧딇
	int type;			//!< 롰쀞
} HumanParameter;


//! 븧딇먠믦뾭궻?몾뫬
typedef struct
{
	const char *name;		//!< 븧딇뼹
	const char *model;		//!< 긾긢깑긲?귽깑뼹
	const char *texture;	//!< 긡긏긚?긿긲?귽깑뼹
	int attacks;		//!< 뛘똼쀍
	int penetration;	//!< 듥믅쀍
	float blazings;		//!< 쁀롅듩둼
	int speed;			//!< 뭙뫊
	int nbsmax;			//!< 몧뭙릶
	int reloads;		//!< 깏깓?긤렄듩
	int reaction;		//!< 뵿벍둷뱗
	int ErrorRangeMIN;	//!< 뭙벞뚮뜼 띍룷
	int ErrorRangeMAX;	//!< 뭙벞뚮뜼 띍묈
	float mx;		//!< 뢤궻댧뭫갌X
	float my;		//!< 뢤궻댧뭫갌Y
	float mz;		//!< 뢤궻댧뭫갌Z
	float flashx;	//!< ?긛깑긲깋긞긘깄궻댧뭫갌X
	float flashy;	//!< ?긛깑긲깋긞긘깄궻댧뭫갌Y
	float flashz;	//!< ?긛깑긲깋긞긘깄궻댧뭫갌Z
	float yakkyou_px;	//!< 뽶娥봱뢯 댧뭫갌X
	float yakkyou_py;	//!< 뽶娥봱뢯 댧뭫갌Y
	float yakkyou_pz;	//!< 뽶娥봱뢯 댧뭫갌Z
	float yakkyou_sx;	//!< 뽶娥봱뢯 룊뫊갌X
	float yakkyou_sy;	//!< 뽶娥봱뢯 룊뫊갌Y
	float yakkyou_size;	//!< 뽶娥 ?됪?뿦걁겍븠릶궳뛘똼쀍궔귞럁뢯걂
	int yakkyou_delay;	//!< 뽶娥봱뢯 뭯뎱긇긂깛긣
	int burstcnt;		//!< 쁀롅뭙릶걁0갌긲깑긆?긣?1갌긜?긆?긣?2댥뤵갌긫?긚긣긾?긤걂
	int scopemode;		//!< 긚긓?긵긾?긤
	float size;			//!< ?됪?뿦
	int soundid;		//!< 뵯뺺돶궻붥뜂
	int soundvolume;	//!< 뵯뺺돶궻돶쀊
	bool silencer;		//!< 긖귽깒깛긖?궻뾎뼰
	int WeaponP;		//!< 몧뷈뺴?
	int ChangeWeaponID;		//!< 먛귟뫶궑귡븧딇뫮뤭
	int ChangeWeaponCnt;	//!< 븧딇먛귟뫶궑렄듩
	int pellet;			//!< 뵯롅뭙릶걁긻깒긞긣릶걂
	bool is_shell_reload;	//!< 셸 단위 재장전 여부 태그

	bool bHasChamber;           //!< 약실 기능 지원 여부 (True: 폐쇄노리쇠, +1발 가능 / False: 오픈볼트, 리볼버 등)
	int TacticalReloadFrame;    //!< 전술 재장전 시 애니메이션 종료 프레임 (0이면 기본값 사용, 단축용)
	int SoundID_Reload;         // 일반 재장전 소리 (탄창 교체 / 샷건 쉘 넣기)
	int SoundID_Tactical;       //!< 전술 재장전 사운드 ID (-1이면 기본 soundid 사용)

	int SoundID_ReloadStart;   // 셸 단위 재장전 시작음
	int SoundID_ReloadEnd;     // 셸 단위 재장전 종료음

	int SoundID_Action;      // 발사 후 펌프/노리쇠/레버 조작음
	int ActionSoundFrame;    // 발사 후 몇 프레임 뒤에 조작음을 낼지
} WeaponParameter;

//! 룷븿먠믦뾭궻?몾뫬
typedef struct
{
	const char *model;		//!< 긾긢깑뼹
	const char *texture;	//!< 긡긏긚?긿뼹
	int decide;		//!< 뱰궫귟뵽믦궻묈궖궠
	int hp;			//!< 뫯땦쀍
	int sound;		//!< 뼺뭷렄궻뚼됈돶붥뜂
	int jump;		//!< 뷅귂뗰뜃
} SmallObjectParameter;

//! 뢤뭙긆긳긙긃긏긣뾭궻?몾뫬
typedef struct
{
	const char *model;		//!< 긾긢깑뼹
	const char *texture;	//!< 긡긏긚?긿뼹
	float size;				//!< ?됪?뿦
} BulletParameter;

//! 뷭??긞긘깈깛먠믦뾭궻?몾뫬
typedef struct
{
	const char *name;				//!< ?긞긘깈깛렞빶뼹
	const char *fullname;			//!< ?긞긘깈깛맫렜뼹뤝
	const char *directory;			//!< 긢??긢귻깒긏긣깏
	const char *txt;				//!< 긲?귽깑뼹걁.pd1/.txt걂
	bool collision;			//!< 믁돿궻뱰궫귟뵽믦귩렑궥긲깋긐
	bool screen;			//!< 됪뽋귩댠궘궥귡긲깋긐
} OfficialMissionParameter;

//! AI깒긹깑궸귝귡먠믦걁맜?뭠걂뾭?몾뫬
typedef struct
{
	int aiming;			//!< 긄귽?깛긐?쀍
	int attack;			//!< 뛘똼맜
	int search;			//!< 뵯뙥?쀍
	int limitserror;	//!< 떀뾢뚮뜼
} AIParameter;

//! 긚긓?긵먠믦뾭궻?몾뫬
typedef struct
{
	float ViewAngle;				//!< 럨뽰둷
	float MouseRange;				//!< ?띿뒾뱗
	int ScopeGunsight;				//!< 긚긓?긵럊뾭렄궻뤖??렑걁0갌믅륂갂1갌듗댲갂2갌맱뼤걂
	bool NoScopeGunsight;			//!< 믅륂렄궻뤖??렑
	int WeaponNoScopeReaction;		//!< 믅륂뵯뺺렄궻뵿벍
	float WeaponMotionReactionY;	//!< 뵯뺺렄궻뵿벍걁긾?긘깈깛걂
	bool WeaponRecoil_ScopeOnly;	//!< 긚긓?긵 뵯뺺렄궻뵿벍 긚긓?긵렄궻귒
	float WeaponRecoil_ScopeRx;		//!< 긚긓?긵 뵯뺺렄궻뵿벍 돘렡?겍뱗릶? 0.1f뜌귒
	float WeaponRecoil_ScopeRyMin;	//!< 긚긓?긵 뵯뺺렄궻뵿벍 뢢렡띍룷?겍뱗릶? 0.1f뜌귒
	float WeaponRecoil_ScopeRyMax;	//!< 긚긓?긵 뵯뺺렄궻뵿벍 뢢렡띍묈?겍뱗릶? 0.1f뜌귒
	float AI_ShotAngle;							//!< AI 뵯뺺렄궸밎귩뫅궑귡둷뱗
	float AI_ShotAngleLong;						//!< AI 뵯뺺렄궸밎귩뫅궑귡둷뱗걁뮮떁뿣걂
	float AI_AddSearchEnemyMaxDist_Normal;		//!< AI 밎귩뙥궰궚귡떁뿣 돿럁빁걁믅륂렄걂
	float AI_AddSearchEnemyMaxDist_Caution;		//!< AI 밎귩뙥궰궚귡떁뿣 돿럁빁걁똸둀렄걂
} ScopeParameter;

//! @brief 먠믦궠귢궫뭠귩듖뿚궥귡긏깋긚
//! @details ?귕뚂믦궠귢궫먠믦뭠귩듖뿚궢귏궥갃
//! @details 뗰뫬밒궸궼갂릐갋븧딇갋룷븿궔귞갂뷭??긞긘깈깛궸듫똚궥귡먠믦뭠궶궵궳궥갃
class ParameterInfo
{
	HumanParameter *Human;								//!< 릐궻륃뺪귩둰?궥귡?귽깛?
	const char *HumanTexturePath[TOTAL_HUMANTEXTURE];	//!< 릐궻긡긏긚?긿귩둰?궥귡?귽깛?
	WeaponParameter *Weapon;							//!< 븧딇궻륃뺪귩둰?궥귡?귽깛?
#ifdef ENABLE_BUG_HUMANWEAPON
	WeaponParameter *BugWeapon;							//!< 긫긐븧딇궻륃뺪귩둰?궥귡?귽깛?
#endif
	SmallObjectParameter *SmallObject;					//!< 룷븿궻륃뺪귩둰?궥귡?귽깛?
	BulletParameter *Bullet;							//!< 뢤뭙긆긳긙긃긏긣궻륃뺪귩둰?궥귡?귽깛?
	OfficialMissionParameter *MissionData;				//!< 뷭??긞긘깈깛궻륃뺪귩둰?궥귡?귽깛?
	AIParameter *AIlevel;								//!< AI궻맜?뭠
	ScopeParameter *Scope;								//!< 긚긓?긵먠믦귩둰?궥귡?귽깛?

public:
	ParameterInfo();
	~ParameterInfo();
	void InitInfo();
	void DestroyInfo();
	int GetHuman(int id, HumanParameter *out_data);
	int GetHumanTexturePath(int id, char *out_str);
	int GetWeapon(int id, WeaponParameter *out_data);
#ifdef ENABLE_BUG_HUMANWEAPON
	int GetBugWeapon(int id, WeaponParameter *out_data);
#endif
	int GetSmallObject(int id, SmallObjectParameter *out_data);
	int GetBullet(int id, BulletParameter *out_data);
	int GetOfficialMission(int id, char *name, char *fullname, char* directory, char *txt, bool *collisionflag, bool *screenflag);
	int GetAIlevel(int level, AIParameter **out_AIlevel);
	int GetScopeParam(int id, ScopeParameter *out_data);
};

#endif