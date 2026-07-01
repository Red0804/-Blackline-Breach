//! @file objectmanager.cpp
//! @brief ObjectManager긏깋긚궻믦?

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

#include "objectmanager.h"
#include "playerskill.h"
#include <algorithm>

// ------------------------------------------------------------
// Crouch weapon/camera offset policy
// 앉기 상태에서 총알/총구화염/탄피 위치를 한 번에 조절하기 위한 값
// 값이 너무 낮으면 총알/탄피가 아래에서 나오고,
// 값이 너무 높으면 서 있을 때 위치에 가까워진다.
// ------------------------------------------------------------

static const float CROUCH_WEAPON_Y_OFFSET = -2.0f;

static float GetCrouchWeaponYOffset(bool crouchflag)
{
	if (crouchflag == true) {
		return CROUCH_WEAPON_Y_OFFSET;
	}
	return 0.0f;
}

static float GetHumanShotY(float pos_y, bool crouchflag)
{
	return pos_y + WEAPONSHOT_HEIGHT + GetCrouchWeaponYOffset(crouchflag);
}

static float GetHumanWeaponBaseY(float pos_y, bool crouchflag)
{
	return pos_y + 16.0f + GetCrouchWeaponYOffset(crouchflag);
}

// ------------------------------------------------------------
// Shield bullet defense policy
// 방패 1단계 방어:
// 실제 모델/박스 충돌이 아니라, 방패 장착 상태 + 정면 각도 기준으로 피해만 줄인다.
// 이후 단계에서 방패 모델 기준 충돌 판정으로 교체/보강할 수 있다.
// ------------------------------------------------------------
static const float SHIELD_BLOCK_HALF_ANGLE = DegreeToRadian(70.0f);
static const int SHIELD_PENETRATION_MAX = 10;

// 방패 관통 피해율.
// 기존 Weapon[].penetration은 맵 벽/오브젝트/사람 관통에도 쓰이므로 직접 바꾸지 않는다.
// 아래 수치는 방패를 통과해서 신체에 들어가는 피해율에만 사용한다.
static const float SHIELD_DAMAGE_RATE_PEN0 = 0.01f;		// 관통력 0: 0~1% 피해
static const float SHIELD_DAMAGE_RATE_PEN1 = 0.10f;		// 관통력 1: 8~12% 피해
static const float SHIELD_DAMAGE_RATE_PEN2 = 0.2f;		// 관통력 2: 15~20% 피해
static const float SHIELD_DAMAGE_RATE_PEN3 = 0.3f;		// 관통력 3: 25~30% 피해
static const float SHIELD_DAMAGE_RATE_PEN4 = 0.4f;		// 관통력 4: 35~40% 피해
static const float SHIELD_DAMAGE_RATE_PEN5 = 0.5f;		// 관통력 5 이상: 45~50% 피해

// 방패에 맞은 뒤 총알이 계속 진행할지 결정하는 값.
// 관통력 0 총알은 방패에서 완전히 멈추고,
// 관통력 1 이상 총알은 약화된 상태로 통과할 수 있다.
static const int SHIELD_BULLET_STOP_WEAPON_PENETRATION_MAX = 0;
static const int SHIELD_BULLET_PENETRATION_COST = 2;
// 기존 Weapon[].penetration은 맵 벽/오브젝트/사람 관통에도 쓰이므로 직접 바꾸지 않는다.
// 방패에 대해서만 별도 관통력으로 변환해서 사용한다.
static int GetShieldPenetrationFromWeaponPenetration(int weapon_penetration)
{
	if (weapon_penetration <= 0) {
		return 0;
	}

	// penetration 1 무기도 방패에는 약하게 관통 가능하게 한다.
	// 맵 벽 관통 수치는 바뀌지 않는다.
	if (weapon_penetration == 1) {
		return 2;
	}

	if (weapon_penetration == 2) {
		return 4;
	}

	if (weapon_penetration == 3) {
		return 5;
	}

	int shield_penetration = weapon_penetration + 2;

	if (shield_penetration > SHIELD_PENETRATION_MAX) {
		shield_penetration = SHIELD_PENETRATION_MAX;
	}

	return shield_penetration;
}

static float GetShieldDamageRateFromWeaponPenetration(int weapon_penetration)
{
	if (weapon_penetration <= 0) {
		return SHIELD_DAMAGE_RATE_PEN0;
	}

	if (weapon_penetration == 1) {
		return SHIELD_DAMAGE_RATE_PEN1;
	}

	if (weapon_penetration == 2) {
		return SHIELD_DAMAGE_RATE_PEN2;
	}

	if (weapon_penetration == 3) {
		return SHIELD_DAMAGE_RATE_PEN3;
	}

	if (weapon_penetration == 4) {
		return SHIELD_DAMAGE_RATE_PEN4;
	}

	return SHIELD_DAMAGE_RATE_PEN5;
}

// 방패 임시 판정 박스.
// 아직 실제 모델 충돌은 아니며, 사람 기준 좌우/높이/앞뒤 범위만 제한한다.
// 모델 기준 판정으로 넘어가면 이 상수들은 제거하거나 디버그용으로만 남기면 된다.
static const float SHIELD_APPROX_HALF_WIDTH = 4.6f;		// 방패 좌우 반폭. 가장자리 판정 보강
static const float SHIELD_APPROX_BOTTOM_Y = 5.8f;		// 발 기준 방패 하단. 하단 가장자리 약간 보강
static const float SHIELD_APPROX_TOP_Y = 23.6f;			// 발 기준 방패 상단. 상단 가장자리 약간 보강
static const float SHIELD_APPROX_FRONT_MIN = 3.0f;		// 사람 중심 뒤쪽까지 막히는 현상을 줄임
static const float SHIELD_APPROX_FRONT_MAX = 4.4f;		// 사람 중심 앞쪽 방패 충돌 판정 거리

// 스파크/효과 표시 위치.
// 방패 박스의 실제 앞면(SHIELD_APPROX_FRONT_MAX)보다 아주 살짝 앞에 표시한다.
// 이렇게 하면 디버그 박스의 주황색 앞면과 스파크 위치가 거의 일치한다.
static const float SHIELD_EFFECT_FRONT_OFFSET = 0.15f;

// 방패 모델 앞쪽에 적/좀비가 너무 가까이 붙어서 뚫고 보이는 현상 완화용.
// 실제 메시 충돌이 아니라, 사람끼리 충돌 처리 단계에서 방패 정면에만 약한 거리 보정을 추가한다.
static const bool SHIELD_ENABLE_FRONT_HUMAN_PUSH = true;
static const float SHIELD_HUMAN_PUSH_SIDE_MARGIN = 1.8f;

// 적/좀비의 중심이 방패 앞면에 너무 가까워지기 전에 더 일찍 밀어낸다.
// 기존 0.85f는 얼굴/상체 모델이 방패 안쪽으로 보일 수 있으므로 조금 더 여유를 준다.
static const float SHIELD_HUMAN_PUSH_MIN_FRONT = SHIELD_APPROX_FRONT_MAX + HUMAN_HUMANCOLLISION_R * 1.45f;

static const float SHIELD_HUMAN_PUSH_MAX_Y_DIFF = 6.0f;

// 파고든 적을 더 확실히 밀어낸다.
// 너무 튕기면 0.65f → 0.55f로 낮춘다.
static const float SHIELD_HUMAN_PUSH_RATE = 0.75f;

// 한 프레임에 밀어내는 최대량.
// 너무 떨리거나 튕기면 1.10f → 0.85f로 낮춘다.
static const float SHIELD_HUMAN_PUSH_MAX_PER_FRAME = 1.30f;

// AI 방패 자동 전환 기준.
// AI가 방패로 일정 횟수 이상 막거나, 누적 방어 피해가 일정 이상이면
// 보유 중인 다른 총기로 전환한다.
// 너무 자주 전환 판정이 반복되지 않도록 짧은 쿨다운도 둔다.
static const bool AI_SHIELD_AUTO_SWITCH_TO_GUN = true;
static const int AI_SHIELD_SWITCH_BLOCK_COUNT = 6;
static const int AI_SHIELD_SWITCH_BLOCK_DAMAGE = 100;
static const int AI_SHIELD_SWITCH_COOLDOWN_FRAMES = (int)(2.0f * GAMEFPS);

// 방패 스파크는 월드 좌표에 오래 남으면 방패 이동/밀림과 어긋나 보인다.
// 따라서 짧은 플래시성 이펙트로 처리한다.
static const float SHIELD_EFFECT_SURFACE_OFFSET = 0.10f;
static const float SHIELD_SPARK_OUTWARD_SPEED = 0.010f;
static const float SHIELD_SPARK_SIDE_SPEED = 0.004f;
static const int SHIELD_SPARK_LIFE = 2;
static const int SHIELD_SMOKE_LIFE = 3;

// 방패 박스 선행 판정이 안정화되었으므로,
// 기존 "사람 히트박스에 맞은 뒤 방패 방어" 방식은 기본적으로 끈다.
// 만약 방패 판정이 너무 빡빡하면 true로 바꾸면 이전 방식에 가깝게 복구된다.
static const bool SHIELD_ALLOW_LEGACY_HITBOX_SHIELD_FALLBACK = false;

static bool IsHumanShieldEquippedForBullet(human* h)
{
	if (h == NULL) { return false; }
	if (h->GetHP() <= 0) { return false; }
	if (h->GetDeadFlag() == true) { return false; }

	// E 스킬 임시 방패는 render_override_weapon_id로 표시된다.
	if (h->GetRenderOverrideWeaponID() == ID_WEAPON_SHIELD) {
		return true;
	}

	// F7 등으로 실제 무기 슬롯에 92번 방패가 들어간 경우.
	if (h->GetMainWeaponTypeNO() == ID_WEAPON_SHIELD) {
		return true;
	}

	return false;
}

static bool IsBulletInsideApproxShieldArea(human* hitHuman, float hit_x, float hit_y, float hit_z)
{
	if (hitHuman == NULL) { return false; }

	float hx, hy, hz, hrx;
	hitHuman->GetPosData(&hx, &hy, &hz, &hrx);

	float shield_forward_x = cosf(hrx * -1.0f + (float)M_PI / 2.0f);
	float shield_forward_z = sinf(hrx * -1.0f + (float)M_PI / 2.0f);

	// 좌우 판정을 위한 오른쪽 벡터.
	float shield_right_x = shield_forward_z;
	float shield_right_z = -shield_forward_x;

	float rel_x = hit_x - hx;
	float rel_y = hit_y - hy;
	float rel_z = hit_z - hz;

	float side = rel_x * shield_right_x + rel_z * shield_right_z;
	float front = rel_x * shield_forward_x + rel_z * shield_forward_z;

	if (fabsf(side) > SHIELD_APPROX_HALF_WIDTH) {
		return false;
	}

	if ((rel_y < SHIELD_APPROX_BOTTOM_Y) || (rel_y > SHIELD_APPROX_TOP_Y)) {
		return false;
	}

	if ((front < SHIELD_APPROX_FRONT_MIN) || (front > SHIELD_APPROX_FRONT_MAX)) {
		return false;
	}

	return true;
}

static bool IsBulletBlockedByShieldFront(human* hitHuman, human* shotHuman, float bullet_vx, float bullet_vz, float hit_x, float hit_y, float hit_z)
{
	if (IsHumanShieldEquippedForBullet(hitHuman) == false) {
		return false;
	}

	if (IsBulletInsideApproxShieldArea(hitHuman, hit_x, hit_y, hit_z) == false) {
		return false;
	}

	float hx, hy, hz, hrx;
	hitHuman->GetPosData(&hx, &hy, &hz, &hrx);

	float dir_to_shooter_x = 0.0f;
	float dir_to_shooter_z = 0.0f;

	if ((shotHuman != NULL) &&
		(shotHuman->GetEnableFlag() == true) &&
		(shotHuman->GetDeadFlag() == false)) {
		float sx, sy, sz;
		shotHuman->GetPosData(&sx, &sy, &sz, NULL);

		dir_to_shooter_x = sx - hx;
		dir_to_shooter_z = sz - hz;
	}
	else {
		// 발사자를 알 수 없는 경우에는 총알 진행 방향의 반대쪽을 공격자 방향으로 본다.
		dir_to_shooter_x = -bullet_vx;
		dir_to_shooter_z = -bullet_vz;
	}

	float len = sqrtf(dir_to_shooter_x * dir_to_shooter_x + dir_to_shooter_z * dir_to_shooter_z);
	if (len < 0.001f) {
		return false;
	}

	dir_to_shooter_x /= len;
	dir_to_shooter_z /= len;

	float shield_forward_x = cosf(hrx * -1.0f + (float)M_PI / 2.0f);
	float shield_forward_z = sinf(hrx * -1.0f + (float)M_PI / 2.0f);

	float dot = shield_forward_x * dir_to_shooter_x + shield_forward_z * dir_to_shooter_z;

	return (dot >= cosf(SHIELD_BLOCK_HALF_ANGLE));
}

static void GetShieldBlockSurfacePoint(human* hitHuman, float hit_x, float hit_y, float hit_z, float* out_x, float* out_y, float* out_z)
{
	if ((hitHuman == NULL) || (out_x == NULL) || (out_y == NULL) || (out_z == NULL)) { return; }

	float hx, hy, hz, hrx;
	hitHuman->GetPosData(&hx, &hy, &hz, &hrx);

	float shield_forward_x = cosf(hrx * -1.0f + (float)M_PI / 2.0f);
	float shield_forward_z = sinf(hrx * -1.0f + (float)M_PI / 2.0f);
	float shield_right_x = shield_forward_z;
	float shield_right_z = -shield_forward_x;

	float rel_x = hit_x - hx;
	float rel_y = hit_y - hy;
	float rel_z = hit_z - hz;

	float side = rel_x * shield_right_x + rel_z * shield_right_z;

	if (side < -SHIELD_APPROX_HALF_WIDTH) { side = -SHIELD_APPROX_HALF_WIDTH; }
	if (side > SHIELD_APPROX_HALF_WIDTH) { side = SHIELD_APPROX_HALF_WIDTH; }
	if (rel_y < SHIELD_APPROX_BOTTOM_Y) { rel_y = SHIELD_APPROX_BOTTOM_Y; }
	if (rel_y > SHIELD_APPROX_TOP_Y) { rel_y = SHIELD_APPROX_TOP_Y; }

	float effect_front = SHIELD_APPROX_FRONT_MAX + SHIELD_EFFECT_FRONT_OFFSET;

	*out_x = hx + shield_forward_x * effect_front + shield_right_x * side;
	*out_y = hy + rel_y;
	*out_z = hz + shield_forward_z * effect_front + shield_right_z * side;
}

static bool CheckShieldBulletBoxHit(human* hitHuman, human* shotHuman, float bullet_x, float bullet_y, float bullet_z, float bullet_vx, float bullet_vz, float* out_x, float* out_y, float* out_z)
{
	if (IsBulletBlockedByShieldFront(hitHuman, shotHuman, bullet_vx, bullet_vz, bullet_x, bullet_y, bullet_z) == false) {
		return false;
	}

	GetShieldBlockSurfacePoint(hitHuman, bullet_x, bullet_y, bullet_z, out_x, out_y, out_z);
	return true;
}

static bool ClipShieldSegmentSlab(float start, float end, float min_value, float max_value, float* tmin, float* tmax)
{
	if ((tmin == NULL) || (tmax == NULL)) { return false; }

	float delta = end - start;

	if (fabsf(delta) < 0.0001f) {
		if ((start < min_value) || (start > max_value)) {
			return false;
		}
		return true;
	}

	float t1 = (min_value - start) / delta;
	float t2 = (max_value - start) / delta;

	if (t1 > t2) {
		float temp = t1;
		t1 = t2;
		t2 = temp;
	}

	if (t1 > *tmin) { *tmin = t1; }
	if (t2 < *tmax) { *tmax = t2; }

	if (*tmin > *tmax) {
		return false;
	}

	return true;
}

static bool CheckShieldBulletBoxSegmentHit(
	human* hitHuman,
	human* shotHuman,
	float start_x,
	float start_y,
	float start_z,
	float end_x,
	float end_y,
	float end_z,
	float bullet_vx,
	float bullet_vz,
	float* out_x,
	float* out_y,
	float* out_z
)
{
	if ((hitHuman == NULL) || (out_x == NULL) || (out_y == NULL) || (out_z == NULL)) {
		return false;
	}

	if (IsHumanShieldEquippedForBullet(hitHuman) == false) {
		return false;
	}

	float hx, hy, hz, hrx;
	hitHuman->GetPosData(&hx, &hy, &hz, &hrx);

	// 방패 정면에서 날아온 총알인지 먼저 검사한다.
	float dir_to_shooter_x = 0.0f;
	float dir_to_shooter_z = 0.0f;

	if ((shotHuman != NULL) &&
		(shotHuman->GetEnableFlag() == true) &&
		(shotHuman->GetDeadFlag() == false)) {
		float sx, sy, sz;
		shotHuman->GetPosData(&sx, &sy, &sz, NULL);

		dir_to_shooter_x = sx - hx;
		dir_to_shooter_z = sz - hz;
	}
	else {
		dir_to_shooter_x = -bullet_vx;
		dir_to_shooter_z = -bullet_vz;
	}

	float dir_len = sqrtf(dir_to_shooter_x * dir_to_shooter_x + dir_to_shooter_z * dir_to_shooter_z);
	if (dir_len < 0.001f) {
		return false;
	}

	dir_to_shooter_x /= dir_len;
	dir_to_shooter_z /= dir_len;

	float shield_forward_x = cosf(hrx * -1.0f + (float)M_PI / 2.0f);
	float shield_forward_z = sinf(hrx * -1.0f + (float)M_PI / 2.0f);
	float shield_right_x = shield_forward_z;
	float shield_right_z = -shield_forward_x;

	float front_dot = shield_forward_x * dir_to_shooter_x + shield_forward_z * dir_to_shooter_z;
	if (front_dot < cosf(SHIELD_BLOCK_HALF_ANGLE)) {
		return false;
	}

	// 총알 이동 구간을 방패 로컬 좌표로 변환한다.
	float start_rel_x = start_x - hx;
	float start_rel_y = start_y - hy;
	float start_rel_z = start_z - hz;

	float end_rel_x = end_x - hx;
	float end_rel_y = end_y - hy;
	float end_rel_z = end_z - hz;

	float start_side = start_rel_x * shield_right_x + start_rel_z * shield_right_z;
	float end_side = end_rel_x * shield_right_x + end_rel_z * shield_right_z;

	float start_front = start_rel_x * shield_forward_x + start_rel_z * shield_forward_z;
	float end_front = end_rel_x * shield_forward_x + end_rel_z * shield_forward_z;

	float tmin = 0.0f;
	float tmax = 1.0f;

	if (ClipShieldSegmentSlab(start_side, end_side, -SHIELD_APPROX_HALF_WIDTH, SHIELD_APPROX_HALF_WIDTH, &tmin, &tmax) == false) {
		return false;
	}
	if (ClipShieldSegmentSlab(start_rel_y, end_rel_y, SHIELD_APPROX_BOTTOM_Y, SHIELD_APPROX_TOP_Y, &tmin, &tmax) == false) {
		return false;
	}
	if (ClipShieldSegmentSlab(start_front, end_front, SHIELD_APPROX_FRONT_MIN, SHIELD_APPROX_FRONT_MAX, &tmin, &tmax) == false) {
		return false;
	}

	if ((tmin < 0.0f) || (tmin > 1.0f)) {
		return false;
	}

	*out_x = start_x + (end_x - start_x) * tmin;
	*out_y = start_y + (end_y - start_y) * tmin;
	*out_z = start_z + (end_z - start_z) * tmin;

	return true;
}

static int CalculateShieldDamageFromBulletPenetration(int attacks, int bullet_penetration)
{
	if (attacks <= 0) {
		return attacks;
	}

	float damage_rate = GetShieldDamageRateFromWeaponPenetration(bullet_penetration);

	if (damage_rate < SHIELD_DAMAGE_RATE_PEN0) { damage_rate = SHIELD_DAMAGE_RATE_PEN0; }
	if (damage_rate > 1.0f) { damage_rate = 1.0f; }

	int reduced_attacks = (int)((float)attacks * damage_rate + 0.5f);

	if (reduced_attacks < 1) {
		reduced_attacks = 1;
	}

	if (reduced_attacks > attacks) {
		reduced_attacks = attacks;
	}

	return reduced_attacks;
}

static int ApplyShieldFrontDefenseToBulletDamage(human* hitHuman, human* shotHuman, int attacks, int bullet_penetration, float bullet_vx, float bullet_vz, float hit_x, float hit_y, float hit_z, bool* shield_blocked)
{
	if (shield_blocked != NULL) {
		*shield_blocked = false;
	}

	if (attacks <= 0) { return attacks; }

	if (IsBulletBlockedByShieldFront(hitHuman, shotHuman, bullet_vx, bullet_vz, hit_x, hit_y, hit_z) == false) {
		return attacks;
	}

	if (shield_blocked != NULL) {
		*shield_blocked = true;
	}

	return CalculateShieldDamageFromBulletPenetration(attacks, bullet_penetration);
}

static bool ApplyShieldBlockToBulletState(int original_attacks, int bullet_penetration, int shield_damage, int* out_attacks, int* out_penetration)
{
	if (out_attacks == NULL || out_penetration == NULL) {
		return true;
	}

	if (original_attacks < 0) { original_attacks = 0; }

	int shield_pen = GetShieldPenetrationFromWeaponPenetration(bullet_penetration);

	// 원래 무기 관통력 0은 방패에서 완전히 멈춘다.
	// 관통력 1 이상은 방패 피해율 표에 따라 약화된 상태로 통과할 수 있다.
	if ((bullet_penetration <= SHIELD_BULLET_STOP_WEAPON_PENETRATION_MAX) || (shield_pen <= 0)) {
		*out_attacks = 0;
		*out_penetration = -1;
		return true;
	}

	float pass_damage_rate = GetShieldDamageRateFromWeaponPenetration(bullet_penetration);

	if (pass_damage_rate < SHIELD_DAMAGE_RATE_PEN0) { pass_damage_rate = SHIELD_DAMAGE_RATE_PEN0; }
	if (pass_damage_rate > 1.0f) { pass_damage_rate = 1.0f; }

	int next_attacks = (int)((float)original_attacks * pass_damage_rate + 0.5f);

	// 방패에 이미 들어간 피해보다 뒤로 관통하는 피해가 너무 커지지 않도록 제한한다.
	if ((shield_damage > 0) && (next_attacks > original_attacks - shield_damage)) {
		next_attacks = original_attacks - shield_damage;
	}

	if (next_attacks < 1) {
		next_attacks = 1;
	}

	if (next_attacks > original_attacks) {
		next_attacks = original_attacks;
	}

	*out_attacks = next_attacks;

	// 방패를 통과한 이후의 남은 관통력은 방패 전용 관통력에서 소모한다.
	// parameter.cpp의 원래 무기 관통 수치는 변경하지 않는다.
	*out_penetration = shield_pen - SHIELD_BULLET_PENETRATION_COST;

	if (*out_penetration < 0) {
		*out_penetration = -1;
		return true;
	}

	return false;
}

static int ApplyShieldFallbackDefenseToBulletDamage(human* hitHuman, human* shotHuman, int attacks, int bullet_penetration, float bullet_vx, float bullet_vz, float hit_x, float hit_y, float hit_z, bool* shield_blocked) 
{
	if (shield_blocked != NULL) {
		*shield_blocked = false;
	}

	if (SHIELD_ALLOW_LEGACY_HITBOX_SHIELD_FALLBACK == false) {
		return attacks;
	}

	return ApplyShieldFrontDefenseToBulletDamage(
		hitHuman,
		shotHuman,
		attacks,
		bullet_penetration,
		bullet_vx,
		bullet_vz,
		hit_x,
		hit_y,
		hit_z,
		shield_blocked
	);
}

static void GetHumanBulletHitboxParts(
	human* h,
	float base_y,
	float* leg_y,
	float* leg_h,
	float* up_y,
	float* up_h,
	float* head_y,
	float* head_h
)
{
	// 서 있는 상태 기본값
	*leg_y = base_y;
	*leg_h = HUMAN_BULLETCOLLISION_LEG_H;

	*up_y = *leg_y + *leg_h;
	*up_h = HUMAN_BULLETCOLLISION_UP_H;

	*head_y = *up_y + *up_h;
	*head_h = HUMAN_BULLETCOLLISION_HEAD_H;

	if (h != NULL && h->GetCrouchFlag() == true) {
		// 앉기 상태에서는 상체/머리 전체를 조금 낮추되,
		// 몸통과 머리 높이 자체는 크게 줄이지 않는다.
		// 기존 0.55 / 0.85 배율 방식은 머리 히트박스를 너무 아래로 내린다.
		const float crouch_hitbox_y_offset = -2.0f;

		*leg_y = base_y;

		// 다리 높이만 줄여서 상체 시작 위치를 낮춘다.
		// 10.5 + (-3.5) = 7.0
		*leg_h = HUMAN_BULLETCOLLISION_LEG_H + crouch_hitbox_y_offset;
		if (*leg_h < 1.0f) {
			*leg_h = 1.0f;
		}

		*up_y = *leg_y + *leg_h;
		*up_h = HUMAN_BULLETCOLLISION_UP_H;

		*head_y = *up_y + *up_h;
		*head_h = HUMAN_BULLETCOLLISION_HEAD_H;
	}
}


// ------------------------------------------------------------
// Human ID safety policy
//
// 목적:
// 1. 정상 인물 0~43은 그대로 사용한다.
// 2. 범위 밖/음수 인물은 직접 로딩하지 않고 안전한 정상 인물로 대체한다.
// 3. 원본에서 맵 텍스처 0번으로 보이던 버그 인물은 흰 더미 인물 + 맵 텍스처 0번으로 처리한다.
// 4. 튕김 원인이 되는 버그 무기는 절대 들려주지 않는다.
// ------------------------------------------------------------

static bool IsNormalHumanID(int id)
{
	return (id >= 0 && id < TOTAL_PARAMETERINFO_HUMAN);
}

static bool IsWhiteDummyHumanID(int id)
{
	return (id == ID_HUMAN_WHITE_DUMMY || id == ID_HUMAN_WHITE_DUMMY_ALIAS);
}

struct BugHumanProfile
{
	int humanID;             // 실제 사용할 정상 인물 ID
	bool useMapTexture0;     // 인물 텍스처 대신 맵 텍스처 0번 사용
	bool noWeapon;           // 무기 제거
	int forceHP;             // -1이면 기본 HP 유지, 0 이상이면 강제 HP
	bool spawnDeadBody;      // true면 생성 직후 시체 상태
	int renderMask;          // 표시할 신체 파츠

	int forceWeapon1;        // 강제 주무기
	int forceWeapon2;        // 강제 보조무기
};

// ------------------------------------------------------------
// 맵 텍스처 0번 더미 중 표에 정상 무기가 있는 번호만 무기 허용.
// 위험한 문자열 무기, 0번 무기, CASE, ? 계열은 제외.
// ------------------------------------------------------------
static bool GetBugHumanSafeWeapon(int original_id, int* out_weapon1, int* out_weapon2)
{
	if (out_weapon1 == NULL || out_weapon2 == NULL) { return false; }

	*out_weapon1 = ID_WEAPON_NONE;
	*out_weapon2 = ID_WEAPON_NONE;

	switch (original_id) {
		// 양수 맵 텍스처 0번
	case 116:
		*out_weapon1 = 71;  // CF-05 (기존 P90)
		return true;

	case 99:
		*out_weapon2 = 51;  // MP9 (기존 MP5SD)
		return true;

	case 91:
		*out_weapon1 = 29;  // Galil (기존 AK47)
		return true;

	case 90:
		*out_weapon1 = 57;  // Calico M950 (기존 MK23 SD 대체용 권총)
		// 보조 ./data/map7 은 위험하므로 제거
		return true;

	case 86:
		*out_weapon1 = 47;  // KRISS Vector (기존 MAC10)
		return true;

	case 44:
		*out_weapon1 = 65;  // M1928A1 Thompson (기존 MAC10)
		*out_weapon2 = 53;  // M29 Satan (기존 Desert Eagle)
		return true;

		// 음수 맵 텍스처 0번
	case -1:
		*out_weapon1 = 73;  // K1A (기존 MP5)
		return true;

	case -11:
		*out_weapon1 = 37;  // SCAR (기존 AUG)
		*out_weapon2 = 41;  // HK416 (기존 M4)
		return true;

	case -16:
		*out_weapon1 = 26;  // UMP SD (기존 UMP)
		*out_weapon2 = 71;  // CF-05 (기존 P90)
		return true;

	case -17:
		*out_weapon1 = 31;  // SIG 556 (기존 M4)
		*out_weapon2 = 28;  // KAC PDW (기존 P90)
		return true;

	case -22:
		*out_weapon1 = 52;  // Beretta 93R (기존 M92F)
		*out_weapon2 = 69;  // Vz.61 Skorpion (기존 M92F)
		return true;

	case -23:
		*out_weapon1 = 51;  // MP9 (기존 MP5)
		*out_weapon2 = 46;  // TAR-21 (기존 M4)
		return true;

	case -25:
		// 주무기 .data/model/is 는 위험하므로 제거
		*out_weapon2 = 33;  // MP40 (기존 UMP)
		return true;

	case -45:
		*out_weapon1 = 30;  // M60 (기존 M249)
		*out_weapon2 = 71;  // CF-05 (기존 P90)
		return true;

	case -46:
		*out_weapon1 = 74;  // K2C (기존 AUG)
		*out_weapon2 = 52;  // Beretta 93R (기존 GLOCK18)
		return true;

	case -52:
		*out_weapon2 = 47;  // KRISS Vector (기존 P90)
		return true;

	case -69:
		*out_weapon1 = 69;  // Vz.61 Skorpion (기존 GLOCK18)
		*out_weapon2 = 23;  // M1 카빈 (기존 PSG-1)
		return true;

	case -74:
		*out_weapon1 = 37;  // SCAR (기존 AUG)
		*out_weapon2 = 63;  // 스프링필드 (기존 PSG-1)
		return true;

	case -75:
		*out_weapon1 = 64;  // STG44 (기존 AK47)
		*out_weapon2 = 53;  // M29 Satan (기존 Desert Eagle)
		return true;

	case -111:
		*out_weapon1 = 41;  // HK416 (기존 M4)
		*out_weapon2 = 71;  // CF-05 (기존 P90)
		return true;

	case -116:
		*out_weapon1 = 23;  // M1 카빈 (기존 PSG-1)
		*out_weapon2 = 53;  // M29 Satan (기존 Desert Eagle)
		return true;

	case -76:
		*out_weapon1 = 52; // Beretta 93R
		*out_weapon2 = 57; // Calico M950 (기존 GLOCK18 / GLOCK18)
		return true;

		// --- [새로 추가/수정: 표의 내용을 반영 및 튕기는 무기 교체] ---
	case -62:
		*out_weapon1 = 57; // Calico M950 (기존 M92F)
		*out_weapon2 = 47; // KRISS Vector (기존 MAC10)
		return true;
	case -65:
		*out_weapon1 = 71; // CF-05 (기존 P90)
		*out_weapon2 = 65; // M1928A1 (기존 MAC10 대체)
		return true;
	case -66:
		*out_weapon1 = 25; // SPAS AUTO (기존 M1)
		*out_weapon2 = 23; // M1 카빈 (기존 PSG-1)
		return true;
	case -67:
		*out_weapon1 = ID_WEAPON_NONE; // -
		*out_weapon2 = 69; // Vz.61 Skorpion (기존 M92F 대체)
		return true;
	case -77:
		*out_weapon1 = 46; // TAR-21 (기존 M4)
		*out_weapon2 = 53; // M29 Satan (기존 Desert Eagle 대체)
		return true;
	case 122:
		*out_weapon2 = 53; // M29 Satan (기존 Desert Eagle)
		return true;
	case 123:
		*out_weapon2 = 52; // Beretta 93R (기존 GLOCK18)
		return true;
	case 127:
		*out_weapon1 = 71; // CF-05 (기존 P90)
		return true;
	case 124:
		*out_weapon2 = 53; // M29 Satan (기존 Desert Eagle)
		return true;
	case 119:
		*out_weapon1 = 28; // KAC PDW (기존 P90)
		return true;
	case 117:
		*out_weapon1 = 73; // K1A (기존 MP5)
		return true;
	case 114:
		*out_weapon2 = 71; // CF-05 (기존 P90)
		return true;
	case 111:
		*out_weapon1 = 47; // KRISS Vector (기존 P90)
		return true;

		// --- [표 데이터 기반 누락된 음수 번호 인물 추가] ---
	case -4:
		*out_weapon1 = 57;  // Calico M950 (기존 M92F)
		*out_weapon2 = 31;  // SIG 556 (기존 M4)
		return true;
	case -8:
		*out_weapon2 = 27;  // Mk14 (기존 PSG-1)
		return true;
	case -12:
		*out_weapon1 = 41;  // HK416 (기존 M4)
		*out_weapon2 = 35;  // Saiga-12 (기존 M1)
		return true;
	case -13:
		*out_weapon1 = 51;  // MP9 (기존 MP5)
		*out_weapon2 = 69;  // Vz.61 Skorpion (기존 M92F)
		return true;
	case -14:
		*out_weapon1 = 63;  // 스프링필드 (기존 PSG-1)
		return true;
	case -18:
		*out_weapon1 = 52;  // Beretta 93R (기존 GLOCK18)
		*out_weapon2 = 52;  // Beretta 93R (기존 GLOCK18)
		return true;
	case -19:
		*out_weapon1 = 37;  // SCAR (기존 M4)
		return true;
	case -21:
		*out_weapon2 = 74;  // K2C (기존 M4)
		return true;
	case -24:
		*out_weapon1 = 27;  // MK14 (기존 PSG-1)
		return true;
	case -28:
		*out_weapon2 = 59;  // KAR98K 스나이퍼 (기존 PSG-1)
		return true;
	case -34:
		*out_weapon2 = 73;  // K1A (기존 MP5)
		return true;
	case -37:
		*out_weapon2 = 23;  // M1 카빈 (기존 PSG-1)
		return true;
	case -41:
		*out_weapon1 = 26;  // UMP SD (기존 MP5)
		return true;
	case -43:
		*out_weapon1 = 53;  // M29 Satan (기존 Desert Eagle)
		return true;
	case -47:
		*out_weapon1 = 53;  // M29 Satan (기존 Desert Eagle)
		*out_weapon2 = 57;  // Calico M950 (기존 M92F)
		return true;
	case -48:
		*out_weapon1 = 71;  // CF-05 (기존 P90)
		return true;
	case -50:
		*out_weapon2 = 37;  // SCAR (기존 AUG)
		return true;
	case -51:
		*out_weapon1 = 33;  // MP40 (기존 MP5)
		*out_weapon2 = 53;  // M29 Satan (기존 Desert Eagle)
		return true;
	case -57:
		*out_weapon2 = 41;  // HK416 (기존 M4)
		return true;
	case -58:
		*out_weapon1 = 47;  // KRISS Vector (기존 P90)
		return true;
	case -80:
		*out_weapon1 = 51;  // MP9 (기존 MP5)
		*out_weapon2 = 52;  // Beretta 93R (기존 GLOCK18)
		return true;
	case -82:
		*out_weapon1 = 59;  // KAR98K 스나이퍼 (기존 PSG-1)
		return true;
	case -85:
		*out_weapon1 = 40;  // AWP (기존 PSG-1)
		*out_weapon2 = 28;  // KAC PDW (기존 MP5)
		return true;
	case -86:
		*out_weapon2 = 71;  // CF-05 (기존 P90)
		return true;
	case -89:
		*out_weapon1 = 69;  // Vz.61 Skorpion (기존 GLOCK18)
		return true;
	case -91:
		*out_weapon1 = 23;  // M1 카빈 (기존 PSG-1)
		*out_weapon2 = 47;  // KRISS Vector (기존 MAC10)
		return true;
	case -99:
		*out_weapon2 = 57;  // Calico M950 (기존 M92F)
		return true;
	}

	return false;
}

