//! @file ai.cpp
//! @brief AIcontrol긏깋긚궻믦?

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

#include "ai.h"

// AI 방패 복귀 기준.
// 너무 복잡한 전술 판단 없이, 현재 총이 비었거나 탄약이 거의 없는 경우에만 방패로 돌아간다.
static const bool AI_SHIELD_RETURN_ON_AMMO_SHORTAGE = true;

// 예비 탄이 없고 현재 탄창도 이 수 이하라면 방패로 복귀한다.
// 0이면 "완전히 비었을 때만" 복귀한다.
// 2 정도로 올리면 탄이 거의 떨어졌을 때 미리 방패를 든다.
static const int AI_SHIELD_RETURN_NO_RESERVE_LOADED_BULLETS = 0;

static bool IsAIShieldReturnCandidateWeapon(int weapon_id)
{
	if (weapon_id == ID_WEAPON_NONE) { return false; }
	if (weapon_id == ID_WEAPON_SHIELD) { return false; }
	if (weapon_id == ID_WEAPON_GRENADE) { return false; }
	if (weapon_id == ID_WEAPON_CASE) { return false; }

	return true;
}

static int FindAIShieldSlot(int selectweapon, class weapon* weaponlist[])
{
	for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
		if (i == selectweapon) { continue; }
		if (weaponlist[i] == NULL) { continue; }

		int weapon_id = ID_WEAPON_NONE;
		weaponlist[i]->GetParamData(&weapon_id, NULL, NULL);

		if (weapon_id == ID_WEAPON_SHIELD) {
			return i;
		}
	}

	return -1;
}

static bool ShouldAIReturnToShieldForAmmoShortage(int weapon_id, int loaded_bullets, int total_bullets)
{
	if (AI_SHIELD_RETURN_ON_AMMO_SHORTAGE == false) { return false; }
	if (IsAIShieldReturnCandidateWeapon(weapon_id) == false) { return false; }

	// 탄창이 비었으면 원래 재장전/무기전환 판단으로 들어가므로,
	// 그 전에 방패가 있으면 방패로 복귀시킨다.
	if (loaded_bullets <= 0) {
		return true;
	}

	// 예비 탄이 없고 현재 탄창도 거의 비었으면 방패로 복귀한다.
	if ((total_bullets <= 0) &&
		(loaded_bullets <= AI_SHIELD_RETURN_NO_RESERVE_LOADED_BULLETS)) {
		return true;
	}

	return false;
}

//! @brief 긓깛긚긣깋긏?
AIcontrol::AIcontrol(class ObjectManager *in_ObjMgr, int in_ctrlid, class BlockDataInterface *in_blocks, class PointDataInterface *in_Points, class ParameterInfo *in_Param, class Collision *in_CollD, class SoundManager *in_GameSound)
{
	ObjMgr = in_ObjMgr;
	ctrlid = in_ctrlid;
	if( in_ObjMgr != NULL ){ ctrlhuman = in_ObjMgr->GetHumanObject(in_ctrlid); }
	else{ ctrlhuman = NULL; }
	enemyhuman = NULL;
	blocks = in_blocks;
	Param = in_Param;
	CollD = in_CollD;
	GameSound = in_GameSound;

	AIlevel = 0;
	battlemode = AI_NORMAL;
	NoFight = false;
	ForceNoAI = false;
	posx = 0.0f;
	posy = 0.0f;
	posz = 0.0f;
	rx = 0.0f;
	ry = 0.0f;
	cautionback_posx = 0.0f;
	cautionback_posz = 0.0f;
	FaceCaution_flag = false;
	FaceCaution_rx = 0.0f;
	total_move = 0.0f;
	waitcnt = 0;
	movejumpcnt = 1*((int)GAMEFPS);
	gotocnt = 0;
	cautioncnt = 0;
	actioncnt = 0;
	longattack = false;
	EventWeaponShot = false;
	crouchcnt = 0;
	LevelParam = NULL;

	MoveNavi = new AIMoveNavi;
	ObjDriver = new AIObjectDriver;
	MoveNavi->SetClass(in_ObjMgr, in_ctrlid, in_Points);
	ObjDriver->SetClass(in_ObjMgr, in_ctrlid);
}

//! @brief 긢귻긚긣깋긏?
AIcontrol::~AIcontrol()
{
	delete MoveNavi;
	delete ObjDriver;
}

//! @brief 뫮뤭긏깋긚귩먠믦
//! @attention 궞궻듫릶궳먠믦귩뛱귦궶궋궴갂긏깋긚렔뫬궕맫궢궘??궢귏궧귪갃
void AIcontrol::SetClass(class ObjectManager *in_ObjMgr, int in_ctrlid, class BlockDataInterface *in_blocks, class PointDataInterface *in_Points, class ParameterInfo *in_Param, class Collision *in_CollD, class SoundManager *in_GameSound)
{
	ObjMgr = in_ObjMgr;
	ctrlid = in_ctrlid;
	ctrlhuman = in_ObjMgr->GetHumanObject(in_ctrlid);
	blocks = in_blocks;
	Param = in_Param;
	CollD = in_CollD;
	GameSound = in_GameSound;

	MoveNavi->SetClass(in_ObjMgr, in_ctrlid, in_Points);
	ObjDriver->SetClass(in_ObjMgr, in_ctrlid);
}

//! @brief 뽞뷭뭤?궸댷벍궢궲궋귡궔둴봃
//! @return 뱸묪갌true?뷄뱸묪갌false
bool AIcontrol::CheckTargetPos(bool back)
{
	float target_posx, target_posz;
	int movemode;
	if( back == false ){
		MoveNavi->GetTargetPos(&target_posx, &target_posz, NULL, &movemode, NULL);
	}
	else{
		target_posx = cautionback_posx;
		target_posz = cautionback_posz;
		movemode = AI_NAVI_MOVE_RUN;
	}

	//떁뿣귩럁뢯
	float x = posx - target_posx;
	float z = posz - target_posz;
	float r = x * x + z * z;

	if( movemode == AI_NAVI_MOVE_TRACKING ){	//믁뷉뭷궶귞
		if( r < AI_ARRIVALDIST_TRACKING * AI_ARRIVALDIST_TRACKING ){
			return true;
		}
	}
	else{							//궩귢댥둖궶귞
		if( r < (AI_ARRIVALDIST_PATH * AI_ARRIVALDIST_PATH) ){
			return true;
		}
	}

	return false;
}

//! @brief 뽞뷭뭤?궸댷벍
void AIcontrol::MoveTarget(bool back)
{
	float r, atan;
	int paramid;
	HumanParameter Paraminfo;
	bool zombie;

	float target_posx, target_posz;	
	int movemode;
	if( back == false ){
		MoveNavi->GetTargetPos(&target_posx, &target_posz, NULL, &movemode, NULL);
	}
	else{
		target_posx = cautionback_posx;
		target_posz = cautionback_posz;
		movemode = AI_NAVI_MOVE_RUN;
	}

	//?깛긮궔궵궎궔뵽믦
	ctrlhuman->GetParamData(&paramid, NULL, NULL, NULL);
	Param->GetHuman(paramid, &Paraminfo);
	if( Paraminfo.type == 2 ){
		zombie = true;
	}
	else{
		zombie = false;
	}

	//덇뱗멣궲궻벍궖귩?귕귡
	ObjDriver->ResetMode();

	//뽞뷭뭤?귉궻둷뱗귩땫귕귡
	CheckTargetAngle(posx, 0.0f, posz, rx*-1 + (float)M_PI/2, 0.0f, target_posx, 0.0f, target_posz, 0.0f, &atan, NULL, &r);

	//먿됷
	if( atan > DegreeToRadian(0.5f) ){
		ObjDriver->SetModeFlag(AI_CTRL_TURNLEFT);
	}
	if( atan < DegreeToRadian(-0.5f) ){
		ObjDriver->SetModeFlag(AI_CTRL_TURNRIGHT);
	}

	//멟릋궥귡
	if( zombie == true ){
		if( fabsf(atan) < DegreeToRadian(20) ){
			ObjDriver->SetModeFlag(AI_CTRL_MOVEWALK);
		}
	}
	else if( movemode == AI_NAVI_MOVE_RUN ){
		if( fabsf(atan) < DegreeToRadian(50) ){
			ObjDriver->SetModeFlag(AI_CTRL_MOVEFORWARD);
		}
	}
	else if( movemode == AI_NAVI_MOVE_RUN2 ){
		if( fabsf(atan) < DegreeToRadian(50) ){
			ObjDriver->SetModeFlag(AI_CTRL_MOVEFORWARD);
		}
	}
	else if( movemode == AI_NAVI_MOVE_WALK ){
		if( fabsf(atan) < DegreeToRadian(6) ){
			ObjDriver->SetModeFlag(AI_CTRL_MOVEWALK);
		}
	}
	else if( movemode == AI_NAVI_MOVE_TRACKING ){
		if( fabsf(atan) < DegreeToRadian(20) ){
			if( r < (AI_ARRIVALDIST_WALKTRACKING * AI_ARRIVALDIST_WALKTRACKING) ){
				ObjDriver->SetModeFlag(AI_CTRL_MOVEWALK);
			}
			else{
				ObjDriver->SetModeFlag(AI_CTRL_MOVEFORWARD);
			}
		}
	}

	//긙긿깛긵
	if( GetRand(16) == 0 ){
		MoveJump();
	}

	//덙궯?궔궯궲궋궫귞갂뜺덭귉궻됷?귩깋깛??궸뛱궎
	if( GetRand(28) == 0 ){
		if( ctrlhuman->GetMovemode(true) != 0 ){
			if( ctrlhuman->GetTotalMove() - total_move < 0.1f ){
				if( GetRand(2) == 0 ){ ObjDriver->SetModeFlag(AI_CTRL_TURNRIGHT); }
				else{ ObjDriver->SetModeFlag(AI_CTRL_TURNLEFT); }
			}
		}
	}
	total_move = ctrlhuman->GetTotalMove();
}

//! @brief 뽞뷭뭤?궸댷벍걁뾆먩밒궶몪귟뾭걂
void AIcontrol::MoveTarget2(bool back)
{
	float atan;

	float target_posx, target_posz;	
	if( back == false ){
		MoveNavi->GetTargetPos(&target_posx, &target_posz, NULL, NULL, NULL);
	}
	else{
		target_posx = cautionback_posx;
		target_posz = cautionback_posz;
	}

	//뽞뷭뭤?귉궻둷뱗귩땫귕귡
	CheckTargetAngle(posx, 0.0f, posz, rx*-1 + (float)M_PI/2, 0.0f, target_posx, 0.0f, target_posz, 0.0f, &atan, NULL, NULL);

	//멟뚣댷벍궻룉뿚
	if( fabsf(atan) < DegreeToRadian(56) ){
		ObjDriver->SetModeFlag(AI_CTRL_MOVEFORWARD);
	}
	if( fabsf(atan) > DegreeToRadian(123.5f) ){
		ObjDriver->SetModeFlag(AI_CTRL_MOVEBACKWARD);
	}

	//뜺덭댷벍궻룉뿚
	if( (DegreeToRadian(-146) < atan)&&(atan < DegreeToRadian(-33)) ){
		ObjDriver->SetModeFlag(AI_CTRL_MOVERIGHT);
	}
	if( (DegreeToRadian(33) < atan)&&(atan < DegreeToRadian(146)) ){
		ObjDriver->SetModeFlag(AI_CTRL_MOVELEFT);
	}

	//긙긿깛긵
	if( GetRand(16) == 0 ){
		MoveJump();
	}

	//덙궯?궔궯궲궋궫귞갂뜺덭귉궻됷?귩깋깛??궸뛱궎
	if( GetRand(28) == 0 ){
		if( ctrlhuman->GetMovemode(true) != 0 ){
			if( ctrlhuman->GetTotalMove() - total_move < 0.1f ){
				if( GetRand(2) == 0 ){ ObjDriver->SetModeFlag(AI_CTRL_TURNRIGHT); }
				else{ ObjDriver->SetModeFlag(AI_CTRL_TURNLEFT); }
			}
		}
	}
	total_move = ctrlhuman->GetTotalMove();
}

