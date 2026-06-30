//! @file gamemain_skilleffect.cpp
//! @brief 플레이어 스킬 월드 이펙트 표시 처리

#include "gamemain.h"
#include "playerskill.h"

void maingame::RenderPlayerHealEffect(human* myHuman)
{
	if (myHuman == NULL) { return; }
	if (player_skill_heal_effect_timer <= 0) { return; }
	if (Camera_Debugmode == true) { return; }
	if (myHuman->GetHP() <= 0) { return; }

	float x, y, z, rx;
	myHuman->GetPosData(&x, &y, &z, &rx);

	d3dg->ResetWorldTransform();

	// ObjMgr.Render() 이후에 호출되므로, 캐릭터 주변 효과가 몸에 가려지지 않도록 한다.
	d3dg->ResetZbuffer();

	float rate = (float)player_skill_heal_effect_timer / (float)PLAYER_SKILL_HEAL_EFFECT_FRAMES;
	if (rate < 0.0f) { rate = 0.0f; }
	if (rate > 1.0f) { rate = 1.0f; }

	// 좀비 격노 파티클과 같은 구조를 사용하되, 색만 연두색으로 바꾼다.
	// 단색 느낌을 유지하기 위해 RGB는 같게 두고 알파만 다르게 사용한다.
	int strong_color = d3dg->GetColorCode(0.55f, 1.0f, 0.25f, 0.88f * rate);
	int weak_color = d3dg->GetColorCode(0.55f, 1.0f, 0.25f, 0.58f * rate);
	int trail_color = d3dg->GetColorCode(0.55f, 1.0f, 0.25f, 0.34f * rate);

	const float dir_x[16] = {
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f,
		0.0000f, 0.3827f, 0.7071f, 0.9239f
	};

	const float dir_z[16] = {
		0.0000f, 0.3827f, 0.7071f, 0.9239f,
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f
	};

	int base_time = time;

	// 격노와 같은 느낌이지만, 응급 치료는 순간 효과이므로 0.3초 타이머 동안만 표시된다.
	const int PARTICLE_COUNT = 18;

	for (int i = 0; i < PARTICLE_COUNT; i++) {
		int seed = i * 37 + (base_time / 2) * 13;
		int dir = seed & 15;

		float phase = (float)((base_time * 2 + i * 7) % 48) / 48.0f;
		float radius = 1.4f + (float)((seed / 3) % 12) * 0.18f;

		float start_y = y + 0.4f + phase * (HUMAN_HEIGHT * 0.78f);
		float line_len = 2.2f + (float)((seed / 5) % 7) * 0.35f;
		float end_y = start_y + line_len;

		if (end_y > y + HUMAN_HEIGHT * 0.95f) {
			end_y = y + HUMAN_HEIGHT * 0.95f;
		}

		float sx = x + dir_x[dir] * radius;
		float sz = z + dir_z[dir] * radius;

		int sway_dir = (dir + 1 + (i % 3)) & 15;
		float ex = sx + dir_x[sway_dir] * 0.35f;
		float ez = sz + dir_z[sway_dir] * 0.35f;

		int color = weak_color;
		if ((i & 1) == 0) {
			color = strong_color;
		}

		d3dg->Renderline(
			sx, start_y, sz,
			ex, end_y, ez,
			color
		);

		// 일부 파티클은 발밑에서 위로 올라가는 짧은 꼬리를 추가한다.
		if ((i % 3) == 0) {
			float br = radius * 0.70f;
			float bx = x + dir_x[dir] * br;
			float bz = z + dir_z[dir] * br;

			d3dg->Renderline(
				bx, y + 0.25f, bz,
				sx, start_y, sz,
				trail_color
			);
		}
	}

	// 어깨 높이 주변에도 몇 개의 짧은 상승선을 추가한다.
	// 격노와 같은 구조지만, 응급 치료는 타이머가 짧아 발동 직후 1회성으로만 보인다.
	for (int j = 0; j < 4; j++) {
		int dir = ((base_time / 3) + j * 4) & 15;

		float shoulder_y = y + HUMAN_HEIGHT * 0.45f + (float)((base_time + j) % 3) * 0.35f;
		float radius = 2.0f + (float)j * 0.25f;

		float sx = x + dir_x[dir] * radius;
		float sz = z + dir_z[dir] * radius;

		int end_dir = (dir + 2) & 15;
		float ex = sx + dir_x[end_dir] * 0.45f;
		float ez = sz + dir_z[end_dir] * 0.45f;

		d3dg->Renderline(
			sx, shoulder_y, sz,
			ex, shoulder_y + 1.8f, ez,
			weak_color
		);
	}
}

void maingame::RenderPlayerSurvivalEffect(human* myHuman)
{
	if (myHuman == NULL) { return; }
	if (player_skill_survival_effect_timer <= 0) { return; }
	if (Camera_Debugmode == true) { return; }
	if (myHuman->GetHP() <= 0) { return; }

	float x, y, z, rx;
	myHuman->GetPosData(&x, &y, &z, &rx);

	d3dg->ResetWorldTransform();

	// ObjMgr.Render() 이후에 호출되므로, 캐릭터 주변 효과가 몸에 가려지지 않도록 한다.
	d3dg->ResetZbuffer();

	float rate = (float)player_skill_survival_effect_timer / (float)PLAYER_SKILL_SURVIVAL_EFFECT_FRAMES;
	if (rate < 0.0f) { rate = 0.0f; }
	if (rate > 1.0f) { rate = 1.0f; }

	// 기존 회복 파티클 구조를 사용하되, 생존 본능은 옅은 노랑/금색 계열로 표시한다.
	// 응급 치료의 연두색과 겹치지 않도록 G를 낮추고 R을 높게 둔다.
	int strong_color = d3dg->GetColorCode(1.0f, 0.85f, 0.25f, 0.88f * rate);
	int weak_color = d3dg->GetColorCode(1.0f, 0.85f, 0.25f, 0.58f * rate);
	int trail_color = d3dg->GetColorCode(1.0f, 0.85f, 0.25f, 0.34f * rate);

	const float dir_x[16] = {
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f,
		0.0000f, 0.3827f, 0.7071f, 0.9239f
	};

	const float dir_z[16] = {
		0.0000f, 0.3827f, 0.7071f, 0.9239f,
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f
	};

	int base_time = time;

	const int PARTICLE_COUNT = 18;

	for (int i = 0; i < PARTICLE_COUNT; i++) {
		int seed = i * 37 + (base_time / 2) * 13;
		int dir = seed & 15;

		float phase = (float)((base_time * 2 + i * 7) % 48) / 48.0f;
		float radius = 1.4f + (float)((seed / 3) % 12) * 0.18f;

		float start_y = y + 0.4f + phase * (HUMAN_HEIGHT * 0.78f);
		float line_len = 2.2f + (float)((seed / 5) % 7) * 0.35f;
		float end_y = start_y + line_len;

		if (end_y > y + HUMAN_HEIGHT * 0.95f) {
			end_y = y + HUMAN_HEIGHT * 0.95f;
		}

		float sx = x + dir_x[dir] * radius;
		float sz = z + dir_z[dir] * radius;

		int sway_dir = (dir + 1 + (i % 3)) & 15;
		float ex = sx + dir_x[sway_dir] * 0.35f;
		float ez = sz + dir_z[sway_dir] * 0.35f;

		int color = weak_color;
		if ((i & 1) == 0) {
			color = strong_color;
		}

		d3dg->Renderline(
			sx, start_y, sz,
			ex, end_y, ez,
			color
		);

		if ((i % 3) == 0) {
			float br = radius * 0.70f;
			float bx = x + dir_x[dir] * br;
			float bz = z + dir_z[dir] * br;

			d3dg->Renderline(
				bx, y + 0.25f, bz,
				sx, start_y, sz,
				trail_color
			);
		}
	}

	for (int j = 0; j < 4; j++) {
		int dir = ((base_time / 3) + j * 4) & 15;

		float shoulder_y = y + HUMAN_HEIGHT * 0.45f + (float)((base_time + j) % 3) * 0.35f;
		float radius = 2.0f + (float)j * 0.25f;

		float sx = x + dir_x[dir] * radius;
		float sz = z + dir_z[dir] * radius;

		int end_dir = (dir + 2) & 15;
		float ex = sx + dir_x[end_dir] * 0.45f;
		float ez = sz + dir_z[end_dir] * 0.45f;

		d3dg->Renderline(
			sx, shoulder_y, sz,
			ex, shoulder_y + 1.8f, ez,
			weak_color
		);
	}
}

