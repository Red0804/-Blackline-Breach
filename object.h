//! @file object.h
//! @brief object긏깋긚궻먬뙻

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

#ifndef OBJECT_H
#define OBJECT_H

// 뭾댰갌뫊뱗빾뛛렄궼갂뱰궫귟뵽믦똭궻믦릶귖뾴둴봃
#define HUMAN_PROGRESSRUN_ACCELERATION	0.7f			//!< 릐궻멟릋걁몪귟걂돿뫊뱗
#define HUMAN_SIDEWAYSRUN_ACCELERATION 0.5f			//!< 릐궻돘몪귟돿뫊뱗
#define HUMAN_REGRESSRUN_ACCELERATION 0.38f			//!< 릐궻뚣뫿돿뫊뱗
//! 릐궕롈귕멟궸릋귔돿뫊뱗
#define HUMAN_PROGRESSRUN_SIDEWAYSRUN_ACCELERATION ((HUMAN_PROGRESSRUN_ACCELERATION + HUMAN_SIDEWAYSRUN_ACCELERATION) / 2)
//! 릐궕롈귕뚣귣궸릋귔돿뫊뱗
#define HUMAN_REGRESSRUN_SIDEWAYSRUN_ACCELERATION (HUMAN_REGRESSRUN_ACCELERATION)
#define HUMAN_PROGRESSWALK_ACCELERATION	0.35f		//!< 릐궕뺖궘돿뫊뱗
#define HUMAN_ATTENUATION			0.5f		//!< 릐궻1긲깒??궇궫귟궻뙵릨뿦?걁0.0 < x < 1.0걂

#define HUMAN_JUMP_SPEED 2.2f					//!< 긙긿깛긵궥귡뫊뱗

#define HUMAN_HEIGHT 20.0f			//!< 릐궻뛼궠
#define HUMAN_DAMAGE_HEAD 2.0f		//!< 뭙궕 벆 궸뱰궫궯궫뤾뜃궻?긽?긙?뿦
#define HUMAN_DAMAGE_UP 1.0f		//!< 뭙궕 뤵뵾릆 궸뱰궫궯궫뤾뜃궻?긽?긙?뿦
#define HUMAN_DAMAGE_LEG 0.7f		//!< 뭙궕 돷뵾릆 궸뱰궫궯궫뤾뜃궻?긽?긙?뿦
#define HUMAN_DAMAGE_ZOMBIEU 15		//!< ?깛긮궻뛘똼귩롷궚궫뤾뜃궻띍믟?긽?긙
#define HUMAN_DAMAGE_ZOMBIEA 5		//!< ?깛긮궻뛘똼귩롷궚궫뤾뜃궻믁돿?긽?긙쀊
#define HUMAN_DAMAGE_SPEED 0.066f	//!< 뿇돷쀊
#define HUMAN_DAMAGE_MINSPEED -3.8f	//!< 뿇돷궸귝귟?긽?긙귩롷궚궶궋뫊뱗
#define HUMAN_DAMAGE_MAXSPEED -7.0f	//!< 띍묈뿇돷뫊뱗
#define HUMAN_DAMAGE_MAXFALL 120	//!< 뿇돷궸귝귡띍묈?긽?긙
#define MAX_DAMAGE_GRENADE_DISTANCE 80.0f		//!< 롨왮뭙궸귝귟?긽?긙귩롷궚귡띍묈떁뿣
#define HUMAN_DAMAGE_GRENADE_HEAD 100			//!< 롨왮뭙궸귝귡 벆 귉궻띍묈?긽?긙
#define HUMAN_DAMAGE_GRENADE_LEG 80				//!< 롨왮뭙궸귝귡 뫉 귉궻띍묈?긽?긙
#define SMALLOBJECT_DAMAGE_GRENADE 80			//!< 롨왮뭙궸귝귡 룷븿 귉궻띍묈?긽?긙

#define IMPACT_GRENADE_DAMAGE_SCALE 0.5f			//!< 충격 수류탄 피해 배율

#define ARMRAD_NOWEAPON DegreeToRadian((-90 + 20))			//!< 롨귆귞렄궻쁱궻?됪둷뱗
#define ARMRAD_RELOADWEAPON DegreeToRadian(-20)				//!< 깏깓?긤렄궻쁱궻?됪둷뱗
#define ARMRAD_ZOMBIEATTACK DegreeToRadian(-0)

#define HUMAN_RECOIL_MAX_Y DegreeToRadian(30.0f)

