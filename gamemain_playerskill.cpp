//! @file gamemain_playerskill.cpp
//! @brief 플레이어 스킬 입력 및 발동 처리

#include "gamemain.h"
#include "playerskill.h"

extern Collision CollD;
extern ObjectManager ObjMgr;
extern AIcontrol HumanAI[MAX_HUMAN];
extern ParameterInfo GameParamInfo;
extern ResourceManager Resource;
extern SoundControl SoundCtrl;

static bool IsPlayerSkillHumanSpaceBlocked(float x, float y, float z);
static bool IsPlayerSkillSummonPlacementValid(float x, float y, float z);
static bool IsPlayerSkillSingleHumanPlacementValid(float x, float y, float z);
static bool IsPlayerSkillTeleportPlacementValid(float x, float y, float z);
static bool IsPlayerSkillSmallDevicePlacementValid(float x, float y, float z);
static bool IsPlayerSkillArtilleryStrikePlacementValid(float x, float y, float z);
static bool IsPlayerInsideArtilleryStrikeDangerArea(float x, float z);
static bool IsPlayerSkillInstallHeightAllowedFromPlayer(int skilltype, float target_y);
static bool IsPlayerSkillInstallTooCloseToPlayer(int skilltype, float target_x, float target_z);
static bool IsPlayerSkillInstallOwnerValid(human* myHuman);
static void ResetPlayerSkillInstallState(bool& pending, int& timer, int& slot, float& x, float& y, float& z, int default_slot);
static bool IsPlayerSkillInstallCancelKeyDown(int install_slot, bool primary_key_down, bool secondary_key_down);
static bool CancelPlayerSkillInstallStateForSlot(bool& pending, int& timer, int& slot, float& x, float& y, float& z, int default_slot, bool primary_key_down, bool secondary_key_down);
static void StartPlayerSkillInstallState(bool& pending, int& timer, int& slot, float& x, float& y, float& z, int use_slot, float target_x, float target_y, float target_z, int delay_frames);
static bool CanPlayerStandForShieldEquip(human* myHuman);
static void GetPlayerSkillSummonFormationOffsets(float center_x, float center_z, float* offset_x, float* offset_z);
static void SpawnPlayerSkillDisappearSmokeEffect(float x, float y, float z);
static bool IsPlayerSkillRobotHuman(human* target);
static bool IsRobotExecuteBlockedByFrontShield(human* myHuman, human* target, bool play_effect);
static bool IsRobotExecuteBlockedByAnyEnemyFrontShield(human* myHuman, human* target, bool play_effect);

static const int PLAYER_SKILL_AIM_SMOOTH_KEEP_FRAMES = 4;

// 소환 동료 전용 텍스쳐 ID.
// Human 파라미터 ID가 아니라 HumanTexturePath[]의 인덱스다.
// HumanTexturePath[30] = "./data/model/summon.bmp"
static const int PLAYER_SKILL_SUMMON_ALLY_TEXTURE_ID = 30;

// 0 = up0.x 일반 상체
// 1 = up1.x 썬글라스 상체
static const int PLAYER_SKILL_SUMMON_ALLY_UPMODEL_SUNGLASSES = 1;

// 임시 스킬 무기 전환 시간.
// Impact grenade는 기존처럼 짧은 전환 모션을 유지한다.
// 방패는 장착/해제 전환 페널티와 팔 내림/올림 모션을 쓰지 않는다.
static const int PLAYER_TEMP_SKILL_WEAPON_CHANGE_FRAMES = 5;

// 소형 설치물 공통 설치 판정값.
// 교란 장치, 지뢰, 생체장, 전술 폭격 표식처럼 사람 키 전체 공간을 요구하지 않는 설치물에 사용한다.
static const float PLAYER_SKILL_SMALL_DEVICE_CHECK_R = 2.0f;
static const float PLAYER_SKILL_SMALL_DEVICE_LOW_Y = 1.0f;
static const float PLAYER_SKILL_SMALL_DEVICE_BODY_Y = 4.0f;
static const float PLAYER_SKILL_SMALL_DEVICE_TOP_Y = 8.0f;

// 설치/지점 지정 미리보기 표시 높이.
// 실제 판정값이 아니라 HUD/월드 미리보기 표시 위치용이다.
static const float PLAYER_SKILL_PREVIEW_HEIGHT_DEFAULT = 8.0f;
static const float PLAYER_SKILL_PREVIEW_HEIGHT_BIOTIC_FIELD = 0.6f;
static const float PLAYER_SKILL_PREVIEW_HEIGHT_ARTILLERY_STRIKE = 0.8f;
static const float PLAYER_SKILL_PREVIEW_HEIGHT_TERRORIST_MINE = 1.4f;

// 전술 폭격은 멀리 지시하는 스킬이므로 일반 소형 설치물보다 배치 판정을 완화한다.
// 단, 좁은 위층/난간/상자 위처럼 폭격 범위가 지나치게 작게 잡히는 지점은 막는다.
static const float PLAYER_SKILL_ARTILLERY_PLACEMENT_CLEAR_R = 1.0f;
static const float PLAYER_SKILL_ARTILLERY_PLACEMENT_CLEAR_Y = 3.0f;
static const float PLAYER_SKILL_ARTILLERY_FOOTPRINT_SAMPLE_R = 10.0f;
static const float PLAYER_SKILL_ARTILLERY_FOOTPRINT_GROUND_UP = 6.0f;
static const float PLAYER_SKILL_ARTILLERY_FOOTPRINT_GROUND_DOWN = 24.0f;
static const float PLAYER_SKILL_ARTILLERY_FOOTPRINT_MAX_Y_DIFF = 7.0f;

// 평지/넓은 위층에서는 이 정도 지지점이 있으면 허용한다.
static const int PLAYER_SKILL_ARTILLERY_FOOTPRINT_MIN_SUPPORT = 4;

// 열린 가장자리/구멍을 통해 아래로 퍼질 수 있다면 조금 더 좁은 위층도 허용한다.
// 단, 1~2점만 걸친 난간/상자 모서리는 계속 막는다.
static const int PLAYER_SKILL_ARTILLERY_FOOTPRINT_MIN_SUPPORT_WITH_DOWN_SPREAD = 3;

// 전술 폭격은 원거리 지점 지정이므로 조준 마커가 잠깐 실패해도 조금 더 부드럽게 유지한다.
static const int PLAYER_SKILL_ARTILLERY_AIM_SMOOTH_KEEP_FRAMES = 8;

// 전술 폭격 미리보기 위치 보간.
// 값이 낮을수록 더 부드럽지만, 너무 낮으면 조준이 늦게 따라오는 느낌이 난다.
static const float PLAYER_SKILL_ARTILLERY_AIM_SMOOTH_LERP_RATE = 0.35f;
static const float PLAYER_SKILL_ARTILLERY_AIM_SMOOTH_MAX_BLEND_DIST = 80.0f;

// 전술 폭격이 열린 가장자리/구멍을 통해 아래층으로 퍼지는 보조 폭발.
// 바닥을 뚫는 피해가 아니라, 수직 아래로 열린 공간이 있을 때만 약하게 추가한다.
static const float PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_SAMPLE_R = 13.0f;
static const float PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_RAY_UP = 6.0f;
static const float PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_MIN_DROP = 14.0f;
static const float PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_MAX_DROP = 90.0f;
static const float PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_CLEAR_Y = 3.0f;
static const float PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_RANGE_RATE = 0.55f;
static const int PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_CHANCE_PERCENT = 35;
static const int PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_MAX_PER_TICK = 2;

// 설치형 스킬 공통 위치 제한.
// 너무 가까운 위치에 설치하면 플레이어 몸과 겹치거나 즉시 밟는 느낌이 나므로 최소 거리만 둔다.
static const float PLAYER_SKILL_INSTALL_MIN_DIST_SMALL_DEVICE = 8.0f;
static const float PLAYER_SKILL_INSTALL_MIN_DIST_HUMAN_OBJECT = 10.0f;

// 최종 설치 판정에서 사용할 높이 차 제한.
// 허공 조준 보정뿐 아니라 직접 바닥/상자를 맞힌 경우에도 큰 층 차이를 막기 위한 값이다.
static const float PLAYER_SKILL_INSTALL_MAX_UP_DIFF_SMALL_DEVICE = 18.0f;
static const float PLAYER_SKILL_INSTALL_MAX_DOWN_DIFF_SMALL_DEVICE = 24.0f;
static const float PLAYER_SKILL_INSTALL_MAX_UP_DIFF_HUMAN_OBJECT = 24.0f;
static const float PLAYER_SKILL_INSTALL_MAX_DOWN_DIFF_HUMAN_OBJECT = 32.0f;
static const float PLAYER_SKILL_INSTALL_MAX_UP_DIFF_ARTILLERY = 80.0f;
static const float PLAYER_SKILL_INSTALL_MAX_DOWN_DIFF_ARTILLERY = 90.0f;

static bool player_skill_aim_smooth_has_cache = false;
static int player_skill_aim_smooth_keep_timer = 0;
static int player_skill_aim_smooth_type = PLAYER_SKILL_NONE;
static float player_skill_aim_smooth_x = 0.0f;
static float player_skill_aim_smooth_y = 0.0f;
static float player_skill_aim_smooth_z = 0.0f;

static void ClearPlayerSkillAimTargetSmoothCache()
{
	player_skill_aim_smooth_has_cache = false;
	player_skill_aim_smooth_keep_timer = 0;
	player_skill_aim_smooth_type = PLAYER_SKILL_NONE;
	player_skill_aim_smooth_x = 0.0f;
	player_skill_aim_smooth_y = 0.0f;
	player_skill_aim_smooth_z = 0.0f;
}

static bool IsPlayerSkillInstallOwnerValid(human* myHuman)
{
	if (myHuman == NULL) { return false; }
	if (myHuman->GetEnableFlag() == false) { return false; }
	if (myHuman->GetDeadFlag() == true) { return false; }
	if (myHuman->GetHP() <= 0) { return false; }

	return true;
}

static void ResetPlayerSkillInstallState(bool& pending, int& timer, int& slot, float& x, float& y, float& z, int default_slot)
{
	pending = false;
	timer = 0;
	slot = default_slot;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

static void SpawnPlayerSkillDisappearSmokeEffect(float x, float y, float z)
{
	// 그림자 잔상/소환 동료가 사라질 때 쓰는 짧은 연기.
	ObjMgr.AddEffect(
		x,
		y + HUMAN_HEIGHT * 0.45f,
		z,
		0.0f,
		0.08f,
		0.0f,
		0.0f,
		DegreeToRadian(10) * GetRand(36),
		DegreeToRadian(GetRand(9) - 4),
		18.0f,
		0.28f,
		Resource.GetEffectSmokeTexture(),
		40,
		0,
		0.58f,
		-0.030f,
		EFFECT_NORMAL,
		24
	);

	// 순간적으로 사라졌다는 느낌을 주는 아주 짧은 섬광.
	ObjMgr.AddEffect(
		x,
		y + HUMAN_HEIGHT * 0.50f,
		z,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		DegreeToRadian(10) * GetRand(36),
		0.0f,
		10.0f,
		0.0f,
		Resource.GetEffectMflashTexture(),
		0,
		0,
		0.32f,
		-0.070f,
		EFFECT_NORMAL,
		6
	);
}

static bool IsPlayerSkillRobotHuman(human* target)
{
	if (target == NULL) { return false; }

	int param_id = -1;
	target->GetParamData(&param_id, NULL, NULL, NULL);

	if ((param_id < 0) || (TOTAL_PARAMETERINFO_HUMAN <= param_id)) {
		return false;
	}

	HumanParameter human_param;
	if (GameParamInfo.GetHuman(param_id, &human_param) != 0) {
		return false;
	}

	// parameter.cpp 기준 type 1 = 로봇
	return (human_param.type == 1);
}

static bool IsRobotExecuteBlockedByFrontShield(human* myHuman, human* target, bool play_effect)
{
	if (myHuman == NULL) { return false; }
	if (target == NULL) { return false; }

	float px, py, pz, prx;
	float tx, ty, tz, trx;

	myHuman->GetPosData(&px, &py, &pz, &prx);
	target->GetPosData(&tx, &ty, &tz, &trx);

	float start_y = py + VIEW_HEIGHT;
	if (myHuman->GetCrouchFlag() == true) {
		start_y -= 2.0f;
	}

	float target_aim_y = ty + HUMAN_HEIGHT * 0.88f;

	return ObjMgr.CheckShieldHookBlockSegment(
		target,
		myHuman,
		px,
		start_y,
		pz,
		tx,
		target_aim_y,
		tz,
		play_effect
	);
}

static bool IsRobotExecuteBlockedByAnyEnemyFrontShield(human* myHuman, human* target, bool play_effect)
{
	if (myHuman == NULL) { return false; }
	if (target == NULL) { return false; }

	int player_id = ObjMgr.GetPlayerID();

	int player_team = -1;
	myHuman->GetParamData(NULL, NULL, NULL, &player_team);

	float px, py, pz, prx;
	float tx, ty, tz, trx;

	myHuman->GetPosData(&px, &py, &pz, &prx);
	target->GetPosData(&tx, &ty, &tz, &trx);

	float start_y = py + VIEW_HEIGHT;
	if (myHuman->GetCrouchFlag() == true) {
		start_y -= 2.0f;
	}

	float target_aim_y = ty + HUMAN_HEIGHT * 0.88f;

	for (int i = 0; i < MAX_HUMAN; i++) {
		if (i == player_id) { continue; }

		human* blocker = ObjMgr.GetHumanObject(i);
		if (blocker == NULL) { continue; }
		if (blocker->GetEnableFlag() == false) { continue; }
		if (blocker->GetDeadFlag() == true) { continue; }
		if (blocker->GetHP() <= 0) { continue; }

		int blocker_team = -1;
		blocker->GetParamData(NULL, NULL, NULL, &blocker_team);

		// 플레이어 팀 방패가 적을 보호하면 조작감이 나빠지므로,
		// 처형 사격 차단은 적 방패만 검사한다.
		if (blocker_team == player_team) {
			continue;
		}

		if (ObjMgr.CheckShieldHookBlockSegment(
			blocker,
			myHuman,
			px,
			start_y,
			pz,
			tx,
			target_aim_y,
			tz,
			play_effect
		) == true) {
			return true;
		}
	}

	return false;
}

// 방패는 앉은 상태에서 장착하지 않는다.
// 방패가 캐릭터 키보다 높게 올라가므로, 일반 기립 높이보다 조금 더 높은 공간이 필요하다.
static bool CanPlayerStandForShieldEquip(human* myHuman)
{
	if (myHuman == NULL) { return false; }

	float px, py, pz;
	myHuman->GetPosData(&px, &py, &pz, NULL);

	const float SHIELD_EQUIP_REQUIRED_HEIGHT = 23.6f;

	float stand_check_y = py + HUMAN_HEIGHT - 1.0f;
	float shield_check_y = py + SHIELD_EQUIP_REQUIRED_HEIGHT;

	if (CollD.CheckALLBlockInside(px, py + 4.0f, pz) == true) { return false; }

	if (CollD.CheckALLBlockInside(px, stand_check_y, pz) == true) { return false; }
	if (CollD.CheckALLBlockInside(px + HUMAN_MAPCOLLISION_R, stand_check_y, pz) == true) { return false; }
	if (CollD.CheckALLBlockInside(px - HUMAN_MAPCOLLISION_R, stand_check_y, pz) == true) { return false; }
	if (CollD.CheckALLBlockInside(px, stand_check_y, pz + HUMAN_MAPCOLLISION_R) == true) { return false; }
	if (CollD.CheckALLBlockInside(px, stand_check_y, pz - HUMAN_MAPCOLLISION_R) == true) { return false; }

	if (CollD.CheckALLBlockInside(px, shield_check_y, pz) == true) { return false; }
	if (CollD.CheckALLBlockInside(px + HUMAN_MAPCOLLISION_R, shield_check_y, pz) == true) { return false; }
	if (CollD.CheckALLBlockInside(px - HUMAN_MAPCOLLISION_R, shield_check_y, pz) == true) { return false; }
	if (CollD.CheckALLBlockInside(px, shield_check_y, pz + HUMAN_MAPCOLLISION_R) == true) { return false; }
	if (CollD.CheckALLBlockInside(px, shield_check_y, pz - HUMAN_MAPCOLLISION_R) == true) { return false; }

	return true;
}

// 스킬 효과음 재생
void maingame::PlayPlayerSkillSound(int soundtype)
{
	int soundid = Resource.GetSkillSound(soundtype);

	if (soundid != -1) {
		int volume = (int)(85.0f * GameConfig.GetVolume());
		SoundCtrl.PlaySound(soundid, volume, 0);
	}
}

// 위치 기반 스킬 효과음 재생
void maingame::PlayPlayerSkill3DSound(int soundtype, float x, float y, float z)
{
	int soundid = Resource.GetSkillSound(soundtype);

	if (soundid != -1) {
		int volume = (int)(90.0f * GameConfig.GetVolume());
		SoundCtrl.Play3DSoundEx(soundid, x, y, z, volume, 30.0f, 180.0f);
	}
}

static void StopReloadSoundByReloadID(int reload_sound_id)
{
	if (reload_sound_id == RELOAD_NONE) { return; }
	if (reload_sound_id < 0) { return; }

	int soundid = Resource.GetReloadSound(reload_sound_id);

	if (soundid != -1) {
		SoundCtrl.StopSound(soundid);
	}
}

bool maingame::IsPlayerCurrentWeaponReloading(human* myHuman)
{
	if (myHuman == NULL) { return false; }

	if (myHuman->GetWeaponReloadCnt() > 0) {
		return true;
	}

	if (myHuman->GetShellReloadState() != human::SHELL_RELOAD_NONE) {
		return true;
	}

	return false;
}

static void StopCurrentPlayerReloadSound(human* myHuman)
{
	if (myHuman == NULL) { return; }

	if (myHuman->GetWeaponReloadCnt() <= 0 &&
		myHuman->GetShellReloadState() == human::SHELL_RELOAD_NONE) {
		return;
	}

	int selectweapon = 0;
	class weapon* slot_weapon[TOTAL_HAVEWEAPON];
	int lnbslist[TOTAL_HAVEWEAPON];
	int nbslist[TOTAL_HAVEWEAPON];

	for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
		slot_weapon[i] = NULL;
		lnbslist[i] = 0;
		nbslist[i] = 0;
	}

	myHuman->GetWeapon(&selectweapon, slot_weapon, lnbslist, nbslist);

	if ((selectweapon < 0) || (TOTAL_HAVEWEAPON <= selectweapon)) { return; }
	if (slot_weapon[selectweapon] == NULL) { return; }

	int weapon_id = ID_WEAPON_NONE;
	slot_weapon[selectweapon]->GetParamData(&weapon_id, NULL, NULL);

	WeaponParameter paramdata;

	if (GameParamInfo.GetWeapon(weapon_id, &paramdata) != 0) {
		return;
	}

	if (paramdata.is_shell_reload == true) {
		StopReloadSoundByReloadID(paramdata.SoundID_ReloadStart);
		StopReloadSoundByReloadID(paramdata.SoundID_Reload);
		StopReloadSoundByReloadID(paramdata.SoundID_ReloadEnd);
	}
	else {
		StopReloadSoundByReloadID(paramdata.SoundID_Reload);
		StopReloadSoundByReloadID(paramdata.SoundID_Tactical);
	}
}

void maingame::ResetPlayerSkillState()
{
	for (int i = 0; i < PLAYER_SKILL_SLOT_COUNT; i++) {
		player_skill_cooldown_timer[i] = 0;
		player_skill_cooldown_total_timer[i] = 0;
		player_skill_ready_sound_played[i] = true;
		player_skill_current_type[i] = PLAYER_SKILL_NONE;
	}

	player_skill_using_slot = PLAYER_SKILL_SLOT_PRIMARY;

	player_skill_scan_charge_timer = 0;
	player_skill_scan_timer = 0;
	player_skill_scan_charge_sound_timer = 0;

	player_skill_mark_prep_timer = 0;
	player_skill_mark_prep_slot = PLAYER_SKILL_SLOT_PRIMARY;
	player_skill_mark_timer = 0;
	player_skill_mark_target_id = -1;

	player_skill_robot_target_count = 0;
	for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
		player_skill_robot_target_ids[i] = -1;
	}

	player_skill_robot_fire_queue_count = 0;
	player_skill_robot_fire_queue_index = 0;
	player_skill_robot_fire_queue_timer = 0;
	for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
		player_skill_robot_fire_queue_ids[i] = -1;
	}

	player_skill_robot_execute_visual_ammo = 0;
	player_skill_robot_execute_visual_ammo_max = 0;

	player_skill_robot_prev_marker_count = 0;
	for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
		player_skill_robot_prev_marker_ids[i] = -1;
	}

	player_skill_aimhack_timer = 0;
	player_skill_aimhack_target_id = -1;

	for (int i = 0; i < MAX_HUMAN; i++) {
		player_skill_suppress_mark_timer[i] = 0;
	}

	player_skill_hook_target_id = -1;
	player_skill_hook_prep_timer = 0;
	player_skill_hook_pull_timer = 0;
	player_skill_hook_target_prev_force_no_ai = false;
	player_skill_hook_force_no_ai_applied = false;

	player_skill_distraction_timer = 0;
	player_skill_distraction_sound_timer = 0;
	player_skill_distraction_sound_count = 0;
	player_skill_distraction_x = 0.0f;
	player_skill_distraction_y = 0.0f;
	player_skill_distraction_z = 0.0f;

	player_skill_distraction_install_pending = false;
	player_skill_distraction_install_timer = 0;
	player_skill_distraction_install_slot = PLAYER_SKILL_SLOT_PRIMARY;
	player_skill_distraction_install_x = 0.0f;
	player_skill_distraction_install_y = 0.0f;
	player_skill_distraction_install_z = 0.0f;

	player_skill_terrorist_mine_install_pending = false;
	player_skill_terrorist_mine_install_timer = 0;
	player_skill_terrorist_mine_install_slot = PLAYER_SKILL_SLOT_SECONDARY;
	player_skill_terrorist_mine_install_x = 0.0f;
	player_skill_terrorist_mine_install_y = 0.0f;
	player_skill_terrorist_mine_install_z = 0.0f;

	ClearPlayerTerroristMines();

	player_skill_heal_effect_timer = 0;

	player_skill_survival_effect_timer = 0;

	player_skill_dodge_trail_timer = 0;
	player_skill_dodge_trail_start_x = 0.0f;
	player_skill_dodge_trail_start_y = 0.0f;
	player_skill_dodge_trail_start_z = 0.0f;
	player_skill_dodge_trail_end_x = 0.0f;
	player_skill_dodge_trail_end_y = 0.0f;
	player_skill_dodge_trail_end_z = 0.0f;

	player_skill_targeting_type = PLAYER_SKILL_NONE;
	player_skill_targeting_slot = PLAYER_SKILL_SLOT_PRIMARY;
	player_skill_target_x = 0.0f;
	player_skill_target_y = 0.0f;
	player_skill_target_z = 0.0f;
	player_skill_target_valid = false;

	ClearPlayerSkillAimTargetSmoothCache();

	player_skill_teleport_cast_timer = 0;
	player_skill_teleport_slot = PLAYER_SKILL_SLOT_PRIMARY;
	player_skill_teleport_x = 0.0f;
	player_skill_teleport_y = 0.0f;
	player_skill_teleport_z = 0.0f;

	player_skill_teleport_start_effect_timer = 0;
	player_skill_teleport_start_effect_x = 0.0f;
	player_skill_teleport_start_effect_y = 0.0f;
	player_skill_teleport_start_effect_z = 0.0f;

	player_skill_teleport_end_effect_timer = 0;
	player_skill_teleport_end_effect_x = 0.0f;
	player_skill_teleport_end_effect_y = 0.0f;
	player_skill_teleport_end_effect_z = 0.0f;

	player_skill_input_lock_timer = 0;
	player_skill_block_shot_until_release = false;

	for (int i = 0; i < PLAYER_SKILL_SUMMON_COUNT; i++) {
		player_skill_summon_ids[i] = -1;
		player_skill_summon_remove_timer[i] = 0;
		player_skill_summon_effect_x[i] = 0.0f;
		player_skill_summon_effect_y[i] = 0.0f;
		player_skill_summon_effect_z[i] = 0.0f;
	}

	player_skill_summon_effect_timer = 0;
	player_skill_summon_effect_count = 0;

	player_skill_summon_install_pending = false;
	player_skill_summon_install_timer = 0;
	player_skill_summon_install_slot = PLAYER_SKILL_SLOT_PRIMARY;
	player_skill_summon_install_x = 0.0f;
	player_skill_summon_install_y = 0.0f;
	player_skill_summon_install_z = 0.0f;

	player_skill_shadow_decoy_id = -1;
	player_skill_shadow_decoy_timer = 0;
	player_skill_shadow_decoy_install_pending = false;
	player_skill_shadow_decoy_install_timer = 0;
	player_skill_shadow_decoy_install_slot = PLAYER_SKILL_SLOT_SECONDARY;
	player_skill_shadow_decoy_install_x = 0.0f;
	player_skill_shadow_decoy_install_y = 0.0f;
	player_skill_shadow_decoy_install_z = 0.0f;

	for (int i = 0; i < MAX_HUMAN; i++) {
		player_skill_shadow_decoy_ping_timer[i] = 0;
	}

	player_skill_biotic_field_timer = 0;
	player_skill_biotic_field_heal_timer = 0;
	player_skill_biotic_field_x = 0.0f;
	player_skill_biotic_field_y = 0.0f;
	player_skill_biotic_field_z = 0.0f;

	player_skill_biotic_field_install_pending = false;
	player_skill_biotic_field_install_timer = 0;
	player_skill_biotic_field_install_slot = PLAYER_SKILL_SLOT_SECONDARY;
	player_skill_biotic_field_install_x = 0.0f;
	player_skill_biotic_field_install_y = 0.0f;
	player_skill_biotic_field_install_z = 0.0f;

	player_skill_artillery_strike_timer = 0;
	player_skill_artillery_strike_tick_timer = 0;
	player_skill_artillery_strike_x = 0.0f;
	player_skill_artillery_strike_y = 0.0f;
	player_skill_artillery_strike_z = 0.0f;
	player_skill_artillery_strike_owner_team = -1;
	player_skill_artillery_strike_owner_human_id = -1;

	player_skill_artillery_strike_install_pending = false;
	player_skill_artillery_strike_install_timer = 0;
	player_skill_artillery_strike_install_slot = PLAYER_SKILL_SLOT_SECONDARY;
	player_skill_artillery_strike_install_x = 0.0f;
	player_skill_artillery_strike_install_y = 0.0f;
	player_skill_artillery_strike_install_z = 0.0f;

	player_skill_prev_active_timer = 0;

	player_skill_impact_grenade_ready = false;
	player_skill_impact_grenade_equip_pending = false;
	player_skill_impact_grenade_unequip_pending = false;
	player_skill_impact_grenade_fire_lock = false;
	player_skill_impact_grenade_change_timer = 0;
	player_skill_impact_grenade_shot_delay_timer = 0;
	player_skill_impact_grenade_return_weapon_slot = -1;
	player_skill_impact_grenade_ammo = 5;

	player_skill_shield_ready = false;
	player_skill_shield_equip_pending = false;
	player_skill_shield_unequip_pending = false;
	player_skill_shield_change_timer = 0;
	player_skill_shield_return_weapon_slot = -1;

	player_skill_dp28_ready = false;
	player_skill_dp28_return_weapon_slot = -1;
	player_skill_dp28_stored_weapon_id = ID_WEAPON_NONE;
	player_skill_dp28_stored_lnbs = 0;
	player_skill_dp28_stored_nbs = 0;
	player_skill_dp28_saved_lnbs = -1;
	player_skill_dp28_saved_nbs = -1;
}

// 플레이어 캐릭터 ID에 따른 스킬 타입 반환
int maingame::GetPlayerSkillTypeBySlot(human* myHuman, int slot)
{
	if (GameConfig.GetSkillModeFlag() == false) {
		return PLAYER_SKILL_NONE;
	}

	int group = PlayerSkill_GetGroupByHuman(myHuman);

	return PlayerSkill_GetTypeByGroup(group, slot);
}

int maingame::GetPlayerSkillType(human* myHuman)
{
	return GetPlayerSkillTypeBySlot(myHuman, PLAYER_SKILL_SLOT_PRIMARY);
}

bool maingame::HasPlayerSkill(human* myHuman)
{
	if (GameConfig.GetSkillModeFlag() == false) {
		return false;
	}

	return PlayerSkill_HasSkill(myHuman);
}

bool maingame::IsPlayerInstallSkillPending()
{
	return ((player_skill_distraction_install_pending == true) ||
		(player_skill_terrorist_mine_install_pending == true) ||
		(player_skill_summon_install_pending == true) ||
		(player_skill_biotic_field_install_pending == true) ||
		(player_skill_artillery_strike_install_pending == true) ||
		(player_skill_shadow_decoy_install_pending == true));
}

bool maingame::IsAnyPlayerSkillActive(human* myHuman)
{
	if (myHuman == NULL) { return false; }

	if (myHuman->GetSkillStealthFlag() == true) { return true; }
	if (myHuman->GetSkillAmmoBoostFlag() == true) { return true; }
	if (myHuman->GetSkillSurvivalFlag() == true) { return true; }
	if (myHuman->GetSkillDodgeFlag() == true) { return true; }
	if (myHuman->GetSkillIronBodyFlag() == true) { return true; }
	if (myHuman->GetSkillAbsorbFlag() == true) { return true; }
	if (myHuman->GetSkillRobotExecuteFlag() == true) { return true; }
	if (myHuman->GetSkillRageFlag() == true) { return true; }

	if (player_skill_aimhack_timer > 0) { return true; }

	if (player_skill_scan_charge_timer > 0) { return true; }
	if (player_skill_scan_timer > 0) { return true; }
	if (player_skill_mark_prep_timer > 0) { return true; }
	if (player_skill_mark_timer > 0) { return true; }
	if (player_skill_distraction_timer > 0) { return true; }
	if (player_skill_hook_prep_timer > 0) { return true; }
	if (player_skill_hook_pull_timer > 0) { return true; }
	if (player_skill_teleport_cast_timer > 0) { return true; }
	if (player_skill_biotic_field_timer > 0) { return true; }
	if (player_skill_artillery_strike_timer > 0) { return true; }
	if (IsPlayerInstallSkillPending() == true) { return true; }

	if (player_skill_targeting_type != PLAYER_SKILL_NONE) { return true; }

	return false;
}

bool maingame::IsPlayerSkillBlockedByActiveSkill(human* myHuman, int skilltype)
{
	if (myHuman == NULL) { return true; }

	// ----------------------------------------------------------
	// 1. 입력/조작 상태 계열은 항상 다른 스킬 진입을 막는다.
	// ----------------------------------------------------------
	// 설치 대기 중에는 이동/사격/재장전/무기 전환/다른 스킬 사용이 모두 막히는 정책이다.
	if (IsPlayerInstallSkillPending() == true) {
		return true;
	}

	// 조준 확정형 스킬 대기 중에는 다른 스킬로 넘어가지 않는다.
	// 취소 후 다시 사용하도록 한다.
	if (player_skill_targeting_type != PLAYER_SKILL_NONE) {
		return true;
	}

	// 마킹 샷 준비 중에는 아직 실제 마킹이 확정되지 않은 상태이므로 다른 스킬을 막는다.
	if (player_skill_mark_prep_timer > 0) {
		return true;
	}

	// 갈고리 준비/끌기 중에는 위치 보정과 상태 전환이 들어가므로 다른 스킬을 막는다.
	if ((player_skill_hook_prep_timer > 0) ||
		(player_skill_hook_pull_timer > 0)) {
		return true;
	}

	// 텔레포트 시전 중에는 위치 이동이 확정되기 전이므로 다른 스킬을 막는다.
	if (player_skill_teleport_cast_timer > 0) {
		return true;
	}

	// ----------------------------------------------------------
	// 2. 실제로 같이 쓰면 안 되는 조합만 명시적으로 차단한다.
	// ----------------------------------------------------------

	// 로봇: 처형 프로토콜과 에임핵은 동시에 사용하지 않는다.
	if ((myHuman->GetSkillRobotExecuteFlag() == true) &&
		(skilltype == PLAYER_SKILL_AIMHACK)) {
		return true;
	}

	if ((player_skill_aimhack_timer > 0) &&
		(skilltype == PLAYER_SKILL_ROBOT_EXECUTE)) {
		return true;
	}

	// 좀비: 갈고리 조준/준비/끌기 중에는 격노를 새로 켜지 않는다.
	// 반대로 격노 중 갈고리는 허용한다.
	if (skilltype == PLAYER_SKILL_RAGE) {
		if ((player_skill_targeting_type == PLAYER_SKILL_HOOK) ||
			(player_skill_hook_prep_timer > 0) ||
			(player_skill_hook_pull_timer > 0)) {
			return true;
		}
	}

	// ----------------------------------------------------------
	// 3. 그 외 지속형/표시형 active 스킬은 다른 스킬 사용을 막지 않는다.
	// ----------------------------------------------------------
	// 예:
	// 철괴 중 응급 치료
	// 전투 강화 중 Impact grenade
	// 생존 본능 중 긴급 회피
	// 은신 중 텔레포트
	// 전투 흡수 중 DP28
	// 교란 신호 중 지뢰
	// 생체장 중 동료 소환
	// 전술 폭격 중 전술 스캔
	// 전술 스캔 중 전술 폭격
	// 마킹 샷 지속 중 그림자 잔상
	// 위 조합들은 기본적으로 허용한다.

	return false;
}

static int NormalizePlayerSkillSlot(int slot)
{
	if ((slot < 0) || (slot >= PLAYER_SKILL_SLOT_COUNT)) {
		return PLAYER_SKILL_SLOT_PRIMARY;
	}

	return slot;
}

static void StartPlayerSkillInstallState(bool& pending, int& timer, int& slot, float& x, float& y, float& z, int use_slot, float target_x, float target_y, float target_z, int delay_frames)
{
	pending = true;
	timer = delay_frames;
	slot = NormalizePlayerSkillSlot(use_slot);
	x = target_x;
	y = target_y;
	z = target_z;
}

static bool IsPlayerSkillInstallCancelKeyDown(int install_slot, bool primary_key_down, bool secondary_key_down)
{
	int slot = NormalizePlayerSkillSlot(install_slot);

	if ((slot == PLAYER_SKILL_SLOT_PRIMARY) && (primary_key_down == true)) {
		return true;
	}

	if ((slot == PLAYER_SKILL_SLOT_SECONDARY) && (secondary_key_down == true)) {
		return true;
	}

	return false;
}

static bool CancelPlayerSkillInstallStateForSlot(bool& pending, int& timer, int& slot, float& x, float& y, float& z, int default_slot, bool primary_key_down, bool secondary_key_down)
{
	if (pending == false) {
		return false;
	}

	if (IsPlayerSkillInstallCancelKeyDown(slot, primary_key_down, secondary_key_down) == false) {
		return false;
	}

	ResetPlayerSkillInstallState(
		pending,
		timer,
		slot,
		x,
		y,
		z,
		default_slot
	);

	return true;
}

// 긴급 회피는 최종 지점만 보지 않고, 짧은 간격으로 나누어 지형을 따라간다.
static const float PLAYER_SKILL_DODGE_STEP_DIST = 4.0f;
static const float PLAYER_SKILL_DODGE_MIN_SUCCESS_DIST = 8.0f;

// 한 단계마다 허용하는 높이 차이.
// 완만한 오르막/계단은 허용하고, 큰 턱/낭떠러지는 막는다.
static const float PLAYER_SKILL_DODGE_MAX_STEP_UP_DIFF = HUMAN_MAPCOLLISION_CLIMBHEIGHT + 1.8f;
static const float PLAYER_SKILL_DODGE_MAX_STEP_DOWN_DIFF = 7.0f;

// 낮은 천장이 있을 때 위쪽 ray가 천장을 먼저 잡는 문제를 피하기 위해,
// 지면 탐색 시작 높이는 "올라갈 수 있는 높이보다 조금 위" 정도로만 둔다.
static const float PLAYER_SKILL_DODGE_GROUND_RAY_UP = HUMAN_MAPCOLLISION_CLIMBHEIGHT + 5.8f;
static const float PLAYER_SKILL_DODGE_GROUND_RAY_DOWN = 120.0f;
static const float PLAYER_SKILL_DODGE_GROUND_CLEARANCE = 0.8f;
static const float PLAYER_SKILL_DODGE_GROUND_SAMPLE_R = HUMAN_MAPCOLLISION_R * 0.85f;

static const float PLAYER_SKILL_HOOK_GROUND_RAY_UP = 192.0f;
static const float PLAYER_SKILL_HOOK_GROUND_RAY_DOWN = 240.0f;
static const float PLAYER_SKILL_HOOK_MAX_UP_PER_FRAME = 12.0f;
static const float PLAYER_SKILL_HOOK_MAX_DOWN_PER_FRAME = 10.0f;

// 언덕 위에서 아래 적을 끌 때, 적이 바닥에만 붙지 않고 약간 공중으로 끌려오도록 허용한다.
static const float PLAYER_SKILL_HOOK_AIR_LIFT_HEIGHT_DIFF = 10.0f;
static const float PLAYER_SKILL_HOOK_AIR_LIFT_MAX_HEIGHT_FROM_GROUND = 128.0f;

