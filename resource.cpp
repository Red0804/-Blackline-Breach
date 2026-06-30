//! @file resource.cpp
//! @brief ResourceManager긏깋긚궻믦?

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

#include "resource.h"

//! @brief 긓깛긚긣깋긏?
ResourceManager::ResourceManager()
{
	ParamInfo = NULL;
	d3dg = NULL;
	SoundCtrl = NULL;

	for(int i=0; i<TOTAL_UPMODE; i++){
		human_upmodel[i] = -1;
	}
	for(int i=0; i<TOTAL_ARMMODE; i++){
		human_armmodel[i] = -1;
	}
	human_legmodel = -1;
	for (int i = 0; i < TOTAL_WALKMODE; i++) {
		human_walkmodel[i] = -1;
	}
	for (int i = 0; i < TOTAL_RUNMODE; i++) {
		human_runmodel[i] = -1;
	}

	human_sitlegmodel = -1;
	for (int i = 0; i < TOTAL_SITWALKMODE; i++) {
		human_sitwalkmodel[i] = -1;
	}
	for(int i=0; i<MAX_LOADHUMANTEXTURE; i++){
		human_texture_Param[i] = -1;
		human_texture_d3dg[i] = -1;
	}
	for(int i=0; i<TOTAL_PARAMETERINFO_WEAPON; i++){
		weapon_model[i] = -1;
		weapon_texture[i] = -1;
		weapon_sound[i] = -1;
	}
	weapon_reloadsound = -1;
	for (int i = 0; i < TOTAL_RELOAD_SOUND; i++) {
		weapon_reloadsound_group[i] = -1;
	}
	for(int i=0; i<(TOTAL_PARAMETERINFO_SMALLOBJECT + MAX_ADDSMALLOBJECT); i++){
		smallobject_model[i] = -1;
		smallobject_texture[i] = -1;
		smallobject_sound[i] = -1;
	}
	for(int i=0; i<TOTAL_PARAMETERINFO_BULLET; i++){
		bullet_model[i] = -1;
		bullet_texture[i] = -1;
	}

	scopetexture = -1;
	skymodel = -1;
	skytexture = -1;
	bullet_hitsoundA = -1;
	bullet_hitsoundB = -1;
	bullet_humanhitsound = -1;
	bullet_passingsound = -1;
	grenade_bang = -1;
	grenade_cco = -1;
	effecttexture_blood = -1;
	effecttexture_mflash = -1;
	effecttexture_smoke = -1;
	effecttexture_yakkyou = -1;

	sound_nvg = -1;             // [✨ 추가] -1로 초기화

	for (int i = 0; i < TOTAL_SKILL_SOUND; i++) {
		skill_sound[i] = -1;
	}

	for (int i = 0; i < TOTAL_SKILL_ICON; i++) {
		skillicon_texture[i] = -1;
	}

	for (int i = 0; i < 10; i++) {
		footstep_sound[i] = -1;
	}
}

//! @brief 긢귻긚긣깋긏?
ResourceManager::~ResourceManager()
{
	DestroyResource();
}

//! @brief 랷뤖궥귡ParameterInfo긏깋긚귩먠믦
//! @param *_ParamInfo ParameterInfo긏깋긚궻?귽깛?
//! @attention 궞궻듫릶궳먠믦귩뛱귦궶궋궴갂긏깋긚렔뫬궕맫궢궘??궢귏궧귪갃
void ResourceManager::SetParameterInfo(ParameterInfo *_ParamInfo)
{
	ParamInfo = _ParamInfo;
}

//! @brief 랷뤖궥귡D3DGraphics긏깋긚귩먠믦
//! @param *_d3dg D3DGraphics긏깋긚궻?귽깛?
//! @attention 궞궻듫릶궳먠믦귩뛱귦궶궋궴갂긏깋긚렔뫬궕맫궢궘??궢귏궧귪갃
void ResourceManager::SetD3DGraphics(D3DGraphics *_d3dg)
{
	d3dg = _d3dg;
}

//! @brief 랷뤖궥귡SoundControl긏깋긚귩먠믦
//! @param *_SoundCtrl SoundControl긏깋긚궻?귽깛?
//! @attention 궞궻듫릶궳먠믦귩뛱귦궶궋궴갂긏깋긚렔뫬궕맫궢궘??궢귏궧귪갃
void ResourceManager::SetSoundControl(SoundControl *_SoundCtrl)
{
	SoundCtrl = _SoundCtrl;
}

//! @brief 됶뺳
//! @attention ?듫릶궼렔벍밒궸뚁귂뢯궠귢귏궥궕갂뼻렑밒궸뚁귂뢯궥궞궴귖됀?궳궥갃
void ResourceManager::DestroyResource()
{
	CleanupHumanModel();
	CleanupHumanTexture();
	CleanupWeaponModelTexture();
	CleanupWeaponSound();
	CleanupSmallObjectModelTexture();
	CleanupSmallObjectSound();

	CleanupSkillSound();
	CleanupSkillIconTexture();

	CleanupFootstepSound();
	CleanupScopeTexture();
	CleanupSkyModelTexture();
	CleanupBulletModelTexture();
	CleanupBulletSound();
	CleanupEffectTexture();
	CleanupNVGSound();      // [✨ 추가]
}

//! @brief 릐궻긾긢깑귩벶귒뜛귒
//! @return 맟뚻갌0?렪봲갌1
int ResourceManager::LoadHumanModel()
{
	if( d3dg == NULL ){ return 1; }

	human_upmodel[0] = d3dg->LoadModel("data\\model\\up0.x");
	human_upmodel[1] = d3dg->LoadModel("data\\model\\up1.x");
	human_upmodel[2] = d3dg->LoadModel("data\\model\\up2.x");
	human_upmodel[3] = d3dg->LoadModel("data\\model\\up3.x");
	human_upmodel[4] = d3dg->LoadModel("data\\model\\up4.x");
	human_upmodel[5] = d3dg->LoadModel("data\\model\\up5.x");
	human_armmodel[0] = d3dg->LoadModel("data\\model\\arm00.x");
	human_armmodel[1] = d3dg->LoadModel("data\\model\\arm01.x");
	human_armmodel[2] = d3dg->LoadModel("data\\model\\arm02.x");
	human_armmodel[3] = d3dg->LoadModel("data\\model\\arm_shield.x");
	human_legmodel = d3dg->LoadModel("data\\model\\leg.x");

	human_sitlegmodel = d3dg->LoadModel("data\\model\\sitleg.x");
	human_sitwalkmodel[0] = d3dg->LoadModel("data\\model\\sitwalkleft1.x");
	human_sitwalkmodel[1] = d3dg->LoadModel("data\\model\\sitwalkleft2.x");
	human_sitwalkmodel[2] = d3dg->LoadModel("data\\model\\sitwalkright1.x");
	human_sitwalkmodel[3] = d3dg->LoadModel("data\\model\\sitwalkright2.x");

	human_walkmodel[0] = d3dg->LoadModel("data\\model\\walk01.x");
	human_walkmodel[2] = d3dg->LoadModel("data\\model\\walk02.x");
	human_walkmodel[4] = d3dg->LoadModel("data\\model\\walk03.x");
	human_walkmodel[6] = d3dg->LoadModel("data\\model\\walk04.x");
	human_runmodel[0] = d3dg->LoadModel("data\\model\\run01.x");
	human_runmodel[2] = d3dg->LoadModel("data\\model\\run02.x");
	human_runmodel[4] = d3dg->LoadModel("data\\model\\run03.x");
	human_runmodel[6] = d3dg->LoadModel("data\\model\\run04.x");
	human_runmodel[8] = d3dg->LoadModel("data\\model\\run05.x");
	human_runmodel[10] = d3dg->LoadModel("data\\model\\run06.x");

#ifdef ENABLE_HUMANMODEL_MORPHING
	//긾?긲귻깛긐룉뿚귩렳뛱궥귡
	human_walkmodel[1] = d3dg->MorphingModel(human_walkmodel[0], human_walkmodel[2]);
	human_walkmodel[3] = d3dg->MorphingModel(human_walkmodel[2], human_walkmodel[4]);
	human_walkmodel[5] = d3dg->MorphingModel(human_walkmodel[4], human_walkmodel[6]);
	human_walkmodel[7] = d3dg->MorphingModel(human_walkmodel[6], human_walkmodel[0]);
	human_runmodel[1] = d3dg->MorphingModel(human_runmodel[0], human_runmodel[2]);
	human_runmodel[3] = d3dg->MorphingModel(human_runmodel[2], human_runmodel[4]);
	human_runmodel[5] = d3dg->MorphingModel(human_runmodel[4], human_runmodel[6]);
	human_runmodel[7] = d3dg->MorphingModel(human_runmodel[6], human_runmodel[8]);
	human_runmodel[9] = d3dg->MorphingModel(human_runmodel[8], human_runmodel[10]);
	human_runmodel[11] = d3dg->MorphingModel(human_runmodel[10], human_runmodel[0]);
#else
	//긾?긲귻깛긐룉뿚귩렳뛱궢궶궋
	human_walkmodel[1] = human_walkmodel[0];
	human_walkmodel[3] = human_walkmodel[2];
	human_walkmodel[5] = human_walkmodel[4];
	human_walkmodel[7] = human_walkmodel[6];
	human_runmodel[1] = human_runmodel[0];
	human_runmodel[3] = human_runmodel[2];
	human_runmodel[5] = human_runmodel[4];
	human_runmodel[7] = human_runmodel[6];
	human_runmodel[9] = human_runmodel[8];
	human_runmodel[11] = human_runmodel[10];
#endif

	return 0;
}

