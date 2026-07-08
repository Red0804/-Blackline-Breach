//! @file object.cpp
//! @brief object긏깋긚궻믦?

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

#include <stdio.h>
#include <windows.h>
#include "object.h"
#include "playerskill.h"

//! @brief 긓깛긚긣깋긏?
object::object(class ParameterInfo *in_Param, float x, float y, float z, float rx, float size, bool flag)
{
	Param = in_Param;
	pos_x = x;
	pos_y = y;
	pos_z = z;
	rotation_x = rx;

	EnableFlag = flag;
	model_size = size;

	id_parameter = 0;
	id_model = -1;
	id_texture = -1;
	DarkModelFlag = false;
}

//! @brief 긢귻긚긣깋긏?
object::~object()
{}

//! @brief 먠믦뭠귩듖뿚궥귡긏깋긚귩뱋?
//! @attention 둫듫릶귩럊뾭궥귡멟궸렳뛱궥귡궞궴갃
void object::SetParameterInfoClass(class ParameterInfo *in_Param)
{
	Param = in_Param;
}

//! @brief 띆뷭궴둷뱗귩먠믦
//! @param x X띆뷭
//! @param y Y띆뷭
//! @param z Z띆뷭
//! @param rx 돘렡됷?
void object::SetPosData(float x, float y, float z, float rx)
{
	pos_x = x;
	pos_y = y;
	pos_z = z;
	rotation_x = rx;

}

//! @brief 띆뷭궴둷뱗귩롦벦
//! @param x X띆뷭귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param y Y띆뷭귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param z Z띆뷭귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param rx 돘렡됷?귩롷궚롦귡?귽깛?걁NULL됀걂
void object::GetPosData(float *x, float *y, float *z, float *rx)
{
	if( x != NULL ){ *x = pos_x; }
	if( y != NULL ){ *y = pos_y; }
	if( z != NULL ){ *z = pos_z; }
	if( rx != NULL ){ *rx = rotation_x; }
}


//! @brief 뾎뚼돸긲깋긐귩먠믦
//! @param flag 먠믦궥귡긲깋긐
void object::SetEnableFlag(bool flag)
{
	EnableFlag = flag;
}

//! @brief 뾎뚼돸긲깋긐귩롦벦
//! @return 뙸띪먠믦궠귢궲궋귡긲깋긐
bool object::GetEnableFlag()
{
	return EnableFlag;
}

//! @brief 긾긢깑긢??귩먠믦
//! @param id 긾긢깑궻긢??붥뜂
//! @param size ?됪?뿦
void object::SetModel(int id, float size)
{
	id_model = id;
	model_size = size;
}

//! @brief 긾긢깑긢??귩롦벦
//! @param id 긾긢깑궻긢??붥뜂귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param size ?됪?뿦귩롷궚롦귡?귽깛?걁NULL됀걂
void object::GetModel(int *id, float *size)
{
	if( id != NULL ){ *id = id_model; }
	if( size != NULL ){ *size = model_size; }
}

//! @brief 긡긏긚?긿귩먠믦
//! @param id 긡긏긚?긿궻긢??붥뜂
void object::SetTexture(int id)
{
	id_texture = id;
}

//! @brief 긡긏긚?긿귩롦벦
//! @return 긡긏긚?긿궻긢??붥뜂
int object::GetTexture()
{
	return id_texture;
}

//! @brief 긾긢깑귩댠궘궥귡긲깋긐귩먠믦
//! @param flag 긲깋긐
void object::SetDarkModelFlag(bool flag)
{
	DarkModelFlag = flag;
}

//! @brief 똶럁귩렳뛱걁렔뾕뿇돷궶궵걂
int object::ProcessObject()
{
	return 0;
}

//! @brief ?됪
//! @param d3dg D3DGraphics궻?귽깛?
//! @param NoModel 긾긢깑?됪뼰뚼돸
void object::Render(D3DGraphics *d3dg, bool NoModel)
{
	if( d3dg == NULL ){ return; }
	if( EnableFlag == false ){ return; }

	d3dg->SetWorldTransform(pos_x, pos_y, pos_z, rotation_x, 0.0f, model_size);
	d3dg->RenderModel(id_model, id_texture, DarkModelFlag, false, NoModel);
}

//! @brief 긓깛긚긣깋긏?
human::human(class ParameterInfo* in_Param, float x, float y, float z, float rx, int id_param, int dataid, signed short int p4, int team, bool flag)
{
	Param = in_Param;
	pos_x = x;
	pos_y = y;
	pos_z = z;
	move_x = 0.0f;
	move_y = 0.0f;
	move_z = 0.0f;
	move_y_flag = false;
	move_y_upper = 0;
	move_y_landing = false;
	underblock_id = -1;
	underblock_face = -1;
	rotation_x = rx;
	id_parameter = id_param;
	upmodel_size = 9.4f;
	armmodel_size = 9.0f;
	legmodel_size = 9.0f;
	EnableFlag = flag;
	rotation_y = 0.0f;
	armrotation_y = 0.0f;

	point_dataid = dataid;
	point_p4 = p4;
	teamid = team;
	NightVision = false;
	CustomZoom = 1.5f; // 기본 1배율로 초기화

	skill_stealth_timer = 0;
	skill_ammoboost_timer = 0;
	skill_survival_timer = 0;
	skill_dodge_timer = 0;
	skill_teleport_cast_timer = 0;
	skill_ironbody_timer = 0;

	skill_absorb_timer = 0;
	skill_suppressed_timer = 0;
	skill_robot_execute_timer = 0;
	skill_aimhack_accuracy_flag = false;
	skill_rage_timer = 0;

	for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
		weapon[i] = NULL;
		noweapon_bullets[i] = 0;
		noweapon_loadbullets[i] = 0;
	}
	selectweapon = 1;
	selectweaponcnt = 0;
	changeweaponidcnt = 0;
	changeweaponid_target = -1;
	weaponshotcnt = 0;
	weaponreloadcnt = 0;
	weaponshotframe = false;
	weaponburstmodecnt = 0;
	hp = 0;
	deadstate = 0;
	add_ry = 0.0f;
	id_texture = -1;

	move_rx = 0.0f;
	MoveFlag = 0x00;
	MoveFlag_lt = MoveFlag;
	scopemode = 0;
	HitFlag = false;
	Hit_rx = 0.0f;
	silent_hit_flag = false;
	totalmove = 0.0f;
	StateGunsightErrorRange = 0;
	ReactionGunsightErrorRange = 0;
	render_override_weapon_id = -1;
	Invincible = false;
	isPlayer = false; // ✨ 추가: isPlayer 플래그 초기화
	mission_event_target_type = MISSION_EVENT_TARGET_NONE;
	mission_event_hp_multiplier = 1;
	crouchflag = false;
	sitlegmodel = -1;
	for (int i = 0; i < TOTAL_SITWALKMODE; i++) {
		sitwalkmodel[i] = -1;
	}
	sitwalkcnt = 0;

	MotionCtrl = new HumanMotionControl;

	
	recoil_y_offset = 0.0f;
	recoil_x_offset = 0.0f;

	hit_effect_timer = 0;
	shield_hit_effect_flag = false;
	hit_marker_timer = 0;
	is_headshot_hit = false;
	render_parts_mask = HUMAN_RENDER_ALL;
	render_override_weapon_id = -1;
	render_generation = 0;
	render_position_override = false;
	render_position_x = pos_x;
	render_position_y = pos_y;
	render_position_z = pos_z;
	render_angle_offset_override = false;
	render_rotation_x_offset = 0.0f;
	render_view_y_offset = 0.0f;
	needs_recovery_reset = false;
	recovery_target_angle = 0.0f;
	recoil_accumulator = 0.0f;
	recoil_accum_timer = 0;

	recoil_side_shot_count = 0;
	recoil_side_timer = 0;

	spinup_timer = 0;               // 미니건 예열 타이머 초기화
	minigun_spin_sound_timer = 0;   // 미니건 회전음 반복 타이머 초기화
	minigun_spin_loop_enabled = false;
	boltActionTimer = 0;
	boltActionTotalTime = 0;

	changeweaponidcnt = 0;
	changeweaponid_target = -1;
	changeweaponid_total_time = 0;

	shell_reload_state = SHELL_RELOAD_NONE;
	shell_reload_timer = 0;
	shell_reload_time_per_shell = 0;

	SpeedMultiplier = 1.0f;

	skill_stealth_timer = 0;
	skill_ammoboost_timer = 0;
	skill_survival_timer = 0;
	skill_dodge_timer = 0;
	skill_teleport_cast_timer = 0;
	skill_ironbody_timer = 0;

	skill_absorb_timer = 0;
	skill_suppressed_timer = 0;
	skill_robot_execute_timer = 0;
	skill_aimhack_accuracy_flag = false;
	skill_rage_timer = 0;
	skill_summoned_ally_flag = false;

	skill_shadow_decoy_flag = false;
	skill_shadow_decoy_last_attacker_id = -1;

	last_shot_fail_reason = SHOT_FAIL_NONE;
}

void human::SetHeadshotFlag(bool flag)
{
	is_headshot_hit = flag;
}

bool human::IsHeadshot()
{
	return is_headshot_hit;
}

// 원래 최대 체력 반환
int human::GetBaseMaxHP()
{
	if (Param == NULL) { return 100; }

	HumanParameter data;
	if (Param->GetHuman(id_parameter, &data) != 0) {
		return 100;
	}

	return data.hp;
}

// 이벤트 대상 배율을 반영한 최대 체력 반환
int human::GetMaxHP()
{
	int base_hp = GetBaseMaxHP();
	int multiplier = mission_event_hp_multiplier;

	if (multiplier < 1) { multiplier = 1; }
	return base_hp * multiplier;
}

void human::ResetMissionEventTarget()
{
	mission_event_target_type = MISSION_EVENT_TARGET_NONE;
	mission_event_hp_multiplier = 1;
}

void human::SetMissionEventTarget(int target_type, int hp_multiplier, bool apply_hp_boost)
{
	mission_event_target_type = target_type;
	mission_event_hp_multiplier = 1;

	if ((apply_hp_boost == false) || (hp_multiplier <= 1) || (hp <= 0)) {
		return;
	}

	mission_event_hp_multiplier = hp_multiplier;
	hp *= hp_multiplier;

	int max_hp = GetMaxHP();
	if (hp > max_hp) {
		hp = max_hp;
	}
}

int human::GetMissionEventTargetType()
{
	return mission_event_target_type;
}

bool human::IsMissionEventTarget()
{
	return (mission_event_target_type != MISSION_EVENT_TARGET_NONE);
}

// 체력 회복. 최대 체력 초과 불가.
bool human::HealHP(int value)
{
	if (value <= 0) { return false; }
	if (hp <= 0) { return false; }

	int max_hp = GetMaxHP();

	if (hp >= max_hp) {
		return false;
	}

	hp += value;

	if (hp > max_hp) {
		hp = max_hp;
	}

	return true;
}

// [✨ 아래 함수 3개를 파일의 아무 곳에나 추가하세요 ✨]
int human::GetHitMarkerTimer()
{
	return hit_marker_timer;
}

void human::SetHitMarkerTimer(int t)
{
	hit_marker_timer = t;
}

void human::DecreaseHitMarkerTimer()
{
	if (hit_marker_timer > 0) {
		hit_marker_timer--;
	}
}

int human::GetLastShotFailReason()
{
	return last_shot_fail_reason;
}

//! @brief 긢귻긚긣깋긏?
human::~human()
{
	delete MotionCtrl;
}

//! @brief 먠믦뭠귩듖뿚궥귡긏깋긚귩뱋?
//! @attention 둫듫릶귩럊뾭궥귡멟궸렳뛱궥귡궞궴갃
void human::SetParameterInfoClass(class ParameterInfo *in_Param)
{
	Param = in_Param;

	MotionCtrl->SetParameterInfoClass(in_Param);
}

//! @brief 먠믦뭠귩먠믦
//! @param id_param 릐궻롰쀞붥뜂
//! @param dataid ?귽깛긣궻긢??붥뜂
//! @param p4 묉4긬깋긽??
//! @param team ???붥뜂
//! @param init 긆긳긙긃긏긣귩룊딖돸
//! @warning init덙릶귩true궸궢궲룊딖돸궥귡뤾뜃갂먩궸SetPosData()듫릶궳둷뱗귩먠믦궢궲궓궘궞궴갃
void human::SetParamData(int id_param, int dataid, signed short int p4, int team, bool init)
{
	id_parameter = id_param;
	point_dataid = dataid;
	point_p4 = p4;
	teamid = team;

	if( init == true ){
		HumanParameter data;

		// 같은 배열 슬롯에 새 인물이 생성될 때 이전 렌더 상태와 섞이지 않게 한다.
		render_generation += 1;

		move_x = 0.0f;
		move_y = 0.0f;
		move_z = 0.0f;
		move_y_flag = false;
		move_y_upper = 0;
		move_y_landing = false;
		underblock_id = -1;
		underblock_face = -1;
		rotation_y = 0.0f;
		armrotation_y = DegreeToRadian(-30);

		for(int i=0; i<TOTAL_HAVEWEAPON; i++){
			weapon[i] = NULL;
			noweapon_bullets[i] = 0;
			noweapon_loadbullets[i] = 0;
		}
		selectweapon = 1;
		selectweaponcnt = 0;
		changeweaponidcnt = 0;
		weaponshotcnt = 0;
		weaponreloadcnt = 0;
		weaponshotframe = false;
		weaponburstmodecnt = 0;
		if( Param->GetHuman(id_param, &data) == 0 ){
			hp = data.hp;
		}
		else{
			hp = 0;
		}
		deadstate = 0;
		add_ry = 0.0f;
		MoveFlag = 0x00;
		MoveFlag_lt = MoveFlag;
		scopemode = 0;
		HitFlag = false;
		Hit_rx = 0.0f;
		silent_hit_flag = false;
		totalmove = 0.0f;
		Invincible = false;
		ResetMissionEventTarget();

		skill_stealth_timer = 0;
		skill_ammoboost_timer = 0;
		skill_survival_timer = 0;
		skill_dodge_timer = 0;
		skill_teleport_cast_timer = 0;
		skill_ironbody_timer = 0;

		skill_absorb_timer = 0;
		skill_suppressed_timer = 0;
		skill_robot_execute_timer = 0;
		skill_rage_timer = 0;
		skill_summoned_ally_flag = false;

		skill_shadow_decoy_flag = false;
		skill_shadow_decoy_last_attacker_id = -1;

		spinup_timer = 0;


		spinup_timer = 0;               // 미니건 예열 타이머 초기화
		minigun_spin_sound_timer = 0;   // 미니건 회전음 반복 타이머 초기화
		minigun_spin_loop_enabled = false;

		changeweaponidcnt = 0;
		changeweaponid_target = -1;
		changeweaponid_total_time = 0;

		render_parts_mask = HUMAN_RENDER_ALL;
		render_override_weapon_id = -1;
		render_position_override = false;
		render_angle_offset_override = false;
		render_rotation_x_offset = 0.0f;
		render_view_y_offset = 0.0f;

		MotionCtrl->Init(rotation_x);
	}
}

//! @brief 먠믦뭠귩롦벦
//! @param id_param 릐궻롰쀞붥뜂귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param dataid ?귽깛긣궻긢??붥뜂귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param p4 묉4긬깋긽??귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param team ???붥뜂귩롷궚롦귡?귽깛?걁NULL됀걂
void human::GetParamData(int *id_param, int *dataid, signed short int *p4, int *team)
{
	if( id_param != NULL ){ *id_param = id_parameter; }
	if( dataid != NULL ){ *dataid = point_dataid; }
	if( p4 != NULL ){ *p4 = point_p4; }
	if( team != NULL ){ *team = teamid; }
}

//! @brief HP귩롦벦
//! @return HP
int human::GetHP()
{
	return hp;
}

//! @brief HP귩먠믦
//! @param in_hp 륷궫궸먠믦궥귡HP
//! @return 맟뚻갌true?렪봲갌false
//! @attention HP궕1댥뤵궻릐궸뫮궢궲렳뛱궢궶궋궴렪봲궢귏궥갃
bool human::SetHP(int in_hp)
{
	if( hp > 0 ){
		hp = in_hp;
		return true;
	}
	return false;
}

//! @brief 처음부터 시체 상태로 만든다
//! @param faceup true면 앞으로/한쪽 방향으로 눕힘, false면 반대 방향으로 눕힘
//! @brief 처음부터 죽은 상태로 만든다
//! @param faceup true면 한쪽 방향으로 쓰러지고, false면 반대 방향으로 쓰러진다
void human::ForceDeadBody(bool faceup)
{
	hp = 0;

	// 바로 deadstate = 5로 만들면 바닥 보정 과정을 건너뛰어서 공중에 뜰 수 있다.
	// 따라서 기존 사망 처리 루틴이 쓰러짐/낙하/바닥 보정을 수행하도록 0에서 시작한다.
	deadstate = 0;

	move_x = 0.0f;
	move_y = 0.0f;
	move_z = 0.0f;
	move_y_flag = false;
	move_y_upper = 0;
	move_y_landing = false;

	add_ry = 0.0f;
	rotation_y = 0.0f;

	// 죽는 방향 지정.
	// CheckAndProcessDead()가 Hit_rx와 rotation_x 차이를 보고 쓰러지는 방향을 정한다.
	if (faceup == true) {
		Hit_rx = rotation_x;
	}
	else {
		Hit_rx = AngleNormalization(rotation_x + (float)M_PI);
	}

	armrotation_y = DegreeToRadian(-30);

	MoveFlag = 0x00;
	MoveFlag_lt = 0x00;

	weaponshotcnt = 0;
	weaponreloadcnt = 0;
	weaponshotframe = false;
	weaponburstmodecnt = 0;

	selectweaponcnt = 0;
	changeweaponidcnt = 0;

	scopemode = 0;
	HitFlag = false;
	totalmove = 0.0f;
}

//! @brief 표시할 신체 파츠 마스크를 설정한다
//! @param mask HUMAN_RENDER_* 조합
void human::SetRenderPartsMask(int mask)
{
	render_parts_mask = mask;
}

//! @brief 표시할 신체 파츠 마스크를 얻는다
//! @return HUMAN_RENDER_* 조합
int human::GetRenderPartsMask()
{
	return render_parts_mask;
}

void human::SetRenderOverrideWeaponID(int weapon_id)
{
	render_override_weapon_id = weapon_id;
}

int human::GetRenderOverrideWeaponID()
{
	return render_override_weapon_id;
}

int human::GetRenderGeneration()
{
	return render_generation;
}

void human::SetRenderPositionOverride(float x, float y, float z)
{
	render_position_override = true;
	render_position_x = x;
	render_position_y = y;
	render_position_z = z;
}

void human::ClearRenderPositionOverride()
{
	render_position_override = false;
}

void human::SetRenderAngleOffset(float rotation_x_offset, float view_y_offset)
{
	render_angle_offset_override = true;
	render_rotation_x_offset = rotation_x_offset;
	render_view_y_offset = view_y_offset;
}

void human::ClearRenderAngleOffset()
{
	render_angle_offset_override = false;
	render_rotation_x_offset = 0.0f;
	render_view_y_offset = 0.0f;
}

void human::StartRenderWeaponChangeMotion(int weapon_id, int frames)
{
	// 실제 장착 무기는 바꾸지 않고, 기존 무기 변경과 같은 손 내림 모션만 발생시킨다.
	MotionCtrl->ChangeHaveWeapon(weapon_id);

	if (frames < 1) {
		frames = 1;
	}

	if (selectweaponcnt < frames) {
		selectweaponcnt = frames;
	}
}

void human::StartSkillWeaponShotMotion(int weapon_id)
{
	// 스킬 전용 무기가 실제 human::ShotWeapon()을 거치지 않고 발사될 때,
	// 기존 무기 발사/수류탄 투척 팔 반동 모션만 재사용한다.
	if (hp <= 0) { return; }
	if (MotionCtrl == NULL) { return; }

	MotionCtrl->ShotWeapon(weapon_id);
}



//! @brief ?뫬궔궵궎궔뵽믦
//! @return ?뫬갌true??뫬궳궶궋갌false
//! @warning 뒶멣궸먄?궢궫륉뫴귩걏?뫬걐궴뵽믦궢귏궥갃?귢궲궋귡띍뭷궻릐궼뫮뤭궸듵귏귢궶궋궫귕갂hp <= 0 궕멣궲?뫬궴뵽믦궠귢귡궴궼뙽귟귏궧귪갃
bool human::GetDeadFlag()
{
	if( deadstate == 5 ){ return true; }
	return false;
}

//! @brief ?뻊긾?긘깈깛뭷궔궵궎궔뵽믦
//! @return 긾?긘깈깛뭷궳궇귡갌true?긾?긘깈깛뭷궳궶궋갌false
//! @attention 뫬궕?귢궲궋귡띍뭷귘갂뫉귩뒍귞귢궲궋귡륉뫴귩뵽믦궢귏궥갃뿇돷뭷궼뵽믦궢귏궧귪갃
bool human::GetDeadMotionFlag()
{
	if( deadstate == 1 ){ return true; }
	if( deadstate == 3 ){ return true; }
	return false;
}

//! @brief ???붥뜂귩먠믦걁뤵룕궖걂
//! @param id 륷궢궋???붥뜂
void human::SetTeamID(int id)
{
	teamid = id;
}

//! @brief 뼰밎긲깋긐귩롦벦
//! @return 뼰밎갌true?믅륂갌false
//! @attention 뼰밎륉뫴궻뤾뜃갂뢤뭙갋롨왮뭙궻뵚뵯갋뿇돷?궸귝귡?긽?긙귩덇먛롷궚귏궧귪갃
bool human::GetInvincibleFlag()
{
	return Invincible;
}

//! @brief 뼰밎긲깋긐귩먠믦
//! @param flag 뼰밎갌true?믅륂갌false
//! @attention 뼰밎륉뫴궻뤾뜃갂뢤뭙갋롨왮뭙궻뵚뵯갋뿇돷?궸귝귡?긽?긙귩덇먛롷궚귏궧귪갃
void human::SetInvincibleFlag(bool flag)
{
	Invincible = flag;
}

//! @brief 멟룉뿚궻댷벍쀊귩롦벦
//! @param *x X렡댷벍쀊귩롦벦궥귡?귽깛?걁NULL됀걂
//! @param *y Y렡댷벍쀊귩롦벦궥귡?귽깛?걁NULL됀걂
//! @param *z Z렡댷벍쀊귩롦벦궥귡?귽깛?걁NULL됀걂
void human::GetMovePos(float* x, float* y, float* z)
{
	if (x != NULL) { *x = move_x; }
	if (y != NULL) { *y = move_y; }
	if (z != NULL) { *z = move_z; }
}

void human::ResetVerticalMove()
{
	move_y = 0.0f;
	move_y_flag = false;
	move_y_upper = 0;
	move_y_landing = false;
}

//! @brief 긾긢깑긢??귩먠믦
//! @param upmodel 뤵뵾릆궻긾긢깑
//! @param armmodel[] 쁱궻긾긢깑궻봹쀱걁봹쀱릶갌TOTAL_ARMMODE걂
//! @param legmodel 뫉걁먄?륉뫴걂궻긾긢깑
//! @param walkmodel[] 쁱궻긾긢깑궻봹쀱걁봹쀱릶갌TOTAL_WALKMODE걂
//! @param runmodel[] 쁱궻긾긢깑궻봹쀱걁봹쀱릶갌TOTAL_RUNMODE걂
void human::SetModel(int upmodel, int armmodel[], int legmodel, int walkmodel[], int runmodel[])
{
	MotionCtrl->SetModel(upmodel, armmodel, legmodel, walkmodel, runmodel);
}

//! @brief 븧딇귩먠믦
//! @param in_weapon[] 먠믦궥귡weapon긏깋긚궻?귽깛?봹쀱
//! @param nlnbs[] 븧딇뼟룋렃렄궻몧뭙릶귩먠믦궥귡int봹쀱
//! @param nnbs[] 븧딇뼟룋렃렄궻뜃똶뭙릶귩먠믦궥귡int봹쀱
//! @warning 믅륂궼 PickupWeapon()듫릶 귩럊뾭궥귡궞궴
void human::SetWeapon(class weapon *in_weapon[], int nlnbs[], int nnbs[])
{
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		if( in_weapon[i] == NULL ){
			weapon[i] = NULL;
		}
		else{
			//븧딇귩맫궢궘뢉궑귢궽갂룋렃븧딇궴궢궲뱋?
			if( in_weapon[i]->Pickup() == 0 ){
				weapon[i] = in_weapon[i];
			}
		}
		noweapon_loadbullets[i] = nlnbs[i];
		noweapon_bullets[i] = nnbs[i];
	}
}

//! @brief 븧딇귩뢉궎
//! @param in_weapon[] 먠믦궥귡weapon긏깋긚궻?귽깛?
//! @return 맟뚻갌1?렪봲갌0
int human::PickupWeapon(class weapon *in_weapon)
{
	if( in_weapon == NULL ){ return 0; }

	if( weapon[selectweapon] == NULL ){
		//븧딇귩맫궢궘뢉궑귢궽갂룋렃븧딇궴궢궲뱋?
		if( in_weapon->Pickup() == 0 ){
			weapon[selectweapon] = in_weapon;

			//쁱궻둷뱗걁뵿벍걂귩먠믦
			class weapon *nowweapon;
			int id_param = 0;
			nowweapon = weapon[selectweapon];
			nowweapon->GetParamData(&id_param, NULL, NULL);
			MotionCtrl->PickupWeapon(id_param);

			//뼟룋렃렄궻뭙릶륃뺪궼룊딖돸
			noweapon_bullets[selectweapon] = 0;
			noweapon_loadbullets[selectweapon] = 0;

			//먛귟뫶궑뒶뿹궻긇긂깛긣
			selectweaponcnt = 10;

			// ▼▼▼ 버그 해결: 총기를 주울 때 점사 및 특수 장전 상태 초기화 ▼▼▼
			weaponburstmodecnt = 0;
			weaponshotcnt = 0;
			boltActionTimer = 0;
			boltActionTotalTime = 0;
			shell_reload_state = SHELL_RELOAD_NONE;
			// ▲▲▲ 추가 끝 ▲▲▲

			return 1;
		}
	}
	return 0;
}