static BugHumanProfile MakeBugHumanProfile(int original_id)
{
	BugHumanProfile p;
	p.humanID = original_id;
	p.useMapTexture0 = false;
	p.noWeapon = true;     // 기본적으로 버그 인물은 안전을 위해 무기를 뺍니다.
	p.forceHP = 20;      // 기본 체력
	p.spawnDeadBody = false;
	p.renderMask = HUMAN_RENDER_ALL;
	p.forceWeapon1 = ID_WEAPON_NONE;
	p.forceWeapon2 = ID_WEAPON_NONE;

	// 정상 인물은 그대로 사용
	if (IsNormalHumanID(original_id) == true) {
		p.noWeapon = false;
		p.forceHP = -1;
		return p;
	}

	// --- [1단계] 외형(Skin) 및 기본 속성 지정 ---
	if (IsWhiteDummyHumanID(original_id) == true) {
		p.humanID = ID_HUMAN_WHITE_DUMMY;
		p.useMapTexture0 = true;
	}
	else {
		switch (original_id) {
			// 맵 텍스처 0번 계열
		case 116: case 109: case 108: case 102: case 101: case 99: case 97: case 95: case 93: case 91: case 90:
		case 86: case 75: case 67: case 59: case 53: case 51: case 49: case 45: case 44:
		case -1: case -11: case -16: case -17: case -22: case -23: case -25: case -40: case -45: case -46:
		case -52: case -54: case -69: case -74: case -75: case -96: case -99: case -101: case -103: case -111:
		case -115: case -116: case -123:
			p.humanID = ID_HUMAN_WHITE_DUMMY;
			p.useMapTexture0 = true;
			break;

			// 검은 군인 계열
		case 127: case 124: case 123: case 121: case 119: case 117: case 115: case 114: case 113: case 111:
		case 58: case 43: case -18: case -21: case -43: case -50: case -76:
		case -93: case -94: case -95: case -117: case -118: case -119:
			p.humanID = 0;
			break;

			// 초록 군인 계열
		case 126: case -14: case -41: case -47: case -58:
			p.humanID = 2;
			break;

			// 파란 군인 계열
		case 122: case -4: case -24: case -34: case -67: case -91:
			p.humanID = 9;
			break;

			// 보라 군인 계열
		case -62: case -82:
			p.humanID = 8;
			break;

			// 하얀 군인 계열
		case -27: case -51: case -87:
			p.humanID = 5;
			break;

			// 군인 0 / 헬멧 없는 군인 계열
		case 120: case 100: case 112: case -85:
			p.humanID = 26;
			break;

			// 경찰 계열
		case -8: case -37:
			p.humanID = 14;
			break;

			// 장군/이슬람 계열
		case -12:
			p.humanID = 35;
			break;
		case -48: case -89:
			p.humanID = 17;
			break;

			// 여자 계열
		case -19: case -77:
			p.humanID = 18;
			break;

			// 파란 정장 계열
		case -28:
			p.humanID = 37;
			break;

			// 좀비 계열
		case -57: case -80: case -86:
			p.humanID = 29;
			break;

			// 로봇 계열
		case -65:
			p.humanID = 10;
			break;

			// 갈색 정장 계열
		case -66:
			p.humanID = 12;
			break;

			// 알 수 없는 버그 번호는 안정성을 위해 맵 텍스처 0번 더미로 처리
		default:
			p.humanID = ID_HUMAN_WHITE_DUMMY;
			p.useMapTexture0 = true;
			break;
		}
	}

	// --- [2단계] 무기 지급 검사 ---
	// 외형에 상관없이, 무기 목록(GetBugHumanSafeWeapon)에 등록된 인물이면 무기를 쥐여줍니다.
	if (GetBugHumanSafeWeapon(original_id, &p.forceWeapon1, &p.forceWeapon2) == true) {
		p.noWeapon = false;
	}

	// --- [3단계] 체력(HP) 표 기반 할당 ---
		// 외형에 상관없이, 표에 명시된 체력을 덮어씌웁니다.
	switch (original_id) {
		// 체력 18000 짜리 (맵 텍스처 0번 더미 등)
	case 109: case 108: case 101: case 99: case 95: case 93: case 90: case 86:
	case 75: case 67: case 59: case 53: case 51: case 49: case 45: case 44:
	case -1: case -96: case -99: case -101: case -103: case -111:
		p.forceHP = 18000;
		p.spawnDeadBody = false;
		break;

		// 체력 약 1000 짜리
	case -116:
		p.forceHP = 1000;
		p.spawnDeadBody = false;
		break;

		// 체력 약 800 짜리
	case -115:
		p.forceHP = 800;
		p.spawnDeadBody = false;
		break;

		// 체력 100 짜리
	case -67: case -17:
		p.forceHP = 100;
		p.spawnDeadBody = false;
		break;

		// 체력 50 짜리
	case -14: case -22:
		p.forceHP = 50;
		p.spawnDeadBody = false;
		break;

		// 체력 40 짜리
	case -75: case -8: case -27: case -37: case -51: case -80:
		p.forceHP = 40;
		p.spawnDeadBody = false;
		break;

		// 체력 30 짜리
	case -62: case -85: case -91:
		p.forceHP = 30;
		p.spawnDeadBody = false;
		break;

		// 체력 20 짜리
	case 126: case 122: case 120: case 114:
	case -65: case -66: case -69: case -74: case -76: case -77:
	case -4: case -11: case -12: case -16: case -18: case -19:
	case -23: case -24: case -25: case -28: case -34: case -40:
	case -45: case -46: case -47: case -48: case -52: case -57:
	case -58: case -82: case -87: case -89:
		p.forceHP = 20;
		p.spawnDeadBody = false;
		break;

		// 체력 10 짜리
	case 43: case -50: case -54: case -86:
		p.forceHP = 10;
		p.spawnDeadBody = false;
		break;

		// 체력 0 짜리 (스폰 즉시 사망)
	case 127: case 124: case 123: case 121: case 119:
	case 117: case 116: case 115: case 113: case 112: case 111:
	case 102: case 100: case 97: case 91: case 58:
	case -13: case -21: case -41: case -93: case -94: case -95:
	case -117: case -118: case -119:
		p.forceHP = 0;
		p.spawnDeadBody = true; // 체력이 0이면 눕도록 처리
		break;
	}

	// 모든 설정을 마친 후 최종 반환
	return p;
}

// ------------------------------------------------------------
// Bug human render parts policy
//
// 안정성 우선:
// - 원본의 잘못된 모델/무기 참조는 재현하지 않는다.
// - 히트박스, AI, 피격 판정은 그대로 둔다.
// - 보이는 신체 파츠만 숨긴다.
// - HUMAN_RENDER_WEAPON은 되도록 사용하지 않는다.
// ------------------------------------------------------------
static bool IsBugHumanNoAI(int original_id)
{
	switch (original_id) {
	case 126:
	case 122:
	case 120:
	case 114:

	case 109:
	case 108:
	case 95:
	case 93:
	case 75:
	case 67:
	case 59:

	case -17:
	case -89:
	case -101:

	case 43:
	case -19:
	case -48:
	case -96:
	case -111:
		return true;
	}

	return false;
}

static int GetBugHumanRenderMask(int original_id)
{
	// --------------------------------------------------------
	// 발 부분에 팔/팔모션 계열
	// 팔만 보이면 공중에 떠 보일 수 있으므로, 안정적으로 팔 + 다리를 표시한다.
	// --------------------------------------------------------
	switch (original_id) {
	case -4:
	case -11:
	case -52:
	case -86:
	case -93:
		return HUMAN_RENDER_ARM | HUMAN_RENDER_LEG;
	}

	// --------------------------------------------------------
// 맵 텍스처 0번 + 정상 무기 허용 계열
// 팔 + 다리 + 손에 든 무기를 표시한다.
// --------------------------------------------------------
	switch (original_id) {
	case 99:
	case 91:
	case 90:
	case 86:
	case 44:

	case -1:
	case -11:
	case -16:
	case -17:
	case -22:
	case -23:
	case -25:
	case -45:
	case -46:
	case -52:
	case -69:
	case -74:
	case -75:
	case -111:
	case -116:
		return HUMAN_RENDER_ARM | HUMAN_RENDER_LEG | HUMAN_RENDER_WEAPON;
	}



	// --------------------------------------------------------
		// 몸통까지 존재 계열
		// 머리, 몸통, 팔, 다리 모두 기존대로 보존하여 전부 표시합니다.
		// --------------------------------------------------------
	switch (original_id) {
	case 116:
	case 109:
	case 108:
	case 93:
	case -14:
	case -21:
	case -41:
	case -43:
	case -94:
	case -95:
	case -99:
	case -117:
	case -118:
	case -119:
		return HUMAN_RENDER_ALL;
	}

	// --------------------------------------------------------
	// 발 부분에 총/무기 모델링 계열
	// 안정성 때문에 실제 무기는 표시하지 않고 다리만 남긴다.
	// 원본의 "발 부분 무기"는 잘못된 모델 참조에 가까우므로
	// HUMAN_RENDER_WEAPON은 켜지 않는다.
	// --------------------------------------------------------
	switch (original_id) {
	case 127:
	case 123:
	case 121:

	case -27:
	case -37:
	case -50:
	case -51:
	case -62:
	case -65:
	case -66:
	case -80:
	case -82:
	case -85:
	case -91:
		return HUMAN_RENDER_ARM | HUMAN_RENDER_LEG | HUMAN_RENDER_WEAPON;
	}

	// --------------------------------------------------------
	// 걷는 모션 중첩 / 외형이 애매한 계열
	// 안정성을 위해 신체는 전부 표시한다.
	// --------------------------------------------------------
	switch (original_id) {
	case -12:
	case -16:
	case -28:
	case -34:
	case -45:
	case -47:
	case -58:
	case -67:
	case -76:

		return HUMAN_RENDER_ALL;
	}

	// --------------------------------------------------------
// 맵 텍스처 0번 더미 계열
// 위에서 따로 지정되지 않은 맵 텍스처 0번 더미는
// 신체 파츠를 표시하지 않는다.
//
// 중요:
// MakeBugHumanProfile()에서는 맵 텍스처 0번 더미로 치환되지만,
// 여기서는 반드시 원본 포인트 번호 original_id 기준으로 판단해야 한다.
// --------------------------------------------------------
	switch (original_id) {
	case 102:
	case 101:
	case 99:
	case 97:
	case 95:
	case 91:
	case 90:
	case 86:
	case 75:
	case 67:
	case 59:
	case 53:
	case 51:
	case 49:
	case 45:
	case 44:

	case -1:
	case -17:
	case -22:
	case -23:
	case -25:
	case -40:
	case -46:
	case -54:
	case -69:
	case -74:
	case -75:
	case -96:
	case -101:
	case -103:
	case -111:
	case -115:
	case -116:
	case -123:
		// 완전 투명 방지:
		// 맵 텍스처 0번 더미는 아무 파츠도 안 그리면 화면에서 사라진다.
		// 특이점이 없는 더미는 안정적으로 다리만 표시한다.
		return HUMAN_RENDER_ARM | HUMAN_RENDER_LEG;
	}

	// 그 외는 기본적으로 전부 표시
	return HUMAN_RENDER_ALL;
}

static int ResolveHumanIDForPoint(int id)
{
	return MakeBugHumanProfile(id).humanID;
}

void ObjectManager::ResetRuntimeState()
{
	for (int i = 0; i < MAX_HUMAN; i++) {
		RuntimeLandingCooldown[i] = 0;
		RuntimeFallingFrames[i] = 0;
		RuntimeLastTexture[i] = -1;
		RuntimeLastY[i] = 0.0f;
		RuntimeGroundGrace[i] = 0;
	}

	RuntimeEffectLastIndex = 0;
	RuntimeMapEffectLastIndex = 0;

	if (Human_ShieldBlockSwitchCount != NULL &&
		Human_ShieldBlockSwitchDamage != NULL &&
		Human_ShieldBlockSwitchCooldown != NULL) {
		for (int i = 0; i < MAX_HUMAN; i++) {
			Human_ShieldBlockSwitchCount[i] = 0;
			Human_ShieldBlockSwitchDamage[i] = 0;
			Human_ShieldBlockSwitchCooldown[i] = 0;
		}
	}

	// 추가 권장
	framecnt = 0;
}

//! @brief 긓깛긚긣깋긏?
ObjectManager::ObjectManager()
{
	HumanIndex = new human[MAX_HUMAN];
	WeaponIndex = new weapon[MAX_WEAPON];
	SmallObjectIndex = new smallobject[MAX_SMALLOBJECT];
	BulletIndex = new bullet[MAX_BULLET];
	GrenadeIndex = new grenade[MAX_GRENADE];
	EffectIndex = new effect[MAX_EFFECT];

	framecnt = 0;

	Human_ontarget = new float[MAX_HUMAN];
	Human_kill = new int[MAX_HUMAN];
	Human_headshot = new int[MAX_HUMAN];
	Human_ShotFlag = new bool[MAX_HUMAN];
	Human_YakkyouCnt = new int[MAX_HUMAN];
	BulletObj_HumanIndex = new BulletObjectHumanIndex[MAX_BULLET];

	Human_ActionSoundTimer = new int[MAX_HUMAN];
	Human_ActionSoundID = new int[MAX_HUMAN];
	Human_ShellReloadPrevState = new int[MAX_HUMAN];
	Human_ShellReloadPrevLoad = new int[MAX_HUMAN];
	Human_MinigunSpinSoundTimer = new int[MAX_HUMAN];

	Human_ShieldBlockSwitchCount = new int[MAX_HUMAN];
	Human_ShieldBlockSwitchDamage = new int[MAX_HUMAN];
	Human_ShieldBlockSwitchCooldown = new int[MAX_HUMAN];

	Human_ForceNoAI = new bool[MAX_HUMAN];
	for (int i = 0; i < MAX_HUMAN; i++) {
		Human_ForceNoAI[i] = false;
	}

	AddCollisionFlag = false;
	FriendlyFire = new bool[MAX_HUMAN];
	Player_HumanID = 0;
	Human_FrameTextureRefresh = new bool[MAX_HUMAN];

	GameParamInfo = NULL;
	d3dg = NULL;
	Resource = NULL;
	BlockData = NULL;
	PointData = NULL;
	CollD = NULL;
	GameSound = NULL;
	MIFdata = NULL;

	ObjectLog = new ObjectManagerLog;

	ShowHitboxFlag = false;

	ResetRuntimeState();

}


//! @brief 긢귻긚긣깋긏?
ObjectManager::~ObjectManager()
{
	Cleanup();

	if( HumanIndex != NULL ){ delete [] HumanIndex; }
	if( WeaponIndex != NULL ){ delete [] WeaponIndex; }
	if( SmallObjectIndex != NULL ){ delete [] SmallObjectIndex; }
	if( BulletIndex != NULL ){ delete [] BulletIndex; }
	if( GrenadeIndex != NULL ){ delete [] GrenadeIndex; }
	if( EffectIndex != NULL ){ delete [] EffectIndex; }

	if( Human_ontarget != NULL ){ delete [] Human_ontarget; }
	if( Human_kill != NULL ){ delete [] Human_kill; }
	if( Human_headshot != NULL ){ delete [] Human_headshot; }
	if( Human_ShotFlag != NULL ){ delete [] Human_ShotFlag; }
	if( Human_YakkyouCnt != NULL ){ delete [] Human_YakkyouCnt; }
	if( BulletObj_HumanIndex != NULL ){ delete [] BulletObj_HumanIndex; }
	if (Human_ActionSoundTimer != NULL) { delete[] Human_ActionSoundTimer; }
	if (Human_ActionSoundID != NULL) { delete[] Human_ActionSoundID; }
	if (Human_ShellReloadPrevState != NULL) { delete[] Human_ShellReloadPrevState; }
	if (Human_ShellReloadPrevLoad != NULL) { delete[] Human_ShellReloadPrevLoad; }
	if (Human_MinigunSpinSoundTimer != NULL) { delete[] Human_MinigunSpinSoundTimer; }

	if (Human_ShieldBlockSwitchCount != NULL) { delete[] Human_ShieldBlockSwitchCount; }
	if (Human_ShieldBlockSwitchDamage != NULL) { delete[] Human_ShieldBlockSwitchDamage; }
	if (Human_ShieldBlockSwitchCooldown != NULL) { delete[] Human_ShieldBlockSwitchCooldown; }

	if (Human_ForceNoAI != NULL) { delete[] Human_ForceNoAI; }

	if (FriendlyFire != NULL) { delete[] FriendlyFire; }
	if (Human_FrameTextureRefresh != NULL) { delete[] Human_FrameTextureRefresh; }

	if (ObjectLog != NULL) { delete ObjectLog; }
}

//! @brief 랷뤖궥귡긏깋긚귩먠믦
//! @param in_GameParamInfo 긒??먠믦긢??듖뿚긏깋긚
//! @param in_d3dg ?됪룉뿚긏깋긚
//! @param in_Resource 깏??긚듖뿚긏깋긚
//! @param in_BlockData 긳깓긞긏긢??듖뿚긏깋긚
//! @param in_PointData ?귽깛긣긢??듖뿚긏깋긚
//! @param in_CollD 뱰궫귟뵽믦룉뿚긏깋긚
//! @param in_GameSound 뚼됈돶띋맯긏깋긚
//! @param in_MIFdata MIF긓깛긣깓?깑긏깋긚
//! @attention 궞궻듫릶궳먠믦귩뛱귦궶궋궴갂긏깋긚렔뫬궕맫궢궘??궢귏궧귪갃
void ObjectManager::SetClass(ParameterInfo *in_GameParamInfo, D3DGraphics *in_d3dg, ResourceManager *in_Resource, BlockDataInterface *in_BlockData, PointDataInterface *in_PointData, Collision *in_CollD, SoundManager *in_GameSound, MIFInterface *in_MIFdata)
{
	GameParamInfo = in_GameParamInfo;
	d3dg = in_d3dg;
	Resource = in_Resource;
	BlockData = in_BlockData;
	PointData = in_PointData;
	CollD = in_CollD;
	GameSound = in_GameSound;
	MIFdata = in_MIFdata;

	for(int i=0; i<MAX_HUMAN; i++){
		HumanIndex[i].SetParameterInfoClass(GameParamInfo);
	}
	for(int i=0; i<MAX_WEAPON; i++){
		WeaponIndex[i].SetParameterInfoClass(GameParamInfo);
	}
	for(int i=0; i<MAX_SMALLOBJECT; i++){
		SmallObjectIndex[i].SetParameterInfoClass(GameParamInfo);
		SmallObjectIndex[i].SetMIFInterfaceClass(MIFdata);
	}
	for(int i=0; i<MAX_GRENADE; i++){
		GrenadeIndex[i].SetParameterInfoClass(GameParamInfo);
	}

	int bulletmodel, bullettexture;
	BulletParameter data;

	if( Resource->GetBulletModelTexture(0, &bulletmodel, &bullettexture) == 0 ){
		if( GameParamInfo->GetBullet(0, &data) == 0 ){
			for(int i=0; i<MAX_BULLET; i++){
				BulletIndex[i].SetModel(bulletmodel, data.size);
				BulletIndex[i].SetTexture(bullettexture);
			}
		}
	}

	WeaponParameter w_data;
	if (Resource->GetWeaponModelTexture(ID_WEAPON_GRENADE, &bulletmodel, &bullettexture) == 0) {
		if (GameParamInfo->GetWeapon(ID_WEAPON_GRENADE, &w_data) == 0) {
			for (int i = 0; i < MAX_GRENADE; i++) {
				GrenadeIndex[i].SetModel(bulletmodel, w_data.size);
				GrenadeIndex[i].SetTexture(bullettexture);
			}
		}
	}

	ObjectLog->SetClass(d3dg);
}

//! @brief 릐믁돿
//! @param data 릐궻?귽깛긣긢???걁pointdata?몾뫬걂
//! @param infodata 랷뤖궥귡릐륃뺪궻?귽깛긣긢???걁갫걂
//! @param RandomFlag 깋깛??뺚맫귩뾎뚼궸궥귡
//! @return 맟뚻갌긢??붥뜂걁0댥뤵걂?렪봲갌-1
//! @attention 뼰뚼궶릐궻롰쀞붥뜂궕럚믦궠귢궫뤾뜃궼 믅뤝갌볞릐듩 궕뱋뤾궢귏궥갃긡긏긚?긿궼?긞긵긡긏긚?긿0붥궕럊뾭궠귢갂HP궼 0 궕럚믦걁걖뫂?걂궠귢귏궥갃
int ObjectManager::AddHumanIndex(pointdata data, pointdata infodata, bool RandomFlag)
{
	int raw_human_id = infodata.p2;
	BugHumanProfile bugprofile = MakeBugHumanProfile(raw_human_id);

	// 표 기준 신체 파츠 표시 설정.
	// MakeBugHumanProfile()의 각 switch에 일일이 넣지 않고,
	// 여기서 원본 포인트 번호 기준으로 한 번 더 적용한다.
	bugprofile.renderMask = GetBugHumanRenderMask(raw_human_id);

	infodata.p2 = bugprofile.humanID;
	bool white_dummy_human = bugprofile.useMapTexture0;
	
	int GetHumanFlag;
	HumanParameter HumanParam;
	GetHumanFlag = GameParamInfo->GetHuman(infodata.p2, &HumanParam);
	int Humanindexid = -1;
	int Weaponindexid = -1;
	class weapon *Weapon[TOTAL_HAVEWEAPON];
	int lnbslist[TOTAL_HAVEWEAPON];
	int nbslist[TOTAL_HAVEWEAPON];
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		Weapon[i] = NULL;
		lnbslist[i] = 0;
		nbslist[i] = 0;
	}

#ifndef ENABLE_BUG_HUMANWEAPON
	if( GetHumanFlag != 0 ){
		return -1;
	}
#endif

	//릐궻긾긢깑붥뜂귩롦벦
	int upmodel[TOTAL_UPMODE];
	int armmodel[TOTAL_ARMMODE];
	int legmodel;
	int walkmodel[TOTAL_WALKMODE];
	int runmodel[TOTAL_RUNMODE];
	int sitlegmodel;
	int sitwalkmodel[TOTAL_SITWALKMODE];

	Resource->GetHumanModel(upmodel, armmodel, &legmodel, walkmodel, runmodel);
	Resource->GetHumanSitModel(&sitlegmodel, sitwalkmodel);

	float px = data.x;
	float py = data.y;
	float pz = data.z;
	float rx = data.r;
	if( RandomFlag == true ){
		px += 0.001f*GetRand(100*2) - 0.1f;
		pz += 0.001f*GetRand(100*2) - 0.1f;
		rx += DegreeToRadian(0.01f)*GetRand(80*2) - DegreeToRadian(0.8f);
	}

	// 최종적으로 할당될 무기 ID
	int final_weapon_id_1 = HumanParam.Weapon[0];
	int final_weapon_id_2 = HumanParam.Weapon[1];
	int char_id = infodata.p2; // 현재 생성되는 캐릭터의 ID

	// 무기 선택 배열 선언
	int weapon_options_0[] = { 8, 62, 70 };
	int weapon_options_1[] = { 40, 23, 63 };
	int weapon_options_2[] = { 6, 65, 73 };
	int weapon_options_3[] = { 10, 41, 58 };
	int weapon_options_4[] = { 2, 27, 54 };
	int weapon_options_5[] = { 7, 26, 47 };
	int weapon_options_6[] = { 2, 39, 59 };
	int weapon_options_8[] = { 14, 45, 72 };
	int weapon_options_9[] = { 1, 28, 60 };
	int weapon_options_10_1[] = { 6, 48, 49 };
	int weapon_options_10_2[] = { 12, 55, 67 };
	int weapon_options_12[] = { 5, 50, 69 };
	int weapon_options_14[] = { 3, 57, 36 };
	int weapon_options_17[] = { 10, 64, 29 };
	int weapon_options_34[] = { 17, 52, 69 };
	int weapon_options_39[] = { 20, 46, 56 };

	switch (char_id)
	{
	case 0: // 경찰관 A
		final_weapon_id_1 = (GetRand(2) == 0) ? 21 : 49;
		final_weapon_id_2 = weapon_options_0[GetRand(3)];
		break;
	case 1: // 경찰관 B
		final_weapon_id_1 = (GetRand(2) == 0) ? 21 : 53;
		final_weapon_id_2 = weapon_options_1[GetRand(3)];
		break;
	case 2: // 범죄자 A
		final_weapon_id_1 = (GetRand(2) == 0) ? 3 : 34;
		final_weapon_id_2 = weapon_options_2[GetRand(3)];
		break;
	case 3: // 범죄자 B
		final_weapon_id_1 = (GetRand(2) == 0) ? 3 : 34;
		final_weapon_id_2 = weapon_options_3[GetRand(3)];
		break;
	case 4: // 범죄자 C
		final_weapon_id_1 = (GetRand(2) == 0) ? 3 : 53;
		final_weapon_id_2 = weapon_options_4[GetRand(3)];
		break;
	case 5: // 범죄자 흰색 A
		final_weapon_id_1 = (GetRand(2) == 0) ? 4 : 61;
		final_weapon_id_2 = weapon_options_5[GetRand(3)];
		break;
	case 6: // 범죄자 흰색 B
		final_weapon_id_1 = (GetRand(2) == 0) ? 4 : 61;
		final_weapon_id_2 = weapon_options_6[GetRand(3)];
		break;
	case 7: // 조폭
		final_weapon_id_1 = (GetRand(2) == 0) ? 4 : 19;
		break;
	case 8: // 범죄자 보라
		final_weapon_id_1 = (GetRand(2) == 0) ? 22 : 51;
		final_weapon_id_2 = weapon_options_8[GetRand(3)];
		break;
	case 9: // 범죄자 파랑
		final_weapon_id_1 = (GetRand(2) == 0) ? 3 : 48;
		final_weapon_id_2 = weapon_options_9[GetRand(3)];
		break;
	case 10: // 인조인간
		final_weapon_id_1 = weapon_options_10_1[GetRand(3)];
		final_weapon_id_2 = weapon_options_10_2[GetRand(3)];
		break;
	case 11: // 회사원 SG
		final_weapon_id_1 = (GetRand(2) == 0) ? 18 : 52;
		break;
	case 12: // 회사원
		final_weapon_id_1 = weapon_options_12[GetRand(3)];
		break;
	case 14: // 경찰
		final_weapon_id_1 = weapon_options_14[GetRand(3)];
		break;
	case 16: // 긴팔셔츠 1
		final_weapon_id_1 = (GetRand(2) == 0) ? 17 : 68;
		break;
	case 17: // 이슬람 병사
		final_weapon_id_1 = (GetRand(2) == 0) ? 17 : 33;
		final_weapon_id_2 = weapon_options_17[GetRand(3)];
		break;
	case 26: // 특수부대 1A
		final_weapon_id_1 = (GetRand(2) == 0) ? 5 : 66;
		final_weapon_id_2 = (GetRand(2) == 0) ? 9 : 43;
		break;
	case 27: // 특수부대 1B
		final_weapon_id_1 = (GetRand(2) == 0) ? 5 : 66;
		final_weapon_id_2 = (GetRand(2) == 0) ? 12 : 30;
		break;
	case 28: // 특수부대 2
		final_weapon_id_1 = (GetRand(2) == 0) ? 13 : 35;
		final_weapon_id_2 = (GetRand(2) == 0) ? 10 : 44;
		break;
	case 34: // 회사원 국방 SG
		final_weapon_id_1 = weapon_options_34[GetRand(3)];
		break;
	case 35: // 이슬람 병사
		final_weapon_id_1 = (GetRand(2) == 0) ? 17 : 35;
		break;
	case 37: // 회사원 파랑 SG
		final_weapon_id_1 = (GetRand(2) == 0) ? 18 : 68;
		break;
	case 38: // 긴팔셔츠 2 SG
		final_weapon_id_1 = (GetRand(2) == 0) ? 6 : 52;
		break;
	case 39: // 특수부대 3
		final_weapon_id_1 = (GetRand(2) == 0) ? 5 : 53;
		final_weapon_id_2 = weapon_options_39[GetRand(3)];
		break;
	case 40: // 특수부대 3 SG
		final_weapon_id_1 = (GetRand(2) == 0) ? 21 : 61;
		break;
	case 42: // 빌게이츠 SG
		final_weapon_id_1 = (GetRand(2) == 0) ? 22 : 51;
		break;
	}



	for(int j=0; j<MAX_HUMAN; j++){
		if( HumanIndex[j].GetEnableFlag() == false ){
			//룊딖돸궥귡
			HumanIndex[j].SetPosData(px, py, pz, rx);
			HumanIndex[j].SetParamData(infodata.p2, data.id, data.p4, infodata.p3, true);
			if (GetHumanFlag == 0) {
				int id;

				if (white_dummy_human == true) {
					// 맵 텍스처 0번 더미
					id = d3dg->GetMapTextureID(0);

					// 안전장치:
					// 맵 텍스처 0번을 얻지 못하면 완전 투명/무텍스처가 될 수 있으므로
					// 흰 더미 인물 텍스처로 fallback한다.
					if (id == -1) {
						Resource->AddHumanTexture(ID_HUMAN_WHITE_DUMMY);
						id = Resource->GetHumanTexture(ID_HUMAN_WHITE_DUMMY);
					}
				}
				else {
					id = Resource->GetHumanTexture(infodata.p2);
				}

				if (id == -1) {
					if (white_dummy_human == false) {
						Human_FrameTextureRefresh[j] = true;
					}
					else {
						Human_FrameTextureRefresh[j] = false;
					}
				}
				else {
					Human_FrameTextureRefresh[j] = false;
				}

				HumanIndex[j].SetTexture(id);
				HumanIndex[j].SetModel(upmodel[HumanParam.model], armmodel, legmodel, walkmodel, runmodel);
				HumanIndex[j].SetSitModel(sitlegmodel, sitwalkmodel);
			}
			else{
#ifdef ENABLE_BUG_HUMANWEAPON
				HumanIndex[j].SetTexture(d3dg->GetMapTextureID(0));
				HumanIndex[j].SetModel(upmodel[0], armmodel, legmodel, walkmodel, runmodel);
#endif
			}
			HumanIndex[j].SetEnableFlag(true);
			Humanindexid = j;
			break;
		}
	}

	//룊딖돸궳궖궶궔궯궫귞긄깋?궴궢궲뺅궥
	if( Humanindexid == -1 ){
		return -1;
	}

	// 버그 인물은 위험한 무기를 들려주지 않는다.
	// 버그 인물 무기 처리
	if (bugprofile.noWeapon == true) {
		final_weapon_id_1 = ID_WEAPON_NONE;
		final_weapon_id_2 = ID_WEAPON_NONE;
	}
	else if (IsNormalHumanID(raw_human_id) == false) {
		// ▼ 수정됨: 맵 텍스처 0번이 아니더라도, 버그 인물이면 무조건 표에 적힌 강제 무기를 우선 지급합니다.
		final_weapon_id_1 = bugprofile.forceWeapon1;
		final_weapon_id_2 = bugprofile.forceWeapon2;
	}


	if (GetHumanFlag == 0) {
		// 1번 무기
		Weaponindexid = AddVisualWeaponIndex(final_weapon_id_1, true);
		if (Weaponindexid != -1) {
			Weapon[0] = &WeaponIndex[Weaponindexid];
		}

		if (data.p1 == POINT_P1TYPE_HUMAN) {
			// 2번 무기
			Weaponindexid = AddVisualWeaponIndex(final_weapon_id_2, true);
			if (Weaponindexid != -1) {
				Weapon[1] = &WeaponIndex[Weaponindexid];
			}
		}

		//릐궸렃궫궧귡
		HumanIndex[Humanindexid].SetWeapon(Weapon, lnbslist, nbslist);
	}

	// 버그 인물별 신체 파츠 표시 설정
	HumanIndex[Humanindexid].SetRenderPartsMask(bugprofile.renderMask);

	if (Human_ForceNoAI != NULL) {
		Human_ForceNoAI[Humanindexid] = IsBugHumanNoAI(raw_human_id);
	}

	// 버그 인물별 강제 HP / 시체 상태 적용
	if (bugprofile.spawnDeadBody == true) {
		// 포인트 ID에 따라 눕는 방향을 약간 바꿔서 같은 방향으로만 눕지 않게 한다.
		bool faceup = ((raw_human_id & 1) == 0);
		HumanIndex[Humanindexid].ForceDeadBody(faceup);
	}
	else if (bugprofile.forceHP >= 0) {
		HumanIndex[Humanindexid].SetHP(bugprofile.forceHP);
	}

	//뵯뺺긲깋긐귩룊딖돸
	Human_ShotFlag[Humanindexid] = false;
	Human_YakkyouCnt[Humanindexid] = -1;

	Human_ActionSoundTimer[Humanindexid] = -1;
	Human_ActionSoundID[Humanindexid] = RELOAD_NONE;
	Human_MinigunSpinSoundTimer[Humanindexid] = 0;

	if (Human_ShieldBlockSwitchCount != NULL) {
		Human_ShieldBlockSwitchCount[Humanindexid] = 0;
	}
	if (Human_ShieldBlockSwitchDamage != NULL) {
		Human_ShieldBlockSwitchDamage[Humanindexid] = 0;
	}
	if (Human_ShieldBlockSwitchCooldown != NULL) {
		Human_ShieldBlockSwitchCooldown[Humanindexid] = 0;
	}

	//긵깒귽깂?궶귞궽갂붥뜂귩딯?
	if( ( (data.p1 == POINT_P1TYPE_HUMAN)||(data.p1 == POINT_P1TYPE_HUMAN2) )&&(data.p4 == 0) ){
		//Player_HumanID = Humanindexid;
		SetPlayerID(Humanindexid);
	}

	return Humanindexid;
}

void ObjectManager::ClearHumanFrameTextureRefresh(int id)
{
	if ((id < 0) || (MAX_HUMAN <= id)) {
		return;
	}

	if (Human_FrameTextureRefresh == NULL) {
		return;
	}

	Human_FrameTextureRefresh[id] = false;
}

void ObjectManager::ApplyHumanUpperModelIndex(int human_id, int upper_model_index)
{
	if ((human_id < 0) || (MAX_HUMAN <= human_id)) {
		return;
	}

	if ((upper_model_index < 0) || (TOTAL_UPMODE <= upper_model_index)) {
		return;
	}

	if (HumanIndex[human_id].GetEnableFlag() == false) {
		return;
	}

	int upmodel[TOTAL_UPMODE];
	int armmodel[TOTAL_ARMMODE];
	int legmodel;
	int walkmodel[TOTAL_WALKMODE];
	int runmodel[TOTAL_RUNMODE];
	int sitlegmodel;
	int sitwalkmodel[TOTAL_SITWALKMODE];

	Resource->GetHumanModel(upmodel, armmodel, &legmodel, walkmodel, runmodel);
	Resource->GetHumanSitModel(&sitlegmodel, sitwalkmodel);

	HumanIndex[human_id].SetModel(
		upmodel[upper_model_index],
		armmodel,
		legmodel,
		walkmodel,
		runmodel
	);

	HumanIndex[human_id].SetSitModel(sitlegmodel, sitwalkmodel);
}


//! @brief 릐믁돿걁긒??뭷뾭걂
//! @param px X띆뷭
//! @param py Y띆뷭
//! @param pz Z띆뷭
//! @param rx X렡뛀궖
//! @param paramID 롰쀞붥뜂
//! @param TeamID ???붥뜂
//! @param WeaponID 븧딇롰쀞붥뜂궻봹쀱걁뾴멹릶갌TOTAL_HAVEWEAPON걂
//! @param param4 묉4긬깋긽???
//! @param RandomFlag 깋깛??뺚맫귩뾎뚼궸궥귡
//! @return 맟뚻갌긢??붥뜂걁0댥뤵걂?렪봲갌-1
//! @attention 뼰뚼궶릐궻롰쀞붥뜂궕럚믦궠귢궫뤾뜃궼 믅뤝갌볞릐듩 궕뱋뤾궢귏궥갃긡긏긚?긿궼?긞긵긡긏긚?긿0붥궕럊뾭궠귢갂HP궼 0 궕럚믦걁걖뫂?걂궠귢귏궥갃
int ObjectManager::AddHumanIndex(float px, float py, float pz, float rx, int paramID, int TeamID, int WeaponID[], signed short int param4, bool RandomFlag)
{

	if (IsNormalHumanID(paramID) == false) {
		return -1;
	}

	int GetHumanFlag;
	HumanParameter HumanParam;
	GetHumanFlag = GameParamInfo->GetHuman(paramID, &HumanParam);
	int Humanindexid = -1;
	int Weaponindexid = -1;
	class weapon *Weapon[TOTAL_HAVEWEAPON];
	int lnbslist[TOTAL_HAVEWEAPON];
	int nbslist[TOTAL_HAVEWEAPON];
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		Weapon[i] = NULL;
		lnbslist[i] = 0;
		nbslist[i] = 0;
	}

#ifndef ENABLE_BUG_HUMANWEAPON
	if( GetHumanFlag != 0 ){
		return -1;
	}
#endif

	//릐궻긾긢깑붥뜂귩롦벦
	//릐궻긾긢깑붥뜂귩롦벦
	int upmodel[TOTAL_UPMODE];
	int armmodel[TOTAL_ARMMODE];
	int legmodel;
	int walkmodel[TOTAL_WALKMODE];
	int runmodel[TOTAL_RUNMODE];
	int sitlegmodel;
	int sitwalkmodel[TOTAL_SITWALKMODE];

	Resource->GetHumanModel(upmodel, armmodel, &legmodel, walkmodel, runmodel);
	Resource->GetHumanSitModel(&sitlegmodel, sitwalkmodel);

	if( RandomFlag == true ){
		px += 0.001f*GetRand(100*2) - 0.1f;
		pz += 0.001f*GetRand(100*2) - 0.1f;
		rx += DegreeToRadian(0.01f)*GetRand(80*2) - DegreeToRadian(0.8f);
	}

	for(int i=0; i<MAX_HUMAN; i++){
		if( HumanIndex[i].GetEnableFlag() == false ){
			//룊딖돸궥귡
			HumanIndex[i].SetPosData(px, py, pz, rx);
			HumanIndex[i].SetParamData(paramID, -1, param4, TeamID, true);
			if (GetHumanFlag == 0) {
				int id = Resource->GetHumanTexture(paramID);
				if (id == -1) {
					Human_FrameTextureRefresh[i] = true;
				}

				HumanIndex[i].SetTexture(id);
				HumanIndex[i].SetModel(upmodel[HumanParam.model], armmodel, legmodel, walkmodel, runmodel);
				HumanIndex[i].SetSitModel(sitlegmodel, sitwalkmodel);
			}
			else{
#ifdef ENABLE_BUG_HUMANWEAPON
				HumanIndex[i].SetModel(upmodel[0], armmodel, legmodel, walkmodel, runmodel);
				HumanIndex[i].SetSitModel(sitlegmodel, sitwalkmodel);
#endif
			}
			HumanIndex[i].SetEnableFlag(true);
			Humanindexid = i;
			break;
		}
	}

	//룊딖돸궳궖궶궔궯궫귞긄깋?궴궢궲뺅궥
	if( Humanindexid == -1 ){
		return -1;
	}

	if (Human_ForceNoAI != NULL) {
		Human_ForceNoAI[Humanindexid] = false;
	}

	if( GetHumanFlag == 0 ){
		//돹몒븧딇귩믁돿
		for(int i=0; i<TOTAL_HAVEWEAPON; i++){
			Weaponindexid = AddVisualWeaponIndex(WeaponID[i], true);
			if( Weaponindexid != -1 ){
				//맟뚻궥귢궽봹쀱궸딯?
				Weapon[i] = &WeaponIndex[Weaponindexid];
			}
		}

		//릐궸렃궫궧귡
		HumanIndex[Humanindexid].SetWeapon(Weapon, lnbslist, nbslist);
	}

	//깓긐듫똚궻룉뿚
	int player_teamid;
	HumanIndex[Player_HumanID].GetParamData(NULL, NULL, NULL, &player_teamid);
	ObjectLog->AddHuman(Humanindexid, TeamID, player_teamid);

	return Humanindexid;
}