#define HUMAN_MAPCOLLISION_R 2.8f							//!< 릐궴?긞긵궻뱰궫귟뵽믦?뵾똞
#define HUMAN_MAPCOLLISION_HEIGHT 10.0f						//!< 릐궴?긞긵궻뱰궫귟뵽믦?뛼궠걁뭾갌뜕믴뱗걂
#define HUMAN_MAPCOLLISION_ADD_HEIGHT_A 9.0f				//!< 릐궴?긞긵궻뱰궫귟뵽믦 믁돿빁?뛼궠걁뭾갌뷭??긞긵 SCHOOL 궸뜃귦궧귡걂
#define HUMAN_MAPCOLLISION_ADD_HEIGHT_B 13.0f				//!< 릐궴?긞긵궻뱰궫귟뵽믦 믁돿빁?뛼궠걁뭾갌뷭??긞긵 SCHOOL 궸뜃귦궧귡걂
#define HUMAN_MAPCOLLISION_SLOPEANGLE DegreeToRadian(50)	//!< 릐궴?긞긵궻뱰궫귟뵽믦?뱋귢궶궋롈뽋궻둷뱗
#define HUMAN_MAPCOLLISION_CLIMBHEIGHT 3.2f					//!< 릐궴?긞긵궻뱰궫귟뵽믦?뱋귢귡뛼궠
#define HUMAN_MAPCOLLISION_CLIMBADDY 0.4f					//!< 릐궴?긞긵궻뱰궫귟뵽믦?1긲깒??궳뱋귡뛼궠
#define HUMAN_MAPCOLLISION_GROUND_HEIGHT -0.5f				//!< 릐궴?긞긵궻뱰궫귟뵽믦?뭤뽋뵽믦?뛼궠
#define HUMAN_MAPCOLLISION_GROUND_R1 0.15f					//!< 릐궴?긞긵궻뱰궫귟뵽믦?뭤뽋뵽믦?뵾똞걁1걂
#define HUMAN_MAPCOLLISION_GROUND_R2 0.50f					//!< 릐궴?긞긵궻뱰궫귟뵽믦?뭤뽋뵽믦?뵾똞걁2걂
#define HUMAN_MAPCOLLISION_GROUND_R3 0.30f					//!< 릐궴?긞긵궻뱰궫귟뵽믦?뭤뽋뵽믦?뵾똞걁3걂
#define HUMAN_MAPCOLLISION_CHECK_MAXDIST 12.0f				//!< 릐궴?긞긵궻뱰궫귟뵽믦?1긲깒??궳댷벍궥귡띍묈떁뿣
#define HUMAN_MAPCOLLISION_CHECK_HEIGHT (HUMAN_HEIGHT-3.0f)	//!< 릐궴?긞긵궻뱰궫귟뵽믦?띍뢎밒궶댷벍됀붬귩뵽믦궥귡뛼궠걁뭾갌뙣믴뱗걂
#define HUMAN_DEADLINE -100.0f						//!< 릐궕?뻊궥귡Y띆뷭걁긢긞긤깋귽깛걂
#define HUMAN_DEADADDRY DegreeToRadian(0.75f)		//!< ?뫬궻?귢귡돿뫊뱗

#define BULLET_SPEEDSCALE 2.5f			//!< 뭙뫊궻?뿦
#define BULLET_DESTROYFRAME 40			//!< 뭙궻뤑뽅긲깒??릶
#define GRENADE_DESTROYFRAME 100		//!< 롨왮뭙궻뵚뵯긲깒??릶

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 2		//!< Select include file.
#endif
#include "main.h"

#define HUMAN_DEADBODY_COLLISION	//!< @brief 릐궕?귢귡띧궸?긞긵궴뱰궫귟뵽믦귩렳?궥귡궔 @details 믦릶먬뙻뾎뚼갌뱰궫귟뵽믦귩렳??믦릶먬뙻뼰뚼걁긓긽깛긣돸걂갌궩궻귏귏?궥궬궚

//! @brief 긆긳긙긃긏긣듖뿚긏깋긚걁딈믨긏깋긚걂
//! @details 3D궳띆뷭듖뿚귘?됪귩뛱궎긆긳긙긃긏긣멣붗귩듖뿚궥귡긏깋긚똒궻딈믨긏깋긚궳궥갃
class object
{
protected:
	class ParameterInfo* Param;	//!< 먠믦뭠귩듖뿚궥귡긏깋긚귉궻?귽깛?
	float pos_x;		//!< X띆뷭
	float pos_y;		//!< Y띆뷭
	float pos_z;		//!< Z띆뷭
	float rotation_x;	//!< 됷?둷뱗

	float model_size;	//!< ?됪긖귽긛
	int id_parameter;	//!< 긢??궻롰쀞
	int id_model;		//!< 긾긢깑궻긢??붥뜂
	int id_texture;		//!< 긡긏긚?긿궻긢??붥뜂
	bool DarkModelFlag;	//!< 긾긢깑귩댠궘궥귡
	bool EnableFlag;	//!< 뾎뚼돸긲깋긐

public:
	object(class ParameterInfo* in_Param = NULL, float x = 0.0f, float y = 0.0f, float z = 0.0f, float rx = 0.0f, float size = 1.0f, bool flag = true);
	~object();
	virtual void SetParameterInfoClass(class ParameterInfo* in_Param);
	virtual void SetPosData(float x, float y, float z, float rx);
	virtual void GetPosData(float* x, float* y, float* z, float* rx);