//! @brief 릐궻긾긢깑궻긢??붥뜂귩롦벦
//! @param out_upmodel[] 뤵뵾릆궻긾긢깑?걁봹쀱릶갌TOTAL_UPMODE걂
//! @param out_armmodel[] 쁱궻긾긢깑?걁봹쀱릶갌TOTAL_ARMMODE걂
//! @param *legmodel 뫉걁먄?륉뫴걂궻긾긢깑궻?귽깛?
//! @param out_walkmodel[] 뫉걁뺖궖걂궻긾긢깑?걁봹쀱릶갌TOTAL_WALKMODE걂
//! @param out_runmodel[] 뫉걁몪귟걂궻긾긢깑?걁봹쀱릶갌TOTAL_RUNMODE걂
void ResourceManager::GetHumanModel(int out_upmodel[], int out_armmodel[], int* legmodel, int out_walkmodel[], int out_runmodel[])
{
	for (int i = 0; i < TOTAL_UPMODE; i++) {
		out_upmodel[i] = human_upmodel[i];
	}
	for (int i = 0; i < TOTAL_ARMMODE; i++) {
		out_armmodel[i] = human_armmodel[i];
	}
	*legmodel = human_legmodel;
	for (int i = 0; i < TOTAL_WALKMODE; i++) {
		out_walkmodel[i] = human_walkmodel[i];
	}
	for (int i = 0; i < TOTAL_RUNMODE; i++) {
		out_runmodel[i] = human_runmodel[i];
	}
}

void ResourceManager::GetHumanSitModel(int* sitlegmodel, int out_sitwalkmodel[])
{
	if (sitlegmodel != NULL) {
		*sitlegmodel = human_sitlegmodel;
	}
	for (int i = 0; i < TOTAL_SITWALKMODE; i++) {
		out_sitwalkmodel[i] = human_sitwalkmodel[i];
	}
}

//! @brief 릐궻긾긢깑귩됶뺳
void ResourceManager::CleanupHumanModel()
{
	if( d3dg == NULL ){ return; }

	for(int i=0; i<TOTAL_UPMODE; i++){
		d3dg->CleanupModel(human_upmodel[i]);
		human_upmodel[i] = -1;
	}
	for(int i=0; i<TOTAL_ARMMODE; i++){
		d3dg->CleanupModel(human_armmodel[i]);
		human_armmodel[i] = -1;
	}
	d3dg->CleanupModel(human_legmodel);
	human_legmodel = -1;

	d3dg->CleanupModel(human_sitlegmodel);
	human_sitlegmodel = -1;
	for (int i = 0; i < TOTAL_SITWALKMODE; i++) {
		d3dg->CleanupModel(human_sitwalkmodel[i]);
		human_sitwalkmodel[i] = -1;
	}

	for (int i = 0; i < TOTAL_WALKMODE; i++) {
		d3dg->CleanupModel(human_walkmodel[i]);
		human_walkmodel[i] = -1;
	}
	for(int i=0; i<TOTAL_RUNMODE; i++){
		d3dg->CleanupModel(human_runmodel[i]);
		human_runmodel[i] = -1;
	}
}

//! @brief 릐궻긡긏긚?긿귩벶귒뜛귔
//! @param id 릐궻롰쀞ID
//! @return 맟뚻갌0댥뤵?렪봲갌-1
int ResourceManager::AddHumanTexture(int id)
{
	if( d3dg == NULL ){ return -1; }
	if( (id < 0)||(TOTAL_PARAMETERINFO_HUMAN <= id ) ){ return -1; }

	HumanParameter data;
	char path[MAX_PATH];
	if( ParamInfo->GetHuman(id, &data) == 1 ){ return -1; }

	//딓궸벶귒뜛귪궬긡긏긚?긿궔궵궎궔뮧귊귡
	for(int i=0; i<MAX_LOADHUMANTEXTURE; i++){
		if( human_texture_Param[i] == data.texture ){
			return -1;
		}
	}

	//긡긏긚?긿귩벶귒뜛귔
	for(int i=0; i<MAX_LOADHUMANTEXTURE; i++){
		if( human_texture_Param[i] == -1 ){
			int TexturePathID = data.texture;
			if( ParamInfo->GetHumanTexturePath(TexturePathID, path) == 1 ){ return -1; }
			human_texture_Param[i] = TexturePathID;				//긢??붥뜂귩딯?
			human_texture_d3dg[i] = d3dg->LoadTexture(path, false, false);	//긡긏긚?긿귩벶귒뜛귔
			return i;
		}
	}

	return -1;
}

//! @brief 벶귒뜛귪궳궋귡릐궻긡긏긚?긿릶귩롦벦
//! @return 긡긏긚?긿릶걁띍묈 MAX_LOADHUMANTEXTURE걂
int ResourceManager::GetHumanTextures()
{
	int Textures = 0;

	for(int i=0; i<MAX_LOADHUMANTEXTURE; i++){
		if( human_texture_Param[i] != -1 ){
			Textures += 1;
		}
	}

	return Textures;
}

//! @brief 릐궻긡긏긚?긿붥뜂귩롦벦
//! @param id 릐궻롰쀞ID
//! @return 맟뚻갌긡긏긚?긿ID?렪봲갌-1
int ResourceManager::GetHumanTexture(int id)
{
	if ((id < 0) || (TOTAL_PARAMETERINFO_HUMAN <= id)) { return -1; }

	HumanParameter data;
	if (ParamInfo->GetHuman(id, &data) == 1) { return -1; }

	for (int i = 0; i < MAX_LOADHUMANTEXTURE; i++) {
		if (human_texture_Param[i] == data.texture) {
			return human_texture_d3dg[i];
		}
	}

	return -1;
}

int ResourceManager::AddHumanTextureByTextureID(int texture_id)
{
	if (d3dg == NULL) { return -1; }
	if ((texture_id < 0) || (TOTAL_HUMANTEXTURE <= texture_id)) { return -1; }

	for (int i = 0; i < MAX_LOADHUMANTEXTURE; i++) {
		if (human_texture_Param[i] == texture_id) {
			return -1;
		}
	}

	for (int i = 0; i < MAX_LOADHUMANTEXTURE; i++) {
		if (human_texture_Param[i] == -1) {
			char path[MAX_PATH];

			if (ParamInfo->GetHumanTexturePath(texture_id, path) == 1) {
				return -1;
			}

			human_texture_Param[i] = texture_id;
			human_texture_d3dg[i] = d3dg->LoadTexture(path, false, false);

			return i;
		}
	}

	return -1;
}

int ResourceManager::GetHumanTextureByTextureID(int texture_id)
{
	if ((texture_id < 0) || (TOTAL_HUMANTEXTURE <= texture_id)) {
		return -1;
	}

	for (int i = 0; i < MAX_LOADHUMANTEXTURE; i++) {
		if (human_texture_Param[i] == texture_id) {
			return human_texture_d3dg[i];
		}
	}

	return -1;
}

//! @brief 벶귒뜛귪궳궋귡릐궻긡긏긚?긿륃뺪귩롦벦
//! @param dataid 긢??붥뜂
//! @param ParamID 릐궻롰쀞ID귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param TextureID 긡긏긚?긿ID귩롷궚롦귡?귽깛?걁NULL됀걂
//! @return 맟뚻갌true?렪봲갌false
//! @attention 긢긫긞긏뾭궻듫릶궳궥갃믅륂궼럊뾭궢귏궧귪갃
bool ResourceManager::GetHumanTextureInfo(int dataid, int *ParamID, int *TextureID)
{
	if( (dataid < 0)||(MAX_LOADHUMANTEXTURE <= dataid ) ){ return false; }

	if( ParamID != NULL ){ *ParamID = human_texture_Param[dataid]; }
	if( TextureID != NULL ){ *TextureID = human_texture_d3dg[dataid]; }
	return true;
}

//! @brief 릐궻긡긏긚?긿귩덇뒊됶뺳
void ResourceManager::CleanupHumanTexture()
{
	if( d3dg == NULL ){ return; }

	for(int i=0; i<MAX_LOADHUMANTEXTURE; i++){
		if( human_texture_Param[i] != -1 ){
			d3dg->CleanupTexture(human_texture_d3dg[i]);
			human_texture_Param[i] = -1;
			human_texture_d3dg[i] = -1;
		}
	}
}

