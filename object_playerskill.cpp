//! @file object_playerskill.cpp
//! @brief human 플레이어 스킬 관련 함수 정의

#include "object.h"
#include "playerskill.h"

// 플레이어 스킬: 은신 시작
void human::StartSkillStealth(int frames)
{
	if (frames < 0) { frames = 0; }
	skill_stealth_timer = frames;
}

// 플레이어 스킬: 은신 타이머 감소
void human::UpdateSkillStealth()
{
	if (skill_stealth_timer > 0) {
		skill_stealth_timer--;
	}
}

// 플레이어 스킬: 은신 중인지 확인
bool human::GetSkillStealthFlag()
{
	return (skill_stealth_timer > 0);
}

// 플레이어 스킬: 남은 은신 시간
int human::GetSkillStealthTimer()
{
	return skill_stealth_timer;
}

// 플레이어 스킬: 군인 탄약 강화 시작
void human::StartSkillAmmoBoost(int frames)
{
	if (frames < 0) { frames = 0; }
	skill_ammoboost_timer = frames;
}

// 플레이어 스킬: 군인 탄약 강화 타이머 감소
void human::UpdateSkillAmmoBoost()
{
	if (skill_ammoboost_timer > 0) {
		skill_ammoboost_timer--;
	}
}

// 플레이어 스킬: 군인 탄약 강화 중인지 확인
bool human::GetSkillAmmoBoostFlag()
{
	return (skill_ammoboost_timer > 0);
}

// 플레이어 스킬: 군인 탄약 강화 남은 시간
int human::GetSkillAmmoBoostTimer()
{
	return skill_ammoboost_timer;
}

// 플레이어 스킬: 시민B 생존 본능 시작
void human::StartSkillSurvival(int frames)
{
	if (frames < 0) { frames = 0; }

	skill_survival_timer = frames;
}

// 플레이어 스킬: 시민B 생존 본능 갱신
void human::UpdateSkillSurvival()
{
	if (hp <= 0) {
		skill_survival_timer = 0;
		return;
	}

	if (skill_survival_timer > 0) {
		skill_survival_timer--;
	}
}

// 플레이어 스킬: 시민B 생존 본능 중인지 확인
bool human::GetSkillSurvivalFlag()
{
	return (skill_survival_timer > 0);
}

// 플레이어 스킬: 시민B 생존 본능 남은 시간
int human::GetSkillSurvivalTimer()
{
	return skill_survival_timer;
}

// 플레이어 스킬: 시민B 긴급 회피 시작
void human::StartSkillDodge(int frames)
{
	if (frames < 0) { frames = 0; }

	skill_dodge_timer = frames;
}

// 플레이어 스킬: 시민B 긴급 회피 갱신
void human::UpdateSkillDodge()
{
	if (hp <= 0) {
		skill_dodge_timer = 0;
		return;
	}

	if (skill_dodge_timer > 0) {
		skill_dodge_timer--;
	}
}

// 플레이어 스킬: 시민B 긴급 회피 무적 중인지 확인
bool human::GetSkillDodgeFlag()
{
	return (skill_dodge_timer > 0);
}

// 플레이어 스킬: 시민B 긴급 회피 남은 무적 시간
int human::GetSkillDodgeTimer()
{
	return skill_dodge_timer;
}

// 플레이어 스킬: 비밀 요원 텔레포트 시전 시작
void human::StartSkillTeleportCast(int frames)
{
	if (frames < 0) { frames = 0; }

	skill_teleport_cast_timer = frames;
}

// 플레이어 스킬: 비밀 요원 텔레포트 시전 갱신
void human::UpdateSkillTeleportCast()
{
	if (hp <= 0) {
		skill_teleport_cast_timer = 0;
		return;
	}

	if (skill_teleport_cast_timer > 0) {
		skill_teleport_cast_timer--;
	}
}

