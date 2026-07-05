//! @file soundmanager.cpp
//! @brief SoundManager긏깋긚궻믦?

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

#include "soundmanager.h"

// 방패 방어음 재생 제한.
// 방패음은 총성을 밀어내면 안 되므로 우선 재생하지 않는다.
// 이 값은 방패음 이벤트가 사운드 큐를 과점하지 않게 하는 최소 제한이다.
// 너무 자주 들리면 4~5, 너무 적게 들리면 2로 조정한다.
static const int SHIELD_BLOCK_SOUND_COOLDOWN_FRAMES = 3;

// 한 프레임에 실제 재생을 허용할 방패 피격음 수.
// 총소리 보호가 우선이므로 방패음은 많이 들어와도 일부만 들리게 한다.
static const int SHIELD_BLOCK_SOUND_MAX_PLAY_PER_FRAME = 1;

static bool IsHighPriorityWorldSound(int paramid)
{
	return
		(paramid == SHOT_WEAPON) ||
		(paramid == SHOT_WEAPON_PLAYER) ||
		(paramid == GRE_EXPLOSION) ||
		(paramid == HIT_HUMAN_HEAD) ||
		(paramid == HIT_HUMAN_UP) ||
		(paramid == HIT_HUMAN_LEG) ||
		(paramid == HIT_HUMAN_ZOMBIE) ||
		(paramid == ZOMBIE_ATTACK) ||
		(paramid == ZOMBIE_ATTACK_PLAYER);
}

static bool IsLowPriorityWorldSound(int paramid)
{
	return (paramid == HIT_SHIELD_BLOCK);
}

// 재장전음은 용도에 따라 기준 음량을 다르게 사용한다.
// 일반 재장전 > 반복되는 기계 동작 > 미니건 회전음 순으로 낮춘다.
static int GetReloadPlaybackVolume(int reloadSoundID, bool player)
{
	switch (reloadSoundID) {
	case RELOAD_MINIGUN_SPIN_START:
	case RELOAD_MINIGUN_SPIN:
	case RELOAD_MINIGUN_SPIN_END:
		return player ? 72 : 78;

	case RELOAD_RELOAD_START:
	case RELOAD_SHOTGUN_SHELL_INSERT:
	case RELOAD_LEVER_SHELL_INSERT:
	case RELOAD_BOLT_RELOAD_START:
	case RELOAD_SNIPER_SHELL_INSERT:
	case RELOAD_BOLT_RELOAD_END:
	case RELOAD_AWP_BOLT:
	case RELOAD_KAR98_SNIPER_BOLT:
	case RELOAD_SHOTGUN_ACTION:
	case RELOAD_WINCHESTER_LEVER:
		return player ? 76 : 82;

	default:
		return player ? 80 : 88;
	}
}

//! @brief 긓깛긚긣깋긏?
SoundManager::SoundManager(SoundControl* in_SoundCtrl, ResourceManager* in_Resource, ParameterInfo* in_Param)
{
	SoundCtrl = in_SoundCtrl;
	Resource = in_Resource;
	Param = in_Param;
	soundlistA = new soundlist[MAX_SOUNDMGR_LIST];
	soundlistB = new soundlist[MAX_SOUNDMGR_LIST];
	changeAB = false;
	listAdatas = 0;
	listBdatas = 0;
	zombie_attack_sound_toggle = 0;
	shield_block_sound_cooldown = 0;
}

//! @brief 긢귻긚긣깋긏?
SoundManager::~SoundManager()
{
	if( soundlistA != NULL ){ delete [] soundlistA; }
	if( soundlistB != NULL ){ delete [] soundlistB; }
}

//! @brief 럊뾭궥귡긏깋긚귩먠믦
//! @param in_SoundCtrl 긖긂깛긤긓깛긣깓?깑긏깋긚
//! @param in_Resource 깏??긚듖뿚긏깋긚
//! @param in_Param 먠믦뭠듖뿚긏깋긚
//! @attention 궞궻듫릶궳먠믦귩뛱귦궶궋궴갂긏깋긚렔뫬궕맫궢궘??궢귏궧귪갃
void SoundManager::SetClass(SoundControl *in_SoundCtrl, ResourceManager *in_Resource, ParameterInfo *in_Param)
{
	SoundCtrl = in_SoundCtrl;
	Resource = in_Resource;
	Param = in_Param;
}

//! @brief 뗴듩궻돶뙶귩룊딖돸
void SoundManager::InitWorldSound()
{
	changeAB = false;
	listAdatas = 0;
	listBdatas = 0;
	zombie_attack_sound_toggle = 0;
	shield_block_sound_cooldown = 0;
}

//! @brief 뗴듩궸뵯뺺돶귩믁돿
//! @param x 돶뙶궻X띆뷭
//! @param y 돶뙶궻Y띆뷭
//! @param z 돶뙶궻Z띆뷭
//! @param id 븧딇궻롰쀞붥뜂
//! @param teamID ???붥뜂
//! @param player 긵깒귽깂?궸귝귡뵯뺺궔궵궎궔걁true궳긵깒귽깂?걂
//! @return 맟뚻갌true?렪봲갌false
bool SoundManager::ShotWeapon(float x, float y, float z, int id, int teamID, bool player)
{
	soundlist* plist = NULL;

	// 총소리는 방패 방어음보다 우선한다.
	// 사운드 큐가 꽉 찬 경우, 이미 큐에 들어간 방패 방어음을 하나 밀어내고 총소리를 넣는다.
	if (GetNewListPreferDropShield(&plist) == false) { return false; }

	if (player == true) {
		plist->paramid = SHOT_WEAPON_PLAYER;
	}
	else{
		plist->paramid = SHOT_WEAPON;
	}
	plist->dataid = id;
	plist->x = x;
	plist->y = y;
	plist->z = z;
	plist->teamid = teamID;

	return true;
}