// 갈고리 중 Y 이동은 절대 한 번에 크게 보정하지 않는다.
// 계단/언덕을 오르듯 보이게 하기 위한 전용 상승/하강 속도.
static const float PLAYER_SKILL_HOOK_Y_UP_STEP = 4.0f;
static const float PLAYER_SKILL_HOOK_Y_DOWN_STEP = 3.0f;

// 경사면에서는 X/Z 이동을 따라가며 발이 묻히지 않도록 지면 높이를 바로 따라간다.
// 단, 큰 단차는 여기서 바로 점프시키지 않고 기존 Y 보정 속도를 유지한다.
static const float PLAYER_SKILL_HOOK_SLOPE_FOLLOW_MAX_UP = 5.5f;

// X/Z가 이미 가까워도 Y 차이가 남아 있으면 갈고리를 종료하지 않는다.
static const float PLAYER_SKILL_HOOK_STOP_XZ_DIST = 1.5f;
static const float PLAYER_SKILL_HOOK_STOP_Y_DIST = 4.0f;

// 아래에서 위쪽 언덕으로 끌 때, 앞쪽 지형이 높으면 살짝 공중 리프트를 준다.
static const float PLAYER_SKILL_HOOK_UPHILL_LIFT_START_DIFF = 4.0f;

// 단차를 넘기 위한 보조값.
// 기존 18.0f는 땅에 닿는 순간 한 번 튀어오르는 원인이 될 수 있다.
static const float PLAYER_SKILL_HOOK_UPHILL_LIFT_EXTRA_HEIGHT = 2.0f;
static const float PLAYER_SKILL_HOOK_UPHILL_LIFT_MAX_HEIGHT_FROM_GROUND = 8.0f;

static float MovePlayerSkillHookYToward(float current_y, float goal_y)
{
	float diff = goal_y - current_y;

	if (diff > PLAYER_SKILL_HOOK_Y_UP_STEP) {
		return current_y + PLAYER_SKILL_HOOK_Y_UP_STEP;
	}

	if (diff < -PLAYER_SKILL_HOOK_Y_DOWN_STEP) {
		return current_y - PLAYER_SKILL_HOOK_Y_DOWN_STEP;
	}

	return goal_y;
}

static const float PLAYER_SKILL_HOOK_GROUND_CLEARANCE = 0.8f;
static const float PLAYER_SKILL_HOOK_GROUND_SAMPLE_R = HUMAN_MAPCOLLISION_R * 0.85f;

static bool FindPlayerSkillHookGroundYAt(float x, float z, float current_y, float* out_y)
{
	if (out_y == NULL) { return false; }

	float ray_start_y = current_y + PLAYER_SKILL_HOOK_GROUND_RAY_UP;
	float ground_dist = 0.0f;

	bool ground_hit = CollD.CheckALLBlockIntersectRay(
		x,
		ray_start_y,
		z,
		0.0f,
		-1.0f,
		0.0f,
		NULL,
		NULL,
		&ground_dist,
		PLAYER_SKILL_HOOK_GROUND_RAY_UP + PLAYER_SKILL_HOOK_GROUND_RAY_DOWN
	);

	if (ground_hit == false) {
		return false;
	}

	*out_y = ray_start_y - ground_dist + PLAYER_SKILL_HOOK_GROUND_CLEARANCE;
	return true;
}

static bool FindPlayerSkillHookGroundY(float x, float z, float current_y, float* out_y)
{
	if (out_y == NULL) { return false; }

	float best_y = 0.0f;
	float test_y = 0.0f;
	bool found = false;

	const float r = PLAYER_SKILL_HOOK_GROUND_SAMPLE_R;

	if (FindPlayerSkillHookGroundYAt(x, z, current_y, &test_y) == true) {
		best_y = test_y;
		found = true;
	}

	if (FindPlayerSkillHookGroundYAt(x + r, z, current_y, &test_y) == true) {
		if ((found == false) || (test_y > best_y)) { best_y = test_y; }
		found = true;
	}

	if (FindPlayerSkillHookGroundYAt(x - r, z, current_y, &test_y) == true) {
		if ((found == false) || (test_y > best_y)) { best_y = test_y; }
		found = true;
	}

	if (FindPlayerSkillHookGroundYAt(x, z + r, current_y, &test_y) == true) {
		if ((found == false) || (test_y > best_y)) { best_y = test_y; }
		found = true;
	}

	if (FindPlayerSkillHookGroundYAt(x, z - r, current_y, &test_y) == true) {
		if ((found == false) || (test_y > best_y)) { best_y = test_y; }
		found = true;
	}

	if (found == false) {
		return false;
	}

	*out_y = best_y;
	return true;
}

static bool IsPlayerSkillHookMoveBlocked(float x, float y, float z)
{
	float check_y = y + HUMAN_MAPCOLLISION_HEIGHT;

	return
		CollD.CheckALLBlockInside(x, check_y, z) ||
		CollD.CheckALLBlockInside(x + HUMAN_MAPCOLLISION_R, check_y, z) ||
		CollD.CheckALLBlockInside(x - HUMAN_MAPCOLLISION_R, check_y, z) ||
		CollD.CheckALLBlockInside(x, check_y, z + HUMAN_MAPCOLLISION_R) ||
		CollD.CheckALLBlockInside(x, check_y, z - HUMAN_MAPCOLLISION_R);
}

static bool IsPlayerSkillHookLineBlocked(
	float sx, float sy, float sz,
	float tx, float ty, float tz
)
{
	float vx = tx - sx;
	float vy = ty - sy;
	float vz = tz - sz;

	float dist_sq = vx * vx + vy * vy + vz * vz;
	if (dist_sq <= 0.001f) {
		return false;
	}

	float dist = sqrtf(dist_sq);
	float dir_x = vx / dist;
	float dir_y = vy / dist;
	float dir_z = vz / dist;

	float block_dist = 0.0f;

	bool low_blocked = CollD.CheckALLBlockIntersectRay(
		sx,
		sy,
		sz,
		dir_x,
		dir_y,
		dir_z,
		NULL,
		NULL,
		&block_dist,
		dist
	);

	if (low_blocked == false) {
		return false;
	}

	// 아래에서 위로 언덕을 향해 쓸 때, 낮은 몸통 레이는 지형 모서리에 닿을 수 있다.
	// 한 번 더 높은 레이를 검사해서 높은 레이가 통과하면 지형 언덕으로 보고 허용한다.
	float upper_offset = HUMAN_MAPCOLLISION_CLIMBHEIGHT + 8.0f;

	float upper_block_dist = 0.0f;
	bool upper_blocked = CollD.CheckALLBlockIntersectRay(
		sx,
		sy + upper_offset,
		sz,
		dir_x,
		dir_y,
		dir_z,
		NULL,
		NULL,
		&upper_block_dist,
		dist
	);

	return upper_blocked;
}

static bool FindPlayerSkillDodgeGroundYAt(float x, float z, float current_y, float* out_y)
{
	if (out_y == NULL) { return false; }

	float ray_start_y = current_y + PLAYER_SKILL_DODGE_GROUND_RAY_UP;
	float ground_dist = 0.0f;

	bool ground_hit = CollD.CheckALLBlockIntersectRay(
		x,
		ray_start_y,
		z,
		0.0f,
		-1.0f,
		0.0f,
		NULL,
		NULL,
		&ground_dist,
		PLAYER_SKILL_DODGE_GROUND_RAY_UP + PLAYER_SKILL_DODGE_GROUND_RAY_DOWN
	);

	if (ground_hit == false) {
		return false;
	}

	*out_y = ray_start_y - ground_dist + PLAYER_SKILL_DODGE_GROUND_CLEARANCE;
	return true;
}

static bool FindPlayerSkillDodgeGroundY(float x, float z, float current_y, float* out_y)
{
	if (out_y == NULL) { return false; }

	float best_y = 0.0f;
	float test_y = 0.0f;
	bool found = false;

	const float r = PLAYER_SKILL_DODGE_GROUND_SAMPLE_R;

	if (FindPlayerSkillDodgeGroundYAt(x, z, current_y, &test_y) == true) {
		best_y = test_y;
		found = true;
	}

	if (FindPlayerSkillDodgeGroundYAt(x + r, z, current_y, &test_y) == true) {
		if ((found == false) || (test_y > best_y)) { best_y = test_y; }
		found = true;
	}

	if (FindPlayerSkillDodgeGroundYAt(x - r, z, current_y, &test_y) == true) {
		if ((found == false) || (test_y > best_y)) { best_y = test_y; }
		found = true;
	}

	if (FindPlayerSkillDodgeGroundYAt(x, z + r, current_y, &test_y) == true) {
		if ((found == false) || (test_y > best_y)) { best_y = test_y; }
		found = true;
	}

	if (FindPlayerSkillDodgeGroundYAt(x, z - r, current_y, &test_y) == true) {
		if ((found == false) || (test_y > best_y)) { best_y = test_y; }
		found = true;
	}

	if (found == false) {
		return false;
	}

	*out_y = best_y;
	return true;
}

static bool IsPlayerSkillDodgeMoveBlocked(float x, float y, float z)
{
	float check_y = y + HUMAN_MAPCOLLISION_HEIGHT;

	return
		CollD.CheckALLBlockInside(x, check_y, z) ||
		CollD.CheckALLBlockInside(x + HUMAN_MAPCOLLISION_R, check_y, z) ||
		CollD.CheckALLBlockInside(x - HUMAN_MAPCOLLISION_R, check_y, z) ||
		CollD.CheckALLBlockInside(x, check_y, z + HUMAN_MAPCOLLISION_R) ||
		CollD.CheckALLBlockInside(x, check_y, z - HUMAN_MAPCOLLISION_R);
}

static bool IsPlayerSkillDodgeLineBlocked(
	float sx, float sy, float sz,
	float tx, float ty, float tz
)
{
	float vx = tx - sx;
	float vy = ty - sy;
	float vz = tz - sz;

	float dist_sq = vx * vx + vy * vy + vz * vz;
	if (dist_sq <= 0.001f) {
		return false;
	}

	float dist = sqrtf(dist_sq);
	float dir_x = vx / dist;
	float dir_z = vz / dist;

	float check_sy = sy + HUMAN_MAPCOLLISION_HEIGHT;
	float check_ty = ty + HUMAN_MAPCOLLISION_HEIGHT;

	float block_dist = 0.0f;

	bool low_blocked = CollD.CheckALLBlockIntersectRay(
		sx,
		check_sy,
		sz,
		dir_x,
		(check_ty - check_sy) / dist,
		dir_z,
		NULL,
		NULL,
		&block_dist,
		dist
	);

	if (low_blocked == false) {
		return false;
	}

	// 낮은 레이가 지형 모서리에 닿아도, 높은 레이가 통과하면 완만한 언덕으로 보고 허용한다.
	float upper_offset = HUMAN_MAPCOLLISION_CLIMBHEIGHT + 8.0f;

	float upper_block_dist = 0.0f;
	bool upper_blocked = CollD.CheckALLBlockIntersectRay(
		sx,
		check_sy + upper_offset,
		sz,
		dir_x,
		(check_ty - check_sy) / dist,
		dir_z,
		NULL,
		NULL,
		&upper_block_dist,
		dist
	);

	return upper_blocked;
}

void maingame::StartPlayerSkillCooldown(int skilltype, int cooldown_frames)
{
	StartPlayerSkillCooldownWithTotal(skilltype, cooldown_frames, cooldown_frames);
}

void maingame::StartPlayerSkillCooldownWithTotal(int skilltype, int cooldown_frames, int cooldown_total_frames)
{
	StartPlayerSkillCooldownForSlotWithTotal(
		player_skill_using_slot,
		skilltype,
		cooldown_frames,
		cooldown_total_frames
	);
}

void maingame::StartPlayerSkillCooldownForSlot(int slot, int skilltype, int cooldown_frames)
{
	StartPlayerSkillCooldownForSlotWithTotal(slot, skilltype, cooldown_frames, cooldown_frames);
}

void maingame::StartPlayerSkillCooldownForSlotWithTotal(int slot, int skilltype, int cooldown_frames, int cooldown_total_frames)
{
	slot = NormalizePlayerSkillSlot(slot);

	if (cooldown_frames < 0) {
		cooldown_frames = 0;
	}

	if (cooldown_total_frames < cooldown_frames) {
		cooldown_total_frames = cooldown_frames;
	}

	player_skill_cooldown_timer[slot] = cooldown_frames;
	player_skill_cooldown_total_timer[slot] = cooldown_total_frames;
	player_skill_ready_sound_played[slot] = false;
	player_skill_current_type[slot] = skilltype;
}

void maingame::CleanupPlayerSummonedZombies()
{
	for (int i = 0; i < PLAYER_SKILL_SUMMON_COUNT; i++) {
		int id = player_skill_summon_ids[i];

		if ((id < 0) || (id >= MAX_HUMAN)) {
			player_skill_summon_ids[i] = -1;
			player_skill_summon_remove_timer[i] = 0;
			continue;
		}

		human* h = ObjMgr.GetHumanObject(id);

		if (h == NULL) {
			player_skill_summon_ids[i] = -1;
			player_skill_summon_remove_timer[i] = 0;
			continue;
		}

		if (h->GetEnableFlag() == false) {
			player_skill_summon_ids[i] = -1;
			player_skill_summon_remove_timer[i] = 0;
			continue;
		}

		// 소환 동료가 죽으면 바로 지우지 않고, 시체를 잠깐 남긴 뒤 제거한다.
		if ((h->GetHP() <= 0) || (h->GetDeadFlag() == true)) {
			if (player_skill_summon_remove_timer[i] <= 0) {
				h->SetSkillSummonedAllyFlag(false);
				player_skill_summon_remove_timer[i] = PLAYER_SKILL_SUMMON_DEAD_REMOVE_DELAY_FRAMES;
				continue;
			}

			player_skill_summon_remove_timer[i]--;

			if (player_skill_summon_remove_timer[i] <= 0) {
				float x, y, z, rx;
				h->GetPosData(&x, &y, &z, &rx);

				SpawnPlayerSkillDisappearSmokeEffect(x, y, z);

				h->SetEnableFlag(false);
				player_skill_summon_ids[i] = -1;
				player_skill_summon_remove_timer[i] = 0;
			}

			continue;
		}

		// 혹시 죽었다가 제거 대기 중이던 슬롯이 정상 상태로 돌아온 경우를 대비한다.
		if (player_skill_summon_remove_timer[i] > 0) {
			player_skill_summon_remove_timer[i] = 0;
		}
	}
}

void maingame::UpdatePlayerSummonedAllyFollow()
{
	int player_id = ObjMgr.GetPlayerID();

	if ((player_id < 0) || (player_id >= MAX_HUMAN)) {
		return;
	}

	for (int i = 0; i < PLAYER_SKILL_SUMMON_COUNT; i++) {
		int id = player_skill_summon_ids[i];

		if ((id < 0) || (id >= MAX_HUMAN)) {
			continue;
		}

		human* h = ObjMgr.GetHumanObject(id);

		if (h == NULL) { continue; }
		if (h->GetEnableFlag() == false) { continue; }
		if (h->GetDeadFlag() == true) { continue; }
		if (h->GetHP() <= 0) { continue; }

		// 소환 동료는 AI FireRate 설정과 관계없이 이동 속도를 고정 2배로 보정한다.
		// 평상시 플레이어를 따라올 때와 전투/경계 중 적을 쫓을 때 모두 적용된다.
		h->SpeedMultiplier = 2.0f;

		int ai_mode = -1;
		HumanAI[id].GetBattleMode(&ai_mode, NULL);

		// 전투/경계 중에는 AI 판단을 덮어쓰지 않는다.
		// 속도 보정은 위에서 이미 적용했으므로 유지한다.
		if ((ai_mode == AI_ACTION) || (ai_mode == AI_CAUTION)) {
			continue;
		}

		// 평상시에는 소환한 플레이어를 따라오게 한다.
		HumanAI[id].SetHoldTracking(player_id);
	}
}

// 스킬 사용 직전에 조준/수류탄 궤적을 강제로 해제한다.
// 실패 처리하지 않고, 조준/궤적을 끈 뒤 스킬을 그대로 발동시키기 위한 함수다.
void maingame::ClearAimStateForPlayerSkill(human* myHuman)
{
	if (myHuman == NULL) { return; }

	// 스코프/조준 해제
	if (myHuman->GetScopeMode() != 0) {
		myHuman->SetDisableScope();
	}

	// 수류탄 궤적 토글 상태는 끄지 않는다.
// 실제 표시는 ShouldRenderGrenadeTrajectory()에서 스킬 조준 중에만 잠시 숨긴다.

	// 우클릭/좌클릭 입력이 다음 무기 처리로 이어지는 것을 막기 위한 짧은 잠금
	if (player_skill_input_lock_timer < 3) {
		player_skill_input_lock_timer = 3;
	}
}

// 경찰 제압 명령: 대상이 플레이어 시야 안에 있고 벽에 막히지 않았는지 확인
bool maingame::IsEnemyInPlayerSuppressSight(human* myHuman, human* target)
{
	if (myHuman == NULL) { return false; }
	if (target == NULL) { return false; }
	if (target == myHuman) { return false; }
	if (target->GetEnableFlag() == false) { return false; }
	if (target->GetDeadFlag() == true) { return false; }
	if (target->GetHP() <= 0) { return false; }

	int myteam = -1;
	int target_team = -1;
	myHuman->GetParamData(NULL, NULL, NULL, &myteam);
	target->GetParamData(NULL, NULL, NULL, &target_team);

	if (target_team == myteam) { return false; }

	float px, py, pz, prx;
	float tx, ty, tz, trx;
	float rx, ry;

	myHuman->GetPosData(&px, &py, &pz, &prx);
	myHuman->GetRxRy(&rx, &ry);
	target->GetPosData(&tx, &ty, &tz, &trx);

	float player_eye_y = py + VIEW_HEIGHT;
	float target_eye_y = ty + VIEW_HEIGHT;

	if (myHuman->GetCrouchFlag() == true) {
		player_eye_y -= 2.0f;
	}
	if (target->GetCrouchFlag() == true) {
		target_eye_y -= 2.0f;
	}

	float vx = tx - px;
	float vy = target_eye_y - player_eye_y;
	float vz = tz - pz;
	float dist_sq = vx * vx + vy * vy + vz * vz;

	if (dist_sq <= 0.001f) { return false; }

	float dist = sqrtf(dist_sq);
	if (dist > PLAYER_SKILL_SUPPRESS_MAX_DIST) { return false; }

	vx /= dist;
	vy /= dist;
	vz /= dist;

	float aim_rx = rx * -1.0f + (float)M_PI / 2.0f;
	float fx = cosf(aim_rx) * cosf(ry);
	float fy = sinf(ry);
	float fz = sinf(aim_rx) * cosf(ry);

	float dot = fx * vx + fy * vy + fz * vz;
	float min_dot = cosf(DegreeToRadian(35.0f));

	if (dot < min_dot) { return false; }

	float block_dist = 0.0f;
	bool blocked = CollD.CheckALLBlockIntersectRay(
		px,
		player_eye_y,
		pz,
		vx,
		vy,
		vz,
		NULL,
		NULL,
		&block_dist,
		dist
	);

	if (blocked == true) { return false; }

	return true;
}

bool maingame::IsEnemySuppressableByPlayerSkill(human* myHuman, human* target)
{
	if (target == NULL) { return false; }

	// 로봇은 심리적 제압/무기 투하 대상에서 제외한다.
	// 로봇 대응은 나중에 EMP/해킹 계열 스킬로 분리하는 것이 자연스럽다.
	if (IsPlayerSkillRobotHuman(target) == true) {
		return false;
	}

	// 무기가 없는 적은 제압해도 실제 효과가 없으므로 먼저 제외한다.
	// 이 검사는 가볍기 때문에, 벽 ray 검사보다 먼저 하는 것이 안전하다.
	if (target->HasAnyWeaponForSuppress() == false) {
		return false;
	}

	return IsEnemyInPlayerSuppressSight(myHuman, target);
}

bool maingame::HasSuppressableEnemyInPlayerSightForHud(human* myHuman)
{
	if (myHuman == NULL) { return false; }

	int player_id = ObjMgr.GetPlayerID();

	for (int i = 0; i < MAX_HUMAN; i++) {
		if (i == player_id) { continue; }

		human* target = ObjMgr.GetHumanObject(i);
		if (target == NULL) { continue; }

		if (IsEnemySuppressableByPlayerSkill(myHuman, target) == true) {
			return true;
		}
	}

	return false;
}

// 경찰 제압 명령: 시야 내 적 무기 강제 드롭
int maingame::SuppressEnemiesInSight(human* myHuman)
{
	if (myHuman == NULL) { return 0; }

	int player_id = ObjMgr.GetPlayerID();
	int suppressed_count = 0;

	for (int i = 0; i < MAX_HUMAN; i++) {
		if (i == player_id) { continue; }

		human* target = ObjMgr.GetHumanObject(i);
		if (target == NULL) { continue; }

		if (IsEnemySuppressableByPlayerSkill(myHuman, target) == false) {
			continue;
		}

		if (target->ForceDropAllWeapons() == true) {
			target->StartSkillSuppressed(PLAYER_SKILL_SUPPRESS_MARK_FRAMES);
			player_skill_suppress_mark_timer[i] = PLAYER_SKILL_SUPPRESS_MARK_FRAMES;
			suppressed_count++;
		}
	}

	return suppressed_count;
}


// 요원A 마킹 샷: 가장 가까운 적 찾기
int maingame::FindNearestEnemyForMarkShot(human* myHuman)
{
	if (myHuman == NULL) { return -1; }

	int myteam = -1;
	myHuman->GetParamData(NULL, NULL, NULL, &myteam);

	float myx, myy, myz, myrx;
	float view_rx, view_ry;

	myHuman->GetPosData(&myx, &myy, &myz, &myrx);
	myHuman->GetRxRy(&view_rx, &view_ry);

	float player_eye_y = myy + VIEW_HEIGHT;
	if (myHuman->GetCrouchFlag() == true) {
		player_eye_y -= 2.0f;
	}

	// 현재 조준 방향. 로봇 처형 프로토콜과 같은 방식으로 계산한다.
	float aim_rx = view_rx * -1.0f + (float)M_PI / 2.0f;

	float forward_x = cosf(aim_rx) * cosf(view_ry);
	float forward_y = sinf(view_ry);
	float forward_z = sinf(aim_rx) * cosf(view_ry);

	int nearest_id = -1;
	float nearest_score = 0.0f;
	float max_dist_sq = PLAYER_SKILL_MARKSHOT_MAX_DIST * PLAYER_SKILL_MARKSHOT_MAX_DIST;

	// 점프 가능 높이 정도는 같은 높이로 취급한다.
	const float MARKSHOT_SAME_HEIGHT_THRESHOLD = 15.0f;

	// 큰 단차에 대한 보정 강도.
	const float MARKSHOT_HEIGHT_PENALTY = 6.0f;

	// 조준 방향/화면 중앙 보정 강도.
	// 값이 너무 크면 가까운 적보다 멀리 있는 정면 적이 과하게 우선될 수 있으므로 약하게 둔다.
	const float MARKSHOT_AIM_PENALTY = 12000.0f;

	for (int i = 0; i < MAX_HUMAN; i++) {
		human* target = ObjMgr.GetHumanObject(i);

		if (target == NULL) { continue; }
		if (target == myHuman) { continue; }
		if (target->GetEnableFlag() == false) { continue; }
		if (target->GetDeadFlag() == true) { continue; }
		if (target->GetHP() <= 0) { continue; }

		int target_team = -1;
		target->GetParamData(NULL, NULL, NULL, &target_team);

		if (target_team == myteam) { continue; }

		float x, y, z, rx;
		target->GetPosData(&x, &y, &z, &rx);

		float target_center_y = y + HUMAN_HEIGHT * 0.60f;

		float dx = x - myx;
		float dy = y - myy;
		float dz = z - myz;

		// 사거리 판정은 기존처럼 실제 3D 거리로 유지한다.
		float dist_sq = dx * dx + dy * dy + dz * dz;

		if (dist_sq > max_dist_sq) {
			continue;
		}

		// 기본 선택 기준은 XZ 거리.
		float xz_dist_sq = dx * dx + dz * dz;

		// 점프 가능 높이를 넘는 큰 Y축 차이에만 불이익을 준다.
		float abs_dy = fabsf(dy);
		float adjusted_dy = 0.0f;

		if (abs_dy > MARKSHOT_SAME_HEIGHT_THRESHOLD) {
			adjusted_dy = abs_dy - MARKSHOT_SAME_HEIGHT_THRESHOLD;
		}

		float height_score = adjusted_dy * adjusted_dy * MARKSHOT_HEIGHT_PENALTY;

		// 조준 방향/화면 중앙 보정.
// 대상이 조준 방향에 가까울수록 aim_penalty가 작아진다.
		float vx = x - myx;
		float vy = target_center_y - player_eye_y;
		float vz = z - myz;

		float aim_dist_sq = vx * vx + vy * vy + vz * vz;
		float aim_penalty = MARKSHOT_AIM_PENALTY;

		if (aim_dist_sq > 0.001f) {
			float aim_dist = sqrtf(aim_dist_sq);

			vx /= aim_dist;
			vy /= aim_dist;
			vz /= aim_dist;

			float aim_dot = forward_x * vx + forward_y * vy + forward_z * vz;

			if (aim_dot < -1.0f) { aim_dot = -1.0f; }
			if (aim_dot > 1.0f) { aim_dot = 1.0f; }

			// aim_dot이 1이면 정중앙, 0이면 옆쪽, -1이면 뒤쪽이다.
			aim_penalty = (1.0f - aim_dot) * MARKSHOT_AIM_PENALTY;

			// 높이 차이가 큰 대상은 조준 보정을 약하게 적용한다.
			// 단, 정말 정중앙에 가까운 경우에는 플레이어가 의도적으로 조준한 것으로 보고 보정을 어느 정도 허용한다.
			if (adjusted_dy > 0.0f) {
				if (aim_dot >= 0.97f) {
					// 거의 정중앙이면 위/아래층 적도 선택될 수 있게 한다.
					aim_penalty *= 0.75f;
				}
				else if (aim_dot >= 0.92f) {
					// 대략 조준 중이면 보정을 조금만 약화한다.
					aim_penalty *= 1.20f;
				}
				else {
					// 높이 차이가 큰데 정중앙도 아니면 조준 보정을 강하게 약화한다.
					aim_penalty *= 1.80f;
				}
			}
		}

		float score = xz_dist_sq + height_score + aim_penalty;

		if ((nearest_id < 0) || (score < nearest_score)) {
			nearest_id = i;
			nearest_score = score;
		}
	}

	return nearest_id;
}

bool maingame::HasEnemyInMarkShotRangeForHud(human* myHuman)
{
	if (myHuman == NULL) { return false; }

	return (FindNearestEnemyForMarkShot(myHuman) >= 0);
}



static bool IsPlayerSkillDeviceVisibleToHuman(float device_x, float device_y, float device_z, human* target, float device_view_height)
{
	if (target == NULL) { return false; }
	if (target->GetEnableFlag() == false) { return false; }
	if (target->GetHP() <= 0) { return false; }
	if (target->GetDeadFlag() == true) { return false; }

	float target_x, target_y, target_z;
	target->GetPosData(&target_x, &target_y, &target_z, NULL);

	float ray_start_y = device_y + device_view_height;
	float ray_end_y = target_y + HUMAN_HEIGHT * 0.5f;

	float ray_x = target_x - device_x;
	float ray_y = ray_end_y - ray_start_y;
	float ray_z = target_z - device_z;

	float ray_len = sqrtf(ray_x * ray_x + ray_y * ray_y + ray_z * ray_z);

	if (ray_len <= 0.001f) {
		return true;
	}

	float dummy_dist = 0.0f;

	if (CollD.CheckALLBlockIntersectRay(
		device_x,
		ray_start_y,
		device_z,
		ray_x / ray_len,
		ray_y / ray_len,
		ray_z / ray_len,
		NULL,
		NULL,
		&dummy_dist,
		ray_len
	) == true) {
		return false;
	}

	return true;
}

void maingame::ApplyDistractionSignal()
{
	if (player_skill_distraction_timer <= 0) { return; }

	int player_id = ObjMgr.GetPlayerID();

	human* playerHuman = ObjMgr.GetPlayerHumanObject();
	if (playerHuman == NULL) { return; }

	int player_team = -1;
	playerHuman->GetParamData(NULL, NULL, NULL, &player_team);

	bool affected_any_enemy = false;

	for (int i = 0; i < MAX_HUMAN; i++) {
		if (i == player_id) { continue; }

		human* target = ObjMgr.GetHumanObject(i);
		if (target == NULL) { continue; }
		if (target->GetEnableFlag() == false) { continue; }
		if (target->GetHP() <= 0) { continue; }
		if (target->GetDeadFlag() == true) { continue; }

		int target_team = -1;
		target->GetParamData(NULL, NULL, NULL, &target_team);

		// 아군은 교란하지 않는다.
		if (target_team == player_team) { continue; }

		float x, y, z, rx;
		target->GetPosData(&x, &y, &z, &rx);

		float dx = player_skill_distraction_x - x;
		float dz = player_skill_distraction_z - z;
		float dist_sq = dx * dx + dz * dz;
		if (dist_sq > PLAYER_SKILL_DISTRACTION_RADIUS * PLAYER_SKILL_DISTRACTION_RADIUS) {
			continue;
		}

		// 교란 장치는 소리/전자 신호처럼 취급한다.
		// 벽 너머의 적도 반응해야 하므로, 여기서는 벽 체크를 하지 않는다.
		// 단, 실제 화면 렌더링은 ResetZbuffer()를 쓰지 않아 벽/지형 뒤에서는 보이지 않는다.

		// 이미 플레이어를 직접 보고 전투 중인 적은 완전히 속지 않는다.
		int ai_mode = 0;
		HumanAI[i].GetBattleMode(&ai_mode, NULL);

		// 이미 플레이어와 직접 전투 중인 적은 완전히 속지 않는다.
		if ((ai_mode == AI_ACTION) && (HumanAI[i].GetEnemyHumanID() == player_id)) {
			continue;
		}

		// 교란 지점을 바라보는 각도
		float look_rx = (float)M_PI / 2.0f - atan2f(dz, dx);

		HumanAI[i].SetCautionMode();
		HumanAI[i].SetHoldWait(player_skill_distraction_x, player_skill_distraction_z, look_rx);

		affected_any_enemy = true;
	}
}

bool maingame::StartPlayerDistractionInstall(human* myHuman, int slot, float tx, float ty, float tz)
{
	if (myHuman == NULL) { return false; }

	if (IsPlayerInstallSkillPending() == true) {
		return false;
	}

	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_DISTRACTION, tx, ty, tz) == false) {
		return false;
	}

	StartPlayerSkillInstallState(
		player_skill_distraction_install_pending,
		player_skill_distraction_install_timer,
		player_skill_distraction_install_slot,
		player_skill_distraction_install_x,
		player_skill_distraction_install_y,
		player_skill_distraction_install_z,
		slot,
		tx,
		ty,
		tz,
		PLAYER_SKILL_DISTRACTION_INSTALL_DELAY_FRAMES
	);

	return true;
}

void maingame::ProcessPlayerDistractionInstall(human* myHuman)
{
	if (player_skill_distraction_install_pending == false) {
		return;
	}

	if (IsPlayerSkillInstallOwnerValid(myHuman) == false) {
		ResetPlayerSkillInstallState(
			player_skill_distraction_install_pending,
			player_skill_distraction_install_timer,
			player_skill_distraction_install_slot,
			player_skill_distraction_install_x,
			player_skill_distraction_install_y,
			player_skill_distraction_install_z,
			PLAYER_SKILL_SLOT_PRIMARY
		);
		return;
	}

	if (IsPlayerCurrentWeaponReloading(myHuman) == true) {
		ResetPlayerSkillInstallState(
			player_skill_distraction_install_pending,
			player_skill_distraction_install_timer,
			player_skill_distraction_install_slot,
			player_skill_distraction_install_x,
			player_skill_distraction_install_y,
			player_skill_distraction_install_z,
			PLAYER_SKILL_SLOT_PRIMARY
		);
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	if (player_skill_distraction_install_timer > 0) {
		player_skill_distraction_install_timer--;
		return;
	}

	float tx = player_skill_distraction_install_x;
	float ty = player_skill_distraction_install_y;
	float tz = player_skill_distraction_install_z;
	int install_slot = NormalizePlayerSkillSlot(player_skill_distraction_install_slot);

	ResetPlayerSkillInstallState(
		player_skill_distraction_install_pending,
		player_skill_distraction_install_timer,
		player_skill_distraction_install_slot,
		player_skill_distraction_install_x,
		player_skill_distraction_install_y,
		player_skill_distraction_install_z,
		PLAYER_SKILL_SLOT_PRIMARY
	);

	// 0.5초 뒤 실제 생성 시점에도 다시 한 번 검사한다.
	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_DISTRACTION, tx, ty, tz) == false) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	player_skill_distraction_x = tx;
	player_skill_distraction_y = ty;
	player_skill_distraction_z = tz;
	player_skill_distraction_timer = PLAYER_SKILL_DISTRACTION_FRAMES;
	player_skill_distraction_sound_timer = PLAYER_SKILL_DISTRACTION_BEEP_INTERVAL_FRAMES;
	player_skill_distraction_sound_count = 1;

	StartPlayerSkillCooldownForSlot(
		install_slot,
		PLAYER_SKILL_DISTRACTION,
		PLAYER_SKILL_DISTRACTION_COOLDOWN_FRAMES
	);

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);

	// 신호기 위치에서 즉시 1회 beep
	PlayPlayerSkill3DSound(
		SKILL_SOUND_DISTRACTION_BEEP,
		player_skill_distraction_x,
		player_skill_distraction_y,
		player_skill_distraction_z
	);
}

int maingame::GetPlayerTerroristMineCount()
{
	int count = 0;

	for (int i = 0; i < MAX_TERRORIST_MINES; i++) {
		if (player_skill_terrorist_mine_active[i] == true) {
			count++;
		}
	}

	return count;
}

void maingame::ClearPlayerTerroristMines()
{
	for (int i = 0; i < MAX_TERRORIST_MINES; i++) {
		player_skill_terrorist_mine_active[i] = false;
		player_skill_terrorist_mine_x[i] = 0.0f;
		player_skill_terrorist_mine_y[i] = 0.0f;
		player_skill_terrorist_mine_z[i] = 0.0f;
		player_skill_terrorist_mine_rx[i] = 0.0f;
		player_skill_terrorist_mine_owner_team[i] = -1;
		player_skill_terrorist_mine_owner_human_id[i] = -1;
		player_skill_terrorist_mine_arm_timer[i] = 0;
	}

	player_skill_terrorist_mine_next_index = 0;

	player_skill_terrorist_mine_install_pending = false;
	player_skill_terrorist_mine_install_timer = 0;
	player_skill_terrorist_mine_install_slot = PLAYER_SKILL_SLOT_SECONDARY;
	player_skill_terrorist_mine_install_x = 0.0f;
	player_skill_terrorist_mine_install_y = 0.0f;
	player_skill_terrorist_mine_install_z = 0.0f;

	player_skill_terrorist_mine_pending_explosion = false;
	player_skill_terrorist_mine_pending_timer = 0;
	player_skill_terrorist_mine_pending_x = 0.0f;
	player_skill_terrorist_mine_pending_y = 0.0f;
	player_skill_terrorist_mine_pending_z = 0.0f;
	player_skill_terrorist_mine_pending_owner_team = -1;
	player_skill_terrorist_mine_pending_owner_human_id = -1;
}


bool maingame::InstallPlayerTerroristMineAt(human* myHuman, float tx, float ty, float tz)
{
	if (myHuman == NULL) { return false; }

	if (IsPlayerSkillSmallDevicePlacementValid(tx, ty, tz) == false) {
		return false;
	}

	int slot = -1;

	for (int i = 0; i < MAX_TERRORIST_MINES; i++) {
		int check_slot = (player_skill_terrorist_mine_next_index + i) % MAX_TERRORIST_MINES;

		if (player_skill_terrorist_mine_active[check_slot] == false) {
			slot = check_slot;
			break;
		}
	}

	if (slot < 0) {
		// 빈 슬롯이 없으면 next_index가 가리키는 가장 오래된 지뢰를 덮어쓴다.
		slot = player_skill_terrorist_mine_next_index;
	}

	int owner_team = -1;
	myHuman->GetParamData(NULL, NULL, NULL, &owner_team);

	float px, py, pz, prx;
	myHuman->GetPosData(&px, &py, &pz, &prx);

	float dir_x = tx - px;
	float dir_z = tz - pz;
	float dir_len = sqrtf(dir_x * dir_x + dir_z * dir_z);
	float mine_rx = prx * -1.0f + (float)M_PI / 2.0f;

	if (dir_len > 0.001f) {
		mine_rx = atan2f(dir_z, dir_x);
	}

	player_skill_terrorist_mine_active[slot] = true;
	player_skill_terrorist_mine_x[slot] = tx;
	player_skill_terrorist_mine_y[slot] = ty;
	player_skill_terrorist_mine_z[slot] = tz;
	player_skill_terrorist_mine_rx[slot] = mine_rx;
	player_skill_terrorist_mine_owner_team[slot] = owner_team;
	player_skill_terrorist_mine_owner_human_id[slot] = ObjMgr.GetPlayerID();
	player_skill_terrorist_mine_arm_timer[slot] = PLAYER_SKILL_TERRORIST_MINE_ARM_DELAY_FRAMES;

	player_skill_terrorist_mine_next_index = (slot + 1) % MAX_TERRORIST_MINES;

	return true;
}