//! @brief 븧딇궻긾긢깑귘긡긏긚?긿귩덇뒊벶귒뜛귒
//! @return 맟뚻갌0?렪봲갌1댥뤵
int ResourceManager::LoadWeaponModelTexture()
{
	int cnt = 0;

	if( d3dg == NULL ){ return 1; }

	for(int i=0; i<TOTAL_PARAMETERINFO_WEAPON; i++){
		WeaponParameter data;
		if( ParamInfo->GetWeapon(i, &data) == 0 ){
			//긾긢깑궴긡긏긚?긿귩벶귒뜛귒갂긄깋?궕뢯귢궽긇긂깛긣궥귡갃
			weapon_model[i] = d3dg->LoadModel(data.model);
			weapon_texture[i] = d3dg->LoadTexture(data.texture, false, false);
			if( weapon_model[i] == -1 ){ cnt += 1; }
			if( weapon_texture[i] == -1 ){ cnt += 1; }
		}
		else{
			//먠믦긢??궕궓궔궢궚귢궽갂긾긢깑궴긡긏긚?긿 2궰궴귖긄깋?갃
			cnt += 2;
		}
	}

	return cnt;
}

//! @brief 븧딇궻긾긢깑궴긡긏긚?긿귩롦벦
//! @return 맟뚻갌0?렪봲갌1
int ResourceManager::GetWeaponModelTexture(int id, int *model, int *texture)
{
	if( model == NULL ){ return 1; }
	if( texture == NULL ){ return 1; }

	if( (id < 0)||(TOTAL_PARAMETERINFO_WEAPON <= id ) ){
#ifdef ENABLE_BUG_HUMANWEAPON
		return GetBugWeaponModelTexture(id, model, texture);
#else
		return 1;
#endif
	}

	*model = weapon_model[id];
	*texture = weapon_texture[id];
	return 0;
}

#ifdef ENABLE_BUG_HUMANWEAPON
//! @brief 긫긐븧딇궻긾긢깑궴긡긏긚?긿귩롦벦
//! @return 맟뚻갌0?렪봲갌1
//! @warning 먩궸 GetWeaponModelTexture()듫릶 귩렳뛱궢궲롦벦궸렪봲궢궫렄궸뙽귟갂궞궻듫릶귩럊궯궲궘궬궠궋갃
//! @attention 긫긐븧딇귩믁돿궥귡뤾뜃궼갂ParameterInfo긏깋긚궻 GetBugWeapon() 듫릶귖뺂뢜궢궲궘궬궠궋갃
int ResourceManager::GetBugWeaponModelTexture(int id, int *model, int *texture)
{
	if( d3dg == NULL ){ return 1; }
	if( model == NULL ){ return 1; }
	if( texture == NULL ){ return 1; }

	if( id == 23 ){
		*model = human_upmodel[0];
		*texture = d3dg->GetMapTextureID(0);
		return 0;
	}
	if( id == 24 ){
		*model = human_upmodel[0];
		*texture = d3dg->GetMapTextureID(3);
		return 0;
	}
	if( id == 30 ){
		*model = human_upmodel[0];
		*texture = d3dg->GetMapTextureID(2);
		return 0;
	}
	if( id == 53 ){
		*model = human_upmodel[0];
		*texture = d3dg->GetMapTextureID(8);
		return 0;
	}

	return 1;
}
#endif

//! @brief 븧딇궻긾긢깑귘긡긏긚?긿귩덇뒊됶뺳
void ResourceManager::CleanupWeaponModelTexture()
{
	if( d3dg == NULL ){ return; }

	for(int i=0; i<TOTAL_PARAMETERINFO_WEAPON; i++){
		d3dg->CleanupModel(weapon_model[i]);
		d3dg->CleanupTexture(weapon_texture[i]);
		weapon_model[i] = -1;
		weapon_texture[i] = -1;
	}
}