//! @brief 뗴듩궸?긞긵뭶뭙돶귩믁돿
//! @param x 돶뙶궻X띆뷭
//! @param y 돶뙶궻Y띆뷭
//! @param z 돶뙶궻Z띆뷭
//! @param teamID ???붥뜂
//! @return 맟뚻갌true?렪봲갌false
bool SoundManager::HitMap(float x, float y, float z, int teamID)
{
	soundlist *plist = NULL;
	if( GetNewList(&plist) == false ){ return false; }

	plist->paramid = HIT_MAP;
	plist->x = x;
	plist->y = y;
	plist->z = z;
	plist->teamid = teamID;

	return true;
}

//! @brief 뗴듩궸뷀뭙돶귩믁돿
//! @param x 돶뙶궻X띆뷭
//! @param y 돶뙶궻Y띆뷭
//! @param z 돶뙶궻Z띆뷭
//! @param Hit_id 뷀뭙됒룋걁벆갌0?벜뫬갌1?뫉갌2??깛긮뛘똼갌3걂
//! @param teamID ???붥뜂
//! @return 맟뚻갌true?렪봲갌false
bool SoundManager::HitHuman(float x, float y, float z, int Hit_id, int teamID)
{
	soundlist* plist = NULL;
	if (GetNewList(&plist) == false) { return false; }

	if ((Hit_id < 0) || (3 < Hit_id)) { return false; }

	if (Hit_id == 0) { plist->paramid = HIT_HUMAN_HEAD; }
	if (Hit_id == 1) { plist->paramid = HIT_HUMAN_UP; }
	if (Hit_id == 2) { plist->paramid = HIT_HUMAN_LEG; }
	if (Hit_id == 3) { plist->paramid = HIT_HUMAN_ZOMBIE; }
	plist->x = x;
	plist->y = y;
	plist->z = z;
	plist->teamid = teamID;

	return true;
}

//! @brief 방패로 총알을 막았을 때의 피격음 추가
bool SoundManager::HitShieldBlock(float x, float y, float z, int teamID)
{
	// 방패 방어음은 연사/산탄/미니건 상황에서 한 프레임에 여러 번 들어올 수 있다.
	// 총소리가 씹히는 것보다 방패음이 씹히는 쪽이 낫기 때문에,
	// 방패음은 절대 PlaySoundPriority()로 강제 재생하지 않는다.
	if (shield_block_sound_cooldown > 0) {
		return true;
	}

	soundlist* plist = NULL;

	if (GetNewList(&plist) == false) {
		// 사운드 큐가 꽉 차면 방패음은 포기한다.
		// 총소리, 피격음, 폭발음 같은 핵심 전투음을 보호하기 위한 처리다.
		return false;
	}

	shield_block_sound_cooldown = SHIELD_BLOCK_SOUND_COOLDOWN_FRAMES;

	plist->paramid = HIT_SHIELD_BLOCK;
	plist->x = x;
	plist->y = y;
	plist->z = z;
	plist->teamid = teamID;

	return true;
}

//! @brief 뗴듩궸룷븿뭶뭙돶귩믁돿
//! @param x 돶뙶궻X띆뷭
//! @param y 돶뙶궻Y띆뷭
//! @param z 돶뙶궻Z띆뷭
//! @param id 룷븿궻롰쀞붥뜂
//! @param teamID ???붥뜂
//! @return 맟뚻갌true?렪봲갌false
bool SoundManager::HitSmallObject(float x, float y, float z, int id, int teamID)
{
	soundlist *plist = NULL;
	if( GetNewList(&plist) == false ){ return false; }

	plist->paramid = HIT_SMALLOBJECT;
	plist->dataid = id;
	plist->x = x;
	plist->y = y;
	plist->z = z;
	plist->teamid = teamID;

	return true;
}

//! @brief 뭙궻믅됡갋돘먛귡돶귩믁돿
//! @param x 돶뙶궻X띆뷭
//! @param y 돶뙶궻Y띆뷭
//! @param z 돶뙶궻Z띆뷭
//! @param move_x 돶뙶궻X렡댷벍쀊
//! @param move_y 돶뙶궻Y렡댷벍쀊
//! @param move_z 돶뙶궻Z렡댷벍쀊
//! @param teamID ???붥뜂
//! @return 맟뚻갌true?렪봲갌false
//! @attention move_x갋move_y갋move_z궻댷벍쀊궼갂1긲깒??빁귩럚믦궢궲궘궬궠궋갃렳띧궸띆뷭궕댷벍궥귡귦궚궳궼궇귟귏궧귪갃
bool SoundManager::PassingBullet(float x, float y, float z, float move_x, float move_y, float move_z, int teamID)
{
	soundlist *plist = NULL;
	if( GetNewList(&plist) == false ){ return false; }

	plist->paramid = BULLET;
	plist->x = x;
	plist->y = y;
	plist->z = z;
	plist->move_x = move_x;
	plist->move_y = move_y;
	plist->move_z = move_z;
	plist->teamid = teamID;

	return true;
}

