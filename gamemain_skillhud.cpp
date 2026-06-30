//! @file gamemain_skillhud.cpp
//! @brief 플레이어 스킬 HUD 표시 처리

#include "gamemain.h"
#include "playerskill.h"

// gamemain.cpp에 정의되어 있는 전역 리소스 관리자 사용
extern ResourceManager Resource;
extern ObjectManager ObjMgr;
extern Collision CollD;

int maingame::GetPlayerSkillHudThemeColor(int skilltype, float alpha)
{
	if (alpha < 0.0f) { alpha = 0.0f; }
	if (alpha > 1.0f) { alpha = 1.0f; }

	// 1. 회복/생존/소환 계열: 녹색
	if ((skilltype == PLAYER_SKILL_HEAL) ||
		(skilltype == PLAYER_SKILL_SURVIVAL) ||
		(skilltype == PLAYER_SKILL_SUMMON) ||
		(skilltype == PLAYER_SKILL_BIOTIC_FIELD)) {
		return d3dg->GetColorCode(0.35f, 0.95f, 0.45f, alpha);
	}

	// 2. 강화/제압 계열: 청색
	if ((skilltype == PLAYER_SKILL_AMMOBOOST) ||
		(skilltype == PLAYER_SKILL_SUPPRESS) ||
		(skilltype == PLAYER_SKILL_IRONBODY) ||
		(skilltype == PLAYER_SKILL_SHIELD)) {
		return d3dg->GetColorCode(0.25f, 0.70f, 1.0f, alpha);
	}

	// 3. 탐지/교란/투척 계열: 주황
	if ((skilltype == PLAYER_SKILL_TACTICAL_SCAN) ||
		(skilltype == PLAYER_SKILL_DISTRACTION) ||
		(skilltype == PLAYER_SKILL_IMPACT_GRENADE) ||
		(skilltype == PLAYER_SKILL_TERRORIST_MINE) ||
		(skilltype == PLAYER_SKILL_ARTILLERY_STRIKE)) {
		return d3dg->GetColorCode(1.0f, 0.65f, 0.10f, alpha);
	}

	// 4. 은신/이동/흡수 계열: 보라
	if ((skilltype == PLAYER_SKILL_STEALTH) ||
		(skilltype == PLAYER_SKILL_DODGE) ||
		(skilltype == PLAYER_SKILL_TELEPORT) ||
		(skilltype == PLAYER_SKILL_ABSORB) ||
		(skilltype == PLAYER_SKILL_SHADOW_DECOY)) {
		return d3dg->GetColorCode(0.55f, 0.25f, 1.0f, alpha);
	}

	// 5. 공격/좀비/처형/중화기 계열: 붉은색
	if ((skilltype == PLAYER_SKILL_MARKSHOT) ||
		(skilltype == PLAYER_SKILL_ROBOT_EXECUTE) ||
		(skilltype == PLAYER_SKILL_AIMHACK) ||
		(skilltype == PLAYER_SKILL_HOOK) ||
		(skilltype == PLAYER_SKILL_RAGE) ||
		(skilltype == PLAYER_SKILL_DP28)) {
		return d3dg->GetColorCode(1.0f, 0.22f, 0.10f, alpha);
	}

	// 기본값: 기존 보라색 계열
	return d3dg->GetColorCode(0.75f, 0.55f, 1.0f, alpha);
}

void maingame::RenderSkillTargetCylinder(float x, float y, float z, float radius, float height, int fill_color, int border_color)
{
	float y0 = y + 0.2f;
	float y1 = y + height;
	float k = 0.7071f;

	// 내부 채움
	int fill_steps_y = 5;
	int fill_steps_z = 10;

	for (int iy = 0; iy <= fill_steps_y; iy++) {
		float yy = y0 + ((float)iy / (float)fill_steps_y) * height;

		for (int iz = -fill_steps_z; iz <= fill_steps_z; iz++) {
			float nz = (float)iz / (float)fill_steps_z;
			float inside = 1.0f - nz * nz;

			if (inside < 0.0f) { continue; }

			float zz = z + nz * radius;
			float half_width = sqrtf(inside) * radius;

			d3dg->Renderline(
				x - half_width, yy, zz,
				x + half_width, yy, zz,
				fill_color
			);
		}
	}

	// 아래 링
	d3dg->Renderline(x + radius, y0, z, x + radius * k, y0, z + radius * k, border_color);
	d3dg->Renderline(x + radius * k, y0, z + radius * k, x, y0, z + radius, border_color);
	d3dg->Renderline(x, y0, z + radius, x - radius * k, y0, z + radius * k, border_color);
	d3dg->Renderline(x - radius * k, y0, z + radius * k, x - radius, y0, z, border_color);
	d3dg->Renderline(x - radius, y0, z, x - radius * k, y0, z - radius * k, border_color);
	d3dg->Renderline(x - radius * k, y0, z - radius * k, x, y0, z - radius, border_color);
	d3dg->Renderline(x, y0, z - radius, x + radius * k, y0, z - radius * k, border_color);
	d3dg->Renderline(x + radius * k, y0, z - radius * k, x + radius, y0, z, border_color);

	// 위 링
	d3dg->Renderline(x + radius, y1, z, x + radius * k, y1, z + radius * k, border_color);
	d3dg->Renderline(x + radius * k, y1, z + radius * k, x, y1, z + radius, border_color);
	d3dg->Renderline(x, y1, z + radius, x - radius * k, y1, z + radius * k, border_color);
	d3dg->Renderline(x - radius * k, y1, z + radius * k, x - radius, y1, z, border_color);
	d3dg->Renderline(x - radius, y1, z, x - radius * k, y1, z - radius * k, border_color);
	d3dg->Renderline(x - radius * k, y1, z - radius * k, x, y1, z - radius, border_color);
	d3dg->Renderline(x, y1, z - radius, x + radius * k, y1, z - radius * k, border_color);
	d3dg->Renderline(x + radius * k, y1, z - radius * k, x + radius, y1, z, border_color);

	// 세로선
	d3dg->Renderline(x + radius, y0, z, x + radius, y1, z, border_color);
	d3dg->Renderline(x - radius, y0, z, x - radius, y1, z, border_color);
	d3dg->Renderline(x, y0, z + radius, x, y1, z + radius, border_color);
	d3dg->Renderline(x, y0, z - radius, x, y1, z - radius, border_color);
}

void maingame::RenderPlayerSkillEnemyPing(float x, float y, float z, float radius, float height)
{
	int fill_color = d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 0.28f);
	int border_color = d3dg->GetColorCode(1.0f, 0.25f, 0.25f, 1.0f);
	int core_color = d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f);

	RenderSkillTargetCylinder(x, y, z, radius, height, fill_color, border_color);

	// ߾ : ġ ĺ
	d3dg->Renderline(x, y + 0.2f, z, x, y + height, z, core_color);
}



// 대장 전술 스캔: 적 위치에 공용 적 핑 표시
void maingame::RenderTacticalScanPings(human* myHuman)
{
	if (myHuman == NULL) { return; }
	if (player_skill_scan_timer <= 0) { return; }
	if (Camera_Debugmode == true) { return; }

	int myteam = -1;
	myHuman->GetParamData(NULL, NULL, NULL, &myteam);

	d3dg->ResetWorldTransform();

	// 벽 너머에서도 보이게 하기 위해, 오브젝트 렌더 후 깊이 버퍼를 비운다.
	// 이 함수는 ObjMgr.Render() 이후에 호출되어야 한다.
	d3dg->ResetZbuffer();

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

		// 몸통 아래쪽에 짧은 원기둥 핑 표시
		RenderPlayerSkillEnemyPing(
			x,
			y + 1.0f,
			z,
			3.0f,
			HUMAN_HEIGHT * 0.25f
		);
	}
}

