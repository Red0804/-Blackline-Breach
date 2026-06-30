#include "playerskill.h"
#include "resource.h"
#include "object.h"

int PlayerSkill_GetGroupByHuman(human* myHuman)
{
	if (myHuman == NULL) { return PLAYER_SKILL_GROUP_NONE; }

	int human_id = -1;
	myHuman->GetParamData(&human_id, NULL, NULL, NULL);

	// 비밀 요원
	if ((human_id == 0) || (human_id == 1) || (human_id == 8)) {
		return PLAYER_SKILL_GROUP_SECRET_AGENT;
	}

	// 군인
	if ((human_id == 26) || (human_id == 27) || (human_id == 28)) {
		return PLAYER_SKILL_GROUP_SOLDIER;
	}

	// 대장
	if ((human_id == 7) || (human_id == 35) || (human_id == 41) || (human_id == 42)) {
		return PLAYER_SKILL_GROUP_CAPTAIN;
	}

	// 시민A
	if ((human_id == 13) || (human_id == 18) || (human_id == 20) || (human_id == 21)) {
		return PLAYER_SKILL_GROUP_CIVILIAN_A;
	}

	// 시민B
	if ((human_id == 15) || (human_id == 16) || (human_id == 19) || (human_id == 33) || (human_id == 36)) {
		return PLAYER_SKILL_GROUP_CIVILIAN_B;
	}

	// 용병
	if ((human_id == 5) || (human_id == 6) || (human_id == 39) || (human_id == 40)) {
		return PLAYER_SKILL_GROUP_MERCENARY;
	}

	// 요원A
	if ((human_id == 11) || (human_id == 12) || (human_id == 24) || (human_id == 25)) {
		return PLAYER_SKILL_GROUP_AGENT_A;
	}

	// 로봇
	if (human_id == 10) {
		return PLAYER_SKILL_GROUP_ROBOT;
	}

	// 테러리스트
	if ((human_id == 2) || (human_id == 3) || (human_id == 4) || (human_id == 17)) {
		return PLAYER_SKILL_GROUP_TERRORIST;
	}

	// 요원B
	if ((human_id == 22) || (human_id == 23) || (human_id == 34) || (human_id == 37) || (human_id == 38)) {
		return PLAYER_SKILL_GROUP_AGENT_B;
	}

	// 경찰
	if ((human_id == 9) || (human_id == 14)) {
		return PLAYER_SKILL_GROUP_POLICE;
	}

	// 좀비
	if ((human_id == 29) || (human_id == 30) || (human_id == 31) || (human_id == 32)) {
		return PLAYER_SKILL_GROUP_ZOMBIE;
	}

	return PLAYER_SKILL_GROUP_NONE;
}

static int PlayerSkill_GetPrimarySkillByGroup(int group)
{
	switch (group) {
	case PLAYER_SKILL_GROUP_SECRET_AGENT:
		return PLAYER_SKILL_TELEPORT;

	case PLAYER_SKILL_GROUP_SOLDIER:
		return PLAYER_SKILL_IMPACT_GRENADE;

	case PLAYER_SKILL_GROUP_CAPTAIN:
		return PLAYER_SKILL_TACTICAL_SCAN;

	case PLAYER_SKILL_GROUP_CIVILIAN_A:
		return PLAYER_SKILL_HEAL;

	case PLAYER_SKILL_GROUP_CIVILIAN_B:
		return PLAYER_SKILL_DODGE;

	case PLAYER_SKILL_GROUP_MERCENARY:
		return PLAYER_SKILL_DP28;

	case PLAYER_SKILL_GROUP_AGENT_A:
		return PLAYER_SKILL_MARKSHOT;

	case PLAYER_SKILL_GROUP_ROBOT:
		return PLAYER_SKILL_ROBOT_EXECUTE;

	case PLAYER_SKILL_GROUP_TERRORIST:
		return PLAYER_SKILL_DISTRACTION;

	case PLAYER_SKILL_GROUP_AGENT_B:
		return PLAYER_SKILL_SUMMON;

	case PLAYER_SKILL_GROUP_POLICE:
		return PLAYER_SKILL_SHIELD;

	case PLAYER_SKILL_GROUP_ZOMBIE:
		return PLAYER_SKILL_HOOK;
	}

	return PLAYER_SKILL_NONE;
}