//! @brief 븧딇믁돿
//! @param data 븧딇궻?귽깛긣긢???걁pointdata?몾뫬걂
//! @return 맟뚻갌긢??붥뜂걁0댥뤵걂?렪봲갌-1
int ObjectManager::AddWeaponIndex(pointdata data)
{
	//븧딇붥뜂궴뭙릶귩돹뱋?
	int WeaponID = data.p2;
	int nbs = PointParamToInt(data.p3);

	//깋깛??궶븧딇궶귞궽
	if( data.p1 == POINT_P1TYPE_RAND_WEAPON ){
		WeaponParameter WeaponParam;

		if( GetRand(2) == 0 ){
			if( GameParamInfo->GetWeapon(data.p2, &WeaponParam) == 1 ){ return -1; }
			nbs = (int)WeaponParam.nbsmax * TOTAL_WEAPON_AUTOBULLET;		//뭙릶뤵룕궖
		}
		else{
			if( GameParamInfo->GetWeapon(data.p3, &WeaponParam) == 1 ){ return -1; }
			WeaponID = data.p3;						//븧딇붥뜂뤵룕궖
			nbs = (int)WeaponParam.nbsmax * TOTAL_WEAPON_AUTOBULLET;		//뭙릶뤵룕궖
		}
	}

	//NONE궶귞궽렪봲
	if( WeaponID == ID_WEAPON_NONE ){ return -1; }

	//긾긢깑궴긡긏긚?긿귩롦벦
	int model, texture;
	if( Resource->GetWeaponModelTexture(WeaponID, &model, &texture) == 1 ){
		return -1;
	}

	//먠믦뭠귩롦벦
	WeaponParameter WeaponParam;
	if( GameParamInfo->GetWeapon(WeaponID, &WeaponParam) == 1 ){ return -1; }

	for(int i=0; i<MAX_WEAPON; i++){
		if( WeaponIndex[i].GetEnableFlag() == false ){
			//룊딖돸궥귡
			WeaponIndex[i].SetPosData(data.x, data.y, data.z, data.r);
			WeaponIndex[i].SetParamData(WeaponID, 0, nbs, true);
			WeaponIndex[i].SetModel(model, WeaponParam.size);
			WeaponIndex[i].SetTexture(texture);
			WeaponIndex[i].SetEnableFlag(true);
			WeaponIndex[i].RunReload();
			return i;
		}
	}
	return -1;
}

//! @brief 돹몒븧딇믁돿
//! @param WeaponID 븧딇궻롰쀞붥뜂
//! @param loadbullet 뭙귩깓?긤궥귡
//! @return 맟뚻갌긢??붥뜂걁0댥뤵걂?렪봲갌-1
//! @attention 릐궕띍룊궔귞룋렃궢궲궋귡븧딇귩갂븧딇긢??걁긆긳긙긃긏긣걂궴궢궲믁돿룉뿚궥귡궫귕궻듫릶궳궥갃
//! @attention 븧딇뭁뫬궴궢궲봹뭫궥귡뤾뜃궼갂AddWeaponIndex()듫릶귩럊뾭궢궲궘궬궠궋갃
int ObjectManager::AddVisualWeaponIndex(int WeaponID, bool loadbullet)
{
	//NONE궶귞궽렪봲
	if (WeaponID == ID_WEAPON_NONE) { return -1; }

	//륃뺪귩롦벦
	WeaponParameter WeaponParam;
	if (GameParamInfo->GetWeapon(WeaponID, &WeaponParam) == 1) { return -1; }

	//긾긢깑궴긡긏긚?긿귩롦벦
	int model, texture;
	if (Resource->GetWeaponModelTexture(WeaponID, &model, &texture) == 1) {
		return -1;
	}

	for (int i = 0; i < MAX_WEAPON; i++) {
		if (WeaponIndex[i].GetEnableFlag() == false) {
			//룊딖돸
			WeaponIndex[i].SetPosData(0.0f, 0.0f, 0.0f, 0.0f);
			if (loadbullet == false) {
				WeaponIndex[i].SetParamData(WeaponID, 0, 0, true);
			}
			else {
				// ✨ [수정] 장탄수에 따른 예비 탄약 차등 지급
				int total_ammo;
				int nbsmax = WeaponParam.nbsmax; // 가독성을 위해 장탄수 변수 사용

				if (WeaponID == ID_WEAPON_SHIELD) {
					// 방패는 AI가 빈 무기로 판단하지 않도록 1발만 가진 것으로 처리한다.
					// 실제 발사는 object.cpp의 human::ShotWeapon()에서 차단한다.
					total_ammo = 1;
				}
				else if (nbsmax >= 7 && nbsmax <= 10) {
					// 장탄수 7~10: 예비 탄약 3배 (총 4배)
					total_ammo = nbsmax * 4;
				}
				else if (nbsmax >= 4 && nbsmax <= 6) {
					// 장탄수 4~6: 예비 탄약 5배 (총 6배)
					total_ammo = nbsmax * 5;
				}
				else if (nbsmax >= 2 && nbsmax <= 3) {
					// 장탄수 2~3: 예비 탄약 10배 (총 11배)
					total_ammo = nbsmax * 11;
				}
				else if (WeaponID == ID_WEAPON_IMPACT_GRENADE) {
					// 충격 수류탄은 1발 장전 + 총 5개 보유
					total_ammo = 5;
				}
				else {
					// 그 외 (장탄수 1, 11 이상)는 기존 로직
					total_ammo = nbsmax * TOTAL_WEAPON_AUTOBULLET;
				}

				// 수정된 총 탄약(total_ammo)을 SetParamData에 전달
				WeaponIndex[i].SetParamData(WeaponID, nbsmax, total_ammo, true);
			}
			WeaponIndex[i].SetModel(model, WeaponParam.size);
			WeaponIndex[i].SetTexture(texture);
			WeaponIndex[i].SetEnableFlag(true);
			return i;
		}
	}
	return -1;
}

//! @brief 룷븿믁돿
//! @param data 룷븿궻?귽깛긣긢???걁pointdata?몾뫬걂
//! @return 맟뚻갌긢??붥뜂걁0댥뤵걂?렪봲갌-1
int ObjectManager::AddSmallObjectIndex(pointdata data)
{
	//긾긢깑궴긡긏긚?긿귩롦벦
	int model, texture;
	if( Resource->GetSmallObjectModelTexture(data.p2, &model, &texture) == 1 ){
		return -1;
	}

	for(int i=0; i<MAX_SMALLOBJECT; i++){
		if( SmallObjectIndex[i].GetEnableFlag() == false ){
			//룊딖돸
			SmallObjectIndex[i].SetPosData(data.x, data.y, data.z, data.r);
			SmallObjectIndex[i].SetParamData(data.p2, data.p4, true);
			SmallObjectIndex[i].SetModel(model, SMALLOBJECT_SCALE);
			SmallObjectIndex[i].SetTexture(texture);
			SmallObjectIndex[i].SetEnableFlag(true);

			//댧뭫뢇맫긲깋긐궕뾎뚼궶귞궽갂?긞긵궴뵽믦
			if( data.p3 !=0 ){
				SmallObjectIndex[i].CollisionMap(CollD);
			}
			return i;
		}
	}
	return -1;
}

//! @brief 룷븿믁돿걁긒??뭷뾭걂
//! @param px X띆뷭
//! @param py Y띆뷭
//! @param pz Z띆뷭
//! @param rx X렡뛀궖
//! @param paramID 롰쀞붥뜂
//! @param MapColl 댧뭫뢇맫긲깋긐
//! @return 맟뚻갌긢??붥뜂걁0댥뤵걂?렪봲갌-1
int ObjectManager::AddSmallObjectIndex(float px, float py, float pz, float rx, int paramID, bool MapColl)
{
	//긾긢깑궴긡긏긚?긿귩롦벦
	int model, texture;
	if( Resource->GetSmallObjectModelTexture(paramID, &model, &texture) == 1 ){
		return -1;
	}

	for(int i=0; i<MAX_SMALLOBJECT; i++){
		if( SmallObjectIndex[i].GetEnableFlag() == false ){
			//룊딖돸
			SmallObjectIndex[i].SetPosData(px, py, pz, rx);
			SmallObjectIndex[i].SetParamData(paramID, 0, true);
			SmallObjectIndex[i].SetModel(model, SMALLOBJECT_SCALE);
			SmallObjectIndex[i].SetTexture(texture);
			SmallObjectIndex[i].SetEnableFlag(true);

			//댧뭫뢇맫긲깋긐궕뾎뚼궶귞궽갂?긞긵궴뵽믦
			if( MapColl == true ){
				SmallObjectIndex[i].CollisionMap(CollD);
			}

			//깓긐듫똚궻룉뿚
			ObjectLog->AddSmallObject(i);

			return i;
		}
	}
	return -1;
}

//! @brief 긄긲긃긏긣믁돿
//! @param pos_x X띆뷭
//! @param pos_y Y띆뷭
//! @param pos_z Z띆뷭
//! @param move_x X렡댷벍쀊
//! @param move_y Y렡댷벍쀊
//! @param move_z Z렡댷벍쀊
//! @param addmove_y Y렡댷벍쀊궻돿럁쀊
//! @param rotation 됷?둷뱗
//! @param addrotation 됷?둷뱗궻돿럁쀊
//! @param size ?됪?뿦
//! @param addsize ?됪?뿦궻돿럁쀊
//! @param texture 긡긏긚?긿궻긢??붥뜂
//! @param brightness 뼻귡궠
//! @param addbrightness 뼻귡궠궻돿럁쀊
//! @param alpha 벁뼻뱗
//! @param addalpha 벁뼻뱗궻돿럁쀊
//! @param settype 긄긲긃긏긣궻롰쀞?걁Effect_Type 귩멺귒뜃궧귡걂
//! @param count ?됪긲깒??릶
//! @return 맟뚻갌긢??붥뜂걁0댥뤵걂?렪봲갌-1
int ObjectManager::AddEffect(
	float pos_x, float pos_y, float pos_z,
	float move_x, float move_y, float move_z,
	float addmove_y,
	float rotation, float addrotation,
	float size, float addsize,
	int texture,
	int brightness, int addbrightness,
	float alpha, float addalpha,
	int settype,
	int count
)
{
	for (int i = 0; i < MAX_EFFECT; i++) {
		int idx = (RuntimeEffectLastIndex + i) % MAX_EFFECT;

		if (EffectIndex[idx].GetEnableFlag() == false) {
			RuntimeEffectLastIndex = (idx + 1) % MAX_EFFECT;

			EffectIndex[idx].SetPosData(pos_x, pos_y, pos_z, 0.0f);
			EffectIndex[idx].SetParamData(
				move_x, move_y, move_z,
				addmove_y,
				rotation, addrotation,
				size, addsize,
				count,
				settype
			);
			EffectIndex[idx].SetTexture(texture, brightness, addbrightness, alpha, addalpha);
			EffectIndex[idx].SetEnableFlag(true);
			return idx;
		}
	}

	return -1;
}

//! @brief 걁긮깑??긤궳궶궋걂긄긲긃긏긣믁돿
//! @param id 긳깓긞긏붥뜂
//! @param face 뽋붥뜂
//! @param pos_x X띆뷭
//! @param pos_y Y띆뷭
//! @param pos_z Z띆뷭
//! @param size ?됪?뿦
//! @param rotation 됷?둷뱗
//! @param texture 긡긏긚?긿궻긢??붥뜂
//! @param brightness 뼻귡궠
//! @param addbrightness 뼻귡궠궻돿럁쀊
//! @param alpha 벁뼻뱗
//! @param addalpha 벁뼻뱗궻돿럁쀊
//! @param count ?됪긲깒??릶
//! @return 맟뚻갌긢??붥뜂걁0댥뤵걂?렪봲갌-1
int ObjectManager::AddMapEffect(int id, int face, float pos_x, float pos_y, float pos_z, float size, float rotation, int texture, int brightness, int addbrightness, float alpha, float addalpha, int count)
{

	blockdata data;
	float vx, vy, vz, rx, ry;

	BlockData->Getdata(&data, id);
	vx = data.material[face].vx;
	vy = data.material[face].vy;
	vz = data.material[face].vz;

	rx = atan2f(vz, vx) * -1 + (float)M_PI;
	ry = atan2f(vy, sqrtf(vx * vx + vz * vz)) * -1;

	for (int i = 0; i < MAX_EFFECT; i++) {
		int idx = (RuntimeMapEffectLastIndex + i) % MAX_EFFECT;

		if (EffectIndex[idx].GetEnableFlag() == false) {
			RuntimeMapEffectLastIndex = (idx + 1) % MAX_EFFECT;

			EffectIndex[idx].SetPosData(pos_x, pos_y, pos_z, 0.0f);
			EffectIndex[idx].SetParamData(0.0f, 0.0f, 0.0f, 0.0f, rotation, 0.0f, size, 0.0f, count, EFFECT_NOBILLBOARD);
			EffectIndex[idx].SetRxRy(rx, ry);
			EffectIndex[idx].SetTexture(texture, brightness, addbrightness, alpha, addalpha);
			EffectIndex[idx].SetEnableFlag(true);
			return idx;
		}
	}
	return -1;
}

//! @brief 뢯뙆궠궧귡
//! @param x X띆뷭
//! @param y Y띆뷭
//! @param z Z띆뷭
//! @param damage ?긽?긙
//! @param flowing 뿬뙆궻뾎뼰
void ObjectManager::SetHumanBlood(float x, float y, float z, int damage, bool flowing)
{
	int addtype;

	if (flowing == false) {
		addtype = EFFECT_NORMAL;
	}
	else {
		addtype = EFFECT_COLLIDEMAP;
	}

	if (GameConfig.GetBloodFlag() == true) {
		AddEffect(x, y, z, 0.0f, 0.0f, 0.0f, 0.0f, DegreeToRadian(10) * GetRand(36), 0.0f, 10.0f, 0.2f, Resource->GetEffectBloodTexture(), 10, 0, 0.5f, -0.04f, addtype, (int)(GAMEFPS * 0.4f));
		if (flowing == true) {
			for (int i = 0; i < (damage / 10); i++) {
				float rx = DegreeToRadian(10) * GetRand(36);
				AddEffect(x + cosf(rx) * 1.0f, y + (float)(GetRand(20) - 10) / 10, z + sinf(rx) * 1.0f, cosf(rx) * 0.5f, GetRand(5) * 0.1f + 0.5f, sinf(rx) * 0.5f, -0.09f, DegreeToRadian(10) * GetRand(36), 0.0f, GetRand(40) * 0.1f + 0.1f, 0.1f, Resource->GetEffectBloodTexture(), 10, 0, 1.0f, 0.0f, addtype, (int)(GAMEFPS * 0.5f));
			}
		}
	}
}

// 방패로 총알을 막았을 때의 피격 효과.
// 혈흔 대신 작은 밝은 충돌 이펙트를 만든다.
void ObjectManager::SetShieldBlockBulletEffect(float x, float y, float z, int damage, float shield_forward_x, float shield_forward_z)
{
	int spark_count = 1;

	if (damage >= 60) {
		spark_count = 2;
	}

	float len = sqrtf(shield_forward_x * shield_forward_x + shield_forward_z * shield_forward_z);
	if (len < 0.001f) {
		shield_forward_x = 0.0f;
		shield_forward_z = 1.0f;
	}
	else {
		shield_forward_x /= len;
		shield_forward_z /= len;
	}

	float shield_side_x = shield_forward_z;
	float shield_side_z = -shield_forward_x;

	// 스파크 시작점은 방패 면에서 아주 살짝 바깥쪽으로만 띄운다.
	// 이동량을 상속하지 않으므로 방패를 관통해 플레이어 쪽으로 흐르지 않는다.
	float spawn_x = x + shield_forward_x * SHIELD_EFFECT_SURFACE_OFFSET;
	float spawn_y = y;
	float spawn_z = z + shield_forward_z * SHIELD_EFFECT_SURFACE_OFFSET;

	for (int i = 0; i < spark_count; i++) {
		float side_speed = ((float)(GetRand(21) - 10) / 10.0f) * SHIELD_SPARK_SIDE_SPEED;
		float outward_speed = SHIELD_SPARK_OUTWARD_SPEED + 0.002f * (float)GetRand(3);

		AddEffect(
			spawn_x,
			spawn_y,
			spawn_z,
			shield_forward_x * outward_speed + shield_side_x * side_speed,
			0.006f + 0.002f * (float)GetRand(3),
			shield_forward_z * outward_speed + shield_side_z * side_speed,
			-0.004f,
			DegreeToRadian(10) * GetRand(36),
			0.0f,
			0.38f,
			-0.060f,
			Resource->GetEffectMflashTexture(),
			20,
			-1,
			0.28f,
			-0.14f,
			EFFECT_NORMAL,
			SHIELD_SPARK_LIFE
		);
	}

	// 연기는 방패가 밀릴 때 가장 어긋나 보이기 쉬우므로 매우 짧고 약하게만 남긴다.
	AddEffect(
		spawn_x,
		spawn_y,
		spawn_z,
		shield_forward_x * 0.001f,
		0.002f,
		shield_forward_z * 0.001f,
		0.0f,
		DegreeToRadian(10) * GetRand(36),
		0.0f,
		0.20f,
		0.004f,
		Resource->GetEffectSmokeTexture(),
		14,
		0,
		0.07f,
		-0.060f,
		EFFECT_NORMAL,
		SHIELD_SMOKE_LIFE
	);
}

static bool ResolveShieldFrontHumanOverlap(human* shieldHuman, human* otherHuman)
{
	if (SHIELD_ENABLE_FRONT_HUMAN_PUSH == false) { return false; }
	if (shieldHuman == NULL) { return false; }
	if (otherHuman == NULL) { return false; }

	if (IsHumanShieldEquippedForBullet(shieldHuman) == false) {
		return false;
	}

	if (shieldHuman->GetEnableFlag() == false) { return false; }
	if (otherHuman->GetEnableFlag() == false) { return false; }
	if (shieldHuman->GetHP() <= 0) { return false; }
	if (otherHuman->GetHP() <= 0) { return false; }
	if (shieldHuman->GetDeadFlag() == true) { return false; }
	if (otherHuman->GetDeadFlag() == true) { return false; }

	float sx, sy, sz, srx;
	float ox, oy, oz;

	shieldHuman->GetPosData(&sx, &sy, &sz, &srx);
	otherHuman->GetPosData(&ox, &oy, &oz, NULL);

	// 높이 차이가 큰 경우에는 다른 층/계단 상황일 가능성이 있으므로 보정하지 않는다.
	if (fabsf(oy - sy) > SHIELD_HUMAN_PUSH_MAX_Y_DIFF) {
		return false;
	}

	float shield_forward_x = cosf(srx * -1.0f + (float)M_PI / 2.0f);
	float shield_forward_z = sinf(srx * -1.0f + (float)M_PI / 2.0f);
	float shield_right_x = shield_forward_z;
	float shield_right_z = -shield_forward_x;

	float rel_x = ox - sx;
	float rel_z = oz - sz;

	float side = rel_x * shield_right_x + rel_z * shield_right_z;
	float front = rel_x * shield_forward_x + rel_z * shield_forward_z;

	float side_limit = SHIELD_APPROX_HALF_WIDTH + SHIELD_HUMAN_PUSH_SIDE_MARGIN;

	// 방패 정면 영역 안에 있는 경우에만 처리한다.
	if (front <= 0.0f) { return false; }
	if (fabsf(side) > side_limit) { return false; }

	// 다른 인간의 중심이 방패 앞면에 너무 가까우면 앞으로 밀어낸다.
	if (front >= SHIELD_HUMAN_PUSH_MIN_FRONT) {
		return false;
	}

	float push = (SHIELD_HUMAN_PUSH_MIN_FRONT - front) * SHIELD_HUMAN_PUSH_RATE;

	if (push <= 0.0f) { return false; }

	if (push > SHIELD_HUMAN_PUSH_MAX_PER_FRAME) {
		push = SHIELD_HUMAN_PUSH_MAX_PER_FRAME;
	}

	float push_angle = atan2f(shield_forward_z, shield_forward_x);

	otherHuman->AddPosOrder(push_angle, 0.0f, push);

	return true;
}

//! @brief 릐벏럐궻뱰궫귟뵽믦
//! @param in_humanA 뫮뤭궻릐긆긳긙긃긏긣A
//! @param in_humanB 뫮뤭궻릐긆긳긙긃긏긣B
//! @return 뱰궫궯궲궋귡갌true?뱰궫궯궲궋궶궋갌false
//! @warning in_humanA궴in_humanB궳뗦빶궼궇귟귏궧귪갃
//! @warning in_humanA궴in_humanB궳궻멺귒뜃궧궼갂1긲깒??듩궸1뱗궬궚렳뛱궢궲궘궬궠궋갃
//! @attention 뿼긏깋긚궼렔벍밒궸AddPosOrder()귩뾭궋궲갂궓뚚궋귩돓궢뜃궋귏궥갃
bool ObjectManager::CollideHuman(human* in_humanA, human* in_humanB)
{
	if (in_humanA == NULL) { return false; }
	if (in_humanB == NULL) { return false; }

	float h1_x, h1_y, h1_z;
	float h2_x, h2_y, h2_z;
	float angle, length;
	bool collided = false;

	//룊딖돸궠귢궲궋궶궋궔갂?뻊궢궲볺귢궽뵽믦궢궶궋
	if (in_humanA->GetEnableFlag() == false) { return false; }
	if (in_humanB->GetEnableFlag() == false) { return false; }
	if (in_humanA->GetHP() <= 0) { return false; }
	if (in_humanB->GetHP() <= 0) { return false; }

	//궓뚚궋궻띆뷭귩롦벦
	in_humanA->GetPosData(&h1_x, &h1_y, &h1_z, NULL);
	in_humanB->GetPosData(&h2_x, &h2_y, &h2_z, NULL);

	//?뭽궻뱰궫귟뵽믦
	if (CollideCylinder(h1_x, h1_y, h1_z, HUMAN_HUMANCOLLISION_R, HUMAN_HEIGHT - 0.5f, h2_x, h2_y, h2_z, HUMAN_HUMANCOLLISION_R, HUMAN_HEIGHT - 0.5f, &angle, &length) == true) {
		//귕귟뜛귪궬빁궬궚돓궢뢯궥
		in_humanA->AddPosOrder(angle, 0.0f, length / 2);
		in_humanB->AddPosOrder(angle + (float)M_PI, 0.0f, length / 2);
		collided = true;
	}

	// 방패를 든 인간의 정면에 다른 인간/좀비가 너무 파고들면,
	// 방패 앞쪽으로 약하게 밀어내서 모델 겹침을 줄인다.
	if (ResolveShieldFrontHumanOverlap(in_humanA, in_humanB) == true) {
		collided = true;
	}
	if (ResolveShieldFrontHumanOverlap(in_humanB, in_humanA) == true) {
		collided = true;
	}

	return collided;
}

//! @brief 뭙궻뱰궫귟뵽믦궴룉뿚
//! @param in_bullet 뫮뤭궻뭙긆긳긙긃긏긣
//! @return 뱰궫궯궫갌true?뱰궫궯궲궋궶궋갌false
//! @attention 뵽믦귩뛱궎뫮뤭궼걏?긞긵걐걏릐걁벆갋뤵뵾릆갋돷뵾릆걂걐걏룷븿걐궳궥갃
//! @attention 뵽믦궸뙽귞궦갂?긽?긙똶럁귘뚼됈돶띋맯귏궳덇듥궢궲뛱궋귏궥갃
bool ObjectManager::CollideBullet(bullet* in_bullet)
{
	if (in_bullet == NULL) { return false; }

	//럊뾭궠귢궲궋궶궋뭙듴궶귞궽갂룉뿚궧궦궸뺅궥갃
	if (in_bullet->GetEnableFlag() == false) { return false; }

	int objectID;
	float bx, by, bz;
	float brx, bry;
	int attacks;
	int penetration;
	float speed;
	int teamid;
	int humanid;
	float ontargetcnt;
	float vx, vy, vz;
	bool CollideFlag = false;
	int mapflag = 0;		//0갌긳깓긞긏궶궢갂1갌긳깓긞긏궇귟뼟뵽믦갂2갌긳깓긞긏궇귟뵽믦띙귒

	float Dist;

	//뭙긆긳긙긃긏긣궻긢??붥뜂귩롦벦
	objectID = GetBulletObjectID(in_bullet);

	//뭙듴궻띆뷭귩롦벦궢갂긹긏긣깑귩럁뢯갃
	in_bullet->GetPosData(&bx, &by, &bz, &brx, &bry);
	in_bullet->GetParamData(&attacks, &penetration, &speed, &teamid, &humanid, &ontargetcnt);
	bool bullet_silencer = in_bullet->GetSilencerFlag();
	vx = cosf(brx) * cosf(bry);
	vy = sinf(bry);
	vz = sinf(brx) * cosf(bry);

	//?긞긵궴궻뱰궫귟뵽믦걁뭙벞뤵궸긳깓긞긏궕궇귡궔걂
	if (CollD->CheckALLBlockIntersectRay(bx, by, bz, vx, vy, vz, NULL, NULL, &Dist, speed) == true) {
		mapflag = 1;
	}

	// BULLET_SPEEDSCALE믦릶 궻빁됶?귩럁뢯
	int maxcnt = (int)roundf(speed / BULLET_SPEEDSCALE);

	// 방패를 관통한 총알이 다음 분할 검사(cnt)에서 머리에 닿아도
	// 헤드샷으로 처리되지 않게 하기 위한 총알 1발 단위 기록.
	// CollideBullet() 한 번 안에서만 유지되므로 다음 총알에는 영향이 없다.
	bool shield_penetrated_human_flags[MAX_HUMAN];

	for (int i = 0; i < MAX_HUMAN; i++) {
		shield_penetrated_human_flags[i] = false;
	}

	// BULLET_SPEEDSCALE믦릶 궻빁됶?궳뱰궫귟뵽믦
	for (int cnt = 0; cnt < maxcnt; cnt++) {
		//듥믅쀍궕럄궯궲궋궶궚귢궽뭙궼뼰뚼궸궥귡걁뵽믦 1됷뽞걂
		if (penetration < 0) {
			in_bullet->SetEnableFlag(false);
			mapflag = 0;
			break;
		}

		float prev_bvx, prev_bvy, prev_bvz;
		prev_bvx = bx + vx * BULLET_SPEEDSCALE * cnt;
		prev_bvy = by + vy * BULLET_SPEEDSCALE * cnt;
		prev_bvz = bz + vz * BULLET_SPEEDSCALE * cnt;

		float bvx, bvy, bvz;
		bvx = bx + vx * BULLET_SPEEDSCALE * (cnt + 1);
		bvy = by + vy * BULLET_SPEEDSCALE * (cnt + 1);
		bvz = bz + vz * BULLET_SPEEDSCALE * (cnt + 1);

		//릐궴궻뱰궫귟뵽믦
		for (int i = 0; i < MAX_HUMAN; i++) {
			if (i == humanid) { continue; }
			if (HumanIndex[i].GetEnableFlag() == false) { continue; }
			if (HumanIndex[i].GetHP() <= 0) { continue; }
			if (HumanIndex[i].GetDeadFlag() == true) { continue; }

			if (BulletObj_HumanIndex[objectID].GetIndexFlag(i) == true) { continue; }

			//띆뷭귩롦벦
			float ox, oy, oz;
			int h_teamid;
			HumanIndex[i].GetPosData(&ox, &oy, &oz, NULL);
			HumanIndex[i].GetParamData(NULL, NULL, NULL, &h_teamid);

			float rough_collision_radius = HUMAN_BULLETCOLLISION_LEG_R;
			if (IsHumanShieldEquippedForBullet(&HumanIndex[i]) == true) {
				rough_collision_radius = SHIELD_APPROX_FRONT_MAX + SHIELD_APPROX_HALF_WIDTH;
			}

			float seg_min_x = (prev_bvx < bvx) ? prev_bvx : bvx;
			float seg_max_x = (prev_bvx > bvx) ? prev_bvx : bvx;
			float seg_min_z = (prev_bvz < bvz) ? prev_bvz : bvz;
			float seg_max_z = (prev_bvz > bvz) ? prev_bvz : bvz;

			if ((ox < seg_min_x - rough_collision_radius) ||
				(ox > seg_max_x + rough_collision_radius) ||
				(oz < seg_min_z - rough_collision_radius) ||
				(oz > seg_max_z + rough_collision_radius)) {
				continue;
			}

			if (FriendlyFire[i] == false) {
				//벏궣???붥뜂걁뼞뺴걂궶귞룉뿚궢궶궋
				if (h_teamid == teamid) { continue; }
			}

			human* shieldShotHuman = NULL;
			if ((0 <= humanid) && (humanid < MAX_HUMAN)) {
				shieldShotHuman = &HumanIndex[humanid];
			}

			// 방패를 맞고 관통한 총알인지 기록한다.
// 이전 분할 검사(cnt)에서 방패를 관통한 경우도 유지한다.
// 이 경우 머리 히트박스에 닿아도 헤드샷이 아니라 일반 몸통 피격으로 처리한다.
			bool shield_penetrated_this_human = shield_penetrated_human_flags[i];

			// 방패 박스 선행 판정.
			// 사람 히트박스에 들어가기 전에 방패 기준 박스에 먼저 닿았는지 검사한다.
			// 안정화 전까지는 기존 HitBulletHuman(shield_blocked=true) 경로를 재사용한다.
			float shield_hit_x = bvx;
			float shield_hit_y = bvy;
			float shield_hit_z = bvz;

			if (CheckShieldBulletBoxSegmentHit(
				&HumanIndex[i],
				shieldShotHuman,
				prev_bvx,
				prev_bvy,
				prev_bvz,
				bvx,
				bvy,
				bvz,
				vx,
				vz,
				&shield_hit_x,
				&shield_hit_y,
				&shield_hit_z
			) == true) {
				int original_attacks = attacks;
				int original_penetration = penetration;

				int hit_attacks = CalculateShieldDamageFromBulletPenetration(
					original_attacks,
					original_penetration
				);

				// CheckShieldBulletBoxSegmentHit()에서 이미 방패 박스/정면 판정이 끝났으므로
				// 여기서 다시 IsBulletBlockedByShieldFront()를 호출하지 않는다.
				// 재검사 오차 때문에 관통 상황에서 방패 효과음이 빠지는 문제를 막는다.
				HitBulletShieldOnly(i, shield_hit_x, shield_hit_y, shield_hit_z, hit_attacks, humanid);

				bool shield_stopped_bullet = ApplyShieldBlockToBulletState(
					original_attacks,
					original_penetration,
					hit_attacks,
					&attacks,
					&penetration
				);

				CollideFlag = true;

				if (shield_stopped_bullet == true) {
					BulletObj_HumanIndex[objectID].Init();
					BulletObj_HumanIndex[objectID].SetIndexFlag(i);
					in_bullet->SetEnableFlag(false);
					mapflag = 0;
					break;
				}

				// 관통력이 충분하면 약화된 총알로 아래 신체 히트박스 검사를 계속한다.
// 이때 머리에 닿아도 헤드샷이 아니라 일반 피격으로 처리한다.
// 같은 총알이 다음 분할 검사에서 머리에 닿는 경우도 있으므로 human id 기준으로 기록한다.
				shield_penetrated_this_human = true;
				shield_penetrated_human_flags[i] = true;
			}

			float hit_leg_y, hit_leg_h;
			float hit_up_y, hit_up_h;
			float hit_head_y, hit_head_h;

			GetHumanBulletHitboxParts(
				&HumanIndex[i],
				oy,
				&hit_leg_y,
				&hit_leg_h,
				&hit_up_y,
				&hit_up_h,
				&hit_head_y,
				&hit_head_h
			);

			// 머리
			if (CollideCylinderInside(ox, hit_head_y, oz, HUMAN_BULLETCOLLISION_HEAD_R, hit_head_h, bvx, bvy, bvz) == true) {
				human* shotHuman = NULL;
				if ((0 <= humanid) && (humanid < MAX_HUMAN)) {
					shotHuman = &HumanIndex[humanid];
				}

				bool shield_blocked = false;

				int hit_attacks = ApplyShieldFallbackDefenseToBulletDamage(
					&HumanIndex[i],
					shotHuman,
					attacks,
					penetration,
					vx,
					vz,
					bvx,
					bvy,
					bvz,
					&shield_blocked
				);

				int hit_part = 0;
				float pass_rate = 0.5f;

				// 방패를 관통한 뒤 머리에 닿은 경우에는 헤드샷으로 보지 않고 일반 몸통 피격으로 처리한다.
				// 방패 뒤 머리 판정이 완전히 무효가 되는 문제를 막으면서, 헤드샷 보너스는 주지 않는다.
				if (shield_penetrated_this_human == true) {
					hit_part = 1;
					pass_rate = 0.6f;
				}

				HitBulletHuman(i, hit_part, bvx, bvy, bvz, brx, hit_attacks, humanid, ontargetcnt, shield_blocked, bullet_silencer);
				BulletObj_HumanIndex[objectID].Init();
				BulletObj_HumanIndex[objectID].SetIndexFlag(i);
				attacks = (int)((float)hit_attacks * pass_rate);
				penetration -= 1;
				CollideFlag = true;
			}

			// 몸통
						// 몸통
			if (CollideCylinderInside(ox, hit_up_y, oz, HUMAN_BULLETCOLLISION_UP_R, hit_up_h, bvx, bvy, bvz) == true) {
				human* shotHuman = NULL;
				if ((0 <= humanid) && (humanid < MAX_HUMAN)) {
					shotHuman = &HumanIndex[humanid];
				}

				bool shield_blocked = false;

				int hit_attacks = ApplyShieldFallbackDefenseToBulletDamage(
					&HumanIndex[i],
					shotHuman,
					attacks,
					penetration,
					vx,
					vz,
					bvx,
					bvy,
					bvz,
					&shield_blocked
				);

				HitBulletHuman(i, 1, bvx, bvy, bvz, brx, hit_attacks, humanid, ontargetcnt, shield_blocked, bullet_silencer);
				BulletObj_HumanIndex[objectID].Init();
				BulletObj_HumanIndex[objectID].SetIndexFlag(i);
				attacks = (int)((float)hit_attacks * 0.6f);
				penetration -= 1;
				CollideFlag = true;
			}

			// 다리
			if (CollideCylinderInside(ox, hit_leg_y, oz, HUMAN_BULLETCOLLISION_LEG_R, hit_leg_h, bvx, bvy, bvz) == true) {
				human* shotHuman = NULL;
				if ((0 <= humanid) && (humanid < MAX_HUMAN)) {
					shotHuman = &HumanIndex[humanid];
				}

				bool shield_blocked = false;

				int hit_attacks = ApplyShieldFallbackDefenseToBulletDamage(
					&HumanIndex[i],
					shotHuman,
					attacks,
					penetration,
					vx,
					vz,
					bvx,
					bvy,
					bvz,
					&shield_blocked
				);

				HitBulletHuman(i, 2, bvx, bvy, bvz, brx, hit_attacks, humanid, ontargetcnt, shield_blocked, bullet_silencer);
				BulletObj_HumanIndex[objectID].Init();
				BulletObj_HumanIndex[objectID].SetIndexFlag(i);
				attacks = (int)((float)hit_attacks * 0.7f);
				penetration -= 1;
				CollideFlag = true;
			}
		}

		if (penetration < 0) {
			in_bullet->SetEnableFlag(false);
			mapflag = 0;
			break;
		}

		//룷븿궴궻뱰궫귟뵽믦
		for (int i = 0; i < MAX_SMALLOBJECT; i++) {
			if (SmallObjectIndex[i].GetEnableFlag() == false) { continue; }
			if (SmallObjectIndex[i].GetHP() <= 0) { continue; }

			float ox, oy, oz;
			int id;
			float decide;
			SmallObjectIndex[i].GetPosData(&ox, &oy, &oz, NULL);
			SmallObjectIndex[i].GetParamData(&id, NULL);

			if ((TOTAL_PARAMETERINFO_SMALLOBJECT <= id) && (id <= (TOTAL_PARAMETERINFO_SMALLOBJECT + MAX_ADDSMALLOBJECT - 1))) {
				decide = (float)MIFdata->GetAddSmallobjectDecide(id - TOTAL_PARAMETERINFO_SMALLOBJECT) * SMALLOBJECT_COLLISIONSCALE;
			}
			else {
				SmallObjectParameter Param;
				GameParamInfo->GetSmallObject(id, &Param);
				decide = (float)Param.decide * SMALLOBJECT_COLLISIONSCALE;
			}

			if (CollideSphereInside(ox, oy, oz, decide, bvx, bvy, bvz) == true) {
				int attack2 = (int)(floorf((float)attacks * 0.25f));

				// [✨ 핵심 수정 ✨]
				// 총알을 쏜 사람이 플레이어일 경우, 히트마커 타이머를 설정합니다.
				int myid = GetPlayerID();
				if (humanid == myid) {
					GetHumanObject(myid)->SetHitMarkerTimer(8);
				}

				HitBulletSmallObject(i, bvx, bvy, bvz, attack2, teamid);
				attacks = (int)(floorf((float)attacks * 0.7f));
				CollideFlag = true;
			}
		}

		if (mapflag > 0) {
			if (CollD->CheckALLBlockInside(bvx, bvy, bvz) == true) {
				HitBulletMap(bx + vx * (Dist - 1.0f), by + vy * (Dist - 1.0f), bz + vz * (Dist - 1.0f), teamid);
				penetration -= 1;
				if (penetration >= 0) {
					attacks = (int)((float)attacks * 0.6f);
				}
				mapflag = 2;
				CollideFlag = true;
			}
		}
	}

	//뭙벞뤵궸긳깓긞긏궕궇귡궸귖듫귦귞궦갂긳깓긞긏궴뱰궫귟뵽믦귩뛱궯궲궋궶궋궶귞궽
	//?걁걖뵔궋긳깓긞긏궕궇귡걂
	if( mapflag == 1 ){
		//뭙궕?긞긵궸뱰궫궯궫룉뿚
		HitBulletMap(bx + vx*(Dist-1.0f), by + vy*(Dist-1.0f), bz + vz*(Dist-1.0f), teamid);

		//뛘똼쀍귩똶럁
		if( penetration > 0 ){
			attacks = (int)((float)attacks * 0.75f);
		}
		else{
			attacks = (int)((float)attacks * 0.55f);
		}

		CollideFlag = true;
	}

	//먠믦귩밙뾭걁벫궸뛘똼쀍갋듥믅쀍걂
	in_bullet->SetParamData(attacks, penetration, speed, teamid, humanid, ontargetcnt, false);

	return CollideFlag;
}