// 요원A 마킹 샷: 마킹된 적 위치 표시
void maingame::RenderMarkedShotPing(human* myHuman)
{
	if (myHuman == NULL) { return; }
	if (player_skill_mark_timer <= 0) { return; }
	if (player_skill_mark_target_id < 0) { return; }
	if (Camera_Debugmode == true) { return; }

	human* target = ObjMgr.GetHumanObject(player_skill_mark_target_id);

	if (target == NULL) { return; }
	if (target->GetEnableFlag() == false) { return; }
	if (target->GetDeadFlag() == true) { return; }
	if (target->GetHP() <= 0) { return; }

	d3dg->ResetWorldTransform();

	// 벽 뒤에서도 보이게 한다.
	d3dg->ResetZbuffer();

	float x, y, z, rx;
	target->GetPosData(&x, &y, &z, &rx);

	RenderPlayerSkillEnemyPing(
		x,
		y + 1.0f,
		z,
		3.0f,
		HUMAN_HEIGHT * 0.25f
	);
}

void maingame::RenderShadowDecoyPings(human* myHuman)
{
	if (myHuman == NULL) { return; }
	if (Camera_Debugmode == true) { return; }

	d3dg->ResetWorldTransform();
	d3dg->ResetZbuffer();

	for (int i = 0; i < MAX_HUMAN; i++) {
		if (player_skill_shadow_decoy_ping_timer[i] <= 0) { continue; }

		human* target = ObjMgr.GetHumanObject(i);
		if (target == NULL) { continue; }
		if (target->GetEnableFlag() == false) { continue; }
		if (target->GetDeadFlag() == true) { continue; }
		if (target->GetHP() <= 0) { continue; }

		float x, y, z, rx;
		target->GetPosData(&x, &y, &z, &rx);

		// 그림자 잔상을 공격한 적에게 5초간 공용 핑 표시.
		RenderPlayerSkillEnemyPing(
			x,
			y + HUMAN_HEIGHT + 2.0f,
			z,
			1.8f,
			2.5f
		);
	}
}

// 경찰 제압 명령: 제압된 적 머리 위에 공용 적 핑 표시
void maingame::RenderSuppressedEnemyPings(human* myHuman)
{
	if (myHuman == NULL) { return; }
	if (Camera_Debugmode == true) { return; }

	d3dg->ResetWorldTransform();
	d3dg->ResetZbuffer();

	for (int i = 0; i < MAX_HUMAN; i++) {
		if (player_skill_suppress_mark_timer[i] <= 0) { continue; }

		human* target = ObjMgr.GetHumanObject(i);
		if (target == NULL) { continue; }
		if (target->GetEnableFlag() == false) { continue; }
		if (target->GetDeadFlag() == true) { continue; }
		if (target->GetHP() <= 0) { continue; }

		float x, y, z, rx;
		target->GetPosData(&x, &y, &z, &rx);

		// 제압 명령은 전술 스캔/마킹 샷과 같은 디자인을 사용하되,
		// 적 몸통이 아니라 머리 위에 작게 표시한다.
		RenderPlayerSkillEnemyPing(
			x,
			y + HUMAN_HEIGHT + 2.0f,
			z,
			1.8f,
			2.5f
		);
	}
}

void maingame::RenderDistractionSignalMarker()
{
	if (player_skill_distraction_timer <= 0) { return; }
	if (Camera_Debugmode == true) { return; }

	d3dg->ResetWorldTransform();

	// 설치형 장치는 실제 화면에서 벽/지형 너머로 보이지 않게 한다.
	// 따라서 ResetZbuffer()를 호출하지 않는다.
	int fill_color = d3dg->GetColorCode(1.0f, 0.75f, 0.0f, 0.28f);
	int border_color = d3dg->GetColorCode(1.0f, 0.95f, 0.1f, 1.0f);

	// 발동된 실제 교란 지점 표시
	RenderSkillTargetCylinder(
		player_skill_distraction_x,
		player_skill_distraction_y,
		player_skill_distraction_z,
		6.0f,
		10.0f,
		fill_color,
		border_color
	);

	// 중심 기둥을 추가해서 "여기가 실제 발동 지점"이라는 느낌을 강화
	int core_color = d3dg->GetColorCode(1.0f, 0.65f, 0.0f, 1.0f);
	d3dg->Renderline(
		player_skill_distraction_x,
		player_skill_distraction_y,
		player_skill_distraction_z,
		player_skill_distraction_x,
		player_skill_distraction_y + 18.0f,
		player_skill_distraction_z,
		core_color
	);
}

// 좀비 갈고리: 발사 전 조준선 표시
void maingame::RenderPlayerHookAimPreview(human* myHuman)
{
	if (myHuman == NULL) { return; }
	if (player_skill_targeting_type != PLAYER_SKILL_HOOK) { return; }
	if (Camera_Debugmode == true) { return; }

	float px, py, pz, prx;
	float rx, ry;

	myHuman->GetPosData(&px, &py, &pz, &prx);
	myHuman->GetRxRy(&rx, &ry);

	float aim_rx = rx * -1.0f + (float)M_PI / 2.0f;

	// 조준 미리보기 선도 실제 갈고리 시작점과 같은 위치에서 나오게 한다.
	float sx = px + cosf(aim_rx) * PLAYER_SKILL_HOOK_PLAYER_FORWARD_OFFSET;
	float sy = py + PLAYER_SKILL_HOOK_PLAYER_ATTACH_HEIGHT;
	float sz = pz + sinf(aim_rx) * PLAYER_SKILL_HOOK_PLAYER_FORWARD_OFFSET;

	if (myHuman->GetCrouchFlag() == true) {
		sy -= 2.0f;
	}

	float dir_x = cosf(aim_rx) * cosf(ry);
	float dir_y = sinf(ry);
	float dir_z = sinf(aim_rx) * cosf(ry);

	float ray_dist = PLAYER_SKILL_HOOK_MAX_DIST;
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

	if (hit_wall == true) {
		ray_dist = wall_dist;
	}

	float ex = sx + dir_x * ray_dist;
	float ey = sy + dir_y * ray_dist;
	float ez = sz + dir_z * ray_dist;

	int target_id = FindPlayerHookTargetByAimRay(myHuman);
	player_skill_target_valid = (target_id >= 0);

	int line_color;
	int point_color;

	if (target_id >= 0) {
		line_color = d3dg->GetColorCode(0.35f, 1.0f, 0.25f, 0.95f);
		point_color = d3dg->GetColorCode(0.35f, 1.0f, 0.25f, 1.0f);
	}
	else {
		line_color = d3dg->GetColorCode(1.0f, 0.15f, 0.05f, 0.75f);
		point_color = d3dg->GetColorCode(1.0f, 0.15f, 0.05f, 1.0f);
	}

	d3dg->ResetWorldTransform();
	d3dg->ResetZbuffer();

	// 조준선.
	// 월드 X축 offset을 제거해서 선이 옆에서 나오는 것처럼 보이는 문제를 줄인다.
	d3dg->Renderline(sx, sy, sz, ex, ey, ez, line_color);

	// 조준선 끝점 표시
	RenderSkillTargetCylinder(
		ex,
		ey - 2.0f,
		ez,
		1.8f,
		3.0f,
		d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.0f),
		point_color
	);

	// 맞출 수 있는 대상이 있으면 대상에게도 표시
	if ((target_id >= 0) && (target_id < MAX_HUMAN)) {
		human* target = ObjMgr.GetHumanObject(target_id);

		if ((target != NULL) &&
			(target->GetEnableFlag() == true) &&
			(target->GetDeadFlag() == false) &&
			(target->GetHP() > 0)) {

			float tx, ty, tz, trx;
			target->GetPosData(&tx, &ty, &tz, &trx);

			float hook_wrap_height = HUMAN_HEIGHT * PLAYER_SKILL_HOOK_WRAP_HEIGHT_RATE;
			float hook_wrap_base_y =
				ty + HUMAN_HEIGHT * PLAYER_SKILL_HOOK_WRAP_CENTER_RATE - hook_wrap_height * 0.5f;

			RenderSkillTargetCylinder(
				tx,
				hook_wrap_base_y,
				tz,
				2.8f,
				hook_wrap_height,
				d3dg->GetColorCode(0.35f, 1.0f, 0.25f, 0.22f),
				d3dg->GetColorCode(0.35f, 1.0f, 0.25f, 1.0f)
			);
		}
	}
}

