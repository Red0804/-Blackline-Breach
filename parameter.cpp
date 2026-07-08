//! @file parameter.cpp
//! @brief ParameterInfo긏깋긚궻믦?

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

// 릐갋븧딇갋룷븿 먠믦긢??궻둴봃띿떾
// ?Manda궠귪?걁2014봏12뙉걂

#include "parameter.h"

//! @brief 긓깛긚긣깋긏?
ParameterInfo::ParameterInfo()
{
	Human = NULL;
	for(int i=0; i<TOTAL_HUMANTEXTURE; i++){
		HumanTexturePath[i] = (char*)'\0';
	}
	Weapon = NULL;
#ifdef ENABLE_BUG_HUMANWEAPON
	BugWeapon = NULL;
#endif
	SmallObject = NULL;
	Bullet = NULL;
	MissionData = NULL;
	AIlevel = NULL;
	Scope = NULL;
}

//! @brief 긢귻긚긣깋긏?
ParameterInfo::~ParameterInfo()
{
	DestroyInfo();
}

//! @brief 룊딖돸걁긬깋긽??궻먠믦걂
//! @attention 궞궻듫릶귩뚁귂뢯궠궶궋궴갂긏깋긚렔뫬궕맫궢궘??궢귏궧귪갃
void ParameterInfo::InitInfo()
{
#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_INIT, "ParameterInfo", "");
#endif

	Human = new HumanParameter[TOTAL_PARAMETERINFO_HUMAN];
	Weapon = new WeaponParameter[TOTAL_PARAMETERINFO_WEAPON];
#ifdef ENABLE_BUG_HUMANWEAPON
	BugWeapon = new WeaponParameter[1];
#endif
	SmallObject = new SmallObjectParameter[TOTAL_PARAMETERINFO_SMALLOBJECT];

	for (int i = 0; i < TOTAL_PARAMETERINFO_HUMAN; i++) {
		for (int j = 0; j < TOTAL_HAVEWEAPON; j++) {
			Human[i].Weapon[j] = ID_WEAPON_NONE;
		}
	}
	Bullet = new BulletParameter[TOTAL_PARAMETERINFO_BULLET];
	MissionData = new OfficialMissionParameter[TOTAL_OFFICIALMISSION];
	AIlevel = new AIParameter[TOTAL_PARAMETERINFO_AILEVEL];
	Scope = new ScopeParameter[TOTAL_PARAMETERINFO_SCOPE];

	//벫롪 뜒 A
	Human[0].texture = 14;
	Human[0].model = 5;
	Human[0].hp = 120;
	Human[0].AIlevel = 3;
	Human[0].AItrackability = -1;
	Human[0].Weapon[0] = 21;
	Human[0].Weapon[1] = 8;
	Human[0].type = 0;
	//벫롪 뜒 B
	Human[1].texture = 14;
	Human[1].model = 5;
	Human[1].hp = 120;
	Human[1].AIlevel = 3;
	Human[1].AItrackability = -1;
	Human[1].Weapon[0] = 21;
	Human[1].Weapon[1] = 2;
	Human[1].type = 0;
	//벫롪 쀎 A
	Human[2].texture = 16;
	Human[2].model = 0;
	Human[2].hp = 110;
	Human[2].AIlevel = 2;
	Human[2].AItrackability = -1;
	Human[2].Weapon[0] = 3;
	Human[2].Weapon[1] = 6;
	Human[2].type = 0;
	//벫롪 쀎 B
	Human[3].texture = 16;
	Human[3].model = 0;
	Human[3].hp = 110;
	Human[3].AIlevel = 2;
	Human[3].AItrackability = -1;
	Human[3].Weapon[0] = 3;
	Human[3].Weapon[1] = 10;
	Human[3].type = 0;
	//벫롪 쀎 C
	Human[4].texture = 16;
	Human[4].model = 0;
	Human[4].hp = 110;
	Human[4].AIlevel = 2;
	Human[4].AItrackability = -1;
	Human[4].Weapon[0] = 3;
	Human[4].Weapon[1] = 2;
	Human[4].type = 0;
	//벫롪 뵏 A
	Human[5].texture = 18;
	Human[5].model = 0;
	Human[5].hp = 110;
	Human[5].AIlevel = 2;
	Human[5].AItrackability = -1;
	Human[5].Weapon[0] = 4;
	Human[5].Weapon[1] = 7;
	Human[5].type = 0;
	//벫롪 뵏 B
	Human[6].texture = 18;
	Human[6].model = 0;
	Human[6].hp = 110;
	Human[6].AIlevel = 2;
	Human[6].AItrackability = -1;
	Human[6].Weapon[0] = 4;
	Human[6].Weapon[1] = 2;
	Human[6].type = 0;
	//긪긒
	Human[7].texture = 5;
	Human[7].model = 0;
	Human[7].hp = 100;
	Human[7].AIlevel = 3;
	Human[7].AItrackability = -1;
	Human[7].Weapon[0] = 4;
	Human[7].Weapon[1] = 0;
	Human[7].type = 0;
	//벫롪 럤
	Human[8].texture = 17;
	Human[8].model = 0;
	Human[8].hp = 110;
	Human[8].AIlevel = 3;
	Human[8].AItrackability = -1;
	Human[8].Weapon[0] = 22;
	Human[8].Weapon[1] = 14;
	Human[8].type = 0;
	//벫롪 먃
	Human[9].texture = 15;
	Human[9].model = 5;
	Human[9].hp = 120;
	Human[9].AIlevel = 2;
	Human[9].AItrackability = -1;
	Human[9].Weapon[0] = 3;
	Human[9].Weapon[1] = 1;
	Human[9].type = 0;
	//먰벉뾭깓?긞긣
	Human[10].texture = 13;
	Human[10].model = 0;
	Human[10].hp = 2500;
	Human[10].AIlevel = 4;
	Human[10].AItrackability = -1;
	Human[10].Weapon[0] = 6;
	Human[10].Weapon[1] = 12;
	Human[10].type = 1;
	//긚?긟 뜒 SG
	Human[11].texture = 4;
	Human[11].model = 1;
	Human[11].hp = 100;
	Human[11].AIlevel = 1;
	Human[11].AItrackability = -1;
	Human[11].Weapon[0] = 18;
	Human[11].Weapon[1] = 0;
	Human[11].type = 0;
	//긚?긟 뜒
	Human[12].texture = 4;
	Human[12].model = 0;
	Human[12].hp = 100;
	Human[12].AIlevel = 1;
	Human[12].AItrackability = -1;
	Human[12].Weapon[0] = 5;
	Human[12].Weapon[1] = 0;
	Human[12].type = 0;
	//긚?긟 둈
	Human[13].texture = 11;
	Human[13].model = 0;
	Human[13].hp = 100;
	Human[13].AIlevel = 0;
	Human[13].AItrackability = -1;
	Human[13].Weapon[0] = 0;
	Human[13].Weapon[1] = 0;
	Human[13].type = 0;
	//똸?뒸
	Human[14].texture = 8;
	Human[14].model = 2;
	Human[14].hp = 100;
	Human[14].AIlevel = 1;
	Human[14].AItrackability = -1;
	Human[14].Weapon[0] = 3;
	Human[14].Weapon[1] = 0;
	Human[14].type = 0;
	//긚?긟 뭴
	Human[15].texture = 9;
	Human[15].model = 0;
	Human[15].hp = 100;
	Human[15].AIlevel = 0;
	Human[15].AItrackability = -1;
	Human[15].Weapon[0] = 0;
	Human[15].Weapon[1] = 0;
	Human[15].type = 0;
	//긘긿긟뭞 1
	Human[16].texture = 23;
	Human[16].model = 0;
	Human[16].hp = 100;
	Human[16].AIlevel = 0;
	Human[16].AItrackability = -1;
	Human[16].Weapon[0] = 17;
	Human[16].Weapon[1] = 0;
	Human[16].type = 0;
	//뭷뱦빜
	Human[17].texture = 6;
	Human[17].model = 0;
	Human[17].hp = 100;
	Human[17].AIlevel = 1;
	Human[17].AItrackability = -1;
	Human[17].Weapon[0] = 17;
	Human[17].Weapon[1] = 10;
	Human[17].type = 0;
	//룛
	Human[18].texture = 25;
	Human[18].model = 3;
	Human[18].hp = 90;
	Human[18].AIlevel = 0;
	Human[18].AItrackability = -1;
	Human[18].Weapon[0] = 0;
	Human[18].Weapon[1] = 0;
	Human[18].type = 0;
	//뗠뵱뭞
	Human[19].texture = 0;
	Human[19].model = 0;
	Human[19].hp = 100;
	Human[19].AIlevel = 0;
	Human[19].AItrackability = -1;
	Human[19].Weapon[0] = 0;
	Human[19].Weapon[1] = 0;
	Human[19].type = 0;
	//럖뼬 1
	Human[20].texture = 1;
	Human[20].model = 0;
	Human[20].hp = 100;
	Human[20].AIlevel = 0;
	Human[20].AItrackability = -1;
	Human[20].Weapon[0] = 0;
	Human[20].Weapon[1] = 0;
	Human[20].type = 0;
	//럖뼬 2
	Human[21].texture = 2;
	Human[21].model = 0;
	Human[21].hp = 100;
	Human[21].AIlevel = 0;
	Human[21].AItrackability = -1;
	Human[21].Weapon[0] = 0;
	Human[21].Weapon[1] = 0;
	Human[21].type = 0;
	//긘긿긟뭞 1 SG
	Human[22].texture = 23;
	Human[22].model = 1;
	Human[22].hp = 100;
	Human[22].AIlevel = 0;
	Human[22].AItrackability = -1;
	Human[22].Weapon[0] = 0;
	Human[22].Weapon[1] = 0;
	Human[22].type = 0;
	//뗠뵱뭞 SG
	Human[23].texture = 0;
	Human[23].model = 1;
	Human[23].hp = 100;
	Human[23].AIlevel = 0;
	Human[23].AItrackability = -1;
	Human[23].Weapon[0] = 0;
	Human[23].Weapon[1] = 0;
	Human[23].type = 0;
	//럖뼬 1 SG
	Human[24].texture = 1;
	Human[24].model = 1;
	Human[24].hp = 100;
	Human[24].AIlevel = 0;
	Human[24].AItrackability = -1;
	Human[24].Weapon[0] = 0;
	Human[24].Weapon[1] = 0;
	Human[24].type = 0;
	//럖뼬 2 SG
	Human[25].texture = 2;
	Human[25].model = 1;
	Human[25].hp = 100;
	Human[25].AIlevel = 0;
	Human[25].AItrackability = -1;
	Human[25].Weapon[0] = 0;
	Human[25].Weapon[1] = 0;
	Human[25].type = 0;
	//빜럐 1 A
	Human[26].texture = 19;
	Human[26].model = 4;
	Human[26].hp = 115;
	Human[26].AIlevel = 2;
	Human[26].AItrackability = -1;
	Human[26].Weapon[0] = 5;
	Human[26].Weapon[1] = 9;
	Human[26].type = 0;
	//빜럐 1 B
	Human[27].texture = 20;
	Human[27].model = 4;
	Human[27].hp = 115;
	Human[27].AIlevel = 2;
	Human[27].AItrackability = -1;
	Human[27].Weapon[0] = 5;
	Human[27].Weapon[1] = 12;
	Human[27].type = 0;
	//빜럐 2
	Human[28].texture = 21;
	Human[28].model = 4;
	Human[28].hp = 115;
	Human[28].AIlevel = 2;
	Human[28].AItrackability = -1;
	Human[28].Weapon[0] = 13;
	Human[28].Weapon[1] = 10;
	Human[28].type = 0;
	//?깛긮 1
	Human[29].texture = 26;
	Human[29].model = 0;
	Human[29].hp = 1000;
	Human[29].AIlevel = 0;
	Human[29].AItrackability = 1;
	Human[29].Weapon[0] = 0;
	Human[29].Weapon[1] = 0;
	Human[29].type = 2;
	//?깛긮 2
	Human[30].texture = 27;
	Human[30].model = 0;
	Human[30].hp = 1000;
	Human[30].AIlevel = 0;
	Human[30].AItrackability = 3;
	Human[30].Weapon[0] = 0;
	Human[30].Weapon[1] = 0;
	Human[30].type = 2;
	//?깛긮 3
	Human[31].texture = 28;
	Human[31].model = 0;
	Human[31].hp = 1000;
	Human[31].AIlevel = 0;
	Human[31].AItrackability = 0;
	Human[31].Weapon[0] = 0;
	Human[31].Weapon[1] = 0;
	Human[31].type = 2;
	//?깛긮 4
	Human[32].texture = 29;
	Human[32].model = 3;
	Human[32].hp = 950;
	Human[32].AIlevel = 0;
	Human[32].AItrackability = 2;
	Human[32].Weapon[0] = 0;
	Human[32].Weapon[1] = 0;
	Human[32].type = 2;
	//긚?긟 뜮
	Human[33].texture = 12;
	Human[33].model = 0;
	Human[33].hp = 100;
	Human[33].AIlevel = 0;
	Human[33].AItrackability = -1;
	Human[33].Weapon[0] = 0;
	Human[33].Weapon[1] = 0;
	Human[33].type = 0;
	//긚?긟 뜮 SG
	Human[34].texture = 12;
	Human[34].model = 1;
	Human[34].hp = 100;
	Human[34].AIlevel = 1;
	Human[34].AItrackability = -1;
	Human[34].Weapon[0] = 17;
	Human[34].Weapon[1] = 0;
	Human[34].type = 0;
	//룶똓
	Human[35].texture = 7;
	Human[35].model = 0;
	Human[35].hp = 95;
	Human[35].AIlevel = 1;
	Human[35].AItrackability = -1;
	Human[35].Weapon[0] = 17;
	Human[35].Weapon[1] = 0;
	Human[35].type = 0;
	//긚?긟 먃
	Human[36].texture = 10;
	Human[36].model = 0;
	Human[36].hp = 100;
	Human[36].AIlevel = 0;
	Human[36].AItrackability = -1;
	Human[36].Weapon[0] = 0;
	Human[36].Weapon[1] = 0;
	Human[36].type = 0;
	//긚?긟 먃 SG
	Human[37].texture = 10;
	Human[37].model = 1;
	Human[37].hp = 100;
	Human[37].AIlevel = 1;
	Human[37].AItrackability = -1;
	Human[37].Weapon[0] = 18;
	Human[37].Weapon[1] = 0;
	Human[37].type = 0;
	//긘긿긟뭞 2 SG
	Human[38].texture = 24;
	Human[38].model = 1;
	Human[38].hp = 100;
	Human[38].AIlevel = 1;
	Human[38].AItrackability = -1;
	Human[38].Weapon[0] = 6;
	Human[38].Weapon[1] = 0;
	Human[38].type = 0;
	//빜럐 3
	Human[39].texture = 22;
	Human[39].model = 0;
	Human[39].hp = 105;
	Human[39].AIlevel = 3;
	Human[39].AItrackability = -1;
	Human[39].Weapon[0] = 5;
	Human[39].Weapon[1] = 20;
	Human[39].type = 0;
	//빜럐 3 SG
	Human[40].texture = 22;
	Human[40].model = 1;
	Human[40].hp = 105;
	Human[40].AIlevel = 3;
	Human[40].AItrackability = -1;
	Human[40].Weapon[0] = 21;
	Human[40].Weapon[1] = 0;
	Human[40].type = 0;
	//긒귽긟
	Human[41].texture = 3;
	Human[41].model = 0;
	Human[41].hp = 100;
	Human[41].AIlevel = 0;
	Human[41].AItrackability = -1;
	Human[41].Weapon[0] = 0;
	Human[41].Weapon[1] = 0;
	Human[41].type = 0;
	//긒귽긟 SG
	Human[42].texture = 3;
	Human[42].model = 1;
	Human[42].hp = 100;
	Human[42].AIlevel = 4;
	Human[42].AItrackability = -1;
	Human[42].Weapon[0] = 22;
	Human[42].Weapon[1] = 0;
	Human[42].type = 0;

	// White Dummy
	Human[43].texture = 0;              // soldier_white.bmp
	Human[43].model = 0;                // 일반 인간 모델
	Human[43].hp = 3000;                // 높은 체력
	Human[43].AIlevel = 0;              // 가장 낮은 AI
	Human[43].AItrackability = -1;
	Human[43].Weapon[0] = 0;
	Human[43].Weapon[1] = 0;
	Human[43].type = 0;

	
	HumanTexturePath[0] = "./data/model/civ1.bmp";
	HumanTexturePath[1] = "./data/model/civ2.bmp";
	HumanTexturePath[2] = "./data/model/civ3.bmp";
	HumanTexturePath[3] = "./data/model/gates.bmp";
	HumanTexturePath[4] = "./data/model/gs.bmp";
	HumanTexturePath[5] = "./data/model/hage.bmp";
	HumanTexturePath[6] = "./data/model/islam.bmp";
	HumanTexturePath[7] = "./data/model/islam2.bmp";
	HumanTexturePath[8] = "./data/model/police.bmp";
	HumanTexturePath[9] = "./data/model/riiman.bmp";
	HumanTexturePath[10] = "./data/model/riiman_b.bmp";
	HumanTexturePath[11] = "./data/model/riiman_g.bmp";
	HumanTexturePath[12] = "./data/model/riiman_k.bmp";
	HumanTexturePath[13] = "./data/model/robot.bmp";
	HumanTexturePath[14] = "./data/model/soldier_black.bmp";
	HumanTexturePath[15] = "./data/model/soldier_blue.bmp";
	HumanTexturePath[16] = "./data/model/soldier_green.bmp";
	HumanTexturePath[17] = "./data/model/soldier_violet.bmp";
	HumanTexturePath[18] = "./data/model/soldier_white.bmp";
	HumanTexturePath[19] = "./data/model/soldier0.bmp";
	HumanTexturePath[20] = "./data/model/soldier1.bmp";
	HumanTexturePath[21] = "./data/model/soldier2.bmp";
	HumanTexturePath[22] = "./data/model/soldier3.bmp";
	HumanTexturePath[23] = "./data/model/syatu.bmp";
	HumanTexturePath[24] = "./data/model/syatu2.bmp";
	HumanTexturePath[25] = "./data/model/woman.bmp";
	HumanTexturePath[26] = "./data/model/zombie1.bmp";
	HumanTexturePath[27] = "./data/model/zombie2.bmp";
	HumanTexturePath[28] = "./data/model/zombie3.bmp";
	HumanTexturePath[29] = "./data/model/zombie4.bmp";
	HumanTexturePath[30] = "./data/model/summon.bmp";




	Weapon[0].name = "NONE";		// 이름
	Weapon[0].model = "";
	Weapon[0].texture = "";
	Weapon[0].attacks = 0;
	Weapon[0].penetration = 0;
	Weapon[0].blazings = 0;
	Weapon[0].speed = 0;
	Weapon[0].nbsmax = 0;
	Weapon[0].reloads = 0;
	Weapon[0].reaction = 0;
	Weapon[0].ErrorRangeMIN = 0;
	Weapon[0].ErrorRangeMAX = 0;
	Weapon[0].mx = 0.0f;
	Weapon[0].my = 0.0f;
	Weapon[0].mz = 0.0f;
	Weapon[0].flashx = 0.0f;
	Weapon[0].flashy = 0.0f;
	Weapon[0].flashz = 0.0f;
	Weapon[0].yakkyou_px = 0.0f;
	Weapon[0].yakkyou_py = 0.0f;
	Weapon[0].yakkyou_pz = 0.0f;
	Weapon[0].yakkyou_sx = 0.0f;
	Weapon[0].yakkyou_sy = 0.0f;
	Weapon[0].yakkyou_size = -1.0f;
	Weapon[0].yakkyou_delay = 0;
	Weapon[0].burstcnt = 0;
	Weapon[0].scopemode = 0;
	Weapon[0].size = 9.0f;
	Weapon[0].soundid = 0;
	Weapon[0].soundvolume = 0;
	Weapon[0].silencer = false;
	Weapon[0].WeaponP = 2;
	Weapon[0].ChangeWeaponID = -1;
	Weapon[0].ChangeWeaponCnt = 0;
	Weapon[0].pellet = 0;
	Weapon[0].is_shell_reload = false;
	Weapon[0].bHasChamber = false;
	Weapon[0].TacticalReloadFrame = 0;
	Weapon[0].SoundID_Reload = -1;
	Weapon[0].SoundID_Tactical = -1;

	Weapon[1].name = "MP5";						// 이름
	Weapon[1].model = "./data/model/weapon/mp5.x";			// 3D 모델 파일 경로
	Weapon[1].texture = "./data/model/weapon/mp5.bmp";		// 텍스처 파일 경로
	Weapon[1].attacks = 44;						// 공격력
	Weapon[1].penetration = 0;					// 관통력
	Weapon[1].blazings = 4;						// 연사 속도 (값이 작을수록 빠름)
	Weapon[1].speed = 10;						// 탄속
	Weapon[1].nbsmax = 30;						// 최대 장탄수
	Weapon[1].reloads = 79;						// 재장전 시간
	Weapon[1].reaction = 6;						// 반동
	Weapon[1].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[1].ErrorRangeMAX = 21;					// 최대 명중률 오차
	Weapon[1].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[1].my = 2.0f;						// 1인칭 손 위치 Y
	Weapon[1].mz = 30.0f;						// 1인칭 손 위치 Z
	Weapon[1].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[1].flashy = 14.0f;					// 총구 화염 위치 Y
	Weapon[1].flashz = 69.0f;					// 총구 화염 위치 Z
	Weapon[1].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[1].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[1].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[1].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[1].yakkyou_sy = 7.0f;					// 탄피 배출 속도 Y
	Weapon[1].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[1].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[1].burstcnt = 0;						// 점사 (0: 자동, 1: 단발, 3: 3점사, 4: 볼트액션)
	Weapon[1].scopemode = 0;					// 스코프 모드
	Weapon[1].size = 7.0f;						// 3인칭 모델 크기
	Weapon[1].soundid = 0;						// 발사음 ID
	Weapon[1].soundvolume = 90;				// 발사음 크기
	Weapon[1].silencer = false;					// 소음기 여부
	Weapon[1].WeaponP = 0;						// 무기 종류 (0: 양손, 1: 한손)
	Weapon[1].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[1].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[1].pellet = 1;						// 샷건 펠릿 수
	Weapon[1].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[1].bHasChamber = true;
	Weapon[1].TacticalReloadFrame = 57;
	Weapon[1].SoundID_Reload = -1;
	Weapon[1].SoundID_Tactical = -1;

	Weapon[2].name = "PSG-1";					// 이름
	Weapon[2].model = "./data/model/weapon/psg1.x";			// 3D 모델 파일 경로
	Weapon[2].texture = "./data/model/weapon/psg1.bmp";		// 텍스처 파일 경로
	Weapon[2].attacks = 70;						// 공격력
	Weapon[2].penetration = 4;					// 관통력
	Weapon[2].blazings = 7;						// 연사 속도
	Weapon[2].speed = 255;						// 탄속
	Weapon[2].nbsmax = 5;						// 최대 장탄수
	Weapon[2].reloads = 84;						// 재장전 시간
	Weapon[2].reaction = 30;					// 반동
	Weapon[2].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[2].ErrorRangeMAX = 19;					// 최대 명중률 오차
	Weapon[2].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[2].my = 9.0f;						// 1인칭 손 위치 Y
	Weapon[2].mz = 45.0f;						// 1인칭 손 위치 Z
	Weapon[2].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[2].flashy = 16.0f;					// 총구 화염 위치 Y
	Weapon[2].flashz = 108.0f;					// 총구 화염 위치 Z
	Weapon[2].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[2].yakkyou_py = 13.0f;					// 탄피 배출 위치 Y
	Weapon[2].yakkyou_pz = 38.0f;					// 탄피 배출 위치 Z
	Weapon[2].yakkyou_sx = 6.0f;					// 탄피 배출 속도 X
	Weapon[2].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[2].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[2].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[2].burstcnt = 1;						// 점사
	Weapon[2].scopemode = 7;					// 스코프 모드
	Weapon[2].size = 7.0f;						// 3인칭 모델 크기
	Weapon[2].soundid = 1;						// 발사음 ID
	Weapon[2].soundvolume = 90;				// 발사음 크기
	Weapon[2].silencer = false;					// 소음기 여부
	Weapon[2].WeaponP = 0;						// 무기 종류
	Weapon[2].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[2].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[2].pellet = 1;						// 샷건 펠릿 수
	Weapon[2].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[2].bHasChamber = true;
	Weapon[2].TacticalReloadFrame = 62;
	Weapon[2].SoundID_Reload = -1;
	Weapon[2].SoundID_Tactical = -1;

	Weapon[3].name = "M92F";					// 이름
	Weapon[3].model = "./data/model/weapon/m92f.x";			// 3D 모델 파일 경로
	Weapon[3].texture = "./data/model/weapon/m92f.bmp";		// 텍스처 파일 경로
	Weapon[3].attacks = 41;						// 공격력
	Weapon[3].penetration = 0;					// 관통력
	Weapon[3].blazings = 5;						// 연사 속도
	Weapon[3].speed = 9;						// 탄속
	Weapon[3].nbsmax = 15;						// 최대 장탄수
	Weapon[3].reloads = 55;						// 재장전 시간
	Weapon[3].reaction = 10;					// 반동
	Weapon[3].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[3].ErrorRangeMAX = 26;					// 최대 명중률 오차
	Weapon[3].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[3].my = 10.0f;						// 1인칭 손 위치 Y
	Weapon[3].mz = 58.0f;						// 1인칭 손 위치 Z
	Weapon[3].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[3].flashy = 19.0f;					// 총구 화염 위치 Y
	Weapon[3].flashz = 73.0f;					// 총구 화염 위치 Z
	Weapon[3].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[3].yakkyou_py = 15.0f;					// 탄피 배출 위치 Y
	Weapon[3].yakkyou_pz = 56.0f;					// 탄피 배출 위치 Z
	Weapon[3].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[3].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[3].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[3].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[3].burstcnt = 1;						// 점사
	Weapon[3].scopemode = 0;					// 스코프 모드
	Weapon[3].size = 7.0f;						// 3인칭 모델 크기
	Weapon[3].soundid = 28;						// 발사음 ID
	Weapon[3].soundvolume = 90;				// 발사음 크기
	Weapon[3].silencer = false;					// 소음기 여부
	Weapon[3].WeaponP = 1;						// 무기 종류
	Weapon[3].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[3].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[3].pellet = 1;						// 샷건 펠릿 수
	Weapon[3].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[3].bHasChamber = true;
	Weapon[3].TacticalReloadFrame = 35;
	Weapon[3].SoundID_Reload = -1;
	Weapon[3].SoundID_Tactical = -1;

	Weapon[4].name = "GLOCK18 (SEMI)";				// 이름
	Weapon[4].model = "./data/model/weapon/glock18.x";		// 3D 모델 파일 경로
	Weapon[4].texture = "./data/model/weapon/glock18.bmp";		// 텍스처 파일 경로
	Weapon[4].attacks = 38;						// 공격력
	Weapon[4].penetration = 0;					// 관통력
	Weapon[4].blazings = 5;						// 연사 속도
	Weapon[4].speed = 9;						// 탄속
	Weapon[4].nbsmax = 19;						// 최대 장탄수
	Weapon[4].reloads = 55;						// 재장전 시간
	Weapon[4].reaction = 9;						// 반동
	Weapon[4].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[4].ErrorRangeMAX = 26;					// 최대 명중률 오차
	Weapon[4].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[4].my = 11.0f;						// 1인칭 손 위치 Y
	Weapon[4].mz = 59.0f;						// 1인칭 손 위치 Z
	Weapon[4].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[4].flashy = 19.0f;					// 총구 화염 위치 Y
	Weapon[4].flashz = 72.0f;					// 총구 화염 위치 Z
	Weapon[4].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[4].yakkyou_py = 14.0f;					// 탄피 배출 위치 Y
	Weapon[4].yakkyou_pz = 54.0f;					// 탄피 배출 위치 Z
	Weapon[4].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[4].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[4].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[4].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[4].burstcnt = 1;						// 점사
	Weapon[4].scopemode = 0;					// 스코프 모드
	Weapon[4].size = 7.0f;						// 3인칭 모델 크기
	Weapon[4].soundid = 34;						// 발사음 ID
	Weapon[4].soundvolume = 90;				// 발사음 크기
	Weapon[4].silencer = false;					// 소음기 여부
	Weapon[4].WeaponP = 1;						// 무기 종류
	Weapon[4].ChangeWeaponID = 16;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[4].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[4].pellet = 1;						// 샷건 펠릿 수
	Weapon[4].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[4].bHasChamber = true;
	Weapon[4].TacticalReloadFrame = 35;
	Weapon[4].SoundID_Reload = -1;
	Weapon[4].SoundID_Tactical = -1;

	Weapon[5].name = "DESERT EAGLE";				// 이름
	Weapon[5].model = "./data/model/weapon/de.x";			// 3D 모델 파일 경로
	Weapon[5].texture = "./data/model/weapon/de.bmp";		// 텍스처 파일 경로
	Weapon[5].attacks = 68;						// 공격력
	Weapon[5].penetration = 2;					// 관통력
	Weapon[5].blazings = 5;						// 연사 속도
	Weapon[5].speed = 10;						// 탄속
	Weapon[5].nbsmax = 7;						// 최대 장탄수
	Weapon[5].reloads = 53;						// 재장전 시간
	Weapon[5].reaction = 13;					// 반동
	Weapon[5].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[5].ErrorRangeMAX = 26;					// 최대 명중률 오차
	Weapon[5].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[5].my = 10.0f;						// 1인칭 손 위치 Y
	Weapon[5].mz = 56.0f;						// 1인칭 손 위치 Z
	Weapon[5].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[5].flashy = 19.0f;					// 총구 화염 위치 Y
	Weapon[5].flashz = 76.0f;					// 총구 화염 위치 Z
	Weapon[5].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[5].yakkyou_py = 15.0f;					// 탄피 배출 위치 Y
	Weapon[5].yakkyou_pz = 56.0f;					// 탄피 배출 위치 Z
	Weapon[5].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[5].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[5].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[5].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[5].burstcnt = 1;						// 점사
	Weapon[5].scopemode = 0;					// 스코프 모드
	Weapon[5].size = 7.0f;						// 3인칭 모델 크기
	Weapon[5].soundid = 2;						// 발사음 ID
	Weapon[5].soundvolume = 90;				// 발사음 크기
	Weapon[5].silencer = false;					// 소음기 여부
	Weapon[5].WeaponP = 1;						// 무기 종류
	Weapon[5].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[5].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[5].pellet = 1;						// 샷건 펠릿 수
	Weapon[5].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[5].bHasChamber = true;
	Weapon[5].TacticalReloadFrame = 37;
	Weapon[5].SoundID_Reload = -1;
	Weapon[5].SoundID_Tactical = -1;

	Weapon[6].name = "MAC10 (Std.)";					// 이름
	Weapon[6].model = "./data/model/weapon/mac10.x";		// 3D 모델 파일 경로
	Weapon[6].texture = "./data/model/weapon/mac10.bmp";		// 텍스처 파일 경로
	Weapon[6].attacks = 47;						// 공격력
	Weapon[6].penetration = 0;					// 관통력
	Weapon[6].blazings = 3;						// 연사 속도
	Weapon[6].speed = 10;						// 탄속
	Weapon[6].nbsmax = 30;						// 최대 장탄수
	Weapon[6].reloads = 67;						// 재장전 시간
	Weapon[6].reaction = 9;						// 반동
	Weapon[6].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[6].ErrorRangeMAX = 25;					// 최대 명중률 오차
	Weapon[6].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[6].my = 11.0f;						// 1인칭 손 위치 Y
	Weapon[6].mz = 56.0f;						// 1인칭 손 위치 Z
	Weapon[6].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[6].flashy = 19.0f;					// 총구 화염 위치 Y
	Weapon[6].flashz = 73.0f;					// 총구 화염 위치 Z
	Weapon[6].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[6].yakkyou_py = 16.0f;					// 탄피 배출 위치 Y
	Weapon[6].yakkyou_pz = 52.0f;					// 탄피 배출 위치 Z
	Weapon[6].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[6].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[6].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[6].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[6].burstcnt = 0;						// 점사
	Weapon[6].scopemode = 0;					// 스코프 모드
	Weapon[6].size = 7.0f;						// 3인칭 모델 크기
	Weapon[6].soundid = 22;						// 발사음 ID
	Weapon[6].soundvolume = 90;				// 발사음 크기
	Weapon[6].silencer = false;					// 소음기 여부
	Weapon[6].WeaponP = 1;						// 무기 종류
	Weapon[6].ChangeWeaponID = 79;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[6].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[6].pellet = 1;						// 샷건 펠릿 수
	Weapon[6].is_shell_reload = false;				// 셸 단위 재장전 여부
	// ▼ [수정] 약실 없음 (노리쇠가 후퇴 고정된 상태에서 사격)
	Weapon[6].bHasChamber = false;
	Weapon[6].TacticalReloadFrame = 0; // 단축 불가
	Weapon[6].SoundID_Reload = -1;
	Weapon[6].SoundID_Tactical = -1;

	Weapon[7].name = "UMP";						// 이름
	Weapon[7].model = "./data/model/weapon/ump_eotech.x";		// 3D 모델 파일 경로
	Weapon[7].texture = "./data/model/weapon/ump.bmp";		// 텍스처 파일 경로
	Weapon[7].attacks = 50;						// 공격력
	Weapon[7].penetration = 0;					// 관통력
	Weapon[7].blazings = 5;						// 연사 속도
	Weapon[7].speed = 10;						// 탄속
	Weapon[7].nbsmax = 25;						// 최대 장탄수
	Weapon[7].reloads = 79;						// 재장전 시간
	Weapon[7].reaction = 7;						// 반동
	Weapon[7].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[7].ErrorRangeMAX = 19;					// 최대 명중률 오차
	Weapon[7].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[7].my = 6.0f;						// 1인칭 손 위치 Y
	Weapon[7].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[7].flashx = 8.0f;					// 총구 화염 위치 X
	Weapon[7].flashy = 16.5f;					// 총구 화염 위치 Y
	Weapon[7].flashz = 64.0f;					// 총구 화염 위치 Z
	Weapon[7].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[7].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[7].yakkyou_pz = 41.0f;					// 탄피 배출 위치 Z
	Weapon[7].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[7].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[7].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[7].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[7].burstcnt = 0;						// 점사
	Weapon[7].scopemode = 1;					// 스코프 모드
	Weapon[7].size = 7.0f;						// 3인칭 모델 크기
	Weapon[7].soundid = 32;						// 발사음 ID
	Weapon[7].soundvolume = 90;				// 발사음 크기
	Weapon[7].silencer = false;					// 소음기 여부
	Weapon[7].WeaponP = 0;						// 무기 종류
	Weapon[7].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[7].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[7].pellet = 1;						// 샷건 펠릿 수
	Weapon[7].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[7].bHasChamber = true;
	Weapon[7].TacticalReloadFrame = 57;
	Weapon[7].SoundID_Reload = -1;
	Weapon[7].SoundID_Tactical = -1;

	Weapon[8].name = "P90 (Std.)";						// 이름
	Weapon[8].model = "./data/model/weapon/p90.x";			// 3D 모델 파일 경로
	Weapon[8].texture = "./data/model/weapon/p90.bmp";		// 텍스처 파일 경로
	Weapon[8].attacks = 42;						// 공격력
	Weapon[8].penetration = 1;					// 관통력
	Weapon[8].blazings = 4;						// 연사 속도
	Weapon[8].speed = 11;						// 탄속
	Weapon[8].nbsmax = 50;						// 최대 장탄수
	Weapon[8].reloads = 91;						// 재장전 시간
	Weapon[8].reaction = 6;						// 반동
	Weapon[8].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[8].ErrorRangeMAX = 18;					// 최대 명중률 오차
	Weapon[8].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[8].my = 9.0f;						// 1인칭 손 위치 Y
	Weapon[8].mz = 38.0f;						// 1인칭 손 위치 Z
	Weapon[8].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[8].flashy = 15.0f;					// 총구 화염 위치 Y
	Weapon[8].flashz = 54.0f;					// 총구 화염 위치 Z
	Weapon[8].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[8].yakkyou_py = 4.0f;					// 탄피 배출 위치 Y
	Weapon[8].yakkyou_pz = 18.0f;					// 탄피 배출 위치 Z
	Weapon[8].yakkyou_sx = -1.0f;					// 탄피 배출 속도 X
	Weapon[8].yakkyou_sy = 0.0f;					// 탄피 배출 속도 Y
	Weapon[8].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[8].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[8].burstcnt = 0;						// 점사
	Weapon[8].scopemode = 1;					// 스코프 모드
	Weapon[8].size = 7.0f;						// 3인칭 모델 크기
	Weapon[8].soundid = 9;						// 발사음 ID
	Weapon[8].soundvolume = 90;				// 발사음 크기
	Weapon[8].silencer = false;					// 소음기 여부
	Weapon[8].WeaponP = 0;						// 무기 종류
	Weapon[8].ChangeWeaponID = 80;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[8].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[8].pellet = 1;						// 샷건 펠릿 수
	Weapon[8].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[8].bHasChamber = true;
	Weapon[8].TacticalReloadFrame = 74;
	Weapon[8].SoundID_Reload = -1;
	Weapon[8].SoundID_Tactical = -1;

	Weapon[9].name = "M4 (Std.)";						// 이름
	Weapon[9].model = "./data/model/weapon/m4_eotech.x";		// 3D 모델 파일 경로
	Weapon[9].texture = "./data/model/weapon/m4.bmp";		// 텍스처 파일 경로
	Weapon[9].attacks = 61;						// 공격력
	Weapon[9].penetration = 1;					// 관통력
	Weapon[9].blazings = 4;						// 연사 속도
	Weapon[9].speed = 11;						// 탄속
	Weapon[9].nbsmax = 30;						// 최대 장탄수
	Weapon[9].reloads = 78;						// 재장전 시간
	Weapon[9].reaction = 8;						// 반동
	Weapon[9].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[9].ErrorRangeMAX = 19;					// 최대 명중률 오차
	Weapon[9].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[9].my = 6.0f;						// 1인칭 손 위치 Y
	Weapon[9].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[9].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[9].flashy = 16.0f;					// 총구 화염 위치 Y
	Weapon[9].flashz = 84.0f;					// 총구 화염 위치 Z
	Weapon[9].yakkyou_px = 10.0f;					// 탄피 배출 위치 X
	Weapon[9].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[9].yakkyou_pz = 40.0f;					// 탄피 배출 위치 Z
	Weapon[9].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[9].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[9].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[9].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[9].burstcnt = 0;						// 점사
	Weapon[9].scopemode = 2;					// 스코프 모드
	Weapon[9].size = 7.0f;						// 3인칭 모델 크기
	Weapon[9].soundid = 13;						// 발사음 ID
	Weapon[9].soundvolume = 90;				// 발사음 크기
	Weapon[9].silencer = false;					// 소음기 여부
	Weapon[9].WeaponP = 0;						// 무기 종류
	Weapon[9].ChangeWeaponID = 24;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[9].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[9].pellet = 1;						// 샷건 펠릿 수
	Weapon[9].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[9].bHasChamber = true;
	Weapon[9].TacticalReloadFrame = 51;
	Weapon[9].SoundID_Reload = -1;
	Weapon[9].SoundID_Tactical = -1;

	Weapon[10].name = "AK47 (Std.)";					// 이름
	Weapon[10].model = "./data/model/weapon/ak47.x";		// 3D 모델 파일 경로
	Weapon[10].texture = "./data/model/weapon/ak47.bmp";		// 텍스처 파일 경로
	Weapon[10].attacks = 65;					// 공격력
	Weapon[10].penetration = 1;					// 관통력
	Weapon[10].blazings = 4;					// 연사 속도
	Weapon[10].speed = 11;						// 탄속
	Weapon[10].nbsmax = 30;						// 최대 장탄수
	Weapon[10].reloads = 89;					// 재장전 시간
	Weapon[10].reaction = 9;					// 반동
	Weapon[10].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[10].ErrorRangeMAX = 20;					// 최대 명중률 오차
	Weapon[10].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[10].my = 7.0f;						// 1인칭 손 위치 Y
	Weapon[10].mz = 28.0f;						// 1인칭 손 위치 ZF
	Weapon[10].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[10].flashy = 14.5f;					// 총구 화염 위치 Y
	Weapon[10].flashz = 88.0f;					// 총구 화염 위치 Z
	Weapon[10].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[10].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[10].yakkyou_pz = 41.0f;					// 탄피 배출 위치 Z
	Weapon[10].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[10].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[10].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[10].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[10].burstcnt = 0;					// 점사
	Weapon[10].scopemode = 0;					// 스코프 모드
	Weapon[10].size = 7.0f;						// 3인칭 모델 크기
	Weapon[10].soundid = 6;						// 발사음 ID
	Weapon[10].soundvolume = 90;				// 발사음 크기
	Weapon[10].silencer = false;					// 소음기 여부
	Weapon[10].WeaponP = 0;						// 무기 종류
	Weapon[10].ChangeWeaponID = 85;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[10].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[10].pellet = 1;						// 샷건 펠릿 수
	Weapon[10].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[10].bHasChamber = true;
	Weapon[10].TacticalReloadFrame = 64;
	Weapon[10].SoundID_Reload = -1;
	Weapon[10].SoundID_Tactical = -1;

	Weapon[11].name = "AUG (Std.)";					// 이름
	Weapon[11].model = "./data/model/weapon/aug.x";			// 3D 모델 파일 경로
	Weapon[11].texture = "./data/model/weapon/aug.bmp";		// 텍스처 파일 경로
	Weapon[11].attacks = 60;					// 공격력
	Weapon[11].penetration = 1;					// 관통력
	Weapon[11].blazings = 5;					// 연사 속도
	Weapon[11].speed = 11;						// 탄속
	Weapon[11].nbsmax = 30;						// 최대 장탄수
	Weapon[11].reloads = 91;					// 재장전 시간
	Weapon[11].reaction = 7;					// 반동
	Weapon[11].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[11].ErrorRangeMAX = 18;					// 최대 명중률 오차
	Weapon[11].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[11].my = 9.0f;						// 1인칭 손 위치 Y
	Weapon[11].mz = 31.0f;						// 1인칭 손 위치 Z
	Weapon[11].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[11].flashy = 16.0f;					// 총구 화염 위치 Y
	Weapon[11].flashz = 76.0f;					// 총구 화염 위치 Z
	Weapon[11].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[11].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[11].yakkyou_pz = 15.0f;					// 탄피 배출 위치 Z
	Weapon[11].yakkyou_sx = 3.0f;					// 탄피 배출 속도 X
	Weapon[11].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[11].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[11].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[11].burstcnt = 0;					// 점사
	Weapon[11].scopemode = 3;					// 스코프 모드
	Weapon[11].size = 7.0f;						// 3인칭 모델 크기
	Weapon[11].soundid = 69;						// 발사음 ID
	Weapon[11].soundvolume = 90;				// 발사음 크기
	Weapon[11].silencer = false;					// 소음기 여부
	Weapon[11].WeaponP = 0;						// 무기 종류
	Weapon[11].ChangeWeaponID = 90;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[11].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[11].pellet = 1;						// 샷건 펠릿 수
	Weapon[11].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[11].bHasChamber = true;
	Weapon[11].TacticalReloadFrame = 67;
	Weapon[11].SoundID_Reload = -1;
	Weapon[11].SoundID_Tactical = -1;

	Weapon[12].name = "M249";					// 이름
	Weapon[12].model = "./data/model/weapon/m249.x";		// 3D 모델 파일 경로
	Weapon[12].texture = "./data/model/weapon/m249.bmp";		// 텍스처 파일 경로
	Weapon[12].attacks = 59;					// 공격력
	Weapon[12].penetration = 3;					// 관통력
	Weapon[12].blazings = 4;					// 연사 속도
	Weapon[12].speed = 11;						// 탄속
	Weapon[12].nbsmax = 100;					// 최대 장탄수
	Weapon[12].reloads = 167;					// 재장전 시간
	Weapon[12].reaction = 7;					// 반동
	Weapon[12].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[12].ErrorRangeMAX = 22;					// 최대 명중률 오차
	Weapon[12].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[12].my = 7.0f;						// 1인칭 손 위치 Y
	Weapon[12].mz = 31.0f;						// 1인칭 손 위치 Z
	Weapon[12].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[12].flashy = 19.5f;					// 총구 화염 위치 Y
	Weapon[12].flashz = 88.0f;					// 총구 화염 위치 Z
	Weapon[12].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[12].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[12].yakkyou_pz = 45.0f;					// 탄피 배출 위치 Z
	Weapon[12].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[12].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[12].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[12].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[12].burstcnt = 0;					// 점사
	Weapon[12].scopemode = 0;					// 스코프 모드
	Weapon[12].size = 7.0f;						// 3인칭 모델 크기
	Weapon[12].soundid = 17;						// 발사음 ID
	Weapon[12].soundvolume = 90;				// 발사음 크기
	Weapon[12].silencer = false;					// 소음기 여부
	Weapon[12].WeaponP = 0;						// 무기 종류
	Weapon[12].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[12].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[12].pellet = 1;						// 샷건 펠릿 수
	Weapon[12].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[12].bHasChamber = false;
	Weapon[12].TacticalReloadFrame = 0;
	Weapon[12].SoundID_Reload = -1;
	Weapon[12].SoundID_Tactical = -1;

	Weapon[13].name = "GRENADE";					// 이름
	Weapon[13].model = "./data/model/weapon/grenade.x";		// 3D 모델 파일 경로
	Weapon[13].texture = "./data/model/weapon/grenade.bmp";		// 텍스처 파일 경로
	Weapon[13].attacks = 0;						// 공격력
	Weapon[13].penetration = 0;					// 관통력
	Weapon[13].blazings = 35;					// 연사 속도
	Weapon[13].speed = 0;						// 탄속
	Weapon[13].nbsmax = 1;						// 최대 장탄수
	Weapon[13].reloads = 0;						// 재장전 시간
	Weapon[13].reaction = 0;					// 반동
	Weapon[13].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[13].ErrorRangeMAX = 20;					// 최대 명중률 오차
	Weapon[13].mx = 2.0f;						// 1인칭 손 위치 X
	Weapon[13].my = 12.0f;						// 1인칭 손 위치 Y
	Weapon[13].mz = 50.0f;						// 1인칭 손 위치 Z
	Weapon[13].flashx = 2.0f;					// 총구 화염 위치 X
	Weapon[13].flashy = 12.0f;					// 총구 화염 위치 Y
	Weapon[13].flashz = 50.0f;					// 총구 화염 위치 Z
	Weapon[13].yakkyou_px = 0.0f;					// 탄피 배출 위치 X
	Weapon[13].yakkyou_py = 0.0f;					// 탄피 배출 위치 Y
	Weapon[13].yakkyou_pz = 0.0f;					// 탄피 배출 위치 Z
	Weapon[13].yakkyou_sx = 0.0f;					// 탄피 배출 속도 X
	Weapon[13].yakkyou_sy = 0.0f;					// 탄피 배출 속도 Y
	Weapon[13].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[13].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[13].burstcnt = 1;					// 점사
	Weapon[13].scopemode = 0;					// 스코프 모드
	Weapon[13].size = 7.0f;						// 3인칭 모델 크기
	Weapon[13].soundid = 62;						// 발사음 ID
	Weapon[13].soundvolume = 90;				// 발사음 크기
	Weapon[13].silencer = false;					// 소음기 여부
	Weapon[13].WeaponP = 1;						// 무기 종류
	Weapon[13].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[13].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[13].pellet = 1;						// 샷건 펠릿 수
	Weapon[13].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[13].bHasChamber = false;
	Weapon[13].TacticalReloadFrame = 0;
	Weapon[13].SoundID_Reload = -1;
	Weapon[13].SoundID_Tactical = -1;

	Weapon[14].name = "MP5SD";					// 이름
	Weapon[14].model = "./data/model/weapon/mp5sd.x";		// 3D 모델 파일 경로
	Weapon[14].texture = "./data/model/weapon/mp5sd.bmp";		// 텍스처 파일 경로
	Weapon[14].attacks = 37;					// 공격력
	Weapon[14].penetration = 0;					// 관통력
	Weapon[14].blazings = 4;					// 연사 속도
	Weapon[14].speed = 9;						// 탄속
	Weapon[14].nbsmax = 30;						// 최대 장탄수
	Weapon[14].reloads = 79;					// 재장전 시간
	Weapon[14].reaction = 6;					// 반동
	Weapon[14].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[14].ErrorRangeMAX = 21;					// 최대 명중률 오차
	Weapon[14].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[14].my = 3.0f;						// 1인칭 손 위치 Y
	Weapon[14].mz = 30.0f;						// 1인칭 손 위치 Z
	Weapon[14].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[14].flashy = 14.0f;					// 총구 화염 위치 Y
	Weapon[14].flashz = 79.0f;					// 총구 화염 위치 Z
	Weapon[14].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[14].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[14].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[14].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[14].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[14].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[14].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[14].burstcnt = 0;					// 점사
	Weapon[14].scopemode = 4;					// 스코프 모드
	Weapon[14].size = 7.0f;						// 3인칭 모델 크기
	Weapon[14].soundid = 5;					// 발사음 ID
	Weapon[14].soundvolume = 80;				// 발사음 크기
	Weapon[14].silencer = true;					// 소음기 여부
	Weapon[14].WeaponP = 0;						// 무기 종류
	Weapon[14].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[14].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[14].pellet = 1;						// 샷건 펠릿 수
	Weapon[14].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[14].bHasChamber = true;
	Weapon[14].TacticalReloadFrame = 57;
	Weapon[14].SoundID_Reload = -1;
	Weapon[14].SoundID_Tactical = -1;

	Weapon[15].name = "CASE";					// 이름
	Weapon[15].model = "./data/model/weapon/case.x";		// 3D 모델 파일 경로
	Weapon[15].texture = "./data/model/weapon/case.bmp";		// 텍스처 파일 경로
	Weapon[15].attacks = 0;						// 공격력
	Weapon[15].penetration = 0;					// 관통력
	Weapon[15].blazings = 0;					// 연사 속도
	Weapon[15].speed = 0;						// 탄속
	Weapon[15].nbsmax = 0;						// 최대 장탄수
	Weapon[15].reloads = 0;						// 재장전 시간
	Weapon[15].reaction = 0;					// 반동
	Weapon[15].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[15].ErrorRangeMAX = 0;					// 최대 명중률 오차
	Weapon[15].mx = 15.0f;						// 1인칭 손 위치 X
	Weapon[15].my = -2.0f;						// 1인칭 손 위치 Y
	Weapon[15].mz = 35.0f;						// 1인칭 손 위치 Z
	Weapon[15].flashx = 0.0f;					// 총구 화염 위치 X
	Weapon[15].flashy = 0.0f;					// 총구 화염 위치 Y
	Weapon[15].flashz = 0.0f;					// 총구 화염 위치 Z
	Weapon[15].yakkyou_px = 0.0f;					// 탄피 배출 위치 X
	Weapon[15].yakkyou_py = 0.0f;					// 탄피 배출 위치 Y
	Weapon[15].yakkyou_pz = 0.0f;					// 탄피 배출 위치 Z
	Weapon[15].yakkyou_sx = 0.0f;					// 탄피 배출 속도 X
	Weapon[15].yakkyou_sy = 0.0f;					// 탄피 배출 속도 Y
	Weapon[15].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[15].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[15].burstcnt = 0;					// 점사
	Weapon[15].scopemode = 0;					// 스코프 모드
	Weapon[15].size = 5.0f;						// 3인칭 모델 크기
	Weapon[15].soundid = 0;						// 발사음 ID
	Weapon[15].soundvolume = 0;					// 발사음 크기
	Weapon[15].silencer = false;					// 소음기 여부
	Weapon[15].WeaponP = 2;						// 무기 종류
	Weapon[15].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[15].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[15].pellet = 1;						// 샷건 펠릿 수
	Weapon[15].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[15].bHasChamber = false;
	Weapon[15].TacticalReloadFrame = 0;
	Weapon[15].SoundID_Reload = -1;
	Weapon[15].SoundID_Tactical = -1;

	Weapon[16].name = "GLOCK18 (FULL)";				// 이름
	Weapon[16].model = "./data/model/weapon/glock18.x";		// 3D 모델 파일 경로
	Weapon[16].texture = "./data/model/weapon/glock18.bmp";		// 텍스처 파일 경로
	Weapon[16].attacks = 38;					// 공격력
	Weapon[16].penetration = 0;					// 관통력
	Weapon[16].blazings = 3;					// 연사 속도
	Weapon[16].speed = 9;						// 탄속
	Weapon[16].nbsmax = 19;						// 최대 장탄수
	Weapon[16].reloads = 55;					// 재장전 시간
	Weapon[16].reaction = 9;					// 반동
	Weapon[16].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[16].ErrorRangeMAX = 26;					// 최대 명중률 오차
	Weapon[16].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[16].my = 11.0f;						// 1인칭 손 위치 Y
	Weapon[16].mz = 59.0f;						// 1인칭 손 위치 Z
	Weapon[16].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[16].flashy = 19.0f;					// 총구 화염 위치 Y
	Weapon[16].flashz = 72.0f;					// 총구 화염 위치 Z
	Weapon[16].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[16].yakkyou_py = 14.0f;					// 탄피 배출 위치 Y
	Weapon[16].yakkyou_pz = 54.0f;					// 탄피 배출 위치 Z
	Weapon[16].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[16].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[16].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[16].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[16].burstcnt = 0;					// 점사
	Weapon[16].scopemode = 0;					// 스코프 모드
	Weapon[16].size = 7.0f;						// 3인칭 모델 크기
	Weapon[16].soundid = 34;						// 발사음 ID
	Weapon[16].soundvolume = 90;				// 발사음 크기
	Weapon[16].silencer = false;					// 소음기 여부
	Weapon[16].WeaponP = 1;						// 무기 종류
	Weapon[16].ChangeWeaponID = 4;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[16].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[16].pellet = 1;						// 샷건 펠릿 수
	Weapon[16].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[16].bHasChamber = true;
	Weapon[16].TacticalReloadFrame = 35;
	Weapon[16].SoundID_Reload = -1;
	Weapon[16].SoundID_Tactical = -1;

	Weapon[17].name = "M1911";					// 이름
	Weapon[17].model = "./data/model/weapon/cg.x";			// 3D 모델 파일 경로
	Weapon[17].texture = "./data/model/weapon/cg.bmp";		// 텍스처 파일 경로
	Weapon[17].attacks = 46;					// 공격력
	Weapon[17].penetration = 0;					// 관통력
	Weapon[17].blazings = 5;					// 연사 속도
	Weapon[17].speed = 9;						// 탄속
	Weapon[17].nbsmax = 7;						// 최대 장탄수
	Weapon[17].reloads = 55;					// 재장전 시간
	Weapon[17].reaction = 11;					// 반동
	Weapon[17].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[17].ErrorRangeMAX = 26;					// 최대 명중률 오차
	Weapon[17].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[17].my = 10.0f;						// 1인칭 손 위치 Y
	Weapon[17].mz = 58.0f;						// 1인칭 손 위치 Z
	Weapon[17].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[17].flashy = 17.5f;					// 총구 화염 위치 Y
	Weapon[17].flashz = 75.0f;					// 총구 화염 위치 Z
	Weapon[17].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[17].yakkyou_py = 16.0f;					// 탄피 배출 위치 Y
	Weapon[17].yakkyou_pz = 57.0f;					// 탄피 배출 위치 Z
	Weapon[17].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[17].yakkyou_sy = 3.0f;					// 탄피 배출 속도 Y
	Weapon[17].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[17].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[17].burstcnt = 1;					// 점사
	Weapon[17].scopemode = 0;					// 스코프 모드
	Weapon[17].size = 7.0f;						// 3인칭 모델 크기
	Weapon[17].soundid = 4;						// 발사음 ID
	Weapon[17].soundvolume = 90;				// 발사음 크기
	Weapon[17].silencer = false;					// 소음기 여부
	Weapon[17].WeaponP = 1;						// 무기 종류
	Weapon[17].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[17].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[17].pellet = 1;						// 샷건 펠릿 수
	Weapon[17].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[17].bHasChamber = true;
	Weapon[17].TacticalReloadFrame = 35;
	Weapon[17].SoundID_Reload = -1;
	Weapon[17].SoundID_Tactical = -1;

	Weapon[18].name = "GLOCK17 (Std.)";					// 이름
	Weapon[18].model = "./data/model/weapon/glock17.x";		// 3D 모델 파일 경로
	Weapon[18].texture = "./data/model/weapon/glock17.bmp";		// 텍스처 파일 경로
	Weapon[18].attacks = 38;					// 공격력
	Weapon[18].penetration = 0;					// 관통력
	Weapon[18].blazings = 5;					// 연사 속도
	Weapon[18].speed = 9;						// 탄속
	Weapon[18].nbsmax = 17;						// 최대 장탄수
	Weapon[18].reloads = 55;					// 재장전 시간
	Weapon[18].reaction = 9;					// 반동
	Weapon[18].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[18].ErrorRangeMAX = 26;					// 최대 명중률 오차
	Weapon[18].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[18].my = 11.0f;						// 1인칭 손 위치 Y
	Weapon[18].mz = 58.0f;						// 1인칭 손 위치 Z
	Weapon[18].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[18].flashy = 19.0f;					// 총구 화염 위치 Y
	Weapon[18].flashz = 72.0f;					// 총구 화염 위치 Z
	Weapon[18].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[18].yakkyou_py = 14.0f;					// 탄피 배출 위치 Y
	Weapon[18].yakkyou_pz = 54.0f;					// 탄피 배출 위치 Z
	Weapon[18].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[18].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[18].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[18].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[18].burstcnt = 1;					// 점사
	Weapon[18].scopemode = 0;					// 스코프 모드
	Weapon[18].size = 7.0f;						// 3인칭 모델 크기
	Weapon[18].soundid = 70;						// 발사음 ID
	Weapon[18].soundvolume = 90;				// 발사음 크기
	Weapon[18].silencer = false;					// 소음기 여부
	Weapon[18].WeaponP = 1;						// 무기 종류
	Weapon[18].ChangeWeaponID = 75;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[18].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[18].pellet = 1;						// 샷건 펠릿 수
	Weapon[18].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[18].bHasChamber = true;
	Weapon[18].TacticalReloadFrame = 35;
	Weapon[18].SoundID_Reload = -1;
	Weapon[18].SoundID_Tactical = -1;

	Weapon[19].name = "M1";						// 이름
	Weapon[19].model = "./data/model/weapon/m1.x";			// 3D 모델 파일 경로
	Weapon[19].texture = "./data/model/weapon/m1.bmp";		// 텍스처 파일 경로
	Weapon[19].attacks = 90;					// 공격력
	Weapon[19].penetration = 1;					// 관통력
	Weapon[19].blazings = 30;					// 연사 속도
	Weapon[19].speed = 9;						// 탄속
	Weapon[19].nbsmax = 7;						// 최대 장탄수
	Weapon[19].reloads = 20;					// 재장전 시간
	Weapon[19].reaction = 18;					// 반동
	Weapon[19].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[19].ErrorRangeMAX = 23;					// 최대 명중률 오차
	Weapon[19].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[19].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[19].mz = 13.0f;						// 1인칭 손 위치 Z
	Weapon[19].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[19].flashy = 16.0f;					// 총구 화염 위치 Y
	Weapon[19].flashz = 97.0f;					// 총구 화염 위치 Z
	Weapon[19].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[19].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[19].yakkyou_pz = 42.0f;					// 탄피 배출 위치 Z
	Weapon[19].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[19].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[19].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[19].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[19].burstcnt = 4;					// 점사
	Weapon[19].scopemode = 0;					// 스코프 모드
	Weapon[19].size = 7.0f;						// 3인칭 모델 크기
	Weapon[19].soundid = 18;						// 발사음 ID
	Weapon[19].soundvolume = 90;				// 발사음 크기
	Weapon[19].silencer = false;					// 소음기 여부
	Weapon[19].WeaponP = 0;						// 무기 종류
	Weapon[19].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[19].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[19].pellet = 6;						// 샷건 펠릿 수
	Weapon[19].is_shell_reload = true;				// 셸 단위 재장전 여부
	Weapon[19].bHasChamber = true; // +1발 장전 허용 (튜브 꽉 채우고 약실 1발)
	Weapon[19].TacticalReloadFrame = 0;
	Weapon[19].SoundID_Reload = -1;
	Weapon[19].SoundID_Tactical = -1;

	Weapon[20].name = "FAMAS (Std.)";					// 이름
	Weapon[20].model = "./data/model/weapon/FAMAS_Eotech.x";		// 3D 모델 파일 경로
	Weapon[20].texture = "./data/model/weapon/famas.bmp";		// 텍스처 파일 경로
	Weapon[20].attacks = 55;					// 공격력
	Weapon[20].penetration = 1;					// 관통력
	Weapon[20].blazings = 3;					// 연사 속도
	Weapon[20].speed = 11;						// 탄속
	Weapon[20].nbsmax = 25;						// 최대 장탄수
	Weapon[20].reloads = 91;					// 재장전 시간
	Weapon[20].reaction = 7;					// 반동
	Weapon[20].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[20].ErrorRangeMAX = 19;					// 최대 명중률 오차
	Weapon[20].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[20].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[20].mz = 32.0f;						// 1인칭 손 위치 Z
	Weapon[20].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[20].flashy = 15.0f;					// 총구 화염 위치 Y
	Weapon[20].flashz = 70.0f;					// 총구 화염 위치 Z
	Weapon[20].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[20].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[20].yakkyou_pz = 14.0f;					// 탄피 배출 위치 Z
	Weapon[20].yakkyou_sx = 3.0f;					// 탄피 배출 속도 X
	Weapon[20].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[20].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[20].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[20].burstcnt = 0;					// 점사
	Weapon[20].scopemode = 2;					// 스코프 모드
	Weapon[20].size = 7.0f;						// 3인칭 모델 크기
	Weapon[20].soundid = 10;						// 발사음 ID
	Weapon[20].soundvolume = 90;				// 발사음 크기
	Weapon[20].silencer = false;					// 소음기 여부
	Weapon[20].WeaponP = 0;						// 무기 종류
	Weapon[20].ChangeWeaponID = 86;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[20].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[20].pellet = 1;						// 샷건 펠릿 수
	Weapon[20].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[20].bHasChamber = true;
	Weapon[20].TacticalReloadFrame = 67;
	Weapon[20].SoundID_Reload = -1;
	Weapon[20].SoundID_Tactical = -1;

	Weapon[21].name = "MK23 (Std.)";					// 이름
	Weapon[21].model = "./data/model/weapon/mk23.x";		// 3D 모델 파일 경로
	Weapon[21].texture = "./data/model/weapon/mk23.bmp";		// 텍스처 파일 경로
	Weapon[21].attacks = 48;					// 공격력
	Weapon[21].penetration = 0;					// 관통력
	Weapon[21].blazings = 5;					// 연사 속도
	Weapon[21].speed = 9;						// 탄속
	Weapon[21].nbsmax = 12;						// 최대 장탄수
	Weapon[21].reloads = 53;					// 재장전 시간
	Weapon[21].reaction = 10;					// 반동
	Weapon[21].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[21].ErrorRangeMAX = 26;					// 최대 명중률 오차
	Weapon[21].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[21].my = 10.0f;						// 1인칭 손 위치 Y
	Weapon[21].mz = 58.0f;						// 1인칭 손 위치 Z
	Weapon[21].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[21].flashy = 18.5f;					// 총구 화염 위치 Y
	Weapon[21].flashz = 75.0f;					// 총구 화염 위치 Z
	Weapon[21].yakkyou_px = 1.0f;					// 탄피 배출 위치 X
	Weapon[21].yakkyou_py = 16.0f;					// 탄피 배출 위치 Y
	Weapon[21].yakkyou_pz = 60.0f;					// 탄피 배출 위치 Z
	Weapon[21].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[21].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[21].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[21].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[21].burstcnt = 1;					// 점사
	Weapon[21].scopemode = 0;					// 스코프 모드
	Weapon[21].size = 7.0f;						// 3인칭 모델 크기
	Weapon[21].soundid = 23;						// 발사음 ID
	Weapon[21].soundvolume = 90;				// 발사음 크기
	Weapon[21].silencer = false;					// 소음기 여부
	Weapon[21].WeaponP = 1;						// 무기 종류
	Weapon[21].ChangeWeaponID = 22;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[21].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[21].pellet = 1;						// 샷건 펠릿 수
	Weapon[21].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[21].bHasChamber = true;
	Weapon[21].TacticalReloadFrame = 37;
	Weapon[21].SoundID_Reload = -1;
	Weapon[21].SoundID_Tactical = -1;

	Weapon[22].name = "MK23 (SD)";					// 이름
	Weapon[22].model = "./data/model/weapon/mk23sd.x";		// 3D 모델 파일 경로
	Weapon[22].texture = "./data/model/weapon/mk23.bmp";		// 텍스처 파일 경로
	Weapon[22].attacks = 39;					// 공격력
	Weapon[22].penetration = 0;					// 관통력
	Weapon[22].blazings = 5;					// 연사 속도
	Weapon[22].speed = 9;						// 탄속
	Weapon[22].nbsmax = 12;						// 최대 장탄수
	Weapon[22].reloads = 53;					// 재장전 시간
	Weapon[22].reaction = 9;					// 반동
	Weapon[22].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[22].ErrorRangeMAX = 26;					// 최대 명중률 오차
	Weapon[22].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[22].my = 10.0f;						// 1인칭 손 위치 Y
	Weapon[22].mz = 58.0f;						// 1인칭 손 위치 Z
	Weapon[22].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[22].flashy = 18.5f;					// 총구 화염 위치 Y
	Weapon[22].flashz = 94.0f;					// 총구 화염 위치 Z
	Weapon[22].yakkyou_px = 1.0f;					// 탄피 배출 위치 X
	Weapon[22].yakkyou_py = 16.0f;					// 탄피 배출 위치 Y
	Weapon[22].yakkyou_pz = 60.0f;					// 탄피 배출 위치 Z
	Weapon[22].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[22].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[22].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[22].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[22].burstcnt = 1;					// 점사
	Weapon[22].scopemode = 0;					// 스코프 모드
	Weapon[22].size = 7.0f;						// 3인칭 모델 크기
	Weapon[22].soundid = 24;					// 발사음 ID
	Weapon[22].soundvolume = 80;				// 발사음 크기
	Weapon[22].silencer = true;					// 소음기 여부
	Weapon[22].WeaponP = 1;						// 무기 종류
	Weapon[22].ChangeWeaponID = 21;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[22].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[22].pellet = 1;						// 샷건 펠릿 수
	Weapon[22].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[22].bHasChamber = true;
	Weapon[22].TacticalReloadFrame = 37;
	Weapon[22].SoundID_Reload = -1;
	Weapon[22].SoundID_Tactical = -1;

	Weapon[23].name = "M1 Carbine";    				// 이름
	Weapon[23].model = "./data/model/weapon/1/M1_Carbine_Scope.x";	// 3D 모델 파일 경로
	Weapon[23].texture = "./data/model/weapon/1/M1_Carbine.bmp";	// 텍스처 파일 경로
	Weapon[23].attacks = 70;         				// 공격력
	Weapon[23].penetration = 2;      				// 관통력
	Weapon[23].blazings = 7;         				// 연사 속도
	Weapon[23].speed = 80;           				// 탄속
	Weapon[23].nbsmax = 15;          				// 최대 장탄수
	Weapon[23].reloads = 79;         				// 재장전 시간
	Weapon[23].reaction = 10;         				// 반동
	Weapon[23].ErrorRangeMIN = 0;    				// 최소 명중률 오차
	Weapon[23].ErrorRangeMAX = 19;   				// 최대 명중률 오차
	Weapon[23].mx = 9.0f;            				// 1인칭 손 위치 X
	Weapon[23].my = 8.0f;            				// 1인칭 손 위치 Y
	Weapon[23].mz = 28.0f;           				// 1인칭 손 위치 Z
	Weapon[23].flashx = 9.0f;        				// 총구 화염 위치 X
	Weapon[23].flashy = 16.0f;       				// 총구 화염 위치 Y
	Weapon[23].flashz = 91.0f;       				// 총구 화염 위치 Z
	Weapon[23].yakkyou_px = 10.0f;   				// 탄피 배출 위치 X
	Weapon[23].yakkyou_py = 12.0f;   				// 탄피 배출 위치 Y
	Weapon[23].yakkyou_pz = 40.0f;   				// 탄피 배출 위치 Z
	Weapon[23].yakkyou_sx = 4.0f;    				// 탄피 배출 속도 X
	Weapon[23].yakkyou_sy = 6.0f;    				// 탄피 배출 속도 Y
	Weapon[23].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[23].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[23].burstcnt = 1;         				// 점사
	Weapon[23].scopemode = 4;        				// 스코프 모드
	Weapon[23].size = 7.0f;          				// 3인칭 모델 크기
	Weapon[23].soundid = 12;          				// 발사음 ID
	Weapon[23].soundvolume = 90;     				// 발사음 크기
	Weapon[23].silencer = false;     				// 소음기 여부
	Weapon[23].WeaponP = 0;          				// 무기 종류
	Weapon[23].ChangeWeaponID = -1;  				// 발사모드 전환 시 바뀔 무기 ID
	Weapon[23].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[23].pellet = 1;           				// 샷건 펠릿 수
	Weapon[23].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[23].bHasChamber = true;
	Weapon[23].TacticalReloadFrame = 57;
	Weapon[23].SoundID_Reload = -1;
	Weapon[23].SoundID_Tactical = -1;

	Weapon[24].name = "M4 (SD)";					// 이름
	Weapon[24].model = "./data/model/weapon/m4_eotech_sd.x";	// 3D 모델 파일 경로
	Weapon[24].texture = "./data/model/weapon/m4.bmp";		// 텍스처 파일 경로
	Weapon[24].attacks = 55;					// 공격력
	Weapon[24].penetration = 1;					// 관통력
	Weapon[24].blazings = 4;					// 연사 속도
	Weapon[24].speed = 11;						// 탄속
	Weapon[24].nbsmax = 30;						// 최대 장탄수
	Weapon[24].reloads = 78;					// 재장전 시간
	Weapon[24].reaction = 8;					// 반동
	Weapon[24].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[24].ErrorRangeMAX = 19;					// 최대 명중률 오차
	Weapon[24].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[24].my = 6.0f;						// 1인칭 손 위치 Y
	Weapon[24].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[24].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[24].flashy = 16.0f;					// 총구 화염 위치 Y
	Weapon[24].flashz = 98.0f;					// 총구 화염 위치 Z
	Weapon[24].yakkyou_px = 10.0f;					// 탄피 배출 위치 X
	Weapon[24].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[24].yakkyou_pz = 40.0f;					// 탄피 배출 위치 Z
	Weapon[24].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[24].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[24].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[24].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[24].burstcnt = 0;					// 점사
	Weapon[24].scopemode = 2;					// 스코프 모드
	Weapon[24].size = 7.0f;						// 3인칭 모델 크기
	Weapon[24].soundid = 14;					// 발사음 ID
	Weapon[24].soundvolume = 80;				// 발사음 크기
	Weapon[24].silencer = true;					// 소음기 여부
	Weapon[24].WeaponP = 0;						// 무기 종류
	Weapon[24].ChangeWeaponID = 9;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[24].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[24].pellet = 1;						// 샷건 펠릿 수
	Weapon[24].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[24].bHasChamber = true;
	Weapon[24].TacticalReloadFrame = 51;
	Weapon[24].SoundID_Reload = -1;
	Weapon[24].SoundID_Tactical = -1;

	Weapon[25].name = "SPAS-12 (AUTO)";					// 이름
	Weapon[25].model = "./data/model/weapon/1/spas.x";		// 3D 모델 파일 경로
	Weapon[25].texture = "./data/model/weapon/1/spas.bmp";		// 텍스처 파일 경로
	Weapon[25].attacks = 85;					// 공격력
	Weapon[25].penetration = 0;					// 관통력
	Weapon[25].blazings = 10;					// 연사 속도
	Weapon[25].speed = 9;						// 탄속
	Weapon[25].nbsmax = 8;						// 최대 장탄수
	Weapon[25].reloads = 20;					// 재장전 시간
	Weapon[25].reaction = 20;					// 반동
	Weapon[25].ErrorRangeMIN = 4;					// 최소 명중률 오차
	Weapon[25].ErrorRangeMAX = 23;					// 최대 명중률 오차
	Weapon[25].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[25].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[25].mz = 28.0f;						// 1인칭 손 위치 Z
	Weapon[25].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[25].flashy = 17.0f;					// 총구 화염 위치 Y
	Weapon[25].flashz = 106.0f;					// 총구 화염 위치 Z
	Weapon[25].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[25].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[25].yakkyou_pz = 42.0f;					// 탄피 배출 위치 Z
	Weapon[25].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[25].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[25].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[25].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[25].burstcnt = 0;					// 점사
	Weapon[25].scopemode = 0;					// 스코프 모드
	Weapon[25].size = 7.0f;						// 3인칭 모델 크기
	Weapon[25].soundid = 31;						// 발사음 ID
	Weapon[25].soundvolume = 90;				// 발사음 크기
	Weapon[25].silencer = false;					// 소음기 여부
	Weapon[25].WeaponP = 0;						// 무기 종류
	Weapon[25].ChangeWeaponID = 68;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[25].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[25].pellet = 6;						// 샷건 펠릿 수
	Weapon[25].is_shell_reload = true;				// 셸 단위 재장전 여부
	Weapon[25].bHasChamber = true;
	Weapon[25].TacticalReloadFrame = 40;
	Weapon[25].SoundID_Reload = -1;
	Weapon[25].SoundID_Tactical = -1;

	Weapon[26].name = "UMPSD";					// 이름
	Weapon[26].model = "./data/model/weapon/ump_acog_sd.x";		// 3D 모델 파일 경로
	Weapon[26].texture = "./data/model/weapon/ump.bmp";		// 텍스처 파일 경로
	Weapon[26].attacks = 48;					// 공격력
	Weapon[26].penetration = 0;					// 관통력
	Weapon[26].blazings = 5;					// 연사 속도
	Weapon[26].speed = 10;						// 탄속
	Weapon[26].nbsmax = 25;						// 최대 장탄수
	Weapon[26].reloads = 79;					// 재장전 시간
	Weapon[26].reaction = 7;					// 반동
	Weapon[26].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[26].ErrorRangeMAX = 19;					// 최대 명중률 오차
	Weapon[26].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[26].my = 5.0f;						// 1인칭 손 위치 Y
	Weapon[26].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[26].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[26].flashy = 16.5f;					// 총구 화염 위치 Y
	Weapon[26].flashz = 80.0f;					// 총구 화염 위치 Z
	Weapon[26].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[26].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[26].yakkyou_pz = 41.0f;					// 탄피 배출 위치 Z
	Weapon[26].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[26].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[26].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[26].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[26].burstcnt = 0;					// 점사
	Weapon[26].scopemode = 3;					// 스코프 모드
	Weapon[26].size = 7.0f;						// 3인칭 모델 크기
	Weapon[26].soundid = 33;					// 발사음 ID
	Weapon[26].soundvolume = 80;				// 발사음 크기
	Weapon[26].silencer = true;					// 소음기 여부
	Weapon[26].WeaponP = 0;						// 무기 종류
	Weapon[26].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[26].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[26].pellet = 1;						// 샷건 펠릿 수
	Weapon[26].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[26].bHasChamber = true;
	Weapon[26].TacticalReloadFrame = 57;
	Weapon[26].SoundID_Reload = -1;
	Weapon[26].SoundID_Tactical = -1;

	Weapon[27].name = "Mk14 EBR";    				// 이름
	Weapon[27].model = "./data/model/weapon/1/mk14_EBR.x";  		// 3D 모델 파일 경로
	Weapon[27].texture = "./data/model/weapon/1/mk14_EBR_acog_sd.bmp"; // 텍스처 파일 경로
	Weapon[27].attacks = 60;         				// 공격력
	Weapon[27].penetration = 3;      				// 관통력
	Weapon[27].blazings = 10;        				// 연사 속도
	Weapon[27].speed = 80;           				// 탄속
	Weapon[27].nbsmax = 20;          				// 최대 장탄수
	Weapon[27].reloads = 84;         				// 재장전 시간
	Weapon[27].reaction = 15;         				// 반동
	Weapon[27].ErrorRangeMIN = 0;    				// 최소 명중률 오차
	Weapon[27].ErrorRangeMAX = 19;   				// 최대 명중률 오차
	Weapon[27].mx = 9.0f;            				// 1인칭 손 위치 X
	Weapon[27].my = 7.0f;            				// 1인칭 손 위치 Y
	Weapon[27].mz = 28.0f;           				// 1인칭 손 위치 Z
	Weapon[27].flashx = 9.0f;        				// 총구 화염 위치 X
	Weapon[27].flashy = 17.0f;       				// 총구 화염 위치 Y
	Weapon[27].flashz = 99.0f;       				// 총구 화염 위치 Z
	Weapon[27].yakkyou_px = 10.0f;   				// 탄피 배출 위치 X
	Weapon[27].yakkyou_py = 12.0f;   				// 탄피 배출 위치 Y
	Weapon[27].yakkyou_pz = 40.0f;   				// 탄피 배출 위치 Z
	Weapon[27].yakkyou_sx = 4.0f;    				// 탄피 배출 속도 X
	Weapon[27].yakkyou_sy = 6.0f;    				// 탄피 배출 속도 Y
	Weapon[27].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[27].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[27].burstcnt = 0;         				// 점사
	Weapon[27].scopemode = 6;        				// 스코프 모드
	Weapon[27].size = 7.0f;          				// 3인칭 모델 크기
	Weapon[27].soundid = 16;          				// 발사음 ID
	Weapon[27].soundvolume = 90;     				// 발사음 크기
	Weapon[27].silencer = false;     				// 소음기 여부
	Weapon[27].WeaponP = 0;          				// 무기 종류
	Weapon[27].ChangeWeaponID = -1;  				// 발사모드 전환 시 바뀔 무기 ID
	Weapon[27].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[27].pellet = 1;           				// 샷건 펠릿 수
	Weapon[27].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[27].bHasChamber = true;
	Weapon[27].TacticalReloadFrame = 62;
	Weapon[27].SoundID_Reload = -1;
	Weapon[27].SoundID_Tactical = -1;

	Weapon[28].name = "KAC PDW (Std.)";					// 이름
	Weapon[28].model = "./data/model/weapon/1/kac_pdw_eotech_grip.x";	// 3D 모델 파일 경로
	Weapon[28].texture = "./data/model/weapon/1/kac_pdw.bmp";		// 텍스처 파일 경로
	Weapon[28].attacks = 56;					// 공격력
	Weapon[28].penetration = 1;					// 관통력
	Weapon[28].blazings = 5;					// 연사 속도
	Weapon[28].speed = 11;						// 탄속
	Weapon[28].nbsmax = 30;						// 최대 장탄수
	Weapon[28].reloads = 79;					// 재장전 시간
	Weapon[28].reaction = 6;					// 반동
	Weapon[28].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[28].ErrorRangeMAX = 15;					// 최대 명중률 오차
	Weapon[28].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[28].my = 7.0f;						// 1인칭 손 위치 Y
	Weapon[28].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[28].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[28].flashy = 16.5f;					// 총구 화염 위치 Y
	Weapon[28].flashz = 65.0f;					// 총구 화염 위치 Z
	Weapon[28].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[28].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[28].yakkyou_pz = 25.0f;					// 탄피 배출 위치 Z
	Weapon[28].yakkyou_sx = 3.0f;					// 탄피 배출 속도 X
	Weapon[28].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[28].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[28].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[28].burstcnt = 3;         				// 점사
	Weapon[28].scopemode = 2;        				// 스코프 모드
	Weapon[28].size = 7.0f;          				// 3인칭 모델 크기
	Weapon[28].soundid = 29;          				// 발사음 ID
	Weapon[28].soundvolume = 90;     				// 발사음 크기
	Weapon[28].silencer = false;     				// 소음기 여부
	Weapon[28].WeaponP = 0;          				// 무기 종류
	Weapon[28].ChangeWeaponID = 81;  				// 발사모드 전환 시 바뀔 무기 ID
	Weapon[28].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[28].pellet = 1;           				// 샷건 펠릿 수
	Weapon[28].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[28].bHasChamber = true;
	Weapon[28].TacticalReloadFrame = 57;
	Weapon[28].SoundID_Reload = -1;
	Weapon[28].SoundID_Tactical = -1;

	Weapon[29].name = "Galil";					// 이름
	Weapon[29].model = "./data/model/weapon/1/galil_762.x";		// 3D 모델 파일 경로
	Weapon[29].texture = "./data/model/weapon/1/galil.bmp";		// 텍스처 파일 경로
	Weapon[29].attacks = 63;					// 공격력
	Weapon[29].penetration = 1;					// 관통력
	Weapon[29].blazings = 4;					// 연사 속도
	Weapon[29].speed = 11;						// 탄속
	Weapon[29].nbsmax = 35;						// 최대 장탄수
	Weapon[29].reloads = 89;					// 재장전 시간
	Weapon[29].reaction = 9;					// 반동
	Weapon[29].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[29].ErrorRangeMAX = 20;					// 최대 명중률 오차
	Weapon[29].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[29].my = 9.0f;						// 1인칭 손 위치 Y
	Weapon[29].mz = 30.0f;						// 1인칭 손 위치 Z
	Weapon[29].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[29].flashy = 17.0f;					// 총구 화염 위치 Y
	Weapon[29].flashz = 96.0f;					// 총구 화염 위치 Z
	Weapon[29].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[29].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[29].yakkyou_pz = 41.0f;					// 탄피 배출 위치 Z
	Weapon[29].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[29].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[29].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[29].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[29].burstcnt = 0;					// 점사
	Weapon[29].scopemode = 0;					// 스코프 모드
	Weapon[29].size = 7.0f;						// 3인칭 모델 크기
	Weapon[29].soundid = 19;						// 발사음 ID
	Weapon[29].soundvolume = 90;				// 발사음 크기
	Weapon[29].silencer = false;					// 소음기 여부
	Weapon[29].WeaponP = 0;						// 무기 종류
	Weapon[29].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[29].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[29].pellet = 1;						// 샷건 펠릿 수
	Weapon[29].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[29].bHasChamber = true;
	Weapon[29].TacticalReloadFrame = 64;
	Weapon[29].SoundID_Reload = -1;
	Weapon[29].SoundID_Tactical = -1;

	Weapon[30].name = "M60";					// 이름
	Weapon[30].model = "./data/model/weapon/1/m60.x";			// 3D 모델 파일 경로
	Weapon[30].texture = "./data/model/weapon/1/m60.bmp";		// 텍스처 파일 경로
	Weapon[30].attacks = 64;					// 공격력
	Weapon[30].penetration = 4;					// 관통력
	Weapon[30].blazings = 4.5;					// 연사 속도
	Weapon[30].speed = 11;						// 탄속
	Weapon[30].nbsmax = 100;					// 최대 장탄수
	Weapon[30].reloads = 167;					// 재장전 시간
	Weapon[30].reaction = 8;					// 반동
	Weapon[30].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[30].ErrorRangeMAX = 20;					// 최대 명중률 오차
	Weapon[30].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[30].my = 7.0f;						// 1인칭 손 위치 Y
	Weapon[30].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[30].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[30].flashy = 17.0f;					// 총구 화염 위치 Y
	Weapon[30].flashz = 104.0f;					// 총구 화염 위치 Z
	Weapon[30].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[30].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[30].yakkyou_pz = 45.0f;					// 탄피 배출 위치 Z
	Weapon[30].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[30].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[30].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[30].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[30].burstcnt = 0;					// 점사
	Weapon[30].scopemode = 0;					// 스코프 모드
	Weapon[30].size = 7.0f;						// 3인칭 모델 크기
	Weapon[30].soundid = 3;						// 발사음 ID
	Weapon[30].soundvolume = 90;				// 발사음 크기
	Weapon[30].silencer = false;					// 소음기 여부
	Weapon[30].WeaponP = 0;						// 무기 종류
	Weapon[30].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[30].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[30].pellet = 1;						// 샷건 펠릿 수
	Weapon[30].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[30].bHasChamber = false;
	Weapon[30].TacticalReloadFrame = 0;
	Weapon[30].SoundID_Reload = -1;
	Weapon[30].SoundID_Tactical = -1;

	Weapon[31].name = "SIG 556 (Std.)";					// 이름
	Weapon[31].model = "./data/model/weapon/1/SIG556.x";		// 3D 모델 파일 경로
	Weapon[31].texture = "./data/model/weapon/1/SIG556.bmp";		// 텍스처 파일 경로
	Weapon[31].attacks = 57;					// 공격력
	Weapon[31].penetration = 1;					// 관통력
	Weapon[31].blazings = 5;					// 연사 속도
	Weapon[31].speed = 11;						// 탄속
	Weapon[31].nbsmax = 30;						// 최대 장탄수
	Weapon[31].reloads = 78;					// 재장전 시간
	Weapon[31].reaction = 6;					// 반동
	Weapon[31].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[31].ErrorRangeMAX = 15;					// 최대 명중률 오차
	Weapon[31].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[31].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[31].mz = 30.0f;						// 1인칭 손 위치 Z
	Weapon[31].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[31].flashy = 16.0f;					// 총구 화염 위치 Y
	Weapon[31].flashz = 106.0f;					// 총구 화염 위치 Z
	Weapon[31].yakkyou_px = 10.0f;					// 탄피 배출 위치 X
	Weapon[31].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[31].yakkyou_pz = 40.0f;					// 탄피 배출 위치 Z
	Weapon[31].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[31].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[31].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[31].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[31].burstcnt = 0;					// 점사
	Weapon[31].scopemode = 1;					// 스코프 모드
	Weapon[31].size = 7.0f;						// 3인칭 모델 크기
	Weapon[31].soundid = 20;						// 발사음 ID
	Weapon[31].soundvolume = 90;				// 발사음 크기
	Weapon[31].silencer = false;					// 소음기 여부
	Weapon[31].WeaponP = 0;						// 무기 종류
	Weapon[31].ChangeWeaponID = 32;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[31].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[31].pellet = 1;						// 샷건 펠릿 수
	Weapon[31].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[31].bHasChamber = true;
	Weapon[31].TacticalReloadFrame = 51;
	Weapon[31].SoundID_Reload = -1;
	Weapon[31].SoundID_Tactical = -1;

	Weapon[32].name = "SIG 556 (SD)";					// 이름
	Weapon[32].model = "./data/model/weapon/1/SIG556_sd.x";		// 3D 모델 파일 경로
	Weapon[32].texture = "./data/model/weapon/1/SIG556.bmp";		// 텍스처 파일 경로
	Weapon[32].attacks = 52;					// 공격력
	Weapon[32].penetration = 1;					// 관통력
	Weapon[32].blazings = 5;					// 연사 속도
	Weapon[32].speed = 11;						// 탄속
	Weapon[32].nbsmax = 30;						// 최대 장탄수
	Weapon[32].reloads = 78;					// 재장전 시간
	Weapon[32].reaction = 6;					// 반동
	Weapon[32].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[32].ErrorRangeMAX = 15;					// 최대 명중률 오차
	Weapon[32].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[32].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[32].mz = 30.0f;						// 1인칭 손 위치 Z
	Weapon[32].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[32].flashy = 16.0f;					// 총구 화염 위치 Y
	Weapon[32].flashz = 126.0f;					// 총구 화염 위치 Z
	Weapon[32].yakkyou_px = 10.0f;					// 탄피 배출 위치 X
	Weapon[32].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[32].yakkyou_pz = 40.0f;					// 탄피 배출 위치 Z
	Weapon[32].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[32].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[32].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[32].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[32].burstcnt = 0;					// 점사
	Weapon[32].scopemode = 1;					// 스코프 모드
	Weapon[32].size = 7.0f;						// 3인칭 모델 크기
	Weapon[32].soundid = 21;					// 발사음 ID
	Weapon[32].soundvolume = 80;				// 발사음 크기
	Weapon[32].silencer = true;					// 소음기 여부
	Weapon[32].WeaponP = 0;						// 무기 종류
	Weapon[32].ChangeWeaponID = 31;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[32].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[32].pellet = 1;						// 샷건 펠릿 수
	Weapon[32].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[32].bHasChamber = true;
	Weapon[32].TacticalReloadFrame = 51;
	Weapon[32].SoundID_Reload = -1;
	Weapon[32].SoundID_Tactical = -1;

	Weapon[33].name = "MP40";					// 이름
	Weapon[33].model = "./data/model/weapon/1/mp40.x";		// 3D 모델 파일 경로
	Weapon[33].texture = "./data/model/weapon/1/mp40.bmp";		// 텍스처 파일 경로
	Weapon[33].attacks = 38;					// 공격력
	Weapon[33].penetration = 0;					// 관통력
	Weapon[33].blazings = 4;					// 연사 속도
	Weapon[33].speed = 10;						// 탄속
	Weapon[33].nbsmax = 32;						// 최대 장탄수
	Weapon[33].reloads = 67;					// 재장전 시간
	Weapon[33].reaction = 5;					// 반동
	Weapon[33].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[33].ErrorRangeMAX = 25;					// 최대 명중률 오차
	Weapon[33].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[33].my = 7.0f;						// 1인칭 손 위치 Y
	Weapon[33].mz = 28.0f;						// 1인칭 손 위치 Z
	Weapon[33].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[33].flashy = 18.0f;					// 총구 화염 위치 Y
	Weapon[33].flashz = 83.0f;					// 총구 화염 위치 Z
	Weapon[33].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[33].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[33].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[33].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[33].yakkyou_sy = 7.0f;					// 탄피 배출 속도 Y
	Weapon[33].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[33].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[33].burstcnt = 0;					// 점사
	Weapon[33].scopemode = 0;					// 스코프 모드
	Weapon[33].size = 7.0f;						// 3인칭 모델 크기
	Weapon[33].soundid = 7;						// 발사음 ID
	Weapon[33].soundvolume = 90;				// 발사음 크기
	Weapon[33].silencer = false;					// 소음기 여부
	Weapon[33].WeaponP = 0;						// 무기 종류
	Weapon[33].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[33].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[33].pellet = 1;						// 샷건 펠릿 수
	Weapon[33].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[33].bHasChamber = false;
	Weapon[33].TacticalReloadFrame = 0;
	Weapon[33].SoundID_Reload = -1;
	Weapon[33].SoundID_Tactical = -1;

	Weapon[34].name = "MP7A1";					// 이름
	Weapon[34].model = "./data/model/weapon/1/mp7a1_aimpoint.x";	// 3D 모델 파일 경로
	Weapon[34].texture = "./data/model/weapon/1/mp7a1.bmp";		// 텍스처 파일 경로
	Weapon[34].attacks = 46;					// 공격력
	Weapon[34].penetration = 0;					// 관통력
	Weapon[34].blazings = 4;					// 연사 속도
	Weapon[34].speed = 10;						// 탄속
	Weapon[34].nbsmax = 30;						// 최대 장탄수
	Weapon[34].reloads = 79;					// 재장전 시간
	Weapon[34].reaction = 7;					// 반동
	Weapon[34].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[34].ErrorRangeMAX = 19;					// 최대 명중률 오차
	Weapon[34].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[34].my = 10.0f;						// 1인칭 손 위치 Y
	Weapon[34].mz = 35.0f;						// 1인칭 손 위치 Z
	Weapon[34].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[34].flashy = 16.0f;					// 총구 화염 위치 Y
	Weapon[34].flashz = 59.0f;					// 총구 화염 위치 Z
	Weapon[34].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[34].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[34].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[34].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[34].yakkyou_sy = 7.0f;					// 탄피 배출 속도 Y
	Weapon[34].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[34].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[34].burstcnt = 0;					// 점사
	Weapon[34].scopemode = 3;					// 스코프 모드
	Weapon[34].size = 7.0f;						// 3인칭 모델 크기
	Weapon[34].soundid = 27;						// 발사음 ID
	Weapon[34].soundvolume = 90;				// 발사음 크기
	Weapon[34].silencer = false;					// 소음기 여부
	Weapon[34].WeaponP = 0;						// 무기 종류
	Weapon[34].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[34].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[34].pellet = 1;						// 샷건 펠릿 수
	Weapon[34].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[34].bHasChamber = true;
	Weapon[34].TacticalReloadFrame = 57;
	Weapon[34].SoundID_Reload = -1;
	Weapon[34].SoundID_Tactical = -1;

	Weapon[35].name = "Saiga-12";					// 이름
	Weapon[35].model = "./data/model/weapon/2/saiga12.x";		// 3D 모델 파일 경로
	Weapon[35].texture = "./data/model/weapon/2/saiga12.bmp";		// 텍스처 파일 경로
	Weapon[35].attacks = 80;					// 공격력
	Weapon[35].penetration = 0;					// 관통력
	Weapon[35].blazings = 9;					// 연사 속도
	Weapon[35].speed = 9;						// 탄속
	Weapon[35].nbsmax = 8;						// 최대 장탄수
	Weapon[35].reloads = 82;					// 재장전 시간
	Weapon[35].reaction = 25;					// 반동
	Weapon[35].ErrorRangeMIN = 5;					// 최소 명중률 오차
	Weapon[35].ErrorRangeMAX = 27;					// 최대 명중률 오차
	Weapon[35].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[35].my = 6.0f;						// 1인칭 손 위치 Y
	Weapon[35].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[35].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[35].flashy = 14.0f;					// 총구 화염 위치 Y
	Weapon[35].flashz = 97.0f;					// 총구 화염 위치 Z
	Weapon[35].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[35].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[35].yakkyou_pz = 42.0f;					// 탄피 배출 위치 Z
	Weapon[35].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[35].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[35].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[35].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[35].burstcnt = 0;					// 점사
	Weapon[35].scopemode = 0;					// 스코프 모드
	Weapon[35].size = 7.0f;						// 3인칭 모델 크기
	Weapon[35].soundid = 30;						// 발사음 ID
	Weapon[35].soundvolume = 90;				// 발사음 크기
	Weapon[35].silencer = false;					// 소음기 여부
	Weapon[35].WeaponP = 0;						// 무기 종류
	Weapon[35].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[35].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[35].pellet = 6;						// 샷건 펠릿 수
	Weapon[35].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[35].bHasChamber = true;
	Weapon[35].TacticalReloadFrame = 57;
	Weapon[35].SoundID_Reload = -1;
	Weapon[35].SoundID_Tactical = -1;

	Weapon[36].name = "Winchester1873";				// 이름
	Weapon[36].model = "./data/model/weapon/2/winchester1873_scope.x";	// 3D 모델 파일 경로
	Weapon[36].texture = "./data/model/weapon/2/Winchester1873.bmp"; // 텍스처 파일 경로
	Weapon[36].attacks = 70;					// 공격력
	Weapon[36].penetration = 2;					// 관통력
	Weapon[36].blazings = 25;					// 연사 속도
	Weapon[36].speed = 80;						// 탄속
	Weapon[36].nbsmax = 15;						// 최대 장탄수
	Weapon[36].reloads = 10;					// 재장전 시간
	Weapon[36].reaction = 10;					// 반동
	Weapon[36].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[36].ErrorRangeMAX = 20;					// 최대 명중률 오차
	Weapon[36].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[36].my = 4.0f;						// 1인칭 손 위치 Y
	Weapon[36].mz = 18.0f;						// 1인칭 손 위치 Z
	Weapon[36].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[36].flashy = 16.5f;					// 총구 화염 위치 Y
	Weapon[36].flashz = 101.0f;					// 총구 화염 위치 Z
	Weapon[36].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[36].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[36].yakkyou_pz = 42.0f;					// 탄피 배출 위치 Z
	Weapon[36].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[36].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[36].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[36].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[36].burstcnt = 4;					// 점사
	Weapon[36].scopemode = 4;					// 스코프 모드
	Weapon[36].size = 7.0f;						// 3인칭 모델 크기
	Weapon[36].soundid = 11;						// 발사음 ID
	Weapon[36].soundvolume = 90;				// 발사음 크기
	Weapon[36].silencer = false;					// 소음기 여부
	Weapon[36].WeaponP = 0;						// 무기 종류
	Weapon[36].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[36].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[36].pellet = 1;						// 샷건 펠릿 수
	Weapon[36].is_shell_reload = true;				// 셸 단위 재장전 여부
	Weapon[36].bHasChamber = true;
	Weapon[36].TacticalReloadFrame = 0;
	Weapon[36].SoundID_Reload = -1;
	Weapon[36].SoundID_Tactical = -1;

	Weapon[37].name = "SCAR (Std.)";					// 이름
	Weapon[37].model = "./data/model/weapon/2/scar_acog.x";		// 3D 모델 파일 경로
	Weapon[37].texture = "./data/model/weapon/2/scar_tan.bmp";	// 텍스처 파일 경로
	Weapon[37].attacks = 60;					// 공격력
	Weapon[37].penetration = 1;					// 관통력
	Weapon[37].blazings = 5.5;					// 연사 속도
	Weapon[37].speed = 11;						// 탄속
	Weapon[37].nbsmax = 30;						// 최대 장탄수
	Weapon[37].reloads = 78;					// 재장전 시간
	Weapon[37].reaction = 7;					// 반동
	Weapon[37].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[37].ErrorRangeMAX = 17;					// 최대 명중률 오차
	Weapon[37].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[37].my = 5.0f;						// 1인칭 손 위치 Y
	Weapon[37].mz = 28.0f;						// 1인칭 손 위치 Z
	Weapon[37].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[37].flashy = 14.5f;					// 총구 화염 위치 Y
	Weapon[37].flashz = 75.0f;					// 총구 화염 위치 Z
	Weapon[37].yakkyou_px = 10.0f;					// 탄피 배출 위치 X
	Weapon[37].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[37].yakkyou_pz = 40.0f;					// 탄피 배출 위치 Z
	Weapon[37].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[37].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[37].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[37].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[37].burstcnt = 0;					// 점사
	Weapon[37].scopemode = 4;					// 스코프 모드
	Weapon[37].size = 7.0f;						// 3인칭 모델 크기
	Weapon[37].soundid = 25;						// 발사음 ID
	Weapon[37].soundvolume = 90;				// 발사음 크기
	Weapon[37].silencer = false;					// 소음기 여부
	Weapon[37].WeaponP = 0;						// 무기 종류
	Weapon[37].ChangeWeaponID = 38;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[37].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[37].pellet = 1;						// 샷건 펠릿 수
	Weapon[37].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[37].bHasChamber = true;
	Weapon[37].TacticalReloadFrame = 51;
	Weapon[37].SoundID_Reload = -1;
	Weapon[37].SoundID_Tactical = -1;

	Weapon[38].name = "SCAR (SD)";					// 이름
	Weapon[38].model = "./data/model/weapon/2/scar_acog_sd.x";	// 3D 모델 파일 경로
	Weapon[38].texture = "./data/model/weapon/2/scar_tan.bmp";	// 텍스처 파일 경로
	Weapon[38].attacks = 54;					// 공격력
	Weapon[38].penetration = 1;					// 관통력
	Weapon[38].blazings = 5.5;					// 연사 속도
	Weapon[38].speed = 11;						// 탄속
	Weapon[38].nbsmax = 30;						// 최대 장탄수
	Weapon[38].reloads = 78;					// 재장전 시간
	Weapon[38].reaction = 7;					// 반동
	Weapon[38].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[38].ErrorRangeMAX = 17;					// 최대 명중률 오차
	Weapon[38].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[38].my = 5.0f;						// 1인칭 손 위치 Y
	Weapon[38].mz = 28.0f;						// 1인칭 손 위치 Z
	Weapon[38].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[38].flashy = 14.5f;					// 총구 화염 위치 Y
	Weapon[38].flashz = 92.0f;					// 총구 화염 위치 Z
	Weapon[38].yakkyou_px = 10.0f;					// 탄피 배출 위치 X
	Weapon[38].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[38].yakkyou_pz = 40.0f;					// 탄피 배출 위치 Z
	Weapon[38].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[38].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[38].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[38].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[38].burstcnt = 0;					// 점사
	Weapon[38].scopemode = 4;					// 스코프 모드
	Weapon[38].size = 7.0f;						// 3인칭 모델 크기
	Weapon[38].soundid = 26;					// 발사음 ID
	Weapon[38].soundvolume = 80;				// 발사음 크기
	Weapon[38].silencer = true;					// 소음기 여부
	Weapon[38].WeaponP = 0;						// 무기 종류
	Weapon[38].ChangeWeaponID = 37;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[38].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[38].pellet = 1;						// 샷건 펠릿 수
	Weapon[38].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[38].bHasChamber = true;
	Weapon[38].TacticalReloadFrame = 51;
	Weapon[38].SoundID_Reload = -1;
	Weapon[38].SoundID_Tactical = -1;

	Weapon[39].name = "SVD";					// 이름
	Weapon[39].model = "./data/model/weapon/2/SVD_scope.x";		// 3D 모델 파일 경로
	Weapon[39].texture = "./data/model/weapon/2/SVD.bmp";		// 텍스처 파일 경로
	Weapon[39].attacks = 60;					// 공격력
	Weapon[39].penetration = 4;					// 관통력
	Weapon[39].blazings = 13;					// 연사 속도
	Weapon[39].speed = 255;						// 탄속
	Weapon[39].nbsmax = 10;						// 최대 장탄수
	Weapon[39].reloads = 84;					// 재장전 시간
	Weapon[39].reaction = 15;					// 반동
	Weapon[39].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[39].ErrorRangeMAX = 19;					// 최대 명중률 오차
	Weapon[39].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[39].my = 5.0f;						// 1인칭 손 위치 Y
	Weapon[39].mz = 28.0f;						// 1인칭 손 위치 Z
	Weapon[39].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[39].flashy = 14.5f;					// 총구 화염 위치 Y
	Weapon[39].flashz = 125.0f;					// 총구 화염 위치 Z
	Weapon[39].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[39].yakkyou_py = 13.0f;					// 탄피 배출 위치 Y
	Weapon[39].yakkyou_pz = 38.0f;					// 탄피 배출 위치 Z
	Weapon[39].yakkyou_sx = 6.0f;					// 탄피 배출 속도 X
	Weapon[39].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[39].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[39].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[39].burstcnt = 1;					// 점사
	Weapon[39].scopemode = 5;					// 스코프 모드
	Weapon[39].size = 7.0f;						// 3인칭 모델 크기
	Weapon[39].soundid = 15;						// 발사음 ID
	Weapon[39].soundvolume = 90;				// 발사음 크기
	Weapon[39].silencer = false;					// 소음기 여부
	Weapon[39].WeaponP = 0;						// 무기 종류
	Weapon[39].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[39].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[39].pellet = 1;						// 샷건 펠릿 수
	Weapon[39].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[39].bHasChamber = true;
	Weapon[39].TacticalReloadFrame = 62;
	Weapon[39].SoundID_Reload = -1;
	Weapon[39].SoundID_Tactical = -1;

	Weapon[40].name = "AWP";					// 이름
	Weapon[40].model = "./data/model/weapon/2/awp.x";			// 3D 모델 파일 경로
	Weapon[40].texture = "./data/model/weapon/2/awp.bmp";		// 텍스처 파일 경로
	Weapon[40].attacks = 100;					// 공격력
	Weapon[40].penetration = 5;					// 관통력
	Weapon[40].blazings = 40;					// 연사 속도(값이 작을수록 빠름)
	Weapon[40].speed = 255;						// 탄속
	Weapon[40].nbsmax = 10;						// 최대 장탄수
	Weapon[40].reloads = 96;					// 재장전 시간
	Weapon[40].reaction = 17;					// 반동
	Weapon[40].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[40].ErrorRangeMAX = 19;					// 최대 명중률 오차
	Weapon[40].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[40].my = 5.0f;						// 1인칭 손 위치 Y
	Weapon[40].mz = 28.0f;						// 1인칭 손 위치 Z
	Weapon[40].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[40].flashy = 15.0f;					// 총구 화염 위치 Y
	Weapon[40].flashz = 109.0f;					// 총구 화염 위치 Z
	Weapon[40].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[40].yakkyou_py = 13.0f;					// 탄피 배출 위치 Y
	Weapon[40].yakkyou_pz = 38.0f;					// 탄피 배출 위치 Z
	Weapon[40].yakkyou_sx = 6.0f;					// 탄피 배출 속도 X
	Weapon[40].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[40].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[40].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[40].burstcnt = 4;					// 점사 (0: 자동, 1: 단발, 3: 3점사)
	Weapon[40].scopemode = 7;					 // 스코프 모드 (0: 자동, 1:acog, 2:저격, 3:레드닷)
	Weapon[40].size = 7.0f;						// 3인칭 모델 크기
	Weapon[40].soundid = 8;						// 발사음 ID (0:mp5,1 :ak47,2 :이글,3 :m249, 4 :AUG, 13 :소음기)
	Weapon[40].soundvolume = 90;				// 발사음 크기
	Weapon[40].silencer = false;					// 소음기 여부
	Weapon[40].WeaponP = 0;						// 무기 종류 (0: 양손, 1: 한손)
	Weapon[40].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[40].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[40].pellet = 1;						// 샷건 펠릿 수
	Weapon[40].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[40].bHasChamber = true;
	Weapon[40].TacticalReloadFrame = 66;
	Weapon[40].SoundID_Reload = -1;
	Weapon[40].SoundID_Tactical = -1;

	Weapon[41].name = "HK 416 (Std.)";					// 이름
	Weapon[41].model = "./data/model/weapon/2/HK416D_eotech.x";		// 3D 모델 파일 경로
	Weapon[41].texture = "./data/model/weapon/2/HK416D.bmp";		// 텍스처 파일 경로
	Weapon[41].attacks = 61;					// 공격력
	Weapon[41].penetration = 1;					// 관통력
	Weapon[41].blazings = 4;					// 연사 속도
	Weapon[41].speed = 11;						// 탄속
	Weapon[41].nbsmax = 30;						// 최대 장탄수
	Weapon[41].reloads = 78;					// 재장전 시간
	Weapon[41].reaction = 7;					// 반동
	Weapon[41].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[41].ErrorRangeMAX = 15;					// 최대 명중률 오차
	Weapon[41].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[41].my = 5.0f;						// 1인칭 손 위치 Y
	Weapon[41].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[41].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[41].flashy = 14.5f;					// 총구 화염 위치 Y
	Weapon[41].flashz = 89.0f;					// 총구 화염 위치 Z
	Weapon[41].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[41].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[41].yakkyou_pz = 41.0f;					// 탄피 배출 위치 Z
	Weapon[41].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[41].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[41].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[41].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[41].burstcnt = 0;					// 점사
	Weapon[41].scopemode = 2;					// 스코프 모드
	Weapon[41].size = 7.0f;						// 3인칭 모델 크기
	Weapon[41].soundid = 37;						// 발사음 ID
	Weapon[41].soundvolume = 90;				// 발사음 크기
	Weapon[41].silencer = false;					// 소음기 여부
	Weapon[41].WeaponP = 0;						// 무기 종류
	Weapon[41].ChangeWeaponID = 42;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[41].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[41].pellet = 1;						// 샷건 펠릿 수
	Weapon[41].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[41].bHasChamber = true;
	Weapon[41].TacticalReloadFrame = 51;
	Weapon[41].SoundID_Reload = -1;
	Weapon[41].SoundID_Tactical = -1;

	Weapon[42].name = "HK 416 (SD)";					// 이름
	Weapon[42].model = "./data/model/weapon/2/HK416D_eotech_sd.x";		// 3D 모델 파일 경로
	Weapon[42].texture = "./data/model/weapon/2/HK416D.bmp";		// 텍스처 파일 경로
	Weapon[42].attacks = 55;					// 공격력
	Weapon[42].penetration = 1;					// 관통력
	Weapon[42].blazings = 4;					// 연사 속도
	Weapon[42].speed = 11;						// 탄속
	Weapon[42].nbsmax = 30;						// 최대 장탄수
	Weapon[42].reloads = 78;					// 재장전 시간
	Weapon[42].reaction = 7;					// 반동
	Weapon[42].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[42].ErrorRangeMAX = 15;					// 최대 명중률 오차
	Weapon[42].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[42].my = 5.0f;						// 1인칭 손 위치 Y
	Weapon[42].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[42].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[42].flashy = 14.5f;					// 총구 화염 위치 Y
	Weapon[42].flashz = 103.0f;					// 총구 화염 위치 Z
	Weapon[42].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[42].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[42].yakkyou_pz = 41.0f;					// 탄피 배출 위치 Z
	Weapon[42].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[42].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[42].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[42].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[42].burstcnt = 0;					// 점사
	Weapon[42].scopemode = 2;					// 스코프 모드
	Weapon[42].size = 7.0f;						// 3인칭 모델 크기
	Weapon[42].soundid = 38;						// 발사음 ID
	Weapon[42].soundvolume = 80;				// 발사음 크기
	Weapon[42].silencer = true;					// 소음기 여부
	Weapon[42].WeaponP = 0;						// 무기 종류
	Weapon[42].ChangeWeaponID = 41;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[42].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[42].pellet = 1;						// 샷건 펠릿 수
	Weapon[42].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[42].bHasChamber = true;
	Weapon[42].TacticalReloadFrame = 51;
	Weapon[42].SoundID_Reload = -1;
	Weapon[42].SoundID_Tactical = -1;

	Weapon[43].name = "M16A4 (Std.)";						// 이름
	Weapon[43].model = "./data/model/weapon/2/M16A4_Acog.x";		// 3D 모델 파일 경로
	Weapon[43].texture = "./data/model/weapon/2/M16A4.bmp";		// 텍스처 파일 경로
	Weapon[43].attacks = 67;						// 공격력
	Weapon[43].penetration = 1;					// 관통력
	Weapon[43].blazings = 4;						// 연사 속도
	Weapon[43].speed = 11;						// 탄속
	Weapon[43].nbsmax = 30;						// 최대 장탄수
	Weapon[43].reloads = 78;						// 재장전 시간
	Weapon[43].reaction = 6;						// 반동
	Weapon[43].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[43].ErrorRangeMAX = 19;					// 최대 명중률 오차
	Weapon[43].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[43].my = 5.0f;						// 1인칭 손 위치 Y
	Weapon[43].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[43].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[43].flashy = 14.5f;					// 총구 화염 위치 Y
	Weapon[43].flashz = 103.0f;					// 총구 화염 위치 Z
	Weapon[43].yakkyou_px = 10.0f;					// 탄피 배출 위치 X
	Weapon[43].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[43].yakkyou_pz = 40.0f;					// 탄피 배출 위치 Z
	Weapon[43].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[43].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[43].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[43].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[43].burstcnt = 3;						// 점사
	Weapon[43].scopemode = 5;					// 스코프 모드
	Weapon[43].size = 7.0f;						// 3인칭 모델 크기
	Weapon[43].soundid = 41;						// 발사음 ID
	Weapon[43].soundvolume = 90;				// 발사음 크기
	Weapon[43].silencer = false;					// 소음기 여부
	Weapon[43].WeaponP = 0;						// 무기 종류
	Weapon[43].ChangeWeaponID = 87;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[43].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[43].pellet = 1;						// 샷건 펠릿 수
	Weapon[43].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[43].bHasChamber = true;
	Weapon[43].TacticalReloadFrame = 51;
	Weapon[43].SoundID_Reload = -1;
	Weapon[43].SoundID_Tactical = -1;

	Weapon[44].name = "G36K (Std.)";					// 이름
	Weapon[44].model = "./data/model/weapon/2/G36K_Acog.x";			// 3D 모델 파일 경로
	Weapon[44].texture = "./data/model/weapon/2/G36K.bmp";		// 텍스처 파일 경로
	Weapon[44].attacks = 57;					// 공격력
	Weapon[44].penetration = 1;					// 관통력
	Weapon[44].blazings = 4;					// 연사 속도
	Weapon[44].speed = 11;						// 탄속
	Weapon[44].nbsmax = 30;						// 최대 장탄수
	Weapon[44].reloads = 78;					// 재장전 시간
	Weapon[44].reaction = 8;					// 반동
	Weapon[44].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[44].ErrorRangeMAX = 20;					// 최대 명중률 오차
	Weapon[44].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[44].my = 5.0f;						// 1인칭 손 위치 Y
	Weapon[44].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[44].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[44].flashy = 14.5f;					// 총구 화염 위치 Y
	Weapon[44].flashz = 83.0f;					// 총구 화염 위치 Z
	Weapon[44].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[44].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[44].yakkyou_pz = 15.0f;					// 탄피 배출 위치 Z
	Weapon[44].yakkyou_sx = 3.0f;					// 탄피 배출 속도 X
	Weapon[44].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[44].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[44].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[44].burstcnt = 0;					// 점사
	Weapon[44].scopemode = 3;					// 스코프 모드
	Weapon[44].size = 7.0f;						// 3인칭 모델 크기
	Weapon[44].soundid = 40;						// 발사음 ID
	Weapon[44].soundvolume = 90;				// 발사음 크기
	Weapon[44].silencer = false;					// 소음기 여부
	Weapon[44].WeaponP = 0;						// 무기 종류
	Weapon[44].ChangeWeaponID = 88;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[44].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[44].pellet = 1;						// 샷건 펠릿 수
	Weapon[44].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[44].bHasChamber = true;
	Weapon[44].TacticalReloadFrame = 51;
	Weapon[44].SoundID_Reload = -1;
	Weapon[44].SoundID_Tactical = -1;

	Weapon[45].name = "F2000";					// 이름
	Weapon[45].model = "./data/model/weapon/2/F2000_scope.x";			// 3D 모델 파일 경로
	Weapon[45].texture = "./data/model/weapon/2/F2000_scope_sd.bmp";		// 텍스처 파일 경로
	Weapon[45].attacks = 61;					// 공격력
	Weapon[45].penetration = 1;					// 관통력
	Weapon[45].blazings = 5;					// 연사 속도
	Weapon[45].speed = 11;						// 탄속
	Weapon[45].nbsmax = 30;						// 최대 장탄수
	Weapon[45].reloads = 91;					// 재장전 시간
	Weapon[45].reaction = 6;					// 반동
	Weapon[45].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[45].ErrorRangeMAX = 25;					// 최대 명중률 오차
	Weapon[45].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[45].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[45].mz = 31.0f;						// 1인칭 손 위치 Z
	Weapon[45].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[45].flashy = 17.0f;					// 총구 화염 위치 Y
	Weapon[45].flashz = 84.0f;					// 총구 화염 위치 Z
	Weapon[45].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[45].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[45].yakkyou_pz = 15.0f;					// 탄피 배출 위치 Z
	Weapon[45].yakkyou_sx = 3.0f;					// 탄피 배출 속도 X
	Weapon[45].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[45].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[45].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[45].burstcnt = 0;					// 점사
	Weapon[45].scopemode = 5;					// 스코프 모드
	Weapon[45].size = 7.0f;						// 3인칭 모델 크기
	Weapon[45].soundid = 43;						// 발사음 ID
	Weapon[45].soundvolume = 80;				// 발사음 크기
	Weapon[45].silencer = true;					// 소음기 여부
	Weapon[45].WeaponP = 0;						// 무기 종류
	Weapon[45].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[45].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[45].pellet = 1;						// 샷건 펠릿 수
	Weapon[45].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[45].bHasChamber = true;
	Weapon[45].TacticalReloadFrame = 67;
	Weapon[45].SoundID_Reload = -1;
	Weapon[45].SoundID_Tactical = -1;

	Weapon[46].name = "TAR-21 (Std.)";					// 이름
	Weapon[46].model = "./data/model/weapon/3/TAR21_eotech.x";		// 3D 모델 파일 경로
	Weapon[46].texture = "./data/model/weapon/3/TAR21.bmp";		// 텍스처 파일 경로
	Weapon[46].attacks = 26;					// 공격력
	Weapon[46].penetration = 2;					// 관통력
	Weapon[46].blazings = 4;					// 연사 속도
	Weapon[46].speed = 11;						// 탄속
	Weapon[46].nbsmax = 30;						// 최대 장탄수
	Weapon[46].reloads = 91;					// 재장전 시간
	Weapon[46].reaction = 8;					// 반동
	Weapon[46].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[46].ErrorRangeMAX = 20;					// 최대 명중률 오차
	Weapon[46].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[46].my = 9.0f;						// 1인칭 손 위치 Y
	Weapon[46].mz = 31.0f;						// 1인칭 손 위치 Z
	Weapon[46].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[46].flashy = 16.0f;					// 총구 화염 위치 Y
	Weapon[46].flashz = 65.0f;					// 총구 화염 위치 Z
	Weapon[46].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[46].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[46].yakkyou_pz = 41.0f;					// 탄피 배출 위치 Z
	Weapon[46].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[46].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[46].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[46].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[46].burstcnt = 0;					// 점사
	Weapon[46].scopemode = 1;					// 스코프 모드
	Weapon[46].size = 7.0f;						// 3인칭 모델 크기
	Weapon[46].soundid = 39;						// 발사음 ID
	Weapon[46].soundvolume = 90;				// 발사음 크기
	Weapon[46].silencer = false;					// 소음기 여부
	Weapon[46].WeaponP = 0;						// 무기 종류
	Weapon[46].ChangeWeaponID = 89;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[46].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[46].pellet = 3;						// 샷건 펠릿 수
	Weapon[46].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[46].bHasChamber = true;
	Weapon[46].TacticalReloadFrame = 67;
	Weapon[46].SoundID_Reload = -1;
	Weapon[46].SoundID_Tactical = -1;

	Weapon[47].name = "KRISS Vector (Std.)";						// 이름
	Weapon[47].model = "./data/model/weapon/3/KRISS_Vector_eotech.x";			// 3D 모델 파일 경로
	Weapon[47].texture = "./data/model/weapon/3/KRISS_Vector.bmp";		// 텍스처 파일 경로
	Weapon[47].attacks = 41;						// 공격력
	Weapon[47].penetration = 0;					// 관통력
	Weapon[47].blazings = 3;						// 연사 속도 (값이 작을수록 빠름)
	Weapon[47].speed = 10;						// 탄속
	Weapon[47].nbsmax = 30;						// 최대 장탄수
	Weapon[47].reloads = 79;						// 재장전 시간
	Weapon[47].reaction = 8;						// 반동
	Weapon[47].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[47].ErrorRangeMAX = 25;					// 최대 명중률 오차
	Weapon[47].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[47].my = 12.0f;						// 1인칭 손 위치 Y
	Weapon[47].mz = 28.0f;						// 1인칭 손 위치 Z
	Weapon[47].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[47].flashy = 15.0f;					// 총구 화염 위치 Y
	Weapon[47].flashz = 62.0f;					// 총구 화염 위치 Z
	Weapon[47].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[47].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[47].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[47].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[47].yakkyou_sy = 7.0f;					// 탄피 배출 속도 Y
	Weapon[47].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[47].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[47].burstcnt = 0;						// 점사 (0: 자동, 1: 단발, 3: 3점사, 4: 볼트액션)
	Weapon[47].scopemode = 1;					// 스코프 모드
	Weapon[47].size = 7.0f;						// 3인칭 모델 크기
	Weapon[47].soundid = 44;						// 발사음 ID
	Weapon[47].soundvolume = 90;				// 발사음 크기
	Weapon[47].silencer = false;					// 소음기 여부
	Weapon[47].WeaponP = 0;						// 무기 종류 (0: 양손, 1: 한손)
	Weapon[47].ChangeWeaponID = 82;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[47].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[47].pellet = 1;						// 샷건 펠릿 수
	Weapon[47].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[47].bHasChamber = true;
	Weapon[47].TacticalReloadFrame = 57;
	Weapon[47].SoundID_Reload = -1;
	Weapon[47].SoundID_Tactical = -1;

	Weapon[48].name = "UZI (Std.)";					// 이름
	Weapon[48].model = "./data/model/weapon/3/mini_UZI_stock.x";		// 3D 모델 파일 경로
	Weapon[48].texture = "./data/model/weapon/3/mini_UZI.bmp";		// 텍스처 파일 경로
	Weapon[48].attacks = 43;						// 공격력
	Weapon[48].penetration = 0;					// 관통력
	Weapon[48].blazings = 3.5;						// 연사 속도
	Weapon[48].speed = 10;						// 탄속
	Weapon[48].nbsmax = 25;						// 최대 장탄수
	Weapon[48].reloads = 67;						// 재장전 시간
	Weapon[48].reaction = 7;						// 반동
	Weapon[48].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[48].ErrorRangeMAX = 25;					// 최대 명중률 오차
	Weapon[48].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[48].my = 6.0f;						// 1인칭 손 위치 Y
	Weapon[48].mz = 38.0f;						// 1인칭 손 위치 Z
	Weapon[48].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[48].flashy = 13.0f;					// 총구 화염 위치 Y
	Weapon[48].flashz = 57.0f;					// 총구 화염 위치 Z
	Weapon[48].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[48].yakkyou_py = 13.0f;					// 탄피 배출 위치 Y
	Weapon[48].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[48].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[48].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[48].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[48].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[48].burstcnt = 0;						// 점사
	Weapon[48].scopemode = 0;					// 스코프 모드
	Weapon[48].size = 7.0f;						// 3인칭 모델 크기
	Weapon[48].soundid = 45;						// 발사음 ID
	Weapon[48].soundvolume = 90;				// 발사음 크기
	Weapon[48].silencer = false;					// 소음기 여부
	Weapon[48].WeaponP = 0;						// 무기 종류
	Weapon[48].ChangeWeaponID = 83;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[48].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[48].pellet = 1;						// 샷건 펠릿 수
	Weapon[48].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[48].bHasChamber = false;
	Weapon[48].TacticalReloadFrame = 0;
	Weapon[48].SoundID_Reload = -1;
	Weapon[48].SoundID_Tactical = -1;

	Weapon[49].name = "Jackhammer";					// 이름
	Weapon[49].model = "./data/model/weapon/3/Jackhammer.x";		// 3D 모델 파일 경로
	Weapon[49].texture = "./data/model/weapon/3/Jackhammer.bmp";		// 텍스처 파일 경로
	Weapon[49].attacks = 75;					// 공격력
	Weapon[49].penetration = 0;					// 관통력
	Weapon[49].blazings = 12;					// 연사 속도
	Weapon[49].speed = 9;						// 탄속
	Weapon[49].nbsmax = 10;						// 최대 장탄수
	Weapon[49].reloads = 102;					// 재장전 시간
	Weapon[49].reaction = 25;					// 반동
	Weapon[49].ErrorRangeMIN = 3;					// 최소 명중률 오차
	Weapon[49].ErrorRangeMAX = 27;					// 최대 명중률 오차
	Weapon[49].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[49].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[49].mz = 30.0f;						// 1인칭 손 위치 Z
	Weapon[49].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[49].flashy = 18.0f;					// 총구 화염 위치 Y
	Weapon[49].flashz = 71.0f;					// 총구 화염 위치 Z
	Weapon[49].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[49].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[49].yakkyou_pz = 42.0f;					// 탄피 배출 위치 Z
	Weapon[49].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[49].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[49].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[49].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[49].burstcnt = 0;					// 점사
	Weapon[49].scopemode = 0;					// 스코프 모드
	Weapon[49].size = 7.0f;						// 3인칭 모델 크기
	Weapon[49].soundid = 48;						// 발사음 ID
	Weapon[49].soundvolume = 90;				// 발사음 크기
	Weapon[49].silencer = false;					// 소음기 여부
	Weapon[49].WeaponP = 0;						// 무기 종류
	Weapon[49].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[49].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[49].pellet = 7;						// 샷건 펠릿 수
	Weapon[49].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[49].bHasChamber = false;
	Weapon[49].TacticalReloadFrame = 0;
	Weapon[49].SoundID_Reload = -1;
	Weapon[49].SoundID_Tactical = -1;

	Weapon[50].name = "Double Barrel";					// 이름
	Weapon[50].model = "./data/model/weapon/3/Double_Barrel.x";		// 3D 모델 파일 경로
	Weapon[50].texture = "./data/model/weapon/3/Double_Barrel.bmp";		// 텍스처 파일 경로
	Weapon[50].attacks = 75;					// 공격력
	Weapon[50].penetration = 1;					// 관통력
	Weapon[50].blazings = 15;					// 연사 속도
	Weapon[50].speed = 9;						// 탄속
	Weapon[50].nbsmax = 2;						// 최대 장탄수
	Weapon[50].reloads = 69;					// 재장전 시간
	Weapon[50].reaction = 15;					// 반동
	Weapon[50].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[50].ErrorRangeMAX = 27;					// 최대 명중률 오차
	Weapon[50].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[50].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[50].mz = 28.0f;						// 1인칭 손 위치 Z
	Weapon[50].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[50].flashy = 15.0f;					// 총구 화염 위치 Y
	Weapon[50].flashz = 102.0f;					// 총구 화염 위치 Z
	Weapon[50].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[50].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[50].yakkyou_pz = 42.0f;					// 탄피 배출 위치 Z
	Weapon[50].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[50].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[50].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[50].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[50].burstcnt = 0;					// 점사
	Weapon[50].scopemode = 0;					// 스코프 모드
	Weapon[50].size = 7.0f;						// 3인칭 모델 크기
	Weapon[50].soundid = 49;						// 발사음 ID
	Weapon[50].soundvolume = 90;				// 발사음 크기
	Weapon[50].silencer = false;					// 소음기 여부
	Weapon[50].WeaponP = 0;						// 무기 종류
	Weapon[50].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[50].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[50].pellet = 8;						// 샷건 펠릿 수
	Weapon[50].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[50].bHasChamber = false;
	Weapon[50].TacticalReloadFrame = 0;
	Weapon[50].SoundID_Reload = -1;
	Weapon[50].SoundID_Tactical = -1;

	Weapon[51].name = "MP9 (SD)";					// 이름
	Weapon[51].model = "./data/model/weapon/3/MP9_sd.x";		// 3D 모델 파일 경로
	Weapon[51].texture = "./data/model/weapon/3/MP9.bmp";		// 텍스처 파일 경로
	Weapon[51].attacks = 41;						// 공격력
	Weapon[51].penetration = 0;					// 관통력
	Weapon[51].blazings = 4;						// 연사 속도
	Weapon[51].speed = 10;						// 탄속
	Weapon[51].nbsmax = 20;						// 최대 장탄수
	Weapon[51].reloads = 79;						// 재장전 시간
	Weapon[51].reaction = 6;						// 반동
	Weapon[51].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[51].ErrorRangeMAX = 20;					// 최대 명중률 오차
	Weapon[51].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[51].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[51].mz = 45.0f;						// 1인칭 손 위치 Z
	Weapon[51].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[51].flashy = 14.5f;					// 총구 화염 위치 Y
	Weapon[51].flashz = 82.0f;					// 총구 화염 위치 Z
	Weapon[51].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[51].yakkyou_py = 13.0f;					// 탄피 배출 위치 Y
	Weapon[51].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[51].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[51].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[51].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[51].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[51].burstcnt = 0;						// 점사
	Weapon[51].scopemode = 0;					// 스코프 모드
	Weapon[51].size = 7.0f;						// 3인칭 모델 크기
	Weapon[51].soundid = 42;						// 발사음 ID
	Weapon[51].soundvolume = 80;				// 발사음 크기
	Weapon[51].silencer = true;					// 소음기 여부
	Weapon[51].WeaponP = 0;						// 무기 종류
	Weapon[51].ChangeWeaponID = 78;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[51].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[51].pellet = 1;						// 샷건 펠릿 수
	Weapon[51].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[51].bHasChamber = true;
	Weapon[51].TacticalReloadFrame = 57;
	Weapon[51].SoundID_Reload = -1;
	Weapon[51].SoundID_Tactical = -1;

	Weapon[52].name = "Beretta 93R (Std.)";					// 이름
	Weapon[52].model = "./data/model/weapon/3/93R.x";		// 3D 모델 파일 경로
	Weapon[52].texture = "./data/model/weapon/3/93R.bmp";		// 텍스처 파일 경로
	Weapon[52].attacks = 35;						// 공격력
	Weapon[52].penetration = 0;					// 관통력
	Weapon[52].blazings = 3;						// 연사 속도
	Weapon[52].speed = 10;						// 탄속
	Weapon[52].nbsmax = 15;						// 최대 장탄수
	Weapon[52].reloads = 55;						// 재장전 시간
	Weapon[52].reaction = 9;						// 반동
	Weapon[52].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[52].ErrorRangeMAX = 28;					// 최대 명중률 오차
	Weapon[52].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[52].my = 10.0f;						// 1인칭 손 위치 Y
	Weapon[52].mz = 58.0f;						// 1인칭 손 위치 Z
	Weapon[52].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[52].flashy = 19.0f;					// 총구 화염 위치 Y
	Weapon[52].flashz = 72.0f;					// 총구 화염 위치 Z
	Weapon[52].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[52].yakkyou_py = 16.0f;					// 탄피 배출 위치 Y
	Weapon[52].yakkyou_pz = 52.0f;					// 탄피 배출 위치 Z
	Weapon[52].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[52].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[52].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[52].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[52].burstcnt = 0;						// 점사
	Weapon[52].scopemode = 0;					// 스코프 모드
	Weapon[52].size = 7.0f;						// 3인칭 모델 크기
	Weapon[52].soundid = 46;						// 발사음 ID
	Weapon[52].soundvolume = 90;				// 발사음 크기
	Weapon[52].silencer = false;					// 소음기 여부
	Weapon[52].WeaponP = 1;						// 무기 종류
	Weapon[52].ChangeWeaponID = 76;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[52].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[52].pellet = 1;						// 샷건 펠릿 수
	Weapon[52].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[52].bHasChamber = true;
	Weapon[52].TacticalReloadFrame = 35;
	Weapon[52].SoundID_Reload = -1;
	Weapon[52].SoundID_Tactical = -1;

	Weapon[53].name = "M29 Satan";				// 이름
	Weapon[53].model = "./data/model/weapon/3/M29_Satan.x";			// 3D 모델 파일 경로
	Weapon[53].texture = "./data/model/weapon/3/M29_Satan.bmp";		// 텍스처 파일 경로
	Weapon[53].attacks = 65;						// 공격력
	Weapon[53].penetration = 1;					// 관통력
	Weapon[53].blazings = 5.5;						// 연사 속도
	Weapon[53].speed = 10;						// 탄속
	Weapon[53].nbsmax = 6;						// 최대 장탄수
	Weapon[53].reloads = 74;						// 재장전 시간
	Weapon[53].reaction = 10;					// 반동
	Weapon[53].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[53].ErrorRangeMAX = 23;					// 최대 명중률 오차
	Weapon[53].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[53].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[53].mz = 59.0f;						// 1인칭 손 위치 Z
	Weapon[53].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[53].flashy = 18.0f;					// 총구 화염 위치 Y
	Weapon[53].flashz = 89.0f;					// 총구 화염 위치 Z
	Weapon[53].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[53].yakkyou_py = 15.0f;					// 탄피 배출 위치 Y
	Weapon[53].yakkyou_pz = 56.0f;					// 탄피 배출 위치 Z
	Weapon[53].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[53].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[53].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[53].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[53].burstcnt = 1;						// 점사
	Weapon[53].scopemode = 0;					// 스코프 모드
	Weapon[53].size = 7.0f;						// 3인칭 모델 크기
	Weapon[53].soundid = 47;						// 발사음 ID
	Weapon[53].soundvolume = 90;				// 발사음 크기
	Weapon[53].silencer = false;					// 소음기 여부
	Weapon[53].WeaponP = 1;						// 무기 종류
	Weapon[53].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[53].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[53].pellet = 1;						// 샷건 펠릿 수
	Weapon[53].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[53].bHasChamber = false;
	Weapon[53].TacticalReloadFrame = 0;
	Weapon[53].SoundID_Reload = -1;
	Weapon[53].SoundID_Tactical = -1;

	Weapon[54].name = "WA2000";					// 이름
	Weapon[54].model = "./data/model/weapon/3/WA2000.x";		// 3D 모델 파일 경로
	Weapon[54].texture = "./data/model/weapon/3/WA2000.bmp";		// 텍스처 파일 경로
	Weapon[54].attacks = 65;					// 공격력
	Weapon[54].penetration = 3;					// 관통력
	Weapon[54].blazings = 12;					// 연사 속도
	Weapon[54].speed = 255;						// 탄속
	Weapon[54].nbsmax = 6;						// 최대 장탄수
	Weapon[54].reloads = 84;					// 재장전 시간
	Weapon[54].reaction = 15;					// 반동
	Weapon[54].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[54].ErrorRangeMAX = 19;					// 최대 명중률 오차
	Weapon[54].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[54].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[54].mz = 30.0f;						// 1인칭 손 위치 Z
	Weapon[54].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[54].flashy = 17.5f;					// 총구 화염 위치 Y
	Weapon[54].flashz = 90.0f;					// 총구 화염 위치 Z
	Weapon[54].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[54].yakkyou_py = 13.0f;					// 탄피 배출 위치 Y
	Weapon[54].yakkyou_pz = 38.0f;					// 탄피 배출 위치 Z
	Weapon[54].yakkyou_sx = 6.0f;					// 탄피 배출 속도 X
	Weapon[54].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[54].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[54].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[54].burstcnt = 0;					// 점사
	Weapon[54].scopemode = 5;					// 스코프 모드
	Weapon[54].size = 7.0f;						// 3인칭 모델 크기
	Weapon[54].soundid = 50;						// 발사음 ID
	Weapon[54].soundvolume = 90;				// 발사음 크기
	Weapon[54].silencer = false;					// 소음기 여부
	Weapon[54].WeaponP = 0;						// 무기 종류
	Weapon[54].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[54].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[54].pellet = 1;						// 샷건 펠릿 수
	Weapon[54].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[54].bHasChamber = true;
	Weapon[54].TacticalReloadFrame = 62;
	Weapon[54].SoundID_Reload = -1;
	Weapon[54].SoundID_Tactical = -1;

	Weapon[55].name = "Minigun";					// 이름
	Weapon[55].model = "./data/model/weapon/3/Minigun.x";		// 3D 모델 파일 경로
	Weapon[55].texture = "./data/model/weapon/3/Minigun.bmp";		// 텍스처 파일 경로
	Weapon[55].attacks = 65;					// 공격력
	Weapon[55].penetration = 4;					// 관통력
	Weapon[55].blazings = 4;					// 연사 속도
	Weapon[55].speed = 11;						// 탄속
	Weapon[55].nbsmax = 200;					// 최대 장탄수
	Weapon[55].reloads = 167;					// 재장전 시간
	Weapon[55].reaction = 10;					// 반동
	Weapon[55].ErrorRangeMIN = 5;					// 최소 명중률 오차
	Weapon[55].ErrorRangeMAX = 22;					// 최대 명중률 오차
	Weapon[55].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[55].my = -4.0f;						// 1인칭 손 위치 Y
	Weapon[55].mz = 41.0f;						// 1인칭 손 위치 Z
	Weapon[55].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[55].flashy = -4.0f;					// 총구 화염 위치 Y
	Weapon[55].flashz = 102.0f;					// 총구 화염 위치 Z
	Weapon[55].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[55].yakkyou_py = -4.0f;					// 탄피 배출 위치 Y
	Weapon[55].yakkyou_pz = 40.0f;					// 탄피 배출 위치 Z
	Weapon[55].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[55].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[55].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[55].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[55].burstcnt = 6;					// 점사 (0: 연사, 1:단발, 3:3점사, 4:볼트액션, 6:미니건)
	Weapon[55].scopemode = 0;					// 스코프 모드
	Weapon[55].size = 7.0f;						// 3인칭 모델 크기
	Weapon[55].soundid = 51;						// 발사음 ID
	Weapon[55].soundvolume = 90;				// 발사음 크기
	Weapon[55].silencer = false;					// 소음기 여부
	Weapon[55].WeaponP = 0;						// 무기 종류
	Weapon[55].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[55].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[55].pellet = 1;						// 샷건 펠릿 수
	Weapon[55].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[55].bHasChamber = false;
	Weapon[55].TacticalReloadFrame = 0;
	Weapon[55].SoundID_Reload = -1;
	Weapon[55].SoundID_Tactical = -1;

	Weapon[56].name = "M1918 BAR";					// 이름
	Weapon[56].model = "./data/model/weapon/4/bar.x";		// 3D 모델 파일 경로
	Weapon[56].texture = "./data/model/weapon/4/bar.bmp";		// 텍스처 파일 경로
	Weapon[56].attacks = 67;					// 공격력
	Weapon[56].penetration = 3;					// 관통력
	Weapon[56].blazings = 7.5;					// 연사 속도
	Weapon[56].speed = 11;						// 탄속
	Weapon[56].nbsmax = 20;						// 최대 장탄수
	Weapon[56].reloads = 95;					// 재장전 시간
	Weapon[56].reaction = 9;					// 반동
	Weapon[56].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[56].ErrorRangeMAX = 20;					// 최대 명중률 오차
	Weapon[56].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[56].my = 15.0f;						// 1인칭 손 위치 Y
	Weapon[56].mz = 33.0f;						// 1인칭 손 위치 Z
	Weapon[56].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[56].flashy = 16.0f;					// 총구 화염 위치 Y
	Weapon[56].flashz = 121.0f;					// 총구 화염 위치 Z
	Weapon[56].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[56].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[56].yakkyou_pz = 41.0f;					// 탄피 배출 위치 Z
	Weapon[56].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[56].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[56].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[56].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[56].burstcnt = 0;					// 점사
	Weapon[56].scopemode = 0;					// 스코프 모드
	Weapon[56].size = 7.0f;						// 3인칭 모델 크기
	Weapon[56].soundid = 52;						// 발사음 ID
	Weapon[56].soundvolume = 90;				// 발사음 크기
	Weapon[56].silencer = false;					// 소음기 여부
	Weapon[56].WeaponP = 0;						// 무기 종류
	Weapon[56].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[56].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[56].pellet = 1;						// 샷건 펠릿 수
	Weapon[56].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[56].bHasChamber = false;
	Weapon[56].TacticalReloadFrame = 0;
	Weapon[56].SoundID_Reload = -1;
	Weapon[56].SoundID_Tactical = -1;

	Weapon[57].name = "Calico M950";					// 이름
	Weapon[57].model = "./data/model/weapon/4/Calico_M950.x";		// 3D 모델 파일 경로
	Weapon[57].texture = "./data/model/weapon/4/Calico_M950.png";		// 텍스처 파일 경로
	Weapon[57].attacks = 10;						// 공격력
	Weapon[57].penetration = 0;					// 관통력
	Weapon[57].blazings = 6;						// 연사 속도
	Weapon[57].speed = 10;						// 탄속
	Weapon[57].nbsmax = 50;						// 최대 장탄수
	Weapon[57].reloads = 86;						// 재장전 시간
	Weapon[57].reaction = 8;						// 반동
	Weapon[57].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[57].ErrorRangeMAX = 0;					// 최대 명중률 오차
	Weapon[57].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[57].my = 10.0f;						// 1인칭 손 위치 Y
	Weapon[57].mz = 52.0f;						// 1인칭 손 위치 Z
	Weapon[57].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[57].flashy = 14.0f;					// 총구 화염 위치 Y
	Weapon[57].flashz = 94.0f;					// 총구 화염 위치 Z
	Weapon[57].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[57].yakkyou_py = 16.0f;					// 탄피 배출 위치 Y
	Weapon[57].yakkyou_pz = 52.0f;					// 탄피 배출 위치 Z
	Weapon[57].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[57].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[57].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[57].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[57].burstcnt = 0;						// 점사
	Weapon[57].scopemode = 0;					// 스코프 모드
	Weapon[57].size = 7.0f;						// 3인칭 모델 크기
	Weapon[57].soundid = 58;						// 발사음 ID
	Weapon[57].soundvolume = 90;				// 발사음 크기
	Weapon[57].silencer = false;					// 소음기 여부
	Weapon[57].WeaponP = 1;						// 무기 종류
	Weapon[57].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[57].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[57].pellet = 5;						// 샷건 펠릿 수
	Weapon[57].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[57].bHasChamber = true;
	Weapon[57].TacticalReloadFrame = 65;
	Weapon[57].SoundID_Reload = -1;
	Weapon[57].SoundID_Tactical = -1;

	Weapon[58].name = "KAR98K";				// 이름
	Weapon[58].model = "./data/model/weapon/4/kar98k.x";	// 3D 모델 파일 경로
	Weapon[58].texture = "./data/model/weapon/4/kar98k.bmp"; // 텍스처 파일 경로
	Weapon[58].attacks = 80;					// 공격력
	Weapon[58].penetration = 3;					// 관통력
	Weapon[58].blazings = 40;					// 연사 속도
	Weapon[58].speed = 70;						// 탄속
	Weapon[58].nbsmax = 5;						// 최대 장탄수
	Weapon[58].reloads = 85;					// 재장전 시간
	Weapon[58].reaction = 5;					// 반동
	Weapon[58].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[58].ErrorRangeMAX = 20;					// 최대 명중률 오차
	Weapon[58].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[58].my = 9.0f;						// 1인칭 손 위치 Y
	Weapon[58].mz = 30.0f;						// 1인칭 손 위치 Z
	Weapon[58].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[58].flashy = 13.5f;					// 총구 화염 위치 Y
	Weapon[58].flashz = 103.0f;					// 총구 화염 위치 Z
	Weapon[58].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[58].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[58].yakkyou_pz = 42.0f;					// 탄피 배출 위치 Z
	Weapon[58].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[58].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[58].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[58].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[58].burstcnt = 4;					// 점사
	Weapon[58].scopemode = 0;					// 스코프 모드
	Weapon[58].size = 7.0f;						// 3인칭 모델 크기
	Weapon[58].soundid = 53;						// 발사음 ID
	Weapon[58].soundvolume = 90;				// 발사음 크기
	Weapon[58].silencer = false;					// 소음기 여부
	Weapon[58].WeaponP = 0;						// 무기 종류
	Weapon[58].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[58].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[58].pellet = 1;						// 샷건 펠릿 수
	Weapon[58].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[58].bHasChamber = true;
	Weapon[58].TacticalReloadFrame = -1;
	Weapon[58].SoundID_Reload = -1;
	Weapon[58].SoundID_Tactical = -1;

	Weapon[59].name = "KAR98K Sniper";					// 이름
	Weapon[59].model = "./data/model/weapon/4/kar98_scope.x";			// 3D 모델 파일 경로
	Weapon[59].texture = "./data/model/weapon/4/kar98_scope.bmp";		// 텍스처 파일 경로
	Weapon[59].attacks = 93;					// 공격력
	Weapon[59].penetration = 3;					// 관통력
	Weapon[59].blazings = 40;					// 연사 속도(값이 작을수록 빠름)
	Weapon[59].speed = 255;						// 탄속
	Weapon[59].nbsmax = 5;						// 최대 장탄수
	Weapon[59].reloads = 18;					// 재장전 시간
	Weapon[59].reaction = 10;					// 반동
	Weapon[59].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[59].ErrorRangeMAX = 19;					// 최대 명중률 오차
	Weapon[59].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[59].my = 9.0f;						// 1인칭 손 위치 Y
	Weapon[59].mz = 30.0f;						// 1인칭 손 위치 Z
	Weapon[59].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[59].flashy = 13.5f;					// 총구 화염 위치 Y
	Weapon[59].flashz = 103.0f;					// 총구 화염 위치 Z
	Weapon[59].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[59].yakkyou_py = 13.0f;					// 탄피 배출 위치 Y
	Weapon[59].yakkyou_pz = 38.0f;					// 탄피 배출 위치 Z
	Weapon[59].yakkyou_sx = 6.0f;					// 탄피 배출 속도 X
	Weapon[59].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[59].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[59].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[59].burstcnt = 4;					// 점사 (0: 자동, 1: 단발, 3: 3점사)
	Weapon[59].scopemode = 6;					 // 스코프 모드 (0: 자동, 1:acog, 2:저격, 3:레드닷)
	Weapon[59].size = 7.0f;						// 3인칭 모델 크기
	Weapon[59].soundid = 86;						// 발사음 ID (0:mp5,1 :ak47,2 :이글,3 :m249, 4 :AUG, 13 :소음기)
	Weapon[59].soundvolume = 90;				// 발사음 크기
	Weapon[59].silencer = false;					// 소음기 여부
	Weapon[59].WeaponP = 0;						// 무기 종류 (0: 양손, 1: 한손)
	Weapon[59].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[59].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[59].pellet = 1;						// 샷건 펠릿 수
	Weapon[59].is_shell_reload = true;				// 셸 단위 재장전 여부
	Weapon[59].bHasChamber = true;
	Weapon[59].TacticalReloadFrame = 45;
	Weapon[59].SoundID_Reload = -1;
	Weapon[59].SoundID_Tactical = -1;

	Weapon[60].name = "M1 Garand";    				// 이름
	Weapon[60].model = "./data/model/weapon/4/M1Garand.x";	// 3D 모델 파일 경로
	Weapon[60].texture = "./data/model/weapon/4/M1Garand.bmp";	// 텍스처 파일 경로
	Weapon[60].attacks = 65;         				// 공격력
	Weapon[60].penetration = 2;      				// 관통력
	Weapon[60].blazings = 7;         				// 연사 속도
	Weapon[60].speed = 70;           				// 탄속
	Weapon[60].nbsmax = 8;          				// 최대 장탄수
	Weapon[60].reloads = 58;         				// 재장전 시간
	Weapon[60].reaction = 10;         				// 반동
	Weapon[60].ErrorRangeMIN = 0;    				// 최소 명중률 오차
	Weapon[60].ErrorRangeMAX = 15;   				// 최대 명중률 오차
	Weapon[60].mx = 9.0f;            				// 1인칭 손 위치 X
	Weapon[60].my = 5.0f;            				// 1인칭 손 위치 Y
	Weapon[60].mz = 15.0f;           				// 1인칭 손 위치 Z
	Weapon[60].flashx = 9.0f;        				// 총구 화염 위치 X
	Weapon[60].flashy = 16.5f;       				// 총구 화염 위치 Y
	Weapon[60].flashz = 99.0f;       				// 총구 화염 위치 Z
	Weapon[60].yakkyou_px = 10.0f;   				// 탄피 배출 위치 X
	Weapon[60].yakkyou_py = 12.0f;   				// 탄피 배출 위치 Y
	Weapon[60].yakkyou_pz = 40.0f;   				// 탄피 배출 위치 Z
	Weapon[60].yakkyou_sx = 4.0f;    				// 탄피 배출 속도 X
	Weapon[60].yakkyou_sy = 6.0f;    				// 탄피 배출 속도 Y
	Weapon[60].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[60].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[60].burstcnt = 1;         				// 점사
	Weapon[60].scopemode = 0;        				// 스코프 모드
	Weapon[60].size = 7.0f;          				// 3인칭 모델 크기
	Weapon[60].soundid = 54;          				// 발사음 ID
	Weapon[60].soundvolume = 90;     				// 발사음 크기
	Weapon[60].silencer = false;     				// 소음기 여부
	Weapon[60].WeaponP = 0;          				// 무기 종류
	Weapon[60].ChangeWeaponID = -1;  				// 발사모드 전환 시 바뀔 무기 ID
	Weapon[60].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[60].pellet = 1;           				// 샷건 펠릿 수
	Weapon[60].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[60].bHasChamber = false;
	Weapon[60].TacticalReloadFrame = 0;
	Weapon[60].SoundID_Reload = -1;
	Weapon[60].SoundID_Tactical = -1;

	Weapon[61].name = "P38";					// 이름
	Weapon[61].model = "./data/model/weapon/4/P38.x";			// 3D 모델 파일 경로
	Weapon[61].texture = "./data/model/weapon/4/P38.bmp";		// 텍스처 파일 경로
	Weapon[61].attacks = 45;					// 공격력
	Weapon[61].penetration = 0;					// 관통력
	Weapon[61].blazings = 5;					// 연사 속도
	Weapon[61].speed = 9;						// 탄속
	Weapon[61].nbsmax = 8;						// 최대 장탄수
	Weapon[61].reloads = 55;					// 재장전 시간
	Weapon[61].reaction = 12;					// 반동
	Weapon[61].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[61].ErrorRangeMAX = 26;					// 최대 명중률 오차
	Weapon[61].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[61].my = 10.0f;						// 1인칭 손 위치 Y
	Weapon[61].mz = 62.0f;						// 1인칭 손 위치 Z
	Weapon[61].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[61].flashy = 18.5f;					// 총구 화염 위치 Y
	Weapon[61].flashz = 75.0f;					// 총구 화염 위치 Z
	Weapon[61].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[61].yakkyou_py = 16.0f;					// 탄피 배출 위치 Y
	Weapon[61].yakkyou_pz = 57.0f;					// 탄피 배출 위치 Z
	Weapon[61].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[61].yakkyou_sy = 3.0f;					// 탄피 배출 속도 Y
	Weapon[61].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[61].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[61].burstcnt = 1;					// 점사
	Weapon[61].scopemode = 0;					// 스코프 모드
	Weapon[61].size = 7.0f;						// 3인칭 모델 크기
	Weapon[61].soundid = 35;						// 발사음 ID
	Weapon[61].soundvolume = 90;				// 발사음 크기
	Weapon[61].silencer = false;					// 소음기 여부
	Weapon[61].WeaponP = 1;						// 무기 종류
	Weapon[61].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[61].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[61].pellet = 1;						// 샷건 펠릿 수
	Weapon[61].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[61].bHasChamber = true;
	Weapon[61].TacticalReloadFrame = 35;
	Weapon[61].SoundID_Reload = -1;
	Weapon[61].SoundID_Tactical = -1;

	Weapon[62].name = "PPSH-41";					// 이름
	Weapon[62].model = "./data/model/weapon/4/ppsh-41.x";		// 3D 모델 파일 경로
	Weapon[62].texture = "./data/model/weapon/4/ppsh-41.bmp";		// 텍스처 파일 경로
	Weapon[62].attacks = 35;					// 공격력
	Weapon[62].penetration = 0;					// 관통력
	Weapon[62].blazings = 4;					// 연사 속도
	Weapon[62].speed = 10;						// 탄속
	Weapon[62].nbsmax = 71;						// 최대 장탄수
	Weapon[62].reloads = 95;					// 재장전 시간
	Weapon[62].reaction = 6;					// 반동
	Weapon[62].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[62].ErrorRangeMAX = 25;					// 최대 명중률 오차
	Weapon[62].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[62].my = 7.0f;						// 1인칭 손 위치 Y
	Weapon[62].mz = 33.0f;						// 1인칭 손 위치 Z
	Weapon[62].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[62].flashy = 11.0f;					// 총구 화염 위치 Y
	Weapon[62].flashz = 86.0f;					// 총구 화염 위치 Z
	Weapon[62].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[62].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[62].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[62].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[62].yakkyou_sy = 7.0f;					// 탄피 배출 속도 Y
	Weapon[62].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[62].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[62].burstcnt = 0;					// 점사
	Weapon[62].scopemode = 0;					// 스코프 모드
	Weapon[62].size = 7.0f;						// 3인칭 모델 크기
	Weapon[62].soundid = 55;						// 발사음 ID
	Weapon[62].soundvolume = 90;				// 발사음 크기
	Weapon[62].silencer = false;					// 소음기 여부
	Weapon[62].WeaponP = 0;						// 무기 종류
	Weapon[62].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[62].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[62].pellet = 1;						// 샷건 펠릿 수
	Weapon[62].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[62].bHasChamber = false;
	Weapon[62].TacticalReloadFrame = 0;
	Weapon[62].SoundID_Reload = -1;
	Weapon[62].SoundID_Tactical = -1;

	Weapon[63].name = "Springfield M1903";					// 이름
	Weapon[63].model = "./data/model/weapon/4/Springfield.x";			// 3D 모델 파일 경로
	Weapon[63].texture = "./data/model/weapon/4/Springfield.bmp";		// 텍스처 파일 경로
	Weapon[63].attacks = 95;					// 공격력
	Weapon[63].penetration = 3;					// 관통력
	Weapon[63].blazings = 40;					// 연사 속도(값이 작을수록 빠름)
	Weapon[63].speed = 255;						// 탄속
	Weapon[63].nbsmax = 5;						// 최대 장탄수
	Weapon[63].reloads = 18;					// 재장전 시간
	Weapon[63].reaction = 15;					// 반동
	Weapon[63].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[63].ErrorRangeMAX = 19;					// 최대 명중률 오차
	Weapon[63].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[63].my = 5.0f;						// 1인칭 손 위치 Y
	Weapon[63].mz = 26.0f;						// 1인칭 손 위치 Z
	Weapon[63].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[63].flashy = 14.0f;					// 총구 화염 위치 Y
	Weapon[63].flashz = 111.0f;					// 총구 화염 위치 Z
	Weapon[63].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[63].yakkyou_py = 13.0f;					// 탄피 배출 위치 Y
	Weapon[63].yakkyou_pz = 38.0f;					// 탄피 배출 위치 Z
	Weapon[63].yakkyou_sx = 6.0f;					// 탄피 배출 속도 X
	Weapon[63].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[63].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[63].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[63].burstcnt = 4;					// 점사 (0: 자동, 1: 단발, 3: 3점사)
	Weapon[63].scopemode = 6;					 // 스코프 모드 (0: 자동, 1:acog, 2:저격, 3:레드닷)
	Weapon[63].size = 7.0f;						// 3인칭 모델 크기
	Weapon[63].soundid = 60;						// 발사음 ID (0:mp5,1 :ak47,2 :이글,3 :m249, 4 :AUG, 13 :소음기)
	Weapon[63].soundvolume = 90;				// 발사음 크기
	Weapon[63].silencer = false;					// 소음기 여부
	Weapon[63].WeaponP = 0;						// 무기 종류 (0: 양손, 1: 한손)
	Weapon[63].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[63].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[63].pellet = 1;						// 샷건 펠릿 수
	Weapon[63].is_shell_reload = true;				// 셸 단위 재장전 여부
	Weapon[63].bHasChamber = true;
	Weapon[63].TacticalReloadFrame = 45;
	Weapon[63].SoundID_Reload = -1;
	Weapon[63].SoundID_Tactical = -1;

	Weapon[64].name = "STG44";					// 이름
	Weapon[64].model = "./data/model/weapon/4/stg44.x";		// 3D 모델 파일 경로
	Weapon[64].texture = "./data/model/weapon/4/stg44.bmp";		// 텍스처 파일 경로
	Weapon[64].attacks = 58;					// 공격력
	Weapon[64].penetration = 1;					// 관통력
	Weapon[64].blazings = 4.5;					// 연사 속도
	Weapon[64].speed = 11;						// 탄속
	Weapon[64].nbsmax = 30;						// 최대 장탄수
	Weapon[64].reloads = 89;					// 재장전 시간
	Weapon[64].reaction = 8;					// 반동
	Weapon[64].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[64].ErrorRangeMAX = 20;					// 최대 명중률 오차
	Weapon[64].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[64].my = 13.0f;						// 1인칭 손 위치 Y
	Weapon[64].mz = 34.0f;						// 1인칭 손 위치 Z
	Weapon[64].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[64].flashy = 12.5f;					// 총구 화염 위치 Y
	Weapon[64].flashz = 89.0f;					// 총구 화염 위치 Z
	Weapon[64].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[64].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[64].yakkyou_pz = 41.0f;					// 탄피 배출 위치 Z
	Weapon[64].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[64].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[64].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[64].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[64].burstcnt = 0;					// 점사
	Weapon[64].scopemode = 0;					// 스코프 모드
	Weapon[64].size = 7.0f;						// 3인칭 모델 크기
	Weapon[64].soundid = 56;						// 발사음 ID
	Weapon[64].soundvolume = 90;				// 발사음 크기
	Weapon[64].silencer = false;					// 소음기 여부
	Weapon[64].WeaponP = 0;						// 무기 종류
	Weapon[64].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[64].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[64].pellet = 1;						// 샷건 펠릿 수
	Weapon[64].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[64].bHasChamber = true;
	Weapon[64].TacticalReloadFrame = 64;
	Weapon[64].SoundID_Reload = -1;
	Weapon[64].SoundID_Tactical = -1;

	Weapon[65].name = "M1928A1 Thompson";					// 이름
	Weapon[65].model = "./data/model/weapon/4/thompson.x";		// 3D 모델 파일 경로
	Weapon[65].texture = "./data/model/weapon/4/thompson.bmp";		// 텍스처 파일 경로
	Weapon[65].attacks = 36;					// 공격력
	Weapon[65].penetration = 0;					// 관통력
	Weapon[65].blazings = 4;					// 연사 속도
	Weapon[65].speed = 10;						// 탄속
	Weapon[65].nbsmax = 30;						// 최대 장탄수
	Weapon[65].reloads = 67;					// 재장전 시간
	Weapon[65].reaction = 4;					// 반동
	Weapon[65].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[65].ErrorRangeMAX = 25;					// 최대 명중률 오차
	Weapon[65].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[65].my = 9.0f;						// 1인칭 손 위치 Y
	Weapon[65].mz = 33.0f;						// 1인칭 손 위치 Z
	Weapon[65].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[65].flashy = 14.0f;					// 총구 화염 위치 Y
	Weapon[65].flashz = 81.0f;					// 총구 화염 위치 Z
	Weapon[65].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[65].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[65].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[65].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[65].yakkyou_sy = 7.0f;					// 탄피 배출 속도 Y
	Weapon[65].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[65].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[65].burstcnt = 0;					// 점사
	Weapon[65].scopemode = 0;					// 스코프 모드
	Weapon[65].size = 7.0f;						// 3인칭 모델 크기
	Weapon[65].soundid = 57;						// 발사음 ID
	Weapon[65].soundvolume = 90;				// 발사음 크기
	Weapon[65].silencer = false;					// 소음기 여부
	Weapon[65].WeaponP = 0;						// 무기 종류
	Weapon[65].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[65].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[65].pellet = 1;						// 샷건 펠릿 수
	Weapon[65].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[65].bHasChamber = false;
	Weapon[65].TacticalReloadFrame = 0;
	Weapon[65].SoundID_Reload = -1;
	Weapon[65].SoundID_Tactical = -1;

	Weapon[66].name = "USAS-12";					// 이름
	Weapon[66].model = "./data/model/weapon/4/usas-12.x";		// 3D 모델 파일 경로
	Weapon[66].texture = "./data/model/weapon/4/usas-12.bmp";		// 텍스처 파일 경로
	Weapon[66].attacks = 78;					// 공격력
	Weapon[66].penetration = 0;					// 관통력
	Weapon[66].blazings = 10;					// 연사 속도
	Weapon[66].speed = 9;						// 탄속
	Weapon[66].nbsmax = 20;						// 최대 장탄수
	Weapon[66].reloads = 82;					// 재장전 시간
	Weapon[66].reaction = 15;					// 반동
	Weapon[66].ErrorRangeMIN = 4;					// 최소 명중률 오차
	Weapon[66].ErrorRangeMAX = 27;					// 최대 명중률 오차
	Weapon[66].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[66].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[66].mz = 40.0f;						// 1인칭 손 위치 Z
	Weapon[66].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[66].flashy = 11.5f;					// 총구 화염 위치 Y
	Weapon[66].flashz = 84.0f;					// 총구 화염 위치 Z
	Weapon[66].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[66].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[66].yakkyou_pz = 42.0f;					// 탄피 배출 위치 Z
	Weapon[66].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[66].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[66].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[66].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[66].burstcnt = 0;					// 점사
	Weapon[66].scopemode = 0;					// 스코프 모드
	Weapon[66].size = 7.0f;						// 3인칭 모델 크기
	Weapon[66].soundid = 59;						// 발사음 ID
	Weapon[66].soundvolume = 90;				// 발사음 크기
	Weapon[66].silencer = false;					// 소음기 여부
	Weapon[66].WeaponP = 0;						// 무기 종류
	Weapon[66].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[66].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[66].pellet = 6;						// 샷건 펠릿 수
	Weapon[66].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[66].bHasChamber = true;
	Weapon[66].TacticalReloadFrame = 57;
	Weapon[66].SoundID_Reload = -1;
	Weapon[66].SoundID_Tactical = -1;

	Weapon[67].name = "MG42";					// 이름
	Weapon[67].model = "./data/model/weapon/4/mg42.x";		// 3D 모델 파일 경로
	Weapon[67].texture = "./data/model/weapon/4/mg42.bmp";		// 텍스처 파일 경로
	Weapon[67].attacks = 56;					// 공격력
	Weapon[67].penetration = 4;					// 관통력
	Weapon[67].blazings = 3.5;					// 연사 속도
	Weapon[67].speed = 11;						// 탄속
	Weapon[67].nbsmax = 50;					// 최대 장탄수
	Weapon[67].reloads = 167;					// 재장전 시간
	Weapon[67].reaction = 9;					// 반동
	Weapon[67].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[67].ErrorRangeMAX = 30;					// 최대 명중률 오차
	Weapon[67].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[67].my = 12.0f;						// 1인칭 손 위치 Y
	Weapon[67].mz = 31.0f;						// 1인칭 손 위치 Z
	Weapon[67].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[67].flashy = 11.0f;					// 총구 화염 위치 Y
	Weapon[67].flashz = 98.0f;					// 총구 화염 위치 Z
	Weapon[67].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[67].yakkyou_py = 8.0f;					// 탄피 배출 위치 Y
	Weapon[67].yakkyou_pz = 55.0f;					// 탄피 배출 위치 Z
	Weapon[67].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[67].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[67].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[67].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[67].burstcnt = 0;					// 점사
	Weapon[67].scopemode = 0;					// 스코프 모드
	Weapon[67].size = 7.0f;						// 3인칭 모델 크기
	Weapon[67].soundid = 61;						// 발사음 ID
	Weapon[67].soundvolume = 90;				// 발사음 크기
	Weapon[67].silencer = false;					// 소음기 여부
	Weapon[67].WeaponP = 0;						// 무기 종류
	Weapon[67].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[67].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[67].pellet = 1;						// 샷건 펠릿 수
	Weapon[67].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[67].bHasChamber = false;
	Weapon[67].TacticalReloadFrame = 0;
	Weapon[67].SoundID_Reload = -1;
	Weapon[67].SoundID_Tactical = -1;

	Weapon[68].name = "SPAS-12 (PUMP)";					// 이름
	Weapon[68].model = "./data/model/weapon/1/spas.x";		// 3D 모델 파일 경로
	Weapon[68].texture = "./data/model/weapon/1/spas.bmp";		// 텍스처 파일 경로
	Weapon[68].attacks = 85;					// 공격력
	Weapon[68].penetration = 1;					// 관통력
	Weapon[68].blazings = 25;					// 연사 속도
	Weapon[68].speed = 11;						// 탄속
	Weapon[68].nbsmax = 8;						// 최대 장탄수
	Weapon[68].reloads = 20;					// 재장전 시간
	Weapon[68].reaction = 15;					// 반동
	Weapon[68].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[68].ErrorRangeMAX = 18;					// 최대 명중률 오차
	Weapon[68].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[68].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[68].mz = 28.0f;						// 1인칭 손 위치 Z
	Weapon[68].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[68].flashy = 17.0f;					// 총구 화염 위치 Y
	Weapon[68].flashz = 106.0f;					// 총구 화염 위치 Z
	Weapon[68].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[68].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[68].yakkyou_pz = 42.0f;					// 탄피 배출 위치 Z
	Weapon[68].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[68].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[68].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[68].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[68].burstcnt = 4;					// 점사
	Weapon[68].scopemode = 0;					// 스코프 모드
	Weapon[68].size = 7.0f;						// 3인칭 모델 크기
	Weapon[68].soundid = 31;						// 발사음 ID
	Weapon[68].soundvolume = 90;				// 발사음 크기
	Weapon[68].silencer = false;					// 소음기 여부
	Weapon[68].WeaponP = 0;						// 무기 종류
	Weapon[68].ChangeWeaponID = 25;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[68].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[68].pellet = 6;						// 샷건 펠릿 수
	Weapon[68].is_shell_reload = true;				// 셸 단위 재장전 여부
	Weapon[68].bHasChamber = true;
	Weapon[68].TacticalReloadFrame = 0;
	Weapon[68].SoundID_Reload = -1;
	Weapon[68].SoundID_Tactical = -1;

	Weapon[69].name = "Vz.61 Skorpion (Std.)";					// 이름
	Weapon[69].model = "./data/model/weapon/3/Vz.61.x";		// 3D 모델 파일 경로
	Weapon[69].texture = "./data/model/weapon/3/Vz.61.bmp";		// 텍스처 파일 경로
	Weapon[69].attacks = 33;						// 공격력
	Weapon[69].penetration = 0;					// 관통력
	Weapon[69].blazings = 4;						// 연사 속도
	Weapon[69].speed = 10;						// 탄속
	Weapon[69].nbsmax = 20;						// 최대 장탄수
	Weapon[69].reloads = 79;						// 재장전 시간
	Weapon[69].reaction = 10;						// 반동
	Weapon[69].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[69].ErrorRangeMAX = 25;					// 최대 명중률 오차
	Weapon[69].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[69].my = 9.0f;						// 1인칭 손 위치 Y
	Weapon[69].mz = 55.0f;						// 1인칭 손 위치 Z
	Weapon[69].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[69].flashy = 18.0f;					// 총구 화염 위치 Y
	Weapon[69].flashz = 77.0f;					// 총구 화염 위치 Z
	Weapon[69].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[69].yakkyou_py = 16.0f;					// 탄피 배출 위치 Y
	Weapon[69].yakkyou_pz = 52.0f;					// 탄피 배출 위치 Z
	Weapon[69].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[69].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[69].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[69].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[69].burstcnt = 0;						// 점사
	Weapon[69].scopemode = 0;					// 스코프 모드
	Weapon[69].size = 7.0f;						// 3인칭 모델 크기
	Weapon[69].soundid = 64;						// 발사음 ID
	Weapon[69].soundvolume = 90;				// 발사음 크기
	Weapon[69].silencer = false;					// 소음기 여부
	Weapon[69].WeaponP = 1;						// 무기 종류
	Weapon[69].ChangeWeaponID = 77;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[69].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[69].pellet = 1;						// 샷건 펠릿 수
	Weapon[69].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[69].bHasChamber = true;
	Weapon[69].TacticalReloadFrame = 57;
	Weapon[69].SoundID_Reload = -1;
	Weapon[69].SoundID_Tactical = -1;

	Weapon[70].name = "PP-19 Bizon (Std.)";						// 이름
	Weapon[70].model = "./data/model/weapon/3/PP19.x";			// 3D 모델 파일 경로
	Weapon[70].texture = "./data/model/weapon/3/PP19.bmp";		// 텍스처 파일 경로
	Weapon[70].attacks = 43;						// 공격력
	Weapon[70].penetration = 0;					// 관통력
	Weapon[70].blazings = 4.5;						// 연사 속도 (값이 작을수록 빠름)
	Weapon[70].speed = 10;						// 탄속
	Weapon[70].nbsmax = 64;						// 최대 장탄수
	Weapon[70].reloads = 86;						// 재장전 시간
	Weapon[70].reaction = 6;						// 반동
	Weapon[70].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[70].ErrorRangeMAX = 21;					// 최대 명중률 오차
	Weapon[70].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[70].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[70].mz = 28.0f;						// 1인칭 손 위치 Z
	Weapon[70].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[70].flashy = 16.0f;					// 총구 화염 위치 Y
	Weapon[70].flashz = 68.0f;					// 총구 화염 위치 Z
	Weapon[70].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[70].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[70].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[70].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[70].yakkyou_sy = 7.0f;					// 탄피 배출 속도 Y
	Weapon[70].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[70].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[70].burstcnt = 0;						// 점사 (0: 자동, 1: 단발, 3: 3점사, 4: 볼트액션)
	Weapon[70].scopemode = 0;					// 스코프 모드
	Weapon[70].size = 7.0f;						// 3인칭 모델 크기
	Weapon[70].soundid = 63;						// 발사음 ID
	Weapon[70].soundvolume = 90;				// 발사음 크기
	Weapon[70].silencer = false;					// 소음기 여부
	Weapon[70].WeaponP = 0;						// 무기 종류 (0: 양손, 1: 한손)
	Weapon[70].ChangeWeaponID = 84;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[70].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[70].pellet = 1;						// 샷건 펠릿 수
	Weapon[70].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[70].bHasChamber = true;
	Weapon[70].TacticalReloadFrame = 65;
	Weapon[70].SoundID_Reload = -1;
	Weapon[70].SoundID_Tactical = -1;

	Weapon[71].name = "CF-05";						// 이름
	Weapon[71].model = "./data/model/weapon/3/CF05_Scope.x";			// 3D 모델 파일 경로
	Weapon[71].texture = "./data/model/weapon/3/CF05_Scope.bmp";		// 텍스처 파일 경로
	Weapon[71].attacks = 50;						// 공격력
	Weapon[71].penetration = 0;					// 관통력
	Weapon[71].blazings = 3;						// 연사 속도 (값이 작을수록 빠름)
	Weapon[71].speed = 10;						// 탄속
	Weapon[71].nbsmax = 50;						// 최대 장탄수
	Weapon[71].reloads = 86;						// 재장전 시간
	Weapon[71].reaction = 7;						// 반동
	Weapon[71].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[71].ErrorRangeMAX = 21;					// 최대 명중률 오차
	Weapon[71].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[71].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[71].mz = 31.0f;						// 1인칭 손 위치 Z
	Weapon[71].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[71].flashy = 17.0f;					// 총구 화염 위치 Y
	Weapon[71].flashz = 59.0f;					// 총구 화염 위치 Z
	Weapon[71].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[71].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[71].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[71].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[71].yakkyou_sy = 7.0f;					// 탄피 배출 속도 Y
	Weapon[71].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[71].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[71].burstcnt = 2;						// 점사 (0: 자동, 1: 단발, 3: 3점사, 4: 볼트액션)
	Weapon[71].scopemode = 3;					// 스코프 모드
	Weapon[71].size = 7.0f;						// 3인칭 모델 크기
	Weapon[71].soundid = 65;						// 발사음 ID
	Weapon[71].soundvolume = 90;				// 발사음 크기
	Weapon[71].silencer = false;					// 소음기 여부
	Weapon[71].WeaponP = 0;						// 무기 종류 (0: 양손, 1: 한손)
	Weapon[71].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[71].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[71].pellet = 1;						// 샷건 펠릿 수
	Weapon[71].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[71].bHasChamber = true;
	Weapon[71].TacticalReloadFrame = 65;
	Weapon[71].SoundID_Reload = -1;
	Weapon[71].SoundID_Tactical = -1;

	Weapon[72].name = "AR-57";						// 이름
	Weapon[72].model = "./data/model/weapon/3/AR57_dot_grip_sd.x";			// 3D 모델 파일 경로
	Weapon[72].texture = "./data/model/weapon/3/AR57.bmp";		// 텍스처 파일 경로
	Weapon[72].attacks = 37;						// 공격력
	Weapon[72].penetration = 0;					// 관통력
	Weapon[72].blazings = 5;						// 연사 속도 (값이 작을수록 빠름)
	Weapon[72].speed = 10;						// 탄속
	Weapon[72].nbsmax = 50;						// 최대 장탄수
	Weapon[72].reloads = 91;						// 재장전 시간
	Weapon[72].reaction = 7;						// 반동
	Weapon[72].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[72].ErrorRangeMAX = 21;					// 최대 명중률 오차
	Weapon[72].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[72].my = 7.0f;						// 1인칭 손 위치 Y
	Weapon[72].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[72].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[72].flashy = 16.5f;					// 총구 화염 위치 Y
	Weapon[72].flashz = 86.0f;					// 총구 화염 위치 Z
	Weapon[72].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[72].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[72].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[72].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[72].yakkyou_sy = 7.0f;					// 탄피 배출 속도 Y
	Weapon[72].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[72].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[72].burstcnt = 0;						// 점사 (0: 자동, 1: 단발, 3: 3점사, 4: 볼트액션)
	Weapon[72].scopemode = 3;					// 스코프 모드
	Weapon[72].size = 7.0f;						// 3인칭 모델 크기
	Weapon[72].soundid = 66;						// 발사음 ID
	Weapon[72].soundvolume = 80;				// 발사음 크기
	Weapon[72].silencer = true;					// 소음기 여부
	Weapon[72].WeaponP = 0;						// 무기 종류 (0: 양손, 1: 한손)
	Weapon[72].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[72].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[72].pellet = 1;						// 샷건 펠릿 수
	Weapon[72].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[72].bHasChamber = true;
	Weapon[72].TacticalReloadFrame = 74;
	Weapon[72].SoundID_Reload = -1;
	Weapon[72].SoundID_Tactical = -1;

	Weapon[73].name = "K1A";						// 이름
	Weapon[73].model = "./data/model/weapon/2/k1a.x";		// 3D 모델 파일 경로
	Weapon[73].texture = "./data/model/weapon/2/k1a.bmp";		// 텍스처 파일 경로
	Weapon[73].attacks = 46;						// 공격력
	Weapon[73].penetration = 1;					// 관통력
	Weapon[73].blazings = 4;						// 연사 속도
	Weapon[73].speed = 11;						// 탄속
	Weapon[73].nbsmax = 20;						// 최대 장탄수
	Weapon[73].reloads = 79;						// 재장전 시간
	Weapon[73].reaction = 5;						// 반동
	Weapon[73].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[73].ErrorRangeMAX = 21;					// 최대 명중률 오차
	Weapon[73].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[73].my = 6.0f;						// 1인칭 손 위치 Y
	Weapon[73].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[73].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[73].flashy = 11.5f;					// 총구 화염 위치 Y
	Weapon[73].flashz = 86.0f;					// 총구 화염 위치 Z
	Weapon[73].yakkyou_px = 10.0f;					// 탄피 배출 위치 X
	Weapon[73].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[73].yakkyou_pz = 40.0f;					// 탄피 배출 위치 Z
	Weapon[73].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[73].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[73].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[73].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[73].burstcnt = 0;						// 점사
	Weapon[73].scopemode = 0;					// 스코프 모드
	Weapon[73].size = 7.0f;						// 3인칭 모델 크기
	Weapon[73].soundid = 67;						// 발사음 ID
	Weapon[73].soundvolume = 90;				// 발사음 크기
	Weapon[73].silencer = false;					// 소음기 여부
	Weapon[73].WeaponP = 0;						// 무기 종류
	Weapon[73].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[73].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[73].pellet = 1;						// 샷건 펠릿 수
	Weapon[73].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[73].bHasChamber = true;
	Weapon[73].TacticalReloadFrame = 57;
	Weapon[73].SoundID_Reload = -1;
	Weapon[73].SoundID_Tactical = -1;

	Weapon[74].name = "K2C";						// 이름
	Weapon[74].model = "./data/model/weapon/2/K2C_Aimpoint_Grip.x";		// 3D 모델 파일 경로
	Weapon[74].texture = "./data/model/weapon/2/K2C.bmp";		// 텍스처 파일 경로
	Weapon[74].attacks = 59;						// 공격력
	Weapon[74].penetration = 1;					// 관통력
	Weapon[74].blazings = 4;						// 연사 속도
	Weapon[74].speed = 11;						// 탄속
	Weapon[74].nbsmax = 30;						// 최대 장탄수
	Weapon[74].reloads = 78;						// 재장전 시간
	Weapon[74].reaction = 6;						// 반동
	Weapon[74].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[74].ErrorRangeMAX = 17;					// 최대 명중률 오차
	Weapon[74].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[74].my = 7.0f;						// 1인칭 손 위치 Y
	Weapon[74].mz = 28.0f;						// 1인칭 손 위치 Z
	Weapon[74].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[74].flashy = 16.0f;					// 총구 화염 위치 Y
	Weapon[74].flashz = 87.0f;					// 총구 화염 위치 Z
	Weapon[74].yakkyou_px = 10.0f;					// 탄피 배출 위치 X
	Weapon[74].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[74].yakkyou_pz = 40.0f;					// 탄피 배출 위치 Z
	Weapon[74].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[74].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[74].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[74].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[74].burstcnt = 0;						// 점사
	Weapon[74].scopemode = 4;					// 스코프 모드
	Weapon[74].size = 7.0f;						// 3인칭 모델 크기
	Weapon[74].soundid = 68;						// 발사음 ID
	Weapon[74].soundvolume = 90;				// 발사음 크기
	Weapon[74].silencer = false;					// 소음기 여부
	Weapon[74].WeaponP = 0;						// 무기 종류
	Weapon[74].ChangeWeaponID = -1;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[74].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[74].pellet = 1;						// 샷건 펠릿 수
	Weapon[74].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[74].bHasChamber = true;
	Weapon[74].TacticalReloadFrame = 51;
	Weapon[74].SoundID_Reload = -1;
	Weapon[74].SoundID_Tactical = -1;

	Weapon[75].name = "GLOCK17 (SD)";					// 이름
	Weapon[75].model = "./data/model/weapon/Glock17_SD.x";		// 3D 모델 파일 경로
	Weapon[75].texture = "./data/model/weapon/glock17.bmp";		// 텍스처 파일 경로
	Weapon[75].attacks = 30;					// 공격력
	Weapon[75].penetration = 0;					// 관통력
	Weapon[75].blazings = 5;					// 연사 속도
	Weapon[75].speed = 9;						// 탄속
	Weapon[75].nbsmax = 17;						// 최대 장탄수
	Weapon[75].reloads = 55;					// 재장전 시간
	Weapon[75].reaction = 9;					// 반동
	Weapon[75].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[75].ErrorRangeMAX = 26;					// 최대 명중률 오차
	Weapon[75].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[75].my = 11.0f;						// 1인칭 손 위치 Y
	Weapon[75].mz = 58.0f;						// 1인칭 손 위치 Z
	Weapon[75].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[75].flashy = 19.0f;					// 총구 화염 위치 Y
	Weapon[75].flashz = 91.0f;					// 총구 화염 위치 Z
	Weapon[75].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[75].yakkyou_py = 14.0f;					// 탄피 배출 위치 Y
	Weapon[75].yakkyou_pz = 54.0f;					// 탄피 배출 위치 Z
	Weapon[75].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[75].yakkyou_sy = 4.0f;					// 탄피 배출 속도 Y
	Weapon[75].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[75].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[75].burstcnt = 1;					// 점사
	Weapon[75].scopemode = 0;					// 스코프 모드
	Weapon[75].size = 7.0f;						// 3인칭 모델 크기
	Weapon[75].soundid = 71;						// 발사음 ID
	Weapon[75].soundvolume = 80;				// 발사음 크기
	Weapon[75].silencer = true;					// 소음기 여부
	Weapon[75].WeaponP = 1;						// 무기 종류
	Weapon[75].ChangeWeaponID = 18;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[75].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[75].pellet = 1;						// 샷건 펠릿 수
	Weapon[75].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[75].bHasChamber = true;
	Weapon[75].TacticalReloadFrame = 35;
	Weapon[75].SoundID_Reload = -1;
	Weapon[75].SoundID_Tactical = -1;

	Weapon[76].name = "Beretta 93R (SD)";					// 이름
	Weapon[76].model = "./data/model/weapon/3/93R_SD.x";		// 3D 모델 파일 경로
	Weapon[76].texture = "./data/model/weapon/3/93R.bmp";		// 텍스처 파일 경로
	Weapon[76].attacks = 28;						// 공격력
	Weapon[76].penetration = 0;					// 관통력
	Weapon[76].blazings = 3;						// 연사 속도
	Weapon[76].speed = 10;						// 탄속
	Weapon[76].nbsmax = 15;						// 최대 장탄수
	Weapon[76].reloads = 55;						// 재장전 시간
	Weapon[76].reaction = 9;						// 반동
	Weapon[76].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[76].ErrorRangeMAX = 28;					// 최대 명중률 오차
	Weapon[76].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[76].my = 10.0f;						// 1인칭 손 위치 Y
	Weapon[76].mz = 58.0f;						// 1인칭 손 위치 Z
	Weapon[76].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[76].flashy = 19.0f;					// 총구 화염 위치 Y
	Weapon[76].flashz = 89.0f;					// 총구 화염 위치 Z
	Weapon[76].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[76].yakkyou_py = 16.0f;					// 탄피 배출 위치 Y
	Weapon[76].yakkyou_pz = 52.0f;					// 탄피 배출 위치 Z
	Weapon[76].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[76].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[76].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[76].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[76].burstcnt = 0;						// 점사
	Weapon[76].scopemode = 0;					// 스코프 모드
	Weapon[76].size = 7.0f;						// 3인칭 모델 크기
	Weapon[76].soundid = 72;						// 발사음 ID
	Weapon[76].soundvolume = 80;				// 발사음 크기
	Weapon[76].silencer = true;					// 소음기 여부
	Weapon[76].WeaponP = 1;						// 무기 종류
	Weapon[76].ChangeWeaponID = 52;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[76].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[76].pellet = 1;						// 샷건 펠릿 수
	Weapon[76].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[76].bHasChamber = true;
	Weapon[76].TacticalReloadFrame = 35;
	Weapon[76].SoundID_Reload = -1;
	Weapon[76].SoundID_Tactical = -1;

	Weapon[77].name = "Vz.61 Skorpion (SD)";					// 이름
	Weapon[77].model = "./data/model/weapon/3/Vz.61_SD.x";		// 3D 모델 파일 경로
	Weapon[77].texture = "./data/model/weapon/3/Vz.61.bmp";		// 텍스처 파일 경로
	Weapon[77].attacks = 27;						// 공격력
	Weapon[77].penetration = 0;					// 관통력
	Weapon[77].blazings = 4;						// 연사 속도
	Weapon[77].speed = 10;						// 탄속
	Weapon[77].nbsmax = 20;						// 최대 장탄수
	Weapon[77].reloads = 79;						// 재장전 시간
	Weapon[77].reaction = 10;						// 반동
	Weapon[77].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[77].ErrorRangeMAX = 25;					// 최대 명중률 오차
	Weapon[77].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[77].my = 9.0f;						// 1인칭 손 위치 Y
	Weapon[77].mz = 58.0f;						// 1인칭 손 위치 Z
	Weapon[77].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[77].flashy = 18.0f;					// 총구 화염 위치 Y
	Weapon[77].flashz = 101.0f;					// 총구 화염 위치 Z
	Weapon[77].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[77].yakkyou_py = 16.0f;					// 탄피 배출 위치 Y
	Weapon[77].yakkyou_pz = 52.0f;					// 탄피 배출 위치 Z
	Weapon[77].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[77].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[77].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[77].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[77].burstcnt = 0;						// 점사
	Weapon[77].scopemode = 0;					// 스코프 모드
	Weapon[77].size = 7.0f;						// 3인칭 모델 크기
	Weapon[77].soundid = 73;						// 발사음 ID
	Weapon[77].soundvolume = 80;				// 발사음 크기
	Weapon[77].silencer = true;					// 소음기 여부
	Weapon[77].WeaponP = 1;						// 무기 종류
	Weapon[77].ChangeWeaponID = 69;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[77].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[77].pellet = 1;						// 샷건 펠릿 수
	Weapon[77].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[77].bHasChamber = true;
	Weapon[77].TacticalReloadFrame = 57;
	Weapon[77].SoundID_Reload = -1;
	Weapon[77].SoundID_Tactical = -1;

	Weapon[78].name = "MP9 (Std.)";					// 이름
	Weapon[78].model = "./data/model/weapon/3/MP9.x";		// 3D 모델 파일 경로
	Weapon[78].texture = "./data/model/weapon/3/MP9.bmp";		// 텍스처 파일 경로
	Weapon[78].attacks = 47;						// 공격력
	Weapon[78].penetration = 0;					// 관통력
	Weapon[78].blazings = 4;						// 연사 속도
	Weapon[78].speed = 10;						// 탄속
	Weapon[78].nbsmax = 20;						// 최대 장탄수
	Weapon[78].reloads = 79;						// 재장전 시간
	Weapon[78].reaction = 6;						// 반동
	Weapon[78].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[78].ErrorRangeMAX = 20;					// 최대 명중률 오차
	Weapon[78].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[78].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[78].mz = 45.0f;						// 1인칭 손 위치 Z
	Weapon[78].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[78].flashy = 14.5f;					// 총구 화염 위치 Y
	Weapon[78].flashz = 60.0f;					// 총구 화염 위치 Z
	Weapon[78].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[78].yakkyou_py = 13.0f;					// 탄피 배출 위치 Y
	Weapon[78].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[78].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[78].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[78].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[78].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[78].burstcnt = 0;						// 점사
	Weapon[78].scopemode = 0;					// 스코프 모드
	Weapon[78].size = 7.0f;						// 3인칭 모델 크기
	Weapon[78].soundid = 74;						// 발사음 ID
	Weapon[78].soundvolume = 90;				// 발사음 크기
	Weapon[78].silencer = false;					// 소음기 여부
	Weapon[78].WeaponP = 0;						// 무기 종류
	Weapon[78].ChangeWeaponID = 51;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[78].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[78].pellet = 1;						// 샷건 펠릿 수
	Weapon[78].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[78].bHasChamber = true;
	Weapon[78].TacticalReloadFrame = 57;
	Weapon[78].SoundID_Reload = -1;
	Weapon[78].SoundID_Tactical = -1;

	Weapon[79].name = "MAC10 (SD)";					// 이름
	Weapon[79].model = "./data/model/weapon/MAC10_SD.x";		// 3D 모델 파일 경로
	Weapon[79].texture = "./data/model/weapon/mac10.bmp";		// 텍스처 파일 경로
	Weapon[79].attacks = 41;						// 공격력
	Weapon[79].penetration = 0;					// 관통력
	Weapon[79].blazings = 3;						// 연사 속도
	Weapon[79].speed = 10;						// 탄속
	Weapon[79].nbsmax = 30;						// 최대 장탄수
	Weapon[79].reloads = 67;						// 재장전 시간
	Weapon[79].reaction = 9;						// 반동
	Weapon[79].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[79].ErrorRangeMAX = 25;					// 최대 명중률 오차
	Weapon[79].mx = 3.0f;						// 1인칭 손 위치 X
	Weapon[79].my = 11.0f;						// 1인칭 손 위치 Y
	Weapon[79].mz = 56.0f;						// 1인칭 손 위치 Z
	Weapon[79].flashx = 3.0f;					// 총구 화염 위치 X
	Weapon[79].flashy = 19.0f;					// 총구 화염 위치 Y
	Weapon[79].flashz = 96.0f;					// 총구 화염 위치 Z
	Weapon[79].yakkyou_px = 2.0f;					// 탄피 배출 위치 X
	Weapon[79].yakkyou_py = 16.0f;					// 탄피 배출 위치 Y
	Weapon[79].yakkyou_pz = 52.0f;					// 탄피 배출 위치 Z
	Weapon[79].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[79].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[79].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[79].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[79].burstcnt = 0;						// 점사
	Weapon[79].scopemode = 0;					// 스코프 모드
	Weapon[79].size = 7.0f;						// 3인칭 모델 크기
	Weapon[79].soundid = 75;						// 발사음 ID
	Weapon[79].soundvolume = 80;				// 발사음 크기
	Weapon[79].silencer = true;					// 소음기 여부
	Weapon[79].WeaponP = 1;						// 무기 종류
	Weapon[79].ChangeWeaponID = 6;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[79].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[79].pellet = 1;						// 샷건 펠릿 수
	Weapon[79].is_shell_reload = false;				// 셸 단위 재장전 여부
	// ▼ [수정] 약실 없음 (노리쇠가 후퇴 고정된 상태에서 사격)
	Weapon[79].bHasChamber = false;
	Weapon[79].TacticalReloadFrame = 0; // 단축 불가
	Weapon[79].SoundID_Reload = -1;
	Weapon[79].SoundID_Tactical = -1;

	Weapon[80].name = "P90 (SD)";						// 이름
	Weapon[80].model = "./data/model/weapon/P90_SD.x";			// 3D 모델 파일 경로
	Weapon[80].texture = "./data/model/weapon/p90.bmp";		// 텍스처 파일 경로
	Weapon[80].attacks = 36;						// 공격력
	Weapon[80].penetration = 1;					// 관통력
	Weapon[80].blazings = 4;						// 연사 속도
	Weapon[80].speed = 11;						// 탄속
	Weapon[80].nbsmax = 50;						// 최대 장탄수
	Weapon[80].reloads = 91;						// 재장전 시간
	Weapon[80].reaction = 6;						// 반동
	Weapon[80].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[80].ErrorRangeMAX = 18;					// 최대 명중률 오차
	Weapon[80].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[80].my = 9.0f;						// 1인칭 손 위치 Y
	Weapon[80].mz = 38.0f;						// 1인칭 손 위치 Z
	Weapon[80].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[80].flashy = 15.0f;					// 총구 화염 위치 Y
	Weapon[80].flashz = 71.0f;					// 총구 화염 위치 Z
	Weapon[80].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[80].yakkyou_py = 4.0f;					// 탄피 배출 위치 Y
	Weapon[80].yakkyou_pz = 18.0f;					// 탄피 배출 위치 Z
	Weapon[80].yakkyou_sx = -1.0f;					// 탄피 배출 속도 X
	Weapon[80].yakkyou_sy = 0.0f;					// 탄피 배출 속도 Y
	Weapon[80].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[80].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[80].burstcnt = 0;						// 점사
	Weapon[80].scopemode = 1;					// 스코프 모드
	Weapon[80].size = 7.0f;						// 3인칭 모델 크기
	Weapon[80].soundid = 76;						// 발사음 ID
	Weapon[80].soundvolume = 80;				// 발사음 크기
	Weapon[80].silencer = true;					// 소음기 여부
	Weapon[80].WeaponP = 0;						// 무기 종류
	Weapon[80].ChangeWeaponID = 8;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[80].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[80].pellet = 1;						// 샷건 펠릿 수
	Weapon[80].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[80].bHasChamber = true;
	Weapon[80].TacticalReloadFrame = 74;
	Weapon[80].SoundID_Reload = -1;
	Weapon[80].SoundID_Tactical = -1;

	Weapon[81].name = "KAC PDW (SD)";					// 이름
	Weapon[81].model = "./data/model/weapon/1/KAC_PDW_Eotech_Grip_SD.x";	// 3D 모델 파일 경로
	Weapon[81].texture = "./data/model/weapon/1/kac_pdw.bmp";		// 텍스처 파일 경로
	Weapon[81].attacks = 51;					// 공격력
	Weapon[81].penetration = 1;					// 관통력
	Weapon[81].blazings = 5;					// 연사 속도
	Weapon[81].speed = 11;						// 탄속
	Weapon[81].nbsmax = 30;						// 최대 장탄수
	Weapon[81].reloads = 79;					// 재장전 시간
	Weapon[81].reaction = 6;					// 반동
	Weapon[81].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[81].ErrorRangeMAX = 15;					// 최대 명중률 오차
	Weapon[81].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[81].my = 7.0f;						// 1인칭 손 위치 Y
	Weapon[81].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[81].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[81].flashy = 16.5f;					// 총구 화염 위치 Y
	Weapon[81].flashz = 77.0f;					// 총구 화염 위치 Z
	Weapon[81].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[81].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[81].yakkyou_pz = 25.0f;					// 탄피 배출 위치 Z
	Weapon[81].yakkyou_sx = 3.0f;					// 탄피 배출 속도 X
	Weapon[81].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[81].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[81].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[81].burstcnt = 3;         				// 점사
	Weapon[81].scopemode = 2;        				// 스코프 모드
	Weapon[81].size = 7.0f;          				// 3인칭 모델 크기
	Weapon[81].soundid = 77;          				// 발사음 ID
	Weapon[81].soundvolume = 80;     				// 발사음 크기
	Weapon[81].silencer = true;     				// 소음기 여부
	Weapon[81].WeaponP = 0;          				// 무기 종류
	Weapon[81].ChangeWeaponID = 28;  				// 발사모드 전환 시 바뀔 무기 ID
	Weapon[81].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[81].pellet = 1;           				// 샷건 펠릿 수
	Weapon[81].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[81].bHasChamber = true;
	Weapon[81].TacticalReloadFrame = 57;
	Weapon[81].SoundID_Reload = -1;
	Weapon[81].SoundID_Tactical = -1;

	Weapon[82].name = "KRISS Vector (SD)";						// 이름
	Weapon[82].model = "./data/model/weapon/3/KRISS_Vector_Eotech_SD.x";			// 3D 모델 파일 경로
	Weapon[82].texture = "./data/model/weapon/3/KRISS_Vector.bmp";		// 텍스처 파일 경로
	Weapon[82].attacks = 37;						// 공격력
	Weapon[82].penetration = 0;					// 관통력
	Weapon[82].blazings = 3;						// 연사 속도 (값이 작을수록 빠름)
	Weapon[82].speed = 10;						// 탄속
	Weapon[82].nbsmax = 30;						// 최대 장탄수
	Weapon[82].reloads = 79;						// 재장전 시간
	Weapon[82].reaction = 8;						// 반동
	Weapon[82].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[82].ErrorRangeMAX = 25;					// 최대 명중률 오차
	Weapon[82].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[82].my = 12.0f;						// 1인칭 손 위치 Y
	Weapon[82].mz = 28.0f;						// 1인칭 손 위치 Z
	Weapon[82].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[82].flashy = 15.0f;					// 총구 화염 위치 Y
	Weapon[82].flashz = 80.0f;					// 총구 화염 위치 Z
	Weapon[82].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[82].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[82].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[82].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[82].yakkyou_sy = 7.0f;					// 탄피 배출 속도 Y
	Weapon[82].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[82].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[82].burstcnt = 0;						// 점사 (0: 자동, 1: 단발, 3: 3점사, 4: 볼트액션)
	Weapon[82].scopemode = 1;					// 스코프 모드
	Weapon[82].size = 7.0f;						// 3인칭 모델 크기
	Weapon[82].soundid = 78;						// 발사음 ID
	Weapon[82].soundvolume = 80;				// 발사음 크기
	Weapon[82].silencer = true;					// 소음기 여부
	Weapon[82].WeaponP = 0;						// 무기 종류 (0: 양손, 1: 한손)
	Weapon[82].ChangeWeaponID = 47;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[82].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[82].pellet = 1;						// 샷건 펠릿 수
	Weapon[82].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[82].bHasChamber = true;
	Weapon[82].TacticalReloadFrame = 57;
	Weapon[82].SoundID_Reload = -1;
	Weapon[82].SoundID_Tactical = -1;

	Weapon[83].name = "UZI (SD)";					// 이름
	Weapon[83].model = "./data/model/weapon/3/Mini_UZI_Stock_SD.x";		// 3D 모델 파일 경로
	Weapon[83].texture = "./data/model/weapon/3/mini_UZI.bmp";		// 텍스처 파일 경로
	Weapon[83].attacks = 38;						// 공격력
	Weapon[83].penetration = 0;					// 관통력
	Weapon[83].blazings = 3.5;						// 연사 속도
	Weapon[83].speed = 10;						// 탄속
	Weapon[83].nbsmax = 25;						// 최대 장탄수
	Weapon[83].reloads = 67;						// 재장전 시간
	Weapon[83].reaction = 7;						// 반동
	Weapon[83].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[83].ErrorRangeMAX = 25;					// 최대 명중률 오차
	Weapon[83].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[83].my = 6.0f;						// 1인칭 손 위치 Y
	Weapon[83].mz = 38.0f;						// 1인칭 손 위치 Z
	Weapon[83].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[83].flashy = 13.0f;					// 총구 화염 위치 Y
	Weapon[83].flashz = 68.0f;					// 총구 화염 위치 Z
	Weapon[83].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[83].yakkyou_py = 13.0f;					// 탄피 배출 위치 Y
	Weapon[83].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[83].yakkyou_sx = 2.0f;					// 탄피 배출 속도 X
	Weapon[83].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[83].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[83].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[83].burstcnt = 0;						// 점사
	Weapon[83].scopemode = 0;					// 스코프 모드
	Weapon[83].size = 7.0f;						// 3인칭 모델 크기
	Weapon[83].soundid = 79;						// 발사음 ID
	Weapon[83].soundvolume = 80;				// 발사음 크기
	Weapon[83].silencer = true;					// 소음기 여부
	Weapon[83].WeaponP = 0;						// 무기 종류
	Weapon[83].ChangeWeaponID = 48;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[83].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[83].pellet = 1;						// 샷건 펠릿 수
	Weapon[83].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[83].bHasChamber = false;
	Weapon[83].TacticalReloadFrame = 0;
	Weapon[83].SoundID_Reload = -1;
	Weapon[83].SoundID_Tactical = -1;

	Weapon[84].name = "PP-19 Bizon (SD)";						// 이름
	Weapon[84].model = "./data/model/weapon/3/PP19_SD.x";			// 3D 모델 파일 경로
	Weapon[84].texture = "./data/model/weapon/3/PP19.bmp";		// 텍스처 파일 경로
	Weapon[84].attacks = 37;						// 공격력
	Weapon[84].penetration = 0;					// 관통력
	Weapon[84].blazings = 4.5;						// 연사 속도 (값이 작을수록 빠름)
	Weapon[84].speed = 10;						// 탄속
	Weapon[84].nbsmax = 64;						// 최대 장탄수
	Weapon[84].reloads = 86;						// 재장전 시간
	Weapon[84].reaction = 6;						// 반동
	Weapon[84].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[84].ErrorRangeMAX = 21;					// 최대 명중률 오차
	Weapon[84].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[84].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[84].mz = 28.0f;						// 1인칭 손 위치 Z
	Weapon[84].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[84].flashy = 16.0f;					// 총구 화염 위치 Y
	Weapon[84].flashz = 85.0f;					// 총구 화염 위치 Z
	Weapon[84].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[84].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[84].yakkyou_pz = 35.0f;					// 탄피 배출 위치 Z
	Weapon[84].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[84].yakkyou_sy = 7.0f;					// 탄피 배출 속도 Y
	Weapon[84].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[84].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[84].burstcnt = 0;						// 점사 (0: 자동, 1: 단발, 3: 3점사, 4: 볼트액션)
	Weapon[84].scopemode = 0;					// 스코프 모드
	Weapon[84].size = 7.0f;						// 3인칭 모델 크기
	Weapon[84].soundid = 80;						// 발사음 ID
	Weapon[84].soundvolume = 80;				// 발사음 크기
	Weapon[84].silencer = true;					// 소음기 여부
	Weapon[84].WeaponP = 0;						// 무기 종류 (0: 양손, 1: 한손)
	Weapon[84].ChangeWeaponID = 70;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[84].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[84].pellet = 1;						// 샷건 펠릿 수
	Weapon[84].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[84].bHasChamber = true;
	Weapon[84].TacticalReloadFrame = 65;
	Weapon[84].SoundID_Reload = -1;
	Weapon[84].SoundID_Tactical = -1;

	Weapon[85].name = "AK47 (SD)";					// 이름
	Weapon[85].model = "./data/model/weapon/AK47_SD.x";		// 3D 모델 파일 경로
	Weapon[85].texture = "./data/model/weapon/ak47.bmp";		// 텍스처 파일 경로
	Weapon[85].attacks = 60;					// 공격력
	Weapon[85].penetration = 1;					// 관통력
	Weapon[85].blazings = 4;					// 연사 속도
	Weapon[85].speed = 11;						// 탄속
	Weapon[85].nbsmax = 30;						// 최대 장탄수
	Weapon[85].reloads = 89;					// 재장전 시간
	Weapon[85].reaction = 9;					// 반동
	Weapon[85].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[85].ErrorRangeMAX = 20;					// 최대 명중률 오차
	Weapon[85].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[85].my = 7.0f;						// 1인칭 손 위치 Y
	Weapon[85].mz = 28.0f;						// 1인칭 손 위치 Z
	Weapon[85].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[85].flashy = 14.5f;					// 총구 화염 위치 Y
	Weapon[85].flashz = 110.0f;					// 총구 화염 위치 Z
	Weapon[85].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[85].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[85].yakkyou_pz = 41.0f;					// 탄피 배출 위치 Z
	Weapon[85].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[85].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[85].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[85].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[85].burstcnt = 0;					// 점사
	Weapon[85].scopemode = 0;					// 스코프 모드
	Weapon[85].size = 7.0f;						// 3인칭 모델 크기
	Weapon[85].soundid = 81;						// 발사음 ID
	Weapon[85].soundvolume = 80;				// 발사음 크기
	Weapon[85].silencer = true;					// 소음기 여부
	Weapon[85].WeaponP = 0;						// 무기 종류
	Weapon[85].ChangeWeaponID = 10;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[85].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[85].pellet = 1;						// 샷건 펠릿 수
	Weapon[85].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[85].bHasChamber = true;
	Weapon[85].TacticalReloadFrame = 64;
	Weapon[85].SoundID_Reload = -1;
	Weapon[85].SoundID_Tactical = -1;

	Weapon[86].name = "FAMAS (SD)";					// 이름
	Weapon[86].model = "./data/model/weapon/FAMAS_Eotech_SD.x";		// 3D 모델 파일 경로
	Weapon[86].texture = "./data/model/weapon/famas.bmp";		// 텍스처 파일 경로
	Weapon[86].attacks = 49;					// 공격력
	Weapon[86].penetration = 1;					// 관통력
	Weapon[86].blazings = 3;					// 연사 속도
	Weapon[86].speed = 11;						// 탄속
	Weapon[86].nbsmax = 25;						// 최대 장탄수
	Weapon[86].reloads = 91;					// 재장전 시간
	Weapon[86].reaction = 7;					// 반동
	Weapon[86].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[86].ErrorRangeMAX = 19;					// 최대 명중률 오차
	Weapon[86].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[86].my = 8.0f;						// 1인칭 손 위치 Y
	Weapon[86].mz = 32.0f;						// 1인칭 손 위치 Z
	Weapon[86].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[86].flashy = 15.0f;					// 총구 화염 위치 Y
	Weapon[86].flashz = 86.0f;					// 총구 화염 위치 Z
	Weapon[86].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[86].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[86].yakkyou_pz = 14.0f;					// 탄피 배출 위치 Z
	Weapon[86].yakkyou_sx = 3.0f;					// 탄피 배출 속도 X
	Weapon[86].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[86].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[86].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[86].burstcnt = 0;					// 점사
	Weapon[86].scopemode = 2;					// 스코프 모드
	Weapon[86].size = 7.0f;						// 3인칭 모델 크기
	Weapon[86].soundid = 82;						// 발사음 ID
	Weapon[86].soundvolume = 80;				// 발사음 크기
	Weapon[86].silencer = true;					// 소음기 여부
	Weapon[86].WeaponP = 0;						// 무기 종류
	Weapon[86].ChangeWeaponID = 20;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[86].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[86].pellet = 1;						// 샷건 펠릿 수
	Weapon[86].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[86].bHasChamber = true;
	Weapon[86].TacticalReloadFrame = 67;
	Weapon[86].SoundID_Reload = -1;
	Weapon[86].SoundID_Tactical = -1;

	Weapon[87].name = "M16A4 (SD)";						// 이름
	Weapon[87].model = "./data/model/weapon/2/M16A4_Acog_SD.x";		// 3D 모델 파일 경로
	Weapon[87].texture = "./data/model/weapon/2/M16A4.bmp";		// 텍스처 파일 경로
	Weapon[87].attacks = 63;						// 공격력
	Weapon[87].penetration = 1;					// 관통력
	Weapon[87].blazings = 4;						// 연사 속도
	Weapon[87].speed = 11;						// 탄속
	Weapon[87].nbsmax = 30;						// 최대 장탄수
	Weapon[87].reloads = 78;						// 재장전 시간
	Weapon[87].reaction = 6;						// 반동
	Weapon[87].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[87].ErrorRangeMAX = 19;					// 최대 명중률 오차
	Weapon[87].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[87].my = 5.0f;						// 1인칭 손 위치 Y
	Weapon[87].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[87].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[87].flashy = 14.5f;					// 총구 화염 위치 Y
	Weapon[87].flashz = 120.0f;					// 총구 화염 위치 Z
	Weapon[87].yakkyou_px = 10.0f;					// 탄피 배출 위치 X
	Weapon[87].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[87].yakkyou_pz = 40.0f;					// 탄피 배출 위치 Z
	Weapon[87].yakkyou_sx = 4.0f;					// 탄피 배출 속도 X
	Weapon[87].yakkyou_sy = 6.0f;					// 탄피 배출 속도 Y
	Weapon[87].yakkyou_size = -1.0f;					// 탄피 크기
	Weapon[87].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[87].burstcnt = 3;						// 점사
	Weapon[87].scopemode = 5;					// 스코프 모드
	Weapon[87].size = 7.0f;						// 3인칭 모델 크기
	Weapon[87].soundid = 83;						// 발사음 ID
	Weapon[87].soundvolume = 80;				// 발사음 크기
	Weapon[87].silencer = true;					// 소음기 여부
	Weapon[87].WeaponP = 0;						// 무기 종류
	Weapon[87].ChangeWeaponID = 43;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[87].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[87].pellet = 1;						// 샷건 펠릿 수
	Weapon[87].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[87].bHasChamber = true;
	Weapon[87].TacticalReloadFrame = 51;
	Weapon[87].SoundID_Reload = -1;
	Weapon[87].SoundID_Tactical = -1;

	Weapon[88].name = "G36K (SD)";					// 이름
	Weapon[88].model = "./data/model/weapon/2/G36K_Acog_SD.x";			// 3D 모델 파일 경로
	Weapon[88].texture = "./data/model/weapon/2/G36K.bmp";		// 텍스처 파일 경로
	Weapon[88].attacks = 53;					// 공격력
	Weapon[88].penetration = 1;					// 관통력
	Weapon[88].blazings = 4;					// 연사 속도
	Weapon[88].speed = 11;						// 탄속
	Weapon[88].nbsmax = 30;						// 최대 장탄수
	Weapon[88].reloads = 78;					// 재장전 시간
	Weapon[88].reaction = 8;					// 반동
	Weapon[88].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[88].ErrorRangeMAX = 20;					// 최대 명중률 오차
	Weapon[88].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[88].my = 5.0f;						// 1인칭 손 위치 Y
	Weapon[88].mz = 29.0f;						// 1인칭 손 위치 Z
	Weapon[88].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[88].flashy = 14.5f;					// 총구 화염 위치 Y
	Weapon[88].flashz = 100.0f;					// 총구 화염 위치 Z
	Weapon[88].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[88].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[88].yakkyou_pz = 15.0f;					// 탄피 배출 위치 Z
	Weapon[88].yakkyou_sx = 3.0f;					// 탄피 배출 속도 X
	Weapon[88].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[88].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[88].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[88].burstcnt = 0;					// 점사
	Weapon[88].scopemode = 3;					// 스코프 모드
	Weapon[88].size = 7.0f;						// 3인칭 모델 크기
	Weapon[88].soundid = 84;						// 발사음 ID
	Weapon[88].soundvolume = 80;				// 발사음 크기
	Weapon[88].silencer = true;					// 소음기 여부
	Weapon[88].WeaponP = 0;						// 무기 종류
	Weapon[88].ChangeWeaponID = 44;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[88].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[88].pellet = 1;						// 샷건 펠릿 수
	Weapon[88].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[88].bHasChamber = true;
	Weapon[88].TacticalReloadFrame = 51;
	Weapon[88].SoundID_Reload = -1;
	Weapon[88].SoundID_Tactical = -1;

	Weapon[89].name = "TAR-21 (SD)";					// 이름
	Weapon[89].model = "./data/model/weapon/3/TAR21_Eotech_SD.x";		// 3D 모델 파일 경로
	Weapon[89].texture = "./data/model/weapon/3/TAR21.bmp";		// 텍스처 파일 경로
	Weapon[89].attacks = 23;					// 공격력
	Weapon[89].penetration = 2;					// 관통력
	Weapon[89].blazings = 4;					// 연사 속도
	Weapon[89].speed = 11;						// 탄속
	Weapon[89].nbsmax = 30;						// 최대 장탄수
	Weapon[89].reloads = 91;					// 재장전 시간
	Weapon[89].reaction = 8;					// 반동
	Weapon[89].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[89].ErrorRangeMAX = 20;					// 최대 명중률 오차
	Weapon[89].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[89].my = 9.0f;						// 1인칭 손 위치 Y
	Weapon[89].mz = 31.0f;						// 1인칭 손 위치 Z
	Weapon[89].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[89].flashy = 16.0f;					// 총구 화염 위치 Y
	Weapon[89].flashz = 80.0f;					// 총구 화염 위치 Z
	Weapon[89].yakkyou_px = 9.0f;					// 탄피 배출 위치 X
	Weapon[89].yakkyou_py = 12.0f;					// 탄피 배출 위치 Y
	Weapon[89].yakkyou_pz = 41.0f;					// 탄피 배출 위치 Z
	Weapon[89].yakkyou_sx = 5.0f;					// 탄피 배출 속도 X
	Weapon[89].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[89].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[89].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[89].burstcnt = 0;					// 점사
	Weapon[89].scopemode = 1;					// 스코프 모드
	Weapon[89].size = 7.0f;						// 3인칭 모델 크기
	Weapon[89].soundid = 85;						// 발사음 ID
	Weapon[89].soundvolume = 80;				// 발사음 크기
	Weapon[89].silencer = true;					// 소음기 여부
	Weapon[89].WeaponP = 0;						// 무기 종류
	Weapon[89].ChangeWeaponID = 46;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[89].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[89].pellet = 3;						// 샷건 펠릿 수
	Weapon[89].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[89].bHasChamber = true;
	Weapon[89].TacticalReloadFrame = 67;
	Weapon[89].SoundID_Reload = -1;
	Weapon[89].SoundID_Tactical = -1;

	Weapon[90].name = "AUG (SD)";					// 이름
	Weapon[90].model = "./data/model/weapon/AUGA1_SD.x";			// 3D 모델 파일 경로
	Weapon[90].texture = "./data/model/weapon/aug.bmp";		// 텍스처 파일 경로
	Weapon[90].attacks = 53;					// 공격력
	Weapon[90].penetration = 1;					// 관통력
	Weapon[90].blazings = 5;					// 연사 속도
	Weapon[90].speed = 11;						// 탄속
	Weapon[90].nbsmax = 30;						// 최대 장탄수
	Weapon[90].reloads = 91;					// 재장전 시간
	Weapon[90].reaction = 7;					// 반동
	Weapon[90].ErrorRangeMIN = 0;					// 최소 명중률 오차
	Weapon[90].ErrorRangeMAX = 18;					// 최대 명중률 오차
	Weapon[90].mx = 9.0f;						// 1인칭 손 위치 X
	Weapon[90].my = 9.0f;						// 1인칭 손 위치 Y
	Weapon[90].mz = 31.0f;						// 1인칭 손 위치 Z
	Weapon[90].flashx = 9.0f;					// 총구 화염 위치 X
	Weapon[90].flashy = 16.0f;					// 총구 화염 위치 Y
	Weapon[90].flashz = 90.0f;					// 총구 화염 위치 Z
	Weapon[90].yakkyou_px = 8.0f;					// 탄피 배출 위치 X
	Weapon[90].yakkyou_py = 11.0f;					// 탄피 배출 위치 Y
	Weapon[90].yakkyou_pz = 15.0f;					// 탄피 배출 위치 Z
	Weapon[90].yakkyou_sx = 3.0f;					// 탄피 배출 속도 X
	Weapon[90].yakkyou_sy = 5.0f;					// 탄피 배출 속도 Y
	Weapon[90].yakkyou_size = -1.0f;				// 탄피 크기
	Weapon[90].yakkyou_delay = 0;					// 탄피 배출 딜레이
	Weapon[90].burstcnt = 0;					// 점사
	Weapon[90].scopemode = 3;					// 스코프 모드
	Weapon[90].size = 7.0f;						// 3인칭 모델 크기
	Weapon[90].soundid = 36;						// 발사음 ID
	Weapon[90].soundvolume = 80;				// 발사음 크기
	Weapon[90].silencer = true;					// 소음기 여부
	Weapon[90].WeaponP = 0;						// 무기 종류
	Weapon[90].ChangeWeaponID = 11;					// 발사모드 전환 시 바뀔 무기 ID
	Weapon[90].ChangeWeaponCnt = 0;					// 발사모드 전환 시간
	Weapon[90].pellet = 1;						// 샷건 펠릿 수
	Weapon[90].is_shell_reload = false;				// 셸 단위 재장전 여부
	Weapon[90].bHasChamber = true;
	Weapon[90].TacticalReloadFrame = 67;
	Weapon[90].SoundID_Reload = -1;
	Weapon[90].SoundID_Tactical = -1;

	Weapon[91].name = "IMPACT GRENADE";						// 이름
	Weapon[91].model = "./data/model/weapon/5/RGD5.x";	// 3D 모델 파일 경로
	Weapon[91].texture = "./data/model/weapon/5/RGD5.bmp";	// 텍스처 파일 경로
	Weapon[91].attacks = 0;									// 직접 공격력 없음
	Weapon[91].penetration = 0;								// 관통력
	Weapon[91].blazings = 35;								// 투척 간격
	Weapon[91].speed = 0;									// 탄속
	Weapon[91].nbsmax = 1;									// 1개씩 장전
	Weapon[91].reloads = 0;									// 자동 장전
	Weapon[91].reaction = 0;								// 반동
	Weapon[91].ErrorRangeMIN = 0;							// 최소 오차
	Weapon[91].ErrorRangeMAX = 20;							// 최대 오차
	Weapon[91].mx = 2.0f;									// 1인칭 손 위치 X
	Weapon[91].my = 12.0f;									// 1인칭 손 위치 Y
	Weapon[91].mz = 50.0f;									// 1인칭 손 위치 Z
	Weapon[91].flashx = 2.0f;								// 투척 시작 위치 X
	Weapon[91].flashy = 12.0f;								// 투척 시작 위치 Y
	Weapon[91].flashz = 50.0f;								// 투척 시작 위치 Z
	Weapon[91].yakkyou_px = 0.0f;
	Weapon[91].yakkyou_py = 0.0f;
	Weapon[91].yakkyou_pz = 0.0f;
	Weapon[91].yakkyou_sx = 0.0f;
	Weapon[91].yakkyou_sy = 0.0f;
	Weapon[91].yakkyou_size = -1.0f;
	Weapon[91].yakkyou_delay = 0;
	Weapon[91].burstcnt = 1;
	Weapon[91].scopemode = 0;
	Weapon[91].size = 7.0f;
	Weapon[91].soundid = 62;								// 기존 수류탄 투척음 사용
	Weapon[91].soundvolume = 80;
	Weapon[91].silencer = false;
	Weapon[91].WeaponP = 1;
	Weapon[91].ChangeWeaponID = -1;
	Weapon[91].ChangeWeaponCnt = 0;
	Weapon[91].pellet = 1;
	Weapon[91].is_shell_reload = false;
	Weapon[91].bHasChamber = false;
	Weapon[91].TacticalReloadFrame = 0;
	Weapon[91].SoundID_Reload = -1;
	Weapon[91].SoundID_Tactical = -1;

	Weapon[92].name = "SHIELD";							// 이름
	Weapon[92].model = "./data/model/weapon/5/shield.x";		// 실제 방패 모델 경로로 수정
	Weapon[92].texture = "./data/model/weapon/5/shield.bmp";	// 실제 방패 텍스처 경로로 수정
	Weapon[92].attacks = 0;								// 공격 없음
	Weapon[92].penetration = 0;							// 방어 계산은 이후 단계에서 공격자 무기 관통력으로 처리
	Weapon[92].blazings = 0;							// 발사/투척 딜레이 없음
	Weapon[92].speed = 0;								// 탄속 없음
	Weapon[92].nbsmax = 1;								// 탄약 없음
	Weapon[92].reloads = 0;								// 재장전 없음
	Weapon[92].reaction = 0;							// 반동 없음
	Weapon[92].ErrorRangeMIN = 0;
	Weapon[92].ErrorRangeMAX = 0;
	Weapon[92].mx = 9.0f;								// 1인칭 손 위치 X, 모델 확인 후 조정
	Weapon[92].my = 12.0f;								// 1인칭 손 위치 Y, 모델 확인 후 조정
	Weapon[92].mz = 40.0f;								// 1인칭 손 위치 Z, 모델 확인 후 조정
	Weapon[92].flashx = 0.0f;
	Weapon[92].flashy = 0.0f;
	Weapon[92].flashz = 0.0f;
	Weapon[92].yakkyou_px = 0.0f;
	Weapon[92].yakkyou_py = 0.0f;
	Weapon[92].yakkyou_pz = 0.0f;
	Weapon[92].yakkyou_sx = 0.0f;
	Weapon[92].yakkyou_sy = 0.0f;
	Weapon[92].yakkyou_size = -1.0f;
	Weapon[92].yakkyou_delay = 0;
	Weapon[92].burstcnt = 0;							// 공격 불가
	Weapon[92].scopemode = 0;
	Weapon[92].size = 8.5f;								// 모델 확인 후 조정
	Weapon[92].soundid = 0;
	Weapon[92].soundvolume = 0;
	Weapon[92].silencer = false;
	Weapon[92].WeaponP = WEAPONP_SHIELD;					// 방패 전용 팔 자세
	Weapon[92].ChangeWeaponID = -1;
	Weapon[92].ChangeWeaponCnt = 0;
	Weapon[92].pellet = 0;								// 실수로 Shot()에 들어가도 발사 실패
	Weapon[92].is_shell_reload = false;
	Weapon[92].bHasChamber = false;
	Weapon[92].TacticalReloadFrame = 0;
	Weapon[92].SoundID_Reload = -1;
	Weapon[92].SoundID_Tactical = -1;

	Weapon[93].name = "DP-28";									// 이름
	Weapon[93].model = "./data/model/weapon/5/dp28_elcan.x";			// 3D 모델 파일 경로
	Weapon[93].texture = "./data/model/weapon/5/dp28.bmp";		// 텍스처 파일 경로
	Weapon[93].attacks = 63;									// 공격력
	Weapon[93].penetration = 4;									// 관통력
	Weapon[93].blazings = 6.5f;									// 약 550rpm 기준
	Weapon[93].speed = 11;										// 탄속
	Weapon[93].nbsmax = 47;										// DP28 팬 탄창 47발
	Weapon[93].reloads = 164;									// dp28 reload 2.wav 4.94초 기준
	Weapon[93].reaction = 9;									// 반동
	Weapon[93].ErrorRangeMIN = 0;								// 최소 명중률 오차
	Weapon[93].ErrorRangeMAX = 23;								// 최대 명중률 오차
	Weapon[93].mx = 9.0f;										// 1인칭 손 위치 X
	Weapon[93].my = 6.0f;										// 1인칭 손 위치 Y
	Weapon[93].mz = 28.0f;										// 1인칭 손 위치 Z
	Weapon[93].flashx = 9.0f;									// 총구 화염 위치 X
	Weapon[93].flashy = 14.0f;									// 총구 화염 위치 Y
	Weapon[93].flashz = 126.0f;									// 총구 화염 위치 Z
	Weapon[93].yakkyou_px = 8.0f;								// 탄피 배출 위치 X
	Weapon[93].yakkyou_py = 10.0f;								// 탄피 배출 위치 Y
	Weapon[93].yakkyou_pz = 42.0f;								// 탄피 배출 위치 Z
	Weapon[93].yakkyou_sx = 4.0f;								// 탄피 배출 속도 X
	Weapon[93].yakkyou_sy = 4.0f;								// 탄피 배출 속도 Y
	Weapon[93].yakkyou_size = -1.0f;							// 탄피 크기
	Weapon[93].yakkyou_delay = 0;								// 탄피 배출 딜레이
	Weapon[93].burstcnt = 0;									// 자동 사격
	Weapon[93].scopemode = 5;									// 중거리 조준경
	Weapon[93].size = 7.0f;										// 3인칭 모델 크기
	Weapon[93].soundid = 87;										// 임시: M60 발사음 사용
	Weapon[93].soundvolume = 90;							// 발사음 크기
	Weapon[93].silencer = false;								// 소음기 여부
	Weapon[93].WeaponP = WEAPONP_TWOHAND;						// 양손 무기
	Weapon[93].ChangeWeaponID = -1;								// 발사모드 전환 없음
	Weapon[93].ChangeWeaponCnt = 0;								// 발사모드 전환 시간
	Weapon[93].pellet = 1;										// 펠릿 수
	Weapon[93].is_shell_reload = false;							// 탄창 교체식
	Weapon[93].bHasChamber = false;								// DP28은 약실 +1 처리 없음
	Weapon[93].TacticalReloadFrame = 0;							// 전술 재장전 없음
	Weapon[93].SoundID_Reload = -1;								// 아래 재장전 그룹 지정에서 설정
	Weapon[93].SoundID_Tactical = -1;

	// 로봇 처형 프로토콜 전용 Mk14 EBR.
	// 기존 Mk14 EBR의 총구화염/탄피/사운드/손 위치 값을 그대로 사용하고,
	// 모델과 스킨만 조준경 없는 전용 버전으로 교체한다.
	Weapon[ID_WEAPON_ROBOT_EXECUTE_RIFLE] = Weapon[ID_WEAPON_MK14EBR];
	Weapon[ID_WEAPON_ROBOT_EXECUTE_RIFLE].name = "Execute";
	Weapon[ID_WEAPON_ROBOT_EXECUTE_RIFLE].model = "./data/model/weapon/5/mk14_ebr_execute.x";
	Weapon[ID_WEAPON_ROBOT_EXECUTE_RIFLE].texture = "./data/model/weapon/5/mk14_ebr_execute.bmp";
	Weapon[ID_WEAPON_ROBOT_EXECUTE_RIFLE].scopemode = 0;

	for (int i = 0; i < TOTAL_PARAMETERINFO_WEAPON; i++) {
		Weapon[i].SoundID_Action = -1;
		Weapon[i].ActionSoundFrame = 0;

		Weapon[i].SoundID_ReloadStart = -1;
		Weapon[i].SoundID_ReloadEnd = -1;
	}

	// ------------------------------------------------------------
// 재장전 사운드 그룹 지정
// 큰 그룹 기준 1차 적용.
// 나중에 싱크가 어색한 총기만 별도 그룹으로 분리하면 된다.
// ------------------------------------------------------------
#define SET_RELOAD_SOUND(id, normal, tactical) \
    Weapon[id].SoundID_Reload = normal; \
    Weapon[id].SoundID_Tactical = tactical;

#define SET_SHELL_RELOAD_SOUND(id, start, insert, end) \
    Weapon[id].SoundID_ReloadStart = start; \
    Weapon[id].SoundID_Reload = insert; \
    Weapon[id].SoundID_ReloadEnd = end;

#define SET_ACTION_SOUND(id, action, frame) \
    Weapon[id].SoundID_Action = action; \
    Weapon[id].ActionSoundFrame = frame;

#define SET_SHELL_RELOAD_SOUND(id, start, insert, end) \
    Weapon[id].SoundID_ReloadStart = start; \
    Weapon[id].SoundID_Reload = insert; \
    Weapon[id].SoundID_ReloadEnd = end;

#define SET_ACTION_SOUND(id, action, frame) \
    Weapon[id].SoundID_Action = action; \
    Weapon[id].ActionSoundFrame = frame;

// 권총류
	SET_RELOAD_SOUND(3, RELOAD_PISTOL_EMPTY, RELOAD_PISTOL_TACTICAL); // M92F
	SET_RELOAD_SOUND(4, RELOAD_PISTOL_EMPTY, RELOAD_PISTOL_TACTICAL); // GLOCK18 SEMI
	SET_RELOAD_SOUND(16, RELOAD_PISTOL_EMPTY, RELOAD_PISTOL_TACTICAL); // GLOCK18 FULL
	SET_RELOAD_SOUND(17, RELOAD_PISTOL_EMPTY, RELOAD_PISTOL_TACTICAL); // M1911
	SET_RELOAD_SOUND(18, RELOAD_PISTOL_EMPTY, RELOAD_PISTOL_TACTICAL); // GLOCK17
	SET_RELOAD_SOUND(52, RELOAD_PISTOL_EMPTY, RELOAD_PISTOL_TACTICAL); // Beretta 93R
	SET_RELOAD_SOUND(61, RELOAD_PISTOL_EMPTY, RELOAD_PISTOL_TACTICAL); // P38
	SET_RELOAD_SOUND(75, RELOAD_PISTOL_EMPTY, RELOAD_PISTOL_TACTICAL); // GLOCK17 SD
	SET_RELOAD_SOUND(76, RELOAD_PISTOL_EMPTY, RELOAD_PISTOL_TACTICAL); // Beretta 93R SD

	// 권총류 2
	SET_RELOAD_SOUND(5, RELOAD_HEAVY_PISTOL_EMPTY, RELOAD_HEAVY_PISTOL_TACTICAL); // DESERT EAGLE
	SET_RELOAD_SOUND(21, RELOAD_HEAVY_PISTOL_EMPTY, RELOAD_HEAVY_PISTOL_TACTICAL); // MK23
	SET_RELOAD_SOUND(22, RELOAD_HEAVY_PISTOL_EMPTY, RELOAD_HEAVY_PISTOL_TACTICAL); // MK23 SD

	// 리볼버
	SET_RELOAD_SOUND(53, RELOAD_REVOLVER, -1); // M29 Satan

	// 일반 SMG
	SET_RELOAD_SOUND(1, RELOAD_SMG_EMPTY, RELOAD_SMG_TACTICAL); // MP5
	SET_RELOAD_SOUND(7, RELOAD_SMG_EMPTY, RELOAD_SMG_TACTICAL); // UMP
	SET_RELOAD_SOUND(14, RELOAD_SMG_EMPTY, RELOAD_SMG_TACTICAL); // MP5SD
	SET_RELOAD_SOUND(23, RELOAD_SMG_EMPTY, RELOAD_SMG_TACTICAL); // M1 Carbine
	SET_RELOAD_SOUND(26, RELOAD_SMG_EMPTY, RELOAD_SMG_TACTICAL); // UMPSD
	SET_RELOAD_SOUND(28, RELOAD_SMG_EMPTY, RELOAD_SMG_TACTICAL); // KAC PDW
	SET_RELOAD_SOUND(34, RELOAD_SMG_EMPTY, RELOAD_SMG_TACTICAL); // MP7A1
	SET_RELOAD_SOUND(47, RELOAD_SMG_EMPTY, RELOAD_SMG_TACTICAL); // KRISS Vector
	SET_RELOAD_SOUND(51, RELOAD_SMG_EMPTY, RELOAD_SMG_TACTICAL); // MP9 SD
	SET_RELOAD_SOUND(69, RELOAD_SMG_EMPTY, RELOAD_SMG_TACTICAL); // Vz.61 Skorpion
	SET_RELOAD_SOUND(73, RELOAD_SMG_EMPTY, RELOAD_SMG_TACTICAL); // K1A
	SET_RELOAD_SOUND(77, RELOAD_SMG_EMPTY, RELOAD_SMG_TACTICAL); // Vz.61 Skorpion SD
	SET_RELOAD_SOUND(78, RELOAD_SMG_EMPTY, RELOAD_SMG_TACTICAL); // MP9 Std
	SET_RELOAD_SOUND(81, RELOAD_SMG_EMPTY, RELOAD_SMG_TACTICAL); // KAC PDW SD
	SET_RELOAD_SOUND(82, RELOAD_SMG_EMPTY, RELOAD_SMG_TACTICAL); // KRISS Vector SD


	// 구형 / 오픈볼트 SMG
	SET_RELOAD_SOUND(6, RELOAD_CLASSIC_SMG, -1); // MAC10
	SET_RELOAD_SOUND(33, RELOAD_CLASSIC_SMG, -1); // MP40
	SET_RELOAD_SOUND(48, RELOAD_CLASSIC_SMG, -1); // UZI
	SET_RELOAD_SOUND(65, RELOAD_CLASSIC_SMG, -1); // Thompson
	SET_RELOAD_SOUND(79, RELOAD_CLASSIC_SMG, -1); // MAC10 SD
	SET_RELOAD_SOUND(83, RELOAD_CLASSIC_SMG, -1); // UZI SD

	// 일반 AR / 현대 소총
	SET_RELOAD_SOUND(9, RELOAD_AR_EMPTY, RELOAD_AR_TACTICAL); // M4
	SET_RELOAD_SOUND(24, RELOAD_AR_EMPTY, RELOAD_AR_TACTICAL); // M4 SD
	SET_RELOAD_SOUND(31, RELOAD_AR_EMPTY, RELOAD_AR_TACTICAL); // SIG 556
	SET_RELOAD_SOUND(32, RELOAD_AR_EMPTY, RELOAD_AR_TACTICAL); // SIG 556 SD
	SET_RELOAD_SOUND(37, RELOAD_AR_EMPTY, RELOAD_AR_TACTICAL); // SCAR
	SET_RELOAD_SOUND(38, RELOAD_AR_EMPTY, RELOAD_AR_TACTICAL); // SCAR SD
	SET_RELOAD_SOUND(41, RELOAD_AR_EMPTY, RELOAD_AR_TACTICAL); // HK416
	SET_RELOAD_SOUND(42, RELOAD_AR_EMPTY, RELOAD_AR_TACTICAL); // HK416 SD
	SET_RELOAD_SOUND(43, RELOAD_AR_EMPTY, RELOAD_AR_TACTICAL); // M16A4
	SET_RELOAD_SOUND(44, RELOAD_AR_EMPTY, RELOAD_AR_TACTICAL); // G36K
	SET_RELOAD_SOUND(74, RELOAD_AR_EMPTY, RELOAD_AR_TACTICAL); // K2C
	SET_RELOAD_SOUND(87, RELOAD_AR_EMPTY, RELOAD_AR_TACTICAL); // M16A4 SD
	SET_RELOAD_SOUND(88, RELOAD_AR_EMPTY, RELOAD_AR_TACTICAL); // G36K SD

	// AK 계열
	SET_RELOAD_SOUND(10, RELOAD_AK_EMPTY, RELOAD_AK_TACTICAL); // AK47
	SET_RELOAD_SOUND(29, RELOAD_AK_EMPTY, RELOAD_AK_TACTICAL); // Galil
	SET_RELOAD_SOUND(64, RELOAD_AK_EMPTY, RELOAD_AK_TACTICAL); // STG44
	SET_RELOAD_SOUND(85, RELOAD_AK_EMPTY, RELOAD_AK_TACTICAL); // AK47 SD


	// 불펍
	SET_RELOAD_SOUND(11, RELOAD_BULLPUP_EMPTY, RELOAD_BULLPUP_TACTICAL); // AUG
	SET_RELOAD_SOUND(20, RELOAD_BULLPUP_EMPTY, RELOAD_BULLPUP_TACTICAL); // FAMAS
	SET_RELOAD_SOUND(45, RELOAD_BULLPUP_EMPTY, RELOAD_BULLPUP_TACTICAL); // F2000
	SET_RELOAD_SOUND(46, RELOAD_BULLPUP_EMPTY, RELOAD_BULLPUP_TACTICAL); // TAR-21
	SET_RELOAD_SOUND(86, RELOAD_BULLPUP_EMPTY, RELOAD_BULLPUP_TACTICAL); // FAMAS SD
	SET_RELOAD_SOUND(89, RELOAD_BULLPUP_EMPTY, RELOAD_BULLPUP_TACTICAL); // TAR-21 SD
	SET_RELOAD_SOUND(90, RELOAD_BULLPUP_EMPTY, RELOAD_BULLPUP_TACTICAL); // AUG SD

	// P90 / Bizon
	SET_RELOAD_SOUND(8, RELOAD_P90_EMPTY, RELOAD_P90_TACTICAL); // P90
	SET_RELOAD_SOUND(80, RELOAD_P90_EMPTY, RELOAD_P90_TACTICAL); // P90 SD
	SET_RELOAD_SOUND(72, RELOAD_P90_EMPTY, RELOAD_P90_TACTICAL); // AR-57

	SET_RELOAD_SOUND(70, RELOAD_BIZON_EMPTY, RELOAD_BIZON_TACTICAL); // Bizon
	SET_RELOAD_SOUND(84, RELOAD_BIZON_EMPTY, RELOAD_BIZON_TACTICAL); // Bizon SD
	SET_RELOAD_SOUND(57, RELOAD_BIZON_EMPTY, RELOAD_BIZON_TACTICAL); // Calico, 나중에 전용으로 분리 가능
	SET_RELOAD_SOUND(71, RELOAD_BIZON_EMPTY, RELOAD_BIZON_TACTICAL); // CF-05


	// LMG / 미니건
		// LMG / 미니건
	SET_RELOAD_SOUND(12, RELOAD_LMG_BELT, -1); // M249
	SET_RELOAD_SOUND(30, RELOAD_LMG_BELT, -1); // M60
	SET_RELOAD_SOUND(67, RELOAD_LMG_BELT, -1); // MG42
	SET_RELOAD_SOUND(ID_WEAPON_DP28, RELOAD_DP28, -1); // DP28
	SET_RELOAD_SOUND(55, RELOAD_MINIGUN, -1); // Minigun



	// 저격 / DMR
	SET_RELOAD_SOUND(2, RELOAD_DMR_EMPTY, RELOAD_DMR_TACTICAL); // PSG-1
	SET_RELOAD_SOUND(27, RELOAD_DMR_EMPTY, RELOAD_DMR_TACTICAL); // Mk14 EBR
	SET_RELOAD_SOUND(39, RELOAD_DMR_EMPTY, RELOAD_DMR_TACTICAL); // SVD
	SET_RELOAD_SOUND(54, RELOAD_DMR_EMPTY, RELOAD_DMR_TACTICAL); // WA2000
	SET_RELOAD_SOUND(40, RELOAD_SNIPER_EMPTY, RELOAD_SNIPER_TACTICAL); // AWP, 나중에 볼트액션 전용으로 분리 가능

	// 볼트액션 장전
	SET_ACTION_SOUND(40, RELOAD_AWP_BOLT, 18);                 // AWP
	SET_ACTION_SOUND(58, RELOAD_KAR98_SNIPER_BOLT, 18);        // KAR98K Sniper
	SET_ACTION_SOUND(59, RELOAD_KAR98_SNIPER_BOLT, 18);        // KAR98K Sniper
	SET_ACTION_SOUND(63, RELOAD_KAR98_SNIPER_BOLT, 18);         // Springfield M1903

	// 샷건 계열: 시작 - 셸 삽입 - 펌프/종료
	SET_SHELL_RELOAD_SOUND(19, RELOAD_RELOAD_START, RELOAD_SHOTGUN_SHELL_INSERT, RELOAD_SHOTGUN_ACTION); // M1
	SET_SHELL_RELOAD_SOUND(25, RELOAD_RELOAD_START, RELOAD_SHOTGUN_SHELL_INSERT, RELOAD_SHOTGUN_ACTION); // SPAS-12 AUTO
	SET_SHELL_RELOAD_SOUND(68, RELOAD_RELOAD_START, RELOAD_SHOTGUN_SHELL_INSERT, RELOAD_SHOTGUN_ACTION); // SPAS-12 PUMP

	// 윈체스터: 시작 - 레버탄 삽입 - 레버액션
	SET_SHELL_RELOAD_SOUND(36, RELOAD_RELOAD_START, RELOAD_LEVER_SHELL_INSERT, RELOAD_WINCHESTER_LEVER); // Winchester1873

	// 볼트액션 내부장전: 볼트 열기 - 탄 삽입 - 볼트 닫기
	SET_SHELL_RELOAD_SOUND(59, RELOAD_BOLT_RELOAD_START, RELOAD_SNIPER_SHELL_INSERT, RELOAD_KAR98_SNIPER_BOLT); // KAR98K Sniper
	SET_SHELL_RELOAD_SOUND(63, RELOAD_BOLT_RELOAD_START, RELOAD_SNIPER_SHELL_INSERT, RELOAD_KAR98_SNIPER_BOLT); // Springfield M1903

	// 볼트액션 장전
	SET_ACTION_SOUND(19, RELOAD_SHOTGUN_ACTION, 14);     // M1
	SET_ACTION_SOUND(68, RELOAD_SHOTGUN_ACTION, 14);      // SPAS-12 PUMP
	SET_ACTION_SOUND(36, RELOAD_WINCHESTER_LEVER, 16);      // Winchester1873

	// 특수
	SET_RELOAD_SOUND(35, RELOAD_SHOTGUN_MAG_EMPTY, RELOAD_SHOTGUN_MAG_TACTICAL); // Saiga-12
	SET_RELOAD_SOUND(49, RELOAD_DRUM_SHOTGUN, -1); // Jackhammer, 나중에 drum_shotgun으로 분리 가능
	SET_RELOAD_SOUND(50, RELOAD_DOUBLE_BARREL, -1); // Double Barrel
	SET_RELOAD_SOUND(56, RELOAD_BAR, -1); // BAR
	SET_RELOAD_SOUND(62, RELOAD_BAR, -1); // PPSH-41
	SET_RELOAD_SOUND(58, RELOAD_KAR98_CLIP, -1); // KAR98K, 임시. 나중에 kar98_clip으로 분리 가능
	SET_RELOAD_SOUND(60, RELOAD_GARAND_CLIP, -1); // M1 Garand
	SET_RELOAD_SOUND(66, RELOAD_SHOTGUN_MAG_EMPTY, RELOAD_SHOTGUN_MAG_TACTICAL); // USAS-12

#undef SET_RELOAD_SOUND
#undef SET_ACTION_SOUND
#undef SET_SHELL_RELOAD_SOUND



#ifdef ENABLE_BUG_HUMANWEAPON
	//벫롪궶긫긐븧딇뾭긢??
	BugWeapon[0].name = "BugWeapon";
	BugWeapon[0].model = "";
	BugWeapon[0].texture = "";
	BugWeapon[0].attacks = 0;
	BugWeapon[0].penetration = 0;
	BugWeapon[0].blazings = 0;
	BugWeapon[0].speed = 0;
	BugWeapon[0].nbsmax = 0;
	BugWeapon[0].reloads = 0;
	BugWeapon[0].reaction = 0;
	BugWeapon[0].ErrorRangeMIN = 0;
	BugWeapon[0].ErrorRangeMAX = 0;
	BugWeapon[0].mx = 0.0f;
	BugWeapon[0].my = 0.0f;
	BugWeapon[0].mz = 0.0f;
	BugWeapon[0].flashx = 0.0f;
	BugWeapon[0].flashy = 0.0f;
	BugWeapon[0].flashz = 0.0f;
	BugWeapon[0].yakkyou_px = 0.0f;
	BugWeapon[0].yakkyou_py = 0.0f;
	BugWeapon[0].yakkyou_pz = 0.0f;
	BugWeapon[0].yakkyou_sx = 0.0f;
	BugWeapon[0].yakkyou_sy = 0.0f;
	BugWeapon[0].yakkyou_size = -1.0f;
	BugWeapon[0].yakkyou_delay = 0;
	BugWeapon[0].burstcnt = 0;
	BugWeapon[0].scopemode = 0;
	BugWeapon[0].size = 9.0f;
	BugWeapon[0].soundid = 0;
	BugWeapon[0].soundvolume = 0;
	BugWeapon[0].silencer = false;
	BugWeapon[0].WeaponP = 1;
	BugWeapon[0].ChangeWeaponID = -1;
	BugWeapon[0].ChangeWeaponCnt = 0;
	BugWeapon[0].pellet = 0;
#endif


	//듙
	SmallObject[0].model = "./data/article/can.x";
	SmallObject[0].texture = "./data/article/can.bmp";
	SmallObject[0].decide = 10;
	SmallObject[0].hp = 6;
	SmallObject[0].sound = 0;
	SmallObject[0].jump = 10;
	//긬?긓깛
	SmallObject[1].model = "./data/article/pc.x";
	SmallObject[1].texture = "./data/article/pc.bmp";
	SmallObject[1].decide = 29;
	SmallObject[1].hp = 60;
	SmallObject[1].sound = 0;
	SmallObject[1].jump = 5;
	//긬?긓깛 긌???긤뤵
	SmallObject[2].model = "./data/article/pc2.x";
	SmallObject[2].texture = "./data/article/pc.bmp";
	SmallObject[2].decide = 29;
	SmallObject[2].hp = 60;
	SmallObject[2].sound = 0;
	SmallObject[2].jump = 5;
	//긬?긓깛 ?뫬땤
	SmallObject[3].model = "./data/article/pc3.x";
	SmallObject[3].texture = "./data/article/pc.bmp";
	SmallObject[3].decide = 29;
	SmallObject[3].hp = 60;
	SmallObject[3].sound = 0;
	SmallObject[3].jump = 5;
	//긬?긓깛 깗귽긤
	SmallObject[4].model = "./data/article/pc_w.x";
	SmallObject[4].texture = "./data/article/pc.bmp";
	SmallObject[4].decide = 29;
	SmallObject[4].hp = 60;
	SmallObject[4].sound = 0;
	SmallObject[4].jump = 5;
	//긬?긓깛 깗귽긤 긌???긤뤵
	SmallObject[5].model = "./data/article/pc2_w.x";
	SmallObject[5].texture = "./data/article/pc.bmp";
	SmallObject[5].decide = 29;
	SmallObject[5].hp = 60;
	SmallObject[5].sound = 0;
	SmallObject[5].jump = 5;
	//긬?긓깛 깗귽긤 ?뫬땤
	SmallObject[6].model = "./data/article/pc3_w.x";
	SmallObject[6].texture = "./data/article/pc.bmp";
	SmallObject[6].decide = 29;
	SmallObject[6].hp = 60;
	SmallObject[6].sound = 0;
	SmallObject[6].jump = 5;
	//댳럔
	SmallObject[7].model = "./data/article/isu.x";
	SmallObject[7].texture = "./data/article/isu.bmp";
	SmallObject[7].decide = 56;
	SmallObject[7].hp = 50;
	SmallObject[7].sound = 1;
	SmallObject[7].jump = 6;
	//?깛??깑
	SmallObject[8].model = "./data/article/dan.x";
	SmallObject[8].texture = "./data/article/dan.bmp";
	SmallObject[8].decide = 28;
	SmallObject[8].hp = 35;
	SmallObject[8].sound = 1;
	SmallObject[8].jump = 8;
	//긬?긓깛 딳벍뭷
	SmallObject[9].model = "./data/article/pc.x";
	SmallObject[9].texture = "./data/article/pc_sw.bmp";
	SmallObject[9].decide = 29;
	SmallObject[9].hp = 60;
	SmallObject[9].sound = 0;
	SmallObject[9].jump = 5;
	//긬?긓깛 딳벍뭷 댠
	SmallObject[10].model = "./data/article/pc.x";
	SmallObject[10].texture = "./data/article/pc_d.bmp";
	SmallObject[10].decide = 29;
	SmallObject[10].hp = 60;
	SmallObject[10].sound = 0;
	SmallObject[10].jump = 5;
	//긬귽깓깛
	SmallObject[11].model = "./data/article/cone.x";
	SmallObject[11].texture = "./data/article/cone.bmp";
	SmallObject[11].decide = 35;
	SmallObject[11].hp = 30;
	SmallObject[11].sound = 1;
	SmallObject[11].jump = 7;


	//뢤뭙긆긳긙긃긏긣
	Bullet[0].model = "./data/model/bullet.x";
	Bullet[0].texture = "./data/model/bullet.bmp";
	Bullet[0].size = 1.0f;
	//롨왮뭙긆긳긙긃긏긣
	Bullet[1].model = Weapon[ID_WEAPON_GRENADE].model;
	Bullet[1].texture = Weapon[ID_WEAPON_GRENADE].texture;
	Bullet[1].size = Weapon[ID_WEAPON_GRENADE].size;


	MissionData[0].name			= "TRAINING YARD";
	MissionData[0].fullname		= "TRAINING YARD training";
	MissionData[0].directory	= "data\\map0\\";
	MissionData[0].txt			= "tr";
	MissionData[0].collision	= false;
	MissionData[0].screen		= false;
	MissionData[1].name			= "UNDERGROUND_EXT";
	MissionData[1].fullname		= "UNDERGROUND extermination";
	MissionData[1].directory	= "data\\map5\\";
	MissionData[1].txt			= "ext";
	MissionData[1].collision	= false;
	MissionData[1].screen		= false;
	MissionData[2].name			= "BUILDING_EXT";
	MissionData[2].fullname		= "BUILDING extermination";
	MissionData[2].directory	= "data\\map1\\";
	MissionData[2].txt			= "ext";
	MissionData[2].collision	= false;
	MissionData[2].screen		= false;
	MissionData[3].name			= "SNOW BASE_EXT";
	MissionData[3].fullname		= "SNOW BASE extermination";
	MissionData[3].directory	= "data\\map2\\";
	MissionData[3].txt			= "ext";
	MissionData[3].collision	= false;
	MissionData[3].screen		= false;
	MissionData[4].name			= "MBASE_EXT";
	MissionData[4].fullname		= "MILITARY BASE extermination";
	MissionData[4].directory	= "data\\map4\\";
	MissionData[4].txt			= "ext";
	MissionData[4].collision	= false;
	MissionData[4].screen		= false;
	MissionData[5].name			= "WAREHOUSE_EXT";
	MissionData[5].fullname		= "WAREHOUSE AREA extermination";
	MissionData[5].directory	= "data\\map7\\";
	MissionData[5].txt			= "ext";
	MissionData[5].collision	= false;
	MissionData[5].screen		= false;
	MissionData[6].name			= "DUEL_EXT";
	MissionData[6].fullname		= "DUEL extermination";
	MissionData[6].directory	= "data\\map9\\";
	MissionData[6].txt			= "ext";
	MissionData[6].collision	= false;
	MissionData[6].screen		= false;
	MissionData[7].name			= "STATION_EXT";
	MissionData[7].fullname		= "SUBWAY STATION extermination";
	MissionData[7].directory	= "data\\map6\\";
	MissionData[7].txt			= "ext";
	MissionData[7].collision	= false;
	MissionData[7].screen		= false;
	MissionData[8].name			= "MAZE_EXT";
	MissionData[8].fullname		= "MAZE extermination";
	MissionData[8].directory	= "data\\map13\\";
	MissionData[8].txt			= "ext";
	MissionData[8].collision	= false;
	MissionData[8].screen		= false;
	MissionData[9].name			= "RUINS_EXT";
	MissionData[9].fullname		= "RUINS extermination";
	MissionData[9].directory	= "data\\map14\\";
	MissionData[9].txt			= "ext";
	MissionData[9].collision	= false;
	MissionData[9].screen		= false;
	MissionData[10].name		= "URBAN_EXT";
	MissionData[10].fullname	= "URBAN extermination";
	MissionData[10].directory	= "data\\map8\\";
	MissionData[10].txt			= "ext";
	MissionData[10].collision	= false;
	MissionData[10].screen		= false;
	MissionData[11].name		= "UNDERGROUND_EXT2";
	MissionData[11].fullname	= "UNDERGROUND extermination2";
	MissionData[11].directory	= "data\\map5\\";
	MissionData[11].txt			= "ext2";
	MissionData[11].collision	= false;
	MissionData[11].screen		= false;
	MissionData[12].name		= "TUNNEL_EXT";
	MissionData[12].fullname	= "TUNNEL extermination";
	MissionData[12].directory	= "data\\map12\\";
	MissionData[12].txt			= "ext";
	MissionData[12].collision	= false;
	MissionData[12].screen		= false;
	MissionData[13].name		= "URBAN_DEF";
	MissionData[13].fullname	= "URBAN defend target";
	MissionData[13].directory	= "data\\map8\\";
	MissionData[13].txt			= "def";
	MissionData[13].collision	= false;
	MissionData[13].screen		= false;
	MissionData[14].name		= "DTOWN_EXT";
	MissionData[14].fullname	= "DESERT TOWN extermination";
	MissionData[14].directory	= "data\\map3\\";
	MissionData[14].txt			= "ext";
	MissionData[14].collision	= false;
	MissionData[14].screen		= false;
	MissionData[15].name		= "URBAN_DEF2";
	MissionData[15].fullname	= "URBAN defend target2";
	MissionData[15].directory	= "data\\map8\\";
	MissionData[15].txt			= "def2";
	MissionData[15].collision	= false;
	MissionData[15].screen		= false;
	MissionData[16].name		= "WAREHOUSE_KT";
	MissionData[16].fullname	= "WAREHOUSE AREA kill the target";
	MissionData[16].directory	= "data\\map7\\";
	MissionData[16].txt			= "kt";
	MissionData[16].collision	= false;
	MissionData[16].screen		= false;
	MissionData[17].name		= "RUINS_RE";
	MissionData[17].fullname	= "RUINS release";
	MissionData[17].directory	= "data\\map14\\";
	MissionData[17].txt			= "re";
	MissionData[17].collision	= false;
	MissionData[17].screen		= false;
	MissionData[18].name		= "RELIC_CAP";
	MissionData[18].fullname	= "RELIC capture";
	MissionData[18].directory	= "data\\map16\\";
	MissionData[18].txt			= "cap";
	MissionData[18].collision	= false;
	MissionData[18].screen		= false;
	MissionData[19].name		= "MBASE_DE";
	MissionData[19].fullname	= "MILITARY BASE destroy";
	MissionData[19].directory	= "data\\map4\\";
	MissionData[19].txt			= "de";
	MissionData[19].collision	= false;
	MissionData[19].screen		= false;
	MissionData[20].name		= "RUINS_CAP";
	MissionData[20].fullname	= "RUINS capture";
	MissionData[20].directory	= "data\\map14\\";
	MissionData[20].txt			= "cap";
	MissionData[20].collision	= false;
	MissionData[20].screen		= false;
	MissionData[21].name		= "DTOWN_KT";
	MissionData[21].fullname	= "DESERT TOWN kill the target";
	MissionData[21].directory	= "data\\map3\\";
	MissionData[21].txt			= "kt";
	MissionData[21].collision	= false;
	MissionData[21].screen		= false;
	MissionData[22].name		= "SNOW BASE_RE";
	MissionData[22].fullname	= "SNOW BASE release";
	MissionData[22].directory	= "data\\map2\\";
	MissionData[22].txt			= "re";
	MissionData[22].collision	= false;
	MissionData[22].screen		= false;
	MissionData[23].name		= "UNDERGROUND_DEF";
	MissionData[23].fullname	= "UNDERGROUND defend target";
	MissionData[23].directory	= "data\\map5\\";
	MissionData[23].txt			= "def";
	MissionData[23].collision	= false;
	MissionData[23].screen		= false;
	MissionData[24].name		= "TUNNEL_EXT2";
	MissionData[24].fullname	= "TUNNEL extermination2";
	MissionData[24].directory	= "data\\map12\\";
	MissionData[24].txt			= "ext2";
	MissionData[24].collision	= false;
	MissionData[24].screen		= false;
	MissionData[25].name		= "RELIC_DEF";
	MissionData[25].fullname	= "RELIC defend target";
	MissionData[25].directory	= "data\\map16\\";
	MissionData[25].txt			= "def";
	MissionData[25].collision	= false;
	MissionData[25].screen		= false;
	MissionData[26].name		= "URBAN_KT";
	MissionData[26].fullname	= "URBAN kill the target";
	MissionData[26].directory	= "data\\map8\\";
	MissionData[26].txt			= "kt";
	MissionData[26].collision	= false;
	MissionData[26].screen		= false;
	MissionData[27].name		= "ALLEY_KT";
	MissionData[27].fullname	= "ALLEY kill the target";
	MissionData[27].directory	= "data\\map10\\";
	MissionData[27].txt			= "kt";
	MissionData[27].collision	= false;
	MissionData[27].screen		= false;
	MissionData[28].name		= "STATION_KT";
	MissionData[28].fullname	= "SUBWAY STATION kill the target";
	MissionData[28].directory	= "data\\map6\\";
	MissionData[28].txt			= "kt";
	MissionData[28].collision	= false;
	MissionData[28].screen		= false;
	MissionData[29].name		= "WAREHOUSE_DEF";
	MissionData[29].fullname	= "WAREHOUSE AREA defend target";
	MissionData[29].directory	= "data\\map7\\";
	MissionData[29].txt			= "def";
	MissionData[29].collision	= false;
	MissionData[29].screen		= false;
	MissionData[30].name		= "URBAN_KT2";
	MissionData[30].fullname	= "URBAN kill the target2";
	MissionData[30].directory	= "data\\map8\\";
	MissionData[30].txt			= "kt2";
	MissionData[30].collision	= false;
	MissionData[30].screen		= false;
	MissionData[31].name		= "OFFICE_DEF";
	MissionData[31].fullname	= "OFFICE defend target";
	MissionData[31].directory	= "data\\map15\\";
	MissionData[31].txt			= "def";
	MissionData[31].collision	= false;
	MissionData[31].screen		= false;
	MissionData[32].name		= "URBAN_CAP";
	MissionData[32].fullname	= "URBAN capture";
	MissionData[32].directory	= "data\\map8\\";
	MissionData[32].txt			= "cap";
	MissionData[32].collision	= false;
	MissionData[32].screen		= false;
	MissionData[33].name		= "ALLEY_EXT";
	MissionData[33].fullname	= "ALLEY extermination";
	MissionData[33].directory	= "data\\map10\\";
	MissionData[33].txt			= "ext";
	MissionData[33].collision	= false;
	MissionData[33].screen		= true;
	MissionData[34].name		= "TUNNEL_ESC";
	MissionData[34].fullname	= "TUNNEL escape";
	MissionData[34].directory	= "data\\map12\\";
	MissionData[34].txt			= "esc";
	MissionData[34].collision	= false;
	MissionData[34].screen		= true;
	MissionData[35].name		= "MAZE_CAP";
	MissionData[35].fullname	= "MAZE capture";
	MissionData[35].directory	= "data\\map13\\";
	MissionData[35].txt			= "cap";
	MissionData[35].collision	= false;
	MissionData[35].screen		= true;
	MissionData[36].name		= "WAREHOUSE_DEF2";
	MissionData[36].fullname	= "WAREHOUSE AREA defend target2";
	MissionData[36].directory	= "data\\map7\\";
	MissionData[36].txt			= "def2";
	MissionData[36].collision	= false;
	MissionData[36].screen		= false;
	MissionData[37].name		= "RUINS_DE";
	MissionData[37].fullname	= "RUINS destroy";
	MissionData[37].directory	= "data\\map14\\";
	MissionData[37].txt			= "de";
	MissionData[37].collision	= false;
	MissionData[37].screen		= false;
	MissionData[38].name		= "URBAN_KT3";
	MissionData[38].fullname	= "URBAN kill the target3";
	MissionData[38].directory	= "data\\map8\\";
	MissionData[38].txt			= "kt3";
	MissionData[38].collision	= false;
	MissionData[38].screen		= false;
	MissionData[39].name		= "DUEL_EXT2";
	MissionData[39].fullname	= "DUEL extermination2";
	MissionData[39].directory	= "data\\map9\\";
	MissionData[39].txt			= "ext2";
	MissionData[39].collision	= false;
	MissionData[39].screen		= false;
	MissionData[40].name		= "RELIC_EXT";
	MissionData[40].fullname	= "RELIC extermination";
	MissionData[40].directory	= "data\\map16\\";
	MissionData[40].txt			= "ext";
	MissionData[40].collision	= false;
	MissionData[40].screen		= false;
	MissionData[41].name		= "SNOW BASE_DEF";
	MissionData[41].fullname	= "SNOW BASE defend target";
	MissionData[41].directory	= "data\\map2\\";
	MissionData[41].txt			= "def";
	MissionData[41].collision	= false;
	MissionData[41].screen		= false;
	MissionData[42].name		= "BUILDING_KT";
	MissionData[42].fullname	= "BUILDING kill the target";
	MissionData[42].directory	= "data\\map1\\";
	MissionData[42].txt			= "kt";
	MissionData[42].collision	= false;
	MissionData[42].screen		= false;
	MissionData[43].name		= "MAZE_ESC";
	MissionData[43].fullname	= "MAZE escape";
	MissionData[43].directory	= "data\\map13\\";
	MissionData[43].txt			= "esc";
	MissionData[43].collision	= false;
	MissionData[43].screen		= true;
	MissionData[44].name		= "ALLEY_EXT2";
	MissionData[44].fullname	= "ALLEY extermination2";
	MissionData[44].directory	= "data\\map10\\";
	MissionData[44].txt			= "ext2";
	MissionData[44].collision	= false;
	MissionData[44].screen		= false;
	MissionData[45].name		= "MBASE_ESC";
	MissionData[45].fullname	= "MILITARY BASE escape";
	MissionData[45].directory	= "data\\map4\\";
	MissionData[45].txt			= "esc";
	MissionData[45].collision	= false;
	MissionData[45].screen		= false;
	MissionData[46].name		= "DTOWN_DEF";
	MissionData[46].fullname	= "DESERT TOWN defend target";
	MissionData[46].directory	= "data\\map3\\";
	MissionData[46].txt			= "def";
	MissionData[46].collision	= false;
	MissionData[46].screen		= false;
	MissionData[47].name		= "OFFICE_KT";
	MissionData[47].fullname	= "OFFICE kill the target";
	MissionData[47].directory	= "data\\map15\\";
	MissionData[47].txt			= "kt";
	MissionData[47].collision	= false;
	MissionData[47].screen		= false;
	MissionData[48].name		= "BUILDING_DEF";
	MissionData[48].fullname	= "BUILDING defend target";
	MissionData[48].directory	= "data\\map1\\";
	MissionData[48].txt			= "def";
	MissionData[48].collision	= false;
	MissionData[48].screen		= false;
	MissionData[49].name		= "OFFICE_KT2";
	MissionData[49].fullname	= "OFFICE kill the target2";
	MissionData[49].directory	= "data\\map15\\";
	MissionData[49].txt			= "kt2";
	MissionData[49].collision	= false;
	MissionData[49].screen		= false;
	MissionData[50].name		= "MBASE_CAP";
	MissionData[50].fullname	= "MILITARY BASE capture";
	MissionData[50].directory	= "data\\map4\\";
	MissionData[50].txt			= "cap";
	MissionData[50].collision	= false;
	MissionData[50].screen		= false;
	MissionData[51].name		= "TUNNEL_KT";
	MissionData[51].fullname	= "TUNNEL kill the target";
	MissionData[51].directory	= "data\\map12\\";
	MissionData[51].txt			= "kt";
	MissionData[51].collision	= false;
	MissionData[51].screen		= false;
	MissionData[52].name		= "OFFICE_RE";
	MissionData[52].fullname	= "OFFICE release";
	MissionData[52].directory	= "data\\map15\\";
	MissionData[52].txt			= "re";
	MissionData[52].collision	= false;
	MissionData[52].screen		= false;
	MissionData[53].name		= "URBAN_KT4";
	MissionData[53].fullname	= "URBAN kill the target4";
	MissionData[53].directory	= "data\\map8\\";
	MissionData[53].txt			= "kt4";
	MissionData[53].collision	= false;
	MissionData[53].screen		= false;
	MissionData[54].name		= "UNDERGROUND_EXT3";
	MissionData[54].fullname	= "UNDERGROUND extermination3";
	MissionData[54].directory	= "data\\map5\\";
	MissionData[54].txt			= "ext3";
	MissionData[54].collision	= false;
	MissionData[54].screen		= false;
	MissionData[55].name		= "SCHOOL_EXT";
	MissionData[55].fullname	= "SCHOOL extermination";
	MissionData[55].directory	= "data\\map11\\";
	MissionData[55].txt			= "ext";
	MissionData[55].collision	= true;
	MissionData[55].screen		= false;
	MissionData[56].name		= "SCHOOL_EXT2";
	MissionData[56].fullname	= "SCHOOL extermination2";
	MissionData[56].directory	= "data\\map11\\";
	MissionData[56].txt			= "ext2";
	MissionData[56].collision	= true;
	MissionData[56].screen		= false;
	MissionData[57].name		= "SCHOOL_DE";
	MissionData[57].fullname	= "SCHOOL destroy";
	MissionData[57].directory	= "data\\map11\\";
	MissionData[57].txt			= "de";
	MissionData[57].collision	= true;
	MissionData[57].screen		= false;


	AIlevel[0].aiming = 1;
	AIlevel[0].attack = 99;
	AIlevel[0].search = 1;
	AIlevel[0].limitserror = 0;
	AIlevel[1].aiming = 1;
	AIlevel[1].attack = 18;
	AIlevel[1].search = 2;
	AIlevel[1].limitserror = 2;
	AIlevel[2].aiming = 2;
	AIlevel[2].attack = 16;
	AIlevel[2].search = 3;
	AIlevel[2].limitserror = 0;
	AIlevel[3].aiming = 3;
	AIlevel[3].attack = 14;
	AIlevel[3].search = 4;
	AIlevel[3].limitserror = -1;
	AIlevel[4].aiming = 4;
	AIlevel[4].attack = 12;
	AIlevel[4].search = 5;
	AIlevel[4].limitserror = -2;
	AIlevel[5].aiming = 5;
	AIlevel[5].attack = 10;
	AIlevel[5].search = 6;
	AIlevel[5].limitserror = -3;

	for (int i = 0; i < TOTAL_PARAMETERINFO_AILEVEL; i++) {
		if (AIlevel[i].aiming < 1) AIlevel[i].aiming = 1;

		if (AIlevel[i].attack < 1) AIlevel[i].attack = 1;
		}

	// 수정 후 코드:
	//僗僐?僾側偟, 扁夯 (조준경 없음, 기본 시야각)
	Scope[0].ViewAngle = DegreeToRadian(65.0f);			// 기존 Scope[0]
	Scope[0].MouseRange = 0.0100f;
	Scope[0].ScopeGunsight = 0;
	Scope[0].NoScopeGunsight = true;
	Scope[0].WeaponNoScopeReaction = 0;
	Scope[0].WeaponMotionReactionY = -1.0f;
	Scope[0].WeaponRecoil_ScopeRx = 0.0f;
	Scope[0].WeaponRecoil_ScopeRyMin = 0.0f;
	Scope[0].WeaponRecoil_ScopeRyMax = 0.0f;
	Scope[0].AI_ShotAngle = DegreeToRadian(8);
	Scope[0].AI_ShotAngleLong = DegreeToRadian(4);
	Scope[0].AI_AddSearchEnemyMaxDist_Normal = 0.0f;
	Scope[0].AI_AddSearchEnemyMaxDist_Caution = 0.0f;

	// 1.5배율 조준경 (레드닷)
	Scope[1].ViewAngle = DegreeToRadian(43.3f);			// 기존 Scope[3]
	Scope[1].MouseRange = 0.006667f;
	Scope[1].ScopeGunsight = 3;
	Scope[1].NoScopeGunsight = true;
	Scope[1].WeaponNoScopeReaction = 0;
	Scope[1].WeaponMotionReactionY = -1.0f;
	Scope[1].WeaponRecoil_ScopeRx = 0.4f;
	Scope[1].WeaponRecoil_ScopeRyMin = 0.2f;
	Scope[1].WeaponRecoil_ScopeRyMax = 0.3f;
	Scope[1].AI_ShotAngle = DegreeToRadian(7);
	Scope[1].AI_ShotAngleLong = DegreeToRadian(3);
	Scope[1].AI_AddSearchEnemyMaxDist_Normal = 15.0f;
	Scope[1].AI_AddSearchEnemyMaxDist_Caution = 35.0f;

	// ✨ 새로 추가된 1.5배율 조준경 (디자인 ID: 7) ✨
	Scope[2].ViewAngle = DegreeToRadian(43.3f);			// 신규 추가
	Scope[2].MouseRange = 0.006667f;
	Scope[2].ScopeGunsight = 7;					// 새로운 조준선 디자인 ID
	Scope[2].NoScopeGunsight = true;
	Scope[2].WeaponNoScopeReaction = 0;
	Scope[2].WeaponMotionReactionY = -1.0f;
	Scope[2].WeaponRecoil_ScopeRx = 0.4f;
	Scope[2].WeaponRecoil_ScopeRyMin = 0.2f;
	Scope[2].WeaponRecoil_ScopeRyMax = 0.3f;
	Scope[2].AI_ShotAngle = DegreeToRadian(7);
	Scope[2].AI_ShotAngleLong = DegreeToRadian(3);
	Scope[2].AI_AddSearchEnemyMaxDist_Normal = 15.0f;
	Scope[2].AI_AddSearchEnemyMaxDist_Caution = 35.0f;

	// 2배율 조준경
	Scope[3].ViewAngle = DegreeToRadian(32.5f);			// 기존 Scope[1]
	Scope[3].MouseRange = 0.005000f;
	Scope[3].ScopeGunsight = 1;
	Scope[3].NoScopeGunsight = true;
	Scope[3].WeaponNoScopeReaction = 0;
	Scope[3].WeaponMotionReactionY = -1.0f;
	Scope[3].WeaponRecoil_ScopeRx = 0.4f;
	Scope[3].WeaponRecoil_ScopeRyMin = 0.3f;
	Scope[3].WeaponRecoil_ScopeRyMax = 0.5f;
	Scope[3].AI_ShotAngle = DegreeToRadian(6);
	Scope[3].AI_ShotAngleLong = DegreeToRadian(3);
	Scope[3].AI_AddSearchEnemyMaxDist_Normal = 25.0f;
	Scope[3].AI_AddSearchEnemyMaxDist_Caution = 40.0f;

	// 2.5배율 조준경 (ACOG)
	Scope[4].ViewAngle = DegreeToRadian(26.0f); 			// 기존 Scope[6]
	Scope[4].MouseRange = 0.005385f;
	Scope[4].ScopeGunsight = 6;
	Scope[4].NoScopeGunsight = true;
	Scope[4].WeaponNoScopeReaction = 0;
	Scope[4].WeaponMotionReactionY = -1.0f;
	Scope[4].WeaponRecoil_ScopeOnly = false;
	Scope[4].WeaponRecoil_ScopeRx = 0.4f;
	Scope[4].WeaponRecoil_ScopeRyMin = 0.3f;
	Scope[4].WeaponRecoil_ScopeRyMax = 0.5f;
	Scope[4].AI_ShotAngle = DegreeToRadian(6);
	Scope[4].AI_ShotAngleLong = DegreeToRadian(3);
	Scope[4].AI_AddSearchEnemyMaxDist_Normal = 30.0f;
	Scope[4].AI_AddSearchEnemyMaxDist_Caution = 50.0f;

	// 3배율 조준경 (V-type 저격)
	Scope[5].ViewAngle = DegreeToRadian(21.7f);			// 기존 Scope[5]
	Scope[5].MouseRange = 0.003333f;
	Scope[5].ScopeGunsight = 5;
	Scope[5].NoScopeGunsight = true;
	Scope[5].WeaponNoScopeReaction = 0;
	Scope[5].WeaponMotionReactionY = DegreeToRadian(4.0f);
	Scope[5].WeaponRecoil_ScopeRx = 0.6f;
	Scope[5].WeaponRecoil_ScopeRyMin = 0.4f;
	Scope[5].WeaponRecoil_ScopeRyMax = 0.6f;
	Scope[5].AI_ShotAngle = DegreeToRadian(4);
	Scope[5].AI_ShotAngleLong = DegreeToRadian(2);
	Scope[5].AI_AddSearchEnemyMaxDist_Normal = 50.0f;
	Scope[5].AI_AddSearchEnemyMaxDist_Caution = 80.0f;

	// 4배율 조준경 (저격)
	Scope[6].ViewAngle = DegreeToRadian(16.25f);			// 기존 Scope[2]
	Scope[6].MouseRange = 0.002500f;
	Scope[6].ScopeGunsight = 2;
	Scope[6].NoScopeGunsight = false;
	Scope[6].WeaponNoScopeReaction = 0;
	Scope[6].WeaponMotionReactionY = DegreeToRadian(4.0f);
	Scope[6].WeaponRecoil_ScopeRx = 0.6f;
	Scope[6].WeaponRecoil_ScopeRyMin = 0.4f;
	Scope[6].WeaponRecoil_ScopeRyMax = 0.6f;
	Scope[6].AI_ShotAngle = DegreeToRadian(4);
	Scope[6].AI_ShotAngleLong = DegreeToRadian(2);
	Scope[6].AI_AddSearchEnemyMaxDist_Normal = 50.0f;
	Scope[6].AI_AddSearchEnemyMaxDist_Caution = 80.0f;

	// 8배율 조준경 (고배율 저격)
	Scope[7].ViewAngle = DegreeToRadian(8.125f);			// 기존 Scope[4]
	Scope[7].MouseRange = 0.002500f;                 //0.001250f;
	Scope[7].ScopeGunsight = 4;
	Scope[7].NoScopeGunsight = false;
	Scope[7].WeaponNoScopeReaction = 0;
	Scope[7].WeaponMotionReactionY = DegreeToRadian(4.0f);
	Scope[7].WeaponRecoil_ScopeRx = 0.6f;
	Scope[7].WeaponRecoil_ScopeRyMin = 0.4f;
	Scope[7].WeaponRecoil_ScopeRyMax = 0.6f;
	Scope[7].AI_ShotAngle = DegreeToRadian(4);
	Scope[7].AI_ShotAngleLong = DegreeToRadian(2);
	Scope[7].AI_AddSearchEnemyMaxDist_Normal = 50.0f;
	Scope[7].AI_AddSearchEnemyMaxDist_Caution = 80.0f;


	//뱳?긚긓?긵
	//Scope[4] = Scope[0];

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif
}