//! @brief 븧딇귩먛귟뫶궑귡걁렃궭뫶궑귡걂
//! @param id 렃궭뫶궑귡븧딇?걁-1 궳렅궻븧딇걂
//! @attention 긒??뤵궔귞뮳먝뚁귂뢯궥궞궴궼뷃궚갂ObjectManager긏깋긚궔귞뚁귂뢯궢궲궘궬궠궋갃
void human::ChangeHaveWeapon(int id)
{
	// 체력이 없으면 리턴
	if (hp <= 0) { return; }

	// 일반 재장전 또는 셀 단위 재장전 중이면 리턴
	if (weaponreloadcnt > 0) { return; }
	if (shell_reload_state != SHELL_RELOAD_NONE) { return; }

	// 볼트액션 장전 중이면 리턴
	if (boltActionTimer > 0) { return; }

	// 같은 무기로 바꾸려고 하면 리턴
	if (selectweapon == id) { return; }

	// 무기 변경 중이면 리턴
	if (selectweaponcnt > 0) { return; }
	if (changeweaponidcnt > 0) { return; }

	NightVision = false;

	if (id == -1) {
		// 다음 무기 번호 선택
		selectweapon += 1;
		if (selectweapon == TOTAL_HAVEWEAPON) {
			selectweapon = 0;
		}
	}
	else {
		// 지정된 무기 번호가 범위 내인지 확인
		if ((id < 0) || (TOTAL_HAVEWEAPON <= id)) { return; }
		selectweapon = id;
	}

	// 스코프 모드 해제
	SetDisableScope();


	// 현재 선택된 무기 클래스 가져오기
	class weapon* nowweapon;
	nowweapon = weapon[selectweapon];

	int new_weapon_id = ID_WEAPON_NONE;

	// 캐릭터 모션 설정
	if (nowweapon == NULL) {	// 비무장
		MotionCtrl->ChangeHaveWeapon(ID_WEAPON_NONE);
	}
	else {
		nowweapon->GetParamData(&new_weapon_id, NULL, NULL);
		MotionCtrl->ChangeHaveWeapon(new_weapon_id);
	}

	// 무기 변경 시간은 모든 무기 공통으로 유지한다.
	selectweaponcnt = 5;

	// 점사 카운터를 초기화합니다.
	weaponburstmodecnt = 0;
	weaponshotcnt = 0;
	shell_reload_state = SHELL_RELOAD_NONE;
}

//! @brief 븧딇궻먛귟뫶궑긇긂깛긣귩롦벦
//! @return 긇긂깛긣릶?걁1댥뤵궳먛귟뫶궑뭷걂
int human::GetChangeHaveWeaponCnt()
{
	return selectweaponcnt;
}

//! @brief 븧딇궻롰쀞먛귟뫶궑
//! @return 맟뚻갌0?렪봲갌1
//! @attention 긒??뤵궔귞뮳먝뚁귂뢯궥궞궴궼뷃궚갂ObjectManager긏깋긚궔귞뚁귂뢯궢궲궘궬궠궋갃
int human::ChangeWeaponID(int change_time)
{
	if (weapon[selectweapon] == NULL) { return 1; }

	// 일반 재장전, 셀 단위 재장전, 무기 전환/모드 전환/볼트 동작 중에는 무기 모드 변경을 막는다.
	// 셀 단위 재장전은 weaponreloadcnt가 0이므로 shell_reload_state를 별도로 확인해야 한다.
	if (weaponreloadcnt > 0 ||
		shell_reload_state != SHELL_RELOAD_NONE ||
		selectweaponcnt > 0 ||
		changeweaponidcnt > 0 ||
		boltActionTimer > 0) {
		return 1;
	}

	int param_id;
	weapon[selectweapon]->GetParamData(&param_id, NULL, NULL);

	WeaponParameter ParamData;
	if (Param->GetWeapon(param_id, &ParamData) != 0) { return 1; }
	int ChangeWeaponID = ParamData.ChangeWeaponID;

	if (ChangeWeaponID == -1 || ChangeWeaponID == param_id) { return 1; }

	// [✨ 수정된 로직 ✨]
	// 1. 모드 변경 시 즉시 조준경을 해제합니다.
	SetDisableScope();

	// 2. 재장전 애니메이션을 재생합니다.
	MotionCtrl->ReloadWeapon(param_id);

	// 3. 변경할 무기 ID를 임시 변수에 저장합니다.
	changeweaponid_target = ChangeWeaponID;

	// 4. 모드 변환 시간이 지정된 경우에는 사운드 길이 기준 시간을 사용한다.
// 지정되지 않은 경우에는 기존 reloads 기반 시간을 사용한다.
	if (change_time > 0) {
		changeweaponidcnt = change_time;
	}
	else {
		changeweaponidcnt = (ParamData.reloads * 1) / 3;
		if (changeweaponidcnt < 15) {
			changeweaponidcnt = 15;
		}
	}

	changeweaponid_total_time = changeweaponidcnt;

	weaponburstmodecnt = 0;

	NightVision = false;
	CustomZoom = 1.0f;

	return 0;
}

//! @brief 븧딇궻롰쀞먛귟뫶궑긇긂깛긣귩롦벦
//! @return 긇긂깛긣릶?걁1댥뤵궳먛귟뫶궑뭷걂
int human::GetChangeWeaponIDCnt()
{
	return changeweaponidcnt;
}

int human::GetChangeWeaponIDTotalTime()
{
	return changeweaponid_total_time;
}

//! @brief 븧딇궻깏깓?긤긇긂깛긣귩롦벦
//! @return 긇긂깛긣릶?걁깏깓?긤뭷갌1댥뤵걂
int human::GetWeaponReloadCnt()
{
	return weaponreloadcnt;
}

// 현재 무기의 전체 재장전 시간을 반환하는 함수
int human::GetTotalWeaponReloadCnt()
{
	if (weapon[selectweapon] == NULL) {
		return 0;
	}

	int param_id;
	WeaponParameter ParamData;
	weapon[selectweapon]->GetParamData(&param_id, NULL, NULL);
	if (Param->GetWeapon(param_id, &ParamData) != 0) {
		return 0;
	}

	return ParamData.reloads;
}

//! @brief 븧딇귩롦벦
//! @param out_selectweapon 멗묖궠귢궲궋귡븧딇?걁0 ? [TOTAL_HAVEWEAPON]-1걂
//! @param out_weapon 롷궚롦귡weapon긏깋긚궻?귽깛?봹쀱?걁봹쀱릶갌TOTAL_HAVEWEAPON걂
//! @param nlnbs[] 븧딇뼟룋렃렄궻몧뭙릶귩롷궚롦귡int봹쀱?걁봹쀱릶갌TOTAL_HAVEWEAPON갂NULL됀걂
//! @param nnbs[] 븧딇뼟룋렃렄궻뜃똶뭙릶귩롷궚롦귡int봹쀱?걁봹쀱릶갌TOTAL_HAVEWEAPON갂NULL됀걂
void human::GetWeapon(int *out_selectweapon, class weapon *out_weapon[], int nlnbs[], int nnbs[])
{
	*out_selectweapon = selectweapon;
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		out_weapon[i] = weapon[i];
		if( nlnbs != NULL ){ nlnbs[i] = noweapon_loadbullets[i]; }
		if( nnbs != NULL ){ nnbs[i] = noweapon_bullets[i]; }
	}
}

//! @brief 뙸띪몧뷈궢궲궋귡븧딇궻롰쀞붥뜂귩롦벦
//! @return 븧딇궻롰쀞붥뜂걁0 ? TOTAL_PARAMETERINFO_WEAPON -1걂
//! @attention 뙸띪롨궸렃궯궲궋귡븧딇궻롰쀞붥뜂궳궥갃?GetWeapon()듫릶 귩뾭궋궲뮧귊귡궻궴벏뱳궳궥갃
int human::GetMainWeaponTypeNO()
{
	if( weapon[selectweapon] == NULL ){
		return ID_WEAPON_NONE;
	}

	int id_param;
	weapon[selectweapon]->GetParamData(&id_param, NULL, NULL);
	return id_param;
}

//! @brief 쁀롅먠믦귩롦벦
//! @return 쁀롅됀?갌true?쁀롅븉됀갌false
bool human::GetWeaponBlazingmode()
{
	int param_id;
	WeaponParameter data;

	//븧딇귩몧뷈궢궲궋궶궚귢궽갂렪봲
	if( weapon[selectweapon] == NULL ){ return false; }

	//몧뷈궢궲궋귡븧딇궻롰쀞붥뜂귩롦벦
	weapon[selectweapon]->GetParamData(&param_id, NULL, NULL);

	//쁀롅먠믦귩뺅궥
	Param->GetWeapon(param_id, &data);
	if( data.burstcnt == 1 ){ return false; }
	return true;
}

//! @brief 뵯뺺룉뿚
//! @param weapon_paramid 뵯뺺궢궫븧딇궻붥뜂귩롷궚롦귡?귽깛?
//! @param GunsightErrorRange 뵯뺺궢궫띧궻뤖?뚮뜼귩롷궚롦귡?귽깛?
//! @return 맟뚻갌true?렪봲갌false
//! @attention 뭙긆긳긙긃긏긣궻룉뿚귘갂뵯뺺돶궻띋맯궼빶뱑뛱궎뷠뾴궕궇귟귏궥갃
//! @attention 긒??뤵궔귞뮳먝뚁귂뢯궥궞궴궼뷃궚갂ObjectManager긏깋긚궔귞뚁귂뢯궢궲궘궬궠궋갃
bool human::ShotWeapon(int* weapon_paramid, int* GunsightErrorRange)
{
	last_shot_fail_reason = SHOT_FAIL_NONE;

	// [✨ 수정 ✨] 셸 단위 재장전 중에는 발사할 수 없습니다.
	if (shell_reload_state != SHELL_RELOAD_NONE) {
		last_shot_fail_reason = SHOT_FAIL_NOT_READY;
		return false;
	}

	if (boltActionTimer > 0) {
		last_shot_fail_reason = SHOT_FAIL_NOT_READY;
		return false;
	}
	if (weapon_paramid == NULL || GunsightErrorRange == NULL) {
		last_shot_fail_reason = SHOT_FAIL_NOT_READY;
		return false;
	}
	if (selectweaponcnt > 0 || changeweaponidcnt > 0 || weapon[selectweapon] == NULL) {
		last_shot_fail_reason = SHOT_FAIL_NOT_READY;
		return false;
	}
	if (weaponshotcnt > 0 || weaponreloadcnt > 0) {
		last_shot_fail_reason = SHOT_FAIL_NOT_READY;
		return false;
	}

	int param_id;
	weapon[selectweapon]->GetParamData(&param_id, NULL, NULL);

	// 발사 성공/실패와 관계없이 현재 무기 ID를 호출자에게 전달한다.
	// 빈 탄창 실패 시 ObjectManager 쪽에서 무기별 제외 처리를 하기 위해 필요하다.
	*weapon_paramid = param_id;

	WeaponParameter ParamData;
	ScopeParameter sparam;

	if (Param->GetWeapon(param_id, &ParamData) != 0) return false;
	if (Param->GetScopeParam(ParamData.scopemode, &sparam) != 0) {
		sparam = ScopeParameter();
	}

	// 방패는 탄약을 1발 가진 것처럼 보이게 하되, 실제 발사/공격은 절대 하지 않는다.
	// AI가 방패를 빈 무기로 판단해 버리는 문제를 막으면서, 드라이파이어/탄 소모/발사 모션도 방지한다.
	if (param_id == ID_WEAPON_SHIELD) {
		last_shot_fail_reason = SHOT_FAIL_NOT_READY;
		return false;
	}

	// 플레이어 미니건만 예열 완료 전에는 발사 불가.
// AI는 기존처럼 별도 예열 없이 바로 발사한다.
	if (ParamData.burstcnt == 6 && isPlayer == true) {
		if (IsMinigunSpinReady() == false) {
			last_shot_fail_reason = SHOT_FAIL_NOT_READY;
			return false;
		}
	}

	if (ParamData.burstcnt > 1 && ParamData.burstcnt != 6 && weaponburstmodecnt >= ParamData.burstcnt) return false;

	int shot_result = 0;

	if ((GetSkillAmmoBoostFlag() == true) && (isPlayer == true)) {
		shot_result = weapon[selectweapon]->ShotNoAmmoConsume();
	}
	else {
		shot_result = weapon[selectweapon]->Shot();
	}

	if (shot_result != 0) {
		// 실제 발사 타이밍이 되었지만 탄이 없어서 실패한 경우
		last_shot_fail_reason = SHOT_FAIL_EMPTY;

		// 점사 무기 카운터 유지
		if (ParamData.burstcnt > 1 && ParamData.burstcnt < 4) {
			weaponburstmodecnt++;
		}

		// 중요:
		// 이 값 때문에 연사총은 연사력에 맞춰 빈 탄창 소리가 반복됨
		weaponshotcnt = static_cast<int>(ParamData.blazings);

		return false;
	}

	weaponshotframe = true;

	if (ParamData.burstcnt > 1 && ParamData.burstcnt < 4) {
		weaponburstmodecnt++;
	}
	int shot_wait = static_cast<int>(ParamData.blazings);

	if ((GetSkillAmmoBoostFlag() == true) && (isPlayer == true)) {
		// 연사 속도 20% 증가 = 발사 대기시간 20% 감소
		shot_wait = (int)((float)shot_wait * 0.80f);
		if (shot_wait < 1) { shot_wait = 1; }
	}

	weaponshotcnt = shot_wait;

	float recoil_min, recoil_max;
	if (scopemode == 0) {
		float reaction = (float)ParamData.reaction;
		if (reaction < 1.0f) reaction = 1.0f;
		recoil_min = 0.05f * reaction;
		recoil_max = 0.2f * reaction;

		float hipMult = GameConfig.GetRecoilHipMultiplier();
		recoil_min *= hipMult;
		recoil_max *= hipMult;
	}
	else {
		recoil_min = sparam.WeaponRecoil_ScopeRyMin;
		recoil_max = sparam.WeaponRecoil_ScopeRyMax;
		bool is_marksman_rifle =
			(param_id == 2) ||      // PSG-1
			(param_id == 54) ||      // WA2000
			(param_id == 27) ||      // Mk14 EBR
			(param_id == 23) ||      // M1 Carbine
			(param_id == 39) ||      // SVD
			(param_id == 40) ||      // AWP
			(param_id == 59) ||      // KAR98K Sniper
			(param_id == 63) ||      // Springfield M1903
			(param_id == 36);      // Winchester1873

		if (is_marksman_rifle) {
			recoil_min *= 9.0f;
			recoil_max *= 11.0f;
		}
		else if (ParamData.burstcnt == 1 || ParamData.burstcnt == 4) {
			recoil_min *= 5.0f;
			recoil_max *= 7.0f;
		}

		float aimMult = GameConfig.GetRecoilAimMultiplier();
		recoil_min *= aimMult;
		recoil_max *= aimMult;
	}

	int ry_rnds = (int)((recoil_max - recoil_min) * 10);
	float ry_degree = 0.0f;
	if (ry_rnds > 0) {
		ry_degree = recoil_min + ((float)GetRand(ry_rnds)) / 10.0f;
	}
	else {
		ry_degree = recoil_min;
	}
	float rad_y = DegreeToRadian(ry_degree);

	// 앉기 상태에서는 새로 발생하는 반동을 30% 줄인다.
	// 1.0f = 기존 반동, 0.7f = 기존의 70%
	if (crouchflag == true) {
		rad_y *= 0.7f;
	}

	// 군인 스킬: 반동 50% 감소
	if ((GetSkillAmmoBoostFlag() == true) && (isPlayer == true)) {
		rad_y *= 0.5f;
	}

	float final_recoil = rad_y + recoil_accumulator;
	recoil_y_offset += final_recoil;

	float sideMult = GameConfig.GetRecoilSideMultiplier();

	if (sideMult < 0.0f) sideMult = 0.0f;
	if (sideMult > 1.0f) sideMult = 1.0f;

	// 연속 사격 판정 유지 시간
	// 이 시간 안에 다음 발이 나가면 "연사"로 취급
	const int SIDE_RECOIL_CHAIN_TIME = 12;

	// 첫발인지 확인
	bool applySideRecoil = false;

	if (recoil_side_timer > 0 && recoil_side_shot_count >= 1) {
		applySideRecoil = true;
	}

	// 이번 발사를 연속 사격으로 기록
	recoil_side_shot_count++;
	recoil_side_timer = SIDE_RECOIL_CHAIN_TIME;

	// 첫발에는 좌우 반동 없음, 2발째부터 좌우 반동 발생
	if (sideMult > 0.0f && applySideRecoil == true) {
		float side_dir = (GetRand(2) == 0) ? -1.0f : 1.0f;

		// 좌우 반동은 수직 반동의 일부 비율
		float rad_x = rad_y * 0.35f * sideMult * side_dir;

		recoil_x_offset += rad_x;
	}

	if (recoil_y_offset > HUMAN_RECOIL_MAX_Y) {
		recoil_y_offset = HUMAN_RECOIL_MAX_Y;
	}

	// [✨ R6S 스타일: 단발/연사 반동 예외 처리 ✨]
	if (ParamData.burstcnt == 1 || ParamData.burstcnt == 4) {
		// 단발(1) 및 저격(4) 무기: 광클 가중치를 대폭 줄이고, 쏘자마자 즉시 회복되도록 타이머를 짧게 설정
		recoil_accumulator += rad_y * 0.2f;
		float max_accum = rad_y * 1.5f;
		if (recoil_accumulator > max_accum) {
			recoil_accumulator = max_accum;
		}
		recoil_accum_timer = 1; // 1프레임 대기 후 즉각 회복 시작 (쫀득한 타격감)
	}
	else {
		// 연사 무기 (점사 포함): 기존 로직 유지 (부드럽게 쌓이고 부드럽게 대기)
		recoil_accumulator += rad_y * 0.2f;
		float max_accum = rad_y * 2.0f;
		if (recoil_accumulator > max_accum) {
			recoil_accumulator = max_accum;
		}
		recoil_accum_timer = 6; // 연사 중 덜덜거림을 잡아주는 대기 시간
	}

	*weapon_paramid = param_id;

	if (skill_aimhack_accuracy_flag == true) {
		// 에임핵 중에는 실제 탄 생성 시에도 이동/반응 탄퍼짐을 제거한다.
		*GunsightErrorRange = 0;
		StateGunsightErrorRange = 0;
		ReactionGunsightErrorRange = 0;
	}
	else {
		*GunsightErrorRange = StateGunsightErrorRange + ReactionGunsightErrorRange;
	}

	MotionCtrl->ShotWeapon(param_id);


	if (ParamData.burstcnt == 4) {
		int loadbullets_after_shot = 0;

		if (weapon[selectweapon] != NULL) {
			weapon[selectweapon]->GetParamData(NULL, &loadbullets_after_shot, NULL);
		}

		// 마지막 탄을 쏜 경우에는 볼트액션 타이머를 걸지 않는다.
		// 빈 상태 재장전 사운드에서 노리쇠 후퇴/장전 동작을 처리한다.
		if (loadbullets_after_shot > 0) {
			SetDisableScope();
			int bolt_time = static_cast<int>(ceilf(ParamData.blazings));

			boltActionTimer = bolt_time;
			boltActionTotalTime = bolt_time;
		}
		else {
			boltActionTimer = 0;
			boltActionTotalTime = 0;
		}
	}

	if (!weapon[selectweapon]->GetEnableFlag()) {
		weapon[selectweapon] = NULL;
	}

	return true;
}

//! @brief 깏깓?긤
//! @return 맟뚻갌true?렪봲갌false
//! @attention 긒??뤵궔귞뮳먝뚁귂뢯궥궞궴궼뷃궚갂ObjectManager긏깋긚궔귞뚁귂뢯궢궲궘궬궠궋갃
bool human::ReloadWeapon()
{
	// 1. 기본 상태 체크
	if (selectweaponcnt > 0) { return false; }
	if (changeweaponidcnt > 0) { return false; }
	if (boltActionTimer > 0) { return false; }
	if (weapon[selectweapon] == NULL) { return false; }

	int param_id;
	WeaponParameter ParamData;

	// 현재 탄약 정보 가져오기 (전술 재장전 판별용)
	int now_ammo = 0;
	weapon[selectweapon]->GetParamData(&param_id, &now_ammo, NULL);

	if (Param->GetWeapon(param_id, &ParamData) != 0) { return false; }

	// -------------------------------------------------------------
	// [CASE A] 셸 단위 장전 (샷건, 레버액션 등)
	// -------------------------------------------------------------
	if (ParamData.is_shell_reload == true) {

		// [✨ 중요 복구 ✨] 삭제되었던 셸 장전 초기화 로직 복구

		// 1. 이미 재장전 중이라면 중단(Interrupt) 처리 (사격 가능하게)
		if (shell_reload_state != SHELL_RELOAD_NONE) {
			InterruptShellReload();
			return true;
		}

		// 2. 다른 재장전 모션 중이면 불가
		if (weaponreloadcnt > 0) { return false; }

		// 3. 무기 클래스의 장전 시작 검사 (StartReload 호출)
		//    (weapon.cpp에서 수정한 로직에 의해 꽉 차지 않았다면 0을 리턴함)
		if (weapon[selectweapon]->StartReload() != 0) { return false; }

		// 4. 셸 장전 상태 머신 가동
		shell_reload_state = SHELL_RELOAD_START_DELAY;

		// 5. 초기 딜레이 설정 (발사 속도의 절반 정도)
		int delay_time = static_cast<int>(ParamData.blazings * 0.5f);
		if (delay_time <= 0) { delay_time = 1; }
		boltActionTimer = delay_time;
		boltActionTotalTime = delay_time;

		// 6. 1발당 장전 시간 계산
		//    (전체 재장전 시간 / 전체 장탄수)

// 셸 단위 재장전에서는 ParamData.reloads를 "1발당 장전 프레임"으로 사용한다.
		shell_reload_time_per_shell = ParamData.reloads;

		if (shell_reload_time_per_shell <= 0) {
			shell_reload_time_per_shell = 1;
		}

		// 7. 일반 재장전 카운터는 사용하지 않음
		weaponreloadcnt = 0;

		if (ParamData.SoundID_Reload != -1) {
			//PlaySound(ParamData.SoundID_Reload);
		}

		// 8. 스코프 해제 및 모션 재생
		SetDisableScope();
		MotionCtrl->ReloadWeapon(param_id);
		weaponburstmodecnt = 0;

		return true;
	}
	// -------------------------------------------------------------
	// [CASE B] 탄창 교체식 장전 (소총, 권총 등)
	// -------------------------------------------------------------
	else {
		if (weaponreloadcnt > 0) { return false; }
		if (weapon[selectweapon]->StartReload() != 0) { return false; }

		SetDisableScope();
		MotionCtrl->ReloadWeapon(param_id);

		// [전술 재장전 판별]
		// 조건: 탄이 1발 이상 남아있음(now_ammo > 0) AND 약실 기능 있음(bHasChamber)
		bool isTactical = (now_ammo > 0) && (ParamData.bHasChamber);

		if (isTactical)
		{
			// 1. 시간(모션) 단축
			if (ParamData.TacticalReloadFrame > 0) {
				weaponreloadcnt = ParamData.TacticalReloadFrame;
			}
			else {
				weaponreloadcnt = ParamData.reloads + 1;
			}

			// 2. 사운드 변경 (구현 예정 시 여기에 추가)
			if (ParamData.SoundID_Tactical != -1) {
				//PlaySound(ParamData.SoundID_Tactical);
			}
			else if (ParamData.SoundID_Reload != -1) {
				//PlaySound(ParamData.SoundID_Reload); // 전술 소리 없으면 일반 소리
			}
		}
		else
		{
			// [일반 재장전]
			weaponreloadcnt = ParamData.reloads + 1;
		}

		boltActionTotalTime = 0;
		boltActionTimer = 0;
		weaponburstmodecnt = 0;
		return true;
	}
}

bool human::InstantReloadWeapon()
{
	if (weapon[selectweapon] == NULL) {
		return false;
	}

	// 무기 교체/모드 변경 중에는 강제 장전을 건드리지 않는다.
	if (selectweaponcnt > 0) { return false; }
	if (changeweaponidcnt > 0) { return false; }

	int param_id = ID_WEAPON_NONE;
	weapon[selectweapon]->GetParamData(&param_id, NULL, NULL);

	// 이미 장전할 탄이 없거나 가득 찬 경우에는 실패해도 스킬 발동 자체는 막지 않는다.
	if (weapon[selectweapon]->StartReload() != 0) {
		return false;
	}

	if (weapon[selectweapon]->RunReload() != 0) {
		return false;
	}

	// 즉시 장전이므로 기존 장전/셸 장전/볼트액션 상태를 모두 정리한다.
	weaponreloadcnt = 0;

	shell_reload_state = SHELL_RELOAD_NONE;
	shell_reload_timer = 0;
	shell_reload_time_per_shell = 0;

	boltActionTimer = 0;
	boltActionTotalTime = 0;

	weaponburstmodecnt = 0;
	weaponshotcnt = 0;

	SetDisableScope();

	return true;
}