void maingame::RenderPlayerSummonEffect()
{
	if (player_skill_summon_effect_timer <= 0) { return; }
	if (player_skill_summon_effect_count <= 0) { return; }
	if (Camera_Debugmode == true) { return; }

	d3dg->ResetWorldTransform();

	// ObjMgr.Render() 이후에 호출되므로, 소환 위치의 파티클이 동료 몸에 묻히지 않도록 한다.
	d3dg->ResetZbuffer();

	float rate = (float)player_skill_summon_effect_timer / (float)PLAYER_SKILL_SUMMON_EFFECT_FRAMES;
	if (rate < 0.0f) { rate = 0.0f; }
	if (rate > 1.0f) { rate = 1.0f; }

	const float dir_x[16] = {
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f,
		0.0000f, 0.3827f, 0.7071f, 0.9239f
	};

	const float dir_z[16] = {
		0.0000f, 0.3827f, 0.7071f, 0.9239f,
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f
	};

	// 동료 소환은 밝은 청록 계열로 유지한다.
	int strong_color = d3dg->GetColorCode(0.35f, 1.0f, 0.95f, 0.88f * rate);
	int weak_color = d3dg->GetColorCode(0.75f, 1.0f, 1.0f, 0.58f * rate);
	int trail_color = d3dg->GetColorCode(0.35f, 1.0f, 0.95f, 0.34f * rate);

	int base_time = time;

	for (int s = 0; s < player_skill_summon_effect_count; s++) {
		if (s < 0) { continue; }
		if (s >= PLAYER_SKILL_SUMMON_COUNT) { continue; }

		float x = player_skill_summon_effect_x[s];
		float y = player_skill_summon_effect_y[s];
		float z = player_skill_summon_effect_z[s];

		// 기존 18개보다 조금 늘리고, 격노처럼 넓은 반경에서 긴 선이 솟도록 한다.
		const int PARTICLE_COUNT = 22;

		for (int i = 0; i < PARTICLE_COUNT; i++) {
			int seed = s * 53 + i * 37 + (base_time / 2) * 13;
			int dir = seed & 15;

			float phase = (float)((base_time * 2 + i * 7 + s * 11) % 48) / 48.0f;

			// 기존보다 넓게: 대략 1.4 ~ 3.4 범위
			float radius = 1.4f + (float)((seed / 3) % 12) * 0.18f;

			float start_y = y + 0.35f + phase * (HUMAN_HEIGHT * 0.75f);

			// 기존보다 길게: 격노와 비슷한 긴 상승선
			float line_len = 2.2f + (float)((seed / 5) % 7) * 0.35f;
			float end_y = start_y + line_len;

			if (end_y > y + HUMAN_HEIGHT * 1.08f) {
				end_y = y + HUMAN_HEIGHT * 1.08f;
			}

			float sx = x + dir_x[dir] * radius;
			float sz = z + dir_z[dir] * radius;

			int sway_dir = (dir + 1 + (i % 3)) & 15;
			float ex = sx + dir_x[sway_dir] * 0.42f;
			float ez = sz + dir_z[sway_dir] * 0.42f;

			int color = weak_color;
			if ((i & 1) == 0) {
				color = strong_color;
			}

			d3dg->Renderline(
				sx, start_y, sz,
				ex, end_y, ez,
				color
			);

			// 발밑에서 위로 이어지는 꼬리선을 추가해서 격노 파티클처럼 넓고 강하게 보이게 한다.
			if ((i % 3) == 0) {
				float br = radius * 0.70f;
				float bx = x + dir_x[dir] * br;
				float bz = z + dir_z[dir] * br;

				d3dg->Renderline(
					bx, y + 0.25f, bz,
					sx, start_y, sz,
					trail_color
				);
			}
		}

		// 어깨 높이 주변에도 긴 상승선을 추가한다.
		for (int j = 0; j < 4; j++) {
			int dir = ((base_time / 3) + s * 5 + j * 4) & 15;

			float shoulder_y = y + HUMAN_HEIGHT * 0.45f + (float)((base_time + j) % 3) * 0.35f;
			float radius = 2.0f + (float)j * 0.30f;

			float sx = x + dir_x[dir] * radius;
			float sz = z + dir_z[dir] * radius;

			int end_dir = (dir + 2) & 15;
			float ex = sx + dir_x[end_dir] * 0.50f;
			float ez = sz + dir_z[end_dir] * 0.50f;

			d3dg->Renderline(
				sx, shoulder_y, sz,
				ex, shoulder_y + 1.9f, ez,
				weak_color
			);
		}

		// 소환 위치 바닥 링도 기존보다 넓게 퍼지게 한다.
		const int RING_COUNT = 24;
		float elapsed = 1.0f - rate;
		float ring_radius = 1.4f + elapsed * 2.0f;
		const float PI2 = (float)M_PI * 2.0f;

		for (int i = 0; i < RING_COUNT; i++) {
			float a1 = PI2 * (float)i / (float)RING_COUNT;
			float a2 = PI2 * (float)(i + 1) / (float)RING_COUNT;

			d3dg->Renderline(
				x + cosf(a1) * ring_radius,
				y + 0.20f,
				z + sinf(a1) * ring_radius,
				x + cosf(a2) * ring_radius,
				y + 0.20f,
				z + sinf(a2) * ring_radius,
				trail_color
			);
		}
	}
}