//! @brief 됶뺳
//! @attention ?듫릶궼렔벍밒궸뚁귂뢯궠귢귏궥궕갂뼻렑밒궸뚁귂뢯궥궞궴귖됀?궳궥갃
void ParameterInfo::DestroyInfo()
{
	//CleanupModelTextureWeapon();
	//CleanupModelTextureSmallObject();

	if( (Human == NULL)&&(Weapon == NULL)&&(SmallObject == NULL)&&(AIlevel == NULL) ){ return; }

	if( Human != NULL ){
		delete [] Human;
		Human = NULL;
	}
	if( Weapon != NULL ){
		delete [] Weapon;
		Weapon = NULL;
	}
#ifdef ENABLE_BUG_HUMANWEAPON
	if( BugWeapon != NULL ){
		delete [] BugWeapon;
		BugWeapon = NULL;
	}
#endif
	if( SmallObject != NULL ){
		delete [] SmallObject;
		SmallObject = NULL;
	}
	if( Bullet != NULL ){
		delete [] Bullet;
		Bullet = NULL;
	}
	if( MissionData != NULL ){
		delete [] MissionData;
		MissionData = NULL;
	}
	if( AIlevel != NULL ){
		delete [] AIlevel;
		AIlevel = NULL;
	}
	if( Scope != NULL ){
		delete [] Scope;
		Scope = NULL;
	}

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CLEANUP, "ParameterInfo", "");
#endif
}