//! @brief 뭙궕?긞긵궸뱰궫궯궫룉뿚
//! @param x 뭶뭙X띆뷭
//! @param y 뭶뭙Y띆뷭
//! @param z 뭶뭙Z띆뷭
//! @param teamID 뵯롅뙰궻???붥뜂
void ObjectManager::HitBulletMap(float x, float y, float z, int teamID)
{
	//긄긲긃긏긣걁뎹걂귩?됪
	AddEffect(x, y, z, 0.0f, 0.05f, 0.0f, 0.0f, DegreeToRadian(10)*GetRand(36), 0.0f, 3.0f, 0.24f, Resource->GetEffectSmokeTexture(), 20, 0, 0.63f, -0.03f, EFFECT_NORMAL, 20);

	//뚼됈돶귩띋맯
	GameSound->HitMap(x, y, z, teamID);
}

// 플레이어 스킬용 피해 처리
// gamemain.cpp에서는 private HitBulletHuman()에 직접 접근할 수 없으므로
// 이 public 래퍼를 통해 기존 피격/사망/로그/혈흔/킬 카운트 처리를 그대로 사용한다.
void ObjectManager::SkillDamageHuman(int HitHuman_id, int Hit_id, float x, float y, float z, float brx, int attacks, int Shothuman_id, float ontargetcnt)
{
	HitBulletHuman(
		HitHuman_id,
		Hit_id,
		x,
		y,
		z,
		brx,
		attacks,
		Shothuman_id,
		ontargetcnt,
		false
	);
}

//! @brief 뭙궕릐궸뱰궫궯궫룉뿚
//! @param HitHuman_id 릐궻붥뜂
//! @param Hit_id 뷀뭙됒룋걁벆갌0?벜뫬갌1?뫉갌2걂
//! @param x 뷀뭙X띆뷭
//! @param y 뷀뭙Y띆뷭
//! @param z 뷀뭙Z띆뷭
//! @param brx 릣빟둷뱗
//! @param attacks 뛘똼쀍
//! @param Shothuman_id 뵯롅궢궫릐궻붥뜂
//! @param ontargetcnt 뼺뭷렄궻긇긂깛긣릶
void ObjectManager::AddAIShieldBlockSwitchScore(int HitHuman_id, int shield_damage)
{
	if (AI_SHIELD_AUTO_SWITCH_TO_GUN == false) { return; }

	if ((HitHuman_id < 0) || (HitHuman_id >= MAX_HUMAN)) { return; }
	if (HitHuman_id == Player_HumanID) { return; }

	if (Human_ShieldBlockSwitchCount == NULL ||
		Human_ShieldBlockSwitchDamage == NULL ||
		Human_ShieldBlockSwitchCooldown == NULL) {
		return;
	}

	if (HumanIndex[HitHuman_id].GetEnableFlag() == false) { return; }
	if (HumanIndex[HitHuman_id].GetDeadFlag() == true) { return; }

	// 플레이어 임시 방패나 렌더 override가 아니라,
	// AI가 실제 92번 방패를 선택 중인 경우에만 처리한다.
	if (HumanIndex[HitHuman_id].GetMainWeaponTypeNO() != ID_WEAPON_SHIELD) {
		Human_ShieldBlockSwitchCount[HitHuman_id] = 0;
		Human_ShieldBlockSwitchDamage[HitHuman_id] = 0;
		return;
	}

	if (Human_ShieldBlockSwitchCooldown[HitHuman_id] > 0) {
		return;
	}

	int selectweapon = 0;
	class weapon* weaponlist[TOTAL_HAVEWEAPON];

	for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
		weaponlist[i] = NULL;
	}

	HumanIndex[HitHuman_id].GetWeapon(&selectweapon, weaponlist, NULL, NULL);

	int switch_slot = -1;

	for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
		if (i == selectweapon) { continue; }
		if (weaponlist[i] == NULL) { continue; }

		int weapon_id = ID_WEAPON_NONE;
		int load_bullets = 0;
		int total_bullets = 0;

		weaponlist[i]->GetParamData(&weapon_id, &load_bullets, &total_bullets);

		if (weapon_id == ID_WEAPON_NONE) { continue; }
		if (weapon_id == ID_WEAPON_SHIELD) { continue; }
		if (weapon_id == ID_WEAPON_GRENADE) { continue; }
		if (weapon_id == ID_WEAPON_CASE) { continue; }

		// 방패에서 총으로 돌아갈 때는 즉시 쏠 수 있는 총만 선택한다.
// 장전 탄이 0인 총을 선택하면 AI가 곧바로 다시 방패로 돌아가는 반복이 생길 수 있다.
		if (load_bullets <= 0) { continue; }

		switch_slot = i;
		break;
	}

	if (switch_slot < 0) {
		return;
	}

	if (shield_damage < 1) {
		shield_damage = 1;
	}

	Human_ShieldBlockSwitchCount[HitHuman_id] += 1;
	Human_ShieldBlockSwitchDamage[HitHuman_id] += shield_damage;

	if (Human_ShieldBlockSwitchCount[HitHuman_id] >= AI_SHIELD_SWITCH_BLOCK_COUNT ||
		Human_ShieldBlockSwitchDamage[HitHuman_id] >= AI_SHIELD_SWITCH_BLOCK_DAMAGE) {

		ChangeHaveWeapon(HitHuman_id, switch_slot);

		Human_ShieldBlockSwitchCount[HitHuman_id] = 0;
		Human_ShieldBlockSwitchDamage[HitHuman_id] = 0;
		Human_ShieldBlockSwitchCooldown[HitHuman_id] = AI_SHIELD_SWITCH_COOLDOWN_FRAMES;
	}
}

void ObjectManager::HitBulletShieldOnly(int HitHuman_id, float x, float y, float z, int visual_damage, int Shothuman_id)
{
	if ((HitHuman_id < 0) || (HitHuman_id >= MAX_HUMAN)) { return; }
	if (HumanIndex[HitHuman_id].GetEnableFlag() == false) { return; }
	if (HumanIndex[HitHuman_id].GetDeadFlag() == true) { return; }

	int Shothuman_TeamID = -1;

	if ((0 <= Shothuman_id) && (Shothuman_id < MAX_HUMAN)) {
		HumanIndex[Shothuman_id].GetParamData(NULL, NULL, NULL, &Shothuman_TeamID);
	}

	if (visual_damage < 1) {
		visual_damage = 1;
	}

	float effect_x = x;
	float effect_y = y;
	float effect_z = z;

	GetShieldBlockSurfacePoint(&HumanIndex[HitHuman_id], x, y, z, &effect_x, &effect_y, &effect_z);

	float hx, hy, hz, hrx;
	HumanIndex[HitHuman_id].GetPosData(&hx, &hy, &hz, &hrx);

	float shield_forward_x = cosf(hrx * -1.0f + (float)M_PI / 2.0f);
	float shield_forward_z = sinf(hrx * -1.0f + (float)M_PI / 2.0f);

	// 방패 박스 선행 판정으로 막은 경우에는 HitBulletHuman()을 거치지 않으므로,
	// 여기서 직접 방패 피격 플래그를 세워 HUD 흰색 원호가 뜨게 한다.
	float hitflag_rx = hrx;

	if ((0 <= Shothuman_id) && (Shothuman_id < MAX_HUMAN)) {
		float shooter_x, shooter_y, shooter_z;

		HumanIndex[Shothuman_id].GetPosData(&shooter_x, &shooter_y, &shooter_z, NULL);
		hitflag_rx = atan2f(shooter_z - hz, shooter_x - hx);
	}

	HumanIndex[HitHuman_id].SetShieldHitFlag(hitflag_rx);

	SetShieldBlockBulletEffect(effect_x, effect_y, effect_z, visual_damage, shield_forward_x, shield_forward_z);
	GameSound->HitShieldBlock(effect_x, effect_y, effect_z, Shothuman_TeamID);

	// AI가 방패로 일정 횟수/피해 이상 막으면 총으로 전환한다.
	// 플레이어 방패에는 적용되지 않는다.
	AddAIShieldBlockSwitchScore(HitHuman_id, visual_damage);
}

bool ObjectManager::CheckShieldHookBlockSegment(
	human* hitHuman,
	human* hookHuman,
	float start_x,
	float start_y,
	float start_z,
	float end_x,
	float end_y,
	float end_z,
	bool play_effect
)
{
	if (hitHuman == NULL) { return false; }

	float hit_x = end_x;
	float hit_y = end_y;
	float hit_z = end_z;

	float hook_vx = end_x - start_x;
	float hook_vz = end_z - start_z;

	if (CheckShieldBulletBoxSegmentHit(
		hitHuman,
		hookHuman,
		start_x,
		start_y,
		start_z,
		end_x,
		end_y,
		end_z,
		hook_vx,
		hook_vz,
		&hit_x,
		&hit_y,
		&hit_z
	) == false) {
		return false;
	}

	if (play_effect == true) {
		float effect_x = hit_x;
		float effect_y = hit_y;
		float effect_z = hit_z;

		GetShieldBlockSurfacePoint(hitHuman, hit_x, hit_y, hit_z, &effect_x, &effect_y, &effect_z);

		float hx, hy, hz, hrx;
		hitHuman->GetPosData(&hx, &hy, &hz, &hrx);

		float shield_forward_x = cosf(hrx * -1.0f + (float)M_PI / 2.0f);
		float shield_forward_z = sinf(hrx * -1.0f + (float)M_PI / 2.0f);

		int attacker_team = -1;
		if (hookHuman != NULL) {
			hookHuman->GetParamData(NULL, NULL, NULL, &attacker_team);
		}

		SetShieldBlockBulletEffect(effect_x, effect_y, effect_z, 20, shield_forward_x, shield_forward_z);
		GameSound->HitShieldBlock(effect_x, effect_y, effect_z, attacker_team);
	}

	return true;
}

void ObjectManager::HitBulletHuman(int HitHuman_id, int Hit_id, float x, float y, float z, float brx, int attacks, int Shothuman_id, float ontargetcnt, bool shield_blocked, bool bullet_silencer)
{
	int Shothuman_TeamID;
	int HitHuman_TeamID;
	int hp_old;
	int damage = 0;
	int paramid;
	HumanParameter Paraminfo;
	bool NotRobot;

	//럊뾭궠귢궲궋궶궋궔갂?뫬궶귞궽룉뿚궢궶궋갃
	if( HumanIndex[HitHuman_id].GetEnableFlag() == false ){ return; }
	if( HumanIndex[HitHuman_id].GetDeadFlag() == true ){ return; }

	//뵯롅뙰궴뫮뤭릐븿궻???붥뜂롦벦
	HumanIndex[Shothuman_id].GetParamData(NULL, NULL, NULL, &Shothuman_TeamID);
	HumanIndex[HitHuman_id].GetParamData(NULL, NULL, NULL, &HitHuman_TeamID);

	//?긽?긙똶럁멟궻HP롦벦
	hp_old = HumanIndex[HitHuman_id].GetHP();

	

	//adjust damage between player and ai
	int newAttacks = attacks;

	/*
	//人にダメージと衝撃を与える
	// 데미지를 입히는 코드를 조건문 밖으로 이동시켜 항상 피해를 받도록 합니다.
	if (HitHuman_id == Player_HumanID) {
		newAttacks *= HUMAN_DAMAGE_MULT_PLAYER_GET;
	}
	else {
		newAttacks *= HUMAN_DAMAGE_MULT_AI_GET;
	}
	newAttacks = (int)newAttacks;
	*/

	// 피격 대상의 종류를 먼저 확인한다.
// type 1 = 로봇, type 2 = 좀비
	HumanIndex[HitHuman_id].GetParamData(&paramid, NULL, NULL, NULL);
	GameParamInfo->GetHuman(paramid, &Paraminfo);

	bool isRobot = false;
	bool isZombie = false;

	if (Paraminfo.type == 1) { isRobot = true; }
	if (Paraminfo.type == 2) { isZombie = true; }

	// 혈흔/스파크 효과용 기존 변수
	if (isRobot == true) {
		NotRobot = false;
	}
	else {
		NotRobot = true;
	}

	bool hit_shadow_decoy = HumanIndex[HitHuman_id].GetSkillShadowDecoyFlag();

	// AI FireRate에 따라 좀비/로봇의 넉백과 피격 경직을 줄인다.
	// 플레이어가 로봇/좀비 타입일 가능성을 고려하여 플레이어에게는 적용하지 않는다.
	float knockback_scale = 1.0f;
	int hitflag_skip_percent = 0;

	if (HitHuman_id != Player_HumanID) {
		float firerate_mult = GameConfig.GetAIFireRateMultiplier();

		if (firerate_mult < 1.0f) { firerate_mult = 1.0f; }
		if (firerate_mult > 4.0f) { firerate_mult = 4.0f; }

		// FireRate 1.0 -> 0.0
		// FireRate 4.0 -> 1.0
		float ai_bonus = (firerate_mult - 1.0f) / 3.0f;

		if (isZombie == true) {
			// 좀비는 접근해야 하므로 넉백 감소를 크게 준다.
			// 최대 FireRate에서 넉백 40% 감소, 경직 발생 25% 감소.
			knockback_scale = 1.0f - 0.60f * ai_bonus;
			hitflag_skip_percent = (int)(45.0f * ai_bonus);
		}
		else if (isRobot == true) {
			// 로봇은 1대1 밸런스를 해치지 않도록 약하게 준다.
			// 최대 FireRate에서 넉백 25% 감소, 경직 발생 10% 감소.
			knockback_scale = 1.0f - 0.25f * ai_bonus;
			hitflag_skip_percent = (int)(20.0f * ai_bonus);
		}

		if (knockback_scale < 0.40f) { knockback_scale = 0.40f; }
		if (hitflag_skip_percent < 0) { hitflag_skip_percent = 0; }
		if (hitflag_skip_percent > 80) { hitflag_skip_percent = 80; }
	}

	// 피해량은 기존 그대로 유지한다.
	if (Hit_id == 0) { HumanIndex[HitHuman_id].HitBulletHead(newAttacks); }
	if (Hit_id == 1) { HumanIndex[HitHuman_id].HitBulletUp(newAttacks); }
	if (Hit_id == 2) { HumanIndex[HitHuman_id].HitBulletLeg(newAttacks); }

	if ((HumanIndex[HitHuman_id].GetSkillShadowDecoyFlag() == true) &&
		(0 <= Shothuman_id) && (Shothuman_id < MAX_HUMAN)) {
		HumanIndex[HitHuman_id].SetSkillShadowDecoyLastAttackerID(Shothuman_id);
	}

	// 용병 전투 흡수: 플레이어가 실제로 준 피해의 10%를 즉시 회복한다.
	if ((0 <= Shothuman_id) && (Shothuman_id < MAX_HUMAN) &&
		(Shothuman_id == Player_HumanID)) {

		int hp_after = HumanIndex[HitHuman_id].GetHP();
		int dealt_damage = hp_old - hp_after;

		if (dealt_damage > 0) {
			int heal_value = (int)((float)dealt_damage * 0.10f + 0.5f);

			if (heal_value < 1) {
				heal_value = 1;
			}

			HumanIndex[Shothuman_id].AddSkillAbsorbHeal(heal_value);
		}
	}

	// 총알 넉백 감소.
	// 기존은 항상 1.0f였다.
	HumanIndex[HitHuman_id].AddPosOrder(brx, 0.0f, 1.0f * knockback_scale);

	// 피격 경직 감소.
	// SetHitFlag 자체를 일정 확률로 생략하여 경직 빈도를 줄인다.
	bool set_hitflag = true;

	if (hitflag_skip_percent > 0) {
		if (GetRand(100) < hitflag_skip_percent) {
			set_hitflag = false;
		}
	}

	if (set_hitflag == true) {
		float hitflag_rx = brx * -1 - (float)M_PI / 2;

		if ((0 <= Shothuman_id) && (Shothuman_id < MAX_HUMAN)) {
			float hit_x, hit_y, hit_z;
			float shooter_x, shooter_y, shooter_z;

			HumanIndex[HitHuman_id].GetPosData(&hit_x, &hit_y, &hit_z, NULL);
			HumanIndex[Shothuman_id].GetPosData(&shooter_x, &shooter_y, &shooter_z, NULL);

			hitflag_rx = atan2f(shooter_z - hit_z, shooter_x - hit_x);
		}

#ifdef ENABLE_BUG_TEAMID
		bool flag = true;

		if ((HitHuman_TeamID < 0) && (Shothuman_TeamID < 0)) {
			if (HitHuman_TeamID < Shothuman_TeamID) {
				flag = false;
			}
		}

		if (flag == true) {
			if (shield_blocked == true) {
				HumanIndex[HitHuman_id].SetShieldHitFlag(hitflag_rx);
			}
			else {
				if (bullet_silencer == true) {
					HumanIndex[HitHuman_id].SetSilentHitFlag(hitflag_rx);
				}
				else {
					HumanIndex[HitHuman_id].SetHitFlag(hitflag_rx);
				}
			}
		}
#else
		if (shield_blocked == true) {
			HumanIndex[HitHuman_id].SetShieldHitFlag(hitflag_rx);
		}
		else {
			if (bullet_silencer == true) {
				HumanIndex[HitHuman_id].SetSilentHitFlag(hitflag_rx);
			}
			else {
				HumanIndex[HitHuman_id].SetHitFlag(hitflag_rx);
			}
	}
#endif
	}

	//긄긲긃긏긣걁뙆걂귩?됪
	if (Hit_id == 0) { damage = (int)((float)newAttacks * HUMAN_DAMAGE_HEAD); }
	if (Hit_id == 1) { damage = (int)((float)newAttacks * HUMAN_DAMAGE_UP); }
	if (Hit_id == 2) { damage = (int)((float)newAttacks * HUMAN_DAMAGE_LEG); }

	if (shield_blocked == true) {
		float effect_x = x;
		float effect_y = y;
		float effect_z = z;

		// 방패 표면 위치 계산은 공통 helper를 사용한다.
		// 방패 박스 크기/위치 상수를 조정해도 이펙트 위치가 같은 기준으로 따라가게 한다.
		GetShieldBlockSurfacePoint(&HumanIndex[HitHuman_id], x, y, z, &effect_x, &effect_y, &effect_z);

		float hx, hy, hz, hrx;
		HumanIndex[HitHuman_id].GetPosData(&hx, &hy, &hz, &hrx);

		float shield_forward_x = cosf(hrx * -1.0f + (float)M_PI / 2.0f);
		float shield_forward_z = sinf(hrx * -1.0f + (float)M_PI / 2.0f);

		SetShieldBlockBulletEffect(effect_x, effect_y, effect_z, damage, shield_forward_x, shield_forward_z);
		GameSound->HitShieldBlock(effect_x, effect_y, effect_z, Shothuman_TeamID);
	}
	else {
		if (hit_shadow_decoy == false) {
			SetHumanBlood(x, y, z, damage, NotRobot);
		}

		GameSound->HitHuman(x, y, z, Hit_id, Shothuman_TeamID);
	}


	if( (0 <= Shothuman_id)&&(Shothuman_id < MAX_HUMAN) ){
		//뭙귩뵯롅궢궫릐궻맟됈궸돿럁
		Human_ontarget[Shothuman_id] += ontargetcnt;
		if( Hit_id == 0 ){ Human_headshot[Shothuman_id] += 1; }
		if( (hp_old > 0)&&(HumanIndex[HitHuman_id].GetHP() <= 0) ){
			//?긽?긙똶럁멟궸 hp>0 궳궔궰갂똶럁뚣궸 hp <= 0 궶귞갂뜞됷?뻊궢궫갃
			Human_kill[Shothuman_id] += 1;
		}
	}

	//깓긐듫똚궻룉뿚
	if( (hp_old > 0)&&(HumanIndex[HitHuman_id].GetHP() <= 0) ){
		//?긽?긙똶럁멟궸 hp>0 궳궔궰갂똶럁뚣궸 hp <= 0 궶귞갂뜞됷?뻊궢궫갃

		int player_teamid;
		HumanIndex[Player_HumanID].GetParamData(NULL, NULL, NULL, &player_teamid);

		ObjectLog->DiedHuman(Shothuman_id, HitHuman_id, Shothuman_TeamID, HitHuman_TeamID, player_teamid);
	}
}

//! @brief 뭙궕룷븿궸뱰궫궯궫룉뿚
//! @param HitSmallObject_id 룷븿궻붥뜂
//! @param x 뭶뭙X띆뷭
//! @param y 뭶뭙Y띆뷭
//! @param z 뭶뭙Z띆뷭
//! @param attacks 뛘똼쀍
//! @param teamID 뵯롅뙰궻???붥뜂
void ObjectManager::HitBulletSmallObject(int HitSmallObject_id, float x, float y, float z, int attacks, int teamID)
{
	int hp;

	//럊뾭궠귢궲궋궶궚귢궽룉뿚궢궶궋갃
	if( SmallObjectIndex[HitSmallObject_id].GetEnableFlag() == false ){ return; }

	//뫬쀍궕궶궚귢궽룉뿚궢궶궋
	hp = SmallObjectIndex[HitSmallObject_id].GetHP();
	if( hp <= 0 ){ return; }

	//룷븿궸?긽?긙귩?궑귡
	SmallObjectIndex[HitSmallObject_id].HitBullet(attacks);

	//긄긲긃긏긣걁뎹걂귩?됪
	//AddEffect(x, y, z, 0.01f*(GetRand(101)-50), 0.01f*(GetRand(101)-50), 0.01f*(GetRand(101)-50), 0.0f, DegreeToRadian(10)*GetRand(36), 0.0f, 1.0f, 0.1f, Resource->GetEffectSmokeTexture(), 10, 0, 0.625f, -0.047f, EFFECT_NORMAL, 255);
	//AddEffect(x, y, z, 0.01f*(GetRand(101)-50), 0.01f*(GetRand(101)-50), 0.01f*(GetRand(101)-50), 0.0f, DegreeToRadian(10)*GetRand(36), 0.0f, 1.0f, 0.1f, Resource->GetEffectSmokeTexture(), 10, 0, 0.625f, -0.047f, EFFECT_NORMAL, 255);
	AddEffect(x, y, z, 0.0f, 0.0f, 0.0f, 0.0f, DegreeToRadian(10)*GetRand(36), 0.0f, 1.0f, 0.1f, Resource->GetEffectSmokeTexture(), 10, 0, 0.625f, -0.047f, EFFECT_NORMAL, 255);
	AddEffect(x, y, z, 0.0f, 0.01f*(GetRand(10)-5), 0.0f, 0.0f, DegreeToRadian(10)*GetRand(36), 0.0f, 1.0f, 0.1f, Resource->GetEffectSmokeTexture(), 10, 0, 0.625f, -0.047f, EFFECT_NORMAL, 255);

	//뚼됈돶귩띋맯
	int id;
	SmallObjectIndex[HitSmallObject_id].GetParamData(&id, NULL);
	GameSound->HitSmallObject(x, y, z, id, teamID);

	//깓긐듫똚궻룉뿚
	hp = SmallObjectIndex[HitSmallObject_id].GetHP();
	if( hp <= 0 ){
		//?긽?긙똶럁멟궸 hp>0 궳궔궰갂똶럁뚣궸 hp <= 0 궶귞갂뜞됷봨됹궠귢궫갃
		ObjectLog->BreakSmallObject(HitSmallObject_id);
	}
}

bool ObjectManager::ExplodeSkillMine(float x, float y, float z, int teamid, int humanid, int weapon_paramid, float range_scale)
{
	if (weapon_paramid < 0) {
		weapon_paramid = ID_WEAPON_GRENADE;
	}

	grenade temp_grenade;

	temp_grenade.SetPosData(x, y, z, 0.0f, 0.0f);
	temp_grenade.SetParamData(0.0f, teamid, humanid, 1.0f, weapon_paramid, true);
	temp_grenade.SetEnableFlag(true);

	return GrenadeExplosion(&temp_grenade, range_scale);
}

//! @brief 롨왮뭙궻?긽?긙뵽믦궴룉뿚
//! @param in_grenade 뫮뤭궻롨왮뭙긆긳긙긃긏긣
//! @return 뱰궫궯궫갌true?뱰궫궲궋궶궋갌false
//! @attention 뵽믦귩뛱궎뫮뤭궼걏릐걐궴걏룷븿걐궳궥갃
//! @attention ?긽?긙뵽믦궸뙽귞궦갂?긽?긙똶럁귘뚼됈돶띋맯귏궳덇듥궢궲뛱궋귏궥갃
bool ObjectManager::GrenadeExplosion(grenade* in_grenade, float range_scale)
{
	if (in_grenade == NULL) { return false; }

	bool returnflag = false;

	//띆뷭귩롦벦
	float gx, gy, gz;
	int teamid, humanid;
	in_grenade->GetPosData(&gx, &gy, &gz, NULL, NULL);
	in_grenade->GetParamData(NULL, &teamid, &humanid, NULL);

	int grenade_weapon_id = in_grenade->GetWeaponParamID();

	const float IMPACT_GRENADE_RANGE_SCALE = 0.85f;

	float damage_scale = 1.0f;
	float grenade_damage_distance = MAX_DAMAGE_GRENADE_DISTANCE;
	if (grenade_weapon_id == ID_WEAPON_IMPACT_GRENADE) {
		damage_scale = IMPACT_GRENADE_DAMAGE_SCALE;
		grenade_damage_distance = MAX_DAMAGE_GRENADE_DISTANCE * IMPACT_GRENADE_RANGE_SCALE;
	}

	if (range_scale < 0.1f) {
		range_scale = 0.1f;
	}

	grenade_damage_distance *= range_scale;

	// ▼▼▼ [최적화: 활성화된 인물만 순회하여 폭발 피해 계산] ▼▼▼
	for (int i = 0; i < MAX_HUMAN; i++) {
		if (HumanIndex[i].GetEnableFlag() == false) { continue; }
		if (HumanIndex[i].GetDeadFlag() == true) { continue; }

		int HitHuman_TeamID;
		float hx, hy, hz;
		float x, y, z, r;

		//릐궻띆뷭귩롦벦궢갂떁뿣귩똶럁
		HumanIndex[i].GetPosData(&hx, &hy, &hz, NULL);
		x = hx - gx; y = hy - gy; z = hz - gz;
		float r_sq = x * x + y * y + z * z;
		float max_dist = grenade_damage_distance + HUMAN_HEIGHT;

		if (r_sq > max_dist * max_dist) { continue; }
		// 범위 안에 있을 때만 sqrtf 연산 수행 (최적화)
		r = sqrtf(r_sq);

		float dummy = 0.0f;
		int total_damage = 0;
		int damage;

		//뫉뙰궸뱰궫귟뵽믦
		y = hy + 2.0f - gy;
		r = sqrtf(x*x + y*y + z*z);
		//긳깓긞긏궕롏궯궲궋궶궚귢궽?걁깒귽궳뱰궫귟뵽믦귩뛱궋갂뱰궫궯궲궋궶궚귢궽걂
		if( CollD->CheckALLBlockIntersectRay(gx, gy, gz, x/r, y/r, z/r, NULL, NULL, &dummy, r) == false ){
			//?긽?긙쀊귩똶럁
			damage = HUMAN_DAMAGE_GRENADE_LEG - (int)((float)HUMAN_DAMAGE_GRENADE_LEG / grenade_damage_distance * r);
			damage = (int)((float)damage * damage_scale);
			if( damage > 0 ){
				total_damage += damage;
			}
		}

		//벆궸뱰궫귟뵽믦
		y = hy + (HUMAN_HEIGHT-2.0f) - gy;
		r = sqrtf(x*x + y*y + z*z);
		//긳깓긞긏궕롏궯궲궋궶궚귢궽?걁깒귽궳뱰궫귟뵽믦귩뛱궋갂뱰궫궯궲궋궶궚귢궽걂
		if( CollD->CheckALLBlockIntersectRay(gx, gy, gz, x/r, y/r, z/r, NULL, NULL, &dummy, r) == false ){
			//?긽?긙쀊귩똶럁
			damage = HUMAN_DAMAGE_GRENADE_HEAD - (int)((float)HUMAN_DAMAGE_GRENADE_HEAD / grenade_damage_distance * r);
			damage = (int)((float)damage * damage_scale);
			if( damage > 0 ){
				total_damage += damage;
			}
		}

		if (total_damage > 0) {
			int hp_old;
			bool hit_shadow_decoy = HumanIndex[i].GetSkillShadowDecoyFlag();

			//?긽?긙똶럁멟궻HP롦벦
			hp_old = HumanIndex[i].GetHP();

			//?긽?긙귩뵿뎕
			HumanIndex[i].HitGrenadeExplosion(total_damage);

			if ((HumanIndex[i].GetSkillShadowDecoyFlag() == true) &&
				(0 <= humanid) && (humanid < MAX_HUMAN)) {
				HumanIndex[i].SetSkillShadowDecoyLastAttackerID(humanid);
			}

			HumanIndex[i].GetParamData(NULL, NULL, NULL, &HitHuman_TeamID);

			float y2;
			float arx, ary;

			if( (0 <= humanid)&&(humanid < MAX_HUMAN) ){
				//?궢궲궋귢궽갂뵯롅궢궫릐궻맟됈궸돿럁
				if( HumanIndex[i].GetHP() <= 0 ){ Human_kill[humanid] += 1; }
			}

			//긄긲긃긏긣걁뙆걂귩?됪
			if (hit_shadow_decoy == false) {
				SetHumanBlood(hx, hy + 15.0f, hz, total_damage, false);
			}

			//릐궴롨왮뭙궻떁뿣귩럁뢯
			x = gx - hx;
			y = gy - (hy + 1.0f);
			z = gz - hz;

			//둷뱗귩땫귕귡
			arx = atan2f(z, x);

			if( sinf(atan2f(y, sqrtf(x*x + z*z))) < 0.0f ){		//뤵뺴뛀궸뷅귆궶귞갂둷뱗귩똶럁
				y2 = gy - (hy + HUMAN_HEIGHT);
				ary = atan2f(y2, sqrtf(x*x + z*z)) + (float)M_PI;
			}
			else{		//돷뺴뛀궸뷅귆궶귞갂릠뮳둷뱗궼뼰뚼갃걁뵚븮궳뭤뽋궸귕귟뜛귔궻귩뻞?걂
				ary = 0.0f;
			}

#ifdef ENABLE_BUG_TEAMID
			bool flag = true;

			//???붥뜂궕븠릶갂궔궰???붥뜂궕묈궖궋궶귞갂긲깋긐뼰뚼
			if( (HitHuman_TeamID < 0)&&(teamid < 0) ){
				if( HitHuman_TeamID < teamid ){
					flag = false;
				}
			}

			if( flag == true ){
				HumanIndex[i].SetHitFlag(arx*-1 + (float)M_PI/2);
			}
#else
			HumanIndex[i].SetHitFlag(arx*-1 + (float)M_PI/2);
#endif

			//뵚븮궸귝귡븮댊
			HumanIndex[i].AddPosOrder(arx, ary, 2.2f / grenade_damage_distance * (grenade_damage_distance - sqrtf(x * x + y * y + z * z)));

			//깓긐듫똚궻룉뿚
			if( (hp_old > 0)&&(HumanIndex[i].GetHP() <= 0) ){
				//?긽?긙똶럁멟궸 hp>0 궳궔궰갂똶럁뚣궸 hp <= 0 궶귞갂뜞됷?뻊궢궫갃

				int player_teamid;
				HumanIndex[Player_HumanID].GetParamData(NULL, NULL, NULL, &player_teamid);

				ObjectLog->DiedHuman(humanid, i, teamid, HitHuman_TeamID, player_teamid);
			}

			returnflag = true;
		}
	}

	// ▼▼▼ [최적화: 활성화된 소형 오브젝트만 순회] ▼▼▼
	for (int i = 0; i < MAX_SMALLOBJECT; i++) {
		if (SmallObjectIndex[i].GetEnableFlag() == false) { continue; }

		float sx, sy, sz;
		float x, y, z, r;

		//룷븿궻띆뷭귩롦벦궢갂떁뿣귩똶럁
		SmallObjectIndex[i].GetPosData(&sx, &sy, &sz, NULL);
		x = sx - gx;
		y = sy - gy;
		z = sz - gz;
		r = sqrtf(x*x + y*y + z*z);

		//100.0귝귟돀궚귢궽똶럁궢궶궋
		if (r > grenade_damage_distance) { continue; }

		float dummy = 0.0f;

		//긳깓긞긏궕롏궯궲궋궶궚귢궽?걁깒귽궳뱰궫귟뵽믦귩뛱궋갂뱰궫궯궲궋궶궚귢궽걂
		if( CollD->CheckALLBlockIntersectRay(gx, gy, gz, x/r, y/r, z/r, NULL, NULL, &dummy, r) == false ){
			int id, damage;
			SmallObjectIndex[i].GetParamData(&id, NULL);

			//?긽?긙쀊귩똶럁궢갂뵿뎕
			damage = SMALLOBJECT_DAMAGE_GRENADE - (int)((float)SMALLOBJECT_DAMAGE_GRENADE / grenade_damage_distance * r);
			damage = (int)((float)damage * damage_scale);
			SmallObjectIndex[i].HitGrenadeExplosion(damage);
			SmallObjectIndex[i].HitGrenadeExplosion(damage);

			//룷븿궔귞뚼됈돶귩뵯궥귡			damage = SMALLOBJECT_DAMAGE_GRENADE - (int)((float)SMALLOBJECT_DAMAGE_GRENADE / grenade_damage_distance * r);
			GameSound->HitSmallObject(sx, sy, sz, id, teamid);

			//깓긐듫똚궻룉뿚
			if( SmallObjectIndex[i].GetHP() <= 0 ){
				//?긽?긙똶럁멟궸 hp>0 궳궔궰갂똶럁뚣궸 hp <= 0 궶귞갂뜞됷봨됹궠귢궫갃
				ObjectLog->BreakSmallObject(i);
			}

			returnflag = true;
		}
	}

	//긄긲긃긏긣걁긲깋긞긘깄걂궻?됪
	AddEffect(gx, gy, gz, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 40.0f, 0.0f, Resource->GetEffectMflashTexture(), 0, 0, 0.63f, 0.0f, EFFECT_NORMAL, 2);

	//긄긲긃긏긣걁뎹걂궻?됪
	AddEffect(gx, gy, gz, 0.0f, 0.1f, 0.0f, 0.0f,
					DegreeToRadian(10)*GetRand(36), 0.0f, 36.0f, 0.2f, Resource->GetEffectSmokeTexture(), 220, 0, 0.6f, -0.01f, EFFECT_NORMAL, 50);
	AddEffect(gx + ((float)GetRand(7)/10-0.3f), gy + ((float)GetRand(5)/10-0.2f), gz + ((float)GetRand(7)/10-0.3f), (float)GetRand(5)/10-0.2f, (float)GetRand(3)/10, (float)GetRand(5)/10-0.2f, 0.0f,
					DegreeToRadian(10)*GetRand(36), 0.0f, ((float)GetRand(11)+25.0f)*2, 0.3f, Resource->GetEffectSmokeTexture(), 0, 0, 0.47f, -0.004f, EFFECT_NORMAL, 120);
	AddEffect(gx + ((float)GetRand(13)/10-0.6f), gy + ((float)GetRand(11)/10-0.5f), gz + ((float)GetRand(13)/10-0.6f), (float)GetRand(7)/10-0.3f, (float)GetRand(7)/10-0.3f, (float)GetRand(7)/10-0.3f, 0.0f,
					DegreeToRadian(10)*GetRand(36), DegreeToRadian(1), ((float)GetRand(21)+20.0f)*2, 0.2f, Resource->GetEffectSmokeTexture(), 0, 0, 0.4f, -0.004f, EFFECT_NORMAL, 100);
	AddEffect(gx + ((float)GetRand(13)/10-0.6f), gy + ((float)GetRand(11)/10-0.5f), gz + ((float)GetRand(13)/10-0.6f), (float)GetRand(7)/10-0.3f, (float)GetRand(7)/10-0.3f, (float)GetRand(7)/10-0.3f, 0.0f,
					DegreeToRadian(10)*GetRand(36), DegreeToRadian(-1), ((float)GetRand(21)+20.0f)*2, 0.2f, Resource->GetEffectSmokeTexture(), 0, 0, 0.4f, -0.004f, EFFECT_NORMAL, 100);

	//뚼됈돶귩띋맯
	GameSound->GrenadeExplosion(gx, gy, gz, teamid);

	return returnflag;
}