void maingame::RenderPlayerBioticField()
{
	if (player_skill_biotic_field_timer <= 0) { return; }
	if (Camera_Debugmode == true) { return; }

	d3dg->ResetWorldTransform();

	// 생체장은 설치형 바닥 효과다.
	// 벽/지형 너머로 완전히 뚫려 보이지 않도록 ResetZbuffer()는 쓰지 않는다.
	float x = player_skill_biotic_field_x;
	float y = player_skill_biotic_field_y;
	float z = player_skill_biotic_field_z;

	float fade = 1.0f;

	if (player_skill_biotic_field_timer < (GAMEFPS / 2)) {
		fade = (float)player_skill_biotic_field_timer / (float)(GAMEFPS / 2);
	}

	int elapsed = PLAYER_SKILL_BIOTIC_FIELD_FRAMES - player_skill_biotic_field_timer;
	if (elapsed < (GAMEFPS / 3)) {
		float fade_in = (float)elapsed / (float)(GAMEFPS / 3);
		if (fade_in < fade) {
			fade = fade_in;
		}
	}

	if (fade < 0.0f) { fade = 0.0f; }
	if (fade > 1.0f) { fade = 1.0f; }

	int fill_color = d3dg->GetColorCode(0.18f, 1.0f, 0.42f, 0.10f * fade);
	int border_color = d3dg->GetColorCode(0.38f, 1.0f, 0.58f, 0.88f * fade);
	int weak_color = d3dg->GetColorCode(0.55f, 1.0f, 0.72f, 0.42f * fade);
	int strong_color = d3dg->GetColorCode(0.72f, 1.0f, 0.82f, 0.82f * fade);

	// 회복 범위 바닥 표시
	RenderSkillTargetCylinder(
		x,
		y,
		z,
		PLAYER_SKILL_BIOTIC_FIELD_RADIUS,
		0.45f,
		fill_color,
		border_color
	);

	// 생체장 중심 장치 본체.
	// 별도 모델 없이 선/원통 표시만으로 아이콘 이미지의 장치 느낌을 낸다.
	int device_fill_color = d3dg->GetColorCode(0.08f, 0.22f, 0.14f, 0.42f * fade);
	int device_border_color = d3dg->GetColorCode(0.70f, 1.0f, 0.78f, 0.92f * fade);
	int device_dark_color = d3dg->GetColorCode(0.03f, 0.10f, 0.06f, 0.70f * fade);

	RenderSkillTargetCylinder(
		x,
		y + 0.05f,
		z,
		7.2f,
		2.2f,
		device_fill_color,
		device_border_color
	);

	// 장치 상단의 회복 + 표시
	float device_top_y = y + 2.65f;

	for (int t = 0; t < 3; t++) {
		float offset = (float)(t - 1) * 0.22f;

		d3dg->Renderline(
			x - 3.5f,
			device_top_y + offset,
			z,
			x + 3.5f,
			device_top_y + offset,
			z,
			device_border_color
		);

		d3dg->Renderline(
			x + offset,
			device_top_y,
			z - 3.5f,
			x + offset,
			device_top_y,
			z + 3.5f,
			device_border_color
		);
	}

	// 장치 하단 지지대 4개
	for (int leg = 0; leg < 4; leg++) {
		float angle = ((float)M_PI / 4.0f) + ((float)M_PI / 2.0f) * (float)leg;
		float dir_x = cosf(angle);
		float dir_z = sinf(angle);

		d3dg->Renderline(
			x + dir_x * 5.8f,
			y + 0.95f,
			z + dir_z * 5.8f,
			x + dir_x * 11.0f,
			y + 0.35f,
			z + dir_z * 11.0f,
			device_border_color
		);

		d3dg->Renderline(
			x + dir_x * 6.8f,
			y + 0.55f,
			z + dir_z * 6.8f,
			x + dir_x * 12.0f,
			y + 0.20f,
			z + dir_z * 12.0f,
			device_dark_color
		);
	}

	const int RING_COUNT = 48;
	float radius = PLAYER_SKILL_BIOTIC_FIELD_RADIUS;
	float spin = (float)(time % 120) * 0.015f;
	float pi2 = (float)M_PI * 2.0f;

	for (int i = 0; i < RING_COUNT; i++) {
		float a1 = pi2 * (float)i / (float)RING_COUNT + spin;
		float a2 = pi2 * (float)(i + 1) / (float)RING_COUNT + spin;

		if ((i % 3) == 2) {
			continue;
		}

		d3dg->Renderline(
			x + cosf(a1) * radius,
			y + 0.35f,
			z + sinf(a1) * radius,
			x + cosf(a2) * radius,
			y + 0.35f,
			z + sinf(a2) * radius,
			border_color
		);
	}

	// 참고 이미지처럼 중심에서 위로 올라가는 회복 기둥 5개
	const int PILLAR_COUNT = 5;

	for (int i = 0; i < PILLAR_COUNT; i++) {
		float offset = (float)(i - 2) * 4.2f;
		float height_rate = 1.0f;

		if ((i == 0) || (i == 4)) {
			height_rate = 0.55f;
		}
		else if ((i == 1) || (i == 3)) {
			height_rate = 0.78f;
		}

		float pulse = 0.0f;
		if (((time + i * 7) % 30) < 15) {
			pulse = 1.2f;
		}

		d3dg->Renderline(
			x + offset,
			y + 2.0f,
			z,
			x + offset,
			y + 2.0f + HUMAN_HEIGHT * height_rate + pulse,
			z,
			strong_color
		);

		d3dg->Renderline(
			x + offset + 0.8f,
			y + 2.0f,
			z,
			x + offset + 0.8f,
			y + 2.0f + HUMAN_HEIGHT * height_rate + pulse,
			z,
			weak_color
		);
	}

	// 범위 안에 부드럽게 떠오르는 작은 회복선
	const float dir_x[16] = {
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f,
		0.0000f, 0.3827f, 0.7071f, 0.9239f
	};

	const float dir_z[16] = {
		0.0000f, 0.3827f, 0.7071f, 0.9239f,
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f
	};

	for (int i = 0; i < 18; i++) {
		int seed = i * 41 + (time / 3) * 17;
		int dir = seed & 15;

		float particle_radius =
			6.0f + (float)((seed / 5) % 18) *
			(PLAYER_SKILL_BIOTIC_FIELD_RADIUS - 6.0f) / 18.0f;

		float sx = x + dir_x[dir] * particle_radius;
		float sz = z + dir_z[dir] * particle_radius;

		float phase = (float)((time + i * 9) % 45) / 45.0f;
		float sy = y + 0.45f + phase * 8.0f;

		int side_dir = (dir + 3 + (i % 4)) & 15;

		d3dg->Renderline(
			sx,
			sy,
			sz,
			sx + dir_x[side_dir] * 0.7f,
			sy + 1.8f,
			sz + dir_z[side_dir] * 0.7f,
			weak_color
		);
	}
}

void maingame::RenderPlayerArtilleryStrike()
{
	if (player_skill_artillery_strike_timer <= 0) { return; }
	if (Camera_Debugmode == true) { return; }

	d3dg->ResetWorldTransform();

	// 폭격 범위도 설치형 바닥 효과처럼 벽/지형 너머로 완전히 뚫려 보이지 않게 한다.
	float x = player_skill_artillery_strike_x;
	float y = player_skill_artillery_strike_y;
	float z = player_skill_artillery_strike_z;

	float fade = 1.0f;

	if (player_skill_artillery_strike_timer < (GAMEFPS / 2)) {
		fade = (float)player_skill_artillery_strike_timer / (float)(GAMEFPS / 2);
	}

	int elapsed = PLAYER_SKILL_ARTILLERY_STRIKE_FRAMES - player_skill_artillery_strike_timer;
	if (elapsed < (GAMEFPS / 3)) {
		float fade_in = (float)elapsed / (float)(GAMEFPS / 3);
		if (fade_in < fade) {
			fade = fade_in;
		}
	}

	if (fade < 0.0f) { fade = 0.0f; }
	if (fade > 1.0f) { fade = 1.0f; }

	int fill_color = d3dg->GetColorCode(1.0f, 0.20f, 0.04f, 0.14f * fade);
	int border_color = d3dg->GetColorCode(1.0f, 0.55f, 0.08f, 1.0f * fade);
	int weak_color = d3dg->GetColorCode(1.0f, 0.38f, 0.04f, 0.62f * fade);
	int strong_color = d3dg->GetColorCode(1.0f, 0.80f, 0.10f, 1.0f * fade);

	RenderSkillTargetCylinder(
		x,
		y,
		z,
		PLAYER_SKILL_ARTILLERY_STRIKE_RADIUS,
		0.55f,
		fill_color,
		border_color
	);

	// 중심 경고 십자
	d3dg->Renderline(
		x - 8.0f,
		y + 0.9f,
		z,
		x + 8.0f,
		y + 0.9f,
		z,
		strong_color
	);

	d3dg->Renderline(
		x,
		y + 0.9f,
		z - 8.0f,
		x,
		y + 0.9f,
		z + 8.0f,
		strong_color
	);

	// 범위 안에 낙하 경고선을 많이 표시해서 멀리서도 폭격 지점이 보이게 한다.
	const int MARK_COUNT = 14;

	for (int i = 0; i < MARK_COUNT; i++) {
		float base_angle =
			((float)((i * 137 + time * 5) % 360) / 360.0f) *
			(float)M_PI * 2.0f;

		float pulse = (float)((time * 2 + i * 11) % 60) / 60.0f;
		float radius = PLAYER_SKILL_ARTILLERY_STRIKE_RADIUS * (0.14f + 0.82f * pulse);

		float sx = x + cosf(base_angle) * radius;
		float sz = z + sinf(base_angle) * radius;

		int line_color = weak_color;
		if ((i % 3) == 0) {
			line_color = strong_color;
		}

		float top_y = y + 38.0f + (float)(i % 4) * 6.0f;
		float bottom_y = y + 1.2f;

		d3dg->Renderline(
			sx,
			top_y,
			sz,
			sx,
			bottom_y,
			sz,
			line_color
		);

		// 사선 꼬리를 더 길게 해서 위에서 떨어지는 포탄 느낌을 강화한다.
		d3dg->Renderline(
			sx,
			top_y,
			sz,
			sx - cosf(base_angle) * 4.2f,
			top_y + 8.0f,
			sz - sinf(base_angle) * 4.2f,
			line_color
		);

		// 강한 선 일부에는 짧은 하단 잔광을 추가한다.
		if ((i % 3) == 0) {
			d3dg->Renderline(
				sx - cosf(base_angle) * 1.2f,
				bottom_y + 2.5f,
				sz - sinf(base_angle) * 1.2f,
				sx + cosf(base_angle) * 1.2f,
				bottom_y + 2.5f,
				sz + sinf(base_angle) * 1.2f,
				strong_color
			);
		}
	}

	// 원거리 식별용 폭격 연기 기둥.
	// AddEffect 연기가 멀리서 거의 안 보일 수 있으므로,
	// Renderline 기반의 큰 연기 실루엣을 폭격 지속 중 직접 그린다.
	int smoke_color = d3dg->GetColorCode(0.36f, 0.34f, 0.30f, 0.58f * fade);
	int smoke_dark_color = d3dg->GetColorCode(0.18f, 0.17f, 0.15f, 0.48f * fade);
	int smoke_hot_color = d3dg->GetColorCode(1.0f, 0.42f, 0.08f, 0.45f * fade);

	const int SMOKE_COLUMN_COUNT = 10;

	for (int i = 0; i < SMOKE_COLUMN_COUNT; i++) {
		float angle =
			((float)((i * 71 + time * 2) % 360) / 360.0f) *
			(float)M_PI * 2.0f;

		float radius_rate = (float)((i * 37 + time) % 100) / 100.0f;
		float radius = PLAYER_SKILL_ARTILLERY_STRIKE_RADIUS * (0.08f + 0.42f * radius_rate);

		float sx = x + cosf(angle) * radius;
		float sz = z + sinf(angle) * radius;

		float sway_angle = angle + (float)M_PI * 0.5f;
		float sway = 3.0f + (float)(i % 4) * 1.5f;

		float mid_y = y + 18.0f + (float)(i % 3) * 6.0f;
		float top_y = y + 62.0f + (float)(i % 5) * 7.0f;

		int color = smoke_color;
		if ((i % 3) == 0) {
			color = smoke_dark_color;
		}

		// 큰 수직 연기 줄기
		d3dg->Renderline(
			sx,
			y + 3.0f,
			sz,
			sx + cosf(sway_angle) * sway,
			top_y,
			sz + sinf(sway_angle) * sway,
			color
		);

		// 중간에 퍼지는 연기 가지
		d3dg->Renderline(
			sx - cosf(angle) * 4.0f,
			mid_y,
			sz - sinf(angle) * 4.0f,
			sx + cosf(angle) * 7.0f,
			mid_y + 8.0f,
			sz + sinf(angle) * 7.0f,
			color
		);

		// 일부는 탄착 화염 잔광도 표시한다.
		if ((i % 4) == 0) {
			d3dg->Renderline(
				sx - 3.0f,
				y + 2.0f,
				sz,
				sx + 3.0f,
				y + 2.0f,
				sz,
				smoke_hot_color
			);
		}
	}
}