//! @brief 릐궻먠믦귩롦벦
//! @param id 붥뜂
//! @param out_data 롷궚롦귡HumanParameter??귽깛?
//! @return 맟뚻갌0?렪봲갌1
int ParameterInfo::GetHuman(int id, HumanParameter *out_data)
{
	if( out_data == NULL ){ return 1; }
	if( (id < 0)||(TOTAL_PARAMETERINFO_HUMAN <= id ) ){ return 1; }

	*out_data = Human[id];
	return 0;
}

//! @brief 릐궻긡긏긚?긿긲?귽깑궻긬긚귩롦벦
//! @param id 붥뜂
//! @param *out_str 롷궚롦귡?귽깛?
//! @return 맟뚻갌0?렪봲갌1
int ParameterInfo::GetHumanTexturePath(int id, char *out_str)
{
	if( out_str == NULL ){ return 1; }
	if( (id < 0)||(TOTAL_HUMANTEXTURE <= id ) ){ return 1; }

	strcpy(out_str, HumanTexturePath[id]);
	return 0;
}

//! @brief 븧딇궻먠믦귩롦벦
//! @param id 붥뜂
//! @param out_data 롷궚롦귡WeaponParameter??귽깛?
//! @return 맟뚻갌0?렪봲갌1
int ParameterInfo::GetWeapon(int id, WeaponParameter *out_data)
{
	if( out_data == NULL ){ return 1; }
	if( (id < 0)||(TOTAL_PARAMETERINFO_WEAPON <= id ) ){
#ifdef ENABLE_BUG_HUMANWEAPON
		return GetBugWeapon(id, out_data);
#else
		return 1;
#endif
	}

	*out_data = Weapon[id];
	return 0;
}