static int PlayerSkill_GetSecondarySkillByGroup(int group)
{
	switch (group) {
	case PLAYER_SKILL_GROUP_SECRET_AGENT:
		return PLAYER_SKILL_STEALTH;

	case PLAYER_SKILL_GROUP_CAPTAIN:
		return PLAYER_SKILL_ARTILLERY_STRIKE;

	case PLAYER_SKILL_GROUP_SOLDIER:
		return PLAYER_SKILL_AMMOBOOST;

	case PLAYER_SKILL_GROUP_CIVILIAN_A:
		return PLAYER_SKILL_IRONBODY;

	case PLAYER_SKILL_GROUP_CIVILIAN_B:
		return PLAYER_SKILL_SURVIVAL;

	case PLAYER_SKILL_GROUP_MERCENARY:
		return PLAYER_SKILL_ABSORB;

	case PLAYER_SKILL_GROUP_POLICE:
		return PLAYER_SKILL_SUPPRESS;

	case PLAYER_SKILL_GROUP_AGENT_A:
		return PLAYER_SKILL_SHADOW_DECOY;

	case PLAYER_SKILL_GROUP_TERRORIST:
		return PLAYER_SKILL_TERRORIST_MINE;

	case PLAYER_SKILL_GROUP_ROBOT:
		return PLAYER_SKILL_AIMHACK;

	case PLAYER_SKILL_GROUP_AGENT_B:
		return PLAYER_SKILL_BIOTIC_FIELD;

	case PLAYER_SKILL_GROUP_ZOMBIE:
		return PLAYER_SKILL_RAGE;
	}

	return PLAYER_SKILL_NONE;
}

int PlayerSkill_GetTypeByGroup(int group, int slot)
{
	if (slot == PLAYER_SKILL_SLOT_PRIMARY) {
		return PlayerSkill_GetPrimarySkillByGroup(group);
	}

	if (slot == PLAYER_SKILL_SLOT_SECONDARY) {
		return PlayerSkill_GetSecondarySkillByGroup(group);
	}

	return PLAYER_SKILL_NONE;
}
int PlayerSkill_GetTypeByHuman(human* myHuman)
{
	int group = PlayerSkill_GetGroupByHuman(myHuman);

	return PlayerSkill_GetTypeByGroup(group, PLAYER_SKILL_SLOT_PRIMARY);
}

bool PlayerSkill_HasSkill(human* myHuman)
{
	int group = PlayerSkill_GetGroupByHuman(myHuman);

	if (PlayerSkill_GetTypeByGroup(group, PLAYER_SKILL_SLOT_PRIMARY) != PLAYER_SKILL_NONE) {
		return true;
	}

	if (PlayerSkill_GetTypeByGroup(group, PLAYER_SKILL_SLOT_SECONDARY) != PLAYER_SKILL_NONE) {
		return true;
	}

	return false;
}