// 플레이어 스킬: 비밀 요원 텔레포트 시전 중인지 확인
bool human::GetSkillTeleportCastFlag()
{
	return (skill_teleport_cast_timer > 0);
}

// 플레이어 스킬: 비밀 요원 텔레포트 시전 남은 시간
int human::GetSkillTeleportCastTimer()
{
	return skill_teleport_cast_timer;
}

// 플레이어 스킬: 시민A 철괴 시작
void human::StartSkillIronBody(int frames)
{
	if (frames < 0) { frames = 0; }

	skill_ironbody_timer = frames;
}

// 플레이어 스킬: 시민A 철괴 갱신
void human::UpdateSkillIronBody()
{
	if (hp <= 0) {
		skill_ironbody_timer = 0;
		return;
	}

	if (skill_ironbody_timer > 0) {
		skill_ironbody_timer--;
	}
}

// 플레이어 스킬: 시민A 철괴 중인지 확인
bool human::GetSkillIronBodyFlag()
{
	return (skill_ironbody_timer > 0);
}

// 플레이어 스킬: 시민A 철괴 남은 시간
int human::GetSkillIronBodyTimer()
{
	return skill_ironbody_timer;
}

// 플레이어 스킬: 전투 흡수 시작
void human::StartSkillAbsorb(int frames)
{
	if (frames < 0) { frames = 0; }

	skill_absorb_timer = frames;
}

// 플레이어 스킬: 전투 흡수 갱신
void human::UpdateSkillAbsorb()
{
	if (skill_absorb_timer > 0) {
		skill_absorb_timer--;
	}
}

// 플레이어 스킬: 전투 흡수 중인지 확인
bool human::GetSkillAbsorbFlag()
{
	return (skill_absorb_timer > 0);
}

// 플레이어 스킬: 전투 흡수 남은 시간
int human::GetSkillAbsorbTimer()
{
	return skill_absorb_timer;
}

// 전투 흡수 즉시 회복
void human::AddSkillAbsorbHeal(int value)
{
	if (value <= 0) { return; }

	// 플레이어가 전투 흡수 중일 때만 회복한다.
	if ((isPlayer == false) || (GetSkillAbsorbFlag() == false)) {
		return;
	}

	// HealHP 내부에서 사망 상태 / 최대 체력 초과를 막는다.
	HealHP(value);
}

// 플레이어 방어형 스킬 피해 처리
int human::ApplySkillAbsorbDamage(int damage)
{
	if (damage <= 0) { return 0; }

	if (isPlayer == true) {
		// 비밀 요원: 텔레포트 시전
		// 0.5초 시전 중에는 피해를 받지 않는다.
		if (GetSkillTeleportCastFlag() == true) {
			return 0;
		}

		// 시민B: 긴급 회피
		// 짧은 무적 시간 동안 피해를 받지 않는다.
		if (GetSkillDodgeFlag() == true) {
			return 0;
		}

		// 용병: 전투 흡수
		// 받는 피해 50% 감소
		if (GetSkillAbsorbFlag() == true) {
			int reduced_damage = (int)((float)damage * 0.50f + 0.5f);

			if (reduced_damage < 1) {
				reduced_damage = 1;
			}

			return reduced_damage;
		}

		// 로봇: 처형 프로토콜
		// 스킬 유지 중 받는 피해 50% 감소
		if (GetSkillRobotExecuteFlag() == true) {
			int reduced_damage = (int)((float)damage * 0.50f + 0.5f);

			if (reduced_damage < 1) {
				reduced_damage = 1;
			}

			return reduced_damage;
		}

		// 시민A: 철괴
		// 받는 피해 99% 감소 = 실제 피해 1%
		if (GetSkillIronBodyFlag() == true) {
			int reduced_damage = (int)((float)damage * 0.01f + 0.5f);

			if (reduced_damage < 1) {
				reduced_damage = 1;
			}

			return reduced_damage;
		}

		// 시민B: 생존 본능
// 받는 피해 30% 감소 = 실제 피해 70%
		if (GetSkillSurvivalFlag() == true) {
			int reduced_damage = (int)((float)damage * 0.70f + 0.5f);

			if (reduced_damage < 1) {
				reduced_damage = 1;
			}

			return reduced_damage;
		}

		// 좀비: 격노
		// 받는 피해 50% 증가 = 실제 피해 150%
		if (GetSkillRageFlag() == true) {
			int increased_damage = (int)((float)damage * PLAYER_SKILL_RAGE_DAMAGE_TAKEN_MULT + 0.5f);

			if (increased_damage < 1) {
				increased_damage = 1;
			}

			return increased_damage;
		}
	}

	return damage;
}