#ifdef ENABLE_BUG_HUMANWEAPON
//! @brief 븧딇궻먠믦귩롦벦
//! @param id 붥뜂
//! @param out_data 롷궚롦귡WeaponParameter??귽깛?
//! @return 맟뚻갌0?렪봲갌1
//! @warning 먩궸 GetWeapon()듫릶 귩렳뛱궢궲롦벦궸렪봲궢궫렄궸뙽귟갂궞궻듫릶귩럊궯궲궘궬궠궋갃
//! @attention 긫긐븧딇귩믁돿궥귡뤾뜃궼갂ResourceManager긏깋긚궻 GetBugWeaponModelTexture() 듫릶귖뺂뢜궢궲궘궬궠궋갃
int ParameterInfo::GetBugWeapon(int id, WeaponParameter *out_data)
{
	if( out_data == NULL ){ return 1; }
	if( (id == 23)||(id == 24)||(id == 30)||(id == 53) ){
		*out_data = BugWeapon[0];
		return 0;
	}

	return 1;
}
#endif


//! @brief 룷븿궻먠믦귩롦벦
//! @param id 붥뜂
//! @param out_data 롷궚롦귡SmallObjectParameter??귽깛?
//! @return 맟뚻갌0?렪봲갌1
//! @warning 믁돿룷븿궻륃뺪궼롦벦궳궖귏궧귪갃믁돿룷븿궻륃뺪궼 MIFInterface긏깋긚 궔귞롦벦궢궲궘궬궠궋갃
int ParameterInfo::GetSmallObject(int id, SmallObjectParameter *out_data)
{
	if( out_data == NULL ){ return 1; }
	if( (id < 0)||(TOTAL_PARAMETERINFO_SMALLOBJECT <= id ) ){ return 1; }

	*out_data = SmallObject[id];
	return 0;
}