//! @brief 븧딇귩롆궲귡
//! @return 맟뚻갌true?렪봲갌false
//! @attention 긒??뤵궔귞뮳먝뚁귂뢯궥궞궴궼뷃궚갂ObjectManager긏깋긚궔귞뚁귂뢯궢궲궘궬궠궋갃
bool human::DumpWeapon(class Collision* CollD)
{
	// 무기 전환/모드 전환/볼트 동작 중이면 실패
	if (selectweaponcnt > 0) { return false; }
	if (changeweaponidcnt > 0) { return false; }
	if (boltActionTimer > 0) { return false; }

	// 셀 단위 재장전 중에는 무기를 버리지 못하게 한다.
	// 버리기를 허용하면 장전 상태만 남거나 탄약/모션이 꼬일 수 있다.
	if (shell_reload_state != SHELL_RELOAD_NONE) { return false; }

	//돺궔궢귞궻븧딇귩몧뷈궢궲궋귢궽?
	if (weapon[selectweapon] != NULL) {
		//깏깓?긤뭷궶귞렪봲
		if (weaponreloadcnt > 0) { return false; }

		//븧딇귩롆궲갂몧뷈귩됶룣
		// 앞으로 움직이며 버릴 때 무기가 손에서 갑자기 뒤처져 보이지 않도록
		// 플레이어 이동 속도를 일부 이어받게 한다.
		float drop_inherit_x = move_x * 0.5f;
		float drop_inherit_z = move_z * 0.5f;

		weapon[selectweapon]->Dropoff(pos_x, pos_y, pos_z, rotation_x, 1.63f, drop_inherit_x, drop_inherit_z, CollD);
		weapon[selectweapon] = NULL;

		//뼟룋렃렄궻뭙릶륃뺪궼룊딖돸
		noweapon_bullets[selectweapon] = 0;
		noweapon_loadbullets[selectweapon] = 0;

		//긚긓?긵긾?긤귩됶룣
		SetDisableScope();

		//긾?긘깈깛렳뛱
		MotionCtrl->DumpWeapon();

		NightVision = false;
		CustomZoom = 1.0f;

		// ▼▼▼ 버그 해결: 총기를 버릴 때 점사 및 특수 장전 상태 초기화 ▼▼▼
		weaponburstmodecnt = 0;
		weaponshotcnt = 0;
		shell_reload_state = SHELL_RELOAD_NONE;
		// ▲▲▲ 추가 끝 ▲▲▲

		return true;
	}

	return false;
}

//! @brief 멟릋걁몪귟걂귩먠믦
//! @attention 긒??뤵궔귞뮳먝뚁귂뢯궥궞궴궼뷃궚갂ObjectManager긏깋긚궔귞뚁귂뢯궢궲궘궬궠궋갃
void human::SetMoveForward()
{
	SetFlag(MoveFlag, MOVEFLAG_FORWARD);
}

//! @brief 뚣뫿귩먠믦
//! @attention 긒??뤵궔귞뮳먝뚁귂뢯궥궞궴궼뷃궚갂ObjectManager긏깋긚궔귞뚁귂뢯궢궲궘궬궠궋갃
void human::SetMoveBack()
{
	SetFlag(MoveFlag, MOVEFLAG_BACK);
}

//! @brief 뜺몪귟귩먠믦
//! @attention 긒??뤵궔귞뮳먝뚁귂뢯궥궞궴궼뷃궚갂ObjectManager긏깋긚궔귞뚁귂뢯궢궲궘궬궠궋갃
void human::SetMoveLeft()
{
	SetFlag(MoveFlag, MOVEFLAG_LEFT);
}

//! @brief 덭몪귟귩먠믦
//! @attention 긒??뤵궔귞뮳먝뚁귂뢯궥궞궴궼뷃궚갂ObjectManager긏깋긚궔귞뚁귂뢯궢궲궘궬궠궋갃
void human::SetMoveRight()
{
	SetFlag(MoveFlag, MOVEFLAG_RIGHT);
}

//! @brief 뺖궖귩먠믦
//! @attention 긒??뤵궔귞뮳먝뚁귂뢯궥궞궴궼뷃궚갂ObjectManager긏깋긚궔귞뚁귂뢯궢궲궘궬궠궋갃
void human::SetMoveWalk()
{
	SetFlag(MoveFlag, MOVEFLAG_WALK);
}

//! @brief 릐궻댷벍긾?긤귩롦벦
//! @param nowdata 뙸띪궻뭠귩롦벦갌true?멟긲깒??궻뭠귩럊뾭갌false
//! @return 뺖궖갌1?멟몪귟갌2?뚣귣몪귟갌3?뜺덭몪귟갌4?댷벍궢궲궶궋갌0
int human::GetMovemode(bool nowdata)
{
	//럊뾭궠귢궲궋궶궋궔갂룉뿚궠귢궲궋궶궚귢궽뢎뿹
	if( EnableFlag == false ){ return 0; }
	if( hp <= 0 ){ return 0; }

	if( nowdata == false ){	//멟궻긢??
		//뺖궖궶귞궽 1
		if( GetFlag(MoveFlag_lt, MOVEFLAG_WALK) ){
			return 1;
		}
		//멟몪귟궶귞궽 2
		if( GetFlag(MoveFlag_lt, MOVEFLAG_FORWARD) ){
			return 2;
		}
		//뚣귣몪귟궶귞궽 3
		if( GetFlag(MoveFlag_lt, MOVEFLAG_BACK) ){
			return 3;
		}
		//뜺덭몪귟궶귞궽 4
		if( GetFlag(MoveFlag_lt, (MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) ){
			return 4;
		}
	}
	else{					//뙸띪궻긢??
		//뺖궖궶귞궽 1
		if( GetFlag(MoveFlag, MOVEFLAG_WALK) ){
			return 1;
		}
		//멟몪귟궶귞궽 2
		if( GetFlag(MoveFlag, MOVEFLAG_FORWARD) ){
			return 2;
		}
		//뚣귣몪귟궶귞궽 3
		if( GetFlag(MoveFlag, MOVEFLAG_BACK) ){
			return 3;
		}
		//뜺덭몪귟궶귞궽 4
		if( GetFlag(MoveFlag, (MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) ){
			return 4;
		}
	}

	//댷벍궢궲궶궋
	return 0;
}

//! @brief 댷벍뺴뛀귩?궥긲깋긐귩롦벦
//! @return 먠믦뭠걁Human_MoveFlag쀱땽?걂
//! @attention 긢긫긞긏뾭듫릶궳궥갃믅륂궼GetMovemode()듫릶귩럊뾭궢궲궘궬궠궋갃
int human::GetMoveFlag(bool nowdata)
{
	if( nowdata == false ){	//멟궻긢??
		return MoveFlag_lt;
	}
	else{					//뙸띪궻긢??
		return MoveFlag;
	}
}

//! @brief 긚긓?긵귩먠믦
//! @return 맟뚻갌true?렪봲갌false
bool human::SetEnableScope()
{
	int param_id;
	WeaponParameter data;

	//돺귖븧딇귩몧뷈궢궲궶궚귢궽렪봲
	if( weapon[selectweapon] == NULL ){ return false; }

	//깏깓?긤뭷궶귞렪봲
	if (weaponreloadcnt > 0) { return false; }

	if (changeweaponidcnt > 0) { return false; }
	if (boltActionTimer > 0) { return false; }

	//븧딇궻롰쀞붥뜂귩롦벦
	weapon[selectweapon]->GetParamData(&param_id, NULL, NULL);

	//븧딇궻먠믦뭠귩롦벦
	Param->GetWeapon(param_id, &data);

	//긚긓?긵귩먠믦
	scopemode = data.scopemode;
	return true;
}

//! @brief 긚긓?긵귩됶룣
void human::SetDisableScope()
{
	scopemode = 0;
}

//! @brief 긚긓?긵먠믦귩롦벦
int human::GetScopeMode()
{
	return scopemode;
}

int human::GetHitEffectTimer()
{
	return hit_effect_timer;
}

void human::SetHitEffectTimer(int t)
{
	hit_effect_timer = t;
}

void human::DecreaseHitEffectTimer()
{
	if (hit_effect_timer > 0) {
		hit_effect_timer--;
	}
}

bool human::IsShieldHitEffect()
{
	return shield_hit_effect_flag;
}


//! @brief 돘렡궴뢢렡궻뛀궖귩롦벦
//! @param rx 돘렡귩롦벦궥귡?귽깛?
//! @param ry 뢢렡귩롦벦궥귡?귽깛?
void human::GetRxRy(float* rx, float* ry)
{
	if ((rx == NULL) || (ry == NULL)) { return; }

	*rx = rotation_x;
	// 기본 시야각에 반동 오프셋을 더한 최종 값을 전달
	*ry = armrotation_y + recoil_y_offset;
}

//! @brief 돘렡궴뢢렡궻뛀궖귩먠믦
//! @param rx 먠믦궥귡돘렡
//! @param ry 먠믦궥귡뢢렡
void human::SetRxRy(float rx, float ry)
{
	rotation_x = rx;
	armrotation_y = ry;
}

//! @brief 멣뫬궻됷?둷뱗롦벦
//! @return 뢢렡귩롦벦궥귡?귽깛?
//! @warning ?뻊궢궲?귢귡띧궻둷뱗궳궥갃GetRxRy()듫릶궳롷궚롦귡뭠궴궼댶궶귟귏궥갃
float human::GetDeadRy()
{
	return rotation_y;
}

//! @brief 긙긿깛긵
//! @attention 긒??뤵궔귞뮳먝뚁귂뢯궥궞궴궼뷃궚갂ObjectManager긏깋긚궔귞뚁귂뢯궢궲궘궬궠궋갃
void human::Jump()
{
	SetFlag(MoveFlag, MOVEFLAG_JUMP);
}

//! @brief 긙긿깛긵갋뭶뭤궻롦벦
//! @param nowdata 뙸띪궻뭠귩롦벦갌true?멟긲깒??궻뭠귩럊뾭갌false
//! @return 긙긿깛긵갌1?뭶뭤갌2?궩귢댥둖갌0
//! @attention 뭶뭤궻뵽믦궼륂궸뙸띪궻뭠귩롦벦궢귏궥갃멟긲깒??궻륉뫴궼롦벦궢귏궧귪갃
//! @note 뫉돶띋맯뵽믦뾭
int human::GetJumpLanding(bool nowdata)
{
	//럊뾭궠귢궲궋궶궋궔갂룉뿚궠귢궲궋궶궚귢궽뢎뿹
	if( EnableFlag == false ){ return 0; }
	if( hp <= 0 ){ return 0; }

	if( nowdata == false ){	//멟궻긢??
		if( move_y_landing == true ){ return 2; }
		if( GetFlag(MoveFlag_lt, MOVEFLAG_JUMP) ){ return 1; }
	}
	else{
		if( move_y_landing == true ){ return 2; }
		if( GetFlag(MoveFlag, MOVEFLAG_JUMP) ){ return 1; }
	}

	//긙긿깛긵궢궲궋궶궋
	return 0;
}

//! @brief 뫉뙰궻긳깓긞긏ID갋뽋붥뜂귩롦벦
//! @param id 뫉뙰궻긳깓긞긏ID귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param face 뫉뙰궻긳깓긞긏궻뽋붥뜂귩롷궚롦귡?귽깛?걁NULL됀걂
//! @attention 뗴뭷궻뤾뜃궶궵뫉뙰궸긳깓긞긏궕궶궋뤾뜃갂긳깓긞긏ID궴뽋붥뜂궼 -1 귩뺅궢귏궥갃
void human::GetUnderBlock(int *id, int *face)
{
	if( id != NULL ){ *id = underblock_id; }
	if( face != NULL ){ *face = underblock_face; }
}

//! @brief 돓궢궬궥갋쀍귩돿궑귡
//! @param rx 돘렡
//! @param ry 뢢렡
//! @param speed 뫊뱗
void human::AddPosOrder(float rx, float ry, float speed)
{
	move_x += cosf(rx) * cosf(ry) * speed;
	move_y += sinf(ry) * speed;
	move_z += sinf(rx) * cosf(ry) * speed;
}

//! @brief 뭙궕 벆 궸긭긞긣
//! @param attacks 뭙궻뛘똼쀍
void human::HitBulletHead(int attacks)
{
	if (Invincible == false) {
		float damage = (float)((int)((float)attacks * HUMAN_DAMAGE_HEAD) + GetRand(16));
		if (isPlayer) { // 플레이어인 경우에만 피해 배율 적용
			damage *= GameConfig.GetDamageMultiplier();
		}

		int final_damage = ApplySkillAbsorbDamage((int)damage);
		hp -= final_damage;
	}
	ReactionGunsightErrorRange = 15;
}

//! @brief 뭙궕 뤵뵾릆 궸긭긞긣
//! @param attacks 뭙궻뛘똼쀍
void human::HitBulletUp(int attacks)
{
	if (Invincible == false) {
		float damage = (float)((int)((float)attacks * HUMAN_DAMAGE_UP) + GetRand(8));
		if (isPlayer) {
			damage *= GameConfig.GetDamageMultiplier();
		}

		int final_damage = ApplySkillAbsorbDamage((int)damage);
		hp -= final_damage;
	}
	ReactionGunsightErrorRange = 12;
}

//! @brief 뭙궕 돷뵾릆 궸긭긞긣
//! @param attacks 뭙궻뛘똼쀍
void human::HitBulletLeg(int attacks)
{
	if (Invincible == false) {
		float damage = (float)((int)((float)attacks * HUMAN_DAMAGE_LEG) + GetRand(6));
		if (isPlayer) {
			damage *= GameConfig.GetDamageMultiplier();
		}

		int final_damage = ApplySkillAbsorbDamage((int)damage);
		hp -= final_damage;
	}
	ReactionGunsightErrorRange = 8;
}

//! @brief ?깛긮궻뛘똼궕긭긞긣
void human::HitZombieAttack()
{
	if (Invincible == false) {
		float damage = (float)(HUMAN_DAMAGE_ZOMBIEU + GetRand(HUMAN_DAMAGE_ZOMBIEA));

		int final_damage = ApplySkillAbsorbDamage((int)damage);
		hp -= final_damage;
	}
	ReactionGunsightErrorRange = 10;
}

//! @brief 롨왮뭙궻뵚븮궕긭긞긣
//! @param attacks 뵚븮궻뛘똼쀍
//! @attention 떁뿣궸귝귡똶럁귩럷멟궸띙귏궧궲궘궬궠궋갃
void human::HitGrenadeExplosion(int attacks)
{
	if (Invincible == false) {
		float damage = (float)attacks;

		int final_damage = ApplySkillAbsorbDamage((int)damage);
		hp -= final_damage;
	}
	ReactionGunsightErrorRange = 10;
}

//! @brief 뷀뭙긲깋긐귩긜긞긣
//! @param rx 뷀뭙궢궫뺴뛀
void human::SetHitFlag(float rx)
{
	HitFlag = true;
	Hit_rx = rx;
	hit_effect_timer = 10;  // 10 프레임 유지
	shield_hit_effect_flag = false;
	silent_hit_flag = false;
}

// 소음기 총에 의한 피격.
// 일반 피격 효과는 유지하되, 피해 방향 원호 표시만 생략하기 위한 플래그를 남긴다.
void human::SetSilentHitFlag(float rx)
{
	HitFlag = true;
	Hit_rx = rx;
	hit_effect_timer = 10;
	shield_hit_effect_flag = false;
	silent_hit_flag = true;
}

// 방패로 막은 피격.

// 방패로 막은 피격.
// 일반 빨강 피격 효과 대신 하양 피격 효과를 쓰기 위한 플래그를 남긴다.
void human::SetShieldHitFlag(float rx)
{
	HitFlag = true;
	Hit_rx = rx;
	hit_effect_timer = 10;
	shield_hit_effect_flag = true;
	silent_hit_flag = false;
}

//! @brief 뷀뭙궢궫궔?긃긞긏궥귡
//! @param rx 뷀뭙궢궫뺴뛀귩롷궚롦귡?귽깛?걁NULL됀걂
//! @return 뷀뭙궢궫갌true?뷀뭙궢궲궶궋갌false
//! @attention 렳뛱궥귡궴갂긲깋긐궼 false 궸룊딖돸궠귢귏궥갃
bool human::CheckHit(float* rx, bool* shield_hit, bool* silent_hit)
{
	bool returnflag = HitFlag;

	if (rx != NULL) { *rx = Hit_rx; }

	if (shield_hit != NULL) {
		*shield_hit = shield_hit_effect_flag;
	}

	if (silent_hit != NULL) {
		*silent_hit = silent_hit_flag;
	}

	HitFlag = false;
	//Hit_rx = 0.0f;
	return returnflag;
}

//! @brief 뜃똶댷벍쀊귩롦벦
//! @return 뜃똶댷벍쀊
float human::GetTotalMove()
{
	return totalmove;
}

//! @brief 뺖궖갋몪귟긾?긘깈깛궻긇긂깛긣뭠롦벦
//! @return 긇긂깛긣뭠
int human::GetMoveMotionCount()
{
	return MotionCtrl->GetMoveMotionCnt();
}

//! @brief 뤖?궻륉뫴뚮뜼궻룉뿚
//!	@attention ControlProcess()귝귟멟궳룉뿚궥귡궞궴
void human::GunsightErrorRange()
{
	//룊딖돸
	StateGunsightErrorRange = 0;

	// 로봇 F: 에임핵 중에는 이동/점프/피격 반응에 의한 탄퍼짐을 제거한다.
	if (skill_aimhack_accuracy_flag == true) {
		ReactionGunsightErrorRange = 0;
		return;
	}

	//둫?띿궸귝귡뚮뜼귩먠믦
	if (GetFlag(MoveFlag, MOVEFLAG_WALK)) {
		StateGunsightErrorRange = 4;
	}
	if( GetFlag(MoveFlag, MOVEFLAG_FORWARD) ){
		StateGunsightErrorRange = 8;
	}
	if( GetFlag(MoveFlag, MOVEFLAG_BACK) ){
		StateGunsightErrorRange = 6;
	}
	if( GetFlag(MoveFlag, (MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) ){
		StateGunsightErrorRange = 7;
	}
	if( move_y_flag == true ){
		StateGunsightErrorRange = 22;
	}
	if( hp < 40 ){
		StateGunsightErrorRange += 3;
	}

	//돺궔븧딇귩몧뷈궢궲궋귢궽
	if( weapon[selectweapon] != NULL ){
		//븧딇궻먠믦긢??귩롦벦
		int param;
		WeaponParameter data;
		weapon[selectweapon]->GetParamData(&param, NULL, NULL);
		Param->GetWeapon(param, &data);

		//뚮뜼귩 1 뙵귞궥
		ReactionGunsightErrorRange -= 1;

		//뚮뜼궻붝댪귩뺚맫
		if( ReactionGunsightErrorRange < 0 ){ ReactionGunsightErrorRange = 0; }
		if( ReactionGunsightErrorRange > data.ErrorRangeMAX ){ ReactionGunsightErrorRange = data.ErrorRangeMAX; }
	}
	else{
		ReactionGunsightErrorRange = 0;
	}
}

int human::GetBurstModeCnt()
{
	return weaponburstmodecnt;
}

void human::ResetBurstModeCnt()
{
	int main_weapon_id = GetMainWeaponTypeNO();
	if (main_weapon_id != ID_WEAPON_NONE) {
		WeaponParameter param;
		if (Param->GetWeapon(main_weapon_id, &param) == 0) {
			// 점사가 모두 끝났을 때만 카운터를 0으로 초기화합니다.
			if (weaponburstmodecnt >= param.burstcnt) {
				weaponburstmodecnt = 0;
			}
		}
	}
	else {
		weaponburstmodecnt = 0;
	}
}

//! @brief ?뻊뵽믦궴?귢귡룉뿚
//! @return 먄?궢궫?뫬갌4??귢뢎귦궯궫뮳뚣갌3??귢궲궋귡띍뭷갌2??귢럑귕귡갌1?돺귖궢궶궋갌0
int human::CheckAndProcessDead(class Collision *CollD)
{
	if( CollD == NULL ){ return 0; }

	if( hp <= 0 ){
		//쁱궻둷뱗
		if( armrotation_y < 0.0f ){
			armrotation_y -= DegreeToRadian(6);
			if( armrotation_y < DegreeToRadian(-90) ){ armrotation_y = DegreeToRadian(-90); }
		}
		else{
			armrotation_y += DegreeToRadian(6);
			if( armrotation_y > DegreeToRadian(90) ){ armrotation_y = DegreeToRadian(90); }
		}
	}

#ifdef HUMAN_DEADBODY_COLLISION

	//긽긾갌
	//
	//륉뫴갌0
	//걅돺귖궢궶궋걆
	//?HP궕0댥돷궳갂벆궕븊궔궹궶귞?륉뫴갌1
	//?HP궕0댥돷궳갂빮궸벆궕븊궘궶귞?륉뫴갌2
	//
	//륉뫴갌1
	//걅똛궖럑귕귡걆
	//?135뱗뛱궯궫귞?륉뫴갌2
	//?빮궸벆귩뫥궯궫귞?륉뫴갌3
	//
	//륉뫴갌2
	//걅뿇돷걆
	//?뫉궕뭤뽋궸븊궋궫귞?륉뫴갌4
	//
	//륉뫴갌3
	//걅뫉귩뒍귞궧귡걆
	//??
	//륉뫴갌4
	//걅뚂믦걆

	float check_posx, check_posy, check_posz;

	if( deadstate == 0 ){
		if( hp <= 0 ){		//HP궕 0 댥돷궸궶궯궫걁?뻊궢궫걂뢷듩궶귞갂?궢럑귕귡
			float tr;

			//띍뚣궸뛘똼귩롷궚궫뺴뛀귩똶럁
			tr = Hit_rx - rotation_x;
			tr = AngleNormalization(tr);

			//뫬궻둷뱗
			if( ((float)M_PI/2*-1 < tr)&&(tr < (float)M_PI/2) ){
				add_ry = HUMAN_DEADADDRY;
			}
			else{
				add_ry = HUMAN_DEADADDRY * -1;
			}

			//?뫬궕뻹귏귞궹귝궎갂뛼궠귩 +1.0 궥귡
			pos_y += 1.0f;

			//룋렃궢궲궋귡븧딇귩멣궲롆궲귡
			for(int i=0; i<TOTAL_HAVEWEAPON; i++){
				if( weapon[i] != NULL ){
					weapon[i]->Dropoff(pos_x, pos_y, pos_z, DegreeToRadian(10) * GetRand(36), 1.5f, 0.0f, 0.0f, CollD);
					weapon[i] = NULL;

					//뼟룋렃렄궻뭙릶륃뺪궼룊딖돸
					noweapon_bullets[i] = 0;
					noweapon_loadbullets[i] = 0;
				}
			}

			//긚긓?긵긾?긤귩됶룣
			SetDisableScope();

			//둫롰긇긂깛긣룊딖돸
			selectweaponcnt = 0;
			changeweaponidcnt = 0;
			weaponshotcnt = 0;
			weaponreloadcnt = 0;

			//렅궻긲깒??궻벆궻띆뷭귩롦벦
			check_posx = pos_x + cosf(rotation_x*-1 - (float)M_PI/2) * sinf(add_ry) * HUMAN_HEIGHT;
			check_posy = pos_y + cosf(add_ry) * HUMAN_HEIGHT;
			check_posz = pos_z + sinf(rotation_x*-1 - (float)M_PI/2) * sinf(add_ry) * HUMAN_HEIGHT;

			if( CollD->CheckALLBlockInside(check_posx, check_posy, check_posz) == true ){
				deadstate = 2;
			}
			else{
				rotation_y += add_ry;
				deadstate = 1;
			}
			return 1;
		}
		return 0;
	}

	if( deadstate == 1 ){
		//135뱗댥뤵?귢궲궋귢궽
		if( fabsf(rotation_y) >= DegreeToRadian(135) ){
			deadstate = 2;
			move_y = 0.0f;
			return 2;
		}

		if( pos_y <= (HUMAN_DEADLINE + 10.0f) ){
			//90뱗댥뤵?귢궲궋귢궽
			if( fabsf(rotation_y) >= DegreeToRadian(90) ){
				deadstate = 4;
				return 2;
			}
		}

		//멟뚣궸?궥
		if( rotation_y > 0.0f ){		//rotation_y < DegreeToRadian(135)
			add_ry += HUMAN_DEADADDRY;
		}
		else if( rotation_y < 0.0f ){	//rotation_y > DegreeToRadian(-135)
			add_ry -= HUMAN_DEADADDRY;
		}

		if( pos_y <= HUMAN_DEADLINE ){
			rotation_y += add_ry;
		}
		else{
			//렅궻긲깒??궻벆궻띆뷭귩롦벦
			check_posx = pos_x + cosf(rotation_x*-1 - (float)M_PI/2) * sinf(rotation_y + add_ry) * HUMAN_HEIGHT;
			check_posy = pos_y + cosf(rotation_y + add_ry) * HUMAN_HEIGHT;
			check_posz = pos_z + sinf(rotation_x*-1 - (float)M_PI/2) * sinf(rotation_y + add_ry) * HUMAN_HEIGHT;

			if( CollD->CheckALLBlockInside(check_posx, check_posy, check_posz) == true ){
				deadstate = 3;
			}
			else{
				rotation_y += add_ry;
			}
		}

		return 2;
	}

	if( deadstate == 2 ){
		if( pos_y <= HUMAN_DEADLINE ){
			deadstate = 4;
			return 2;
		}

		if( move_y > -4.0f ){
			move_y -= 0.1f;
		}

		//렅궻긲깒??궻뫉궻띆뷭
		check_posx = pos_x;
		check_posy = pos_y - move_y - HUMAN_HEIGHT*0.5f;
		check_posz = pos_z;

		if( CollD->CheckALLBlockInside(check_posx, check_posy, check_posz) == true ){
			deadstate = 4;
		}
		else{
			pos_y += move_y;
		}

		return 2;
	}

	if( deadstate == 3 ){
		//deadstate = 4;

		//90뱗댥뤵?귢궲궋귢궽
		if( fabsf(rotation_y) >= DegreeToRadian(90) ){
			deadstate = 4;
			return 2;
		}

		//멟뚣궸?궥
		if( rotation_y > 0.0f ){		//rotation_y < DegreeToRadian(90)
			add_ry += HUMAN_DEADADDRY;
		}
		else if( rotation_y < 0.0f ){	//rotation_y > DegreeToRadian(-90)
			add_ry -= HUMAN_DEADADDRY;
		}

		//렅궻긲깒??궻뫉궻띆뷭귩롦벦
		check_posx = pos_x - cosf(rotation_x*-1 - (float)M_PI/2) * sinf(rotation_y + add_ry) * HUMAN_HEIGHT;
		check_posy = pos_y + 0.1f;
		check_posz = pos_z - sinf(rotation_x*-1 - (float)M_PI/2) * sinf(rotation_y + add_ry) * HUMAN_HEIGHT;

		if( CollD->CheckALLBlockInside(check_posx, check_posy, check_posz) == true ){
			deadstate = 4;
			return 2;
		}

		//렅궻긲깒??궻벆궻띆뷭귩롦벦
		check_posx = pos_x - cosf(rotation_x*-1 - (float)M_PI/2) * sinf(rotation_y + add_ry) * HUMAN_HEIGHT;
		check_posy = pos_y + cosf(rotation_y + add_ry) * HUMAN_HEIGHT;
		check_posz = pos_z - sinf(rotation_x*-1 - (float)M_PI/2) * sinf(rotation_y + add_ry) * HUMAN_HEIGHT;

		if( CollD->CheckALLBlockInside(check_posx, check_posy, check_posz) == true ){
			deadstate = 4;
			return 2;
		}

		//뫉궻띆뷭귩댷벍
		pos_x -= cosf(rotation_x*-1 - (float)M_PI/2) * sinf(add_ry) * HUMAN_HEIGHT;
		pos_z -= sinf(rotation_x*-1 - (float)M_PI/2) * sinf(add_ry) * HUMAN_HEIGHT;

		rotation_y += add_ry;
		return 2;
	}

	if( deadstate == 4 ){
		//돺귖궢궶궋걁뚂믦걂
		deadstate = 5;
		return 3;
	}

	if( deadstate == 5 ){
		//돺귖궢궶궋걁뚂믦걂
		return 4;
	}

	return 0;
#else
	if( fabsf(rotation_y) >= DegreeToRadian(90) ){
		return 4;
	}
	else if( rotation_y > 0.0f ){		//?귢럑귕궲궋귢궽갂궩궻귏귏?귢귡갃
		add_ry += HUMAN_DEADADDRY;
		rotation_y += add_ry;
		if( rotation_y >= DegreeToRadian(90) ){
			rotation_y = DegreeToRadian(90);
			deadstate = 5;
			return 3;
		}
		return 2;
	}
	else if( rotation_y < 0.0f ){	//?귢럑귕궲궋귢궽갂궩궻귏귏?귢귡갃
		add_ry -= HUMAN_DEADADDRY;
		rotation_y += add_ry;
		if( rotation_y <= DegreeToRadian(-90) ){
			rotation_y = DegreeToRadian(-90);
			deadstate = 5;
			return 3;
		}
		return 2;
	}
	else if( hp <= 0 ){		//HP궕 0 댥돷궸궶궯궫걁?뻊궢궫걂뢷듩궶귞갂?궢럑귕귡
		float tr;

		//띍뚣궸뛘똼귩롷궚궫뺴뛀귩똶럁
		tr = Hit_rx - rotation_x;
		tr = AngleNormalization(tr);

		//뫬궻둷뱗
		if( ((float)M_PI/2*-1 < tr)&&(tr < (float)M_PI/2) ){
			add_ry = HUMAN_DEADADDRY;
		}
		else{
			add_ry = HUMAN_DEADADDRY * -1;
		}

		rotation_y += add_ry;

		//?뫬궕뻹귏귞궹귝궎갂뛼궠귩 +1.0 궥귡
		pos_y += 1.0f;

		//룋렃궢궲궋귡븧딇귩멣궲롆궲귡
		for(int i=0; i<TOTAL_HAVEWEAPON; i++){
			if( weapon[i] != NULL ){
				weapon[i]->Dropoff(pos_x, pos_y, pos_z, DegreeToRadian(10)*GetRand(36), 1.5f);
				weapon[i] = NULL;

				//뼟룋렃렄궻뭙릶륃뺪궼룊딖돸
				noweapon_bullets[i] = 0;
				noweapon_loadbullets[i] = 0;
			}
		}

		//긚긓?긵긾?긤귩됶룣
		SetDisableScope();

		deadstate = 1;
		return 1;
	}

	return 0;
#endif
}

//! @brief ?띿궸귝귡댷벍똶럁
//! @attention 렳뛱궥귡궴갂둫긌?긲깋긐궼 false 궸룊딖돸궠귢귏궥갃
void human::ControlProcess()
{
	// (기존 이동 처리 코드는 동일)
	if (GetFlag(MoveFlag, MOVEFLAG_WALK)) {
		move_rx = DegreeToRadian(0);
		AddPosOrder(rotation_x * -1 + move_rx + (float)M_PI / 2, 0.0f, HUMAN_PROGRESSWALK_ACCELERATION * SpeedMultiplier);
	}
	else if (GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_FORWARD) {
		move_rx = DegreeToRadian(0);
		AddPosOrder(rotation_x * -1 + move_rx + (float)M_PI / 2, 0.0f, HUMAN_PROGRESSRUN_ACCELERATION * SpeedMultiplier);
	}
	else if (GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_BACK) {
		move_rx = DegreeToRadian(180);
		AddPosOrder(rotation_x * -1 + move_rx + (float)M_PI / 2, 0.0f, HUMAN_REGRESSRUN_ACCELERATION * SpeedMultiplier);
	}
	else if (GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_LEFT) {
		move_rx = DegreeToRadian(90);
		AddPosOrder(rotation_x * -1 + move_rx + (float)M_PI / 2, 0.0f, HUMAN_SIDEWAYSRUN_ACCELERATION * SpeedMultiplier);
	}
	else if (GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_RIGHT) {
		move_rx = DegreeToRadian(-90);
		AddPosOrder(rotation_x * -1 + move_rx + (float)M_PI / 2, 0.0f, HUMAN_SIDEWAYSRUN_ACCELERATION * SpeedMultiplier);
	}
	else if (GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_FORWARD | MOVEFLAG_LEFT)) {
		move_rx = DegreeToRadian(45);
		AddPosOrder(rotation_x * -1 + move_rx + (float)M_PI / 2, 0.0f, HUMAN_PROGRESSRUN_SIDEWAYSRUN_ACCELERATION * SpeedMultiplier);
	}
	else if (GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_BACK | MOVEFLAG_LEFT)) {
		move_rx = DegreeToRadian(135);
		AddPosOrder(rotation_x * -1 + move_rx + (float)M_PI / 2, 0.0f, HUMAN_REGRESSRUN_SIDEWAYSRUN_ACCELERATION * SpeedMultiplier);
	}
	else if (GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_BACK | MOVEFLAG_RIGHT)) {
		move_rx = DegreeToRadian(-135);
		AddPosOrder(rotation_x * -1 + move_rx + (float)M_PI / 2, 0.0f, HUMAN_REGRESSRUN_SIDEWAYSRUN_ACCELERATION * SpeedMultiplier);
	}
	else if (GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_FORWARD | MOVEFLAG_RIGHT)) {
		move_rx = DegreeToRadian(-45);
		AddPosOrder(rotation_x * -1 + move_rx + (float)M_PI / 2, 0.0f, HUMAN_PROGRESSRUN_SIDEWAYSRUN_ACCELERATION * SpeedMultiplier);
	}
	else {
		move_rx = 0.0f;
	}

	MoveFlag_lt = MoveFlag;
	MoveFlag = 0x00;

	// [✨ 수정된 반동 회복 로직 ✨]
	// 1. 타이머 감소 (사격 직후 일정 프레임 동안 회복 대기)
	if (recoil_accum_timer > 0) {
		recoil_accum_timer--;
	}
	// 2. 타이머가 끝났을 때(연사 중지) 반동 풀과 누적치 회복
	else {
		// 에임을 부드럽게 원위치시킵니다.
		if (recoil_y_offset > 0.0f) {
			recoil_y_offset *= HUMAN_RECOIL_RECOVERY_RATE; // 1.0에 가까울수록 천천히 내려오고, 낮을수록 팍 내려옵니다.

			if (recoil_y_offset < 0.001f) {
				recoil_y_offset = 0.0f;
			}
		}
		else {
			recoil_y_offset = 0.0f;
		}

		if (recoil_side_timer > 0) {
			recoil_side_timer--;
		}
		else {
			recoil_side_shot_count = 0;
		}

		if (recoil_x_offset > 0.0f || recoil_x_offset < 0.0f) {
			recoil_x_offset *= HUMAN_RECOIL_RECOVERY_RATE;

			if (recoil_x_offset < 0.001f && recoil_x_offset > -0.001f) {
				recoil_x_offset = 0.0f;
			}
		}

		// 연사 시 더해졌던 가중치(accumulator)도 서서히 초기화합니다.
		if (recoil_accumulator > 0.0f) {
			recoil_accumulator *= 0.95f;
			if (recoil_accumulator < 0.001f) {
				recoil_accumulator = 0.0f;
			}
		}
	}
}