	virtual void SetEnableFlag(bool flag);
	virtual bool GetEnableFlag();
	virtual void SetModel(int id, float size);
	virtual void GetModel(int* id, float* size);
	virtual void SetTexture(int id);
	virtual int GetTexture();
	virtual void SetDarkModelFlag(bool flag);
	virtual int ProcessObject();
	virtual void Render(class D3DGraphics* d3dg, bool NoModel);
	enum ShellReloadState {
		SHELL_RELOAD_NONE,
		SHELL_RELOAD_START_DELAY,
		SHELL_RELOAD_INSERTING,
		SHELL_RELOAD_INTERRUPT_DELAY, // ✨ 추가: 중단 시 대기할 상태
		SHELL_RELOAD_END_DELAY
	};
};


// ------------------------------------------------------------
// Human render parts mask
// 버그 인물의 일부 신체만 안 보이게 하기 위한 렌더링 마스크.
// 히트박스, AI, 피격 판정은 그대로 유지된다.
// ------------------------------------------------------------
#define HUMAN_RENDER_NONE		0x00	// 아무 신체 파츠도 표시하지 않음
#define HUMAN_RENDER_UPPER		0x01	// 상체: 몸통 + 머리
#define HUMAN_RENDER_LEG		0x02	// 다리
#define HUMAN_RENDER_ARM		0x04	// 팔
#define HUMAN_RENDER_WEAPON		0x08	// 손에 든 무기
#define HUMAN_RENDER_ALL		(HUMAN_RENDER_UPPER | HUMAN_RENDER_LEG | HUMAN_RENDER_ARM | HUMAN_RENDER_WEAPON)


//! @brief 릐듖뿚긏깋긚
class human : public object
{
protected:
	int point_dataid;			//!< ?귽깛긣궻긢??붥뜂
	signed short int point_p4;	//!< ?귽깛긣궻렞빶붥뜂
	int teamid;					//!< ???붥뜂
	float move_x;				//!< X렡뫊뱗
	float move_y;				//!< Y렡걁뿇돷걂뫊뱗
	float move_z;				//!< Z렡뫊뱗
	bool move_y_flag;			//!< Y렡댷벍긲깋긐
	int move_y_upper;			//!< Y렡뤵뤈뗕?긇긂깛??
	bool move_y_landing;		//!< 뭶뭤긲깋긐
	int underblock_id;			//!< 뫉뙰궻긳깓긞긏ID
	int underblock_face;		//!< 뫉뙰궻긳깓긞긏궻뽋붥뜂
	float rotation_y;			//!< 멣뫬궻됷?둷뱗
	float armrotation_y;		//!< 쁱궻됷?둷뱗

	float upmodel_size;
	float armmodel_size;		//!< 쁱?됪긖귽긛
	float legmodel_size;		//!< 뫉?됪긖귽긛
	class weapon *weapon[TOTAL_HAVEWEAPON];			//!< 븧딇
	int noweapon_bullets[TOTAL_HAVEWEAPON];			//!< 븧딇뼟룋렃렄궻뜃똶뭙릶
	int noweapon_loadbullets[TOTAL_HAVEWEAPON];		//!< 븧딇뼟룋렃렄궻몧뭙릶
	int selectweapon;			//!< 븧딇A/B궻멗묖
	int selectweaponcnt;		//!< 븧딇궻먛귟뫶궑걁렃궭뫶궑걂긇긂깛긣
	int changeweaponidcnt;		//!< 븧딇궻롰쀞먛귟뫶궑긇긂깛긣
	int changeweaponid_target;
	int changeweaponid_total_time;
	int weaponshotcnt;			//!< 븧딇궻쁀롅긇긂깛긣
	int weaponreloadcnt;		//!< 븧딇궻깏깓?긤긇긂깛긣
	bool weaponshotframe;		//!< 뵯뺺긲깋긐
	int weaponburstmodecnt;		//!< 긫?긚긣긾?긤긇긂깛긣
	int hp;						//!< 뫬쀍
	int deadstate;				//!< ?뫬궸궶궯궲궋귡궔
	float add_ry;				//!< ?뫬궻?귢귡돿뫊뱗
	float move_rx;				//!< 댷벍둷뱗
	int MoveFlag;				//!< 댷벍뺴뛀귩?궥긲깋긐
	int MoveFlag_lt;			//!< 걁멟됷궻걂댷벍뺴뛀귩?궥긲깋긐
	int scopemode;				//!< 긚긓?긵럊뾭긾?긤
	bool HitFlag;				//!< 뷀뭙귩?궥긲깋긐
	float Hit_rx;				//!< 뷀뭙궢궫뺴뛀
	bool silent_hit_flag;		//!< 소음기 총 피격 여부. 피해 방향 표시를 생략할 때 사용
	float totalmove;			//!< 뜃똶댷벍쀊
	int StateGunsightErrorRange;		//!< 뤖?궻륉뫴뚮뜼
	int ReactionGunsightErrorRange;		//!< 뤖?궻뵿벍뚮뜼
	bool Invincible;			//!< 뼰밎긲깋긐
	bool isPlayer;				// ✨ 추가: 이 객체가 플레이어인지 여부
	bool crouchflag;			// 앉기 상태
	int sitlegmodel;			// 앉기 정지 다리 모델
	int sitwalkmodel[TOTAL_SITWALKMODE];	// 앉기 이동 다리 모델
	int sitwalkcnt;
	int spinup_timer;               // 미니건 예열 타이머
	int minigun_spin_sound_timer;   // 미니건 회전음 반복 타이머
	bool minigun_spin_loop_enabled;   // 최대 예열 도달 후 지속음 유지 여부