//! @brief 멟뚣뜺덭깋깛??궸댷벍걁뛘똼뭷뾭걂
void AIcontrol::MoveRandom()
{
	int forwardstart, backstart, sidestart;

	if( longattack == false ){
		forwardstart = 80;
		backstart = 90;
		sidestart = 70;
	}
	else{
		forwardstart = 120;
		backstart = 150;
		sidestart = 130;
	}

	//깋깛??궸댷벍귩럑귕귡
	if( GetRand(forwardstart) == 0 ){
		ObjDriver->SetModeFlag(AI_CTRL_MOVEFORWARD);
	}
	if( GetRand(backstart) == 0 ){
		ObjDriver->SetModeFlag(AI_CTRL_MOVEBACKWARD);
	}
	if( GetRand(sidestart) == 0 ){
		ObjDriver->SetModeFlag(AI_CTRL_MOVELEFT);
	}
	if( GetRand(sidestart) == 0 ){
		ObjDriver->SetModeFlag(AI_CTRL_MOVERIGHT);
	}

	//븧딇귩렃궯궲궓귞궦갂롨귆귞궶귞궽
	if( ctrlhuman->GetMainWeaponTypeNO() == ID_WEAPON_NONE ){
		// 1/80궻둴뿦궳돷궕귟럑귕귡
		if( GetRand(80) == 0 ){
			ObjDriver->SetModeFlag(AI_CTRL_MOVEBACKWARD);
		}
	}

	// 1/3궻둴뿦궔갂댷벍긲깋긐궕먠믦궠귢궲궋궫귞
	if( (GetRand(3) == 0)||(ObjDriver->GetModeFlag(AI_CTRL_MOVEFORWARD | AI_CTRL_MOVEBACKWARD | AI_CTRL_MOVELEFT | AI_CTRL_MOVERIGHT)) ){
		float vx, vz;
		float Dist;

		if( GetRand(2) == 0 ){
			//멟뺴뛀궻긹긏긣깑귩똶럁
			vx = cosf(rx*-1 + (float)M_PI/2);
			vz = sinf(rx*-1 + (float)M_PI/2);
			if(
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy + HUMAN_MAPCOLLISION_HEIGHT, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == true)||		//뜕궻뛼궠궸긳깓긞긏궕궇귡걁귆궰궔귡걂
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy - 1.0f, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == false)							//뫉뙰궸긳깓긞긏궕궶궋걁뿇궭귡걂
			){
				//멟릋긲깋긐귩랁룣궢갂뚣뫿긲깋긐귩먠믦
				ObjDriver->DelModeFlag(AI_CTRL_MOVEFORWARD);
				ObjDriver->SetModeFlag(AI_CTRL_MOVEBACKWARD);
			}

			//뚣뺴뛀궻긹긏긣깑귩똶럁
			vx = cosf(rx*-1 + (float)M_PI/2 + (float)M_PI);
			vz = sinf(rx*-1 + (float)M_PI/2 + (float)M_PI);
			if(
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy + HUMAN_MAPCOLLISION_HEIGHT, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == true)||		//뜕궻뛼궠궸긳깓긞긏궕궇귡걁귆궰궔귡걂
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy - 1.0f, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == false)							//뫉뙰궸긳깓긞긏궕궶궋걁뿇궭귡걂
			){
				//뚣뫿긲깋긐귩랁룣궢갂멟릋긲깋긐귩먠믦
				ObjDriver->DelModeFlag(AI_CTRL_MOVEBACKWARD);
				ObjDriver->SetModeFlag(AI_CTRL_MOVEFORWARD);
			}
		}
		else{
			//
			vx = cosf(rx*-1);
			vz = sinf(rx*-1);
			if(
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy + HUMAN_MAPCOLLISION_HEIGHT, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == true)||		//뜕궻뛼궠궸긳깓긞긏궕궇귡걁귆궰궔귡걂
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy - 1.0f, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == false)							//뫉뙰궸긳깓긞긏궕궶궋걁뿇궭귡걂
			){
				//덭댷벍긲깋긐귩랁룣궢갂뜺댷벍긲깋긐귩먠믦
				ObjDriver->DelModeFlag(AI_CTRL_MOVERIGHT);
				ObjDriver->SetModeFlag(AI_CTRL_MOVELEFT);
			}

			vx = cosf(rx*-1 + (float)M_PI);
			vz = sinf(rx*-1 + (float)M_PI);
			if(
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy + HUMAN_MAPCOLLISION_HEIGHT, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == true)||		//뜕궻뛼궠궸긳깓긞긏궕궇귡걁귆궰궔귡걂
				(CollD->CheckALLBlockIntersectDummyRay(posx, posy - 1.0f, posz, vx, 0, vz, NULL, NULL, &Dist, HUMAN_MAPCOLLISION_R) == false)							//뫉뙰궸긳깓긞긏궕궶궋걁뿇궭귡걂
			){
				//뜺댷벍긲깋긐귩랁룣궢갂덭댷벍긲깋긐귩먠믦
				ObjDriver->DelModeFlag(AI_CTRL_MOVELEFT);
				ObjDriver->SetModeFlag(AI_CTRL_MOVERIGHT);
			}
		}
	}

	//뛘똼뫮뤭궕궋귡궶귞
	if( enemyhuman != NULL ){
		float tx, ty, tz;

		enemyhuman->GetPosData(&tx, &ty, &tz, NULL);

		float x = posx - tx;
		float y = posy - ty;
		float z = posz - tz;
		float r = x * x + y * y + z * z;

		//밎궸뗟궱궖궥궗궫궶귞뚣뫿궥귡
		if( r < 20.0f * 20.0f ){
			ObjDriver->DelModeFlag(AI_CTRL_MOVEFORWARD);
			if( GetRand(70) == 0 ){
				ObjDriver->SetModeFlag(AI_CTRL_MOVEBACKWARD);
			}
		}
	}
}

//! @brief 궩궻뤾귩뙥귏귦궥
void AIcontrol::TurnSeen()
{
	int turnstart, turnstop;

	float target_rx;
	int pointmode;
	MoveNavi->GetTargetPos(NULL, NULL, &target_rx, NULL, &pointmode);

	//똼궫귢궲똸둀궢궫궶귞갂똼궫귢궫뺴뛀귩뛀궘
	if( (battlemode == AI_CAUTION)&&(FaceCaution_flag == true) ){
		float tr;

		//뺴뛀귩똶럁
		tr = FaceCaution_rx - rx;
		tr = AngleNormalization(tr);

		//먿됷
		if( tr > DegreeToRadian(2.5f) ){
			ObjDriver->SetModeFlag(AI_CTRL_TURNRIGHT);
		}
		if( tr < DegreeToRadian(-2.5f) ){
			ObjDriver->SetModeFlag(AI_CTRL_TURNLEFT);
		}

		//벫믦뺴뛀궸뛀궖뢎귦귢궽갂븕믅궻똸둀궸댷뛱궥귡
		if( fabsf(tr) <= DegreeToRadian(2.5f) ){
			FaceCaution_flag = false;
		}

		return;
	}

	//됷?궻둎럑갋뢎뿹둴뿦귩먠믦
	if( battlemode == AI_ACTION ){
		return;
	}
	else if( battlemode == AI_CAUTION ){
		turnstart = 20;
		turnstop = 20;
	}
	else{
		if( pointmode == AI_NAVI_POINT_TRACKING ){ turnstart = 65; }
		else{ turnstart = 85; }
		turnstop = 18;
	}

	//깋깛??궸됷?귩럑귕귡
	if( GetRand(turnstart) == 0 ){
		ObjDriver->SetModeFlag(AI_CTRL_TURNRIGHT);
	}
	if( GetRand(turnstart) == 0 ){
		ObjDriver->SetModeFlag(AI_CTRL_TURNLEFT);
	}

	if( (battlemode == AI_NORMAL)&&(pointmode == AI_NAVI_POINT_WAIT) ){
		//깋깛??궸?귽깛긣궻뺴귩뛀궞궎궴궥귡
		//걏?귽깛긣궻뺴뛀귩룺궢뢣럨궥귡걐궻띋뙸 
		if( GetRand(80) == 0 ){
			float tr;
			tr = target_rx - rx;
			tr = AngleNormalization(tr);

			if( tr > 0.0f ){
				ObjDriver->SetModeFlag(AI_CTRL_TURNRIGHT);
			}
			if( tr < 0.0f ){
				ObjDriver->SetModeFlag(AI_CTRL_TURNLEFT);
			}
		}
	}

	//됷?귩깋깛??궸?귕귡
	if( GetRand(turnstop) == 0 ){
		ObjDriver->DelModeFlag(AI_CTRL_TURNRIGHT);
	}
	if( GetRand(turnstop) == 0 ){
		ObjDriver->DelModeFlag(AI_CTRL_TURNLEFT);
	}
}

//! @brief 벫믦궻뺴뛀귩뙥뫏궚귡
bool AIcontrol::StopSeen()
{
	float tr;
	bool returnflag = false;

	float target_rx;
	MoveNavi->GetTargetPos(NULL, NULL, &target_rx, NULL, NULL);

	tr = target_rx - rx;
	tr = AngleNormalization(tr);

	ObjDriver->DelModeFlag(AI_CTRL_TURNRIGHT);
	ObjDriver->DelModeFlag(AI_CTRL_TURNLEFT);

	//먿됷
	if( tr > DegreeToRadian(2.5f) ){
		ObjDriver->SetModeFlag(AI_CTRL_TURNRIGHT);
		returnflag = false;
	}
	if( tr < DegreeToRadian(-2.5f) ){
		ObjDriver->SetModeFlag(AI_CTRL_TURNLEFT);
		returnflag = false;
	}

	//벫믦뺴뛀궸뛀궚궲궋귡궔뵽믦
	if( fabsf(tr) <= DegreeToRadian(2.5f) ){
		returnflag = true;
	}

	return returnflag;
}

//! @brief 릋뛱뺴뛀궸뤳둙븿궕궇귢궽긙긿깛긵궥귡
bool AIcontrol::MoveJump()
{
	//뿧궭?귏궯궲궋귢궽룉뿚궢궶궋
	//?겍AIObjectDriver(ObjDriver)궻댷벍긲깋긐궳?긃긞긏궢궲귖쀇궋갃
	if( ctrlhuman->GetMovemode(false) == 0 ){ return false; }

	float dist_dummy = 0.0f;

	float check_rx = 0.0f;
	float new_posx, new_posy, new_posz;

	//릋뛱뺴뛀귩럁뢯
	if( ObjDriver->GetModeFlag(AI_CTRL_MOVEWALK) == true ){
		check_rx = DegreeToRadian(0.0f);
	}
	if( ObjDriver->GetModeFlag(AI_CTRL_MOVEFORWARD) == true ){
		check_rx = DegreeToRadian(0.0f);
	}
	if( ObjDriver->GetModeFlag(AI_CTRL_MOVEBACKWARD) == true ){
		check_rx = DegreeToRadian(180.0f);
	}
	if( ObjDriver->GetModeFlag(AI_CTRL_MOVELEFT) == true ){
		check_rx = DegreeToRadian(90.0f);
	}
	if( ObjDriver->GetModeFlag(AI_CTRL_MOVERIGHT) == true ){
		check_rx = DegreeToRadian(-90.0f);
	}

	//뜕븊뗟궻뱰궫귟뵽믦
	new_posx = posx + cosf(rx*-1 + (float)M_PI/2 + check_rx) * (AI_CHECKJUMP_DIST + HUMAN_MAPCOLLISION_R);
	new_posy = posy + HUMAN_MAPCOLLISION_HEIGHT;
	new_posz = posz + sinf(rx*-1 + (float)M_PI/2 + check_rx) * (AI_CHECKJUMP_DIST + HUMAN_MAPCOLLISION_R);
	if( CollD->CheckALLBlockInside(new_posx, new_posy, new_posz) == true ){
		ObjMgr->MoveJump(ctrlid);
		return true;
	}

	//뫬멣뫬궻뱰궫귟뵽믦
	new_posx = posx + cosf(rx*-1 + (float)M_PI/2 + check_rx) * AI_CHECKJUMP_DIST;
	new_posy = posy + AI_CHECKJUMP_HEIGHT;
	new_posz = posz + sinf(rx*-1 + (float)M_PI/2 + check_rx) * AI_CHECKJUMP_DIST;
	if( CollD->CheckALLBlockInside(new_posx, new_posy, new_posz) == true ){
		ObjMgr->MoveJump(ctrlid);
		return true;
	}
	else if( CollD->CheckALLBlockIntersectRay(new_posx, new_posy, new_posz, 0.0f, 1.0f, 0.0f, NULL, NULL, &dist_dummy, HUMAN_HEIGHT - AI_CHECKJUMP_HEIGHT) == true ){
		ObjMgr->MoveJump(ctrlid);
		return true;
	}

	return false;
}