void maingame::RenderPlayerDodgeTrail()
{
	if (player_skill_dodge_trail_timer <= 0) { return; }
	if (Camera_Debugmode == true) { return; }

	d3dg->ResetWorldTransform();

	float rate = (float)player_skill_dodge_trail_timer / 8.0f;
	if (rate < 0.0f) { rate = 0.0f; }
	if (rate > 1.0f) { rate = 1.0f; }

	float alpha_main = 0.75f * rate;
	float alpha_sub = 0.38f * rate;

	int main_color = d3dg->GetColorCode(0.55f, 0.90f, 1.0f, alpha_main);
	int sub_color = d3dg->GetColorCode(0.20f, 0.55f, 1.0f, alpha_sub);

	float sx = player_skill_dodge_trail_start_x;
	float sy = player_skill_dodge_trail_start_y;
	float sz = player_skill_dodge_trail_start_z;

	float ex = player_skill_dodge_trail_end_x;
	float ey = player_skill_dodge_trail_end_y;
	float ez = player_skill_dodge_trail_end_z;

	// 이동 방향
	float vx = ex - sx;
	float vz = ez - sz;
	float len = sqrtf(vx * vx + vz * vz);

	float side_x = 0.0f;
	float side_z = 0.0f;

	if (len > 0.001f) {
		vx /= len;
		vz /= len;

		side_x = -vz;
		side_z = vx;
	}

	// 중심 잔상선
	d3dg->Renderline(
		sx,
		sy + HUMAN_HEIGHT * 0.35f,
		sz,
		ex,
		ey + HUMAN_HEIGHT * 0.35f,
		ez,
		main_color
	);

	// 어깨 높이 잔상선
	d3dg->Renderline(
		sx,
		sy + HUMAN_HEIGHT * 0.62f,
		sz,
		ex,
		ey + HUMAN_HEIGHT * 0.62f,
		ez,
		sub_color
	);

	// 좌우로 살짝 퍼진 잔상선
	float side_width = 1.2f;

	d3dg->Renderline(
		sx + side_x * side_width,
		sy + HUMAN_HEIGHT * 0.45f,
		sz + side_z * side_width,
		ex + side_x * side_width,
		ey + HUMAN_HEIGHT * 0.45f,
		ez + side_z * side_width,
		sub_color
	);

	d3dg->Renderline(
		sx - side_x * side_width,
		sy + HUMAN_HEIGHT * 0.45f,
		sz - side_z * side_width,
		ex - side_x * side_width,
		ey + HUMAN_HEIGHT * 0.45f,
		ez - side_z * side_width,
		sub_color
	);

	// 도착 지점에 짧은 상승선 몇 개를 추가해서 순간 이동 느낌을 준다.
	for (int i = 0; i < 4; i++) {
		float angle = ((float)i / 4.0f) * (float)M_PI * 2.0f + (float)(time % 8) * 0.25f;
		float ox = cosf(angle) * 1.4f;
		float oz = sinf(angle) * 1.4f;

		d3dg->Renderline(
			ex + ox,
			ey + 0.5f,
			ez + oz,
			ex + ox * 0.45f,
			ey + HUMAN_HEIGHT * 0.55f,
			ez + oz * 0.45f,
			sub_color
		);
	}
}