bool maingame::StartPlayerTerroristMineInstall(human* myHuman, int slot, float tx, float ty, float tz)
{
	if (myHuman == NULL) { return false; }

	if (IsPlayerInstallSkillPending() == true) {
		return false;
	}

	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_TERRORIST_MINE, tx, ty, tz) == false) {
		return false;
	}

	StartPlayerSkillInstallState(
		player_skill_terrorist_mine_install_pending,
		player_skill_terrorist_mine_install_timer,
		player_skill_terrorist_mine_install_slot,
		player_skill_terrorist_mine_install_x,
		player_skill_terrorist_mine_install_y,
		player_skill_terrorist_mine_install_z,
		slot,
		tx,
		ty,
		tz,
		PLAYER_SKILL_TERRORIST_MINE_INSTALL_DELAY_FRAMES
	);

	return true;
}

void maingame::ProcessPlayerTerroristMineInstall(human* myHuman)
{
	if (player_skill_terrorist_mine_install_pending == false) {
		return;
	}

	if (IsPlayerSkillInstallOwnerValid(myHuman) == false) {
		ResetPlayerSkillInstallState(
			player_skill_terrorist_mine_install_pending,
			player_skill_terrorist_mine_install_timer,
			player_skill_terrorist_mine_install_slot,
			player_skill_terrorist_mine_install_x,
			player_skill_terrorist_mine_install_y,
			player_skill_terrorist_mine_install_z,
			PLAYER_SKILL_SLOT_SECONDARY
		);
		return;
	}

	if (IsPlayerCurrentWeaponReloading(myHuman) == true) {
		ResetPlayerSkillInstallState(
			player_skill_terrorist_mine_install_pending,
			player_skill_terrorist_mine_install_timer,
			player_skill_terrorist_mine_install_slot,
			player_skill_terrorist_mine_install_x,
			player_skill_terrorist_mine_install_y,
			player_skill_terrorist_mine_install_z,
			PLAYER_SKILL_SLOT_SECONDARY
		);
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	if (player_skill_terrorist_mine_install_timer > 0) {
		player_skill_terrorist_mine_install_timer--;
		return;
	}

	float tx = player_skill_terrorist_mine_install_x;
	float ty = player_skill_terrorist_mine_install_y;
	float tz = player_skill_terrorist_mine_install_z;
	int install_slot = NormalizePlayerSkillSlot(player_skill_terrorist_mine_install_slot);

	ResetPlayerSkillInstallState(
		player_skill_terrorist_mine_install_pending,
		player_skill_terrorist_mine_install_timer,
		player_skill_terrorist_mine_install_slot,
		player_skill_terrorist_mine_install_x,
		player_skill_terrorist_mine_install_y,
		player_skill_terrorist_mine_install_z,
		PLAYER_SKILL_SLOT_SECONDARY
	);

	// 0.3초 뒤 실제 설치 시점에도 다시 한 번 검사한다.
	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_TERRORIST_MINE, tx, ty, tz) == false) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	if (InstallPlayerTerroristMineAt(myHuman, tx, ty, tz) == false) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	StartPlayerSkillCooldownForSlot(
		install_slot,
		PLAYER_SKILL_TERRORIST_MINE,
		PLAYER_SKILL_TERRORIST_MINE_COOLDOWN_FRAMES
	);

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
}

void maingame::ProcessPlayerTerroristMines()
{
	// 감지음이 폭발음에 묻히지 않도록, 감지 후 몇 프레임 뒤에 폭발시킨다.
	if (player_skill_terrorist_mine_pending_explosion == true) {
		if (player_skill_terrorist_mine_pending_timer > 0) {
			player_skill_terrorist_mine_pending_timer--;
			return;
		}

		ObjMgr.ExplodeSkillMine(
			player_skill_terrorist_mine_pending_x,
			player_skill_terrorist_mine_pending_y + 2.0f,
			player_skill_terrorist_mine_pending_z,
			player_skill_terrorist_mine_pending_owner_team,
			player_skill_terrorist_mine_pending_owner_human_id,
			ID_WEAPON_GRENADE,
			PLAYER_SKILL_TERRORIST_MINE_EXPLOSION_RANGE_SCALE
		);

		player_skill_terrorist_mine_pending_explosion = false;
		player_skill_terrorist_mine_pending_timer = 0;
		player_skill_terrorist_mine_pending_x = 0.0f;
		player_skill_terrorist_mine_pending_y = 0.0f;
		player_skill_terrorist_mine_pending_z = 0.0f;
		player_skill_terrorist_mine_pending_owner_team = -1;
		player_skill_terrorist_mine_pending_owner_human_id = -1;
		return;
	}

	for (int mine_index = 0; mine_index < MAX_TERRORIST_MINES; mine_index++) {
		if (player_skill_terrorist_mine_active[mine_index] == false) {
			continue;
		}

		if (player_skill_terrorist_mine_arm_timer[mine_index] > 0) {
			player_skill_terrorist_mine_arm_timer[mine_index]--;

			if (player_skill_terrorist_mine_arm_timer[mine_index] == 0) {
				PlayPlayerSkillSound(SKILL_SOUND_TERRORIST_MINE_ARMED);
			}

			continue;
		}

		float mine_x = player_skill_terrorist_mine_x[mine_index];
		float mine_y = player_skill_terrorist_mine_y[mine_index];
		float mine_z = player_skill_terrorist_mine_z[mine_index];
		int owner_team = player_skill_terrorist_mine_owner_team[mine_index];
		int owner_human_id = player_skill_terrorist_mine_owner_human_id[mine_index];

		for (int human_index = 0; human_index < MAX_HUMAN; human_index++) {
			if (human_index == owner_human_id) {
				continue;
			}

			human* target = ObjMgr.GetHumanObject(human_index);

			if (target == NULL) { continue; }
			if (target->GetEnableFlag() == false) { continue; }
			if (target->GetHP() <= 0) { continue; }
			if (target->GetDeadFlag() == true) { continue; }

			int target_team = -1;
			target->GetParamData(NULL, NULL, NULL, &target_team);

			// 설치자의 아군은 감지 대상으로 보지 않는다.
			if ((owner_team >= 0) && (target_team == owner_team)) {
				continue;
			}

			float target_x, target_y, target_z;
			target->GetPosData(&target_x, &target_y, &target_z, NULL);

			float dx = target_x - mine_x;
			float dy = target_y - mine_y;
			float dz = target_z - mine_z;

			if (dy < 0.0f) {
				dy *= -1.0f;
			}

			if (dy > PLAYER_SKILL_TERRORIST_MINE_TRIGGER_HEIGHT) {
				continue;
			}

			float dist_sq = dx * dx + dz * dz;
			float trigger_radius = PLAYER_SKILL_TERRORIST_MINE_TRIGGER_RADIUS;

			if (dist_sq > trigger_radius * trigger_radius) {
				continue;
			}

			// 지뢰와 적 사이에 벽이 있으면 적이 지뢰를 보지 못하므로 감지하지 않는다.
			if (IsPlayerSkillDeviceVisibleToHuman(
				mine_x,
				mine_y,
				mine_z,
				target,
				1.0f
			) == false) {
				continue;
			}

			// 감지된 지뢰는 즉시 제거하고, 짧은 시간 뒤 폭발시킨다.
			player_skill_terrorist_mine_active[mine_index] = false;

			player_skill_terrorist_mine_pending_explosion = true;
			player_skill_terrorist_mine_pending_timer = PLAYER_SKILL_TERRORIST_MINE_EXPLODE_DELAY_FRAMES;
			player_skill_terrorist_mine_pending_x = mine_x;
			player_skill_terrorist_mine_pending_y = mine_y;
			player_skill_terrorist_mine_pending_z = mine_z;
			player_skill_terrorist_mine_pending_owner_team = owner_team;
			player_skill_terrorist_mine_pending_owner_human_id = owner_human_id;

			// 3D 사운드는 거리 감쇠와 폭발음 때문에 묻힐 수 있으므로 감지음은 2D로 확실히 들리게 한다.
			PlayPlayerSkillSound(SKILL_SOUND_TERRORIST_MINE_TRIGGER);

			// 한 프레임에 여러 지뢰가 동시에 터지면 사운드/피해가 과하게 겹칠 수 있으므로 1개만 처리한다.
			return;
		}
	}
}

void maingame::RemovePlayerSummonedZombies()
{
	for (int i = 0; i < PLAYER_SKILL_SUMMON_COUNT; i++) {
		int id = player_skill_summon_ids[i];

		if ((0 <= id) && (id < MAX_HUMAN)) {
			human* h = ObjMgr.GetHumanObject(id);

			if ((h != NULL) && (h->GetEnableFlag() == true)) {
				h->SetSkillSummonedAllyFlag(false);
				h->SetHP(0);
				h->SetEnableFlag(false);
			}
		}

		player_skill_summon_ids[i] = -1;
		player_skill_summon_remove_timer[i] = 0;
	}
}

// 동료 소환용 아군 텍스쳐 후보 선택
int maingame::GetRandomSummonAllyTextureParamID(int player_team)
{
	// 요원B 계열을 기본 아군 외형 후보로 사용한다.
	// 실제 능력치와 AI는 좀비 29, 30, 31을 유지하고, 외형만 이 후보에서 고른다.
	int fallback_ids[PLAYER_SKILL_SUMMON_ALLY_TEXTURE_COUNT] = {
		22, 23, 34, 37, 38
	};

	int candidates[MAX_HUMAN + PLAYER_SKILL_SUMMON_ALLY_TEXTURE_COUNT];
	int candidate_count = 0;

	// 현재 맵에 살아 있는 같은 팀 인물이 있으면 우선 후보에 넣는다.
	for (int i = 0; i < MAX_HUMAN; i++) {
		human* h = ObjMgr.GetHumanObject(i);

		if (h == NULL) { continue; }
		if (h->GetEnableFlag() == false) { continue; }
		if (h->GetHP() <= 0) { continue; }
		if (h->GetDeadFlag() == true) { continue; }

		int param_id = -1;
		int team_id = -1;
		h->GetParamData(&param_id, NULL, NULL, &team_id);

		if (team_id != player_team) { continue; }

		if ((param_id < 0) || (TOTAL_PARAMETERINFO_HUMAN <= param_id)) {
			continue;
		}

		HumanParameter human_param;
		if (GameParamInfo.GetHuman(param_id, &human_param) != 0) {
			continue;
		}

		// 동료 소환 외형 후보는 일반 인간만 허용한다.
		// type 1 = 로봇, type 2 = 좀비이므로 제외한다.
		if (human_param.type != 0) {
			continue;
		}

		// 소환 좀비 자신은 외형 후보에서 제외한다.
		if ((param_id == PLAYER_SKILL_SUMMON_ZOMBIE_ID1) ||
			(param_id == PLAYER_SKILL_SUMMON_ZOMBIE_ID2) ||
			(param_id == PLAYER_SKILL_SUMMON_ZOMBIE_ID3)) {
			continue;
		}

		if ((param_id < 0) || (TOTAL_PARAMETERINFO_HUMAN <= param_id)) {
			continue;
		}

		candidates[candidate_count] = param_id;
		candidate_count++;
	}

	// 현재 맵에 같은 팀 후보가 하나도 없을 때만 요원B 계열 후보를 추가한다.
	if (candidate_count <= 0) {
		for (int i = 0; i < PLAYER_SKILL_SUMMON_ALLY_TEXTURE_COUNT; i++) {
			int id = fallback_ids[i];

			if ((id < 0) || (TOTAL_PARAMETERINFO_HUMAN <= id)) {
				continue;
			}

			candidates[candidate_count] = id;
			candidate_count++;
		}
	}

	if (candidate_count <= 0) {
		return -1;
	}

	return candidates[GetRand(candidate_count)];
}

// 소환된 좀비의 외형만 아군 텍스쳐로 교체한다.
void maingame::ApplySummonedAllyTexture(int human_id, int player_team)
{
	if ((human_id < 0) || (MAX_HUMAN <= human_id)) {
		return;
	}

	human* h = ObjMgr.GetHumanObject(human_id);

	if (h == NULL) { return; }
	if (h->GetEnableFlag() == false) { return; }

	// 소환 동료는 새 Human 파라미터를 만들지 않고,
	// 전용 인간 텍스쳐 ID를 직접 로드한다.
	// 이렇게 하면 44, 45 같은 기존 버그 인물 번호를 보존할 수 있다.
	int texture_id_param = PLAYER_SKILL_SUMMON_ALLY_TEXTURE_ID;

	if ((texture_id_param < 0) || (TOTAL_HUMANTEXTURE <= texture_id_param)) {
		return;
	}

	// 해당 텍스쳐가 현재 맵에 로드되어 있지 않으면 먼저 로드한다.
	Resource.AddHumanTextureByTextureID(texture_id_param);

	int texture_id = Resource.GetHumanTextureByTextureID(texture_id_param);

	if (texture_id != -1) {
		h->SetTexture(texture_id);

		// 소환 직후 텍스쳐를 직접 지정했으므로,
		// ObjectManager::Render()의 지연 텍스쳐 보정이 다시 덮어쓰지 못하게 한다.
		ObjMgr.ClearHumanFrameTextureRefresh(human_id);

		// 소환 동료는 상체 모델만 up1.x로 강제해서 썬글라스를 쓰게 한다.
		ObjMgr.ApplyHumanUpperModelIndex(
			human_id,
			PLAYER_SKILL_SUMMON_ALLY_UPMODEL_SUNGLASSES
		);
	}
}

bool maingame::StartPlayerSummonInstall(human* myHuman, int slot, float tx, float ty, float tz)
{
	if (myHuman == NULL) {
		return false;
	}

	if (IsPlayerInstallSkillPending() == true) {
		return false;
	}

	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_SUMMON, tx, ty, tz) == false) {
		return false;
	}

	StartPlayerSkillInstallState(
		player_skill_summon_install_pending,
		player_skill_summon_install_timer,
		player_skill_summon_install_slot,
		player_skill_summon_install_x,
		player_skill_summon_install_y,
		player_skill_summon_install_z,
		slot,
		tx,
		ty,
		tz,
		PLAYER_SKILL_SUMMON_INSTALL_DELAY_FRAMES
	);

	return true;
}

void maingame::ProcessPlayerSummonInstall(human* myHuman)
{
	if (player_skill_summon_install_pending == false) {
		return;
	}

	if (IsPlayerSkillInstallOwnerValid(myHuman) == false) {
		ResetPlayerSkillInstallState(
			player_skill_summon_install_pending,
			player_skill_summon_install_timer,
			player_skill_summon_install_slot,
			player_skill_summon_install_x,
			player_skill_summon_install_y,
			player_skill_summon_install_z,
			PLAYER_SKILL_SLOT_PRIMARY
		);
		return;
	}

	if (IsPlayerCurrentWeaponReloading(myHuman) == true) {
		ResetPlayerSkillInstallState(
			player_skill_summon_install_pending,
			player_skill_summon_install_timer,
			player_skill_summon_install_slot,
			player_skill_summon_install_x,
			player_skill_summon_install_y,
			player_skill_summon_install_z,
			PLAYER_SKILL_SLOT_PRIMARY
		);
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	if (player_skill_summon_install_timer > 0) {
		player_skill_summon_install_timer--;
		return;
	}

	float tx = player_skill_summon_install_x;
	float ty = player_skill_summon_install_y;
	float tz = player_skill_summon_install_z;
	int slot = player_skill_summon_install_slot;

	ResetPlayerSkillInstallState(
		player_skill_summon_install_pending,
		player_skill_summon_install_timer,
		player_skill_summon_install_slot,
		player_skill_summon_install_x,
		player_skill_summon_install_y,
		player_skill_summon_install_z,
		PLAYER_SKILL_SLOT_PRIMARY
	);

	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_SUMMON, tx, ty, tz) == false) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	int old_slot = player_skill_using_slot;
	player_skill_using_slot = slot;

	if (SummonPlayerZombies(tx, ty, tz) == false) {
		player_skill_using_slot = old_slot;
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	StartPlayerSkillCooldownForSlot(slot, PLAYER_SKILL_SUMMON, PLAYER_SKILL_SUMMON_COOLDOWN_FRAMES);

	player_skill_using_slot = old_slot;

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
}

void maingame::RemovePlayerShadowDecoy()
{
	if ((0 <= player_skill_shadow_decoy_id) && (player_skill_shadow_decoy_id < MAX_HUMAN)) {
		human* decoy = ObjMgr.GetHumanObject(player_skill_shadow_decoy_id);

		if (decoy != NULL) {
			if (decoy->GetEnableFlag() == true) {
				float x, y, z, rx;
				decoy->GetPosData(&x, &y, &z, &rx);

				SpawnPlayerSkillDisappearSmokeEffect(x, y, z);
			}

			decoy->SetSkillShadowDecoyFlag(false);
			decoy->ClearSkillShadowDecoyLastAttackerID();
			decoy->SetEnableFlag(false);
		}

		HumanAI[player_skill_shadow_decoy_id].SetForceNoAI(false);
		HumanAI[player_skill_shadow_decoy_id].SetNoFightFlag(false);
	}

	player_skill_shadow_decoy_id = -1;
	player_skill_shadow_decoy_timer = 0;
}

bool maingame::SummonPlayerShadowDecoy(human* myHuman, float x, float y, float z)
{
	if (myHuman == NULL) {
		return false;
	}

	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_SHADOW_DECOY, x, y, z) == false) {
		return false;
	}

	int player_param_id = -1;
	int player_team = -1;
	myHuman->GetParamData(&player_param_id, NULL, NULL, &player_team);

	if (player_param_id < 0) {
		return false;
	}

	float px = 0.0f;
	float py = 0.0f;
	float pz = 0.0f;
	float prx = 0.0f;
	myHuman->GetPosData(&px, &py, &pz, &prx);

	float sr = prx;
	float dx = x - px;
	float dz = z - pz;

	if ((dx * dx + dz * dz) > 0.0001f) {
		sr = (float)(M_PI / 2.0f) - atan2f(dz, dx);
	}

	int weapon_id[TOTAL_HAVEWEAPON];
	for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
		weapon_id[i] = ID_WEAPON_NONE;
	}

	RemovePlayerShadowDecoy();

	int new_id = ObjMgr.AddHumanIndex(x, y, z, sr, player_param_id, player_team, weapon_id, 0, false);

	if ((new_id < 0) || (MAX_HUMAN <= new_id)) {
		player_skill_shadow_decoy_id = -1;
		player_skill_shadow_decoy_timer = 0;
		return false;
	}

	human* decoy = ObjMgr.GetHumanObject(new_id);

	if (decoy == NULL) {
		player_skill_shadow_decoy_id = -1;
		player_skill_shadow_decoy_timer = 0;
		return false;
	}

	decoy->SetHP(PLAYER_SKILL_SHADOW_DECOY_HP);
	decoy->SetSkillShadowDecoyFlag(true);
	decoy->ClearSkillShadowDecoyLastAttackerID();

	HumanAI[new_id].Init();
	HumanAI[new_id].SetForceNoAI(true);
	HumanAI[new_id].SetNoFightFlag(true);

	player_skill_shadow_decoy_id = new_id;

	// 그림자 잔상은 지속시간으로 자동 제거하지 않는다.
	// 기존 잔상은 다음 그림자 잔상을 다시 설치할 때 RemovePlayerShadowDecoy()로 제거한다.
	player_skill_shadow_decoy_timer = 0;

	return true;
}

bool maingame::StartPlayerShadowDecoyInstall(human* myHuman, int slot, float tx, float ty, float tz)
{
	if (myHuman == NULL) {
		return false;
	}

	if (IsPlayerInstallSkillPending() == true) {
		return false;
	}

	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_SHADOW_DECOY, tx, ty, tz) == false) {
		return false;
	}

	StartPlayerSkillInstallState(
		player_skill_shadow_decoy_install_pending,
		player_skill_shadow_decoy_install_timer,
		player_skill_shadow_decoy_install_slot,
		player_skill_shadow_decoy_install_x,
		player_skill_shadow_decoy_install_y,
		player_skill_shadow_decoy_install_z,
		slot,
		tx,
		ty,
		tz,
		PLAYER_SKILL_SHADOW_DECOY_INSTALL_DELAY_FRAMES
	);

	return true;
}

void maingame::ProcessPlayerShadowDecoyInstall(human* myHuman)
{
	if (player_skill_shadow_decoy_install_pending == false) {
		return;
	}

	if (IsPlayerSkillInstallOwnerValid(myHuman) == false) {
		ResetPlayerSkillInstallState(
			player_skill_shadow_decoy_install_pending,
			player_skill_shadow_decoy_install_timer,
			player_skill_shadow_decoy_install_slot,
			player_skill_shadow_decoy_install_x,
			player_skill_shadow_decoy_install_y,
			player_skill_shadow_decoy_install_z,
			PLAYER_SKILL_SLOT_SECONDARY
		);
		return;
	}

	if (IsPlayerCurrentWeaponReloading(myHuman) == true) {
		ResetPlayerSkillInstallState(
			player_skill_shadow_decoy_install_pending,
			player_skill_shadow_decoy_install_timer,
			player_skill_shadow_decoy_install_slot,
			player_skill_shadow_decoy_install_x,
			player_skill_shadow_decoy_install_y,
			player_skill_shadow_decoy_install_z,
			PLAYER_SKILL_SLOT_SECONDARY
		);
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	if (player_skill_shadow_decoy_install_timer > 0) {
		player_skill_shadow_decoy_install_timer--;
		return;
	}

	float tx = player_skill_shadow_decoy_install_x;
	float ty = player_skill_shadow_decoy_install_y;
	float tz = player_skill_shadow_decoy_install_z;
	int slot = player_skill_shadow_decoy_install_slot;

	ResetPlayerSkillInstallState(
		player_skill_shadow_decoy_install_pending,
		player_skill_shadow_decoy_install_timer,
		player_skill_shadow_decoy_install_slot,
		player_skill_shadow_decoy_install_x,
		player_skill_shadow_decoy_install_y,
		player_skill_shadow_decoy_install_z,
		PLAYER_SKILL_SLOT_SECONDARY
	);

	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_SHADOW_DECOY, tx, ty, tz) == false) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	int old_slot = player_skill_using_slot;
	player_skill_using_slot = slot;

	if (SummonPlayerShadowDecoy(myHuman, tx, ty, tz) == false) {
		player_skill_using_slot = old_slot;
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	StartPlayerSkillCooldownForSlot(slot, PLAYER_SKILL_SHADOW_DECOY, PLAYER_SKILL_SHADOW_DECOY_COOLDOWN_FRAMES);

	player_skill_using_slot = old_slot;

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
}

void maingame::ProcessPlayerShadowDecoy(human* myHuman)
{
	if ((player_skill_shadow_decoy_id < 0) || (MAX_HUMAN <= player_skill_shadow_decoy_id)) {
		player_skill_shadow_decoy_id = -1;
		player_skill_shadow_decoy_timer = 0;
		return;
	}

	human* decoy = ObjMgr.GetHumanObject(player_skill_shadow_decoy_id);

	if (decoy == NULL) {
		player_skill_shadow_decoy_id = -1;
		player_skill_shadow_decoy_timer = 0;
		return;
	}

	if (decoy->GetEnableFlag() == false) {
		RemovePlayerShadowDecoy();
		return;
	}

	int attacker_id = decoy->GetSkillShadowDecoyLastAttackerID();

	if ((0 <= attacker_id) && (attacker_id < MAX_HUMAN)) {
		human* attacker = ObjMgr.GetHumanObject(attacker_id);

		if ((attacker != NULL) && (attacker->GetEnableFlag() == true) &&
			(attacker->GetDeadFlag() == false) && (attacker->GetHP() > 0)) {
			int player_team = -1;
			int attacker_team = -1;

			if (myHuman != NULL) {
				myHuman->GetParamData(NULL, NULL, NULL, &player_team);
			}

			attacker->GetParamData(NULL, NULL, NULL, &attacker_team);

			if ((player_team < 0) || (attacker_team != player_team)) {
				// 새 핑이 처음 생기는 순간에만 효과음을 재생한다.
				// 이미 핑이 유지 중인 적을 다시 공격했을 때는 타이머만 갱신한다.
				if (player_skill_shadow_decoy_ping_timer[attacker_id] <= 0) {
					PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
				}

				player_skill_shadow_decoy_ping_timer[attacker_id] = PLAYER_SKILL_SHADOW_DECOY_PING_FRAMES;
			}
		}

		decoy->ClearSkillShadowDecoyLastAttackerID();
	}

	if ((decoy->GetDeadFlag() == true) || (decoy->GetHP() <= 0)) {
		// 잔상이 죽으면 일반 시체로 남기지 않고 바로 비활성화한다.
		RemovePlayerShadowDecoy();
		return;
	}

		// 그림자 잔상은 시간 경과로 자동 제거하지 않는다.
	// 제거 조건은 아래 두 가지로 제한한다.
	// 1. 잔상이 죽었을 때
	// 2. 새 그림자 잔상을 다시 설치할 때
}

void maingame::ProcessPlayerShadowDecoyPings()
{
	for (int i = 0; i < MAX_HUMAN; i++) {
		if (player_skill_shadow_decoy_ping_timer[i] > 0) {
			player_skill_shadow_decoy_ping_timer[i]--;
		}
	}
}

bool maingame::SummonPlayerZombies(float x, float y, float z)
{
	human* playerHuman = ObjMgr.GetPlayerHumanObject();
	if (playerHuman == NULL) { return false; }

	int player_id = ObjMgr.GetPlayerID();

	float px, py, pz, prx;
	playerHuman->GetPosData(&px, &py, &pz, &prx);

	int player_team = -1;
	playerHuman->GetParamData(NULL, NULL, NULL, &player_team);

	// 기존 소환 좀비 제거
	RemovePlayerSummonedZombies();

	int weapon_id[TOTAL_HAVEWEAPON];
	for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
		weapon_id[i] = ID_WEAPON_NONE;
	}

	int zombie_ids[PLAYER_SKILL_SUMMON_COUNT] = {
	PLAYER_SKILL_SUMMON_ZOMBIE_ID1,
	PLAYER_SKILL_SUMMON_ZOMBIE_ID2,
	PLAYER_SKILL_SUMMON_ZOMBIE_ID3
	};

	float offset_x[PLAYER_SKILL_SUMMON_COUNT];
	float offset_z[PLAYER_SKILL_SUMMON_COUNT];

	GetPlayerSkillSummonFormationOffsets(x, z, offset_x, offset_z);

	float center_dx = x - px;
	float center_dz = z - pz;
	float center_r = prx;

	if ((center_dx * center_dx + center_dz * center_dz) > 0.001f) {
		center_r = (float)M_PI / 2.0f - atan2f(center_dz, center_dx);
	}

	int created_count = 0;

	for (int i = 0; i < PLAYER_SKILL_SUMMON_COUNT; i++) {
		float sx = x + offset_x[i];
		float sy = y;
		float sz = z + offset_z[i];

		// 벽 안/낮은 천장/머리 걸림 소환 방지
		if (IsPlayerSkillHumanSpaceBlocked(sx, sy, sz) == true) {
			continue;
		}

		float sr = center_r;

		int new_id = ObjMgr.AddHumanIndex(
			sx,
			sy,
			sz,
			sr,
			zombie_ids[i],
			player_team,
			weapon_id,
			0,
			false
		);

		if (new_id >= 0) {
			int effect_index = created_count;

			player_skill_summon_ids[effect_index] = new_id;
			player_skill_summon_remove_timer[effect_index] = 0;
			player_skill_summon_effect_x[effect_index] = sx;
			player_skill_summon_effect_y[effect_index] = sy;
			player_skill_summon_effect_z[effect_index] = sz;

			created_count++;

			// 좀비 능력치는 유지하되, 외형만 아군 텍스쳐로 바꾼다.
			ApplySummonedAllyTexture(new_id, player_team);

			human* summoned = ObjMgr.GetHumanObject(new_id);
			if (summoned != NULL) {
				summoned->SetSkillSummonedAllyFlag(true);
			}

			HumanAI[new_id].Init();
			HumanAI[new_id].SetForceNoAI(false);

			// 소환 직후에는 평상시 플레이어를 따라오게 한다.
			// 적을 발견해 전투/경계 상태가 되면 기존 AI 판단을 우선한다.
			HumanAI[new_id].SetHoldTracking(player_id);
		}
	}

	if (created_count <= 0) {
		return false;
	}

	player_skill_summon_effect_count = created_count;
	player_skill_summon_effect_timer = PLAYER_SKILL_SUMMON_EFFECT_FRAMES;

	return true;
}

bool maingame::StartPlayerBioticField(human* myHuman, float tx, float ty, float tz)
{
	if (myHuman == NULL) { return false; }

	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_BIOTIC_FIELD, tx, ty, tz) == false) {
		return false;
	}

	player_skill_biotic_field_timer = PLAYER_SKILL_BIOTIC_FIELD_FRAMES;
	player_skill_biotic_field_heal_timer = 1;
	player_skill_biotic_field_x = tx;
	player_skill_biotic_field_y = ty;
	player_skill_biotic_field_z = tz;

	StartPlayerSkillCooldown(
		PLAYER_SKILL_BIOTIC_FIELD,
		PLAYER_SKILL_BIOTIC_FIELD_COOLDOWN_FRAMES
	);

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
	return true;
}

bool maingame::StartPlayerBioticFieldInstall(human* myHuman, int slot, float tx, float ty, float tz)
{
	if (myHuman == NULL) { return false; }

	if (IsPlayerInstallSkillPending() == true) {
		return false;
	}

	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_BIOTIC_FIELD, tx, ty, tz) == false) {
		return false;
	}

	StartPlayerSkillInstallState(
		player_skill_biotic_field_install_pending,
		player_skill_biotic_field_install_timer,
		player_skill_biotic_field_install_slot,
		player_skill_biotic_field_install_x,
		player_skill_biotic_field_install_y,
		player_skill_biotic_field_install_z,
		slot,
		tx,
		ty,
		tz,
		PLAYER_SKILL_BIOTIC_FIELD_INSTALL_DELAY_FRAMES
	);

	return true;
}

void maingame::ProcessPlayerBioticFieldInstall(human* myHuman)
{
	if (player_skill_biotic_field_install_pending == false) {
		return;
	}

	if (IsPlayerSkillInstallOwnerValid(myHuman) == false) {
		ResetPlayerSkillInstallState(
			player_skill_biotic_field_install_pending,
			player_skill_biotic_field_install_timer,
			player_skill_biotic_field_install_slot,
			player_skill_biotic_field_install_x,
			player_skill_biotic_field_install_y,
			player_skill_biotic_field_install_z,
			PLAYER_SKILL_SLOT_SECONDARY
		);
		return;
	}

	if (IsPlayerCurrentWeaponReloading(myHuman) == true) {
		ResetPlayerSkillInstallState(
			player_skill_biotic_field_install_pending,
			player_skill_biotic_field_install_timer,
			player_skill_biotic_field_install_slot,
			player_skill_biotic_field_install_x,
			player_skill_biotic_field_install_y,
			player_skill_biotic_field_install_z,
			PLAYER_SKILL_SLOT_SECONDARY
		);
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	if (player_skill_biotic_field_install_timer > 0) {
		player_skill_biotic_field_install_timer--;
		return;
	}

	float tx = player_skill_biotic_field_install_x;
	float ty = player_skill_biotic_field_install_y;
	float tz = player_skill_biotic_field_install_z;
	int install_slot = NormalizePlayerSkillSlot(player_skill_biotic_field_install_slot);

	ResetPlayerSkillInstallState(
		player_skill_biotic_field_install_pending,
		player_skill_biotic_field_install_timer,
		player_skill_biotic_field_install_slot,
		player_skill_biotic_field_install_x,
		player_skill_biotic_field_install_y,
		player_skill_biotic_field_install_z,
		PLAYER_SKILL_SLOT_SECONDARY
	);

	// 0.5초 뒤 실제 생성 시점에도 다시 한 번 검사한다.
	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_BIOTIC_FIELD, tx, ty, tz) == false) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	// StartPlayerBioticField()는 player_skill_using_slot 기준으로 쿨타임을 건다.
	// 설치 대기 중에는 입력 슬롯이 바뀌지 않지만, 안전하게 설치 슬롯을 잠시 복원한다.
	int prev_using_slot = player_skill_using_slot;
	player_skill_using_slot = install_slot;

	if (StartPlayerBioticField(myHuman, tx, ty, tz) == false) {
		player_skill_using_slot = prev_using_slot;
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	player_skill_using_slot = prev_using_slot;
}

void maingame::ProcessPlayerBioticField(human* myHuman)
{
	if (player_skill_biotic_field_timer <= 0) { return; }

	if (IsPlayerSkillInstallOwnerValid(myHuman) == false) {
		player_skill_biotic_field_timer = 0;
		player_skill_biotic_field_heal_timer = 0;
		return;
	}

	player_skill_biotic_field_timer--;

	if (player_skill_biotic_field_heal_timer > 0) {
		player_skill_biotic_field_heal_timer--;
	}

	if (player_skill_biotic_field_heal_timer <= 0) {
		player_skill_biotic_field_heal_timer = PLAYER_SKILL_BIOTIC_FIELD_HEAL_INTERVAL_FRAMES;

		int player_team = -1;
		myHuman->GetParamData(NULL, NULL, NULL, &player_team);

		float radius_sq =
			PLAYER_SKILL_BIOTIC_FIELD_RADIUS *
			PLAYER_SKILL_BIOTIC_FIELD_RADIUS;

		for (int i = 0; i < MAX_HUMAN; i++) {
			human* target = ObjMgr.GetHumanObject(i);
			if (target == NULL) { continue; }
			if (target->GetEnableFlag() == false) { continue; }
			if (target->GetDeadFlag() == true) { continue; }
			if (target->GetHP() <= 0) { continue; }

			int target_team = -1;
			target->GetParamData(NULL, NULL, NULL, &target_team);

			if (target_team != player_team) {
				continue;
			}

			float x, y, z, rx;
			target->GetPosData(&x, &y, &z, &rx);

			// 다른 층/큰 높이 차이의 아군까지 회복하지 않도록 높이 제한을 둔다.
			if (y < player_skill_biotic_field_y - 5.0f) { continue; }
			if (y > player_skill_biotic_field_y + PLAYER_SKILL_BIOTIC_FIELD_HEIGHT) { continue; }

			float dx = x - player_skill_biotic_field_x;
			float dz = z - player_skill_biotic_field_z;

			if ((dx * dx + dz * dz) > radius_sq) {
				continue;
			}

			target->HealHP(PLAYER_SKILL_BIOTIC_FIELD_HEAL_AMOUNT);
		}
	}

	if (player_skill_biotic_field_timer <= 0) {
		player_skill_biotic_field_timer = 0;
		player_skill_biotic_field_heal_timer = 0;
	}
}

static bool FindPlayerArtilleryStrikeDownSpreadPoint(float x, float y, float z, float* out_x, float* out_y, float* out_z)
{
	if (out_x == NULL) { return false; }
	if (out_y == NULL) { return false; }
	if (out_z == NULL) { return false; }

	float sample_r = PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_SAMPLE_R;
	float diag_r = sample_r * 0.70710678f;

	float sample_x[9] = {
		0.0f,
		sample_r,
		-sample_r,
		0.0f,
		0.0f,
		diag_r,
		diag_r,
		-diag_r,
		-diag_r
	};

	float sample_z[9] = {
		0.0f,
		0.0f,
		0.0f,
		sample_r,
		-sample_r,
		diag_r,
		-diag_r,
		diag_r,
		-diag_r
	};

	for (int i = 0; i < 9; i++) {
		float sx = x + sample_x[i];
		float sz = z + sample_z[i];

		// 시작점이 벽 내부면 열린 공간으로 보지 않는다.
		if (CollD.CheckALLBlockInside(sx, y + PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_CLEAR_Y, sz) == true) {
			continue;
		}

		float ray_start_y = y + PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_RAY_UP;
		float ground_dist = 0.0f;

		bool ground_hit = CollD.CheckALLBlockIntersectRay(
			sx,
			ray_start_y,
			sz,
			0.0f,
			-1.0f,
			0.0f,
			NULL,
			NULL,
			&ground_dist,
			PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_RAY_UP + PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_MAX_DROP
		);

		if (ground_hit == false) {
			continue;
		}

		float lower_y = ray_start_y - ground_dist + 0.5f;
		float drop = y - lower_y;

		// 바로 현재 바닥을 맞은 경우는 구멍/가장자리가 아니다.
		if (drop < PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_MIN_DROP) {
			continue;
		}

		if (drop > PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_MAX_DROP) {
			continue;
		}

		if (CollD.CheckALLBlockInside(sx, lower_y + PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_CLEAR_Y, sz) == true) {
			continue;
		}

		*out_x = sx;
		*out_y = lower_y;
		*out_z = sz;
		return true;
	}

	return false;
}