// 로봇: 처형 프로토콜 시작
void human::StartSkillRobotExecute(int frames)
{
	if (frames < 0) { frames = 0; }
	skill_robot_execute_timer = frames;
}

// 로봇: 처형 프로토콜 강제 종료
void human::StopSkillRobotExecute()
{
	skill_robot_execute_timer = 0;
}

// 로봇: 처형 프로토콜 갱신
void human::UpdateSkillRobotExecute()
{
	if (hp <= 0) {
		skill_robot_execute_timer = 0;
		return;
	}

	if (skill_robot_execute_timer > 0) {
		skill_robot_execute_timer--;
	}
}

// 로봇: 처형 프로토콜 중인지 확인
bool human::GetSkillRobotExecuteFlag()
{
	return (skill_robot_execute_timer > 0);
}

// 로봇: 처형 프로토콜 남은 시간
int human::GetSkillRobotExecuteTimer()
{
	return skill_robot_execute_timer;
}

// 경찰 제압 명령: 제압 상태 시작
void human::StartSkillSuppressed(int frames)
{
	if (frames < 0) { frames = 0; }
	skill_suppressed_timer = frames;
}

// 경찰 제압 명령: 제압 상태 갱신
void human::UpdateSkillSuppressed()
{
	if (skill_suppressed_timer > 0) {
		skill_suppressed_timer--;
	}
}

// 경찰 제압 명령: 제압 상태 여부
bool human::GetSkillSuppressedFlag()
{
	return (skill_suppressed_timer > 0);
}

// 경찰 제압 명령: 제압 상태 남은 시간
int human::GetSkillSuppressedTimer()
{
	return skill_suppressed_timer;
}

// 충격 수류탄 임시 장착 해제용: 같은 슬롯으로 돌아가도 기존 무기 전환처럼 손 올림 모션을 강제로 발생시킨다.
void human::ForceSelectWeaponSlotForSkillReturn(int id, int frames)
{
	if (hp <= 0) { return; }

	if ((id < 0) || (TOTAL_HAVEWEAPON <= id)) {
		return;
	}

	if (frames < 1) {
		frames = 1;
	}

	selectweapon = id;
	SetDisableScope();

	int id_param = ID_WEAPON_NONE;
	class weapon* nowweapon = weapon[selectweapon];

	if (nowweapon != NULL) {
		nowweapon->GetParamData(&id_param, NULL, NULL);
	}

	if (MotionCtrl != NULL) {
		MotionCtrl->ChangeHaveWeapon(id_param);
	}

	selectweaponcnt = frames;

	weaponburstmodecnt = 0;
	weaponshotcnt = 0;
	weaponshotframe = false;
	shell_reload_state = SHELL_RELOAD_NONE;

	NightVision = false;
}

// 경찰 제압 명령: 현재 제압으로 떨어뜨릴 무기를 가지고 있는지 확인한다.
// HUD 판정용이며, 실제 무기 상태를 바꾸지 않는다.
bool human::HasAnyWeaponForSuppress()
{
	// 경찰 E 방패 스킬처럼 렌더링 override로 표시되는 방패도 제압 대상으로 본다.
	if (GetRenderOverrideWeaponID() == ID_WEAPON_SHIELD) {
		return true;
	}

	for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
		if (weapon[i] != NULL) {
			return true;
		}
	}

	return false;
}