// 좀비 갈고리 표시
void maingame::RenderPlayerHookSkill(human* myHuman)
{
	if (myHuman == NULL) { return; }
	if ((player_skill_hook_prep_timer <= 0) &&
		(player_skill_hook_pull_timer <= 0)) {
		return;
	}
	if (Camera_Debugmode == true) { return; }

	if ((player_skill_hook_target_id < 0) ||
		(player_skill_hook_target_id >= MAX_HUMAN)) {
		return;
	}

	human* target = ObjMgr.GetHumanObject(player_skill_hook_target_id);
	if (target == NULL) { return; }
	if (target->GetEnableFlag() == false) { return; }
	if (target->GetDeadFlag() == true) { return; }
	if (target->GetHP() <= 0) { return; }

	float px, py, pz, prx;
	float tx, ty, tz, trx;
	myHuman->GetPosData(&px, &py, &pz, &prx);
	target->GetPosData(&tx, &ty, &tz, &trx);

	float prx2, pry2;
	myHuman->GetRxRy(&prx2, &pry2);

	float aim_rx = prx2 * -1.0f + (float)M_PI / 2.0f;

	// 갈고리 시작점:
	// 카메라/눈 위치가 아니라 플레이어 몸통 앞쪽에서 나가게 한다.
	float sx = px + cosf(aim_rx) * PLAYER_SKILL_HOOK_PLAYER_FORWARD_OFFSET;
	float sy = py + PLAYER_SKILL_HOOK_PLAYER_ATTACH_HEIGHT;
	float sz = pz + sinf(aim_rx) * PLAYER_SKILL_HOOK_PLAYER_FORWARD_OFFSET;

	if (myHuman->GetCrouchFlag() == true) {
		sy -= 2.0f;
	}

	// 갈고리 끝점:
	// 다리/발밑이 아니라 대상 몸통 중심에 붙게 한다.
	float ex = tx;
	float ey = ty + HUMAN_HEIGHT * PLAYER_SKILL_HOOK_ATTACH_HEIGHT_RATE;
	float ez = tz;

	d3dg->ResetWorldTransform();
	d3dg->ResetZbuffer();

	int line_color;
	int fill_color;
	int border_color;

	if (player_skill_hook_prep_timer > 0) {
		line_color = d3dg->GetColorCode(1.0f, 0.85f, 0.15f, 0.95f);
		fill_color = d3dg->GetColorCode(1.0f, 0.75f, 0.10f, 0.22f);
		border_color = d3dg->GetColorCode(1.0f, 0.90f, 0.20f, 1.0f);
	}
	else {
		line_color = d3dg->GetColorCode(1.0f, 0.10f, 0.05f, 0.95f);
		fill_color = d3dg->GetColorCode(1.0f, 0.05f, 0.02f, 0.25f);
		border_color = d3dg->GetColorCode(1.0f, 0.20f, 0.10f, 1.0f);
	}

	// 갈고리 줄.
	// 두 번째 줄을 월드 X축으로만 밀면 시점에 따라 이상한 위치에서 나오는 것처럼 보일 수 있다.
	// 우선 한 줄만 사용해서 시작점/끝점이 정확히 보이게 한다.
	d3dg->Renderline(sx, sy, sz, ex, ey, ez, line_color);

	// 대상 표시
	float hook_wrap_height = HUMAN_HEIGHT * PLAYER_SKILL_HOOK_WRAP_HEIGHT_RATE;
	float hook_wrap_base_y =
		ty + HUMAN_HEIGHT * PLAYER_SKILL_HOOK_WRAP_CENTER_RATE - hook_wrap_height * 0.5f;

	RenderSkillTargetCylinder(
		tx,
		hook_wrap_base_y,
		tz,
		2.4f,
		hook_wrap_height,
		fill_color,
		border_color
	);
}