static void SpawnPlayerArtilleryStrikeDownSpreadExplosion(float x, float y, float z, int owner_team, int owner_human_id)
{
	float explosion_y = y + 2.0f;

	// 아래층으로 퍼진 보조 폭발은 시각 효과를 작게 유지한다.
	ObjMgr.AddEffect(
		x,
		explosion_y + 5.0f,
		z,
		0.0f,
		0.08f,
		0.0f,
		0.0f,
		DegreeToRadian(10) * GetRand(36),
		DegreeToRadian(GetRand(9) - 4),
		48.0f,
		0.32f,
		Resource.GetEffectSmokeTexture(),
		70,
		0,
		0.42f,
		-0.006f,
		EFFECT_NORMAL,
		38
	);

	ObjMgr.AddEffect(
		x,
		explosion_y,
		z,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		22.0f,
		0.0f,
		Resource.GetEffectMflashTexture(),
		0,
		0,
		0.45f,
		-0.080f,
		EFFECT_NORMAL,
		6
	);

	ObjMgr.ExplodeSkillMine(
		x,
		explosion_y,
		z,
		owner_team,
		owner_human_id,
		ID_WEAPON_GRENADE,
		PLAYER_SKILL_ARTILLERY_STRIKE_EXPLOSION_RANGE_SCALE * PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_RANGE_RATE
	);
}

static void SpawnPlayerArtilleryStrikeExplosion(float x, float y, float z, int owner_team, int owner_human_id)
{
	float explosion_y = y + PLAYER_SKILL_ARTILLERY_STRIKE_EXPLOSION_Y_OFFSET;

	// 실제 폭발 직전, 위에서 떨어지는 탄착 느낌을 주는 선행 연기.
	ObjMgr.AddEffect(
		x,
		explosion_y + 14.0f,
		z,
		0.0f,
		-0.20f,
		0.0f,
		0.0f,
		DegreeToRadian(10) * GetRand(36),
		0.0f,
		16.0f,
		0.28f,
		Resource.GetEffectSmokeTexture(),
		60,
		0,
		0.50f,
		-0.020f,
		EFFECT_NORMAL,
		24
	);

	// 근거리 섬광.
	ObjMgr.AddEffect(
		x,
		explosion_y,
		z,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		24.0f,
		0.0f,
		Resource.GetEffectMflashTexture(),
		0,
		0,
		0.74f,
		-0.090f,
		EFFECT_NORMAL,
		7
	);

	// 원거리에서도 보이는 큰 순간 섬광.
	// 일반 수류탄이 아니라 폭격 전용으로만 추가한다.
	ObjMgr.AddEffect(
		x,
		explosion_y + 5.0f,
		z,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		DegreeToRadian(10) * GetRand(36),
		0.0f,
		78.0f,
		-1.6f,
		Resource.GetEffectMflashTexture(),
		0,
		0,
		0.70f,
		-0.045f,
		EFFECT_NORMAL,
		16
	);

	// 원거리 식별용 큰 연기 구름.
	ObjMgr.AddEffect(
		x,
		explosion_y + 8.0f,
		z,
		0.0f,
		0.12f,
		0.0f,
		0.0f,
		DegreeToRadian(10) * GetRand(36),
		DegreeToRadian(GetRand(9) - 4),
		120.0f,
		0.42f,
		Resource.GetEffectSmokeTexture(),
		100,
		0,
		0.55f,
		-0.0045f,
		EFFECT_NORMAL,
		60
	);

	// 위로 치솟는 폭격 연기 기둥.
	// 멀리서 봤을 때 폭발 위치가 계속 남아 보이게 한다.
	ObjMgr.AddEffect(
		x,
		explosion_y + 24.0f,
		z,
		0.0f,
		0.20f,
		0.0f,
		0.0f,
		DegreeToRadian(10) * GetRand(36),
		DegreeToRadian(GetRand(11) - 5),
		64.0f,
		0.58f,
		Resource.GetEffectSmokeTexture(),
		80,
		0,
		0.40f,
		-0.0048f,
		EFFECT_NORMAL,
		104
	);

	// 실제 수류탄 폭발을 사용한다.
	// GrenadeExplosion() 내부에서 사람/소형 오브젝트 피해, 벽 체크, 피격 밀림, 폭발음, 폭발 이펙트를 처리한다.
	ObjMgr.ExplodeSkillMine(
		x,
		explosion_y,
		z,
		owner_team,
		owner_human_id,
		ID_WEAPON_GRENADE,
		PLAYER_SKILL_ARTILLERY_STRIKE_EXPLOSION_RANGE_SCALE
	);
}

bool maingame::StartPlayerArtilleryStrike(human* myHuman, int slot, float tx, float ty, float tz)
{
	if (myHuman == NULL) { return false; }

	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_ARTILLERY_STRIKE, tx, ty, tz) == false) {
		return false;
	}

	int owner_team = -1;
	myHuman->GetParamData(NULL, NULL, NULL, &owner_team);

	player_skill_artillery_strike_timer = PLAYER_SKILL_ARTILLERY_STRIKE_FRAMES;
	player_skill_artillery_strike_tick_timer = 1;
	player_skill_artillery_strike_x = tx;
	player_skill_artillery_strike_y = ty;
	player_skill_artillery_strike_z = tz;
	player_skill_artillery_strike_owner_team = owner_team;
	player_skill_artillery_strike_owner_human_id = ObjMgr.GetPlayerID();

	StartPlayerSkillCooldownForSlot(
		NormalizePlayerSkillSlot(slot),
		PLAYER_SKILL_ARTILLERY_STRIKE,
		PLAYER_SKILL_ARTILLERY_STRIKE_COOLDOWN_FRAMES
	);

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
	return true;
}

bool maingame::StartPlayerArtilleryStrikeInstall(human* myHuman, int slot, float tx, float ty, float tz)
{
	if (myHuman == NULL) { return false; }

	if (IsPlayerInstallSkillPending() == true) {
		return false;
	}

	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_ARTILLERY_STRIKE, tx, ty, tz) == false) {
		return false;
	}

	StartPlayerSkillInstallState(
		player_skill_artillery_strike_install_pending,
		player_skill_artillery_strike_install_timer,
		player_skill_artillery_strike_install_slot,
		player_skill_artillery_strike_install_x,
		player_skill_artillery_strike_install_y,
		player_skill_artillery_strike_install_z,
		slot,
		tx,
		ty,
		tz,
		PLAYER_SKILL_ARTILLERY_STRIKE_INSTALL_DELAY_FRAMES
	);

	return true;
}

void maingame::ProcessPlayerArtilleryStrikeInstall(human* myHuman)
{
	if (player_skill_artillery_strike_install_pending == false) {
		return;
	}

	if (IsPlayerSkillInstallOwnerValid(myHuman) == false) {
		ResetPlayerSkillInstallState(
			player_skill_artillery_strike_install_pending,
			player_skill_artillery_strike_install_timer,
			player_skill_artillery_strike_install_slot,
			player_skill_artillery_strike_install_x,
			player_skill_artillery_strike_install_y,
			player_skill_artillery_strike_install_z,
			PLAYER_SKILL_SLOT_SECONDARY
		);
		return;
	}

	if (IsPlayerCurrentWeaponReloading(myHuman) == true) {
		ResetPlayerSkillInstallState(
			player_skill_artillery_strike_install_pending,
			player_skill_artillery_strike_install_timer,
			player_skill_artillery_strike_install_slot,
			player_skill_artillery_strike_install_x,
			player_skill_artillery_strike_install_y,
			player_skill_artillery_strike_install_z,
			PLAYER_SKILL_SLOT_SECONDARY
		);
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	if (player_skill_artillery_strike_install_timer > 0) {
		player_skill_artillery_strike_install_timer--;
		return;
	}

	float tx = player_skill_artillery_strike_install_x;
	float ty = player_skill_artillery_strike_install_y;
	float tz = player_skill_artillery_strike_install_z;
	int install_slot = NormalizePlayerSkillSlot(player_skill_artillery_strike_install_slot);

	ResetPlayerSkillInstallState(
		player_skill_artillery_strike_install_pending,
		player_skill_artillery_strike_install_timer,
		player_skill_artillery_strike_install_slot,
		player_skill_artillery_strike_install_x,
		player_skill_artillery_strike_install_y,
		player_skill_artillery_strike_install_z,
		PLAYER_SKILL_SLOT_SECONDARY
	);

	// 호출 대기 완료 시점에도 다시 한 번 검사한다.
	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_ARTILLERY_STRIKE, tx, ty, tz) == false) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	if (StartPlayerArtilleryStrike(myHuman, install_slot, tx, ty, tz) == false) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}
}

void maingame::ApplyPlayerArtilleryStrikeTick(human* myHuman)
{
	if (myHuman == NULL) { return; }

	float cx = player_skill_artillery_strike_x;
	float cy = player_skill_artillery_strike_y;
	float cz = player_skill_artillery_strike_z;

	int owner_team = player_skill_artillery_strike_owner_team;
	int owner_human_id = player_skill_artillery_strike_owner_human_id;

	if ((owner_human_id < 0) || (owner_human_id >= MAX_HUMAN)) {
		owner_human_id = ObjMgr.GetPlayerID();
	}

	int explosion_count = PLAYER_SKILL_ARTILLERY_STRIKE_EFFECT_MIN_COUNT;

	int explosion_range =
		PLAYER_SKILL_ARTILLERY_STRIKE_EFFECT_MAX_COUNT -
		PLAYER_SKILL_ARTILLERY_STRIKE_EFFECT_MIN_COUNT + 1;

	if (explosion_range > 0) {
		explosion_count += GetRand(explosion_range);
	}

	int down_spread_count = 0;

	for (int i = 0; i < explosion_count; i++) {
		float angle = ((float)GetRand(360) / 360.0f) * (float)M_PI * 2.0f;

		// sqrtf를 사용해 중심에만 몰리지 않고 원 안에 자연스럽게 퍼지게 한다.
		float radius_rate = sqrtf((float)GetRand(1000) / 1000.0f);

		float explosion_radius =
			PLAYER_SKILL_ARTILLERY_STRIKE_RADIUS *
			PLAYER_SKILL_ARTILLERY_STRIKE_EXPLOSION_SPAWN_RADIUS_RATE *
			radius_rate;

		float ex = cx + cosf(angle) * explosion_radius;
		float ez = cz + sinf(angle) * explosion_radius;

		// 탄착 지점이 벽 내부로 들어가면 중심점 쪽으로 약간 당겨서 폭발시킨다.
		if (CollD.CheckALLBlockInside(ex, cy + PLAYER_SKILL_ARTILLERY_STRIKE_EXPLOSION_Y_OFFSET, ez) == true) {
			ex = cx + cosf(angle) * explosion_radius * 0.45f;
			ez = cz + sinf(angle) * explosion_radius * 0.45f;
		}

		SpawnPlayerArtilleryStrikeExplosion(ex, cy, ez, owner_team, owner_human_id);

		if ((down_spread_count < PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_MAX_PER_TICK) &&
			(GetRand(100) < PLAYER_SKILL_ARTILLERY_DOWN_SPREAD_CHANCE_PERCENT)) {

			float down_x = 0.0f;
			float down_y = 0.0f;
			float down_z = 0.0f;

			if (FindPlayerArtilleryStrikeDownSpreadPoint(ex, cy, ez, &down_x, &down_y, &down_z) == true) {
				SpawnPlayerArtilleryStrikeDownSpreadExplosion(
					down_x,
					down_y,
					down_z,
					owner_team,
					owner_human_id
				);

				down_spread_count++;
			}
		}
	}
}

void maingame::ProcessPlayerArtilleryStrike(human* myHuman)
{
	if (player_skill_artillery_strike_timer <= 0) { return; }

	if ((myHuman == NULL) ||
		(myHuman->GetEnableFlag() == false) ||
		(myHuman->GetDeadFlag() == true) ||
		(myHuman->GetHP() <= 0)) {

		player_skill_artillery_strike_timer = 0;
		player_skill_artillery_strike_tick_timer = 0;
		return;
	}

	player_skill_artillery_strike_timer--;

	if (player_skill_artillery_strike_tick_timer > 0) {
		player_skill_artillery_strike_tick_timer--;
	}

	if (player_skill_artillery_strike_tick_timer <= 0) {
		player_skill_artillery_strike_tick_timer = PLAYER_SKILL_ARTILLERY_STRIKE_TICK_INTERVAL_FRAMES;
		ApplyPlayerArtilleryStrikeTick(myHuman);
	}

	if (player_skill_artillery_strike_timer <= 0) {
		player_skill_artillery_strike_timer = 0;
		player_skill_artillery_strike_tick_timer = 0;
		player_skill_artillery_strike_owner_team = -1;
		player_skill_artillery_strike_owner_human_id = -1;
	}
}

// 스킬로 임시 무기를 들고 있는 상태인지 확인한다.
// Impact grenade와 방패를 같은 임시 스킬 무기 통로로 처리한다.
bool maingame::IsPlayerShieldWeaponEquipped(human* myHuman)
{
	if (myHuman == NULL) { return false; }
	if (myHuman->GetHP() <= 0) { return false; }

	// 임시 스킬 무기를 들고 있는 동안에는 현재 화면에 보이는 임시 무기를 우선한다.
	if (IsPlayerTemporarySkillWeaponActive() == true) {
		return (GetPlayerTemporarySkillWeaponID() == ID_WEAPON_SHIELD);
	}

	// F7 등으로 실제 무기 슬롯에 92번 방패가 들어간 경우.
	if (myHuman->GetMainWeaponTypeNO() == ID_WEAPON_SHIELD) {
		return true;
	}

	return false;
}

bool maingame::IsPlayerTemporarySkillWeaponActive()
{
	if ((player_skill_impact_grenade_ready == true) ||
		(player_skill_impact_grenade_equip_pending == true) ||
		(player_skill_impact_grenade_unequip_pending == true) ||
		(player_skill_impact_grenade_change_timer > 0)) {
		return true;
	}

	if ((player_skill_shield_ready == true) ||
		(player_skill_shield_equip_pending == true) ||
		(player_skill_shield_unequip_pending == true) ||
		(player_skill_shield_change_timer > 0)) {
		return true;
	}

	return false;
}

bool maingame::IsPlayerTemporarySkillWeaponReady()
{
	if (player_skill_impact_grenade_ready == true) {
		return true;
	}

	if (player_skill_shield_ready == true) {
		return true;
	}

	return false;
}

bool maingame::IsPlayerTemporarySkillWeaponVisible()
{
	if ((player_skill_impact_grenade_ready == true) ||
		(player_skill_impact_grenade_equip_pending == true)) {
		return true;
	}

	// 방패는 팔 모션과 렌더링 무기가 같이 바뀌어야 한다.
	// 표시 지연을 넣으면 실제 총 + 방패 팔 모션이 섞여 보일 수 있으므로 바로 표시한다.
	if ((player_skill_shield_ready == true) ||
		(player_skill_shield_equip_pending == true)) {
		return true;
	}

	return false;
}

bool maingame::IsPlayerTemporarySkillWeaponChanging()
{
	if ((player_skill_impact_grenade_equip_pending == true) ||
		(player_skill_impact_grenade_unequip_pending == true) ||
		(player_skill_impact_grenade_change_timer > 0)) {
		return true;
	}

	if ((player_skill_shield_equip_pending == true) ||
		(player_skill_shield_unequip_pending == true) ||
		(player_skill_shield_change_timer > 0)) {
		return true;
	}

	return false;
}

int maingame::GetPlayerTemporarySkillWeaponID()
{
	if ((player_skill_impact_grenade_ready == true) ||
		(player_skill_impact_grenade_equip_pending == true) ||
		(player_skill_impact_grenade_unequip_pending == true) ||
		(player_skill_impact_grenade_change_timer > 0)) {
		return ID_WEAPON_IMPACT_GRENADE;
	}

	if ((player_skill_shield_ready == true) ||
		(player_skill_shield_equip_pending == true) ||
		(player_skill_shield_unequip_pending == true) ||
		(player_skill_shield_change_timer > 0)) {
		return ID_WEAPON_SHIELD;
	}

	return ID_WEAPON_NONE;
}

int maingame::GetPlayerTemporarySkillWeaponAmmo()
{
	if (GetPlayerTemporarySkillWeaponID() == ID_WEAPON_IMPACT_GRENADE) {
		if (player_skill_impact_grenade_ammo < 0) {
			return 0;
		}

		return player_skill_impact_grenade_ammo;
	}

	return 0;
}

int maingame::GetPlayerTemporarySkillWeaponStockAmmo(int weapon_id)
{
	if (weapon_id == ID_WEAPON_IMPACT_GRENADE) {
		if (player_skill_impact_grenade_ammo < 0) {
			return 0;
		}

		return player_skill_impact_grenade_ammo;
	}

	if (weapon_id == ID_WEAPON_SHIELD) {
		return 1;
	}

	return 0;
}

int maingame::GetPlayerTemporarySkillWeaponReturnSlot()
{
	int return_slot = -1;

	if (GetPlayerTemporarySkillWeaponID() == ID_WEAPON_IMPACT_GRENADE) {
		return_slot = player_skill_impact_grenade_return_weapon_slot;
	}
	else if (GetPlayerTemporarySkillWeaponID() == ID_WEAPON_SHIELD) {
		return_slot = player_skill_shield_return_weapon_slot;
	}

	if ((0 <= return_slot) && (return_slot < TOTAL_HAVEWEAPON)) {
		return return_slot;
	}

	return -1;
}

int maingame::GetPlayerTemporarySkillWeaponChangeTimer()
{
	if (GetPlayerTemporarySkillWeaponID() == ID_WEAPON_IMPACT_GRENADE) {
		if (player_skill_impact_grenade_change_timer < 0) { return 0; }
		return player_skill_impact_grenade_change_timer;
	}

	if (GetPlayerTemporarySkillWeaponID() == ID_WEAPON_SHIELD) {
		if (player_skill_shield_change_timer < 0) { return 0; }
		return player_skill_shield_change_timer;
	}

	return 0;
}

bool maingame::IsPlayerTemporarySkillWeaponFireLocked()
{
	if (GetPlayerTemporarySkillWeaponID() == ID_WEAPON_IMPACT_GRENADE) {
		return player_skill_impact_grenade_fire_lock;
	}

	return false;
}

void maingame::SetPlayerTemporarySkillWeaponFireLock(bool flag)
{
	if (GetPlayerTemporarySkillWeaponID() == ID_WEAPON_IMPACT_GRENADE) {
		player_skill_impact_grenade_fire_lock = flag;
	}
}

bool maingame::IsPlayerTemporarySkillWeaponShotDelayed()
{
	if (GetPlayerTemporarySkillWeaponID() == ID_WEAPON_IMPACT_GRENADE) {
		return (player_skill_impact_grenade_shot_delay_timer > 0);
	}

	return false;
}

void maingame::StartPlayerTemporarySkillWeaponShotDelay()
{
	if (GetPlayerTemporarySkillWeaponID() != ID_WEAPON_IMPACT_GRENADE) {
		return;
	}

	int delay = 1;
	int weapon_id = GetPlayerTemporarySkillWeaponID();

	WeaponParameter ParamData;
	if (GameParamInfo.GetWeapon(weapon_id, &ParamData) == 0) {
		delay = static_cast<int>(ParamData.blazings);
	}

	if (delay < 1) {
		delay = 1;
	}

	player_skill_impact_grenade_shot_delay_timer = delay;
}

int maingame::ConsumePlayerTemporarySkillWeaponAmmo(int amount)
{
	if (GetPlayerTemporarySkillWeaponID() != ID_WEAPON_IMPACT_GRENADE) {
		return 0;
	}

	if (amount < 1) {
		amount = 1;
	}

	player_skill_impact_grenade_ammo -= amount;

	if (player_skill_impact_grenade_ammo < 0) {
		player_skill_impact_grenade_ammo = 0;
	}

	return player_skill_impact_grenade_ammo;
}

void maingame::KeepPlayerTemporarySkillWeaponReadyAfterFire()
{
	if (GetPlayerTemporarySkillWeaponID() != ID_WEAPON_IMPACT_GRENADE) {
		return;
	}

	player_skill_impact_grenade_ready = true;
	player_skill_impact_grenade_equip_pending = false;
	player_skill_impact_grenade_unequip_pending = false;
	player_skill_impact_grenade_change_timer = 0;
}

bool maingame::StartPlayerTemporarySkillWeaponEquip(human* myHuman, int weapon_id, int return_slot, int frames)
{
	if (myHuman == NULL) { return false; }

	if ((weapon_id != ID_WEAPON_IMPACT_GRENADE) &&
		(weapon_id != ID_WEAPON_SHIELD)) {
		return false;
	}

	if ((weapon_id == ID_WEAPON_IMPACT_GRENADE) &&
		(GetPlayerTemporarySkillWeaponStockAmmo(weapon_id) <= 0)) {
		return false;
	}

	if (frames < 1) {
		frames = 1;
	}

	if ((return_slot < 0) || (TOTAL_HAVEWEAPON <= return_slot)) {
		return_slot = 0;
	}

	// 같은 통로에는 임시 무기를 하나만 올린다.
	player_skill_impact_grenade_ready = false;
	player_skill_impact_grenade_equip_pending = false;
	player_skill_impact_grenade_unequip_pending = false;
	player_skill_impact_grenade_fire_lock = false;
	player_skill_impact_grenade_change_timer = 0;
	player_skill_impact_grenade_shot_delay_timer = 0;
	player_skill_impact_grenade_return_weapon_slot = -1;

	player_skill_shield_ready = false;
	player_skill_shield_equip_pending = false;
	player_skill_shield_unequip_pending = false;
	player_skill_shield_change_timer = 0;
	player_skill_shield_return_weapon_slot = -1;

	if (weapon_id == ID_WEAPON_IMPACT_GRENADE) {
		player_skill_impact_grenade_return_weapon_slot = return_slot;
		player_skill_impact_grenade_equip_pending = true;
		player_skill_impact_grenade_change_timer = frames;

		// Impact grenade만 기존 무기 변경처럼 손을 내리는 모션을 발생시킨다.
		myHuman->StartRenderWeaponChangeMotion(weapon_id, frames);
	}
	else if (weapon_id == ID_WEAPON_SHIELD) {
		// 방패도 Impact grenade처럼 기존 무기 전환 모션을 사용한다.
		// 장착 모션 중에는 render_override_weapon_id를 통해 방패 팔/무기를 표시한다.
		player_skill_shield_return_weapon_slot = return_slot;
		player_skill_shield_equip_pending = true;
		player_skill_shield_unequip_pending = false;
		player_skill_shield_ready = false;
		player_skill_shield_change_timer = frames;

		myHuman->StartRenderWeaponChangeMotion(weapon_id, frames);
	}

	ClearGrenadeTrajectoryToggle();

	return true;
}

void maingame::CancelPlayerTemporarySkillWeapon(human* myHuman)
{
	if (IsPlayerTemporarySkillWeaponActive() == false) {
		return;
	}

	player_skill_impact_grenade_ready = false;
	player_skill_impact_grenade_equip_pending = false;
	player_skill_impact_grenade_unequip_pending = false;
	player_skill_impact_grenade_fire_lock = false;
	player_skill_impact_grenade_change_timer = 0;
	player_skill_impact_grenade_shot_delay_timer = 0;
	player_skill_impact_grenade_return_weapon_slot = -1;

	player_skill_shield_ready = false;
	player_skill_shield_equip_pending = false;
	player_skill_shield_unequip_pending = false;
	player_skill_shield_change_timer = 0;
	player_skill_shield_return_weapon_slot = -1;

	ClearGrenadeTrajectoryToggle();

	if (myHuman != NULL) {
		myHuman->SetRenderOverrideWeaponID(-1);
	}
}

void maingame::ProcessPlayerTemporarySkillWeaponState()
{
	if (player_skill_impact_grenade_shot_delay_timer > 0) {
		player_skill_impact_grenade_shot_delay_timer--;
	}

	if (player_skill_impact_grenade_change_timer > 0) {
		player_skill_impact_grenade_change_timer--;

		if (player_skill_impact_grenade_change_timer <= 0) {
			player_skill_impact_grenade_change_timer = 0;

			if (player_skill_impact_grenade_equip_pending == true) {
				player_skill_impact_grenade_equip_pending = false;
				player_skill_impact_grenade_ready = true;
				player_skill_impact_grenade_fire_lock = false;
			}

			if (player_skill_impact_grenade_unequip_pending == true) {
				player_skill_impact_grenade_unequip_pending = false;
				player_skill_impact_grenade_ready = false;
				player_skill_impact_grenade_fire_lock = false;
				player_skill_impact_grenade_return_weapon_slot = -1;
				ClearGrenadeTrajectoryToggle();
			}
		}
	}

	if (player_skill_shield_change_timer > 0) {
		player_skill_shield_change_timer--;

		if (player_skill_shield_change_timer <= 0) {
			player_skill_shield_change_timer = 0;

			if (player_skill_shield_equip_pending == true) {
				player_skill_shield_equip_pending = false;
				player_skill_shield_ready = true;
			}

			if (player_skill_shield_unequip_pending == true) {
				player_skill_shield_unequip_pending = false;
				player_skill_shield_ready = false;
				player_skill_shield_return_weapon_slot = -1;
				ClearGrenadeTrajectoryToggle();
			}
		}
	}
}

// 임시 스킬 무기 상태에서 기존 무기로 돌아간다.
// return_slot == -1 이면 기존 무기 전환키처럼 다음 슬롯으로 복귀한다.
void maingame::StartPlayerTemporarySkillWeaponReturn(human* myHuman, int return_slot)
{
	if (myHuman == NULL) { return; }

	int temp_weapon_id = GetPlayerTemporarySkillWeaponID();
	if (temp_weapon_id == ID_WEAPON_NONE) { return; }

	int selectweapon = 0;
	class weapon* slot_weapon[TOTAL_HAVEWEAPON];
	int lnbslist[TOTAL_HAVEWEAPON];
	int nbslist[TOTAL_HAVEWEAPON];

	for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
		slot_weapon[i] = NULL;
		lnbslist[i] = 0;
		nbslist[i] = 0;
	}

	myHuman->GetWeapon(&selectweapon, slot_weapon, lnbslist, nbslist);

	if (return_slot == -1) {
		return_slot = selectweapon + 1;

		if (return_slot == TOTAL_HAVEWEAPON) {
			return_slot = 0;
		}
	}

	if ((return_slot < 0) || (TOTAL_HAVEWEAPON <= return_slot)) {
		return_slot = selectweapon;
	}

	int return_frames = PLAYER_TEMP_SKILL_WEAPON_CHANGE_FRAMES;

	if (temp_weapon_id == ID_WEAPON_SHIELD) {
		player_skill_shield_return_weapon_slot = return_slot;
		player_skill_shield_ready = false;
		player_skill_shield_equip_pending = false;
		player_skill_shield_unequip_pending = true;
		player_skill_shield_change_timer = return_frames;
	}
	else if (temp_weapon_id == ID_WEAPON_IMPACT_GRENADE) {
		player_skill_impact_grenade_return_weapon_slot = return_slot;
		player_skill_impact_grenade_ready = false;
		player_skill_impact_grenade_equip_pending = false;
		player_skill_impact_grenade_unequip_pending = true;
		player_skill_impact_grenade_fire_lock = true;
		player_skill_impact_grenade_change_timer = return_frames;
		player_skill_impact_grenade_shot_delay_timer = 0;
	}

	ClearGrenadeTrajectoryToggle();

	// 핵심: 같은 슬롯이어도 selectweaponcnt와 손 올림 모션을 강제로 발생시킨다.
	myHuman->ForceSelectWeaponSlotForSkillReturn(return_slot, return_frames);
}

// 기존 호출부 호환용 래퍼.
// 기존 gamemain.cpp의 StartPlayerImpactGrenadeReturn() 호출을 모두 바꾸지 않아도 된다.
void maingame::StartPlayerImpactGrenadeReturn(human* myHuman, int return_slot)
{
	StartPlayerTemporarySkillWeaponReturn(myHuman, return_slot);
}

bool maingame::IsPlayerDP28SkillWeaponActive()
{
	return player_skill_dp28_ready;
}

int maingame::GetPlayerDP28SkillWeaponReturnSlot()
{
	if ((0 <= player_skill_dp28_return_weapon_slot) &&
		(player_skill_dp28_return_weapon_slot < TOTAL_HAVEWEAPON)) {
		return player_skill_dp28_return_weapon_slot;
	}

	return -1;
}

int maingame::GetPlayerDP28SkillStoredWeaponID()
{
	return player_skill_dp28_stored_weapon_id;
}

// 용병 E: DP28 스킬 무기 시작.
// 기존 무기를 완전히 잃지 않도록 현재 슬롯의 무기 ID/탄약만 저장하고,
// DP28은 실제 무기 슬롯에 임시 삽입한다.
bool maingame::StartPlayerDP28SkillWeapon(human* myHuman)
{
	if (myHuman == NULL) { return false; }

	if (player_skill_dp28_ready == true) {
		ReturnPlayerDP28SkillWeapon(myHuman);
		return true;
	}

	int selectweapon = 0;
	class weapon* slot_weapon[TOTAL_HAVEWEAPON];
	int lnbslist[TOTAL_HAVEWEAPON];
	int nbslist[TOTAL_HAVEWEAPON];

	for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
		slot_weapon[i] = NULL;
		lnbslist[i] = 0;
		nbslist[i] = 0;
	}

	myHuman->GetWeapon(&selectweapon, slot_weapon, lnbslist, nbslist);

	if ((selectweapon < 0) || (TOTAL_HAVEWEAPON <= selectweapon)) {
		selectweapon = 0;
	}

	player_skill_dp28_return_weapon_slot = selectweapon;
	player_skill_dp28_stored_weapon_id = ID_WEAPON_NONE;
	player_skill_dp28_stored_lnbs = lnbslist[selectweapon];
	player_skill_dp28_stored_nbs = nbslist[selectweapon];

	if (slot_weapon[selectweapon] != NULL) {
		slot_weapon[selectweapon]->GetParamData(
			&player_skill_dp28_stored_weapon_id,
			&player_skill_dp28_stored_lnbs,
			&player_skill_dp28_stored_nbs
		);

		// 기존 무기 오브젝트는 슬롯에서 빼기 전 비활성화한다.
		// 복귀 시에는 저장한 ID/탄약으로 새 오브젝트를 만들어 복구한다.
		slot_weapon[selectweapon]->SetEnableFlag(false);
	}

	int weapon_index = ObjMgr.AddVisualWeaponIndex(ID_WEAPON_DP28, true);
	if (weapon_index < 0) {
		player_skill_dp28_return_weapon_slot = -1;
		player_skill_dp28_stored_weapon_id = ID_WEAPON_NONE;
		player_skill_dp28_stored_lnbs = 0;
		player_skill_dp28_stored_nbs = 0;
		return false;
	}

	class weapon* dp28_weapon = ObjMgr.GetWeaponObject(weapon_index);
	if (dp28_weapon == NULL) {
		player_skill_dp28_return_weapon_slot = -1;
		player_skill_dp28_stored_weapon_id = ID_WEAPON_NONE;
		player_skill_dp28_stored_lnbs = 0;
		player_skill_dp28_stored_nbs = 0;
		return false;
	}

	// 이전에 DP28을 사용한 적이 있다면, 껐다 켜도 장탄/예비탄을 유지한다.
	// 아직 저장된 값이 없으면 AddVisualWeaponIndex()가 넣은 기본 탄약을 그대로 사용한다.
	if ((player_skill_dp28_saved_lnbs >= 0) &&
		(player_skill_dp28_saved_nbs >= 0)) {
		dp28_weapon->SetParamData(
			ID_WEAPON_DP28,
			player_skill_dp28_saved_lnbs,
			player_skill_dp28_saved_nbs,
			false
		);
	}

	slot_weapon[selectweapon] = dp28_weapon;
	lnbslist[selectweapon] = 0;
	nbslist[selectweapon] = 0;

	myHuman->SetWeapon(slot_weapon, lnbslist, nbslist);
	myHuman->ForceSelectWeaponSlotForSkillReturn(selectweapon, 5);

	player_skill_dp28_ready = true;

	return true;
}

// 용병 E: DP28 스킬 무기 종료.
// DP28을 제거하고 저장해둔 기존 무기 ID/탄약으로 원래 슬롯을 복구한다.
void maingame::ReturnPlayerDP28SkillWeapon(human* myHuman)
{
	if (myHuman == NULL) { return; }
	if (player_skill_dp28_ready == false) { return; }

	int return_slot = GetPlayerDP28SkillWeaponReturnSlot();
	if (return_slot < 0) {
		return_slot = 0;
	}

	int selectweapon = 0;
	class weapon* slot_weapon[TOTAL_HAVEWEAPON];
	int lnbslist[TOTAL_HAVEWEAPON];
	int nbslist[TOTAL_HAVEWEAPON];

	for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
		slot_weapon[i] = NULL;
		lnbslist[i] = 0;
		nbslist[i] = 0;
	}

	myHuman->GetWeapon(&selectweapon, slot_weapon, lnbslist, nbslist);

	if (slot_weapon[return_slot] != NULL) {
		int weapon_id = ID_WEAPON_NONE;
		int dp28_lnbs = 0;
		int dp28_nbs = 0;

		slot_weapon[return_slot]->GetParamData(&weapon_id, &dp28_lnbs, &dp28_nbs);

		if (weapon_id == ID_WEAPON_DP28) {
			// DP28을 해제하기 직전 현재 장탄/예비탄을 저장한다.
			// 다음 E 재사용 시 새 DP28 오브젝트에 이 값을 복구한다.
			player_skill_dp28_saved_lnbs = dp28_lnbs;
			player_skill_dp28_saved_nbs = dp28_nbs;

			slot_weapon[return_slot]->SetEnableFlag(false);
			slot_weapon[return_slot] = NULL;
		}
	}

	if (player_skill_dp28_stored_weapon_id != ID_WEAPON_NONE) {
		int restore_weapon_index = ObjMgr.AddVisualWeaponIndex(player_skill_dp28_stored_weapon_id, true);
		if (restore_weapon_index >= 0) {
			class weapon* restore_weapon = ObjMgr.GetWeaponObject(restore_weapon_index);

			if (restore_weapon != NULL) {
				slot_weapon[return_slot] = restore_weapon;
				lnbslist[return_slot] = player_skill_dp28_stored_lnbs;
				nbslist[return_slot] = player_skill_dp28_stored_nbs;
			}
		}
	}
	else {
		slot_weapon[return_slot] = NULL;
		lnbslist[return_slot] = player_skill_dp28_stored_lnbs;
		nbslist[return_slot] = player_skill_dp28_stored_nbs;
	}

	myHuman->SetWeapon(slot_weapon, lnbslist, nbslist);
	myHuman->ForceSelectWeaponSlotForSkillReturn(return_slot, 5);

	player_skill_dp28_ready = false;
	player_skill_dp28_return_weapon_slot = -1;
	player_skill_dp28_stored_weapon_id = ID_WEAPON_NONE;
	player_skill_dp28_stored_lnbs = 0;
	player_skill_dp28_stored_nbs = 0;
}

// 스킬 쿨타임/지속시간 처리
void maingame::ProcessPlayerSkill(human* myHuman)
{
	if (myHuman == NULL) { return; }

	if ((myHuman->GetHP() <= 0) || (myHuman->GetDeadFlag() == true)) {
		ClearPlayerTerroristMines();
	}

	ProcessPlayerSkillCooldown();

	ProcessPlayerTemporarySkillWeaponState();

	ProcessTacticalScanChargeTimer();

	// 소환 좀비 정리
	CleanupPlayerSummonedZombies();

	// 소환 동료는 전투/경계 상태가 아닐 때 플레이어를 따라오게 한다.
	UpdatePlayerSummonedAllyFollow();

	ProcessPlayerSummonInstall(myHuman);
	ProcessPlayerShadowDecoyInstall(myHuman);
	ProcessPlayerShadowDecoy(myHuman);
	ProcessPlayerShadowDecoyPings();

	ProcessPlayerBioticField(myHuman);

	ProcessPlayerArtilleryStrike(myHuman);

	// 로봇 처형 프로토콜 순차 처형 큐 처리
	ProcessRobotExecuteFireQueue();

	// 좀비 갈고리 처리
	ProcessPlayerHookSkill(myHuman);

	// 비밀 요원 텔레포트 시전/이동 처리
	ProcessPlayerTeleportSkill(myHuman);

	ProcessRobotExecuteSkillState(myHuman);

	ProcessAimHackSkill(myHuman);

	ProcessSuppressMarkTimers();

	ProcessDistractionSignalTimer();

	ProcessPlayerDistractionInstall(myHuman);

	ProcessPlayerBioticFieldInstall(myHuman);

	ProcessPlayerArtilleryStrikeInstall(myHuman);

	ProcessPlayerTerroristMineInstall(myHuman);

	ProcessPlayerTerroristMines();

	ProcessTacticalScanActiveTimer();

	ProcessMarkedShotTimer();

	ProcessPlayerHealEffectTimer();

	ProcessPlayerSummonEffectTimer();

	ProcessPlayerSurvivalEffectTimer();

	ProcessPlayerDodgeTrailTimer();

	ProcessPlayerTeleportEffectTimer();

	ProcessPlayerSkillEndSound(myHuman);
}

// 플레이어 스킬 쿨타임 처리
void maingame::ProcessPlayerSkillCooldown()
{
	for (int slot = 0; slot < PLAYER_SKILL_SLOT_COUNT; slot++) {
		if (player_skill_cooldown_timer[slot] > 0) {
			player_skill_cooldown_timer[slot]--;

			if (player_skill_cooldown_timer[slot] <= 0) {
				player_skill_cooldown_timer[slot] = 0;
				player_skill_cooldown_total_timer[slot] = 0;

				if (player_skill_ready_sound_played[slot] == false) {
					player_skill_ready_sound_played[slot] = true;
					PlayPlayerSkillSound(SKILL_SOUND_COMMON_READY);
				}
			}
		}
	}
}