//! @brief 븧딇궻긖긂깛긤귩벶귒뜛귔
//! @return 맟뚻갌0?렪봲갌1
int ResourceManager::LoadWeaponSound()
{
	if( SoundCtrl == NULL ){ return 1; }

	int soundid[89];
	int reloadid;
	int reloadsound[TOTAL_RELOAD_SOUND];

	//긖긂깛긤긲?귽깑귩귏궴귕궲벶귒뜛귔
	soundid[0] = SoundCtrl->LoadSound("data\\sound\\bang1.wav");  //glock
	soundid[1] = SoundCtrl->LoadSound("data\\sound\\bang2.wav");  //psg
	soundid[2] = SoundCtrl->LoadSound("data\\sound\\bang3.wav");  //de
	soundid[3] = SoundCtrl->LoadSound("data\\sound\\bang4.wav");  //bar
	soundid[4] = SoundCtrl->LoadSound("data\\sound\\bang5.wav");  //aug
	soundid[5] = SoundCtrl->LoadSound("data\\sound\\ka.wav");
	soundid[6] = SoundCtrl->LoadSound("data\\sound\\1\\ak47.wav");
	soundid[7] = SoundCtrl->LoadSound("data\\sound\\2\\mp40.wav");
	soundid[8] = SoundCtrl->LoadSound("data\\sound\\1\\awp.wav");
	soundid[9] = SoundCtrl->LoadSound("data\\sound\\2\\p90.wav");
	//soundid[9] = SoundCtrl->LoadSound("data\\sound\\1\\deagle.wav");
	soundid[10] = SoundCtrl->LoadSound("data\\sound\\1\\famas.wav");
	//soundid[13] = SoundCtrl->LoadSound("data\\sound\\1\\glock_01.wav");
	soundid[11] = SoundCtrl->LoadSound("data\\sound\\2\\winchester.wav");
	soundid[12] = SoundCtrl->LoadSound("data\\sound\\1\\m1_carbine.wav");
	soundid[13] = SoundCtrl->LoadSound("data\\sound\\1\\m4a1.wav");
	soundid[14] = SoundCtrl->LoadSound("data\\sound\\1\\m4a1sd.wav");
	soundid[15] = SoundCtrl->LoadSound("data\\sound\\2\\svd.wav");
	soundid[16] = SoundCtrl->LoadSound("data\\sound\\1\\mk14.wav");
	soundid[17] = SoundCtrl->LoadSound("data\\sound\\1\\m249.wav");
	soundid[18] = SoundCtrl->LoadSound("data\\sound\\1\\m1shot.wav");
	soundid[19] = SoundCtrl->LoadSound("data\\sound\\1\\galil.wav");
	soundid[20] = SoundCtrl->LoadSound("data\\sound\\2\\sig556.wav");
	soundid[21] = SoundCtrl->LoadSound("data\\sound\\2\\sig556sd.wav");
	soundid[22] = SoundCtrl->LoadSound("data\\sound\\1\\mac10.wav");
	soundid[23] = SoundCtrl->LoadSound("data\\sound\\1\\mk23.wav");
	soundid[24] = SoundCtrl->LoadSound("data\\sound\\1\\mk23sd.wav");
	soundid[25] = SoundCtrl->LoadSound("data\\sound\\2\\scar.wav");
	soundid[26] = SoundCtrl->LoadSound("data\\sound\\2\\scarsd.wav");
	soundid[27] = SoundCtrl->LoadSound("data\\sound\\1\\mp7.wav");
	soundid[28] = SoundCtrl->LoadSound("data\\sound\\1\\m29f.wav");
	soundid[29] = SoundCtrl->LoadSound("data\\sound\\2\\pdw.wav");
	soundid[30] = SoundCtrl->LoadSound("data\\sound\\2\\saiga.wav");
	soundid[31] = SoundCtrl->LoadSound("data\\sound\\2\\spas.wav");
	soundid[32] = SoundCtrl->LoadSound("data\\sound\\2\\ump.wav");
	soundid[33] = SoundCtrl->LoadSound("data\\sound\\2\\umpsd.wav");
	soundid[34] = SoundCtrl->LoadSound("data\\sound\\1\\glock_18.wav");
	soundid[35] = SoundCtrl->LoadSound("data\\sound\\2\\m1911.wav");
	soundid[36] = SoundCtrl->LoadSound("data\\sound\\2\\mk23sd.wav");
	soundid[37] = SoundCtrl->LoadSound("data\\sound\\2\\hk416.wav");
	soundid[38] = SoundCtrl->LoadSound("data\\sound\\2\\hk416sd.wav");
	soundid[39] = SoundCtrl->LoadSound("data\\sound\\2\\tar21.wav");
	soundid[40] = SoundCtrl->LoadSound("data\\sound\\2\\g36k.wav");
	soundid[41] = SoundCtrl->LoadSound("data\\sound\\2\\m16.wav");
	soundid[42] = SoundCtrl->LoadSound("data\\sound\\2\\mp9sd.wav");
	soundid[43] = SoundCtrl->LoadSound("data\\sound\\3\\f2000sd.wav");
	soundid[44] = SoundCtrl->LoadSound("data\\sound\\3\\kriss.wav");
	soundid[45] = SoundCtrl->LoadSound("data\\sound\\3\\uzi.wav");
	soundid[46] = SoundCtrl->LoadSound("data\\sound\\3\\b93r.wav");
	soundid[47] = SoundCtrl->LoadSound("data\\sound\\3\\revolver.wav");
	soundid[48] = SoundCtrl->LoadSound("data\\sound\\3\\jackhammer.wav");
	soundid[49] = SoundCtrl->LoadSound("data\\sound\\3\\db.wav");
	soundid[50] = SoundCtrl->LoadSound("data\\sound\\3\\wa2000.wav");
	soundid[51] = SoundCtrl->LoadSound("data\\sound\\3\\minigun.wav");
	soundid[52] = SoundCtrl->LoadSound("data\\sound\\4\\bar.wav");
	soundid[53] = SoundCtrl->LoadSound("data\\sound\\4\\kar98k.wav");
	soundid[54] = SoundCtrl->LoadSound("data\\sound\\4\\m1garand.wav");
	soundid[55] = SoundCtrl->LoadSound("data\\sound\\4\\ppsh.wav");
	soundid[56] = SoundCtrl->LoadSound("data\\sound\\4\\stg44.wav");
	soundid[57] = SoundCtrl->LoadSound("data\\sound\\4\\thompson.wav");
	soundid[58] = SoundCtrl->LoadSound("data\\sound\\4\\m950.wav");
	soundid[59] = SoundCtrl->LoadSound("data\\sound\\4\\usas.wav");
	soundid[60] = SoundCtrl->LoadSound("data\\sound\\4\\springfield.wav");
	soundid[61] = SoundCtrl->LoadSound("data\\sound\\4\\mg42.wav");
	soundid[62] = SoundCtrl->LoadSound("data\\sound\\3\\throw.wav");
	soundid[63] = SoundCtrl->LoadSound("data\\sound\\3\\bizon.wav");
	soundid[64] = SoundCtrl->LoadSound("data\\sound\\3\\vz61.wav");
	soundid[65] = SoundCtrl->LoadSound("data\\sound\\3\\cf05.wav");
	soundid[66] = SoundCtrl->LoadSound("data\\sound\\3\\ar57.wav");
	soundid[67] = SoundCtrl->LoadSound("data\\sound\\2\\k1a.wav");
	soundid[68] = SoundCtrl->LoadSound("data\\sound\\2\\k2c.wav");
	soundid[69] = SoundCtrl->LoadSound("data\\sound\\1\\aug.wav");
	soundid[70] = SoundCtrl->LoadSound("data\\sound\\1\\glock_01.wav");
	soundid[71] = SoundCtrl->LoadSound("data\\sound\\1\\glock_01_SD.wav"); 
	soundid[72] = SoundCtrl->LoadSound("data\\sound\\3\\b93rsd.wav");
	soundid[73] = SoundCtrl->LoadSound("data\\sound\\3\\vz61sd.wav");
	soundid[74] = SoundCtrl->LoadSound("data\\sound\\2\\mp9.wav");
	soundid[75] = SoundCtrl->LoadSound("data\\sound\\1\\mac10sd.wav");
	soundid[76] = SoundCtrl->LoadSound("data\\sound\\2\\p90sd.wav");
	soundid[77] = SoundCtrl->LoadSound("data\\sound\\2\\pdwsd.wav");
	soundid[78] = SoundCtrl->LoadSound("data\\sound\\3\\krisssd.wav");
	soundid[79] = SoundCtrl->LoadSound("data\\sound\\3\\uzisd.wav");
	soundid[80] = SoundCtrl->LoadSound("data\\sound\\3\\bizonsd.wav");
	soundid[81] = SoundCtrl->LoadSound("data\\sound\\1\\ak47sd.wav");
	soundid[82] = SoundCtrl->LoadSound("data\\sound\\1\\famassd.wav");
	soundid[83] = SoundCtrl->LoadSound("data\\sound\\2\\m16sd.wav");
	soundid[84] = SoundCtrl->LoadSound("data\\sound\\2\\g36ksd.wav");
	soundid[85] = SoundCtrl->LoadSound("data\\sound\\2\\tar21sd.wav");
	soundid[86] = SoundCtrl->LoadSound("data\\sound\\1\\augsd.wav");
	soundid[87] = SoundCtrl->LoadSound("data\\sound\\4\\kar98s.wav");
	soundid[88] = SoundCtrl->LoadSound("data\\sound\\5\\dp28.wav");

	//soundid[42] = SoundCtrl->LoadSound("data\\sound\\2\\mp9.wav");
	//soundid[21] = SoundCtrl->LoadSound("data\\sound\\1\\mk14sd.wav");
	//soundid[40] = SoundCtrl->LoadSound("data\\sound\\2\\grenade.wav");



	// 기존 단일 재장전 사운드는 사용하지 않는다.
	reloadid = -1;

	// 큰 그룹 기준 재장전 사운드
	reloadsound[RELOAD_PISTOL_EMPTY] = SoundCtrl->LoadSound("data\\sound\\reload\\1\\pistol_empty.wav");
	reloadsound[RELOAD_PISTOL_TACTICAL] = SoundCtrl->LoadSound("data\\sound\\reload\\1\\pistol_tactical.wav");

	reloadsound[RELOAD_HEAVY_PISTOL_EMPTY] = SoundCtrl->LoadSound("data\\sound\\reload\\1\\heavy_pistol_empty.wav");
	reloadsound[RELOAD_HEAVY_PISTOL_TACTICAL] = SoundCtrl->LoadSound("data\\sound\\reload\\1\\heavy_pistol_tactical.wav");

	reloadsound[RELOAD_SMG_EMPTY] = SoundCtrl->LoadSound("data\\sound\\reload\\1\\smg_empty.wav");
	reloadsound[RELOAD_SMG_TACTICAL] = SoundCtrl->LoadSound("data\\sound\\reload\\1\\smg_tactical.wav");
	reloadsound[RELOAD_CLASSIC_SMG] = SoundCtrl->LoadSound("data\\sound\\reload\\1\\classic_smg_reload.wav");

	reloadsound[RELOAD_BAR] = SoundCtrl->LoadSound("data\\sound\\reload\\2\\bar_reload.wav");

	reloadsound[RELOAD_AR_EMPTY] = SoundCtrl->LoadSound("data\\sound\\reload\\2\\ar_empty.wav");
	reloadsound[RELOAD_AR_TACTICAL] = SoundCtrl->LoadSound("data\\sound\\reload\\2\\ar_tactical.wav");
	reloadsound[RELOAD_AK_EMPTY] = SoundCtrl->LoadSound("data\\sound\\reload\\2\\ak_empty.wav");
	reloadsound[RELOAD_AK_TACTICAL] = SoundCtrl->LoadSound("data\\sound\\reload\\2\\ak_tactical.wav");
	reloadsound[RELOAD_BULLPUP_EMPTY] = SoundCtrl->LoadSound("data\\sound\\reload\\2\\bullpup_empty.wav");
	reloadsound[RELOAD_BULLPUP_TACTICAL] = SoundCtrl->LoadSound("data\\sound\\reload\\2\\bullpup_tactical.wav");

	reloadsound[RELOAD_P90_EMPTY] = SoundCtrl->LoadSound("data\\sound\\reload\\1\\p90_empty.wav");
	reloadsound[RELOAD_P90_TACTICAL] = SoundCtrl->LoadSound("data\\sound\\reload\\1\\p90_tactical.wav");
	reloadsound[RELOAD_BIZON_EMPTY] = SoundCtrl->LoadSound("data\\sound\\reload\\1\\bizon_empty.wav");
	reloadsound[RELOAD_BIZON_TACTICAL] = SoundCtrl->LoadSound("data\\sound\\reload\\1\\bizon_tactical.wav");

	reloadsound[RELOAD_LMG_BOX] = SoundCtrl->LoadSound("data\\sound\\reload\\3\\lmg_box_reload.wav");
	reloadsound[RELOAD_LMG_BELT] = SoundCtrl->LoadSound("data\\sound\\reload\\3\\lmg_belt_reload.wav");
	reloadsound[RELOAD_DP28] = SoundCtrl->LoadSound("data\\sound\\reload\\5\\dp28_reload_empty.wav");
	reloadsound[RELOAD_MINIGUN] = SoundCtrl->LoadSound("data\\sound\\reload\\3\\minigun_reload.wav");

	reloadsound[RELOAD_MINIGUN_SPIN_START] = SoundCtrl->LoadSound("data\\sound\\reload\\3\\minigun_spin_start.wav");
	reloadsound[RELOAD_MINIGUN_SPIN] = SoundCtrl->LoadSound("data\\sound\\reload\\3\\minigun_spin.wav");
	reloadsound[RELOAD_MINIGUN_SPIN_END] = SoundCtrl->LoadSound("data\\sound\\reload\\3\\minigun_spin_end.wav");

	reloadsound[RELOAD_DMR_EMPTY] = SoundCtrl->LoadSound("data\\sound\\reload\\3\\DMR_empty.wav");
	reloadsound[RELOAD_DMR_TACTICAL] = SoundCtrl->LoadSound("data\\sound\\reload\\3\\DMR_tactical.wav");

	reloadsound[RELOAD_SNIPER_EMPTY] = SoundCtrl->LoadSound("data\\sound\\reload\\3\\sniper_empty.wav");
	reloadsound[RELOAD_SNIPER_TACTICAL] = SoundCtrl->LoadSound("data\\sound\\reload\\3\\sniper_tactical.wav");

	reloadsound[RELOAD_RELOAD_START] =
		SoundCtrl->LoadSound("data\\sound\\reload\\4\\reload_start.wav");

	reloadsound[RELOAD_SHOTGUN_SHELL_INSERT] =
		SoundCtrl->LoadSound("data\\sound\\reload\\4\\shotgun_shell_insert.wav");

	reloadsound[RELOAD_LEVER_SHELL_INSERT] =
		SoundCtrl->LoadSound("data\\sound\\reload\\4\\lever_shell_insert.wav");

	reloadsound[RELOAD_BOLT_RELOAD_START] =
		SoundCtrl->LoadSound("data\\sound\\reload\\4\\bolt_reload_start.wav");

	reloadsound[RELOAD_SNIPER_SHELL_INSERT] =
		SoundCtrl->LoadSound("data\\sound\\reload\\4\\sniper_shell_insert.wav");

	reloadsound[RELOAD_BOLT_RELOAD_END] =
		SoundCtrl->LoadSound("data\\sound\\reload\\4\\bolt_reload_end.wav");

	reloadsound[RELOAD_SHOTGUN_MAG_EMPTY] = SoundCtrl->LoadSound("data\\sound\\reload\\4\\shotgun_mag_empty.wav");
	reloadsound[RELOAD_SHOTGUN_MAG_TACTICAL] = SoundCtrl->LoadSound("data\\sound\\reload\\4\\shotgun_mag_tactical.wav");

	reloadsound[RELOAD_DRUM_SHOTGUN] = SoundCtrl->LoadSound("data\\sound\\reload\\4\\drum_shotgun_reload.wav");

	reloadsound[RELOAD_REVOLVER] =
		SoundCtrl->LoadSound("data\\sound\\reload\\1\\revolver_reload.wav");

	reloadsound[RELOAD_KAR98_CLIP] =
		SoundCtrl->LoadSound("data\\sound\\reload\\3\\kar98_clip.wav");

	reloadsound[RELOAD_GARAND_CLIP] =
		SoundCtrl->LoadSound("data\\sound\\reload\\3\\garand_clip.wav");

	reloadsound[RELOAD_DOUBLE_BARREL] =
		SoundCtrl->LoadSound("data\\sound\\reload\\4\\double_barrel_reload.wav");

	reloadsound[RELOAD_AWP_BOLT] = SoundCtrl->LoadSound("data\\sound\\reload\\awp_bolt.wav");
	reloadsound[RELOAD_KAR98_SNIPER_BOLT] = SoundCtrl->LoadSound("data\\sound\\reload\\kar98_sniper_bolt.wav");

	reloadsound[RELOAD_SHOTGUN_ACTION] = SoundCtrl->LoadSound("data\\sound\\reload\\shotgun_pump_action.wav");
	reloadsound[RELOAD_WINCHESTER_LEVER] = SoundCtrl->LoadSound("data\\sound\\reload\\winchester_lever.wav");

	reloadsound[RELOAD_DRY_FIRE] = SoundCtrl->LoadSound("data\\sound\\reload\\dry_fire.wav");

	reloadsound[ZOMBIE_ATTACK_1] = SoundCtrl->LoadSound("data\\sound\\zombie_attack1.wav");
	reloadsound[ZOMBIE_ATTACK_2] = SoundCtrl->LoadSound("data\\sound\\zombie_attack2.wav");

	reloadsound[WEAPONMODE_PISTOL_SUPPRESSOR_ATTACH] =
		SoundCtrl->LoadSound("data\\sound\\weaponmode\\pistol_suppressor_attach.wav");

	reloadsound[WEAPONMODE_PISTOL_SUPPRESSOR_DETACH] =
		SoundCtrl->LoadSound("data\\sound\\weaponmode\\pistol_suppressor_detach.wav");

	reloadsound[WEAPONMODE_LONGGUN_SUPPRESSOR_ATTACH] =
		SoundCtrl->LoadSound("data\\sound\\weaponmode\\longgun_suppressor_attach.wav");

	reloadsound[WEAPONMODE_LONGGUN_SUPPRESSOR_DETACH] =
		SoundCtrl->LoadSound("data\\sound\\weaponmode\\longgun_suppressor_detach.wav");

	reloadsound[WEAPONMODE_FIREMODE_CHANGE] =
		SoundCtrl->LoadSound("data\\sound\\weaponmode\\firemode_change.wav");

	for (int i = 0; i < TOTAL_RELOAD_SOUND; i++) {
		weapon_reloadsound_group[i] = reloadsound[i];
	}

	for (int i = 0; i < TOTAL_PARAMETERINFO_WEAPON; i++) {
		WeaponParameter data;
		if (ParamInfo->GetWeapon(i, &data) == 0) {
			// 발사음 ID 범위 확인
			if (data.soundid >= 0 && data.soundid <= 88) {
				weapon_sound[i] = soundid[data.soundid];
			}
			else {
				weapon_sound[i] = -1;
			}
		}
	}
	weapon_reloadsound = reloadid;

	//긲?귽깑벶귒뜛귒렄궸긄깋?궕뢯궶궔궯궫궔뮧귊귡
	for(int i=0; i<6; i++){
		if( soundid[i] == -1 ){ return 1; }
	}
	return 0;
}

