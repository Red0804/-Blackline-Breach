//! @file soundmanager.h
//! @brief SoundManager긏깋긚궻먬뙻

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

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#define MAX_SOUNDMGR_LIST 200		//!< 긖긂깛긤깏긚긣궻띍묈릶

#define MAX_SOUNDHITMAP 75			//!< ?긞긵뭶뭙돶궻띍묈돶쀊
#define MAX_SOUNDHITSMALLOBJ 110	//!< 룷븿뭶뭙돶궻띍묈돶쀊
#define MAX_SOUNDHITHUMAN 80		//!< 릐뭶뭙걁뷀뭙걂돶궻띍묈돶쀊
#define MAX_SOUNDPASSING 90			//!< 뭙궕돘먛귡돶궻띍묈돶쀊
#define MAX_SOUNDCCOGRENADE 100		//!< 롨왮뭙 긫긂깛긤갋뮫궺뺅귟돶궻띍묈돶쀊
#define MAX_SOUNDHITGRENADE 120		//!< 롨왮뭙 뵚뵯돶궻띍묈돶쀊
#define MAX_SOUNDSHIELDBLOCK 90		//!< 방패 총알 방어 피격음 최대 볼륨

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 2		//!< Select include file.
#endif
#include "main.h"

//! 긖긂깛긤깏긚긣걏paramid걐뾭믦릶
enum SoundmgrID
{
	SHOT_WEAPON = 0,		//!< 뵯뺺돶
	SHOT_WEAPON_PLAYER,		//!< 긵깒귽깂?렔릆궻뵯뺺돶
	HIT_MAP,				//!< ?긞긵뭶뭙돶
	HIT_HUMAN_HEAD,			//!< 뷀뭙돶걁벆걂
	HIT_HUMAN_UP,			//!< 뷀뭙돶걁뤵뵾릆걂
	HIT_HUMAN_LEG,			//!< 뷀뭙돶걁돷뵾릆걂
	HIT_HUMAN_ZOMBIE,		//!< ?깛긮뛘똼돶
	HIT_SHIELD_BLOCK,		//!< 방패 총알 방어 피격음
	HIT_SMALLOBJECT,		//!< 룷븿봨됹돶
	BULLET,					//!< 뢤뭙궻돶갋돘먛귡돶
	GRE_BOUND,				//!< 롨왮뭙 긫긂깛긤돶
	GRE_EXPLOSION,			//!< 롨왮뭙 뵚뵯돶
	FOOTSTEPS_WALK,			//!< 뫉돶갋뺖궖
	FOOTSTEPS_FORWARD,		//!< 뫉돶갋몪귡돶걁멟릋걂
	FOOTSTEPS_BACK,			//!< 뫉돶갋몪귡돶걁뚣뫿걂
	FOOTSTEPS_SIDE,			//!< 뫉돶갋몪귡돶걁뜺덭걂
	FOOTSTEPS_JUMP,			//!< 뫉돶갋긙긿깛긵
	FOOTSTEPS_LANDING,		//!< 뫉돶갋뭶뭤돶
	FOOTSTEPS_RUN,			//!< 전력질주 발소리
	FOOTSTEPS_CROUCH,		//!< 앉은 이동 발소리
	DRY_FIRE,				//빈 탄창 발사 소리
	DRY_FIRE_PLAYER,

	ZOMBIE_ATTACK,
	ZOMBIE_ATTACK_PLAYER,

	WEAPON_MODE_CHANGE,
	WEAPON_MODE_CHANGE_PLAYER,

	WEAPON_RELOAD,			//!< 깏깓?긤돶
	WEAPON_RELOAD_PLAYER	//!< 플레이어 자신의 재장전 소리
};