//! @brief ?긞긵궴궻뱰궫귟뵽믦
//! @param CollD Collision긏깋긚궻?귽깛?
//! @param inblockdata BlockDataInterface긏깋긚궻?귽깛?
//! @param AddCollisionFlag 믁돿궻뱰궫귟뵽믦긲깋긐
//! @param player 뫮뤭궻릐븿궕긵깒귽깂?궔궵궎궔
void human::CollisionMap(class Collision *CollD, class BlockDataInterface *inblockdata, bool AddCollisionFlag, bool player)
{
	if( CollD == NULL ){ return; }
	if( inblockdata == NULL ){ return; }

	float pos_x2, pos_y2, pos_z2;
	float dist_x, dist_y, dist_z;
	float box_min_x, box_min_y, box_min_z, box_max_x, box_max_y, box_max_z;
	bool CheckBlockID[MAX_BLOCKS];
	float speed;
	float Dist;
	bool FallFlag;

	//긳깓긞긏ID궴뽋붥뜂궼갂궿궴귏궦 -1 궸궥귡갃
	underblock_id = -1;
	underblock_face = -1;

	//뙸렄?궻띆뷭귩긫긞긏귺긞긵
	pos_x2 = pos_x;
	pos_y2 = pos_y;
	pos_z2 = pos_z;

	//댷벍뵿뎕
	pos_x += move_x;
	pos_z += move_z;

	//렅긲깒??궻댷벍쀊귩뙵릨
	move_x *= HUMAN_ATTENUATION;
	move_z *= HUMAN_ATTENUATION;

	//겍?됄XOPS궳궼궞궞궳릐벏럐궻뱰궫귟뵽믦귩뛱궎갃
	//?OpenXOPS궸궓궋궲궼빶?귽?깛긐궳똶럁궥귡궫귕갂댥돷dist_y궼 0.0f 둴믦궳궇귟뭠궴궢궲궼뼰댰뼞갃

	//댷벍긹긏긣깑럁뢯
	dist_x = pos_x - pos_x2;
	dist_y = pos_y - pos_y2;
	dist_z = pos_z - pos_z2;
	speed = sqrtf(dist_x*dist_x + dist_z*dist_z);
	VectorNormalization(&dist_x, &dist_y, &dist_z);

	int bs = inblockdata->GetTotaldatas();

	//릐궻AABB귩띿맟
	box_min_x = pos_x - HUMAN_MAPCOLLISION_CHECK_MAXDIST - COLLISION_ADDSIZE;
	box_min_y = pos_y - HUMAN_MAPCOLLISION_CHECK_MAXDIST - COLLISION_ADDSIZE;
	box_min_z = pos_z - HUMAN_MAPCOLLISION_CHECK_MAXDIST - COLLISION_ADDSIZE;
	box_max_x = pos_x + HUMAN_MAPCOLLISION_CHECK_MAXDIST + COLLISION_ADDSIZE;
	box_max_y = (pos_y + HUMAN_HEIGHT) + HUMAN_MAPCOLLISION_CHECK_MAXDIST + COLLISION_ADDSIZE;
	box_max_z = pos_z + HUMAN_MAPCOLLISION_CHECK_MAXDIST + COLLISION_ADDSIZE;

	//AABB뵽믦궳뫮뤭긳깓긞긏귩멷랁귟궥귡
	for(int i=0; i<bs; i++){
		CheckBlockID[i] = CollD->CheckBlockAABB(i, box_min_x, box_min_y, box_min_z, box_max_x, box_max_y, box_max_z);
	}

	if( (speed != 0.0f)||(move_y != 0.0f) ){

		//벆븫궳뱰궫귟뵽믦
		for(int i=0; i<bs; i++){
			if( CheckBlockID[i] == true ){
				if( CollisionBlockScratch(CollD, inblockdata, i, &pos_x, &pos_y, &pos_z, pos_x2, pos_y2, pos_z2, pos_x, pos_y + HUMAN_HEIGHT-0.22f, pos_z, 0x01) == true ){
					if( move_y > 0.0f ){ move_y = 0.0f; }
				}
			}
		}

		//뫉뙰궳뱰궫귟뵽믦
		for(int i=0; i<bs; i++){
			if( CheckBlockID[i] == true ){
				CollisionBlockScratch(CollD, inblockdata, i, &pos_x, &pos_y, &pos_z, pos_x2, pos_y2, pos_z2, pos_x, pos_y, pos_z, 0x00);
			}
		}

		//뜕궘귞궋궳뱰궫귟뵽믦
		for(int i=0; i<bs; i++){
			if( CheckBlockID[i] == true ){
				CollisionBlockScratch(CollD, inblockdata, i, &pos_x, &pos_y, &pos_z, pos_x2, pos_y2, pos_z2, pos_x + dist_x*HUMAN_MAPCOLLISION_R, pos_y + HUMAN_MAPCOLLISION_HEIGHT, pos_z + dist_z*HUMAN_MAPCOLLISION_R, 0x02);
				CollisionBlockScratch(CollD, inblockdata, i, &pos_x, &pos_y, &pos_z, pos_x2, pos_y2, pos_z2, pos_x + dist_z*HUMAN_MAPCOLLISION_R, pos_y + HUMAN_MAPCOLLISION_HEIGHT, pos_z + dist_x*HUMAN_MAPCOLLISION_R, 0x02);
				CollisionBlockScratch(CollD, inblockdata, i, &pos_x, &pos_y, &pos_z, pos_x2, pos_y2, pos_z2, pos_x - dist_z*HUMAN_MAPCOLLISION_R, pos_y + HUMAN_MAPCOLLISION_HEIGHT, pos_z - dist_x*HUMAN_MAPCOLLISION_R, 0x02);
			}
		}

		if( AddCollisionFlag == true ){
			//뜕븊뗟궻믁돿뱰궫귟뵽믦
			for(int i=0; i<bs; i++){
				if( CheckBlockID[i] == true ){
					CollisionBlockScratch(CollD, inblockdata, i, &pos_x, &pos_y, &pos_z, pos_x2, pos_y2, pos_z2, pos_x, pos_y + HUMAN_MAPCOLLISION_ADD_HEIGHT_A, pos_z, 0x02);
					CollisionBlockScratch(CollD, inblockdata, i, &pos_x, &pos_y, &pos_z, pos_x2, pos_y2, pos_z2, pos_x, pos_y + HUMAN_MAPCOLLISION_ADD_HEIGHT_B, pos_z, 0x02);
				}
			}
		}

		//롈뽋귩뱋귡
		if( (fabsf(move_x) > 0.2f)||(fabsf(move_z) > 0.2f) ){
			if( move_y_upper == 0 ){
				bool flag = false;
				for(int i=0; i<bs; i++){
					if( CheckBlockID[i] == true ){
						//뫉궻궰귏먩궻귒긳깓긞긏궸뻹귏궯궲궋귢궽
						if(
							(CollD->CheckBlockInside(i, pos_x + dist_x*2.0f, pos_y, pos_z + dist_z*2.0f, true, NULL) == true)&&
							(CollD->CheckBlockInside(i, pos_x + dist_x*2.0f, pos_y + HUMAN_MAPCOLLISION_CLIMBHEIGHT, pos_z + dist_z*2.0f, true, NULL) == false)
						){
							flag = true;

							int face;
							blockdata bdata;

							//뫉뙰궻뽋궻둷뱗귩롦벦
							//?겍뫉뙰궸긳깓긞긏궕궇귡궞궴궼둴믦궶궻궳갂DummyRay궼뷠뾴궶궋갋갋갋궼궦갃
							if( CollD->CheckBlockIntersectRay(i, pos_x, pos_y, pos_z, 0.0f, -1.0f, 0.0f, &face, &Dist, 1.2f) == true ){

								inblockdata->Getdata(&bdata, i);

								//롈뽋궸뿧궯궲궋귡궶귞궽갂뤵뤈뼰뚼
								if( acosf(bdata.material[face].vy) > HUMAN_MAPCOLLISION_SLOPEANGLE ){
									flag = false;
									break;
								}
							}
						}
					}
				}

				//뤵뤈궥귡걁뱋귡걂
				if( flag == true ){
					pos_y += HUMAN_MAPCOLLISION_CLIMBADDY;
					move_y *= 0.2f;
				}
			}
		}

		//댷벍먩궕긳깓긞긏귉귕귟뜛귪궳궋귡궶귞댷벍귩뼰뚼궸궥귡
		for(int i=0; i<bs; i++){
			if( CheckBlockID[i] == true ){
				//?겍깒귽궻럑?궕긳깓긞긏궻볙뫀궔궼뵽믦궢궶궋갃
				if( CollD->CheckBlockIntersectRay(i, pos_x, pos_y + HUMAN_HEIGHT-2.0f, pos_z, 0.0f, -1.0f, 0.0f, NULL, &Dist, (HUMAN_HEIGHT-2.0f*2)) == true ){
					pos_x = pos_x2;
					pos_z = pos_z2;
				}

				//댷벍먩궕긳깓긞긏궸귕귟뜛귔걬댷벍먩귖귕귟뜛귔?궶귞궽갂댷벍귩뼰뚼궸궥귡
				if( CollD->CheckBlockInside(i, pos_x, pos_y + HUMAN_HEIGHT-0.6f, pos_z, true, NULL) == true ){
					//긽긾갌겓궞궻뵽믦됼궢궋갃?빁궸돀궋댧뭫궳궇귢궽쀇궘갂11?궳궇귡뷠뾴궼궶궋갎
					//?겍AABB궻뵽믦붝댪뎭궑궲돀궋궻궳갂멣븫긳깓긞긏뮧귊뮳궥갃
					if( CollD->CheckALLBlockInside(pos_x + move_x*11.0f, pos_y + HUMAN_HEIGHT-0.6f, pos_z + move_z*11.0f) == true ){
						pos_x = pos_x2;
						pos_y = pos_y2;
						pos_z = pos_z2;
						if( move_y > 0.0f ){ move_y = 0.0f; }
					}
				}
			}
		}
	}

	if( move_y_upper > 0 ) { move_y_upper -= 1; }

	//뿇돷룉뿚
	move_y_flag = false;
	FallFlag = false;
	for(int ycnt=0; ycnt<3; ycnt++){
		int cnt;
		float ang = atan2f(move_z, move_x);
		float x, y, z;

		//뿇돷
		pos_y += move_y*0.33f;

		if( player == true ){
			y = pos_y + HUMAN_MAPCOLLISION_GROUND_HEIGHT;

			//4뺴뛀뵽믦 No.1
			cnt = 0;
			x = pos_x + cosf(ang)*HUMAN_MAPCOLLISION_GROUND_R1;
			z = pos_z + sinf(ang)*HUMAN_MAPCOLLISION_GROUND_R1;
			for(int i=0; i<bs; i++){
				if( CheckBlockID[i] == true ){
					if( CollD->CheckBlockInside(i, x, y, z, true, NULL) == true ){ cnt += 1; break; }
				}
			}
			x = pos_x - cosf(ang)*HUMAN_MAPCOLLISION_GROUND_R1;
			z = pos_z - sinf(ang)*HUMAN_MAPCOLLISION_GROUND_R1;
			for(int i=0; i<bs; i++){
				if( CheckBlockID[i] == true ){
					if( CollD->CheckBlockInside(i, x, y, z, true, NULL) == true ){ cnt += 1; break; }
				}
			}
			x = pos_x + cosf(ang + (float)M_PI/2)*HUMAN_MAPCOLLISION_GROUND_R1;
			z = pos_z + sinf(ang + (float)M_PI/2)*HUMAN_MAPCOLLISION_GROUND_R1;
			for(int i=0; i<bs; i++){
				if( CheckBlockID[i] == true ){
					if( CollD->CheckBlockInside(i, x, y, z, true, NULL) == true ){ cnt += 1; break; }
				}
			}
			x = pos_x + cosf(ang - (float)M_PI/2)*HUMAN_MAPCOLLISION_GROUND_R1;
			z = pos_z + sinf(ang - (float)M_PI/2)*HUMAN_MAPCOLLISION_GROUND_R1;
			for(int i=0; i<bs; i++){
				if( CheckBlockID[i] == true ){
					if( CollD->CheckBlockInside(i, x, y, z, true, NULL) == true ){ cnt += 1; break; }
				}
			}
			if( cnt == 4 ){ FallFlag = true; break; }

			//4뺴뛀뵽믦 No.2
			cnt = 0;
			x = pos_x + cosf(ang)*HUMAN_MAPCOLLISION_GROUND_R2;
			z = pos_z + sinf(ang)*HUMAN_MAPCOLLISION_GROUND_R2;
			for(int i=0; i<bs; i++){
				if( CheckBlockID[i] == true ){
					if( CollD->CheckBlockInside(i, x, y, z, true, NULL) == true ){ cnt += 1; break; }
				}
			}
			x = pos_x - cosf(ang)*HUMAN_MAPCOLLISION_GROUND_R2;
			z = pos_z - sinf(ang)*HUMAN_MAPCOLLISION_GROUND_R2;
			for(int i=0; i<bs; i++){
				if( CheckBlockID[i] == true ){
					if( CollD->CheckBlockInside(i, x, y, z, true, NULL) == true ){ cnt += 1; break; }
				}
			}
			x = pos_x + cosf(ang + (float)M_PI/2)*HUMAN_MAPCOLLISION_GROUND_R2;
			z = pos_z + sinf(ang + (float)M_PI/2)*HUMAN_MAPCOLLISION_GROUND_R2;
			for(int i=0; i<bs; i++){
				if( CheckBlockID[i] == true ){
					if( CollD->CheckBlockInside(i, x, y, z, true, NULL) == true ){ cnt += 1; break; }
				}
			}
			x = pos_x + cosf(ang - (float)M_PI/2)*HUMAN_MAPCOLLISION_GROUND_R2;
			z = pos_z + sinf(ang - (float)M_PI/2)*HUMAN_MAPCOLLISION_GROUND_R2;
			for(int i=0; i<bs; i++){
				if( CheckBlockID[i] == true ){
					if( CollD->CheckBlockInside(i, x, y, z, true, NULL) == true ){ cnt += 1; break; }
				}
			}
			if( cnt == 4 ){ FallFlag = true; break; }
		}
		else{
			y = pos_y + HUMAN_MAPCOLLISION_GROUND_HEIGHT;

			//?돷뵽믦
			x = pos_x;
			z = pos_z;
			for(int i=0; i<bs; i++){
				if( CheckBlockID[i] == true ){
					if( CollD->CheckBlockInside(i, x, y, z, true, NULL) == true ){ FallFlag = true; break; }
				}
			}
			if( FallFlag == true ){ break; }

			//둖뫀귉룺궢둖궢궲뵽믦
			x = pos_x + cosf(ang)*HUMAN_MAPCOLLISION_GROUND_R3;
			z = pos_z + sinf(ang)*HUMAN_MAPCOLLISION_GROUND_R3;
			for(int i=0; i<bs; i++){
				if( CheckBlockID[i] == true ){
					if( CollD->CheckBlockInside(i, x, y, z, true, NULL) == true ){ FallFlag = true; break; }
				}
			}
			if( FallFlag == true ){ break; }
		}

		//뿇돷뫊뱗돿럁
		move_y -= HUMAN_DAMAGE_SPEED;
		if( move_y < HUMAN_DAMAGE_MAXSPEED ){ move_y = HUMAN_DAMAGE_MAXSPEED; }

		move_y_flag = true;
	}

	move_y_landing = false;

	//뭤뽋궸먝뭤궢궲궋귡뤾뜃궻룉뿚
	if( FallFlag == true ){
		int id, face;
		blockdata bdata;
		bool flag = true;

		if( move_y < HUMAN_MAPCOLLISION_GROUND_HEIGHT ){ move_y_landing = true; }

		if( Invincible == false ){
			//뿇돷?긽?긙귩?궑귡
			if( move_y < HUMAN_DAMAGE_MINSPEED ){
				hp -= GetRand(6) + (int)((float)HUMAN_DAMAGE_MAXFALL / fabsf(HUMAN_DAMAGE_MAXSPEED - (HUMAN_DAMAGE_MINSPEED)) * fabsf(move_y - (HUMAN_DAMAGE_MINSPEED)));
			}
		}

		move_y = 0.0f;

		if( (GetFlag(MoveFlag_lt, MOVEFLAG_JUMP))&&(move_y_upper == 0) ){
			move_y = HUMAN_JUMP_SPEED;

			//긾?긘깈깛렳뛱
			MotionCtrl->Jump();
		}

		if( GetRand(4) == 0 ){
			if( (GetFlag(MoveFlag_lt, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT | MOVEFLAG_WALK)) == 0x00)&&(move_y_upper == 0) ){ flag = false; }
		}

		//뫉뙰궻긳깓긞긏궴뽋붥뜂귩롦벦
		// 경사면을 특정 각도로 빠르게 달릴 때 중심 바로 아래의 단일 레이가
		// 삼각면 경계나 블록 모서리를 지나가면, 실제로는 지면에 닿아 있어도
		// underblock_id가 -1로 남아 발소리 재질 판정이 끊길 수 있다.
		// 먼저 기존 중심 레이를 유지하고, 실패했을 때만 발 주변을 추가 탐색한다.
		id = -1;
		for(int i=0; i<bs; i++){
			if( CheckBlockID[i] == true ){
				if( CollD->CheckBlockIntersectRay(i, pos_x, pos_y + 2.5f, pos_z, 0.0f, -1.0f, 0.0f, &face, &Dist, (2.5f + 1.0f)) == true ){
					id = i;
					break;
				}
			}
		}

		if( id == -1 ){
			float ground_probe_angle = atan2f(pos_z - pos_z2, pos_x - pos_x2);
			float ground_probe_x[8];
			float ground_probe_z[8];
			float ground_probe_radius[2] = {
				HUMAN_MAPCOLLISION_GROUND_R1,
				HUMAN_MAPCOLLISION_GROUND_R2
			};

			int probe_index = 0;
			for(int radius_index=0; radius_index<2; radius_index++){
				float radius = ground_probe_radius[radius_index];
				ground_probe_x[probe_index] = pos_x + cosf(ground_probe_angle) * radius;
				ground_probe_z[probe_index] = pos_z + sinf(ground_probe_angle) * radius;
				probe_index++;

				ground_probe_x[probe_index] = pos_x - cosf(ground_probe_angle) * radius;
				ground_probe_z[probe_index] = pos_z - sinf(ground_probe_angle) * radius;
				probe_index++;

				ground_probe_x[probe_index] = pos_x + cosf(ground_probe_angle + (float)M_PI/2) * radius;
				ground_probe_z[probe_index] = pos_z + sinf(ground_probe_angle + (float)M_PI/2) * radius;
				probe_index++;

				ground_probe_x[probe_index] = pos_x + cosf(ground_probe_angle - (float)M_PI/2) * radius;
				ground_probe_z[probe_index] = pos_z + sinf(ground_probe_angle - (float)M_PI/2) * radius;
				probe_index++;
			}

			float nearest_ground_dist = 999999.0f;
			int nearest_ground_id = -1;
			int nearest_ground_face = -1;

			for(int probe=0; probe<8; probe++){
				for(int i=0; i<bs; i++){
					if( CheckBlockID[i] == true ){
						int probe_face = -1;
						float probe_dist = 0.0f;

						if( CollD->CheckBlockIntersectRay(
							i,
							ground_probe_x[probe],
							pos_y + 2.5f,
							ground_probe_z[probe],
							0.0f, -1.0f, 0.0f,
							&probe_face,
							&probe_dist,
							(2.5f + 1.0f)) == true ){
							if( probe_dist < nearest_ground_dist ){
								nearest_ground_dist = probe_dist;
								nearest_ground_id = i;
								nearest_ground_face = probe_face;
							}
						}
					}
				}
			}

			if( nearest_ground_id != -1 ){
				id = nearest_ground_id;
				face = nearest_ground_face;
				Dist = nearest_ground_dist;
			}
		}

		if( id != -1 ){
			//긳깓긞긏ID궴뽋붥뜂귩딯돬
			underblock_id = id;
			underblock_face = face;

			if( flag == true ){
				inblockdata->Getdata(&bdata, id);

				//롈뽋궸뿧궯궲궋귡궶귞궽갂롈뽋귩뒍귞궧귡룉뿚
				if( acosf(bdata.material[face].vy) > HUMAN_MAPCOLLISION_SLOPEANGLE ){
					move_x = bdata.material[face].vx * 1.2f;
					move_y = bdata.material[face].vy * -0.5f;
					move_z = bdata.material[face].vz * 1.2f;

					for(int i=0; i<bs; i++){
						if( CheckBlockID[i] == true ){
							if( CollD->CheckBlockInside(i, pos_x + move_x*3.0f, pos_y + move_y*3.0f, pos_z + move_z*3.0f, true, NULL) == true ){
								move_y = 0.0f;
								if( CollD->CheckBlockInside(i, pos_x + move_x*3.0f, pos_y, pos_z + move_z*3.0f, true, NULL) == true ){
									move_x = 0.0f;
									move_z = 0.0f;
									break;
								}
							}
						}
					}

					move_y_upper = 8;
				}
			}
		}
	}

	//댷벍긹긏긣깑띋럁뢯
	dist_x = pos_x - pos_x2;
	dist_y = pos_y - pos_y2;
	dist_z = pos_z - pos_z2;

	Dist = sqrtf(dist_x*dist_x + dist_z*dist_z);
	speed = sqrtf((move_x*2)*(move_x*2) + (move_z*2)*(move_z*2));

	//뼻귞궔궸댷벍쀊궕궓궔궢궋궶귞궽뙰궸뽣궥
	if( (sqrtf(dist_x*dist_x + dist_y*dist_y + dist_z*dist_z) > HUMAN_MAPCOLLISION_CHECK_MAXDIST)||(Dist - speed > 1.0f) ){
		pos_x = pos_x2;
		pos_y = pos_y2;
		pos_z = pos_z2;
	}

	if( player == true ){
		speed = VectorNormalization(&dist_x, &dist_y, &dist_z);

		//댷벍먩궴궻듩궸긳깓긞긏궕궇귡궶귞궽뙰궸뽣궥
		//?겍깒귽궻럑?궕긳깓긞긏궻볙뫀궔궼뵽믦궢궶궋갃
		if( CollD->CheckALLBlockIntersectRay(pos_x2, pos_y2 + HUMAN_MAPCOLLISION_CHECK_HEIGHT, pos_z2, dist_x, dist_y, dist_z, NULL, NULL, &Dist, speed) == true ){
			pos_x = pos_x2;
			pos_y = pos_y2;
			pos_z = pos_z2;
		}
	}
}