// 대장 전술 스캔 준비 시간 처리
void maingame::ProcessTacticalScanChargeTimer()
{
	if (player_skill_scan_charge_timer > 0) {
		player_skill_scan_charge_timer--;
		player_skill_scan_charge_sound_timer--;

		// 먼저 0초 도달 여부를 처리한다.
		// 이렇게 해야 ready와 start가 같은 프레임에 겹쳐서 나지 않는다.
		if (player_skill_scan_charge_timer <= 0) {
			player_skill_scan_charge_timer = 0;
			player_skill_scan_charge_sound_timer = 0;
			player_skill_scan_timer = PLAYER_SKILL_SCAN_FRAMES;

			// 0초가 되어 실제 스캔이 발동되는 순간 start 1회
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
		}
		else if (player_skill_scan_charge_sound_timer <= 0) {
			player_skill_scan_charge_sound_timer = GAMEFPS;

			// 준비 중 1초마다 ready 효과음
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_READY);
		}
	}
}

// 경찰 제압 표시 타이머 처리
void maingame::ProcessSuppressMarkTimers()
{
	for (int i = 0; i < MAX_HUMAN; i++) {
		if (player_skill_suppress_mark_timer[i] > 0) {
			player_skill_suppress_mark_timer[i]--;
		}
	}
}

// 테러리스트 교란 신호 처리
void maingame::ProcessDistractionSignalTimer()
{
	if (player_skill_distraction_timer > 0) {
		player_skill_distraction_timer--;

		ApplyDistractionSignal();

		// 신호기 작동음: 총 3회까지만 재생한다.
		if (player_skill_distraction_sound_timer > 0) {
			player_skill_distraction_sound_timer--;
		}
		else if (player_skill_distraction_sound_count < PLAYER_SKILL_DISTRACTION_BEEP_MAX_COUNT) {
			PlayPlayerSkill3DSound(
				SKILL_SOUND_DISTRACTION_BEEP,
				player_skill_distraction_x,
				player_skill_distraction_y,
				player_skill_distraction_z
			);

			player_skill_distraction_sound_count++;
			player_skill_distraction_sound_timer = PLAYER_SKILL_DISTRACTION_BEEP_INTERVAL_FRAMES;
		}

		if (player_skill_distraction_timer <= 0) {
			player_skill_distraction_timer = 0;
			player_skill_distraction_sound_timer = 0;
			player_skill_distraction_sound_count = 0;
		}
	}
}

// 대장 전술 스캔 표시 시간 처리
void maingame::ProcessTacticalScanActiveTimer()
{
	if (player_skill_scan_timer > 0) {
		player_skill_scan_timer--;
		if (player_skill_scan_timer < 0) {
			player_skill_scan_timer = 0;
		}
	}
}


void maingame::ProcessPlayerHealEffectTimer()
{
	if (player_skill_heal_effect_timer > 0) {
		player_skill_heal_effect_timer--;

		if (player_skill_heal_effect_timer < 0) {
			player_skill_heal_effect_timer = 0;
		}
	}
}

void maingame::ProcessPlayerSummonEffectTimer()
{
	if (player_skill_summon_effect_timer > 0) {
		player_skill_summon_effect_timer--;

		if (player_skill_summon_effect_timer <= 0) {
			player_skill_summon_effect_timer = 0;
			player_skill_summon_effect_count = 0;
		}
	}
}

void maingame::ProcessPlayerSurvivalEffectTimer()
{
	if (player_skill_survival_effect_timer > 0) {
		player_skill_survival_effect_timer--;

		if (player_skill_survival_effect_timer < 0) {
			player_skill_survival_effect_timer = 0;
		}
	}
}

void maingame::ProcessPlayerDodgeTrailTimer()
{
	if (player_skill_dodge_trail_timer > 0) {
		player_skill_dodge_trail_timer--;

		if (player_skill_dodge_trail_timer < 0) {
			player_skill_dodge_trail_timer = 0;
		}
	}
}

void maingame::ProcessPlayerTeleportEffectTimer()
{
	if (player_skill_teleport_start_effect_timer > 0) {
		player_skill_teleport_start_effect_timer--;

		if (player_skill_teleport_start_effect_timer < 0) {
			player_skill_teleport_start_effect_timer = 0;
		}
	}

	if (player_skill_teleport_end_effect_timer > 0) {
		player_skill_teleport_end_effect_timer--;

		if (player_skill_teleport_end_effect_timer < 0) {
			player_skill_teleport_end_effect_timer = 0;
		}
	}
}

// 요원A 마킹 샷 타이머 / 쿨타임 환급 처리
void maingame::ProcessMarkedShotTimer()
{
	if (player_skill_mark_prep_timer > 0) {
		player_skill_mark_prep_timer--;

		if (player_skill_mark_prep_timer > 0) {
			return;
		}

		player_skill_mark_prep_timer = 0;

		human* myHuman = ObjMgr.GetPlayerHumanObject();

		if ((myHuman == NULL) || (myHuman->GetHP() <= 0)) {
			player_skill_mark_target_id = -1;
			player_skill_mark_prep_slot = PLAYER_SKILL_SLOT_PRIMARY;
			return;
		}

		int target_id = FindNearestEnemyForMarkShot(myHuman);

		if (target_id < 0) {
			player_skill_mark_target_id = -1;
			player_skill_mark_prep_slot = PLAYER_SKILL_SLOT_PRIMARY;

			// 준비가 끝났지만 마킹 가능한 적이 없으면 실패 처리한다.
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return;
		}

		player_skill_mark_target_id = target_id;
		player_skill_mark_timer = PLAYER_SKILL_MARKSHOT_FRAMES;

		StartPlayerSkillCooldownForSlot(
			player_skill_mark_prep_slot,
			PLAYER_SKILL_MARKSHOT,
			PLAYER_SKILL_MARKSHOT_COOLDOWN_FRAMES
		);

		player_skill_mark_prep_slot = PLAYER_SKILL_SLOT_PRIMARY;

		PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
	}

	if (player_skill_mark_timer > 0) {
		bool clear_mark = false;
		bool target_dead = false;

		human* markedHuman = NULL;

		if ((0 <= player_skill_mark_target_id) && (player_skill_mark_target_id < MAX_HUMAN)) {
			markedHuman = ObjMgr.GetHumanObject(player_skill_mark_target_id);
		}

		if (markedHuman == NULL) {
			clear_mark = true;
		}
		else if (markedHuman->GetEnableFlag() == false) {
			clear_mark = true;
		}
		else if ((markedHuman->GetDeadFlag() == true) || (markedHuman->GetHP() <= 0)) {
			clear_mark = true;
			target_dead = true;
		}

		if (target_dead == true) {
			// 마킹된 적 사망 시 해당 슬롯의 쿨타임 15초 환급
			int refund_slot = -1;

			for (int slot = 0; slot < PLAYER_SKILL_SLOT_COUNT; slot++) {
				if ((player_skill_current_type[slot] == PLAYER_SKILL_MARKSHOT) &&
					(player_skill_cooldown_timer[slot] > 0)) {
					refund_slot = slot;
					break;
				}
			}

			if (refund_slot >= 0) {
				if (player_skill_cooldown_timer[refund_slot] <= PLAYER_SKILL_MARKSHOT_REFUND_FRAMES) {
					player_skill_cooldown_timer[refund_slot] = 0;
					player_skill_cooldown_total_timer[refund_slot] = 0;

					if (player_skill_ready_sound_played[refund_slot] == false) {
						player_skill_ready_sound_played[refund_slot] = true;
						PlayPlayerSkillSound(SKILL_SOUND_COMMON_READY);
					}
				}
				else {
					player_skill_cooldown_timer[refund_slot] -= PLAYER_SKILL_MARKSHOT_REFUND_FRAMES;
				}
			}
		}

		if (clear_mark == false) {
			player_skill_mark_timer--;

			if (player_skill_mark_timer <= 0) {
				clear_mark = true;
			}
		}

		if (clear_mark == true) {
			player_skill_mark_prep_timer = 0;
			player_skill_mark_prep_slot = PLAYER_SKILL_SLOT_PRIMARY;
			player_skill_mark_timer = 0;
			player_skill_mark_target_id = -1;
		}
	}
}

// 스킬 종료음 처리
void maingame::ProcessPlayerSkillEndSound(human* myHuman)
{
	int current_active_timer = 0;

	if (myHuman != NULL) {
		if (myHuman->GetSkillStealthTimer() > 0) {
			current_active_timer = myHuman->GetSkillStealthTimer();
		}
		else if (myHuman->GetSkillAmmoBoostTimer() > 0) {
			current_active_timer = myHuman->GetSkillAmmoBoostTimer();
		}
		else if (myHuman->GetSkillSurvivalTimer() > 0) {
			current_active_timer = myHuman->GetSkillSurvivalTimer();
		}
		else if (myHuman->GetSkillIronBodyTimer() > 0) {
			current_active_timer = myHuman->GetSkillIronBodyTimer();
		}
		else if (myHuman->GetSkillAbsorbTimer() > 0) {
			current_active_timer = myHuman->GetSkillAbsorbTimer();
		}
		else if (player_skill_mark_prep_timer > 0) {
			current_active_timer = player_skill_mark_prep_timer;
		}
		else if (player_skill_mark_timer > 0) {
			current_active_timer = player_skill_mark_timer;
		}
		else if (myHuman->GetSkillRobotExecuteTimer() > 0) {
			current_active_timer = myHuman->GetSkillRobotExecuteTimer();
		}
		else if (player_skill_distraction_timer > 0) {
			current_active_timer = player_skill_distraction_timer;
		}
		else if (player_skill_hook_prep_timer > 0) {
			current_active_timer = player_skill_hook_prep_timer;
		}
		else if (player_skill_hook_pull_timer > 0) {
			current_active_timer = player_skill_hook_pull_timer;
		}
		else if (player_skill_scan_charge_timer > 0) {
			current_active_timer = player_skill_scan_charge_timer;
		}
		else if (player_skill_scan_timer > 0) {
			current_active_timer = player_skill_scan_timer;
		}
		else if (player_skill_artillery_strike_timer > 0) {
			current_active_timer = player_skill_artillery_strike_timer;
		}
	}

	if ((player_skill_prev_active_timer > 0) &&
		(current_active_timer <= 0) &&
		(myHuman != NULL) &&
		(myHuman->GetHP() > 0)) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_END);
	}

	player_skill_prev_active_timer = current_active_timer;
}

// 로봇 처형 프로토콜 상태 갱신
void maingame::ProcessRobotExecuteSkillState(human* myHuman)
{
	if (myHuman == NULL) { return; }

	if (myHuman->GetSkillRobotExecuteFlag() == true) {
		float orders[PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS];

		if (IsRobotExecuteInPrep(myHuman) == true) {
			// 준비 페널티 구간에는 마커 대상 없음
			player_skill_robot_target_count = 0;
			for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
				player_skill_robot_target_ids[i] = -1;
			}

			player_skill_robot_prev_marker_count = 0;
			for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
				player_skill_robot_prev_marker_ids[i] = -1;
			}
		}
		else {
			int old_count = player_skill_robot_prev_marker_count;
			int old_ids[PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS];

			for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
				old_ids[i] = player_skill_robot_prev_marker_ids[i];
			}

			player_skill_robot_target_count = CollectRobotExecuteTargets(
				myHuman,
				player_skill_robot_target_ids,
				orders,
				PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS
			);

			// 새로 마커가 생긴 대상이 있는지 확인한다.
			bool new_marker_found = false;

			for (int i = 0; i < player_skill_robot_target_count; i++) {
				int id = player_skill_robot_target_ids[i];
				bool existed = false;

				for (int j = 0; j < old_count; j++) {
					if (old_ids[j] == id) {
						existed = true;
						break;
					}
				}

				if (existed == false) {
					new_marker_found = true;
					break;
				}
			}

			// 한 프레임에 여러 마커가 새로 생겨도 효과음은 1번만 재생
			if (new_marker_found == true) {
				PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
			}

			// 이번 프레임의 마커 목록을 다음 프레임 비교용으로 저장
			player_skill_robot_prev_marker_count = player_skill_robot_target_count;
			for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
				if (i < player_skill_robot_target_count) {
					player_skill_robot_prev_marker_ids[i] = player_skill_robot_target_ids[i];
				}
				else {
					player_skill_robot_prev_marker_ids[i] = -1;
				}
			}
		}
	}
	else {
		int robot_skill_slot = -1;

		for (int slot = 0; slot < PLAYER_SKILL_SLOT_COUNT; slot++) {
			if (player_skill_current_type[slot] == PLAYER_SKILL_ROBOT_EXECUTE) {
				robot_skill_slot = slot;
				break;
			}
		}

		if ((robot_skill_slot >= 0) &&
			(player_skill_prev_active_timer > 0) &&
			(myHuman->GetSkillRobotExecuteTimer() <= 0)) {

			// 시간이 자연 종료된 경우: 스킬 종료 + 취소 쿨타임 20초
			player_skill_robot_target_count = 0;
			for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
				player_skill_robot_target_ids[i] = -1;
			}

			player_skill_robot_prev_marker_count = 0;
			for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
				player_skill_robot_prev_marker_ids[i] = -1;
			}

			player_skill_robot_fire_queue_count = 0;
			player_skill_robot_fire_queue_index = 0;
			player_skill_robot_fire_queue_timer = 0;
			for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
				player_skill_robot_fire_queue_ids[i] = -1;
			}

			player_skill_robot_execute_visual_ammo = 0;
			player_skill_robot_execute_visual_ammo_max = 0;

			StopRobotExecuteVisualWeapon(myHuman);

		StartPlayerSkillCooldownForSlotWithTotal(
			robot_skill_slot,
			PLAYER_SKILL_ROBOT_EXECUTE,
			PLAYER_SKILL_ROBOT_EXECUTE_CANCEL_COOLDOWN_FRAMES,
			PLAYER_SKILL_ROBOT_EXECUTE_FIRE_COOLDOWN_FRAMES
		);
		player_skill_prev_active_timer = 0;
		}
	}
}

// 로봇 처형 프로토콜: 발동 직후 준비 페널티 구간인지 확인
bool maingame::IsRobotExecuteInPrep(human* myHuman)
{
	if (myHuman == NULL) { return false; }
	if (myHuman->GetSkillRobotExecuteFlag() == false) { return false; }

	int remain = myHuman->GetSkillRobotExecuteTimer();
	int elapsed = PLAYER_SKILL_ROBOT_EXECUTE_FRAMES - remain;

	if (elapsed < 0) { elapsed = 0; }

	return (elapsed < PLAYER_SKILL_ROBOT_EXECUTE_PREP_FRAMES);
}

// 로봇 처형 프로토콜: 준비 페이드 진행률
float maingame::GetRobotExecutePrepRate(human* myHuman)
{
	if (myHuman == NULL) { return 1.0f; }
	if (myHuman->GetSkillRobotExecuteFlag() == false) { return 1.0f; }

	int remain = myHuman->GetSkillRobotExecuteTimer();
	int elapsed = PLAYER_SKILL_ROBOT_EXECUTE_FRAMES - remain;

	if (elapsed < 0) { elapsed = 0; }

	float rate = (float)elapsed / (float)PLAYER_SKILL_ROBOT_EXECUTE_PREP_FRAMES;

	if (rate < 0.0f) { rate = 0.0f; }
	if (rate > 1.0f) { rate = 1.0f; }

	return rate;
}

// 로봇 처형 프로토콜: 준비 2초가 끝난 뒤 실제 8초 구간의 남은 비율
float maingame::GetRobotExecuteActiveRate(human* myHuman)
{
	if (myHuman == NULL) { return 0.0f; }
	if (myHuman->GetSkillRobotExecuteFlag() == false) { return 0.0f; }

	int remain = myHuman->GetSkillRobotExecuteTimer();

	if (remain > PLAYER_SKILL_ROBOT_EXECUTE_ACTIVE_FRAMES) {
		return 1.0f;
	}

	float rate = (float)remain / (float)PLAYER_SKILL_ROBOT_EXECUTE_ACTIVE_FRAMES;

	if (rate < 0.0f) { rate = 0.0f; }
	if (rate > 1.0f) { rate = 1.0f; }

	return rate;
}

// 로봇 처형 프로토콜: 대상이 현재 시야 안에 있고 벽에 막히지 않았는지 확인
bool maingame::IsEnemyInRobotExecuteSight(human* myHuman, human* target, float* out_screen_order)
{
	if (myHuman == NULL) { return false; }
	if (target == NULL) { return false; }
	if (target == myHuman) { return false; }
	if (target->GetEnableFlag() == false) { return false; }
	if (target->GetDeadFlag() == true) { return false; }
	if (target->GetHP() <= 0) { return false; }

	int myteam = -1;
	int target_team = -1;
	myHuman->GetParamData(NULL, NULL, NULL, &myteam);
	target->GetParamData(NULL, NULL, NULL, &target_team);

	if (target_team == myteam) { return false; }

	float px, py, pz, prx;
	float tx, ty, tz, trx;
	float rx, ry;

	myHuman->GetPosData(&px, &py, &pz, &prx);
	myHuman->GetRxRy(&rx, &ry);
	target->GetPosData(&tx, &ty, &tz, &trx);

	float player_eye_y = py + VIEW_HEIGHT;
	if (myHuman->GetCrouchFlag() == true) {
		player_eye_y -= 2.0f;
	}

	// 로봇 처형은 갈고리처럼 몸통에서 줄이 나가는 스킬이 아니라,
	// 화면/시야 기준 판정이므로 기존처럼 눈/카메라 기준을 사용한다.
	// 기존 상체 중심보다 위쪽, 머리 근처를 노린다.
// 0.90f가 너무 높아서 머리 위로 빗나가면 0.82f~0.86f로 낮추면 된다.
	float target_aim_y = ty + HUMAN_HEIGHT * 0.88f;

	// 앉은 적은 실제 머리 위치가 낮으므로 조준점을 약간 낮춘다.
	if (target->GetCrouchFlag() == true) {
		target_aim_y -= 2.0f;
	}

	float vx = tx - px;
	float vy = target_aim_y - player_eye_y;
	float vz = tz - pz;

	float dist_sq = vx * vx + vy * vy + vz * vz;

	if (dist_sq <= 0.001f) { return false; }

	float dist = sqrtf(dist_sq);
	if (dist > PLAYER_SKILL_ROBOT_EXECUTE_MAX_DIST) { return false; }

	vx /= dist;
	vy /= dist;
	vz /= dist;

	float aim_rx = rx * -1.0f + (float)M_PI / 2.0f;

	float fx = cosf(aim_rx) * cosf(ry);
	float fy = sinf(ry);
	float fz = sinf(aim_rx) * cosf(ry);

	float dot = fx * vx + fy * vy + fz * vz;
	float min_dot = cosf(DegreeToRadian(PLAYER_SKILL_ROBOT_EXECUTE_FOV_DEG));

	if (dot < min_dot) { return false; }

	float block_dist = 0.0f;
	bool blocked = CollD.CheckALLBlockIntersectRay(
		px,
		player_eye_y,
		pz,
		vx,
		vy,
		vz,
		NULL,
		NULL,
		&block_dist,
		dist
	);

	if (blocked == true) { return false; }

	// 방패 여부는 타겟팅/마킹 단계에서는 제외하지 않는다.
// 정면 방패에 막히는지는 실제 처형 사격 큐 처리 단계에서 판정한다.
// 그래야 방패 든 대상도 마커는 찍히지만, 좌클릭 처형 사격은 방패에 막힐 수 있다.

	if (out_screen_order != NULL) {
		// 화면 좌우 정렬용 값.
		// 값이 작을수록 왼쪽으로 취급한다.
		float right_x = cosf(aim_rx + (float)M_PI / 2.0f);
		float right_z = sinf(aim_rx + (float)M_PI / 2.0f);

		*out_screen_order = vx * right_x + vz * right_z;
	}

	return true;
}

// 로봇 F 에임핵: 조준점 근처의 가장 가까운 적 1명을 찾는다.
// 탄 판정은 건드리지 않고, 시야/벽/팀/사망 여부만 검사한다.
int maingame::FindAimHackTarget(human* myHuman, float* out_x, float* out_y, float* out_z)
{
	if (myHuman == NULL) { return -1; }

	int player_id = ObjMgr.GetPlayerID();

	int myteam = -1;
	myHuman->GetParamData(NULL, NULL, NULL, &myteam);

	float px, py, pz, prx;
	float rx, ry;

	myHuman->GetPosData(&px, &py, &pz, &prx);

	// 에임핵 타겟 탐색도 실제 보정에 사용하는 기준값과 맞춘다.
	// GetRxRy()는 반동 오프셋이 포함될 수 있으므로 mouse_rx / mouse_ry를 사용한다.
	rx = mouse_rx;
	ry = mouse_ry;

	float player_eye_y = py + VIEW_HEIGHT;
	if (myHuman->GetCrouchFlag() == true) {
		player_eye_y -= 2.0f;
	}

	float aim_rx = rx * -1.0f + (float)M_PI / 2.0f;

	float fx = cosf(aim_rx) * cosf(ry);
	float fy = sinf(ry);
	float fz = sinf(aim_rx) * cosf(ry);

	float min_dot = cosf(DegreeToRadian(PLAYER_SKILL_AIMHACK_FOV_DEG));

	int best_id = -1;
	float best_score = 0.0f;
	float best_x = 0.0f;
	float best_y = 0.0f;
	float best_z = 0.0f;

	for (int i = 0; i < MAX_HUMAN; i++) {
		if (i == player_id) { continue; }

		human* target = ObjMgr.GetHumanObject(i);
		if (target == NULL) { continue; }
		if (target == myHuman) { continue; }
		if (target->GetEnableFlag() == false) { continue; }
		if (target->GetDeadFlag() == true) { continue; }
		if (target->GetHP() <= 0) { continue; }

		int target_team = -1;
		target->GetParamData(NULL, NULL, NULL, &target_team);
		if (target_team == myteam) { continue; }

		float tx, ty, tz, trx;
		target->GetPosData(&tx, &ty, &tz, &trx);

		// 에임핵은 실제 총알 헤드샷 히트박스의 중심보다 살짝 위를 노린다.
// 기존 ty + HUMAN_HEIGHT * 0.88f는 머리 시작점/목 근처라서
// 보정이 조금만 덜 들어가도 몸통 판정이 나기 쉽다.
		float leg_h = HUMAN_BULLETCOLLISION_LEG_H;

		if (target->GetCrouchFlag() == true) {
			// objectmanager.cpp의 GetHumanBulletHitboxParts()와 같은 기준.
			leg_h = HUMAN_BULLETCOLLISION_LEG_H - 2.0f;

			if (leg_h < 1.0f) {
				leg_h = 1.0f;
			}
		}

		float head_y = ty + leg_h + HUMAN_BULLETCOLLISION_UP_H;
		float target_aim_y = head_y + HUMAN_BULLETCOLLISION_HEAD_H * 0.55f;

		float vx = tx - px;
		float vy = target_aim_y - player_eye_y;
		float vz = tz - pz;

		float dist_sq = vx * vx + vy * vy + vz * vz;
		if (dist_sq <= 0.001f) { continue; }

		float dist = sqrtf(dist_sq);
		if (dist > PLAYER_SKILL_AIMHACK_MAX_DIST) { continue; }

		vx /= dist;
		vy /= dist;
		vz /= dist;

		float dot = fx * vx + fy * vy + fz * vz;
		if (dot < min_dot) { continue; }

		float block_dist = 0.0f;

		// 목표 지점 바로 뒤의 벽이나 지형 끝점 때문에 막힘으로 판정되는 것을 줄이기 위해
		// 실제 대상 거리보다 약간 짧게 벽 검사를 한다.
		float ray_max_dist = dist - 2.0f;
		if (ray_max_dist < 1.0f) {
			ray_max_dist = dist;
		}

		bool blocked = CollD.CheckALLBlockIntersectRay(
			px,
			player_eye_y,
			pz,
			vx,
			vy,
			vz,
			NULL,
			NULL,
			&block_dist,
			ray_max_dist
		);

		if (blocked == true) { continue; }

		// 조준점에 가까운 적을 우선하고, 거의 비슷하면 가까운 적을 우선한다.
		float score = (1.0f - dot) * 10000.0f + dist * 0.02f;

		// 이전 프레임의 타겟은 약간 유지한다.
		// 여러 적이 조준점 근처에 있을 때 타겟이 매 프레임 튀는 것을 줄인다.
		if (i == player_skill_aimhack_target_id) {
			score *= 0.75f;
		}

		if ((best_id < 0) || (score < best_score)) {
			best_id = i;
			best_score = score;
			best_x = tx;
			best_y = target_aim_y;
			best_z = tz;
		}
	}

	if (best_id < 0) {
		return -1;
	}

	if (out_x != NULL) { *out_x = best_x; }
	if (out_y != NULL) { *out_y = best_y; }
	if (out_z != NULL) { *out_z = best_z; }

	return best_id;
}

// 로봇 F 에임핵: 현재 조준 방향을 대상 상체 쪽으로 약하게 보정한다.
void maingame::ApplyAimHackAssist(human* myHuman, float target_x, float target_y, float target_z)
{
	if (myHuman == NULL) { return; }

	float px, py, pz, prx;

	myHuman->GetPosData(&px, &py, &pz, &prx);

	// GetRxRy()는 recoil_y_offset이 포함된 값을 반환한다.
	// 에임핵은 반동으로 튄 각도가 아니라 mouse_rx / mouse_ry의 기본 조준값을 직접 보정한다.
	float rx = mouse_rx;
	float ry = mouse_ry;

	float player_eye_y = py + VIEW_HEIGHT;
	if (myHuman->GetCrouchFlag() == true) {
		player_eye_y -= 2.0f;
	}

	float vx = target_x - px;
	float vy = target_y - player_eye_y;
	float vz = target_z - pz;

	float xz_len = sqrtf(vx * vx + vz * vz);
	if (xz_len <= 0.001f) { return; }

	float desired_aim_rx = atan2f(vz, vx);
	float desired_rx = -1.0f * (desired_aim_rx - (float)M_PI / 2.0f);
	desired_rx = AngleNormalization(desired_rx);

	float desired_ry = atan2f(vy, xz_len);

	float max_pitch = DegreeToRadian(70);
	if (desired_ry > max_pitch) { desired_ry = max_pitch; }
	if (desired_ry < -max_pitch) { desired_ry = -max_pitch; }

	float diff_rx = AngleNormalization(desired_rx - rx);
	float diff_ry = desired_ry - ry;

	float max_yaw_step = DegreeToRadian(PLAYER_SKILL_AIMHACK_MAX_YAW_STEP_DEG);
	float max_pitch_step = DegreeToRadian(PLAYER_SKILL_AIMHACK_MAX_PITCH_STEP_DEG);

	if (diff_rx > max_yaw_step) { diff_rx = max_yaw_step; }
	if (diff_rx < -max_yaw_step) { diff_rx = -max_yaw_step; }

	if (diff_ry > max_pitch_step) { diff_ry = max_pitch_step; }
	if (diff_ry < -max_pitch_step) { diff_ry = -max_pitch_step; }

	rx = AngleNormalization(rx + diff_rx * PLAYER_SKILL_AIMHACK_ASSIST_RATE);
	ry = ry + diff_ry * PLAYER_SKILL_AIMHACK_ASSIST_RATE;

	if (ry > max_pitch) { ry = max_pitch; }
	if (ry < -max_pitch) { ry = -max_pitch; }

	// 에임핵 active 중에는 반동 오프셋이 조준 보정을 밀어내지 못하게 한다.
	// 수직/수평 반동 모두 제거한다.
	myHuman->SetRecoilYOffset(0.0f);
	myHuman->SetRecoilXOffset(0.0f);

	// 실제 카메라 기준값과 human 기준값을 동시에 갱신한다.
	mouse_rx = rx;
	mouse_ry = ry;

	myHuman->SetRxRy(rx, ry);
}

// 로봇 F 에임핵: 지속시간 처리 및 조준 보정
void maingame::ProcessAimHackSkill(human* myHuman)
{
	if (player_skill_aimhack_timer <= 0) {
		player_skill_aimhack_target_id = -1;

		if (myHuman != NULL) {
			myHuman->SetSkillAimHackAccuracyFlag(false);
		}

		return;
	}

	if ((myHuman == NULL) ||
		(myHuman->GetHP() <= 0) ||
		(myHuman->GetDeadFlag() == true)) {
		if (myHuman != NULL) {
			myHuman->SetSkillAimHackAccuracyFlag(false);
		}

		player_skill_aimhack_timer = 0;
		player_skill_aimhack_target_id = -1;
		return;
	}

	myHuman->SetSkillAimHackAccuracyFlag(true);

	player_skill_aimhack_timer--;

	if (player_skill_aimhack_timer <= 0) {
		player_skill_aimhack_timer = 0;
		player_skill_aimhack_target_id = -1;
		myHuman->SetSkillAimHackAccuracyFlag(false);
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_END);
		return;
	}

	float target_x = 0.0f;
	float target_y = 0.0f;
	float target_z = 0.0f;

	int target_id = FindAimHackTarget(myHuman, &target_x, &target_y, &target_z);
	player_skill_aimhack_target_id = target_id;

	if (target_id >= 0) {
		ApplyAimHackAssist(myHuman, target_x, target_y, target_z);
	}
}

// 로봇 처형 프로토콜: 현재 시야 안의 적을 왼쪽부터 최대 10명 수집
int maingame::CollectRobotExecuteTargets(human* myHuman, int out_ids[], float out_orders[], int max_count)
{
	if (myHuman == NULL) { return 0; }
	if (out_ids == NULL) { return 0; }
	if (out_orders == NULL) { return 0; }
	if (max_count <= 0) { return 0; }

	int count = 0;
	int player_id = ObjMgr.GetPlayerID();

	for (int i = 0; i < max_count; i++) {
		out_ids[i] = -1;
		out_orders[i] = 0.0f;
	}

	for (int i = 0; i < MAX_HUMAN; i++) {
		if (i == player_id) { continue; }

		human* target = ObjMgr.GetHumanObject(i);
		if (target == NULL) { continue; }

		float order = 0.0f;

		if (IsEnemyInRobotExecuteSight(myHuman, target, &order) == false) {
			continue;
		}

		int insert_pos = count;

		while ((insert_pos > 0) && (out_orders[insert_pos - 1] > order)) {
			insert_pos--;
		}

		if (insert_pos >= max_count) {
			continue;
		}

		if (count < max_count) {
			count++;
		}

		for (int j = count - 1; j > insert_pos; j--) {
			out_ids[j] = out_ids[j - 1];
			out_orders[j] = out_orders[j - 1];
		}

		out_ids[insert_pos] = i;
		out_orders[insert_pos] = order;
	}

	return count;
}

// 로봇 처형 프로토콜: 순차 처형 큐 처리
void maingame::ProcessRobotExecuteFireQueue()
{
	if (player_skill_robot_fire_queue_count <= 0) { return; }

	if (player_skill_robot_fire_queue_timer > 0) {
		player_skill_robot_fire_queue_timer--;
		return;
	}

	human* myHuman = ObjMgr.GetPlayerHumanObject();
	if (myHuman == NULL) { return; }

	int fire_index = -1;

	// 큐에 들어간 대상 중, 현재 시야에 다시 보이는 적을 찾는다.
	// 이미 활성화된 마커 대상만 대상으로 삼는다.
	for (int i = 0; i < player_skill_robot_fire_queue_count; i++) {
		int target_id = player_skill_robot_fire_queue_ids[i];

		if ((target_id < 0) || (target_id >= MAX_HUMAN)) {
			continue;
		}

		human* target = ObjMgr.GetHumanObject(target_id);
		if (target == NULL) { continue; }
		if (target->GetEnableFlag() == false) { continue; }
		if (target->GetDeadFlag() == true) { continue; }
		if (target->GetHP() <= 0) { continue; }

		float order = 0.0f;

		// 현재 화면을 돌려서 다시 시야에 들어온 경우에만 사격/방패 판정을 진행한다.
		// 시야 밖이거나 벽에 막힌 대상에게 처형탄이 소모되는 것을 막는다.
		if (IsEnemyInRobotExecuteSight(myHuman, target, &order) == false) {
			continue;
		}

		// 마커 이후 대상이 정면 방패를 들었다면,
		// 처형탄이 방패에 막힌 것으로 처리하고 큐에서 제거한다.
		if (IsRobotExecuteBlockedByAnyEnemyFrontShield(myHuman, target, true) == true) {
			int player_id = ObjMgr.GetPlayerID();

			PlayPlayerSkillSound(SKILL_SOUND_ROBOT_EXECUTE_SHOT);
			ObjMgr.PlayWeaponShotVisualEffect(player_id, true);

			if (player_skill_robot_execute_visual_ammo > 0) {
				player_skill_robot_execute_visual_ammo--;
			}

			for (int j = i; j < player_skill_robot_fire_queue_count - 1; j++) {
				player_skill_robot_fire_queue_ids[j] = player_skill_robot_fire_queue_ids[j + 1];
			}

			player_skill_robot_fire_queue_count--;

			if (player_skill_robot_fire_queue_count >= 0 &&
				player_skill_robot_fire_queue_count < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS) {
				player_skill_robot_fire_queue_ids[player_skill_robot_fire_queue_count] = -1;
			}

			if (player_skill_robot_fire_queue_count <= 0) {
				player_skill_robot_fire_queue_count = 0;
				player_skill_robot_fire_queue_index = 0;
				player_skill_robot_fire_queue_timer = 0;

				for (int k = 0; k < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; k++) {
					player_skill_robot_fire_queue_ids[k] = -1;
				}

				player_skill_robot_execute_visual_ammo = 0;
				player_skill_robot_execute_visual_ammo_max = 0;

				StopRobotExecuteVisualWeapon(myHuman);
				return;
			}

			player_skill_robot_fire_queue_timer = PLAYER_SKILL_ROBOT_EXECUTE_FIRE_INTERVAL_FRAMES;
			return;
		}

		fire_index = i;
		break;
	}

	// 큐 대상 중 현재 처리 가능한 적이 없으면 큐를 종료한다.
// 기존처럼 큐를 유지하면, 시야 밖/벽 뒤/사망/비활성/거리 밖 대상이 남았을 때
// 처형 프로토콜 화면 효과와 전용 무기 표시가 끝나지 않는 문제가 생길 수 있다.
	if (fire_index < 0) {
		player_skill_robot_fire_queue_count = 0;
		player_skill_robot_fire_queue_index = 0;
		player_skill_robot_fire_queue_timer = 0;

		for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
			player_skill_robot_fire_queue_ids[i] = -1;
		}

		player_skill_robot_execute_visual_ammo = 0;
		player_skill_robot_execute_visual_ammo_max = 0;

		StopRobotExecuteVisualWeapon(myHuman);
		return;
	}

	int target_id = player_skill_robot_fire_queue_ids[fire_index];
	human* target = ObjMgr.GetHumanObject(target_id);

	if (target != NULL) {
		float x, y, z, rx;
		target->GetPosData(&x, &y, &z, &rx);

		int player_id = ObjMgr.GetPlayerID();

		int target_hp = target->GetHP();
		int execute_damage = target_hp + 50;

		if (target_hp <= 100) {
			execute_damage = 150;
		}
		else if (target_hp <= 300) {
			execute_damage = 350;
		}
		else if (target_hp <= 800) {
			execute_damage = 900;
		}
		else if (target_hp <= 1000) {
			execute_damage = 1100;
		}
		else if (target_hp <= 2500) {
			execute_damage = 2600;
		}
		else if (target_hp <= 5000) {
			execute_damage = 5100;
		}
		else if (target_hp <= 10000) {
			execute_damage = 10100;
		}
		else if (target_hp <= 18000) {
			execute_damage = 18100;
		}
		else {
			execute_damage = target_hp + 100;
		}

		// 실제로 처형되는 대상에게만 총소리 재생
		PlayPlayerSkillSound(SKILL_SOUND_ROBOT_EXECUTE_SHOT);

		// 실제 총알은 만들지 않고, 현재 들고 있는 총의 총구화염만 재사용한다.
		// 처형 프로토콜의 즉시 처치/순차 처리 구조는 그대로 유지한다.
		ObjMgr.PlayWeaponShotVisualEffect(player_id, true);

		ObjMgr.SkillDamageHuman(
			target_id,
			0,
			x,
			y + HUMAN_HEIGHT * 0.82f,
			z,
			0.0f,
			execute_damage,
			player_id,
			1.0f
		);

		// 실제 탄약은 건드리지 않고, 처형 프로토콜 HUD 전용 탄약만 줄인다.
		if (player_skill_robot_execute_visual_ammo > 0) {
			player_skill_robot_execute_visual_ammo--;
		}
	}

	// 방금 처형한 대상을 큐에서 제거한다.
	for (int i = fire_index; i < player_skill_robot_fire_queue_count - 1; i++) {
		player_skill_robot_fire_queue_ids[i] = player_skill_robot_fire_queue_ids[i + 1];
	}

	player_skill_robot_fire_queue_count--;

	if (player_skill_robot_fire_queue_count < 0) {
		player_skill_robot_fire_queue_count = 0;
	}

	if (player_skill_robot_fire_queue_count < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS) {
		player_skill_robot_fire_queue_ids[player_skill_robot_fire_queue_count] = -1;
	}

	// 다음 처형까지 0.2초 대기
	player_skill_robot_fire_queue_timer = PLAYER_SKILL_ROBOT_EXECUTE_FIRE_INTERVAL_FRAMES;

	// 큐가 모두 끝났으면 정리
	if (player_skill_robot_fire_queue_count <= 0) {
		player_skill_robot_fire_queue_count = 0;
		player_skill_robot_fire_queue_index = 0;
		player_skill_robot_fire_queue_timer = 0;

		for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
			player_skill_robot_fire_queue_ids[i] = -1;
		}

		player_skill_robot_execute_visual_ammo = 0;
		player_skill_robot_execute_visual_ammo_max = 0;

		// 처형 큐가 모두 끝났을 때 원래 무기 렌더링으로 복귀한다.
		StopRobotExecuteVisualWeapon(myHuman);
	}
}