//! @brief ?귢궫띧궻긄긲긃긏긣먠믦
//! @param in_human 뫮뤭궻릐긆긳긙긃긏긣
void ObjectManager::DeadEffect(human *in_human)
{
	if( in_human == NULL ){ return; }

	int paramid;
	HumanParameter data;
	bool robot;

	in_human->GetParamData(&paramid, NULL, NULL, NULL);

	if( GameParamInfo->GetHuman(paramid, &data) != 0 ){ return; }

	if( data.type == 1 ){
		robot = true;
	}
	else{
		robot = false;
	}

	//귖궢깓?긞긣궶귞궽
	if( robot == true ){
		float hx, hy, hz, hrx, hry;

		//둫롰긬깋긽???귩롦벦
		in_human->GetPosData(&hx, &hy, &hz, &hrx);
		hry = in_human->GetDeadRy();

		//뜕뺃귟궻띆뷭귩럁뢯
		hx += cosf(hrx*-1 - (float)M_PI/2) * sinf(hry) * HUMAN_HEIGHT/2;
		hz += sinf(hrx*-1 - (float)M_PI/2) * sinf(hry) * HUMAN_HEIGHT/2;

		//긄긲긃긏긣걁뎹걂궻?됪
		float rnd = DegreeToRadian(10)*GetRand(18);
		AddEffect(hx+1.0f, hy+1.0f, hz+1.0f, 0.1f, 0.05f, 0.1f, 0.0f, rnd, 0, GetRand(30)*0.1f+30.0f, 0.1f, Resource->GetEffectSmokeTexture(), 10, 0, 1.0f, -1.0f / (GAMEFPS * 3), EFFECT_NORMAL, (int)GAMEFPS * 3);
		AddEffect(hx+1.0f, hy-1.0f, hz-1.0f, 0.2f, 0.05f, -0.2f, 0.0f, rnd*-1, 0, GetRand(30)*0.1f+30.0f, 0.1f, Resource->GetEffectSmokeTexture(), 10, 0, 1.0f, -1.0f / (GAMEFPS * 3), EFFECT_NORMAL, (int)GAMEFPS * 3);
		AddEffect(hx-1.0f, hy-1.0f, hz-1.0f, -0.3f, 0.05f, -0.3f, 0.0f, rnd, 0, GetRand(30)*0.1f+30.0f, 0.1f, Resource->GetEffectSmokeTexture(), 10, 0, 1.0f, -1.0f / (GAMEFPS * 3), EFFECT_NORMAL, (int)GAMEFPS * 3);
	}
}

//! ?긞긵궸뙆궕븊뭶궥귡궔뵽믦
//! @param in_effect 뫮뤭궻긄긲긃긏긣긆긳긙긃긏긣
//! @param id 븊뭶궥귡긳깓긞긏붥뜂귩롷궚롦귡?귽깛?
//! @param face 븊뭶궥귡뽋붥뜂귩롷궚롦귡?귽깛?
//! @param pos_x X띆뷭귩롷궚롦귡?귽깛?
//! @param pos_y Y띆뷭귩롷궚롦귡?귽깛?
//! @param pos_z Z띆뷭귩롷궚롦귡?귽깛?
//! @return 븊뭶궥귡갌true?븊뭶궢궶궋갌false
bool ObjectManager::CollideBlood(effect *in_effect, int *id, int *face, float *pos_x, float *pos_y, float *pos_z)
{
	if( in_effect == NULL ){ return false; }
	if( (id == NULL)||(face == NULL) ){ return false; }
	if( (pos_x == NULL)||(pos_y == NULL)||(pos_z == NULL) ){ return false; }

	//뼰뚼궶긄긲긃긏긣궶귞궽룉뿚궢궶궋
	if( in_effect->GetEnableFlag() == false ){ return false; }
	if( in_effect->GetCollideMapFlag() == false ){ return false; }
	if( in_effect->GetTextureID() != Resource->GetEffectBloodTexture() ){ return false; }

	//뙆궕뢯궶궋먠믦궶귞룉뿚궢궶궋
	if( GameConfig.GetBloodFlag() == false ){ return false; }

	float x, y, z;
	float move_x, move_y, move_z;
	float vx, vy, vz, dist, dist2;

	//긄긲긃긏긣궻긬깋긽???롦벦
	in_effect->GetPosData(&x, &y, &z, NULL);
	in_effect->GetMove(&move_x, &move_y, &move_z);

	//긄긲긃긏긣궕댷벍궢궶궋궶귞룉뿚궢궶궋
	if( (move_x == 0.0f)&&(move_y == 0.0f)&&(move_z == 0.0f) ){ return false; }

	//긹긏긣깑귩땫귕귡
	dist = sqrtf(move_x*move_x + move_y*move_y + move_z*move_z);
	vx = move_x / dist;
	vy = move_y / dist;
	vz = move_z / dist;

	//깒귽궻뱰궫귟뵽믦
	if( CollD->CheckALLBlockIntersectRay(x, y, z, vx, vy, vz, id, face, &dist2, dist) == false ){
		return false;
	}

	//?긞긵귝귟귦궦궔궸븖궔궧귡
	dist2 -= 1.0f;

	//븊뭶궥귡띆뷭귩땫귕귡
	*pos_x = x + vx*dist2;
	*pos_y = y + vy*dist2;
	*pos_z = z + vz*dist2;

	return true;
}

//! @brief 븧딇귩뢉궎
//! @param in_human 뫮뤭궻릐긆긳긙긃긏긣
//! @param in_weapon 뫮뤭궻븧딇긆긳긙긃긏긣
//! @attention 릐궻롰쀞궕 ?깛긮 궻뤾뜃갂궞궻듫릶궼렪봲궢귏궥갃
void ObjectManager::PickupWeapon(human *in_human, weapon *in_weapon)
{
	if( in_human == NULL ){ return; }
	if( in_weapon == NULL ){ return; }

	//뼰뚼궶릐궶귞궽룉뿚궢궶궋
	if (in_human->GetEnableFlag() == false) { return; }
	if (in_human->GetHP() <= 0) { return; }

	// 경찰 제압 명령에 걸린 동안은 방금 버린 무기를 바로 다시 줍지 못하게 한다.
	if (in_human->GetSkillSuppressedFlag() == true) { return; }

	//릐궻롰쀞궕 ?깛긮 궶귞궽갂렪봲
	int paramid;
	HumanParameter Paraminfo;
	in_human->GetParamData(&paramid, NULL, NULL, NULL);
	GameParamInfo->GetHuman(paramid, &Paraminfo);
	if( Paraminfo.type == 2 ){ return; }

	//룊딖돸궠귢궲궋귡븧딇궳갂궔궰묿귖럊궯궲궋궶궋븧딇궶귞궽?
	if( (in_weapon->GetEnableFlag() == true)&&(in_weapon->GetUsingFlag() == false) ){
		float human_x, human_y, human_z;
		float weapon_x, weapon_y, weapon_z;
		float x, z;
		float r;

		//릐궴븧딇궻띆뷭귩롦벦
		in_human->GetPosData(&human_x, &human_y, &human_z, NULL);
		in_weapon->GetPosData(&weapon_x, &weapon_y, &weapon_z, NULL);

		//뛼궠궕붝댪볙궶귞궽
		if( ((human_y + HUMAN_PICKUPWEAPON_L) <= weapon_y)&&(weapon_y <= (human_y + HUMAN_PICKUPWEAPON_H)) ){
			//떁뿣귩똶럁
			x = human_x - weapon_x;
			z = human_z - weapon_z;
			r = x*x + z*z;
			//떁뿣귖붝댪볙궶귞궽갂뢉귦궧귡갃
			if( r < (HUMAN_PICKUPWEAPON_R * HUMAN_PICKUPWEAPON_R) ){
				in_human->PickupWeapon(in_weapon);
			}
		}
	}
}

//! @brief 긆긳긙긃긏긣귩됶뺳
void ObjectManager::CleanupPointDataToObject()
{
	// 추가
	ResetRuntimeState();

	for (int i = 0; i < MAX_HUMAN; i++) {
		HumanIndex[i].SetEnableFlag(false);
		HumanIndex[i].SetIsPlayer(false);
		Human_ActionSoundTimer[i] = -1;
		Human_ActionSoundID[i] = RELOAD_NONE;
		Human_ShellReloadPrevState[i] = human::SHELL_RELOAD_NONE;
		Human_ShellReloadPrevLoad[i] = -1;
		Human_MinigunSpinSoundTimer[i] = 0;

		if (Human_ShieldBlockSwitchCount != NULL) {
			Human_ShieldBlockSwitchCount[i] = 0;
		}
		if (Human_ShieldBlockSwitchDamage != NULL) {
			Human_ShieldBlockSwitchDamage[i] = 0;
		}
		if (Human_ShieldBlockSwitchCooldown != NULL) {
			Human_ShieldBlockSwitchCooldown[i] = 0;
		}

		if (Human_ForceNoAI != NULL) {
			Human_ForceNoAI[i] = false;
		}
	}

	for (int i = 0; i < MAX_WEAPON; i++) {
		WeaponIndex[i].SetEnableFlag(false);
	}

	for (int i = 0; i < MAX_SMALLOBJECT; i++) {
		SmallObjectIndex[i].SetEnableFlag(false);
	}

	for (int i = 0; i < MAX_BULLET; i++) {
		BulletIndex[i].SetEnableFlag(false);
	}

	for (int i = 0; i < MAX_GRENADE; i++) {
		GrenadeIndex[i].SetEnableFlag(false);
	}

	for (int i = 0; i < MAX_EFFECT; i++) {
		EffectIndex[i].SetEnableFlag(false);
	}

	if (Resource != NULL) {
		Resource->CleanupHumanTexture();
	}
}

//! @brief ?귽깛긣긢??귩뙰궸긆긳긙긃긏긣귩봹뭫
void ObjectManager::LoadPointData()
{
	CleanupPointDataToObject();

	AddCollisionFlag = false;
	Player_HumanID = 0;

	framecnt = 0;


	//뵯뺺긲깋긐룊딖돸
	for (int i = 0; i < MAX_HUMAN; i++) {
		Human_ShotFlag[i] = false;
		Human_YakkyouCnt[i] = -1;
		Human_ActionSoundTimer[i] = -1;
		Human_ActionSoundID[i] = RELOAD_NONE;
		Human_ShellReloadPrevState[i] = human::SHELL_RELOAD_NONE;
		Human_ShellReloadPrevLoad[i] = -1;
		Human_MinigunSpinSoundTimer[i] = 0;

		if (Human_ShieldBlockSwitchCount != NULL) {
			Human_ShieldBlockSwitchCount[i] = 0;
		}
		if (Human_ShieldBlockSwitchDamage != NULL) {
			Human_ShieldBlockSwitchDamage[i] = 0;
		}
		if (Human_ShieldBlockSwitchCooldown != NULL) {
			Human_ShieldBlockSwitchCooldown[i] = 0;
		}

		if (Human_ForceNoAI != NULL) {
			Human_ForceNoAI[i] = false;
		}
	}

	//FF걁벏럐뱼궭걂뾎뚼돸긲깋긐먠믦
	for(int i=0; i<MAX_HUMAN; i++){
		FriendlyFire[i] = false;
	}

	//릐궻긡긏긚?긿깏긲깒긞긘깄긲깋긐룊딖돸
	for(int i=0; i<MAX_HUMAN; i++){
		Human_FrameTextureRefresh[i] = false;
	}

	/*
	//릐륃뺪?귽깛긣귩뭈궥
	for(int i=0; i<PointData->GetTotaldatas(); i++){
		pointdata data;
		PointData->Getdata(&data, i);

		if( data.p1 == POINT_P1TYPE_HUMANINFO ){
			//릐궻긡긏긚?긿귩뱋?
			Resource->AddHumanTexture(data.p2);
		}
	}
	*/

	//릐갋븧딇갋룷븿귩뭈궥
	for(int i=0; i<PointData->GetTotaldatas(); i++){
		pointdata data;
		PointData->Getdata(&data, i);

		//릐궶귞궽
		if( (data.p1 == POINT_P1TYPE_HUMAN)||(data.p1 == POINT_P1TYPE_HUMAN2) ){
			pointdata humaninfodata;

			//릐륃뺪?귽깛긣귩뭈궥
			if( PointData->SearchPointdata(&humaninfodata, 0x01 + 0x08, POINT_P1TYPE_HUMANINFO, 0, 0, data.p2, 0) == 0 ){
				//continue;

				// HUMANINFO가 없는 음수/범위 밖 인물은 기본 0번 인물로 대체하지 않고,
// 원래 data.p2 값을 버그 인물 ID로 해석해서 안전 매핑 처리한다.
				if (IsNormalHumanID(data.p2) == false) {
					pointdata compat_humaninfo;

					memset(&compat_humaninfo, 0, sizeof(pointdata));

					compat_humaninfo.p1 = POINT_P1TYPE_HUMANINFO;
					compat_humaninfo.p2 = data.p2;  // 중요: 44, -93, -94, -95 등 원본 번호 유지
					compat_humaninfo.p3 = 0;        // 팀 ID. 기존 fallback도 0을 사용하므로 0으로 둠
					compat_humaninfo.p4 = 0;

					AddHumanIndex(data, compat_humaninfo, true);

					continue;
				}
				//릐륃뺪?귽깛긣궕뙥궰궔귞궶궔궯궫귞갂궴귟궇궑궦걏벫롪 뜒 A걐궴궢궲믁돿갃걁긫긐궻띋뙸걂

				HumanParameter HumanParam;
				int Weapon[TOTAL_HAVEWEAPON];
				GameParamInfo->GetHuman(0, &HumanParam);
				for(int j=0; j<TOTAL_HAVEWEAPON; j++){
					Weapon[j] = HumanParam.Weapon[j];
				}

				//릐궻긡긏긚?긿귩뱋?
				Resource->AddHumanTexture(0);

				//릐귩믁돿
				if( data.p1 == POINT_P1TYPE_HUMAN2 ){
					Weapon[1] = ID_WEAPON_NONE;
				}

				//긵깒귽깂?궶귞궽갂붥뜂귩딯?
				if( (data.p4 == 0)&&(Player_HumanID == 0) ){
					Player_HumanID = AddHumanIndex(data.x, data.y, data.z, data.r, 0, 0, Weapon, data.p4, true);
				}
				else{
					AddHumanIndex(data.x, data.y, data.z, data.r, 0, 0, Weapon, data.p4, true);
				}
			}
			else {
				int raw_human_id = humaninfodata.p2;
				BugHumanProfile bugprofile = MakeBugHumanProfile(raw_human_id);

				// 중요:
				// humaninfodata.p2는 여기서 바꾸지 않는다.
				// AddHumanIndex()가 원본 음수 번호를 알아야
				// useMapTexture0, noWeapon, spawnDeadBody 처리를 적용할 수 있다.

				// 맵 텍스처 0번 계열은 인물 텍스처를 등록하지 않는다.
				// 그 외에는 치환된 정상 인물 텍스처를 등록한다.
				if (bugprofile.useMapTexture0 == false) {
					Resource->AddHumanTexture(bugprofile.humanID);
				}

				// 원본 humaninfodata.p2를 유지한 채 AddHumanIndex()로 보낸다.
				AddHumanIndex(data, humaninfodata, true);
			}
		}

		//븧딇궶귞궽
		if( (data.p1 == POINT_P1TYPE_WEAPON)||(data.p1 == POINT_P1TYPE_RAND_WEAPON) ){
			AddWeaponIndex(data);
		}

		//룷븿궶귞궽
		if( data.p1 == POINT_P1TYPE_SMALLOBJ ){
			AddSmallObjectIndex(data);
		}
	}

	//깓긐듫똚궻룉뿚
	ObjectLog->ClearLog();
	ObjectLog->InfoLog("Set object...");
}

//! @brief 깏??긚궻됷븳
//! @todo 멣궲궻긄긲긃긏긣궕뼰뚼돸궥귡갃
void ObjectManager::Recovery()
{
	ResetRuntimeState();

	//릐궻긾긢깑붥뜂귩롦벦
	int upmodel[TOTAL_UPMODE];
	int armmodel[TOTAL_ARMMODE];
	int legmodel;
	int walkmodel[TOTAL_WALKMODE];
	int runmodel[TOTAL_RUNMODE];
	int sitlegmodel;
	int sitwalkmodel[TOTAL_SITWALKMODE];

	Resource->GetHumanModel(upmodel, armmodel, &legmodel, walkmodel, runmodel);
	Resource->GetHumanSitModel(&sitlegmodel, sitwalkmodel);

	//릐
	int HumanID;
	int GetHumanFlag;
	HumanParameter HumanParam;
	for(int i=0; i<MAX_HUMAN; i++){
		if( HumanIndex[i].GetEnableFlag() == true ){
			HumanIndex[i].GetParamData(&HumanID, NULL, NULL, NULL);

			GetHumanFlag = GameParamInfo->GetHuman(HumanID, &HumanParam);

#ifndef ENABLE_BUG_HUMANWEAPON
			if( GetHumanFlag != 0 ){
				HumanIndex[i].SetEnableFlag(false);
				continue;
			}
#endif

			if (GetHumanFlag == 0) {
				if (HumanID == ID_HUMAN_WHITE_DUMMY) {
					int id = d3dg->GetMapTextureID(0);

					// 맵 텍스처 0번을 얻지 못했을 때 완전 투명 방지
					if (id == -1) {
						Resource->AddHumanTexture(ID_HUMAN_WHITE_DUMMY);
						id = Resource->GetHumanTexture(ID_HUMAN_WHITE_DUMMY);
					}

					// 맵 텍스처 더미는 프레임 텍스처 복구 로직을 타면 안 된다.
					// 그 로직은 이전 인물/무기 텍스처를 복사할 수 있다.
					Human_FrameTextureRefresh[i] = false;

					HumanIndex[i].SetTexture(id);
					HumanIndex[i].SetModel(upmodel[HumanParam.model], armmodel, legmodel, walkmodel, runmodel);
				}
				else {
					// 일반 인물은 기존처럼 인물 텍스처 사용
					Resource->AddHumanTexture(HumanID);

					int id = Resource->GetHumanTexture(HumanID);
					if (id == -1) {
						Human_FrameTextureRefresh[i] = true;
					}

					HumanIndex[i].SetTexture(id);
					HumanIndex[i].SetModel(upmodel[HumanParam.model], armmodel, legmodel, walkmodel, runmodel);
				}
			}
			else{
#ifdef ENABLE_BUG_HUMANWEAPON
				HumanIndex[i].SetTexture(d3dg->GetMapTextureID(0));
				HumanIndex[i].SetModel(upmodel[0], armmodel, legmodel, walkmodel, runmodel);
#endif
			}
		}
	}

	//븧딇
	int WeaponID;
	int Weaponmodel, Weapontexture;
	for(int i=0; i<MAX_WEAPON; i++){
		if( WeaponIndex[i].GetEnableFlag() == true ){
			//먠믦뭠귩롦벦
			WeaponParameter WeaponParam;
			WeaponIndex[i].GetParamData(&WeaponID, NULL, NULL);
			if( Resource->GetWeaponModelTexture(WeaponID, &Weaponmodel, &Weapontexture) == 1 ){ continue; }
			if( GameParamInfo->GetWeapon(WeaponID, &WeaponParam) == 1 ){ continue; }

			//밙뾭
			WeaponIndex[i].SetModel(Weaponmodel, WeaponParam.size);
			WeaponIndex[i].SetTexture(Weapontexture);
		}
	}

	//룷븿
	int SmallObjectID;
	int SmallObjectmodel, SmallObjecttexture;
	for(int i=0; i<MAX_SMALLOBJECT; i++){
		if( SmallObjectIndex[i].GetEnableFlag() == true ){
			//먠믦뭠귩롦벦
			SmallObjectIndex[i].GetParamData(&SmallObjectID, NULL);
			if( Resource->GetSmallObjectModelTexture(SmallObjectID, &SmallObjectmodel, &SmallObjecttexture) == 1 ){ continue; }

			//밙뾭
			SmallObjectIndex[i].SetModel(SmallObjectmodel, SMALLOBJECT_SCALE);
			SmallObjectIndex[i].SetTexture(SmallObjecttexture);
		}
	}

	int bulletmodel, bullettexture;
	BulletParameter data;

	//뢤뭙밙뾭
	if( Resource->GetBulletModelTexture(0, &bulletmodel, &bullettexture) == 0 ){
		if( GameParamInfo->GetBullet(0, &data) == 0 ){
			for(int i=0; i<MAX_BULLET; i++){
				BulletIndex[i].SetModel(bulletmodel, data.size);
				BulletIndex[i].SetTexture(bullettexture);
			}
		}
	}

	//롨왮뭙밙뾭
	if( Resource->GetBulletModelTexture(1, &bulletmodel, &bullettexture) == 0 ){
		if( GameParamInfo->GetBullet(1, &data) == 0 ){
			for(int i=0; i<MAX_GRENADE; i++){
				GrenadeIndex[i].SetModel(bulletmodel, data.size);
				GrenadeIndex[i].SetTexture(bullettexture);
			}
		}
	}

	//멣궲궻긄긲긃긏긣귩뼰뚼돸
	for(int i=0; i<MAX_EFFECT; i++){
		if( EffectIndex[i].GetEnableFlag() == true ){
			EffectIndex[i].SetEnableFlag(false);
		}
	}

	//깓긐듫똚궻룉뿚
	ObjectLog->InfoLog("Recovery object...");
}

//! @brief 믁돿궻뱰궫귟뵽믦긲깋긐귩먠믦
//! @param flag 긲깋긐
void ObjectManager::SetAddCollisionFlag(bool flag)
{
	AddCollisionFlag = flag;
}

//! @brief FF걁벏럐뱼궭걂뾎뚼돸긲깋긐귩롦벦
//! @param id 긢??붥뜂
//! @return 긲깋긐
bool ObjectManager::GetFriendlyFireFlag(int id)
{
	if( (id < 0)||(MAX_HUMAN <= id) ){ return false; }
	return FriendlyFire[id];
}

//! @brief FF걁벏럐뱼궭걂뾎뚼돸긲깋긐귩먠믦
//! @param id 긢??붥뜂
//! @param flag 긲깋긐
void ObjectManager::SetFriendlyFireFlag(int id, bool flag)
{
	if( (id < 0)||(MAX_HUMAN <= id) ){ return; }
	FriendlyFire[id] = flag;
}

//! @brief 긵깒귽깂?붥뜂귩롦벦
//! @return 긵깒귽깂?궻긢??붥뜂
int ObjectManager::GetPlayerID()
{
	return Player_HumanID;
}

//! @brief 긵깒귽깂?붥뜂귩먠믦
//! @param id 긵깒귽깂?궻긢??붥뜂
void ObjectManager::SetPlayerID(int id)
{
	if ((Player_HumanID >= 0) && (Player_HumanID < MAX_HUMAN)) {
		if (HumanIndex[Player_HumanID].GetEnableFlag() == true) {
			HumanIndex[Player_HumanID].SetIsPlayer(false);
		}
	}

	Player_HumanID = id;

	// 새 플레이어의 플래그 설정
	if ((Player_HumanID >= 0) && (Player_HumanID < MAX_HUMAN)) {
		if (HumanIndex[Player_HumanID].GetEnableFlag() == true) {
			HumanIndex[Player_HumanID].SetIsPlayer(true);
		}
	}
}

//! @brief 럚믦궢궫긢??붥뜂궻human긏깋긚귩롦벦
//! @param id 긢??붥뜂
//! @return 릐긆긳긙긃긏긣궻?귽깛??걁뼰뚼궶긢??붥뜂궳 NULL걂
human* ObjectManager::GetHumanObject(int id)
{
	if( (id < 0)||(MAX_HUMAN <= id) ){ return NULL; }
	return &(HumanIndex[id]);
}

//! @brief 긵깒귽깂?궻human긏깋긚귩롦벦
//! @return 릐긆긳긙긃긏긣걁긵깒귽깂?걂궻?귽깛?
human* ObjectManager::GetPlayerHumanObject()
{
	return GetHumanObject(Player_HumanID);
}

//! @brief 럚믦궢궫human?귽깛?궻긢??붥뜂귩롦벦
//! @param object 릐긆긳긙긃긏긣궻?귽깛?
//! @return 긢??붥뜂?걁긄깋?갌-1걂
int ObjectManager::GetHumanObjectID(human* object)
{
	if (object == NULL) { return -1; }

	// 배열의 시작 주소(HumanIndex)와 현재 객체의 주소 차이를 계산
	ptrdiff_t index = object - HumanIndex;

	// 유효한 범위인지 체크 후 반환
	if (index >= 0 && index < MAX_HUMAN) {
		return (int)index;
	}
	return -1;
}

bool ObjectManager::GetHumanForceNoAI(int id)
{
	if (id < 0) { return false; }
	if (MAX_HUMAN <= id) { return false; }
	if (Human_ForceNoAI == NULL) { return false; }

	return Human_ForceNoAI[id];
}

//! @brief 럚믦궢궫긢??붥뜂궻weapon긏깋긚귩롦벦
//! @param id 긢??붥뜂
//! @return 븧딇긆긳긙긃긏긣궻?귽깛??걁뼰뚼궶긢??붥뜂궳 NULL걂
weapon* ObjectManager::GetWeaponObject(int id)
{
	if( (id < 0)||(MAX_WEAPON <= id) ){ return NULL; }
	return &(WeaponIndex[id]);
}

//! @brief 럚믦궢궫긢??붥뜂궻smallobject긏깋긚귩롦벦
//! @param id 긢??붥뜂
//! @return 룷븿긆긳긙긃긏긣궻?귽깛??걁뼰뚼궶긢??붥뜂궳 NULL걂
smallobject* ObjectManager::GetSmallObject(int id)
{
	if( (id < 0)||(MAX_SMALLOBJECT <= id) ){ return NULL; }
	return &(SmallObjectIndex[id]);
}

//! @brief 럚믦궢궫긢??붥뜂궻bullet긏깋긚귩롦벦
//! @param id 긢??붥뜂
//! @return 뭙긆긳긙긃긏긣궻?귽깛??걁뼰뚼궶긢??붥뜂궳 NULL걂
bullet* ObjectManager::GetBulletObject(int id)
{
	if( (id < 0)||(MAX_BULLET <= id) ){ return NULL; }
	return &(BulletIndex[id]);
}

//! @brief 럚믦궢궫bullet?귽깛?궻긢??붥뜂귩롦벦
//! @param object 뭙긆긳긙긃긏긣궻?귽깛?
//! @return 긢??붥뜂?걁긄깋?갌-1걂
int ObjectManager::GetBulletObjectID(bullet* object)
{
	if( object == NULL ){ return -1; }

	for(int i=0; i<MAX_BULLET; i++){
		if( &(BulletIndex[i]) == object ){
			return i;
		}
	}

	return -1;
}

//! @brief 럊뾭궠귢궲궋궶궋bullet긏깋긚귩롦벦
//! @return 뙸띪뼟럊뾭궻뭙긆긳긙긃긏긣궻?귽깛??걁렪봲궥귡궴 NULL걂
bullet* ObjectManager::GetNewBulletObject()
{
	// ▼▼▼ [최적화: 총알 생성 시 멤버 변수를 이용한 O(1) 순환 탐색 적용] ▼▼▼
	for (int i = 0; i < MAX_BULLET; i++) {
		if (BulletIndex[i].GetEnableFlag() == false) {
			return &(BulletIndex[i]);
		}
	}
	// ▲▲▲ 여기까지 ▲▲▲
	return NULL;
}

//! @brief 럊뾭궠귢궲궋궶궋grenade긏깋긚귩롦벦
//! @return 뙸띪뼟럊뾭궻롨왮뭙긆긳긙긃긏긣궻?귽깛??걁렪봲궥귡궴 NULL걂
grenade* ObjectManager::GetNewGrenadeObject()
{
	for(int i=0; i<MAX_GRENADE; i++){
		if( GrenadeIndex[i].GetEnableFlag() == false ){
			return &(GrenadeIndex[i]);
		}
	}
	return NULL;
}

//! @brief 릐귩뙚랊
//! @param p4 뙚랊뫮뤭궻렞빶붥뜂
//! @return 둤뱰궢궫human긏깋긚궻?귽깛??걁뙥궰궔귞궶궋뤾뜃궼NULL걂
//! @attention 븸릶둤뱰궥귡뤾뜃갂띍룊궸둤뱰궢궫긢??귩뺅궢귏궥갃
human* ObjectManager::SearchHuman(signed short int p4)
{
	signed short int humanp4;

	for(int i=0; i<MAX_HUMAN; i++){
		//럊귦귢궲궋궶궋릐궶귞궽룉뿚궢궶궋
		if( HumanIndex[i].GetEnableFlag() == false ){ continue; }

		//묉4긬깋긽??귩롦벦
		HumanIndex[i].GetParamData(NULL, NULL, &humanp4, NULL);

		//럚믦궠귢궫p4궴덇뭭궥귢궽뺅궥
		if( humanp4 == p4 ){
			return &(HumanIndex[i]);
		}
	}
	return NULL;
}

//! @brief 룷븿귩뙚랊
//! @param p4 뙚랊뫮뤭궻렞빶붥뜂
//! @return 둤뱰궢궫smallobject긏깋긚궻?귽깛??걁뙥궰궔귞궶궋뤾뜃궼NULL걂
//! @attention 븸릶둤뱰궥귡뤾뜃갂띍룊궸둤뱰궢궫긢??귩뺅궢귏궥갃
smallobject* ObjectManager::SearchSmallobject(signed short int p4)
{
	signed short int smallobjectp4;

	for(int i=0; i<MAX_SMALLOBJECT; i++){
		//럊귦귢궲궋궶궋릐궶귞궽룉뿚궢궶궋
		//?걓봨됹먑귒궻긆긳긙긃긏긣귖뵽믦궥귡궫귕갂뼰뚼걕
		//if( SmallObjectIndex[i].GetEnableFlag() == false ){ continue; }

		//묉4긬깋긽??귩롦벦
		SmallObjectIndex[i].GetParamData(NULL, &smallobjectp4);

		//럚믦궠귢궫p4궴덇뭭궥귢궽뺅궥
		if( smallobjectp4 == p4 ){
			return &(SmallObjectIndex[i]);
		}
	}
	return NULL;
}

//! @brief 뾎뚼궶긆긳긙긃긏긣릶귩롦벦
//! @param HumanCnt 릐긆긳긙긃긏긣궻뾎뚼릶귩롷궚롦귡?귽깛??걁NULL됀걂
//! @param WeaponCnt 븧딇긆긳긙긃긏긣궻뾎뚼릶귩롷궚롦귡?귽깛??걁NULL됀걂
//! @param SmallobjectCnt 룷븿긆긳긙긃긏긣궻뾎뚼릶귩롷궚롦귡?귽깛??걁NULL됀걂
//! @param BulletCnt 뭙긆긳긙긃긏긣궻뾎뚼릶귩롷궚롦귡?귽깛??걁NULL됀걂
//! @param GrenadeCnt 롨왮뭙긆긳긙긃긏긣궻뾎뚼릶귩롷궚롦귡?귽깛??걁NULL됀걂
//! @param EffectCnt 긄긲긃긏긣긆긳긙긃긏긣궻뾎뚼릶귩롷궚롦귡?귽깛??걁NULL됀걂
void ObjectManager::GetTotalObjects(int *HumanCnt, int *WeaponCnt, int *SmallobjectCnt, int *BulletCnt, int *GrenadeCnt, int *EffectCnt)
{
	if( HumanCnt != NULL ){
		*HumanCnt = 0;
		for(int i=0; i<MAX_HUMAN; i++){
			if( HumanIndex[i].GetEnableFlag() == true ){ *HumanCnt += 1; }
		}
	}

	if( WeaponCnt != NULL ){
		*WeaponCnt = 0;
		for(int i=0; i<MAX_WEAPON; i++){
			if( WeaponIndex[i].GetEnableFlag() == true ){ *WeaponCnt += 1; }
		}
	}

	if( SmallobjectCnt != NULL ){
		*SmallobjectCnt = 0;
		for(int i=0; i<MAX_SMALLOBJECT; i++){
			if( SmallObjectIndex[i].GetEnableFlag() == true ){ *SmallobjectCnt += 1; }
		}
	}

	if( BulletCnt != NULL ){
		*BulletCnt = 0;
		for(int i=0; i<MAX_BULLET; i++){
			if( BulletIndex[i].GetEnableFlag() == true ){ *BulletCnt += 1; }
		}
	}

	if( GrenadeCnt != NULL ){
		*GrenadeCnt = 0;
		for(int i=0; i<MAX_GRENADE; i++){
			if( GrenadeIndex[i].GetEnableFlag() == true ){ *GrenadeCnt += 1; }
		}
	}

	if( EffectCnt != NULL ){
		*EffectCnt = 0;
		for(int i=0; i<MAX_EFFECT; i++){
			if( EffectIndex[i].GetEnableFlag() == true ){ *EffectCnt += 1; }
		}
	}
}

//! @brief 멟릋걁몪귟걂귩렳뛱
//! @param human_id 릐궻붥뜂걁0?MAX_HUMAN-1걂
void ObjectManager::MoveForward(int human_id)
{
	//뭠궻붝댪귩?긃긞긏
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//긆긳긙긃긏긣궸긲깋긐귩먠믦
	HumanIndex[human_id].SetMoveForward();
}

//! @brief 뚣뫿귩렳뛱
//! @param human_id 릐궻붥뜂걁0?MAX_HUMAN-1걂
void ObjectManager::MoveBack(int human_id)
{
	//뭠궻붝댪귩?긃긞긏
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//긆긳긙긃긏긣궸긲깋긐귩먠믦
	HumanIndex[human_id].SetMoveBack();
}

//! @brief 뜺몪귟귩렳뛱
//! @param human_id 릐궻붥뜂걁0?MAX_HUMAN-1걂
void ObjectManager::MoveLeft(int human_id)
{
	//뭠궻붝댪귩?긃긞긏
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//긆긳긙긃긏긣궸긲깋긐귩먠믦
	HumanIndex[human_id].SetMoveLeft();
}

//! @brief 덭몪귟귩렳뛱
//! @param human_id 릐궻붥뜂걁0?MAX_HUMAN-1걂
void ObjectManager::MoveRight(int human_id)
{
	//뭠궻붝댪귩?긃긞긏
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//긆긳긙긃긏긣궸긲깋긐귩먠믦
	HumanIndex[human_id].SetMoveRight();
}

//! @brief 뺖궖귩렳뛱
//! @param human_id 릐궻붥뜂걁0?MAX_HUMAN-1걂
void ObjectManager::MoveWalk(int human_id)
{
	//뭠궻붝댪귩?긃긞긏
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	//긆긳긙긃긏긣궸긲깋긐귩먠믦
	HumanIndex[human_id].SetMoveWalk();
}

//! @brief 긙긿깛긵
//! @param human_id 릐궻붥뜂걁0?MAX_HUMAN-1걂
void ObjectManager::MoveJump(int human_id)
{
	//뭠궻붝댪귩?긃긞긏
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	HumanIndex[human_id].Jump();
}


static bool IsNoDryFireWeapon(int weapon_paramid)
{
	switch (weapon_paramid) {
	case 0:	// None
	case 13:	// 슈류탄
	case 15:	// Case
		return true;
	default:
		return false;
	}
}