//! @brief 긳깓긞긏궴뤧벺렄궻긹긏긣깑똶럁
//! @param CollD Collision긏깋긚궻?귽깛?
//! @param inblockdata BlockDataInterface긏깋긚궻?귽깛?
//! @param blockid 긳깓긞긏ID
//! @param px X띆뷭
//! @param py Y띆뷭
//! @param pz Z띆뷭
//! @param px_old 멟긲깒??궻X띆뷭
//! @param py_old 멟긲깒??궻Y띆뷭
//! @param pz_old 멟긲깒??궻Z띆뷭
//! @param in_vx 뵽믦궥귡X띆뷭
//! @param in_vy 뵽믦궥귡Y띆뷭
//! @param in_vz 뵽믦궥귡Z띆뷭
//! @param mode 띆뷭뺚맫긾?긤걁0x00갌믅륂갂0x01갌Y띆뷭뤵뤈뗕?갂0x02갌Y띆뷭뚂믦걂
//! @return 맟뚻갌true?렪봲갌false
bool human::CollisionBlockScratch(class Collision *CollD, class BlockDataInterface *inblockdata, int blockid, float *px, float *py, float *pz, float px_old, float py_old, float pz_old, float in_vx, float in_vy, float in_vz, int mode)
{
	if( CollD == NULL ){ return false; }
	if( inblockdata == NULL ){ return false; }
	if( (blockid < 0)||(inblockdata->GetTotaldatas() <= blockid) ){ return false; }
	//if( (*px == px_old)&&(*py == py_old)&&(*pz == pz_old) ){ return false; }

	float px2, py2, pz2;
	float vx, vy, vz;
	float dist;
	int face;
	float face_vx, face_vy, face_vz, face_angle, face_angle_per;
	float temp;

	//긽긾갌
	//OpenXOPS궻뱰궫귟뵽믦궼갂똶럁뚮뜼귩땪뢁궥귡궫귕뗇궔궸묈궖귕궸긳깓긞긏귩뵽믦궢궲궋귡갃
	//궩궻궫귕갂뫉뙰궻긳깓긞긏궻궰궶궗뽞궳덙궯?궔귡뤾뜃궕맯궣귡갃
	//뫮랉궴궢궲갂뫉뙰궻뵽믦걁mode 0x00 믅륂걂궻뤾뜃궼갂뵽믦궥귡Y띆뷭귩뺚맫궥귡갃
	if( mode == 0 ){
		in_vy += COLLISION_ADDSIZE;
	}

	px2 = *px;
	py2 = *py;
	pz2 = *pz;

	//뙸띪궻댷벍긹긏긣깑궴떁뿣럁뢯
	vx = *px - px_old;
	vy = *py - py_old;
	vz = *pz - pz_old;
	dist = VectorNormalization(&vx, &vy, &vz);

	//AABB귩띿귡
	float rmin_x, rmin_y, rmin_z, rmax_x, rmax_y, rmax_z;
	GetAABBRay(in_vx - vx*dist, in_vy - vy*dist, in_vz - vz*dist, vx, vy, vz, dist, &rmin_x, &rmin_y, &rmin_z, &rmax_x, &rmax_y, &rmax_z);

	//믁벺궢궫긳깓긞긏뽋롦벦
	face = -1;
	if( CollD->CheckBlockAABB(blockid, rmin_x, rmin_y, rmin_z, rmax_x, rmax_y, rmax_z) == false ){ return false; } 
	if( CollD->CheckBlockIntersectRay(blockid, in_vx - vx*dist, in_vy - vy*dist, in_vz - vz*dist, vx, vy, vz, &face, &temp, dist) == false ){ return false; }

	//긳깓긞긏궻뽋궸뫮궥귡긹긏긣깑궻궶궥둷귩땫귕귡
	if( CollD->AngleVector2(blockid, face, vx, vy, vz, &face_angle) == false ){ return false; }

	//둷뱗궻뒆뜃럁뢯
	//if( face_angle != 0.0f ){ face_angle_per = 1.0f - face_angle / ((float)M_PI); }
	if( face_angle != 0.0f ){ face_angle_per = ((float)M_PI) / face_angle - 1.0f; }
	else{ face_angle_per = 0.0f; }

	//긳깓긞긏뽋궻?멄긹긏긣깑롦벦
	blockdata bdata;
	inblockdata->Getdata(&bdata, blockid);
	face_vx = bdata.material[face].vx;
	face_vy = bdata.material[face].vy;
	face_vz = bdata.material[face].vz;

	//댷벍긹긏긣깑??멄긹긏긣깑귩럁뢯
	float vx2, vy2, vz2;
	vx2 = vx + face_vx;
	vy2 = vy + face_vy;
	vz2 = vz + face_vz;
	VectorNormalization(&vx2, &vy2, &vz2);

	//궶궥둷궸돒궣궲댷벍긹긏긣깑귩럁뢯
	vx = vx2 * (1.0f-face_angle_per) + vx * face_angle_per;
	vy = vy2 * (1.0f-face_angle_per) + vy * face_angle_per;
	vz = vz2 * (1.0f-face_angle_per) + vz * face_angle_per;
	VectorNormalization(&vx, &vy, &vz);

	//릣빟뺴뛀궸댷벍궢궶궋궶귞궽갂?멄긹긏긣깑귩뾭궋귡
	if( (vx == 0.0f)&&(vz == 0.0f) ){
		vx = bdata.material[face].vx;
		vy = bdata.material[face].vy;
		vz = bdata.material[face].vz;
	}

	//띍뢎밒궶댧뭫귩똶럁
	temp = face_angle_per * dist;
	*px = vx*temp + px_old;
	*py = vy*temp + py_old;
	*pz = vz*temp + pz_old;

	//띍뢎밒궶댧뭫궕긳깓긞긏궻볙뫀궶귞궽갂댷벍뼰뚼
	//if( CollD->CheckALLBlockInside(*px, *py, *pz) == true ){
	if( CollD->CheckBlockInside(blockid, *px, *py, *pz, true, NULL) == true ){
		*px = px_old;
		*py = py_old;
		*pz = pz_old;
	}

	//띆뷭뺚맫
	if( mode == 0x01 ){
		if( *py > py2 ){ *py = py2; }	//뜞귝귟뤵궸궼댷벍궠궧궶궋
	}
	if( mode == 0x02 ){
		*py = py2;						//Y띆뷭궼빾궑궶궋
	}

	return true;
}

//! @brief 똶럁귩렳뛱걁뱰궫귟뵽믦걂
//! @param CollD Collision궻?귽깛?
//! @param inblockdata BlockDataInterface궻?귽깛?
//! @param AddCollisionFlag 믁돿궻뱰궫귟뵽믦긲깋긐
//! @param player 뫮뤭궻릐븿궕긵깒귽깂?궔궵궎궔
//! @param F5mode 뤵뤈??걁F5뿞땆걂궻긲깋긐?걁뾎뚼갌true?뼰뚼갌false걂
//! @return 룉뿚궶궢갌0?믅륂룉뿚갌1??뻊궢궲?귢뢎귦궯궫뮳뚣갌2?먄?궢궫?뫬갌3?뭤?궸귝귟?뻊궢궫뮳뚣갌4
int human::ProcessObject(class Collision* CollD, class BlockDataInterface* inblockdata, bool AddCollisionFlag, bool player, bool F5mode)
{

	if (CollD == NULL) { return 0; }
	if (inblockdata == NULL) { return 0; }
	if (EnableFlag == false) { return 0; }

	UpdateSkillStealth();
	UpdateSkillAmmoBoost();
	UpdateSkillSurvival();
	UpdateSkillDodge();
	UpdateSkillTeleportCast();
	UpdateSkillIronBody();
	UpdateSkillAbsorb();
	UpdateSkillSuppressed();
	UpdateSkillRobotExecute();
	UpdateSkillRage();

	if (F5mode == true) {
		move_y = 0.0f;
		pos_y += 5.0f;
	}

	if (boltActionTimer > 0) {
		boltActionTimer--;
	}

	switch (shell_reload_state)
	{
		case SHELL_RELOAD_START_DELAY:
			if (boltActionTimer <= 0) {
				shell_reload_state = SHELL_RELOAD_INSERTING;
				shell_reload_timer = shell_reload_time_per_shell;
			}
			break;

		case SHELL_RELOAD_INSERTING:
			if (shell_reload_timer > 0) {
				shell_reload_timer--;
			}

			if (shell_reload_timer <= 0) {
				if (weapon[selectweapon] != NULL) {
					int current_load, total_ammo, param_id;
					WeaponParameter p_data;

					weapon[selectweapon]->GetParamData(&param_id, &current_load, &total_ammo);
					Param->GetWeapon(param_id, &p_data);

					int limit_load = p_data.nbsmax;
					if (p_data.bHasChamber) {
						limit_load += 1;
					}

					// 1. 탄을 무조건 1발 넣습니다.[cite: 1]
					current_load++;
					weapon[selectweapon]->SetParamData(param_id, current_load, total_ammo, false);

					// 2. 방금 넣은 탄이 마지막 탄이라면 긴 대기 시간 없이 즉시 10프레임 딜레이로 넘깁니다.[cite: 1]
					if (current_load >= limit_load || (total_ammo - current_load) <= 0) {
						shell_reload_state = SHELL_RELOAD_INTERRUPT_DELAY;
						shell_reload_timer = 10;
						boltActionTimer = 0;
					}
					else {
						// 3. 아직 덜 찼다면 다음 탄을 넣기 위해 정상적인 단발 장전 시간만큼 대기합니다.[cite: 1]
						shell_reload_timer = shell_reload_time_per_shell;
					}
				}
				else {
					shell_reload_state = SHELL_RELOAD_NONE;
				}
			}
			break;

		case SHELL_RELOAD_INTERRUPT_DELAY:
			// 양수 10에서 정상적으로 카운트다운 합니다. (UI에는 GetShellReloadTimer()에 의해 0으로 전달됨)[cite: 1]
			if (shell_reload_timer > 0) {
				shell_reload_timer--;
			}
			if (shell_reload_timer <= 0) {
				StartEndDelay();
			}
			break;

		case SHELL_RELOAD_END_DELAY:
			if (boltActionTimer <= 0) {
				shell_reload_state = SHELL_RELOAD_NONE;
				weaponshotcnt = 2;
			}
			break;

		case SHELL_RELOAD_NONE:
		default:
			break;
	}

	// [자동 점사 로직]
	if (weaponburstmodecnt > 0) {
		int weapon_id = GetMainWeaponTypeNO();
		if (weapon_id != ID_WEAPON_NONE) {
			WeaponParameter params;
			if (Param->GetWeapon(weapon_id, &params) == 0) {
				if (params.burstcnt > 1 && params.burstcnt < 4) {
					if (weaponburstmodecnt < params.burstcnt && weaponshotcnt == 0) {
						return 5;
					}
				}
			}
		}
	}

	// [✨ 수정된 발사 모드 변경 타이머 로직 ✨]
	// 타이머가 0이 되었을 때의 처리를 이 위치로 옮겨서 다른 로직에 의해 중단되지 않도록 합니다.
	if (changeweaponidcnt > 0) {
		changeweaponidcnt--;
		if (changeweaponidcnt == 0) {
			int lnbs, nbs;
			weapon[selectweapon]->GetParamData(NULL, &lnbs, &nbs);
			weapon[selectweapon]->SetParamData(changeweaponid_target, lnbs, nbs, false);

			MotionCtrl->ChangeWeaponID(changeweaponid_target);
			changeweaponid_target = -1;
			changeweaponid_total_time = 0;
		}
	}

	if (deadstate == 5) { return 3; }

	int WeaponReloadMotionCnt;
	int WeaponChangeWeaponIDCnt;
	int CheckDead;

	if (selectweaponcnt > 0) {
		selectweaponcnt -= 1;
	}

	if (weaponshotcnt > 0) {
		weaponshotcnt -= 1;
	}

	weaponshotframe = false;

	if (weaponreloadcnt > 0) {
		weaponreloadcnt -= 1;
		if (weaponreloadcnt == 0) {
			if (weapon[selectweapon] != NULL) { weapon[selectweapon]->RunReload(); }
		}
	}

	if (weapon[selectweapon] != NULL) {
		WeaponReloadMotionCnt = weaponreloadcnt;
		WeaponChangeWeaponIDCnt = changeweaponidcnt;
	}
	else {
		WeaponReloadMotionCnt = 0;
		WeaponChangeWeaponIDCnt = 0;
	}

	GunsightErrorRange();
	CheckDead = CheckAndProcessDead(CollD);
	if (CheckDead == 3) { return 2; }

	if ((CheckDead == 0) || (deadstate == 1)) {
		int hp_old = hp;
		ControlProcess();
		CollisionMap(CollD, inblockdata, AddCollisionFlag, player);
		if ((hp_old > 0) && (hp <= 0)) {
			return 4;
		}
		if (CheckDead == 0) {
			if (pos_y < HUMAN_DEADLINE) {
				hp = 0;
				return 4;
			}
		}
	}

	class weapon* nowweapon;
	int weapon_paramid;
	nowweapon = weapon[selectweapon];

	if (nowweapon == NULL) {
		weapon_paramid = ID_WEAPON_NONE;
	}
	else {
		nowweapon->GetParamData(&weapon_paramid, NULL, NULL);
	}

	// 플레이어 충격 수류탄 준비 중에는 실제 장착 무기와 무관하게
	// 손 모션 계산만 충격 수류탄 무기 기준으로 처리한다.
	if ((player == true) && (render_override_weapon_id >= 0)) {
		weapon_paramid = render_override_weapon_id;
	}

	// 현재 human 파라미터가 좀비인지 확인한다.
	// parameter.cpp 기준으로 좀비는 HumanParameter.type == 2.
	bool zombie = false;
	if (Param != NULL) {
		HumanParameter hparam;
		if (Param->GetHuman(id_parameter, &hparam) == 0) {
			if (hparam.type == 2) {
				zombie = true;
			}
		}
	}

	MotionCtrl->ProcessObject(rotation_x, armrotation_y, weapon_paramid, WeaponReloadMotionCnt, WeaponChangeWeaponIDCnt, MoveFlag_lt, hp, player, zombie);

	if (CheckDead != 0) { return 3; }
	return 1;
}

//! @brief 뷭?뚮뜼귩롦벦
int human::GetGunsightErrorRange()
{
	if (skill_aimhack_accuracy_flag == true) {
		return 0;
	}

	return StateGunsightErrorRange + ReactionGunsightErrorRange;
}