	// === 수직 반동 관련 멤버 ===
// === 반동 오프셋 관련 변수 (최종) ===
	float recoil_y_offset;			// 현재 총기의 수직 반동 각도
	float recoil_accumulator;		// 연사 시 누적되는 추가 반동
	int recoil_accum_timer;			// 사격이 진행 중인지 확인하는 타이머
	float recoil_x_offset;			// 현재 총기의 좌우 반동 각도

	int recoil_side_shot_count;     // 연속 사격 카운터
	int recoil_side_timer;          // 연속 사격 유지 타이머

	int boltActionTimer;       // [✨ 추가 ✨] 볼트 액션 타이머
	int boltActionTotalTime;   // [✨ 추가 ✨] 볼트 액션 전체 시간 (게이지 표시용)
	void StartEndDelay();

	//산탄총 장전
	ShellReloadState shell_reload_state;
	int shell_reload_timer;
	int shell_reload_time_per_shell;

	bool needs_recovery_reset;
	float recovery_target_angle;	// 반동 회복이 멈출 최종 목표 각도

	// === 피격 효과 변수 ===
	int hit_effect_timer;
	bool shield_hit_effect_flag;
	int hit_marker_timer;
	bool is_headshot_hit;
	int render_parts_mask;		//!< 표시할 신체 파츠 마스크
	int render_override_weapon_id;	//!< 1인칭 렌더링용 임시 무기 ID

	int last_shot_fail_reason;

	// 플레이어 스킬: 은신 타이머
	int skill_stealth_timer;

	// 플레이어 스킬: 군인 탄약 강화 타이머
	int skill_ammoboost_timer;

	// 플레이어 스킬: 시민B 생존 본능 타이머
	int skill_survival_timer;

	// 플레이어 스킬: 시민B 긴급 회피 무적 타이머
	int skill_dodge_timer;

	// 플레이어 스킬: 비밀 요원 텔레포트 시전 무적 타이머
	int skill_teleport_cast_timer;

	// 플레이어 스킬: 시민A 철괴 타이머
	int skill_ironbody_timer;

	// 플레이어 스킬: 용병 전투 흡수 타이머
	int skill_absorb_timer;

	// 경찰 제압 명령: 제압된 적의 자동 무기 줍기 방지 타이머
	int skill_suppressed_timer;

	// 로봇: 처형 프로토콜 타이머
	int skill_robot_execute_timer;

	// 로봇 F: 에임핵 중 이동/피격 반응 탄퍼짐 제거용 플래그
	bool skill_aimhack_accuracy_flag;

	// 좀비: 격노 타이머
	int skill_rage_timer;

	// 플레이어 스킬: 동료 소환으로 생성된 인물인지 여부
	bool skill_summoned_ally_flag;

	bool skill_shadow_decoy_flag;
	int skill_shadow_decoy_last_attacker_id;

	class HumanMotionControl* MotionCtrl;