void maingame::RenderPlayerSkillTargetPreview(human* myHuman)
{
	if (myHuman == NULL) { return; }
	if (player_skill_targeting_type == PLAYER_SKILL_NONE) { return; }
	if (Camera_Debugmode == true) { return; }

	// 조준선 발사형 스킬은 원형 지점 미리보기를 쓰지 않는다.
	if (PlayerSkill_IsRayTargetingSkill(player_skill_targeting_type) == true) {
		RenderPlayerHookAimPreview(myHuman);
		return;
	}

	if (PlayerSkill_IsGroundTargetingSkill(player_skill_targeting_type) == false) {
		return;
	}

	float max_dist = PlayerSkill_GetTargetingMaxDist(player_skill_targeting_type);

	if (max_dist <= 0.0f) {
		return;
	}

	float tx = player_skill_target_x;
	float ty = player_skill_target_y;
	float tz = player_skill_target_z;

	bool aim_found = UpdatePlayerSkillAimTarget(myHuman, max_dist, true, &tx, &ty, &tz);
	bool valid = false;

	if (aim_found == true) {
		player_skill_target_x = tx;
		player_skill_target_y = ty;
		player_skill_target_z = tz;

		valid = IsPlayerSkillGroundTargetValid(player_skill_targeting_type, tx, ty, tz);
	}

	player_skill_target_valid = valid;

	d3dg->ResetWorldTransform();
	d3dg->ResetZbuffer();

	int fill_color;
	int border_color;

	if (valid == true) {
		if (player_skill_targeting_type == PLAYER_SKILL_DISTRACTION) {
			fill_color = d3dg->GetColorCode(1.0f, 0.8f, 0.0f, 0.24f);
			border_color = d3dg->GetColorCode(1.0f, 0.9f, 0.1f, 1.0f);
		}
		else if (player_skill_targeting_type == PLAYER_SKILL_TERRORIST_MINE) {
			fill_color = d3dg->GetColorCode(1.0f, 0.45f, 0.05f, 0.22f);
			border_color = d3dg->GetColorCode(1.0f, 0.65f, 0.05f, 1.0f);
		}
		else if (player_skill_targeting_type == PLAYER_SKILL_BIOTIC_FIELD) {
			fill_color = d3dg->GetColorCode(0.25f, 1.0f, 0.55f, 0.18f);
			border_color = d3dg->GetColorCode(0.45f, 1.0f, 0.65f, 1.0f);
		}
		else if (player_skill_targeting_type == PLAYER_SKILL_ARTILLERY_STRIKE) {
			fill_color = d3dg->GetColorCode(1.0f, 0.25f, 0.05f, 0.16f);
			border_color = d3dg->GetColorCode(1.0f, 0.55f, 0.08f, 1.0f);
		}
		else {
			fill_color = d3dg->GetColorCode(0.2f, 1.0f, 0.35f, 0.24f);
			border_color = d3dg->GetColorCode(0.3f, 1.0f, 0.45f, 1.0f);
		}
	}
	else {
		fill_color = d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 0.20f);
		border_color = d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f);
	}

	float preview_radius = 5.0f;
	float preview_height = GetPlayerSkillTargetPreviewHeight(player_skill_targeting_type);

	if (player_skill_targeting_type == PLAYER_SKILL_TERRORIST_MINE) {
		// 실제 설치된 지뢰와 같은 크기
		preview_radius = 3.2f;
		preview_height = 1.4f;
	}
	else if (player_skill_targeting_type == PLAYER_SKILL_BIOTIC_FIELD) {
		// 생체장은 실제 회복 범위가 바닥에 보이도록 크게 표시한다.
		preview_radius = PLAYER_SKILL_BIOTIC_FIELD_RADIUS;
		preview_height = 0.6f;
	}
	else if (player_skill_targeting_type == PLAYER_SKILL_ARTILLERY_STRIKE) {
		// 폭격은 실제 피해 범위가 바닥 원으로 보이게 표시한다.
		preview_radius = PLAYER_SKILL_ARTILLERY_STRIKE_RADIUS;
		preview_height = 0.8f;
	}

	bool render_preview = false;
	float render_x = 0.0f;
	float render_y = 0.0f;
	float render_z = 0.0f;

	if (aim_found == true) {
		render_preview = true;
		render_x = tx;
		render_y = ty;
		render_z = tz;
	}
	else {
		if ((player_skill_target_x != 0.0f) ||
			(player_skill_target_y != 0.0f) ||
			(player_skill_target_z != 0.0f)) {
			render_preview = true;
			render_x = player_skill_target_x;
			render_y = player_skill_target_y;
			render_z = player_skill_target_z;
		}
	}

	if (render_preview == true) {
		RenderSkillTargetCylinder(
			render_x,
			render_y,
			render_z,
			preview_radius,
			preview_height,
			fill_color,
			border_color
		);

		if (player_skill_targeting_type == PLAYER_SKILL_TERRORIST_MINE) {
			// 실제 지뢰와 같은 중심 십자/방향선도 미리보기에서 표시한다.
			int core_color = border_color;

			d3dg->Renderline(
				render_x - 3.8f,
				render_y + 1.8f,
				render_z,
				render_x + 3.8f,
				render_y + 1.8f,
				render_z,
				core_color
			);

			d3dg->Renderline(
				render_x,
				render_y + 1.8f,
				render_z - 3.8f,
				render_x,
				render_y + 1.8f,
				render_z + 3.8f,
				core_color
			);

			float px, py, pz, prx;
			myHuman->GetPosData(&px, &py, &pz, &prx);

			float mine_rx = prx * -1.0f + (float)M_PI / 2.0f;
			float dir_x = render_x - px;
			float dir_z = render_z - pz;
			float dir_len = sqrtf(dir_x * dir_x + dir_z * dir_z);

			if (dir_len > 0.001f) {
				mine_rx = atan2f(dir_z, dir_x);
			}

			float mine_dir_x = cosf(mine_rx);
			float mine_dir_z = sinf(mine_rx);

			d3dg->Renderline(
				render_x,
				render_y + 2.2f,
				render_z,
				render_x + mine_dir_x * 5.0f,
				render_y + 2.2f,
				render_z + mine_dir_z * 5.0f,
				border_color
			);
		}
		else if (player_skill_targeting_type == PLAYER_SKILL_BIOTIC_FIELD) {
			// 생체장 설치 미리보기 중앙 장치.
			// 실제 RenderPlayerBioticField()의 중심 장치와 비슷한 크기로 표시한다.
			int device_fill_color;
			int device_dark_color;
			int device_border_color = border_color;

			if (valid == true) {
				device_fill_color = d3dg->GetColorCode(0.08f, 0.22f, 0.14f, 0.36f);
				device_dark_color = d3dg->GetColorCode(0.03f, 0.10f, 0.06f, 0.55f);
			}
			else {
				device_fill_color = d3dg->GetColorCode(0.25f, 0.02f, 0.02f, 0.34f);
				device_dark_color = d3dg->GetColorCode(0.12f, 0.01f, 0.01f, 0.55f);
			}

			RenderSkillTargetCylinder(
				render_x,
				render_y + 0.05f,
				render_z,
				7.2f,
				2.2f,
				device_fill_color,
				device_border_color
			);

			float device_top_y = render_y + 2.65f;

			// 장치 상단 회복 + 표시
			for (int t = 0; t < 3; t++) {
				float offset = (float)(t - 1) * 0.22f;

				d3dg->Renderline(
					render_x - 3.5f,
					device_top_y + offset,
					render_z,
					render_x + 3.5f,
					device_top_y + offset,
					render_z,
					device_border_color
				);

				d3dg->Renderline(
					render_x + offset,
					device_top_y,
					render_z - 3.5f,
					render_x + offset,
					device_top_y,
					render_z + 3.5f,
					device_border_color
				);
			}

			// 장치 하단 지지대 4개
			for (int leg = 0; leg < 4; leg++) {
				float angle = ((float)M_PI / 4.0f) + ((float)M_PI / 2.0f) * (float)leg;
				float leg_dir_x = cosf(angle);
				float leg_dir_z = sinf(angle);

				d3dg->Renderline(
					render_x + leg_dir_x * 5.8f,
					render_y + 0.95f,
					render_z + leg_dir_z * 5.8f,
					render_x + leg_dir_x * 11.0f,
					render_y + 0.35f,
					render_z + leg_dir_z * 11.0f,
					device_border_color
				);

				d3dg->Renderline(
					render_x + leg_dir_x * 6.8f,
					render_y + 0.55f,
					render_z + leg_dir_z * 6.8f,
					render_x + leg_dir_x * 12.0f,
					render_y + 0.20f,
					render_z + leg_dir_z * 12.0f,
					device_dark_color
				);
			}
		}
	}
}



int maingame::GetPlayerSkillSuppressActiveTimer()
{
	int active_timer = 0;

	for (int i = 0; i < MAX_HUMAN; i++) {
		if (player_skill_suppress_mark_timer[i] > active_timer) {
			active_timer = player_skill_suppress_mark_timer[i];
		}
	}

	return active_timer;
}