// 로봇 처형 프로토콜: 좌클릭 발동
void maingame::FireRobotExecuteSkill(human* myHuman)
{
	if (myHuman == NULL) { return; }

	// 준비 페널티 구간에는 발동 불가
	if (IsRobotExecuteInPrep(myHuman) == true) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	int ids[PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS];
	float orders[PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS];

	int count = CollectRobotExecuteTargets(
		myHuman,
		ids,
		orders,
		PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS
	);

	if (count <= 0) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	// 현재 마커 대상들을 왼쪽 순서 그대로 큐에 저장한다.
	player_skill_robot_fire_queue_count = count;
	player_skill_robot_fire_queue_index = 0;
	player_skill_robot_fire_queue_timer = 0;

	for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
		player_skill_robot_fire_queue_ids[i] = -1;
	}

	for (int i = 0; i < count; i++) {
		player_skill_robot_fire_queue_ids[i] = ids[i];
	}

	myHuman->StopSkillRobotExecute();

	player_skill_robot_target_count = 0;
	for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
		player_skill_robot_target_ids[i] = -1;
	}

	// 좌클릭 처형 직후 연발 무기가 이어서 발사되지 않도록 짧게 입력 잠금
	player_skill_input_lock_timer = 5;

	// 좌클릭을 계속 누르고 있으면 입력 잠금 시간이 끝난 뒤 일반 총 발사로 이어질 수 있다.
	// 처형 후에는 좌클릭을 한 번 뗄 때까지 일반 사격 입력을 막는다.
	player_skill_block_shot_until_release = true;

	// 실제 처형 성공 시에는 긴 쿨타임
	StartPlayerSkillCooldown(PLAYER_SKILL_ROBOT_EXECUTE, PLAYER_SKILL_ROBOT_EXECUTE_FIRE_COOLDOWN_FRAMES);
	player_skill_prev_active_timer = 0;

	// 실제 총소리는 큐 처리에서 대상마다 재생한다.
}

// 로봇 처형 프로토콜: 우클릭/E 취소
void maingame::CancelRobotExecuteSkill(human* myHuman)
{
	if (myHuman == NULL) { return; }

	myHuman->StopSkillRobotExecute();
	StopRobotExecuteVisualWeapon(myHuman);

	player_skill_robot_target_count = 0;
	for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
		player_skill_robot_target_ids[i] = -1;
	}

	player_skill_robot_prev_marker_count = 0;
	for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
		player_skill_robot_prev_marker_ids[i] = -1;
	}

	player_skill_robot_fire_queue_count = 0;
	player_skill_robot_fire_queue_index = 0;
	player_skill_robot_fire_queue_timer = 0;
	for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
		player_skill_robot_fire_queue_ids[i] = -1;
	}

	player_skill_robot_execute_visual_ammo = 0;
	player_skill_robot_execute_visual_ammo_max = 0;

	// 취소는 45초 쿨타임 중 20초만 남은 상태로 표시한다.
	StartPlayerSkillCooldownWithTotal(
		PLAYER_SKILL_ROBOT_EXECUTE,
		PLAYER_SKILL_ROBOT_EXECUTE_CANCEL_COOLDOWN_FRAMES,
		PLAYER_SKILL_ROBOT_EXECUTE_FIRE_COOLDOWN_FRAMES
	);
	player_skill_prev_active_timer = 0;

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_END);
}

bool maingame::HandleRobotExecuteSkillInput(human* myHuman, bool primary_skill_key_down, bool secondary_skill_key_down)
{
	if (myHuman == NULL) { return false; }

	// 로봇 처형 프로토콜 중 F는 에임핵 사용 시도이므로 취소하지 않는다.
	// 처형 프로토콜과 에임핵은 동시에 사용할 수 없으므로 금지/실패 효과음만 재생한다.
	if ((secondary_skill_key_down == true) && (primary_skill_key_down == false)) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	// E / 우클릭 = 처형 프로토콜 취소
	if ((primary_skill_key_down == true) || (inputCtrl->CheckMouseButtonDownR() == true)) {
		CancelRobotExecuteSkill(myHuman);
		return true;
	}

	// 좌클릭 = 현재 마커 대상 처형
	if (inputCtrl->CheckMouseButtonDownL() == true) {
		// 발동 직후 2초 준비 구간에는 처형 불가
		if (IsRobotExecuteInPrep(myHuman) == true) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		FireRobotExecuteSkill(myHuman);
		return true;
	}

	// 스킬 중에는 좌클릭/우클릭이 무기 입력으로 내려가지 않게 한다.
	return true;
}

// 좀비 갈고리: 조준선 레이가 실제로 적 몸통을 지나가야 명중
int maingame::FindPlayerHookTargetByAimRay(human* myHuman)
{
	if (myHuman == NULL) { return -1; }

	int myteam = -1;
	myHuman->GetParamData(NULL, NULL, NULL, &myteam);

	float px, py, pz, prx;

	myHuman->GetPosData(&px, &py, &pz, &prx);

	// GetRxRy()는 recoil_y_offset이 더해진 값을 반환한다.
	// 에임핵 타겟 탐색은 반동으로 튄 시야가 아니라, 플레이어의 기본 조준 방향을 기준으로 한다.
	float rx = mouse_rx;
	float ry = mouse_ry;

	float aim_rx = rx * -1.0f + (float)M_PI / 2.0f;

	// 실제 명중 판정도 조준 미리보기와 같은 시작점에서 나가게 한다.
	float sx = px + cosf(aim_rx) * PLAYER_SKILL_HOOK_PLAYER_FORWARD_OFFSET;
	float sy = py + PLAYER_SKILL_HOOK_PLAYER_ATTACH_HEIGHT;
	float sz = pz + sinf(aim_rx) * PLAYER_SKILL_HOOK_PLAYER_FORWARD_OFFSET;

	if (myHuman->GetCrouchFlag() == true) {
		sy -= 2.0f;
	}

	float dir_x = cosf(aim_rx) * cosf(ry);
	float dir_y = sinf(ry);
	float dir_z = sinf(aim_rx) * cosf(ry);

	float wall_dist = PLAYER_SKILL_HOOK_MAX_DIST;

	bool hit_wall = CollD.CheckALLBlockIntersectRay(
		sx,
		sy,
		sz,
		dir_x,
		dir_y,
		dir_z,
		NULL,
		NULL,
		&wall_dist,
		PLAYER_SKILL_HOOK_MAX_DIST
	);

	if (hit_wall == false) {
		wall_dist = PLAYER_SKILL_HOOK_MAX_DIST;
	}

	int player_id = ObjMgr.GetPlayerID();

	int best_id = -1;
	float best_t = PLAYER_SKILL_HOOK_MAX_DIST + 1.0f;

	for (int i = 0; i < MAX_HUMAN; i++) {
		if (i == player_id) { continue; }

		human* target = ObjMgr.GetHumanObject(i);
		if (target == NULL) { continue; }
		if (target == myHuman) { continue; }
		if (target->GetEnableFlag() == false) { continue; }
		if (target->GetDeadFlag() == true) { continue; }
		if (target->GetHP() <= 0) { continue; }

		int target_team = -1;
		target->GetParamData(NULL, NULL, NULL, &target_team);

		if (target_team == myteam) { continue; }

		float tx, ty, tz, trx;
		target->GetPosData(&tx, &ty, &tz, &trx);

		float target_attach_y = ty + HUMAN_HEIGHT * PLAYER_SKILL_HOOK_ATTACH_HEIGHT_RATE;

		float vx = tx - sx;
		float vy = target_attach_y - sy;
		float vz = tz - sz;

		float t = vx * dir_x + vy * dir_y + vz * dir_z;

		if (t < 0.0f) { continue; }
		if (t > wall_dist) { continue; }
		if (t > PLAYER_SKILL_HOOK_MAX_DIST) { continue; }

		float hx = sx + dir_x * t;
		float hy = sy + dir_y * t;
		float hz = sz + dir_z * t;

		if (hy < ty + 1.0f) { continue; }
		if (hy > ty + HUMAN_HEIGHT) { continue; }

		float dx = tx - hx;
		float dz = tz - hz;
		float hit_dist_sq = dx * dx + dz * dz;

		if (hit_dist_sq > PLAYER_SKILL_HOOK_HIT_RADIUS * PLAYER_SKILL_HOOK_HIT_RADIUS) {
			continue;
		}

		if ((best_id < 0) || (t < best_t)) {
			best_id = i;
			best_t = t;
		}
	}

	return best_id;
}

// 좀비 갈고리 시작
bool maingame::StartPlayerHookSkill(human* myHuman)
{
	if (myHuman == NULL) { return false; }

	int target_id = FindPlayerHookTargetByAimRay(myHuman);
	if (target_id < 0) {
		// 못 맞힌 경우:
		// 여기서 쿨타임을 설정하지 않는다.
		return false;
	}

	human* target = ObjMgr.GetHumanObject(target_id);
	if (target == NULL) {
		return false;
	}

	player_skill_hook_target_id = target_id;
	player_skill_hook_prep_timer = PLAYER_SKILL_HOOK_PREP_FRAMES;
	player_skill_hook_pull_timer = 0;
	player_skill_hook_target_prev_force_no_ai = false;
	player_skill_hook_force_no_ai_applied = false;

	// 조준에 성공한 시점에서 갈고리는 사용된 것으로 본다.
	// 이후 방패에 막혀도 쿨타임은 유지된다.
	StartPlayerSkillCooldown(PLAYER_SKILL_HOOK, PLAYER_SKILL_HOOK_COOLDOWN_FRAMES);
	player_skill_prev_active_timer = PLAYER_SKILL_HOOK_PREP_FRAMES;

	player_skill_input_lock_timer = 5;

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_READY);

	return true;
}

// 좀비 갈고리 종료 처리
void maingame::EndPlayerHookSkill(bool play_fail_sound)
{
	if ((player_skill_hook_force_no_ai_applied == true) &&
		(player_skill_hook_target_id >= 0) &&
		(player_skill_hook_target_id < MAX_HUMAN)) {

		int target_id = player_skill_hook_target_id;

		HumanAI[target_id].SetForceNoAI(
			player_skill_hook_target_prev_force_no_ai
		);

		if (player_skill_hook_target_prev_force_no_ai == false) {
			HumanAI[target_id].SetNoFightFlag(false);
			HumanAI[target_id].SetCautionMode();
		}
	}

	player_skill_hook_target_id = -1;
	player_skill_hook_prep_timer = 0;
	player_skill_hook_pull_timer = 0;
	player_skill_hook_target_prev_force_no_ai = false;
	player_skill_hook_force_no_ai_applied = false;

	if (play_fail_sound == true) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
	}
}

// 좀비 갈고리 처리
void maingame::ProcessPlayerHookSkill(human* myHuman)
{
	if (myHuman == NULL) { return; }

	if ((player_skill_hook_prep_timer <= 0) &&
		(player_skill_hook_pull_timer <= 0)) {
		return;
	}

	if ((player_skill_hook_target_id < 0) ||
		(player_skill_hook_target_id >= MAX_HUMAN)) {
		EndPlayerHookSkill(false);
		return;
	}

	human* target = ObjMgr.GetHumanObject(player_skill_hook_target_id);
	if (target == NULL ||
		target->GetEnableFlag() == false ||
		target->GetDeadFlag() == true ||
		target->GetHP() <= 0) {
		EndPlayerHookSkill(false);
		return;
	}

	int myteam = -1;
	int target_team = -1;
	myHuman->GetParamData(NULL, NULL, NULL, &myteam);
	target->GetParamData(NULL, NULL, NULL, &target_team);

	if (target_team == myteam) {
		EndPlayerHookSkill(false);
		return;
	}

	float px, py, pz, prx;
	float tx, ty, tz, trx;
	myHuman->GetPosData(&px, &py, &pz, &prx);
	target->GetPosData(&tx, &ty, &tz, &trx);

	float prx2, pry2;
	myHuman->GetRxRy(&prx2, &pry2);

	float aim_rx_for_attach = prx2 * -1.0f + (float)M_PI / 2.0f;

	float hook_start_x = px + cosf(aim_rx_for_attach) * PLAYER_SKILL_HOOK_PLAYER_FORWARD_OFFSET;
	float hook_start_y = py + PLAYER_SKILL_HOOK_PLAYER_ATTACH_HEIGHT;
	float hook_start_z = pz + sinf(aim_rx_for_attach) * PLAYER_SKILL_HOOK_PLAYER_FORWARD_OFFSET;

	if (myHuman->GetCrouchFlag() == true) {
		hook_start_y -= 2.0f;
	}

	float target_attach_x = tx;
	float target_attach_y = ty + HUMAN_HEIGHT * PLAYER_SKILL_HOOK_ATTACH_HEIGHT_RATE;
	float target_attach_z = tz;

	float vx = target_attach_x - hook_start_x;
	float vy = target_attach_y - hook_start_y;
	float vz = target_attach_z - hook_start_z;

	float dist_sq = vx * vx + vy * vy + vz * vz;
	if (dist_sq <= 0.001f) {
		EndPlayerHookSkill(false);
		return;
	}

	float dist = sqrtf(dist_sq);
	if (dist > PLAYER_SKILL_HOOK_MAX_DIST + 40.0f) {
		EndPlayerHookSkill(false);
		return;
	}

	if (player_skill_hook_prep_timer > 0) {
		bool blocked = IsPlayerSkillHookLineBlocked(
			hook_start_x,
			hook_start_y,
			hook_start_z,
			target_attach_x,
			target_attach_y,
			target_attach_z
		);

		if (blocked == true) {
			EndPlayerHookSkill(false);
			return;
		}

		player_skill_hook_prep_timer--;

		if (player_skill_hook_prep_timer <= 0) {
			player_skill_hook_prep_timer = 0;

			// 갈고리가 도착하는 순간 방패 정면 박스에 막히면,
			// 끌어당김으로 넘어가지 않고 방패 스파크/효과음만 발생시킨다.
			// 쿨타임은 StartPlayerHookSkill()에서 이미 시작되었으므로 유지된다.
			if (ObjMgr.CheckShieldHookBlockSegment(
				target,
				myHuman,
				hook_start_x,
				hook_start_y,
				hook_start_z,
				target_attach_x,
				target_attach_y,
				target_attach_z,
				true
			) == true) {
				EndPlayerHookSkill(false);
				return;
			}

			// 실제 끌어당김이 시작되는 순간에만 대상 AI를 잠근다.
			// 갈고리가 날아가는 도중 방패/벽에 막힌 경우에는 AI 상태를 건드리지 않는다.
			player_skill_hook_target_prev_force_no_ai = HumanAI[player_skill_hook_target_id].GetForceNoAI();
			HumanAI[player_skill_hook_target_id].SetForceNoAI(true);
			player_skill_hook_force_no_ai_applied = true;

			player_skill_hook_pull_timer = PLAYER_SKILL_HOOK_PULL_FRAMES;

			PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
		}

		return;
	}

	if (player_skill_hook_pull_timer <= 0) { return; }

	float aim_rx = aim_rx_for_attach;

	float front_attach_x = px + cosf(aim_rx) * PLAYER_SKILL_HOOK_STOP_DIST;
	float front_attach_z = pz + sinf(aim_rx) * PLAYER_SKILL_HOOK_STOP_DIST;

	// 실제 이동은 발/바닥 기준으로 안정적으로 처리하기 위해 X/Z 중심으로 끌어온다.
	// 갈고리 시각 부착점은 위에서 계산한 target_attach_y를 계속 사용하므로 몸통에 걸린 것처럼 보인다.
	float front_y = py;

	float pull_x = front_attach_x - tx;
	float pull_y = front_y - ty;
	float pull_z = front_attach_z - tz;

	float pull_dist_xz = sqrtf(pull_x * pull_x + pull_z * pull_z);
	float pull_dist = sqrtf(pull_x * pull_x + pull_y * pull_y + pull_z * pull_z);

	float remain_y_dist = fabsf(front_y - ty);

	// X/Z가 가까워도 Y 차이가 남아 있으면 갈고리를 유지한다.
	if ((pull_dist_xz <= PLAYER_SKILL_HOOK_STOP_XZ_DIST) &&
		(remain_y_dist <= PLAYER_SKILL_HOOK_STOP_Y_DIST)) {
		EndPlayerHookSkill(false);
		return;
	}

	float next_x = tx;
	float next_y = ty;
	float next_z = tz;

	if (pull_dist > 0.001f) {
		float pull_speed = PLAYER_SKILL_HOOK_PULL_SPEED;

		if (pull_dist < pull_speed) {
			pull_speed = pull_dist;
		}

		next_x = tx + (pull_x / pull_dist) * pull_speed;
		next_y = ty + (pull_y / pull_dist) * pull_speed;
		next_z = tz + (pull_z / pull_dist) * pull_speed;
	}

	// Y축은 한 프레임에 너무 크게 오르내리지 않게 제한한다.
	if (next_y > ty + PLAYER_SKILL_HOOK_Y_UP_STEP) {
		next_y = ty + PLAYER_SKILL_HOOK_Y_UP_STEP;
	}
	else if (next_y < ty - PLAYER_SKILL_HOOK_Y_DOWN_STEP) {
		next_y = ty - PLAYER_SKILL_HOOK_Y_DOWN_STEP;
	}

	float ground_y = ty;
	bool ground_found = FindPlayerSkillHookGroundY(next_x, next_z, ty, &ground_y);

	// 지면 아래로 파고드는 것만 방지한다.
	// 단, 아래로 끌어내리는 중에 ray가 위쪽 단차를 지면으로 잘못 잡으면 튀어 오를 수 있으므로,
	// 플레이어가 더 낮은 위치에 있을 때는 과한 위쪽 지면 보정을 하지 않는다.
	if ((ground_found == true) && (next_y < ground_y)) {
		float ground_up_diff = ground_y - ty;

		// 일반 경사면은 바로 지면 높이를 따라가게 한다.
		// 이렇게 해야 X/Z만 먼저 이동해서 다리가 땅에 묻히는 현상을 줄일 수 있다.
		if ((ground_up_diff > 0.0f) &&
			(ground_up_diff <= PLAYER_SKILL_HOOK_SLOPE_FOLLOW_MAX_UP)) {
			next_y = ground_y;
		}
		else {
			// 큰 단차나 위아래 높이 차가 큰 상황은 기존처럼 천천히 보정한다.
			// 여기서 바로 ground_y로 올리면 다시 위로 튀는 문제가 생길 수 있다.
			if ((ground_y <= ty + 0.5f) || (front_y >= ty - 0.5f)) {
				next_y = MovePlayerSkillHookYToward(ty, ground_y);
			}
		}
	}

	bool blocked_next = IsPlayerSkillHookMoveBlocked(next_x, next_y, next_z);

	// 경사지지 않은 언덕/단차에서는 첫 판정이 막힐 수 있으므로,
// 한 번 더 위쪽으로 보정해서 재검사한다.
// 단, ground_y로 바로 점프하지 않고 갈고리 전용 Y 이동량만큼만 올린다.
	if ((blocked_next == true) &&
		(ground_found == true) &&
		(front_y >= ty - 0.5f) &&
		(ground_y > ty + 0.5f)) {
		float retry_y = MovePlayerSkillHookYToward(ty, ground_y);

		if (IsPlayerSkillHookMoveBlocked(next_x, retry_y, next_z) == false) {
			next_y = retry_y;
			blocked_next = false;
		}
	}

	if (blocked_next == true) {
		EndPlayerHookSkill(false);
		return;
	}

	target->ResetVerticalMove();
	target->SetPosData(next_x, next_y, next_z, trx);

	player_skill_hook_pull_timer--;

	if (player_skill_hook_pull_timer <= 0) {
		EndPlayerHookSkill(false);
	}
}

bool maingame::ActivateSimpleBuffPlayerSkill(human* myHuman, int skilltype)
{
	if (myHuman == NULL) { return false; }

	if (PlayerSkill_IsSimpleBuffSkill(skilltype) == false) {
		return false;
	}

	// ----------------------------------------------------------
	// 비밀 요원: 은신
	// ----------------------------------------------------------
	if (skilltype == PLAYER_SKILL_STEALTH) {
		myHuman->StartSkillStealth(PLAYER_SKILL_STEALTH_FRAMES);
		player_skill_prev_active_timer = PLAYER_SKILL_STEALTH_FRAMES;

		StartPlayerSkillCooldown(skilltype, PLAYER_SKILL_STEALTH_COOLDOWN_FRAMES);

		PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
		return true;
	}

	// ----------------------------------------------------------
	// 군인: 탄약 강화
	// 장전 중에는 사용 불가 처리한다.
	// ----------------------------------------------------------
	if (skilltype == PLAYER_SKILL_AMMOBOOST) {
		// 스킬 발동 직전에 가능한 경우 현재 무기를 즉시 장전 완료한다.
		// 재장전 중이었다면 기존 재장전음도 같이 멈춘다.
		// 장전할 탄이 없거나 이미 가득 찬 경우에도 스킬 발동은 막지 않는다.
		StopCurrentPlayerReloadSound(myHuman);
		myHuman->InstantReloadWeapon();

		myHuman->StartSkillAmmoBoost(PLAYER_SKILL_AMMOBOOST_FRAMES);

		StartPlayerSkillCooldown(skilltype, PLAYER_SKILL_AMMOBOOST_COOLDOWN_FRAMES);

		PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
		return true;
	}

	// ----------------------------------------------------------
// 시민A: 철괴
// ----------------------------------------------------------
	if (skilltype == PLAYER_SKILL_IRONBODY) {
		myHuman->StartSkillIronBody(PLAYER_SKILL_IRONBODY_FRAMES);
		player_skill_prev_active_timer = PLAYER_SKILL_IRONBODY_FRAMES;

		StartPlayerSkillCooldown(skilltype, PLAYER_SKILL_IRONBODY_COOLDOWN_FRAMES);

		PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
		return true;
	}

	// ----------------------------------------------------------
	// 시민B: 생존 본능
	// ----------------------------------------------------------
	if (skilltype == PLAYER_SKILL_SURVIVAL) {
		myHuman->StartSkillSurvival(PLAYER_SKILL_SURVIVAL_FRAMES);

		// 성공 발동 직후에만 짧은 생존 본능 이펙트를 표시한다.
		player_skill_survival_effect_timer = PLAYER_SKILL_SURVIVAL_EFFECT_FRAMES;

		StartPlayerSkillCooldown(skilltype, PLAYER_SKILL_SURVIVAL_COOLDOWN_FRAMES);

		PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
		return true;
	}

	// ----------------------------------------------------------
// 용병: 전투 흡수
// ----------------------------------------------------------
	if (skilltype == PLAYER_SKILL_ABSORB) {
		// 전투 흡수도 발동 직전에 가능한 경우 현재 무기를 즉시 장전 완료한다.
		// 재장전 중이었다면 기존 재장전음도 같이 멈춘다.
		StopCurrentPlayerReloadSound(myHuman);
		myHuman->InstantReloadWeapon();

		myHuman->StartSkillAbsorb(PLAYER_SKILL_ABSORB_FRAMES);

		StartPlayerSkillCooldown(skilltype, PLAYER_SKILL_ABSORB_COOLDOWN_FRAMES);

		PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
		return true;
	}

	// ----------------------------------------------------------
// 좀비: 격노
// ----------------------------------------------------------
	if (skilltype == PLAYER_SKILL_RAGE) {
		myHuman->StartSkillRage(PLAYER_SKILL_RAGE_FRAMES);

		StartPlayerSkillCooldown(skilltype, PLAYER_SKILL_RAGE_COOLDOWN_FRAMES);

		PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
		return true;
	}

	return false;
}



bool maingame::ActivateTacticalScanSkill(human* myHuman)
{
	if (myHuman == NULL) { return false; }

	player_skill_scan_charge_timer = PLAYER_SKILL_SCAN_CHARGE_FRAMES;
	player_skill_scan_timer = 0;

	// 준비 중 1초마다 ready 효과음을 내기 위한 타이머
	player_skill_scan_charge_sound_timer = GAMEFPS;

	StartPlayerSkillCooldown(PLAYER_SKILL_TACTICAL_SCAN, PLAYER_SKILL_SCAN_COOLDOWN_FRAMES);

	// 사용 즉시 ready 1회
	PlayPlayerSkillSound(SKILL_SOUND_COMMON_READY);
	return true;
}

bool maingame::ActivateHealSkill(human* myHuman)
{
	if (myHuman == NULL) { return false; }

	if (myHuman->HealHP(50) == false) {
		// ̹ ִ ṵ̈ų  ¸ 
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	StartPlayerSkillCooldown(PLAYER_SKILL_HEAL, PLAYER_SKILL_HEAL_COOLDOWN_FRAMES);

	// 성공 발동 직후에만 짧은 회복 이펙트를 표시한다.
	player_skill_heal_effect_timer = PLAYER_SKILL_HEAL_EFFECT_FRAMES;

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
	return true;
}

bool maingame::FindPlayerSkillDodgeDestination(human* myHuman, float* out_x, float* out_y, float* out_z, float* out_dist)
{
	if (myHuman == NULL) { return false; }
	if (out_x == NULL) { return false; }
	if (out_y == NULL) { return false; }
	if (out_z == NULL) { return false; }
	if (out_dist == NULL) { return false; }

	float px, py, pz, prx;

	myHuman->GetPosData(&px, &py, &pz, &prx);

	// 회피 이동은 일반 이동과 같은 방향 계산식을 사용한다.
	// 일반 이동(object.cpp)은 다음 기준으로 이동한다.
	// rotation_x * -1 + move_rx + M_PI / 2
	//
	// 기존처럼 view_rx 기준으로 forward/right 벡터를 따로 만들면,
	// 특정 방향에서 좌우 회피가 시계/반시계 방향으로 어긋날 수 있다.

	bool forward = CheckInputControl(KEY_MOVEFORWARD, 0);
	bool back = CheckInputControl(KEY_MOVEBACKWARD, 0);
	bool left = CheckInputControl(KEY_MOVELEFT, 0);
	bool right = CheckInputControl(KEY_MOVERIGHT, 0);

	int forward_axis = 0;
	int side_axis = 0;

	if ((forward == true) && (back == false)) {
		forward_axis = 1;
	}
	else if ((back == true) && (forward == false)) {
		forward_axis = -1;
	}

	if ((left == true) && (right == false)) {
		side_axis = 1;
	}
	else if ((right == true) && (left == false)) {
		side_axis = -1;
	}

	float move_rx = 0.0f;

	if ((forward_axis == 1) && (side_axis == 0)) {
		move_rx = DegreeToRadian(0);
	}
	else if ((forward_axis == -1) && (side_axis == 0)) {
		move_rx = DegreeToRadian(180);
	}
	else if ((forward_axis == 0) && (side_axis == 1)) {
		move_rx = DegreeToRadian(90);
	}
	else if ((forward_axis == 0) && (side_axis == -1)) {
		move_rx = DegreeToRadian(-90);
	}
	else if ((forward_axis == 1) && (side_axis == 1)) {
		move_rx = DegreeToRadian(45);
	}
	else if ((forward_axis == -1) && (side_axis == 1)) {
		move_rx = DegreeToRadian(135);
	}
	else if ((forward_axis == -1) && (side_axis == -1)) {
		move_rx = DegreeToRadian(-135);
	}
	else if ((forward_axis == 1) && (side_axis == -1)) {
		move_rx = DegreeToRadian(-45);
	}
	else {
		// 아무 이동키도 누르지 않았거나,
		// 앞/뒤 또는 좌/우가 동시에 눌린 경우에는 정면 회피.
		move_rx = DegreeToRadian(0);
	}

	float dodge_rx = prx * -1.0f + move_rx + (float)M_PI / 2.0f;

	float move_x = cosf(dodge_rx);
	float move_z = sinf(dodge_rx);

	float best_x = px;
	float best_y = py;
	float best_z = pz;
	float best_dist = 0.0f;

	float prev_x = px;
	float prev_y = py;
	float prev_z = pz;

	int step_count = (int)(PLAYER_SKILL_DODGE_DISTANCE / PLAYER_SKILL_DODGE_STEP_DIST);
	if (step_count < 1) {
		step_count = 1;
	}

	for (int i = 1; i <= step_count; i++) {
		float step_dist = PLAYER_SKILL_DODGE_STEP_DIST * (float)i;

		if (step_dist > PLAYER_SKILL_DODGE_DISTANCE) {
			step_dist = PLAYER_SKILL_DODGE_DISTANCE;
		}

		float test_x = px + move_x * step_dist;
		float test_z = pz + move_z * step_dist;
		float test_y = prev_y;

		// 현재 단계의 X/Z 지점에서 지면 높이를 찾는다.
		if (FindPlayerSkillDodgeGroundY(test_x, test_z, prev_y, &test_y) == false) {
			break;
		}

		float diff_y = test_y - prev_y;

		// 한 번에 너무 높은 곳으로 올라가려 하면 큰 턱/벽으로 보고 중단한다.
		if (diff_y > PLAYER_SKILL_DODGE_MAX_STEP_UP_DIFF) {
			break;
		}

		// 한 번에 너무 낮은 곳으로 떨어지려 하면 낭떠러지로 보고 중단한다.
		if (diff_y < -PLAYER_SKILL_DODGE_MAX_STEP_DOWN_DIFF) {
			break;
		}

		// 이전 유효 위치에서 다음 후보 위치까지 벽이 막고 있는지 확인한다.
		if (IsPlayerSkillDodgeLineBlocked(prev_x, prev_y, prev_z, test_x, test_y, test_z) == true) {
			break;
		}

		// 목표 위치에 몸이 들어갈 공간이 있는지 확인한다.
		if (IsPlayerSkillDodgeMoveBlocked(test_x, test_y, test_z) == true) {
			break;
		}

		best_x = test_x;
		best_y = test_y;
		best_z = test_z;
		best_dist = step_dist;

		prev_x = test_x;
		prev_y = test_y;
		prev_z = test_z;
	}

	if (best_dist < PLAYER_SKILL_DODGE_MIN_SUCCESS_DIST) {
		return false;
	}

	*out_x = best_x;
	*out_y = best_y;
	*out_z = best_z;
	*out_dist = best_dist;

	return true;
}

bool maingame::ActivateDodgeSkill(human* myHuman)
{
	if (myHuman == NULL) { return false; }

	float px, py, pz, prx;
	myHuman->GetPosData(&px, &py, &pz, &prx);

	float best_x = px;
	float best_y = py;
	float best_z = pz;
	float best_dist = 0.0f;

	if (FindPlayerSkillDodgeDestination(myHuman, &best_x, &best_y, &best_z, &best_dist) == false) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	player_skill_dodge_trail_timer = 8;
	player_skill_dodge_trail_start_x = px;
	player_skill_dodge_trail_start_y = py;
	player_skill_dodge_trail_start_z = pz;
	player_skill_dodge_trail_end_x = best_x;
	player_skill_dodge_trail_end_y = best_y;
	player_skill_dodge_trail_end_z = best_z;

	myHuman->ResetVerticalMove();
	myHuman->SetPosData(best_x, best_y, best_z, prx);
	InvalidatePlayerRenderInterpolation();

	myHuman->StartSkillDodge(PLAYER_SKILL_DODGE_INVINCIBLE_FRAMES);

	StartPlayerSkillCooldown(PLAYER_SKILL_DODGE, PLAYER_SKILL_DODGE_COOLDOWN_FRAMES);

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
	return true;
}

bool maingame::ActivateMarkShotSkill(human* myHuman)
{
	if (myHuman == NULL) { return false; }

	if ((player_skill_mark_prep_timer > 0) || (player_skill_mark_timer > 0)) {
		return true;
	}

	int target_id = FindNearestEnemyForMarkShot(myHuman);

	if (target_id < 0) {
		// 마킹할 적이 없으면 실패
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	// 즉시 마킹하지 않고 1초 준비 상태에 들어간다.
	// 실제 대상 탐색/쿨타임 시작은 ProcessMarkedShotTimer()에서 준비 완료 후 처리한다.
	player_skill_mark_prep_timer = PLAYER_SKILL_MARKSHOT_PREP_FRAMES;
	player_skill_mark_prep_slot = NormalizePlayerSkillSlot(player_skill_using_slot);
	player_skill_mark_target_id = -1;
	player_skill_mark_timer = 0;

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_READY);
	return true;
}

void maingame::StartRobotExecuteVisualWeapon(human* myHuman)
{
	if (myHuman == NULL) { return; }

	// 실제 무기 슬롯/탄약/UI는 건드리지 않고, 1인칭 렌더링만 처형 프로토콜 전용 Mk14로 바꾼다.
	myHuman->SetRenderOverrideWeaponID(ID_WEAPON_ROBOT_EXECUTE_RIFLE);
	myHuman->StartRenderWeaponChangeMotion(ID_WEAPON_ROBOT_EXECUTE_RIFLE, PLAYER_TEMP_SKILL_WEAPON_CHANGE_FRAMES);
}

void maingame::StopRobotExecuteVisualWeapon(human* myHuman)
{
	if (myHuman == NULL) { return; }

	// 다른 스킬의 렌더 override를 실수로 지우지 않도록 처형 프로토콜 전용 무기 override일 때만 해제한다.
	if (myHuman->GetRenderOverrideWeaponID() != ID_WEAPON_ROBOT_EXECUTE_RIFLE) {
		return;
	}

	myHuman->SetRenderOverrideWeaponID(-1);

	int weapon_id = myHuman->GetMainWeaponTypeNO();
	if (weapon_id < 0) {
		weapon_id = ID_WEAPON_NONE;
	}

	// 실제 슬롯은 그대로 두고, 원래 무기를 다시 드는 모션만 준다.
	myHuman->StartRenderWeaponChangeMotion(weapon_id, PLAYER_TEMP_SKILL_WEAPON_CHANGE_FRAMES);
}

bool maingame::ActivateRobotExecuteSkill(human* myHuman)
{
	if (myHuman == NULL) { return false; }

	myHuman->StartSkillRobotExecute(PLAYER_SKILL_ROBOT_EXECUTE_FRAMES);
	StartRobotExecuteVisualWeapon(myHuman);

	player_skill_robot_target_count = 0;
	for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
		player_skill_robot_target_ids[i] = -1;
	}

	player_skill_robot_prev_marker_count = 0;
	for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
		player_skill_robot_prev_marker_ids[i] = -1;
	}

	player_skill_robot_fire_queue_count = 0;
	player_skill_robot_fire_queue_index = 0;
	player_skill_robot_fire_queue_timer = 0;
	for (int i = 0; i < PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS; i++) {
		player_skill_robot_fire_queue_ids[i] = -1;
	}

	player_skill_robot_execute_visual_ammo_max = PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS;
	player_skill_robot_execute_visual_ammo = player_skill_robot_execute_visual_ammo_max;

	// 로봇 처형은 시작 시 쿨타임을 걸지 않는다.
// 취소 또는 실제 처형 시점에 쿨타임이 시작된다.
	int skill_slot = NormalizePlayerSkillSlot(player_skill_using_slot);

	player_skill_ready_sound_played[skill_slot] = false;
	player_skill_current_type[skill_slot] = PLAYER_SKILL_ROBOT_EXECUTE;
	player_skill_prev_active_timer = PLAYER_SKILL_ROBOT_EXECUTE_FRAMES;

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_READY);
	return true;
}

bool maingame::ActivateAimHackSkill(human* myHuman)
{
	if (myHuman == NULL) { return false; }

	if (IsPlayerCurrentWeaponReloading(myHuman) == true) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	player_skill_aimhack_timer = PLAYER_SKILL_AIMHACK_FRAMES;
	player_skill_aimhack_target_id = -1;

	myHuman->SetSkillAimHackAccuracyFlag(true);

	StartPlayerSkillCooldown(PLAYER_SKILL_AIMHACK, PLAYER_SKILL_AIMHACK_COOLDOWN_FRAMES);

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
	return true;
}

bool maingame::ActivateSuppressSkill(human* myHuman)
{
	if (myHuman == NULL) { return false; }

	int suppressed_count = SuppressEnemiesInSight(myHuman);

	if (suppressed_count <= 0) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	StartPlayerSkillCooldown(PLAYER_SKILL_SUPPRESS, PLAYER_SKILL_SUPPRESS_COOLDOWN_FRAMES);

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
	return true;
}