// 경찰 제압 명령: 현재 가진 모든 무기를 강제로 버린다.
bool human::ForceDropAllWeapons()
{
	bool dropped = false;

	// 경찰 E 방패 스킬처럼 실제 슬롯 무기가 아니라 렌더링 override로 표시되는 방패도 제압한다.
	if (GetRenderOverrideWeaponID() == ID_WEAPON_SHIELD) {
		SetRenderOverrideWeaponID(-1);
		dropped = true;
	}

	for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
		if (weapon[i] == NULL) { continue; }

		int weapon_id = ID_WEAPON_NONE;
		weapon[i]->GetParamData(&weapon_id, NULL, NULL);

		// 용병 고유 무기 DP28은 강제 무기 버리기에서도 제외한다.
		if (weapon_id == ID_WEAPON_DP28) {
			continue;
		}

		weapon[i]->Dropoff(pos_x, pos_y, pos_z, DegreeToRadian(10) * GetRand(36), 1.5f);
		weapon[i] = NULL;

		noweapon_bullets[i] = 0;
		noweapon_loadbullets[i] = 0;

		dropped = true;
	}

	if (dropped == true) {
		SetDisableScope();

		selectweaponcnt = 0;
		changeweaponidcnt = 0;
		changeweaponid_target = -1;
		changeweaponid_total_time = 0;
		weaponshotcnt = 0;
		weaponreloadcnt = 0;
		weaponshotframe = false;
		weaponburstmodecnt = 0;
		boltActionTimer = 0;
		boltActionTotalTime = 0;
		shell_reload_state = SHELL_RELOAD_NONE;
		shell_reload_timer = 0;

		NightVision = false;
		CustomZoom = 1.0f;
	}

	return dropped;
}


// 로봇 F: 에임핵 중 이동/반응 탄퍼짐 제거 플래그
void human::SetSkillAimHackAccuracyFlag(bool flag)
{
	skill_aimhack_accuracy_flag = flag;
}

bool human::GetSkillAimHackAccuracyFlag()
{
	return skill_aimhack_accuracy_flag;
}

// 좀비: 격노 시작
void human::StartSkillRage(int frames)
{
	if (frames < 0) { frames = 0; }

	skill_rage_timer = frames;
}

// 좀비: 격노 갱신
void human::UpdateSkillRage()
{
	if (hp <= 0) {
		skill_rage_timer = 0;
		return;
	}

	if (skill_rage_timer > 0) {
		skill_rage_timer--;
	}
}

// 좀비: 격노 중인지 확인
bool human::GetSkillRageFlag()
{
	return (skill_rage_timer > 0);
}

// 좀비: 격노 남은 시간
int human::GetSkillRageTimer()
{
	return skill_rage_timer;
}

// 플레이어 스킬: 동료 소환으로 생성된 인물인지 설정
void human::SetSkillSummonedAllyFlag(bool flag)
{
	skill_summoned_ally_flag = flag;
}

// 플레이어 스킬: 동료 소환으로 생성된 인물인지 확인
bool human::GetSkillSummonedAllyFlag()
{
	return skill_summoned_ally_flag;
}

void human::SetSkillShadowDecoyFlag(bool flag)
{
	skill_shadow_decoy_flag = flag;

	if (flag == false) {
		skill_shadow_decoy_last_attacker_id = -1;
	}
}

bool human::GetSkillShadowDecoyFlag()
{
	return skill_shadow_decoy_flag;
}

void human::SetSkillShadowDecoyLastAttackerID(int id)
{
	skill_shadow_decoy_last_attacker_id = id;
}

int human::GetSkillShadowDecoyLastAttackerID()
{
	return skill_shadow_decoy_last_attacker_id;
}

void human::ClearSkillShadowDecoyLastAttackerID()
{
	skill_shadow_decoy_last_attacker_id = -1;
}