//! 긖긂깛긤깏긚긣뾭?몾뫬
typedef struct
{
	int paramid;	//!< 돶뙶궻롰쀞붥뜂갋SoundmgrID믦릶
	int dataid;		//!< 긢??붥뜂
	float x;		//!< X띆뷭
	float y;		//!< Y띆뷭
	float z;		//!< Z띆뷭
	float move_x;	//!< 1긲깒??뚣궻 X띆뷭댷벍쀊
	float move_y;	//!< 1긲깒??뚣궻 Y띆뷭댷벍쀊
	float move_z;	//!< 1긲깒??뚣궻 Z띆뷭댷벍쀊
	int teamid;		//!< ???붥뜂
	int cnt;		//!< 긇긂깛긣뭠
} soundlist;

//! @brief 긖긂깛긤듖뿚긏깋긚
//! @details 둫긖긂깛긤궻룊딖돸갋똶럁갋띋맯궶궵귩뛱궋듖뿚궢귏궥갃
class SoundManager
{
	class SoundControl *SoundCtrl;		//!< 긖긂깛긤띋맯긏깋긚
	class ResourceManager *Resource;	//!< 깏??긚듖뿚긏깋긚
	class ParameterInfo *Param;			//!< 먠믦뭠귩듖뿚궥귡긏깋긚
	soundlist *soundlistA;		//!< 긖긂깛긤깏긚긣 A
	soundlist *soundlistB;		//!< 긖긂깛긤깏긚긣 B
	bool changeAB;				//!< 긖긂깛긤깏긚긣궻뵿?
	int listAdatas;				//!< 긖긂깛긤깏긚긣 A 궻뱋?릶
	int listBdatas;				//!< 긖긂깛긤깏긚긣 B 궻뱋?릶

	int zombie_attack_sound_toggle;

	// 방패 방어음은 연사/산탄/미니건 상황에서 너무 자주 발생할 수 있다.
	// 너무 많은 우선 재생 호출이 다른 효과음을 밀어내지 않도록 짧은 재생 제한을 둔다.
	int shield_block_sound_cooldown;

	bool GetNewList(soundlist** plist);
	bool GetNewListPreferDropShield(soundlist** plist);
	int GetTargetList(soundlist** plist);
	bool CheckApproach(soundlist *plist, float camera_x, float camera_y, float camera_z, float *min_x, float *min_y, float *min_z);
	void PlaySound(soundlist *plist, float camera_x, float camera_y, float camera_z, int teamID);

public:
	SoundManager(SoundControl *in_SoundCtrl = NULL, ResourceManager *in_Resource = NULL, ParameterInfo *in_Param = NULL);
	~SoundManager();
	void SetClass(SoundControl *in_SoundCtrl, ResourceManager *in_Resource, ParameterInfo *in_Param);
	void InitWorldSound();
	bool ShotWeapon(float x, float y, float z, int id, int teamID, bool player);
	bool HitMap(float x, float y, float z, int teamID);
	bool HitHuman(float x, float y, float z, int Hit_id, int teamID);
	bool HitShieldBlock(float x, float y, float z, int teamID);
	bool HitSmallObject(float x, float y, float z, int id, int teamID);
	bool PassingBullet(float x, float y, float z, float move_x, float move_y, float move_z, int teamID);
	bool GrenadeBound(float x, float y, float z, int teamID);
	bool GrenadeExplosion(float x, float y, float z, int teamID);
	bool SetFootsteps(float x, float y, float z, int id, int MoveMode, int teamID, int cnt);
	bool ReloadWeapon(float x, float y, float z, int reloadSoundID, int teamID, bool player = false);
	bool DryFire(float x, float y, float z, int teamID, bool player = false);
	bool ZombieAttack(float x, float y, float z, int teamID, bool player = false);
	bool WeaponModeChange(float x, float y, float z, int modeSoundID, int teamID, bool player = false);
	int GetTotalSoundList();
	int GetWorldSound(float pos_x, float pos_y, float pos_z, int teamID, soundlist *psoundlist);
	void PlayWorldSound(float camera_x, float camera_y, float camera_z, float camera_rx, int teamID);

};

#endif