//! @brief 뛘똼
//! @todo ?깛긮궻몜롨귩뺕귏궑귡룉뿚
void AIcontrol::Action()
{
	human* EnemyHuman = NULL;
	int paramid;
	HumanParameter Paraminfo;
	bool zombie;
	float posy2;
	float tx, ty, tz;

	EnemyHuman = enemyhuman;

	//띆뷭귩롦벦
	EnemyHuman->GetPosData(&tx, &ty, &tz, NULL);
	posy2 = posy + VIEW_HEIGHT;
	ty += VIEW_HEIGHT;

	//?깛긮궔궵궎궔뵽믦
	ctrlhuman->GetParamData(&paramid, NULL, NULL, NULL);
	Param->GetHuman(paramid, &Paraminfo);
	if( Paraminfo.type == 2 ){
		zombie = true;
	}
	else{
		zombie = false;
	}

	//룋렃궢궲궋귡븧딇궻롰쀞귩롦벦
	int weaponid = ctrlhuman->GetMainWeaponTypeNO();

	float atanx, atany, r;

	//??긒긞긣궻댧뭫귩뺚맫
	if( zombie == true ){		//?깛긮궻뤾뜃
		float x = posx - tx;
		float z = posz - tz;
		float r = sqrtf(x * x + z * z);
		float scale;
		float mx, mz;

		if( r > 180.0f ){ r = 180.0f; }		//귖궢궔궢궫귞 1800.0f 뤵뙽궔귖

		//믁?맜궻긬깋긽???귩롦벦
		switch( Paraminfo.AItrackability ){
			case 0: scale = (0.05f * 2); break;
			case 1: scale = (0.10f * 2); break;
			case 2: scale = (0.13f * 2); break;
			case 3: scale = (0.17f * 2); break;
			default: scale = 0.0f;
		}

		//밎궻댷벍귩뙥뮪궥
		EnemyHuman->GetMovePos(&mx, NULL, &mz);
		tx += mx * r * scale;
		tz += mz * r * scale;
	}
	else{		//?깛긮궳궶궋뤾뜃
		//렔빁궕롨왮뭙귩렃궯궲궋귢궽?
		if( weaponid == ID_WEAPON_GRENADE ){
			if( zombie == false ){
				float x = posx - tx;
				float z = posz - tz;
				float r = sqrtf(x * x + z * z);
				float scale;
				if( longattack == false ){ scale = 0.12f; }
				else{ scale = 0.4f; }

				//떁뿣궸돒궣궲뛼궠귩빾궑귡
				ty += (r - AI_CHECKSHORTATTACK_DIST) * scale;
			}
		}
		else{
			float mx, mz;
			float scale;
			EnemyHuman->GetMovePos(&mx, NULL, &mz);
			if( longattack == false ){ scale = 1.5f; }
			else{
				float x = posx - tx;
				float z = posz - tz;
				float r = sqrtf(x * x + z * z);

				scale = r * 0.12f;
			}

			//밎궻댷벍귩뙥뮪궥
			tx += mx * scale;
			tz += mz * scale;
		}
	}

	//뽞뷭뭤?귉궻둷뱗귩땫귕귡
	CheckTargetAngle(posx, posy2, posz, rx*-1 + (float)M_PI/2, ry, tx, ty, tz, 0.0f, &atanx, &atany, &r);

	//뛀궖귩빾궑귡?귽?깛긐궔뙂믦
	int randr = LevelParam->aiming;
	if( longattack == false ){ randr += 1; }
	else{ randr += 2; }

	if( randr != 0 ){
		//먿됷
		// 좌우 회전: 목표 근처에서는 회전 플래그를 끄고 떨림 방지
		float deadzone_x = DegreeToRadian(0.8f);

		if (atanx > deadzone_x) {
			ObjDriver->SetModeFlag(AI_CTRL_TURNLEFT);
			ObjDriver->DelModeFlag(AI_CTRL_TURNRIGHT);
		}
		else if (atanx < -deadzone_x) {
			ObjDriver->SetModeFlag(AI_CTRL_TURNRIGHT);
			ObjDriver->DelModeFlag(AI_CTRL_TURNLEFT);
		}
		else {
			ObjDriver->DelModeFlag(AI_CTRL_TURNLEFT);
			ObjDriver->DelModeFlag(AI_CTRL_TURNRIGHT);
		}

		//쁱궻둷뱗
		if( zombie == true ){
			//ry = AI_ZOMBIEATTACK_ARMRY;

			//먿됷
			if( ry < AI_ZOMBIEATTACK_ARMRY ){
				ObjDriver->SetModeFlag(AI_CTRL_TURNUP);
				ObjDriver->DelModeFlag(AI_CTRL_TURNDOWN);
			}
			if( ry > AI_ZOMBIEATTACK_ARMRY ){
				ObjDriver->SetModeFlag(AI_CTRL_TURNDOWN);
				ObjDriver->DelModeFlag(AI_CTRL_TURNUP);
			}
		}
		else{
			//렔빁궕롨귆귞궶귞궽?
			if( weaponid == ID_WEAPON_NONE ){
				if( EnemyHuman->GetMainWeaponTypeNO() == ID_WEAPON_NONE ){	//밎귖롨귆귞궶귞궽?
					//돷궸뛀궚뫏궚귡
					ObjDriver->SetModeFlag(AI_CTRL_TURNDOWN);
					ObjDriver->DelModeFlag(AI_CTRL_TURNUP);
				}
				else{														//밎궕븧딇귩렃궯궲궋귢궽?
					//뤵궸뛀궚뫏궚귡
					ObjDriver->SetModeFlag(AI_CTRL_TURNUP);
					ObjDriver->DelModeFlag(AI_CTRL_TURNDOWN);
				}
			}
			else{
				//먿됷
				// 상하 회전: 목표 근처에서는 회전 플래그를 끄고 떨림 방지
				float deadzone_y = DegreeToRadian(0.8f);

				if (atany > deadzone_y) {
					ObjDriver->SetModeFlag(AI_CTRL_TURNUP);
					ObjDriver->DelModeFlag(AI_CTRL_TURNDOWN);
				}
				else if (atany < -deadzone_y) {
					ObjDriver->SetModeFlag(AI_CTRL_TURNDOWN);
					ObjDriver->DelModeFlag(AI_CTRL_TURNUP);
				}
				else {
					ObjDriver->DelModeFlag(AI_CTRL_TURNUP);
					ObjDriver->DelModeFlag(AI_CTRL_TURNDOWN);
				}
			}
		}
	}

	//?깛긮댥둖궳롨귆귞궶귞궽
	if( zombie == false ){
		if( weaponid == ID_WEAPON_NONE ){
			//덇믦궻둴뿦궳뚣뫿궥귡
			if( GetRand(80) == 0 ){
				ObjDriver->SetModeFlag(AI_CTRL_MOVEBACKWARD);
			}
		}
	}

	
	if (zombie == true) {	//?깛긮궻뛘똼
		float y = posy2 - ty;

		bool summoned_ally = false;
		if (ctrlhuman != NULL) {
			summoned_ally = ctrlhuman->GetSkillSummonedAllyFlag();
		}

		//귖궢몪궯궎귢궽갂덇뱗뺖궖궸먛귟뫶궑귡
		if (ObjDriver->GetModeFlag(AI_CTRL_MOVEFORWARD)) {
			ObjDriver->DelModeFlag(AI_CTRL_MOVEFORWARD);
			ObjDriver->SetModeFlag(AI_CTRL_MOVEWALK);
		}

		// 소환 동료는 AI FireRate 기반 좀비 돌진을 사용하지 않는다.
		// 대신 걷기 이동 + 고정 SpeedMultiplier 2.0f로만 적을 추적한다.
		if (summoned_ally == true) {
			if (fabsf(atanx) <= DegreeToRadian(25)) {
				ObjDriver->SetModeFlag(AI_CTRL_MOVEWALK);
				ObjDriver->DelModeFlag(AI_CTRL_MOVEFORWARD);
			}

			ctrlhuman->SpeedMultiplier = 2.0f;
		}
		// 적을 향해 전진한다.
		// AI FireRate가 높을수록 좀비의 돌진 빈도, 돌진 거리, 돌진 속도를 올린다.
		else if (fabsf(atanx) <= DegreeToRadian(25)) {
			bool zombie_run = false;

			float firerate_mult = GameConfig.GetAIFireRateMultiplier();

			if (firerate_mult < 1.0f) { firerate_mult = 1.0f; }
			if (firerate_mult > 4.0f) { firerate_mult = 4.0f; }

			// FireRate 1.0 -> 0.0
			// FireRate 4.0 -> 1.0
			float zombie_bonus = (firerate_mult - 1.0f) / 3.0f;

			// 기존 돌진 시작 거리: 24
			// FireRate 최대 시: 36
			// 너무 크게 잡으면 좀비가 맵 전체에서 달려드는 느낌이 나므로 36 정도가 안정적이다.
			float zombie_run_dist = 24.0f + 96.0f * zombie_bonus;

			// 기존보다 FireRate가 높을수록 더 자주 달린다.
			// 기존 조건은 actioncnt % 50 > 20 이므로 대략 29/50 프레임 정도 달림.
			// 최대 FireRate에서는 조건 만족 시 항상 달림.
			float zombie_run_angle = 15.0f + 10.0f * zombie_bonus;

			if ((fabsf(atanx) <= DegreeToRadian(zombie_run_angle)) && (r < zombie_run_dist * zombie_run_dist)) {
				if (zombie_bonus >= 1.0f) {
					zombie_run = true;
				}
				else {
					int run_start = 20 - (int)(20.0f * zombie_bonus);

					if (run_start < 0) { run_start = 0; }
					if (run_start > 20) { run_start = 20; }

					if (actioncnt % 50 > run_start) {
						zombie_run = true;
					}
				}
			}

			if (zombie_run == true) {
				// 달리기 입력
				ObjDriver->SetModeFlag(AI_CTRL_MOVEFORWARD);
				ObjDriver->DelModeFlag(AI_CTRL_MOVEWALK);

				// 돌진 중에만 이동 속도 상승.
				// FireRate 최대 시 1.15배.
				ctrlhuman->SpeedMultiplier = 1.0f + 0.3f * zombie_bonus;
			}
			else {
				// 평소에는 기존처럼 걷기
				ObjDriver->SetModeFlag(AI_CTRL_MOVEWALK);

				ctrlhuman->SpeedMultiplier = 1.0f;
			}
		}

		/*
		//긙긿깛긵
		if( GetRand(16) == 0 ){
			MoveJump();
		}
		*/

		if( (r < 9.0f*9.0f)&&( fabsf(y) < 10.0f) ){
			float x = posx - tx;
			float z = posz - tz;

			//뺕귏궑귡?걁밎귩덙궖븊궚귡걂
			float firerate_mult = GameConfig.GetAIFireRateMultiplier();

			if (firerate_mult < 1.0f) { firerate_mult = 1.0f; }
			if (firerate_mult > 4.0f) { firerate_mult = 4.0f; }

			float zombie_bonus = (firerate_mult - 1.0f) / 3.0f;

			float zombie_pull = 0.5f + 0.2f * zombie_bonus;

			// 방패를 든 대상이 좀비를 정면으로 막고 있으면,
			// 좀비의 끌어당김이 방패 밀어내기와 충돌하지 않도록 당기지 않는다.
			// 측면/후면에서는 기존처럼 끌어당김이 적용된다.
			if (ObjMgr->IsZombieAttackBlockedByShield(ctrlhuman, EnemyHuman, false) == false) {
				EnemyHuman->AddPosOrder(atan2f(z, x), 0.0f, zombie_pull);
			}

			//밎궻럨?귩깋깛??궸벍궔궥
			float erx, ery;
			EnemyHuman->GetRxRy(&erx, &ery);
			switch(GetRand(3)){
				case 0: erx -= DegreeToRadian(2); break;
				case 1: erx += DegreeToRadian(2); break;
				default: break;
			}
			switch(GetRand(3)){
				case 0: ery -= DegreeToRadian(2); break;
				case 1: ery += DegreeToRadian(2); break;
				default: break;
			}
			EnemyHuman->SetRxRy(erx, ery);
		}

		float firerate_mult = GameConfig.GetAIFireRateMultiplier();

		if (firerate_mult < 1.0f) { firerate_mult = 1.0f; }
		if (firerate_mult > 4.0f) { firerate_mult = 4.0f; }

		float zombie_bonus = (firerate_mult - 1.0f) / 3.0f;

		// 기존 50프레임 → 최대 FireRate에서 35프레임
		int zombie_attack_interval = 50 - (int)(15.0f * zombie_bonus);

		if (zombie_attack_interval < 35) { zombie_attack_interval = 35; }

		if (actioncnt % zombie_attack_interval == 0) {
			if (ObjMgr->CheckZombieAttack(ctrlhuman, EnemyHuman) == true) {
				ObjMgr->HitZombieAttack(ctrlhuman, EnemyHuman);
			}
		}
	}
	else {					//뵯뺺궥귡
		int scopemode = 0;
		ScopeParameter sparam;
		float ShotAngle;

		//븧딇궻긚긓?긵먠믦귩롦벦
		if (weaponid != ID_WEAPON_NONE) {
			WeaponParameter wparam;
			Param->GetWeapon(weaponid, &wparam);
			scopemode = wparam.scopemode;
		}

		if (longattack == false) {
			Param->GetScopeParam(scopemode, &sparam);
			ShotAngle = sparam.AI_ShotAngle;

			// AI 레벨별 조준 오차 보정
			ShotAngle += DegreeToRadian(0.5f) * LevelParam->limitserror;

			float accuracy_mult = GameConfig.GetAIAccuracyMultiplier();

			if (accuracy_mult <= 0.0f) {
				// Accuracy 0.0이면 거의 맞추지 못하게 조준 오차를 크게 만든다.
				ShotAngle = DegreeToRadian(90.0f);
			}
			else {
				if (accuracy_mult > 3.0f) {
					accuracy_mult = 3.0f;
				}

				ShotAngle /= accuracy_mult;

				if (ShotAngle < DegreeToRadian(0.8f)) {
					ShotAngle = DegreeToRadian(0.8f);
				}
			}

			if (ShotAngle < DegreeToRadian(0.8f)) {
				ShotAngle = DegreeToRadian(0.8f);
			}
		}
		else {
			//밎귩뫅궑궫궴뵽믦궥귡갂떀뾢뚮뜼귩똶럁궥귡
			Param->GetScopeParam(scopemode, &sparam);
			ShotAngle = sparam.AI_ShotAngleLong;

			//AI깒긹깑궟궴궸뮧맢
			ShotAngle += DegreeToRadian(0.2f) * LevelParam->limitserror;

			float accuracy_mult = GameConfig.GetAIAccuracyMultiplier();
			if (accuracy_mult < 0.1f) accuracy_mult = 0.1f;
			if (accuracy_mult > 3.0f) accuracy_mult = 3.0f;

			ShotAngle /= accuracy_mult;

			if (ShotAngle < DegreeToRadian(0.8f)) {
				ShotAngle = DegreeToRadian(0.8f);
			}
		}

		// [✨ 여기가 수정된 부분입니다 ✨]
		// Get weapon parameters to check for Minigun
		WeaponParameter wparam;
		if (weaponid != ID_WEAPON_NONE) {
			Param->GetWeapon(weaponid, &wparam);
		}
		else {
			wparam.burstcnt = 1; // 무기가 없을 경우 기본값
		}

		// 敵を捉えていれば
		float atanxy = fabsf(atanx) + fabsf(atany);

		if (atanxy < ShotAngle) {
			float firerate_mult = GameConfig.GetAIFireRateMultiplier();

			// 미니건을 들고 있다면 설정된 배율을 무시하고 항상 최대 연사 속도(4.0f)로 고정합니다.
			if (wparam.burstcnt == 6) {
				firerate_mult = 4.0f;
			}

			if (firerate_mult <= 0.0f) {
				// FireRate 0.0이면 AI가 발사하지 않음
				return;
			}

			if (firerate_mult > 4.0f) {
				firerate_mult = 4.0f;
			}

			// 플레이어 로직과 동일하게 burstcnt == 0 또는 6을 연사 가능 무기로 취급
			bool fullauto_weapon = (wparam.burstcnt == 0 || wparam.burstcnt == 6);

			// FireRate 1.0은 기존 AI 발사 로직을 그대로 사용한다.
// FireRate 1.0 초과 ~ 4.0 미만은 4.0에 가까워질수록 발사 시도 확률을 부드럽게 높인다.
// FireRate 4.0 이상은 모든 무기에서 매 프레임 발사 시도한다.
// 실제 발사 간격은 ObjMgr->ShotWeapon() / weapon 쪽의 무기 딜레이가 제한한다.

			if (firerate_mult <= 1.0f) {
				// 기존 로직 유지.
				// FireRate 1.0일 때 이전과 같은 발사 확률이 나오도록 한다.
				float safe_firerate = firerate_mult;

				if (safe_firerate < 0.1f) {
					safe_firerate = 0.1f;
				}

				int rand = (int)((float)LevelParam->attack / safe_firerate);

				if (longattack == true) {
					rand += 1;
				}

				if (rand < 1) {
					rand = 1;
				}

				if (GetRand(rand) == 0) {
					ObjMgr->ShotWeapon(ctrlid);
				}
			}
			else if (firerate_mult >= 4.0f) {
				// FireRate 4.0 이상에서는 연사/단발 구분 없이 매 프레임 발사 시도.
				// 권총, 저격총, 샷건 같은 단발 무기도 무기 자체 딜레이가 끝나는 즉시 발사하게 된다.
				ObjMgr->ShotWeapon(ctrlid);
			}
			else {
				// 1.0 ~ 4.0 범위를 0.0 ~ 1.0으로 정규화한다.
				float rate_t = (firerate_mult - 1.0f) / 3.0f;

				if (rate_t < 0.0f) {
					rate_t = 0.0f;
				}
				if (rate_t > 1.0f) {
					rate_t = 1.0f;
				}

				// 기본 발사 간격.
				// FireRate 1.0일 때의 기존 rand 값과 같은 기준으로 시작한다.
				float base_rand = (float)LevelParam->attack;

				if (longattack == true) {
					base_rand += 1.0f;
				}

				if (base_rand < 1.0f) {
					base_rand = 1.0f;
				}

				// FireRate가 4.0에 가까워질수록 virtual_rand가 1.0에 가까워진다.
				// virtual_rand == 1.0이면 사실상 매 프레임 발사 시도에 가깝다.
				float remain = 1.0f - rate_t;
				float virtual_rand = 1.0f + (base_rand - 1.0f) * remain * remain;

				if (virtual_rand < 1.0f) {
					virtual_rand = 1.0f;
				}

				// GetRand(1000)을 사용해서 정수 절단으로 인한 계단식 변화를 줄인다.
				int fire_chance = (int)(1000.0f / virtual_rand);

				if (fire_chance < 1) {
					fire_chance = 1;
				}
				if (fire_chance > 1000) {
					fire_chance = 1000;
				}

				if (GetRand(1000) < fire_chance) {
					ObjMgr->ShotWeapon(ctrlid);
				}
			}
		}
		else {
			// 조준이 완전히 벗어났을 때만 점사 카운터를 초기화한다.
			// 매번 실패 프레임마다 ResetBurstModeCnt()를 부르면 연사가 끊길 수 있다.
			ctrlhuman->ResetBurstModeCnt();
		}
	}

	//떁뿣궸돒궣궲뗟떁뿣갋돀떁뿣귩먛귟뫶궑귡
	//?겍 AI_CHECKSHORTATTACK_DIST 긯긞?깏궳먠믦뭠댸렃
	if( r < AI_CHECKSHORTATTACK_DIST * AI_CHECKSHORTATTACK_DIST ){
		longattack = false;
	}
	if( (r > AI_CHECKSHORTATTACK_DIST * AI_CHECKSHORTATTACK_DIST)&&(MoveNavi->GetRun2() == false) ){
		longattack = true;
	}

	if (zombie == false) {
		// 앉은 상태에서는 랜덤 이동을 만들지 않는다.
		// 이동 플래그가 생기면 ControlCrouch()에서 바로 일어나는 문제가 발생할 수 있다.
		if (crouchcnt <= 0 && ctrlhuman->GetCrouchFlag() == false) {
			MoveRandom();
		}
	}

	actioncnt += 1;
}