bool maingame::ActivateSpecialPlayerSkill(human* myHuman, int skilltype)
{
	if (myHuman == NULL) { return false; }

	// ----------------------------------------------------------
	// 대장: 전술 스캔
	// ----------------------------------------------------------
	if (skilltype == PLAYER_SKILL_TACTICAL_SCAN) {
		return ActivateTacticalScanSkill(myHuman);
	}

	// ----------------------------------------------------------
	// 시민A: 응급 치료
	// ----------------------------------------------------------
	if (skilltype == PLAYER_SKILL_HEAL) {
		return ActivateHealSkill(myHuman);
	}

	// ----------------------------------------------------------
// 시민B: 긴급 회피
// ----------------------------------------------------------
	if (skilltype == PLAYER_SKILL_DODGE) {
		return ActivateDodgeSkill(myHuman);
	}

	// ----------------------------------------------------------
	// 요원A: 마킹 샷
	// ----------------------------------------------------------
	if (skilltype == PLAYER_SKILL_MARKSHOT) {
		return ActivateMarkShotSkill(myHuman);
	}

	// ----------------------------------------------------------
	// 로봇: 처형 프로토콜
	// ----------------------------------------------------------
	if (skilltype == PLAYER_SKILL_ROBOT_EXECUTE) {
		return ActivateRobotExecuteSkill(myHuman);
	}

	// ----------------------------------------------------------
	// 로봇: 에임핵
	// ----------------------------------------------------------
	if (skilltype == PLAYER_SKILL_AIMHACK) {
		return ActivateAimHackSkill(myHuman);
	}

	// ----------------------------------------------------------
	// 경찰: 제압 명령
	// ----------------------------------------------------------
	if (skilltype == PLAYER_SKILL_SUPPRESS) {
		return ActivateSuppressSkill(myHuman);
	}

	return false;
}

static bool IsPlayerSkillAimTargetVisible(float sx, float sy, float sz, float tx, float ty, float tz)
{
	// 허공 조준 후 아래 바닥을 찾은 경우에만 사용하는 시야 검사.
	// 직접 맞힌 구조물/바닥에는 사용하지 않는다.
	const float PLAYER_SKILL_AIM_VISIBLE_HEIGHT = 4.0f;
	const float PLAYER_SKILL_AIM_VISIBLE_MARGIN = 2.0f;

	float vx = tx - sx;
	float vy = (ty + PLAYER_SKILL_AIM_VISIBLE_HEIGHT) - sy;
	float vz = tz - sz;

	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (dist <= PLAYER_SKILL_AIM_VISIBLE_MARGIN) {
		return true;
	}

	vx /= dist;
	vy /= dist;
	vz /= dist;

	int block_id = -1;
	int face = -1;
	float hit_dist = 0.0f;

	bool blocked = CollD.CheckALLBlockIntersectRay(
		sx, sy, sz,
		vx, vy, vz,
		&block_id, &face, &hit_dist,
		dist - PLAYER_SKILL_AIM_VISIBLE_MARGIN
	);

	return (blocked == false);
}

static bool TryPlayerSkillAimGroundCandidate(float x, float hit_y, float z, float* out_y)
{
	const float PLAYER_SKILL_AIM_HIT_SNAP_UP = 4.0f;
	const float PLAYER_SKILL_AIM_HIT_GROUND_SEARCH = 18.0f;
	const float PLAYER_SKILL_AIM_MAX_SNAP_Y_DIFF = 14.0f;

	if (CollD.CheckALLBlockInside(x, hit_y + PLAYER_SKILL_AIM_HIT_SNAP_UP, z) == true) {
		return false;
	}

	float ground_start_y = hit_y + PLAYER_SKILL_AIM_HIT_SNAP_UP;
	float ground_dist = 0.0f;
	int ground_block = -1;
	int ground_face = -1;

	bool ground_hit = CollD.CheckALLBlockIntersectRay(
		x, ground_start_y, z,
		0.0f, -1.0f, 0.0f,
		&ground_block, &ground_face, &ground_dist,
		PLAYER_SKILL_AIM_HIT_GROUND_SEARCH
	);

	if (ground_hit == false) {
		return false;
	}

	float final_y = ground_start_y - ground_dist + 0.5f;

	float y_diff = final_y - hit_y;
	if (y_diff < 0.0f) {
		y_diff = -y_diff;
	}

	// 벽을 조준했는데 한참 아래 바닥으로 붙는 것을 방지한다.
	if (y_diff > PLAYER_SKILL_AIM_MAX_SNAP_Y_DIFF) {
		return false;
	}

	if (CollD.CheckALLBlockInside(x, final_y + 4.0f, z) == true) {
		return false;
	}

	if (out_y != NULL) {
		*out_y = final_y;
	}

	return true;
}

static bool IsPlayerSkillHumanFootSupportStable(float x, float y, float z)
{
	// 캐릭터 반경 기준으로 발밑이 모두 비슷한 높이인지 확인한다.
	// 상자 모서리/난간 끝/좁은 구조물 위를 걸러내는 공통 검사다.
	const float FOOT_SUPPORT_R = HUMAN_MAPCOLLISION_R * 0.85f;
	const float FOOT_SUPPORT_START_Y = 4.0f;
	const float FOOT_SUPPORT_RAY_DIST = 8.0f;
	const float FOOT_SUPPORT_MAX_Y_DIFF = 2.5f;

	float sample_x[9];
	float sample_z[9];

	sample_x[0] = 0.0f;
	sample_z[0] = 0.0f;

	sample_x[1] = FOOT_SUPPORT_R;
	sample_z[1] = 0.0f;

	sample_x[2] = -FOOT_SUPPORT_R;
	sample_z[2] = 0.0f;

	sample_x[3] = 0.0f;
	sample_z[3] = FOOT_SUPPORT_R;

	sample_x[4] = 0.0f;
	sample_z[4] = -FOOT_SUPPORT_R;

	sample_x[5] = FOOT_SUPPORT_R * 0.7071f;
	sample_z[5] = FOOT_SUPPORT_R * 0.7071f;

	sample_x[6] = -FOOT_SUPPORT_R * 0.7071f;
	sample_z[6] = FOOT_SUPPORT_R * 0.7071f;

	sample_x[7] = FOOT_SUPPORT_R * 0.7071f;
	sample_z[7] = -FOOT_SUPPORT_R * 0.7071f;

	sample_x[8] = -FOOT_SUPPORT_R * 0.7071f;
	sample_z[8] = -FOOT_SUPPORT_R * 0.7071f;

	for (int i = 0; i < 9; i++) {
		float sx = x + sample_x[i];
		float sz = z + sample_z[i];

		float start_y = y + FOOT_SUPPORT_START_Y;
		float ground_dist = 0.0f;
		int ground_block = -1;
		int ground_face = -1;

		bool ground_hit = CollD.CheckALLBlockIntersectRay(
			sx, start_y, sz,
			0.0f, -1.0f, 0.0f,
			&ground_block, &ground_face, &ground_dist,
			FOOT_SUPPORT_RAY_DIST
		);

		if (ground_hit == false) {
			return false;
		}

		float ground_y = start_y - ground_dist + 0.5f;
		float y_diff = ground_y - y;

		if (y_diff < 0.0f) {
			y_diff = -y_diff;
		}

		if (y_diff > FOOT_SUPPORT_MAX_Y_DIFF) {
			return false;
		}
	}

	return true;
}

static bool FindPlayerSkillAimGroundNearHit(float hit_x, float hit_y, float hit_z, float dir_x, float dir_z, float* out_x, float* out_y, float* out_z)
{
	const float PLAYER_SKILL_AIM_CENTER_KEEP_Y_DIFF = 2.0f;
	const float PLAYER_SKILL_AIM_SAMPLE_STEP = 3.0f;
	const int PLAYER_SKILL_AIM_SAMPLE_RING = 2;

	float center_y = 0.0f;

	if (TryPlayerSkillAimGroundCandidate(hit_x, hit_y, hit_z, &center_y) == true) {
		float center_diff = center_y - hit_y;
		if (center_diff < 0.0f) {
			center_diff = -center_diff;
		}

		// 중심 후보가 평평하고 발밑 지지면도 안정적이면 그대로 사용한다.
		// 그렇지 않으면 상자/난간 모서리일 가능성이 있으므로 주변 안전 지점을 찾는다.
		if ((center_diff <= PLAYER_SKILL_AIM_CENTER_KEEP_Y_DIFF) &&
			(IsPlayerSkillHumanFootSupportStable(hit_x, center_y, hit_z) == true)) {
			if (out_x != NULL) { *out_x = hit_x; }
			if (out_y != NULL) { *out_y = center_y; }
			if (out_z != NULL) { *out_z = hit_z; }
			return true;
		}
	}

	float hlen = sqrtf(dir_x * dir_x + dir_z * dir_z);
	float ndx = 1.0f;
	float ndz = 0.0f;

	if (hlen > 0.001f) {
		ndx = dir_x / hlen;
		ndz = dir_z / hlen;
	}

	float side_x = -ndz;
	float side_z = ndx;

	bool found_stable = false;
	float stable_x = hit_x;
	float stable_y = hit_y;
	float stable_z = hit_z;
	float stable_offset = 999999.0f;

	bool found_fallback = false;
	float fallback_x = hit_x;
	float fallback_y = hit_y;
	float fallback_z = hit_z;
	float fallback_offset = 999999.0f;

	for (int forward_i = -PLAYER_SKILL_AIM_SAMPLE_RING; forward_i <= PLAYER_SKILL_AIM_SAMPLE_RING; forward_i++) {
		for (int side_i = -PLAYER_SKILL_AIM_SAMPLE_RING; side_i <= PLAYER_SKILL_AIM_SAMPLE_RING; side_i++) {
			float local_forward = (float)forward_i * PLAYER_SKILL_AIM_SAMPLE_STEP;
			float local_side = (float)side_i * PLAYER_SKILL_AIM_SAMPLE_STEP;

			float cx = hit_x + ndx * local_forward + side_x * local_side;
			float cz = hit_z + ndz * local_forward + side_z * local_side;
			float cy = 0.0f;

			if (TryPlayerSkillAimGroundCandidate(cx, hit_y, cz, &cy) == false) {
				continue;
			}

			float offset = local_forward * local_forward + local_side * local_side;

			// fallback 후보도 저장해둔다.
			// 안전 후보를 못 찾았을 때 기존 동작에 가깝게 유지하기 위함이다.
			if (
				(found_fallback == false) ||
				(cy > fallback_y + 0.5f) ||
				((cy > fallback_y - 0.5f) && (offset < fallback_offset))
				) {
				found_fallback = true;
				fallback_x = cx;
				fallback_y = cy;
				fallback_z = cz;
				fallback_offset = offset;
			}

			// 텔레포트처럼 사람 크기의 공간이 필요한 스킬을 위해
			// 발밑이 안정적인 후보를 우선 찾는다.
			if (IsPlayerSkillHumanFootSupportStable(cx, cy, cz) == false) {
				continue;
			}

			// 상자/난간 위처럼 높은 후보를 우선하고,
			// 높이가 거의 같으면 조준 지점에 가까운 후보를 선택한다.
			if (
				(found_stable == false) ||
				(cy > stable_y + 0.5f) ||
				((cy > stable_y - 0.5f) && (offset < stable_offset))
				) {
				found_stable = true;
				stable_x = cx;
				stable_y = cy;
				stable_z = cz;
				stable_offset = offset;
			}
		}
	}

	if (found_stable == true) {
		if (out_x != NULL) { *out_x = stable_x; }
		if (out_y != NULL) { *out_y = stable_y; }
		if (out_z != NULL) { *out_z = stable_z; }
		return true;
	}

	if (found_fallback == true) {
		if (out_x != NULL) { *out_x = fallback_x; }
		if (out_y != NULL) { *out_y = fallback_y; }
		if (out_z != NULL) { *out_z = fallback_z; }
		return true;
	}

	return false;
}

static bool IsPlayerSkillAirGroundHeightAllowed(int skilltype, float player_y, float target_y)
{
	// 허공 조준으로 아래 바닥을 찾은 경우에만 사용하는 높이 제한.
	// 직접 맞힌 상자/난간/바닥에는 적용하지 않는다.
	float max_up_diff = 24.0f;
	float max_down_diff = 32.0f;

	if (skilltype == PLAYER_SKILL_TELEPORT) {
		// 텔레포트는 아래층/천장 위로 잘못 잡히면 위험하므로 가장 엄격하게 둔다.
		max_up_diff = 22.0f;
		max_down_diff = 28.0f;
	}
	else if (skilltype == PLAYER_SKILL_SUMMON) {
		// 동료 소환은 사람 3명이 생기므로 텔레포트에 가깝게 제한한다.
		max_up_diff = 24.0f;
		max_down_diff = 32.0f;
	}
	else if (skilltype == PLAYER_SKILL_DISTRACTION) {
		// 교란 장치도 설치형 물체이므로 지뢰와 비슷하게 높이 제한을 둔다.
		max_up_diff = 18.0f;
		max_down_diff = 24.0f;
	}
	else if (skilltype == PLAYER_SKILL_TERRORIST_MINE) {
		// 지뢰는 짧은 거리 설치형이므로 높이 차이를 엄격하게 둔다.
		max_up_diff = 18.0f;
		max_down_diff = 24.0f;
	}
	else if (skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) {
		// 폭격은 텔레포트처럼 위쪽 지점도 지정할 수 있게 한다.
		// 단, 허공 조준으로 엉뚱한 아래층/위층 바닥이 잡히는 경우는 제한한다.
		max_up_diff = 80.0f;
		max_down_diff = 90.0f;
	}

	float diff = target_y - player_y;

	if (diff > max_up_diff) {
		return false;
	}

	if (diff < -max_down_diff) {
		return false;
	}

	return true;
}

static bool UpdatePlayerSkillAimTargetRawFromPose(
	human* myHuman,
	int skilltype,
	float max_dist,
	bool need_ground,
	float px,
	float py,
	float pz,
	float aim_rx,
	float aim_ry,
	float* out_x,
	float* out_y,
	float* out_z
)
{
	if (myHuman == NULL) { return false; }

	float crouch_y = 0.0f;
	if (myHuman->GetCrouchFlag() == true) {
		crouch_y = -2.0f;
	}

	float sx = px;
	float sy = py + VIEW_HEIGHT + crouch_y;
	float sz = pz;

	float dir_x = cosf(aim_rx) * cosf(aim_ry);
	float dir_y = sinf(aim_ry);
	float dir_z = sinf(aim_rx) * cosf(aim_ry);

	int block_id = -1;
	int face = -1;
	float dist = 0.0f;

	bool hit = CollD.CheckALLBlockIntersectRay(
		sx, sy, sz,
		dir_x, dir_y, dir_z,
		&block_id, &face, &dist,
		max_dist
	);

	float tx, ty, tz;

	if (hit == true) {
		tx = sx + dir_x * dist;
		ty = sy + dir_y * dist;
		tz = sz + dir_z * dist;
	}
	else {
		tx = sx + dir_x * max_dist;
		ty = sy + dir_y * max_dist;
		tz = sz + dir_z * max_dist;
	}

	// 바닥 위 지점으로 보정한다.
	if (need_ground == true) {
		bool ground_hit = false;
		float ground_start_y = 0.0f;
		float ground_dist = 0.0f;
		int ground_block = -1;
		int ground_face = -1;

		if (hit == true) {
			// 직접 무언가를 맞힌 경우:
			// 난간/상자/얇은 구조물은 hit 지점 하나만 보면 옆면이나 모서리에 걸려 흔들릴 수 있다.
			// 따라서 hit 지점 주변을 몇 점 샘플링해서 가장 안정적인 위쪽 지점을 고른다.
			if (FindPlayerSkillAimGroundNearHit(tx, ty, tz, dir_x, dir_z, &tx, &ty, &tz) == false) {
				return false;
			}

			ground_hit = true;

			// 아래쪽 공통 보정식이 ty를 다시 계산하므로,
			// 이미 보정된 ty가 유지되도록 ground_start_y / ground_dist를 맞춰둔다.
			ground_start_y = ty - 0.5f;
			ground_dist = 0.0f;

			// 직접 보이는 구조물을 맞힌 경우이므로 별도 시야 검사는 하지 않는다.
			// 여기서 시야 검사를 하면 상자/난간 자체에 ray가 걸려 조준 불가가 될 수 있다.
		}
		else {
			// 허공을 조준한 경우에만 아래쪽 바닥을 찾는다.
			// 이 경우에는 시야 밖 천장 위/벽 뒤 바닥이 잡힐 수 있으므로 최종 시야 검사를 한다.
			ground_start_y = ty + 40.0f;

			ground_hit = CollD.CheckALLBlockIntersectRay(
				tx, ground_start_y, tz,
				0.0f, -1.0f, 0.0f,
				&ground_block, &ground_face, &ground_dist,
				120.0f
			);

			if (ground_hit == false) {
				return false;
			}
		}

		ty = ground_start_y - ground_dist + 0.5f;

		// 허공 조준으로 아래 바닥을 찾은 경우에는 낙차 제한과 시야 검사를 한다.
// 지뢰/교란 장치는 직접 맞힌 상자/난간/바닥이라도 높이 차이가 너무 크면 설치하지 않는다.
		if ((hit == false) ||
			(skilltype == PLAYER_SKILL_DISTRACTION) ||
			(skilltype == PLAYER_SKILL_TERRORIST_MINE)) {
			if (IsPlayerSkillAirGroundHeightAllowed(skilltype, py, ty) == false) {
				return false;
			}
		}

		if (hit == false) {
			if (IsPlayerSkillAimTargetVisible(sx, sy, sz, tx, ty, tz) == false) {
				return false;
			}
		}
	}

	// 너무 벽 안쪽이면 실패 처리
	if (CollD.CheckALLBlockInside(tx, ty + 4.0f, tz) == true) {
		return false;
	}

	if (out_x != NULL) { *out_x = tx; }
	if (out_y != NULL) { *out_y = ty; }
	if (out_z != NULL) { *out_z = tz; }

	return true;
}

static bool UpdatePlayerSkillAimTargetRaw(human* myHuman, int skilltype, float max_dist, bool need_ground, float* out_x, float* out_y, float* out_z)
{
	if (myHuman == NULL) { return false; }

	float px, py, pz;
	float rx, ry;

	myHuman->GetPosData(&px, &py, &pz, NULL);
	myHuman->GetRxRy(&rx, &ry);

	float aim_rx = rx * -1.0f + (float)M_PI / 2.0f;

	return UpdatePlayerSkillAimTargetRawFromPose(
		myHuman,
		skilltype,
		max_dist,
		need_ground,
		px,
		py,
		pz,
		aim_rx,
		ry,
		out_x,
		out_y,
		out_z
	);
}

bool maingame::UpdatePlayerSkillAimTargetVisualRaw(human* myHuman, float max_dist, bool need_ground, float* out_x, float* out_y, float* out_z)
{
	if (myHuman == NULL) { return false; }

	float px, py, pz;
	GetSkillVisualHumanPosition(myHuman, &px, &py, &pz);

	return UpdatePlayerSkillAimTargetRawFromPose(
		myHuman,
		player_skill_targeting_type,
		max_dist,
		need_ground,
		px,
		py,
		pz,
		draw_camera_rx,
		draw_camera_ry,
		out_x,
		out_y,
		out_z
	);
}

bool maingame::UpdatePlayerSkillAimTarget(human* myHuman, float max_dist, bool need_ground, float* out_x, float* out_y, float* out_z)
{
	// 조준형 지면 스킬의 미리보기에서만 짧은 smoothing을 적용한다.
	// 좌클릭 확정은 아래 ConfirmPlayerSkillTargeting()에서 Raw 함수를 다시 호출한다.
	int skilltype = player_skill_targeting_type;

	if ((skilltype == PLAYER_SKILL_NONE) ||
		(PlayerSkill_IsGroundTargetingSkill(skilltype) == false)) {
		return UpdatePlayerSkillAimTargetRaw(myHuman, skilltype, max_dist, need_ground, out_x, out_y, out_z);
	}

	float tx = 0.0f;
	float ty = 0.0f;
	float tz = 0.0f;

	if (UpdatePlayerSkillAimTargetRaw(myHuman, skilltype, max_dist, need_ground, &tx, &ty, &tz) == true) {
		if ((skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) &&
			(player_skill_aim_smooth_has_cache == true) &&
			(player_skill_aim_smooth_type == skilltype)) {

			float dx = tx - player_skill_aim_smooth_x;
			float dy = ty - player_skill_aim_smooth_y;
			float dz = tz - player_skill_aim_smooth_z;
			float blend_dist_sq =
				PLAYER_SKILL_ARTILLERY_AIM_SMOOTH_MAX_BLEND_DIST *
				PLAYER_SKILL_ARTILLERY_AIM_SMOOTH_MAX_BLEND_DIST;

			// 너무 멀리 순간 이동한 경우에는 보간하지 않고 바로 따라간다.
			// 가까운 모서리/난간에서만 부드럽게 보이도록 한다.
			if ((dx * dx + dy * dy + dz * dz) <= blend_dist_sq) {
				tx = player_skill_aim_smooth_x + dx * PLAYER_SKILL_ARTILLERY_AIM_SMOOTH_LERP_RATE;
				ty = player_skill_aim_smooth_y + dy * PLAYER_SKILL_ARTILLERY_AIM_SMOOTH_LERP_RATE;
				tz = player_skill_aim_smooth_z + dz * PLAYER_SKILL_ARTILLERY_AIM_SMOOTH_LERP_RATE;
			}
		}

		player_skill_aim_smooth_has_cache = true;
		if (skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) {
			player_skill_aim_smooth_keep_timer = PLAYER_SKILL_ARTILLERY_AIM_SMOOTH_KEEP_FRAMES;
		}
		else {
			player_skill_aim_smooth_keep_timer = PLAYER_SKILL_AIM_SMOOTH_KEEP_FRAMES;
		}
		player_skill_aim_smooth_type = skilltype;
		player_skill_aim_smooth_x = tx;
		player_skill_aim_smooth_y = ty;
		player_skill_aim_smooth_z = tz;

		if (out_x != NULL) { *out_x = tx; }
		if (out_y != NULL) { *out_y = ty; }
		if (out_z != NULL) { *out_z = tz; }

		return true;
	}

	// 이번 프레임에만 잠깐 실패한 경우, 직전 유효 위치를 짧게 유지한다.
	// 상자/난간/모서리에서 마커가 빨강/초록으로 빠르게 깜빡이는 현상을 줄인다.
	if ((player_skill_aim_smooth_has_cache == true) &&
		(player_skill_aim_smooth_keep_timer > 0) &&
		(player_skill_aim_smooth_type == skilltype)) {

		player_skill_aim_smooth_keep_timer--;

		if (IsPlayerSkillGroundTargetValid(
			skilltype,
			player_skill_aim_smooth_x,
			player_skill_aim_smooth_y,
			player_skill_aim_smooth_z
		) == true) {
			if (out_x != NULL) { *out_x = player_skill_aim_smooth_x; }
			if (out_y != NULL) { *out_y = player_skill_aim_smooth_y; }
			if (out_z != NULL) { *out_z = player_skill_aim_smooth_z; }

			return true;
		}
	}

	ClearPlayerSkillAimTargetSmoothCache();
	return false;
}

static bool IsPlayerSkillHumanSpaceBlocked(float x, float y, float z)
{
	const float r = HUMAN_MAPCOLLISION_R;

	float low_y = y + 4.0f;
	float body_y = y + HUMAN_MAPCOLLISION_HEIGHT;
	float head_y = y + HUMAN_HEIGHT - 1.0f;

	if (CollD.CheckALLBlockInside(x, low_y, z) == true) { return true; }

	if (CollD.CheckALLBlockInside(x, body_y, z) == true) { return true; }
	if (CollD.CheckALLBlockInside(x + r, body_y, z) == true) { return true; }
	if (CollD.CheckALLBlockInside(x - r, body_y, z) == true) { return true; }
	if (CollD.CheckALLBlockInside(x, body_y, z + r) == true) { return true; }
	if (CollD.CheckALLBlockInside(x, body_y, z - r) == true) { return true; }

	if (CollD.CheckALLBlockInside(x, head_y, z) == true) { return true; }
	if (CollD.CheckALLBlockInside(x + r, head_y, z) == true) { return true; }
	if (CollD.CheckALLBlockInside(x - r, head_y, z) == true) { return true; }
	if (CollD.CheckALLBlockInside(x, head_y, z + r) == true) { return true; }
	if (CollD.CheckALLBlockInside(x, head_y, z - r) == true) { return true; }

	return false;
}

static void GetPlayerSkillSummonFormationOffsets(float center_x, float center_z, float* offset_x, float* offset_z)
{
	if (offset_x == NULL) { return; }
	if (offset_z == NULL) { return; }

	const float spacing = 8.0f;

	for (int i = 0; i < PLAYER_SKILL_SUMMON_COUNT; i++) {
		offset_x[i] = 0.0f;
		offset_z[i] = 0.0f;
	}

	human* playerHuman = ObjMgr.GetPlayerHumanObject();

	float forward_x = 1.0f;
	float forward_z = 0.0f;

	if (playerHuman != NULL) {
		float px, py, pz, prx;
		playerHuman->GetPosData(&px, &py, &pz, &prx);

		forward_x = center_x - px;
		forward_z = center_z - pz;

		float len = sqrtf(forward_x * forward_x + forward_z * forward_z);

		if (len > 0.001f) {
			forward_x /= len;
			forward_z /= len;
		}
		else {
			// 소환 지점이 플레이어와 너무 가까우면 플레이어의 바라보는 방향을 사용한다.
			forward_x = cosf(prx * -1.0f + (float)M_PI / 2.0f);
			forward_z = sinf(prx * -1.0f + (float)M_PI / 2.0f);
		}
	}

	// 플레이어가 보는 방향 기준 좌우 방향.
	float right_x = forward_z;
	float right_z = -forward_x;

	offset_x[0] = 0.0f;
	offset_z[0] = 0.0f;

	offset_x[1] = right_x * spacing;
	offset_z[1] = right_z * spacing;

	offset_x[2] = -right_x * spacing;
	offset_z[2] = -right_z * spacing;
}

static bool IsPlayerSkillTeleportSafeSpaceBlocked(float x, float y, float z)
{
	// 텔레포트는 순간이동 직후 끼임/압사/사망 판정이 생길 수 있으므로
	// 일반 사람 공간 검사보다 약간 더 보수적으로 검사한다.
	const float TELEPORT_SPACE_MARGIN = 0.8f;

	float r = HUMAN_MAPCOLLISION_R + TELEPORT_SPACE_MARGIN;

	// 바닥 자체를 잡지 않도록 y + 2.0f부터 검사한다.
	// 머리는 HUMAN_HEIGHT 바로 끝이 아니라 살짝 아래까지만 검사한다.
	float min_y = y + 2.0f;
	float max_y = y + HUMAN_HEIGHT - 1.0f;

	if (CollD.CheckALLBlockAABB(
		x - r, min_y, z - r,
		x + r, max_y, z + r
	) == true) {
		return true;
	}

	return false;
}

static bool IsPlayerSkillTeleportFootSupportValid(float x, float y, float z)
{
	// 텔레포트 최종 판정도 조준 후보 보정과 같은 발밑 안정성 기준을 사용한다.
	return IsPlayerSkillHumanFootSupportStable(x, y, z);
}

static bool IsPlayerSkillHumanPlacementValid(float x, float y, float z, bool require_stable_foot)
{
	if (IsPlayerSkillHumanSpaceBlocked(x, y, z) == true) {
		return false;
	}

	if (require_stable_foot == true) {
		if (IsPlayerSkillHumanFootSupportStable(x, y, z) == false) {
			return false;
		}
	}

	return true;
}

static bool IsPlayerSkillTeleportPlacementValid(float x, float y, float z)
{
	// 텔레포트는 사람 전체 공간과 안정적인 발밑 지지면이 모두 필요하다.
	if (IsPlayerSkillHumanPlacementValid(x, y, z, true) == false) {
		return false;
	}

	// 텔레포트 전용 여유 공간 검사.
	if (IsPlayerSkillTeleportSafeSpaceBlocked(x, y, z) == true) {
		return false;
	}

	// 텔레포트 전용 발밑 지지면 검사.
	// 위의 사람형 검사와 같은 기준이지만, 텔레포트 전용 의미를 유지하기 위해 분리해둔다.
	if (IsPlayerSkillTeleportFootSupportValid(x, y, z) == false) {
		return false;
	}

	return true;
}

static bool IsPlayerSkillSingleHumanPlacementValid(float x, float y, float z)
{
	return IsPlayerSkillHumanPlacementValid(x, y, z, false);
}

static bool IsPlayerSkillSummonPlacementValid(float x, float y, float z)
{
	float offset_x[PLAYER_SKILL_SUMMON_COUNT];
	float offset_z[PLAYER_SKILL_SUMMON_COUNT];

	GetPlayerSkillSummonFormationOffsets(x, z, offset_x, offset_z);

	for (int i = 0; i < PLAYER_SKILL_SUMMON_COUNT; i++) {
		float sx = x + offset_x[i];
		float sz = z + offset_z[i];

		// 동료 소환은 사람 공간은 필요하지만,
		// 텔레포트처럼 매우 엄격한 발밑 지지면 검사는 아직 적용하지 않는다.
		if (IsPlayerSkillHumanPlacementValid(sx, y, sz, false) == false) {
			return false;
		}
	}

	return true;
}

static bool IsPlayerInsideArtilleryStrikeDangerArea(float x, float z)
{
	human* playerHuman = ObjMgr.GetPlayerHumanObject();

	if (playerHuman == NULL) { return false; }
	if (playerHuman->GetEnableFlag() == false) { return false; }
	if (playerHuman->GetDeadFlag() == true) { return false; }
	if (playerHuman->GetHP() <= 0) { return false; }

	float px = 0.0f;
	float py = 0.0f;
	float pz = 0.0f;
	float prx = 0.0f;

	playerHuman->GetPosData(&px, &py, &pz, &prx);

	float dx = px - x;
	float dz = pz - z;

	// 폭격 미리보기 원 안에 플레이어 몸이 걸치면 사용 불가로 본다.
	float danger_radius = PLAYER_SKILL_ARTILLERY_STRIKE_RADIUS + HUMAN_MAPCOLLISION_R;
	float danger_radius_sq = danger_radius * danger_radius;

	return ((dx * dx + dz * dz) <= danger_radius_sq);
}

static bool FindPlayerSkillArtilleryGroundAt(float x, float y, float z, float* out_y)
{
	if (out_y == NULL) { return false; }

	float ray_start_y = y + PLAYER_SKILL_ARTILLERY_FOOTPRINT_GROUND_UP;
	float ground_dist = 0.0f;
	int ground_block = -1;
	int ground_face = -1;

	bool ground_hit = CollD.CheckALLBlockIntersectRay(
		x,
		ray_start_y,
		z,
		0.0f,
		-1.0f,
		0.0f,
		&ground_block,
		&ground_face,
		&ground_dist,
		PLAYER_SKILL_ARTILLERY_FOOTPRINT_GROUND_UP + PLAYER_SKILL_ARTILLERY_FOOTPRINT_GROUND_DOWN
	);

	if (ground_hit == false) {
		return false;
	}

	float ground_y = ray_start_y - ground_dist + 0.5f;
	float y_diff = ground_y - y;

	if (y_diff < 0.0f) {
		y_diff = -y_diff;
	}

	if (y_diff > PLAYER_SKILL_ARTILLERY_FOOTPRINT_MAX_Y_DIFF) {
		return false;
	}

	if (CollD.CheckALLBlockInside(x, ground_y + PLAYER_SKILL_ARTILLERY_PLACEMENT_CLEAR_Y, z) == true) {
		return false;
	}

	*out_y = ground_y;
	return true;
}

static bool IsPlayerSkillArtilleryStrikeHasDownSpreadPath(float x, float y, float z)
{
	float down_x = 0.0f;
	float down_y = 0.0f;
	float down_z = 0.0f;

	return FindPlayerArtilleryStrikeDownSpreadPoint(x, y, z, &down_x, &down_y, &down_z);
}

static bool IsPlayerSkillArtilleryStrikeFootprintWideEnough(float x, float y, float z)
{
	float sample_r = PLAYER_SKILL_ARTILLERY_FOOTPRINT_SAMPLE_R;
	float diag_r = sample_r * 0.70710678f;

	float sample_x[9] = {
		0.0f,
		sample_r,
		-sample_r,
		0.0f,
		0.0f,
		diag_r,
		diag_r,
		-diag_r,
		-diag_r
	};

	float sample_z[9] = {
		0.0f,
		0.0f,
		0.0f,
		sample_r,
		-sample_r,
		diag_r,
		-diag_r,
		diag_r,
		-diag_r
	};

	int support_count = 0;

	for (int i = 0; i < 9; i++) {
		float gy = 0.0f;

		if (FindPlayerSkillArtilleryGroundAt(x + sample_x[i], y, z + sample_z[i], &gy) == false) {
			continue;
		}

		support_count++;
	}

	if (support_count >= PLAYER_SKILL_ARTILLERY_FOOTPRINT_MIN_SUPPORT) {
		return true;
	}

	// 좁은 위층이라도 열린 가장자리/구멍을 통해 아래로 폭발이 퍼질 수 있으면 허용한다.
	// 단, 너무 좁은 난간/상자 모서리처럼 지지점이 거의 없는 곳은 계속 금지한다.
	if (support_count >= PLAYER_SKILL_ARTILLERY_FOOTPRINT_MIN_SUPPORT_WITH_DOWN_SPREAD) {
		if (IsPlayerSkillArtilleryStrikeHasDownSpreadPath(x, y, z) == true) {
			return true;
		}
	}

	return false;
}

static bool IsPlayerSkillArtilleryStrikePlacementValid(float x, float y, float z)
{
	// 전술 폭격은 실제 장치를 세우는 것이 아니므로 소형 설치물보다 공간 판정을 완화한다.
	// 중심점 주변이 벽 내부만 아니면 기본적으로 허용한다.
	if (CollD.CheckALLBlockInside(x, y + PLAYER_SKILL_ARTILLERY_PLACEMENT_CLEAR_Y, z) == true) {
		return false;
	}

	if (CollD.CheckALLBlockInside(x + PLAYER_SKILL_ARTILLERY_PLACEMENT_CLEAR_R, y + PLAYER_SKILL_ARTILLERY_PLACEMENT_CLEAR_Y, z) == true) {
		return false;
	}

	if (CollD.CheckALLBlockInside(x - PLAYER_SKILL_ARTILLERY_PLACEMENT_CLEAR_R, y + PLAYER_SKILL_ARTILLERY_PLACEMENT_CLEAR_Y, z) == true) {
		return false;
	}

	if (CollD.CheckALLBlockInside(x, y + PLAYER_SKILL_ARTILLERY_PLACEMENT_CLEAR_Y, z + PLAYER_SKILL_ARTILLERY_PLACEMENT_CLEAR_R) == true) {
		return false;
	}

	if (CollD.CheckALLBlockInside(x, y + PLAYER_SKILL_ARTILLERY_PLACEMENT_CLEAR_Y, z - PLAYER_SKILL_ARTILLERY_PLACEMENT_CLEAR_R) == true) {
		return false;
	}

	// 너무 좁은 위층/난간/상자 위처럼 폭격 범위가 사실상 거의 없는 곳은 막는다.
	if (IsPlayerSkillArtilleryStrikeFootprintWideEnough(x, y, z) == false) {
		return false;
	}

	if (IsPlayerInsideArtilleryStrikeDangerArea(x, z) == true) {
		return false;
	}

	return true;
}

static bool IsPlayerSkillInstallHeightAllowedFromPlayer(int skilltype, float target_y)
{
	human* playerHuman = ObjMgr.GetPlayerHumanObject();

	if (playerHuman == NULL) { return true; }
	if (playerHuman->GetEnableFlag() == false) { return true; }
	if (playerHuman->GetDeadFlag() == true) { return true; }
	if (playerHuman->GetHP() <= 0) { return true; }

	float px = 0.0f;
	float py = 0.0f;
	float pz = 0.0f;
	float prx = 0.0f;

	playerHuman->GetPosData(&px, &py, &pz, &prx);

	float max_up_diff = PLAYER_SKILL_INSTALL_MAX_UP_DIFF_SMALL_DEVICE;
	float max_down_diff = PLAYER_SKILL_INSTALL_MAX_DOWN_DIFF_SMALL_DEVICE;

	if ((skilltype == PLAYER_SKILL_SUMMON) ||
		(skilltype == PLAYER_SKILL_SHADOW_DECOY)) {
		max_up_diff = PLAYER_SKILL_INSTALL_MAX_UP_DIFF_HUMAN_OBJECT;
		max_down_diff = PLAYER_SKILL_INSTALL_MAX_DOWN_DIFF_HUMAN_OBJECT;
	}
	else if (skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) {
		max_up_diff = PLAYER_SKILL_INSTALL_MAX_UP_DIFF_ARTILLERY;
		max_down_diff = PLAYER_SKILL_INSTALL_MAX_DOWN_DIFF_ARTILLERY;
	}

	float diff = target_y - py;

	if (diff > max_up_diff) {
		return false;
	}

	if (diff < -max_down_diff) {
		return false;
	}

	return true;
}