	void GunsightErrorRange();
	int CheckAndProcessDead(class Collision *CollD);
	void ControlProcess();
	void CollisionMap(class Collision *CollD, class BlockDataInterface *inblockdata, bool AddCollisionFlag, bool player);
	bool CollisionBlockScratch(class Collision *CollD, class BlockDataInterface *inblockdata, int blockid, float *px, float *py, float *pz, float px_old, float py_old, float pz_old, float in_vx, float in_vy, float in_vz, int mode);

public:
	human(class ParameterInfo* in_Param = NULL, float x = 0.0f, float y = 0.0f, float z = 0.0f, float rx = 0.0f, int id_param = -1, int dataid = 0, signed short int p4 = 0, int team = 0, bool flag = false);
	~human();
	virtual void SetParameterInfoClass(class ParameterInfo* in_Param);
	virtual void SetParamData(int id_param, int dataid, signed short int p4, int team, bool init);
	virtual void GetParamData(int* id_param, int* dataid, signed short int* p4, int* team);
	virtual void GetMovePos(float* x, float* y, float* z);
	virtual void ResetVerticalMove();
	virtual void SetModel(int upmodel, int armmodel[], int legmodel, int walkmodel[], int runmodel[]);
	virtual void SetSitModel(int legmodel, int walkmodel[]);
	virtual int GetHP();
	virtual bool SetHP(int in_hp);
	virtual void ForceDeadBody(bool faceup = true);
	virtual bool GetDeadFlag();
	virtual void SetRenderPartsMask(int mask);
	virtual int GetRenderPartsMask();
	void SetRenderOverrideWeaponID(int weapon_id);
	int GetRenderOverrideWeaponID();
	void StartRenderWeaponChangeMotion(int weapon_id, int frames);
	void StartSkillWeaponShotMotion(int weapon_id);
	void ForceSelectWeaponSlotForSkillReturn(int id, int frames);
	virtual bool GetDeadMotionFlag();
	virtual void SetTeamID(int id);
	virtual bool GetInvincibleFlag();
	virtual void SetInvincibleFlag(bool flag);
	virtual void SetWeapon(class weapon* in_weapon[], int nlnbs[], int nnbs[]);
	virtual int PickupWeapon(class weapon* in_weapon);
	virtual void ChangeHaveWeapon(int id = -1);
	virtual int GetChangeHaveWeaponCnt();
	virtual int ChangeWeaponID(int change_time = 0);
	virtual int GetChangeWeaponIDCnt();
	virtual int GetChangeWeaponIDTotalTime();
	virtual int GetWeaponReloadCnt();
	int GetTotalWeaponReloadCnt();
	virtual void GetWeapon(int* out_selectweapon, class weapon* out_weapon[], int nlnbs[], int nnbs[]);
	virtual int GetMainWeaponTypeNO();
	virtual bool GetWeaponBlazingmode();
	virtual bool ShotWeapon(int* weapon_paramid, int* GunsightErrorRange);
	virtual bool ReloadWeapon();
	bool InstantReloadWeapon();
	virtual bool DumpWeapon(class Collision* CollD = NULL);
	virtual void SetMoveForward();
	virtual void SetMoveBack();
	virtual void SetMoveLeft();
	virtual void SetMoveRight();
	virtual void SetMoveWalk();
	virtual int GetMovemode(bool nowdata);
	virtual int GetMoveFlag(bool nowdata);
	virtual bool SetEnableScope();
	virtual void SetDisableScope();
	virtual int GetScopeMode();
	virtual void GetRxRy(float* rx, float* ry);
	virtual void SetRxRy(float rx, float ry);
	virtual float GetDeadRy();
	virtual void Jump();
	virtual int GetJumpLanding(bool nowdata);
	virtual void GetUnderBlock(int* id, int* face);
	virtual void AddPosOrder(float rx, float ry, float speed);
	virtual void HitBulletHead(int attacks);
	virtual void HitBulletUp(int attacks);
	virtual void HitBulletLeg(int attacks);
	virtual void HitZombieAttack();
	virtual void HitGrenadeExplosion(int attacks);
	virtual void SetHitFlag(float rx);
	virtual void SetShieldHitFlag(float rx);
	virtual void SetSilentHitFlag(float rx);
	virtual bool CheckHit(float* rx, bool* shield_hit = NULL, bool* silent_hit = NULL);
	virtual float GetTotalMove();
	virtual int GetMoveMotionCount();
	virtual int ProcessObject(class Collision* CollD, class BlockDataInterface* inblockdata, bool AddCollisionFlag, bool player, bool F5mode);
	virtual int GetGunsightErrorRange();
	int GetBurstModeCnt();
	int GetBoltActionTimer();      // [✨ 추가 ✨]
	int GetBoltActionTotalTime();  // [✨ 추가 ✨]
	void SetBoltActionTimers(int total, int current); // [✨ 추가 ✨]

	// 미니건 예열
	void UpdateMinigunSpin(bool spinInput);
	bool IsMinigunSpinReady();
	int GetMinigunSpinTimer();
	int GetMinigunSpinTotalTime();
	bool ConsumeMinigunSpinSoundTick();
	void SetMinigunSpinLoopEnabled(bool flag);
	bool IsMinigunSpinLoopEnabled();

	void ResetBurstModeCnt();
	virtual void Render(class D3DGraphics* d3dg, class ResourceManager* Resource, bool DrawArmOnly, bool player, bool NoModel);
	virtual void SetIsPlayer(bool flag); // ✨ 추가
	void SetCrouchFlag(bool flag);
	bool GetCrouchFlag();
	float GetRecoilYOffset();
	void SetRecoilYOffset(float offset); // [✨ 이 줄을 추가하세요 ✨]
	float GetRecoilXOffset();
	void SetRecoilXOffset(float offset);

	int GetHitEffectTimer();
	void SetHitEffectTimer(int t);
	void DecreaseHitEffectTimer();
	bool IsShieldHitEffect();
	int GetHitMarkerTimer();
	void SetHitMarkerTimer(int t);
	void DecreaseHitMarkerTimer();
	void SetHeadshotFlag(bool flag);
	bool IsHeadshot();
	bool IsShellReloading();
	void InterruptShellReload();
	int GetShellReloadTimer();
	int GetShellReloadTimePerShell();
	ShellReloadState GetShellReloadState();
	float SpeedMultiplier;
	float CustomZoom;  // [✨ 추가] 현재 줌 배율 (기본값 1.0f)
	bool NightVision;