//! @brief 뛘똼귩긌긿깛긜깑
bool AIcontrol::ActionCancel()
{
	//뷄먰벉돸긲깋긐궕뾎뚼궶귞뢎뿹
	if (NoFight == true) {
		return true;
	}

	if (enemyhuman == NULL) {
		return true;
	}

	// 플레이어 스킬: 이미 발견한 대상도 은신하면 공격/추적 해제
	if (enemyhuman->GetSkillStealthFlag() == true) {
		return true;
	}

	//밎궕?뻊궢궫귞뢎뿹
	if (enemyhuman->GetDeadFlag() == true) {
		return true;
	}

	//떁뿣귩롦벦
	float tx, ty, tz;
	enemyhuman->GetPosData(&tx, &ty, &tz, NULL);
	float x = posx - tx;
	float y = posy - ty;
	float z = posz - tz;
	float r = x*x + y*y + z*z;

	//떁뿣궕뿣귢됡궗궲궋궫귞뢎뿹
	if( r > 620.0f*620.0f ){
		return true;
	}

	if( longattack == false ){
		//밙뱰궶?귽?깛긐궳밎궕뙥궑귡궔둴봃
		if( GetRand(40) == 0 ){
			//긳깓긞긏궕롏궯궲궋궫걁걖뙥궑궶궋걂궶귞궽뢎뿹
			if( CheckLookEnemy(enemyhuman, AI_SEARCH_RX, AI_SEARCH_RY, 620.0f, NULL) == false ){
				return true;
			}
		}

		//떗맕밒궸뢎뿹
		if( GetRand(550) == 0 ){
			return true;
		}
	}
	else{
		//밙뱰궶?귽?깛긐궳밎궕뙥궑귡궔둴봃
		if( GetRand(30) == 0 ){
			//긳깓긞긏궕롏궯궲궋궫걁걖뙥궑궶궋걂궶귞궽뢎뿹
			if( CheckLookEnemy(enemyhuman, AI_SEARCH_RX, AI_SEARCH_RY, 620.0f, NULL) == false ){
				return true;
			}
		}

		//떗맕밒궸뢎뿹
		if( GetRand(450) == 0 ){
			return true;
		}
	}

	return false;
}

//! @brief 븧딇귩렃궰
int AIcontrol::HaveWeapon()
{
	int selectweapon;
	class weapon *weapon[TOTAL_HAVEWEAPON];
	int lnbslist[TOTAL_HAVEWEAPON];
	int nbslist[TOTAL_HAVEWEAPON];
	int nbs = 0;

	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
		lnbslist[i] = 0;
		nbslist[i] = 0;
	}

	//븧딇궻륃뺪귩롦벦
	ctrlhuman->GetWeapon(&selectweapon, weapon, lnbslist, nbslist);

	//븧딇귩렃궯궲궋궶궋궸귖듫귦귞궦갂몧뭙릶궕궇귡궶귞먛귟뫶궑궶궋
	if( (weapon[selectweapon] == NULL)&&(nbslist[selectweapon] > 0) ){
		return 0;
	}

	//븧딇귩렃궯궲궋귢궽갂뜃똶뭙릶귩롦벦
	if( weapon[selectweapon] != NULL ){
		weapon[selectweapon]->GetParamData(NULL, NULL, &nbs);
	}

	//븧딇귩렃궯궲궋궶궋궔갂뭙궕0궶귞궽
	if( (weapon[selectweapon] == NULL)||(nbs == 0) ){
		//렅궻븧딇귩럚믦
		int notselectweapon = selectweapon + 1;
		if( notselectweapon == TOTAL_HAVEWEAPON ){ notselectweapon = 0; }

		//렅궻븧딇귩렃궯궲궋귢궽
		if( weapon[notselectweapon] != NULL ){
			//렅궻븧딇궻뭙릶궕0궳궶궚귢궽
			weapon[notselectweapon]->GetParamData(NULL, NULL, &nbs);
			if( nbs > 0 ){
				//렃궭뫶궑귡
				ObjMgr->ChangeHaveWeapon(ctrlid);
				return 1;
			}
		}
	}

	return 0;
}

//! @brief 댷벍귘뺴뛀?듂귩깋깛??궸뢎뿹
void AIcontrol::CancelMoveTurn()
{
	int forward, back, side, updown, rightleft;

	if( battlemode == AI_ACTION ){			//뛘똼뭷
		if( MoveNavi->GetRun2() == true ){	//뾆먩밒궶몪귟
			ObjDriver->DelModeFlag(AI_CTRL_MOVEFORWARD);
			ObjDriver->DelModeFlag(AI_CTRL_MOVEBACKWARD);
			ObjDriver->DelModeFlag(AI_CTRL_MOVELEFT);
			ObjDriver->DelModeFlag(AI_CTRL_MOVERIGHT);
			ObjDriver->DelModeFlag(AI_CTRL_TURNUP);
			ObjDriver->DelModeFlag(AI_CTRL_TURNDOWN);
			ObjDriver->DelModeFlag(AI_CTRL_TURNLEFT);
			ObjDriver->DelModeFlag(AI_CTRL_TURNRIGHT);
			if( GetRand(3) == 0 ){
				ObjDriver->DelModeFlag(AI_CTRL_MOVEWALK);
			}
			return;
		}
		else{									//뾆먩밒궶몪귟 댥둖
			if( longattack == false ){
				forward = 6;
				back = 6;
				side = 7;
				updown = 5;
				rightleft = 6;
			}
			else{
				forward = 5;
				back = 4;
				side = 5;
				updown = 3;
				rightleft = 3;
			}
		}
	}
	else if( battlemode == AI_CAUTION ){	//똸둀뭷
		forward = 10;
		back = 10;
		side = 10;
		updown = 14;
		rightleft = 20;
	}
	else{
		forward = 12;
		back = 12;
		side = 12;
		updown = 15;
		rightleft = 18;
	}

	//댷벍귩깋깛??궸?귕귡
	if( GetRand(forward) == 0 ){
		ObjDriver->DelModeFlag(AI_CTRL_MOVEFORWARD);
	}
	if( GetRand(back) == 0 ){
		ObjDriver->DelModeFlag(AI_CTRL_MOVEBACKWARD);
	}
	if( GetRand(side) == 0 ){
		ObjDriver->DelModeFlag(AI_CTRL_MOVELEFT);
	}
	if( GetRand(side) == 0 ){
		ObjDriver->DelModeFlag(AI_CTRL_MOVERIGHT);
	}
	if( GetRand(3) == 0 ){
		ObjDriver->DelModeFlag(AI_CTRL_MOVEWALK);
	}

	//됷?귩깋깛??궸?귕귡
	if( GetRand(updown) == 0 ){
		ObjDriver->DelModeFlag(AI_CTRL_TURNUP);
	}
	if( GetRand(updown) == 0 ){
		ObjDriver->DelModeFlag(AI_CTRL_TURNDOWN);
	}
	if( GetRand(rightleft) == 0 ){
		ObjDriver->DelModeFlag(AI_CTRL_TURNLEFT);
	}
	if( GetRand(rightleft) == 0 ){
		ObjDriver->DelModeFlag(AI_CTRL_TURNRIGHT);
	}
}

//! @brief 븧딇귩깏깓?긤갋롆궲귡
//! @return 롆궲귡갌1?깏깓?긤갌2?렃궭뫶궑귡갌3?FULL/SEMI먛귟뫶궑갌4?돺귖궢궶궋갌0
int AIcontrol::ControlWeapon()
{
	int selectweapon;
	class weapon *weapon[TOTAL_HAVEWEAPON];
	int weaponid, lnbs, nbs;
	WeaponParameter paramdata;
	int burstcntS, burstcntN;

	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
	}

	//븧딇궻륃뺪귩롦벦
	ctrlhuman->GetWeapon(&selectweapon, weapon, NULL, NULL);
	if (weapon[selectweapon] == NULL) { return 0; }
	weapon[selectweapon]->GetParamData(&weaponid, &lnbs, &nbs);

	// AI가 이미 재장전 중이면 human::ChangeHaveWeapon()에서 전환이 막힌다.
	// 따라서 방패 복귀는 재장전이 시작되기 전, 즉 탄창이 비었거나 탄약이 부족한 순간에 처리한다.
	if (ctrlhuman->GetWeaponReloadCnt() > 0 || ctrlhuman->IsShellReloading() == true) {
		return 0;
	}

	// 총 탄약 부족/재장전 직전이면 방패 슬롯으로 복귀한다.
	// 이 처리는 실제 92번 방패를 가진 AI에게만 적용된다.
	if (ShouldAIReturnToShieldForAmmoShortage(weaponid, lnbs, nbs) == true) {
		int shield_slot = FindAIShieldSlot(selectweapon, weapon);

		if (shield_slot >= 0) {
			ObjMgr->ChangeHaveWeapon(ctrlid, shield_slot);
			crouchcnt = 0;
			ctrlhuman->SetCrouchFlag(false);
			return 3;
		}
	}

	//븧딇궻맜?귩롦벦
	if (Param->GetWeapon(weaponid, &paramdata) == 1) { return 0; }

	//긚긓?긵됶룣
	ctrlhuman->SetDisableScope();

	//걏긑?긚걐궶귞돺귖궢궶궋
	if( weaponid == ID_WEAPON_CASE ){ return 0; }

	//먰벉뭷궸긐깒긨?긤귩렃궯궲궋귢궽
	if( battlemode == AI_ACTION ){
		if( weaponid == ID_WEAPON_GRENADE ){
			int nextselectweapon, nextnds;

			//렅궻븧딇궻뭙릶귩롦벦
			nextselectweapon = selectweapon + 1;
			nextnds = 0;
			if( nextselectweapon == TOTAL_HAVEWEAPON ){
				nextselectweapon = 0;
			}
			if( weapon[nextselectweapon] != NULL ){
				weapon[nextselectweapon]->GetParamData(NULL, NULL, &nextnds);
			}

			if( longattack == false ){
				// 1/100궻둴뿦궳렃궭뫶궑귡
				if( (GetRand(100) == 0)&&(nextnds > 0) ){
					ObjMgr->ChangeHaveWeapon(ctrlid);
					return 3;
				}
			}
			else{
				// 1/66궻둴뿦궳렃궭뫶궑귡
				if( (GetRand(66) == 0)&&(nextnds > 0) ){
					ObjMgr->ChangeHaveWeapon(ctrlid);
					return 3;
				}
			}
		}
	}

	//럄뭙릶궕뼰궚귢궽
	if( lnbs == 0 ){
		int ways;

		//룉뿚둴뿦뙂믦
		if( battlemode == AI_NORMAL ){ ways = 1; }
		else if( battlemode == AI_CAUTION ){ ways = 10; }
		else{ ways = 8; }

		// 1/ways궻둴뿦궳룉뿚
		if( GetRand(ways) == 0 ){
			int under;

			//깏깓?긤둴뿦
			if( battlemode == AI_NORMAL ){
				ways = 1;
				under = 0;
			}
			else if( battlemode == AI_CAUTION ){
				ways = 5;
				under = 3;
			}
			else{
				if( longattack == false ){
					// 둴뿦궼?3/4
					ways = 4;
					under = 2;
				}
				else{
					// 둴뿦궼?2/3
					ways = 3;
					under = 1;
				}
			}

			//뭙궕뼰궚귢궽롆궲귡
			if( nbs == 0 ){
				ObjMgr->DumpWeapon(ctrlid);
				return 1;
			}

			//깋깛??궸 깏깓?긤렳뛱 or 븧딇귩렃궭궔궑
			if( GetRand(ways) <= under ){
				ObjMgr->ReloadWeapon(ctrlid);
				return 2;
			}
			//else{
				ObjMgr->ChangeHaveWeapon(ctrlid);
				return 3;
			//}
		}
	}

	//쁀롅먛귟뫶궑궕됀?궶븧딇궶귞
	if( paramdata.ChangeWeaponID != -1 ){
		//뙸띪궻븧딇궻쁀롅먠믦귩롦벦
		burstcntS = paramdata.burstcnt;

		//륷궫궶븧딇궻쁀롅먠믦귩롦벦
		if( Param->GetWeapon(paramdata.ChangeWeaponID, &paramdata) == 1 ){ return 0; }
		burstcntN = paramdata.burstcnt;

		if( longattack == false ){
			//뗟떁뿣뛘똼뭷궳갂뙸띪SEMI갋먛귟뫶궑귡궴FULL궸궶귡궶귞
			if( (burstcntS == 1)&&(burstcntN != 1) ){
				ObjMgr->ChangeWeaponID(ctrlid);	//먛귟뫶궑귡
				return 4;
			}
		}
		else{
			//돀떁뿣뛘똼뭷궳갂뙸띪FULL갋먛귟뫶궑귡궴SEMI궸궶귡궶귞
			if( (burstcntS != 1)&&(burstcntN == 1) ){
				ObjMgr->ChangeWeaponID(ctrlid);	//먛귟뫶궑귡
				return 4;
			}
		}
	}

	return 0;
}