//! @brief 뢤뭙긆긳긙긃긏긣궻먠믦귩롦벦
//! @param id 붥뜂
//! @param out_data 롷궚롦귡BulletParameter??귽깛?
//! @return 맟뚻갌0?렪봲갌1
int ParameterInfo::GetBullet(int id, BulletParameter *out_data)
{
	if( out_data == NULL ){ return 1; }
	if( (id < 0)||(TOTAL_PARAMETERINFO_BULLET <= id ) ){ return 1; }

	*out_data = Bullet[id];
	return 0;
}

//! @brief 뷭??긞긘깈깛귩롦벦
//! @param id 붥뜂
//! @param name ?긞긘깈깛렞빶뼹귩롷궚롦귡?귽깛??걁NULL됀걂
//! @param fullname ?긞긘깈깛맫렜뼹뤝귩롷궚롦귡?귽깛??걁NULL됀걂
//! @param directory 긢??궻둰?먩귩롷궚롦귡?귽깛??걁NULL됀걂
//! @param txt 긲?귽깑뼹귩롷궚롦귡?귽깛??걁NULL됀걂
//! @param collisionflag 믁돿궻뱰궫귟뵽믦귩렑궥긲깋긐귩롷궚롦귡?귽깛??걁NULL됀걂
//! @param screenflag 됪뽋귩댠궘궥귡긲깋긐귩롷궚롦귡?귽깛??걁NULL됀걂
//! @return 맟뚻갌0?렪봲갌1
//! @note 긲?귽깑뼹갋갋갋?긞긘깈깛륃뺪긲?귽깑걁.txt걂궴?귽깛긣긢??긲?귽깑걁.pd1걂궸럊귦귢귡뼹멟
int ParameterInfo::GetOfficialMission(int id, char *name, char *fullname, char* directory, char *txt, bool *collisionflag, bool *screenflag)
{
	if( (id < 0)||(TOTAL_OFFICIALMISSION <= id ) ){ return 1; }

	if( name != NULL ){ strcpy(name, MissionData[id].name); }
	if( fullname != NULL ){ strcpy(fullname, MissionData[id].fullname); }
	if( directory != NULL ){ strcpy(directory, MissionData[id].directory); }
	if( txt != NULL ){ strcpy(txt, MissionData[id].txt); }
	if( collisionflag != NULL ){ *collisionflag = MissionData[id].collision; }
	if( screenflag != NULL ){ *screenflag = MissionData[id].screen; }
	return 0;
}