int ResourceManager::LoadNVGSound()
{
	if (SoundCtrl == NULL) { return 1; }

	// d3dg가 아닌 SoundCtrl을 사용하여 로드합니다.
	sound_nvg = SoundCtrl->LoadSound("data\\sound\\gunswitch.wav");

	if (sound_nvg == -1) { return 1; }
	return 0;
}

//! @brief 야간 투시경 사운드 ID 가져오기
int ResourceManager::GetNVGSound()
{
	return sound_nvg;
}

//! @brief 야간 투시경 사운드 해제
void ResourceManager::CleanupNVGSound()
{
	if (SoundCtrl == NULL) { return; }
	if (sound_nvg != -1) {
		SoundCtrl->CleanupSound(sound_nvg);
		sound_nvg = -1;
	}
}

// 플레이어 스킬 공용 효과음 로드
int ResourceManager::LoadSkillSound()
{
	if (SoundCtrl == NULL) { return 1; }

	skill_sound[SKILL_SOUND_COMMON_START] = SoundCtrl->LoadSound("data\\sound\\skill\\common\\start.wav");
	skill_sound[SKILL_SOUND_COMMON_READY] = SoundCtrl->LoadSound("data\\sound\\skill\\common\\ready.wav");
	skill_sound[SKILL_SOUND_COMMON_FAIL] = SoundCtrl->LoadSound("data\\sound\\skill\\common\\fail.wav");
	skill_sound[SKILL_SOUND_COMMON_END] = SoundCtrl->LoadSound("data\\sound\\skill\\common\\end.wav");

	skill_sound[SKILL_SOUND_DISTRACTION_BEEP] = SoundCtrl->LoadSound("data\\sound\\skill\\distraction\\beep.wav");
	skill_sound[SKILL_SOUND_ROBOT_EXECUTE_SHOT] = SoundCtrl->LoadSound("data\\sound\\skill\\robot_execute\\shot.wav");
	skill_sound[SKILL_SOUND_SHIELD_BLOCK] = SoundCtrl->LoadSound("data\\sound\\skill\\shield\\block.wav");

	// 지뢰 감지음/활성화음은 별도 준비 파일이므로, 없어도 게임 로딩 실패로 처리하지 않는다.
	skill_sound[SKILL_SOUND_TERRORIST_MINE_TRIGGER] = SoundCtrl->LoadSound("data\\sound\\skill\\mine\\trigger.wav");
	skill_sound[SKILL_SOUND_TERRORIST_MINE_ARMED] = SoundCtrl->LoadSound("data\\sound\\skill\\mine\\armed.wav");

	for (int i = 0; i < TOTAL_SKILL_SOUND; i++) {
		if (skill_sound[i] == -1) {
			if ((i == SKILL_SOUND_TERRORIST_MINE_TRIGGER) ||
				(i == SKILL_SOUND_TERRORIST_MINE_ARMED)) {
				continue;
			}

			return 1;
		}
	}

	return 0;
}