//! @brief AI 앉기 제어
void AIcontrol::ControlCrouch()
{
	if (ctrlhuman == NULL) { return; }

	// 사망/비활성/비전투/타겟 없음이면 즉시 일어서기
	if (
		ctrlhuman->GetEnableFlag() == false ||
		ctrlhuman->GetDeadFlag() == true ||
		NoFight == true ||
		battlemode != AI_ACTION ||
		enemyhuman == NULL
		) {
		crouchcnt = 0;
		ctrlhuman->SetCrouchFlag(false);
		return;
	}

	// AI가 경로 이동 중이면 기존 crouchcnt가 남아 있어도 앉기 금지
	bool path_moving = false;
	if (MoveNavi->GetRun2() == true) { path_moving = true; }
	if (MoveNavi->GetMoveMode() == AI_GRENADE) { path_moving = true; }

	if (path_moving == true) {
		crouchcnt = 0;
		ctrlhuman->SetCrouchFlag(false);
		return;
	}

	// 현재 이동 입력이 있으면 새로 앉기 금지
	bool moving = false;
	if (ObjDriver->GetModeFlag(AI_CTRL_MOVEFORWARD) == true) { moving = true; }
	if (ObjDriver->GetModeFlag(AI_CTRL_MOVEBACKWARD) == true) { moving = true; }
	if (ObjDriver->GetModeFlag(AI_CTRL_MOVELEFT) == true) { moving = true; }
	if (ObjDriver->GetModeFlag(AI_CTRL_MOVERIGHT) == true) { moving = true; }

	// 무기 상태 확인
	int weaponid = ctrlhuman->GetMainWeaponTypeNO();

	// 방패를 든 AI는 앉지 못하게 한다.
	// 방패는 서 있는 정면 방어를 전제로 하므로, crouchcnt가 남아 있어도 즉시 일어서게 한다.
	if (
		weaponid == ID_WEAPON_SHIELD ||
		ctrlhuman->GetRenderOverrideWeaponID() == ID_WEAPON_SHIELD
		) {
		crouchcnt = 0;
		ctrlhuman->SetCrouchFlag(false);
		return;
	}

	// 맨손/수류탄/케이스는 앉기 대상 제외
	// 특히 수류탄 투척 전후에는 반드시 일어서게 한다.
	if (
		weaponid == ID_WEAPON_NONE ||
		weaponid == ID_WEAPON_GRENADE ||
		weaponid == ID_WEAPON_CASE
		) {
		crouchcnt = 0;
		ctrlhuman->SetCrouchFlag(false);
		return;
	}

	// 좀비 타입은 앉지 않게 한다.
	int paramid;
	HumanParameter hparam;
	ctrlhuman->GetParamData(&paramid, NULL, NULL, NULL);
	Param->GetHuman(paramid, &hparam);

	if (hparam.type == 2) {
		crouchcnt = 0;
		ctrlhuman->SetCrouchFlag(false);
		return;
	}

	// 재장전 중이면 앉거나, 이미 앉은 상태를 유지한다.
	bool reloading = false;
	if (ctrlhuman->GetWeaponReloadCnt() > 0) { reloading = true; }
	if (ctrlhuman->IsShellReloading() == true) { reloading = true; }

	if (reloading == true) {
		if (moving == false) {
			if (crouchcnt < (int)GAMEFPS) {
				crouchcnt = (int)GAMEFPS;
			}

			ObjDriver->DelModeFlag(AI_CTRL_MOVEFORWARD);
			ObjDriver->DelModeFlag(AI_CTRL_MOVEBACKWARD);
			ObjDriver->DelModeFlag(AI_CTRL_MOVELEFT);
			ObjDriver->DelModeFlag(AI_CTRL_MOVERIGHT);
			ObjDriver->DelModeFlag(AI_CTRL_MOVEWALK);

			ctrlhuman->SetCrouchFlag(true);
			return;
		}

		// 이동 중 재장전이면 강제로 앉히지는 않음
		crouchcnt = 0;
		ctrlhuman->SetCrouchFlag(false);
		return;
	}

	// 이미 앉아 있는 경우에는 이동 플래그보다 먼저 앉기 유지 처리
// Action() 끝의 MoveRandom()이 이동 플래그를 세워도, crouchcnt가 남아 있으면 일어서지 않는다.
	if (crouchcnt > 0) {
		crouchcnt--;

		ObjDriver->DelModeFlag(AI_CTRL_MOVEFORWARD);
		ObjDriver->DelModeFlag(AI_CTRL_MOVEBACKWARD);
		ObjDriver->DelModeFlag(AI_CTRL_MOVELEFT);
		ObjDriver->DelModeFlag(AI_CTRL_MOVERIGHT);
		ObjDriver->DelModeFlag(AI_CTRL_MOVEWALK);

		ctrlhuman->SetCrouchFlag(true);
		return;
	}

	// 일반 이동 중에는 새로 앉기 금지
	// 단, 이미 앉아 있던 경우는 위에서 처리되므로 여기서 끊기지 않는다.
	if (moving == true) {
		crouchcnt = 0;
		ctrlhuman->SetCrouchFlag(false);
		return;
	}

	// 새로 앉을지 결정
	// 근거리: 덜 앉고 짧게 유지
	// 원거리: 더 자주 앉고 오래 유지
	int rand = 220;
	if (longattack == true) {
		rand = 85;
	}

	if (GetRand(rand) == 0) {
		if (longattack == true) {
			// 원거리 교전: 약 5~8초
			crouchcnt = ((int)GAMEFPS * 5) + GetRand((int)GAMEFPS * 3);
		}
		else {
			// 근거리 교전: 약 2~3초
			crouchcnt = ((int)GAMEFPS * 2) + GetRand((int)GAMEFPS * 1);
		}

		ObjDriver->DelModeFlag(AI_CTRL_MOVEFORWARD);
		ObjDriver->DelModeFlag(AI_CTRL_MOVEBACKWARD);
		ObjDriver->DelModeFlag(AI_CTRL_MOVELEFT);
		ObjDriver->DelModeFlag(AI_CTRL_MOVERIGHT);
		ObjDriver->DelModeFlag(AI_CTRL_MOVEWALK);

		ctrlhuman->SetCrouchFlag(true);
	}
	else {
		ctrlhuman->SetCrouchFlag(false);
	}
}

//! @brief 롨왮뭙귩뱤궛귡
//! @return 룉뿚뭷갌0?뱤궛뢎귦궯궫갌1?롨왮뭙귩렃궯궲궋궶궋갌2
//! @attention 롨왮뭙귩뱤궛귡긬긚궸돒궣궫룉뿚궳궥갃밎뵯뙥렄궻뛘똼궼Action()듫릶귩랷뤖궢궲궘궬궠궋갃
int AIcontrol::ThrowGrenade()
{
	// 수류탄 투척 전후에는 앉지 않음
	crouchcnt = 0;
	if (ctrlhuman != NULL) {
		ctrlhuman->SetCrouchFlag(false);
	}

	int selectweapon;
	class weapon* weapon[TOTAL_HAVEWEAPON];
	int weaponid, nbs, i;
	pointdata pdata;
	float posy2;
	float atan_rx, atan_ry;

	for(int i=0; i<TOTAL_HAVEWEAPON; i++){
		weapon[i] = NULL;
	}

	//롨왮뭙귩렃궯궲궋귡궔둴봃
	ctrlhuman->GetWeapon(&selectweapon, weapon, NULL, NULL);
	for(i=0; i<TOTAL_HAVEWEAPON; i++){
		if( weapon[i] != NULL ){
			weapon[i]->GetParamData(&weaponid, NULL, &nbs);
			if( weaponid == ID_WEAPON_GRENADE ){
				break;
			}
		}
	}

	//롨왮뭙궸먛귟뫶궑귞귢귡궶귞갂먛귟뫶궑귡
	if( (i != TOTAL_HAVEWEAPON)&&(i != selectweapon) ){
		ObjMgr->ChangeHaveWeapon(ctrlid, i);
	}

	//긬긚궴릐궻뛼궠귩롦벦
	MoveNavi->GetPathPointData(&pdata);
	posy2 = posy + VIEW_HEIGHT;

	//덇뱗멣궲궻벍궖귩?귕귡
	ObjDriver->ResetMode();

	//뽞뷭뭤?귉궻둷뱗귩땫귕귡
	CheckTargetAngle(posx, posy2, posz, rx*-1 + (float)M_PI/2, ry, pdata.x, pdata.y, pdata.z, 0.0f, &atan_rx, &atan_ry, NULL);

	//롨왮뭙귩렃궯궲궋궶궚귢궽
	if( i == TOTAL_HAVEWEAPON ){
		//겍롨왮뭙귩렃궯궲궋궶궋뤾뜃궳귖갂궞궻긲깒??궳궼궩궻뺴뛀귉뛀궔궧귡귝궎룉뿚궥귡갃
		//?궞궻룉뿚궸귝귟갂롨왮뭙귩렃궯궲궋궶궋륉뫴궳 궔궰롨왮뭙뱤궛궻뼰뙽깑?긵궸듮궯궫뤾뜃갂롨왮뭙뱤궛긬긚궻뺴둷귩뛀궖뫏궚귡갃
		//?걁?됄XOPS궻럅뾩걂

		//먿됷
		if( atan_rx > 0.0f ){
			ObjDriver->SetModeFlag(AI_CTRL_TURNLEFT);
		}
		if( atan_rx < 0.0f ){
			ObjDriver->SetModeFlag(AI_CTRL_TURNRIGHT);
		}
		ArmAngle();

		return 2;
	}

	//먿됷
	if( atan_rx > 0.0f ){
		ObjDriver->SetModeFlag(AI_CTRL_TURNLEFT);
	}
	if( atan_rx < 0.0f ){
		ObjDriver->SetModeFlag(AI_CTRL_TURNRIGHT);
	}
	if( atan_ry > 0.0f ){
		ObjDriver->SetModeFlag(AI_CTRL_TURNUP);
	}
	if( atan_ry < 0.0f ){
		ObjDriver->SetModeFlag(AI_CTRL_TURNDOWN);
	}

	//뤖?궸믦귏궯궲궋귡궔둴봃
	if( (fabsf(atan_rx) < DegreeToRadian(1.5f))&&(fabsf(atan_ry) < DegreeToRadian(1.5f)) ){
		//쁀롅궠궧궶궋
		if( (ctrlhuman->GetWeaponBlazingmode() == false)&&(EventWeaponShot == true) ){ return 0; }

		//둷뱗귩먠믦
		ctrlhuman->SetRxRy(rx, ry);

		//뱤궛귡
		if( ObjMgr->ShotWeapon(ctrlid) != 0 ){
			EventWeaponShot = true;
			return 1;
		}
	}

	return 0;
}

//! @brief 쁱궻둷뱗귩먠믦
void AIcontrol::ArmAngle()
{
	ObjDriver->DelModeFlag(AI_CTRL_TURNUP);
	ObjDriver->DelModeFlag(AI_CTRL_TURNDOWN);

	if( ctrlhuman->GetMainWeaponTypeNO() == ID_WEAPON_NONE ){	//롨귆귞
		//돷궸뛀궚뫏궚귡
		ObjDriver->SetModeFlag(AI_CTRL_TURNDOWN);
	}
	else if( (battlemode == AI_CAUTION)&&(cautioncnt > 0) ){	//똸둀뭷
		float addry2 = 0.0f - ry;

		//먿됷
		if( addry2 > DegreeToRadian(1.0f) ){
			ObjDriver->SetModeFlag(AI_CTRL_TURNUP);
		}
		if( addry2 < DegreeToRadian(-1.0f) ){
			ObjDriver->SetModeFlag(AI_CTRL_TURNDOWN);
		}
	}
	else{									//빟륂렄궳븧딇룋뾎뭷
		//먿됷
		if( ry < DegreeToRadian(-32) ){
			ObjDriver->SetModeFlag(AI_CTRL_TURNUP);
		}
		if( ry > DegreeToRadian(-28) ){
			ObjDriver->SetModeFlag(AI_CTRL_TURNDOWN);
		}
	}
}

//! @brief 밎귩뭈궥
int AIcontrol::SearchEnemy()
{
	//뷄먰벉돸긲깋긐궕뾎뚼궶귞밎귩뙥궰궚궶궋
	if( NoFight == true ){
		return 0;
	}

	if( battlemode == AI_ACTION ){ return 0; }

	int weaponid = ctrlhuman->GetMainWeaponTypeNO();
	int weaponscope = 0;
	ScopeParameter sparam;
	int searchloops;
	float maxDist;
	float A_rx, A_ry, B_rx, B_ry;

	//븧딇귩렃궯궲궋귢궽갂긚긓?긵긾?긤귩롦벦
	if( weaponid != ID_WEAPON_NONE ){
		WeaponParameter Wparam;

		Param->GetWeapon(weaponid, &Wparam);
		weaponscope = Wparam.scopemode;
	}

	//밎궻뭈랊됷릶궴뭈랊붝댪걁떁뿣궴둷뱗걂귩먠믦
	if( battlemode == AI_NORMAL ){
		searchloops = (LevelParam->search) * AI_TOTALHUMAN_SCALE;

		Param->GetScopeParam(weaponscope, &sparam);
		maxDist = sparam.AI_AddSearchEnemyMaxDist_Normal;
		maxDist += 12.0f*((LevelParam->search)-2) + 350.0f;
		float search_mult = GameConfig.GetAISearchRangeMultiplier();

		if (search_mult < 0.0f) {
			search_mult = 0.0f;
		}
		if (search_mult > 2.5f) {
			search_mult = 2.5f;
		}

		maxDist *= search_mult;
		A_rx = DegreeToRadian(110);
		A_ry = DegreeToRadian(60);
		B_rx = DegreeToRadian(60);
		B_ry = DegreeToRadian(40);
	}
	else {	//battlemode == AI_CAUTION
		searchloops = (LevelParam->search) * AI_TOTALHUMAN_SCALE + 4;

		Param->GetScopeParam(weaponscope, &sparam);
		maxDist = sparam.AI_AddSearchEnemyMaxDist_Caution;
		maxDist += 15.0f*((LevelParam->search)-2) + 420.0f;
		float search_mult = GameConfig.GetAISearchRangeMultiplier();
		if (search_mult < 0.1f) search_mult = 0.1f;
		if (search_mult > 2.5f) search_mult = 2.5f;

		maxDist *= search_mult;
		A_rx = DegreeToRadian(130);
		A_ry = DegreeToRadian(80);
		B_rx = DegreeToRadian(80);
		B_ry = DegreeToRadian(50);
	}

	//럚믦됷릶갂밎귩뭈랊
	for(int i=0; i<searchloops; i++){
		int targetid = GetRand(MAX_HUMAN);

		if( CheckLookEnemy(targetid, A_rx, A_ry, AI_CHECKSHORTATTACK_DIST, NULL) == true ){
			longattack = false;
			return 1;
		}

		if( CheckLookEnemy(targetid, B_rx, B_ry, maxDist, NULL) == true ){
			if( GetRand(4) == 0 ){
				if( MoveNavi->GetRun2() == true ){ longattack = false; }
				else{ longattack = true; }
				return 2;
			}
		}
	}
	return 0;
}