	// 플레이어 스킬: 은신
	void StartSkillStealth(int frames);
	void UpdateSkillStealth();
	bool GetSkillStealthFlag();
	int GetSkillStealthTimer();

	// 플레이어 스킬: 군인 탄약 강화
	void StartSkillAmmoBoost(int frames);
	void UpdateSkillAmmoBoost();
	bool GetSkillAmmoBoostFlag();
	int GetSkillAmmoBoostTimer();

	// 플레이어 스킬: 시민B 생존 본능
	void StartSkillSurvival(int frames);
	void UpdateSkillSurvival();
	bool GetSkillSurvivalFlag();
	int GetSkillSurvivalTimer();

	// 플레이어 스킬: 시민B 긴급 회피
	void StartSkillDodge(int frames);
	void UpdateSkillDodge();
	bool GetSkillDodgeFlag();
	int GetSkillDodgeTimer();

	// 플레이어 스킬: 비밀 요원 텔레포트 시전
	void StartSkillTeleportCast(int frames);
	void UpdateSkillTeleportCast();
	bool GetSkillTeleportCastFlag();
	int GetSkillTeleportCastTimer();

	// 플레이어 스킬: 시민A 철괴
	void StartSkillIronBody(int frames);
	void UpdateSkillIronBody();
	bool GetSkillIronBodyFlag();
	int GetSkillIronBodyTimer();

	// 플레이어 스킬: 회복 / 전투 흡수
	int GetMaxHP();
	bool HealHP(int value);
	void StartSkillAbsorb(int frames);
	void UpdateSkillAbsorb();
	bool GetSkillAbsorbFlag();
	int GetSkillAbsorbTimer();
	int ApplySkillAbsorbDamage(int damage);
	void AddSkillAbsorbHeal(int value);

	// 경찰 제압 명령
	void StartSkillSuppressed(int frames);
	void UpdateSkillSuppressed();
	bool GetSkillSuppressedFlag();
	int GetSkillSuppressedTimer();
	bool HasAnyWeaponForSuppress();
	bool ForceDropAllWeapons();

	// 로봇: 처형 프로토콜
	void StartSkillRobotExecute(int frames);
	void StopSkillRobotExecute();
	void UpdateSkillRobotExecute();
	bool GetSkillRobotExecuteFlag();
	int GetSkillRobotExecuteTimer();

	// 로봇 F: 에임핵 명중 보정
	void SetSkillAimHackAccuracyFlag(bool flag);
	bool GetSkillAimHackAccuracyFlag();

	// 좀비: 격노
	void StartSkillRage(int frames);
	void UpdateSkillRage();
	bool GetSkillRageFlag();
	int GetSkillRageTimer();

	// 플레이어 스킬: 동료 소환으로 생성된 인물인지 여부
	void SetSkillSummonedAllyFlag(bool flag);
	bool GetSkillSummonedAllyFlag();

	void SetSkillShadowDecoyFlag(bool flag);
	bool GetSkillShadowDecoyFlag();
	void SetSkillShadowDecoyLastAttackerID(int id);
	int GetSkillShadowDecoyLastAttackerID();
	void ClearSkillShadowDecoyLastAttackerID();

	// [✨ R6S 반동 제어용: 마우스 조작 시 자동 회복 지연 ✨]
	void DelayRecoilRecovery() { recoil_accum_timer = 8; }
	// [✨ 추가 ✨: 사격 중지 시 즉각 반동 회복을 위한 타이머 초기화]
	void ResetRecoilTimer() { recoil_accum_timer = 0; }

	enum ShotFailReason {
		SHOT_FAIL_NONE = 0,
		SHOT_FAIL_NOT_READY,
		SHOT_FAIL_EMPTY
	};

	int GetLastShotFailReason();

};

//! @brief 븧딇듖뿚긏깋긚
class weapon : public object
{
protected:
	float move_x;		//!< X렡댷벍쀊
	float move_y;		//!< Y렡댷벍쀊
	float move_z;		//!< Z렡댷벍쀊
	bool usingflag;		//!< 럊뾭뭷귩?궥긲깋긐
	int bullets;		//!< 뜃똶뭙릶
	int Loadbullets;	//!< 몧뭙릶
	bool motionflag;	//!< 띆뷭댷벍뭷귩?궥긲깋긐


public:
	weapon(class ParameterInfo *in_Param = NULL, float x = 0.0f, float y = 0.0f, float z = 0.0f, float rx = 0.0f, int id_param = 0, int nbs = 0, bool flag = false);
	~weapon();
	virtual void SetPosData(float x, float y, float z, float rx);
	virtual void SetParamData(int id_param, int lnbs, int nbs, bool init);
	virtual void GetParamData(int *id_param, int *lnbs, int *nbs);
	virtual bool GetUsingFlag();
	virtual int Pickup();
	virtual void Dropoff(float x, float y, float z, float rx, float speed, float add_move_x = 0.0f, float add_move_z = 0.0f, class Collision* CollD = NULL);
	virtual int Shot();
	virtual int ShotNoAmmoConsume();
	virtual int StartReload();
	virtual int RunReload();
	virtual bool ResetWeaponParam(class ResourceManager *Resource, int id_param, int lnbs, int nbs);
	virtual int ProcessObject(class Collision *CollD);
	virtual void Render(class D3DGraphics* d3dg, bool NoModel);
};