//! @brief 뵯뺺
//! @param human_id 뵯뺺궥귡릐붥뜂
//! @return 믅륂뭙뵯롅갌1?롨왮뭙뵯롅갌2?렪봲갌0
int ObjectManager::ShotWeapon(int human_id)
{
	//뭠궻붝댪귩?긃긞긏
	if ((human_id < 0) || (MAX_HUMAN <= human_id)) { return 0; }

	human* MyHuman = &(HumanIndex[human_id]);

	float pos_x, pos_y, pos_z;
	int teamid;
	float rotation_x, armrotation_y;
	int weapon_paramid;
	int GunsightErrorRange;
	WeaponParameter ParamData;
	bool playerflag;
	bool grenadeflag;

	//릐궻띆뷭궴둷뱗귩롦벦
	MyHuman->GetPosData(&pos_x, &pos_y, &pos_z, NULL);
	MyHuman->GetParamData(NULL, NULL, NULL, &teamid);
	MyHuman->GetRxRy(&rotation_x, &armrotation_y);

	//뫮뤭롌궕긵깒귽깂?렔릆궔뵽믦
	if (MyHuman == &(HumanIndex[Player_HumanID])) {
		playerflag = true;
	}
	else {
		playerflag = false;
	}

	float shot_y = GetHumanShotY(pos_y, MyHuman->GetCrouchFlag());

	// 뭙궻뵯롅귩뾴땫
	if (MyHuman->ShotWeapon(&weapon_paramid, &GunsightErrorRange) == false) {
		if (MyHuman->GetLastShotFailReason() == human::SHOT_FAIL_EMPTY) {

			if (IsNoDryFireWeapon(weapon_paramid)) {
				return 0;
			}

			float sx, sy, sz;
			int steamid;

			MyHuman->GetPosData(&sx, &sy, &sz, NULL);
			MyHuman->GetParamData(NULL, NULL, NULL, &steamid);
			sy += 16.0f;

			bool isPlayer = (human_id == Player_HumanID);
			GameSound->DryFire(sx, sy, sz, steamid, isPlayer);
		}

		return 0;
	}

	//븧딇궻륃뺪귩롦벦
	if (GameParamInfo->GetWeapon(weapon_paramid, &ParamData) != 0) { return 0; }

	float muzzle_x = pos_x;
	float muzzle_y = shot_y;
	float muzzle_z = pos_z;

	d3dg->SetWorldTransformHumanWeapon(
		pos_x,
		shot_y,
		pos_z,
		ParamData.flashx / 10,
		ParamData.flashy / 10,
		ParamData.flashz / 10,
		rotation_x,
		armrotation_y * -1,
		1.0f
	);
	d3dg->GetWorldTransformPos(&muzzle_x, &muzzle_y, &muzzle_z);
	d3dg->ResetWorldTransform();

	// 수류탄 계열 판정
	if ((weapon_paramid == ID_WEAPON_GRENADE) ||
		(weapon_paramid == ID_WEAPON_IMPACT_GRENADE)) {
		grenadeflag = true;
	}
	else {
		grenadeflag = false;
	}

	//뚮뜼궻붝댪귩똶럁
	int ErrorRange;
	ErrorRange = GunsightErrorRange;

	// 로봇 F: 에임핵 중에는 이동 탄퍼짐뿐 아니라 무기 기본 최소 탄퍼짐도 제거한다.
	// human::ShotWeapon()에서 GunsightErrorRange를 0으로 넘겨도,
	// 여기서 ParamData.ErrorRangeMIN이 다시 적용되면 실제 탄은 여전히 퍼질 수 있다.
	if (MyHuman->GetSkillAimHackAccuracyFlag() == true) {
		ErrorRange = 0;
	}
	else {
		if (ErrorRange < ParamData.ErrorRangeMIN) { ErrorRange = ParamData.ErrorRangeMIN; }
	}

	//뵯롅둷뱗걁뚮뜼?깓궻긹?긚걂귩뙂믦
	float rx, ry;
	rx = rotation_x * -1 + (float)M_PI / 2;
	ry = armrotation_y;

	//뚮뜼귩뙂믦
	if (ErrorRange != 0) {
		int ErrorRange_x, ErrorRange_y;

		ErrorRange_x = GetRand(ErrorRange * 2 + 1) - ErrorRange;
		ErrorRange_y = GetRand(ErrorRange * 2 + 1) - ErrorRange;

		//뵯롅뚮뜼빁?둷뱗궸돿럁
		rx += ErrorRange_x * DegreeToRadian(0.15f);
		ry += ErrorRange_y * DegreeToRadian(0.15f);
	}

	//롨왮뭙궳궶궚귢궽
	if (grenadeflag == false) {
		class bullet* newbullet;

		//긘깈긞긣긊깛궶궵뵯롅궥귡뭙걁긻깒긞긣걂궻릶빁똉귟뺅궥
		for (int i = 0; i < ParamData.pellet; i++) {
			int attacks;
			float ontargetcnt;
			float rx2, ry2;

			//긘깈긞긣긊깛궶궵뵯롅궥귡뭙걁긻깒긞긣걂궕븸릶궇귢궽
			if (ParamData.pellet > 1) {
				//1뙿궻뭙궇궫귟궻뛘똼쀍궴뼺뭷렄궻긇긂깛긣릶귩럁뢯
				//??멣뭙뜃귦궧궲갂2?궸궶귡귝궎궸궥귡갃
				attacks = (int)((float)ParamData.attacks / ((float)ParamData.pellet / 2));
				ontargetcnt = 1.0f / ((float)ParamData.pellet / 2);

				// 무기 이름이 "Calico M950"인지 확인합니다.
				if (ParamData.nbsmax >= 25) {
					// nbsmax가 25 이상이면 탄 퍼짐 없이 발사 각도를 그대로 사용합니다.
					rx2 = rx;
					ry2 = ry;
				}
				else {
					//궠귞궸뚮뜼귩몵귘궢궲둮랼궠궧귡
					int len;
					float a = DegreeToRadian(10) * GetRand(36);
					len = GetRand(5) * 2 + 5;
					rx2 = rx + cosf(a) * len * DegreeToRadian(0.15f);
					ry2 = ry + sinf(a) * len * DegreeToRadian(0.15f);
				}
			}
			else {
				//궩궻귏귏뛘똼쀍궴뼺뭷렄궻긇긂깛긣릶귩뵿뎕
				attacks = ParamData.attacks;
				ontargetcnt = 1.0f;

				//뵯롅뚮뜼궼궩궻귏귏
				rx2 = rx;
				ry2 = ry;
			}

			//뵯롅궥귡뼟럊뾭궻긆긳긙긃긏긣귩롦벦
			newbullet = GetNewBulletObject();
			if (newbullet == NULL) { return 0; }


			//뢤뭙귩뵯롅
			// 판정용 총알은 카메라/팔 보간과 분리한다.
			// 몸 중심(pos_x/pos_z)에서 바로 시작하면 3인칭에서 캐릭터 뒤에서 발사되어
			// 몸을 통과하는 것처럼 보이므로, 사격 방향으로 상체 충돌 반지름만큼 앞에서 시작시킨다.
			const float bullet_forward_offset = HUMAN_BULLETCOLLISION_UP_R + 0.8f;
			float bullet_start_x = pos_x + cosf(rx2) * cosf(ry2) * bullet_forward_offset;
			float bullet_start_y = shot_y + sinf(ry2) * bullet_forward_offset;
			float bullet_start_z = pos_z + sinf(rx2) * cosf(ry2) * bullet_forward_offset;

			newbullet->SetPosData(bullet_start_x, bullet_start_y, bullet_start_z, rx2, ry2);

			newbullet->SetParamData(
				attacks,
				ParamData.penetration,
				(float)ParamData.speed * BULLET_SPEEDSCALE,
				teamid,
				human_id,
				ontargetcnt,
				true
			);

			newbullet->SetSilencerFlag(ParamData.silencer);
			newbullet->SetEnableFlag(true);

			//뫮릐뵽믦뾭깏긚긣룊딖돸
			BulletObj_HumanIndex[GetBulletObjectID(newbullet)].Init();
		}


		// ✨ === 1. 총구 화염 및 탄피 효과 생성 조건 추가 (수정) === ✨
		// 플레이어가 조준 중이 아닐 때만 효과를 생성합니다.
		if ((playerflag == false) || (MyHuman->GetScopeMode() == 0)) {
			//뵯뺺긲깋긐귩먠믦
			Human_ShotFlag[human_id] = true;
			Human_YakkyouCnt[human_id] = ParamData.yakkyou_delay;
		}
	}
	else {
		class grenade* newgrenade;

		//뵯롅궥귡뼟럊뾭궻긆긳긙긃긏긣귩롦벦
		newgrenade = GetNewGrenadeObject();
		if (newgrenade == NULL) { return 0; }

		// 수류탄 오브젝트 모델/텍스처를 현재 무기 기준으로 설정한다.
		int grenade_model = -1;
		int grenade_texture = -1;

		if (Resource->GetWeaponModelTexture(weapon_paramid, &grenade_model, &grenade_texture) == 0) {
			newgrenade->SetModel(grenade_model, ParamData.size);
			newgrenade->SetTexture(grenade_texture);
		}

		// 수류탄 발사
		newgrenade->SetPosData(muzzle_x, muzzle_y, muzzle_z, rx, ry);
		newgrenade->SetParamData(8.0f, teamid, human_id, 1.0f, weapon_paramid, true);
		newgrenade->SetEnableFlag(true);
	}

	if (ParamData.soundvolume > 0) {
		//뢤맳귩띋맯
		GameSound->ShotWeapon(muzzle_x, muzzle_y, muzzle_z, weapon_paramid, teamid, playerflag);
	}

	// AI 미니건은 예열 없이 바로 발사하되, 발사 중 회전음만 반복 재생한다.
// 플레이어 회전음은 gamemain.cpp에서 우클릭 예열/발사/감속까지 처리한다.
	if (ParamData.burstcnt == 6 && playerflag == false) {
		if (Human_MinigunSpinSoundTimer[human_id] <= 0) {
			GameSound->ReloadWeapon(muzzle_x, muzzle_y, muzzle_z, RELOAD_MINIGUN_SPIN, teamid, false);
			Human_MinigunSpinSoundTimer[human_id] = 10;
		}
	}

	// 발사 후 펌프/노리쇠/레버 조작음 예약
// 단, 마지막 탄을 쏜 경우에는 재장전 사운드에서 처리하므로 여기서는 생략한다.
	int bullets_after_shot = 0;

	{
		int selectweapon = 0;
		class weapon* haveweapon[TOTAL_HAVEWEAPON];
		int loadbullets[TOTAL_HAVEWEAPON];
		int bullets[TOTAL_HAVEWEAPON];

		MyHuman->GetWeapon(&selectweapon, haveweapon, loadbullets, bullets);

		if ((selectweapon >= 0) &&
			(selectweapon < TOTAL_HAVEWEAPON) &&
			(haveweapon[selectweapon] != NULL)) {

			haveweapon[selectweapon]->GetParamData(NULL, &bullets_after_shot, NULL);
		}
	}

	if ((grenadeflag == false) &&
		(ParamData.SoundID_Action != RELOAD_NONE) &&
		(ParamData.ActionSoundFrame > 0) &&
		(bullets_after_shot > 0)) {

		Human_ActionSoundTimer[human_id] = ParamData.ActionSoundFrame;
		Human_ActionSoundID[human_id] = ParamData.SoundID_Action;
	}

	if (grenadeflag == true) {
		return 2;
	}
	return 1;
}

// 스킬/연출 전용 총구화염.
// 실제 총알 생성, 탄약 소모, 명중 판정은 하지 않고 현재 들고 있는 총의 flash 위치만 재사용한다.
void ObjectManager::PlayWeaponShotVisualEffect(int human_id, bool include_yakkyou)
{
	if ((human_id < 0) || (MAX_HUMAN <= human_id)) { return; }

	human* h = &(HumanIndex[human_id]);

	if (h->GetEnableFlag() == false) { return; }
	if (h->GetDeadFlag() == true) { return; }
	if (h->GetHP() <= 0) { return; }

	int weapon_paramid = h->GetMainWeaponTypeNO();

	// 렌더링 override 무기가 있으면 총구화염/탄피 연출도 그 무기 기준으로 맞춘다.
	int render_override_weapon_id = h->GetRenderOverrideWeaponID();
	if (render_override_weapon_id >= 0) {
		weapon_paramid = render_override_weapon_id;
	}

	// 총처럼 보이지 않는 무기/스킬 무기는 이번 연출에서 제외한다.
	if ((weapon_paramid == ID_WEAPON_NONE) ||
		(weapon_paramid == ID_WEAPON_GRENADE) ||
		(weapon_paramid == ID_WEAPON_IMPACT_GRENADE) ||
		(weapon_paramid == ID_WEAPON_SHIELD) ||
		(weapon_paramid == ID_WEAPON_CASE)) {
		return;
	}

	WeaponParameter paramdata;
	if (GameParamInfo->GetWeapon(weapon_paramid, &paramdata) != 0) { return; }

	if ((paramdata.WeaponP != WEAPONP_ONEHAND) &&
		(paramdata.WeaponP != WEAPONP_TWOHAND)) {
		return;
	}

	ShotWeaponEffect(human_id);

	if (include_yakkyou == true) {
		ShotWeaponYakkyou(human_id);
	}
}

//! @brief ?긛깑긲깋긞긘깄귩먠믦
//! @param humanid 릐궻붥뜂
//! @attention 궞궻듫릶궻뚁귂뢯궢?귽?깛긐귩뚮귡궴갂뢤뚿궸뫮궢궲?긛깑긲깋긞긘깄궕뜃궋귏궧귪갃
void ObjectManager::ShotWeaponEffect(int humanid)
{
	//뭠궻붝댪귩?긃긞긏
	if( (humanid < 0)||(MAX_HUMAN <= humanid) ){ return; }

	float pos_x, pos_y, pos_z;
	float rotation_x, armrotation_y;
	int weapon_paramid;
	WeaponParameter ParamData;
	float x, y, z;
	float flashsize, smokesize1, smokesize2;
	float rx, emx, emy, emz;

	//릐궻띆뷭궴둷뱗귩롦벦
	HumanIndex[humanid].GetPosData(&pos_x, &pos_y, &pos_z, NULL);
	HumanIndex[humanid].GetRxRy(&rotation_x, &armrotation_y);

	float weapon_y = GetHumanWeaponBaseY(pos_y, HumanIndex[humanid].GetCrouchFlag());

	// 무기 정보를 얻는다.
// 렌더링 override 무기가 있으면 총구화염/탄피 위치도 그 무기 기준으로 사용한다.
	weapon_paramid = HumanIndex[humanid].GetMainWeaponTypeNO();

	int render_override_weapon_id = HumanIndex[humanid].GetRenderOverrideWeaponID();
	if (render_override_weapon_id >= 0) {
		weapon_paramid = render_override_weapon_id;
	}

	if (GameParamInfo->GetWeapon(weapon_paramid, &ParamData) != 0) { return; }

	//?긛깑긲깋긞긘깄궴뎹궻긖귽긛귩뙂믦
	if( ParamData.silencer == true ){ flashsize = 1.2f; }
	else{ flashsize = 0.06f * ParamData.attacks; }
	smokesize1 = 0.04f * ParamData.attacks;
	smokesize2 = 0.032f * ParamData.attacks;

	//뎹궻댷벍긹긏긣깑귩뙂믦
	rx = rotation_x*-1 + (float)M_PI/2;
	emx = cosf(rx)*cosf(armrotation_y);
	emy = sinf(armrotation_y);
	emz = sinf(rx)*cosf(armrotation_y);

	//뛱쀱궳긄긲긃긏긣띆뷭귩똶럁
	d3dg->SetWorldTransformHumanWeapon(pos_x, weapon_y, pos_z, ParamData.flashx / 10, ParamData.flashy / 10, ParamData.flashz / 10, rotation_x, armrotation_y * -1, 1.0f);
	d3dg->GetWorldTransformPos(&x, &y, &z);
	d3dg->ResetWorldTransform();

	//?긛깑긲깋긞긘깄?됪
	AddEffect(x, y, z, 0.0f, 0.0f, 0.0f, 0.0f, DegreeToRadian(10)*GetRand(36), 0.0f, flashsize, 0.0f, Resource->GetEffectMflashTexture(), 0, 0, 0.63f, 0.0f, EFFECT_NORMAL, 1);

	//뛱쀱궳긄긲긃긏긣띆뷭귩똶럁
	d3dg->SetWorldTransformHumanWeapon(pos_x, weapon_y, pos_z, ParamData.flashx / 10, ParamData.flashy / 10, ParamData.flashz / 10 - 0.1f, rotation_x, armrotation_y * -1, 1.0f);
	d3dg->GetWorldTransformPos(&x, &y, &z);
	d3dg->ResetWorldTransform();

	//긄긲긃긏긣걁뎹걂궻?됪
	AddEffect(x, y, z, emx*0.15f, emy*0.15f + 0.02f, emz*0.15f, 0.0f, DegreeToRadian(10)*GetRand(36), 0.0f, smokesize1, 0.3f, Resource->GetEffectSmokeTexture(), 0, 0, 0.47f, -0.047f, EFFECT_NORMAL, 12);
	AddEffect(x, y, z, emx*0.45f, emy*0.45f, emz*0.45f, 0.0f, DegreeToRadian(10)*GetRand(36), 0.0f, smokesize2, 0.36f, Resource->GetEffectSmokeTexture(), 0, 0, 0.47f, -0.055f, EFFECT_NORMAL, 10);
}

//! @brief 뽶娥봱뢯귩먠믦
//! @param humanid 릐궻붥뜂
//! @attention 궞궻듫릶궻뚁귂뢯궢?귽?깛긐귩뚮귡궴갂뢤궸뫮궢궲뽶娥봱뢯댧뭫궕뜃궋귏궧귪갃
void ObjectManager::ShotWeaponYakkyou(int humanid)
{
	//뭠궻붝댪귩?긃긞긏
	if( (humanid < 0)||(MAX_HUMAN <= humanid) ){ return; }

	float pos_x, pos_y, pos_z;
	float rotation_x, armrotation_y;
	int weapon_paramid;
	WeaponParameter ParamData;
	float x, y, z;
	float yakkyousize;
	float rx;
	float mx, my, mz;

	//릐궻띆뷭궴둷뱗귩롦벦
	HumanIndex[humanid].GetPosData(&pos_x, &pos_y, &pos_z, NULL);
	HumanIndex[humanid].GetRxRy(&rotation_x, &armrotation_y);

	// 무기 정보를 얻는다.
// 렌더링 override 무기가 있으면 탄피 위치/크기도 그 무기 기준으로 사용한다.
	weapon_paramid = HumanIndex[humanid].GetMainWeaponTypeNO();

	int render_override_weapon_id = HumanIndex[humanid].GetRenderOverrideWeaponID();
	if (render_override_weapon_id >= 0) {
		weapon_paramid = render_override_weapon_id;
	}

	if (GameParamInfo->GetWeapon(weapon_paramid, &ParamData) != 0) { return; }

	// 탄피 크기를 결정한다.
	if (ParamData.yakkyou_size >= 0.0f) { yakkyousize = ParamData.yakkyou_size / 10; }
	else { yakkyousize = 0.012f * ParamData.attacks; }

	// 탄피 이동 방향을 결정한다.
	rx = rotation_x * -1 + (float)M_PI / 2;

	//뽶娥궻댷벍쀊귩뙂믦
	mx = cosf(rx - (float)M_PI/2) * ParamData.yakkyou_sx * 0.12f;
	my = (ParamData.yakkyou_sy + (GetRand(3)-1)) * 0.12f;
	mz = sinf(rx - (float)M_PI/2) * ParamData.yakkyou_sx * 0.12f;

	float weapon_y = GetHumanWeaponBaseY(pos_y, HumanIndex[humanid].GetCrouchFlag());

	//뛱쀱궳긄긲긃긏긣띆뷭귩똶럁
	d3dg->SetWorldTransformHumanWeapon(pos_x, weapon_y, pos_z, ParamData.yakkyou_px / 10, ParamData.yakkyou_py / 10, ParamData.yakkyou_pz / 10, rotation_x, armrotation_y * -1, 1.0f);
	d3dg->GetWorldTransformPos(&x, &y, &z);
	d3dg->ResetWorldTransform();

	//뽶娥?됪
	AddEffect(x, y, z, mx, my, mz, -0.15f, DegreeToRadian(10)*GetRand(36), DegreeToRadian(GetRand(61)-30), yakkyousize, 0.0f, Resource->GetEffectYakkyouTexture(), 0, 0, 1.0f, 0.0f, EFFECT_NORMAL, (int)(GAMEFPS*0.75f));
}

//! @brief 븧딇귩깏깓?긤
//! @param human_id 뫮뤭궻릐붥뜂
//! @return 맟뚻갌true?렪봲갌false
bool ObjectManager::ReloadWeapon(int human_id)
{
	// 대상 범위 체크
	if ((human_id < 0) || (MAX_HUMAN <= human_id)) { return false; }

	// 무효한 인간이면 처리하지 않음
	if (HumanIndex[human_id].GetEnableFlag() == false) { return false; }
	if (HumanIndex[human_id].GetHP() <= 0) { return false; }

	// 재장전 실행 전, 현재 무기 정보를 미리 얻는다.
	// ReloadWeapon() 호출 후에는 탄약/상태가 바뀔 수 있으므로 반드시 먼저 읽는다.
	int selectweapon = -1;
	weapon* haveweapon[TOTAL_HAVEWEAPON];
	int loadbullets[TOTAL_HAVEWEAPON];
	int bullets[TOTAL_HAVEWEAPON];

	HumanIndex[human_id].GetWeapon(&selectweapon, haveweapon, loadbullets, bullets);

	int weaponParamID = -1;
	int nowAmmo = 0;
	int reloadSoundID = RELOAD_NONE;

	WeaponParameter WParam;

	if ((selectweapon >= 0) && (selectweapon < TOTAL_HAVEWEAPON) && (haveweapon[selectweapon] != NULL)) {
		haveweapon[selectweapon]->GetParamData(&weaponParamID, &nowAmmo, NULL);

		if (GameParamInfo->GetWeapon(weaponParamID, &WParam) == 0) {
			bool isTactical = false;

			// 전술 재장전 판별:
			// 탄창식 + 탄이 남아 있음 + 약실 기능 있음
			if ((WParam.is_shell_reload == false) && (nowAmmo > 0) && (WParam.bHasChamber == true)) {
				isTactical = true;
			}

			if (WParam.is_shell_reload == true) {
				reloadSoundID = WParam.SoundID_ReloadStart;
				Human_ShellReloadPrevState[human_id] = HumanIndex[human_id].GetShellReloadState();
				Human_ShellReloadPrevLoad[human_id] = nowAmmo;
			}
			else if ((isTactical == true) && (WParam.SoundID_Tactical != -1)) {
				reloadSoundID = WParam.SoundID_Tactical;
			}
			else {
				reloadSoundID = WParam.SoundID_Reload;
			}
		}
	}

	// 재장전 실행
	if (HumanIndex[human_id].ReloadWeapon() == true) {
		float x, y, z;
		int teamid;

		HumanIndex[human_id].GetPosData(&x, &y, &z, NULL);
		HumanIndex[human_id].GetParamData(NULL, NULL, NULL, &teamid);
		y += 16.0f;

		// 재장전 사운드 배치
		if (reloadSoundID != RELOAD_NONE) {
			bool isPlayerReload = (human_id == Player_HumanID);
			GameSound->ReloadWeapon(x, y, z, reloadSoundID, teamid, isPlayerReload);
		}

		return true;
	}

	return false;
}

//! @brief 븧딇귩먛귟뫶궑귡걁렃궭뫶궑귡걂
//! @param human_id 릐궻붥뜂걁0?MAX_HUMAN-1걂
//! @param id 렃궭뫶궑귡븧딇?걁-1 궳렅궻븧딇걂
void ObjectManager::ChangeHaveWeapon(int human_id, int id)
{
	//뭠궻붝댪귩?긃긞긏
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return; }

	HumanIndex[human_id].ChangeHaveWeapon(id);
	Human_YakkyouCnt[human_id] = -1;
}

#define MODETIME_FIREMODE_CHANGE              17
#define MODETIME_LONGGUN_SUPPRESSOR_ATTACH    44
#define MODETIME_LONGGUN_SUPPRESSOR_DETACH    39
#define MODETIME_PISTOL_SUPPRESSOR_ATTACH     52
#define MODETIME_PISTOL_SUPPRESSOR_DETACH     48
#define MINIGUN_SPIN_SOUND_INTERVAL 10

static bool IsPistolSuppressorWeaponID(int weapon_id)
{
	switch (weapon_id) {
	case 18: // GLOCK17 Std.
	case 75: // GLOCK17 SD
	case 21: // MK23 Std.
	case 22: // MK23 SD
	case 52: // Beretta 93R Std.
	case 76: // Beretta 93R SD
		return true;
	default:
		return false;
	}
}

static int GetWeaponModeChangeInfo(
	int old_weapon_id,
	const WeaponParameter* old_param,
	int new_weapon_id,
	const WeaponParameter* new_param,
	int* out_change_time
)
{
	if (out_change_time != NULL) {
		*out_change_time = 0;
	}

	if (old_param == NULL || new_param == NULL) {
		return -1;
	}

	// 소음기 상태가 바뀌는 경우
	if (old_param->silencer != new_param->silencer) {
		bool pistol = IsPistolSuppressorWeaponID(old_weapon_id) || IsPistolSuppressorWeaponID(new_weapon_id);

		if (new_param->silencer == true) {
			if (out_change_time != NULL) {
				*out_change_time = pistol ? MODETIME_PISTOL_SUPPRESSOR_ATTACH : MODETIME_LONGGUN_SUPPRESSOR_ATTACH;
			}
			return pistol ? WEAPONMODE_PISTOL_SUPPRESSOR_ATTACH : WEAPONMODE_LONGGUN_SUPPRESSOR_ATTACH;
		}
		else {
			if (out_change_time != NULL) {
				*out_change_time = pistol ? MODETIME_PISTOL_SUPPRESSOR_DETACH : MODETIME_LONGGUN_SUPPRESSOR_DETACH;
			}
			return pistol ? WEAPONMODE_PISTOL_SUPPRESSOR_DETACH : WEAPONMODE_LONGGUN_SUPPRESSOR_DETACH;
		}
	}

	// 발사 모드가 바뀌는 경우
	if (old_param->burstcnt != new_param->burstcnt) {
		if (out_change_time != NULL) {
			*out_change_time = MODETIME_FIREMODE_CHANGE;
		}
		return WEAPONMODE_FIREMODE_CHANGE;
	}

	return -1;
}

//! @brief 븧딇궻롰쀞먛귟뫶궑
//! @param human_id 릐궻붥뜂걁0?MAX_HUMAN-1걂
//! @return 맟뚻갌0?렪봲갌1
int ObjectManager::ChangeWeaponID(int human_id)
{
	if ((human_id < 0) || (MAX_HUMAN <= human_id)) { return 1; }

	int selectweapon;
	weapon* weapon[TOTAL_HAVEWEAPON];
	int oldWeaponID = -1;
	int newWeaponID = -1;
	int modeSoundID = -1;
	int modeChangeTime = 0;

	HumanIndex[human_id].GetWeapon(&selectweapon, weapon, NULL, NULL);

	if (weapon[selectweapon] != NULL) {
		WeaponParameter oldParam;
		WeaponParameter newParam;

		weapon[selectweapon]->GetParamData(&oldWeaponID, NULL, NULL);

		if (GameParamInfo->GetWeapon(oldWeaponID, &oldParam) == 0) {
			newWeaponID = oldParam.ChangeWeaponID;

			if (newWeaponID != -1 && newWeaponID != oldWeaponID) {
				if (GameParamInfo->GetWeapon(newWeaponID, &newParam) == 0) {
					modeSoundID = GetWeaponModeChangeInfo(
						oldWeaponID,
						&oldParam,
						newWeaponID,
						&newParam,
						&modeChangeTime
					);
				}
			}
		}
	}

	if (HumanIndex[human_id].ChangeWeaponID(modeChangeTime) == 0) {
		int model, texture;
		float WeaponSize;
		int WeaponID;

		// 총 모드 변경음 재생
		if (modeSoundID != -1) {
			float sx, sy, sz;
			int steamid;

			HumanIndex[human_id].GetPosData(&sx, &sy, &sz, NULL);
			HumanIndex[human_id].GetParamData(NULL, NULL, NULL, &steamid);
			sy += 16.0f;

			bool isPlayer = (human_id == Player_HumanID);
			GameSound->WeaponModeChange(sx, sy, sz, modeSoundID, steamid, isPlayer);
		}

		// 기존 코드 유지
		HumanIndex[human_id].GetWeapon(&selectweapon, weapon, NULL, NULL);
		weapon[selectweapon]->GetParamData(&WeaponID, NULL, NULL);

		if (Resource->GetWeaponModelTexture(WeaponID, &model, &texture) == 1) {
			return 1;
		}

		WeaponParameter ParamData;
		if (GameParamInfo->GetWeapon(WeaponID, &ParamData) != 0) { return 1; }

		WeaponSize = ParamData.size;

		weapon[selectweapon]->SetModel(model, WeaponSize);
		weapon[selectweapon]->SetTexture(texture);

		Human_YakkyouCnt[human_id] = -1;

		return 0;
	}

	return 1;
}

//! @brief 븧딇귩롆궲귡
//! @param human_id 릐궻붥뜂걁0?MAX_HUMAN-1걂
//! @return 맟뚻갌true?렪봲갌false
bool ObjectManager::DumpWeapon(int human_id)
{
	//뭠궻붝댪귩?긃긞긏
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return false; }

	Human_YakkyouCnt[human_id] = -1;

	return HumanIndex[human_id].DumpWeapon(CollD);
}

//! @brief 긚긓?긵긾?긤귩먛귟뫶궑
//! @param human_id 릐궻붥뜂걁0?MAX_HUMAN-1걂
void ObjectManager::ChangeScopeMode(int human_id)
{
	//뭠궻붝댪귩?긃긞긏
	if ((human_id < 0) || (MAX_HUMAN <= human_id)) { return; }

	// 재장전/셀 단위 재장전/볼트 동작 중에는 스코프를 새로 켜지 못하게 한다.
	// 이미 재장전 시작 시 스코프는 해제되므로, 여기서는 재활성화만 막는다.
	if ((HumanIndex[human_id].GetWeaponReloadCnt() > 0) ||
		(HumanIndex[human_id].IsShellReloading() == true) ||
		(HumanIndex[human_id].GetBoltActionTimer() > 0)) {
		HumanIndex[human_id].SetDisableScope();
		return;
	}

	if (HumanIndex[human_id].GetScopeMode() == 0) {	//긚긓?긵귩럊뾭궢궲궋궶궚귢궽갂긚긓?긵귩먠믦
		HumanIndex[human_id].SetEnableScope();
	}
	else{											//럊뾭뭷궶귞갂됶룣
		HumanIndex[human_id].SetDisableScope();
	}

	//Human_YakkyouCnt[human_id] = -1;
}

//! @brief 뿞땆갋룋렃궢궲궋귡븧딇궻뭙귩믁돿
//! @param human_id 릐궻붥뜂걁0?MAX_HUMAN-1걂
//! @return 맟뚻갌true?렪봲갌false
bool ObjectManager::CheatAddBullet(int human_id)
{
	//뭠궻붝댪귩?긃긞긏
	if( (human_id < 0)||(MAX_HUMAN <= human_id) ){ return false; }

	int selectweapon;
	weapon *weapon[TOTAL_HAVEWEAPON];
	int id_param, lnbs, nbs;
	WeaponParameter ParamData;

	//룋렃궢궲궋귡븧딇귩롦벦
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
	}
	HumanIndex[human_id].GetWeapon(&selectweapon, weapon, NULL, NULL);

	//돺궔궢귞궻븧딇귩렃궯궲궋귢궽
	if( weapon[selectweapon] != NULL ){
		//븧딇궻롰쀞궴뭙릶갂븧딇궻먠믦뭠귩롦벦
		weapon[selectweapon]->GetParamData(&id_param, &lnbs, &nbs);
		if( GameParamInfo->GetWeapon(id_param, &ParamData) == 0 ){
			//띍묈뭙릶빁돿럁궢갂밙뾭
			nbs += ParamData.nbsmax;
			weapon[selectweapon]->ResetWeaponParam(Resource, id_param, lnbs, nbs);
			return true;
		}
	}

	return false;
}

//! @brief 뿞땆갋룋렃궢궲궋귡븧딇귩빾뛛
//! @param human_id 릐궻붥뜂걁0?MAX_HUMAN-1걂
//! @param new_weaponID 륷궫궸먠믦궥귡븧딇궻롰쀞붥뜂
//! @return 맟뚻갌true?렪봲갌false
bool ObjectManager::CheatNewWeapon(int human_id, int new_weaponID)
{
	// 값의 범위를 체크한다.
	if ((human_id < 0) || (MAX_HUMAN <= human_id)) { return false; }
	if ((new_weaponID < 0) || (TOTAL_PARAMETERINFO_WEAPON <= new_weaponID)) { return false; }

	// 처형 프로토콜 전용 무기는 실제 획득/치트 교체 대상이 아니다.
	// 렌더링 override와 총구화염/탄피 연출에서만 사용한다.
	if (new_weaponID == ID_WEAPON_ROBOT_EXECUTE_RIFLE) {
		return false;
	}

	human* myHuman = &(HumanIndex[human_id]);

	int selectweapon;
	weapon *weapon[TOTAL_HAVEWEAPON];
	int lnbslist[TOTAL_HAVEWEAPON];
	int nbslist[TOTAL_HAVEWEAPON];
	int lnbs, nbs;

	//룋렃궢궲궋귡븧딇귩롦벦
	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
		lnbslist[i] = 0;
		nbslist[i] = 0;
	}
	myHuman->GetWeapon(&selectweapon, weapon, lnbslist, nbslist);

	//뙸띪븧딇귩룋뾎궢궲궓귞궦갂륷궫궸렃궫궧귡븧딇귖궶궋궶귞
	if ((weapon[selectweapon] == NULL) && (new_weaponID == ID_WEAPON_NONE)) {
		return false;		//룉뿚렔뫬궕뼰댰뼞
	}

	if( weapon[selectweapon] == NULL ){		//븧딇귩룋뾎궢궲궋궶궚귢궽
		int dataid = -1;

		//륷궢궋븧딇귩봹뭫
		dataid = AddVisualWeaponIndex(new_weaponID, false);

		//븧딇궕봹뭫궳궖귢궽
		if( dataid != -1 ){
			//븧딇먠믦귩밙뾭걁롥궸뭙릶걂
			weapon[selectweapon] = &(WeaponIndex[dataid]);
			weapon[selectweapon]->ResetWeaponParam(Resource, new_weaponID, lnbslist[selectweapon], nbslist[selectweapon]);

			//뼟룋렃렄궻뭙릶륃뺪궼룊딖돸
			lnbslist[selectweapon] = 0;
			nbslist[selectweapon] = 0;

			//븧딇귩렃궫궧귡
			myHuman->SetWeapon(weapon, lnbslist, nbslist);

			Human_YakkyouCnt[human_id] = -1;
			return true;
		}
	}
	else{									//븧딇귩룋뾎궢궲궋귢궽
		//븧딇먠믦귩롦벦걁뭙릶먠믦귩덙궖똯궙궫귕걂
		weapon[selectweapon]->GetParamData(NULL, &lnbs, &nbs);

		if( new_weaponID == ID_WEAPON_NONE ){			//븧딇귩뤑궥궶귞
			//궩궻븧딇궼랁룣
			weapon[selectweapon]->SetEnableFlag(false);

			//븧딇귩랁룣궢갂뭙릶귩뵿뎕
			weapon[selectweapon] = NULL;
			lnbslist[selectweapon] = lnbs;
			nbslist[selectweapon] = nbs;
			myHuman->SetWeapon(weapon, lnbslist, nbslist);

			//긚긓?긵긾?긤귩됶룣
			myHuman->SetDisableScope();
		}
		else {											//븧딇귩빾뛛궥귡궶귞
			//븧딇먠믦귩밙뾭
			weapon[selectweapon]->ResetWeaponParam(Resource, new_weaponID, lnbs, nbs);
		}

		Human_YakkyouCnt[human_id] = -1;
		return true;
	}

	return false;
}

bool ObjectManager::IsZombieAttackBlockedByShield(human* MyHuman, human* EnemyHuman, bool play_effect)
{
	if (MyHuman == NULL) { return false; }
	if (EnemyHuman == NULL) { return false; }

	if (MyHuman->GetEnableFlag() == false) { return false; }
	if (EnemyHuman->GetEnableFlag() == false) { return false; }
	if (MyHuman->GetHP() <= 0) { return false; }
	if (EnemyHuman->GetHP() <= 0) { return false; }
	if (MyHuman->GetDeadFlag() == true) { return false; }
	if (EnemyHuman->GetDeadFlag() == true) { return false; }

	// 공격을 받는 대상이 방패를 들고 있을 때만 정면 좀비 공격을 막는다.
	if (IsHumanShieldEquippedForBullet(EnemyHuman) == false) {
		return false;
	}

	float mx, my, mz;
	float tx, ty, tz, trx;

	MyHuman->GetPosData(&mx, &my, &mz, NULL);
	EnemyHuman->GetPosData(&tx, &ty, &tz, &trx);

	// 높이 차이가 큰 경우에는 다른 층/계단 상황일 가능성이 있으므로 막지 않는다.
	if (fabsf(my - ty) > SHIELD_HUMAN_PUSH_MAX_Y_DIFF) {
		return false;
	}

	float shield_forward_x = cosf(trx * -1.0f + (float)M_PI / 2.0f);
	float shield_forward_z = sinf(trx * -1.0f + (float)M_PI / 2.0f);
	float shield_right_x = shield_forward_z;
	float shield_right_z = -shield_forward_x;

	float rel_x = mx - tx;
	float rel_z = mz - tz;

	float side = rel_x * shield_right_x + rel_z * shield_right_z;
	float front = rel_x * shield_forward_x + rel_z * shield_forward_z;

	float side_limit = SHIELD_APPROX_HALF_WIDTH + SHIELD_HUMAN_PUSH_SIDE_MARGIN;

	// 방패 정면에 있는 좀비 공격만 막는다.
	// 측면/후면 공격은 기존처럼 통과시킨다.
	if (front <= 0.0f) { return false; }
	if (fabsf(side) > side_limit) { return false; }

	// 너무 멀리 있는 대상은 방패 근접 차단으로 보지 않는다.
	// CheckZombieAttack() 또는 AI 근접 처리 안에서 호출되므로 넉넉한 근접 범위만 둔다.
	if (front > SHIELD_HUMAN_PUSH_MIN_FRONT + HUMAN_HUMANCOLLISION_R) {
		return false;
	}

	if (play_effect == true) {
		float clamped_side = side;

		if (clamped_side < -SHIELD_APPROX_HALF_WIDTH) {
			clamped_side = -SHIELD_APPROX_HALF_WIDTH;
		}
		if (clamped_side > SHIELD_APPROX_HALF_WIDTH) {
			clamped_side = SHIELD_APPROX_HALF_WIDTH;
		}

		float effect_front = SHIELD_APPROX_FRONT_MAX + SHIELD_EFFECT_FRONT_OFFSET;

		float effect_x = tx + shield_forward_x * effect_front + shield_right_x * clamped_side;
		float effect_y = ty + VIEW_HEIGHT;
		float effect_z = tz + shield_forward_z * effect_front + shield_right_z * clamped_side;

		int attacker_team = -1;
		MyHuman->GetParamData(NULL, NULL, NULL, &attacker_team);

		SetShieldBlockBulletEffect(effect_x, effect_y, effect_z, 25, shield_forward_x, shield_forward_z);
		GameSound->HitShieldBlock(effect_x, effect_y, effect_z, attacker_team);
	}

	return true;
}