//! @brief 밎귩뭈궥걁돀떁뿣뛘똼뭷궸뗟떁뿣귩뭈궥걂
int AIcontrol::SearchShortEnemy()
{
	float A_rx, A_ry;
	A_rx = DegreeToRadian(100);
	A_ry = DegreeToRadian(52);

	for(int i=0; i<3; i++){
		int targetid = GetRand(MAX_HUMAN);

		if( CheckLookEnemy(targetid, A_rx, A_ry, AI_CHECKSHORTATTACK_DIST, NULL) == true ){
			longattack = false;
			return 1;
		}
	}
	return 0;
}

//! @brief 밎궕뙥궑귡궔뵽믦
bool AIcontrol::CheckLookEnemy(int id, float search_rx, float search_ry, float maxDist, float *out_minDist)
{
	if( ObjMgr == NULL ){ return false; }

	//릐궻긆긳긙긃긏긣귩롦벦
	class human* thuman;
	thuman = ObjMgr->GetHumanObject(id);
	if( thuman == NULL ){ return false; }

	//벏뼹듫릶귩긆?긫?깓?긤
	return CheckLookEnemy(thuman, search_rx, search_ry, maxDist, out_minDist);
}

//! @brief 밎궕뙥궑귡궔뵽믦
bool AIcontrol::CheckLookEnemy(class human* thuman, float search_rx, float search_ry, float maxDist, float *out_minDist)
{
	//return false;

	//뼰뚼궶긢??궶귞뢎뿹
	if (ctrlhuman == NULL) { return false; }
	if (thuman == NULL) { return false; }
	if (thuman->GetEnableFlag() == false) { return false; }
	if (thuman->GetDeadFlag() == true) { return false; }

	// 플레이어 스킬: 은신 중인 대상은 AI가 볼 수 없음
	if (thuman->GetSkillStealthFlag() == true) { return false; }

	//렔빁궴밎궻띆뷭귩롦벦
	int ctrlteam, targetteam;
	float tx, ty, tz;
	ctrlhuman->GetParamData(NULL, NULL, NULL, &ctrlteam);
	thuman->GetParamData(NULL, NULL, NULL, &targetteam);
	thuman->GetPosData(&tx, &ty, &tz, NULL);

	const float crouch_ai_eye_y_offset = -2.0f;

	float target_view_height = VIEW_HEIGHT;
	if (thuman->GetCrouchFlag() == true) {
		target_view_height += crouch_ai_eye_y_offset;
	}

	float self_view_height = VIEW_HEIGHT;
	if (ctrlhuman->GetCrouchFlag() == true) {
		self_view_height += crouch_ai_eye_y_offset;
	}

	float self_eye_y = posy + self_view_height;
	float target_eye_y = ty + target_view_height;

	// 자신과 적이 같은 팀이 아니면
	if (ctrlteam != targetteam) {
		float mrx, mry;
		float Dist2, Dist;
		float Dist_dummy = 0.0f;

		// 거리와 각도 판정
		if (CheckTargetAngle(posx, self_eye_y, posz, rx * -1 + (float)M_PI / 2, 0.0f, tx, target_eye_y, tz, maxDist, &mrx, &mry, &Dist2) == true) {
			// 시야각 안에 있으면
			if ((fabsf(mrx) < search_rx / 2) && (fabsf(mry) < search_ry / 2)) {
				float vx, vy, vz;

				Dist = sqrtf(Dist2);
				if (Dist <= 0.0f) { return false; }

				// AI 눈높이 → 목표 눈높이 방향 벡터
				vx = (tx - posx) / Dist;
				vy = (target_eye_y - self_eye_y) / Dist;
				vz = (tz - posz) / Dist;

				// 중간에 벽이 없으면 인식
				if (CollD->CheckALLBlockIntersectRay(posx, self_eye_y, posz, vx, vy, vz, NULL, NULL, &Dist_dummy, Dist) == false) {
					if (out_minDist != NULL) { *out_minDist = Dist; }
					enemyhuman = thuman;
					return true;
				}
			}
		}
	}
	return false;
}

//! @brief ?뫬궕궇귡궔둴봃
bool AIcontrol::CheckCorpse(int id)
{
	//긏깋긚먠믦궕궓궔궢궚귢궽룉뿚궢궶궋
	if( ObjMgr == NULL ){ return false; }
	if( ctrlhuman == NULL ){ return false; }

	if( (id < 0)||(MAX_HUMAN <= id) ){ return false; }

	//??긒긞긣궻긏깋긚귩롦벦
	class human* thuman;
	thuman = ObjMgr->GetHumanObject(id);
	if( thuman == NULL ){ return false; }
	if( thuman->GetEnableFlag() == false ){ return false; }

	//?뻊궢궲궋귢궽
	if( thuman->GetDeadFlag() == true ){

		//???붥뜂귩롦벦
		int ctrlteam, targetteam;
		ctrlhuman->GetParamData(NULL, NULL, NULL, &ctrlteam);
		thuman->GetParamData(NULL, NULL, NULL, &targetteam);

		//뼞뺴궶귞궽
		if( ctrlteam == targetteam ){
			float tposx, tposy, tposz;
			float atan;

			//띆뷭궔귞떁뿣귩롦벦
			thuman->GetPosData(&tposx, &tposy, &tposz, NULL);

			//떁뿣궴둷뱗귩똶럁
			if( CheckTargetAngle(posx, posy, posz, rx*-1 + (float)M_PI/2, 0.0f, tposx, tposy, tposz, 22.0f, &atan, NULL, NULL) == true ){
				if( fabsf(atan) < DegreeToRadian(40) ){
					return true;
				}
			}
		}
	}
	return false;
}

//! @brief 긬긚궸귝귡댷벍
void AIcontrol::MovePath()
{
	int movemode, pointmode;
	MoveNavi->GetTargetPos(NULL, NULL, NULL, &movemode, &pointmode);

	if( movemode == AI_NAVI_MOVE_NULL ){			//댶륂궶긬긚
		return;
	}

	if( CheckTargetPos(false) == false ){
		MoveTarget(false);
	}
	else{
		if( (pointmode == AI_NAVI_POINT_WAIT)||(pointmode == AI_NAVI_POINT_TRACKING) ){
			TurnSeen();
		}
		else if( pointmode == AI_NAVI_POINT_STOP_5SEC ){
			if( waitcnt < ((int)GAMEFPS)*5 ){
				if( StopSeen() == true ){
					waitcnt += 1;
				}
			}
			else{
				waitcnt = 0;
				MoveNavi->MovePathNextState();
				MoveNavi->MovePathNowState();
			}
		}
		else{	//pointmode == AI_NAVI_POINT_NULL
			waitcnt = 0;
			MoveNavi->MovePathNextState();
			MoveNavi->MovePathNowState();
		}
	}
}

//! @brief 뛘똼긽귽깛룉뿚
//! @return 븉빾갌false?빾뛛갌true
bool AIcontrol::ActionMain()
{
	int newbattlemode = AI_ACTION;

	//뛘똼룉뿚
	Action();

	//깋깛??긬긚궶귞룉뿚렳뛱?겍걏뾆먩밒궶몪귟걐귩몒믦
	if( MoveNavi->GetMoveMode() == AI_RANDOM ){
		MoveNavi->MovePathNextState();
		MoveNavi->MovePathNowState();
	}

	if( MoveNavi->GetRun2() == true ){				//뾆먩밒궶몪귟
		//뽞뷭뭤?귉댷벍
		if( CheckTargetPos(false) == true ){
			//newbattlemode = AI_NORMAL;

			//궞궻뽞뷭뭤?귩갂똸둀륉뫴됶룣렄궻뽣귡댧뭫궸먠믦궥귡갃
			MoveNavi->GetTargetPos(&cautionback_posx, &cautionback_posz, NULL, NULL, NULL);

			MoveNavi->MovePathNextState();
			MoveNavi->MovePathNowState();
		}
		else{
			MoveTarget2(false);
		}
	}
	else{									//뾆먩밒궶몪귟 댥둖
		//돀떁뿣뛘똼뭷궶귞갂뗟궘궻밎귩뭈궥
		if( longattack == true ){
			SearchShortEnemy();
		}
	}

	//뛘똼뢎뿹뵽믦
	//뛘똼뢎뿹뵽믦
	if (ActionCancel() == true) {
		enemyhuman = NULL;

		// 플레이어/타겟을 잃으면 즉시 일어서기
		crouchcnt = 0;
		ctrlhuman->SetCrouchFlag(false);

		if (MoveNavi->GetRun2() == true) {
			newbattlemode = AI_NORMAL;
		}
		else {
			newbattlemode = AI_CAUTION;
			cautioncnt = 160;
			FaceCaution_flag = false;
			FaceCaution_rx = 0.0f;
		}
	}

	//먠믦귩뵽믦
	if( battlemode != newbattlemode ){
		battlemode = newbattlemode;
		return true;
	}
	return false;
}

//! @brief 똸둀긽귽깛룉뿚
//! @return 븉빾갌false?빾뛛갌true
bool AIcontrol::CautionMain()
{
	int newbattlemode = AI_CAUTION;
	float caution_rx;

	//띆뷭궴???붥뜂귩롦벦
	int teamid;
	ctrlhuman->GetParamData(NULL, NULL, NULL, &teamid);

	//뷀뭙궴돶궻륉떟귩롦벦
	bool HitFlag = ctrlhuman->CheckHit(&caution_rx);
	soundlist soundlist[MAX_SOUNDMGR_LIST];
	int soundlists = GameSound->GetWorldSound(posx, posy + VIEW_HEIGHT, posz, teamid, soundlist);

	//뷄먰벉돸긲깋긐궕뾎뚼궶귞갂돶궼빓궞궑궶궋궞궴궸궥귡
	if( NoFight == true ){
		soundlists = 0;
	}

	//긽귽깛룉뿚
	if( enemyhuman != NULL ){		//딓궸밎귩뙥궰궚궲궋귢궽
		newbattlemode = AI_ACTION;
		actioncnt = 0;
	}
	else if( SearchEnemy() != 0 ){	//밎궕뙥궰궔귢궽
		newbattlemode = AI_ACTION;
		actioncnt = 0;
	}
	else if( HitFlag == true ){		//뷀뭙궢궫
		cautioncnt = 160;					//똸둀귩띋둎
		FaceCaution_flag = true;
		FaceCaution_rx = caution_rx;
	}
	else if( soundlists > 0 ){		//돶궕빓궞궑궫
		cautioncnt = 160;					//똸둀귩띋둎
	}
	else if( cautioncnt == 0 ){		//똸둀귩뢎뿹궥귡궶귞
		if( CheckTargetPos(true) == false ){				//똸둀둎럑뭤?귝귟뿣귢궲궋귡궔
			MoveTarget(true);				//똸둀둎럑뭤?궸뗟궱궘
		}
		else{
			newbattlemode = AI_NORMAL;
			FaceCaution_flag = false;
			FaceCaution_rx = 0.0f;

			//똸둀뫲궭긬긚궶귞렅귉릋귕귡
			pointdata pdata;
			MoveNavi->GetPathPointData(&pdata);
			if( (pdata.p1 == POINT_P1TYPE_AIPATH)&&(pdata.p2 == 4) ){
				MoveNavi->MovePathNextState();
				MoveNavi->MovePathNowState();
			}
		}
	}
	else if( (cautioncnt < 100)&&(FaceCaution_flag == false) ){		//100긲깒??귩먛궯궫귞갂깋깛??궸똸둀뢎뿹걁긇긂깛긣갌0궸걂
		if( GetRand(50) == 0 ){ cautioncnt = 0; }
	}
	else{ cautioncnt -= 1; }

	//믁뷉뭷궳뫮뤭궔귞뿣귢궥궗궫귞갂깋깛??궸똸둀뢎뿹
	if( (MoveNavi->GetMoveMode() == AI_TRACKING)&&(GetRand(3) == 0) ){
		float x, z;
		float tx, tz;
		class human *targethuman = ObjMgr->GetHumanObject(MoveNavi->GetTargetHumanID());
		targethuman->GetPosData(&tx, NULL, &tz, NULL);
		x = posx - tx;
		z = posz - tz;
		if( (x*x + z*z) > 25.0f*25.0f ){
			cautioncnt = 0;
		}
	}

	//됷?궴쁱궻둷뱗
	TurnSeen();
	ArmAngle();

	//먠믦귩뵽믦
	if( battlemode != newbattlemode ){
		battlemode = newbattlemode;
		return true;
	}
	return false;
}