//! @brief AI깒긹깑궻먠믦걁맜?뭠걂귩롦벦
//! @param level AI깒긹깑
//! @param out_AIlevel 롷궚롦귡AIParameter??귽깛?궻?귽깛?걁2뢣?귽깛?걂
//! @return 맟뚻갌0?렪봲갌1
int ParameterInfo::GetAIlevel(int level, AIParameter **out_AIlevel)
{
	if( out_AIlevel == NULL ){ return 1; }
	if( (level < 0)||(TOTAL_PARAMETERINFO_AILEVEL <= level ) ){ return 1; }
	*out_AIlevel = &(AIlevel[level]);
	return 0;
}

//! 긚긓?긵먠믦귩롦벦
//! @param id 긚긓?긵붥뜂
//! @param out_data 롷궚롦귡ScopeParameter??귽깛?
//! @return 맟뚻갌0?렪봲갌1
//! @attention 0뼟뼖귩럚믦궢궫뤾뜃렪봲궢귏궥궕갂TOTAL_PARAMETERINFO_SCOPE댥뤵귩럚믦궢궫뤾뜃 TOTAL_PARAMETERINFO_SCOPE-1 궻먠믦뭠귩뺅궢귏궥갃
int ParameterInfo::GetScopeParam(int id, ScopeParameter *out_data)
{
	if( out_data == NULL ){ return 1; }
	if( id < 0 ){ return 1; }
	if( TOTAL_PARAMETERINFO_SCOPE <= id ){ id = TOTAL_PARAMETERINFO_SCOPE-1; }

	*out_data = Scope[id];
	return 0;
}