//! @brief ?깛긮궻뛘똼귩롷궚귡궔뵽믦
//! @param MyHuman 뛘똼궥귡릐긆긳긙긃긏긣걁?깛긮뫀걂궻?귽깛?
//! @param EnemyHuman 뛘똼귩롷궚궫릐긆긳긙긃긏긣궻?귽깛?
//! @return 맟뿧갌true?븉맟뿧갌false
//! @warning MyHuman 궼?깛긮댥둖귖럚믦궳궖귏궥갃<b>?깛긮궔궵궎궔궼뵽믦궢궶궋</b>궫귕갂?깛긮궳궇귡궔?귕둴봃궢궲궓궘뷠뾴궕궇귟귏궥갃
//! @attention 뵽믦궻귒귩렳?궢귏궥갃궞궻뵽믦궕맟뿧궢궫귞 HitZombieAttack()듫릶 귩뚁귂뢯궢궲궘궬궠궋갃 
bool ObjectManager::CheckZombieAttack(human* MyHuman, human* EnemyHuman)
{
	if( MyHuman == NULL ){ return false; }
	if( EnemyHuman == NULL ){ return false; }

	//럊뾭궠귢궲궋궶궋궔갂?뻊궢궲궋귢궽룉뿚궢궶궋갃
	if( MyHuman->GetEnableFlag() == false ){ return false; }
	if( MyHuman->GetHP() <= 0 ){ return false; }
	if( EnemyHuman->GetEnableFlag() == false ){ return false; }
	if( EnemyHuman->GetDeadFlag() == true ){ return false; }

	float mx, my, mz, mrx, tx, ty, tz;
	int mteam, tteam;
	float AttackPoint_x, AttackPoint_y, AttackPoint_z;
	float ax, az;

	MyHuman->GetPosData(&mx, &my, &mz, &mrx);
	MyHuman->GetParamData(NULL, NULL, NULL, &mteam);
	my += VIEW_HEIGHT;
	EnemyHuman->GetPosData(&tx, &ty, &tz, NULL);
	EnemyHuman->GetParamData(NULL, NULL, NULL, &tteam);
	ty += VIEW_HEIGHT;

	//뼞뺴궶귞궽룉뿚궢궶궋
	if (mteam == tteam) { return false; }

	// 방패를 든 대상의 정면에서 좀비가 충분히 가까이 있으면,
	// 일반 좀비 공격 포인트 판정이 밀어내기 때문에 닿지 않더라도 공격 판정은 성립시킨다.
	// 실제 피해 차단, 스파크, 효과음은 HitZombieAttack() 안의 IsZombieAttackBlockedByShield(..., true)에서 처리한다.
	if (IsZombieAttackBlockedByShield(MyHuman, EnemyHuman, false) == true) {
		return true;
	}

	// 기본 좀비 공격 사거리 및 오프셋 설정
	float attack_range = 3.3f;
	float attack_offset = 2.0f; // 공격 시작점 오프셋

	// 공격자가 플레이어인지 확인
	if (GetHumanObjectID(MyHuman) == Player_HumanID) {
		// 플레이어 좀비의 사거리를 더 길게 설정
		attack_range = 7.0f; // (기존 3.3f -> 7.0f. 이 값을 원하는 대로 조절하세요)
		attack_offset = 4.0f; // (기존 2.0f -> 4.0f. 플레이어의 공격 시작점을 더 앞으로)
	}

	//뛘똼?귽깛긣걁쁱궻먩?걂귩땫귕귡
	float attack_angle = mrx * -1.0f + (float)M_PI / 2.0f;
	AttackPoint_x = mx + cosf(attack_angle) * attack_offset;
	AttackPoint_y = my + VIEW_HEIGHT - 0.5f;
	AttackPoint_z = mz + sinf(attack_angle) * attack_offset;
	ax = AttackPoint_x - tx;
	az = AttackPoint_z - tz;

	//밎걁뛘똼뫮뤭걂궕뛘똼?귽깛긣궸륢귢궲궋귢궽갂뱰궫궯궲궋귡
	if ((ax * ax + az * az) < (attack_range * attack_range)) {	// 3.3f*3.3f -> attack_range * attack_range
		if ((AttackPoint_y >= ty) && (AttackPoint_y <= (ty + HUMAN_HEIGHT))) {
			return true;
		}
	}

	return false;
}

//! @brief ?깛긮궻뛘똼귩롷궚궫룉뿚
//! @param MyHuman 뛘똼궥귡릐긆긳긙긃긏긣걁?깛긮뫀걂궻?귽깛?
//! @param EnemyHuman 뛘똼귩롷궚궫릐긆긳긙긃긏긣궻?귽깛?
void ObjectManager::HitZombieAttack(human* MyHuman, human* EnemyHuman)
{
	if( MyHuman == NULL ){ return; }
	if( EnemyHuman == NULL ){ return; }

	//럊뾭궠귢궲궋궶궋궔갂?뻊궢궲궋귢궽룉뿚궢궶궋갃
	if( EnemyHuman->GetEnableFlag() == false ){ return; }
	if( EnemyHuman->GetDeadFlag() == true ){ return; }

	int MyHuman_dataID, MyHuman_TeamID;
	int EnemyHuman_dataID, EnemyHuman_TeamID;
	float mx, my, mz;
	float tx, ty, tz;
	float arx;
	int paramid;
	HumanParameter Paraminfo;
	bool NotRobot;
	int hp_old;

	//?깛긮뫀궴뛘똼귩롷궚귡뫀???붥뜂롦벦
	MyHuman->GetParamData(NULL, &MyHuman_dataID, NULL, &MyHuman_TeamID);
	EnemyHuman->GetParamData(NULL, &EnemyHuman_dataID, NULL, &EnemyHuman_TeamID);

	MyHuman->GetPosData(&mx, &my, &mz, NULL);
	EnemyHuman->GetPosData(&tx, &ty, &tz, NULL);
	ty += VIEW_HEIGHT;

	// 방패를 든 대상의 정면에서 들어오는 좀비 공격은 피해 없이 막는다.
	// 측면/후면 좀비 공격은 기존처럼 통과한다.
	if (IsZombieAttackBlockedByShield(MyHuman, EnemyHuman, true) == true) {
		return;
	}

	//밎궕뛘똼귩롷궚궫둷뱗귩땫귕귡
	arx = atan2f(mz - tz, mx - tx);

	//깓?긞긣궔궵궎궔뵽믦
	EnemyHuman->GetParamData(&paramid, NULL, NULL, NULL);
	GameParamInfo->GetHuman(paramid, &Paraminfo);
	if( Paraminfo.type == 1 ){
		NotRobot = false;
	}
	else{
		NotRobot = true;
	}

	// ✨ [수정] 공격자가 플레이어(좀비)인지 확인
	int attacker_id = GetHumanObjectID(MyHuman);
	if (attacker_id == Player_HumanID) {
		// 플레이어인 경우, 히트마커 타이머 설정
		MyHuman->SetHitMarkerTimer(8);
		MyHuman->SetHeadshotFlag(false); // 좀비는 헤드샷이 없으므로 false로 설정
	}

	// 좀비 격노 효과는 플레이어 좀비 공격에만 적용한다.
	bool player_rage_attack =
		(attacker_id == Player_HumanID) &&
		(MyHuman->GetSkillRageFlag() == true);

	//?긽?긙똶럁멟궻HP롦벦
	hp_old = EnemyHuman->GetHP();

	bool enemy_shadow_decoy = EnemyHuman->GetSkillShadowDecoyFlag();

	// 기본 좀비 공격 피해
	EnemyHuman->HitZombieAttack();

	if (enemy_shadow_decoy == true) {
		int attacker_id = GetHumanObjectID(MyHuman);

		if ((0 <= attacker_id) && (attacker_id < MAX_HUMAN)) {
			EnemyHuman->SetSkillShadowDecoyLastAttackerID(attacker_id);
		}
	}

	// 좀비 격노: 공격력 20% 증가 + 실제 준 피해의 20% 회복
	if (player_rage_attack == true) {
		int hp_after_base_damage = EnemyHuman->GetHP();
		int base_dealt_damage = hp_old - hp_after_base_damage;

		// 공격력 20% 증가
		// 기존 human::HitZombieAttack()은 고정 피해 함수라 직접 배율을 넘길 수 없으므로,
		// 실제로 들어간 기본 피해량의 20%를 추가 피해로 한 번 더 적용한다.
		if (base_dealt_damage > 0) {
			int bonus_damage = (int)((float)base_dealt_damage * (PLAYER_SKILL_RAGE_ATTACK_DAMAGE_MULT - 1.0f) + 0.5f);

			if (bonus_damage < 1) {
				bonus_damage = 1;
			}

			EnemyHuman->HitGrenadeExplosion(bonus_damage);
		}

		// 최종 실제 피해량 기준 흡혈
		int hp_after_all_damage = EnemyHuman->GetHP();
		int dealt_damage = hp_old - hp_after_all_damage;

		if (dealt_damage > 0) {
			int heal_value = (int)((float)dealt_damage * PLAYER_SKILL_RAGE_LIFESTEAL_RATE + 0.5f);

			if (heal_value < 1) {
				heal_value = 1;
			}

			MyHuman->HealHP(heal_value);
		}
	}

#ifdef ENABLE_BUG_TEAMID
	bool flag = true;

	//???붥뜂궕븠릶갂궔궰???붥뜂궕묈궖궋궶귞갂긲깋긐뼰뚼
	if( (EnemyHuman_TeamID < 0)&&(MyHuman_TeamID < 0) ){
		if( EnemyHuman_TeamID < MyHuman_TeamID ){
			flag = false;
		}
	}

	if( flag == true ){
		EnemyHuman->SetHitFlag(arx*-1 + (float)M_PI/2);
	}
#else
	EnemyHuman->SetHitFlag(arx*-1 + (float)M_PI/2);
#endif

	//긄긲긃긏긣걁뙆걂귩?됪
	if (enemy_shadow_decoy == false) {
		SetHumanBlood(tx, ty, tz, HUMAN_DAMAGE_ZOMBIEU, NotRobot);
	}

	//뚼됈돶귩띋맯
	GameSound->HitHuman(tx, ty, tz, 3, MyHuman_TeamID);

	//깓긐듫똚궻룉뿚
	if( (hp_old > 0)&&(EnemyHuman->GetHP() <= 0) ){
		//?긽?긙똶럁멟궸 hp>0 궳궔궰갂똶럁뚣궸 hp <= 0 궶귞갂뜞됷?뻊궢궫갃

		int player_teamid;
		HumanIndex[Player_HumanID].GetParamData(NULL, NULL, NULL, &player_teamid);

		ObjectLog->DiedHuman(MyHuman_dataID, EnemyHuman_dataID, MyHuman_TeamID, EnemyHuman_TeamID, player_teamid);
	}
}

//! @brief ?롌귩멻맯궥귡
//! @param id 릐궻붥뜂걁0?MAX_HUMAN-1걂
//! @return true갌맟뚻?false갌렪봲
//! @warning 롨귆귞궻귏귏멻맯궢귏궥
//! @attention 뼰뚼궶릐붥뜂궕럚믦궠귢궫뤾뜃귘갂럚믦궢궫릐궕맯뫔궢궲궋궫뤾뜃갂궇귡궋궼볞릐듩궸뫮궢궲렳뛱궢궫뤾뜃갂궞궻듫릶궼렪봲궢귏궥갃
bool ObjectManager::HumanResuscitation(int id)
{
	if( (id < 0)||(MAX_HUMAN <= id) ){ return false; }

	//럊뾭궠귢궲궋궶궋궔갂맯뫔궢궲궋귢궽룉뿚궢궶궋갃
	if( HumanIndex[id].GetEnableFlag() == false ){ return false; }
	if( HumanIndex[id].GetDeadFlag() == false ){ return false; }

	int id_param, dataid, team;
	signed short int p4;
	HumanIndex[id].GetParamData(&id_param, &dataid, &p4, &team);
	if( (id_param < 0)||( TOTAL_PARAMETERINFO_HUMAN <= id_param) ){ return false; }	//볞릐듩궶귞룉뿚궢궶궋
	HumanIndex[id].SetParamData(id_param, dataid, p4, team, true);

	//깓긐듫똚궻룉뿚
	int player_teamid;
	HumanIndex[Player_HumanID].GetParamData(NULL, NULL, NULL, &player_teamid);
	ObjectLog->ReviveHuman(id, team, player_teamid);

	return true;
}

//! @brief 긒??긏깏귺?갋긒??긆?긫?궻뵽믦
//! @return 긒??긏깏귺?갌1?긒??긆?긫?갌2?뵽믦궶궢갌0
//! @attention 긒??긏깏귺?궴긒??긆?긫?궕벏렄궸맟뿧궥귡륆뙊궳궼갂?됄XOPS궴벏뾩궸걏긒??긏깏귺?걐궴뵽믦궠귢귏궥갃
int ObjectManager::CheckGameOverorComplete()
{
	//긽긾갌
	//
	//??뿀궼갂긒??긆?긫?뵽믦귩먩궸뛱궋갂렅궸긒??긏깏귺?뵽믦귩렳몧궢궫뺴궕뚼뿦밒궳궥갃
	//?궢궔궢갂?됄XOPS궼긒??긏깏귺?궻뵽믦귩뾆먩궥귡궫귕갂먩궸긒??긏깏귺?궻뵽믦귩뛱궯궲궋귏궥갃
	//?맯궖궲궋귡밎궻릶귩걁몟HP궴궢궲걂릶궑갂밎궻릶걁몟HP걂궕 0 궶귞궽갂긒??긏깏귺?궴뵽믦궢귏궥갃

	//긒??긏깏귺?뵽믦
	int MyTeamid, teamid;
	int TotalEnemyHP = 0;
	HumanIndex[Player_HumanID].GetParamData(NULL, NULL, NULL, &MyTeamid);	//긵깒귽깂?궻???붥뜂귩롦벦
	for (int i = 0; i < MAX_HUMAN; i++) {
		// 활성화 되지 않은 인물은 skip 하는 코드 추가
		if (HumanIndex[i].GetEnableFlag() == false) { continue; }


		//뮧귊귡뫮뤭궻???붥뜂귩롦벦
		HumanIndex[i].GetParamData(NULL, NULL, NULL, &teamid);

		//댶궶귡???붥뜂걁걖밎걂궶귞궽
		if( teamid != MyTeamid ){
			//맯궖궲궋귢궽갂밎궻HP궴궢궲돿럁
			//if( HumanIndex[i].GetDeadFlag() == false ){
			if( HumanIndex[i].GetHP() > 0 ){
				TotalEnemyHP += HumanIndex[i].GetHP();
			}
		}
	}
	if( TotalEnemyHP == 0 ){	//멣밎궻HP궕 0 궶귞궽
		return 1;
	}


	//긒??긆?긫?뵽믦
	if( HumanIndex[Player_HumanID].GetEnableFlag() == true ){	//?띿뫮뤭궕뾎뚼궶귞궽?걁뭾갌뿞땆궸귝귡빾뛛뫮랉걂
		if( HumanIndex[Player_HumanID].GetDeadFlag() == true ){		//긵깒귽깂?궕?뻊궢궲궋귢궽
			return 2;
		}
	}

	return 0;
}

//! @brief 긆긳긙긃긏긣궻륃뺪귩롦벦
//! @param camera_x 긇긽깋궻X띆뷭
//! @param camera_y 긇긽깋궻Y띆뷭
//! @param camera_z 긇긽깋궻Z띆뷭
//! @param camera_rx 긇긽깋궻돘렡둷뱗
//! @param camera_ry 긇긽깋궻뢢렡둷뱗
//! @param color 륡귩롦벦궥귡?귽깛?
//! @param infostr 빒럻귩롦벦궥귡?귽깛?
//! @return ?렑륃뺪궇귟갌true??렑륃뺪궶궢갌false
bool ObjectManager::GetObjectInfoTag(float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, int *color, char *infostr)
{
	if( color == NULL ){ return false; }
	if( infostr == NULL ){ return false; }

	float dist = 50.0f;
	float px, py, pz;
	float rx, ry;
	float r;
	int Player_teamID;

	//빒럻귩룊딖돸
	infostr[0] = '\0';

	//긵깒귽깂?궻???붥뜂귩롦벦
	HumanIndex[Player_HumanID].GetParamData(NULL, NULL, NULL, &Player_teamID);

	//릐
	for(int i=0; i<MAX_HUMAN; i++){
		//긵깒귽깂?렔릆궶귞룉뿚궢궶궋
		if( i == Player_HumanID ){ continue; }

		if( HumanIndex[i].GetEnableFlag() == true ){
			HumanIndex[i].GetPosData(&px, &py, &pz, NULL);

			//럨?귩딈?궸뫮뤭귏궳궻둷뱗귩럁뢯
			if( CheckTargetAngle(camera_x, camera_y, camera_z, camera_rx, 0.0f, px, py, pz, dist, &rx, NULL, &r) == true ){
				int team;

				//둷뱗뤵갂럨둉궸볺궯궲궋귢궽
				if( fabsf(rx) < DegreeToRadian(10) ){
					HumanIndex[i].GetParamData(NULL, NULL, NULL, &team);
					if( team == Player_teamID ){
						*color = d3dg->GetColorCode(0.0f,0.0f,1.0f,1.0f);
						sprintf(infostr, "Human[%d]  HP %d : Friend", i, HumanIndex[i].GetHP());
					}
					else{
						*color = d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f);
						sprintf(infostr, "Human[%d]  HP %d : Enemy", i, HumanIndex[i].GetHP());
					}
					dist = sqrtf(r);
				}
			}
		}
	}

	//븧딇
	for(int i=0; i<MAX_WEAPON; i++){
		if( (WeaponIndex[i].GetEnableFlag() == true)&&(WeaponIndex[i].GetUsingFlag() == false) ){
			int lnbs, nbs;

			WeaponIndex[i].GetPosData(&px, &py, &pz, NULL);
			WeaponIndex[i].GetParamData(NULL, &lnbs, &nbs);

			//럨?귩딈?궸뫮뤭귏궳궻둷뱗귩럁뢯
			if( CheckTargetAngle(camera_x, camera_y, camera_z, camera_rx, camera_ry, px, py, pz, dist, &rx, &ry, &r) == true ){
				//둷뱗뤵갂럨둉궸볺궯궲궋귢궽
				if( (fabsf(rx) < DegreeToRadian(10))&&(fabsf(ry) < DegreeToRadian(10)) ){
					*color = d3dg->GetColorCode(0.0f,1.0f,0.0f,1.0f);
					sprintf(infostr, "Weapon[%d]  %d:%d", i, lnbs, (nbs - lnbs));
					dist = sqrtf(r);
				}
			}
		}
	}

	//룷븿
	for(int i=0; i<MAX_SMALLOBJECT; i++){
		if( SmallObjectIndex[i].GetEnableFlag() == true ){
			SmallObjectIndex[i].GetPosData(&px, &py, &pz, NULL);
			
			//럨?귩딈?궸뫮뤭귏궳궻둷뱗귩럁뢯
			if( CheckTargetAngle(camera_x, camera_y, camera_z, camera_rx, camera_ry, px, py, pz, dist, &rx, &ry, &r) == true ){
				//둷뱗뤵갂럨둉궸볺궯궲궋귢궽
				if( (fabsf(rx) < DegreeToRadian(10))&&(fabsf(ry) < DegreeToRadian(10)) ){
					*color = d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f);
					sprintf(infostr, "SmallObject[%d]  HP %d", i, SmallObjectIndex[i].GetHP());
					dist = sqrtf(r);
				}
			}
		}
	}

	//빒럻궕먠믦궠귢궲궋귡궔궵궎궔궳갂뾎뚼궶룉뿚궕궠귢궫궔뵽믦갃
	if( infostr[0] == NULL ){
		return false;
	}
	return true;
}


//! @brief 긆긳긙긃긏긣궻롥똶럁룉뿚
//! @param cmdF5id 뤵뤈??걁F5뿞땆걂궠궧귡릐긢??붥뜂걁-1궳??뼰뚼걂
//! @param demomode 긢긾긾?긤
//! @param screen 됪뽋귩댠궘궥귡
//! @return 륂궸 0
//! @attention 덇붗밒궸 cmdF5id 궼갂F5뿞땆럊뾭뭷궼긵깒귽깂?붥뜂걁GetPlayerID()듫릶궳롦벦걂갂뼟럊뾭렄궼 -1 귩럚믦궢귏궥갃
//! @attention demomode 궼롥궸긽긦깄?됪뽋궳럊뾭궢귏궥갃뾎뚼궸궥귡궴갂뢤뭙갋롨왮뭙귩룉뿚궢귏궧귪갃
int ObjectManager::Process(int cmdF5id, bool demomode, bool screen)
{

	//궞궻긲깒??궻먰쀰귩룊딖돸
	for(int i=0; i<MAX_HUMAN; i++){
		Human_ontarget[i] = 0.0f;
		Human_kill[i] = 0;
		Human_headshot[i] = 0;
	}

	// [✨ 1. 여기에 착지 쿨타임 감소 ✨]
	for (int i = 0; i < MAX_HUMAN; i++) {
		if (RuntimeLandingCooldown[i] > 0) {
			RuntimeLandingCooldown[i]--;
		}
	}

	// 2. 물리/로직 연산은 ActiveHumans만 순회하도록 변경
	for (int i = 0; i < MAX_HUMAN; i++) {
		// 활성화 되지 않은 인물은 skip 하는 코드 추가
		if (HumanIndex[i].GetEnableFlag() == false) { continue; }

		bool cmdF5, player;
		int rtn;
		int block_id = -1;
		int block_face = -1;
		int block_textureid = -1;
		int MoveMode;
		int JumpLandingMode;

		// [✨ 2. 높이 변화량(내려가고 있는지) 계산 ✨]
		float current_x, current_y, current_z;
		HumanIndex[i].GetPosData(&current_x, &current_y, &current_z, NULL);
		float delta_y = current_y - RuntimeLastY[i];
		RuntimeLastY[i] = current_y;

		//긵깒귽깂?궔궵궎궔뵽믦
		if( Player_HumanID == i ){
			player = true;
		}
		else{
			player = false;
		}

		if( i == cmdF5id ){
			cmdF5 = true;
		}
		else{
			cmdF5 = false;
		}

		rtn = HumanIndex[i].ProcessObject(CollD, BlockData, AddCollisionFlag, player, cmdF5);

		// 셸 단위 재장전 중 탄 삽입 / 종료 사운드 처리
		{
			human::ShellReloadState shellState = HumanIndex[i].GetShellReloadState();

			int selectweapon = -1;
			weapon* haveweapon[TOTAL_HAVEWEAPON];
			int loadbullets[TOTAL_HAVEWEAPON];
			int bullets[TOTAL_HAVEWEAPON];

			HumanIndex[i].GetWeapon(&selectweapon, haveweapon, loadbullets, bullets);

			if ((selectweapon >= 0) && (selectweapon < TOTAL_HAVEWEAPON) && (haveweapon[selectweapon] != NULL)) {
				int weaponParamID = -1;
				int currentLoad = 0;
				int totalAmmo = 0;
				WeaponParameter WParam;

				haveweapon[selectweapon]->GetParamData(&weaponParamID, &currentLoad, &totalAmmo);

				if (GameParamInfo->GetWeapon(weaponParamID, &WParam) == 0 && WParam.is_shell_reload == true) {
					float sx, sy, sz;
					int steamid;

					HumanIndex[i].GetPosData(&sx, &sy, &sz, NULL);
					HumanIndex[i].GetParamData(NULL, NULL, NULL, &steamid);
					sy += 16.0f;

					// 탄 수가 증가한 순간 = 셸 1발 삽입 완료
					// 탄 수가 증가한 순간 = 셸 1발 삽입 완료
					if (Human_ShellReloadPrevState[i] == human::SHELL_RELOAD_INSERTING &&
						Human_ShellReloadPrevLoad[i] >= 0 &&
						currentLoad > Human_ShellReloadPrevLoad[i]) {

						if (WParam.SoundID_Reload != RELOAD_NONE) {
							bool isPlayerReload = (i == Player_HumanID);
							GameSound->ReloadWeapon(sx, sy, sz, WParam.SoundID_Reload, steamid, isPlayerReload);
						}
					}


					// END_DELAY로 처음 들어간 순간 = 재장전 종료음
					if (shellState == human::SHELL_RELOAD_END_DELAY &&
						Human_ShellReloadPrevState[i] != human::SHELL_RELOAD_END_DELAY) {

						if (WParam.SoundID_ReloadEnd != RELOAD_NONE) {
							bool isPlayerReload = (i == Player_HumanID);
							GameSound->ReloadWeapon(sx, sy, sz, WParam.SoundID_ReloadEnd, steamid, isPlayerReload);
						}
					}

					Human_ShellReloadPrevLoad[i] = currentLoad;
					Human_ShellReloadPrevState[i] = shellState;
				}
				else {
					Human_ShellReloadPrevLoad[i] = -1;
					Human_ShellReloadPrevState[i] = human::SHELL_RELOAD_NONE;
				}
			}
		}
		
		if (rtn == 5) {
			ShotWeapon(i);
		}
		if( rtn == 2 ){
			//?뻊렄궻긄긲긃긏긣
			DeadEffect(&(HumanIndex[i]));
		}
		if( rtn == 4 ){
			int teamid, player_teamid;
			HumanIndex[i].GetParamData(NULL, NULL, NULL, &teamid);
			HumanIndex[Player_HumanID].GetParamData(NULL, NULL, NULL, &player_teamid);

			//깓긐듫똚궻룉뿚
			ObjectLog->DiedHuman(-1, i, -1, teamid, player_teamid);
		}

		HumanIndex[i].SetDarkModelFlag(screen);

		// [✨ 1. 핵심: 현재 프레임의 체공 시간을 먼저 백업해 둡니다 ✨]
		// 바닥 판정 때문에 0으로 초기화되기 전에, 공중에 얼마나 떠있었는지 기억합니다.
		int current_falling = RuntimeFallingFrames[i];

		//뫉뙰궸궇귡긳깓긞긏궻긡긏긚?긿?붥뜂귩롦벦궥귡
		HumanIndex[i].GetUnderBlock(&block_id, &block_face);
		if (block_id != -1) {
			blockdata bdata;
			BlockData->Getdata(&bdata, block_id);
			block_textureid = bdata.material[block_face].textureID;
			RuntimeLastTexture[i] = block_textureid;
			RuntimeFallingFrames[i] = 0;
			RuntimeGroundGrace[i] = 6;
		}
		else {
			block_textureid = -1;
			RuntimeFallingFrames[i]++;
			if (RuntimeGroundGrace[i] > 0) {
				RuntimeGroundGrace[i]--;
			}
		}
		MoveMode = HumanIndex[i].GetMovemode(false);
		JumpLandingMode = HumanIndex[i].GetJumpLanding(false);

		// 진짜 착지/계단 내려가기일 때만 일반 발소리를 막는다.
		// 오르막/경사면에서 발생하는 작은 높이 보정은 JumpLandingMode == 2가 되어도 일반 발소리를 허용한다.
		bool heavy_landing_now =
			(JumpLandingMode == 2) &&
			(current_falling > 8);

		bool small_down_landing_now =
			(JumpLandingMode == 2) &&
			(current_falling > 0) &&
			(delta_y < -0.5f);

		bool suppress_regular_footstep =
			(JumpLandingMode == 1) ||
			(heavy_landing_now == true) ||
			(small_down_landing_now == true);


		// 일반 발소리
		if ((1 <= MoveMode) && (MoveMode <= 4) &&
			(RuntimeGroundGrace[i] > 0) &&
			(RuntimeLastTexture[i] != -1) &&
			(suppress_regular_footstep == false)) {
			float posx, posy, posz;
			int teamid;
			int cnt;
			HumanIndex[i].GetPosData(&posx, &posy, &posz, NULL);
			HumanIndex[i].GetParamData(NULL, NULL, NULL, &teamid);
			cnt = HumanIndex[i].GetMoveMotionCount();

			int sound_mode = MoveMode - 1;
			bool is_crouching = HumanIndex[i].GetCrouchFlag();

			if (is_crouching == true) {
				sound_mode = 7;
			}
			else if (sound_mode >= 1 && sound_mode <= 3 && HumanIndex[i].SpeedMultiplier > 1.2f) {
				sound_mode = 6;
			}

			GameSound->SetFootsteps(posx, posy, posz, RuntimeLastTexture[i], sound_mode, teamid, cnt);
		}

		// [✨ 4. 점프 소리 ✨]
		if (JumpLandingMode == 1) {
			int teamid;
			HumanIndex[i].GetParamData(NULL, NULL, NULL, &teamid);

			if (framecnt > 40 && RuntimeLastTexture[i] != -1) {
				GameSound->SetFootsteps(current_x, current_y, current_z, RuntimeLastTexture[i], 4, teamid, 0);
			}

			RuntimeFallingFrames[i] = 5;
		}

		// 무거운 착지 및 계단 내려가기 소리
		if (JumpLandingMode == 2) {
			if (RuntimeLandingCooldown[i] == 0) {
				int teamid;
				HumanIndex[i].GetParamData(NULL, NULL, NULL, &teamid);

				if (heavy_landing_now == true) {
					if (framecnt > 40 && RuntimeLastTexture[i] != -1) {
						GameSound->SetFootsteps(
							current_x,
							current_y,
							current_z,
							RuntimeLastTexture[i],
							5,
							teamid,
							0
						);
					}

					RuntimeLandingCooldown[i] = 15;
				}
				else if (small_down_landing_now == true) {
					bool is_running = (HumanIndex[i].SpeedMultiplier > 1.2f);

					if (framecnt > 40 && RuntimeLastTexture[i] != -1) {
						GameSound->SetFootsteps(
							current_x,
							current_y,
							current_z,
							RuntimeLastTexture[i],
							5,
							teamid,
							is_running ? 1 : 0
						);
					}

					RuntimeLandingCooldown[i] = is_running ? 9 : 14;
				}
			}

			RuntimeFallingFrames[i] = 0;
		}
	}

	//븧딇긆긳긙긃긏긣궻룉뿚
	for (int i = 0; i < MAX_WEAPON; i++) {
		if (WeaponIndex[i].GetEnableFlag() == false) { continue; }

		WeaponIndex[i].ProcessObject(CollD);
		WeaponIndex[i].SetDarkModelFlag(screen);
	}

	//룷븿긆긳긙긃긏긣궻룉뿚
	for (int i = 0; i < MAX_SMALLOBJECT; i++) {
		if (SmallObjectIndex[i].GetEnableFlag() == false) { continue; }
		SmallObjectIndex[i].ProcessObject();
		SmallObjectIndex[i].SetDarkModelFlag(screen);
	}

	if( demomode == false ){
		//뭙긆긳긙긃긏긣궻룉뿚
		for (int i = 0; i < MAX_BULLET; i++) {
			if (BulletIndex[i].GetEnableFlag() == false) { continue; }

			float bx, by, bz, brx, bry, speed;
			int teamid;
			float mx, my, mz;

			CollideBullet(&BulletIndex[i]);
			BulletIndex[i].ProcessObject();
			BulletIndex[i].SetDarkModelFlag(screen);

			BulletIndex[i].GetParamData(NULL, NULL, &speed, &teamid, NULL, NULL);
			BulletIndex[i].GetPosData(&bx, &by, &bz, &brx, &bry);

			mx = cosf(brx) * cosf(bry) * speed;
			my = sinf(bry) * speed;
			mz = sinf(brx) * cosf(bry) * speed;

			GameSound->PassingBullet(bx, by, bz, mx, my, mz, teamid);
		}
	}
	else{
		//멣궲궻뭙긆긳긙긃긏긣귩뤑궥
		for(int i=0; i<MAX_BULLET; i++){
			if( BulletIndex[i].GetEnableFlag() == true ){
				BulletIndex[i].SetEnableFlag(false);
			}
		}
	}

	// 무기를 발사했을 때의 효과를 출력
	for (int i = 0; i < MAX_HUMAN; i++) {
		if (Human_ShotFlag[i] == true) {
			ShotWeaponEffect(i);
			Human_ShotFlag[i] = false;
		}

		if (Human_YakkyouCnt[i] == 0) {
			ShotWeaponYakkyou(i);
		}
		if (Human_YakkyouCnt[i] > -1) { Human_YakkyouCnt[i] -= 1; }

		if (Human_ActionSoundTimer[i] > 0) {
			Human_ActionSoundTimer[i]--;
		}

		if (Human_MinigunSpinSoundTimer[i] > 0) {
			Human_MinigunSpinSoundTimer[i]--;
		}

		if (Human_ShieldBlockSwitchCooldown != NULL &&
			Human_ShieldBlockSwitchCooldown[i] > 0) {
			Human_ShieldBlockSwitchCooldown[i]--;
		}

		// 방패를 더 이상 들고 있지 않으면 누적 방어 카운트를 초기화한다.
		if (Human_ShieldBlockSwitchCount != NULL &&
			Human_ShieldBlockSwitchDamage != NULL &&
			IsHumanShieldEquippedForBullet(&HumanIndex[i]) == false) {
			Human_ShieldBlockSwitchCount[i] = 0;
			Human_ShieldBlockSwitchDamage[i] = 0;
		}

		if (Human_ActionSoundTimer[i] == 0) {
			if (Human_ActionSoundID[i] != RELOAD_NONE) {
				float x, y, z;
				int teamid;

				HumanIndex[i].GetPosData(&x, &y, &z, NULL);
				HumanIndex[i].GetParamData(NULL, NULL, NULL, &teamid);
				y += 16.0f;

				bool isPlayerReload = (i == Player_HumanID);
				GameSound->ReloadWeapon(x, y, z, Human_ActionSoundID[i], teamid, isPlayerReload);
			}

			Human_ActionSoundTimer[i] = -1;
			Human_ActionSoundID[i] = RELOAD_NONE;
			Human_ShellReloadPrevState[i] = human::SHELL_RELOAD_NONE;
			Human_ShellReloadPrevLoad[i] = -1;
		}
	}

	//긄긲긃긏긣긆긳긙긃긏긣궻룉뿚
	for (int i = 0; i < MAX_EFFECT; i++) {
		if (EffectIndex[i].GetEnableFlag() == false) { continue; }
		int id, face;
		float pos_x, pos_y, pos_z;
		if( CollideBlood(&(EffectIndex[i]), &id, &face, &pos_x, &pos_y, &pos_z) == true ){
			AddMapEffect(id, face, pos_x, pos_y, pos_z, 10.0f, DegreeToRadian(10)*GetRand(18), Resource->GetEffectBloodTexture(),  0, 0, 1.0f, -0.002f, (int)GAMEFPS * 20);
			EffectIndex[i].SetEnableFlag(false);
		}
		else{
			EffectIndex[i].ProcessObject();
			EffectIndex[i].SetDarkModelFlag(screen);
		}
	}

	if( demomode == false ){
		//롨왮뭙궻룉뿚
		for (int i = 0; i < MAX_GRENADE; i++) {
			if (GrenadeIndex[i].GetEnableFlag() == false) { continue; }

			float speed = GrenadeIndex[i].GetSpeed();

			int rcr = GrenadeIndex[i].ProcessObject(CollD);
			GrenadeIndex[i].SetDarkModelFlag(screen);

			if (rcr == 1) {
				if (speed > 3.4f) {
					float x, y, z;
					int teamid;
					GrenadeIndex[i].GetPosData(&x, &y, &z, NULL, NULL);
					GrenadeIndex[i].GetParamData(NULL, &teamid, NULL, NULL);
					GameSound->GrenadeBound(x, y, z, teamid);
				}
			}

			if (rcr == 2) {
				GrenadeExplosion(&(GrenadeIndex[i]));
			}
		}
	}
	else{
		//멣궲궻롨왮뭙귩뤑궥
		for(int i=0; i<MAX_GRENADE; i++){
			if( GrenadeIndex[i].GetEnableFlag() == true ){
				GrenadeIndex[i].SetEnableFlag(false);
			}
		}
	}


	//븧딇귩뢉궎룉뿚
	for (int i = 0; i < MAX_HUMAN; i++) {
		if (HumanIndex[i].GetEnableFlag() == false) continue;
		if (HumanIndex[i].GetHP() <= 0) continue;
		if (HumanIndex[i].GetDeadFlag() == true) continue;

		for (int j = (framecnt & 1); j < MAX_WEAPON; j += 2) {
			if (WeaponIndex[j].GetEnableFlag() == false) continue;
			if (WeaponIndex[j].GetUsingFlag() == true) continue;

			PickupWeapon(&(HumanIndex[i]), &(WeaponIndex[j]));
		}
	}

	for (int i = 0; i < MAX_HUMAN; i++) {
		if (HumanIndex[i].GetEnableFlag() == false) continue;
		if (HumanIndex[i].GetHP() <= 0) continue;
		if (HumanIndex[i].GetDeadFlag() == true) continue;

		for (int j = i + 1; j < MAX_HUMAN; j++) {
			if (HumanIndex[j].GetEnableFlag() == false) continue;
			if (HumanIndex[j].GetHP() <= 0) continue;
			if (HumanIndex[j].GetDeadFlag() == true) continue;

			CollideHuman(&(HumanIndex[i]), &(HumanIndex[j]));
		}
	}

	//깓긐듫똚궻룉뿚
	ObjectLog->Process();

	if ((framecnt % 60) == 0) {
		int humanCnt = 0;
		int weaponCnt = 0;
		int smallCnt = 0;
		int bulletCnt = 0;
		int grenadeCnt = 0;
		int effectCnt = 0;

		GetTotalObjects(
			&humanCnt,
			&weaponCnt,
			&smallCnt,
			&bulletCnt,
			&grenadeCnt,
			&effectCnt
		);

		char buf[128];
		sprintf(
			buf,
			"OBJ H:%d W:%d S:%d B:%d G:%d E:%d",
			humanCnt,
			weaponCnt,
			smallCnt,
			bulletCnt,
			grenadeCnt,
			effectCnt
		);

		ObjectLog->InfoLog(buf);
	}

	framecnt += 1;


	return 0;
}