int PlayerSkill_GetCooldownFrames(int skilltype)
{
	if (skilltype == PLAYER_SKILL_STEALTH) {
		return PLAYER_SKILL_STEALTH_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_AMMOBOOST) {
		return PLAYER_SKILL_AMMOBOOST_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_TACTICAL_SCAN) {
		return PLAYER_SKILL_SCAN_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) {
		return PLAYER_SKILL_ARTILLERY_STRIKE_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_HEAL) {
		return PLAYER_SKILL_HEAL_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_IRONBODY) {
		return PLAYER_SKILL_IRONBODY_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_SURVIVAL) {
		return PLAYER_SKILL_SURVIVAL_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_DODGE) {
		return PLAYER_SKILL_DODGE_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_ABSORB) {
		return PLAYER_SKILL_ABSORB_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_MARKSHOT) {
		return PLAYER_SKILL_MARKSHOT_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_ROBOT_EXECUTE) {
		return PLAYER_SKILL_ROBOT_EXECUTE_CANCEL_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_AIMHACK) {
		return PLAYER_SKILL_AIMHACK_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_DISTRACTION) {
		return PLAYER_SKILL_DISTRACTION_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_TERRORIST_MINE) {
		return PLAYER_SKILL_TERRORIST_MINE_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_BIOTIC_FIELD) {
		return PLAYER_SKILL_BIOTIC_FIELD_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) {
		return PLAYER_SKILL_ARTILLERY_STRIKE_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_SUMMON) {
		return PLAYER_SKILL_SUMMON_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_SHADOW_DECOY) {
		return PLAYER_SKILL_SHADOW_DECOY_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_SUPPRESS) {
		return PLAYER_SKILL_SUPPRESS_COOLDOWN_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_HOOK) {
		return PLAYER_SKILL_HOOK_COOLDOWN_FRAMES;
	}

	if (skilltype == PLAYER_SKILL_RAGE) {
		return PLAYER_SKILL_RAGE_COOLDOWN_FRAMES;
	}

	if (skilltype == PLAYER_SKILL_TELEPORT) {
		return PLAYER_SKILL_TELEPORT_COOLDOWN_FRAMES;
	}

	if (skilltype == PLAYER_SKILL_IMPACT_GRENADE) {
		return 1;
	}

	if (skilltype == PLAYER_SKILL_SHIELD) {
		return 1;
	}

	if (skilltype == PLAYER_SKILL_DP28) {
		return 1;
	}

	return 1;
}

int PlayerSkill_GetDurationFrames(int skilltype)
{
	if (skilltype == PLAYER_SKILL_STEALTH) {
		return PLAYER_SKILL_STEALTH_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_AMMOBOOST) {
		return PLAYER_SKILL_AMMOBOOST_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_IRONBODY) {
		return PLAYER_SKILL_IRONBODY_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_SURVIVAL) {
		return PLAYER_SKILL_SURVIVAL_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_DODGE) {
		return PLAYER_SKILL_DODGE_INVINCIBLE_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_ABSORB) {
		return PLAYER_SKILL_ABSORB_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_MARKSHOT) {
		return PLAYER_SKILL_MARKSHOT_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_DISTRACTION) {
		return PLAYER_SKILL_DISTRACTION_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_BIOTIC_FIELD) {
		return PLAYER_SKILL_BIOTIC_FIELD_FRAMES;
	}
	if (skilltype == PLAYER_SKILL_SUPPRESS) {
		return PLAYER_SKILL_SUPPRESS_MARK_FRAMES;
	}

	if (skilltype == PLAYER_SKILL_SHADOW_DECOY) {
		return PLAYER_SKILL_SHADOW_DECOY_FRAMES;
	}

	if (skilltype == PLAYER_SKILL_AIMHACK) {
		return PLAYER_SKILL_AIMHACK_FRAMES;
	}

	if (skilltype == PLAYER_SKILL_RAGE) {
		return PLAYER_SKILL_RAGE_FRAMES;
	}

	if (skilltype == PLAYER_SKILL_TELEPORT) {
		return PLAYER_SKILL_TELEPORT_CAST_FRAMES;
	}

	return 0;
}

float PlayerSkill_GetTargetingMaxDist(int skilltype)
{
	if (skilltype == PLAYER_SKILL_DISTRACTION) {
		return PLAYER_SKILL_DISTRACTION_AIM_DIST;
	}

	if (skilltype == PLAYER_SKILL_TERRORIST_MINE) {
		return PLAYER_SKILL_TERRORIST_MINE_AIM_DIST;
	}

	if (skilltype == PLAYER_SKILL_SUMMON) {
		return PLAYER_SKILL_SUMMON_AIM_DIST;
	}

	if (skilltype == PLAYER_SKILL_SHADOW_DECOY) {
		return PLAYER_SKILL_SHADOW_DECOY_AIM_DIST;
	}

	if (skilltype == PLAYER_SKILL_BIOTIC_FIELD) {
		return PLAYER_SKILL_BIOTIC_FIELD_AIM_DIST;
	}

	if (skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) {
		return PLAYER_SKILL_ARTILLERY_STRIKE_AIM_DIST;
	}

	if (skilltype == PLAYER_SKILL_HOOK) {
		return PLAYER_SKILL_HOOK_MAX_DIST;
	}

	if (skilltype == PLAYER_SKILL_TELEPORT) {
		return PLAYER_SKILL_TELEPORT_AIM_DIST;
	}

	return 0.0f;
}