//! @brief ?됪
//! @param d3dg D3DGraphics궻?귽깛?
//! @param Resource ResourceManager궻?귽깛?
//! @param DrawArmOnly 쁱궴븧딇궻귒?됪궥귡
//! @param player 뫮뤭궻릐븿궕긵깒귽깂?궔궵궎궔
//! @param NoModel 긾긢깑?됪뼰뚼돸
//! @todo 쁱궻댧뭫귩뛱쀱궳땫귕귡
void human::Render(class D3DGraphics* d3dg, class ResourceManager* Resource, bool DrawArmOnly, bool player, bool NoModel)
{
	if (d3dg == NULL) { return; }
	if (Resource == NULL) { return; }
	if (EnableFlag == false) { return; }

	float human_alpha = 1.0f;
	float human_arm_alpha = 1.0f;
	float human_weapon_alpha = 1.0f;

	if (GetSkillStealthFlag() == true) {
		if ((DrawArmOnly == true) && (player == true)) {
			// 1인칭 은신: 손/총기도 투명하게 보이게 한다.
			// 총기는 손보다 조금 더 투명하게 해서 손을 덜 잡아먹게 한다.
			human_alpha = 1.0f;
			human_arm_alpha = 0.55f;
			human_weapon_alpha = 0.50f;
		}
		else {
			human_alpha = 0.35f;
			human_arm_alpha = 0.35f;
			human_weapon_alpha = 0.35f;
		}
	}

	bool shadow_decoy_render = GetSkillShadowDecoyFlag();

	// 위치 보간은 외부에서 계산한 하나의 기준 위치만 사용한다.
	// 몸통/다리/팔/무기가 모두 같은 좌표를 사용하므로 신체가 따로 흔들리지 않는다.
	float render_pos_x = render_position_override ? render_position_x : pos_x;
	float render_pos_y = render_position_override ? render_position_y : pos_y;
	float render_pos_z = render_position_override ? render_position_z : pos_z;

	float render_yaw_offset =
		render_angle_offset_override ? render_rotation_x_offset : 0.0f;
	float render_pitch_offset =
		render_angle_offset_override ? render_view_y_offset : 0.0f;

	float render_rotation_x = rotation_x + render_yaw_offset;
	float render_rotation_y = rotation_y;
	float render_armrotation_y = armrotation_y;

	// 그림자 잔상은 플레이어와 같은 외형을 유지하되, 살짝 반투명하게 보이게 한다.
	if (shadow_decoy_render == true) {
		if (human_alpha > 0.85f) { human_alpha = 0.85f; }
		if (human_arm_alpha > 0.82f) { human_arm_alpha = 0.82f; }
		if (human_weapon_alpha > 0.78f) { human_weapon_alpha = 0.78f; }
	}

	// 그림자 잔상은 반투명 렌더링 때문에 모델 내부 머리/몸통 draw order가 꼬일 수 있다.
	// 잔상에 한해서 alpha 상태에서도 Z-write를 켜서 가까운 표면이 우선 보이게 한다.
	bool human_alpha_zwrite = shadow_decoy_render;

	bool ironbody_render = GetSkillIronBodyFlag();

	// 철괴는 기존 dark 처리 위에 약간 더 눌러서, 눈에 띄게 더 어둡게 보이게 한다.
	// 완전 투명하게 보이지 않도록 알파 감소는 약하게만 준다.
	int human_render_texture = id_texture;
	bool human_render_dark = DarkModelFlag || ironbody_render || shadow_decoy_render;

	// 철괴는 반투명이 아니라 색 자체를 더 어둡게 한다.
	// -1.0f면 기존 RenderModel 밝기 사용, 0.0f~1.0f면 지정 밝기 사용.
	float human_render_brightness = -1.0f;
	float human_weapon_brightness = -1.0f;

	if (ironbody_render == true) {
		human_render_brightness = 0.48f;
		human_weapon_brightness = 0.48f;
	}

	int upmodel, armmodel, legmodel;
	float armry, legrx;
	MotionCtrl->GetRenderMotion(&armry, &legrx, &upmodel, &armmodel, &legmodel);

	float render_armry = armry;
	// 다리의 이동 방향은 유지하면서 플레이어 전체의 렌더 yaw 보정만 함께 적용한다.
	float render_legrx = legrx + render_yaw_offset;

	bool draw_upper = ((render_parts_mask & HUMAN_RENDER_UPPER) != 0);
	bool draw_leg = ((render_parts_mask & HUMAN_RENDER_LEG) != 0);
	bool draw_arm = ((render_parts_mask & HUMAN_RENDER_ARM) != 0);
	bool draw_weapon = ((render_parts_mask & HUMAN_RENDER_WEAPON) != 0);

	bool render_override_weapon =
		(player == true) &&
		(render_override_weapon_id >= 0);

	// 앉기 상태에서 상체/팔/무기만 적당히 내린다.
// -6.0f는 너무 낮아 다리와 합체되어 보일 수 있으므로 우선 -3.5f 권장.
	const float crouch_render_y = (crouchflag == true) ? -2.0f : 0.0f;
	const float crouch_arm_y = (crouchflag == true) ? -2.0f : 0.0f;
	const float crouch_leg_y = (crouchflag == true) ? -2.0f : 0.0f;

	if (crouchflag == true) {
		bool moving = false;

		// Render 시점에서는 MoveFlag가 이미 0으로 초기화될 수 있으므로
		// 직전 프레임 입력인 MoveFlag_lt를 사용한다.
		if (GetFlag(MoveFlag_lt, MOVEFLAG_FORWARD)) { moving = true; }
		if (GetFlag(MoveFlag_lt, MOVEFLAG_BACK)) { moving = true; }
		if (GetFlag(MoveFlag_lt, MOVEFLAG_LEFT)) { moving = true; }
		if (GetFlag(MoveFlag_lt, MOVEFLAG_RIGHT)) { moving = true; }

		if (moving == false) {
			legmodel = sitlegmodel;
		}
		else {
			// Render() 안에서 sitwalkcnt를 증가시키면 144FPS 기준으로 너무 빨라진다.
			// 대신 로직 프레임에서만 증가하는 MotionCtrl의 이동 모션 카운터를 사용한다.
			int motioncnt = MotionCtrl->GetMoveMotionCnt();
			int sitframe = (motioncnt / 5) % TOTAL_SITWALKMODE;
			legmodel = sitwalkmodel[sitframe];
		}
	}

	// [최종] MotionCtrl에서 계산된 팔 각도(armry)에 반동 오프셋(recoil_y_offset)을 더해 최종 렌더링 각도를 계산
	float final_arm_ry = render_armry + recoil_y_offset;

	// 카메라 상하 회전과 팔/무기가 같은 보간 비율을 사용하도록
	// 현재 최종 팔 각도에 렌더링 전용 pitch 보정만 더한다.
	if (render_angle_offset_override == true) {
		final_arm_ry += render_pitch_offset;
	}

	class weapon* nowweapon;
	nowweapon = weapon[selectweapon];

	bool render_shield_weapon = false;

	int render_weapon_id_for_arm = ID_WEAPON_NONE;

	if (render_override_weapon == true) {
		render_weapon_id_for_arm = render_override_weapon_id;
	}
	else if (nowweapon != NULL) {
		nowweapon->GetParamData(&render_weapon_id_for_arm, NULL, NULL);
	}

	if (render_weapon_id_for_arm != ID_WEAPON_NONE) {
		WeaponParameter render_weapon_param;
		if (Param->GetWeapon(render_weapon_id_for_arm, &render_weapon_param) == 0) {
			if (render_weapon_param.WeaponP == WEAPONP_SHIELD) {
				render_shield_weapon = true;
			}
		}
	}

	// 방패 계열 WeaponP는 사격 반동이 없고,
	// 시점 상하 변화에 따라 같이 기울어지면 어색하므로
	// 방패 전용 팔 모션에서 계산된 armry만 사용한다.
	if (render_shield_weapon == true) {
		final_arm_ry = render_armry;
	}

	if (DrawArmOnly == false) {
		float add_x, add_y, add_z;

		if (render_rotation_y != 0.0f) {
			add_x = cosf(render_rotation_x * -1 - (float)M_PI / 2) * sinf(render_rotation_y) * -1.0f;
			add_y = cosf(render_rotation_y) * -1.0f;
			add_z = sinf(render_rotation_x * -1 - (float)M_PI / 2) * sinf(render_rotation_y) * -1.0f;
		}
		else {
			add_x = 0.0f;
			add_y = -1.0f;
			add_z = 0.0f;
		}
		if (draw_upper == true) {
			d3dg->SetWorldTransform(render_pos_x + add_x, render_pos_y + add_y + crouch_render_y, render_pos_z + add_z, render_rotation_x + (float)M_PI, render_rotation_y, upmodel_size);
			d3dg->RenderModel(upmodel, human_render_texture, human_render_dark, false, NoModel, false, human_alpha, human_render_brightness, human_alpha_zwrite);
		}

		if (render_rotation_y != 0.0f) {
			add_x = cosf(render_rotation_x * -1 - (float)M_PI / 2) * sinf(render_rotation_y) * -0.2f;
			add_y = cosf(render_rotation_y) * -0.2f;
			add_z = sinf(render_rotation_x * -1 - (float)M_PI / 2) * sinf(render_rotation_y) * -0.2f;
		}
		else {
			add_x = 0.0f;
			add_y = -0.2f;
			add_z = 0.0f;
		}
		if (draw_leg == true) {
			d3dg->SetWorldTransform(render_pos_x + add_x, render_pos_y + add_y + crouch_leg_y, render_pos_z + add_z, render_legrx + (float)M_PI, render_rotation_y, legmodel_size);
			d3dg->RenderModel(legmodel, human_render_texture, human_render_dark, false, NoModel, false, human_alpha, human_render_brightness, human_alpha_zwrite);
		}
	}

	// 1인칭 시점에서 팔/무기만 그리도록 요청받았고, 현재 조준 상태라면 아무것도 그리지 않고 함수를 종료합니다.
	if (DrawArmOnly == true && scopemode != 0) {
		return;
	}

	if ((render_rotation_y != 0.0f) && (render_shield_weapon == false)) {
		if (draw_arm == true) {
			float add_x = cosf(render_rotation_x * -1 - (float)M_PI / 2) * sinf(render_rotation_y) * 16.0f;
			float add_y = cosf(render_rotation_y) * 16.0f;
			float add_z = sinf(render_rotation_x * -1 - (float)M_PI / 2) * sinf(render_rotation_y) * 16.0f;
			d3dg->SetWorldTransform(render_pos_x + add_x, render_pos_y + add_y + crouch_arm_y, render_pos_z + add_z, render_rotation_x + (float)M_PI, render_armrotation_y + render_rotation_y, armmodel_size);
			d3dg->RenderModel(armmodel, human_render_texture, human_render_dark, false, NoModel, false, human_arm_alpha, human_render_brightness, human_alpha_zwrite);
		}
	}
	else if ((nowweapon == NULL) && (render_override_weapon == false)) {
		if (draw_arm == true) {
			d3dg->SetWorldTransform(render_pos_x, render_pos_y + 16.0f + crouch_arm_y, render_pos_z, render_rotation_x + (float)M_PI, final_arm_ry, armmodel_size);
			d3dg->RenderModel(armmodel, human_render_texture, human_render_dark, false, NoModel, false, human_arm_alpha, human_render_brightness, human_alpha_zwrite);
		}
	}
	else {
		int id_param = ID_WEAPON_NONE;
		WeaponParameter paramdata;
		int model, texture;

		if (render_override_weapon == true) {
			id_param = render_override_weapon_id;
		}
		else if (nowweapon != NULL) {
			nowweapon->GetParamData(&id_param, NULL, NULL);
		}

		Param->GetWeapon(id_param, &paramdata);
		Resource->GetWeaponModelTexture(id_param, &model, &texture);

		float render_weapon_mx = paramdata.mx / 10 * -1;
		float render_weapon_my = paramdata.my / 10;
		float render_weapon_mz = paramdata.mz / 10 * -1;

		bool draw_weapon_first =
			(GetSkillStealthFlag() == true) &&
			(DrawArmOnly == true) &&
			(player == true);

		if (draw_weapon_first == true) {
			if (draw_weapon == true) {
				d3dg->SetWorldTransformHumanWeapon(render_pos_x, render_pos_y + 16.0f + crouch_arm_y, render_pos_z, render_weapon_mx, render_weapon_my, render_weapon_mz, render_rotation_x + (float)M_PI, final_arm_ry, paramdata.size);
				d3dg->RenderModel(model, texture, human_render_dark, false, NoModel, false, human_weapon_alpha, human_weapon_brightness, human_alpha_zwrite);
			}

			if (draw_arm == true) {
				d3dg->SetWorldTransform(render_pos_x, render_pos_y + 16.0f + crouch_arm_y, render_pos_z, render_rotation_x + (float)M_PI, final_arm_ry, armmodel_size);
				d3dg->RenderModel(armmodel, human_render_texture, human_render_dark, false, NoModel, false, human_arm_alpha, human_render_brightness, human_alpha_zwrite);
			}
		}
		else {
			if (draw_arm == true) {
				d3dg->SetWorldTransform(render_pos_x, render_pos_y + 16.0f + crouch_arm_y, render_pos_z, render_rotation_x + (float)M_PI, final_arm_ry, armmodel_size);
				d3dg->RenderModel(armmodel, human_render_texture, human_render_dark, false, NoModel, false, human_arm_alpha, human_render_brightness, human_alpha_zwrite);
			}

			if (draw_weapon == true) {
				d3dg->SetWorldTransformHumanWeapon(render_pos_x, render_pos_y + 16.0f + crouch_arm_y, render_pos_z, render_weapon_mx, render_weapon_my, render_weapon_mz, render_rotation_x + (float)M_PI, final_arm_ry, paramdata.size);
				d3dg->RenderModel(model, texture, human_render_dark, false, NoModel, false, human_weapon_alpha, human_weapon_brightness, human_alpha_zwrite);
			}
		}
	}
}
float human::GetRecoilYOffset()
{
	return recoil_y_offset;
}

float human::GetRecoilXOffset()
{
	return recoil_x_offset;
}

void human::GetRenderRecoilOffset(
	float alpha,
	float* offset_x,
	float* offset_y
)
{
	if (alpha < 0.0f) {
		alpha = 0.0f;
	}
	if (alpha > 1.0f) {
		alpha = 1.0f;
	}

	float render_x = recoil_x_offset;
	float render_y = recoil_y_offset;

	// 실제 반동값은 건드리지 않고,
	// 다음 로직 틱에서 적용될 회복량만 렌더링에 미리 반영한다.
	//
	// 사격 직후에는 recoil_accum_timer가 남아 있으므로
	// 새로 발생한 반동은 보간하지 않고 즉시 화면에 표시된다.
	if (recoil_accum_timer <= 0) {
		float decay = powf(
			HUMAN_RECOIL_RECOVERY_RATE,
			alpha
		);

		render_x *= decay;
		render_y *= decay;

		if ((render_x < 0.001f) &&
			(render_x > -0.001f)) {
			render_x = 0.0f;
		}

		if ((render_y < 0.001f) &&
			(render_y > -0.001f)) {
			render_y = 0.0f;
		}
	}

	if (offset_x != NULL) {
		*offset_x = render_x;
	}

	if (offset_y != NULL) {
		*offset_y = render_y;
	}
}

bool human::IsShellReloading()
{
	// 상태가 NONE이 아니면 무조건 장전 중인 것으로 게임 엔진에 보고하여 모션과 사운드가 씹히는 것을 방지합니다.[cite: 1]
	return (shell_reload_state != SHELL_RELOAD_NONE);
}

int human::GetShellReloadTimer()
{
	// 10프레임 중단 대기 중이거나 펌프 액션(종료 딜레이) 중일 때는 UI에 0을 반환하여 원형 타이머를 그리지 못하게 차단합니다.[cite: 1]
	if (shell_reload_state == SHELL_RELOAD_INTERRUPT_DELAY ||
		shell_reload_state == SHELL_RELOAD_END_DELAY) {
		return 0;
	}
	return shell_reload_timer;
}

int human::GetShellReloadTimePerShell()
{
	return shell_reload_time_per_shell;
}

human::ShellReloadState human::GetShellReloadState()
{
	return shell_reload_state;
}

void human::StartEndDelay()
{
	WeaponParameter ParamData;
	if (weapon[selectweapon] == NULL) {
		shell_reload_state = SHELL_RELOAD_NONE;
		return;
	}
	int param_id;
	weapon[selectweapon]->GetParamData(&param_id, NULL, NULL);
	if (Param->GetWeapon(param_id, &ParamData) != 0) {
		shell_reload_state = SHELL_RELOAD_NONE;
		return;
	}

	shell_reload_state = SHELL_RELOAD_END_DELAY;

	int delay_time = static_cast<int>(ParamData.blazings * 0.5f);
	if (param_id == 25) {
		delay_time = 25 / 2;
	}

	if (delay_time <= 0) { delay_time = 1; }

	// [✨ 복구됨] 펌프 액션 시에는 UI 타이머가 나타나도록 boltActionTimer를 사용합니다.
	boltActionTimer = delay_time;
	boltActionTotalTime = delay_time;
}

void human::InterruptShellReload()
{
	if (shell_reload_state == SHELL_RELOAD_END_DELAY ||
		shell_reload_state == SHELL_RELOAD_INTERRUPT_DELAY) {
		return;
	}

	if (shell_reload_state == SHELL_RELOAD_NONE) {
		return;
	}

	// 수동 중단 시에도 동일하게 10프레임 대기 버퍼를 줍니다.[cite: 1]
	shell_reload_state = SHELL_RELOAD_INTERRUPT_DELAY;
	shell_reload_timer = 10;

	boltActionTimer = 0;
	boltActionTotalTime = 0;
}


//! @brief 긓깛긚긣깋긏?
weapon::weapon(class ParameterInfo *in_Param, float x, float y, float z, float rx, int id_param, int nbs, bool flag)
{
	Param = in_Param;
	pos_x = x;
	pos_y = y;
	pos_z = z;
	move_x = 0.0f;
	move_y = 0.0f;
	move_z = 0.0f;
	rotation_x = rx;
	EnableFlag = flag;

	id_parameter = id_param;
	usingflag = false;
	bullets = nbs;
	Loadbullets = 0;
	motionflag = true;

	render_transform_override = false;
	render_pos_x = x;
	render_pos_y = y;
	render_pos_z = z;
	render_rotation_x = rx;
	render_generation = 0;

	if( Param != NULL ){
		WeaponParameter ParamData;
		if( Param->GetWeapon(id_param, &ParamData) == 0 ){
			model_size = ParamData.size;
			//id_model = ParamData.id_model;
			//id_texture = ParamData.id_texture;
		}
	}
	id_model = -1;
	id_texture = -1;
}

//! @brief 긢귻긚긣깋긏?
weapon::~weapon()
{}

//! @brief 띆뷭궴둷뱗귩먠믦
//! @param x X띆뷭
//! @param y Y띆뷭
//! @param z Z띆뷭
//! @param rx 돘렡됷?
void weapon::SetPosData(float x, float y, float z, float rx)
{
	pos_x = x;
	pos_y = y;
	pos_z = z;
	move_x = 0.0f;
	move_y = 0.0f;
	move_z = 0.0f;
	rotation_x = rx;

}

//! @brief 먠믦뭠귩먠믦
//! @param id_param 븧딇궻롰쀞붥뜂
//! @param lnbs 몧뭙릶
//! @param nbs 뜃똶뭙릶
//! @param init 긆긳긙긃긏긣귩룊딖돸
void weapon::SetParamData(int id_param, int lnbs, int nbs, bool init)
{
	id_parameter = id_param;
	bullets = nbs;
	Loadbullets = lnbs;

	if( init == true ){
		usingflag = false;
		motionflag = true;
		render_transform_override = false;
		render_generation += 1;
	}
}

//! @brief 먠믦뭠귩롦벦
//! @param id_param 븧딇궻롰쀞붥뜂귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param lnbs 몧뭙릶귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param nbs 뜃똶뭙릶귩롷궚롦귡?귽깛?걁NULL됀걂
void weapon::GetParamData(int *id_param, int *lnbs, int *nbs)
{
	if( id_param != NULL ){ *id_param = id_parameter; }
	if( lnbs != NULL ){ *lnbs = Loadbullets; }
	if( nbs != NULL ){ *nbs = bullets; }
}

//! @brief 븧딇궻럊뾭륉떟궻롦벦
//! @return 럊뾭뭷갌true?뼟럊뾭갌false
bool weapon::GetUsingFlag()
{
	return usingflag;
}

//! @brief 븧딇귩뢉궎
//! @return 맟뚻갌0?렪봲갌1
int weapon::Pickup()
{
	if (usingflag == true) { return 1; }
	usingflag = true;

	return 0;
}

//! @brief 븧딇귩롆궲귡
//! @param x X띆뷭
//! @param y Y띆뷭
//! @param z Z띆뷭
//! @param rx 돘렡됷?
//! @param speed 롆궲귡룊뫊
void weapon::Dropoff(float x, float y, float z, float rx, float speed, float add_move_x, float add_move_z, class Collision* CollD)
{
	//?됪궥귡띆뷭궴둷뱗귩먠믦
	float drop_dir_x = cosf(rx * -1 + (float)M_PI / 2);
	float drop_dir_z = sinf(rx * -1 + (float)M_PI / 2);

	const float default_drop_offset = 7.0f;
	const float wall_margin = 2.0f;

	float drop_offset = default_drop_offset;
	bool blocked_by_wall = false;

	// 벽에 붙어서 무기를 버릴 때, 기본 드랍 위치가 벽 반대편으로 넘어가지 않도록 검사한다.
	// 플레이어 몸 중심에서 드랍 방향으로 레이를 쏴서 벽이 있으면 벽 앞까지만 이동시킨다.
	if (CollD != NULL) {
		float hit_dist = 0.0f;

		if (CollD->CheckALLBlockIntersectRay(
			x,
			y + 10.0f,
			z,
			drop_dir_x,
			0.0f,
			drop_dir_z,
			NULL,
			NULL,
			&hit_dist,
			default_drop_offset + wall_margin
		) == true) {
			blocked_by_wall = true;

			drop_offset = hit_dist - wall_margin;
			if (drop_offset < 0.0f) {
				drop_offset = 0.0f;
			}
			if (drop_offset > default_drop_offset) {
				drop_offset = default_drop_offset;
			}
		}
	}

	// 앞으로 움직이며 무기를 버릴 때, 무기가 플레이어 이동을 전혀 이어받지 않으면
	// 손에서 떨어지는 순간 뒤에 남았다가 끊겨 보일 수 있다.
	move_x = drop_dir_x * speed + add_move_x;
	move_y = 0.0f;
	move_z = drop_dir_z * speed + add_move_z;

	// 단, 앞쪽에 벽이 있으면 무기를 앞으로 던지는 속도를 제거한다.
	// 그렇지 않으면 드랍 위치는 벽 앞이어도 다음 물리 프레임에서 벽을 뚫고 넘어갈 수 있다.
	if (blocked_by_wall == true) {
		move_x = 0.0f;
		move_z = 0.0f;
	}

	pos_x = x + drop_dir_x * drop_offset;
	pos_y = y + 16.0f + move_y;
	pos_z = z + drop_dir_z * drop_offset;
	rotation_x = rx + (float)M_PI;

	usingflag = false;
	motionflag = true;
}

//! @brief 뵯뺺
//! @return 맟뚻갌0?렪봲갌1
//! @attention 쁀롅듩둼귖뛩뿶궠귢귏궥갃
//! @attention 듫릶궕렪봲궥귡궻궼갂궋궦귢궔궻륆뙊궳궥갃?걏쁀롅듩둼궸뼖궫궶궋걐걏깏깓?긤렳뛱뭷걐걏뭙궕궶궋걐걏뵯롅뭙릶궕궶궋걐걏뼰뚼궶븧딇궻롰쀞궕먠믦궠귢궲궋귡걐
int weapon::Shot()
{
	//긏깋긚궕먠믦궠귢궲궋궶궚귢궽렪봲
	if( Param == NULL ){ return 1; }

	//뭙궕뼰궚귢궽렪봲
	if( Loadbullets == 0 ){ return 1; }

	//먠믦뭠귩롦벦
	WeaponParameter ParamData;
	int param_pellet;
	if( Param->GetWeapon(id_parameter, &ParamData) == 1 ){ return 1; }
	param_pellet = ParamData.pellet;

	//뵯롅궥귡뭙궻릶걁긻깒긞긣릶걂궕0뼟뼖궶귞렪봲
	if( param_pellet <= 0 ){ return 1; }

	// 수류탄 계열은 1개씩 던지고, 예비 탄이 남아 있으면 자동으로 다음 1개를 장전한다.
	if ((id_parameter == ID_WEAPON_GRENADE) ||
		(id_parameter == ID_WEAPON_IMPACT_GRENADE)) {
		//뭙귩뙵귞궥
		bullets -= 1;
		Loadbullets -= 1;

		if ((bullets - Loadbullets) <= 0) {		// 예비 탄이 없으면 무기 자체 제거
			EnableFlag = false;
			usingflag = false;
		}
		else if (Loadbullets <= 0) {			// 자동 장전
			Loadbullets = 1;
		}
		return 0;
	}

	//뭙귩뙵귞궥
	Loadbullets -= 1;
	bullets -= 1;
	return 0;
}

//! @brief 탄 소모 없이 발사 가능 여부만 검사
//! @return 성공 0, 실패 1
int weapon::ShotNoAmmoConsume()
{
	if (Param == NULL) { return 1; }

	// 탄창이 비어 있으면 발사 불가
	if (Loadbullets == 0) { return 1; }

	WeaponParameter ParamData;
	int param_pellet;

	if (Param->GetWeapon(id_parameter, &ParamData) == 1) { return 1; }

	param_pellet = ParamData.pellet;

	if (param_pellet <= 0) { return 1; }

	// 수류탄 계열은 무한 투척을 막기 위해 기존 Shot()을 사용한다.
	if ((id_parameter == ID_WEAPON_GRENADE) ||
		(id_parameter == ID_WEAPON_IMPACT_GRENADE)) {
		return Shot();
	}

	// 일반 총기는 탄을 줄이지 않고 성공 처리
	return 0;
}

//! @brief 깏깓?긤귩둎럑
//! @return 맟뚻갌0?렪봲갌1
//! @attention 깏깓?긤렄듩귖뛩뿶궠귢귏궥갃
//! @attention 듫릶궕렪봲궥귡궻궼갂궋궦귢궔궻륆뙊궳궥갃?걏뭙궕궶궋걐걏뼰뚼궶븧딇궻롰쀞궕먠믦궠귢궲궋귡걐
// [weapon.cpp 내부]
int weapon::StartReload()
{
	// 1. 기본 안전 검사
	if (Param == NULL) { return 1; }

	// 2. 예비 탄약 검사 (가진 총알이 총 안에 든 것과 같다면 = 여분 없음)
	// 여분 탄약이 없으면 어떤 무기도 장전 불가
	if ((bullets - Loadbullets) <= 0) { return 1; }

	// 3. 무기 데이터 가져오기
	WeaponParameter ParamData;
	// 데이터를 가져오지 못했으면 안전하게 기본값 처리하거나 실패 반환
	if (Param->GetWeapon(id_parameter, &ParamData) != 0) {
		return 1;
	}

	int nbsmax = ParamData.nbsmax;
	bool bHasChamber = ParamData.bHasChamber;
	bool isShellReload = ParamData.is_shell_reload;

	// ▼▼▼ [조건 분리] 무기 타입별 장전 허용 조건 ▼▼▼

	if (isShellReload == true)
	{
		// [TYPE A: 샷건/단발 장전 무기]
		// ---------------------------------------------------------
		// 특징: 탄창을 갈아끼우는 게 아니라 빈 공간에 채워 넣음.
		// 조건: 탄이 0발이어도, 1발 있어도, 꽉 차지 않았다면 언제든 장전 가능.
		// 약실(+1): 약실 기능이 있다면 (기본용량 + 1)발까지 들어감.
		// ---------------------------------------------------------

		int limit_capacity = nbsmax;
		if (bHasChamber) {
			limit_capacity += 1; // 약실이 있으면 무조건 한도 +1
		}

		// 현재 탄약이 한도에 도달했으면 장전 불가
		if (Loadbullets >= limit_capacity) {
			return 1;
		}
	}
	else
	{
		// [TYPE B: 탄창 교체식 일반 무기]
		// ---------------------------------------------------------
		// 특징: 탄창을 통째로 교환함.
		// 조건: 약실(+1)은 '현재 탄알이 남아있을 때'만 허용됨 (전술 재장전).
		// ---------------------------------------------------------

		int limit_capacity = nbsmax;

		// 전술 재장전 조건: (현재 탄알 있음) AND (약실 기능 있음)
		if (Loadbullets > 0 && bHasChamber) {
			limit_capacity += 1;
		}

		// 현재 탄약이 한도에 도달했으면 장전 불가
		if (Loadbullets >= limit_capacity) {
			return 1;
		}
	}

	return 0; // 장전 시작 승인
}

//! @brief 깏깓?긤귩렳뛱
//! @attention StartReload()듫릶궴댶궶귟갂뢷렄궸뭙귩뺚?궢귏궥갃깏깓?긤렄듩궼뛩뿶궠귢귏궧귪갃
int weapon::RunReload()
{
	// 클래스가 설정되어있지 않으면 실패
	if (Param == NULL) { return 1; }

	// 총알이 없으면 실패 (예비 탄약이 하나도 없다는 뜻)
	if ((bullets - Loadbullets) == 0) { return 1; }

	// 무기의 성능치에서 장전 탄수를 취득
	WeaponParameter ParamData;
	int nbsmax = 0;

	// [✨ 추가] 약실 기능이 있는지 확인할 변수
	bool bHasChamber = false;

	if (Param->GetWeapon(id_parameter, &ParamData) == 0) {
		nbsmax = ParamData.nbsmax;
		// [✨ 추가] 파라미터에서 값 가져오기 (parameter.h 수정 선행 필요)
		bHasChamber = ParamData.bHasChamber;
	}

	// ▼▼▼ [✨ 수정된 부분] 약실(+1) 로직 적용 ▼▼▼

	// 1. 목표 장탄수 설정 (기본값: 탄창 용량)
	int limit_capacity = nbsmax;

	// 2. 전술 재장전 조건: (현재 탄이 들어있고) AND (약실 기능이 있는 총)
	if (Loadbullets > 0 && bHasChamber) {
		limit_capacity += 1; // 30발 -> 31발로 목표 변경
	}

	// 3. 실제 장전 수행 (limit_capacity 사용)
	if (bullets < limit_capacity) {	// 가진 탄약이 목표치보다 적으면 (예: 남은 게 5발뿐)
		// bullets = (bullets - Loadbullets);
		Loadbullets = bullets;      // 가진 거 전부 장전
	}
	else {							// 가진 탄약이 충분하면
		// bullets -= Loadbullets;
		Loadbullets = limit_capacity; // 목표치(30 or 31)만큼 꽉 채움
	}

	// ▲▲▲ [수정 끝] ▲▲▲

	return 0;
}

//! @brief 븧딇궻롰쀞갋몧뭙릶궻빾뛛
//! @param Resource ResourceManager궻?귽깛?
//! @param id_param 롰쀞붥뜂
//! @param lnbs 몧뭙릶
//! @param nbs 뜃똶뭙릶
//! @return 맟뚻갌true?렪봲갌false
//! @attention 긵깒귽깂?궸귝귡뿞땆걁F6갋F7걂뾭궸뾭댰궠귢궫듫릶궳궥갃롨왮뭙궕멗묖궠귢궫뤾뜃갂렔벍밒궸뭙귩뺚?궢귏궥갃
//! @attention 럊뾭궠귢궲궋궶궋븧딇긆긳긙긃긏긣궸뫮궢궲렳뛱궥귡궴갂궞궻듫릶궼렪봲궢귏궥갃
bool weapon::ResetWeaponParam(class ResourceManager *Resource, int id_param, int lnbs, int nbs)
{
	//룊딖돸궠귢궲궋궶궚귢궽갂렪봲
	if( EnableFlag == false ){ return false; }
	if( Resource == NULL ){ return false; }

	//럚믦궠귢궫먠믦뭠귉뤵룕궖
	id_parameter = id_param;
	bullets = nbs;
	Loadbullets = lnbs;

	//귖궢롨왮뭙궶귞궽갂렔벍깏깓?긤
	if( id_param == ID_WEAPON_GRENADE ){
		if( (bullets > 0)&&(Loadbullets == 0) ){
			Loadbullets = 1;
		}
	}

	//긾긢깑궴긡긏긚?긿귩먠믦
	Resource->GetWeaponModelTexture(id_param, &id_model, &id_texture);
	WeaponParameter param;
	if( Param->GetWeapon(id_param, &param) == 0 ){
		model_size = param.size;
	}

	return true;
}