//! @brief 뗴듩궸롨왮뭙긫긂깛긤갋뮫궺뺅귟돶귩믁돿
//! @param x 돶뙶궻X띆뷭
//! @param y 돶뙶궻Y띆뷭
//! @param z 돶뙶궻Z띆뷭
//! @param teamID ???붥뜂
//! @return 맟뚻갌true?렪봲갌false
bool SoundManager::GrenadeBound(float x, float y, float z, int teamID)
{
	soundlist *plist = NULL;
	if( GetNewList(&plist) == false ){ return false; }

	plist->paramid = GRE_BOUND;
	plist->x = x;
	plist->y = y;
	plist->z = z;
	plist->teamid = teamID;

	return true;
}

//! @brief 뗴듩궸롨왮뭙뵚뵯돶귩믁돿
//! @param x 돶뙶궻X띆뷭
//! @param y 돶뙶궻Y띆뷭
//! @param z 돶뙶궻Z띆뷭
//! @param teamID ???붥뜂
//! @return 맟뚻갌true?렪봲갌false
bool SoundManager::GrenadeExplosion(float x, float y, float z, int teamID)
{
	soundlist *plist = NULL;
	if( GetNewList(&plist) == false ){ return false; }

	plist->paramid = GRE_EXPLOSION;
	plist->x = x;
	plist->y = y;
	plist->z = z;
	plist->teamid = teamID;

	return true;
}

//! @brief 뗴듩궸뫉돶귩믁돿
//! @param x 돶뙶궻X띆뷭
//! @param y 돶뙶궻Y띆뷭
//! @param z 돶뙶궻Z띆뷭
//! @param id 뫉돶궻롰쀞붥뜂
//! @param MoveMode 댷벍긾?긤걁뺖궖갌0?멟릋갌1?뚣뫿갌2?뜺덭갌3?긙긿깛긵갌4?뭶뭤갌5걂
//! @param teamID ???붥뜂
//! @param cnt 긇긂깛긣뭠
//! @return 맟뚻갌true?렪봲갌false
bool SoundManager::SetFootsteps(float x, float y, float z, int id, int MoveMode, int teamID, int cnt)
{
	soundlist *plist = NULL;
	if( GetNewList(&plist) == false ){ return false; }

	switch (MoveMode) {
	case 0: plist->paramid = FOOTSTEPS_WALK; break;
	case 1: plist->paramid = FOOTSTEPS_FORWARD; break;
	case 2: plist->paramid = FOOTSTEPS_BACK; break;
	case 3: plist->paramid = FOOTSTEPS_SIDE; break;
	case 4: plist->paramid = FOOTSTEPS_JUMP; break;
	case 5: plist->paramid = FOOTSTEPS_LANDING; break;
	case 6: plist->paramid = FOOTSTEPS_RUN; break;
	case 7: plist->paramid = FOOTSTEPS_CROUCH; break;
	default: return false;
	}
	plist->dataid = id;
	plist->x = x;
	plist->y = y;
	plist->z = z;
	plist->teamid = teamID;
	plist->cnt = cnt;

	return true;
}

//! @brief 뗴듩궸븧딇깏깓?긤돶귩믁돿
//! @param x 돶뙶궻X띆뷭
//! @param y 돶뙶궻Y띆뷭
//! @param z 돶뙶궻Z띆뷭
//! @param teamID ???붥뜂
//! @return 맟뚻갌true?렪봲갌false
bool SoundManager::ReloadWeapon(float x, float y, float z, int reloadSoundID, int teamID, bool player)
{
	soundlist* plist = NULL;
	if (GetNewList(&plist) == false) { return false; }

	if (player == true) {
		plist->paramid = WEAPON_RELOAD_PLAYER;
	}
	else {
		plist->paramid = WEAPON_RELOAD;
	}

	plist->dataid = reloadSoundID;
	plist->x = x;
	plist->y = y;
	plist->z = z;
	plist->teamid = teamID;

	return true;
}

bool SoundManager::DryFire(float x, float y, float z, int teamID, bool player)
{
	soundlist* plist = NULL;
	if (GetNewList(&plist) == false) { return false; }

	if (player == true) {
		plist->paramid = DRY_FIRE_PLAYER;
	}
	else {
		plist->paramid = DRY_FIRE;
	}

	plist->dataid = RELOAD_DRY_FIRE;
	plist->x = x;
	plist->y = y;
	plist->z = z;
	plist->teamid = teamID;

	return true;
}

bool SoundManager::ZombieAttack(float x, float y, float z, int teamID, bool player)
{
	soundlist* plist = NULL;
	if (GetNewList(&plist) == false) { return false; }

	if (player == true) {
		plist->paramid = ZOMBIE_ATTACK_PLAYER;
	}
	else {
		plist->paramid = ZOMBIE_ATTACK;
	}

	if (zombie_attack_sound_toggle == 0) {
		plist->dataid = ZOMBIE_ATTACK_1;
		zombie_attack_sound_toggle = 1;
	}
	else {
		plist->dataid = ZOMBIE_ATTACK_2;
		zombie_attack_sound_toggle = 0;
	}

	plist->x = x;
	plist->y = y;
	plist->z = z;
	plist->teamid = teamID;

	return true;
}

bool SoundManager::WeaponModeChange(float x, float y, float z, int modeSoundID, int teamID, bool player)
{
	soundlist* plist = NULL;
	if (GetNewList(&plist) == false) { return false; }

	if (player == true) {
		plist->paramid = WEAPON_MODE_CHANGE_PLAYER;
	}
	else {
		plist->paramid = WEAPON_MODE_CHANGE;
	}

	plist->dataid = modeSoundID;
	plist->x = x;
	plist->y = y;
	plist->z = z;
	plist->teamid = teamID;

	return true;
}