bool PlayerSkill_IsRayTargetingSkill(int skilltype)
{
	if (skilltype == PLAYER_SKILL_HOOK) {
		return true;
	}

	return false;
}

bool PlayerSkill_IsGroundTargetingSkill(int skilltype)
{
	if (skilltype == PLAYER_SKILL_DISTRACTION) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_TERRORIST_MINE) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_SUMMON) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_SHADOW_DECOY) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_BIOTIC_FIELD) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_TELEPORT) {
		return true;
	}

	return false;
}

bool PlayerSkill_IsTargetingSkill(int skilltype)
{
	if (PlayerSkill_IsGroundTargetingSkill(skilltype) == true) {
		return true;
	}

	if (PlayerSkill_IsRayTargetingSkill(skilltype) == true) {
		return true;
	}

	return false;
}

bool PlayerSkill_IsSimpleBuffSkill(int skilltype)
{
	if (skilltype == PLAYER_SKILL_STEALTH) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_AMMOBOOST) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_SURVIVAL) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_IRONBODY) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_ABSORB) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_RAGE) {
		return true;
	}

	return false;
}

bool PlayerSkill_IsSpecialActiveSkill(int skilltype)
{
	if (skilltype == PLAYER_SKILL_TACTICAL_SCAN) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_HEAL) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_MARKSHOT) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_ROBOT_EXECUTE) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_AIMHACK) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_SUPPRESS) {
		return true;
	}

	if (skilltype == PLAYER_SKILL_DODGE) {
		return true;
	}

	return false;
}

bool PlayerSkill_IsBlockedWhileReloading(int skilltype)
{
	// 무기 전환형 스킬
	if (skilltype == PLAYER_SKILL_IMPACT_GRENADE) { return true; }
	if (skilltype == PLAYER_SKILL_SHIELD) { return true; }
	if (skilltype == PLAYER_SKILL_DP28) { return true; }

	// 설치형 / 위치 지정형 스킬
	if (skilltype == PLAYER_SKILL_DISTRACTION) { return true; }
	if (skilltype == PLAYER_SKILL_TERRORIST_MINE) { return true; }
	if (skilltype == PLAYER_SKILL_SUMMON) { return true; }
	if (skilltype == PLAYER_SKILL_BIOTIC_FIELD) { return true; }
	if (skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) { return true; }
	if (skilltype == PLAYER_SKILL_SHADOW_DECOY) { return true; }

	// 공격 보조형 스킬
	if (skilltype == PLAYER_SKILL_AIMHACK) { return true; }

	return false;
}