bool maingame::IsPlayerSkillHudActive(human* myHuman, int skilltype)
{
	if (myHuman == NULL) { return false; }

	if (skilltype == PLAYER_SKILL_STEALTH) {
		return (myHuman->GetSkillStealthFlag() == true);
	}

	if (skilltype == PLAYER_SKILL_AMMOBOOST) {
		return (myHuman->GetSkillAmmoBoostFlag() == true);
	}

	if (skilltype == PLAYER_SKILL_SURVIVAL) {
		return (myHuman->GetSkillSurvivalFlag() == true);
	}

	if (skilltype == PLAYER_SKILL_IRONBODY) {
		return (myHuman->GetSkillIronBodyFlag() == true);
	}

	if (skilltype == PLAYER_SKILL_ABSORB) {
		return (myHuman->GetSkillAbsorbFlag() == true);
	}

	if (skilltype == PLAYER_SKILL_TACTICAL_SCAN) {
		return ((player_skill_scan_charge_timer > 0) ||
			(player_skill_scan_timer > 0));
	}

	if (skilltype == PLAYER_SKILL_MARKSHOT) {
		return ((player_skill_mark_prep_timer > 0) ||
			(player_skill_mark_timer > 0));
	}

	if (skilltype == PLAYER_SKILL_SUPPRESS) {
		return (GetPlayerSkillSuppressActiveTimer() > 0);
	}

	if (skilltype == PLAYER_SKILL_ROBOT_EXECUTE) {
		return (myHuman->GetSkillRobotExecuteTimer() > 0);
	}

	if (skilltype == PLAYER_SKILL_AIMHACK) {
		return (player_skill_aimhack_timer > 0);
	}

	if (skilltype == PLAYER_SKILL_DISTRACTION) {
		return ((player_skill_distraction_install_pending == true) ||
			(player_skill_distraction_timer > 0));
	}

	if (skilltype == PLAYER_SKILL_TERRORIST_MINE) {
		return (player_skill_terrorist_mine_install_pending == true);
	}

	if (skilltype == PLAYER_SKILL_SUMMON) {
		return (player_skill_summon_install_pending == true);
	}

	if (skilltype == PLAYER_SKILL_BIOTIC_FIELD) {
		return ((player_skill_biotic_field_install_pending == true) ||
			(player_skill_biotic_field_timer > 0));
	}

	if (skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) {
		return ((player_skill_artillery_strike_install_pending == true) ||
			(player_skill_artillery_strike_timer > 0));
	}

	if (skilltype == PLAYER_SKILL_SHADOW_DECOY) {
		return ((player_skill_shadow_decoy_install_pending == true) ||
			(player_skill_shadow_decoy_timer > 0));
	}

	if (skilltype == PLAYER_SKILL_HOOK) {
		return ((player_skill_hook_prep_timer > 0) ||
			(player_skill_hook_pull_timer > 0));
	}

	if (skilltype == PLAYER_SKILL_TELEPORT) {
		return (player_skill_teleport_cast_timer > 0);
	}

	if (skilltype == PLAYER_SKILL_IMPACT_GRENADE) {
		return ((IsPlayerTemporarySkillWeaponVisible() == true) &&
			(GetPlayerTemporarySkillWeaponID() == ID_WEAPON_IMPACT_GRENADE));
	}

	if (skilltype == PLAYER_SKILL_SHIELD) {
		return ((IsPlayerTemporarySkillWeaponVisible() == true) &&
			(GetPlayerTemporarySkillWeaponID() == ID_WEAPON_SHIELD));
	}

	if (skilltype == PLAYER_SKILL_DP28) {
		return IsPlayerDP28SkillWeaponActive();
	}

	return false;
}