//! @brief 뗴듩걁긖긂깛긤깏긚긣걂뤵궳뾎뚼궶돶뙶릶귩롦벦
//! @return 뾎뚼궶돶뙶릶
int SoundManager::GetTotalSoundList()
{
	if( changeAB == false ){
		return listBdatas;
	}
	//else{
		return listAdatas;
	//}
}

//! @brief 럚믦궢궫댧뭫궻뢂뺃궸궇귡돶뙶귩롦벦
//! @param pos_x 돶뙶궻X띆뷭
//! @param pos_y 돶뙶궻Y띆뷭
//! @param pos_z 돶뙶궻Z띆뷭
//! @param teamID ???붥뜂
//! @param psoundlist 긖긂깛긤깏긚긣귩롷궚롦귡?귽깛?
//! @return 뺅궥걁긖긂깛긤깏긚긣궻걂돶뙶궻릶
int SoundManager::GetWorldSound(float pos_x, float pos_y, float pos_z, int teamID, soundlist *psoundlist)
{
	if( psoundlist == NULL ){ return 0; }

	int lists;
	soundlist *getlist = NULL;
	int newlists = 0;
	WeaponParameter WParam;

	//룉뿚궥귡깏긚긣귩뙂믦
	lists = GetTargetList(&getlist);

	for(int i=0; i<lists; i++){
		float x, y, z;
		float maxdist = 20.0f;	//뵽믦떁뿣궻뷭?뭠

		//돶뙶궻롰쀞궸귝귟뵽믦떁뿣귩빾궑귡
		if( (getlist[i].paramid == SHOT_WEAPON)||(getlist[i].paramid == SHOT_WEAPON_PLAYER) ){
			Param->GetWeapon(getlist[i].dataid, &WParam);

			if( getlist[i].teamid == teamID ){ maxdist = 20.0f; }
			else if( WParam.silencer == true ){ maxdist = 30.0f; }
			else{ maxdist = 120.0f; }
		}
		if( getlist[i].paramid == HIT_MAP ){
			maxdist = 30.0f;
		}
		if( getlist[i].paramid == HIT_HUMAN_HEAD ){
			maxdist = 60.0f;
		}
		if( getlist[i].paramid == HIT_HUMAN_UP ){
			maxdist = 50.0f;
		}
		if( getlist[i].paramid == HIT_HUMAN_LEG ){
			maxdist = 40.0f;
		}
		if (getlist[i].paramid == HIT_HUMAN_ZOMBIE) {
			maxdist = 40.0f;
		}
		if (getlist[i].paramid == HIT_SHIELD_BLOCK) {
			maxdist = 45.0f;
		}
		if ((getlist[i].paramid == ZOMBIE_ATTACK) ||
			(getlist[i].paramid == ZOMBIE_ATTACK_PLAYER)) {
			maxdist = 25.0f;
		}

		if( getlist[i].paramid == FOOTSTEPS_WALK ){
			continue;
		}

		if( getlist[i].paramid == FOOTSTEPS_CROUCH ){
			if( getlist[i].teamid == teamID ){ continue; }
			else{ maxdist = 12.0f; }
		}

		if( getlist[i].paramid == FOOTSTEPS_FORWARD ){
			if( getlist[i].teamid == teamID ){ continue; }
			else{ maxdist = 40.0f; }
		}
		if( getlist[i].paramid == FOOTSTEPS_BACK ){
			if( getlist[i].teamid == teamID ){ continue; }
			else{ maxdist = 30.0f; }
		}
		if( getlist[i].paramid == FOOTSTEPS_SIDE ){
			if( getlist[i].teamid == teamID ){ continue; }
			else{ maxdist = 35.0f; }
		}

		if( getlist[i].paramid == FOOTSTEPS_RUN ){
			if( getlist[i].teamid == teamID ){ continue; }
			else{ maxdist = 65.0f; }
		}

		if (getlist[i].paramid == FOOTSTEPS_JUMP) {
			if (getlist[i].teamid == teamID) { continue; }
			else { maxdist = 45.0f; }
		}

		if (getlist[i].paramid == FOOTSTEPS_LANDING) {
			if (getlist[i].teamid == teamID) { continue; }
			else {
				// cnt == 1이면 달리면서/강한 착지로 들어온 경우로 사용한다.
				if (getlist[i].cnt == 1) {
					maxdist = 55.0f;
				}
				else {
					maxdist = 50.0f;
				}
			}
		}
		if( getlist[i].paramid == GRE_BOUND ){
			continue;
		}
		if( getlist[i].paramid == GRE_EXPLOSION ){
			maxdist = 80.0f;
		}
		if (getlist[i].paramid == WEAPON_RELOAD || getlist[i].paramid == WEAPON_RELOAD_PLAYER) {
			if (getlist[i].teamid == teamID) {
				continue;
			}
			else {
				maxdist = 40.0f;

				// 미니건 예열음은 일반 재장전보다 더 눈에 띄게 한다.
				// 플레이어가 예열 중이면 주변 AI가 경계 상태로 들어가기 쉬워진다.
				if (getlist[i].dataid == RELOAD_MINIGUN_SPIN_START ||
					getlist[i].dataid == RELOAD_MINIGUN_SPIN ||
					getlist[i].dataid == RELOAD_MINIGUN_SPIN_END) {
					maxdist = 65.0f;
				}
			}
		}
		if (getlist[i].paramid == DRY_FIRE || getlist[i].paramid == DRY_FIRE_PLAYER) {
			if (getlist[i].teamid == teamID) {
				continue;
			}
			else {
				maxdist = 28.0f;
			}
		}

		if (getlist[i].paramid == WEAPON_MODE_CHANGE || getlist[i].paramid == WEAPON_MODE_CHANGE_PLAYER) {
			if (getlist[i].teamid == teamID) {
				continue;
			}
			else {
				maxdist = 35.0f;
			}
		}

#ifdef ENABLE_BUG_TEAMID
		//???붥뜂궕븠릶갂궔궰돶뙶궻???붥뜂궕묈궖궋궶귞갂돶뙶귩뼰럨
		if( (teamID < 0)&&(getlist[i].teamid < 0) ){
			if( teamID < getlist[i].teamid ){
				continue;
			}
		}
#endif

		//뢤뭙궶귞궽
		if( getlist[i].paramid == BULLET ){
			float min_x, min_y, min_z;

			//뢤뭙궕띍귖뗟궱궘띆뷭귩럁뢯
			if( CheckApproach(&(getlist[i]), pos_x, pos_y, pos_z, &min_x, &min_y, &min_z) == false ){ continue; }

			//돶뙶궴궻떁뿣궕붝댪볙궶귞궽?
			x = min_x - pos_x;
			y = min_y - pos_y;
			z = min_z - pos_z;
			if( x*x + y*y + z*z < maxdist*maxdist ){
				//뢯쀍먩궻깏긚긣궸믁돿
				psoundlist[newlists].paramid = getlist[i].paramid;
				psoundlist[newlists].dataid = getlist[i].dataid;
				psoundlist[newlists].x = min_x;
				psoundlist[newlists].y = min_y;
				psoundlist[newlists].z = min_z;
				newlists += 1;
			}
		}
		else{
			//돶뙶궴궻떁뿣궕붝댪볙궶귞궽?
			x = getlist[i].x - pos_x;
			y = getlist[i].y - pos_y;
			z = getlist[i].z - pos_z;
			if( x*x + y*y + z*z < maxdist*maxdist ){
				//뢯쀍먩궻깏긚긣궸믁돿
				psoundlist[newlists].paramid = getlist[i].paramid;
				psoundlist[newlists].dataid = getlist[i].dataid;
				psoundlist[newlists].x = getlist[i].x;
				psoundlist[newlists].y = getlist[i].y;
				psoundlist[newlists].z = getlist[i].z;
				newlists += 1;
			}
		}
	}

	return newlists;
}