static bool IsPlayerSkillInstallTooCloseToPlayer(int skilltype, float target_x, float target_z)
{
	// 생체장은 자기 주변에 설치해도 자연스럽다.
	// 전술 폭격은 IsPlayerInsideArtilleryStrikeDangerArea()에서 더 강하게 검사한다.
	if ((skilltype == PLAYER_SKILL_BIOTIC_FIELD) ||
		(skilltype == PLAYER_SKILL_ARTILLERY_STRIKE)) {
		return false;
	}

	human* playerHuman = ObjMgr.GetPlayerHumanObject();

	if (playerHuman == NULL) { return false; }
	if (playerHuman->GetEnableFlag() == false) { return false; }
	if (playerHuman->GetDeadFlag() == true) { return false; }
	if (playerHuman->GetHP() <= 0) { return false; }

	float px = 0.0f;
	float py = 0.0f;
	float pz = 0.0f;
	float prx = 0.0f;

	playerHuman->GetPosData(&px, &py, &pz, &prx);

	float min_dist = PLAYER_SKILL_INSTALL_MIN_DIST_SMALL_DEVICE;

	if ((skilltype == PLAYER_SKILL_SUMMON) ||
		(skilltype == PLAYER_SKILL_SHADOW_DECOY)) {
		min_dist = PLAYER_SKILL_INSTALL_MIN_DIST_HUMAN_OBJECT;
	}

	float dx = target_x - px;
	float dz = target_z - pz;
	float min_dist_sq = min_dist * min_dist;

	return ((dx * dx + dz * dz) < min_dist_sq);
}

static bool IsPlayerSkillSmallDevicePlacementValid(float x, float y, float z)
{
	// 교란 신호, 지뢰, 생체장, 전술 폭격 표식 같은 작은 설치물용 검사.
	// 사람 키 전체 공간은 요구하지 않고, 장치가 벽/바닥 내부에 묻히지 않는지만 본다.
	if (CollD.CheckALLBlockInside(x, y + PLAYER_SKILL_SMALL_DEVICE_LOW_Y, z) == true) {
		return false;
	}

	if (CollD.CheckALLBlockInside(x, y + PLAYER_SKILL_SMALL_DEVICE_BODY_Y, z) == true) {
		return false;
	}

	if (CollD.CheckALLBlockInside(x + PLAYER_SKILL_SMALL_DEVICE_CHECK_R, y + PLAYER_SKILL_SMALL_DEVICE_BODY_Y, z) == true) {
		return false;
	}

	if (CollD.CheckALLBlockInside(x - PLAYER_SKILL_SMALL_DEVICE_CHECK_R, y + PLAYER_SKILL_SMALL_DEVICE_BODY_Y, z) == true) {
		return false;
	}

	if (CollD.CheckALLBlockInside(x, y + PLAYER_SKILL_SMALL_DEVICE_BODY_Y, z + PLAYER_SKILL_SMALL_DEVICE_CHECK_R) == true) {
		return false;
	}

	if (CollD.CheckALLBlockInside(x, y + PLAYER_SKILL_SMALL_DEVICE_BODY_Y, z - PLAYER_SKILL_SMALL_DEVICE_CHECK_R) == true) {
		return false;
	}

	if (CollD.CheckALLBlockInside(x, y + PLAYER_SKILL_SMALL_DEVICE_TOP_Y, z) == true) {
		return false;
	}

	return true;
}

bool maingame::IsPlayerSkillGroundTargetValid(int skilltype, float x, float y, float z)
{
	if (skilltype == PLAYER_SKILL_TELEPORT) {
		return IsPlayerSkillTeleportPlacementValid(x, y, z);
	}

	if (IsPlayerSkillInstallHeightAllowedFromPlayer(skilltype, y) == false) {
		return false;
	}

	if (IsPlayerSkillInstallTooCloseToPlayer(skilltype, x, z) == true) {
		return false;
	}

	if (skilltype == PLAYER_SKILL_SUMMON) {
		return IsPlayerSkillSummonPlacementValid(x, y, z);
	}

	if (skilltype == PLAYER_SKILL_SHADOW_DECOY) {
		return IsPlayerSkillSingleHumanPlacementValid(x, y, z);
	}

	if (skilltype == PLAYER_SKILL_BIOTIC_FIELD) {
		return IsPlayerSkillSmallDevicePlacementValid(x, y, z);
	}

	if (skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) {
		return IsPlayerSkillArtilleryStrikePlacementValid(x, y, z);
	}

	if (skilltype == PLAYER_SKILL_DISTRACTION) {
		return IsPlayerSkillSmallDevicePlacementValid(x, y, z);
	}

	if (skilltype == PLAYER_SKILL_TERRORIST_MINE) {
		return IsPlayerSkillSmallDevicePlacementValid(x, y, z);
	}

	// 이후 소형 설치물 스킬이 추가되면 기본값도 작은 장치 기준으로 처리한다.
	return IsPlayerSkillSmallDevicePlacementValid(x, y, z);
}

float maingame::GetPlayerSkillTargetPreviewHeight(int skilltype)
{
	if ((skilltype == PLAYER_SKILL_SUMMON) ||
		(skilltype == PLAYER_SKILL_SHADOW_DECOY) ||
		(skilltype == PLAYER_SKILL_TELEPORT)) {
		return HUMAN_HEIGHT;
	}

	if (skilltype == PLAYER_SKILL_BIOTIC_FIELD) {
		return PLAYER_SKILL_PREVIEW_HEIGHT_BIOTIC_FIELD;
	}

	if (skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) {
		return PLAYER_SKILL_PREVIEW_HEIGHT_ARTILLERY_STRIKE;
	}

	if (skilltype == PLAYER_SKILL_TERRORIST_MINE) {
		return PLAYER_SKILL_PREVIEW_HEIGHT_TERRORIST_MINE;
	}

	return PLAYER_SKILL_PREVIEW_HEIGHT_DEFAULT;
}

bool maingame::StartPlayerTeleportSkill(human* myHuman, float tx, float ty, float tz)
{
	if (myHuman == NULL) { return false; }

	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_TELEPORT, tx, ty, tz) == false) {
		return false;
	}

	player_skill_teleport_cast_timer = PLAYER_SKILL_TELEPORT_CAST_FRAMES;
	player_skill_teleport_slot = NormalizePlayerSkillSlot(player_skill_using_slot);
	player_skill_teleport_x = tx;
	player_skill_teleport_y = ty;
	player_skill_teleport_z = tz;

	float px, py, pz, prx;
	myHuman->GetPosData(&px, &py, &pz, &prx);

	player_skill_teleport_start_effect_timer = PLAYER_SKILL_TELEPORT_EFFECT_FRAMES;
	player_skill_teleport_start_effect_x = px;
	player_skill_teleport_start_effect_y = py;
	player_skill_teleport_start_effect_z = pz;

	myHuman->StartSkillTeleportCast(PLAYER_SKILL_TELEPORT_CAST_FRAMES);

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
	return true;
}

void maingame::ProcessPlayerTeleportSkill(human* myHuman)
{
	if (player_skill_teleport_cast_timer <= 0) {
		return;
	}

	if ((myHuman == NULL) ||
		(myHuman->GetEnableFlag() == false) ||
		(myHuman->GetDeadFlag() == true) ||
		(myHuman->GetHP() <= 0)) {

		if (myHuman != NULL) {
			myHuman->StartSkillTeleportCast(0);
		}

		player_skill_teleport_cast_timer = 0;
		player_skill_teleport_slot = PLAYER_SKILL_SLOT_PRIMARY;
		player_skill_teleport_x = 0.0f;
		player_skill_teleport_y = 0.0f;
		player_skill_teleport_z = 0.0f;
		return;
	}

	player_skill_teleport_cast_timer--;

	if (player_skill_teleport_cast_timer > 0) {
		return;
	}

	myHuman->StartSkillTeleportCast(0);

	int teleport_slot = NormalizePlayerSkillSlot(player_skill_teleport_slot);

	float tx = player_skill_teleport_x;
	float ty = player_skill_teleport_y;
	float tz = player_skill_teleport_z;

	player_skill_teleport_slot = PLAYER_SKILL_SLOT_PRIMARY;
	player_skill_teleport_x = 0.0f;
	player_skill_teleport_y = 0.0f;
	player_skill_teleport_z = 0.0f;

	// 시전 완료 시점에 다시 한 번 검사한다.
	// 낮은 천장/벽 안/머리 걸림 위치가 되면 이동하지 않고 실패 처리한다.
	if (IsPlayerSkillGroundTargetValid(PLAYER_SKILL_TELEPORT, tx, ty, tz) == false) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return;
	}

	float px, py, pz, prx;
	myHuman->GetPosData(&px, &py, &pz, &prx);

	myHuman->ResetVerticalMove();
	myHuman->SetPosData(tx, ty, tz, prx);
	InvalidatePlayerRenderInterpolation();

	player_skill_teleport_end_effect_timer = PLAYER_SKILL_TELEPORT_EFFECT_FRAMES;
	player_skill_teleport_end_effect_x = tx;
	player_skill_teleport_end_effect_y = ty;
	player_skill_teleport_end_effect_z = tz;

	StartPlayerSkillCooldownForSlot(
		teleport_slot,
		PLAYER_SKILL_TELEPORT,
		PLAYER_SKILL_TELEPORT_COOLDOWN_FRAMES
	);

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_END);
}

bool maingame::IsPlayerSkillTargeting()
{
	return (player_skill_targeting_type != PLAYER_SKILL_NONE);
}

void maingame::BeginPlayerSkillTargeting(int skilltype, int slot)
{
	player_skill_targeting_type = skilltype;
	player_skill_targeting_slot = NormalizePlayerSkillSlot(slot);
	player_skill_target_valid = false;
	player_skill_target_x = 0.0f;
	player_skill_target_y = 0.0f;
	player_skill_target_z = 0.0f;

	ClearPlayerSkillAimTargetSmoothCache();

	// 갈고리는 E 입력 직후 무기 입력이 이어지지 않도록 짧게 잠근다.
	if (skilltype == PLAYER_SKILL_HOOK) {
		player_skill_input_lock_timer = 3;
	}

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_READY);
}

bool maingame::BeginTargetingPlayerSkillIfNeeded(int skilltype, int slot)
{
	if (PlayerSkill_IsTargetingSkill(skilltype) == false) {
		return false;
	}

	BeginPlayerSkillTargeting(skilltype, slot);
	return true;
}

void maingame::CancelPlayerSkillTargeting()
{
	player_skill_targeting_type = PLAYER_SKILL_NONE;
	player_skill_targeting_slot = PLAYER_SKILL_SLOT_PRIMARY;
	player_skill_target_valid = false;
	player_skill_target_x = 0.0f;
	player_skill_target_y = 0.0f;
	player_skill_target_z = 0.0f;

	ClearPlayerSkillAimTargetSmoothCache();
}

bool maingame::ActivateGroundTargetingSkill(int skilltype, float tx, float ty, float tz)
{
	// 좌클릭 확정 직후 연발 무기가 이어서 발사되지 않도록 짧게 입력 잠금
	player_skill_input_lock_timer = 5;

	if (skilltype == PLAYER_SKILL_DISTRACTION) {
		human* myHuman = ObjMgr.GetPlayerHumanObject();

		if (StartPlayerDistractionInstall(myHuman, player_skill_using_slot, tx, ty, tz) == false) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return false;
		}

		// 실제 생성과 쿨타임은 0.5초 뒤 ProcessPlayerDistractionInstall()에서 처리한다.
		return true;
	}

	if (skilltype == PLAYER_SKILL_TERRORIST_MINE) {
		human* myHuman = ObjMgr.GetPlayerHumanObject();

		if (StartPlayerTerroristMineInstall(myHuman, player_skill_using_slot, tx, ty, tz) == false) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return false;
		}

		// 실제 설치와 쿨타임은 0.3초 뒤 ProcessPlayerTerroristMineInstall()에서 처리한다.
		return true;
	}

	if (skilltype == PLAYER_SKILL_SUMMON) {
		human* myHuman = ObjMgr.GetPlayerHumanObject();

		if (StartPlayerSummonInstall(myHuman, player_skill_using_slot, tx, ty, tz) == true) {
			return true;
		}

		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return false;
	}

	if (skilltype == PLAYER_SKILL_SHADOW_DECOY) {
		human* myHuman = ObjMgr.GetPlayerHumanObject();

		if (StartPlayerShadowDecoyInstall(myHuman, player_skill_using_slot, tx, ty, tz) == true) {
			return true;
		}

		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return false;
	}

	if (skilltype == PLAYER_SKILL_BIOTIC_FIELD) {
		human* myHuman = ObjMgr.GetPlayerHumanObject();

		if (StartPlayerBioticFieldInstall(myHuman, player_skill_using_slot, tx, ty, tz) == false) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return false;
		}

		// 실제 생성과 쿨타임은 0.5초 뒤 ProcessPlayerBioticFieldInstall()에서 처리한다.
		return true;
	}

	if (skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) {
		human* myHuman = ObjMgr.GetPlayerHumanObject();

		if (StartPlayerArtilleryStrikeInstall(myHuman, player_skill_using_slot, tx, ty, tz) == false) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return false;
		}

		// 실제 폭격 시작과 쿨타임은 0.7초 뒤 ProcessPlayerArtilleryStrikeInstall()에서 처리한다.
		return true;
	}

	if (skilltype == PLAYER_SKILL_TELEPORT) {
		human* myHuman = ObjMgr.GetPlayerHumanObject();

		if (StartPlayerTeleportSkill(myHuman, tx, ty, tz) == false) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return false;
		}

		return true;
	}

	PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
	return false;
}

bool maingame::ConfirmPlayerSkillTargeting(human* myHuman)
{
	if (myHuman == NULL) { return true; }

	int targeting_type = player_skill_targeting_type;
	int targeting_slot = NormalizePlayerSkillSlot(player_skill_targeting_slot);

	if (targeting_type == PLAYER_SKILL_NONE) {
		return false;
	}

	// 조준 확정 시점에도 재장전 중 사용 불가 스킬은 확정하지 않는다.
// 시작 시점의 HUD 빗금/입력 차단과 같은 helper 기준을 사용한다.
	if ((PlayerSkill_IsBlockedWhileReloading(targeting_type) == true) &&
		(IsPlayerCurrentWeaponReloading(myHuman) == true)) {

		CancelPlayerSkillTargeting();
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	// 조준 확정 후 StartPlayerSkillCooldown()이
	// 조준을 시작한 E/F 슬롯에 쿨타임을 걸 수 있도록 기록한다.
	player_skill_using_slot = targeting_slot;

	// ----------------------------------------------------------
	// 조준선 발사형 스킬
	// 현재는 좀비 갈고리만 해당한다.
	// ----------------------------------------------------------
	if (PlayerSkill_IsRayTargetingSkill(targeting_type) == true) {
		CancelPlayerSkillTargeting();

		// 좌클릭 확정 직후 일반 무기 발사가 이어지지 않도록 잠금
		player_skill_input_lock_timer = 5;

		if (StartPlayerHookSkill(myHuman) == false) {
			// 못 맞힌 경우:
			// 쿨타임 없음, 실패음만 재생
			player_skill_input_lock_timer = 6;
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		return true;
	}

	if (PlayerSkill_IsGroundTargetingSkill(targeting_type) == false) {
		CancelPlayerSkillTargeting();
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	float tx, ty, tz;
	float max_dist = PlayerSkill_GetTargetingMaxDist(targeting_type);

	if (max_dist <= 0.0f) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	bool aim_ok = false;

	if (targeting_type == PLAYER_SKILL_ARTILLERY_STRIKE) {
		aim_ok = UpdatePlayerSkillAimTarget(myHuman, max_dist, true, &tx, &ty, &tz);
	}
	else {
		aim_ok = UpdatePlayerSkillAimTargetRaw(myHuman, targeting_type, max_dist, true, &tx, &ty, &tz);
	}

	if (aim_ok == false) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	if (IsPlayerSkillGroundTargetValid(targeting_type, tx, ty, tz) == false) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	CancelPlayerSkillTargeting();

	ActivateGroundTargetingSkill(targeting_type, tx, ty, tz);

	// 좌클릭 입력은 처리했으므로, 발동 성공/실패와 관계없이 true를 반환한다.
	return true;
}

bool maingame::HandlePlayerSkillTargetingInput(human* myHuman, bool skill_key_down)
{
	if (myHuman == NULL) { return false; }

	bool cancel_key_down = skill_key_down;

	// 같은 슬롯 키 / 우클릭은 조준형 스킬 취소
	if ((cancel_key_down == true) || (inputCtrl->CheckMouseButtonDownR() == true)) {
		CancelPlayerSkillTargeting();
		return true;
	}

	// 좌클릭은 조준형 스킬 확정
	if (inputCtrl->CheckMouseButtonDownL() == true) {
		return ConfirmPlayerSkillTargeting(myHuman);
	}

	// 아무 입력이 없으면 기존 동작을 유지한다.
	return false;
}

bool maingame::IsPlayerSkillUnavailableForHud(human* myHuman, int skilltype)
{
	if (myHuman == NULL) { return true; }
	if (skilltype == PLAYER_SKILL_NONE) { return false; }

	if (myHuman->GetHP() <= 0) {
		return true;
	}

	// 설치 대기 중에는 현재 설치 중인 스킬 아이콘은 취소 입력 역할을 하므로 빗금을 띄우지 않는다.
// 반대 슬롯 스킬은 아무 작용을 하지 않으므로 금지 표시를 유지한다.
	if (IsPlayerInstallSkillPending() == true) {
		if (((skilltype == PLAYER_SKILL_DISTRACTION) &&
			(player_skill_distraction_install_pending == true)) ||
			((skilltype == PLAYER_SKILL_TERRORIST_MINE) &&
				(player_skill_terrorist_mine_install_pending == true)) ||
			((skilltype == PLAYER_SKILL_SUMMON) &&
				(player_skill_summon_install_pending == true)) ||
			((skilltype == PLAYER_SKILL_SHADOW_DECOY) &&
				(player_skill_shadow_decoy_install_pending == true)) ||
			((skilltype == PLAYER_SKILL_BIOTIC_FIELD) &&
				(player_skill_biotic_field_install_pending == true)) ||
			((skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) &&
				(player_skill_artillery_strike_install_pending == true))) {
			return false;
		}

		return true;
	}

	// 조준/위치 지정 대기 중에는 현재 조준 중인 스킬 아이콘은 취소 입력 역할을 하므로 빗금을 띄우지 않는다.
	// 반대 슬롯 스킬은 아무 작용을 하지 않으므로 금지 표시를 유지한다.
	if (player_skill_targeting_type != PLAYER_SKILL_NONE) {
		if (skilltype == player_skill_targeting_type) {
			return false;
		}

		return true;
	}

	// DP28은 발동 중이어도 재장전 중에는 E/1/2/무기전환으로 기존 무기 복귀가 불가능하므로
// active 표시 위에 금지 표시도 함께 띄운다.
	if ((skilltype == PLAYER_SKILL_DP28) &&
		(IsPlayerSkillHudActive(myHuman, skilltype) == true) &&
		(IsPlayerCurrentWeaponReloading(myHuman) == true)) {
		return true;
	}

	// 이미 발동 중인 스킬 아이콘에는 금지 표시를 띄우지 않는다.
	// 발동 중 표시는 기존 보라색 active overlay가 담당한다.
	if (IsPlayerSkillHudActive(myHuman, skilltype) == true) {
		return false;
	}

	// 다른 활성 스킬 때문에 현재 스킬을 쓸 수 없는 경우.
	if (IsPlayerSkillBlockedByActiveSkill(myHuman, skilltype) == true) {
		return true;
	}

	// 재장전 중에는 무기 전환형/설치형/공격 보조형 스킬을 사용할 수 없으므로 금지 표시.
// 실제 입력 차단 조건과 HUD 표시를 맞춘다.
	if ((IsPlayerCurrentWeaponReloading(myHuman) == true) &&
		(PlayerSkill_IsBlockedWhileReloading(skilltype) == true)) {
		return true;
	}

	// 군인 E Impact grenade:
// 다른 임시 스킬 무기 사용/전환 중이면 금지 표시.
// Impact grenade 자체가 active인 경우는 active overlay가 담당한다.
	if (skilltype == PLAYER_SKILL_IMPACT_GRENADE) {
		if ((IsPlayerTemporarySkillWeaponActive() == true) &&
			(GetPlayerTemporarySkillWeaponID() != ID_WEAPON_IMPACT_GRENADE)) {
			return true;
		}

		if (GetPlayerTemporarySkillWeaponStockAmmo(ID_WEAPON_IMPACT_GRENADE) <= 0) {
			return true;
		}
	}

	// 경찰 E 방패: 낮은 천장 등으로 방패 높이까지 공간이 없으면 금지 표시.
	// 단, 이미 방패를 들고 있는 상태는 active overlay가 담당하므로 여기서는 사용 불가로 보지 않는다.
	if (skilltype == PLAYER_SKILL_SHIELD) {
		if ((IsPlayerTemporarySkillWeaponActive() == true) &&
			(GetPlayerTemporarySkillWeaponID() != ID_WEAPON_SHIELD)) {
			return true;
		}

		if (CanPlayerStandForShieldEquip(myHuman) == false) {
			return true;
		}
	}

	// 용병 E DP28: 다른 임시 스킬 무기 사용 중이면 금지 표시.
// DP28 사용 중에는 E 재입력으로 복귀해야 하므로 금지 표시하지 않는다.
	if (skilltype == PLAYER_SKILL_DP28) {
		if ((IsPlayerTemporarySkillWeaponActive() == true) ||
			(IsPlayerTemporarySkillWeaponChanging() == true)) {
			return true;
		}
	}

	// 시민A 응급 치료: 체력이 이미 최대면 사용할 수 없으므로 금지 표시.
	if (skilltype == PLAYER_SKILL_HEAL) {
		if (myHuman->GetHP() >= myHuman->GetMaxHP()) {
			return true;
		}
	}

	// 요원A 마킹 샷: 사거리 안에 마킹 가능한 적이 없으면 금지 표시.
	if (skilltype == PLAYER_SKILL_MARKSHOT) {
		if (HasEnemyInMarkShotRangeForHud(myHuman) == false) {
			return true;
		}
	}

	// 경찰 제압 명령: 시야 안에 실제로 무기를 떨어뜨릴 수 있는 적이 없으면 금지 표시.
	if (skilltype == PLAYER_SKILL_SUPPRESS) {
		if (HasSuppressableEnemyInPlayerSightForHud(myHuman) == false) {
			return true;
		}
	}

	// 테러리스트 지뢰/요원B 생체장은 조준 선택형이므로 평상시 HUD에서 현재 정면 위치를 검사하지 않는다.
	// 실제 설치 가능 여부는 조준 미리보기/좌클릭 확정 시점에서 검사한다.

	// 시민B 긴급 회피: 현재 지형/방향 기준으로 최소 이동 거리도 확보할 수 없으면 금지 표시.
	if (skilltype == PLAYER_SKILL_DODGE) {
		float x, y, z, dist;

		if (FindPlayerSkillDodgeDestination(myHuman, &x, &y, &z, &dist) == false) {
			return true;
		}
	}

	return false;
}

bool maingame::TryUsePlayerSkill(human* myHuman)
{
	if (myHuman == NULL) { return false; }

	if (GameConfig.GetSkillModeFlag() == false) {
		return false;
	}

	bool skill_primary_key_down = CheckInputControl(KEY_SKILL, 1);
	bool skill_secondary_key_down = CheckInputControl(KEY_SKILL2, 1);

	// 설치 대기 중에는 같은 슬롯 키를 다시 누르면 설치를 취소한다.
	// E 슬롯 설치 중 E = 취소, F 슬롯 설치 중 F = 취소.
	// 반대 슬롯 키는 아무 작용 없이 설치 대기를 유지한다.
	if (IsPlayerInstallSkillPending() == true) {
		if (CancelPlayerSkillInstallStateForSlot(
			player_skill_distraction_install_pending,
			player_skill_distraction_install_timer,
			player_skill_distraction_install_slot,
			player_skill_distraction_install_x,
			player_skill_distraction_install_y,
			player_skill_distraction_install_z,
			PLAYER_SKILL_SLOT_PRIMARY,
			skill_primary_key_down,
			skill_secondary_key_down
		) == true) {
			return true;
		}

		if (CancelPlayerSkillInstallStateForSlot(
			player_skill_terrorist_mine_install_pending,
			player_skill_terrorist_mine_install_timer,
			player_skill_terrorist_mine_install_slot,
			player_skill_terrorist_mine_install_x,
			player_skill_terrorist_mine_install_y,
			player_skill_terrorist_mine_install_z,
			PLAYER_SKILL_SLOT_SECONDARY,
			skill_primary_key_down,
			skill_secondary_key_down
		) == true) {
			return true;
		}

		if (CancelPlayerSkillInstallStateForSlot(
			player_skill_summon_install_pending,
			player_skill_summon_install_timer,
			player_skill_summon_install_slot,
			player_skill_summon_install_x,
			player_skill_summon_install_y,
			player_skill_summon_install_z,
			PLAYER_SKILL_SLOT_PRIMARY,
			skill_primary_key_down,
			skill_secondary_key_down
		) == true) {
			return true;
		}

		if (CancelPlayerSkillInstallStateForSlot(
			player_skill_shadow_decoy_install_pending,
			player_skill_shadow_decoy_install_timer,
			player_skill_shadow_decoy_install_slot,
			player_skill_shadow_decoy_install_x,
			player_skill_shadow_decoy_install_y,
			player_skill_shadow_decoy_install_z,
			PLAYER_SKILL_SLOT_SECONDARY,
			skill_primary_key_down,
			skill_secondary_key_down
		) == true) {
			return true;
		}

		if (CancelPlayerSkillInstallStateForSlot(
			player_skill_biotic_field_install_pending,
			player_skill_biotic_field_install_timer,
			player_skill_biotic_field_install_slot,
			player_skill_biotic_field_install_x,
			player_skill_biotic_field_install_y,
			player_skill_biotic_field_install_z,
			PLAYER_SKILL_SLOT_SECONDARY,
			skill_primary_key_down,
			skill_secondary_key_down
		) == true) {
			return true;
		}

		if (CancelPlayerSkillInstallStateForSlot(
			player_skill_artillery_strike_install_pending,
			player_skill_artillery_strike_install_timer,
			player_skill_artillery_strike_install_slot,
			player_skill_artillery_strike_install_x,
			player_skill_artillery_strike_install_y,
			player_skill_artillery_strike_install_z,
			PLAYER_SKILL_SLOT_SECONDARY,
			skill_primary_key_down,
			skill_secondary_key_down
		) == true) {
			return true;
		}

		return true;
	}

	// ----------------------------------------------------------
	// 로봇: 처형 프로토콜 모드 중 입력 처리
	// E / 우클릭 = 취소
	// F = 에임핵 사용 불가 실패음
	// 좌클릭 = 현재 마커 대상 처형
	// ----------------------------------------------------------
	if (myHuman->GetSkillRobotExecuteFlag() == true) {
		return HandleRobotExecuteSkillInput(myHuman, skill_primary_key_down, skill_secondary_key_down);
	}

	// ----------------------------------------------------------
// 조준 확정형 스킬 처리
// E 또는 F로 조준 모드 진입 후:
// 좌클릭 = 발동
// 우클릭 = 취소
// 같은 슬롯 키 재입력 = 취소
// 반대 슬롯 키 = 아무 작용 없음
// ----------------------------------------------------------
	if (player_skill_targeting_type != PLAYER_SKILL_NONE) {
		bool targeting_cancel_key_down = IsPlayerSkillInstallCancelKeyDown(
			player_skill_targeting_slot,
			skill_primary_key_down,
			skill_secondary_key_down
		);

		return HandlePlayerSkillTargetingInput(myHuman, targeting_cancel_key_down);
	}

	if ((skill_primary_key_down == false) && (skill_secondary_key_down == false)) {
		return false;
	}

	int skill_slot = PLAYER_SKILL_SLOT_PRIMARY;

	if (skill_secondary_key_down == true) {
		skill_slot = PLAYER_SKILL_SLOT_SECONDARY;
	}

	if (skill_primary_key_down == true) {
		skill_slot = PLAYER_SKILL_SLOT_PRIMARY;
	}

	if (myHuman->GetHP() <= 0) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	int skilltype = GetPlayerSkillTypeBySlot(myHuman, skill_slot);

	// 경찰 E: 방패는 일반 스킬 쿨타임/지속시간을 쓰지 않고,
	// 기존 1/2번 무기를 유지한 채 임시 장착 상태만 켠다.
	if (skilltype == PLAYER_SKILL_SHIELD) {
		if ((IsPlayerTemporarySkillWeaponReady() == true) &&
			(GetPlayerTemporarySkillWeaponID() == ID_WEAPON_SHIELD)) {
			StartPlayerTemporarySkillWeaponReturn(myHuman, GetPlayerTemporarySkillWeaponReturnSlot());
			return true;
		}

		if (IsPlayerTemporarySkillWeaponChanging() == true) {
			return true;
		}

		if (IsPlayerTemporarySkillWeaponActive() == true) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		// 재장전 중에는 방패로 무기를 바꾸지 않는다.
// HUD 빗금 표시와 같은 PlayerSkill_IsBlockedWhileReloading() 기준을 사용한다.
		if ((PlayerSkill_IsBlockedWhileReloading(skilltype) == true) &&
			(IsPlayerCurrentWeaponReloading(myHuman) == true)) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		if (IsPlayerSkillBlockedByActiveSkill(myHuman, skilltype) == true) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		// 방패는 캐릭터보다 높게 올라가므로, 낮은 천장에서는 장착하지 않는다.
		// 앉은 상태라면 설 수 있고 방패 높이까지 공간이 있을 때만 자동으로 일어서고 장착한다.
		if (CanPlayerStandForShieldEquip(myHuman) == false) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		if (myHuman->GetCrouchFlag() == true) {
			myHuman->SetCrouchFlag(false);
		}

		ClearAimStateForPlayerSkill(myHuman);

		int return_slot = 0;
		{
			int selectweapon = 0;
			class weapon* slot_weapon[TOTAL_HAVEWEAPON];
			int lnbslist[TOTAL_HAVEWEAPON];
			int nbslist[TOTAL_HAVEWEAPON];

			for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
				slot_weapon[i] = NULL;
				lnbslist[i] = 0;
				nbslist[i] = 0;
			}

			myHuman->GetWeapon(&selectweapon, slot_weapon, lnbslist, nbslist);

			if ((0 <= selectweapon) && (selectweapon < TOTAL_HAVEWEAPON)) {
				return_slot = selectweapon;
			}
		}

		if (StartPlayerTemporarySkillWeaponEquip(myHuman, ID_WEAPON_SHIELD, return_slot, PLAYER_TEMP_SKILL_WEAPON_CHANGE_FRAMES) == false) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
		return true;
	}

	// 군인 E: Impact grenade는 일반 스킬 쿨타임을 쓰지 않고,
	// 기존 1/2번 무기를 유지한 채 임시 장착 상태만 켠다.
	if (skilltype == PLAYER_SKILL_IMPACT_GRENADE) {

		// 이미 Impact grenade를 들고 있으면 E 재입력으로 기존 무기로 복귀한다.
		if ((IsPlayerTemporarySkillWeaponReady() == true) &&
			(GetPlayerTemporarySkillWeaponID() == ID_WEAPON_IMPACT_GRENADE)) {
			StartPlayerTemporarySkillWeaponReturn(myHuman, GetPlayerTemporarySkillWeaponReturnSlot());
			return true;
		}

		// Impact grenade 장착/해제 CHANGING 중에는 중복 입력을 무시한다.
		if (IsPlayerTemporarySkillWeaponChanging() == true) {
			return true;
		}

		// 다른 임시 스킬 무기 사용 중에는 Impact grenade로 바로 갈아타지 않는다.
		// 방패/DP28과 같은 입력 차단 정책으로 맞춘다.
		if ((IsPlayerTemporarySkillWeaponActive() == true) &&
			(GetPlayerTemporarySkillWeaponID() != ID_WEAPON_IMPACT_GRENADE)) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		if (GetPlayerTemporarySkillWeaponStockAmmo(ID_WEAPON_IMPACT_GRENADE) <= 0) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		// 재장전 중에는 Impact grenade로 무기를 바꾸지 않는다.
// HUD 빗금 표시와 같은 PlayerSkill_IsBlockedWhileReloading() 기준을 사용한다.
		if ((PlayerSkill_IsBlockedWhileReloading(skilltype) == true) &&
			(IsPlayerCurrentWeaponReloading(myHuman) == true)) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		// 현재 활성 스킬 상태에서 Impact grenade를 사용할 수 있는지 확인한다.
// 지속형 버프는 기본 허용하고, 조작 상태/명시적 충돌 조합만 차단한다.
		if (IsPlayerSkillBlockedByActiveSkill(myHuman, skilltype) == true) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		ClearAimStateForPlayerSkill(myHuman);

		int return_slot = 0;

		// Impact grenade를 들기 전의 실제 선택 슬롯을 저장한다.
		// E 재입력 복귀 시 이 슬롯으로 돌아간다.
		{
			int selectweapon = 0;
			class weapon* slot_weapon[TOTAL_HAVEWEAPON];
			int lnbslist[TOTAL_HAVEWEAPON];
			int nbslist[TOTAL_HAVEWEAPON];

			for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
				slot_weapon[i] = NULL;
				lnbslist[i] = 0;
				nbslist[i] = 0;
			}

			myHuman->GetWeapon(&selectweapon, slot_weapon, lnbslist, nbslist);

			if ((0 <= selectweapon) && (selectweapon < TOTAL_HAVEWEAPON)) {
				return_slot = selectweapon;
			}
			else {
				return_slot = 0;
			}
		}

		if (StartPlayerTemporarySkillWeaponEquip(myHuman, ID_WEAPON_IMPACT_GRENADE, return_slot, 5) == false) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
		return true;
	}

	// 용병 E: DP28 스킬 무기.
// 지속시간/쿨타임 없이 E로 꺼내고, E 재입력으로 기존 무기를 복구한다.
	if (skilltype == PLAYER_SKILL_DP28) {
		// DP28 재장전 중에는 E로 기존 무기로 돌아가지 않는다.
		// 재장전 중 복귀를 허용하면 복구된 기존 무기에 재장전 결과가 섞일 수 있다.
		if (player_skill_dp28_ready == true) {
			if ((PlayerSkill_IsBlockedWhileReloading(skilltype) == true) &&
				(IsPlayerCurrentWeaponReloading(myHuman) == true)) {
				PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
				return true;
			}

			ReturnPlayerDP28SkillWeapon(myHuman);
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
			return true;
		}

		// 일반 무기 재장전 중에는 DP28로 바꾸지 않는다.
// HUD 빗금 표시와 같은 PlayerSkill_IsBlockedWhileReloading() 기준을 사용한다.
		if ((PlayerSkill_IsBlockedWhileReloading(skilltype) == true) &&
			(IsPlayerCurrentWeaponReloading(myHuman) == true)) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		if ((IsPlayerTemporarySkillWeaponActive() == true) ||
			(IsPlayerTemporarySkillWeaponChanging() == true)) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		if (IsPlayerSkillBlockedByActiveSkill(myHuman, skilltype) == true) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		ClearAimStateForPlayerSkill(myHuman);

		if (StartPlayerDP28SkillWeapon(myHuman) == false) {
			PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
			return true;
		}

		PlayPlayerSkillSound(SKILL_SOUND_COMMON_START);
		return true;
	}

	if (skilltype == PLAYER_SKILL_NONE) {
		// 2번 스킬은 아직 실제 스킬을 넣지 않았으므로 조용히 무시한다.
		if (skill_slot == PLAYER_SKILL_SLOT_SECONDARY) {
			return false;
		}

		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	// 재장전 중에는 무기 전환형/설치형/공격 보조형 스킬을 사용할 수 없다.
// HUD 빗금 조건과 실제 입력 차단 조건이 어긋나지 않도록 playerskill.cpp helper를 사용한다.
	if ((PlayerSkill_IsBlockedWhileReloading(skilltype) == true) &&
		(IsPlayerCurrentWeaponReloading(myHuman) == true)) {

		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	// 해당 슬롯이 쿨타임 중이면 사용 불가
	if (player_skill_cooldown_timer[skill_slot] > 0) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	// 현재 활성 스킬 상태에서 이 스킬을 사용할 수 있는지 확인
	if (IsPlayerSkillBlockedByActiveSkill(myHuman, skilltype) == true) {
		PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
		return true;
	}

	// 이후 StartPlayerSkillCooldown()을 호출하는 기존 스킬 함수들이
// 현재 입력된 E/F 슬롯에 쿨타임을 걸 수 있도록 기록한다.
	player_skill_using_slot = NormalizePlayerSkillSlot(skill_slot);

	// 스킬 사용 직전에 스코프/수류탄 궤적을 실패 처리하지 않고 해제한다.
	ClearAimStateForPlayerSkill(myHuman);

	// ----------------------------------------------------------
	// 조준 진입형 스킬
	// 교란 신호 / 지뢰 / 동료 소환 / 갈고리
	// ----------------------------------------------------------
	if (BeginTargetingPlayerSkillIfNeeded(skilltype, skill_slot) == true) {
		return true;
	}

	// ----------------------------------------------------------
	// 단순 버프형 스킬
	// 은신 / 탄약 강화 / 생존 본능 / 전투 흡수
	// ----------------------------------------------------------
	if (ActivateSimpleBuffPlayerSkill(myHuman, skilltype) == true) {
		return true;
	}

	// ----------------------------------------------------------
	// 단발/특수형 스킬
	// 전술 스캔 / 응급 치료 / 마킹 샷 / 로봇 처형 / 제압 명령
	// ----------------------------------------------------------
	if (ActivateSpecialPlayerSkill(myHuman, skilltype) == true) {
		return true;
	}

	// 안전장치:
	// 스킬 타입은 있는데 위 분기에서 처리되지 않은 경우 실패음
	PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
	return true;
}