// 용병 전투 흡수: 지속 중 플레이어 주변에 검보라색 흡수막 표시
void maingame::RenderPlayerBattleAbsorbEffect(human* myHuman)
{
	if (myHuman == NULL) { return; }
	if (myHuman->GetSkillAbsorbFlag() == false) { return; }
	if (Camera_Debugmode == true) { return; }
	if (myHuman->GetHP() <= 0) { return; }

	float x, y, z, rx;
	myHuman->GetPosData(&x, &y, &z, &rx);

	d3dg->ResetWorldTransform();

	// ObjMgr.Render() 이후에 호출되므로 캐릭터 주변 효과가 몸에 묻히지 않게 한다.
	d3dg->ResetZbuffer();

	int timer = myHuman->GetSkillAbsorbTimer();
	float fade = 1.0f;

	// 검보라 느낌은 유지하되, 실제 게임 화면에서 보이도록 보라 명도와 알파를 올린다.
	int dark_color = d3dg->GetColorCode(0.30f, 0.03f, 0.48f, 0.95f * fade);
	int purple_color = d3dg->GetColorCode(0.62f, 0.12f, 0.95f, 0.82f * fade);
	int weak_color = d3dg->GetColorCode(0.42f, 0.05f, 0.72f, 0.58f * fade);

	const float dir_x[16] = {
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f,
		0.0000f, 0.3827f, 0.7071f, 0.9239f
	};

	const float dir_z[16] = {
		0.0000f, 0.3827f, 0.7071f, 0.9239f,
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f
	};

	int base_time = time;

	// 전투 흡수는 어두운 색이라 잘 묻히므로 전투 강화보다 조금 많게 표시한다.
// 짧은 검보라 균열선이 몸 주변에 깜빡이는 느낌이다.
	const int PARTICLE_COUNT = 22;

	for (int i = 0; i < PARTICLE_COUNT; i++) {
		int seed = i * 53 + (base_time / 4) * 19;
		int dir = seed & 15;

		float phase = (float)((base_time + i * 11) % 36) / 36.0f;

		// 몸에 너무 붙으면 캐릭터에 묻혀 안 보이므로 약간 바깥쪽으로 넓힌다.
		float radius = 1.35f + (float)((seed / 5) % 8) * 0.18f;

		float start_y = y + HUMAN_HEIGHT * (0.10f + phase * 0.90f);

		// 높이를 올리되 격노처럼 기둥처럼 솟지 않도록 선 길이는 적당히 유지한다.
		float line_len = 0.85f + (float)((seed / 7) % 5) * 0.22f;
		float end_y = start_y + line_len;

		if (end_y > y + HUMAN_HEIGHT * 1.15f) {
			end_y = y + HUMAN_HEIGHT * 1.15f;
		}

		float sx = x + dir_x[dir] * radius;
		float sz = z + dir_z[dir] * radius;

		// 선이 몸을 감싸듯 옆으로 짧게 꺾이게 한다.
		int side_dir = (dir + 4 + (i % 3)) & 15;

		float ex = sx + dir_x[side_dir] * 0.72f;
		float ez = sz + dir_z[side_dir] * 0.72f;

		int color = purple_color;
		if ((i % 3) == 0) {
			color = dark_color;
		}
		else if ((i % 3) == 1) {
			color = weak_color;
		}

		d3dg->Renderline(
			sx, start_y, sz,
			ex, end_y, ez,
			color
		);

		// 일부는 안쪽으로 말려드는 짧은 선을 추가해 흡수막 느낌을 준다.
		if ((i % 4) == 0) {
			float inner_radius = radius * 0.72f;
			float ix = x + dir_x[dir] * inner_radius;
			float iz = z + dir_z[dir] * inner_radius;

			d3dg->Renderline(
				sx, start_y, sz,
				ix, start_y + 0.12f, iz,
				weak_color
			);
		}
	}

	// 몸 주변을 감싸는 얇은 보호막/막 느낌의 짧은 가로선
	for (int j = 0; j < 7; j++) {
		int dir = ((base_time / 6) + j * 3) & 15;
		int end_dir = (dir + 2) & 15;

		float ring_y = y + HUMAN_HEIGHT * (0.14f + 0.15f * (float)j);
		float radius = 1.55f + (float)j * 0.12f;

		float sx = x + dir_x[dir] * radius;
		float sz = z + dir_z[dir] * radius;

		float ex = sx + dir_x[end_dir] * 0.95f;
		float ez = sz + dir_z[end_dir] * 0.95f;

		int ring_color = weak_color;
		if ((j % 2) == 0) {
			ring_color = purple_color;
		}

		d3dg->Renderline(
			sx, ring_y, sz,
			ex, ring_y + 0.08f, ez,
			ring_color
		);
	}
}

// 좀비 격노: 플레이어 주변 붉은 상승 파티클 표시
void maingame::RenderPlayerRageAura(human* myHuman)
{
	if (myHuman == NULL) { return; }
	if (myHuman->GetSkillRageFlag() == false) { return; }
	if (Camera_Debugmode == true) { return; }
	if (myHuman->GetHP() <= 0) { return; }

	float x, y, z, rx;
	myHuman->GetPosData(&x, &y, &z, &rx);

	d3dg->ResetWorldTransform();

	// ObjMgr.Render() 이후에 호출되므로, 캐릭터 주변 효과가 몸에 가려지지 않도록 한다.
	// 벽 너머로 너무 과하게 보이면 이 줄은 제거해도 된다.
	d3dg->ResetZbuffer();

	int strong_color = d3dg->GetColorCode(1.0f, 0.02f, 0.00f, 0.88f);
	int weak_color = d3dg->GetColorCode(1.0f, 0.22f, 0.04f, 0.58f);
	int trail_color = d3dg->GetColorCode(1.0f, 0.05f, 0.00f, 0.34f);

	const float dir_x[16] = {
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f,
		0.0000f, 0.3827f, 0.7071f, 0.9239f
	};

	const float dir_z[16] = {
		0.0000f, 0.3827f, 0.7071f, 0.9239f,
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f
	};

	int base_time = time;

	// 너무 많은 선을 그리면 지저분해지므로 우선 18개 정도만 사용한다.
	const int PARTICLE_COUNT = 18;

	for (int i = 0; i < PARTICLE_COUNT; i++) {
		int seed = i * 37 + (base_time / 2) * 13;
		int dir = seed & 15;

		float phase = (float)((base_time * 2 + i * 7) % 48) / 48.0f;
		float radius = 1.4f + (float)((seed / 3) % 12) * 0.18f;

		float start_y = y + 0.4f + phase * (HUMAN_HEIGHT * 0.78f);
		float line_len = 2.2f + (float)((seed / 5) % 7) * 0.35f;
		float end_y = start_y + line_len;

		if (end_y > y + HUMAN_HEIGHT * 0.95f) {
			end_y = y + HUMAN_HEIGHT * 0.95f;
		}

		float sx = x + dir_x[dir] * radius;
		float sz = z + dir_z[dir] * radius;

		int sway_dir = (dir + 1 + (i % 3)) & 15;
		float ex = sx + dir_x[sway_dir] * 0.35f;
		float ez = sz + dir_z[sway_dir] * 0.35f;

		int color = weak_color;
		if ((i & 1) == 0) {
			color = strong_color;
		}

		d3dg->Renderline(
			sx, start_y, sz,
			ex, end_y, ez,
			color
		);

		// 일부 파티클은 발밑에서 위로 끌려 올라가는 짧은 꼬리를 추가한다.
		if ((i % 3) == 0) {
			float br = radius * 0.70f;
			float bx = x + dir_x[dir] * br;
			float bz = z + dir_z[dir] * br;

			d3dg->Renderline(
				bx, y + 0.25f, bz,
				sx, start_y, sz,
				trail_color
			);
		}
	}

	// 어깨 높이 주변에도 몇 개의 짧은 불꽃선을 추가해서 3인칭에서 더 잘 보이게 한다.
	for (int j = 0; j < 4; j++) {
		int dir = ((base_time / 3) + j * 4) & 15;

		float shoulder_y = y + HUMAN_HEIGHT * 0.45f + (float)((base_time + j) % 3) * 0.35f;
		float radius = 2.0f + (float)j * 0.25f;

		float sx = x + dir_x[dir] * radius;
		float sz = z + dir_z[dir] * radius;

		int end_dir = (dir + 2) & 15;
		float ex = sx + dir_x[end_dir] * 0.45f;
		float ez = sz + dir_z[end_dir] * 0.45f;

		d3dg->Renderline(
			sx, shoulder_y, sz,
			ex, shoulder_y + 1.8f, ez,
			weak_color
		);
	}
}