//! @brief 뗴듩뤵궻긖긂깛긤귩띋맯
//! @param camera_x 긇긽깋궻X띆뷭
//! @param camera_y 긇긽깋궻Y띆뷭
//! @param camera_z 긇긽깋궻Z띆뷭
//! @param camera_rx 긇긽깋궻X렡둷뱗?걁?뽵걂
//! @param teamID ???붥뜂
//! @warning 뻽긲깒??뚁귂뢯궢궲궘궬궠궋갃
void SoundManager::PlayWorldSound(float camera_x, float camera_y, float camera_z, float camera_rx, int teamID)
{
	int lists;
	soundlist* getlist = NULL;

	// 방패 방어음 재생 제한 감소.
	// PlayWorldSound()는 월드 사운드 처리 루프에서 프레임마다 호출되므로 여기서 줄인다.
	if (shield_block_sound_cooldown > 0) {
		shield_block_sound_cooldown--;
	}

	//긲깋긐귩먛귟뫶궑귡
	if( changeAB == false ){
		listBdatas = 0;
		changeAB = true;
	}
	else{
		listAdatas = 0;
		changeAB = false;
	}

	//긇긽깋띆뷭귩먠믦
	SoundCtrl->SetCamera(camera_x, camera_y, camera_z, camera_rx);

	// 처리할 사운드 리스트를 얻는다.
	lists = GetTargetList(&getlist);

	// 1차: 총소리/폭발음/피격음 같은 핵심 전투음을 먼저 재생한다.
	// 방패음이 총성을 밀어내는 상황을 막기 위한 우선순위 처리다.
	for (int i = 0; i < lists; i++) {
		if (IsHighPriorityWorldSound(getlist[i].paramid) == false) { continue; }

		float x = getlist[i].x - camera_x;
		float y = getlist[i].y - camera_y;
		float z = getlist[i].z - camera_z;

		if (x * x + y * y + z * z < MAX_SOUNDDIST * MAX_SOUNDDIST) {
			PlaySound(&(getlist[i]), camera_x, camera_y, camera_z, teamID);
		}
	}

	// 2차: 일반 사운드를 재생한다.
	// 방패 피격음은 여기서도 제외하고 마지막에 처리한다.
	for (int i = 0; i < lists; i++) {
		if (IsHighPriorityWorldSound(getlist[i].paramid) == true) { continue; }
		if (IsLowPriorityWorldSound(getlist[i].paramid) == true) { continue; }

		float x = getlist[i].x - camera_x;
		float y = getlist[i].y - camera_y;
		float z = getlist[i].z - camera_z;

		if (x * x + y * y + z * z < MAX_SOUNDDIST * MAX_SOUNDDIST) {
			PlaySound(&(getlist[i]), camera_x, camera_y, camera_z, teamID);
		}
	}

	// 3차: 방패 피격음은 가장 마지막에 재생한다.
	// 이 단계에서 방패음이 씹히는 것은 허용하지만, 총소리가 씹히면 안 된다.
	int shield_block_play_count = 0;

	for (int i = 0; i < lists; i++) {
		if (getlist[i].paramid != HIT_SHIELD_BLOCK) { continue; }

		if (shield_block_play_count >= SHIELD_BLOCK_SOUND_MAX_PLAY_PER_FRAME) {
			continue;
		}

		float x = getlist[i].x - camera_x;
		float y = getlist[i].y - camera_y;
		float z = getlist[i].z - camera_z;

		if (x * x + y * y + z * z < MAX_SOUNDDIST * MAX_SOUNDDIST) {
			PlaySound(&(getlist[i]), camera_x, camera_y, camera_z, teamID);
			shield_block_play_count++;
		}
	}
}