//! @brief 룷븿듖뿚긏깋긚
class smallobject : public object
{
protected:
	class MIFInterface *MIFdata;		//!< MIF귩듖뿚궥귡긏깋긚귉궻?귽깛?
	float rotation_y;			//!< �盛?�孫�ｱ�


	signed short int point_p4;	//!< ?�ｷｽ�ｹ幀ｸ｣�ｶｻ�橄ｹｶ�ｶ･�怩
	int hp;						//!< 뫬쀍
	float jump_rx;				//!< 뷅궽궥돘렡둷뱗
	float move_rx;				//!< 뷅궽궥돘렡댷벍쀊
	float add_rx;				//!< 뷅궽궥돘렡됷?쀊
	float add_ry;				//!< 뷅궽궥뢢렡됷?쀊
	int jump_cnt;				//!< 뷅궽궥뤵뤈긇긂깛긣

public:
	smallobject(class ParameterInfo *in_Param = NULL, class MIFInterface *in_MIFdata = NULL, float x = 0.0f, float y = 0.0f, float z = 0.0f, float rx = 0.0f, int id_param = 0, signed short int p4 = 0, bool flag = false);
	~smallobject();
	virtual void SetMIFInterfaceClass(class MIFInterface *in_MIFdata);
	virtual void SetParamData(int id_param, signed short int p4, bool init);
	virtual void GetParamData(int *id_param, signed short int *p4);
	virtual int GetHP();
	virtual float CollisionMap(class Collision *CollD);
	virtual void HitBullet(int attacks);
	virtual void HitGrenadeExplosion(int attacks);
	virtual void Destruction();
	virtual int ProcessObject();
	virtual void Render(class D3DGraphics* d3dg, bool NoModel);
};

//! @brief 뭙듴듖뿚긏깋긚
class bullet : public object
{
protected:
	float rotation_y;	//!< �盛?�孫�ｱ�

	int attacks;		//!< �尨�仂��
	int penetration;	//!< 듥믅쀍
	float speed;		//!< 뭙뫊
	int teamid;			//!< ???붥뜂
	int humanid;		//!< 릐궻긢??붥뜂
	float ontargetcnt;	//!< 뼺뭷렄궻긇긂깛긣릶
	bool silencer;		//!< 발사한 무기가 소음기 무기인지 여부
	int cnt;			//!< 긇긂깛긣

public:
	bullet(int modelid = -1, int textureid = -1);
	~bullet();
	virtual void SetPosData(float x, float y, float z, float rx, float ry);
	virtual void SetParamData(int _attacks, int _penetration, float _speed, int _teamid, int _humanid, float _ontargetcnt, bool init);
	virtual void GetPosData(float* x, float* y, float* z, float* rx, float* ry);
	virtual void GetParamData(int* _attacks, int* _penetration, float* _speed, int* _teamid, int* _humanid, float* _ontargetcnt);
	void SetSilencerFlag(bool flag);
	bool GetSilencerFlag();
	virtual int ProcessObject();
	virtual void Render(D3DGraphics* d3dg, bool NoModel);
};

//! @brief 롨왮뭙듖뿚긏깋긚
class grenade : public bullet
{
	float move_x;		//!< X렡댷벍쀊
	float move_y;		//!< Y렡댷벍쀊
	float move_z;		//!< Y렡댷벍쀊
	int weapon_paramid;	//!< 일반 수류탄 / 충격 수류탄 구분용 무기 ID

public:
	grenade(int modelid = -1, int textureid = -1);
	~grenade();
	void SetParamData(float speed, int _teamid, int _humanid, float _ontargetcnt, int _weapon_paramid, bool init);
	void GetParamData(float* _speed, int* _teamid, int* _humanid, float* _ontargetcnt);
	int GetWeaponParamID();
	float GetSpeed();
	int ProcessObject(class Collision* CollD);
	virtual void Render(D3DGraphics* d3dg, bool NoModel);
};