bool maingame::GetPlayerSkillHudActiveGaugeInfo(human* myHuman, int skilltype, int* active_timer, int* active_total, int* active_color)
{
	if ((active_timer == NULL) || (active_total == NULL) || (active_color == NULL)) {
		return false;
	}

	*active_timer = 0;
	*active_total = 1;
	*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);

	if (myHuman == NULL) { return false; }

	if (skilltype == PLAYER_SKILL_STEALTH) {
		*active_timer = myHuman->GetSkillStealthTimer();
		*active_total = PlayerSkill_GetDurationFrames(skilltype);
		*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
	}
	else if (skilltype == PLAYER_SKILL_AMMOBOOST) {
		*active_timer = myHuman->GetSkillAmmoBoostTimer();
		*active_total = PlayerSkill_GetDurationFrames(skilltype);
		*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
	}
	else if (skilltype == PLAYER_SKILL_SURVIVAL) {
		*active_timer = myHuman->GetSkillSurvivalTimer();
		*active_total = PlayerSkill_GetDurationFrames(skilltype);
		*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
	}
	else if (skilltype == PLAYER_SKILL_IRONBODY) {
		*active_timer = myHuman->GetSkillIronBodyTimer();
		*active_total = PlayerSkill_GetDurationFrames(skilltype);
		*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
	}
	else if (skilltype == PLAYER_SKILL_ABSORB) {
		*active_timer = myHuman->GetSkillAbsorbTimer();
		*active_total = PlayerSkill_GetDurationFrames(skilltype);
		*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
	}
	else if (skilltype == PLAYER_SKILL_RAGE) {
		*active_timer = myHuman->GetSkillRageTimer();
		*active_total = PlayerSkill_GetDurationFrames(skilltype);
		*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
	}
	else if (skilltype == PLAYER_SKILL_MARKSHOT) {
		if (player_skill_mark_prep_timer > 0) {
			// 전술 스캔 준비 게이지처럼 시간이 지날수록 차오르게 표시한다.
			*active_timer = PLAYER_SKILL_MARKSHOT_PREP_FRAMES - player_skill_mark_prep_timer;
			*active_total = PLAYER_SKILL_MARKSHOT_PREP_FRAMES;
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
		else {
			*active_timer = player_skill_mark_timer;
			*active_total = PlayerSkill_GetDurationFrames(skilltype);
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
	}
	else if (skilltype == PLAYER_SKILL_ROBOT_EXECUTE) {
		if (IsRobotExecuteInPrep(myHuman) == true) {
			*active_timer = PLAYER_SKILL_ROBOT_EXECUTE_PREP_FRAMES -
				(PLAYER_SKILL_ROBOT_EXECUTE_FRAMES - myHuman->GetSkillRobotExecuteTimer());

			*active_timer = PLAYER_SKILL_ROBOT_EXECUTE_PREP_FRAMES - *active_timer;
			*active_total = PLAYER_SKILL_ROBOT_EXECUTE_PREP_FRAMES;
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
		else {
			*active_timer = myHuman->GetSkillRobotExecuteTimer();
			if (*active_timer > PLAYER_SKILL_ROBOT_EXECUTE_ACTIVE_FRAMES) {
				*active_timer = PLAYER_SKILL_ROBOT_EXECUTE_ACTIVE_FRAMES;
			}

			*active_total = PLAYER_SKILL_ROBOT_EXECUTE_ACTIVE_FRAMES;
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
	}
	else if (skilltype == PLAYER_SKILL_AIMHACK) {
		*active_timer = player_skill_aimhack_timer;
		*active_total = PLAYER_SKILL_AIMHACK_FRAMES;
		*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
	}
	else if (skilltype == PLAYER_SKILL_DISTRACTION) {
		if (player_skill_distraction_install_pending == true) {
			// 설치 대기 게이지는 시간이 지날수록 차오르게 표시한다.
			*active_timer = PLAYER_SKILL_DISTRACTION_INSTALL_DELAY_FRAMES -
				player_skill_distraction_install_timer;
			*active_total = PLAYER_SKILL_DISTRACTION_INSTALL_DELAY_FRAMES;
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
		else {
			*active_timer = player_skill_distraction_timer;
			*active_total = PlayerSkill_GetDurationFrames(skilltype);
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
	}
	else if (skilltype == PLAYER_SKILL_TERRORIST_MINE) {
		if (player_skill_terrorist_mine_install_pending == true) {
			// 설치 대기 게이지는 시간이 지날수록 차오르게 표시한다.
			*active_timer = PLAYER_SKILL_TERRORIST_MINE_INSTALL_DELAY_FRAMES -
				player_skill_terrorist_mine_install_timer;
			*active_total = PLAYER_SKILL_TERRORIST_MINE_INSTALL_DELAY_FRAMES;
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
	}
	else if (skilltype == PLAYER_SKILL_SUMMON) {
		if (player_skill_summon_install_pending == true) {
			// 소환 대기 게이지는 시간이 지날수록 차오르게 표시한다.
			*active_timer = PLAYER_SKILL_SUMMON_INSTALL_DELAY_FRAMES -
				player_skill_summon_install_timer;
			*active_total = PLAYER_SKILL_SUMMON_INSTALL_DELAY_FRAMES;
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
	}
	else if (skilltype == PLAYER_SKILL_BIOTIC_FIELD) {
		if (player_skill_biotic_field_install_pending == true) {
			// 설치 대기 게이지는 시간이 지날수록 차오르게 표시한다.
			*active_timer = PLAYER_SKILL_BIOTIC_FIELD_INSTALL_DELAY_FRAMES -
				player_skill_biotic_field_install_timer;
			*active_total = PLAYER_SKILL_BIOTIC_FIELD_INSTALL_DELAY_FRAMES;
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
		else {
			*active_timer = player_skill_biotic_field_timer;
			*active_total = PLAYER_SKILL_BIOTIC_FIELD_FRAMES;
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
	}
	else if (skilltype == PLAYER_SKILL_ARTILLERY_STRIKE) {
		if (player_skill_artillery_strike_install_pending == true) {
			// 호출 대기 게이지는 시간이 지날수록 차오르게 표시한다.
			*active_timer = PLAYER_SKILL_ARTILLERY_STRIKE_INSTALL_DELAY_FRAMES -
				player_skill_artillery_strike_install_timer;
			*active_total = PLAYER_SKILL_ARTILLERY_STRIKE_INSTALL_DELAY_FRAMES;
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
		else {
			*active_timer = player_skill_artillery_strike_timer;
			*active_total = PLAYER_SKILL_ARTILLERY_STRIKE_FRAMES;
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
		}
	else if (skilltype == PLAYER_SKILL_SHADOW_DECOY) {
			if (player_skill_shadow_decoy_install_pending == true) {
				// 소환 대기 게이지는 시간이 지날수록 차오르게 표시한다.
				*active_timer = PLAYER_SKILL_SHADOW_DECOY_INSTALL_DELAY_FRAMES -
					player_skill_shadow_decoy_install_timer;
				*active_total = PLAYER_SKILL_SHADOW_DECOY_INSTALL_DELAY_FRAMES;
				*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
			}
			else {
				*active_timer = player_skill_shadow_decoy_timer;
				*active_total = PLAYER_SKILL_SHADOW_DECOY_FRAMES;
				*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
			}
			}
	else if (skilltype == PLAYER_SKILL_SUPPRESS) {
		*active_timer = GetPlayerSkillSuppressActiveTimer();
		*active_total = PlayerSkill_GetDurationFrames(skilltype);
		*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
	}
	else if (skilltype == PLAYER_SKILL_HOOK) {
		if (player_skill_hook_prep_timer > 0) {
			*active_timer = PLAYER_SKILL_HOOK_PREP_FRAMES - player_skill_hook_prep_timer;
			*active_total = PLAYER_SKILL_HOOK_PREP_FRAMES;
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
		else {
			*active_timer = player_skill_hook_pull_timer;
			*active_total = PLAYER_SKILL_HOOK_PULL_FRAMES;
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
	}
	else if (skilltype == PLAYER_SKILL_TELEPORT) {
		*active_timer = player_skill_teleport_cast_timer;
		*active_total = PLAYER_SKILL_TELEPORT_CAST_FRAMES;
		*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
	}
	else if (skilltype == PLAYER_SKILL_TACTICAL_SCAN) {
		if (player_skill_scan_charge_timer > 0) {
			*active_timer = PLAYER_SKILL_SCAN_CHARGE_FRAMES - player_skill_scan_charge_timer;
			*active_total = PLAYER_SKILL_SCAN_CHARGE_FRAMES;
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
		else {
			*active_timer = player_skill_scan_timer;
			*active_total = PLAYER_SKILL_SCAN_FRAMES;
			*active_color = GetPlayerSkillHudThemeColor(skilltype, 0.90f);
		}
	}

	if (*active_total <= 0) {
		*active_total = 1;
	}

	return (*active_timer > 0);
}

// 로봇 처형 프로토콜: 마커 표시
void maingame::RenderRobotExecutePings(human* myHuman)
{
	if (myHuman == NULL) { return; }

	// 준비/조준 중뿐 아니라, 좌클릭 후 순차 처형 큐가 남아 있는 동안에도 화면 효과를 유지한다.
	bool robot_execute_effect_active =
		(myHuman->GetSkillRobotExecuteFlag() == true) ||
		(player_skill_robot_fire_queue_count > 0);

	if (robot_execute_effect_active == false) { return; }
	if (Camera_Debugmode == true) { return; }

	// 발동 직후 2초 준비 구간에는 마커가 나오지 않는다.
	if (IsRobotExecuteInPrep(myHuman) == true) { return; }

	int ids[PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS];
	float orders[PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS];

	int count = CollectRobotExecuteTargets(
		myHuman,
		ids,
		orders,
		PLAYER_SKILL_ROBOT_EXECUTE_MAX_TARGETS
	);

	d3dg->ResetWorldTransform();
	d3dg->ResetZbuffer();

	for (int i = 0; i < count; i++) {
		human* target = ObjMgr.GetHumanObject(ids[i]);
		if (target == NULL) { continue; }
		if (target->GetEnableFlag() == false) { continue; }
		if (target->GetDeadFlag() == true) { continue; }
		if (target->GetHP() <= 0) { continue; }

		float x, y, z, rx;
		target->GetPosData(&x, &y, &z, &rx);

		// 머리 쪽에 마커 표시
		RenderPlayerSkillEnemyPing(
			x,
			y + HUMAN_HEIGHT * 0.76f,
			z,
			1.8f,
			2.4f
		);
	}
}

// 로봇 처형 프로토콜: 화면 흑백 느낌 오버레이
void maingame::RenderRobotExecuteScreenEffect(human* myHuman)
{
	if (myHuman == NULL) { return; }

	// 준비/조준 중뿐 아니라, 좌클릭 후 순차 처형 큐가 남아 있는 동안에도 화면 효과를 유지한다.
	bool robot_execute_effect_active =
		(myHuman->GetSkillRobotExecuteFlag() == true) ||
		(player_skill_robot_fire_queue_count > 0);

	if (robot_execute_effect_active == false) { return; }
	if (Camera_Debugmode == true) { return; }

	int sw = GameConfig.GetScreenWidth();
	int sh = GameConfig.GetScreenHeight();

	float prep_rate = GetRobotExecutePrepRate(myHuman);

	// 준비 2초 동안 0 → 1
	// 준비가 끝난 뒤에는 계속 1로 유지한다.
	float effect_rate = prep_rate;
	if (IsRobotExecuteInPrep(myHuman) == false) {
		effect_rate = 1.0f;
	}

	// 밝은 회색 필터: 색을 씻어내는 느낌
	float gray_alpha = 0.10f + 0.32f * effect_rate;

	d3dg->Draw2DBox(
		0,
		0,
		sw,
		sh,
		d3dg->GetColorCode(0.55f, 0.55f, 0.55f, gray_alpha)
	);

	// 약한 검은 필터: 너무 밝게 뜨는 것을 방지
	float dark_alpha = 0.04f + 0.08f * effect_rate;

	d3dg->Draw2DBox(
		0,
		0,
		sw,
		sh,
		d3dg->GetColorCode(0.0f, 0.0f, 0.0f, dark_alpha)
	);

	// 로봇 처형 프로토콜 전용 조준선
	// X 모양이며, 가운데는 비워둔다.
	int cx = sw / 2;
	int cy = sh / 2;

	int color = d3dg->GetColorCode(1.0f, 0.05f, 0.05f, 0.95f);

	int gap = 7;
	int length = 25;
	int thickness = 2;

	for (int t = 0; t < thickness; t++) {
		// 왼쪽 위
		d3dg->Draw2DLine(
			cx - gap - t,
			cy - gap,
			cx - length - t,
			cy - length,
			color
		);

		// 오른쪽 아래
		d3dg->Draw2DLine(
			cx + gap + t,
			cy + gap,
			cx + length + t,
			cy + length,
			color
		);

		// 왼쪽 아래
		d3dg->Draw2DLine(
			cx - gap - t,
			cy + gap,
			cx - length - t,
			cy + length,
			color
		);

		// 오른쪽 위
		d3dg->Draw2DLine(
			cx + gap + t,
			cy - gap,
			cx + length + t,
			cy - length,
			color
		);
	}
}

void maingame::RenderPlayerSkillHUDSlot(human* myHuman, int slot, int icon_x, int icon_y)
{
	if (myHuman == NULL) { return; }

	if ((slot < 0) || (slot >= PLAYER_SKILL_SLOT_COUNT)) {
		return;
	}

	int skilltype = GetPlayerSkillTypeBySlot(myHuman, slot);
	if (skilltype == PLAYER_SKILL_NONE) { return; }

	int cooldown_total = PlayerSkill_GetCooldownFrames(skilltype);

	if ((player_skill_cooldown_timer[slot] > 0) &&
		(player_skill_cooldown_total_timer[slot] > 0)) {
		cooldown_total = player_skill_cooldown_total_timer[slot];
	}

	if (cooldown_total <= 0) {
		cooldown_total = 1;
	}

	float ready_rate = 1.0f;

	if (player_skill_cooldown_timer[slot] > 0) {
		ready_rate = 1.0f - ((float)player_skill_cooldown_timer[slot] / (float)cooldown_total);
	}

	if (ready_rate < 0.0f) { ready_rate = 0.0f; }
	if (ready_rate > 1.0f) { ready_rate = 1.0f; }

	float icon_alpha = 1.0f;

	if (player_skill_cooldown_timer[slot] > 0) {
		icon_alpha = 0.35f + 0.45f * ready_rate;
	}

	int icon_size = 42;

	// 배경은 고정 보라색 대신 어두운 중립색으로 두고,
	// 테두리/진행바/활성 강조만 스킬 계열 색상을 사용한다.
	int bg_color = d3dg->GetColorCode(0.06f, 0.06f, 0.08f, icon_alpha);
	int border_color;

	if (player_skill_cooldown_timer[slot] <= 0) {
		float pulse_alpha = 0.75f;

		if ((time % (int)(GAMEFPS)) < (int)(GAMEFPS / 2)) {
			pulse_alpha = 1.0f;
		}

		border_color = GetPlayerSkillHudThemeColor(skilltype, pulse_alpha);
	}
	else {
		border_color = GetPlayerSkillHudThemeColor(skilltype, 0.35f);
	}

	d3dg->Draw2DBox(icon_x, icon_y, icon_x + icon_size, icon_y + icon_size, bg_color);

	int skill_icon_id = PlayerSkill_GetIconID(skilltype);

	int skill_icon_texture = -1;
	if (skill_icon_id >= 0) {
		skill_icon_texture = Resource.GetSkillIconTexture(skill_icon_id);
	}

	if (skill_icon_texture != -1) {
		d3dg->Draw2DTexture(icon_x + 4, icon_y + 4, skill_icon_texture, icon_size - 8, icon_size - 8, icon_alpha);
	}
	else {
		const char* icon_text = PlayerSkill_GetFallbackIconText(skilltype);

		d3dg->Draw2DTextureDebugFontText(
			icon_x + 15,
			icon_y + 14,
			icon_text,
			d3dg->GetColorCode(1.0f, 1.0f, 1.0f, icon_alpha)
		);
	}

	if (player_skill_cooldown_timer[slot] > 0) {
		int cover_h = (int)((float)icon_size * (1.0f - ready_rate));

		if (cover_h > 0) {
			d3dg->Draw2DBox(
				icon_x,
				icon_y,
				icon_x + icon_size,
				icon_y + cover_h,
				d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.55f)
			);
		}

		int progress_w = (int)((float)icon_size * ready_rate);

		d3dg->Draw2DBox(
			icon_x,
			icon_y + icon_size + 3,
			icon_x + progress_w,
			icon_y + icon_size + 6,
			GetPlayerSkillHudThemeColor(skilltype, 0.85f)
		);
	}

	if (IsPlayerSkillHudActive(myHuman, skilltype) == true) {
		d3dg->Draw2DBox(
			icon_x + 3,
			icon_y + 3,
			icon_x + icon_size - 3,
			icon_y + icon_size - 3,
			GetPlayerSkillHudThemeColor(skilltype, 0.25f)
		);
	}

	// 쿨타임이 아닐 때, 현재 조건상 사용할 수 없는 스킬은 금지 빗금 표시를 한다.
	// 쿨타임 중에는 기존 쿨타임 덮개가 이미 상태를 보여주므로 빗금을 중복 표시하지 않는다.
	if ((player_skill_cooldown_timer[slot] <= 0) &&
		(IsPlayerSkillUnavailableForHud(myHuman, skilltype) == true)) {

		d3dg->Draw2DBox(
			icon_x + 3,
			icon_y + 3,
			icon_x + icon_size - 3,
			icon_y + icon_size - 3,
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.35f)
		);

		int slash_color = d3dg->GetColorCode(1.0f, 0.10f, 0.10f, 0.95f);
		int slash_shadow = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.75f);

		for (int t = 0; t < 3; t++) {
			d3dg->Draw2DLine(
				icon_x + 8 + t,
				icon_y + icon_size - 8,
				icon_x + icon_size - 8 + t,
				icon_y + 8,
				slash_shadow
			);
		}

		for (int t = 0; t < 2; t++) {
			d3dg->Draw2DLine(
				icon_x + 8 + t,
				icon_y + icon_size - 8,
				icon_x + icon_size - 8 + t,
				icon_y + 8,
				slash_color
			);
		}
	}

	d3dg->Draw2DLine(icon_x, icon_y, icon_x + icon_size, icon_y, border_color);
	d3dg->Draw2DLine(icon_x, icon_y + icon_size, icon_x + icon_size, icon_y + icon_size, border_color);
	d3dg->Draw2DLine(icon_x, icon_y, icon_x, icon_y + icon_size, border_color);
	d3dg->Draw2DLine(icon_x + icon_size, icon_y, icon_x + icon_size, icon_y + icon_size, border_color);

	if (skilltype == PLAYER_SKILL_TERRORIST_MINE) {
		int mine_count = GetPlayerTerroristMineCount();

		if (mine_count > 0) {
			if (mine_count > MAX_TERRORIST_MINES) {
				mine_count = MAX_TERRORIST_MINES;
			}

			char mine_count_text[4];
			sprintf(mine_count_text, "%d", mine_count);

			int badge_cx = icon_x + icon_size - 5;
			int badge_cy = icon_y + 6;
			int badge_r = 9;
			int badge_bg = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.72f);
			int badge_border = d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.75f);

			for (int dy = -badge_r; dy <= badge_r; dy++) {
				float inside = (float)(badge_r * badge_r - dy * dy);

				if (inside < 0.0f) {
					continue;
				}

				int half_w = (int)sqrtf(inside);
				d3dg->Draw2DLine(
					badge_cx - half_w,
					badge_cy + dy,
					badge_cx + half_w,
					badge_cy + dy,
					badge_bg
				);
			}

			d3dg->Draw2DCycle(badge_cx, badge_cy, badge_r, badge_border);
			d3dg->Draw2DTextureDebugFontText(
				badge_cx - 4,
				badge_cy - 7,
				mine_count_text,
				d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f)
			);
		}
	}

	int key_id = KEY_SKILL;
	if (slot == PLAYER_SKILL_SLOT_SECONDARY) {
		key_id = KEY_SKILL2;
	}

	const char* skillKeyName = GameConfig.GetOriginalkeycodeString(GameConfig.GetKeycode(key_id));
	int key_text_w = (int)strlen(skillKeyName) * 8;
	int key_text_x = icon_x + (icon_size - key_text_w) / 2;

	d3dg->Draw2DTextureDebugFontText(
		key_text_x,
		icon_y + icon_size + 9,
		skillKeyName,
		d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.80f)
	);
}

void maingame::RenderPlayerSkillScreenEffect(human* myHuman)
{
	if (myHuman == NULL) { return; }

	if (Camera_Debugmode == true) {
		return;
	}

	if (myHuman->GetHP() <= 0) {
		return;
	}

	// 요원A 마킹 샷: 준비 1초 동안 화면을 차가운 청색으로 덮는다.
	if (player_skill_mark_prep_timer > 0) {
		float rate = (float)player_skill_mark_prep_timer / (float)PLAYER_SKILL_MARKSHOT_PREP_FRAMES;
		if (rate < 0.0f) { rate = 0.0f; }
		if (rate > 1.0f) { rate = 1.0f; }

		float pulse = 0.0f;
		if ((time % 12) < 6) {
			pulse = 0.03f;
		}

		// 전술 조준/분석 느낌의 남청색 필터
		d3dg->Draw2DBox(
			0,
			0,
			GameConfig.GetScreenWidth(),
			GameConfig.GetScreenHeight(),
			d3dg->GetColorCode(0.05f, 0.12f, 0.25f, 0.22f + pulse)
		);

		// 너무 밝게 뜨지 않도록 약한 어두운 필터
		d3dg->Draw2DBox(
			0,
			0,
			GameConfig.GetScreenWidth(),
			GameConfig.GetScreenHeight(),
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.05f)
		);
	}

	// 군인 전투 강화: 지속 시간 동안 아주 약한 푸른 화면 필터
	if (myHuman->GetSkillAmmoBoostFlag() == true) {
		int timer = myHuman->GetSkillAmmoBoostTimer();

		float fade = 1.0f;
		if (timer < (GAMEFPS / 2)) {
			fade = (float)timer / (float)(GAMEFPS / 2);
		}

		if (fade < 0.0f) { fade = 0.0f; }
		if (fade > 1.0f) { fade = 1.0f; }

		float alpha = 0.055f;

		// 너무 정적인 느낌을 줄이기 위한 아주 약한 맥동
		if ((time % 24) < 12) {
			alpha = 0.075f;
		}

		alpha *= fade;

		d3dg->Draw2DBox(
			0,
			0,
			GameConfig.GetScreenWidth(),
			GameConfig.GetScreenHeight(),
			d3dg->GetColorCode(0.08f, 0.32f, 0.85f, alpha)
		);
	}

	// 용병 전투 흡수: 지속 시간 동안 약한 검보라 화면 필터
	if (myHuman->GetSkillAbsorbFlag() == true) {
		int timer = myHuman->GetSkillAbsorbTimer();

		float fade = 1.0f;
		if (timer < (GAMEFPS / 2)) {
			fade = (float)timer / (float)(GAMEFPS / 2);
		}

		if (fade < 0.0f) { fade = 0.0f; }
		if (fade > 1.0f) { fade = 1.0f; }

		float alpha = 0.085f;

		// 흡수막 느낌을 위해 전투 강화보다 약간 더 체감되게 한다.
		if ((time % 28) < 14) {
			alpha = 0.115f;
		}

		alpha *= fade;

		d3dg->Draw2DBox(
			0,
			0,
			GameConfig.GetScreenWidth(),
			GameConfig.GetScreenHeight(),
			d3dg->GetColorCode(0.22f, 0.03f, 0.38f, alpha)
		);

		// 너무 보라색만 뜨지 않도록 아주 약한 어두운 필터를 겹친다.
		d3dg->Draw2DBox(
			0,
			0,
			GameConfig.GetScreenWidth(),
			GameConfig.GetScreenHeight(),
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.025f * fade)
		);
	}

	// 좀비 격노: 지속 시간 동안 화면을 약하게 붉게 덮는다.
	if (myHuman->GetSkillRageFlag() == true) {
		float alpha = 0.12f;

		// 너무 정적인 느낌을 줄이기 위한 약한 맥동 효과
		if ((time % 20) < 10) {
			alpha = 0.15f;
		}

		d3dg->Draw2DBox(
			0,
			0,
			GameConfig.GetScreenWidth(),
			GameConfig.GetScreenHeight(),
			d3dg->GetColorCode(1.0f, 0.0f, 0.0f, alpha)
		);
	}
}

void maingame::RenderPlayerSkillHUD(human* myHuman)
{
	if (myHuman == NULL) { return; }

	if (GameConfig.GetSkillModeFlag() == false) {
		return;
	}

	bool has_skill = false;

	for (int slot = 0; slot < PLAYER_SKILL_SLOT_COUNT; slot++) {
		int slot_skilltype = GetPlayerSkillTypeBySlot(myHuman, slot);

		if (slot_skilltype != PLAYER_SKILL_NONE) {
			has_skill = true;
			break;
		}
	}

	if (has_skill == false) { return; }

	int sw = GameConfig.GetScreenWidth();

	if (Camera_Debugmode == false) {
		for (int slot = 0; slot < PLAYER_SKILL_SLOT_COUNT; slot++) {
			int slot_skilltype = GetPlayerSkillTypeBySlot(myHuman, slot);

			if (slot_skilltype == PLAYER_SKILL_NONE) {
				continue;
			}

			int active_timer = 0;
			int active_total = 1;
			int active_color = d3dg->GetColorCode(0.55f, 0.25f, 1.0f, 0.90f);

			if (GetPlayerSkillHudActiveGaugeInfo(myHuman, slot_skilltype, &active_timer, &active_total, &active_color) == false) {
				continue;
			}

			float remain_rate = (float)active_timer / (float)active_total;
			if (remain_rate < 0.0f) { remain_rate = 0.0f; }
			if (remain_rate > 1.0f) { remain_rate = 1.0f; }

			int bar_w = sw / 3;
			if (bar_w < 260) { bar_w = 260; }
			if (bar_w > 520) { bar_w = 520; }

			int bar_h = 9;
			int bar_x = (sw - bar_w) / 2;

			// slot 1(E)은 기존 위치, slot 2(F)는 그 아래에 표시한다.
			int bar_y = 24 + (slot * 13);

			int fill_w = (int)((float)bar_w * remain_rate);

			d3dg->Draw2DBox(bar_x, bar_y, bar_x + bar_w, bar_y + bar_h, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.55f));
			d3dg->Draw2DBox(bar_x, bar_y, bar_x + fill_w, bar_y + bar_h, active_color);

			d3dg->Draw2DLine(bar_x, bar_y, bar_x + bar_w, bar_y, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.65f));
			d3dg->Draw2DLine(bar_x, bar_y + bar_h, bar_x + bar_w, bar_y + bar_h, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.65f));
			d3dg->Draw2DLine(bar_x, bar_y, bar_x, bar_y + bar_h, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.65f));
			d3dg->Draw2DLine(bar_x + bar_w, bar_y, bar_x + bar_w, bar_y + bar_h, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.65f));
		}
	}

	if (Camera_F2mode == 0) {
		int icon_size = 42;
		int icon_gap = 8;

		// F 스킬 아이콘은 기존 오른쪽 위치에 둔다.
		int secondary_icon_x = HUDA_WEAPON_POSX - icon_size - 12;
		int icon_y = HUDA_WEAPON_POSY + 25;

		// E 스킬 아이콘은 F 스킬 왼쪽에 배치한다.
		int primary_icon_x = secondary_icon_x - icon_size - icon_gap;

		RenderPlayerSkillHUDSlot(myHuman, PLAYER_SKILL_SLOT_PRIMARY, primary_icon_x, icon_y);
		RenderPlayerSkillHUDSlot(myHuman, PLAYER_SKILL_SLOT_SECONDARY, secondary_icon_x, icon_y);
	}
}