//! @brief 륷궢궋긖긂깛긤깏긚긣걁뭁덇걂귩롦벦
//! @param plist 륷궢궋긖긂깛긤깏긚긣걁뭁덇걂궻볫뢣?귽깛?
//! @return 맟뚻갌true?렪봲갌false
bool SoundManager::GetNewList(soundlist** plist)
{
	if (changeAB == false) {
		if ((listAdatas + 1) >= MAX_SOUNDMGR_LIST) {
			return false;
		}
		*plist = &(soundlistA[listAdatas]);
		listAdatas += 1;
	}
	else {
		if ((listBdatas + 1) >= MAX_SOUNDMGR_LIST) {
			return false;
		}
		*plist = &(soundlistB[listBdatas]);
		listBdatas += 1;
	}

	return true;
}

//! @brief 새 사운드 리스트를 얻되, 큐가 꽉 찬 경우 방패 방어음을 낮은 우선순위로 보고 교체한다.
bool SoundManager::GetNewListPreferDropShield(soundlist** plist)
{
	if (GetNewList(plist) == true) {
		return true;
	}

	soundlist* target_list = NULL;
	int target_count = 0;

	if (changeAB == false) {
		target_list = soundlistA;
		target_count = listAdatas;
	}
	else {
		target_list = soundlistB;
		target_count = listBdatas;
	}

	// 가장 최근에 들어간 방패음을 먼저 버린다.
	// 총소리는 전투 피드백의 핵심이므로 방패음보다 우선한다.
	for (int i = target_count - 1; i >= 0; i--) {
		if (target_list[i].paramid == HIT_SHIELD_BLOCK) {
			*plist = &(target_list[i]);
			return true;
		}
	}

	return false;
}

//! @brief 룉뿚뫮뤭궻긖긂깛긤깏긚긣귩롦벦
//! @param plist 룉뿚뫮뤭궻긖긂깛긤깏긚긣궻볫뢣?귽깛?
//! @return 롦벦궢궫긖긂깛긤깏긚긣궸듵귏귢귡돶뙶궻릶
int SoundManager::GetTargetList(soundlist **plist)
{
	if( changeAB == false ){
		*plist = soundlistB;
		return listBdatas;
	}
	//else{
		*plist = soundlistA;
		return listAdatas;
	//}
}

//! @brief 돶뙶궕긇긽깋걁럨?걂궸띍귖뗟궱궘궔둴봃
//! @param plist 룉뿚뫮뤭궻긖긂깛긤깏긚긣걁뭁덇걂궻?귽깛?
//! @param camera_x 긇긽깋궻X띆뷭
//! @param camera_y 긇긽깋궻Y띆뷭
//! @param camera_z 긇긽깋궻Z띆뷭
//! @param min_x 띍뭒떁뿣궻X띆뷭귩롷궚롦귡?귽깛?
//! @param min_y 띍뭒떁뿣궻Y띆뷭귩롷궚롦귡?귽깛?
//! @param min_z 띍뭒떁뿣궻Z띆뷭귩롷궚롦귡?귽깛?
//! @return 믅귟됡궗궫갌true?믅귟됡궗궲궋궶궋갌false
bool SoundManager::CheckApproach(soundlist *plist, float camera_x, float camera_y, float camera_z, float *min_x, float *min_y, float *min_z)
{
	float x, y, z;
	float dist1, dist2, dist3;

	//1긲깒??멟궻떁뿣
	x = camera_x - (plist->x - plist->move_x);
	y = camera_y - (plist->y - plist->move_y);
	z = camera_z - (plist->z - plist->move_z);
	dist1 = x*x + y*y + z*z;

	//뙸띪댧뭫궻떁뿣
	x = camera_x - plist->x;
	y = camera_y - plist->y;
	z = camera_z - plist->z;
	dist2 = x*x + y*y + z*z;

	//1긲깒??뚣궻떁뿣
	x = camera_x - (plist->x + plist->move_x);
	y = camera_y - (plist->y + plist->move_y);
	z = camera_z - (plist->z + plist->move_z);
	dist3 = x*x + y*y + z*z;

	//뙸띪댧뭫궻떁뿣궕띍귖뗟궚귢궽?
	if( (dist1 > dist2)&&(dist2 < dist3) ){
		float speed;
		float min_dist, dist;

		//댷벍뫊뱗귩땫귕귡
		speed = sqrtf(plist->move_x*plist->move_x + plist->move_y*plist->move_y + plist->move_z*plist->move_z);

		//띍뭒떁뿣궻띆뷭귩땫귕귡
		min_dist = DistancePosRay(camera_x, camera_y, camera_z, plist->x, plist->y, plist->z, plist->move_x/speed, plist->move_y/speed, plist->move_z/speed);

		//띍뭒떁뿣렄궻띆뷭귩땫귕귡
		dist = sqrtf(dist2 - min_dist*min_dist);
		*min_x = plist->x + plist->move_x/speed * dist;
		*min_y = plist->y + plist->move_y/speed * dist;
		*min_z = plist->z + plist->move_z/speed * dist;

		return true;
	}

	return false;
}