//! @brief 뙸긲깒??궻 뼺뭷뿦갋?궢궫밎궻릶갋긶긞긣긘깈긞긣릶 궻롦벦
//! @param id 롦벦궥귡릐궻긢??붥뜂
//! @param ontarget 뼺뭷릶귩롷궚롦귡?귽깛?
//! @param kill ?궢궫밎궻릶귩롷궚롦귡?귽깛?
//! @param headshot 밎궻벆븫궸뼺뭷궢궫릶귩롷궚롦귡?귽깛?
//! @return 맟뚻갌true?렪봲갌false
bool ObjectManager::GetHumanShotInfo(int id, float *ontarget, int *kill, int *headshot)
{
	if( (id < 0)||(MAX_HUMAN <= id) ){ return false; }
	if( (ontarget == NULL)||(kill == NULL)||(headshot == NULL) ){ return false; }

	*ontarget = Human_ontarget[id];
	*kill = Human_kill[id];
	*headshot = Human_headshot[id];
	return true;
}

//! @brief 긄긲긃긏긣귩??긣궥귡
//! @param camera_x 긇긽깋궻X띆뷭
//! @param camera_y 긇긽깋궻Y띆뷭
//! @param camera_z 긇긽깋궻Z띆뷭
//! @param data 뙅됈귩롷궚롦귡 effectdata?몾뫬?걁뾴멹릶갌MAX_EFFECT걂
//! @return 뾎뚼궶긄긲긃긏긣궻릶
int ObjectManager::SortEffect(float camera_x, float camera_y, float camera_z, effectdata data[])
{
	int cnt = 0;

	for (int i = 0; i < MAX_EFFECT; i++) {
		// [핵심] 활성화된 이펙트만 담아야 유령 이펙트를 정렬하지 않습니다.
		if (EffectIndex[i].GetEnableFlag() == false) { continue; }

		float ex, ey, ez;
		EffectIndex[i].GetPosData(&ex, &ey, &ez, NULL);
		float dx = ex - camera_x;
		float dy = ey - camera_y;
		float dz = ez - camera_z;

		// [최적화] 거리 컬링: 너무 멀리 있는 이펙트는 연산에서 아예 제외
		float distSq = dx * dx + dy * dy + dz * dz;
		if (distSq > 40000.0f) continue;

		data[cnt].id = i;
		data[cnt].dist = distSq;
		cnt += 1;
	}

	// [최적화] C++ 표준 정렬 알고리즘 (삽입/버블 정렬보다 압도적으로 빠름)
	std::sort(data, data + cnt, [](const effectdata& a, const effectdata& b) {
		return a.dist > b.dist; // 먼 것부터 렌더링 (내림차순)
		});

	return cnt;
}




//! @brief 긆긳긙긃긏긣궻?됪룉뿚
//! @param camera_x 긇긽깋궻X띆뷭
//! @param camera_y 긇긽깋궻Y띆뷭
//! @param camera_z 긇긽깋궻Z띆뷭
//! @param camera_rx 긇긽깋궻돘렡둷뱗
//! @param camera_ry 긇긽깋궻뢢렡둷뱗
//! @param HidePlayer 긵깒귽깂?궻?됪먠믦?걁?됪궥귡갌0??됪궢궶궋갌1?쁱궴븧딇궻귒?됪궥귡갌2걂
//! @param NoModel 긾긢깑?됪뼰뚼돸
void ObjectManager::Render(float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, int HidePlayer, bool NoModel)
{
	//깗?깑긤띆뷭귩뙱?귉
	d3dg->ResetWorldTransform();

	//릐?됪
	for (int i = 0; i < MAX_HUMAN; i++) {
		// 활성화 되지 않은 인물은 skip 하는 코드 추가
		if (HumanIndex[i].GetEnableFlag() == false) { continue; };


		// 1. 텍스처 갱신 처리 (위치 아주 좋습니다!)
		if (Human_FrameTextureRefresh[i] == true) {
			int checkHumanID;
			HumanIndex[i].GetParamData(&checkHumanID, NULL, NULL, NULL);

			if (checkHumanID == ID_HUMAN_WHITE_DUMMY) {
				Human_FrameTextureRefresh[i] = false;
			}
			else if (i > 0) {
				int WeaponType = HumanIndex[i - 1].GetMainWeaponTypeNO();

				if (WeaponType != ID_WEAPON_NONE) {
					int model, texture;
					Resource->GetWeaponModelTexture(WeaponType, &model, &texture);
					HumanIndex[i].SetTexture(texture);
				}
				else {
					HumanIndex[i].SetTexture(HumanIndex[i - 1].GetTexture());
				}

				// 중요
				Human_FrameTextureRefresh[i] = false;
			}
			else {
				Human_FrameTextureRefresh[i] = false;
			}
		}

		// 2. 거리 컬링 (스코프 모드 예외 처리 포함 - 이것 하나만 남깁니다)
		bool isScoped = (HumanIndex[Player_HumanID].GetScopeMode() > 0);
		if (i != Player_HumanID && !isScoped) {
			float px, py, pz;
			HumanIndex[i].GetPosData(&px, &py, &pz, NULL);
			if ((px - camera_x) * (px - camera_x) + (py - camera_y) * (py - camera_y) + (pz - camera_z) * (pz - camera_z) > 4000000.0f) {
				continue;
			}
		}

		bool DrawArm, player;



		//쁱궻?됪
		if( HidePlayer == 0 ){
			DrawArm = false;
		}
		else if( Player_HumanID != i ){	// HidePlayer != 0
			DrawArm = false;
		}
		else if( HidePlayer == 2 ){		// Player_HumanID == i
			DrawArm = true;
		}
		else{							// Player_HumanID == i && HidePlayer == 1
			continue;	//긵깒귽깂?렔릆귩?됪궢궶궋궶귞긚긌긞긵
		}

		//긵깒귽깂?궔궵궎궔뵽믦
		if( Player_HumanID == i ){
			player = true;
		}
		else{
			player = false;
		}

		if (HumanIndex[i].GetSkillStealthFlag() == true) {
			continue;
		}

		HumanIndex[i].Render(d3dg, Resource, DrawArm, player, NoModel);

		/*
		//뱰궫귟뵽믦궻듗댲?렑
		float x, y, z;
		HumanIndex[i].GetPosData(&x, &y, &z, NULL);
		if( HumanIndex[i].GetHP() > 0 ){
			d3dg->SetWorldTransform(x, y, z, 0.0f, 0.0f, 1.0f);
			d3dg->Renderline(HUMAN_BULLETCOLLISION_HEAD_R, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H + HUMAN_BULLETCOLLISION_HEAD_H, 0.0f, HUMAN_BULLETCOLLISION_HEAD_R * -1, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H + HUMAN_BULLETCOLLISION_HEAD_H, 0.0f);
			d3dg->Renderline(0.0f, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H + HUMAN_BULLETCOLLISION_HEAD_H, HUMAN_BULLETCOLLISION_HEAD_R, 0.0f, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H + HUMAN_BULLETCOLLISION_HEAD_H, HUMAN_BULLETCOLLISION_HEAD_R * -1);
			d3dg->Renderline(HUMAN_BULLETCOLLISION_UP_R, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H, 0.0f, HUMAN_BULLETCOLLISION_UP_R * -1, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H, 0.0f);
			d3dg->Renderline(0.0f, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H, HUMAN_BULLETCOLLISION_UP_R, 0.0f, HUMAN_BULLETCOLLISION_LEG_H + HUMAN_BULLETCOLLISION_UP_H, HUMAN_BULLETCOLLISION_UP_R * -1);
			d3dg->Renderline(HUMAN_BULLETCOLLISION_LEG_R, HUMAN_BULLETCOLLISION_LEG_H, 0.0f, HUMAN_BULLETCOLLISION_LEG_R * -1, HUMAN_BULLETCOLLISION_LEG_H, 0.0f);
			d3dg->Renderline(0.0f, HUMAN_BULLETCOLLISION_LEG_H, HUMAN_BULLETCOLLISION_LEG_R, 0.0f, HUMAN_BULLETCOLLISION_LEG_H, HUMAN_BULLETCOLLISION_LEG_R * -1);
		}
		*/
	}

	// 디버그 히트박스 표시
	// 은신 중인 인물은 일반 인물 렌더 후에 다시 그린다.
// 투명 모델이 먼저 그려지면 뒤쪽의 일반 모델이 나중에 덮어써서
// 플레이어 모델이 상대 모델에 먹히는 것처럼 보일 수 있다.
	for (int i = 0; i < MAX_HUMAN; i++) {
		if (HumanIndex[i].GetEnableFlag() == false) { continue; }
		if (HumanIndex[i].GetSkillStealthFlag() == false) { continue; }

		bool isScoped = (HumanIndex[Player_HumanID].GetScopeMode() > 0);
		if (i != Player_HumanID && !isScoped) {
			float px, py, pz;
			HumanIndex[i].GetPosData(&px, &py, &pz, NULL);

			if ((px - camera_x) * (px - camera_x) +
				(py - camera_y) * (py - camera_y) +
				(pz - camera_z) * (pz - camera_z) > 4000000.0f) {
				continue;
			}
		}

		bool DrawArm;
		bool player;

		if (HidePlayer == 0) {
			DrawArm = false;
		}
		else if (Player_HumanID != i) {
			DrawArm = false;
		}
		else if (HidePlayer == 2) {
			DrawArm = true;
		}
		else {
			continue;
		}

		if (Player_HumanID == i) {
			player = true;
		}
		else {
			player = false;
		}

		HumanIndex[i].Render(d3dg, Resource, DrawArm, player, NoModel);
	}

	// 디버그 히트박스 표시
	RenderDebugHumanHitbox();

	for (int i = 0; i < MAX_WEAPON; i++) {
		WeaponIndex[i].Render(d3dg, NoModel);
	}
	for (int i = 0; i < MAX_SMALLOBJECT; i++) {
		SmallObjectIndex[i].Render(d3dg, NoModel);
	}
	for (int i = 0; i < MAX_BULLET; i++) {
		BulletIndex[i].Render(d3dg, NoModel);
	}
	for (int i = 0; i < MAX_GRENADE; i++) {
		GrenadeIndex[i].Render(d3dg, NoModel);
	}

	//긄긲긃긏긣?됪
	d3dg->StartEffectRender();
	/*
	for(int i=0; i<MAX_EFFECT; i++){
		EffectIndex[i].Render(d3dg, camera_rx, camera_ry, NoModel);
	}
	*/
	effectdata data[MAX_EFFECT];
	int cnt = SortEffect(camera_x, camera_y, camera_z, data);
	for(int i=0; i<cnt; i++) {
		EffectIndex[ data[i].id ].Render(d3dg, camera_rx, camera_ry, NoModel);
	}
	d3dg->EndEffectRender();
}

//! @brief 긆긳긙긃긏긣깓긐궻?됪룉뿚
//! @param x 뜺뤵궻 x띆뷭
//! @param y 뜺뤵궻 y띆뷭
void ObjectManager::RenderLog(int x, int y)
{
	ObjectLog->Render(x, y);
}

void ObjectManager::SetShowHitboxFlag(bool flag)
{
	ShowHitboxFlag = flag;
}

bool ObjectManager::GetShowHitboxFlag()
{
	return ShowHitboxFlag;
}

void ObjectManager::RenderDebugShieldBox(float x, float y, float z, float rx, int color, int front_color)
{
	float shield_forward_x = cosf(rx * -1.0f + (float)M_PI / 2.0f);
	float shield_forward_z = sinf(rx * -1.0f + (float)M_PI / 2.0f);
	float shield_right_x = shield_forward_z;
	float shield_right_z = -shield_forward_x;

	float side_value[2] = {
		-SHIELD_APPROX_HALF_WIDTH,
		SHIELD_APPROX_HALF_WIDTH
	};

	float y_value[2] = {
		SHIELD_APPROX_BOTTOM_Y,
		SHIELD_APPROX_TOP_Y
	};

	float front_value[2] = {
		SHIELD_APPROX_FRONT_MIN,
		SHIELD_APPROX_FRONT_MAX
	};

	float px[8];
	float py[8];
	float pz[8];

	int index = 0;

	for (int yi = 0; yi < 2; yi++) {
		for (int fi = 0; fi < 2; fi++) {
			for (int si = 0; si < 2; si++) {
				float side = side_value[si];
				float front = front_value[fi];

				px[index] = x + shield_right_x * side + shield_forward_x * front;
				py[index] = y + y_value[yi];
				pz[index] = z + shield_right_z * side + shield_forward_z * front;

				index++;
			}
		}
	}

	d3dg->ResetWorldTransform();

	// 아래면
	d3dg->Renderline(px[0], py[0], pz[0], px[1], py[1], pz[1], color);
	d3dg->Renderline(px[1], py[1], pz[1], px[3], py[3], pz[3], color);
	d3dg->Renderline(px[3], py[3], pz[3], px[2], py[2], pz[2], color);
	d3dg->Renderline(px[2], py[2], pz[2], px[0], py[0], pz[0], color);

	// 윗면
	d3dg->Renderline(px[4], py[4], pz[4], px[5], py[5], pz[5], color);
	d3dg->Renderline(px[5], py[5], pz[5], px[7], py[7], pz[7], color);
	d3dg->Renderline(px[7], py[7], pz[7], px[6], py[6], pz[6], color);
	d3dg->Renderline(px[6], py[6], pz[6], px[4], py[4], pz[4], color);

	// 세로선
	d3dg->Renderline(px[0], py[0], pz[0], px[4], py[4], pz[4], color);
	d3dg->Renderline(px[1], py[1], pz[1], px[5], py[5], pz[5], color);
	d3dg->Renderline(px[2], py[2], pz[2], px[6], py[6], pz[6], color);
	d3dg->Renderline(px[3], py[3], pz[3], px[7], py[7], pz[7], color);

	// 방패 박스의 실제 전방 면을 강조 표시한다.
	// front_max 면: px[2], px[3], px[6], px[7]
	// 이 면이 총알을 막는 방패 앞쪽이라고 보면 된다.
	d3dg->Renderline(px[2], py[2], pz[2], px[3], py[3], pz[3], front_color);
	d3dg->Renderline(px[3], py[3], pz[3], px[7], py[7], pz[7], front_color);
	d3dg->Renderline(px[7], py[7], pz[7], px[6], py[6], pz[6], front_color);
	d3dg->Renderline(px[6], py[6], pz[6], px[2], py[2], pz[2], front_color);

	// 앞면 방향을 더 쉽게 보기 위한 X 표시.
	d3dg->Renderline(px[2], py[2], pz[2], px[7], py[7], pz[7], front_color);
	d3dg->Renderline(px[3], py[3], pz[3], px[6], py[6], pz[6], front_color);

	d3dg->ResetWorldTransform();
}

void ObjectManager::RenderDebugCylinder(float x, float y, float z, float r, float h, int color)
{
	const int div = 12;
	const float pi = 3.14159265f;

	// Renderline은 현재 월드 변환 기준의 로컬 좌표로 그리는 방식이므로
	// 먼저 원통의 기준 위치로 월드 변환을 잡는다.
	d3dg->SetWorldTransform(x, y, z, 0.0f, 0.0f, 1.0f);

	for (int i = 0; i < div; i++) {
		float a1 = (2.0f * pi * i) / div;
		float a2 = (2.0f * pi * (i + 1)) / div;

		float x1 = cosf(a1) * r;
		float z1 = sinf(a1) * r;
		float x2 = cosf(a2) * r;
		float z2 = sinf(a2) * r;

		// 아래 원
		d3dg->Renderline(x1, 0.0f, z1, x2, 0.0f, z2, color);

		// 위 원
		d3dg->Renderline(x1, h, z1, x2, h, z2, color);

		// 세로선
		d3dg->Renderline(x1, 0.0f, z1, x1, h, z1, color);
	}

	d3dg->ResetWorldTransform();
}

void ObjectManager::RenderDebugHumanHitbox()
{
	if (ShowHitboxFlag == false) { return; }

	int color_leg = d3dg->GetColorCode(0.2f, 0.6f, 1.0f, 1.0f);
	int color_up = d3dg->GetColorCode(0.2f, 1.0f, 0.2f, 1.0f);
	int color_head = d3dg->GetColorCode(1.0f, 0.2f, 0.2f, 1.0f);
	int color_shield = d3dg->GetColorCode(1.0f, 0.9f, 0.1f, 1.0f);
	int color_shield_front = d3dg->GetColorCode(1.0f, 0.45f, 0.0f, 1.0f);

	for (int i = 0; i < MAX_HUMAN; i++) {
		if (HumanIndex[i].GetEnableFlag() == false) { continue; }
		if (HumanIndex[i].GetDeadFlag() == true) { continue; }

		float ox, oy, oz, orx;
		HumanIndex[i].GetPosData(&ox, &oy, &oz, &orx);

		float hit_leg_y, hit_leg_h;
		float hit_up_y, hit_up_h;
		float hit_head_y, hit_head_h;

		GetHumanBulletHitboxParts(
			&HumanIndex[i],
			oy,
			&hit_leg_y,
			&hit_leg_h,
			&hit_up_y,
			&hit_up_h,
			&hit_head_y,
			&hit_head_h
		);

		// 다리
		RenderDebugCylinder(
			ox,
			hit_leg_y,
			oz,
			HUMAN_BULLETCOLLISION_LEG_R,
			hit_leg_h,
			color_leg
		);

		// 상체
		RenderDebugCylinder(
			ox,
			hit_up_y,
			oz,
			HUMAN_BULLETCOLLISION_UP_R,
			hit_up_h,
			color_up
		);

		// 머리
		RenderDebugCylinder(
			ox,
			hit_head_y,
			oz,
			HUMAN_BULLETCOLLISION_HEAD_R,
			hit_head_h,
			color_head
		);

		// 방패 박스
		// 기존 히트박스 표시가 켜져 있고, 해당 사람이 방패를 들고 있을 때만 표시한다.
		// 노란색: 전체 방패 판정 박스
		// 주황색 X 표시: 총알을 막는 방패 앞면
		if (IsHumanShieldEquippedForBullet(&HumanIndex[i]) == true) {
			RenderDebugShieldBox(
				ox,
				oy,
				oz,
				orx,
				color_shield,
				color_shield_front
			);
		}
	}
}

//! @brief 긢??궻됶뺳
void ObjectManager::Cleanup()
{
	int i;

	// 1. 소형 오브젝트 비활성화 (기존 코드 유지)
	for (i = 0; i < MAX_SMALLOBJECT; i++) {
		SmallObjectIndex[i].SetEnableFlag(false);
		// ★ 만약 SmallObject에 메모리나 텍스처를 해제하는 함수가 있다면 여기서 호출
	}

	// 기존의 포인트 데이터/이펙트 해제 로직 외에 이 부분을 확실히 하세요.
	for (int i = 0; i < MAX_EFFECT; i++) {
		// 단순히 플래그만 끄는 게 아니라, 객체 데이터 자체를 초기화
		EffectIndex[i].SetEnableFlag(false);
		EffectIndex[i].SetParamData(0, 0, 0, 0, 0, 0, 0, 0, 0, 0); // 모든 수치 강제 0 초기화
	}

	// 3. 총알(Bullet) 및 기타 오브젝트 초기화
	for (i = 0; i < MAX_BULLET; i++) {
		BulletIndex[i].SetEnableFlag(false);
	}

	// 휴먼, 무기 등 모든 인덱스를 명시적으로 초기화하는지 재확인
	for (int i = 0; i < MAX_HUMAN; i++) { HumanIndex[i].SetEnableFlag(false); }

	//?귽깛긣긢??됶뺳
	CleanupPointDataToObject();
}

//! @brief 긓깛긚긣깋긏?
BulletObjectHumanIndex::BulletObjectHumanIndex()
{
	HumanIndex = new bool[MAX_HUMAN];

	Init();
}

//! @brief 긢귻긚긣깋긏?
BulletObjectHumanIndex::~BulletObjectHumanIndex()
{
	if( HumanIndex != NULL ){ delete [] HumanIndex; }
}

//! @brief 룊딖돸
void BulletObjectHumanIndex::Init()
{
	for(int i=0; i<MAX_HUMAN; i++){
		HumanIndex[i] = false;
	}
}

//! @brief 긲깋긐귩롦벦
//! @param id 릐궻긢??붥뜂
//! @return 긲깋긐뭠
bool BulletObjectHumanIndex::GetIndexFlag(int id)
{
	if( (id < 0)||(MAX_HUMAN <= id) ){ return false; }
	return HumanIndex[id];
}

//! @brief 긲깋긐귩뾎뚼돸
//! @param id 릐궻긢??붥뜂
void BulletObjectHumanIndex::SetIndexFlag(int id)
{
	if( (id < 0)||(MAX_HUMAN <= id) ){ return; }
	HumanIndex[id] = true;
}

//! @brief 긓깛긚긣깋긏?
ObjectManagerLog::ObjectManagerLog()
{
	d3dg = NULL;
	for(int i=0; i<MAX_OBJECTMANAGER_LOGLEN; i++){
		TextStr[i] = new char [MAX_OBJECTMANAGER_LOGLINES];
	}
	ClearLog();
}

//! @brief 긢귻긚긣깋긏?
ObjectManagerLog::~ObjectManagerLog()
{
	for(int i=0; i<MAX_OBJECTMANAGER_LOGLEN; i++){
		delete [] TextStr[i];
	}
}

//! @brief 랷뤖궥귡긏깋긚귩먠믦
//! @param in_d3dg ?됪룉뿚긏깋긚
//! @attention 궞궻듫릶궳먠믦귩뛱귦궶궋궴갂긏깋긚렔뫬궕맫궢궘??궢귏궧귪갃
void ObjectManagerLog::SetClass(D3DGraphics *in_d3dg)
{
	d3dg = in_d3dg;
}

//! @brief 긆긳긙긃긏긣깓긐귩룊딖돸
void ObjectManagerLog::ClearLog()
{
	for(int i=0; i<MAX_OBJECTMANAGER_LOGLEN; i++){
		TextCnt[i] = -1;
		TextStr[i][0] = '\0';
		TextColor[i] = 0;
	}
}

//! @brief 륃뺪귩믁딯
//! @param str 빒럻쀱?걁둂뛱긓?긤갌<b>븉됀</b>걂
void ObjectManagerLog::InfoLog(const char *str)
{
	if( str == NULL ){ return; }
	AddTextLog(MAX_OBJECTMANAGER_LOGCNT, str, d3dg->GetColorCode(0.0f,1.0f,0.0f,1.0f));
}

//! @brief 릐귩믁돿궢궫
//! @param humanID 릐궻ID
//! @param TeamID ???붥뜂
//! @param PlayerTeamID 긵깒귽깂?궻???붥뜂
void ObjectManagerLog::AddHuman(int humanID, int TeamID, int PlayerTeamID)
{
	char str[64];
	int str_color;

	//빒럻궻륡귩뙂믦
	if( PlayerTeamID == TeamID ){ str_color = d3dg->GetColorCode(0.0f,1.0f,1.0f,1.0f); }
	else{ str_color = d3dg->GetColorCode(1.0f,0.5f,0.0f,1.0f); }

	sprintf(str, "Added human[%d]", humanID);

	AddTextLog(MAX_OBJECTMANAGER_LOGCNT, str, str_color);
}

//! @brief 릐궕?뻊궢궫
//! @param ShothumanID  뛘똼귩궢궫릐궻ID
//! @param HitHumanID 뛘똼귩롷궚궫릐궻ID
//! @param ShothumanTeamID 뛘똼귩궢궫릐궻???붥뜂
//! @param HitHumanTeamID 뛘똼귩롷궚궫릐궻???붥뜂
//! @param PlayerTeamID 긵깒귽깂?궻???붥뜂
void ObjectManagerLog::DiedHuman(int ShothumanID, int HitHumanID, int ShothumanTeamID, int HitHumanTeamID, int PlayerTeamID)
{
	char str[64];
	int str_color;

	// [복구됨] 로그의 색상을 결정하는 부분 (실수로 지워졌던 부분)
	if (PlayerTeamID == HitHumanTeamID) { str_color = d3dg->GetColorCode(1.0f, 0.5f, 0.0f, 1.0f); }
	else { str_color = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f); }

	// ▼▼▼ [안정성] 버퍼 오버플로우를 막는 snprintf와 안전한 문자열 병합 ▼▼▼
	if ((ShothumanID == -1) || (ShothumanID == HitHumanID)) {
		snprintf(str, sizeof(str), "human[%d] died", HitHumanID);
	}
	else {
		snprintf(str, sizeof(str), "human[%d] killed human[%d]", ShothumanID, HitHumanID);

		// TK 판정
		if (ShothumanTeamID == HitHumanTeamID) {
			// 남은 버퍼 크기만큼만 복사하도록 안전하게 이어붙임
			strncat(str, " (TK)", sizeof(str) - strlen(str) - 1);
		}
	}
	// ▲▲▲ 여기까지 ▲▲▲

	AddTextLog(MAX_OBJECTMANAGER_LOGCNT, str, str_color);
}

//! @brief 릐궕멻맯궢궫
//! @param humanID 릐궻ID
//! @param TeamID ???붥뜂
//! @param PlayerTeamID 긵깒귽깂?궻???붥뜂
void ObjectManagerLog::ReviveHuman(int humanID, int TeamID, int PlayerTeamID)
{
	char str[64];
	int str_color;

	//빒럻궻륡귩뙂믦
	if( PlayerTeamID == TeamID ){ str_color = d3dg->GetColorCode(0.0f,1.0f,1.0f,1.0f); }
	else{ str_color = d3dg->GetColorCode(1.0f,0.5f,0.0f,1.0f); }

	sprintf(str, "Revived human[%d]", humanID);

	AddTextLog(MAX_OBJECTMANAGER_LOGCNT, str, str_color);
}

//! @brief 룷븿귩믁돿궢궫
//! @param objID 룷븿궻ID
void ObjectManagerLog::AddSmallObject(int objID)
{
	char str[64];
	sprintf(str, "Added SmallObject[%d]", objID);
	AddTextLog(MAX_OBJECTMANAGER_LOGCNT, str, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));
}

//! @brief 룷븿귩봨됹궢궫
//! @param objID 룷븿궻ID
void ObjectManagerLog::BreakSmallObject(int objID)
{
	char str[64];
	sprintf(str, "Breaked SmallObject[%d]", objID);
	AddTextLog(MAX_OBJECTMANAGER_LOGCNT, str, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));
}

//! @brief 긆긳긙긃긏긣깓긐믁돿
//! @param cnt ?됪긲깒??릶
//! @param str 빒럻쀱?걁둂뛱긓?긤갌<b>븉됀</b>걂
//! @param color 륡
//! @return 1뛱뤵룕궖갌true?믁돿궻귒갌false
bool ObjectManagerLog::AddTextLog(int cnt, const char *str, int color)
{
	if( str == NULL ){ return false; }

	// ▼▼▼ [안정성/최적화] strcpy를 strncpy로, 통째 복사를 포인터 스왑(O(1))으로 교체 ▼▼▼
	for (int i = 0; i < MAX_OBJECTMANAGER_LOGLEN; i++) {
		if (TextCnt[i] == -1) {
			TextCnt[i] = cnt;
			strncpy(TextStr[i], str, MAX_OBJECTMANAGER_LOGLINES - 1);
			TextStr[i][MAX_OBJECTMANAGER_LOGLINES - 1] = '\0'; // 끝에 NULL 보장
			TextColor[i] = color;
			return false;
		}
	}

	for (int i = 0; i < MAX_OBJECTMANAGER_LOGLEN - 1; i++) {
		TextCnt[i] = TextCnt[i + 1];
		// 무거운 strcpy 대신 포인터 주소만 맞바꿈
		char* temp = TextStr[i]; TextStr[i] = TextStr[i + 1]; TextStr[i + 1] = temp;
		TextColor[i] = TextColor[i + 1];
	}

	TextCnt[MAX_OBJECTMANAGER_LOGLEN - 1] = cnt;
	strncpy(TextStr[MAX_OBJECTMANAGER_LOGLEN - 1], str, MAX_OBJECTMANAGER_LOGLINES - 1);
	TextStr[MAX_OBJECTMANAGER_LOGLEN - 1][MAX_OBJECTMANAGER_LOGLINES - 1] = '\0';
	TextColor[MAX_OBJECTMANAGER_LOGLEN - 1] = color;
	return true;
	// ▲▲▲ 여기까지 ▲▲▲
}

//! @brief 긆긳긙긃긏긣깓긐궻롥똶럁룉뿚
void ObjectManagerLog::Process()
{
	for (int i = 0; i < MAX_OBJECTMANAGER_LOGLEN; i++) {
		if (TextCnt[i] > 0) {
			TextCnt[i] -= 1;
		}
		if (TextCnt[i] == 0) {
			TextCnt[i] = -1;
		}
	}

	for (int i = 0; i < MAX_OBJECTMANAGER_LOGLEN - 1; i++) {
		if (TextCnt[i] == -1) {
			TextCnt[i] = TextCnt[i + 1];

			char* p = TextStr[i];
			TextStr[i] = TextStr[i + 1];
			TextStr[i + 1] = p;

			TextColor[i] = TextColor[i + 1];

			TextCnt[i + 1] = -1;
			TextStr[i + 1][0] = '\0';
			TextColor[i + 1] = 0;
		}
	}
}

//! @brief 긆긳긙긃긏긣깓긐궻?됪룉뿚
//! @param x 뜺뤵궻 x띆뷭
//! @param y 뜺뤵궻 y띆뷭
//! @attention 뤵궔귞땤룈궳?됪궠귢귏궥갃
void ObjectManagerLog::Render(int x, int y)
{
#ifdef ENABLE_DEBUGCONSOLE
	if( d3dg == NULL ){ return; }

	for(int i=MAX_OBJECTMANAGER_LOGLEN-1; i>=0; i--){
		if( TextCnt[i] >= 0 ){
			d3dg->Draw2DTextureDebugFontText(x+2+1, y+2+1, TextStr[i], d3dg->GetColorCode(0.1f,0.1f,0.1f,1.0f));
			d3dg->Draw2DTextureDebugFontText(x+2, y+2, TextStr[i], TextColor[i]);
		}
		y += 20;
	}
#endif
}

// ============================================================================
// [추가됨] 커스텀 그룹 무기 ID 반환 헬퍼 함수 (내부 사용)
// ============================================================================
int GetRandomWeaponIDByCustomGroup(int originalID)
{
	// 1. MP5(1), KAC PDW(28), K1A(73)
	if (originalID == 1 || originalID == 28 || originalID == 73) {
		int list[] = { 1, 28, 73 }; return list[GetRand(3)];
	}
	// 2. PSG-1(2), SVD(39), WA2000(54)
	else if (originalID == 2 || originalID == 39 || originalID == 54) {
		int list[] = { 2, 39, 54 }; return list[GetRand(3)];
	}
	// 3. M92F(3), Winchester1873(36)
	else if (originalID == 3 || originalID == 36) {
		int list[] = { 3, 36 }; return list[GetRand(2)];
	}
	// 4. GLOCK18(4), Beretta 93R(52), Vz.61 Skorpion(69)
	else if (originalID == 4 || originalID == 52 || originalID == 69) {
		int list[] = { 4, 52, 69 }; return list[GetRand(3)];
	}
	// 5. Desert Eagle(5),	M29 Satan(53)
	else if (originalID == 5 || originalID == 53) {
		int list[] = { 5, 53 }; return list[GetRand(2)];
	}
	// 6. MAC10(6), KRISS Vector(47), M1928A1 Thompson(65)
	else if (originalID == 6 || originalID == 47 || originalID == 65) {
		int list[] = { 6, 47, 65 }; return list[GetRand(3)];
	}
	// 7. UMP(7), UMP SD(26), MP40(33)
	else if (originalID == 7 || originalID == 26 || originalID == 33) {
		int list[] = { 7, 26, 33 }; return list[GetRand(3)];
	}
	// 8. P90(8), CF-05(71)
	else if (originalID == 8 || originalID == 71) {
		int list[] = { 8, 71 }; return list[GetRand(2)];
	}
	// 9. M4(9), SIG 556(31), HK416(41)
	else if (originalID == 9 || originalID == 31 || originalID == 41) {
		int list[] = { 9, 31, 41 }; return list[GetRand(3)];
	}
	// 10. AK47(10), Galil(29), STG44(64)
	else if (originalID == 10 || originalID == 29 || originalID == 64) {
		int list[] = { 10, 29, 64 }; return list[GetRand(3)];
	}
	// 11. AUG(11), K2C(74), SCAR(37)
	else if (originalID == 11 || originalID == 74 || originalID == 37) {
		int list[] = { 11, 74, 37 }; return list[GetRand(3)];
	}
	// 12. M249(12), M60(30), MG42(67)
	else if (originalID == 12 || originalID == 30 || originalID == 67) {
		int list[] = { 12, 30, 67 }; return list[GetRand(3)];
	}
	// 13. MP5SD(14), MP9(51)
	else if (originalID == 14 || originalID == 51) {
		int list[] = { 14, 51 }; return list[GetRand(2)];
	}
	// 14. M1911(17), KAR98K(58)
	else if (originalID == 17 || originalID == 58) {
		int list[] = { 17, 58 }; return list[GetRand(2)];
	}
	// 15. GLOCK17(18), Calico M950(57)
	else if (originalID == 18 || originalID == 57) {
		int list[] = { 18, 57 }; return list[GetRand(2)];
	}
	// 16. M1(19), Double Barrel(50), SPAS-12 PUMP(68)
	else if (originalID == 19 || originalID == 50 || originalID == 68) {
		int list[] = { 19, 50, 68 }; return list[GetRand(3)];
	}
	// 17. FAMAS(20), M1918 BAR(56), TAR-21(46)
	else if (originalID == 20 || originalID == 56 || originalID == 46) {
		int list[] = { 20, 56, 46 }; return list[GetRand(3)];
	}
	// 15. MK23(21), M1 Garand(60)
	else if (originalID == 21 || originalID == 60) {
		int list[] = { 21, 60 }; return list[GetRand(2)];
	}

	return originalID; // 그룹에 없으면 원래 무기 유지
}

// ObjectManager 클래스 멤버 함수 구현

void ObjectManager::RandomizeMapWeaponsByCustomGroup(class ParameterInfo* Param, class ResourceManager* Resource)

{

	for (int i = 0; i < MAX_WEAPON; i++)

	{

		// 1. 활성화되어 있고, 바닥에 놓인 무기만 처리

		if (WeaponIndex[i].GetEnableFlag() == true && WeaponIndex[i].GetUsingFlag() == false)

		{

			int currentID = 0;

			int currentLoad = 0;

			int currentTotal = 0;



			// OpenXOPS의 GetParamData는 3번째 인자로 '전체 탄약 수'를 반환합니다.

			WeaponIndex[i].GetParamData(&currentID, &currentLoad, &currentTotal);



			int newWeaponID = GetRandomWeaponIDByCustomGroup(currentID);



			// 무기가 바뀔 때만 로직 수행

			if (newWeaponID != currentID)

			{

				WeaponParameter oldParam;

				WeaponParameter newParam;



				// 무기 데이터 로드 성공 시

				if (Param->GetWeapon(currentID, &oldParam) == 0 && Param->GetWeapon(newWeaponID, &newParam) == 0)

				{

					int totalAmmo = currentTotal;



					// 2. 새 무기의 탄창 용량 확인

					int maxCapacity = newParam.nbsmax;



					int newLoadAmmo = 0;



					// 3. 장전량 결정 (가진 탄약과 탄창 용량 중 작은 값)

					if (totalAmmo < maxCapacity)

					{

						newLoadAmmo = totalAmmo; // 탄알이 부족하면 있는 거 다 장전

					}

					else

					{

						newLoadAmmo = maxCapacity; // 충분하면 탄창 꽉 채움

					}



					// ▼▼▼ [수정됨] 모델 및 텍스처 명시적 적용 (하얗게 나오는 문제 해결) ▼▼▼

					int model, texture;



					// 리소스 매니저에서 새 무기의 모델과 텍스처 ID를 가져옵니다.

					if (Resource->GetWeaponModelTexture(newWeaponID, &model, &texture) == 0)

					{

						// 무기 파라미터 리셋

						WeaponIndex[i].ResetWeaponParam(Resource, newWeaponID, 0, 0);



						// [핵심] 모델과 텍스처를 강제로 다시 설정합니다.

						WeaponIndex[i].SetModel(model, newParam.size);

						WeaponIndex[i].SetTexture(texture);

					}

					else

					{

						// 텍스처 로드 실패 시에도 파라미터는 리셋 (안전장치)

						WeaponIndex[i].ResetWeaponParam(Resource, newWeaponID, 0, 0);

					}

					// ▲▲▲ [수정됨] ▲▲▲



					// 정확한 탄약 수치 주입

					WeaponIndex[i].SetParamData(newWeaponID, newLoadAmmo, totalAmmo, true);

				}

				else

				{

					// 데이터 로드 실패 시 빈 총으로 초기화

					WeaponIndex[i].ResetWeaponParam(Resource, newWeaponID, 0, 0);

				}

			}

		}

	}

}