//! @brief 긄긲긃긏긣듖뿚긏깋긚
class effect : public object
{
protected:
	float move_x;		//!< X렡댷벍쀊
	float move_y;		//!< Y렡댷벍쀊
	float move_z;		//!< Z렡댷벍쀊
	float addmove_y;	//!< Y렡댷벍쀊궻돿럁쀊
	float rotation_y;	//!< 됷?둷뱗
	float rotation_texture;		//!< 됷?둷뱗
	float addrotation_texture;	//!< 됷?둷뱗궻돿럁쀊
	float addmodel_size;		//!< ?됪긖귽긛
	int brightness;				//!< 뼻귡궠
	int addbrightness;			//!< 뼻귡궠궻돿럁쀊
	float alpha;				//!< 벁뼻뱗
	float add_alpha;			//!< 벁뼻뱗궻돿럁쀊
	int cnt;			//!< 긇긂깛긣
	int type;			//!< 롰쀞

public:
	effect(float x = 0.0f, float y = 0.0f, float z = 0.0f, float size = 1.0f, float rotation = 0.0f, int texture = -1, int count = 0);
	~effect();
	virtual void SetParamData(float in_move_x, float in_move_y, float in_move_z, float in_addmove_y, float rotation, float addrotation, float size, float addsize, int count, int settype);
	virtual void SetTexture(int id, int in_brightness, int in_addbrightness, float in_alpha, float in_addalpha);
	virtual void SetRxRy(float rx, float ry);
	virtual int GetTextureID();
	virtual void GetMove(float *mx, float *my, float *mz);
	virtual bool GetCollideMapFlag();
	virtual int ProcessObject();
	virtual void Render(class D3DGraphics *d3dg, float camera_rx, float camera_ry, bool NoModel);
};

//! @brief 릐긾?긘깈깛맕뚥긏깋긚
//! @details 릐궻긾?긘깈깛걁귺긦긽?긘깈깛걂맕뚥귩뛱궎긏깋긚궳궥갃human긏깋긚볙궳럊뾭궢귏궥갃
//! @attention ?긏깋긚궸릐궕?뻊궢궫띧궻?귢귡긾?긘깈깛궼렳몧궠귢궲궋귏궧귪갃
class HumanMotionControl
{
	class ParameterInfo *Param;				//!< 먠믦뭠귩듖뿚궥귡긏깋긚귉궻?귽깛?
	int id_upmodel;							//!< 뤵뵾릆
	int id_armmodel[TOTAL_ARMMODE];			//!< 쁱
	int id_legmodel;						//!< 뫉걁먄?걂
	int id_walkmodel[TOTAL_WALKMODE];		//!< 뫉걁뺖궘걂
	int id_runmodel[TOTAL_RUNMODE];			//!< 뫉걁몪귡걂
	float reaction_y;				//!< 쁱귩뤵궛돷궛궥귡몜뫮둷뱗
	bool slowarm;					//!< 쁱궻뤵궛돷궛귩귚궯궘귟궥귡
	float armmodel_rotation_y;		//!< 쁱궻됷?둷뱗
	float legrotation_x;			//!< 뫉궻됷?둷뱗
	int walkcnt;					//!< 뺖궘긾?긘깈깛궻긇긂깛긣
	int runcnt;						//!< 몪귡긾?긘깈깛궻긇긂깛긣
	int armmodelid;					//!< ?됪궥귡쁱궻긾긢깑
	int legmodelid;					//!< ?됪궥귡뫉궻긾긢깑

public:
	HumanMotionControl(class ParameterInfo *in_Param = NULL);
	~HumanMotionControl();
	void SetParameterInfoClass(class ParameterInfo *in_Param);
	void SetModel(int upmodel, int armmodel[], int legmodel, int walkmodel[], int runmodel[]);
	void Init(float rx);
	void PickupWeapon(int weapon_paramid);
	void ChangeHaveWeapon(int weapon_paramid);
	void ChangeWeaponID(int weapon_paramid);
	void ShotWeapon(int weapon_paramid);
	void ReloadWeapon(int weapon_paramid);
	void DumpWeapon();
	void Jump();
	void ProcessObject(float rotation_x, float armrotation_y, int weapon_paramid, int ReloadCnt, int ChangeWeaponIDCnt, int MoveFlag, int hp, bool PlayerFlag, bool ZombieFlag);
	void GetRenderMotion(float *arm_rotation_y, float *leg_rotation_x, int *upmodel, int *armmodel, int *legmodel);
	int GetMoveMotionCnt();
};

//! 릐궻뫉궻륉뫴귩렑궥믦릶
enum Human_LegState
{
	LEG_STOP = 0,
	LEG_WALK,
	LEG_RUN
};

//! 릐궻댷벍?띿귩?궥긲깋긐
enum Human_MoveFlag
{
	MOVEFLAG_FORWARD = 0x01,
	MOVEFLAG_BACK = 0x02,
	MOVEFLAG_LEFT = 0x04,
	MOVEFLAG_RIGHT = 0x08,
	MOVEFLAG_WALK = 0x10,
	MOVEFLAG_JUMP = 0x20,
};

//! 긄긲긃긏긣궻롰쀞귩?궥믦릶
enum Effect_Type
{
	EFFECT_NORMAL = 0x00,			//!< 긩??깑
	EFFECT_NOBILLBOARD = 0x01,		//!< 긮깑??긤돸궢궶궋
	EFFECT_COLLIDEMAP = 0x02		//!< ?긞긵궴궻뱰궫귟뵽믦
};

#endif