// 군인 전투 강화: 지속 중 플레이어 주변에 약한 푸른 입자 표시
void maingame::RenderPlayerBattleBoostEffect(human* myHuman)
{
	if (myHuman == NULL) { return; }
	if (myHuman->GetSkillAmmoBoostFlag() == false) { return; }
	if (Camera_Debugmode == true) { return; }
	if (myHuman->GetHP() <= 0) { return; }

	float x, y, z, rx;
	myHuman->GetPosData(&x, &y, &z, &rx);

	d3dg->ResetWorldTransform();

	// 지속 파티클이므로 격노보다 밀도와 알파를 낮게 둔다.
	// ObjMgr.Render() 이후에 호출되므로 캐릭터 주변 선이 몸에 완전히 묻히지 않게 한다.
	d3dg->ResetZbuffer();

	int timer = myHuman->GetSkillAmmoBoostTimer();
	float fade = 1.0f;
	if (timer < (GAMEFPS / 2)) {
		fade = (float)timer / (float)(GAMEFPS / 2);
	}
	if (fade < 0.0f) { fade = 0.0f; }
	if (fade > 1.0f) { fade = 1.0f; }

	int strong_color = d3dg->GetColorCode(0.12f, 0.58f, 1.0f, 0.85f * fade);
	int weak_color = d3dg->GetColorCode(0.18f, 0.78f, 1.0f, 0.58f * fade);
	int trail_color = d3dg->GetColorCode(0.45f, 0.92f, 1.0f, 0.38f * fade);

	const float dir_x[16] = {
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f,
		0.0000f, 0.3827f, 0.7071f, 0.9239f
	};

	const float dir_z[16] = {
		0.0000f, 0.3827f, 0.7071f, 0.9239f,
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f
	};

	int base_time = time;

	// 격노 18개보다 적게 사용해서 화면을 가리지 않게 한다.
	const int PARTICLE_COUNT = 16;

	for (int i = 0; i < PARTICLE_COUNT; i++) {
		int seed = i * 41 + (base_time / 3) * 17;
		int dir = seed & 15;

		float phase = (float)((base_time + i * 9) % 40) / 40.0f;
		float radius = 1.35f + (float)((seed / 5) % 8) * 0.18f;

		float start_y = y + HUMAN_HEIGHT * (0.10f + phase * 0.82f);
		float line_len = 1.05f + (float)((seed / 7) % 5) * 0.24f;
		float end_y = start_y + line_len;

		if (end_y > y + HUMAN_HEIGHT * 1.05f) {
			end_y = y + HUMAN_HEIGHT * 1.05f;
		}

		float sx = x + dir_x[dir] * radius;
		float sz = z + dir_z[dir] * radius;

		// 위로 강하게 솟는 느낌을 피하고, 몸 주변에서 짧게 흐르는 선으로 보이게 한다.
		int side_dir = (dir + 4 + (i % 2)) & 15;
		float ex = sx + dir_x[side_dir] * 0.68f;
		float ez = sz + dir_z[side_dir] * 0.68f;

		int color = weak_color;
		if ((i % 3) == 0) {
			color = strong_color;
		}

		d3dg->Renderline(
			sx, start_y, sz,
			ex, end_y, ez,
			color
		);

		// 일부는 아주 짧은 꼬리만 추가한다. 격노처럼 길게 끌어올리지 않는다.
		if ((i % 4) == 0) {
			float br = radius * 0.82f;
			float bx = x + dir_x[dir] * br;
			float bz = z + dir_z[dir] * br;

			d3dg->Renderline(
				bx, start_y - 0.25f, bz,
				sx, start_y, sz,
				trail_color
			);
		}
	}

	// 몸 주변을 감싸는 짧은 가로선을 조금 더 늘려 3인칭에서도 보이게 한다.
	for (int j = 0; j < 5; j++) {
		int dir = ((base_time / 5) + j * 5) & 15;
		int end_dir = (dir + 2) & 15;

		float ring_y = y + HUMAN_HEIGHT * (0.18f + 0.16f * (float)j);
		float radius = 1.85f + (float)j * 0.13f;

		float sx = x + dir_x[dir] * radius;
		float sz = z + dir_z[dir] * radius;
		float ex = sx + dir_x[end_dir] * 0.95f;
		float ez = sz + dir_z[end_dir] * 0.95f;

		d3dg->Renderline(
			sx, ring_y, sz,
			ex, ring_y + 0.12f, ez,
			weak_color
		);
	}
}

// 테러리스트 지뢰: 설치된 지뢰 위치 표시
void maingame::RenderPlayerTerroristMines()
{
	if (Camera_Debugmode == true) { return; }

	bool has_mine = false;

	for (int i = 0; i < MAX_TERRORIST_MINES; i++) {
		if (player_skill_terrorist_mine_active[i] == true) {
			has_mine = true;
			break;
		}
	}

	if (has_mine == false) { return; }

	d3dg->ResetWorldTransform();

	// 별도 모델링 없이 얇은 바닥 표식만 표시한다.
	// ResetZbuffer()는 쓰지 않아 벽/지형 너머로 보이지 않게 한다.
	for (int i = 0; i < MAX_TERRORIST_MINES; i++) {
		if (player_skill_terrorist_mine_active[i] == false) {
			continue;
		}

		float x = player_skill_terrorist_mine_x[i];
		float y = player_skill_terrorist_mine_y[i];
		float z = player_skill_terrorist_mine_z[i];
		float rx = player_skill_terrorist_mine_rx[i];

		int fill_color;
		int border_color;
		int core_color;

		if (player_skill_terrorist_mine_arm_timer[i] > 0) {
			// 비활성화/장전 중: 어두운 회색 + 푸른 테두리
			fill_color = d3dg->GetColorCode(0.04f, 0.04f, 0.05f, 0.35f);
			border_color = d3dg->GetColorCode(0.25f, 0.55f, 1.0f, 0.80f);
			core_color = d3dg->GetColorCode(0.45f, 0.75f, 1.0f, 0.85f);
		}
		else {
			// 활성화 완료: 어두운 본체 + 주황/노랑 테두리
			fill_color = d3dg->GetColorCode(0.05f, 0.05f, 0.05f, 0.45f);
			border_color = d3dg->GetColorCode(1.0f, 0.55f, 0.05f, 0.95f);
			core_color = d3dg->GetColorCode(1.0f, 0.85f, 0.15f, 1.0f);
		}

		// 낮고 작은 원통으로 지뢰 본체 느낌을 낸다.
		RenderSkillTargetCylinder(
			x,
			y,
			z,
			3.2f,
			1.4f,
			fill_color,
			border_color
		);

		// 지뢰 중심 십자 표시
		d3dg->Renderline(
			x - 3.8f,
			y + 1.8f,
			z,
			x + 3.8f,
			y + 1.8f,
			z,
			core_color
		);

		d3dg->Renderline(
			x,
			y + 1.8f,
			z - 3.8f,
			x,
			y + 1.8f,
			z + 3.8f,
			core_color
		);

		// 설치 방향 확인용 짧은 방향선
		float dir_x = cosf(rx);
		float dir_z = sinf(rx);

		d3dg->Renderline(
			x,
			y + 2.2f,
			z,
			x + dir_x * 5.0f,
			y + 2.2f,
			z + dir_z * 5.0f,
			border_color
		);
	}
}