//! @brief 믅륂긽귽깛룉뿚
//! @return 븉빾갌false?빾뛛갌true
bool AIcontrol::NormalMain()
{
	int newbattlemode = AI_NORMAL;
	float caution_rx;
	int EventThrowGrenade = 0;

	MoveNavi->MovePathNowState();
	enemyhuman = NULL;

	//띆뷭궴???붥뜂귩롦벦
	int teamid;
	ctrlhuman->GetParamData(NULL, NULL, NULL, &teamid);

	//뷀뭙궴돶궻륉떟귩롦벦
	bool HitFlag = ctrlhuman->CheckHit(&caution_rx);
	soundlist soundlist[MAX_SOUNDMGR_LIST];
	int soundlists = GameSound->GetWorldSound(posx, posy + VIEW_HEIGHT, posz, teamid, soundlist);

	//뷄먰벉돸긲깋긐궕뾎뚼궶귞갂돶궼빓궞궑궶궋궞궴궸궥귡
	if( NoFight == true ){
		soundlists = 0;
	}

	//깋깛??긬긚궶귞룉뿚렳뛱
	if( MoveNavi->GetMoveMode() == AI_RANDOM ){
		MoveNavi->MovePathNextState();
		MoveNavi->MovePathNowState();
	}

	//롨왮뭙긬긚
	if( MoveNavi->GetMoveMode() == AI_GRENADE ){
		EventThrowGrenade = ThrowGrenade();
		if( EventThrowGrenade != 0 ){
			MoveNavi->MovePathNextState();
			MoveNavi->MovePathNowState();
		}
	}
	if( EventThrowGrenade != 1 ){
		EventWeaponShot = false;
	}

	if( MoveNavi->GetMoveMode() == AI_RUN2 ){		//뾆먩밒궶몪귟궻룉뿚
		//밎귩뙥궰궚궫귞뛘똼궸볺귡
		if( SearchEnemy() != 0 ){
			newbattlemode = AI_ACTION;
			//cautionback_posx = posx;
			//cautionback_posz = posz;
		}
		else{
			MovePath();		//댷벍렳뛱
		}
	}
	else{							//뾆먩밒궶몪귟댥둖궻룉뿚
		//똸둀뵽믦궸볺귡룉뿚
		if( HitFlag == true ){							//뷀뭙궢궫
			newbattlemode = AI_CAUTION;
			cautioncnt = 160;
			cautionback_posx = posx;
			cautionback_posz = posz;
			FaceCaution_flag = true;
			FaceCaution_rx = caution_rx;
		}
		else if(
			(SearchEnemy() != 0)||						//밎귩뙥궰궚궫
			(soundlists > 0)||							//돶궕빓궞궑궫
			(CheckCorpse( GetRand(MAX_HUMAN) ) == true)	//?뫬귩뙥궰궚궫
		){
			newbattlemode = AI_CAUTION;
			cautioncnt = 160;
			cautionback_posx = posx;
			cautionback_posz = posz;
			FaceCaution_flag = false;
			FaceCaution_rx = 0.0f;
		}
		else{
			MovePath();		//댷벍렳뛱
		}
	}

	//쁱궻둷뱗귩먠믦
	if( MoveNavi->GetMoveMode() != AI_GRENADE ){
		ArmAngle();
	}

	//먠믦귩뵽믦
	if( battlemode != newbattlemode ){
		battlemode = newbattlemode;
		return true;
	}
	return false;
}

//! @brief 룊딖돸똭듫릶
void AIcontrol::Init()
{
	//긏깋긚먠믦궕궓궔궢궚귢궽룉뿚궢궶궋
	if( ctrlhuman == NULL ){ return; }
	if( blocks == NULL ){ return; }
	if( CollD == NULL ){ return; }

	//럊뾭궠귢궲궋궶궋릐궶귞룉뿚궢궶궋
	if( ctrlhuman->GetEnableFlag() == false ){ return; }

	// ForceNoAI는 외부에서 설정한 값이므로 Init()에서 false로 되돌리지 않는다.
	NoFight = false;
	battlemode = AI_NORMAL;
	enemyhuman = NULL;
	FaceCaution_flag = false;
	FaceCaution_rx = 0.0f;
	total_move = 0.0f;
	waitcnt = 0;
	gotocnt = 0;
	cautioncnt = 0;
	actioncnt = 0;
	longattack = false;
	EventWeaponShot = false;
	crouchcnt = 0;
	ctrlhuman->SetCrouchFlag(false);
	ctrlhuman->SpeedMultiplier = 1.0f;

	MoveNavi->Init();
	ObjDriver->Init();

	//AI깒긹깑궴먠믦뭠귩롦벦
	int paramid;
	HumanParameter paramdata;
	ctrlhuman->GetParamData(&paramid, NULL, NULL, NULL);
	Param->GetHuman(paramid, &paramdata);
	AIlevel = paramdata.AIlevel;
	Param->GetAIlevel(AIlevel, &LevelParam);

	//렅궻?귽깛긣귩뙚랊
	//MoveNavi->MovePathNextState();
	MoveNavi->MovePathNowState();
}

//! @brief AI깒긹깑귩먠믦
//! @param level AI깒긹깑
void AIcontrol::SetAIlevel(int level)
{
	if( (level < -1)||(TOTAL_PARAMETERINFO_AILEVEL <= level) ){ return; }

	if( level == -1 ){
		//AI깒긹깑귩롦벦
		int paramid;
		HumanParameter paramdata;
		ctrlhuman->GetParamData(&paramid, NULL, NULL, NULL);
		Param->GetHuman(paramid, &paramdata);
		AIlevel = paramdata.AIlevel;
	}
	else{
		AIlevel = level;
	}

	//먠믦뭠귩롦벦
	Param->GetAIlevel(AIlevel, &LevelParam);
}

//! @brief AI깒긹깑귩롦벦
//! @return AI깒긹깑
int AIcontrol::GetAIlevel()
{
	return AIlevel;
}

//! @brief 럚믦궢궫뤾룋귉뫲?궠궧귡
//! @param px X띆뷭
//! @param pz Z띆뷭
//! @param rx 뢣럨궥귡뛀궖
//! @attention 댷벍긬긚궸듫귦귞궦갂럚믦궢궫띆뷭귉궻뫲?귩떗맕궢귏궥갃Init()듫릶귩띋뱗렳뛱궥귡귏궳뙰궸뽣궧귏궧귪갃
void AIcontrol::SetHoldWait(float px, float pz, float rx)
{
	MoveNavi->SetHoldWait(px, pz, rx);
}

//! @brief 럚믦궢궫릐귩믁뷉궠궧귡
//! @param id 릐궻긢??붥뜂
//! @attention 댷벍긬긚궸듫귦귞궦갂럚믦궢궫릐귉궻믁뷉귩떗맕궢귏궥갃Init()듫릶귩띋뱗렳뛱궥귡귏궳뙰궸뽣궧귏궧귪갃
void AIcontrol::SetHoldTracking(int id)
{
	if( (0 <= id)&&(id < MAX_HUMAN) ){ MoveNavi->SetHoldTracking(id); }
}

//! @brief 떗맕밒궸똸둀궠궧귡
//! @warning 뾆먩밒궶몪귟 귩렳뛱뭷궻뤾뜃갂궞궻듫릶궼돺귖궢귏궧귪갃
void AIcontrol::SetCautionMode()
{
	//뾆먩밒궶몪귟궶귞궽돺귖궢궶궋
	if( MoveNavi->GetRun2() == true ){ return; }

	if( battlemode == AI_NORMAL ){
		cautionback_posx = posx;
		cautionback_posz = posz;
	}
	battlemode = AI_CAUTION;
	cautioncnt = 160;
	FaceCaution_flag = false;
	FaceCaution_rx = 0.0f;
}

//! @brief 뷄먰벉돸긲깋긐귩먠믦
//! @param flag true갌먰벉귩뛱귦궶궋걁뷄먰벉돸걂?false갌먰벉귩뛱궎걁믅륂걂
//! @attention 긲깋긐귩뾎뚼궸궥귡궴밎귩봃렞궢궶궘궶귟귏궥갃
void AIcontrol::SetNoFightFlag(bool flag)
{
	NoFight = flag;
}

void AIcontrol::SetForceNoAI(bool flag)
{
	ForceNoAI = flag;

	if (ForceNoAI == true) {
		NoFight = true;
		battlemode = AI_NORMAL;
		enemyhuman = NULL;
		crouchcnt = 0;

		if (ctrlhuman != NULL) {
			ctrlhuman->SetCrouchFlag(false);
		}

		if (ObjDriver != NULL) {
			ObjDriver->Init();
		}
	}
}

bool AIcontrol::GetForceNoAI()
{
	return ForceNoAI;
}

//! @brief 먰벉긾?긤롦벦
//! @param *mode 먰벉긾?긤걁릶럻걂귩롷궚롦귡?귽깛?
//! @param *modestr 먰벉긾?긤뼹궻빒럻쀱귩롷궚롦귡?귽깛?
void AIcontrol::GetBattleMode(int *mode, char *modestr)
{
	if( mode != NULL ){
		*mode = battlemode;
	}
	if( modestr != NULL ){
		switch(battlemode){
			case 0: strcpy(modestr, "AI_DEAD"); break;
			case 1: strcpy(modestr, "AI_ACTION"); break;
			case 2: strcpy(modestr, "AI_CAUTION"); break;
			case 3: strcpy(modestr, "AI_NORMAL"); break;
			default: strcpy(modestr, "");
		}
	}
}

//! @brief 뛘똼뫮뤭궻릐긢??붥뜂
//! @return 릐긢??붥뜂걁??긒긞긣궕궋궶궋뤾뜃궼 -1걂
int AIcontrol::GetEnemyHumanID()
{
	if( enemyhuman == NULL ){
		return -1;
	}
	//else{
		return ObjMgr->GetHumanObjectID(enemyhuman);
	//}
}

//! @brief 댷벍궥귡뽞뷭뭤?걁??긒긞긣걂귩롦벦
//! @param posx 뽞뷭뭤?궻X띆뷭귩롷궚롦귡?귽깛?
//! @param posz 뽞뷭뭤?궻Z띆뷭귩롷궚롦귡?귽깛?
//! @param movemode 뽞뷭뭤?귉궻댷벍긾?긤귩롷궚롦귡?귽깛?
void AIcontrol::GetMoveTargetPos(float *posx, float *posz, int *movemode)
{
	MoveNavi->GetTargetPos(posx, posz, NULL, movemode, NULL);

	//걏뾆먩밒궶몪귟걐궳궼궶궋?뛘똼뭷갋똸둀뭷궶귞궽갂똸둀뚣궸뽣귡댧뭫귩뺅궥갃
	if( MoveNavi->GetRun2() == false ){
		if( (battlemode == AI_ACTION)||(battlemode == AI_CAUTION) ){
			*posx = cautionback_posx;
			*posz = cautionback_posz;
		}
	}
}

//! @brief 뙸띪벶귒뜛귪궳궋귡?귽깛긣긢??긬긚
//! @param out_data ?귽깛긣긢??귩롷궚롦귡?귽깛?
void AIcontrol::GetPathPointData(pointdata *out_data)
{
	MoveNavi->GetPathPointData(out_data);
}

//! @brief 룉뿚똭듫릶
void AIcontrol::Process()
{
	//긏깋긚먠믦궕궓궔궢궚귢궽룉뿚궢궶궋
	if( ctrlhuman == NULL ){ return; }
	if( blocks == NULL ){ return; }
	if( CollD == NULL ){ return; }

	//뼰뚼궶릐긏깋긚궶귞룉뿚궢궶궋
	if( ctrlhuman->GetEnableFlag() == false ){ return; }

	//?뻊궢궫귞
	if( ctrlhuman->GetHP() <= 0 ){
		battlemode = AI_DEAD;
		return;
	}

	// 표 기준 AI 없음 인물은 완전히 정지시킨다.
	if (ForceNoAI == true) {
		battlemode = AI_NORMAL;
		enemyhuman = NULL;
		crouchcnt = 0;
		ctrlhuman->SetCrouchFlag(false);

		if (ObjDriver != NULL) {
			ObjDriver->Init();
		}

		return;
	}

	int human_id;
	ctrlhuman->GetParamData(&human_id, NULL, NULL, NULL);

	/*
	// 주의:
	// 이 처리를 유지하면 맵 텍스처 0번 더미가 전부 AI 없음이 된다.
	// 표 기준으로 AI 존재/AI 없음까지 나누려면 이 조건은 제거하거나 약화하는 것이 좋다.
	if (human_id == ID_HUMAN_WHITE_DUMMY) {
		battlemode = AI_NORMAL;
		CancelMoveTurn();
		return;
	}
	*/

	//HP궕0궳궶궋궻궸 ?뻊궢궫궞궴궸궶궯궲귡?갋갋갋맯궖뺅궯궫갎
	if( battlemode == AI_DEAD ){
		battlemode = AI_NORMAL;
		MoveNavi->MovePathNowState();
	}

	//띆뷭궴둷뱗귩롦벦
	ctrlhuman->GetPosData(&posx, &posy, &posz, NULL);
	ctrlhuman->GetRxRy(&rx, &ry);

	// AI 전용 이동속도 보정은 매 프레임 기본값으로 되돌린다.
// 단, 플레이어 스킬로 소환된 동료는 AI FireRate 이동속도 보정과 관계없이 고정 2배를 유지한다.
	if (ctrlhuman->GetSkillSummonedAllyFlag() == true) {
		ctrlhuman->SpeedMultiplier = 2.0f;
	}
	else {
		ctrlhuman->SpeedMultiplier = 1.0f;
	}

	//깋깛??궸벍띿귩?귕귡
	CancelMoveTurn();

	//뛘똼뭷궔똸둀뭷궶귞궽
	if( (battlemode == AI_ACTION)||(battlemode == AI_CAUTION) ){
		//븧딇귩렃궰
		HaveWeapon();
	}

	//롥똶럁렳뛱
	if( battlemode == AI_ACTION ){			//뛘똼뭷
		ActionMain();
	}
	else if( battlemode == AI_CAUTION ){	//똸둀뭷
		CautionMain();
	}
	else{									//빟륂렄
		NormalMain();
	}

	// AI 앉기 제어
// 이동 플래그가 결정된 뒤, 실제 이동 처리 전에 실행한다.
	ControlCrouch();

	//댷벍갋뺴뛀?듂룉뿚
	ObjDriver->ControlObject();

	//븧딇귩롦귟댌궋
	ControlWeapon();

	// [✨ 추가 ✨] AI의 반동 효과를 제거합니다.
	// 이 프레임에서 발생한 모든 반동 값을 강제로 0으로 초기화합니다.
	ctrlhuman->SetRecoilYOffset(0.0f);
}

//! @brief 긓깛긚긣깋긏?
AIMoveNavi::AIMoveNavi(class ObjectManager *in_ObjMgr, int in_ctrlid, class PointDataInterface *in_Points)
{
	ObjMgr = in_ObjMgr;
	ctrlid = in_ctrlid;
	Points = in_Points;

	movemode = AI_WAIT;
	hold = false;
	path_pointid = 0;
	target_humanid = -1;
	target_posx = 0.0f;
	target_posz = 0.0f;
	target_rx = 0.0f;
}

//! @brief 긢귻긚긣깋긏?
AIMoveNavi::~AIMoveNavi()
{}

//! @brief 뫮뤭긏깋긚귩먠믦
//! @attention 궞궻듫릶궳먠믦귩뛱귦궶궋궴갂긏깋긚렔뫬궕맫궢궘??궢귏궧귪갃
void AIMoveNavi::SetClass(class ObjectManager *in_ObjMgr, int in_ctrlid, class PointDataInterface *in_Points)
{
	ObjMgr = in_ObjMgr;
	ctrlid = in_ctrlid;
	Points = in_Points;
}