int PlayerSkill_GetIconID(int skilltype)
{
	if (skilltype == PLAYER_SKILL_STEALTH) {
		return SKILL_ICON_STEALTH;
	}
	if (skilltype == PLAYER_SKILL_AMMOBOOST) {
		return SKILL_ICON_AMMOBOOST;
	}
	if (skilltype == PLAYER_SKILL_TACTICAL_SCAN) {
		return SKILL_ICON_SCAN;
	}
	if (skilltype == PLAYER_SKILL_HEAL) {
		return SKILL_ICON_HEAL;
	}
	if (skilltype == PLAYER_SKILL_IRONBODY) {
		return SKILL_ICON_IRONBODY;
	}
	if (skilltype == PLAYER_SKILL_SURVIVAL) {
		return SKILL_ICON_SURVIVAL;
	}
	if (skilltype == PLAYER_SKILL_DODGE) {
		return SKILL_ICON_DODGE;
	}
	if (skilltype == PLAYER_SKILL_ABSORB) {
		return SKILL_ICON_ABSORB;
	}
	if (skilltype == PLAYER_SKILL_MARKSHOT) {
		return SKILL_ICON_MARKSHOT;
	}
	if (skilltype == PLAYER_SKILL_ROBOT_EXECUTE) {
		return SKILL_ICON_ROBOT_EXECUTE;
	}
	if (skilltype == PLAYER_SKILL_AIMHACK) {
		return SKILL_ICON_AIMHACK;
	}
	if (skilltype == PLAYER_SKILL_DISTRACTION) {
		return SKILL_ICON_DISTRACTION;
	}
	if (skilltype == PLAYER_SKILL_SUMMON) {
		return SKILL_ICON_SUMMON;
	}
	if (skilltype == PLAYER_SKILL_SUPPRESS) {
		return SKILL_ICON_SUPPRESS;
	}
	if (skilltype == PLAYER_SKILL_HOOK) {
		return SKILL_ICON_HOOK;
	}
	if (skilltype == PLAYER_SKILL_RAGE) {
		return SKILL_ICON_RAGE;
	}
	if (skilltype == PLAYER_SKILL_TELEPORT) {
		return SKILL_ICON_TELEPORT;
	}
	if (skilltype == PLAYER_SKILL_IMPACT_GRENADE) {
		return SKILL_ICON_IMPACT_GRENADE;
	}

	if (skilltype == PLAYER_SKILL_SHIELD) {
		return SKILL_ICON_SHIELD;
	}

	if (skilltype == PLAYER_SKILL_DP28) {
		return SKILL_ICON_DP28;
	}

	if (skilltype == PLAYER_SKILL_TERRORIST_MINE) {
		return SKILL_ICON_TERRORIST_MINE;
	}

	if (skilltype == PLAYER_SKILL_BIOTIC_FIELD) {
		return SKILL_ICON_BIOTIC_FIELD;
	}

	if (skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) {
		return SKILL_ICON_ARTILLERY_STRIKE;
	}

	if (skilltype == PLAYER_SKILL_SHADOW_DECOY) {
		return SKILL_ICON_SHADOW_DECOY;
	}

	return -1;
}

const char* PlayerSkill_GetFallbackIconText(int skilltype)
{
	if (skilltype == PLAYER_SKILL_STEALTH) { return "S"; }
	if (skilltype == PLAYER_SKILL_AMMOBOOST) { return "A"; }
	if (skilltype == PLAYER_SKILL_TACTICAL_SCAN) { return "R"; }
	if (skilltype == PLAYER_SKILL_HEAL) { return "H"; }
	if (skilltype == PLAYER_SKILL_SURVIVAL) { return "V"; }
	if (skilltype == PLAYER_SKILL_DODGE) { return "D"; }
	if (skilltype == PLAYER_SKILL_IRONBODY) { return "I"; }
	if (skilltype == PLAYER_SKILL_ABSORB) { return "B"; }
	if (skilltype == PLAYER_SKILL_MARKSHOT) { return "M"; }
	if (skilltype == PLAYER_SKILL_ROBOT_EXECUTE) { return "X"; }
	if (skilltype == PLAYER_SKILL_AIMHACK) { return "A"; }
	if (skilltype == PLAYER_SKILL_DISTRACTION) { return "D"; }
	if (skilltype == PLAYER_SKILL_SUMMON) { return "Z"; }
	if (skilltype == PLAYER_SKILL_SUPPRESS) { return "P"; }
	if (skilltype == PLAYER_SKILL_HOOK) { return "G"; }
	if (skilltype == PLAYER_SKILL_RAGE) { return "R"; }
	if (skilltype == PLAYER_SKILL_TELEPORT) { return "T"; }
	if (skilltype == PLAYER_SKILL_IMPACT_GRENADE) { return "I"; }
	if (skilltype == PLAYER_SKILL_SHIELD) { return "S"; }
	if (skilltype == PLAYER_SKILL_DP28) { return "D"; }
	if (skilltype == PLAYER_SKILL_TERRORIST_MINE) { return "M"; }
	if (skilltype == PLAYER_SKILL_BIOTIC_FIELD) { return "F"; }
	if (skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) { return "A"; }
	if (skilltype == PLAYER_SKILL_SHADOW_DECOY) { return "S"; }

	return "S";
}