void weapon::SetRenderTransformOverride(float x, float y, float z, float rx)
{
	render_transform_override = true;
	render_pos_x = x;
	render_pos_y = y;
	render_pos_z = z;
	render_rotation_x = rx;
}

void weapon::ClearRenderTransformOverride()
{
	render_transform_override = false;
}

int weapon::GetRenderGeneration()
{
	return render_generation;
}

//! @brief 똶럁귩렳뛱걁렔뾕뿇돷걂
//! @param CollD Collision궻?귽깛?
int weapon::ProcessObject(class Collision* CollD)
{
	//긏깋긚궕먠믦궠귢궲궋궶궚귢궽렪봲
	if (CollD == NULL) { return 0; }

	//룊딖돸궠귢궲궋궶궚귢궽갂렪봲
	if (EnableFlag == false) { return 0; }

	//묿궸귖럊귦귢궲궓귞궦갂댷벍긲깋긐궕뾎뚼궶귞궽?
	if( (usingflag == false)&&(motionflag == true) ){
		float Dist;
		float maxDist;

		//댷벍뫊뱗귩뛛륷
		move_x *= 0.96f;
		move_z *= 0.96f;
		if( move_y > -1.8f ){
			move_y -= 0.3f;
		}

		//긳깓긞긏궸뻹귏궯궲궋귢궽룉뿚궢궶궋
		if( CollD->CheckALLBlockInside(pos_x, pos_y, pos_z) == true ){
			motionflag = false;
			return 0;
		}

		//릣빟궻댷벍뫊뱗귩땫귕귡
		maxDist = sqrtf(move_x*move_x + move_z*move_z);
		if( maxDist < 0.1f ){
			maxDist = 0.0f;
			move_x = 0.0f;
			move_z = 0.0f;
		}

		//긳깓긞긏궸먝궢궲궋귢궽갂궩귢댥뤵궼릣빟댷벍궢궶궋갃
		if( maxDist > 0.0f ){
			if( CollD->CheckALLBlockIntersectDummyRay(pos_x, pos_y, pos_z, move_x/maxDist, 0, move_z/maxDist, NULL, NULL, &Dist, maxDist) == true ){
				//긳깓긞긏궸뻹귏귞궶궋귝궎궸롨멟궸
				Dist -= 0.1f;

				//먝궢궲궋귡띆뷭귏궳댷벍
				pos_x += move_x/maxDist * Dist;
				pos_z += move_z/maxDist * Dist;

				//댷벍쀊귩 0 궸
				move_x = 0.0f;
				move_z = 0.0f;
			}
		}

		//뿇돷룉뿚
		for(int i=0; i<2; i++){
			//긳깓긞긏궸뻹귏궯궲궋귢궽갂궩궞궳룉뿚뢎뿹
			if( CollD->CheckALLBlockInside(pos_x, pos_y + move_y*0.5f, pos_z) == true ){
				motionflag = false;
				return 0;
			}

			//뿇돷궠궧귡
			pos_y += move_y*0.5f;
		}

		//릣빟뺴뛀궻띆뷭귩뵿뎕
		pos_x += move_x;
		pos_z += move_z;
	}

	return 0;
}

//! @brief ?됪
//! @param d3dg D3DGraphics궻?귽깛?
//! @param NoModel 긾긢깑?됪뼰뚼돸
void weapon::Render(class D3DGraphics* d3dg, bool NoModel)
{
	if (d3dg == NULL) { return; }

	if (EnableFlag == false) { return; }
	if (usingflag == true) { return; }

	float draw_pos_x = render_transform_override ? render_pos_x : pos_x;
	float draw_pos_y = render_transform_override ? render_pos_y : pos_y;
	float draw_pos_z = render_transform_override ? render_pos_z : pos_z;
	float draw_rotation_x = render_transform_override ? render_rotation_x : rotation_x;

	d3dg->SetWorldTransform(draw_pos_x, draw_pos_y, draw_pos_z, draw_rotation_x, 0.0f, (float)M_PI / 2, model_size);
	d3dg->RenderModel(id_model, id_texture, DarkModelFlag, false, NoModel);
}

//! @brief 긓깛긚긣깋긏?
smallobject::smallobject(class ParameterInfo *in_Param, class MIFInterface *in_MIFdata, float x, float y, float z, float rx, int id_param, signed short int p4, bool flag)
{
	Param = in_Param;
	MIFdata = in_MIFdata;
	pos_x = x;
	pos_y = y;
	pos_z = z;
	rotation_x = rx;
	rotation_y = 0.0f;
	EnableFlag = flag;
	model_size = 5.0f;

	id_parameter = id_param;
	point_p4 = p4;

	hp = 0;		//럃믦
	jump_rx = 0.0f;
	move_rx = 0.0f;
	add_rx = 0.0f;
	add_ry = 0.0f;
	jump_cnt = 0;

	render_transform_override = false;
	render_pos_x = x;
	render_pos_y = y;
	render_pos_z = z;
	render_rotation_x = rx;
	render_rotation_y = 0.0f;
	render_generation = 0;

	if( Param != NULL ){
		SmallObjectParameter ParamData;
		if( Param->GetSmallObject(id_param, &ParamData) == 0 ){
			hp = ParamData.hp;
		}
	}
	id_model = -1;
	id_texture = -1;
}

//! @brief 긢귻긚긣깋긏?
smallobject::~smallobject()
{}

//! @brief MIF긢??귩듖뿚궥귡긏깋긚귩먠믦
//! @param in_MIFdata MIFInterface긏깋긚궻?귽깛?
void smallobject::SetMIFInterfaceClass(class MIFInterface *in_MIFdata)
{
	MIFdata = in_MIFdata;
}

//! @brief 먠믦뭠귩먠믦
//! @param id_param 룷븿궻롰쀞붥뜂
//! @param p4 묉4긬깋긽??
//! @param init 긆긳긙긃긏긣귩룊딖돸
void smallobject::SetParamData(int id_param, signed short int p4, bool init)
{
	rotation_y = 0.0f;
	id_parameter = id_param;
	point_p4 = p4;

	if( init == true ){
		hp = 0;		//럃믦
		render_transform_override = false;
		render_generation += 1;
		jump_rx = 0.0f;
		move_rx = 0.0f;
		add_rx = 0.0f;
		add_ry = 0.0f;
		jump_cnt = 0;

		if( (TOTAL_PARAMETERINFO_SMALLOBJECT <= id_param)&&(id_param <= (TOTAL_PARAMETERINFO_SMALLOBJECT + MAX_ADDSMALLOBJECT -1)) ){
			if( MIFdata != NULL ){
				hp = MIFdata->GetAddSmallobjectHP(id_param - TOTAL_PARAMETERINFO_SMALLOBJECT);
			}
		}
		else{
			if( Param != NULL ){
				SmallObjectParameter ParamData;
				if( Param->GetSmallObject(id_param, &ParamData) == 0 ){
					hp = ParamData.hp;
				}
			}
		}
	}
}

//! @brief 먠믦뭠귩롦벦
//! @param id_param 룷븿궻롰쀞붥뜂귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param p4 묉4긬깋긽??귩롷궚롦귡?귽깛?걁NULL됀걂
void smallobject::GetParamData(int *id_param, signed short int *p4)
{
	if( id_param != NULL ){ *id_param = id_parameter; }
	if( p4 != NULL ){ *p4 = point_p4; }
}

//! @brief 뫬쀍귩롦벦
//! @return 뫬쀍뭠
int smallobject::GetHP()
{
	return hp;
}

float smallobject::GetRotationY()
{
	return rotation_y;
}

//! @brief 긳깓긞긏궻뤵궸댷벍
//! @param CollD Collision궻?귽깛?
//! @return 뙰궻띆뷭궔귞궻댷벍쀊걁0궳댷벍궶궢걂
//! @attention 긳깓긞긏궸뻹귏궯궲궋귡뤾뜃궳귖갂돷궻긳깓긞긏귏궳 궥귟뵴궚귏궥갃
float smallobject::CollisionMap(class Collision *CollD)
{
	//긏깋긚궕먠믦궠귢궲궋궶궚귢궽렪봲
	if( CollD == NULL ){ return 0.0f; }

	float Dist;
	SmallObjectParameter ParamData;

	//돷뺴뛀궸뱰궫귟뵽믦
	if( CollD->CheckALLBlockIntersectRay(pos_x, pos_y + COLLISION_ADDSIZE, pos_z, 0, -1, 0, NULL, NULL, &Dist, 1000.0f) == true ){
		//뱰궫귟뵽믦궻묈궖궠귩롦벦
		if( (TOTAL_PARAMETERINFO_SMALLOBJECT <= id_parameter)&&(id_parameter <= (TOTAL_PARAMETERINFO_SMALLOBJECT + MAX_ADDSMALLOBJECT -1)) ){
			Dist -= (float)MIFdata->GetAddSmallobjectDecide(id_parameter - TOTAL_PARAMETERINFO_SMALLOBJECT)/10.0f;
		}
		else{
			if( Param->GetSmallObject(id_parameter, &ParamData) == 0 ){
				Dist -= (float)ParamData.decide/10.0f;
			}
		}

		//띆뷭귩댷벍궥귡
		pos_y -= Dist;
		return Dist;
	}

	return 0.0f;
}

//! @brief 뭙궕긭긞긣궢궫
//! @param attacks 뭙궻뛘똼쀍
void smallobject::HitBullet(int attacks)
{
	hp -= attacks;
	if( hp <= 0 ){
		Destruction();
	}
}

//! @brief 롨왮뭙궻뵚븮궕긭긞긣궢궫
//! @param attacks 뵚븮궻뛘똼쀍
//! @attention 떁뿣궸귝귡똶럁귩럷멟궸띙귏궧궲궘궬궠궋갃
void smallobject::HitGrenadeExplosion(int attacks)
{
	hp -= attacks;
	if( hp <= 0 ){
		Destruction();
	}
}

//! @brief 룷븿귩봨됹궥귡
//! @attention 믅륂궼 HitBullet()듫릶 궓귝귂 GrenadeExplosion()듫릶 궔귞렔벍밒궸렳뛱궠귢귡궫귕갂뮳먝뚁귂뢯궥뷠뾴궼궇귟귏궧귪갃
void smallobject::Destruction()
{
	//EnableFlag = false;
	//return;

	int jump;

	//뷅귂뗰뜃귩롦벦
	if( (TOTAL_PARAMETERINFO_SMALLOBJECT <= id_parameter)&&(id_parameter <= (TOTAL_PARAMETERINFO_SMALLOBJECT + MAX_ADDSMALLOBJECT -1)) ){
		jump = MIFdata->GetAddSmallobjectJump(id_parameter - TOTAL_PARAMETERINFO_SMALLOBJECT);
	}
	else{
		SmallObjectParameter paramdata;
		Param->GetSmallObject(id_parameter, &paramdata);
		jump = paramdata.jump;
	}

	//뷅궽궥
	hp = 0;
	jump_cnt = jump;

	//럓맖먠믦
	jump_rx = DegreeToRadian(10) * GetRand(36);
	move_rx = (float)jump * 0.04243f;
	add_rx = DegreeToRadian(1) * GetRand(20);
	add_ry = DegreeToRadian(1) * GetRand(20);
}

void smallobject::SetRenderTransformOverride(float x, float y, float z, float rx, float ry)
{
	render_transform_override = true;
	render_pos_x = x;
	render_pos_y = y;
	render_pos_z = z;
	render_rotation_x = rx;
	render_rotation_y = ry;
}

void smallobject::ClearRenderTransformOverride()
{
	render_transform_override = false;
}

int smallobject::GetRenderGeneration()
{
	return render_generation;
}

//! @brief 똶럁귩렳뛱걁봨됹렄궻댷벍궶궵걂
int smallobject::ProcessObject()
{
	//?됪궠귢궲궋궶궋궔갂뫬쀍궕럄궯궲궋궶궚귢궽룉뿚궢궶궋갃
	if( EnableFlag == false ){ return 0; }
	if( hp > 0 ){ return 0; }

	int cnt;
	int jump;

	//뷅귂뗰뜃귩롦벦
	if( (TOTAL_PARAMETERINFO_SMALLOBJECT <= id_parameter)&&(id_parameter <= (TOTAL_PARAMETERINFO_SMALLOBJECT + MAX_ADDSMALLOBJECT -1)) ){
		jump = MIFdata->GetAddSmallobjectJump(id_parameter - TOTAL_PARAMETERINFO_SMALLOBJECT);
	}
	else{
		SmallObjectParameter paramdata;
		Param->GetSmallObject(id_parameter, &paramdata);
		jump = paramdata.jump;
	}
	
	//릟궖뷅귪궳궋귡긇긂깛긣릶귩똶럁
	cnt = jump - jump_cnt;

	//럓맖궔귞띆뷭갋둷뱗귩똶럁
	pos_x += cosf(jump_rx) * move_rx;
	pos_y += jump_cnt * 0.1f;
	pos_z += sinf(jump_rx) * move_rx;
	rotation_x += add_rx;
	rotation_y += add_ry;

	jump_cnt -= 1;

	//1뷳뷅귪궳궋궫귞?됪뢎뿹
	if( cnt > (int)GAMEFPS ){
		EnableFlag = false;
		return 2;
	}

	return 1;
}


//! @brief ?됪
//! @param d3dg D3DGraphics궻?귽깛?
//! @param NoModel 긾긢깑?됪뼰뚼돸
void smallobject::Render(D3DGraphics* d3dg, bool NoModel)
{
	if (d3dg == NULL) { return; }

	if (EnableFlag == false) { return; }

	float draw_pos_x = render_transform_override ? render_pos_x : pos_x;
	float draw_pos_y = render_transform_override ? render_pos_y : pos_y;
	float draw_pos_z = render_transform_override ? render_pos_z : pos_z;
	float draw_rotation_x = render_transform_override ? render_rotation_x : rotation_x;
	float draw_rotation_y = render_transform_override ? render_rotation_y : rotation_y;

	d3dg->SetWorldTransform(draw_pos_x, draw_pos_y, draw_pos_z, draw_rotation_x, draw_rotation_y, model_size);
	d3dg->RenderModel(id_model, id_texture, DarkModelFlag, false, NoModel);
}

//! @brief 긓깛긚긣깋긏?
bullet::bullet(int modelid, int textureid)
{
	model_size = 1.0f;
	id_model = modelid;
	id_texture = textureid;
	EnableFlag = false;

	rotation_y = 0.0f;

	attacks = 0;
	penetration = 0;
	speed = 0.0f;
	teamid = 0;
	humanid = 0;
	ontargetcnt = 0.0f;
	silencer = false;
	cnt = 0;
}

//! @brief 긢귻긚긣깋긏?
bullet::~bullet()
{}

//! @brief 띆뷭궴둷뱗귩먠믦
//! @param x X띆뷭
//! @param y Y띆뷭
//! @param z Z띆뷭
//! @param rx 돘렡됷?
//! @param ry 뢢렡됷?
void bullet::SetPosData(float x, float y, float z, float rx, float ry)
{
	pos_x = x;
	pos_y = y;
	pos_z = z;
	rotation_x = rx;
	rotation_y = ry;

}

//! @brief 먠믦뭠귩먠믦
//! @param _attacks 뛘똼쀍
//! @param _penetration 듥믅쀍
//! @param _speed 뭙뫊
//! @param _teamid ???붥뜂
//! @param _humanid 릐궻긢??붥뜂
//! @param _ontargetcnt 뼺뭷렄궻긇긂깛긣릶
//! @param init 긆긳긙긃긏긣귩룊딖돸
void bullet::SetParamData(int _attacks, int _penetration, float _speed, int _teamid, int _humanid, float _ontargetcnt, bool init)
{
	attacks = _attacks;
	penetration = _penetration;
	speed = _speed;
	teamid = _teamid;
	humanid = _humanid;
	ontargetcnt = _ontargetcnt;

	if (init == true) {
		silencer = false;
		cnt = 0;
	}
}

//! @brief 띆뷭궴둷뱗귩롦벦
//! @param x X띆뷭귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param y Y띆뷭귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param z Z띆뷭귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param rx 돘렡됷?귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param ry 뢢렡됷?귩롷궚롦귡?귽깛?걁NULL됀걂
void bullet::GetPosData(float *x, float *y, float *z, float *rx, float *ry)
{
	if( x != NULL ){ *x = pos_x; }
	if( y != NULL ){ *y = pos_y; }
	if( z != NULL ){ *z = pos_z; }
	if( rx != NULL ){ *rx = rotation_x; }
	if( ry != NULL ){ *ry = rotation_y; }
}

//! @brief 먠믦뭠귩롦벦
//! @param _attacks 뛘똼쀍귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param _penetration 듥믅쀍귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param _speed 뭙뫊귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param _teamid ???붥뜂귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param _humanid 릐궻긢??붥뜂귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param _ontargetcnt 뼺뭷렄궻긇긂깛긣릶귩롷궚롦귡?귽깛?걁NULL됀걂
void bullet::GetParamData(int* _attacks, int* _penetration, float* _speed, int* _teamid, int* _humanid, float* _ontargetcnt)
{
	if (_attacks != NULL) { *_attacks = attacks; }
	if (_penetration != NULL) { *_penetration = penetration; }
	if (_speed != NULL) { *_speed = speed; }
	if (_teamid != NULL) { *_teamid = teamid; }
	if (_humanid != NULL) { *_humanid = humanid; }
	if (_ontargetcnt != NULL) { *_ontargetcnt = ontargetcnt; }
}

int bullet::GetLifeCount()
{
	return cnt;
}

void bullet::SetSilencerFlag(bool flag)
{
	silencer = flag;
}

bool bullet::GetSilencerFlag()
{
	return silencer;
}

//! @brief 똶럁귩렳뛱걁뭙궻릋뛱갋렄듩뤑뽅걂
int bullet::ProcessObject()
{
	//룊딖돸궠귢궲궋궶궚귢궽룉뿚궢궶궋
	if( EnableFlag == false ){ return 0; }

	//뤑뽅렄듩귩됡궗궲궋귢궽갂긆긳긙긃긏긣귩뼰뚼돸
	if( cnt > BULLET_DESTROYFRAME ){
		EnableFlag = false;
		return 0;
	}

	//댷벍룉뿚
	pos_x += cosf(rotation_x)*cosf(rotation_y)*speed;
	pos_y += sinf(rotation_y)*speed;
	pos_z += sinf(rotation_x)*cosf(rotation_y)*speed;
	cnt += 1;

	return 0;
}

//! @brief ?됪
//! @param d3dg D3DGraphics궻?귽깛?
//! @param NoModel 긾긢깑?됪뼰뚼돸
void bullet::Render(class D3DGraphics* d3dg, bool NoModel)
{
	if (d3dg == NULL) { return; }

	if (EnableFlag == false) { return; }

	if (cnt == 0) { return; }

	// 총알은 판정 오브젝트이므로 144Hz 렌더 보간 대상에서 제외한다.
	// 현재 로직 프레임의 좌표만 사용한다.
	float render_pos_x = pos_x;
	float render_pos_y = pos_y;
	float render_pos_z = pos_z;
	float render_rotation_x = rotation_x;

	float render_rotation_y = rotation_y;

	d3dg->SetWorldTransform(
		render_pos_x,
		render_pos_y,
		render_pos_z,
		(render_rotation_x * -1 - (float)M_PI / 2),
		render_rotation_y,
		model_size
	);

	d3dg->RenderModel(id_model, id_texture, false, false, NoModel);
}

//! @brief 긓깛긚긣깋긏?
grenade::grenade(int modelid, int textureid) : bullet(modelid, textureid)
{
	if( Param != NULL ){
		WeaponParameter ParamData;
		if( Param->GetWeapon(ID_WEAPON_GRENADE, &ParamData) == 0 ){
			model_size = ParamData.size;
		}
	}

	move_x = 0.0f;
	move_y = 0.0f;
	move_z = 0.0f;
	weapon_paramid = ID_WEAPON_GRENADE;

	render_transform_override = false;
	render_pos_x = 0.0f;
	render_pos_y = 0.0f;
	render_pos_z = 0.0f;
	render_rotation_x = 0.0f;
	render_rotation_y = 0.0f;
}

//! @brief 긢귻긚긣깋긏?
grenade::~grenade()
{}

//! @brief 띆뷭궴륃뺪귩먠믦
//! @param speed 룊뫊
//! @param _teamid ???붥뜂
//! @param _humanid 릐궻긢??붥뜂
//! @param _ontargetcnt 뼺뭷렄궻긇긂깛긣릶
//! @param init 긆긳긙긃긏긣귩룊딖돸
//! @attention 먩궸 SetPosData() 귩렳뛱궢궲궘궬궠궋갃
void grenade::SetParamData(float speed, int _teamid, int _humanid, float _ontargetcnt, int _weapon_paramid, bool init)
{
	move_x = cosf(rotation_x) * cosf(rotation_y) * speed;
	move_y = sinf(rotation_y) * speed;
	move_z = sinf(rotation_x) * cosf(rotation_y) * speed;
	teamid = _teamid;
	humanid = _humanid;
	ontargetcnt = _ontargetcnt;
	weapon_paramid = _weapon_paramid;

	if (init == true) {
		cnt = 0;
	}
}

//! @brief 먠믦뭠귩롦벦
//! @param _speed 뫊뱗귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param _teamid ???붥뜂귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param _humanid 릐궻긢??붥뜂귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param _ontargetcnt 뼺뭷렄궻긇긂깛긣릶귩롷궚롦귡?귽깛?걁NULL됀걂
void grenade::GetParamData(float* _speed, int* _teamid, int* _humanid, float* _ontargetcnt)
{
	if (_speed != NULL) { *_speed = GetSpeed(); }
	if (_teamid != NULL) { *_teamid = teamid; }
	if (_humanid != NULL) { *_humanid = humanid; }
	if (_ontargetcnt != NULL) { *_ontargetcnt = ontargetcnt; }
}

int grenade::GetWeaponParamID()
{
	return weapon_paramid;
}

//! @brief 뫊뱗귩롦벦
//! @return 뫊뱗
float grenade::GetSpeed()
{
	return sqrtf(move_x*move_x + move_y*move_y + move_z*move_z);
}

// 렌더링 전용 변환 설정. 실제 수류탄 좌표와 충돌 판정에는 영향을 주지 않는다.
void grenade::SetRenderTransformOverride(float x, float y, float z, float rx, float ry)
{
	render_transform_override = true;
	render_pos_x = x;
	render_pos_y = y;
	render_pos_z = z;
	render_rotation_x = rx;
	render_rotation_y = ry;
}

void grenade::ClearRenderTransformOverride()
{
	render_transform_override = false;
}

//! @brief 똶럁귩렳뛱걁롨왮뭙궻릋뛱갋뵚뵯걂
//! @return 뵚뵯갌2?긫긂깛긤갋뮫궺뺅귟갌1?궩귢댥둖갌0
int grenade::ProcessObject(class Collision* CollD)
{
	if (CollD == NULL) { return 0; }
	if (EnableFlag == false) { return 0; }
	if (cnt > GRENADE_DESTROYFRAME) {
		EnableFlag = false;
		return 2;
	}
	if ((move_x == 0.0f) && (move_y == 0.0f) && (move_z == 0.0f)) {
		cnt += 1;
		return 0;
	}
	int id, face;
	float Dist;
	float maxDist = sqrtf(move_x * move_x + move_y * move_y + move_z * move_z);
	if (CollD->CheckALLBlockIntersectRay(pos_x, pos_y, pos_z, move_x / maxDist, move_y / maxDist, move_z / maxDist, &id, &face, &Dist, maxDist) == true) {

		// Impact grenade는 벽/맵 충돌 시 튕기지 않고 즉시 폭발한다.
		// 폭발 위치가 충돌 전 위치가 되지 않도록 충돌 지점으로 위치를 보정한다.
		if (weapon_paramid == ID_WEAPON_IMPACT_GRENADE) {
			pos_x += (move_x / maxDist) * Dist;
			pos_y += (move_y / maxDist) * Dist;
			pos_z += (move_z / maxDist) * Dist;

			EnableFlag = false;
			return 2;
		}

		float Angle, Acceleration;
		float vx, vy, vz;
		CollD->AngleVector(id, face, move_x, move_y, move_z, &Angle);
		CollD->ReflectVector(id, face, move_x, move_y, move_z, &vx, &vy, &vz);
		Acceleration = Angle * -1 * 0.2546f + 0.7f;
		move_x = vx * Acceleration;
		move_y = vy * Acceleration;
		move_z = vz * Acceleration;
		move_x *= 0.98f;
		move_y = (move_y - 0.17f) * 0.98f;
		move_z *= 0.98f;
		cnt += 1;
		return 1;
	}
	pos_x += move_x;
	pos_y += move_y;
	pos_z += move_z;
	move_x *= 0.98f;
	move_y = (move_y - 0.17f) * 0.98f;
	move_z *= 0.98f;
	rotation_x += 0.1f;
	rotation_y += 0.08f;
	cnt += 1;
	return 0;
}

//! @brief ?됪
//! @param d3dg D3DGraphics궻?귽깛?
//! @param NoModel 긾긢깑?됪뼰뚼돸
void grenade::Render(class D3DGraphics* d3dg, bool NoModel)
{
	if (d3dg == NULL) { return; }
	if (EnableFlag == false) { return; }

	// 실제 충돌 좌표는 그대로 두고 렌더링에서만 보간된 변환을 사용한다.
	float draw_pos_x = render_transform_override ? render_pos_x : pos_x;
	float draw_pos_y = render_transform_override ? render_pos_y : pos_y;
	float draw_pos_z = render_transform_override ? render_pos_z : pos_z;
	float draw_rotation_x = render_transform_override ? render_rotation_x : rotation_x;
	float draw_rotation_y = render_transform_override ? render_rotation_y : rotation_y;

	d3dg->SetWorldTransform(
		draw_pos_x,
		draw_pos_y,
		draw_pos_z,
		(draw_rotation_x * -1 - (float)M_PI / 2),
		draw_rotation_y,
		model_size
	);

	d3dg->RenderModel(id_model, id_texture, DarkModelFlag, false, NoModel, true);
}