// 테러리스트 교란 신호: 발동 지점 주변에 노랑/주황 전파 노이즈 파티클 표시
// 테러리스트 교란 신호: 발동 지점 주변에 노랑/주황 전파 노이즈 파티클 표시
void maingame::RenderDistractionSignalEffect()
{
	if (player_skill_distraction_timer <= 0) { return; }
	if (Camera_Debugmode == true) { return; }

	d3dg->ResetWorldTransform();

	// 설치형 장치는 실제 화면에서 벽/지형 너머로 보이지 않게 한다.
	// 따라서 ResetZbuffer()를 호출하지 않는다.
	float x = player_skill_distraction_x;
	float y = player_skill_distraction_y;
	float z = player_skill_distraction_z;

	float fade = 1.0f;
	if (player_skill_distraction_timer < (GAMEFPS / 2)) {
		fade = (float)player_skill_distraction_timer / (float)(GAMEFPS / 2);
	}

	if (fade < 0.0f) { fade = 0.0f; }
	if (fade > 1.0f) { fade = 1.0f; }

	// 설치 직후에는 파티클이 바로 전부 나오지 않고, 점점 빠르게 증가한다.
	int elapsed_frames = PLAYER_SKILL_DISTRACTION_FRAMES - player_skill_distraction_timer;
	if (elapsed_frames < 0) { elapsed_frames = 0; }

	const int PARTICLE_APPEAR_DELAY_FRAMES = 3;
	const int PARTICLE_WARMUP_FRAMES = (int)(0.75f * GAMEFPS);

	float appear_rate = 0.0f;

	if (elapsed_frames > PARTICLE_APPEAR_DELAY_FRAMES) {
		appear_rate =
			(float)(elapsed_frames - PARTICLE_APPEAR_DELAY_FRAMES) /
			(float)PARTICLE_WARMUP_FRAMES;
	}

	if (appear_rate < 0.0f) { appear_rate = 0.0f; }
	if (appear_rate > 1.0f) { appear_rate = 1.0f; }

	// 처음에는 천천히, 뒤로 갈수록 빠르게 생기게 한다.
	appear_rate = appear_rate * appear_rate;

	if (appear_rate <= 0.0f) {
		return;
	}

	float effect_alpha = fade * appear_rate;

	int strong_color = d3dg->GetColorCode(1.0f, 0.78f, 0.05f, 0.95f * effect_alpha);
	int weak_color = d3dg->GetColorCode(1.0f, 0.95f, 0.20f, 0.55f * effect_alpha);
	int noise_color = d3dg->GetColorCode(1.0f, 0.45f, 0.02f, 0.45f * effect_alpha);

	const float dir_x[16] = {
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f,
		0.0000f, 0.3827f, 0.7071f, 0.9239f
	};

	const float dir_z[16] = {
		0.0000f, 0.3827f, 0.7071f, 0.9239f,
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f
	};

	int base_time = time;

	// 짧은 신호 노이즈 선.
	// 지속 파티클이므로 너무 많지 않게 한다.
	const int PARTICLE_COUNT = 16;

	int visible_particle_count = (int)((float)PARTICLE_COUNT * appear_rate);
	if (visible_particle_count < 1) { visible_particle_count = 1; }
	if (visible_particle_count > PARTICLE_COUNT) { visible_particle_count = PARTICLE_COUNT; }

	// 초반에는 느리게, 이후에는 기존 속도에 가까워지게 한다.
	int anim_step_div = 6 - (int)(appear_rate * 3.0f);
	if (anim_step_div < 3) { anim_step_div = 3; }

	for (int i = 0; i < visible_particle_count; i++) {
		int seed = i * 43 + (base_time / anim_step_div) * 17;
		int dir = seed & 15;

		float phase = (float)((base_time + i * 7) % 36) / 36.0f;
		float radius = 3.0f + (float)((seed / 5) % 10) * 0.45f;

		float sx = x + dir_x[dir] * radius;
		float sz = z + dir_z[dir] * radius;

		// 신호기 주변 하단~중단에서 깜빡이는 느낌
		float sy = y + 0.4f + phase * 8.0f;

		int side_dir = (dir + 4 + (i % 3)) & 15;

		float len = 0.9f + (float)((seed / 7) % 5) * 0.22f;
		float ex = sx + dir_x[side_dir] * len;
		float ez = sz + dir_z[side_dir] * len;
		float ey = sy + 0.35f + (float)((seed / 11) % 3) * 0.25f;

		int color = weak_color;
		if ((i % 3) == 0) {
			color = strong_color;
		}
		else if ((i % 3) == 1) {
			color = noise_color;
		}

		d3dg->Renderline(
			sx, sy, sz,
			ex, ey, ez,
			color
		);
	}

	// 신호가 퍼지는 느낌의 낮은 원형 전파선.
	// 완전한 원을 그리기보다 일부 짧은 선만 그려서 HUD가 복잡해지지 않게 한다.
	int visible_ring_count = (int)(3.0f * appear_rate + 0.5f);
	if (visible_ring_count < 0) { visible_ring_count = 0; }
	if (visible_ring_count > 3) { visible_ring_count = 3; }

	for (int ring = 0; ring < visible_ring_count; ring++) {
		float ring_phase = (float)((base_time + ring * 12) % 48) / 48.0f;
		float radius = 4.0f + ring_phase * 12.0f + (float)ring * 1.5f;
		float ring_y = y + 0.35f + (float)ring * 0.25f;

		int ring_color = weak_color;
		if (ring == 0) {
			ring_color = strong_color;
		}

		for (int j = 0; j < 8; j += 2) {
			int dir1 = (j * 2 + ring + (base_time / 6)) & 15;
			int dir2 = (dir1 + 1) & 15;

			float sx = x + dir_x[dir1] * radius;
			float sz = z + dir_z[dir1] * radius;
			float ex = x + dir_x[dir2] * radius;
			float ez = z + dir_z[dir2] * radius;

			d3dg->Renderline(
				sx, ring_y, sz,
				ex, ring_y, ez,
				ring_color
			);
		}
	}

	// 중심부에 짧은 깜빡임 선을 추가한다.
	// 기존 RenderDistractionSignalMarker()의 긴 중심 기둥과 겹치지 않도록 짧게 둔다.
	int visible_core_count = (int)(4.0f * appear_rate + 0.5f);
	if (visible_core_count < 0) { visible_core_count = 0; }
	if (visible_core_count > 4) { visible_core_count = 4; }

	for (int k = 0; k < visible_core_count; k++) {
		int dir = ((base_time / 4) + k * 4) & 15;

		float sx = x + dir_x[dir] * 1.2f;
		float sz = z + dir_z[dir] * 1.2f;
		float ex = x + dir_x[(dir + 2) & 15] * 2.2f;
		float ez = z + dir_z[(dir + 2) & 15] * 2.2f;

		float sy = y + 1.5f + (float)k * 1.6f;

		d3dg->Renderline(
			sx, sy, sz,
			ex, sy + 0.4f, ez,
			strong_color
		);
	}
}