// 플레이어 스킬 공용 효과음 ID 반환
int ResourceManager::GetSkillSound(int id)
{
	if (SoundCtrl == NULL) { return -1; }
	if ((id < 0) || (TOTAL_SKILL_SOUND <= id)) { return -1; }

	return skill_sound[id];
}

void ResourceManager::CleanupSkillSound()
{
	if (SoundCtrl == NULL) { return; }

	for (int i = 0; i < TOTAL_SKILL_SOUND; i++) {
		if (skill_sound[i] != -1) {
			SoundCtrl->CleanupSound(skill_sound[i]);
			skill_sound[i] = -1;
		}
	}
}

// 플레이어 스킬 아이콘 로드
int ResourceManager::LoadSkillIconTexture()
{
	if (d3dg == NULL) { return 1; }

	skillicon_texture[SKILL_ICON_STEALTH] =
		d3dg->LoadTexture("data\\skill\\stealth_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_AMMOBOOST] =
		d3dg->LoadTexture("data\\skill\\ammoboost_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_SCAN] =
		d3dg->LoadTexture("data\\skill\\scan_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_HEAL] =
		d3dg->LoadTexture("data\\skill\\heal_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_SURVIVAL] =
		d3dg->LoadTexture("data\\skill\\survival_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_DODGE] =
		d3dg->LoadTexture("data\\skill\\dodge_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_ABSORB] =
		d3dg->LoadTexture("data\\skill\\absorb_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_MARKSHOT] =
		d3dg->LoadTexture("data\\skill\\markshot_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_ROBOT_EXECUTE] =
		d3dg->LoadTexture("data\\skill\\robot_execute_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_DISTRACTION] =
		d3dg->LoadTexture("data\\skill\\distraction_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_SUMMON] =
		d3dg->LoadTexture("data\\skill\\summon_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_SUPPRESS] =
		d3dg->LoadTexture("data\\skill\\suppress_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_HOOK] =
		d3dg->LoadTexture("data\\skill\\hook_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_IRONBODY] =
		d3dg->LoadTexture("data\\skill\\ironbody_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_RAGE] =
		d3dg->LoadTexture("data\\skill\\rage_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_TELEPORT] =
		d3dg->LoadTexture("data\\skill\\teleport_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_IMPACT_GRENADE] =
		d3dg->LoadTexture("data\\skill\\impact_grenade_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_SHIELD] =
		d3dg->LoadTexture("data\\skill\\shield_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_DP28] =
		d3dg->LoadTexture("data\\skill\\dp28_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_TERRORIST_MINE] =
		d3dg->LoadTexture("data\\skill\\mine_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_BIOTIC_FIELD] =
		d3dg->LoadTexture("data\\skill\\biofield_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_ARTILLERY_STRIKE] =
		d3dg->LoadTexture("data\\skill\\artillery_strike_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_SHADOW_DECOY] =
		d3dg->LoadTexture("data\\skill\\shadow_decoy_icon.bmp", false, false);

	skillicon_texture[SKILL_ICON_AIMHACK] =
		d3dg->LoadTexture("data\\skill\\aimhack_icon.bmp", false, false);

	for (int i = 0; i < TOTAL_SKILL_ICON; i++) {
		if (skillicon_texture[i] == -1) {
			return 1;
		}
	}

	return 0;
}

// 플레이어 스킬 아이콘 ID 반환
int ResourceManager::GetSkillIconTexture(int id)
{
	if ((id < 0) || (TOTAL_SKILL_ICON <= id)) {
		return -1;
	}

	return skillicon_texture[id];
}

// 기존 은신 아이콘 함수 호환용
int ResourceManager::GetSkillStealthIconTexture()
{
	return GetSkillIconTexture(SKILL_ICON_STEALTH);
}

// 플레이어 스킬 아이콘 해제
void ResourceManager::CleanupSkillIconTexture()
{
	if (d3dg == NULL) { return; }

	for (int i = 0; i < TOTAL_SKILL_ICON; i++) {
		if (skillicon_texture[i] != -1) {
			d3dg->CleanupTexture(skillicon_texture[i]);
			skillicon_texture[i] = -1;
		}
	}
}

// [✨ 추가 ✨] 발소리 파일 로드
int ResourceManager::LoadFootstepSound()
{
	if (SoundCtrl == NULL) { return 1; }

	// 10개의 무작위 발소리 파일을 로드합니다. (이름을 step_1 ~ step_10 으로 맞춰주세요)
	footstep_sound[0] = SoundCtrl->LoadSound("data\\sound\\walk\\step_1.wav");
	footstep_sound[1] = SoundCtrl->LoadSound("data\\sound\\walk\\step_2.wav");
	footstep_sound[2] = SoundCtrl->LoadSound("data\\sound\\walk\\step_3.wav");
	footstep_sound[3] = SoundCtrl->LoadSound("data\\sound\\walk\\step_4.wav");
	footstep_sound[4] = SoundCtrl->LoadSound("data\\sound\\walk\\step_5.wav");
	footstep_sound[5] = SoundCtrl->LoadSound("data\\sound\\walk\\step_6.wav");
	footstep_sound[6] = SoundCtrl->LoadSound("data\\sound\\walk\\step_7.wav");
	footstep_sound[7] = SoundCtrl->LoadSound("data\\sound\\walk\\step_8.wav");
	footstep_sound[8] = SoundCtrl->LoadSound("data\\sound\\walk\\step_9.wav");
	footstep_sound[9] = SoundCtrl->LoadSound("data\\sound\\walk\\step_10.wav");

	// 1번 파일이라도 로드에 실패하면 에러 반환
	for (int i = 0; i < 10; i++) {
		if (footstep_sound[i] == -1) {
			return 1;
		}
	}


	return 0;
}

// [✨ 추가 ✨] 발소리 ID 반환
int ResourceManager::GetFootstepSound(int type)
{
	// 요청한 타입이 0~9 범위를 벗어나면 안전하게 0번 반환
	if (type < 0 || type >= 10) {
		return footstep_sound[0];
	}
	return footstep_sound[type];
}

// [✨ 추가 ✨] 발소리 데이터 초기화
void ResourceManager::CleanupFootstepSound()
{
	if (SoundCtrl == NULL) { return; }

	for (int i = 0; i < 10; i++) {
		if (footstep_sound[i] != -1) {
			SoundCtrl->CleanupSound(footstep_sound[i]);
			footstep_sound[i] = -1;
		}
	}
}

//! @brief 븧딇궻긖긂깛긤귩롦벦
//! @param id 0댥뤵궳븧딇궻긢??붥뜂갂-1궳깏깓?긤돶
//! @return 맟뚻갌긢??붥뜂?렪봲갌-1
int ResourceManager::GetWeaponSound(int id)
{
	if( SoundCtrl == NULL ){ return -1; }
	if( id == -1 ){ return weapon_reloadsound; }

	if( (id < 0)||(TOTAL_PARAMETERINFO_WEAPON <= id ) ){ return -1; }
	return weapon_sound[id];
}


int ResourceManager::GetReloadSound(int id)
{
	if (SoundCtrl == NULL) { return -1; }
	if ((id < 0) || (TOTAL_RELOAD_SOUND <= id)) { return -1; }

	return weapon_reloadsound_group[id];
}

//! @brief 븧딇궻긖긂깛긤귩덇뒊됶뺳
void ResourceManager::CleanupWeaponSound()
{
	if( SoundCtrl == NULL ){ return; }

	for(int i=0; i<TOTAL_PARAMETERINFO_WEAPON; i++){
		SoundCtrl->CleanupSound(weapon_sound[i]);
		weapon_sound[i] = -1;
	}
	SoundCtrl->CleanupSound(weapon_reloadsound);
	weapon_reloadsound = -1;

	for (int i = 0; i < TOTAL_RELOAD_SOUND; i++) {
		SoundCtrl->CleanupSound(weapon_reloadsound_group[i]);
		weapon_reloadsound_group[i] = -1;
	}
}

//! @brief 룷븿궻긾긢깑귘긡긏긚?긿귩덇뒊벶귒뜛귒
//! @return 맟뚻갌0?렪봲갌1댥뤵
int ResourceManager::LoadSmallObjectModelTexture()
{
	int cnt = 0;

	if( d3dg == NULL ){ return 1; }

	for(int i=0; i<TOTAL_PARAMETERINFO_SMALLOBJECT; i++){
		SmallObjectParameter data;
		if( ParamInfo->GetSmallObject(i, &data) == 0 ){
			//긾긢깑궴긡긏긚?긿귩벶귒뜛귒갂긄깋?궕뢯귢궽긇긂깛긣궥귡갃
			smallobject_model[i] = d3dg->LoadModel(data.model);
			smallobject_texture[i] = d3dg->LoadTexture(data.texture, false, false);
			if( smallobject_model[i] == -1 ){ cnt += 1; }
			if( smallobject_texture[i] == -1 ){ cnt += 1; }
		}
		else{
			//먠믦긢??궕궓궔궢궚귢궽갂긾긢깑궴긡긏긚?긿 2궰궴귖긄깋?갃
			cnt += 2;
		}
	}

	return cnt;
}

//! @brief 룷븿궻긾긢깑궴긡긏긚?긿귩롦벦
//! @return 맟뚻갌0?렪봲갌1
int ResourceManager::GetSmallObjectModelTexture(int id, int *model, int *texture)
{
	if( model == NULL ){ return 1; }
	if( texture == NULL ){ return 1; }
	if( (id < 0)||((TOTAL_PARAMETERINFO_SMALLOBJECT + MAX_ADDSMALLOBJECT) <= id ) ){ return 1; }

	*model = smallobject_model[id];
	*texture = smallobject_texture[id];
	return 0;
}

//! @brief 룷븿궻긾긢깑귘긡긏긚?긿귩덇뒊됶뺳
void ResourceManager::CleanupSmallObjectModelTexture()
{
	if( d3dg == NULL ){ return; }

	for(int i=0; i<(TOTAL_PARAMETERINFO_SMALLOBJECT + MAX_ADDSMALLOBJECT); i++){
		d3dg->CleanupModel(smallobject_model[i]);
		d3dg->CleanupTexture(smallobject_texture[i]);
		smallobject_model[i] = -1;
		smallobject_texture[i] = -1;
	}
}

//! @brief 룷븿궻긖긂깛긤귩벶귒뜛귔
//! @return 맟뚻갌0?렪봲갌1
int ResourceManager::LoadSmallObjectSound()
{
	if( SoundCtrl == NULL ){ return 1; }

	int soundid[2];

	//긖긂깛긤긲?귽깑귩귏궴귕궲벶귒뜛귔
	soundid[0] = SoundCtrl->LoadSound("data\\sound\\can.wav");
	soundid[1] = SoundCtrl->LoadSound("data\\sound\\dan.wav");

	for(int i=0; i<TOTAL_PARAMETERINFO_SMALLOBJECT; i++){
		SmallObjectParameter data;
		if( ParamInfo->GetSmallObject(i, &data) == 0 ){
			//긖긂깛긤붥뜂궸뜃귦궧궲갂뚼됈돶귩뒆귟뱰궲귡갃
			if( data.sound == 0 ){
				smallobject_sound[i] = soundid[0];
			}
			else if( data.sound == 1 ){
				smallobject_sound[i] = soundid[1];
			}
			else{
				smallobject_sound[i] = -1;
			}
		}
	}

	//긲?귽깑벶귒뜛귒렄궸긄깋?궕뢯궶궔궯궫궔뮧귊귡
	for(int i=0; i<2; i++){
		if( soundid[i] == -1 ){ return 1; }
	}
	return 0;
}

//! @brief 룷븿궻긖긂깛긤귩롦벦
//! @param id 룷븿궻긢??붥뜂
//! @return 맟뚻갌긢??붥뜂?렪봲갌-1
int ResourceManager::GetSmallObjectSound(int id)
{
	if( SoundCtrl == NULL ){ return -1; }
	if( (id < 0)||((TOTAL_PARAMETERINFO_SMALLOBJECT + MAX_ADDSMALLOBJECT) <= id ) ){ return -1; }
	return smallobject_sound[id];
}

//! @brief 룷븿궻긖긂깛긤귩덇뒊됶뺳
void ResourceManager::CleanupSmallObjectSound()
{
	if( SoundCtrl == NULL ){ return; }

	for(int i=0; i<(TOTAL_PARAMETERINFO_SMALLOBJECT + MAX_ADDSMALLOBJECT); i++){
		SoundCtrl->CleanupSound(smallobject_sound[i]);
		smallobject_sound[i] = -1;
	}
}

//! @brief 믁돿룷븿궻긾긢깑갋긡긏긚?긿갋긖긂깛긤귩롦벦
//! @param id 긢??붥뜂걁믅륂궼 0 뚂믦걂
//! @param modelpath 긾긢깑긢??궻긬긚
//! @param texturepath 긡긏긚?긿긢??궻긬긚
//! @param soundpath 긖긂깛긤긢??궻긬긚
//! @return 맟뚻갌0?렪봲갌1댥뤵
int ResourceManager::LoadAddSmallObject(int id, char* modelpath, char* texturepath, char* soundpath)
{
	if (d3dg == NULL) { return 1; }
	if (SoundCtrl == NULL) { return 1; }
	if (modelpath == NULL) { return 1; }
	if (texturepath == NULL) { return 1; }
	if (soundpath == NULL) { return 1; }
	if ((id < 0) || (MAX_ADDSMALLOBJECT <= id)) { return 1; }

	int dataid = TOTAL_PARAMETERINFO_SMALLOBJECT + id;
	int cnt = 0;

	// 기존 추가 소형 오브젝트 리소스가 남아 있으면 먼저 해제
	if (smallobject_model[dataid] != -1) {
		d3dg->CleanupModel(smallobject_model[dataid]);
		smallobject_model[dataid] = -1;
	}
	if (smallobject_texture[dataid] != -1) {
		d3dg->CleanupTexture(smallobject_texture[dataid]);
		smallobject_texture[dataid] = -1;
	}
	if (smallobject_sound[dataid] != -1) {
		SoundCtrl->CleanupSound(smallobject_sound[dataid]);
		smallobject_sound[dataid] = -1;
	}

	smallobject_model[dataid] = d3dg->LoadModel(modelpath);
	smallobject_texture[dataid] = d3dg->LoadTexture(texturepath, false, false);
	smallobject_sound[dataid] = SoundCtrl->LoadSound(soundpath);

	if (smallobject_model[dataid] == -1) { cnt += 1; }
	if (smallobject_texture[dataid] == -1) { cnt += 1; }
	if (smallobject_sound[dataid] == -1) { cnt += 1; }

	return cnt;
}

//! @brief 믁돿룷븿궻긾긢깑갋긡긏긚?긿갋긖긂깛긤귩됶뺳
//! @param id 긢??붥뜂걁믅륂궼 0 뚂믦걂
void ResourceManager::CleanupAddSmallObject(int id)
{
	if (d3dg == NULL) { return; }
	if (SoundCtrl == NULL) { return; }
	if ((id < 0) || (MAX_ADDSMALLOBJECT <= id)) { return; }

	int dataid = TOTAL_PARAMETERINFO_SMALLOBJECT + id;

	if (smallobject_model[dataid] != -1) {
		d3dg->CleanupModel(smallobject_model[dataid]);
		smallobject_model[dataid] = -1;
	}
	if (smallobject_texture[dataid] != -1) {
		d3dg->CleanupTexture(smallobject_texture[dataid]);
		smallobject_texture[dataid] = -1;
	}
	if (smallobject_sound[dataid] != -1) {
		SoundCtrl->CleanupSound(smallobject_sound[dataid]);
		smallobject_sound[dataid] = -1;
	}
}

//! @brief 뭙궻긾긢깑궴긡긏긚?긿귩벶귒뜛귔
//! @return 맟뚻갌0?렪봲갌1댥뤵
int ResourceManager::LoadBulletModelTexture()
{
	int cnt = 0;

	if( d3dg == NULL ){ return 1; }

	for(int i=0; i<TOTAL_PARAMETERINFO_BULLET; i++){
		BulletParameter data;
		if( ParamInfo->GetBullet(i, &data) == 0 ){
			//긾긢깑궴긡긏긚?긿귩벶귒뜛귒갂긄깋?궕뢯귢궽긇긂깛긣궥귡갃
			bullet_model[i] = d3dg->LoadModel(data.model);
			bullet_texture[i] = d3dg->LoadTexture(data.texture, false, false);
			if( bullet_model[i] == -1 ){ cnt += 1; }
			if( bullet_texture[i] == -1 ){ cnt += 1; }
		}
		else{
			//먠믦긢??궕궓궔궢궚귢궽갂긾긢깑궴긡긏긚?긿 2궰궴귖긄깋?갃
			cnt += 2;
		}
	}

	return cnt;
}

//! @brief 뭙궻긾긢깑궴긡긏긚?긿귩롦벦
//! @return 맟뚻갌0?렪봲갌1
int ResourceManager::GetBulletModelTexture(int id, int *model, int *texture)
{
	if( model == NULL ){ return 1; }
	if( texture == NULL ){ return 1; }
	if( (id < 0)||(TOTAL_PARAMETERINFO_BULLET <= id ) ){ return 1; }

	*model = bullet_model[id];
	*texture = bullet_texture[id];

	return 0;
}

//! @brief 뭙궻긾긢깑궴긡긏긚?긿귩됶뺳
void ResourceManager::CleanupBulletModelTexture()
{
	if( d3dg == NULL ){ return; }

	for(int i=0; i<TOTAL_PARAMETERINFO_BULLET; i++){
		d3dg->CleanupModel(bullet_model[i]);
		bullet_model[i] = -1;

		d3dg->CleanupTexture(bullet_texture[i]);
		bullet_texture[i] = -1;
	}
}

//! @brief 긚긓?긵긡긏긚?긿귩벶귒뜛귔
//! @return 맟뚻갌0?렪봲갌1
int ResourceManager::LoadScopeTexture()
{
	if( d3dg == NULL ){ return 1; }

	scopetexture = d3dg->LoadTexture("data\\scope.dds", false, false);
	if( scopetexture == -1 ){ return 1; }
	return 0;
}

//! @brief 긚긓?긵긡긏긚?긿귩롦벦
//! @return 긡긏긚?긿궻긢??붥뜂
int ResourceManager::GetScopeTexture()
{
	return scopetexture;
}

//! @brief 긚긓?긵긡긏긚?긿귩됶뺳
void ResourceManager::CleanupScopeTexture()
{
	if( d3dg == NULL ){ return; }
	if( scopetexture == -1 ){ return; }

	d3dg->CleanupTexture(scopetexture);
	scopetexture = -1;
}

//! @brief 봶똧뗴궻긡긏긚?긿귩벶귒뜛귔
//! @param id 뗴궻붥뜂걁1?5걂
//! @return 맟뚻갌0?렪봲갌1
int ResourceManager::LoadSkyModelTexture(int id)
{
	if( d3dg == NULL ){ return 1; }
	if( (id < 1)||(5 < id) ){ return 1; }

	//긾긢깑귩벶귒뜛귔걁뚂믦걂
	skymodel = d3dg->LoadModel("data\\sky\\sky.x");

	//긡긏긚?긿뼹귩맯맟궢궲벶귒뜛귔
	char path[MAX_PATH];
	sprintf(path, "data\\sky\\sky%d.bmp", id);
	skytexture = d3dg->LoadTexture(path, false, false);

	if( skymodel == -1 ){ return 1; }
	if( skytexture == -1 ){ return 1; }
	return 0;
}

//! @brief 봶똧뗴궻긾긢깑궴긡긏긚?긿귩롦벦
void ResourceManager::GetSkyModelTexture(int *model, int *texture)
{
	if( model == NULL ){ return; }
	if( texture == NULL ){ return; }

	*model = skymodel;
	*texture = skytexture;
}

//! @brief 봶똧뗴궻긾긢깑궴긡긏긚?긿귩됶뺳
void ResourceManager::CleanupSkyModelTexture()
{
	if( d3dg == NULL ){ return; }

	if( skymodel != -1 ){
		d3dg->CleanupModel(skymodel);
		skymodel = -1;
	}
	if( skytexture != -1 ){
		d3dg->CleanupTexture(skytexture);
		skytexture = -1;
	}
}

//! @brief 뭙갋롨왮뭙궻긖긂깛긤귩벶귒뜛귔
//! @return 맟뚻갌0?렪봲갌1
int ResourceManager::LoadBulletSound()
{
	if( SoundCtrl == NULL ){ return 1; }

	bullet_hitsoundA = SoundCtrl->LoadSound("data\\sound\\hit1.wav");
	bullet_hitsoundB = SoundCtrl->LoadSound("data\\sound\\hit3.wav");
	bullet_humanhitsound = SoundCtrl->LoadSound("data\\sound\\hit2.wav");
	bullet_passingsound = SoundCtrl->LoadSound("data\\sound\\hyu.wav");
	grenade_bang = SoundCtrl->LoadSound("data\\sound\\bang.wav");
	grenade_cco = SoundCtrl->LoadSound("data\\sound\\cco.wav");

	if( bullet_hitsoundA == -1 ){ return 1; }
	if( bullet_hitsoundB == -1 ){ return 1; }
	if( bullet_humanhitsound == -1 ){ return 1; }
	if( bullet_passingsound == -1 ){ return 1; }
	if( grenade_bang == -1 ){ return 1; }
	if( grenade_cco == -1 ){ return 1; }
	return 0;
}

//! @brief 뭙갋롨왮뭙궻긖긂깛긤귩롦벦
//! @param hitsoundA 뭙긭긞긣긖긂깛긤갌A?걁NULL됀걂
//! @param hitsoundB 뭙긭긞긣긖긂깛긤갌B?걁NULL됀걂
//! @param humanhitsound 뭙긭긞긣긖긂깛긤갌릐?걁NULL됀걂
//! @param passingsound 뭙믅됡긖긂깛긤?걁NULL됀걂
//! @param grenadebang 롨왮뭙 뵚뵯긖긂깛긤?걁NULL됀걂
//! @param grenadecco 롨왮뭙긫긂깛긤 긖긂깛긤?걁NULL됀걂
void ResourceManager::GetBulletSound(int *hitsoundA, int *hitsoundB, int *humanhitsound, int *passingsound, int *grenadebang, int *grenadecco)
{
	if( SoundCtrl == NULL ){ return; }

	if( hitsoundA != NULL ){ *hitsoundA = bullet_hitsoundA; }
	if( hitsoundB != NULL ){ *hitsoundB = bullet_hitsoundB; }
	if( humanhitsound != NULL ){ *humanhitsound = bullet_humanhitsound; }
	if( passingsound != NULL ){ *passingsound = bullet_passingsound; }
	if( grenadebang != NULL ){ *grenadebang = grenade_bang; }
	if( grenadecco != NULL ){ *grenadecco = grenade_cco; }
}

//! @brief 뭙갋롨왮뭙궻긖긂깛긤귩덇뒊됶뺳
void ResourceManager::CleanupBulletSound()
{
	if( SoundCtrl == NULL ){ return; }

	SoundCtrl->CleanupSound(bullet_hitsoundA);
	SoundCtrl->CleanupSound(bullet_hitsoundB);
	SoundCtrl->CleanupSound(bullet_humanhitsound);
	SoundCtrl->CleanupSound(bullet_passingsound);
	SoundCtrl->CleanupSound(grenade_bang);
	SoundCtrl->CleanupSound(grenade_cco);

	bullet_hitsoundA = -1;
	bullet_hitsoundB = -1;
	bullet_humanhitsound = -1;
	bullet_passingsound = -1;
	grenade_bang = -1;
	grenade_cco = -1;
}

//! @brief 긄긲긃긏긣궻긡긏긚?긿귩벶귒뜛귔
//! @return 맟뚻갌0?렪봲갌1
int ResourceManager::LoadEffectTexture()
{
	if( d3dg == NULL ){ return 1; }

	effecttexture_blood = d3dg->LoadTexture("data\\blood.dds", false, false);
	effecttexture_mflash = d3dg->LoadTexture("data\\mflash.dds", false, false);
	effecttexture_smoke = d3dg->LoadTexture("data\\smoke.dds", false, false);
	effecttexture_yakkyou = d3dg->LoadTexture("data\\yakkyou.dds", false, false);

	if( effecttexture_blood == -1 ){ return 1; }
	if( effecttexture_mflash == -1 ){ return 1; }
	if( effecttexture_smoke == -1 ){ return 1; }
	if( effecttexture_yakkyou == -1 ){ return 1; }
	return 0;
}

//! @brief 뙆긡긏긚?긿롦벦
//! @return 긡긏긚?긿궻긢??붥뜂
int ResourceManager::GetEffectBloodTexture()
{
	return effecttexture_blood;
}

//! @brief 긲깋긞긘깄긡긏긚?긿롦벦
//! @return 긡긏긚?긿궻긢??붥뜂
int ResourceManager::GetEffectMflashTexture()
{
	return effecttexture_mflash;
}

//! @brief 뎹긡긏긚?긿롦벦
//! @return 긡긏긚?긿궻긢??붥뜂
int ResourceManager::GetEffectSmokeTexture()
{
	return effecttexture_smoke;
}

//! @brief 뽶娥긡긏긚?긿롦벦
//! @return 긡긏긚?긿궻긢??붥뜂
int ResourceManager::GetEffectYakkyouTexture()
{
	return effecttexture_yakkyou;
}

//! @brief 긄긲긃긏긣궻긡긏긚?긿귩됶뺳
void ResourceManager::CleanupEffectTexture()
{
	if( d3dg == NULL ){ return; }

	if( effecttexture_blood != -1 ){
		d3dg->CleanupTexture(effecttexture_blood);
		effecttexture_blood = -1;
	}
	if( effecttexture_mflash != -1 ){
		d3dg->CleanupTexture(effecttexture_mflash);
		effecttexture_mflash = -1;
	}
	if( effecttexture_smoke != -1 ){
		d3dg->CleanupTexture(effecttexture_smoke);
		effecttexture_smoke = -1;
	}
	if( effecttexture_yakkyou != -1 ){
		d3dg->CleanupTexture(effecttexture_yakkyou);
		effecttexture_yakkyou = -1;
	}
}