//! @brief 긓깛긚긣깋긏?
effect::effect(float x, float y, float z, float size, float rotation, int texture, int count)
{
	pos_x = x;
	pos_y = y;
	pos_z = z;
	move_x = 0.0f;
	move_y = 0.0f;
	move_z = 0.0f;
	addmove_y = 0.0f;
	rotation_x = 0.0f;
	rotation_y = 0.0f;
	rotation_texture = rotation;
	addrotation_texture = 0.0f;
	model_size = size;
	addmodel_size = 0.0f;
	id_texture = texture;
	brightness = 0;
	addbrightness = 0;
	alpha = 1.0f;
	add_alpha = 0.0f;
	cnt = count;
	type = EFFECT_NORMAL;

	render_state_override = false;
	render_pos_x = pos_x;
	render_pos_y = pos_y;
	render_pos_z = pos_z;
	render_rotation_texture = rotation_texture;
	render_model_size = model_size;
	render_alpha = alpha;
	render_generation = 0;

	if( cnt > 0 ){
		EnableFlag = true;
	}
	else{
		EnableFlag = false;
	}
}

//! @brief 긢귻긚긣깋긏?
effect::~effect()
{}

//! @brief 먠믦뭠귩먠믦
//! @param in_move_x X렡댷벍쀊
//! @param in_move_y Y렡댷벍쀊
//! @param in_move_z Z렡댷벍쀊
//! @param in_addmove_y Y렡댷벍쀊궻돿럁쀊
//! @param rotation 됷?둷뱗
//! @param addrotation 됷?둷뱗궻돿럁쀊
//! @param size ?됪?뿦
//! @param addsize ?됪?뿦
//! @param count 긇긂깛긣
//! @param settype 긄긲긃긏긣궻롰쀞?걁Effect_Type 귩멺귒뜃궧귡걂
void effect::SetParamData(float in_move_x, float in_move_y, float in_move_z, float in_addmove_y, float rotation, float addrotation, float size, float addsize, int count, int settype)
{
	move_x = in_move_x;
	move_y = in_move_y;
	move_z = in_move_z;
	addmove_y = in_addmove_y;
	rotation_x = addrotation;
	rotation_y = 0.0f;
	rotation_texture = rotation;
	addrotation_texture = addrotation;
	model_size = size;
	addmodel_size = addsize;
	cnt = count;
	type = settype;

	// Every SetParamData call starts a new visual effect in this pool slot.
	// The generation value prevents interpolation from an older effect.
	render_generation += 1;
	render_state_override = false;
}

//! @brief 긡긏긚?긿귩먠믦
//! @param id 긡긏긚?긿궻긢??붥뜂
//! @param in_brightness 뼻귡궠
//! @param in_addbrightness 뼻귡궠궻돿럁쀊
//! @param in_alpha 벁뼻뱗
//! @param in_addalpha 벁뼻뱗궻돿럁쀊
void effect::SetTexture(int id, int in_brightness, int in_addbrightness, float in_alpha, float in_addalpha)
{
	id_texture = id;
	brightness = in_brightness;
	addbrightness = in_addbrightness;
	alpha = in_alpha;
	add_alpha = in_addalpha;
}

//! @brief 돘렡궴뢢렡궻뛀궖귩먠믦
//! @param rx 먠믦궥귡돘렡
//! @param ry 먠믦궥귡뢢렡
void effect::SetRxRy(float rx, float ry)
{
	rotation_x = rx;
	rotation_y = ry;
}

//! 긡긏긚?긿궻긢??붥뜂귩롦벦
//! @return 긡긏긚?긿궻긢??붥뜂
int effect::GetTextureID()
{
	return id_texture;
}

//! 긄긲긃긏긣궻댷벍쀊귩롦벦
//! @param mx X렡댷벍쀊귩롷궚롦귡?귽깛?
//! @param my Y렡댷벍쀊귩롷궚롦귡?귽깛?
//! @param mz Z렡댷벍쀊귩롷궚롦귡?귽깛?
void effect::GetMove(float *mx, float *my, float *mz)
{
	if( (mx == NULL)||(my == NULL)||(mz == NULL) ){ return; }

	*mx = move_x;
	*my = move_y;
	*mz = move_z;
}

//! 긄긲긃긏긣궻?긞긵뱰궫귟뵽믦긲깋긐롦벦
//! @return 뾎뚼갌true?뼰뚼갌false
bool effect::GetCollideMapFlag()
{
	if( type&EFFECT_COLLIDEMAP ){
		return true;
	}
	//else{
		return false;
	//}
}

void effect::GetRenderState(float* rotation, float* size, float* out_alpha, int* generation)
{
	if (rotation != NULL) { *rotation = rotation_texture; }
	if (size != NULL) { *size = model_size; }
	if (out_alpha != NULL) { *out_alpha = alpha; }
	if (generation != NULL) { *generation = render_generation; }
}

void effect::SetRenderStateOverride(float x, float y, float z, float rotation, float size, float in_alpha)
{
	render_state_override = true;
	render_pos_x = x;
	render_pos_y = y;
	render_pos_z = z;
	render_rotation_texture = rotation;
	render_model_size = size;
	render_alpha = in_alpha;
}

void effect::ClearRenderStateOverride()
{
	render_state_override = false;
}

//! @brief 똶럁귩렳뛱걁긄긲긃긏긣댷벍갋렄듩뤑뽅걂
//! @return 룉뿚렳뛱갌1??됪띍뢎긲깒??갌2?룉뿚궶궢갌0
int effect::ProcessObject()
{
	//룊딖돸궠귢궲궋궶궚귢궽룉뿚궢궶궋
	if( EnableFlag == false ){ return 0; }

	//긇긂깛긣궕뢎뿹궢궲궋귢궽갂룉뿚궢궶궋귝궎궸먠믦
	if( cnt <= 0 ){
		EnableFlag = false;
		return 2;
	}

	//띆뷭댷벍
	pos_x += move_x;
	pos_y += move_y;
	pos_z += move_z;
	move_y += addmove_y;

	//됷?
	rotation_texture += addrotation_texture;

	//둮묈
	model_size += addmodel_size;

	//뼻귡궠궴벁뼻뱗
	brightness += addbrightness;
	alpha += add_alpha;

	//벁뼻뱗궕?귽긥긚궸궶궯궫귞갂룉뿚궢궶궋귝궎궸먠믦
	if( alpha <= 0.0f ){
		EnableFlag = false;
		cnt = 0;
		return 2;
	}

	//긇긂깛긣궕 255 뼟뼖궶귞궽갂긇긂깛긣귩 1 덙궘
	if( cnt < 255 ){
		cnt -= 1;
	}
	return 1;
}

//! @brief ?됪
//! @param d3dg D3DGraphics궻?귽깛?
//! @param camera_rx 긇긽깋궻돘렡둷뱗
//! @param camera_ry 긇긽깋궻뢢렡둷뱗
//! @param NoModel 긾긢깑?됪뼰뚼돸
void effect::Render(class D3DGraphics *d3dg, float camera_rx, float camera_ry, bool NoModel)
{
	//긏깋긚궕먠믦궠귢궲궋궶궚귢궽렪봲
	if( d3dg == NULL ){ return; }

	//룊딖돸궠귢궲궋궶궚귢궽룉뿚궢궶궋갃
	if( EnableFlag == false ){ return; }

	if( (type & EFFECT_NOBILLBOARD) == 0 ){
		//긇긽깋띆뷭귩밙뾭궢갂긮깑??긤돸궥귡
		rotation_x = camera_rx*-1;
		rotation_y = camera_ry;
	}

	float draw_pos_x = render_state_override ? render_pos_x : pos_x;
	float draw_pos_y = render_state_override ? render_pos_y : pos_y;
	float draw_pos_z = render_state_override ? render_pos_z : pos_z;
	float draw_rotation_texture = render_state_override ? render_rotation_texture : rotation_texture;
	float draw_model_size = render_state_override ? render_model_size : model_size;
	float draw_alpha = render_state_override ? render_alpha : alpha;

	//?됪
	d3dg->SetWorldTransformEffect(
		draw_pos_x,
		draw_pos_y,
		draw_pos_z,
		rotation_x,
		rotation_y,
		draw_rotation_texture,
		draw_model_size
	);
	d3dg->RenderBoard(id_texture, draw_alpha, NoModel);
}

//! @brief 긓깛긚긣깋긏?
HumanMotionControl::HumanMotionControl(class ParameterInfo *in_Param)
{
	Param = in_Param;

	id_upmodel = -1;
	for(int i=0; i<TOTAL_ARMMODE; i++){
		id_armmodel[i] = -1;
	}
	id_legmodel = -1;
	for(int i=0; i<TOTAL_WALKMODE; i++){
		id_walkmodel[i] = -1;
	}
	for(int i=0; i<TOTAL_RUNMODE; i++){
		id_runmodel[i] = -1;
	}

	reaction_y = 0.0f;
	slowarm = false;
	armmodel_rotation_y = 0.0f;
	legrotation_x = 0.0f;
	walkcnt = 0;
	runcnt = 0;
	armmodelid = -1;
	legmodelid = -1;
}

//! @brief 긢귻긚긣깋긏?
HumanMotionControl::~HumanMotionControl()
{}

//! @brief 먠믦뭠귩듖뿚궥귡긏깋긚귩뱋?
//! @attention 둫듫릶귩럊뾭궥귡멟궸렳뛱궥귡궞궴갃
void HumanMotionControl::SetParameterInfoClass(class ParameterInfo *in_Param)
{
	Param = in_Param;
}

//! @brief 긾긢깑긢??귩먠믦
//! @param upmodel 뤵뵾릆궻긾긢깑
//! @param armmodel[] 쁱궻긾긢깑궻봹쀱걁봹쀱릶갌TOTAL_ARMMODE걂
//! @param legmodel 뫉걁먄?륉뫴걂궻긾긢깑
//! @param walkmodel[] 쁱궻긾긢깑궻봹쀱걁봹쀱릶갌TOTAL_WALKMODE걂
//! @param runmodel[] 쁱궻긾긢깑궻봹쀱걁봹쀱릶갌TOTAL_RUNMODE걂
void HumanMotionControl::SetModel(int upmodel, int armmodel[], int legmodel, int walkmodel[], int runmodel[])
{
	id_upmodel = upmodel;
	for(int i=0; i<TOTAL_ARMMODE; i++){
		id_armmodel[i] = armmodel[i];
	}
	id_legmodel = legmodel;
	for(int i=0; i<TOTAL_WALKMODE; i++){
		id_walkmodel[i] = walkmodel[i];
	}
	for(int i=0; i<TOTAL_RUNMODE; i++){
		id_runmodel[i] = runmodel[i];
	}
}

//! @brief 룊딖돸똭듫릶
//! @param rx 됷?둷뱗
void HumanMotionControl::Init(float rx)
{
	reaction_y = 0.0f;
	slowarm = false;
	armmodel_rotation_y = 0.0f;
	legrotation_x = rx;
	walkcnt = 0;
	runcnt = 0;
	armmodelid = -1;
	legmodelid = -1;
}

//! @brief 븧딇귩뢉궎
//! @param weapon_paramid 븧딇궻롰쀞붥뜂
void HumanMotionControl::PickupWeapon(int weapon_paramid)
{
	//뼟럊뾭덙릶뫮랉
	UNREFERENCED_PARAMETER(weapon_paramid);

	//쁱궻둷뱗걁뵿벍걂귩먠믦
	reaction_y = DegreeToRadian(-20);
	slowarm = true;
}

//! @brief 븧딇귩먛귟뫶궑귡걁렃궭뫶궑귡걂
//! @param weapon_paramid 븧딇궻롰쀞붥뜂
void HumanMotionControl::ChangeHaveWeapon(int weapon_paramid)
{
	//뼟럊뾭덙릶뫮랉
	UNREFERENCED_PARAMETER(weapon_paramid);

	//쁱궻둷뱗걁뵿벍걂귩먠믦
	reaction_y = DegreeToRadian(-20);
	slowarm = true;
}

//! @brief 븧딇궻롰쀞먛귟뫶궑
//! @param weapon_paramid 븧딇궻롰쀞붥뜂
//! @attention 궞궻듫릶궼둮뮗뾭궻???듫릶궳궥갃
void HumanMotionControl::ChangeWeaponID(int weapon_paramid)
{
	//뼟럊뾭덙릶뫮랉
	UNREFERENCED_PARAMETER(weapon_paramid);
}

//! @brief 뵯뺺룉뿚
//! @param weapon_paramid 븧딇궻롰쀞붥뜂
void HumanMotionControl::ShotWeapon(int weapon_paramid)
{
	//쁱궸뵿벍귩?궑귡
	if ((weapon_paramid == ID_WEAPON_GRENADE) ||
		(weapon_paramid == ID_WEAPON_IMPACT_GRENADE)) {
		reaction_y = DegreeToRadian(20);
	}
	else{
		//븧딇궻먠믦뭠귩롦벦
		WeaponParameter ParamData;
		ScopeParameter sparam;
		if( Param->GetWeapon(weapon_paramid, &ParamData) != 0 ){ return; }
		Param->GetScopeParam(ParamData.scopemode, &sparam);

		if( sparam.WeaponMotionReactionY >= 0.0f ){
			reaction_y = sparam.WeaponMotionReactionY;
		}
		else{
			reaction_y = DegreeToRadian(0.5f) * ParamData.reaction;
		}
	}
	slowarm = false;
}

//! @brief 깏깓?긤
//! @param weapon_paramid 븧딇궻롰쀞붥뜂
//! @attention 궞궻듫릶궼둮뮗뾭궻???듫릶궳궥갃
void HumanMotionControl::ReloadWeapon(int weapon_paramid)
{
	//뼟럊뾭덙릶뫮랉
	UNREFERENCED_PARAMETER(weapon_paramid);
}

//! @brief 븧딇귩롆궲귡
//! @attention 궞궻듫릶궼둮뮗뾭궻???듫릶궳궥갃
void HumanMotionControl::DumpWeapon()
{
	// 무기 버리기 시 팔이 갑자기 무기 보유 자세에서 맨손 자세로 튀지 않도록
	// 무기 전환과 같은 손 내림 모션을 준다.
	reaction_y = DegreeToRadian(-20);
	slowarm = true;
}

//! @brief 긙긿깛긵
//! @attention 궞궻듫릶궼둮뮗뾭궻???듫릶궳궥갃
void HumanMotionControl::Jump()
{
	//
}

//! @brief 긾?긘깈깛똶럁귩렳뛱
//! @attention rotation_x 뫬멣뫬궻됷?둷뱗
//! @attention armrotation_y 쁱궻됷?둷뱗
//! @attention weapon_paramid 븧딇궻롰쀞붥뜂
//! @attention ReloadCnt 븧딇궻깏깓?긤긇긂깛긣
//! @attention ChangeWeaponIDCnt 븧딇ID궻먛뫶긇긂깛긣
//! @attention MoveFlag 댷벍뺴뛀귩?궥긲깋긐
//! @attention hp 뫬쀍
//! @attention PlayerFlag 긵깒귽깂?궔궵궎궔
void HumanMotionControl::ProcessObject(float rotation_x, float armrotation_y, int weapon_paramid, int ReloadCnt, int ChangeWeaponIDCnt, int MoveFlag, int hp, bool PlayerFlag, bool ZombieFlag)
{
	int ArmModelID;
	float move_rx;

	if( slowarm == false ){
		//뵯뺺궶궵궸귝귡뵿벍
		if( reaction_y > 0.0f ){
			if( reaction_y > DegreeToRadian(0.01f) ){ reaction_y *= 0.5f; }
			else{ reaction_y = 0.0f; }
		}
		if( reaction_y < 0.0f ){
			if( reaction_y < DegreeToRadian(-0.01f) ){ reaction_y *= 0.5f; }
			else{ reaction_y = 0.0f; }
		}
	}
	else
	{
		// 1프레임당 이동할 고정 각도 (속도 조절 변수)
		const float linear_speed = DegreeToRadian(2.0f);

		// 목표값(0.0f)을 향해 일정한 속도로 이동
		if (reaction_y > 0.0f)
		{
			reaction_y -= linear_speed;
			// 목표값을 지나치면(오버슛) 0으로 보정
			if (reaction_y < 0.0f) { reaction_y = 0.0f; }
		}
		else if (reaction_y < 0.0f)
		{
			reaction_y += linear_speed;
			// 목표값을 지나치면(오버슛) 0으로 보정
			if (reaction_y > 0.0f) { reaction_y = 0.0f; }
		}

		// 목표값에 완전히 도달했을 때 완료 처리
		if (reaction_y == 0.0f)
		{
			slowarm = false;
		}
	}

	//깏깓?긤뭷궶귞쁱궻둷뱗귩띋먠믦
	if( ReloadCnt > 0 ){
		reaction_y = ARMRAD_RELOADWEAPON;
	}

	//븧딇ID먛뫶뭷궶귞쁱궻둷뱗귩띋먠믦
	if( ChangeWeaponIDCnt > 0 ){
		reaction_y = ARMRAD_RELOADWEAPON;
	}

	if (weapon_paramid == ID_WEAPON_NONE) {	// 맨손
		if (ZombieFlag == true) {
			// 좀비는 플레이어/AI 여부와 상관없이 평소에도 팔을 든 자세를 유지한다.
			armmodel_rotation_y = ARMRAD_ZOMBIEATTACK;
		}
		else if (PlayerFlag == true) {
			armmodel_rotation_y = ARMRAD_NOWEAPON;
		}
		else {
			armmodel_rotation_y = armrotation_y;
		}
		ArmModelID = 0;
	}
	else{							//돺궔븧딇귩렃궯궲궋귡
		//븧딇궻륃뺪귩롦벦
		WeaponParameter paramdata;
		Param->GetWeapon(weapon_paramid, &paramdata);

		armmodel_rotation_y = 0.0f;
		ArmModelID = 0;

		// 팔의 종류와 각도를 결정
		if (paramdata.WeaponP == WEAPONP_TWOHAND) {
			armmodel_rotation_y = armrotation_y + reaction_y;
			ArmModelID = 1;
		}
		if (paramdata.WeaponP == WEAPONP_ONEHAND) {
			armmodel_rotation_y = armrotation_y + reaction_y;
			ArmModelID = 2;
		}
		if (paramdata.WeaponP == WEAPONP_NOWEAPON) {
			armmodel_rotation_y = ARMRAD_NOWEAPON;
			ArmModelID = 0;
		}
		if (paramdata.WeaponP == WEAPONP_SHIELD) {
			// 방패 전용 팔 모델.
			// armrotation_y를 더하지 않아 시점 상하 변화에 따라 방패/팔이 같이 기울어지지 않게 한다.
			// reaction_y는 남겨 장착/해제 손 올림 모션은 유지한다.
			armmodel_rotation_y = reaction_y;
			ArmModelID = 3;
		}
	}

	//릋뛱뺴뛀귩뙂믦
	if( GetFlag(MoveFlag, MOVEFLAG_WALK) ){
		move_rx = DegreeToRadian(0);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_FORWARD ){
		move_rx = DegreeToRadian(0);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_BACK ){
		move_rx = DegreeToRadian(180);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_LEFT ){
		move_rx = DegreeToRadian(90);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == MOVEFLAG_RIGHT ){
		move_rx = DegreeToRadian(-90);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_FORWARD | MOVEFLAG_LEFT) ){
		move_rx = DegreeToRadian(45);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_BACK | MOVEFLAG_LEFT) ){
		move_rx = DegreeToRadian(135);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_BACK | MOVEFLAG_RIGHT) ){
		move_rx = DegreeToRadian(-135);
	}
	else if( GetFlag(MoveFlag, (MOVEFLAG_FORWARD | MOVEFLAG_BACK | MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) == (MOVEFLAG_FORWARD | MOVEFLAG_RIGHT) ){
		move_rx = DegreeToRadian(-45);
	}
	else{
		move_rx = 0.0f;
	}

	//뫉궻둷뱗귩럁뢯
	if( hp <= 0 ){
		legrotation_x = rotation_x;
	}
	else{
		float move_rx2;

		//뫉궻뛀궖귩땫귕귡
		if( fabsf(move_rx) > DegreeToRadian(90)){
			move_rx2 = move_rx + (float)M_PI;
		}
		else{
			move_rx2 = move_rx;
		}
		move_rx2 = AngleNormalization(move_rx2);

		//룞갲궸궩궻뛀궖궸
		legrotation_x = legrotation_x*0.85f + (rotation_x + move_rx2*-1)*0.15f;		// 3/4 + 1/4
	}

	//쁱궻긾긢깑먠믦
	armmodelid = id_armmodel[ArmModelID];

	//뫉궻긾긢깑먠믦
	legmodelid = id_legmodel;	//뿧궭?귏귟
	if( hp > 0 ){
		if( GetFlag(MoveFlag, MOVEFLAG_WALK) ){
			legmodelid = id_walkmodel[ (walkcnt/3 % TOTAL_WALKMODE) ];	//뺖궖
			walkcnt += 1;
			runcnt = 0;
		}
		else{
			if( GetFlag(MoveFlag, (MOVEFLAG_LEFT | MOVEFLAG_RIGHT)) ){
				legmodelid = id_runmodel[ (runcnt/3 % TOTAL_RUNMODE) ];		//뜺덭몪귟
			}
			if( GetFlag(MoveFlag, MOVEFLAG_FORWARD) ){
				legmodelid = id_runmodel[ (runcnt/2 % TOTAL_RUNMODE) ];		//멟몪귟
			}
			if( GetFlag(MoveFlag, MOVEFLAG_BACK) ){
				legmodelid = id_runmodel[ (runcnt/4 % TOTAL_RUNMODE) ];		//뚣귣몪귟
			}
			walkcnt = 0;
			runcnt += 1;
		}
	}
}

//! @brief 긾?긘깈깛롦벦
//! @param arm_rotation_y 쁱궻둷뱗귩롦벦궥귡?귽깛?
//! @param leg_rotation_x 뫉궻둷뱗귩롦벦궥귡?귽깛?
//! @param upmodel 뤵뵾릆궻긾긢깑궻긢??붥뜂귩롦벦궥귡?귽깛?
//! @param armmodel 쁱궻긾긢깑궻긢??붥뜂귩롦벦궥귡?귽깛?
//! @param legmodel 뫉궻긾긢깑궻긢??붥뜂귩롦벦궥귡?귽깛?
void HumanMotionControl::GetRenderMotion(float *arm_rotation_y, float *leg_rotation_x, int *upmodel, int *armmodel, int *legmodel)
{
	if( arm_rotation_y != NULL ){ *arm_rotation_y = armmodel_rotation_y; }
	if( leg_rotation_x != NULL ){ *leg_rotation_x = legrotation_x; }

	if( upmodel != NULL ){ *upmodel = id_upmodel; }
	if( armmodel != NULL ){ *armmodel = armmodelid; }
	if( legmodel != NULL ){ *legmodel = legmodelid; }
}

//! @brief 뺖궖갋몪귟긾?긘깈깛궻긇긂깛긣뭠롦벦
//! @return 긇긂깛긣뭠
int HumanMotionControl::GetMoveMotionCnt()
{
	if( walkcnt > 0 ){ return walkcnt; }
	return runcnt;
}

void human::SetIsPlayer(bool flag)
{
	isPlayer = flag;
}

void human::SetCrouchFlag(bool flag)
{
	crouchflag = flag;
}

bool human::GetCrouchFlag()
{
	return crouchflag;
}

void human::SetSitModel(int in_sitlegmodel, int in_sitwalkmodel[])
{
	sitlegmodel = in_sitlegmodel;
	for (int i = 0; i < TOTAL_SITWALKMODE; i++) {
		sitwalkmodel[i] = in_sitwalkmodel[i];
	}
}

void human::SetRecoilYOffset(float offset)
{
	recoil_y_offset = offset;
}

void human::SetRecoilXOffset(float offset)
{
	recoil_x_offset = offset;
}

void human::ClearDebugRecoil()
{
	recoil_y_offset = 0.0f;
	recoil_x_offset = 0.0f;
	recoil_accumulator = 0.0f;
	recoil_accum_timer = 0;
	recoil_side_shot_count = 0;
	recoil_side_timer = 0;
}

int human::GetBoltActionTimer()
{
	return boltActionTimer;
}

int human::GetBoltActionTotalTime()
{
	return boltActionTotalTime;
}

void human::SetBoltActionTimers(int total, int current)
{
	boltActionTotalTime = total;
	boltActionTimer = current;
}

#define MINIGUN_SPINUP_TIME 30
#define MINIGUN_SPIN_SOUND_INTERVAL 10

void human::UpdateMinigunSpin(bool spinInput)
{
	if (spinInput) {
		if (spinup_timer < MINIGUN_SPINUP_TIME) {
			spinup_timer++;
		}
	}
	else {
		if (spinup_timer > 0) {
			spinup_timer--;
		}
	}

	if (spinup_timer < 0) {
		spinup_timer = 0;
	}
	if (spinup_timer > MINIGUN_SPINUP_TIME) {
		spinup_timer = MINIGUN_SPINUP_TIME;
	}
	// 완전히 회전이 멈췄을 때만 지속음 상태와 사운드 반복 타이머 초기화
	if (spinup_timer == 0) {
		minigun_spin_loop_enabled = false;
		minigun_spin_sound_timer = 0;
	}
}

bool human::IsMinigunSpinReady()
{
	return (spinup_timer >= MINIGUN_SPINUP_TIME);
}

int human::GetMinigunSpinTimer()
{
	return spinup_timer;
}

int human::GetMinigunSpinTotalTime()
{
	return MINIGUN_SPINUP_TIME;
}

void human::SetMinigunSpinLoopEnabled(bool flag)
{
	minigun_spin_loop_enabled = flag;
}

bool human::IsMinigunSpinLoopEnabled()
{
	return minigun_spin_loop_enabled;
}

bool human::ConsumeMinigunSpinSoundTick()
{
	if (spinup_timer <= 0) {
		minigun_spin_sound_timer = 0;
		return false;
	}

	if (minigun_spin_sound_timer > 0) {
		minigun_spin_sound_timer--;
		return false;
	}

	minigun_spin_sound_timer = MINIGUN_SPIN_SOUND_INTERVAL;
	return true;
}