//! @brief 룊딖돸똭듫릶
void AIMoveNavi::Init()
{
	movemode = AI_NULL;
	hold = false;
	path_pointid = 0;
	target_humanid = -1;

	ObjMgr->GetHumanObject(ctrlid)->GetParamData(NULL, &path_pointid, NULL, NULL);
	MovePathNextState();
}

//! @brief 댷벍궥귡뽞뷭뭤?귩밙뾭
//! @return 뒶뿹갌true?렪봲갌false
//! @attention 믁뷉렄궸뫮뤭릐븿궻띆뷭귩뵿뎕궠궧귡궫귕갂?듫릶귩뻽긲깒??뚁귂뢯궢궲갂띍륷궻댧뭫륃뺪귩똶럁궥귡뷠뾴궕궇귟귏궥갃
bool AIMoveNavi::MovePathNowState()
{
	if( hold == true ){
		if( movemode == AI_TRACKING ){
			class human *targethuman;
			targethuman = ObjMgr->GetHumanObject(target_humanid);
			targethuman->GetPosData(&target_posx, NULL, &target_posz, &target_rx);
		}
		return true;
	}


	//?귽깛긣궻륃뺪귩롦벦
	pointdata pdata;
	if( Points->Getdata(&pdata, path_pointid) != 0 ){
		movemode = AI_NULL;
		return false;
	}

	//댷벍긬긚궶귞?
	if( pdata.p1 == POINT_P1TYPE_AIPATH ){
		//댷벍긚긡?긣먠믦
		switch(pdata.p2){
			case 0: movemode = AI_WALK; break;
			case 1: movemode = AI_RUN; break;
			case 2: movemode = AI_WAIT; break;
			case 3: movemode = AI_TRACKING; break;
			case 4: movemode = AI_WAIT; break;
			case 5: movemode = AI_STOP_5SEC; break;
			case 6: movemode = AI_GRENADE; break;
			case 7: movemode = AI_RUN2; break;
			default: break;
		}

		if( movemode == AI_TRACKING ){
			class human *targethuman;

			if( target_humanid == -1 ){
				signed short int nextpointp4 = pdata.p3;

				//?귽깛긣걁릐걂궻륃뺪귩롦벦
				if( Points->SearchPointdata(&pdata, 0x08, 0, 0, 0, nextpointp4, 0) == 0 ){
					return false;
				}

				//릐귩뙚랊궢궲긏깋긚귩롦벦
				targethuman = ObjMgr->SearchHuman(pdata.p4);
				if( targethuman == NULL ){ return false; }

				//릐궻긢??붥뜂귩롦벦
				target_humanid = ObjMgr->GetHumanObjectID(targethuman);
			}

			targethuman = ObjMgr->GetHumanObject(target_humanid);
			targethuman->GetPosData(&target_posx, NULL, &target_posz, &target_rx);
		}
		else{
			//륃뺪밙뾭
			target_posx = pdata.x;
			target_posz = pdata.z;
			target_rx = pdata.r;
		}

		return true;
	}

	//깋깛??긬긚궶귞
	if( pdata.p1 == POINT_P1TYPE_RAND_AIPATH ){
		movemode = AI_RANDOM;
		return false;
	}

	movemode = AI_NULL;
	return false;
}

//! @brief 렅궻뽞뷭뭤?귩뙚랊
//! @return 뒶뿹갌true?렪봲갌false
bool AIMoveNavi::MovePathNextState()
{
	//?귽깛긣궻륃뺪귩롦벦
	pointdata pdata;
	if( Points->Getdata(&pdata, path_pointid) != 0 ){
		movemode = AI_NULL;
		return false;
	}

	signed short int nextpointp4 = pdata.p3;

	//깋깛??긬긚룉뿚
	if( pdata.p1 == POINT_P1TYPE_RAND_AIPATH ){
		if( GetRand(2) == 0 ){
			nextpointp4 = pdata.p2;
		}
		else{
			nextpointp4 = pdata.p3;
		}
		movemode = AI_RANDOM;
	}

	//?귽깛긣귩뙚랊
	if( Points->SearchPointdata(&pdata, 0x08, 0, 0, 0, nextpointp4, 0) == 0 ){
		return false;
	}

	path_pointid = pdata.id;

	return true;
}

//! @brief 럚믦궢궫뤾룋귉뫲?궠궧귡
//! @param px X띆뷭
//! @param pz Z띆뷭
//! @param rx 뢣럨궥귡뛀궖
//! @attention 댷벍긬긚궸듫귦귞궦갂럚믦궢궫띆뷭귉궻뫲?귩떗맕궢귏궥갃Init()듫릶귩띋뱗렳뛱궥귡귏궳뙰궸뽣궧귏궧귪갃
void AIMoveNavi::SetHoldWait(float px, float pz, float rx)
{
	movemode = AI_WAIT;
	hold = true;
	target_posx = px;
	target_posz = pz;
	target_rx = rx;
}

//! @brief 럚믦궢궫릐귩믁뷉궠궧귡
//! @param id 릐궻긢??붥뜂
//! @attention 댷벍긬긚궸듫귦귞궦갂럚믦궢궫릐귉궻믁뷉귩떗맕궢귏궥갃Init()듫릶귩띋뱗렳뛱궥귡귏궳뙰궸뽣궧귏궧귪갃
void AIMoveNavi::SetHoldTracking(int id)
{
	if( (id < 0)||(MAX_HUMAN <= id) ){ return; }

	movemode = AI_TRACKING;
	hold = true;
	target_humanid = id;
}

//! @brief 댷벍긾?긤귩롦벦
//! @return 댷벍긾?긤
int AIMoveNavi::GetMoveMode()
{
	return movemode;
}

//! @brief 뾆먩밒궶몪귟긲깋긐귩롦벦
//! @return 뾆먩밒궶몪귟궳궇귡갌true?뾆먩밒궶몪귟궳궶궋갌false
bool AIMoveNavi::GetRun2()
{
	if( movemode == AI_RUN2 ){
		return true;
	}
	//else{
		return false;
	//}
}

//! @brief ??긒긞긣걁릐걂궻긢??붥뜂귩롦벦
//! @return 릐궻긢??붥뜂
int AIMoveNavi::GetTargetHumanID()
{
	return target_humanid;
}

//! @brief 뙸띪벶귒뜛귪궳궋귡?귽깛긣긢??긬긚
//! @param out_data ?귽깛긣긢??귩롷궚롦귡?귽깛?
void AIMoveNavi::GetPathPointData(pointdata *out_data)
{
	Points->Getdata(out_data, path_pointid);
}

//! @brief 댷벍궥귡뽞뷭뭤?걁??긒긞긣걂귩롦벦
//! @param posx 뽞뷭뭤?궻X띆뷭귩롷궚롦귡?귽깛?
//! @param posz 뽞뷭뭤?궻Z띆뷭귩롷궚롦귡?귽깛?
//! @param rx 뽞뷭뭤?궻릣빟둷뱗귩롷궚롦귡?귽깛?
//! @param out_movemode 뽞뷭뭤?귉궻댷벍긾?긤귩롷궚롦귡?귽깛?
//! @param out_pointmode 뽞뷭뭤?궻?귽깛긣긾?긤귩롷궚롦귡?귽깛?
//! @warning ?듫릶궳궼띆뷭궼뛛륷궠귢귏궧귪갃먩궸 MovePathNowState() 듫릶궳띆뷭귩뛛륷궢궫뚣갂?듫릶귩뚁귂뢯궢궲궘궬궠궋갃
void AIMoveNavi::GetTargetPos(float *posx, float *posz, float *rx, int *out_movemode, int *out_pointmode)
{
	if( posx != NULL ){ *posx = target_posx; }
	if( posz != NULL ){ *posz = target_posz; }
	if( rx != NULL ){ *rx = target_rx; }

	if( out_movemode != NULL ){
		if( (movemode == AI_WALK)||(movemode == AI_WAIT)||(movemode == AI_STOP_5SEC) ){
			*out_movemode = AI_NAVI_MOVE_WALK;
		}
		else if( movemode == AI_RUN ){
			*out_movemode = AI_NAVI_MOVE_RUN;
		}
		else if( movemode == AI_RUN2 ){
			*out_movemode = AI_NAVI_MOVE_RUN2;
		}
		else if( movemode == AI_TRACKING ){
			*out_movemode = AI_NAVI_MOVE_TRACKING;
		}
		else{
			*out_movemode = AI_NAVI_MOVE_NULL;
		}
	}

	if( out_pointmode != NULL ){
		if( movemode == AI_WAIT ){
			*out_pointmode = AI_NAVI_POINT_WAIT;
		}
		else if( movemode == AI_STOP_5SEC ){
			*out_pointmode = AI_NAVI_POINT_STOP_5SEC;
		}
		else if( movemode == AI_TRACKING ){
			*out_pointmode = AI_NAVI_POINT_TRACKING;
		}
		else if( movemode == AI_GRENADE ){	//롨왮뭙긬긚
			*out_pointmode = AI_NAVI_POINT_GRENADE;
		}
		else{								//댷벍긬긚갋댶륂궶긬긚
			*out_pointmode = AI_NAVI_POINT_NULL;
		}
	}
}

//! @brief 긓깛긚긣깋긏?
AIObjectDriver::AIObjectDriver(class ObjectManager *in_ObjMgr, int in_ctrlid)
{
	ObjMgr = in_ObjMgr;
	ctrlid = in_ctrlid;

	moveturn_mode = 0;
	addrx = 0.0f;
	addry = 0.0f;
}

//! @brief 긢귻긚긣깋긏?
AIObjectDriver::~AIObjectDriver()
{}

//! @brief 뫮뤭긏깋긚귩먠믦
//! @attention 궞궻듫릶궳먠믦귩뛱귦궶궋궴갂긏깋긚렔뫬궕맫궢궘??궢귏궧귪갃
void AIObjectDriver::SetClass(class ObjectManager *in_ObjMgr, int in_ctrlid)
{
	ObjMgr = in_ObjMgr;
	ctrlid = in_ctrlid;
}

//! @brief 룊딖돸똭듫릶
void AIObjectDriver::Init()
{
	addrx = 0.0f;
	addry = 0.0f;

	ResetMode();
}

//! @brief 댷벍됷?맕뚥긲깋긐귩긏깏귺걁덇뱗멣궲궻벍궖귩?귕귡걂
void AIObjectDriver::ResetMode()
{
	moveturn_mode = 0;
}

//! @brief 댷벍됷?맕뚥긲깋긐 먠믦
//! @param flag ?띿긾?긤귩?궥믦릶걁AIcontrolFlag쀱땽?걂
void AIObjectDriver::SetModeFlag(int flag)
{
	SetFlag(moveturn_mode, flag);
}

//! @brief 댷벍됷?맕뚥긲깋긐 됶룣
//! @param flag ?띿긾?긤귩?궥믦릶걁AIcontrolFlag쀱땽?걂
void AIObjectDriver::DelModeFlag(int flag)
{
	DelFlag(moveturn_mode, flag);
}

//! @brief 댷벍됷?맕뚥긲깋긐 롦벦
//! @param flag ?띿긾?긤귩?궥믦릶걁AIcontrolFlag쀱땽?걂
bool AIObjectDriver::GetModeFlag(int flag)
{
	if( GetFlag(moveturn_mode, flag) == 0 ){
		return false;
	}
	//else{
		return true;
	//}
}

//! @brief 댷벍됷?맕뚥귩렳뛱
//! @attention 뻽긲깒??1됷궬궚뚁귂뢯궢궲궘궬궠궋갃
void AIObjectDriver::ControlObject()
{
	float rx, ry;

	// 이동 실행
	if (GetFlag(moveturn_mode, AI_CTRL_MOVEFORWARD)) {
		ObjMgr->MoveForward(ctrlid);
	}
	if (GetFlag(moveturn_mode, AI_CTRL_MOVEBACKWARD)) {
		ObjMgr->MoveBack(ctrlid);
	}
	if (GetFlag(moveturn_mode, AI_CTRL_MOVELEFT)) {
		ObjMgr->MoveLeft(ctrlid);
	}
	if (GetFlag(moveturn_mode, AI_CTRL_MOVERIGHT)) {
		ObjMgr->MoveRight(ctrlid);
	}
	if (GetFlag(moveturn_mode, AI_CTRL_MOVEWALK)) {
		ObjMgr->MoveWalk(ctrlid);
	}

	// AI Reaction Speed 적용
	float reaction_mult = GameConfig.GetAIReactionSpeedMultiplier();

	if (reaction_mult < 0.0f) {
		reaction_mult = 0.0f;
	}

	if (reaction_mult > 2.0f) {
		reaction_mult = 2.0f;
	}

	float base_turn_boost = 1.30f;

	float turnrad = AI_ADDTURNRAD * base_turn_boost * reaction_mult;

	// 방향 전환 실행
	if (GetFlag(moveturn_mode, AI_CTRL_TURNUP)) {
		addry += turnrad;
	}
	if (GetFlag(moveturn_mode, AI_CTRL_TURNDOWN)) {
		addry -= turnrad;
	}
	if (GetFlag(moveturn_mode, AI_CTRL_TURNLEFT)) {
		addrx -= turnrad;
	}
	if (GetFlag(moveturn_mode, AI_CTRL_TURNRIGHT)) {
		addrx += turnrad;
	}

	// 너무 빠른 누적 회전 방지
	float maxturn = DegreeToRadian(4.5f) * reaction_mult;
	if (addrx > maxturn) { addrx = maxturn; }
	if (addrx < -maxturn) { addrx = -maxturn; }
	if (addry > maxturn) { addry = maxturn; }
	if (addry < -maxturn) { addry = -maxturn; }

	// 각도 취득
	ObjMgr->GetHumanObject(ctrlid)->GetRxRy(&rx, &ry);

	// 각도에 가산
	rx += addrx;
	ry += addry;

	// 상하 회전 제한
	if (ry > DegreeToRadian(70)) { ry = DegreeToRadian(70); }
	if (ry < DegreeToRadian(-70)) { ry = DegreeToRadian(-70); }

	// 각도 설정
	ObjMgr->GetHumanObject(ctrlid)->SetRxRy(rx, ry);

	// 회전 속도 감쇠
	addrx *= 0.65f;
	addry *= 0.65f;

	// 미세 떨림 제거
	if (fabsf(addrx) < DegreeToRadian(0.35f)) { addrx = 0.0f; }
	if (fabsf(addry) < DegreeToRadian(0.35f)) { addry = 0.0f; }
}