//! @brief 럚믦궢궫긖긂깛긤깏긚긣걁1돶뙶걂귩띋맯
//! @param plist 띋맯궥귡긖긂깛긤깏긚긣걁뭁덇걂궻?귽깛?
//! @param camera_x 긇긽깋궻X띆뷭
//! @param camera_y 긇긽깋궻Y띆뷭
//! @param camera_z 긇긽깋궻Z띆뷭
//! @param teamID ???붥뜂
void SoundManager::PlaySound(soundlist* plist, float camera_x, float camera_y, float camera_z, int teamID)
{
	WeaponParameter WParam;
	int hitsoundA, hitsoundB;
	int hitsound;
	int ccosound;
	int bangsound;

	int id = -1;
	int volume = 0;

	//띋맯륆뙊귩먠믦
	switch (plist->paramid) {
	case SHOT_WEAPON:		//뵯뺺돶
		Param->GetWeapon(plist->dataid, &WParam);
		if (WParam.soundvolume == 0) { return; }

		id = Resource->GetWeaponSound(plist->dataid);
		volume = WParam.soundvolume;
		break;

	case SHOT_WEAPON_PLAYER:	//긵깒귽깂?렔릆궻뵯뺺돶
		Param->GetWeapon(plist->dataid, &WParam);
		if (WParam.soundvolume == 0) { return; }

		// [✨ 마스터 볼륨 적용 ✨] 플레이어가 직접 쏘는 총소리에도 설정값이 적용되게 합니다.
		SoundCtrl->PlaySoundPriority(
			Resource->GetWeaponSound(plist->dataid),
			(int)(WParam.soundvolume * GameConfig.GetVolume()),
			0
		);
		return;

	case HIT_MAP:			//?긞긵뭶뭙돶
		Resource->GetBulletSound(&hitsoundA, &hitsoundB, NULL, NULL, NULL, NULL);
		if (GetRand(3) == 0) {
			id = hitsoundB;
		}
		else {
			id = hitsoundA;
		}
		volume = MAX_SOUNDHITMAP;
		break;

	case HIT_HUMAN_HEAD:			//뷀뭙돶
	case HIT_HUMAN_UP:
	case HIT_HUMAN_LEG:
	case HIT_HUMAN_ZOMBIE:
		Resource->GetBulletSound(NULL, NULL, &hitsound, NULL, NULL, NULL);
		id = hitsound;
		volume = MAX_SOUNDHITHUMAN;
		break;

	case HIT_SHIELD_BLOCK:
	{
		id = Resource->GetSkillSound(SKILL_SOUND_SHIELD_BLOCK);

		if (id == -1) {
			return;
		}

		// 방패 방어음은 총성보다 앞으로 튀어나오면 전투음이 묻힌다.
		// 일반 재생은 우선 재생이 아니라 3D 사운드로 처리해서 위치감과 거리감을 유지한다.
		// 짧은 cooldown은 HitShieldBlock()에서 이미 처리하므로 여기서는 볼륨/거리감만 조절한다.
		int play_volume = (int)(MAX_SOUNDSHIELDBLOCK * GameConfig.GetVolume());

		if (play_volume <= 0) {
			return;
		}

		// minDist를 낮게 잡아 가까이에서도 과도하게 크게 들리지 않게 하고,
		// maxDist도 짧게 잡아 총소리보다 멀리 퍼지지 않게 한다.
		SoundCtrl->Play3DSoundEx(id, plist->x, plist->y, plist->z, play_volume, 5.0f, 55.0f);
		return;
	}

	case HIT_SMALLOBJECT:	//룷븿봨됹돶
		id = Resource->GetSmallObjectSound(plist->dataid);
		volume = MAX_SOUNDHITSMALLOBJ;
		break;

	case BULLET:			//뢤뭙궻돶갋돘먛귡돶
		float new_x, new_y, new_z;
		int passingsound;

		if (CheckApproach(plist, camera_x, camera_y, camera_z, &new_x, &new_y, &new_z) == false) { return; }

		//뼞뺴궻뭙궶귞돺귖궧궦뢎뿹
		if (plist->teamid == teamID) { return; }

		// [✨ 마스터 볼륨 적용 ✨] 내 옆을 빗겨 날아가는 총알 소리에도 적용합니다.
		Resource->GetBulletSound(NULL, NULL, NULL, &passingsound, NULL, NULL);
		SoundCtrl->Play3DSound(passingsound, new_x, new_y, new_z, (int)(MAX_SOUNDPASSING * GameConfig.GetVolume()));
		return;

	case GRE_BOUND:			//롨왮뭙 긫긂깛긤돶
		Resource->GetBulletSound(NULL, NULL, NULL, NULL, NULL, &ccosound);
		id = ccosound;
		volume = MAX_SOUNDCCOGRENADE;
		break;

	case GRE_EXPLOSION:		//롨왮뭙 뵚뵯돶
		Resource->GetBulletSound(NULL, NULL, NULL, NULL, &bangsound, NULL);
		id = bangsound;
		volume = MAX_SOUNDHITGRENADE;
		break;

	case FOOTSTEPS_WALK:
	case FOOTSTEPS_FORWARD:
	case FOOTSTEPS_BACK:
	case FOOTSTEPS_SIDE:
	case FOOTSTEPS_RUN:
	case FOOTSTEPS_CROUCH:
	{
		if (plist->dataid == -1) {
			return;
		}

		int interval;
		if (plist->paramid == FOOTSTEPS_WALK) interval = 22;
		else if (plist->paramid == FOOTSTEPS_CROUCH) interval = 24;
		else if (plist->paramid == FOOTSTEPS_RUN) interval = 9;
		else interval = 14;

		if (plist->cnt % interval != 0) {
			return;
		}

		int random_idx = GetRand(10);
		int footstep_sound = Resource->GetFootstepSound(random_idx);
		if (footstep_sound == -1) return;
		id = footstep_sound;

		if (plist->paramid == FOOTSTEPS_WALK) {
			volume = 30;
		}
		else if (plist->paramid == FOOTSTEPS_CROUCH) {
			volume = 15;
		}
		else if (plist->paramid == FOOTSTEPS_RUN) {
			volume = 150;
		}
		else {
			volume = 90;
		}
		break;
	}

	case FOOTSTEPS_JUMP:
	case FOOTSTEPS_LANDING:
	{
		int random_idx = GetRand(10);
		int footstep_sound = Resource->GetFootstepSound(random_idx);
		if (footstep_sound == -1) return;
		id = footstep_sound;

		if (plist->paramid == FOOTSTEPS_LANDING) {
			if (plist->cnt == 1) {
				volume = 120;
			}
			else {
				volume = 150; // 일반 착지
			}
		}
		else {
			volume = 70; // 점프
		}
		break;
	}

	case WEAPON_RELOAD:
		id = Resource->GetReloadSound(plist->dataid);
		if (id == -1) { return; }
		volume = GetReloadPlaybackVolume(plist->dataid, false);
		break;

	case WEAPON_RELOAD_PLAYER:
		id = Resource->GetReloadSound(plist->dataid);
		if (id == -1) { return; }

		// 플레이어 자신의 재장전 소리는 2D로 재생한다.
		// 반복 기계음과 미니건 회전음은 일반 재장전보다 더 작게 재생한다.
		volume = GetReloadPlaybackVolume(plist->dataid, true);
		SoundCtrl->PlaySound(
			id,
			(int)(volume * GameConfig.GetVolume()),
			0
		);
		return;

	case DRY_FIRE:
		id = Resource->GetReloadSound(RELOAD_DRY_FIRE);
		if (id == -1) { return; }
		volume = 35;
		break;

	case DRY_FIRE_PLAYER:
		id = Resource->GetReloadSound(RELOAD_DRY_FIRE);
		if (id == -1) { return; }

		// 플레이어 자신의 빈 탄창 소리는 2D로 재생
		SoundCtrl->PlaySound(id, (int)(70 * GameConfig.GetVolume()), 0);
		return;

	case ZOMBIE_ATTACK:
		id = Resource->GetReloadSound(plist->dataid);
		if (id == -1) { return; }
		volume = 65;
		break;

	case ZOMBIE_ATTACK_PLAYER:
		id = Resource->GetReloadSound(plist->dataid);
		if (id == -1) { return; }

		// 플레이어 자신이 좀비일 때는 2D로 명확하게 들리게 함
		SoundCtrl->PlaySound(id, (int)(75 * GameConfig.GetVolume()), 0);
		return;

	case WEAPON_MODE_CHANGE:
		id = Resource->GetReloadSound(plist->dataid);
		if (id == -1) { return; }
		volume = 55;
		break;

	case WEAPON_MODE_CHANGE_PLAYER:
		id = Resource->GetReloadSound(plist->dataid);
		if (id == -1) { return; }

		// 플레이어 자신의 총 모드 변경음은 2D로 재생
		SoundCtrl->PlaySound(id, (int)(70 * GameConfig.GetVolume()), 0);
		return;

	default:
		return;
	}


	// [✨ 마스터 볼륨 일괄 적용 ✨] 
	if (plist->paramid == WEAPON_RELOAD) {
		float dx = plist->x - camera_x;
		float dy = plist->y - camera_y;
		float dz = plist->z - camera_z;
		float dist = sqrtf(dx * dx + dy * dy + dz * dz);

		// 재장전음이 들리는 최대 거리.
		// DirectSound의 maxDist는 완전한 재생 차단 거리가 아니므로 여기서 직접 제거한다.
		const float reloadMaxDist = 120.0f;

		if (dist >= reloadMaxDist) {
			return;
		}

		float t = dist / reloadMaxDist;
		if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;

		// 가까울수록 1.0, 멀수록 0.0에 가까워짐
		// 1.6~2.0 정도가 자연스럽게 멀어질수록 작아지는 편
		float rate = 1.0f - t;

		// 1.6f는 너무 강하게 줄어든다.
		// 0.45f ~ 0.70f 정도가 재장전음에는 더 적당하다.
		rate = powf(rate, 0.55f);

		// 예전처럼 50%까지 보장하면 너무 멀리서도 크게 들릴 수 있으므로
		// 최소 보정은 20% 정도만 둔다.
		const float minRate = 0.25f;
		rate = minRate + (1.0f - minRate) * rate;

		volume = (int)(volume * rate);

		// 너무 먼 끝부분에서만 제거
		if (volume <= 2) {
			return;
		}
	}

	// 마스터 볼륨 적용
	volume = (int)(volume * GameConfig.GetVolume());

	if (volume <= 0) {
		return;
	}

	//띋맯
	if (plist->paramid == WEAPON_RELOAD) {
		SoundCtrl->Play3DSoundEx(id, plist->x, plist->y, plist->z, volume, 200.0f, 335.0f);
	}
	else if (plist->paramid == DRY_FIRE) {
		SoundCtrl->Play3DSoundEx(id, plist->x, plist->y, plist->z, volume, 25.0f, 45.0f);
	}
	else if (plist->paramid == WEAPON_MODE_CHANGE) {
		SoundCtrl->Play3DSoundEx(id, plist->x, plist->y, plist->z, volume, 20.0f, 35.0f);
	}
	else {
		SoundCtrl->Play3DSound(id, plist->x, plist->y, plist->z, volume);
	}
}