void maingame::RenderPlayerTeleportEffect()
{
	if ((player_skill_teleport_start_effect_timer <= 0) &&
		(player_skill_teleport_end_effect_timer <= 0)) {
		return;
	}

	if (Camera_Debugmode == true) { return; }

	d3dg->ResetWorldTransform();
	d3dg->ResetZbuffer();

	const float PI2 = (float)M_PI * 2.0f;

	const float dir_x[16] = {
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f,
		0.0000f, 0.3827f, 0.7071f, 0.9239f
	};

	const float dir_z[16] = {
		0.0000f, 0.3827f, 0.7071f, 0.9239f,
		1.0000f, 0.9239f, 0.7071f, 0.3827f,
		0.0000f, -0.3827f, -0.7071f, -0.9239f,
		-1.0000f, -0.9239f, -0.7071f, -0.3827f
	};

	int base_time = time;

	// 시작 위치: 격노처럼 긴 보라색 선이 위로 빨려 올라가는 느낌
	if (player_skill_teleport_start_effect_timer > 0) {
		float rate = (float)player_skill_teleport_start_effect_timer / (float)PLAYER_SKILL_TELEPORT_EFFECT_FRAMES;
		if (rate < 0.0f) { rate = 0.0f; }
		if (rate > 1.0f) { rate = 1.0f; }

		float x = player_skill_teleport_start_effect_x;
		float y = player_skill_teleport_start_effect_y;
		float z = player_skill_teleport_start_effect_z;

		int strong_color = d3dg->GetColorCode(0.68f, 0.28f, 1.0f, 0.90f * rate);
		int weak_color = d3dg->GetColorCode(0.68f, 0.28f, 1.0f, 0.58f * rate);
		int trail_color = d3dg->GetColorCode(0.68f, 0.28f, 1.0f, 0.34f * rate);
		int core_color = d3dg->GetColorCode(0.68f, 0.28f, 1.0f, 0.75f * rate);

		// 중심부 긴 빛
		d3dg->Renderline(
			x, y + 0.20f, z,
			x, y + HUMAN_HEIGHT * 1.05f, z,
			core_color
		);

		const int PARTICLE_COUNT = 24;

		for (int i = 0; i < PARTICLE_COUNT; i++) {
			int seed = i * 37 + (base_time / 2) * 13;
			int dir = seed & 15;

			float phase = (float)((base_time * 2 + i * 7) % 48) / 48.0f;

			// 기존보다 넓게: 격노와 비슷한 반경
			float radius = 1.3f + (float)((seed / 3) % 12) * 0.18f;

			float start_y = y + 0.35f + phase * (HUMAN_HEIGHT * 0.78f);

			// 기존보다 길게: 격노와 비슷한 긴 상승선
			float line_len = 2.3f + (float)((seed / 5) % 7) * 0.35f;
			float end_y = start_y + line_len;

			if (end_y > y + HUMAN_HEIGHT * 1.08f) {
				end_y = y + HUMAN_HEIGHT * 1.08f;
			}

			float sx = x + dir_x[dir] * radius;
			float sz = z + dir_z[dir] * radius;

			int sway_dir = (dir + 1 + (i % 3)) & 15;
			float ex = sx + dir_x[sway_dir] * 0.42f;
			float ez = sz + dir_z[sway_dir] * 0.42f;

			int color = weak_color;
			if ((i & 1) == 0) {
				color = strong_color;
			}

			d3dg->Renderline(
				sx, start_y, sz,
				ex, end_y, ez,
				color
			);

			// 일부 선은 발밑에서 위로 빨려 올라가는 꼬리를 추가한다.
			if ((i % 3) == 0) {
				float br = radius * 0.70f;
				float bx = x + dir_x[dir] * br;
				float bz = z + dir_z[dir] * br;

				d3dg->Renderline(
					bx, y + 0.25f, bz,
					sx, start_y, sz,
					trail_color
				);
			}

			// 일부 선은 중심으로 빨려 들어가는 느낌을 유지한다.
			if ((i % 5) == 0) {
				d3dg->Renderline(
					sx, start_y, sz,
					x, y + HUMAN_HEIGHT * 0.62f, z,
					trail_color
				);
			}
		}

		// 어깨 높이 주변에도 긴 보라색 선을 추가한다.
		for (int j = 0; j < 4; j++) {
			int dir = ((base_time / 3) + j * 4) & 15;

			float shoulder_y = y + HUMAN_HEIGHT * 0.45f + (float)((base_time + j) % 3) * 0.35f;
			float radius = 2.0f + (float)j * 0.30f;

			float sx = x + dir_x[dir] * radius;
			float sz = z + dir_z[dir] * radius;

			int end_dir = (dir + 2) & 15;
			float ex = sx + dir_x[end_dir] * 0.50f;
			float ez = sz + dir_z[end_dir] * 0.50f;

			d3dg->Renderline(
				sx, shoulder_y, sz,
				ex, shoulder_y + 1.9f, ez,
				weak_color
			);
		}
	}

	// 도착 위치: 바깥으로 터지는 효과 + 격노처럼 긴 상승선
	if (player_skill_teleport_end_effect_timer > 0) {
		float rate = (float)player_skill_teleport_end_effect_timer / (float)PLAYER_SKILL_TELEPORT_EFFECT_FRAMES;
		if (rate < 0.0f) { rate = 0.0f; }
		if (rate > 1.0f) { rate = 1.0f; }

		float spread = 1.0f - rate;

		float x = player_skill_teleport_end_effect_x;
		float y = player_skill_teleport_end_effect_y;
		float z = player_skill_teleport_end_effect_z;

		float alpha = 0.28f + 0.72f * rate;

		int strong_color = d3dg->GetColorCode(0.68f, 0.28f, 1.0f, 0.95f * alpha);
		int weak_color = d3dg->GetColorCode(0.68f, 0.28f, 1.0f, 0.62f * alpha);
		int trail_color = d3dg->GetColorCode(0.68f, 0.28f, 1.0f, 0.34f * alpha);
		int core_color = d3dg->GetColorCode(0.68f, 0.28f, 1.0f, 0.80f * rate);

		// 도착 순간 중심 섬광
		d3dg->Renderline(
			x, y + 0.15f, z,
			x, y + HUMAN_HEIGHT * 1.05f, z,
			core_color
		);

		const int BURST_COUNT = 32;
		const int RING_COUNT = 32;

		float base_radius = 0.45f + spread * 0.8f;
		float burst_radius = 1.2f + spread * 6.2f;

		// 기존 바깥 확산 효과 유지
		for (int i = 0; i < BURST_COUNT; i++) {
			int seed = i * 43 + base_time * 7;

			float angle = PI2 * (float)i / (float)BURST_COUNT;
			angle += (float)((seed % 17) - 8) * 0.015f;

			float add_radius = (float)((seed / 5) % 8) * 0.18f;
			float end_radius = burst_radius + add_radius;

			float sx = x + cosf(angle) * base_radius;
			float sz = z + sinf(angle) * base_radius;

			float ex = x + cosf(angle) * end_radius;
			float ez = z + sinf(angle) * end_radius;

			float sy = y + 0.25f + (float)((seed / 3) % 5) * 0.10f;
			float ey = y + 0.25f + spread * 0.55f + (float)((seed / 7) % 5) * 0.08f;

			int color = strong_color;
			if ((i % 4) == 0) {
				color = weak_color;
			}

			d3dg->Renderline(
				sx, sy, sz,
				ex, ey, ez,
				color
			);

			if ((i % 3) == 0) {
				d3dg->Renderline(
					ex, ey, ez,
					ex + cosf(angle) * 0.45f,
					ey + 0.08f,
					ez + sinf(angle) * 0.45f,
					weak_color
				);
			}
		}

		// 도착 위치에도 격노처럼 긴 상승선을 추가한다.
		const int RISE_COUNT = 18;

		for (int i = 0; i < RISE_COUNT; i++) {
			int seed = i * 37 + (base_time / 2) * 13;
			int dir = seed & 15;

			float phase = (float)((base_time * 2 + i * 7) % 48) / 48.0f;
			float radius = 1.4f + (float)((seed / 3) % 12) * 0.18f;

			float start_y = y + 0.35f + phase * (HUMAN_HEIGHT * 0.70f);

			float line_len = 2.1f + (float)((seed / 5) % 7) * 0.32f;
			float end_y = start_y + line_len;

			if (end_y > y + HUMAN_HEIGHT * 1.05f) {
				end_y = y + HUMAN_HEIGHT * 1.05f;
			}

			float sx = x + dir_x[dir] * radius;
			float sz = z + dir_z[dir] * radius;

			int sway_dir = (dir + 1 + (i % 3)) & 15;
			float ex = sx + dir_x[sway_dir] * 0.42f;
			float ez = sz + dir_z[sway_dir] * 0.42f;

			int color = weak_color;
			if ((i & 1) == 0) {
				color = strong_color;
			}

			d3dg->Renderline(
				sx, start_y, sz,
				ex, end_y, ez,
				color
			);

			if ((i % 3) == 0) {
				float br = radius * 0.70f;
				float bx = x + dir_x[dir] * br;
				float bz = z + dir_z[dir] * br;

				d3dg->Renderline(
					bx, y + 0.25f, bz,
					sx, start_y, sz,
					trail_color
				);
			}
		}

		// 바닥 충격파 링
		float ring_radius = 1.0f + spread * 5.8f;

		for (int i = 0; i < RING_COUNT; i++) {
			float a1 = PI2 * (float)i / (float)RING_COUNT;
			float a2 = PI2 * (float)(i + 1) / (float)RING_COUNT;

			int color = strong_color;
			if ((i % 2) == 0) {
				color = weak_color;
			}

			d3dg->Renderline(
				x + cosf(a1) * ring_radius,
				y + 0.18f,
				z + sinf(a1) * ring_radius,
				x + cosf(a2) * ring_radius,
				y + 0.18f,
				z + sinf(a2) * ring_radius,
				color
			);
		}
	}
}

void maingame::RenderPlayerTeleportCastPreview()
{
	if (player_skill_teleport_cast_timer <= 0) { return; }
	if (Camera_Debugmode == true) { return; }

	float tx = player_skill_teleport_x;
	float ty = player_skill_teleport_y;
	float tz = player_skill_teleport_z;

	if ((tx == 0.0f) &&
		(ty == 0.0f) &&
		(tz == 0.0f)) {
		return;
	}

	bool valid = IsPlayerSkillGroundTargetValid(PLAYER_SKILL_TELEPORT, tx, ty, tz);

	int fill_color;
	int border_color;

	if (valid == true) {
		// 텔레포트 시전 중에는 조준 원기둥과 구분되도록 보라색 계열로 표시한다.
		fill_color = d3dg->GetColorCode(0.45f, 0.35f, 1.0f, 0.24f);
		border_color = d3dg->GetColorCode(0.65f, 0.55f, 1.0f, 1.0f);
	}
	else {
		// 시전 중 목적지가 막히면 빨간색으로 바뀌고, 실제 이동 시점에서도 실패 처리된다.
		fill_color = d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 0.20f);
		border_color = d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f);
	}

	d3dg->ResetWorldTransform();
	d3dg->ResetZbuffer();

	RenderSkillTargetCylinder(
		tx,
		ty,
		tz,
		5.0f,
		GetPlayerSkillTargetPreviewHeight(PLAYER_SKILL_TELEPORT),
		fill_color,
		border_color
	);
}

