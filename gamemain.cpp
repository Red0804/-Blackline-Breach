//! @file gamemain.cpp
//! @brief 긒??긽귽깛룉뿚

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
// 겍긽긦깄?볙궻깋귽긜깛긚빒뤞궼 mainmenu::CreateOptions() 볙궳믦?궠귢궲궋귡갃

#include "gamemain.h"
#include "playerskill.h"

#include <io.h>
#include <stdlib.h>

#if defined(_MSC_VER)
#pragma execution_character_set("utf-8")
#endif

D3DGraphics d3dg;			//!< ?됪긏깋긚
InputControl inputCtrl;		//!< 볺쀍롦벦긏깋긚
SoundControl SoundCtrl;		//!< 긖긂깛긤띋맯긏깋긚

//! 긒??궻먠믦뭠
ParameterInfo GameParamInfo;

//! 깏??긚듖뿚
ResourceManager Resource;

SoundManager GameSound;		//!< 긒??뚼됈돶띋맯긏깋긚

BlockDataInterface BlockData;		//!< 긳깓긞긏긢??듖뿚긏깋긚
PointDataInterface PointData;		//!< ?귽깛긣긢??듖뿚긏깋긚
MIFInterface MIFdata;				//!< MIF긓깛긣깓?깑
AddonList GameAddon;				//!< addon궻깏긚긣
Collision CollD;					//!< 뱰궫귟뵽믦듖뿚긏깋긚
ObjectManager ObjMgr;				//!< 긆긳긙긃긏긣듖뿚긏깋긚
AIcontrol HumanAI[MAX_HUMAN];		//!< AI듖뿚긏깋긚

GameInfo GameInfoData;				//!< 긒??궻륉뫴

// TRAINING 미션 목록.
// training\beginner, training\advanced, training\skill 안의 .mif를 읽어서 만든다.
#define MAX_TRAINING_MISSION 64

// 메뉴 미션 이름 버퍼 크기.
// 화면 표시에서는 별도 함수로 ... 처리한다.
#define TRAINING_MISSION_NAME_MAX 64
#define MENU_MISSION_NAME_MAX 128

// TRAINING 미션 폴더
#define TRAINING_BEGINNER_DIR "training\\beginner"
#define TRAINING_ADVANCED_DIR "training\\advanced"
#define TRAINING_SKILL_DIR "training\\skill"

typedef struct {
	char name[TRAINING_MISSION_NAME_MAX];
	char path[MAX_PATH];
	int order;
	int group;	// 0: beginner, 1: advanced, 2: skill
} TrainingMissionInfo;

static TrainingMissionInfo TrainingMissionData[MAX_TRAINING_MISSION];
static int TotalTrainingMission = 0;

static void TrimLineEnd(char* str)
{
	if (str == NULL) { return; }

	int len = (int)strlen(str);
	while (len > 0) {
		char c = str[len - 1];
		if ((c == '\r') || (c == '\n')) {
			str[len - 1] = '\0';
			len--;
		}
		else {
			break;
		}
	}
}

static int GetTrainingMissionOrder(const char* name)
{
	if (name == NULL) { return 9999; }

	// "00-Movement", "20-Mission 01"처럼 앞 번호를 기준으로 정렬한다.
	if ((name[0] >= '0') && (name[0] <= '9')) {
		return atoi(name);
	}

	return 9999;
}

static void ReadTrainingMissionName(const char* path, char* out_name, int* out_order)
{
	if (out_name != NULL) {
		strcpy(out_name, "UNKNOWN");
	}
	if (out_order != NULL) {
		*out_order = 9999;
	}

	FILE* fp = fopen(path, "r");
	if (fp == NULL) {
		return;
	}

	char line[MENU_MISSION_NAME_MAX];
	if (fgets(line, sizeof(line), fp) != NULL) {
		TrimLineEnd(line);

		if (out_name != NULL) {
			strncpy(out_name, line, TRAINING_MISSION_NAME_MAX - 1);
			out_name[TRAINING_MISSION_NAME_MAX - 1] = '\0';
		}
		if (out_order != NULL) {
			*out_order = GetTrainingMissionOrder(line);
		}
	}

	fclose(fp);
}

static void AddTrainingMissionFile(const char* path, int group)
{
	if (path == NULL) { return; }
	if (TotalTrainingMission >= MAX_TRAINING_MISSION) { return; }

	TrainingMissionInfo* info = &TrainingMissionData[TotalTrainingMission];

	strcpy(info->path, path);
	info->group = group;
	ReadTrainingMissionName(path, info->name, &info->order);

	TotalTrainingMission++;
}

static void LoadTrainingMissionFolder(const char* folder, int group)
{
	if (folder == NULL) { return; }

	char search_path[MAX_PATH];
	sprintf(search_path, "%s\\*.mif", folder);

	struct _finddata_t fileinfo;
	intptr_t handle = _findfirst(search_path, &fileinfo);

	if (handle == -1) {
		return;
	}

	do {
		if ((fileinfo.attrib & _A_SUBDIR) == 0) {
			char path[MAX_PATH];
			sprintf(path, "%s\\%s", folder, fileinfo.name);
			AddTrainingMissionFile(path, group);
		}
	} while (_findnext(handle, &fileinfo) == 0);

	_findclose(handle);
}

static void SortTrainingMissionList()
{
	for (int i = 0; i < TotalTrainingMission - 1; i++) {
		for (int j = i + 1; j < TotalTrainingMission; j++) {
			bool swapflag = false;

			if (TrainingMissionData[i].order > TrainingMissionData[j].order) {
				swapflag = true;
			}
			else if (TrainingMissionData[i].order == TrainingMissionData[j].order) {
				if (strcmp(TrainingMissionData[i].name, TrainingMissionData[j].name) > 0) {
					swapflag = true;
				}
			}

			if (swapflag == true) {
				TrainingMissionInfo temp = TrainingMissionData[i];
				TrainingMissionData[i] = TrainingMissionData[j];
				TrainingMissionData[j] = temp;
			}
		}
	}
}

static void LoadTrainingMissionList()
{
	TotalTrainingMission = 0;

	// TRAINING 미션 폴더:
	// training\beginner\*.mif
	// training\advanced\*.mif
	// training\skill\*.mif
	LoadTrainingMissionFolder(TRAINING_BEGINNER_DIR, 0);
	LoadTrainingMissionFolder(TRAINING_ADVANCED_DIR, 1);
	LoadTrainingMissionFolder(TRAINING_SKILL_DIR, 2);

	SortTrainingMissionList();
}

// 메뉴 목록에 표시할 미션 이름을 만든다.
// 원본 이름은 유지하고, 화면 폭을 넘는 경우에만 뒤를 ...로 줄인다.
// 주의: 영어/숫자 기준이다. 한글/일본어처럼 멀티바이트 문자는 중간에서 잘릴 수 있다.
static void MakeMenuMissionDisplayName(const char* src, char* dst, int dst_size)
{
	if ((dst == NULL) || (dst_size <= 0)) {
		return;
	}

	dst[0] = '\0';

	if (src == NULL) {
		return;
	}

	// 메뉴 오른쪽 끝에 너무 붙지 않도록 약간 여백을 둔다.
	int max_width = MAINMENU_LIST_W - (int)(8 * MAINMENU_SCALE);
	int max_chars = max_width / MAINMENU_MISSION_FONT_W;

	if (max_chars < 4) {
		max_chars = 4;
	}

	if (max_chars > dst_size - 1) {
		max_chars = dst_size - 1;
	}

	int len = (int)strlen(src);

	if (len <= max_chars) {
		strncpy(dst, src, dst_size - 1);
		dst[dst_size - 1] = '\0';
		return;
	}

	int copy_chars = max_chars - 3;

	if (copy_chars < 1) {
		copy_chars = 1;
	}

	strncpy(dst, src, copy_chars);
	dst[copy_chars] = '\0';
	strcat(dst, "...");
}

// TRAINING 내부 탭에 따라 표시할 미션인지 확인한다.
// TUTORIAL 탭: beginner + advanced
// SKILL 탭: skill
static bool CheckTrainingMissionVisibleByTab(int data_index, int tab)
{
	if ((data_index < 0) || (TotalTrainingMission <= data_index)) {
		return false;
	}

	if (tab == TRAINING_TAB_SKILL) {
		return (TrainingMissionData[data_index].group == 2);
	}

	return (TrainingMissionData[data_index].group != 2);
}

static int GetTrainingMissionCountByTab(int tab)
{
	int count = 0;

	for (int i = 0; i < TotalTrainingMission; i++) {
		if (CheckTrainingMissionVisibleByTab(i, tab) == true) {
			count++;
		}
	}

	return count;
}

static int GetTrainingMissionRealIndexByTab(int tab, int visible_index)
{
	int count = 0;

	for (int i = 0; i < TotalTrainingMission; i++) {
		if (CheckTrainingMissionVisibleByTab(i, tab) == false) {
			continue;
		}

		if (count == visible_index) {
			return i;
		}

		count++;
	}

	return -1;
}

EventControl Event[TOTAL_EVENTLINE];	//!< 귽긹깛긣맕뚥긏깋긚

//! @brief 딈?밒궶룊딖돸룉뿚
int InitGame(WindowControl *WindowCtrl, int mode, char *MIFpath)
{
	if( WindowCtrl == NULL ){ return 1; }

	//D3DGraphics긏깋긚룊딖돸
	if( d3dg.InitD3D(WindowCtrl, "data\\char.dds", GameConfig.GetFullscreenFlag(), GameConfig.GetLanguage()) ){
		WindowCtrl->ErrorInfo("Direct3D궻띿맟궸렪봲궢귏궢궫");
		return 1;
	}

	//InputControl긏깋긚룊딖돸
	if( inputCtrl.InitInput(WindowCtrl) ){
		WindowCtrl->ErrorInfo("Input initialization error");
		return 1;
	}

	//SoundControl긏깋긚룊딖돸
	if( SoundCtrl.InitSound(WindowCtrl) ){
		WindowCtrl->ErrorInfo("dll open failed");
		return 1;
	}


	//먠믦뭠귩룊딖돸
	GameParamInfo.InitInfo();

	//깏??긚궻룊딖먠믦
	Resource.SetParameterInfo(&GameParamInfo);
	Resource.SetD3DGraphics(&d3dg);
	Resource.SetSoundControl(&SoundCtrl);

	//깏??긚귩룊딖돸
	Resource.LoadHumanModel();
	Resource.LoadWeaponModelTexture();
	Resource.LoadWeaponSound();
	Resource.LoadNVGSound();
	Resource.LoadSkillSound();

	Resource.LoadSmallObjectModelTexture();
	Resource.LoadSmallObjectSound();
	Resource.LoadScopeTexture();
	Resource.LoadSkillIconTexture();
	Resource.LoadBulletModelTexture();
	Resource.LoadBulletSound();
	Resource.LoadEffectTexture();
	Resource.LoadFootstepSound();

	//볺쀍룉뿚먠믦
	inputCtrl.SetMouseLimitFlag( GameConfig.GetMouseLimitFlag() );

	//뚼됈돶룊딖돸
	float volume;
	if( GameConfig.GetSoundFlag() == false ){
		volume = 0.0f;
	}
	else{
#ifdef ENABLE_ADDCONFIG
		volume = GameConfig.GetVolume();
#else
		volume = 1.0f;
#endif
	}
	GameSound.SetClass(&SoundCtrl, &Resource, &GameParamInfo);
	SoundCtrl.SetVolume(volume);

	//긆긳긙긃긏긣?긨?긙긿?룊딖돸
	ObjMgr.SetClass(&GameParamInfo, &d3dg, &Resource, &BlockData, &PointData, &CollD, &GameSound, &MIFdata);

	if( (mode != 0)&&(MIFpath != NULL) ){
		//겍?긞긘깈깛귩뮳먝딳벍궥귡궶귞궽갂addon깏긚긣궼븉뾴궳궇귡궫귕띿맟궢궶궋갃

		//딳벍궥귡?긞긘깈깛먠믦
		GameInfoData.selectmission_id = -1;
		strcpy(GameInfoData.mifpath, MIFpath);
		GameState.SetStateMode(mode);
		if( mode == 2 ){
			//mif긲?귽깑귩벶귒뜛귒
			if( MIFdata.LoadFiledata(MIFpath, true) != 0 ){
				WindowCtrl->ErrorInfo("briefing data open failed");
				return 1;
			}
		}
	}
	else{
		//addon깏긚긣띿맟
		char str[16];
		sprintf(str, "%s\\", ADDONDIR);
		GameAddon.LoadFiledata(str);

		GameInfoData.selectmission_id = 0;
		strcpy(GameInfoData.mifpath, "");
	}

	return 0;
}

//! @brief 깏??긚귩깏긜긞긣궥귡
//! @return 맟뚻갌0?렪봲갌-1?뫲?갌1
//! @attention 믅륂궼갂?됪룉뿚궸렪봲궢궫뤾뜃궸뙽귟뚁귂뢯궢궲궘궬궠궋갃
int ResetGame(WindowControl *WindowCtrl)
{
	if( WindowCtrl == NULL ){ return -1; }

	//깏긚긣귩맫궢궘됶뺳궥귡궫귕갂?귕뚁귆갃
	Resource.CleanupHumanTexture();

	int ReturnCode = d3dg.ResetD3D(WindowCtrl);

	if( ReturnCode == 0 ){
		//깏??긚귩룊딖돸
		Resource.LoadHumanModel();
		Resource.LoadWeaponModelTexture();
		Resource.LoadSmallObjectModelTexture();
		Resource.LoadScopeTexture();
		Resource.LoadBulletModelTexture();
		Resource.LoadEffectTexture();

		//WindowCtrl->ErrorInfo("Recovery...");
		return 0;
	}
	if( ReturnCode == 1 ){
		return 1;
	}
	//if( ReturnCode == 2 ){
		//WindowCtrl->ErrorInfo("Reset궸렪봲궢귏궢궫");
		//WindowCtrl->CloseWindow();
		return -1;
	//}
}

//! @brief 딈?밒궶됶뺳룉뿚
void CleanupGame()
{
	//깏??긚귩됶뺳
	Resource.DestroyResource();

	//먠믦뭠귩됶뺳
	GameParamInfo.DestroyInfo();


	//SoundControl긏깋긚됶뺳
	SoundCtrl.DestroySound();

	//InputControl긏깋긚됶뺳
	inputCtrl.DestroyInput();

	//D3DGraphics긏깋긚됶뺳
	d3dg.DestroyD3D();
}

//! @brief 긓깛긚긣깋긏?
opening::opening()
{
	add_camera_x = 0.0f;
	add_camera_y = 0.0f;
	add_camera_z = 0.0f;
	add_camera_rx = 0.0f;
	add_camera_ry = 0.0f;
}

//! @brief 긢귻긚긣깋긏?
opening::~opening()
{}

int opening::Create()
{
	int blockflag, pointflag;

	//긳깓긞긏긢??벶귒뜛귒
	blockflag = BlockData.LoadFiledata("data\\map10\\temp.bd1");

	//?귽깛긣긢??벶귒뜛귒
	pointflag = PointData.LoadFiledata("data\\map10\\op.pd1");

	//긲?귽깑벶귒뜛귒궳긄깋?궕궇궯궫귞뭷뭚
	if( (blockflag != 0)||(pointflag != 0) ){
		//2bit : point data open failed
		//1bit : block data open failed
		return pointflag*2 + blockflag;
	}

	//긳깓긞긏긢??룊딖돸
	BlockData.CalculationBlockdata(false);
	d3dg->LoadMapdata(&BlockData, "data\\map10\\");
	CollD.InitCollision(&BlockData);

	//?귽깛긣긢??룊딖돸
	ObjMgr.LoadPointData();
	ObjMgr.SetPlayerID(MAX_HUMAN-1);	//렳띪궢궶궋릐귩긵깒귽깂?궸걁뢤맳궻긖긂깛긤띋맯뫮랉걂

	//믁돿궻뱰궫귟뵽믦먠믦
	ObjMgr.SetAddCollisionFlag(false);

	//AI먠믦
	for (int i = 0; i < MAX_HUMAN; i++) {
		HumanAI[i].SetClass(&ObjMgr, i, &BlockData, &PointData, &GameParamInfo, &CollD, GameSound);
		HumanAI[i].Init();
		HumanAI[i].SetForceNoAI(ObjMgr.GetHumanForceNoAI(i));
	}

	//봶똧뗴벶귒뜛귒
	Resource.LoadSkyModelTexture(1);

	//opening_banner = d3dg->LoadTexture("banner.png", true, false);

	//긖긂깛긤룊딖돸
	GameSound->InitWorldSound();

	//?긂긚긇??깑귩뭷돍귉댷벍
	inputCtrl->MoveMouseCenter();
	framecnt = 0;

	camera_x = -5.0f;
	camera_y = 58.0f;
	camera_z = 29.0f;
	camera_rx = DegreeToRadian(206);
	camera_ry = DegreeToRadian(12);
	add_camera_x = 0.0f;
	add_camera_y = 0.0f;
	add_camera_z = 0.0f;
	add_camera_rx = 0.0f;
	add_camera_ry = 0.0f;

	GameState->NextState();
	return 0;
}

int opening::Recovery()
{
	//긳깓긞긏긢??룊딖돸
	d3dg->LoadMapdata(&BlockData, "data\\map10\\");

	//?귽깛긣긢??룊딖돸
	ObjMgr.Recovery();

	//봶똧뗴룊딖돸
	Resource.CleanupSkyModelTexture();
	Resource.LoadSkyModelTexture(1);

	//opening_banner = d3dg->LoadTexture("banner.png", true, false);

	return 0;
}

void opening::Input()
{
	inputCtrl->GetInputState(false);

	if( inputCtrl->CheckKeyDown(GetEscKeycode()) ){
		GameState->PushBackSpaceKey();
	}
	else if( inputCtrl->CheckMouseButtonUpL() ){
		GameState->PushMouseButton();
	}
	else if( inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x0F)) ){		// [ENTER]
		//[ENTER]귩돓궢궲귖갂?긂긚귩긏깏긞긏궢궫궞궴궸궥귡갃
		GameState->PushMouseButton();
	}
}

//! @todo 긇긽깋궻댷벍귩뒍귞궔궸궥귡
void opening::Process()
{
	//긆긳긙긃긏긣?긨?긙긿?귩렳뛱
	ObjMgr.Process(-1, false, false);

	//AI귩렳뛱
	for(int i=0; i<MAX_HUMAN; i++){
		HumanAI[i].Process();
	}

	//긇긽깋깗?긏귩땫귕귡걁띆뷭걂
	if( framecnt < (int)(4.0f*GAMEFPS) ){
		add_camera_z = 0.0f;
		add_camera_y = 0.0f;
	}
	else if( framecnt < (int)(5.0f*GAMEFPS) ){
		add_camera_z += (0.08f - add_camera_z) / 5.0f;
		add_camera_y += (-0.05f - add_camera_y) / 5.0f;
	}
	else{
		add_camera_z = 0.08f;
		add_camera_y = -0.05f;
	}
	camera_x += add_camera_x;
	camera_y += add_camera_y;
	camera_z += add_camera_z;

	//긇긽깋깗?긏귩땫귕귡걁됷?걂
	if( framecnt < (int)(2.6f*GAMEFPS) ){
		add_camera_rx = 0.0f;
		add_camera_ry = 0.0f;
	}
	else if( framecnt < (int)(3.6f*GAMEFPS) ){
		add_camera_rx += (DegreeToRadian(0.9f) - add_camera_rx) / 5.0f;
		add_camera_ry += (DegreeToRadian(-0.6f) - add_camera_ry) / 5.0f;
	}
	else if( framecnt < (int)(5.0f*GAMEFPS) ){
		add_camera_rx = DegreeToRadian(0.9f);
		add_camera_ry = DegreeToRadian(-0.6f);
	}
	else{
		add_camera_rx *= 0.8f;
		add_camera_ry *= 0.8f;
	}
	camera_rx += add_camera_rx;
	camera_ry += add_camera_ry;

	//16초가 지나면 오프닝 종료
	if (framecnt >= 16 * ((int)GAMEFPS)) {
		GameState->PushMouseButton();
	}

	framecnt += 1;
}

void opening::Render3D()
{
	int skymodel, skytexture;

	//긲긅긐궴긇긽깋귩먠믦
	d3dg->SetFog(true, 1);
	d3dg->SetCamera(camera_x, camera_y, camera_z, camera_rx, camera_ry, VIEWANGLE_NORMAL);

	//긇긽깋띆뷭궸봶똧뗴귩?됪
	d3dg->SetWorldTransform(camera_x, camera_y, camera_z, 0.0f, 0.0f, 2.0f);
	Resource.GetSkyModelTexture(&skymodel, &skytexture);
	d3dg->RenderModel(skymodel, skytexture, false, false, false);

	//Z긫긞긲?귩룊딖돸
	d3dg->ResetZbuffer();

	//?긞긵귩?됪
	d3dg->ResetWorldTransform();
	d3dg->RenderMapdata(false);

	//긆긳긙긃긏긣귩?됪
	ObjMgr.Render(camera_x, camera_y, camera_z, camera_rx, camera_ry, 0, false);
}

void opening::Render2D()
{
	float effect = 0.0f;

	//float scaling_x = (float)GameConfig.GetScreenWidth() / 640;
	float scaling_y = (float)GameConfig.GetScreenHeight() / 480;
	int swidth = (int)((float)GameConfig.GetScreenWidth() / GameConfig.GetScreenHeight() * 480);

	//긳깋긞긏귺긂긣먠믦
	if( framecnt < (int)(2.0f*GAMEFPS) ){
		effect = GetEffectAlpha(framecnt, 1.0f, 2.0f, 0.0f, true);
	}
	if( ((int)(2.0f*GAMEFPS) <= framecnt)&&(framecnt < (int)(11.0f*GAMEFPS)) ){
		effect = 0.0f;
	}
	if( ((int)(11.0f*GAMEFPS) <= framecnt)&&(framecnt < (int)(15.0f*GAMEFPS)) ){
		effect = GetEffectAlpha(framecnt, 1.0f, 4.0f, 11.0f, false);
	}
	if( (int)(15.0f*GAMEFPS) <= framecnt ){
		effect = 1.0f;
	}
	d3dg->Draw2DBox(0, 0, GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight(), d3dg->GetColorCode(0.0f,0.0f,0.0f,effect));

	//뤵돷궻뜒뎼?됪
	d3dg->Draw2DBox(0, 0, GameConfig.GetScreenWidth(), (int)(scaling_y * 40), d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
	d3dg->Draw2DBox(0, GameConfig.GetScreenHeight() - (int)(scaling_y * 40), GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight(), d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));

	//긵깓긙긃긏긣뼹
	if( ((int)(0.5f*GAMEFPS) < framecnt)&&(framecnt < (int)(4.0f*GAMEFPS)) ){
		char str[32];
		float effectA = 1.0f;
		sprintf(str, "%s project", GAMENAME);
		if( framecnt < (int)(1.5f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 0.5f, false); }
		if( framecnt > (int)(3.0f*GAMEFPS) ){ effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 3.0f, true); }
		d3dg->Draw2DTextureFontTextCenterScaling(0, 340, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,effectA), 22, 22);
	}

	// Staff name 1 - Original X operations
	if (((int)(4.5f * GAMEFPS) < framecnt) && (framecnt < (int)(8.5f * GAMEFPS))) {
		float effectA = 1.0f;
		if (framecnt < (int)(5.5f * GAMEFPS)) { effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 4.5f, false); }
		if (framecnt > (int)(7.5f * GAMEFPS)) { effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 7.5f, true); }
		d3dg->Draw2DTextureFontTextScaling(60, 130, "ORIGINAL", d3dg->GetColorCode(1.0f, 1.0f, 1.0f, effectA), 20, 20);
	}
	if (((int)(5.0f * GAMEFPS) < framecnt) && (framecnt < (int)(9.0f * GAMEFPS))) {
		float effectA = 1.0f;
		if (framecnt < (int)(6.0f * GAMEFPS)) { effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 5.0f, false); }
		if (framecnt > (int)(8.0f * GAMEFPS)) { effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 8.0f, true); }
		d3dg->Draw2DTextureFontTextScaling(100, 160, "nine-two", d3dg->GetColorCode(1.0f, 1.0f, 1.0f, effectA), 20, 20);
		d3dg->Draw2DTextureFontTextScaling(100, 190, "TENNKUU", d3dg->GetColorCode(1.0f, 1.0f, 1.0f, effectA), 20, 20);
	}

	// Staff name 2 - OpenXOPS remake
	if (((int)(9.2f * GAMEFPS) < framecnt) && (framecnt < (int)(12.7f * GAMEFPS))) {
		float effectA = 1.0f;
		if (framecnt < (int)(10.2f * GAMEFPS)) { effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 9.2f, false); }
		if (framecnt > (int)(11.7f * GAMEFPS)) { effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 11.7f, true); }
		d3dg->Draw2DTextureFontTextScaling(swidth - 310, 250, "REMAKE", d3dg->GetColorCode(1.0f, 1.0f, 1.0f, effectA), 20, 20);
	}
	if (((int)(9.7f * GAMEFPS) < framecnt) && (framecnt < (int)(13.2f * GAMEFPS))) {
		float effectA = 1.0f;
		if (framecnt < (int)(10.7f * GAMEFPS)) { effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 9.7f, false); }
		if (framecnt > (int)(12.2f * GAMEFPS)) { effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 12.2f, true); }
		d3dg->Draw2DTextureFontTextScaling(swidth - 270, 280, "[-_-;](mikan)", d3dg->GetColorCode(1.0f, 1.0f, 1.0f, effectA), 20, 20);
	}
	// 게임명 + 개조 제작자 표시
	if ((int)(12.0f * GAMEFPS) <= framecnt) {	//framecnt < (int)(16.0f*GAMEFPS)
		char str[32];
		float effectA = 1.0f;

		sprintf(str, "%s", GAMENAME);

		if (framecnt < (int)(13.0f * GAMEFPS)) {
			effectA = GetEffectAlpha(framecnt, 1.0f, 1.0f, 12.0f, false);
		}
		if (((int)(15.0f * GAMEFPS) < framecnt) && (framecnt < (int)(15.5f * GAMEFPS))) {
			effectA = GetEffectAlpha(framecnt, 1.0f, 0.5f, 15.0f, true);
		}
		if (framecnt >= (int)(15.5f * GAMEFPS)) {
			effectA = 0.0f;
		}

		// 게임 제목
		d3dg->Draw2DTextureFontTextCenterScaling(
			0,
			(480 - 11) / 2,
			str,
			d3dg->GetColorCode(1.0f, 0.0f, 0.0f, effectA),
			22,
			22
		);

		// 개조 제작자 표시
		d3dg->Draw2DTextureFontTextCenterScaling(
			0,
			280,
			"MODIFIED",
			d3dg->GetColorCode(1.0f, 1.0f, 1.0f, effectA),
			18,
			18
		);

		d3dg->Draw2DTextureFontTextCenterScaling(
			0,
			305,
			"SniperRed",
			d3dg->GetColorCode(1.0f, 1.0f, 1.0f, effectA),
			18,
			18
		);
	}
}

void opening::Destroy()
{
	//긳깓긞긏긢??됶뺳
	d3dg->CleanupMapdata();

	//긆긳긙긃긏긣?긨?긙긿?됶뺳
	ObjMgr.Cleanup();

	//봶똧뗴됶뺳
	Resource.CleanupSkyModelTexture();

	//d3dg->CleanupTexture(opening_banner);

	GameState->NextState();
}

//! @brief 긓깛긚긣깋긏?
mainmenu::mainmenu()
{
	demopath[0] = '\0';
	modescreen = 0;
	mission_list_mode = MISSION_LIST_STANDARD;
	training_list_tab = TRAINING_TAB_TUTORIAL;
	mainmenu_mouseX = 0;
	mainmenu_mouseY = 0;
	mainmenu_mouseY_old = mainmenu_mouseY;
	gametitle = -1;

#ifdef ENABLE_MENUOPTIONS
	for (int i = 0; i < OPTIONS_ADVANCED_DATAS; i++) {
		Options_AdvancedLinkData[i].enable = false;      // 활성화 플래그 끄기
		Options_AdvancedLinkData[i].text[0] = '\0';      // 텍스트 비우기
		Options_AdvancedLinkData[i].pos_x = 0;
		Options_AdvancedLinkData[i].pos_y = 0;
		// 색상 초기화 (검은색 등 안전한 값으로)
		Options_AdvancedLinkData[i].text_defaultcolor = 0;
		Options_AdvancedLinkData[i].text_cursorcolor = 0;
	}

	Options_AdvancedID = -1;
	IsDraggingSlider = false;
	DraggingID = -1;

	for (int i = 0; i < OPTIONS_TOGGLE_DATAS; i++) {
		Options_ToggleLinkData[i].enable = false;
		Options_ToggleLinkData[i].text[0] = '\0';
		Options_ToggleLinkData[i].pos_x = 0;
		Options_ToggleLinkData[i].pos_y = 0;
		Options_ToggleLinkData[i].text_defaultcolor = 0;
		Options_ToggleLinkData[i].text_cursorcolor = 0;
	}
	Options_ToggleID = -1;

	for (int i = 0; i < SKILLINFO_GROUP_LINK_DATAS; i++) {
		SkillInfo_GroupLinkData[i].enable = false;
		SkillInfo_GroupLinkData[i].text[0] = '\0';
		SkillInfo_GroupLinkData[i].pos_x = 0;
		SkillInfo_GroupLinkData[i].pos_y = 0;
		SkillInfo_GroupLinkData[i].text_defaultcolor = 0;
		SkillInfo_GroupLinkData[i].text_cursorcolor = 0;
	}

	for (int i = 0; i < SKILLINFO_LANG_DATAS; i++) {
		SkillInfo_LangLinkData[i].enable = false;
		SkillInfo_LangLinkData[i].text[0] = '\0';
		SkillInfo_LangLinkData[i].pos_x = 0;
		SkillInfo_LangLinkData[i].pos_y = 0;
		SkillInfo_LangLinkData[i].text_defaultcolor = 0;
		SkillInfo_LangLinkData[i].text_cursorcolor = 0;
	}

	SkillInfo_SelectedGroup = 0;

	// 기본 언어.
	// 한글/일본어 폰트 표시가 확인되면 SKILLINFO_LANG_KO로 바꿔도 된다.
	SkillInfo_Language = SKILLINFO_LANG_EN;

	for (int i = 0; i < OPTIONS_P1_DATAS; i++) {
		Options_p1LinkTextData[i].enable = false;
	}
	for( int i=0; i<OPTIONS_P2_DATAS; i++ ){
		Options_p2LinkTextData[i].enable = false;
	}
	for( int i=0; i<OPTIONS_P3_DATAS; i++ ){
		Options_p3LinkTextData[i].enable = false;
	}
	Options_p2LinkTextID = -1;
	Options_FullscreenFlag_old = GameConfig.GetFullscreenFlag();
	Options_KeyConfigID = -1;
	Options_KeyConfigCnt = 0;
	licenseinfo[0] = "";
	licenseinfobuf = new char[100*18];
	for( int i=0; i<OPTIONS_LICENSELINE; i++ ){
		licenseinfobuf[i] = '\0';
	}
#endif
}

//! @brief 긢귻긚긣깋긏?
mainmenu::~mainmenu()
{
#ifdef ENABLE_MENUOPTIONS
	if( licenseinfobuf != NULL ){
		delete [] licenseinfobuf;
		licenseinfobuf = NULL;
	}
#endif
}

int mainmenu::Create()
{
	char bdata[MAX_PATH];
	char pdata[MAX_PATH];
	int blockflag, pointflag;

	//긢긾귩뙂믦궢벶귒뜛귔
	switch( GetRand(6) ){
		case 0:
			strcpy(demopath, "data\\map2\\");
			break;
		case 1:
			strcpy(demopath, "data\\map4\\");
			break;
		case 2:
			strcpy(demopath, "data\\map5\\");
			break;
		case 3:
			strcpy(demopath, "data\\map7\\");
			break;
		case 4:
			strcpy(demopath, "data\\map8\\");
			break;
		case 5:
			strcpy(demopath, "data\\map16\\");
			break;
	}
	strcpy(bdata, demopath);
	strcat(bdata, "temp.bd1");
	strcpy(pdata, demopath);
	strcat(pdata, "demo.pd1");


	//긳깓긞긏긢??벶귒뜛귒
	blockflag = BlockData.LoadFiledata(bdata);

	//?귽깛긣긢??벶귒뜛귒
	pointflag = PointData.LoadFiledata(pdata);

	//긲?귽깑벶귒뜛귒궳긄깋?궕궇궯궫귞뭷뭚
	if( (blockflag != 0)||(pointflag != 0) ){
		//2bit : point data open failed
		//1bit : block data open failed
		return pointflag*2 + blockflag;
	}

	//긳깓긞긏긢??룊딖돸
	BlockData.CalculationBlockdata(false);
	d3dg->LoadMapdata(&BlockData, demopath);
	CollD.InitCollision(&BlockData);

	//?귽깛긣긢??룊딖돸
	ObjMgr.LoadPointData();

	// ==========================================================
	// ★ [추가할 코드] 맵 로딩 직후, 무기 랜덤 변경 함수 호출 ★
	// ==========================================================
	// GameParamInfo: 전역 변수로 선언된 파라미터 정보
	// Resource: 전역 변수로 선언된 리소스 관리자
	ObjMgr.RandomizeMapWeaponsByCustomGroup(&GameParamInfo, &Resource);

	//믁돿궻뱰궫귟뵽믦먠믦
	ObjMgr.SetAddCollisionFlag(false);

	//AI設定
	for (int i = 0; i < MAX_HUMAN; i++) {
		HumanAI[i].SetClass(&ObjMgr, i, &BlockData, &PointData, &GameParamInfo, &CollD, GameSound);
		HumanAI[i].Init();
		HumanAI[i].SetForceNoAI(ObjMgr.GetHumanForceNoAI(i));
	}

	modescreen = 0;

	gametitle = d3dg->LoadTexture("data\\title.dds", false, false);

	//긖긂깛긤룊딖돸
	GameSound->InitWorldSound();

	mainmenu_mouseX = GameConfig.GetScreenWidth()/2;
	mainmenu_mouseY = GameConfig.GetScreenHeight()/2;
	mainmenu_mouseY_old = mainmenu_mouseY;

	// TRAINING 미션 목록 생성
	LoadTrainingMissionList();

	//?긞긘깈깛궻긚긏깓?깑긫?궻먠믦
	// 미션 스크롤바 설정
	scrollbar_official.Create(
		MAINMENU_X + MAINMENU_LIST_W + 1,
		MAINMENU_Y + 1,
		MAINMENU_SCROLLBAR_W,
		MAINMENU_H - MAINMENU_SWITCH_H,
		TOTAL_OFFICIALMISSION,
		TOTAL_MENUITEMS,
		scrollbar_official.GetScrollItem()
	);
	scrollbar_official.SetColor(d3dg->GetColorCode(0.5f, 0.5f, 0.5f, 0.5f), d3dg->GetColorCode(0.6f, 0.6f, 0.25f, 1.0f), d3dg->GetColorCode(0.8f, 0.8f, 0.25f, 1.0f),
		d3dg->GetColorCode(0.4f, 0.67f, 0.57f, 1.0f), d3dg->GetColorCode(0.38f, 0.77f, 0.64f, 1.0f), d3dg->GetColorCode(0.6f, 0.3f, 0.25f, 1.0f), d3dg->GetColorCode(0.8f, 0.3f, 0.25f, 1.0f));

	scrollbar_training.Create(
		MAINMENU_X + MAINMENU_LIST_W + 1,
		MAINMENU_Y + 1,
		MAINMENU_SCROLLBAR_W,
		MAINMENU_H - MAINMENU_SWITCH_H,
		GetTrainingMissionCountByTab(training_list_tab),
		TOTAL_MENUITEMS,
		scrollbar_training.GetScrollItem()
	);
	scrollbar_training.SetColor(d3dg->GetColorCode(0.5f, 0.5f, 0.5f, 0.5f), d3dg->GetColorCode(0.6f, 0.6f, 0.25f, 1.0f), d3dg->GetColorCode(0.8f, 0.8f, 0.25f, 1.0f),
		d3dg->GetColorCode(0.4f, 0.67f, 0.57f, 1.0f), d3dg->GetColorCode(0.38f, 0.77f, 0.64f, 1.0f), d3dg->GetColorCode(0.6f, 0.3f, 0.25f, 1.0f), d3dg->GetColorCode(0.8f, 0.3f, 0.25f, 1.0f));

	scrollbar_addon.Create(
		MAINMENU_X + MAINMENU_LIST_W + 1,
		MAINMENU_Y + 1,
		MAINMENU_SCROLLBAR_W,
		MAINMENU_H - MAINMENU_SWITCH_H,
		GameAddon.GetTotaldatas(),
		TOTAL_MENUITEMS,
		scrollbar_addon.GetScrollItem()
	);
	scrollbar_addon.SetColor(d3dg->GetColorCode(0.5f, 0.5f, 0.5f, 0.5f), d3dg->GetColorCode(0.6f, 0.6f, 0.25f, 1.0f), d3dg->GetColorCode(0.8f, 0.8f, 0.25f, 1.0f),
		d3dg->GetColorCode(0.4f, 0.67f, 0.57f, 1.0f), d3dg->GetColorCode(0.38f, 0.77f, 0.64f, 1.0f), d3dg->GetColorCode(0.6f, 0.3f, 0.25f, 1.0f), d3dg->GetColorCode(0.8f, 0.3f, 0.25f, 1.0f));

#ifdef ENABLE_MENUOPTIONS
	CreateOptions();
#endif

	inputCtrl->MoveMouseCenter();
	framecnt = 0;

	GameState->NextState();
	return 0;
}

int mainmenu::Recovery()
{
	//긳깓긞긏긢??룊딖돸
	d3dg->LoadMapdata(&BlockData, demopath);

	//?귽깛긣긢??룊딖돸
	ObjMgr.Recovery();

	gametitle = d3dg->LoadTexture("data\\title.dds", false, false);

	return 0;
}

void mainmenu::Input()
{
	inputCtrl->GetInputState(false);

	//?긂긚띆뷭귩롦벦
	inputCtrl->GetMouseMovement(&mainmenu_mouseX, &mainmenu_mouseY);
	//if( mainmenu_mouseX < 0 ){ mainmenu_mouseX = 0; }
	//if( mainmenu_mouseX > GameConfig.GetScreenWidth()-1 ){ mainmenu_mouseX = GameConfig.GetScreenWidth()-1; }
	//if( mainmenu_mouseY < 0 ){ mainmenu_mouseY = 0; }
	//if( mainmenu_mouseY > GameConfig.GetScreenHeight()-1 ){ mainmenu_mouseY = GameConfig.GetScreenHeight()-1; }

	if( modescreen == 0 ){
		//긚긏깓?깑긫?귩룉뿚궢륃뺪롦벦
		int scrollitems = 0;
		int totalmission = 0;

		if (mission_list_mode == MISSION_LIST_STANDARD) {
			scrollbar_official.Input(mainmenu_mouseX, mainmenu_mouseY, inputCtrl->CheckMouseButtonNowL(), mainmenu_mouseY - mainmenu_mouseY_old);
			scrollitems = scrollbar_official.GetScrollItem();
			totalmission = TOTAL_OFFICIALMISSION;
		}
		else if (mission_list_mode == MISSION_LIST_BASIC) {
			scrollbar_training.Input(mainmenu_mouseX, mainmenu_mouseY, inputCtrl->CheckMouseButtonNowL(), mainmenu_mouseY - mainmenu_mouseY_old);
			scrollitems = scrollbar_training.GetScrollItem();
			totalmission = GetTrainingMissionCountByTab(training_list_tab);
		}
		else {
			scrollbar_addon.Input(mainmenu_mouseX, mainmenu_mouseY, inputCtrl->CheckMouseButtonNowL(), mainmenu_mouseY - mainmenu_mouseY_old);
			scrollitems = scrollbar_addon.GetScrollItem();
			totalmission = GameAddon.GetTotaldatas();
		}

		// TRAINING 내부 TUTORIAL / SKILL 탭 전환
		if ((mission_list_mode == MISSION_LIST_BASIC) && (inputCtrl->CheckMouseButtonUpL())) {
			if ((MAINMENU_X < mainmenu_mouseX)
				&& (mainmenu_mouseX < (MAINMENU_X + MAINMENU_LIST_W))
				&& (TRAINING_TAB_Y < mainmenu_mouseY)
				&& (mainmenu_mouseY < (TRAINING_TAB_Y + TRAINING_TAB_H))) {

				int tab_w = MAINMENU_LIST_W / 2;
				int local_x = mainmenu_mouseX - MAINMENU_X;
				int new_tab = TRAINING_TAB_TUTORIAL;

				if (local_x >= tab_w) {
					new_tab = TRAINING_TAB_SKILL;
				}

				if (training_list_tab != new_tab) {
					training_list_tab = new_tab;

					// 탭 전환 시 스크롤을 맨 위로 초기화한다.
					scrollbar_training.Create(
						MAINMENU_X + MAINMENU_LIST_W + 1,
						MAINMENU_Y + 1,
						MAINMENU_SCROLLBAR_W,
						MAINMENU_H - MAINMENU_SWITCH_H,
						GetTrainingMissionCountByTab(training_list_tab),
						TOTAL_MENUITEMS,
						0
					);

					// 현재 선택 미션도 탭의 첫 미션으로 갱신한다.
					int real_index = GetTrainingMissionRealIndexByTab(training_list_tab, 0);
					GameInfoData.selectmission_id = -1;

					if (real_index >= 0) {
						strcpy(GameInfoData.mifpath, TrainingMissionData[real_index].path);
					}
					else {
						strcpy(GameInfoData.mifpath, "");
					}
				}
			}
		}

		//?긞긘깈깛멗묖
		if (inputCtrl->CheckMouseButtonUpL()) {
			for (int i = 0; i < TOTAL_MENUITEMS; i++) {
				int list_index = scrollitems + i;
				char name[MENU_MISSION_NAME_MAX];
				char display_name[MENU_MISSION_NAME_MAX];

				strcpy(name, "");
				strcpy(display_name, "");

				if (list_index >= totalmission) {
					continue;
				}

				if (mission_list_mode == MISSION_LIST_STANDARD) {
					GameParamInfo.GetOfficialMission(list_index, name, NULL, NULL, NULL, NULL, NULL);
					MakeMenuMissionDisplayName(name, display_name, sizeof(display_name));

					if ((MAINMENU_X < mainmenu_mouseX)
						&& (mainmenu_mouseX < (MAINMENU_X + (signed)strlen(display_name) * MAINMENU_MISSION_FONT_W))
						&& (MAINMENU_Y + MAINMENU_ROW_H + i * MAINMENU_ROW_H < mainmenu_mouseY)
						&& (mainmenu_mouseY < MAINMENU_Y + MAINMENU_ROW_H + i * MAINMENU_ROW_H + MAINMENU_MISSION_FONT_H)) {
						GameInfoData.selectmission_id = list_index;
						strcpy(GameInfoData.mifpath, "");
						GameState->PushMouseButton();
					}
				}
				else if (mission_list_mode == MISSION_LIST_BASIC) {
					int real_index = GetTrainingMissionRealIndexByTab(training_list_tab, list_index);

					if (real_index < 0) {
						continue;
					}

					strncpy(name, TrainingMissionData[real_index].name, sizeof(name) - 1);
					name[sizeof(name) - 1] = '\0';

					MakeMenuMissionDisplayName(name, display_name, sizeof(display_name));

					if ((MAINMENU_X < mainmenu_mouseX)
						&& (mainmenu_mouseX < (MAINMENU_X + (signed)strlen(display_name) * MAINMENU_MISSION_FONT_W))
						&& (MAINMENU_Y + MAINMENU_ROW_H + i * MAINMENU_ROW_H < mainmenu_mouseY)
						&& (mainmenu_mouseY < MAINMENU_Y + MAINMENU_ROW_H + i * MAINMENU_ROW_H + MAINMENU_MISSION_FONT_H)) {
						GameInfoData.selectmission_id = -1;
						strcpy(GameInfoData.mifpath, TrainingMissionData[real_index].path);
						GameState->PushMouseButton();
					}
				}
				else {
					const char* addon_name = GameAddon.GetMissionName(list_index);

					if (addon_name != NULL) {
						strncpy(name, addon_name, sizeof(name) - 1);
						name[sizeof(name) - 1] = '\0';
					}

					MakeMenuMissionDisplayName(name, display_name, sizeof(display_name));

					if ((MAINMENU_X < mainmenu_mouseX)
						&& (mainmenu_mouseX < (MAINMENU_X + (signed)strlen(display_name) * MAINMENU_MISSION_FONT_W))
						&& (MAINMENU_Y + MAINMENU_ROW_H + i * MAINMENU_ROW_H < mainmenu_mouseY)
						&& (mainmenu_mouseY < MAINMENU_Y + MAINMENU_ROW_H + i * MAINMENU_ROW_H + MAINMENU_MISSION_FONT_H)) {
						GameInfoData.selectmission_id = -1;
						strcpy(GameInfoData.mifpath, ADDONDIR);
						strcat(GameInfoData.mifpath, "\\");
						strcat(GameInfoData.mifpath, GameAddon.GetFileName(list_index));
						GameState->PushMouseButton();
					}
				}
			}
		}

		if (inputCtrl->CheckMouseButtonUpL()) {
			// UP
			if ((MAINMENU_X < mainmenu_mouseX)
				&& (mainmenu_mouseX < (MAINMENU_X + MAINMENU_LIST_W))
				&& (MAINMENU_Y < mainmenu_mouseY)
				&& (mainmenu_mouseY < MAINMENU_Y + MAINMENU_ROW_H)) {
				if (mission_list_mode == MISSION_LIST_STANDARD) {
					scrollbar_official.ScrollUP();
				}
				else if (mission_list_mode == MISSION_LIST_BASIC) {
					scrollbar_training.ScrollUP();
				}
				else if (mission_list_mode == MISSION_LIST_ADDON) {
					scrollbar_addon.ScrollUP();
				}
			}

			// DOWN
			if ((MAINMENU_X < mainmenu_mouseX)
				&& (mainmenu_mouseX < (MAINMENU_X + MAINMENU_LIST_W))
				&& (MAINMENU_Y + MAINMENU_H - MAINMENU_SWITCH_H - MAINMENU_ROW_H < mainmenu_mouseY)
				&& (mainmenu_mouseY < MAINMENU_Y + MAINMENU_H - MAINMENU_SWITCH_H)) {
				if (mission_list_mode == MISSION_LIST_STANDARD) {
					scrollbar_official.ScrollDOWN();
				}
				else if (mission_list_mode == MISSION_LIST_BASIC) {
					scrollbar_training.ScrollDOWN();
				}
				else if (mission_list_mode == MISSION_LIST_ADDON) {
					scrollbar_addon.ScrollDOWN();
				}
			}
		}

		// STANDARD / BASIC / ADD-ON 미션 목록 전환
		if (inputCtrl->CheckMouseButtonUpL()) {
			if ((MAINMENU_X < mainmenu_mouseX)
				&& (mainmenu_mouseX < (MAINMENU_X + MAINMENU_LIST_W))
				&& (MAINMENU_Y + MAINMENU_H - MAINMENU_SWITCH_H < mainmenu_mouseY)
				&& (mainmenu_mouseY < MAINMENU_Y + MAINMENU_H)) {

				int switch_w = MAINMENU_LIST_W / 3;
				int local_x = mainmenu_mouseX - MAINMENU_X;

				if (local_x < switch_w) {
					mission_list_mode = MISSION_LIST_STANDARD;
					GameInfoData.selectmission_id = 0;
					strcpy(GameInfoData.mifpath, "");
				}
				else if (local_x < switch_w * 2) {
					if (TotalTrainingMission > 0) {
						mission_list_mode = MISSION_LIST_BASIC;
						GameInfoData.selectmission_id = -1;

						int real_index = GetTrainingMissionRealIndexByTab(training_list_tab, 0);

						if (real_index >= 0) {
							strcpy(GameInfoData.mifpath, TrainingMissionData[real_index].path);
						}
						else {
							strcpy(GameInfoData.mifpath, "");
						}
					}
				}
				else {
					if (GameAddon.GetTotaldatas() > 0) {
						mission_list_mode = MISSION_LIST_ADDON;
						GameInfoData.selectmission_id = -1;
					}
				}
			}
		}

#ifdef ENABLE_MENUOPTIONS
		// 옵션 화면 전환
		if (inputCtrl->CheckMouseButtonUpL()) {
			if ((MAINMENU_OPTION_X < mainmenu_mouseX)
				&& (mainmenu_mouseX < (MAINMENU_OPTION_X + MAINMENU_OPTION_W))
				&& (MAINMENU_OPTION_Y < mainmenu_mouseY)
				&& (mainmenu_mouseY < (MAINMENU_OPTION_Y + MAINMENU_OPTION_H))) {
				modescreen = 1;
			}

			// 스킬 설명 화면 전환
			if ((MAINMENU_SKILLS_X < mainmenu_mouseX)
				&& (mainmenu_mouseX < (MAINMENU_SKILLS_X + MAINMENU_SKILLS_W))
				&& (MAINMENU_SKILLS_Y < mainmenu_mouseY)
				&& (mainmenu_mouseY < (MAINMENU_SKILLS_Y + MAINMENU_SKILLS_H))) {
				modescreen = 7;
			}
		}
#endif

		//ESC긌?귩룉뿚
		if( inputCtrl->CheckKeyDown(GetEscKeycode()) ){
			GameState->PushBackSpaceKey();
		}
	}
	else{
#ifdef ENABLE_MENUOPTIONS
		InputOptions();
#endif
	}

	mainmenu_mouseY_old = mainmenu_mouseY;
}

void mainmenu::Process()
{
	//긆긳긙긃긏긣?긨?긙긿?귩렳뛱
	ObjMgr.Process(-1, true, false);

	//AI귩렳뛱
	for(int i=0; i<MAX_HUMAN; i++){
		HumanAI[i].Process();
	}

	//긇긽깋댧뭫귩똶럁
	human *myHuman = ObjMgr.GetPlayerHumanObject();
	myHuman->GetPosData(&camera_x, &camera_y, &camera_z, NULL);
	camera_x -= 4.0f;
	camera_y += 22.0f;
	camera_z -= 12.0f;
	camera_rx = DegreeToRadian(45);
	camera_ry = DegreeToRadian(-25);

	if( modescreen != 0 ){
#ifdef ENABLE_MENUOPTIONS
		ProcessOptions();
#endif
	}

	framecnt += 1;
}

void mainmenu::Render3D()
{
	//긲긅긐궴긇긽깋귩먠믦
	d3dg->SetFog(true, 0);
	d3dg->SetCamera(camera_x, camera_y, camera_z, camera_rx, camera_ry, VIEWANGLE_NORMAL);

	//Z긫긞긲?귩룊딖돸
	d3dg->ResetZbuffer();

	//?긞긵귩?됪
	d3dg->ResetWorldTransform();
	d3dg->RenderMapdata(false);

	//긆긳긙긃긏긣귩?됪
	// 메뉴/타이틀에서는 우선 보간을 끈다.
	ObjMgr.Render(camera_x, camera_y, camera_z, camera_rx, camera_ry, 0, false);
}

void mainmenu::Render2D()
{
	int color;
	float effect;

	int swidth = (int)((float)GameConfig.GetScreenWidth() / GameConfig.GetScreenHeight() * 480);

	//긒??궻긫?긙깈깛륃뺪?렑
	d3dg->Draw2DTextureFontTextScaling(swidth - 118+1, 75+1, GAMEVERSION, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 18, 22);
	d3dg->Draw2DTextureFontTextScaling(swidth - 118, 75, GAMEVERSION, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 18, 22);

	if( modescreen == 0 ){
		//긚긏깓?깑긫?궻륃뺪귩롦벦
		int scrollitems = 0;
		int totalmission = 0;
		bool scrolling = false;

		if (mission_list_mode == MISSION_LIST_STANDARD) {
			scrollitems = scrollbar_official.GetScrollItem();
			totalmission = TOTAL_OFFICIALMISSION;
			scrolling = scrollbar_official.CheckScrolling();
		}
		else if (mission_list_mode == MISSION_LIST_BASIC) {
			scrollitems = scrollbar_training.GetScrollItem();
			totalmission = GetTrainingMissionCountByTab(training_list_tab);
			scrolling = scrollbar_training.CheckScrolling();
		}
		else {
			scrollitems = scrollbar_addon.GetScrollItem();
			totalmission = GameAddon.GetTotaldatas();
			scrolling = scrollbar_addon.CheckScrolling();
		}

		// 메뉴 영역 렌더링
// STANDARD / BASIC / ADD-ON 전환 버튼을 항상 표시하므로 전체 높이를 사용한다.
		d3dg->Draw2DBox(
			MAINMENU_X - 1,
			MAINMENU_Y,
			MAINMENU_X + MAINMENU_W,
			MAINMENU_Y + MAINMENU_H + 1,
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.5f)
		);

		//긚긏깓?깑긫??됪
		if (mission_list_mode == MISSION_LIST_STANDARD) {
			scrollbar_official.Draw(d3dg);
		}
		else if (mission_list_mode == MISSION_LIST_BASIC) {
			scrollbar_training.Draw(d3dg);
		}
		else if (mission_list_mode == MISSION_LIST_ADDON) {
			scrollbar_addon.Draw(d3dg);
		}

		// TRAINING 내부 TUTORIAL / SKILL 탭 버튼 표시
		if (mission_list_mode == MISSION_LIST_BASIC) {
			const char* tab_text[2] = {
				"TUTORIAL",
				"SKILL"
			};

			int tab_w = MAINMENU_LIST_W / 2;

			for (int i = 0; i < 2; i++) {
				int sx = MAINMENU_X + tab_w * i;
				int ex = sx + tab_w;

				if (i == 1) {
					ex = MAINMENU_X + MAINMENU_LIST_W;
				}

				bool selected = (training_list_tab == i);
				bool hover = false;

				if ((sx < mainmenu_mouseX)
					&& (mainmenu_mouseX < ex)
					&& (TRAINING_TAB_Y < mainmenu_mouseY)
					&& (mainmenu_mouseY < (TRAINING_TAB_Y + TRAINING_TAB_H))
					&& (scrolling == false)) {
					hover = true;
				}

				int fill_color;
				int border_color;
				int text_color;

				if (selected == true) {
					fill_color = d3dg->GetColorCode(0.18f, 0.18f, 0.04f, 0.75f);
					border_color = d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f);
					text_color = d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f);
				}
				else if (hover == true) {
					fill_color = d3dg->GetColorCode(0.04f, 0.12f, 0.14f, 0.70f);
					border_color = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f);
					text_color = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f);
				}
				else {
					fill_color = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.45f);
					border_color = d3dg->GetColorCode(0.55f, 0.55f, 0.55f, 0.75f);
					text_color = d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f);
				}

				d3dg->Draw2DBox(
					sx,
					TRAINING_TAB_Y,
					ex,
					TRAINING_TAB_Y + TRAINING_TAB_H,
					fill_color
				);

				d3dg->Draw2DBox(sx, TRAINING_TAB_Y, ex, TRAINING_TAB_Y + 1, border_color);
				d3dg->Draw2DBox(sx, TRAINING_TAB_Y + TRAINING_TAB_H - 1, ex, TRAINING_TAB_Y + TRAINING_TAB_H, border_color);
				d3dg->Draw2DBox(sx, TRAINING_TAB_Y, sx + 1, TRAINING_TAB_Y + TRAINING_TAB_H, border_color);
				d3dg->Draw2DBox(ex - 1, TRAINING_TAB_Y, ex, TRAINING_TAB_Y + TRAINING_TAB_H, border_color);

				int text_w = (int)strlen(tab_text[i]) * MAINMENU_SWITCH_FONT_W;
				int text_x = sx + (ex - sx - text_w) / 2;

				d3dg->Draw2DTextureFontText(
					text_x + 1,
					TRAINING_TAB_Y + 1,
					tab_text[i],
					d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f),
					MAINMENU_SWITCH_FONT_W,
					MAINMENU_SWITCH_FONT_H
				);

				d3dg->Draw2DTextureFontText(
					text_x,
					TRAINING_TAB_Y,
					tab_text[i],
					text_color,
					MAINMENU_SWITCH_FONT_W,
					MAINMENU_SWITCH_FONT_H
				);
			}
		}

		// '< UP >' 표시
		if (scrollitems > 0) {
			d3dg->Draw2DTextureFontText(
				MAINMENU_X + 1,
				MAINMENU_Y + 1,
				"<  UP  >",
				d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f),
				MAINMENU_TITLE_FONT_W,
				MAINMENU_TITLE_FONT_H
			);

			// 문자 색상 설정
			if ((MAINMENU_X < mainmenu_mouseX)
				&& (mainmenu_mouseX < (MAINMENU_X + MAINMENU_LIST_W))
				&& (MAINMENU_Y < mainmenu_mouseY)
				&& (mainmenu_mouseY < MAINMENU_Y + MAINMENU_ROW_H)
				&& (scrolling == false)) {
				color = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f);
			}
			else {
				color = d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f);
			}

			// 문자 표시
			d3dg->Draw2DTextureFontText(
				MAINMENU_X,
				MAINMENU_Y,
				"<  UP  >",
				color,
				MAINMENU_TITLE_FONT_W,
				MAINMENU_TITLE_FONT_H
			);
		}
		else {
			d3dg->Draw2DTextureFontText(
				MAINMENU_X + 1,
				MAINMENU_Y + 1,
				"<  UP  >",
				d3dg->GetColorCode(0.6f, 0.6f, 0.6f, 1.0f),
				MAINMENU_TITLE_FONT_W,
				MAINMENU_TITLE_FONT_H
			);
		}

		// '< DOWN >' 표시
		if (scrollitems < (totalmission - TOTAL_MENUITEMS)) {
			d3dg->Draw2DTextureFontText(
				MAINMENU_X + 1,
				MAINMENU_Y + MAINMENU_H - MAINMENU_SWITCH_H - MAINMENU_ROW_H + 1,
				"< DOWN >",
				d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f),
				MAINMENU_TITLE_FONT_W,
				MAINMENU_TITLE_FONT_H
			);

			// 문자 색상 설정
			if ((MAINMENU_X < mainmenu_mouseX)
				&& (mainmenu_mouseX < (MAINMENU_X + MAINMENU_LIST_W))
				&& (MAINMENU_Y + MAINMENU_H - MAINMENU_SWITCH_H - MAINMENU_ROW_H < mainmenu_mouseY)
				&& (mainmenu_mouseY < MAINMENU_Y + MAINMENU_H - MAINMENU_SWITCH_H)
				&& (scrolling == false)) {
				color = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f);
			}
			else {
				color = d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f);
			}

			// 문자 표시
			d3dg->Draw2DTextureFontText(
				MAINMENU_X,
				MAINMENU_Y + MAINMENU_H - MAINMENU_SWITCH_H - MAINMENU_ROW_H,
				"< DOWN >",
				color,
				MAINMENU_TITLE_FONT_W,
				MAINMENU_TITLE_FONT_H
			);
		}
		else {
			d3dg->Draw2DTextureFontText(
				MAINMENU_X + 1,
				MAINMENU_Y + MAINMENU_H - MAINMENU_SWITCH_H - MAINMENU_ROW_H + 1,
				"< DOWN >",
				d3dg->GetColorCode(0.6f, 0.6f, 0.6f, 1.0f),
				MAINMENU_TITLE_FONT_W,
				MAINMENU_TITLE_FONT_H
			);
		}

		// STANDARD / TRAINING / ADD-ON 전환 버튼
		{
			const char* switch_text[3] = {
				"STANDARD",
				"TRAINING",
				"ADD-ON"
			};

			int switch_mode[3] = {
				MISSION_LIST_STANDARD,
				MISSION_LIST_BASIC,
				MISSION_LIST_ADDON
			};

			int switch_w = MAINMENU_LIST_W / 3;
			int switch_y = MAINMENU_Y + MAINMENU_H - MAINMENU_SWITCH_H;

			for (int i = 0; i < 3; i++) {
				int sx = MAINMENU_X + switch_w * i;
				int ex = sx + switch_w;
				bool enabled = true;
				bool hover = false;
				bool selected = (mission_list_mode == switch_mode[i]);

				// 마지막 칸은 나눗셈 오차 때문에 오른쪽 끝에 1~2px 틈이 생길 수 있으므로 보정한다.
				if (i == 2) {
					ex = MAINMENU_X + MAINMENU_LIST_W;
				}

				if ((switch_mode[i] == MISSION_LIST_BASIC) && (TotalTrainingMission <= 0)) {
					enabled = false;
				}

				if ((switch_mode[i] == MISSION_LIST_ADDON) && (GameAddon.GetTotaldatas() <= 0)) {
					enabled = false;
				}

				if ((enabled == true)
					&& (sx < mainmenu_mouseX)
					&& (mainmenu_mouseX < ex)
					&& (switch_y < mainmenu_mouseY)
					&& (mainmenu_mouseY < MAINMENU_Y + MAINMENU_H)
					&& (scrolling == false)) {
					hover = true;
				}

				int fill_color;
				int border_color;

				if (enabled == false) {
					fill_color = d3dg->GetColorCode(0.05f, 0.05f, 0.05f, 0.45f);
					border_color = d3dg->GetColorCode(0.35f, 0.35f, 0.35f, 0.75f);
					color = d3dg->GetColorCode(0.45f, 0.45f, 0.45f, 1.0f);
				}
				else if (selected == true) {
					fill_color = d3dg->GetColorCode(0.18f, 0.18f, 0.04f, 0.75f);
					border_color = d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f);
					color = d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f);
				}
				else if (hover == true) {
					fill_color = d3dg->GetColorCode(0.04f, 0.12f, 0.14f, 0.70f);
					border_color = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f);
					color = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f);
				}
				else {
					fill_color = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.45f);
					border_color = d3dg->GetColorCode(0.55f, 0.55f, 0.55f, 0.75f);
					color = d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f);
				}

				// 버튼 배경
				d3dg->Draw2DBox(
					sx,
					switch_y,
					ex,
					MAINMENU_Y + MAINMENU_H,
					fill_color
				);

				// 버튼 경계선
				d3dg->Draw2DBox(sx, switch_y, ex, switch_y + 1, border_color);									// 위
				d3dg->Draw2DBox(sx, MAINMENU_Y + MAINMENU_H - 1, ex, MAINMENU_Y + MAINMENU_H, border_color);		// 아래
				d3dg->Draw2DBox(sx, switch_y, sx + 1, MAINMENU_Y + MAINMENU_H, border_color);					// 왼쪽
				d3dg->Draw2DBox(ex - 1, switch_y, ex, MAINMENU_Y + MAINMENU_H, border_color);					// 오른쪽

				int text_w = (int)strlen(switch_text[i]) * MAINMENU_SWITCH_FONT_W;
				int text_x = sx + (switch_w - text_w) / 2;

				// 혹시 글자가 칸보다 길어져도 왼쪽으로 튀어나가지 않게 한다.
				if (text_x < sx) {
					text_x = sx;
				}

				d3dg->Draw2DTextureFontText(
					text_x + 1,
					switch_y + 1,
					switch_text[i],
					d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f),
					MAINMENU_SWITCH_FONT_W,
					MAINMENU_SWITCH_FONT_H
				);

				d3dg->Draw2DTextureFontText(
					text_x,
					switch_y,
					switch_text[i],
					color,
					MAINMENU_SWITCH_FONT_W,
					MAINMENU_SWITCH_FONT_H
				);
			}
		}

		// 미션 이름 표시
		for (int i = 0; i < TOTAL_MENUITEMS; i++) {
			char name[MENU_MISSION_NAME_MAX];
			char display_name[MENU_MISSION_NAME_MAX];

			strcpy(name, "");
			strcpy(display_name, "");

			// 미션 이름 얻기
			int list_index = scrollitems + i;

			if (list_index >= totalmission) {
				continue;
			}

			if (mission_list_mode == MISSION_LIST_STANDARD) {
				GameParamInfo.GetOfficialMission(list_index, name, NULL, NULL, NULL, NULL, NULL);
			}
			else if (mission_list_mode == MISSION_LIST_BASIC) {
				int real_index = GetTrainingMissionRealIndexByTab(training_list_tab, list_index);

				if (real_index < 0) {
					continue;
				}

				strncpy(name, TrainingMissionData[real_index].name, sizeof(name) - 1);
				name[sizeof(name) - 1] = '\0';
			}
			else {
				const char* addon_name = GameAddon.GetMissionName(list_index);

				if (addon_name != NULL) {
					strncpy(name, addon_name, sizeof(name) - 1);
					name[sizeof(name) - 1] = '\0';
				}
			}

			MakeMenuMissionDisplayName(name, display_name, sizeof(display_name));

			// 문자 색상 설정
			if ((MAINMENU_X < mainmenu_mouseX)
				&& (mainmenu_mouseX < (MAINMENU_X + (signed)strlen(display_name) * MAINMENU_MISSION_FONT_W))
				&& (MAINMENU_Y + MAINMENU_ROW_H + i * MAINMENU_ROW_H < mainmenu_mouseY)
				&& (mainmenu_mouseY < MAINMENU_Y + MAINMENU_ROW_H + i * MAINMENU_ROW_H + MAINMENU_MISSION_FONT_H)
				&& (scrolling == false)) {
				color = d3dg->GetColorCode(1.0f, 0.6f, 0.6f, 1.0f);
			}
			else {
				color = d3dg->GetColorCode(0.6f, 0.6f, 1.0f, 1.0f);
			}

			// 문자 표시
			d3dg->Draw2DTextureFontText(
				MAINMENU_X + 1,
				MAINMENU_Y + MAINMENU_ROW_H + 1 + i * MAINMENU_ROW_H,
				display_name,
				d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f),
				MAINMENU_MISSION_FONT_W,
				MAINMENU_MISSION_FONT_H
			);
			d3dg->Draw2DTextureFontText(
				MAINMENU_X,
				MAINMENU_Y + MAINMENU_ROW_H + i * MAINMENU_ROW_H,
				display_name,
				color,
				MAINMENU_MISSION_FONT_W,
				MAINMENU_MISSION_FONT_H
			);
		}

#ifdef ENABLE_MENUOPTIONS
		// 옵션 버튼
		d3dg->Draw2DBox(
			MAINMENU_OPTION_X,
			MAINMENU_OPTION_Y,
			MAINMENU_OPTION_X + MAINMENU_OPTION_W,
			MAINMENU_OPTION_Y + MAINMENU_OPTION_H,
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.5f)
		);

		// 문자 색상 설정
		if ((MAINMENU_OPTION_X < mainmenu_mouseX)
			&& (mainmenu_mouseX < (MAINMENU_OPTION_X + MAINMENU_OPTION_W))
			&& (MAINMENU_OPTION_Y < mainmenu_mouseY)
			&& (mainmenu_mouseY < (MAINMENU_OPTION_Y + MAINMENU_OPTION_H))
			&& (scrolling == false)) {
			color = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			color = d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f);
		}

		// 문자 표시
		d3dg->Draw2DTextureFontText(
			MAINMENU_OPTION_X + MAINMENU_OPTION_PAD_X + 1,
			MAINMENU_OPTION_Y + MAINMENU_OPTION_PAD_Y + 1,
			MAINMENU_OPTION_TEXT,
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f),
			MAINMENU_OPTION_FONT_W,
			MAINMENU_OPTION_FONT_H
		);
		d3dg->Draw2DTextureFontText(
			MAINMENU_OPTION_X + MAINMENU_OPTION_PAD_X,
			MAINMENU_OPTION_Y + MAINMENU_OPTION_PAD_Y,
			MAINMENU_OPTION_TEXT,
			color,
			MAINMENU_OPTION_FONT_W,
			MAINMENU_OPTION_FONT_H
		);

		// SKILLS 버튼
		d3dg->Draw2DBox(
			MAINMENU_SKILLS_X,
			MAINMENU_SKILLS_Y,
			MAINMENU_SKILLS_X + MAINMENU_SKILLS_W,
			MAINMENU_SKILLS_Y + MAINMENU_SKILLS_H,
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.5f)
		);

		if ((MAINMENU_SKILLS_X < mainmenu_mouseX)
			&& (mainmenu_mouseX < (MAINMENU_SKILLS_X + MAINMENU_SKILLS_W))
			&& (MAINMENU_SKILLS_Y < mainmenu_mouseY)
			&& (mainmenu_mouseY < (MAINMENU_SKILLS_Y + MAINMENU_SKILLS_H))
			&& (scrolling == false)) {
			color = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			color = d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f);
		}

		d3dg->Draw2DTextureFontText(
			MAINMENU_SKILLS_X + MAINMENU_OPTION_PAD_X + 1,
			MAINMENU_SKILLS_Y + MAINMENU_OPTION_PAD_Y + 1,
			MAINMENU_SKILLS_TEXT,
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f),
			MAINMENU_OPTION_FONT_W,
			MAINMENU_OPTION_FONT_H
		);
		d3dg->Draw2DTextureFontText(
			MAINMENU_SKILLS_X + MAINMENU_OPTION_PAD_X,
			MAINMENU_SKILLS_Y + MAINMENU_OPTION_PAD_Y,
			MAINMENU_SKILLS_TEXT,
			color,
			MAINMENU_OPTION_FONT_W,
			MAINMENU_OPTION_FONT_H
		);
#endif	//ENABLE_MENUOPTIONS
	}
	else{
#ifdef ENABLE_MENUOPTIONS
		RenderOptions();
#endif
	}

	//?긂긚긇??깑?렑걁먗멄걂
	d3dg->Draw2DBox(0, mainmenu_mouseY-1, GameConfig.GetScreenWidth(), mainmenu_mouseY+1, d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f));
	d3dg->Draw2DBox(mainmenu_mouseX-1, 0, mainmenu_mouseX+1, GameConfig.GetScreenHeight(), d3dg->GetColorCode(1.0f,0.0f,0.0f,0.5f));
	d3dg->Draw2DLine(0, mainmenu_mouseY, GameConfig.GetScreenWidth(), mainmenu_mouseY, d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));
	d3dg->Draw2DLine(mainmenu_mouseX, 0, mainmenu_mouseX, GameConfig.GetScreenHeight(), d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f));

	//긒??궻깓긕??긏?됪
	d3dg->Draw2DTextureScaling(20, 25, gametitle, 480, 80, 1.0f);

	//긳깋긞긏귺긂긣먠믦
	if( framecnt < (int)(2.0f*GAMEFPS) ){
		effect = GetEffectAlpha(framecnt, 1.0f, 2.0f, 0.0f, true);
	}
	else{
		effect = 0.0f;
	}
	d3dg->Draw2DBox(0, 0, GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight(), d3dg->GetColorCode(0.0f,0.0f,0.0f,effect));
}

#ifdef ENABLE_MENUOPTIONS

typedef struct
{
	int skilltype;
	const char* key;
	const char* name;
	const char* cooldown;
	const char* duration;
	const char* effect1;
	const char* effect2;
	const char* effect3;
} SkillInfoSkillData;

typedef struct
{
	int group;
	const char* groupname;
	const char* targets;
	SkillInfoSkillData skill[PLAYER_SKILL_SLOT_COUNT];
} SkillInfoGroupData;

// 엑셀의 스킬 목록을 메뉴 표시용 영어 문구로 정리.
// UI 폰트 안정성을 위해 메뉴에는 영어를 사용한다.
static const SkillInfoGroupData SkillInfoData[SKILLINFO_GROUP_DATAS] = {
	{
		PLAYER_SKILL_GROUP_SECRET_AGENT, "SECRET AGENT", "0, 1, 8",
		{
			{ PLAYER_SKILL_TELEPORT, "E", "TELEPORT", "35 sec", "Cast 0.5 sec", "Teleport to the targeted position.", "", "" },
			{ PLAYER_SKILL_STEALTH, "F", "STEALTH", "20 sec", "5 sec", "Become invisible.", "AI cannot see or track the player.", "Enemies in combat lose tracking." }
		}
	},
	{
		PLAYER_SKILL_GROUP_AGENT_A, "AGENT A", "11, 12, 24, 25",
		{
			{ PLAYER_SKILL_MARKSHOT, "E", "MARKING SHOT", "30 sec", "Cast 1 sec + 10 sec", "Mark the nearest enemy with a real-time ping.", "If the marked enemy dies, cooldown is reduced by 15 sec.", "" },
			{ PLAYER_SKILL_SHADOW_DECOY, "F", "SHADOW DECOY", "30 sec", "Install 0.7 sec", "Summon a translucent decoy at the target point.", "Enemies may attack the decoy.", "An attacker is pinged for 5 sec." }
		}
	},
	{
		PLAYER_SKILL_GROUP_AGENT_B, "AGENT B", "22, 23, 34, 37, 38",
		{
			{ PLAYER_SKILL_SUMMON, "E", "SUMMON ALLIES", "25 sec", "Install 0.7 sec", "Summon up to 3 allies at the target point.", "Recasting removes previous summoned allies.", "" },
			{ PLAYER_SKILL_BIOTIC_FIELD, "F", "BIOTIC FIELD", "30 sec", "Install 0.5 sec + 6 sec", "Install a healing field.", "All allies inside recover 10 HP every 0.5 sec.", "" }
		}
	},
	{
		PLAYER_SKILL_GROUP_CAPTAIN, "CAPTAIN", "7, 35, 41, 42",
		{
			{ PLAYER_SKILL_TACTICAL_SCAN, "E", "TACTICAL SCAN", "30 sec", "Cast 3 sec + 5 sec", "Reveal enemy positions with pings.", "Enemies behind walls are also displayed.", "" },
			{ PLAYER_SKILL_ARTILLERY_STRIKE, "F", "ARTILLERY STRIKE", "45 sec", "Cast 1.5 sec + 7 sec", "Call repeated explosions at the selected point.", "Cannot install if the player is inside the strike area.", "" }
		}
	},
	{
		PLAYER_SKILL_GROUP_SOLDIER, "SOLDIER", "26, 27, 28",
		{
			{ PLAYER_SKILL_IMPACT_GRENADE, "E", "IMPACT GRENADE", "5 uses", "-", "Grenade explodes immediately on wall contact.", "Damage is about half of a normal grenade.", "" },
			{ PLAYER_SKILL_AMMOBOOST, "F", "COMBAT BOOST", "30 sec", "8 sec", "Reload current weapon immediately if possible.", "No ammo consumption during effect.", "Fire delay -20%, recoil -50%." }
		}
	},
	{
		PLAYER_SKILL_GROUP_MERCENARY, "MERCENARY", "5, 6, 39, 40",
		{
			{ PLAYER_SKILL_DP28, "E", "DP-28", "1 weapon", "-", "Use the DP-28 machine gun weapon.", "", "" },
			{ PLAYER_SKILL_ABSORB, "F", "COMBAT ABSORB", "30 sec", "6 sec", "Reload current weapon immediately.", "Incoming damage -50%.", "Recover 10% of actual damage dealt." }
		}
	},
	{
		PLAYER_SKILL_GROUP_TERRORIST, "TERRORIST", "2, 3, 4, 17",
		{
			{ PLAYER_SKILL_DISTRACTION, "E", "DISTRACTION SIGNAL", "20 sec", "Install 1 sec + 3.5 sec", "Install a signal device at the target point.", "Enemies within radius are lured toward the signal.", "Enemies already in direct combat are less affected." },
			{ PLAYER_SKILL_TERRORIST_MINE, "F", "MINE", "15 sec", "Install 0.5 sec + arm 1 sec", "Install up to 3 mines.", "When over the limit, the oldest mine is removed.", "Explosion damage can still affect allies." }
		}
	},
	{
		PLAYER_SKILL_GROUP_POLICE, "POLICE", "9, 14",
		{
			{ PLAYER_SKILL_SHIELD, "E", "BALLISTIC SHIELD", "1 weapon", "-", "Use a shield weapon that blocks frontal attacks.", "", "" },
			{ PLAYER_SKILL_SUPPRESS, "F", "SUPPRESS ORDER", "35 sec", "3 sec", "Force human enemies in sight to drop their weapons.", "Does not affect robots.", "Suppressed enemies cannot immediately pick weapons up." }
		}
	},
	{
		PLAYER_SKILL_GROUP_CIVILIAN_A, "CIVILIAN A", "13, 18, 20, 21",
		{
			{ PLAYER_SKILL_HEAL, "E", "FIRST AID", "20 sec", "Instant", "Recover 50 HP.", "Cannot be used at full HP or while dead.", "" },
			{ PLAYER_SKILL_IRONBODY, "F", "IRON BODY", "25 sec", "5 sec", "Incoming damage -90%.", "Movement speed -90%.", "" }
		}
	},
	{
		PLAYER_SKILL_GROUP_CIVILIAN_B, "CIVILIAN B", "15, 16, 19, 33, 36",
		{
			{ PLAYER_SKILL_DODGE, "E", "EMERGENCY DODGE", "15 sec", "Instant", "Dodge quickly in the movement direction.", "Brief invincibility during dodge.", "" },
			{ PLAYER_SKILL_SURVIVAL, "F", "SURVIVAL INSTINCT", "25 sec", "5 sec", "Movement speed +20%.", "Incoming damage -30%.", "" }
		}
	},
	{
		PLAYER_SKILL_GROUP_ZOMBIE, "ZOMBIE", "29, 30, 31, 32",
		{
			{ PLAYER_SKILL_HOOK, "E", "HOOK", "10 sec", "Cast 0.35 sec + pull 1.25 sec", "Pull an enemy toward the player.", "Fails if blocked by a wall.", "" },
			{ PLAYER_SKILL_RAGE, "F", "RAGE", "25 sec", "6 sec", "Movement, attack speed, and damage +44%.", "Incoming damage +44%.", "Recover 44% of damage dealt." }
		}
	},
		{
		PLAYER_SKILL_GROUP_ROBOT, "ROBOT", "10",
		{
			{ PLAYER_SKILL_ROBOT_EXECUTE, "E", "EXECUTION PROTOCOL", "45 sec", "Cast 2 sec + 8 sec", "Mark up to 10 visible enemies.", "Left click fires execution shots in sequence.", "Frontal shields can block execution shots." },
			{ PLAYER_SKILL_AIMHACK, "F", "AIM HACK", "35 sec", "8 sec", "Aim is assisted toward enemy heads.", "", "" }
		}
	}
};

static const SkillInfoGroupData* SkillInfo_GetData(int index)
{
	if ((index < 0) || (SKILLINFO_GROUP_DATAS <= index)) {
		index = 0;
	}
	return &SkillInfoData[index];
}

typedef struct
{
	const char* groupname[SKILLINFO_LANG_DATAS];
	const char* skillname[PLAYER_SKILL_SLOT_COUNT][SKILLINFO_LANG_DATAS];
	const char* cooldown[PLAYER_SKILL_SLOT_COUNT][SKILLINFO_LANG_DATAS];
	const char* duration[PLAYER_SKILL_SLOT_COUNT][SKILLINFO_LANG_DATAS];
	const char* effect1[PLAYER_SKILL_SLOT_COUNT][SKILLINFO_LANG_DATAS];
	const char* effect2[PLAYER_SKILL_SLOT_COUNT][SKILLINFO_LANG_DATAS];
	const char* effect3[PLAYER_SKILL_SLOT_COUNT][SKILLINFO_LANG_DATAS];
} SkillInfoLocalTextData;

static const SkillInfoLocalTextData SkillInfoLocalText[SKILLINFO_GROUP_DATAS] = {
	{
		{ "비밀 요원", "SECRET AGENT", "秘密工作員" },
		{
			{ "텔레포트", "TELEPORT", "テレポート" },
			{ "은신", "STEALTH", "ステルス" }
		},
		{
			{ "35초", "35 sec", "35秒" },
			{ "20초", "20 sec", "20秒" }
		},
		{
			{ "준비 0.5초", "Cast 0.5 sec", "準備 0.5秒" },
			{ "5초", "5 sec", "5秒" }
		},
		{
			{ "목표 지점으로 순간 이동", "Teleport to the targeted position.", "目標地点へ瞬間移動する。" },
			{ "플레이어가 투명해진다.", "Become invisible.", "プレイヤーが透明になる。" }
		},
		{
			{ "", "", "" },
			{ "AI가 플레이어를 보거나 추적하지 못한다.", "AI cannot see or track the player.", "AIはプレイヤーを視認・追跡できない。" }
		},
		{
			{ "", "", "" },
			{ "전투 중인 AI도 공격/추적을 해제한다.", "Enemies in combat lose tracking.", "戦闘中のAIも追跡を解除する。" }
		}
	},
	{
		{ "요원 A", "AGENT A", "エージェントA" },
		{
			{ "마킹 샷", "MARKING SHOT", "マーキングショット" },
			{ "그림자 잔상", "SHADOW DECOY", "シャドウデコイ" }
		},
		{
			{ "30초", "30 sec", "30秒" },
			{ "30초", "30 sec", "30秒" }
		},
		{
			{ "준비 1초 + 10초", "Cast 1 sec + 10 sec", "準備1秒 + 10秒" },
			{ "설치 0.7초", "Install 0.7 sec", "設置0.7秒" }
		},
		{
			{ "가장 가까운 적 1명에게 실시간 핑 표시", "Mark the nearest enemy with a real-time ping.", "最も近い敵1体にリアルタイムピンを表示。" },
			{ "조준 지점에 반투명 잔상을 소환", "Summon a translucent decoy at the target point.", "照準地点に半透明のデコイを召喚。" }
		},
		{
			{ "대상이 사망하면 쿨타임 15초 감소", "If the marked enemy dies, cooldown is reduced by 15 sec.", "対象が死亡するとクールタイム15秒減少。" },
			{ "적의 주의를 끌 수 있다.", "Enemies may attack the decoy.", "敵の注意を引くことがある。" }
		},
		{
			{ "", "", "" },
			{ "잔상을 공격한 적에게 5초간 핑 표시", "An attacker is pinged for 5 sec.", "攻撃した敵に5秒間ピンを表示。" }
		}
	},
	{
		{ "요원 B", "AGENT B", "エージェントB" },
		{
			{ "동료 소환", "SUMMON ALLIES", "味方召喚" },
			{ "생체장", "BIOTIC FIELD", "バイオティックフィールド" }
		},
		{
			{ "25초", "25 sec", "25秒" },
			{ "30초", "30 sec", "30秒" }
		},
		{
			{ "설치 0.7초", "Install 0.7 sec", "設置0.7秒" },
			{ "설치 0.5초 + 6초", "Install 0.5 sec + 6 sec", "設置0.5秒 + 6秒" }
		},
		{
			{ "조준 지점에 동료를 최대 3명 소환", "Summon up to 3 allies at the target point.", "照準地点に最大3人の味方を召喚。" },
			{ "범위 내 아군 전체를 회복", "Install a healing field.", "範囲内の味方を回復するフィールドを設置。" }
		},
		{
			{ "재시전 시 기존 소환 동료 제거", "Recasting removes previous summoned allies.", "再使用時、以前の召喚味方を削除。" },
			{ "0.5초마다 HP 10 회복", "All allies inside recover 10 HP every 0.5 sec.", "0.5秒ごとにHPを10回復。" }
		},
		{
			{ "", "", "" },
			{ "", "", "" }
		}
	},
	{
		{ "대장", "CAPTAIN", "隊長" },
		{
			{ "전술 스캔", "TACTICAL SCAN", "戦術スキャン" },
			{ "지점 폭격", "ARTILLERY STRIKE", "地点砲撃" }
		},
		{
			{ "30초", "30 sec", "30秒" },
			{ "45초", "45 sec", "45秒" }
		},
		{
			{ "준비 3초 + 5초", "Cast 3 sec + 5 sec", "準備3秒 + 5秒" },
			{ "준비 1.5초 + 7초", "Cast 1.5 sec + 7 sec", "準備1.5秒 + 7秒" }
		},
		{
			{ "준비 후 적 위치를 핑으로 표시", "Reveal enemy positions with pings.", "準備後、敵の位置をピンで表示。" },
			{ "지정 지점에 지속 폭발 피해", "Call repeated explosions at the selected point.", "指定地点に連続爆発を発生させる。" }
		},
		{
			{ "벽 너머 적도 표시된다.", "Enemies behind walls are also displayed.", "壁越しの敵も表示される。" },
			{ "플레이어가 범위 안에 있으면 설치 불가", "Cannot install if the player is inside the strike area.", "自分が範囲内にいる場合は設置不可。" }
		},
		{
			{ "", "", "" },
			{ "", "", "" }
		}
	},
	{
		{ "군인", "SOLDIER", "兵士" },
		{
			{ "충격 수류탄", "IMPACT GRENADE", "衝撃グレネード" },
			{ "전투 강화", "COMBAT BOOST", "戦闘強化" }
		},
		{
			{ "5개", "5 uses", "5個" },
			{ "30초", "30 sec", "30秒" }
		},
		{
			{ "-", "-", "-" },
			{ "8초", "8 sec", "8秒" }
		},
		{
			{ "벽에 닿으면 즉시 폭발", "Grenade explodes immediately on wall contact.", "壁に触れると即時爆発。" },
			{ "발동 즉시 현재 무기 재장전", "Reload current weapon immediately if possible.", "発動時に現在武器を即時リロード。" }
		},
		{
			{ "피해량은 일반 수류탄의 절반", "Damage is about half of a normal grenade.", "ダメージは通常グレネードの約半分。" },
			{ "지속 중 탄 소모 없음", "No ammo consumption during effect.", "効果中は弾を消費しない。" }
		},
		{
			{ "", "", "" },
			{ "연사 대기 -20%, 반동 -50%", "Fire delay -20%, recoil -50%.", "発射間隔-20%、反動-50%。" }
		}
	},
	{
		{ "용병", "MERCENARY", "傭兵" },
		{
			{ "제 3의 무기", "DP-28", "第三の武器" },
			{ "전투 흡수", "COMBAT ABSORB", "戦闘吸収" }
		},
		{
			{ "1개", "1 weapon", "1個" },
			{ "30초", "30 sec", "30秒" }
		},
		{
			{ "-", "-", "-" },
			{ "6초", "6 sec", "6秒" }
		},
		{
			{ "DP-28 기관총 무기 사용", "Use the DP-28 machine gun weapon.", "DP-28機関銃を使用。" },
			{ "발동 즉시 현재 무기 재장전", "Reload current weapon immediately.", "発動時に現在武器を即時リロード。" }
		},
		{
			{ "", "", "" },
			{ "받는 피해 50% 감소", "Incoming damage -50%.", "受けるダメージ-50%。" }
		},
		{
			{ "", "", "" },
			{ "준 피해량의 10% 회복", "Recover 10% of actual damage dealt.", "与ダメージの10%を回復。" }
		}
	},
	{
		{ "테러리스트", "TERRORIST", "テロリスト" },
		{
			{ "교란 신호", "DISTRACTION SIGNAL", "妨害信号" },
			{ "지뢰 설치", "MINE", "地雷設置" }
		},
		{
			{ "20초", "20 sec", "20秒" },
			{ "15초", "15 sec", "15秒" }
		},
		{
			{ "설치 1초 + 3.5초", "Install 1 sec + 3.5 sec", "設置1秒 + 3.5秒" },
			{ "설치 0.5초 + 대기 1초", "Install 0.5 sec + arm 1 sec", "設置0.5秒 + 起動1秒" }
		},
		{
			{ "조준 지점에 교란 신호 설치", "Install a signal device at the target point.", "照準地点に妨害信号を設置。" },
			{ "최대 3개까지 지뢰 설치", "Install up to 3 mines.", "最大3個まで地雷を設置。" }
		},
		{
			{ "반경 내 적을 신호 지점으로 유도", "Enemies within radius are lured toward the signal.", "範囲内の敵を信号地点へ誘導。" },
			{ "초과 시 가장 오래된 지뢰 제거", "When over the limit, the oldest mine is removed.", "上限超過時、最古の地雷を削除。" }
		},
		{
			{ "직접 전투 중인 적에게는 제한적", "Enemies already in direct combat are less affected.", "直接戦闘中の敵には効果が弱い。" },
			{ "폭발 피해는 아군에게도 닿을 수 있음", "Explosion damage can still affect allies.", "爆発ダメージは味方にも当たる可能性あり。" }
		}
	},
	{
		{ "경찰", "POLICE", "警察" },
		{
			{ "방탄 방패", "BALLISTIC SHIELD", "防弾シールド" },
			{ "제압 명령", "SUPPRESS ORDER", "制圧命令" }
		},
		{
			{ "1개", "1 weapon", "1個" },
			{ "35초", "35 sec", "35秒" }
		},
		{
			{ "-", "-", "-" },
			{ "3초", "3 sec", "3秒" }
		},
		{
			{ "정면 공격을 막는 방패 무기 사용", "Use a shield weapon that blocks frontal attacks.", "正面攻撃を防ぐシールドを使用。" },
			{ "시야 내 인간형 적의 무기를 강제로 떨어뜨림", "Force human enemies in sight to drop their weapons.", "視界内の人間型の敵の武器を落とさせる。" }
		},
		{
			{ "", "", "" },
			{ "로봇에게는 효과 없음", "Does not affect robots.", "ロボットには効果がない。" }
		},
		{
			{ "", "", "" },
			{ "제압된 적은 바로 무기를 줍지 못함", "Suppressed enemies cannot immediately pick weapons up.", "制圧された敵はすぐに武器を拾えない。" }
		}
	},
	{
		{ "시민 A", "CIVILIAN A", "市民A" },
		{
			{ "응급 치료", "FIRST AID", "応急治療" },
			{ "철괴", "IRON BODY", "鉄身" }
		},
		{
			{ "20초", "20 sec", "20秒" },
			{ "25초", "25 sec", "25秒" }
		},
		{
			{ "즉시", "Instant", "即時" },
			{ "5초", "5 sec", "5秒" }
		},
		{
			{ "체력 50 회복", "Recover 50 HP.", "HPを50回復。" },
			{ "받는 피해 90% 감소", "Incoming damage -90%.", "受けるダメージ-90%。" }
		},
		{
			{ "최대 체력이거나 사망 상태면 사용 불가", "Cannot be used at full HP or while dead.", "HP最大または死亡中は使用不可。" },
			{ "이동 속도 90% 감소", "Movement speed -90%.", "移動速度-90%。" }
		},
		{
			{ "", "", "" },
			{ "", "", "" }
		}
	},
	{
		{ "시민 B", "CIVILIAN B", "市民B" },
		{
			{ "긴급 회피", "EMERGENCY DODGE", "緊急回避" },
			{ "생존 본능", "SURVIVAL INSTINCT", "生存本能" }
		},
		{
			{ "15초", "15 sec", "15秒" },
			{ "25초", "25 sec", "25秒" }
		},
		{
			{ "즉시", "Instant", "即時" },
			{ "5초", "5 sec", "5秒" }
		},
		{
			{ "이동 방향으로 순간 회피", "Dodge quickly in the movement direction.", "移動方向へ素早く回避。" },
			{ "이동 속도 20% 증가", "Movement speed +20%.", "移動速度+20%。" }
		},
		{
			{ "짧은 무적 시간 존재", "Brief invincibility during dodge.", "短時間の無敵あり。" },
			{ "받는 피해 30% 감소", "Incoming damage -30%.", "受けるダメージ-30%。" }
		},
		{
			{ "", "", "" },
			{ "", "", "" }
		}
	},
	{
		{ "좀비", "ZOMBIE", "ゾンビ" },
		{
			{ "갈고리", "HOOK", "フック" },
			{ "격노", "RAGE", "怒り" }
		},
		{
			{ "10초", "10 sec", "10秒" },
			{ "25초", "25 sec", "25秒" }
		},
		{
			{ "준비 0.35초 + 끌기 1.25초", "Cast 0.35 sec + pull 1.25 sec", "準備0.35秒 + 引き寄せ1.25秒" },
			{ "6초", "6 sec", "6秒" }
		},
		{
			{ "조준선이 적 근처를 지나면 끌어옴", "Pull an enemy toward the player.", "照準線が敵付近を通ると引き寄せる。" },
			{ "이동/공격속도/공격력 44% 증가", "Movement, attack speed, and damage +44%.", "移動・攻撃速度・攻撃力+44%。" }
		},
		{
			{ "벽에 막히면 실패", "Fails if blocked by a wall.", "壁に遮られると失敗。" },
			{ "받는 피해 44% 증가", "Incoming damage +44%.", "受けるダメージ+44%。" }
		},
		{
			{ "", "", "" },
			{ "준 피해의 44% 회복", "Recover 44% of damage dealt.", "与ダメージの44%を回復。" }
		}
	},
	{
		{ "로봇", "ROBOT", "ロボット" },
		{
			{ "처형 프로토콜", "EXECUTION PROTOCOL", "処刑プロトコル" },
			{ "에임 핵", "AIM HACK", "エイムハック" }
		},
		{
			{ "45초", "45 sec", "45秒" },
			{ "35초", "35 sec", "35秒" }
		},
		{
			{ "준비 2초 + 8초", "Cast 2 sec + 8 sec", "準備2秒 + 8秒" },
			{ "8초", "8 sec", "8秒" }
		},
				{
			{ "시야 내 적을 최대 10명까지 마킹", "Mark up to 10 visible enemies.", "視界内の敵を最大10体までマーキング。" },
			{ "적 머리 쪽으로 조준 보정", "Aim is assisted toward enemy heads.", "敵の頭部へ照準を補正。" }
		},
		{
			{ "좌클릭 시 마킹된 적에게 순차 처형 사격", "Left click fires execution shots at marked enemies in sequence.", "左クリックでマークした敵へ順次処刑射撃。" },
			{ "", "", "" }
		},
		{
			{ "정면 방패는 처형 사격을 막을 수 있음", "Frontal shields can block execution shots.", "正面のシールドは処刑射撃を防げる。" },
			{ "", "", "" }
		}
	}
};

static int SkillInfo_CheckLang(int lang)
{
	if ((lang < 0) || (SKILLINFO_LANG_DATAS <= lang)) {
		return SKILLINFO_LANG_EN;
	}
	return lang;
}

static const char* SkillInfo_GetLocalText(const char* const text[SKILLINFO_LANG_DATAS], int lang)
{
	lang = SkillInfo_CheckLang(lang);

	if ((text[lang] != NULL) && (text[lang][0] != '\0')) {
		return text[lang];
	}

	return text[SKILLINFO_LANG_EN];
}

static const char* SkillInfo_GetGroupName(int groupindex, int lang)
{
	if ((groupindex < 0) || (SKILLINFO_GROUP_DATAS <= groupindex)) {
		groupindex = 0;
	}

	return SkillInfo_GetLocalText(SkillInfoLocalText[groupindex].groupname, lang);
}

static const char* SkillInfo_GetSkillName(int groupindex, int slot, int lang)
{
	if ((groupindex < 0) || (SKILLINFO_GROUP_DATAS <= groupindex)) {
		groupindex = 0;
	}
	if ((slot < 0) || (PLAYER_SKILL_SLOT_COUNT <= slot)) {
		slot = 0;
	}

	return SkillInfo_GetLocalText(SkillInfoLocalText[groupindex].skillname[slot], lang);
}

static const char* SkillInfo_GetCooldownText(int groupindex, int slot, int lang)
{
	return SkillInfo_GetLocalText(SkillInfoLocalText[groupindex].cooldown[slot], lang);
}

static const char* SkillInfo_GetDurationText(int groupindex, int slot, int lang)
{
	return SkillInfo_GetLocalText(SkillInfoLocalText[groupindex].duration[slot], lang);
}

static const char* SkillInfo_GetEffectText(int groupindex, int slot, int effectindex, int lang)
{
	if (effectindex == 0) {
		return SkillInfo_GetLocalText(SkillInfoLocalText[groupindex].effect1[slot], lang);
	}
	if (effectindex == 1) {
		return SkillInfo_GetLocalText(SkillInfoLocalText[groupindex].effect2[slot], lang);
	}
	return SkillInfo_GetLocalText(SkillInfoLocalText[groupindex].effect3[slot], lang);
}

static const char* SkillInfo_GetTitleText(int lang)
{
	lang = SkillInfo_CheckLang(lang);

	if (lang == SKILLINFO_LANG_KO) { return "스킬"; }
	if (lang == SKILLINFO_LANG_JP) { return "スキル"; }
	return "SKILLS";
}

static const char* SkillInfo_GetSelectGroupText(int lang)
{
	lang = SkillInfo_CheckLang(lang);

	if (lang == SKILLINFO_LANG_KO) { return "그룹 선택"; }
	if (lang == SKILLINFO_LANG_JP) { return "グループ選択"; }
	return "SELECT GROUP";
}

static const char* SkillInfo_GetDetailTitleText(int lang)
{
	lang = SkillInfo_CheckLang(lang);

	if (lang == SKILLINFO_LANG_KO) { return "스킬 설명"; }
	if (lang == SKILLINFO_LANG_JP) { return "スキル説明"; }
	return "SKILL INFO";
}

static const char* SkillInfo_GetTargetLabelText(int lang)
{
	lang = SkillInfo_CheckLang(lang);

	if (lang == SKILLINFO_LANG_KO) { return "대상"; }
	if (lang == SKILLINFO_LANG_JP) { return "対象"; }
	return "TARGETS";
}

static const char* SkillInfo_GetCooldownLabelText(int lang)
{
	lang = SkillInfo_CheckLang(lang);

	if (lang == SKILLINFO_LANG_KO) { return "쿨타임"; }
	if (lang == SKILLINFO_LANG_JP) { return "CT"; }
	return "COOLDOWN";
}

static const char* SkillInfo_GetDurationLabelText(int lang)
{
	lang = SkillInfo_CheckLang(lang);

	if (lang == SKILLINFO_LANG_KO) { return "지속"; }
	if (lang == SKILLINFO_LANG_JP) { return "持続"; }
	return "DURATION";
}

static bool SkillInfo_HasNonAscii(const char* text)
{
	if (text == NULL) { return false; }

	const unsigned char* p = (const unsigned char*)text;
	while (*p != '\0') {
		if (*p >= 0x80) {
			return true;
		}
		p++;
	}

	return false;
}

static void SkillInfo_DrawText(D3DGraphics* dg, int x, int y, const char* text, int color, int fw, int fh, int lang)
{
	if (text == NULL) { return; }

	// 한글/일본어는 UTF-8 멀티바이트 문자이므로 기존 텍스처 폰트로 그리면 깨진다.
	if (SkillInfo_HasNonAscii(text)) {
		// 기존 fh <= 16은 너무 좁아서 본문까지 큰 폰트로 출력되었다.
		// SKILLS 설명에서는 제목/그룹명만 큰 폰트, 본문은 작은 폰트로 둔다.
		// CJK는 fh 21 이상부터 큰 폰트를 사용한다.
// 본문 설명은 fh 16이므로 작은 폰트 유지.
// 그룹 선택/대상/스킬 제목은 큰 폰트로 맞춘다.
		bool smallfont = (fh <= 20);

		dg->Draw2DMSFontTextUTF8(
			x + 1,
			y + 1,
			text,
			dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f),
			smallfont,
			lang
		);

		dg->Draw2DMSFontTextUTF8(
			x,
			y,
			text,
			color,
			smallfont,
			lang
		);

		return;
	}

	// 영어/숫자/기호는 기존 텍스처 폰트 유지.
	dg->Draw2DTextureFontText(x + 1, y + 1, text, dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f), fw, fh);
	dg->Draw2DTextureFontText(x, y, text, color, fw, fh);
}

static int SkillInfo_RenderLine(D3DGraphics* dg, int scrollline, int maxline, int line, int* drawline,
	int x, int y, int line_h, const char* text, int color, int fw, int fh, int lang)
{
	if ((line >= scrollline) && (*drawline < maxline)) {
		SkillInfo_DrawText(dg, x, y + (*drawline) * line_h, text, color, fw, fh, lang);
		(*drawline) += 1;
	}
	return line + 1;
}

static int SkillInfo_CountSkillLines(int groupindex, int slot, int lang)
{
	int lines = 0;

	// 제목 줄은 크게 표시하므로 2줄 높이로 계산
	lines += 2;

	// 쿨타임 / 지속
	lines += 2;

	// 효과 설명
	for (int i = 0; i < 3; i++) {
		const char* effect = SkillInfo_GetEffectText(groupindex, slot, i, lang);
		if ((effect != NULL) && (effect[0] != '\0')) {
			lines += 1;
		}
	}

	// 첫 번째 스킬 뒤에는 구분선 1줄
	if (slot == 0) {
		lines += 1;
	}

	return lines;
}

static int SkillInfo_GetDetailTotalLines(int groupindex, int lang)
{
	int total = 0;
	total += SkillInfo_CountSkillLines(groupindex, 0, lang);
	total += SkillInfo_CountSkillLines(groupindex, 1, lang);
	return total;
}

static int SkillInfo_RenderSkillBlock(D3DGraphics* dg, const SkillInfoSkillData* skill,
	int groupindex, int slot, int lang,
	int scrollline, int maxline, int line, int* drawline,
	int x, int y, int line_h, int fw, int fh)
{
	char str[256];

	int title_color = dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f);
	int text_color = dg->GetColorCode(0.82f, 0.82f, 0.82f, 1.0f);
	int separator_color = dg->GetColorCode(0.85f, 0.85f, 0.85f, 0.35f);

	const int icon_size = 52;
	const int text_x = x + 68;

	// E/F 스킬 제목줄 크기
	const int title_fw = fw + 2;
	const int title_fh = fh + 5;

	// 제목줄은 글자가 커서 실제로는 1줄보다 크게 보인다.
	// 그래서 쿨타임 줄과 겹치지 않도록 2줄 높이를 예약한다.
	const int title_lines = 2;

	sprintf(str, "%s : %s", skill->key, SkillInfo_GetSkillName(groupindex, slot, lang));

	// 스킬 제목 줄(E/F + 스킬명)
	if ((line >= scrollline) && (*drawline < maxline)) {
		int draw_y = y + (*drawline) * line_h;
		int icon_id = PlayerSkill_GetIconID(skill->skilltype);
		int icon_texture = -1;

		if (icon_id >= 0) {
			icon_texture = Resource.GetSkillIconTexture(icon_id);
		}

		if (icon_texture != -1) {
			dg->Draw2DTexture(x, draw_y - 10, icon_texture, icon_size, icon_size, 1.0f);
		}

		SkillInfo_DrawText(dg, text_x, draw_y - 1, str, title_color, title_fw, title_fh, lang);
		(*drawline) += title_lines;
	}
	line += title_lines;

	sprintf(str, "%s : %s", SkillInfo_GetCooldownLabelText(lang), SkillInfo_GetCooldownText(groupindex, slot, lang));
	line = SkillInfo_RenderLine(dg, scrollline, maxline, line, drawline, text_x, y, line_h, str, text_color, fw, fh, lang);

	sprintf(str, "%s : %s", SkillInfo_GetDurationLabelText(lang), SkillInfo_GetDurationText(groupindex, slot, lang));
	line = SkillInfo_RenderLine(dg, scrollline, maxline, line, drawline, text_x, y, line_h, str, text_color, fw, fh, lang);

	for (int i = 0; i < 3; i++) {
		const char* effect = SkillInfo_GetEffectText(groupindex, slot, i, lang);
		if ((effect != NULL) && (effect[0] != '\0')) {
			line = SkillInfo_RenderLine(dg, scrollline, maxline, line, drawline, text_x, y, line_h, effect, text_color, fw, fh, lang);
		}
	}

	// 첫 번째 스킬(E 스킬) 뒤에만 경계선 추가
	if (slot == 0) {
		if ((line >= scrollline) && (*drawline < maxline)) {
			int sep_y = y + (*drawline) * line_h + 7;

			dg->Draw2DBox(
				text_x,
				sep_y,
				SKILLINFO_X + SKILLINFO_W - 60,
				sep_y + 2,
				separator_color
			);

			(*drawline) += 1;
		}
		line += 1;
	}

	return line;
}

static void SkillInfo_RenderLanguageButtons(D3DGraphics* dg, MenuLinkTextData* langLinkData,
	int currentLang, int mouseX, int mouseY)
{
	for (int i = 0; i < SKILLINFO_LANG_DATAS; i++) {
		int color;

		if (i == currentLang) {
			color = dg->GetColorCode(1.0f, 0.9f, 0.35f, 1.0f);
		}
		else if (((SKILLINFO_X + langLinkData[i].pos_x) < mouseX) &&
			(mouseX < (SKILLINFO_X + langLinkData[i].pos_x + (signed)strlen(langLinkData[i].text) * langLinkData[i].text_w)) &&
			((SKILLINFO_Y + langLinkData[i].pos_y) < mouseY) &&
			(mouseY < (SKILLINFO_Y + langLinkData[i].pos_y + langLinkData[i].text_h))) {
			color = langLinkData[i].text_cursorcolor;
		}
		else {
			color = langLinkData[i].text_defaultcolor;
		}

		SkillInfo_DrawText(dg,
			SKILLINFO_X + langLinkData[i].pos_x,
			SKILLINFO_Y + langLinkData[i].pos_y,
			langLinkData[i].text,
			color,
			langLinkData[i].text_w,
			langLinkData[i].text_h,
			1);
	}
}

//! @brief 긆긵긘깈깛됪뽋궻룊딖돸룉뿚
void mainmenu::CreateOptions()
{
	Options_FullscreenFlag_old = GameConfig.GetFullscreenFlag();
	Options_KeyConfigID = -1;
	Options_KeyConfigCnt = 0;

	// OPTIONS 메인 버튼용 스케일.
// 창만 커지고 글씨가 작아 보이는 문제를 막기 위해 기본보다 크게 잡는다.
	float ui_scale = ((float)GameConfig.GetScreenHeight() / 1080.0f) * 1.45f;
	if (ui_scale < 1.0f) { ui_scale = 1.0f; }
	if (ui_scale > 1.35f) { ui_scale = 1.35f; }

#define SCALE(x) ((int)((x) * ui_scale))

	// 버튼에 들어갈 텍스트 배열 (원하시는 순서대로 배치)
	const char* p1_texts[OPTIONS_P1_DATAS] = {
	"CONFIG",
	"SOFTWARE LICENSE",
	"GAMEPLAY SETTING",
	"TOGGLE SETTING",
	"CROSSHAIR SETTING",
	"< EXIT >"
	};

	// 반복문으로 한 번에 해상도에 비례한 버튼 생성
	for (int i = 0; i < OPTIONS_P1_DATAS; i++) {
		Options_p1LinkTextData[i].enable = true;
		Options_p1LinkTextData[i].pos_x = SCALE(35);           // 좌측 여백
		Options_p1LinkTextData[i].pos_y = SCALE(45 + (i * 52)); // 버튼 간 세로 간격
		Options_p1LinkTextData[i].text_w = SCALE(22);          // 글자 가로 크기
		Options_p1LinkTextData[i].text_h = SCALE(28);          // 글자 세로 크기

		if (i == OPTIONS_P1_DATAS - 1) { // 마지막 EXIT 버튼 색상
			Options_p1LinkTextData[i].text_defaultcolor = d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f);
			Options_p1LinkTextData[i].text_cursorcolor = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f);
		}
		else { // 일반 버튼 색상
			Options_p1LinkTextData[i].text_defaultcolor = d3dg->GetColorCode(0.6f, 0.6f, 1.0f, 1.0f);
			Options_p1LinkTextData[i].text_cursorcolor = d3dg->GetColorCode(1.0f, 0.6f, 0.6f, 1.0f);
		}
		strcpy(Options_p1LinkTextData[i].text, p1_texts[i]);
	}
#undef SCALE


	//No.2됪뽋?깏깛긏긡긌긚긣
	strcpy(Options_p2LinkTextData[0].text, "< DEFAULT >");
	strcpy(Options_p2LinkTextData[1].text, "< SAVE >");
	strcpy(Options_p2LinkTextData[2].text, "< CANCEL >");
	for(int i=0; i<3; i++){
		Options_p2LinkTextData[i].enable = true;
		Options_p2LinkTextData[i].pos_x = 350;
		Options_p2LinkTextData[i].pos_y = 280 + i*30;
		Options_p2LinkTextData[i].text_w = 12;
		Options_p2LinkTextData[i].text_h = 16;
		Options_p2LinkTextData[i].text_defaultcolor = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
		Options_p2LinkTextData[i].text_cursorcolor = d3dg->GetColorCode(0.0f,1.0f,1.0f,1.0f);
	}

	Options_p2LinkTextData[3].pos_x = 350+20;
	Options_p2LinkTextData[3].pos_y = 20;
	strcpy(Options_p2LinkTextData[3].text, "[<<]");
	Options_p2LinkTextData[4].pos_x = 350+120;
	Options_p2LinkTextData[4].pos_y = 20;
	strcpy(Options_p2LinkTextData[4].text, "[>>]");
	Options_p2LinkTextData[5].pos_x = 350+20;
	Options_p2LinkTextData[5].pos_y = 70;
	strcpy(Options_p2LinkTextData[5].text, "[<<]");
	Options_p2LinkTextData[6].pos_x = 350+120;
	Options_p2LinkTextData[6].pos_y = 70;
	strcpy(Options_p2LinkTextData[6].text, "[>>]");
	for(int i=0; i<4; i++){
		Options_p2LinkTextData[3 + i].enable = true;
		Options_p2LinkTextData[3 + i].text_w = 12;
		Options_p2LinkTextData[3 + i].text_h = 16;
		Options_p2LinkTextData[3 + i].text_defaultcolor = d3dg->GetColorCode(0.6f,0.6f,1.0f,1.0f);
		Options_p2LinkTextData[3 + i].text_cursorcolor = d3dg->GetColorCode(1.0f,0.6f,0.6f,1.0f);
	}

	Options_p2LinkTextData[7].pos_x = 350;
	Options_p2LinkTextData[7].pos_y = 100;
	strcpy(Options_p2LinkTextData[7].text, "[ ] FULL SCREEN");
	Options_p2LinkTextData[8].pos_x = 350;
	Options_p2LinkTextData[8].pos_y = 120;
	strcpy(Options_p2LinkTextData[8].text, "[ ] WINDOW");
	Options_p2LinkTextData[9].pos_x = 350;
	Options_p2LinkTextData[9].pos_y = 160;
	strcpy(Options_p2LinkTextData[9].text, "[ ] ENABLE SOUND");
	Options_p2LinkTextData[10].pos_x = 350;
	Options_p2LinkTextData[10].pos_y = 180;
	strcpy(Options_p2LinkTextData[10].text, "[ ] ENABLE BLOOD");
	Options_p2LinkTextData[11].pos_x = 350;
	Options_p2LinkTextData[11].pos_y = 200;
	strcpy(Options_p2LinkTextData[11].text, "[ ] INVERT MOUSE");
	Options_p2LinkTextData[12].pos_x = 350;
	Options_p2LinkTextData[12].pos_y = 220;
	strcpy(Options_p2LinkTextData[12].text, "[ ] FRAME SKIP");
	Options_p2LinkTextData[13].pos_x = 350;
	Options_p2LinkTextData[13].pos_y = 240;
	strcpy(Options_p2LinkTextData[13].text, "[ ] ANOTHER GUNSIGHT");
	for(int i=0; i<7; i++){
		Options_p2LinkTextData[7 + i].enable = true;
		Options_p2LinkTextData[7 + i].text_w = 12;
		Options_p2LinkTextData[7 + i].text_h = 16;
		Options_p2LinkTextData[7 + i].text_defaultcolor = d3dg->GetColorCode(0.6f,0.6f,1.0f,1.0f);
		Options_p2LinkTextData[7 + i].text_cursorcolor = d3dg->GetColorCode(1.0f,0.6f,0.6f,1.0f);
	}

	for(int i=0; i<18; i++){
		Options_p2LinkTextData[14 + i].enable = true;
		Options_p2LinkTextData[14 + i].pos_x = 170;
		Options_p2LinkTextData[14 + i].pos_y = i*20;
		Options_p2LinkTextData[14 + i].text_w = 12;
		Options_p2LinkTextData[14 + i].text_h = 16;
		Options_p2LinkTextData[14 + i].text_defaultcolor = d3dg->GetColorCode(0.6f,0.6f,1.0f,1.0f);
		Options_p2LinkTextData[14 + i].text_cursorcolor = d3dg->GetColorCode(1.0f,0.6f,0.6f,1.0f);
		strcpy(Options_p2LinkTextData[14 + i].text, "[ ]");
	}

	Options_p3LinkTextData[0].enable = true;
	Options_p3LinkTextData[0].pos_x = (OPTIONS_P3_W - 160) / 2;
	Options_p3LinkTextData[0].pos_y = OPTIONS_P3_H - 42;
	Options_p3LinkTextData[0].text_w = 20;
	Options_p3LinkTextData[0].text_h = 26;
	Options_p3LinkTextData[0].text_defaultcolor = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
	Options_p3LinkTextData[0].text_cursorcolor = d3dg->GetColorCode(0.0f,1.0f,1.0f,1.0f);
	strcpy(Options_p3LinkTextData[0].text, "< EXIT >");

	int maxline = 21;

	// Korean 모드에서는 줄 높이 차이 때문에 약간 줄인다.
	if (GameConfig.GetLanguage() == 1) {
		maxline = 17;
	}

	scrollbar_license.Create(
		OPTIONS_P3_X + OPTIONS_P3_W - 22,
		OPTIONS_P3_Y + 68,
		20,
		OPTIONS_P3_H - 120,
		OPTIONS_LICENSELINE,
		maxline,
		0
	);
	scrollbar_license.SetColor(d3dg->GetColorCode(0.5f,0.5f,0.5f,0.5f), d3dg->GetColorCode(0.6f,0.6f,0.25f,1.0f), d3dg->GetColorCode(0.8f,0.8f,0.25f,1.0f),
		d3dg->GetColorCode(0.4f,0.67f,0.57f,1.0f), d3dg->GetColorCode(0.38f,0.77f,0.64f,1.0f), d3dg->GetColorCode(0.6f,0.3f,0.25f,1.0f), d3dg->GetColorCode(0.8f,0.3f,0.25f,1.0f));


#if (GRAPHIC_ENGINE == 0)||(GRAPHIC_ENGINE == 1)
	//깋귽긜깛긚빒뤞
	licenseinfo[0] = "";
	licenseinfo[1] = "OpenXOPS";
	licenseinfo[2] = "Copyright (c) 2014-2023, OpenXOPS Project / [-_-;](mikan) All rights reserved.";
	licenseinfo[3] = "";
	licenseinfo[4] = "Redistribution and use in source and binary forms, with or without";
	licenseinfo[5] = "modification, are permitted provided that the following conditions are met:";
	licenseinfo[6] = "* Redistributions of source code must retain the above copyright notice, ";
	licenseinfo[7] = "  this list of conditions and the following disclaimer.";
	licenseinfo[8] = "* Redistributions in binary form must reproduce the above copyright notice, ";
	licenseinfo[9] = "  this list of conditions and the following disclaimer in the documentation ";
	licenseinfo[10] = "  and/or other materials provided with the distribution.";
	licenseinfo[11] = "* Neither the name of the OpenXOPS Project nor the names of its contributors ";
	licenseinfo[12] = "  may be used to endorse or promote products derived from this software ";
	licenseinfo[13] = "  without specific prior written permission.";
	licenseinfo[14] = "";
	licenseinfo[15] = "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND";
	licenseinfo[16] = "ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED";
	licenseinfo[17] = "WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE";
	licenseinfo[18] = "DISCLAIMED. IN NO EVENT SHALL OpenXOPS Project BE LIABLE FOR ANY";
	licenseinfo[19] = "DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES";
	licenseinfo[20] = "(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;";
	licenseinfo[21] = "LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND";
	licenseinfo[22] = "ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT";
	licenseinfo[23] = "(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS";
	licenseinfo[24] = "SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.";
	licenseinfo[25] = "";
	//뛱릶믁돿렄 "OPTIONS_LICENSELINE"믦릶 뾴빾뛛
#elif GRAPHIC_ENGINE == 8
	licenseinfo[0] = "";
	licenseinfo[1] = "OpenXOPS";
	licenseinfo[2] = "Copyright (c) 2014-2023, OpenXOPS Project / [-_-;](mikan) All rights reserved.";
	licenseinfo[3] = "";
	licenseinfo[4] = " -- See the document.";
	licenseinfo[5] = "";
	//겍OpenGL궻뤾뜃갂긚긏깓?깑뷠뾴궶빁쀊귩뢯궥궴?깋궰궘궻궳뭾댰갃
#endif	//GRAPHIC_ENGINE

	Options_AdvancedID = -1;
	IsDraggingSlider = false;
	DraggingID = -1;

	// GamePlay Settings 항목
	strcpy(Options_AdvancedLinkData[0].text, "MASTER VOLUME");
	strcpy(Options_AdvancedLinkData[1].text, "PLAYER DAMAGE");
	strcpy(Options_AdvancedLinkData[2].text, "HP REGEN");
	strcpy(Options_AdvancedLinkData[3].text, "AI REACTION");
	strcpy(Options_AdvancedLinkData[4].text, "AI FIRERATE");
	strcpy(Options_AdvancedLinkData[5].text, "AI ACCURACY");
	strcpy(Options_AdvancedLinkData[6].text, "AI VIEWRANGE");
	strcpy(Options_AdvancedLinkData[7].text, "HIP RECOIL");
	strcpy(Options_AdvancedLinkData[8].text, "AIM RECOIL");
	strcpy(Options_AdvancedLinkData[9].text, "SIDE RECOIL");
	strcpy(Options_AdvancedLinkData[10].text, "RUN");
	strcpy(Options_AdvancedLinkData[11].text, "CROUCH");
	strcpy(Options_AdvancedLinkData[12].text, "NIGHTVISION");
	strcpy(Options_AdvancedLinkData[13].text, "MAP");
	strcpy(Options_AdvancedLinkData[14].text, "SKILL");
	strcpy(Options_AdvancedLinkData[15].text, "SKILL2");
	strcpy(Options_AdvancedLinkData[16].text, "[ RESET DEFAULTS ]");
	strcpy(Options_AdvancedLinkData[17].text, "< SAVE >");
	strcpy(Options_AdvancedLinkData[18].text, "< CANCEL >");
	strcpy(Options_AdvancedLinkData[19].text, "RENDER FPS");
	strcpy(Options_AdvancedLinkData[20].text, "SKILL MODE");

	for (int i = 0; i < OPTIONS_ADVANCED_DATAS; i++) {
		Options_AdvancedLinkData[i].enable = true;
		Options_AdvancedLinkData[i].text_w = 14;
		Options_AdvancedLinkData[i].text_h = 18;
		Options_AdvancedLinkData[i].text_defaultcolor = d3dg->GetColorCode(0.8f, 0.8f, 0.8f, 1.0f);
		Options_AdvancedLinkData[i].text_cursorcolor = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f);
	}

	// Gameplay Setting 2열 배치
	// 19: RENDER FPS를 가장 위에 배치
	// 0~9: 왼쪽 슬라이더
	// 10~15: 오른쪽 키 설정
	// 16~18: 아래 버튼
	Options_AdvancedLinkData[19].pos_x = 0;
	Options_AdvancedLinkData[19].pos_y = 0;

	// SKILL MODE는 RENDER FPS 오른쪽에 배치한다.
	Options_AdvancedLinkData[20].pos_x = 500;
	Options_AdvancedLinkData[20].pos_y = 0;

	// RENDER FPS가 맨 위에 들어가므로 기존 슬라이더는 한 줄 아래부터 배치한다.
	// 버튼 영역과 겹치지 않도록 28 간격을 사용한다.
	for (int i = 0; i <= 9; i++) {
		Options_AdvancedLinkData[i].pos_x = 0;
		Options_AdvancedLinkData[i].pos_y = (i + 1) * 28;
	}

	for (int i = 10; i <= 15; i++) {
		// 기존 320은 슬라이더/값 표시 영역과 겹치므로 오른쪽으로 이동
		Options_AdvancedLinkData[i].pos_x = 500;
		Options_AdvancedLinkData[i].pos_y = (i - 10 + 1) * 30;
	}

	// 아래 버튼들은 창의 가장 아래줄에 맞춘다.
	Options_AdvancedLinkData[16].pos_x = 0;
	Options_AdvancedLinkData[16].pos_y = 325;

	Options_AdvancedLinkData[17].pos_x = 500;
	Options_AdvancedLinkData[17].pos_y = 325;

	Options_AdvancedLinkData[18].pos_x = 650;
	Options_AdvancedLinkData[18].pos_y = 325;

	// Toggle Setting 항목
	// SKILL MODE는 Gameplay Setting의 RENDER FPS 오른쪽으로 이동했다.
	strcpy(Options_ToggleLinkData[0].text, "RADAR");
	strcpy(Options_ToggleLinkData[1].text, "RADAR INFO");
	strcpy(Options_ToggleLinkData[2].text, "RADAR SEE THROUGH WALLS");
	strcpy(Options_ToggleLinkData[3].text, "GRENADE TRAJECTORY");
	strcpy(Options_ToggleLinkData[4].text, "SCOPE AIM");
	strcpy(Options_ToggleLinkData[5].text, "RUN");
	strcpy(Options_ToggleLinkData[6].text, "CROUCH");
	strcpy(Options_ToggleLinkData[7].text, "[ RESET DEFAULTS ]");
	strcpy(Options_ToggleLinkData[8].text, "< SAVE >");
	strcpy(Options_ToggleLinkData[9].text, "< CANCEL >");

	for (int i = 0; i < OPTIONS_TOGGLE_DATAS; i++) {
		Options_ToggleLinkData[i].enable = true;
		Options_ToggleLinkData[i].text_w = 14;
		Options_ToggleLinkData[i].text_h = 18;
		Options_ToggleLinkData[i].text_defaultcolor = d3dg->GetColorCode(0.8f, 0.8f, 0.8f, 1.0f);
		Options_ToggleLinkData[i].text_cursorcolor = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f);
	}

	for (int i = 0; i <= 6; i++) {
		Options_ToggleLinkData[i].pos_x = 0;
		Options_ToggleLinkData[i].pos_y = i * 34;
	}

	// 아래 버튼들은 창의 가장 아래줄에 맞춘다.
	// SAVE / CANCEL은 기존 위치보다 살짝 왼쪽으로 평행이동한다.
	Options_ToggleLinkData[7].pos_x = 0;
	Options_ToggleLinkData[7].pos_y = 325;

	Options_ToggleLinkData[8].pos_x = 270;
	Options_ToggleLinkData[8].pos_y = 325;

	Options_ToggleLinkData[9].pos_x = 400;
	Options_ToggleLinkData[9].pos_y = 325;

	// SKILLS 그룹 목록 버튼
	for (int i = 0; i < SKILLINFO_GROUP_DATAS; i++) {
		SkillInfo_GroupLinkData[i].enable = true;
		SkillInfo_GroupLinkData[i].text_w = 18;
		SkillInfo_GroupLinkData[i].text_h = 26;
		SkillInfo_GroupLinkData[i].text_defaultcolor = d3dg->GetColorCode(0.75f, 0.75f, 1.0f, 1.0f);
		SkillInfo_GroupLinkData[i].text_cursorcolor = d3dg->GetColorCode(1.0f, 0.6f, 0.6f, 1.0f);

		// 2열 배치
		if (i < 6) {
			SkillInfo_GroupLinkData[i].pos_x = 78;
			SkillInfo_GroupLinkData[i].pos_y = 112 + i * 56;
		}
		else {
			SkillInfo_GroupLinkData[i].pos_x = 470;
			SkillInfo_GroupLinkData[i].pos_y = 112 + (i - 6) * 56;
		}

		strcpy(SkillInfo_GroupLinkData[i].text, SkillInfoData[i].groupname);
	}

	// BACK 버튼
	SkillInfo_GroupLinkData[SKILLINFO_GROUP_DATAS].enable = true;
	SkillInfo_GroupLinkData[SKILLINFO_GROUP_DATAS].pos_x = (SKILLINFO_W - 140) / 2;
	SkillInfo_GroupLinkData[SKILLINFO_GROUP_DATAS].pos_y = SKILLINFO_H - 44;
	SkillInfo_GroupLinkData[SKILLINFO_GROUP_DATAS].text_w = 20;
	SkillInfo_GroupLinkData[SKILLINFO_GROUP_DATAS].text_h = 28;
	SkillInfo_GroupLinkData[SKILLINFO_GROUP_DATAS].text_defaultcolor = d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f);
	SkillInfo_GroupLinkData[SKILLINFO_GROUP_DATAS].text_cursorcolor = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f);
	strcpy(SkillInfo_GroupLinkData[SKILLINFO_GROUP_DATAS].text, "< BACK >");

	// 언어 선택 버튼
	const char* lang_text[SKILLINFO_LANG_DATAS] = {
		"KOREAN",
		"ENGLISH",
		"JAPANESE"
	};

	for (int i = 0; i < SKILLINFO_LANG_DATAS; i++) {
		SkillInfo_LangLinkData[i].enable = true;
		SkillInfo_LangLinkData[i].pos_x = (SKILLINFO_W / 2) - 240 + i * 175;
		SkillInfo_LangLinkData[i].pos_y = SKILLINFO_H - 86;
		SkillInfo_LangLinkData[i].text_w = 16;
		SkillInfo_LangLinkData[i].text_h = 26;
		SkillInfo_LangLinkData[i].text_defaultcolor = d3dg->GetColorCode(0.72f, 0.72f, 0.72f, 1.0f);
		SkillInfo_LangLinkData[i].text_cursorcolor = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f);
		strcpy(SkillInfo_LangLinkData[i].text, lang_text[i]);
	}

	int skillinfo_total_lines = SkillInfo_GetDetailTotalLines(SkillInfo_SelectedGroup, SkillInfo_Language);
	if (skillinfo_total_lines < SKILLINFO_DETAIL_VISIBLE_LINES) {
		skillinfo_total_lines = SKILLINFO_DETAIL_VISIBLE_LINES;
	}

	scrollbar_skillinfo.Create(
		SKILLINFO_X + SKILLINFO_W - 30,
		SKILLINFO_Y + 108,
		22,
		SKILLINFO_DETAIL_VISIBLE_LINES * SKILLINFO_LINE_H,
		skillinfo_total_lines,
		SKILLINFO_DETAIL_VISIBLE_LINES,
		0
	);
	scrollbar_skillinfo.SetColor(
		d3dg->GetColorCode(0.5f, 0.5f, 0.5f, 0.5f),
		d3dg->GetColorCode(0.6f, 0.6f, 0.25f, 1.0f),
		d3dg->GetColorCode(0.8f, 0.8f, 0.25f, 1.0f),
		d3dg->GetColorCode(0.4f, 0.67f, 0.57f, 1.0f),
		d3dg->GetColorCode(0.38f, 0.77f, 0.64f, 1.0f),
		d3dg->GetColorCode(0.6f, 0.3f, 0.25f, 1.0f),
		d3dg->GetColorCode(0.8f, 0.3f, 0.25f, 1.0f)
	);
}



//! @brief 긆긵긘깈깛됪뽋궻볺쀍룉뿚
// 조작키 중복 여부 확인
bool mainmenu::IsDuplicateControlKey(int targetKeyID, int keycode)
{
	if (keycode < 0) { return false; }

	for (int i = 0; i < TOTAL_ControlKey; i++) {
		if (i == targetKeyID) { continue; }

		if (GameConfig.GetKeycode(i) == keycode) {
			return true;
		}
	}

	return false;
}

// 특정 키 설정에서 막아야 하는 키인지 확인
bool mainmenu::IsBlockedControlKey(int targetKeyID, int keycode)
{
	// 스킬 키는 마우스 좌클릭/우클릭 금지
	if ((targetKeyID == KEY_SKILL) || (targetKeyID == KEY_SKILL2)) {
		if (keycode == 0x12) { return true; } // MOUSE L
		if (keycode == 0x13) { return true; } // MOUSE R
	}

	return false;
}

// 실제 키 적용
bool mainmenu::TryApplyControlKey(int targetKeyID, int keycode)
{
	if (targetKeyID < 0) { return false; }
	if (keycode < 0) { return false; }

	if (IsBlockedControlKey(targetKeyID, keycode) == true) {
		return false;
	}

	if (IsDuplicateControlKey(targetKeyID, keycode) == true) {
		return false;
	}

	GameConfig.SetKeycode(targetKeyID, keycode);
	return true;
}

//! @brief 긆긵긘깈깛됪뽋궻볺쀍룉뿚
void mainmenu::InputOptions()
{
	Options_p2LinkTextID = -1;

	float ui_scale = ((float)GameConfig.GetScreenHeight() / 1080.0f) * 1.45f;
	if (ui_scale < 1.0f) { ui_scale = 1.0f; }
	if (ui_scale > 1.35f) { ui_scale = 1.35f; }

	int scaled_p1_w = (int)(520 * ui_scale);
	int scaled_p1_h = (int)(370 * ui_scale);
	int scaled_p1_x = GameConfig.GetScreenWidth() - scaled_p1_w - (int)(40 * ui_scale);
	int scaled_p1_y = GameConfig.GetScreenHeight() - scaled_p1_h - (int)(40 * ui_scale);

	if (modescreen == 1) {
		int ButtonID = -1;
		for (int i = 0; i < OPTIONS_P1_DATAS; i++) {
			if (Options_p1LinkTextData[i].enable == true) {
				if (inputCtrl->CheckMouseButtonUpL()) {
					if (((scaled_p1_x + Options_p1LinkTextData[i].pos_x) < mainmenu_mouseX) &&
						(mainmenu_mouseX < (scaled_p1_x + Options_p1LinkTextData[i].pos_x + (signed)strlen(Options_p1LinkTextData[i].text) * Options_p1LinkTextData[i].text_w)) &&
						((scaled_p1_y + Options_p1LinkTextData[i].pos_y) < mainmenu_mouseY) &&
						(mainmenu_mouseY < (scaled_p1_y + Options_p1LinkTextData[i].pos_y + Options_p1LinkTextData[i].text_h))
						) {
						ButtonID = i;
					}
				}
			}
		}

		// 옵션 메인 화면 이동
		if (ButtonID == 0) modescreen = 2; // CONFIG
		if (ButtonID == 1) { modescreen = 3; } // SOFTWARE LICENSE
		if (ButtonID == 2) {
			modescreen = 4; // GAMEPLAY SETTING
			Options_AdvancedID = -1;
			IsDraggingSlider = false;
			DraggingID = -1;
		}
		if (ButtonID == 3) {
			modescreen = 6; // TOGGLE SETTING
			Options_ToggleID = -1;
		}
		if (ButtonID == 4) modescreen = 5; // CROSSHAIR SETTING
		if (ButtonID == 5) modescreen = 0; // EXIT

		if (inputCtrl->CheckKeyDown(GetEscKeycode())) { modescreen = 0; }
	}

	else if (modescreen == 2) {
		// [✨ 수정] Config 창 전체 크기 1.2배 확대 및 중앙 정렬 스케일링
		float cfg_scale = ((float)GameConfig.GetScreenHeight() / 480.0f) * 0.7f;
		int cfg_w = (int)(600 * cfg_scale);
		int cfg_h = (int)(350 * cfg_scale);
		int cfg_x = (GameConfig.GetScreenWidth() - cfg_w) / 2;
		int cfg_y = (GameConfig.GetScreenHeight() - cfg_h) / 2 + (int)(50 * cfg_scale);

		if ((Options_KeyConfigID != -1) && (Options_KeyConfigCnt > 0)) {
			int keycode = inputCtrl->CheckInputAll();
			if (keycode != -1) {
				GameConfig.SetKeycode(Options_KeyConfigID, keycode);
				Options_KeyConfigID = -1;
				Options_KeyConfigCnt = 0;
				return;
			}
		}

		for (int i = 0; i < OPTIONS_P2_DATAS; i++) {
			if (Options_p2LinkTextData[i].enable == true) {
				if (inputCtrl->CheckMouseButtonUpL()) {
					// 확대된 크기에 맞춰 마우스 클릭 좌표도 자동 보정
					int x = cfg_x + (int)(Options_p2LinkTextData[i].pos_x * cfg_scale);
					int y = cfg_y + (int)(Options_p2LinkTextData[i].pos_y * cfg_scale);
					int w = (int)(strlen(Options_p2LinkTextData[i].text) * Options_p2LinkTextData[i].text_w * cfg_scale);
					int h = (int)(Options_p2LinkTextData[i].text_h * cfg_scale);

					if ((x < mainmenu_mouseX) && (mainmenu_mouseX < x + w) &&
						(y < mainmenu_mouseY) && (mainmenu_mouseY < y + h)) {
						Options_p2LinkTextID = i;
					}
				}
			}
		}

		if (inputCtrl->CheckKeyDown(GetEscKeycode())) { Options_p2LinkTextID = 2; }
	}

	else if( modescreen == 3 ){
		scrollbar_license.Input(mainmenu_mouseX, mainmenu_mouseY, inputCtrl->CheckMouseButtonNowL(), mainmenu_mouseY - mainmenu_mouseY_old);

		//깏깛긏긡긌긚긣뵽믦룉뿚
		int ButtonID = -1;
		for(int i=0; i<OPTIONS_P3_DATAS; i++){
			if( Options_p3LinkTextData[i].enable == true ){
				if( inputCtrl->CheckMouseButtonUpL() ){
					if( ((OPTIONS_P3_X + Options_p3LinkTextData[i].pos_x) < mainmenu_mouseX)&&(mainmenu_mouseX < (OPTIONS_P3_X + Options_p3LinkTextData[i].pos_x + (signed)strlen(Options_p3LinkTextData[i].text)*Options_p3LinkTextData[i].text_w))
						&&((OPTIONS_P3_Y + Options_p3LinkTextData[i].pos_y) < mainmenu_mouseY)&&(mainmenu_mouseY < (OPTIONS_P3_Y + Options_p3LinkTextData[i].pos_y + Options_p3LinkTextData[i].text_h))
					){
						ButtonID = i;
					}
				}
			}
		}
		if( ButtonID == 0 ){
			modescreen = 1;
		}

		if( inputCtrl->CheckKeyDown(GetEscKeycode()) ){		//ESC긌?귩룉뿚
			modescreen = 1;
		}
	}
	else if (modescreen == 7) {
		int ButtonID = -1;
		int LangID = -1;

		// 그룹 버튼 클릭
		for (int i = 0; i < SKILLINFO_GROUP_LINK_DATAS; i++) {
			if (SkillInfo_GroupLinkData[i].enable == true) {
				const char* linktext = SkillInfo_GroupLinkData[i].text;

				if (i < SKILLINFO_GROUP_DATAS) {
					linktext = SkillInfo_GetGroupName(i, SkillInfo_Language);
				}

				if (inputCtrl->CheckMouseButtonUpL()) {
					if (((SKILLINFO_X + SkillInfo_GroupLinkData[i].pos_x) < mainmenu_mouseX) &&
						(mainmenu_mouseX < (SKILLINFO_X + SkillInfo_GroupLinkData[i].pos_x + (signed)strlen(linktext) * SkillInfo_GroupLinkData[i].text_w)) &&
						((SKILLINFO_Y + SkillInfo_GroupLinkData[i].pos_y) < mainmenu_mouseY) &&
						(mainmenu_mouseY < (SKILLINFO_Y + SkillInfo_GroupLinkData[i].pos_y + SkillInfo_GroupLinkData[i].text_h))) {
						ButtonID = i;
					}
				}
			}
		}

		// 언어 버튼 클릭
		for (int i = 0; i < SKILLINFO_LANG_DATAS; i++) {
			if (SkillInfo_LangLinkData[i].enable == true) {
				if (inputCtrl->CheckMouseButtonUpL()) {
					if (((SKILLINFO_X + SkillInfo_LangLinkData[i].pos_x) < mainmenu_mouseX) &&
						(mainmenu_mouseX < (SKILLINFO_X + SkillInfo_LangLinkData[i].pos_x + (signed)strlen(SkillInfo_LangLinkData[i].text) * SkillInfo_LangLinkData[i].text_w)) &&
						((SKILLINFO_Y + SkillInfo_LangLinkData[i].pos_y) < mainmenu_mouseY) &&
						(mainmenu_mouseY < (SKILLINFO_Y + SkillInfo_LangLinkData[i].pos_y + SkillInfo_LangLinkData[i].text_h))) {
						LangID = i;
					}
				}
			}
		}

		if ((0 <= LangID) && (LangID < SKILLINFO_LANG_DATAS)) {
			SkillInfo_Language = LangID;
		}
		else if ((0 <= ButtonID) && (ButtonID < SKILLINFO_GROUP_DATAS)) {
			SkillInfo_SelectedGroup = ButtonID;

			int skillinfo_total_lines = SkillInfo_GetDetailTotalLines(SkillInfo_SelectedGroup, SkillInfo_Language);
			if (skillinfo_total_lines < SKILLINFO_DETAIL_VISIBLE_LINES) {
				skillinfo_total_lines = SKILLINFO_DETAIL_VISIBLE_LINES;
			}

			scrollbar_skillinfo.Create(
				SKILLINFO_X + SKILLINFO_W - 30,
				SKILLINFO_Y + 108,
				22,
				SKILLINFO_DETAIL_VISIBLE_LINES * SKILLINFO_LINE_H,
				skillinfo_total_lines,
				SKILLINFO_DETAIL_VISIBLE_LINES,
				0
			);
			scrollbar_skillinfo.SetColor(
				d3dg->GetColorCode(0.5f, 0.5f, 0.5f, 0.5f),
				d3dg->GetColorCode(0.6f, 0.6f, 0.25f, 1.0f),
				d3dg->GetColorCode(0.8f, 0.8f, 0.25f, 1.0f),
				d3dg->GetColorCode(0.4f, 0.67f, 0.57f, 1.0f),
				d3dg->GetColorCode(0.38f, 0.77f, 0.64f, 1.0f),
				d3dg->GetColorCode(0.6f, 0.3f, 0.25f, 1.0f),
				d3dg->GetColorCode(0.8f, 0.3f, 0.25f, 1.0f)
			);

			modescreen = 8;
		}
		else if (ButtonID == SKILLINFO_GROUP_DATAS) {
			modescreen = 0;
		}

		if (inputCtrl->CheckKeyDown(GetEscKeycode())) {
			modescreen = 0;
		}
	}

		else if (modescreen == 8) {
		scrollbar_skillinfo.Input(mainmenu_mouseX, mainmenu_mouseY, inputCtrl->CheckMouseButtonNowL(), mainmenu_mouseY - mainmenu_mouseY_old);

		int ButtonID = -1;
		int LangID = -1;
		int i = SKILLINFO_GROUP_DATAS;

		// BACK 버튼
		if (SkillInfo_GroupLinkData[i].enable == true) {
			if (inputCtrl->CheckMouseButtonUpL()) {
				if (((SKILLINFO_X + SkillInfo_GroupLinkData[i].pos_x) < mainmenu_mouseX) &&
					(mainmenu_mouseX < (SKILLINFO_X + SkillInfo_GroupLinkData[i].pos_x + (signed)strlen(SkillInfo_GroupLinkData[i].text) * SkillInfo_GroupLinkData[i].text_w)) &&
					((SKILLINFO_Y + SkillInfo_GroupLinkData[i].pos_y) < mainmenu_mouseY) &&
					(mainmenu_mouseY < (SKILLINFO_Y + SkillInfo_GroupLinkData[i].pos_y + SkillInfo_GroupLinkData[i].text_h))) {
					ButtonID = i;
				}
			}
		}

		// 언어 버튼
		for (int l = 0; l < SKILLINFO_LANG_DATAS; l++) {
			if (SkillInfo_LangLinkData[l].enable == true) {
				if (inputCtrl->CheckMouseButtonUpL()) {
					if (((SKILLINFO_X + SkillInfo_LangLinkData[l].pos_x) < mainmenu_mouseX) &&
						(mainmenu_mouseX < (SKILLINFO_X + SkillInfo_LangLinkData[l].pos_x + (signed)strlen(SkillInfo_LangLinkData[l].text) * SkillInfo_LangLinkData[l].text_w)) &&
						((SKILLINFO_Y + SkillInfo_LangLinkData[l].pos_y) < mainmenu_mouseY) &&
						(mainmenu_mouseY < (SKILLINFO_Y + SkillInfo_LangLinkData[l].pos_y + SkillInfo_LangLinkData[l].text_h))) {
						LangID = l;
					}
				}
			}
		}

		if ((0 <= LangID) && (LangID < SKILLINFO_LANG_DATAS)) {
			SkillInfo_Language = LangID;

			int skillinfo_total_lines = SkillInfo_GetDetailTotalLines(SkillInfo_SelectedGroup, SkillInfo_Language);
			if (skillinfo_total_lines < SKILLINFO_DETAIL_VISIBLE_LINES) {
				skillinfo_total_lines = SKILLINFO_DETAIL_VISIBLE_LINES;
			}

			scrollbar_skillinfo.Create(
				SKILLINFO_X + SKILLINFO_W - 30,
				SKILLINFO_Y + 108,
				22,
				SKILLINFO_DETAIL_VISIBLE_LINES * SKILLINFO_LINE_H,
				skillinfo_total_lines,
				SKILLINFO_DETAIL_VISIBLE_LINES,
				0
			);
			scrollbar_skillinfo.SetColor(
				d3dg->GetColorCode(0.5f, 0.5f, 0.5f, 0.5f),
				d3dg->GetColorCode(0.6f, 0.6f, 0.25f, 1.0f),
				d3dg->GetColorCode(0.8f, 0.8f, 0.25f, 1.0f),
				d3dg->GetColorCode(0.4f, 0.67f, 0.57f, 1.0f),
				d3dg->GetColorCode(0.38f, 0.77f, 0.64f, 1.0f),
				d3dg->GetColorCode(0.6f, 0.3f, 0.25f, 1.0f),
				d3dg->GetColorCode(0.8f, 0.3f, 0.25f, 1.0f)
			);
		}
		else if (ButtonID == SKILLINFO_GROUP_DATAS) {
			modescreen = 7;
		}

		if (inputCtrl->CheckKeyDown(GetEscKeycode())) {
			modescreen = 7;
		}
	}

	if (modescreen == 4) {
		float cfg_scale = ((float)GameConfig.GetScreenHeight() / 480.0f) * 0.7f;

		// 고해상도에서 너무 커지면 내부 항목이 겹치므로 상한을 둔다.
		if (cfg_scale > 1.15f) { cfg_scale = 1.15f; }

		float ui_scale = cfg_scale;

		// Gameplay Setting은 슬라이더 열과 키 설정 열을 함께 보여주므로 900 폭을 사용한다.
// RenderOptions 쪽과 반드시 같은 폭을 사용해야 표시와 클릭 판정이 맞는다.
		int scaled_menuW = (int)(900 * cfg_scale);
		int scaled_menuH = (int)(350 * cfg_scale);
		int startX = (GameConfig.GetScreenWidth() - scaled_menuW) / 2;
		int startY = (GameConfig.GetScreenHeight() - scaled_menuH) / 2 + (int)(50 * cfg_scale);

		int scaled_sliderW = (int)(160 * cfg_scale);

		bool releasedAfterSliderDrag = false;

		if (inputCtrl->CheckMouseButtonUpL()) {
			releasedAfterSliderDrag = IsDraggingSlider;
			IsDraggingSlider = false;
			DraggingID = -1;
		}

		Options_AdvancedID = -1;
		for (int i = 0; i < OPTIONS_ADVANCED_DATAS; i++) {
			int x = startX + (int)(Options_AdvancedLinkData[i].pos_x * ui_scale);
			int y = startY + (int)(Options_AdvancedLinkData[i].pos_y * ui_scale);

			int hit_w = (int)(500 * ui_scale);
			int hit_h = (int)(28 * ui_scale);

			if (i == 19) {
				// RENDER FPS는 이름 + 값 표시 영역까지 클릭 가능하게 한다.
				hit_w = (int)(500 * ui_scale);
			}
			else if (i == 20) {
				// SKILL MODE는 RENDER FPS 오른쪽에 있으므로 이름 + ON/OFF 값까지 클릭 가능하게 한다.
				hit_w = (int)(330 * ui_scale);
			}
			else if ((10 <= i) && (i <= 15)) {
				hit_w = (int)(330 * ui_scale);
			}
			else if (i == 16) {
				// [ RESET DEFAULTS ]는 SAVE/CANCEL보다 글자가 길기 때문에
				// 클릭 범위를 더 넓게 잡는다.
				hit_w = (int)(300 * ui_scale);
			}
			else if (i >= 17) {
				hit_w = (int)(170 * ui_scale);
			}

			if ((x < mainmenu_mouseX) && (mainmenu_mouseX < x + hit_w) &&
				(y < mainmenu_mouseY) && (mainmenu_mouseY < y + hit_h)) {
				Options_AdvancedID = i;
			}
		}

		if (Options_KeyConfigID != -1 && Options_KeyConfigCnt > 0) {
			Options_KeyConfigCnt--;

			if (Options_KeyConfigCnt <= 0) {
				Options_KeyConfigID = -1;
				Options_KeyConfigCnt = 0;
				return;
			}

			// ESC를 누르면 키 변경 취소
			if (inputCtrl->CheckKeyDown(GetEscKeycode())) {
				Options_KeyConfigID = -1;
				Options_KeyConfigCnt = 0;
				return;
			}

			int keycode = inputCtrl->CheckInputAll();

			if (keycode != -1) {
				if (TryApplyControlKey(Options_KeyConfigID, keycode) == true) {
					Options_KeyConfigID = -1;
					Options_KeyConfigCnt = 0;
				}
				else {
					// 중복 키 또는 금지 키면 적용하지 않고 입력 대기 유지
					Options_KeyConfigCnt = (int)(GAMEFPS * 3);
				}

				return;
			}
		}

		if (inputCtrl->CheckMouseButtonUpL() && (releasedAfterSliderDrag == false)) {

			if (Options_AdvancedID == 10) { Options_KeyConfigID = KEY_RUN; Options_KeyConfigCnt = (int)(GAMEFPS * 5); }
			if (Options_AdvancedID == 11) { Options_KeyConfigID = KEY_CROUCH; Options_KeyConfigCnt = (int)(GAMEFPS * 5); }
			if (Options_AdvancedID == 12) { Options_KeyConfigID = KEY_NIGHTVISION; Options_KeyConfigCnt = (int)(GAMEFPS * 5); }
			if (Options_AdvancedID == 13) { Options_KeyConfigID = KEY_MAP; Options_KeyConfigCnt = (int)(GAMEFPS * 5); }
			if (Options_AdvancedID == 14) { Options_KeyConfigID = KEY_SKILL; Options_KeyConfigCnt = (int)(GAMEFPS * 5); }
			if (Options_AdvancedID == 15) { Options_KeyConfigID = KEY_SKILL2; Options_KeyConfigCnt = (int)(GAMEFPS * 5); }

			if (Options_AdvancedID == 19) {
				// 144Hz 렌더 루프는 유지하되 보간은 쓰지 않는다.
				// 여기서는 렌더링 FPS 제한만 순환 변경한다.
				int fps = GameConfig.GetRenderFpsLimit();

				if (fps == 0) {
					GameConfig.SetRenderFpsLimit(60);
				}
				else if (fps <= 60) {
					GameConfig.SetRenderFpsLimit(120);
				}
				else if (fps <= 120) {
					GameConfig.SetRenderFpsLimit(144);
				}
				else if (fps <= 144) {
					GameConfig.SetRenderFpsLimit(240);
				}
				else if (fps <= 240) {
					GameConfig.SetRenderFpsLimit(0);
				}
				else {
					GameConfig.SetRenderFpsLimit(60);
				}
			}

			if (Options_AdvancedID == 20) {
				GameConfig.SetSkillModeFlag(!GameConfig.GetSkillModeFlag());
			}

			if (Options_AdvancedID == 16) {
				GameConfig.SetMasterVolume(1.0f);
				SoundCtrl.SetVolume(1.0f);
				GameConfig.SetPlayerDamageMultiplier(0.1f);
				GameConfig.SetPlayerHealthRegenAmount(10);

				GameConfig.SetAIReactionSpeedMultiplier(1.0f);
				GameConfig.SetAIFireRateMultiplier(1.0f);
				GameConfig.SetAIAccuracyMultiplier(1.0f);
				GameConfig.SetAISearchRangeMultiplier(1.0f);

				GameConfig.SetRecoilHipMultiplier(1.0f);
				GameConfig.SetRecoilAimMultiplier(1.0f);
				GameConfig.SetRecoilSideMultiplier(0.0f);

				// RENDER FPS와 SKILL MODE는 Gameplay Setting 화면에 있으므로 여기서 초기화한다.
				GameConfig.SetRenderFpsLimit(RENDERFPS_DEFAULT);
				GameConfig.SetSkillModeFlag(false);

				// 나머지 토글 계열 설정은 TOGGLE SETTING 화면에서 초기화한다.

// WALK는 이 화면에서 제거했으므로 여기서 초기화하지 않는 것을 추천
				// GameConfig.SetKeycode(KEY_WALK, 0x10);

				GameConfig.SetKeycode(KEY_RUN, 0x14);
				GameConfig.SetKeycode(KEY_CROUCH, 0x15);
				GameConfig.SetKeycode(KEY_NIGHTVISION, 0x2D);
				GameConfig.SetKeycode(KEY_MAP, 0x2C);
				GameConfig.SetKeycode(KEY_SKILL, 0x24); // E
				GameConfig.SetKeycode(KEY_SKILL2, 0x25); // F
			}

			if (Options_AdvancedID == 17) {
				GameConfig.SaveFile("config.dat");
				IsDraggingSlider = false;
				DraggingID = -1;
				modescreen = 1;
				return;
			}

			if (Options_AdvancedID == 18) {
				GameConfig.LoadFile("config.dat");
				IsDraggingSlider = false;
				DraggingID = -1;
				modescreen = 1;
				return;
			}
		}

		if (inputCtrl->CheckMouseButtonDownL()) {
			for (int i = 0; i <= 9; i++) {
				int sliderX = startX + (int)(260 * ui_scale);
				int sliderY = startY + (int)(Options_AdvancedLinkData[i].pos_y * ui_scale);

				if ((sliderX - 30 <= mainmenu_mouseX) && (mainmenu_mouseX <= sliderX + scaled_sliderW + 30) &&
					(sliderY - 10 <= mainmenu_mouseY) && (mainmenu_mouseY <= sliderY + (int)(30 * ui_scale))) {
					IsDraggingSlider = true;
					DraggingID = i;
				}
			}
		}

		if (IsDraggingSlider && DraggingID != -1) {
			int sliderX = startX + (int)(260 * ui_scale);

			// Do not update the slider when the mouse is far outside the slider area.
			// This prevents dragging into the right-side key/button area from changing values.
			if ((sliderX - (int)(30 * ui_scale) <= mainmenu_mouseX) &&
				(mainmenu_mouseX <= sliderX + scaled_sliderW + (int)(30 * ui_scale))) {

				float ratio = (float)(mainmenu_mouseX - sliderX) / (float)scaled_sliderW;
				if (ratio < 0.0f) ratio = 0.0f;
				if (ratio > 1.0f) ratio = 1.0f;

				switch (DraggingID) {
			case 0:
				GameConfig.SetMasterVolume(ratio);
				SoundCtrl.SetVolume(ratio);
				break;

			case 1:
				// PLAYER DAMAGE: 0.0 ~ 3.0
				GameConfig.SetPlayerDamageMultiplier(((int)((ratio * 3.0f) * 10.0f + 0.5f)) / 10.0f);
				break;

			case 2:
				// HP REGEN: 슬라이더 비율(ratio)에 따라 0 ~ 50까지 값 적용
				GameConfig.SetPlayerHealthRegenAmount((int)(ratio * 100.0f));
				break;

			case 3:
				// AI REACTION: 0.0 ~ 2.0, 0.1 단위
				GameConfig.SetAIReactionSpeedMultiplier(((int)((ratio * 2.0f) * 10.0f + 0.5f)) / 10.0f);
				break;

			case 4:
				// AI FIRERATE: 0.0 ~ 4.0, 0.1 단위
				GameConfig.SetAIFireRateMultiplier(((int)((ratio * 4.0f) * 10.0f + 0.5f)) / 10.0f);
				break;

			case 5:
				// AI ACCURACY: 0.0 ~ 3.0, 0.1 단위
				GameConfig.SetAIAccuracyMultiplier(((int)((ratio * 3.0f) * 10.0f + 0.5f)) / 10.0f);
				break;

			case 6:
				// AI VIEWRANGE: 0.0 ~ 2.5, 0.1 단위
				GameConfig.SetAISearchRangeMultiplier(((int)((ratio * 2.5f) * 10.0f + 0.5f)) / 10.0f);
				break;

			case 7:
				GameConfig.SetRecoilHipMultiplier(ratio * 5.0f);
				break;

			case 8:
				GameConfig.SetRecoilAimMultiplier(ratio * 5.0f);
				break;

			case 9:
				// SIDE RECOIL: 0.0 ~ 1.0
				GameConfig.SetRecoilSideMultiplier(((int)(ratio * 10.0f + 0.5f)) / 10.0f);
				break;
				}
			}
		}
		if (inputCtrl->CheckKeyDown(GetEscKeycode()) == true) {
			GameConfig.LoadFile("config.dat"); IsDraggingSlider = false; DraggingID = -1; modescreen = 1;
		}
	}
		else if (modescreen == 6) {
			float cfg_scale = ((float)GameConfig.GetScreenHeight() / 480.0f) * 0.7f;

			// Gameplay Setting과 같은 크기 기준을 사용한다.
			if (cfg_scale > 1.15f) { cfg_scale = 1.15f; }

			float ui_scale = cfg_scale;

			// Toggle Setting은 가장 긴 항목인 RADAR SEE THROUGH WALLS와 값 표시 기준으로 맞춘다.
// 620도 아직 약간 넓으므로 560으로 축소한다.
			int scaled_menuW = (int)(560 * cfg_scale);
			int scaled_menuH = (int)(350 * cfg_scale);
			int startX = (GameConfig.GetScreenWidth() - scaled_menuW) / 2;
			int startY = (GameConfig.GetScreenHeight() - scaled_menuH) / 2 + (int)(50 * cfg_scale);

			Options_ToggleID = -1;
			for (int i = 0; i < OPTIONS_TOGGLE_DATAS; i++) {
				int x = startX + (int)(Options_ToggleLinkData[i].pos_x * ui_scale);
				int y = startY + (int)(Options_ToggleLinkData[i].pos_y * ui_scale);

				int hit_w = (int)(520 * ui_scale);
				int hit_h = (int)(30 * ui_scale);

				if (i <= 6) {
					// 설정 항목은 이름 + 값 표시 영역까지 클릭 가능하게 한다.
					hit_w = (int)(520 * ui_scale);
				}
				else if (i == 7) {
					// [ RESET DEFAULTS ]
					hit_w = (int)(270 * ui_scale);
				}
				else if (i == 8) {
					// < SAVE >
					hit_w = (int)(115 * ui_scale);
				}
				else if (i == 9) {
					// < CANCEL >
					hit_w = (int)(145 * ui_scale);
				}

				if ((x <= mainmenu_mouseX) && (mainmenu_mouseX <= x + hit_w) &&
					(y <= mainmenu_mouseY) && (mainmenu_mouseY <= y + hit_h)) {
					Options_ToggleID = i;
				}
			}

			if (inputCtrl->CheckMouseButtonUpL()) {
				if (Options_ToggleID == 0) {
					GameConfig.SetRadarEnabledFlag(!GameConfig.GetRadarEnabledFlag());
				}

				if (Options_ToggleID == 1) {
					GameConfig.SetRadarInfoHudFlag(!GameConfig.GetRadarInfoHudFlag());
				}

				if (Options_ToggleID == 2) {
					GameConfig.SetRadarSeeThroughWallsFlag(!GameConfig.GetRadarSeeThroughWallsFlag());
				}

				if (Options_ToggleID == 3) {
					GameConfig.SetGrenadeTrajectoryToggleFlag(!GameConfig.GetGrenadeTrajectoryToggleFlag());
				}

				if (Options_ToggleID == 4) {
					GameConfig.SetScopeAimToggleFlag(!GameConfig.GetScopeAimToggleFlag());
				}

				if (Options_ToggleID == 5) {
					GameConfig.SetRunToggleFlag(!GameConfig.GetRunToggleFlag());
				}

				if (Options_ToggleID == 6) {
					GameConfig.SetCrouchToggleFlag(!GameConfig.GetCrouchToggleFlag());
				}

				if (Options_ToggleID == 7) {
					GameConfig.SetRadarEnabledFlag(true);
					GameConfig.SetRadarInfoHudFlag(true);
					GameConfig.SetRadarSeeThroughWallsFlag(true);
					GameConfig.SetGrenadeTrajectoryToggleFlag(true);
					GameConfig.SetScopeAimToggleFlag(true);
					GameConfig.SetRunToggleFlag(false);
					GameConfig.SetCrouchToggleFlag(false);
				}

				if (Options_ToggleID == 8) {
					GameConfig.SaveFile("config.dat");
					modescreen = 1;
					return;
				}

				if (Options_ToggleID == 9) {
					GameConfig.LoadFile("config.dat");
					modescreen = 1;
					return;
				}
			}

			if (inputCtrl->CheckKeyDown(GetEscKeycode())) {
				GameConfig.LoadFile("config.dat");
				modescreen = 1;
				return;
			}
	}
	// -------------------------------------------------------------
	// [✨ 신규] 해상도 설정 화면 (modescreen 5)
	// -------------------------------------------------------------
		else if (modescreen == 5) {
			float ui_scale = (float)GameConfig.GetScreenHeight() / 1080.0f;

			// 화면이 너무 길어지지 않도록 줄 간격은 유지한다.
			int rowStep = (int)(32 * ui_scale);
			int colStep = (int)(380 * ui_scale);

			int scaled_menuW = (int)(750 * ui_scale);

			// STYLE 항목과 COLOR 제목 사이에 공백 1줄을 추가하므로 높이를 약간 늘린다.
			int scaled_menuH = (int)(18 * 32 * ui_scale);

			int startX = (GameConfig.GetScreenWidth() - scaled_menuW) / 2;
			int startY = (GameConfig.GetScreenHeight() - scaled_menuH) / 2 + (int)(50 * ui_scale);

			int hoverID = -1;
			for (int i = 0; i < OPTIONS_CROSSHAIR_DATAS; i++) {
				int row = 0, col = 0;

				if (i < 10) {
					// [STYLE] 제목 아래 1~5행
					row = 1 + (i / 2);
					col = i % 2;
				}
				else if (i < 20) {
					// STYLE 항목 아래 공백 1줄 후 [COLOR] 제목,
					// COLOR 항목은 8~12행에 배치한다.
					row = 8 + ((i - 10) / 2);
					col = (i - 10) % 2;
				}
				else if (i == 20) {
					// COLOR 항목 아래 공백 1줄 후 OUTLINE
					row = 14;
					col = 0;
				}
				else if (i == 21) {
					// OUTLINE 아래 공백 1줄 후 RESET DEFAULTS
					row = 16;
					col = 0;
				}
				else {
					// RESET DEFAULTS와 같은 줄에 BACK
					row = 16;
					col = 1;
				}

				int x = startX + (col * colStep);
				int y = startY + (row * rowStep);

				int hit_w = (int)(300 * ui_scale);

				// 클릭 판정이 표시 위치보다 살짝 빡빡하게 느껴질 수 있으므로
				// 렌더링과 입력 양쪽 모두 30으로 통일한다.
				int hit_h = (int)(30 * ui_scale);

				if (i == 20) {
					// OUTLINE [ ON/OFF ] 값까지 클릭 가능하게 넓게 잡는다.
					hit_w = (int)(430 * ui_scale);
				}
				else if (i == 21) {
					hit_w = (int)(310 * ui_scale);
				}
				else if (i == 22) {
					hit_w = (int)(220 * ui_scale);
				}

				if ((x <= mainmenu_mouseX) && (mainmenu_mouseX <= x + hit_w) &&
					(y <= mainmenu_mouseY) && (mainmenu_mouseY <= y + hit_h)) {
					hoverID = i;
				}
			}

		if (inputCtrl->CheckMouseButtonUpL() && hoverID != -1) {
			if (hoverID >= 0 && hoverID <= 9) {
				GameConfig.SetCrosshairStyle(hoverID);
			}
			if (hoverID >= 10 && hoverID <= 19) {
				GameConfig.SetCrosshairColor(hoverID - 10);
			}
			if (hoverID == 20) {
				GameConfig.SetCrosshairOutline(!GameConfig.GetCrosshairOutline());
			}
			if (hoverID == 21) {
				// Crosshair defaults
				GameConfig.SetCrosshairStyle(0);
				GameConfig.SetCrosshairColor(0);
				GameConfig.SetCrosshairOutline(false);
			}
			if (hoverID == 22) {
				GameConfig.SaveFile("config.dat");
				modescreen = 1;
			}
		}
		if (inputCtrl->CheckKeyDown(GetEscKeycode()) == true) {
			GameConfig.SaveFile("config.dat"); modescreen = 1;
		}
		}
}

//! @brief 긆긵긘깈깛됪뽋궻긽귽깛룉뿚
void mainmenu::ProcessOptions()
{
	if( modescreen == 2 ){

		//Default
		if( Options_p2LinkTextID == 0 ){
			//긢긲긅깑긣궻뭠벶귒뜛귒
			GameConfig.SetDefaultConfig();
		}

		//Save
		if( Options_p2LinkTextID == 1 ){
			//긂귻깛긤긂갋긲깑긚긏깏?깛먛귟뫶궑
			if( Options_FullscreenFlag_old != GameConfig.GetFullscreenFlag() ){
				if( ChangeWindowMode(WindowCtrl, d3dg, inputCtrl, this, GameConfig.GetFullscreenFlag()) != 0 ){
					//렪봲궢궫귞먠믦귩뽣궥
					GameConfig.SetFullscreenFlag(Options_FullscreenFlag_old);
				}
				else{
					//맟뚻궢궫귞먠믦귩뵿뎕궥귡
					Options_FullscreenFlag_old = GameConfig.GetFullscreenFlag();
				}
			}

			//뚼됈돶띋먠믦
			float volume;
			if( GameConfig.GetSoundFlag() == false ){
				volume = 0.0f;
			}
			else{
				volume = 1.0f;
			}
			SoundCtrl.SetVolume(volume);

			//먠믦긲?귽깑룕궖뜛귒
			if( GameConfig.SaveFile("config.dat") == 1 ){
				//MainWindow.ErrorInfo("config data save failed");
				//return 1;
			}

			Options_KeyConfigID = -1;
			Options_KeyConfigCnt = 0;

			modescreen = 1;
		}

		//Cancel
		if( Options_p2LinkTextID == 2 ){
			//먠믦긲?귽깑벶귒뮳궥
			if( GameConfig.LoadFile("config.dat") == 1 ){
				//MainWindow.ErrorInfo("config data open failed");
				//return 1;
			}

			Options_FullscreenFlag_old = GameConfig.GetFullscreenFlag();
			Options_KeyConfigID = -1;
			Options_KeyConfigCnt = 0;

			modescreen = 1;
		}

		if( Options_p2LinkTextID == 3 ){
			int param = GameConfig.GetMouseSensitivity();
			param -= 1;
			if( param < 1 ){ param = 1; }
			GameConfig.SetMouseSensitivity(param);
		}
		if( Options_p2LinkTextID == 4 ){
			int param = GameConfig.GetMouseSensitivity();
			param += 1;
			if( param > 50 ){ param = 50; }
			GameConfig.SetMouseSensitivity(param);
		}
		if( Options_p2LinkTextID == 5 ){
			int param = GameConfig.GetBrightness();
			param -= 1;
			if( param < 0 ){ param = 0; }
			GameConfig.SetBrightness(param);
		}
		if( Options_p2LinkTextID == 6 ){
			int param = GameConfig.GetBrightness();
			param += 1;
			if( param > 10 ){ param = 10; }
			GameConfig.SetBrightness(param);
		}

		if( Options_p2LinkTextID == 7 ){
			GameConfig.SetFullscreenFlag(true);
		}
		if( Options_p2LinkTextID == 8 ){
			GameConfig.SetFullscreenFlag(false);
		}
		if( Options_p2LinkTextID == 9 ){
			if( GameConfig.GetSoundFlag() == true ){ GameConfig.SetSoundFlag(false); }
			else { GameConfig.SetSoundFlag(true); }
		}
		if( Options_p2LinkTextID == 10 ){
			if( GameConfig.GetBloodFlag() == true ){ GameConfig.SetBloodFlag(false); }
			else { GameConfig.SetBloodFlag(true); }
		}
		if( Options_p2LinkTextID == 11 ){
			if( GameConfig.GetInvertMouseFlag() == true ){ GameConfig.SetInvertMouseFlag(false); }
			else { GameConfig.SetInvertMouseFlag(true); }
		}
		if( Options_p2LinkTextID == 12 ){
			if( GameConfig.GetFrameskipFlag() == true ){ GameConfig.SetFrameskipFlag(false); }
			else { GameConfig.SetFrameskipFlag(true); }
		}
		if( Options_p2LinkTextID == 13 ){
			if( GameConfig.GetAnotherGunsightFlag() == true ){ GameConfig.SetAnotherGunsightFlag(false); }
			else { GameConfig.SetAnotherGunsightFlag(true); }
		}

		//긌?먠믦뾭?긇긂깛긣긜긞긣
		for(int i=0; i<18; i++){
			if( Options_p2LinkTextID == (14 + i) ){
				Options_KeyConfigID = i;
				Options_KeyConfigCnt = (int)(GAMEFPS*5);
			}
		}

		//긌?먠믦뾭?긇긂깛긣?긂깛
		if( (Options_KeyConfigID != -1)&&(Options_KeyConfigCnt > 0) ){
			Options_KeyConfigCnt -= 1;
		}
		else{
			Options_KeyConfigID = -1;
			Options_KeyConfigCnt = 0;
		}
	}
}

//! @brief 긆긵긘깈깛됪뽋궻?됪룉뿚
void mainmenu::RenderOptions()
{
	float ui_scale = ((float)GameConfig.GetScreenHeight() / 1080.0f) * 1.45f;
	if (ui_scale < 1.0f) { ui_scale = 1.0f; }
	if (ui_scale > 1.35f) { ui_scale = 1.35f; }

	int scaled_p1_w = (int)(520 * ui_scale);
	int scaled_p1_h = (int)(370 * ui_scale);
	int scaled_p1_x = GameConfig.GetScreenWidth() - scaled_p1_w - (int)(40 * ui_scale);
	int scaled_p1_y = GameConfig.GetScreenHeight() - scaled_p1_h - (int)(40 * ui_scale);

	if (modescreen == 1) {
		int color;

		// 배경 박스
		d3dg->Draw2DBox(scaled_p1_x, scaled_p1_y, scaled_p1_x + scaled_p1_w, scaled_p1_y + scaled_p1_h, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.6f));

		for (int i = 0; i < OPTIONS_P1_DATAS; i++) {
			// (이하 마우스 오버 판정 및 글씨 그리는 로직 유지)
			if (Options_p1LinkTextData[i].enable == true) {
				// 마우스 오버 색상 판정
				if (((scaled_p1_x + Options_p1LinkTextData[i].pos_x) < mainmenu_mouseX) &&
					(mainmenu_mouseX < (scaled_p1_x + Options_p1LinkTextData[i].pos_x + (signed)strlen(Options_p1LinkTextData[i].text) * Options_p1LinkTextData[i].text_w)) &&
					((scaled_p1_y + Options_p1LinkTextData[i].pos_y) < mainmenu_mouseY) &&
					(mainmenu_mouseY < (scaled_p1_y + Options_p1LinkTextData[i].pos_y + Options_p1LinkTextData[i].text_h))
					) {
					color = Options_p1LinkTextData[i].text_cursorcolor;
				}
				else {
					color = Options_p1LinkTextData[i].text_defaultcolor;
				}

				// 그림자(1px 오프셋) 그리기
				d3dg->Draw2DTextureFontText(scaled_p1_x + Options_p1LinkTextData[i].pos_x + 1, scaled_p1_y + Options_p1LinkTextData[i].pos_y + 1,
					Options_p1LinkTextData[i].text, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f), Options_p1LinkTextData[i].text_w, Options_p1LinkTextData[i].text_h);

				// 본 텍스트 그리기
				d3dg->Draw2DTextureFontText(scaled_p1_x + Options_p1LinkTextData[i].pos_x, scaled_p1_y + Options_p1LinkTextData[i].pos_y,
					Options_p1LinkTextData[i].text, color, Options_p1LinkTextData[i].text_w, Options_p1LinkTextData[i].text_h);
			}
		}
	}

	if (modescreen == 2) {
		int color;
		char str[32];

		// [✨ 수정] Config 창 스케일링 및 폰트 크기 1.2배 확대 적용
		float cfg_scale = ((float)GameConfig.GetScreenHeight() / 480.0f) * 0.7f;
		int cfg_w = (int)(600 * cfg_scale);
		int cfg_h = (int)(350 * cfg_scale);
		int cfg_x = (GameConfig.GetScreenWidth() - cfg_w) / 2;
		int cfg_y = (GameConfig.GetScreenHeight() - cfg_h) / 2 + (int)(50 * cfg_scale);

		int fw = (int)(12 * cfg_scale); // 기본 너비 확대
		int fh = (int)(16 * cfg_scale); // 기본 높이 확대

		// 배경 박스 (여백 추가)
		d3dg->Draw2DBox(cfg_x - (int)(30 * cfg_scale), cfg_y - (int)(30 * cfg_scale),
			cfg_x + cfg_w + (int)(30 * cfg_scale), cfg_y + cfg_h + (int)(30 * cfg_scale),
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.6f));

		// 기존 고정 좌표를 비율에 맞춰 변환해 주는 매크로
#define RX(x) (cfg_x + (int)((x) * cfg_scale))
#define RY(y) (cfg_y + (int)((y) * cfg_scale))

// MOUSE SENSITIVITY
		d3dg->Draw2DTextureFontText(RX(350) + 1, RY(0) + 1, "MOUSE SENSITIVITY", d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f), fw, fh);
		d3dg->Draw2DTextureFontText(RX(350), RY(0), "MOUSE SENSITIVITY", d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), fw, fh);
		sprintf(str, "%02d", GameConfig.GetMouseSensitivity());
		d3dg->Draw2DTextureFontText(RX(435) + 1, RY(20) + 1, str, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f), fw, fh);
		d3dg->Draw2DTextureFontText(RX(435), RY(20), str, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), fw, fh);

		// BRIGHTNESS
		d3dg->Draw2DTextureFontText(RX(350) + 1, RY(50) + 1, "BRIGHTNESS", d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f), fw, fh);
		d3dg->Draw2DTextureFontText(RX(350), RY(50), "BRIGHTNESS", d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), fw, fh);
		sprintf(str, "%02d", GameConfig.GetBrightness());
		d3dg->Draw2DTextureFontText(RX(435) + 1, RY(70) + 1, str, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f), fw, fh);
		d3dg->Draw2DTextureFontText(RX(435), RY(70), str, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), fw, fh);

		// 좌측 조작키 텍스트 (최적화)
		const char* keyNames[18] = {
			"TURN UP", "TURN DOWN", "TURN LEFT", "TURN RIGHT",
			"MOVE FORWARD", "MOVE BACKWARD", "MOVE LEFT", "MOVE RIGHT",
			"WALK", "JUMP", "RELOAD", "DROP WEAPON", "ZOOM", "FIRE MODE",
			"SWITCH WEAPON", "WEAPON 1", "WEAPON 2", "FIRE"
		};
		for (int i = 0; i < 18; i++) {
			d3dg->Draw2DTextureFontText(RX(0) + 1, RY(i * 20) + 1, keyNames[i], d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f), fw, fh);
			d3dg->Draw2DTextureFontText(RX(0), RY(i * 20), keyNames[i], d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), fw, fh);
		}

		for (int i = 0; i < 18; i++) {
			if (Options_KeyConfigID == i) sprintf(Options_p2LinkTextData[14 + i].text, "[(%d)]", (int)((float)Options_KeyConfigCnt / GAMEFPS) + 1);
			else sprintf(Options_p2LinkTextData[14 + i].text, "[%s]", GameConfig.GetOriginalkeycodeString(GameConfig.GetKeycode(i)));
		}

		// 우측 체크박스 마킹
		Options_p2LinkTextData[7].text[1] = GameConfig.GetFullscreenFlag() ? '+' : ' ';
		Options_p2LinkTextData[8].text[1] = GameConfig.GetFullscreenFlag() ? ' ' : '+';
		Options_p2LinkTextData[9].text[1] = GameConfig.GetSoundFlag() ? '*' : ' ';
		Options_p2LinkTextData[10].text[1] = GameConfig.GetBloodFlag() ? '*' : ' ';
		Options_p2LinkTextData[11].text[1] = GameConfig.GetInvertMouseFlag() ? '*' : ' ';
		Options_p2LinkTextData[12].text[1] = GameConfig.GetFrameskipFlag() ? '*' : ' ';
		Options_p2LinkTextData[13].text[1] = GameConfig.GetAnotherGunsightFlag() ? '*' : ' ';

		// 버튼 & 체크박스 텍스트 그리기 및 색상 변경
		for (int i = 0; i < OPTIONS_P2_DATAS; i++) {
			if (Options_p2LinkTextData[i].enable == true) {
				int x = RX(Options_p2LinkTextData[i].pos_x);
				int y = RY(Options_p2LinkTextData[i].pos_y);
				int w = (int)(strlen(Options_p2LinkTextData[i].text) * fw);

				if ((x < mainmenu_mouseX) && (mainmenu_mouseX < x + w) &&
					(y < mainmenu_mouseY) && (mainmenu_mouseY < y + fh)) {
					color = Options_p2LinkTextData[i].text_cursorcolor;
				}
				else {
					color = Options_p2LinkTextData[i].text_defaultcolor;
				}

				d3dg->Draw2DTextureFontText(x + 1, y + 1, Options_p2LinkTextData[i].text, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f), fw, fh);
				d3dg->Draw2DTextureFontText(x, y, Options_p2LinkTextData[i].text, color, fw, fh);
			}
		}

#undef RX
#undef RY
	}

	if( modescreen == 3 ){
		int color;
		int maxline = 21;

		// Korean 모드에서는 줄 높이 차이 때문에 약간 줄인다.
		if (GameConfig.GetLanguage() == 1) {
			maxline = 17;
		}

		//긄깏귺?됪
		d3dg->Draw2DBox(OPTIONS_P3_X, OPTIONS_P3_Y, OPTIONS_P3_X + OPTIONS_P3_W, OPTIONS_P3_Y + OPTIONS_P3_H, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.5f));

		d3dg->Draw2DTextureFontText(
			OPTIONS_P3_X + 12 + 1,
			OPTIONS_P3_Y + 18 + 1,
			"SOFTWARE LICENSE",
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f),
			18,
			24
		);
		d3dg->Draw2DTextureFontText(
			OPTIONS_P3_X + 12,
			OPTIONS_P3_Y + 18,
			"SOFTWARE LICENSE",
			d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f),
			18,
			24
		);

		d3dg->Draw2DBox(
			OPTIONS_P3_X + 0,
			OPTIONS_P3_Y + 68,
			OPTIONS_P3_X + OPTIONS_P3_W - 24,
			OPTIONS_P3_Y + OPTIONS_P3_H - 54,
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.7f)
		);

		//깋귽긜깛긚빒?됪
		int scrollline = scrollbar_license.GetScrollItem();
		strcpy(licenseinfobuf, "");
#if OPTIONS_LICENSELINE > maxline
		for(int i=0; i<maxline; i++){
#else
		for(int i=0; i<OPTIONS_LICENSELINE; i++){
#endif
			strcat(licenseinfobuf, licenseinfo[scrollline + i]);
			strcat(licenseinfobuf, "\n");
		}
		d3dg->Draw2DMSSmallFontText(
			OPTIONS_P3_X + 8,
			OPTIONS_P3_Y + 72,
			licenseinfobuf,
			d3dg->GetColorCode(0.8f, 0.8f, 0.8f, 1.0f)
		);

		//긚긏깓?깑긫??됪
		scrollbar_license.Draw(d3dg);

		for(int i=0; i<OPTIONS_P3_DATAS; i++){
			if( Options_p3LinkTextData[i].enable == true ){
				//빒럻궻륡귩먠믦
				if( ((OPTIONS_P3_X + Options_p3LinkTextData[i].pos_x) < mainmenu_mouseX)&&(mainmenu_mouseX < (OPTIONS_P3_X + Options_p3LinkTextData[i].pos_x + (signed)strlen(Options_p3LinkTextData[i].text)*Options_p3LinkTextData[i].text_w))
					&&((OPTIONS_P3_Y + Options_p3LinkTextData[i].pos_y) < mainmenu_mouseY)&&(mainmenu_mouseY < (OPTIONS_P3_Y + Options_p3LinkTextData[i].pos_y + Options_p3LinkTextData[i].text_h))
				){
					color = Options_p3LinkTextData[i].text_cursorcolor;
				}
				else{
					color = Options_p3LinkTextData[i].text_defaultcolor;
				}

				//빒럻귩?렑
				d3dg->Draw2DTextureFontText(OPTIONS_P3_X + Options_p3LinkTextData[i].pos_x +1, OPTIONS_P3_Y + Options_p3LinkTextData[i].pos_y +1, Options_p3LinkTextData[i].text,
					d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), Options_p3LinkTextData[i].text_w, Options_p3LinkTextData[i].text_h);
				d3dg->Draw2DTextureFontText(OPTIONS_P3_X + Options_p3LinkTextData[i].pos_x, OPTIONS_P3_Y + Options_p3LinkTextData[i].pos_y, Options_p3LinkTextData[i].text,
					color, Options_p3LinkTextData[i].text_w, Options_p3LinkTextData[i].text_h);
			}
			else{
				d3dg->Draw2DTextureFontText(OPTIONS_P3_X + Options_p3LinkTextData[i].pos_x, OPTIONS_P3_Y + Options_p3LinkTextData[i].pos_y, Options_p3LinkTextData[i].text,
					d3dg->GetColorCode(0.6f,0.6f,0.6f,1.0f), Options_p3LinkTextData[i].text_w, Options_p3LinkTextData[i].text_h);
			}
		}
	}

	if (modescreen == 7) {
		int color;

		d3dg->Draw2DBox(SKILLINFO_X, SKILLINFO_Y, SKILLINFO_X + SKILLINFO_W, SKILLINFO_Y + SKILLINFO_H,
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.72f));

		SkillInfo_DrawText(d3dg, SKILLINFO_X + 28, SKILLINFO_Y + 22, SkillInfo_GetTitleText(SkillInfo_Language),
			d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), 26, 32, SkillInfo_Language);

		SkillInfo_DrawText(d3dg, SKILLINFO_X + 28, SKILLINFO_Y + 64, SkillInfo_GetSelectGroupText(SkillInfo_Language),
			d3dg->GetColorCode(0.72f, 0.72f, 0.72f, 1.0f), 16, 24, SkillInfo_Language);

		for (int i = 0; i < SKILLINFO_GROUP_LINK_DATAS; i++) {
			if (SkillInfo_GroupLinkData[i].enable == true) {
				const char* linktext = SkillInfo_GroupLinkData[i].text;

				if (i < SKILLINFO_GROUP_DATAS) {
					linktext = SkillInfo_GetGroupName(i, SkillInfo_Language);
				}

				if (((SKILLINFO_X + SkillInfo_GroupLinkData[i].pos_x) < mainmenu_mouseX) &&
					(mainmenu_mouseX < (SKILLINFO_X + SkillInfo_GroupLinkData[i].pos_x + (signed)strlen(linktext) * SkillInfo_GroupLinkData[i].text_w)) &&
					((SKILLINFO_Y + SkillInfo_GroupLinkData[i].pos_y) < mainmenu_mouseY) &&
					(mainmenu_mouseY < (SKILLINFO_Y + SkillInfo_GroupLinkData[i].pos_y + SkillInfo_GroupLinkData[i].text_h))) {
					color = SkillInfo_GroupLinkData[i].text_cursorcolor;
				}
				else {
					color = SkillInfo_GroupLinkData[i].text_defaultcolor;
				}

				SkillInfo_DrawText(d3dg,
					SKILLINFO_X + SkillInfo_GroupLinkData[i].pos_x,
					SKILLINFO_Y + SkillInfo_GroupLinkData[i].pos_y,
					linktext,
					color,
					SkillInfo_GroupLinkData[i].text_w,
					SkillInfo_GroupLinkData[i].text_h,
					SkillInfo_Language);
			}
		}

		SkillInfo_RenderLanguageButtons(d3dg, SkillInfo_LangLinkData, SkillInfo_Language, mainmenu_mouseX, mainmenu_mouseY);
	}

	if (modescreen == 8) {
		int color;
		char str[256];

		const SkillInfoGroupData* info = SkillInfo_GetData(SkillInfo_SelectedGroup);

		d3dg->Draw2DBox(SKILLINFO_X, SKILLINFO_Y, SKILLINFO_X + SKILLINFO_W, SKILLINFO_Y + SKILLINFO_H,
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.72f));

		SkillInfo_DrawText(d3dg, SKILLINFO_X + 28, SKILLINFO_Y + 22, SkillInfo_GetDetailTitleText(SkillInfo_Language),
			d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), 26, 32, SkillInfo_Language);

		SkillInfo_DrawText(d3dg, SKILLINFO_X + 28, SKILLINFO_Y + 62, SkillInfo_GetGroupName(SkillInfo_SelectedGroup, SkillInfo_Language),
			d3dg->GetColorCode(0.7f, 0.7f, 1.0f, 1.0f), 20, 28, SkillInfo_Language);

		sprintf(str, "%s : %s", SkillInfo_GetTargetLabelText(SkillInfo_Language), info->targets);
		SkillInfo_DrawText(d3dg, SKILLINFO_X + 430, SKILLINFO_Y + 68, str,
			d3dg->GetColorCode(0.82f, 0.82f, 0.82f, 1.0f), 15, 22, SkillInfo_Language);

		d3dg->Draw2DBox(SKILLINFO_X + 28, SKILLINFO_Y + 104,
			SKILLINFO_X + SKILLINFO_W - 42,
			SKILLINFO_Y + 104 + SKILLINFO_DETAIL_VISIBLE_LINES * SKILLINFO_LINE_H,
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.45f));

		int scrollline = scrollbar_skillinfo.GetScrollItem();
		int line = 0;
		int drawline = 0;

		line = SkillInfo_RenderSkillBlock(d3dg, &info->skill[0],
			SkillInfo_SelectedGroup, 0, SkillInfo_Language,
			scrollline, SKILLINFO_DETAIL_VISIBLE_LINES,
			line, &drawline,
			SKILLINFO_X + 44, SKILLINFO_Y + 118,
			SKILLINFO_LINE_H, 11, 16);

		line = SkillInfo_RenderSkillBlock(d3dg, &info->skill[1],
			SkillInfo_SelectedGroup, 1, SkillInfo_Language,
			scrollline, SKILLINFO_DETAIL_VISIBLE_LINES,
			line, &drawline,
			SKILLINFO_X + 44, SKILLINFO_Y + 118,
			SKILLINFO_LINE_H, 11, 16);

		scrollbar_skillinfo.Draw(d3dg);

		SkillInfo_RenderLanguageButtons(d3dg, SkillInfo_LangLinkData, SkillInfo_Language, mainmenu_mouseX, mainmenu_mouseY);

		int i = SKILLINFO_GROUP_DATAS;
		if (((SKILLINFO_X + SkillInfo_GroupLinkData[i].pos_x) < mainmenu_mouseX) &&
			(mainmenu_mouseX < (SKILLINFO_X + SkillInfo_GroupLinkData[i].pos_x + (signed)strlen(SkillInfo_GroupLinkData[i].text) * SkillInfo_GroupLinkData[i].text_w)) &&
			((SKILLINFO_Y + SkillInfo_GroupLinkData[i].pos_y) < mainmenu_mouseY) &&
			(mainmenu_mouseY < (SKILLINFO_Y + SkillInfo_GroupLinkData[i].pos_y + SkillInfo_GroupLinkData[i].text_h))) {
			color = SkillInfo_GroupLinkData[i].text_cursorcolor;
		}
		else {
			color = SkillInfo_GroupLinkData[i].text_defaultcolor;
		}

		SkillInfo_DrawText(d3dg,
			SKILLINFO_X + SkillInfo_GroupLinkData[i].pos_x,
			SKILLINFO_Y + SkillInfo_GroupLinkData[i].pos_y,
			SkillInfo_GroupLinkData[i].text,
			color,
			SkillInfo_GroupLinkData[i].text_w,
			SkillInfo_GroupLinkData[i].text_h,
			1);
	}

	if (modescreen == 4) {
		float cfg_scale = ((float)GameConfig.GetScreenHeight() / 480.0f) * 0.7f;

		// 고해상도에서 너무 커지지 않게 제한한다.
		if (cfg_scale > 1.15f) { cfg_scale = 1.15f; }

		float ui_scale = cfg_scale;
		int f_w = (int)(14 * ui_scale);
		int f_h = (int)(18 * ui_scale);

		// Gameplay Setting은 슬라이더 열과 키 설정 열을 함께 보여주므로 900 폭을 사용한다.
// InputOptions 쪽과 반드시 같은 폭을 사용해야 표시와 클릭 판정이 맞는다.
		int scaled_menuW = (int)(900 * cfg_scale);
		int scaled_menuH = (int)(350 * cfg_scale);
		int startX = (GameConfig.GetScreenWidth() - scaled_menuW) / 2;
		int startY = (GameConfig.GetScreenHeight() - scaled_menuH) / 2 + (int)(50 * cfg_scale);

		int scaled_sliderW = (int)(160 * cfg_scale);

		d3dg->Draw2DBox(startX - (int)(30 * ui_scale), startY - (int)(30 * ui_scale),
			startX + scaled_menuW + (int)(30 * ui_scale), startY + scaled_menuH + (int)(30 * ui_scale),
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.8f));

		for (int i = 0; i < OPTIONS_ADVANCED_DATAS; i++) {
			int x = startX + (int)(Options_AdvancedLinkData[i].pos_x * ui_scale);
			int y = startY + (int)(Options_AdvancedLinkData[i].pos_y * ui_scale);

			int color = (i == Options_AdvancedID) ? Options_AdvancedLinkData[i].text_cursorcolor : Options_AdvancedLinkData[i].text_defaultcolor;
			d3dg->Draw2DTextureFontText(x + 1, y + 1, Options_AdvancedLinkData[i].text, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f), f_w, f_h);
			d3dg->Draw2DTextureFontText(x, y, Options_AdvancedLinkData[i].text, color, f_w, f_h);

			if (i == 19) {
				char fpsText[32];
				int fps = GameConfig.GetRenderFpsLimit();

				if (fps == 0) {
					strcpy(fpsText, "[ UNLIMITED ]");
				}
				else {
					sprintf(fpsText, "[ %d FPS ]", fps);
				}

				int fpsTextX = startX + (int)(260 * ui_scale);

				d3dg->Draw2DTextureFontText(
					fpsTextX,
					y,
					fpsText,
					d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f),
					(int)(14 * ui_scale),
					(int)(18 * ui_scale)
				);

				continue;
			}

			if (i == 20) {
				const char* skillModeText = GameConfig.GetSkillModeFlag() ? "[ ON ]" : "[ OFF ]";
				int skillModeTextX = x + (int)(190 * ui_scale);

				d3dg->Draw2DTextureFontText(
					skillModeTextX,
					y,
					skillModeText,
					d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f),
					(int)(14 * ui_scale),
					(int)(18 * ui_scale)
				);

				continue;
			}

			if (i >= 16) continue;

			int valueX = startX + (int)(435 * ui_scale);

			// 키 설정 항목: RUN / CROUCH / NIGHTVISION / MAP / SKILL / SKILL2
			if (i >= 10 && i <= 15) {
				int targetKeyID;
				if (i == 10) targetKeyID = KEY_RUN;
				else if (i == 11) targetKeyID = KEY_CROUCH;
				else if (i == 12) targetKeyID = KEY_NIGHTVISION;
				else if (i == 13) targetKeyID = KEY_MAP;
				else if (i == 14) targetKeyID = KEY_SKILL;
				else targetKeyID = KEY_SKILL2;

				char keyStr[64];
				if (Options_KeyConfigID == targetKeyID) {
					sprintf(keyStr, "[ PRESS KEY %d ]", (int)((float)Options_KeyConfigCnt / GAMEFPS) + 1);
				}
				else {
					sprintf(keyStr, "[ %s ]", GameConfig.GetOriginalkeycodeString(GameConfig.GetKeycode(targetKeyID)));
				}

				// 오른쪽 열의 키 문자열 위치
				valueX = x + (int)(190 * ui_scale);

				d3dg->Draw2DTextureFontText(
					valueX,
					y,
					keyStr,
					d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f),
					(int)(14 * ui_scale),
					(int)(18 * ui_scale)
				);
				continue;
			}

			int sliderX = startX + (int)(260 * ui_scale);

			float ratio = 0.0f;
			char valStr[32];

			switch (i) {
			case 0:
				ratio = GameConfig.GetMasterVolume();
				sprintf(valStr, "%.1f", GameConfig.GetMasterVolume());
				break;

			case 1:
				// PLAYER DAMAGE: 0.0 ~ 3.0
				ratio = GameConfig.GetPlayerDamageMultiplier() / 3.0f;
				sprintf(valStr, "%.1f", GameConfig.GetPlayerDamageMultiplier());
				break;

			case 2:
				// HP REGEN 게이지 바 길이와 텍스트 값 표시
				ratio = GameConfig.GetHealthRegenAmount() / 100.0f; // 최대값 50 기준 비율
				sprintf(valStr, "%d", GameConfig.GetHealthRegenAmount());
				break;

			case 3:
				// AI REACTION: 0.0 ~ 2.0
				ratio = GameConfig.GetAIReactionSpeedMultiplier() / 2.0f;
				sprintf(valStr, "%.1f", GameConfig.GetAIReactionSpeedMultiplier());
				break;

			case 4:
				// AI FIRERATE: 0.0 ~ 4.0
				ratio = GameConfig.GetAIFireRateMultiplier() / 4.0f;
				sprintf(valStr, "%.1f", GameConfig.GetAIFireRateMultiplier());
				break;

			case 5:
				// AI ACCURACY: 0.0 ~ 3.0
				ratio = GameConfig.GetAIAccuracyMultiplier() / 3.0f;
				sprintf(valStr, "%.1f", GameConfig.GetAIAccuracyMultiplier());
				break;

			case 6:
				// AI VIEWRANGE: 0.0 ~ 2.5
				ratio = GameConfig.GetAISearchRangeMultiplier() / 2.5f;
				sprintf(valStr, "%.1f", GameConfig.GetAISearchRangeMultiplier());
				break;

			case 7:
				ratio = GameConfig.GetRecoilHipMultiplier() / 5.0f;
				sprintf(valStr, "%.1f", GameConfig.GetRecoilHipMultiplier());
				break;

			case 8:
				ratio = GameConfig.GetRecoilAimMultiplier() / 5.0f;
				sprintf(valStr, "%.1f", GameConfig.GetRecoilAimMultiplier());
				break;
			case 9:
				ratio = GameConfig.GetRecoilSideMultiplier() / 1.0f;
				sprintf(valStr, "%.1f", GameConfig.GetRecoilSideMultiplier());
				break;
			}
			if (ratio < 0.0f) ratio = 0.0f; if (ratio > 1.0f) ratio = 1.0f;

			int sliderBgColor = (i == Options_AdvancedID) ? d3dg->GetColorCode(0.5f, 0.5f, 0.5f, 1.0f) : d3dg->GetColorCode(0.3f, 0.3f, 0.3f, 1.0f);
			d3dg->Draw2DBox(sliderX, y + (int)(6 * ui_scale), sliderX + scaled_sliderW, y + (int)(14 * ui_scale), sliderBgColor);
			int filledW = (int)(scaled_sliderW * ratio);
			d3dg->Draw2DBox(sliderX, y + (int)(6 * ui_scale), sliderX + filledW, y + (int)(14 * ui_scale), d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f));
			d3dg->Draw2DBox(sliderX + filledW - (int)(3 * ui_scale), y + (int)(2 * ui_scale), sliderX + filledW + (int)(3 * ui_scale), y + (int)(18 * ui_scale), d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f));
			d3dg->Draw2DTextureFontText(sliderX + scaled_sliderW + (int)(15 * ui_scale), y, valStr, d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f), (int)(14 * ui_scale), (int)(18 * ui_scale));
		}
	}

	if (modescreen == 6) {
		float cfg_scale = ((float)GameConfig.GetScreenHeight() / 480.0f) * 0.7f;

		// Gameplay Setting과 같은 크기 기준을 사용한다.
		if (cfg_scale > 1.15f) { cfg_scale = 1.15f; }

		float ui_scale = cfg_scale;
		int f_w = (int)(14 * ui_scale);
		int f_h = (int)(18 * ui_scale);
		int scaled_menuW = (int)(560 * cfg_scale);
		int scaled_menuH = (int)(350 * cfg_scale);
		int startX = (GameConfig.GetScreenWidth() - scaled_menuW) / 2;
		int startY = (GameConfig.GetScreenHeight() - scaled_menuH) / 2 + (int)(50 * cfg_scale);

		d3dg->Draw2DBox(
			startX - (int)(20 * ui_scale),
			startY - (int)(30 * ui_scale),
			startX + scaled_menuW + (int)(20 * ui_scale),
			startY + scaled_menuH + (int)(30 * ui_scale),
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.8f)
		);

		for (int i = 0; i < OPTIONS_TOGGLE_DATAS; i++) {
			int x = startX + (int)(Options_ToggleLinkData[i].pos_x * ui_scale);
			int y = startY + (int)(Options_ToggleLinkData[i].pos_y * ui_scale);

			int color = (i == Options_ToggleID) ? Options_ToggleLinkData[i].text_cursorcolor : Options_ToggleLinkData[i].text_defaultcolor;

			d3dg->Draw2DTextureFontText(
				x + 1,
				y + 1,
				Options_ToggleLinkData[i].text,
				d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f),
				f_w,
				f_h
			);
			d3dg->Draw2DTextureFontText(
				x,
				y,
				Options_ToggleLinkData[i].text,
				color,
				f_w,
				f_h
			);

			if (i <= 6) {
				const char* modeText = "[ OFF ]";

				if (i == 0) {
					modeText = GameConfig.GetRadarEnabledFlag() ? "[ ON ]" : "[ OFF ]";
				}
				else if (i == 1) {
					modeText = GameConfig.GetRadarInfoHudFlag() ? "[ ON ]" : "[ OFF ]";
				}
				else if (i == 2) {
					modeText = GameConfig.GetRadarSeeThroughWallsFlag() ? "[ ON ]" : "[ OFF ]";
				}
				else if (i == 3) {
					modeText = GameConfig.GetGrenadeTrajectoryToggleFlag() ? "[ TOGGLE ]" : "[ HOLD ]";
				}
				else if (i == 4) {
					modeText = GameConfig.GetScopeAimToggleFlag() ? "[ TOGGLE ]" : "[ HOLD ]";
				}
				else if (i == 5) {
					modeText = GameConfig.GetRunToggleFlag() ? "[ TOGGLE ]" : "[ HOLD ]";
				}
				else if (i == 6) {
					modeText = GameConfig.GetCrouchToggleFlag() ? "[ TOGGLE ]" : "[ HOLD ]";
				}

				// 모든 설정값의 괄호 위치를 같은 X 좌표에 맞춘다.
				int modeTextX = startX + (int)(360 * ui_scale);

				d3dg->Draw2DTextureFontText(
					modeTextX,
					y,
					modeText,
					d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f),
					f_w,
					f_h
				);
			}
		}
	}

	if (modescreen == 5) {
		float ui_scale = (float)GameConfig.GetScreenHeight() / 1080.0f;
		int f_w = (int)(16 * ui_scale);
		int f_h = (int)(20 * ui_scale);

		// 화면이 너무 길어지지 않도록 줄 간격을 줄인다.
		int rowStep = (int)(32 * ui_scale);
		int colStep = (int)(380 * ui_scale);

		int scaled_menuW = (int)(750 * ui_scale);

		// STYLE 항목과 COLOR 제목 사이에 공백 1줄을 추가하므로 높이를 약간 늘린다.
// InputOptions 쪽과 반드시 같은 값을 사용해야 클릭 판정과 렌더링 위치가 맞는다.
		int scaled_menuH = (int)(18 * 32 * ui_scale);

		int startX = (GameConfig.GetScreenWidth() - scaled_menuW) / 2;
		int startY = (GameConfig.GetScreenHeight() - scaled_menuH) / 2 + (int)(50 * ui_scale);

		d3dg->Draw2DBox(startX - (int)(20 * ui_scale), startY - (int)(20 * ui_scale),
			startX + scaled_menuW + (int)(20 * ui_scale), startY + scaled_menuH,
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.8f));

		const char* cross_texts[OPTIONS_CROSSHAIR_DATAS] = {
			"CROSS", "CIRCLE", "DOT", "CROSS + CIRCLE",
			"CHEVRON", "X SHAPE", "SQUARE", "CROSS + DOT",
			"TRIANGLE", "T SHAPE",
			"GREEN", "RED", "YELLOW", "WHITE",
			"BLUE", "CYAN", "MAGENTA", "BLACK",
			"ORANGE", "PINK",
			"OUTLINE", "[ RESET DEFAULTS ]", "< BACK >"
		};

		// 구역 제목 표시
		d3dg->Draw2DTextureFontText(
			startX + 1,
			startY + 1,
			"[STYLE]",
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f),
			f_w,
			f_h
		);
		d3dg->Draw2DTextureFontText(
			startX,
			startY,
			"[STYLE]",
			d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f),
			f_w,
			f_h
		);

		// STYLE 항목 5줄 뒤에 공백 1줄을 두고 COLOR 제목을 표시한다.
		d3dg->Draw2DTextureFontText(
			startX + 1,
			startY + 7 * rowStep + 1,
			"[COLOR]",
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f),
			f_w,
			f_h
		);
		d3dg->Draw2DTextureFontText(
			startX,
			startY + 7 * rowStep,
			"[COLOR]",
			d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f),
			f_w,
			f_h
		);

		int hoverID = -1;
		for (int i = 0; i < OPTIONS_CROSSHAIR_DATAS; i++) {
			int row = 0, col = 0;

			if (i < 10) {
				// [STYLE] 제목 아래 1~5행
				row = 1 + (i / 2);
				col = i % 2;
			}
			else if (i < 20) {
				// STYLE 항목 아래 공백 1줄 후 [COLOR] 제목,
				// COLOR 항목은 8~12행에 배치한다.
				row = 8 + ((i - 10) / 2);
				col = (i - 10) % 2;
			}
			else if (i == 20) {
				// COLOR 항목 아래 공백 1줄 후 OUTLINE
				row = 14;
				col = 0;
			}
			else if (i == 21) {
				// OUTLINE 아래 공백 1줄 후 RESET DEFAULTS
				row = 16;
				col = 0;
			}
			else {
				// RESET DEFAULTS와 같은 줄에 BACK
				row = 16;
				col = 1;
			}

			int x = startX + (col * colStep);
			int y = startY + (row * rowStep);

			int hit_w = (int)(300 * ui_scale);

			// 클릭 판정이 표시 위치보다 살짝 빡빡하게 느껴질 수 있으므로
			// 렌더링과 입력 양쪽 모두 30으로 통일한다.
			int hit_h = (int)(30 * ui_scale);

			if (i == 20) {
				// OUTLINE [ ON/OFF ] 값까지 클릭 가능하게 넓게 잡는다.
				hit_w = (int)(430 * ui_scale);
			}
			else if (i == 21) {
				hit_w = (int)(310 * ui_scale);
			}
			else if (i == 22) {
				hit_w = (int)(220 * ui_scale);
			}

			if ((x <= mainmenu_mouseX) && (mainmenu_mouseX <= x + hit_w) &&
				(y <= mainmenu_mouseY) && (mainmenu_mouseY <= y + hit_h)) {
				hoverID = i;
			}

			bool selected = false;

			if (i < 10) {
				selected = (GameConfig.GetCrosshairStyle() == i);
			}
			else if (i < 20) {
				selected = (GameConfig.GetCrosshairColor() == i - 10);
			}
			else if (i == 20) {
				selected = GameConfig.GetCrosshairOutline();
			}

			int color;
			if (i == hoverID) {
				color = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f);
			}
			else if (selected) {
				color = d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f);
			}
			else {
				color = d3dg->GetColorCode(0.8f, 0.8f, 0.8f, 1.0f);
			}

			if (i == 20) {
				const char* outlineText = GameConfig.GetCrosshairOutline() ? "[ ON ]" : "[ OFF ]";

				d3dg->Draw2DTextureFontText(
					x + 1,
					y + 1,
					"OUTLINE",
					d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f),
					f_w,
					f_h
				);
				d3dg->Draw2DTextureFontText(
					x,
					y,
					"OUTLINE",
					color,
					f_w,
					f_h
				);

				d3dg->Draw2DTextureFontText(
					x + (int)(260 * ui_scale),
					y,
					outlineText,
					d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f),
					f_w,
					f_h
				);

				continue;
			}

			d3dg->Draw2DTextureFontText(
				x + 1,
				y + 1,
				cross_texts[i],
				d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f),
				f_w,
				f_h
			);
			d3dg->Draw2DTextureFontText(
				x,
				y,
				cross_texts[i],
				color,
				f_w,
				f_h
			);
		}
	}
}
#endif	//ENABLE_MENUOPTIONS

void mainmenu::Destroy()
{
	//긳깓긞긏긢??됶뺳
	d3dg->CleanupMapdata();

	//긆긳긙긃긏긣?긨?긙긿?됶뺳
	ObjMgr.Cleanup();

	//봶똧뗴됶뺳
	Resource.CleanupSkyModelTexture();

	d3dg->CleanupTexture(gametitle);

	GameState->NextState();
}

//! @brief 긓깛긚긣깋긏?
briefing::briefing()
{
	TwoTexture = false;
	TextureA = -1;
	TextureB = -1;
}

//! @brief 긢귻긚긣깋긏?
briefing::~briefing()
{}

int briefing::Create()
{
	char path[MAX_PATH];
	char pdata[MAX_PATH];
	char PictureA[MAX_PATH];
	char PictureB[MAX_PATH];

	//봶똧됪몴귩롦벦
	gametitle = d3dg->LoadTexture("data\\title.dds", false, false);

	//mif긲?귽깑궻긲?귽깑긬긚롦벦
	if( GameInfoData.selectmission_id >= 0 ){
		GameParamInfo.GetOfficialMission(GameInfoData.selectmission_id, NULL, NULL, path, pdata, NULL, NULL);
		strcat(path, pdata);
		strcat(path, ".txt");
	}
	else{
		strcpy(path, GameInfoData.mifpath);
	}

	//mif긲?귽깑귩벶귒뜛귒
	if( MIFdata.LoadFiledata(path, true) != 0 ){
		//briefing data open failed
		return 1;
	}

	//긳깏?긲귻깛긐됪몴궻긲?귽깑긬긚롦벦
	MIFdata.GetPicturefilePath(PictureA, PictureB);

	//긳깏?긲귻깛긐됪몴벶귒뜛귒
	if( strcmp(PictureB, "!") == 0 ){
		TwoTexture = false;
		TextureA = d3dg->LoadTexture(PictureA, true, false);
		TextureB = -1;
	}
	else{
		TwoTexture = true;
		TextureA = d3dg->LoadTexture(PictureA, true, false);
		TextureB = d3dg->LoadTexture(PictureB, true, false);
	}

	//?긂긚긇??깑귩뭷돍귉댷벍
	inputCtrl->MoveMouseCenter();
	framecnt = 0;

	GameState->NextState();
	return 0;
}

int briefing::Recovery()
{
	char PictureA[MAX_PATH];
	char PictureB[MAX_PATH];

	//봶똧됪몴귩롦벦
	gametitle = d3dg->LoadTexture("data\\title.dds", false, false);

	//긳깏?긲귻깛긐됪몴궻긲?귽깑긬긚롦벦
	MIFdata.GetPicturefilePath(PictureA, PictureB);

	//긳깏?긲귻깛긐됪몴벶귒뜛귒
	if( strcmp(PictureB, "!") == 0 ){
		TwoTexture = false;
		TextureA = d3dg->LoadTexture(PictureA, true, false);
		TextureB = -1;
	}
	else{
		TwoTexture = true;
		TextureA = d3dg->LoadTexture(PictureA, true, false);
		TextureB = d3dg->LoadTexture(PictureB, true, false);
	}

	return 0;
}

void briefing::Render2D()
{
	float effectA = GetEffectAlphaLoop(framecnt, 0.8f, 0.7f, true);
	float effectB = GetEffectAlphaLoop(framecnt, 0.8f, 1.0f, true);
	int effectB_sizeW = (int)( (float)(framecnt%((int)(GAMEFPS*1.0f))) * 0.2f + 18 );
	int effectB_sizeH = (int)( (float)(framecnt%((int)(GAMEFPS*1.0f))) * 1.0f + 26 );

	//긽긾갌봶똧됪몴궻?됪궼갂렔벍밒궸뛱귦귢귡갃

	//뚂믦빒럻?렑
	d3dg->Draw2DTextureFontTextCenterScaling(0, 30, "BRIEFING", d3dg->GetColorCode(1.0f,1.0f,0.0f,effectA), 60, 42);
	d3dg->Draw2DTextureFontText(GameConfig.GetScreenWidth() - 210 - effectB_sizeW*20/2, GameConfig.GetScreenHeight() - 37 - effectB_sizeH/2,
								"LEFT CLICK TO BEGIN", d3dg->GetColorCode(1.0f,1.0f,1.0f,effectB), effectB_sizeW, effectB_sizeH);
	d3dg->Draw2DTextureFontText(GameConfig.GetScreenWidth() - 210 - 18*20/2, GameConfig.GetScreenHeight() - 37 - 26/2, "LEFT CLICK TO BEGIN", d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 18, 26);

	//긳깏?긲귻깛긐됪몴?됪
	if( TwoTexture == false ){
		if( TextureA == -1 ){ d3dg->Draw2DBoxScaling(40, 180, 40+160, 180+150, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f)); }
		else{ d3dg->Draw2DTextureScaling(40, 180, TextureA, 160, 150, 1.0f); }
	}
	else{
		if( TextureA == -1 ){ d3dg->Draw2DBoxScaling(40, 130, 40+160, 130+150, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f)); }
		else{ d3dg->Draw2DTextureScaling(40, 130, TextureA, 160, 150, 1.0f); }

		if( TextureB == -1 ){ d3dg->Draw2DBoxScaling(40, 300, 40+160, 300+150, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f)); }
		else{ d3dg->Draw2DTextureScaling(40, 300, TextureB, 160, 150, 1.0f); }
	}

	//?긞긘깈깛뼹귩롦벦갋?렑
	char mname[64];
	if( MIFdata.GetFiletype() == false ){
		GameParamInfo.GetOfficialMission(GameInfoData.selectmission_id, NULL, mname, NULL, NULL, NULL, NULL);
	}
	else{
		strcpy(mname, MIFdata.GetMissionFullname());
	}
	d3dg->Draw2DTextureFontTextCenterScaling(0, 90, mname, d3dg->GetColorCode(1.0f,0.5f,0.0f,1.0f), 18, 25);

	//?긞긘깈깛먣뼻귩?렑
// 위치는 기존 Draw2DMSFontTextScaling과 같게 유지하고,
// 글씨 크기만 작은 시스템 폰트로 줄인다.
	d3dg->Draw2DMSFontTextBriefingScaling(
		230,
		180,
		MIFdata.GetBriefingText(),
		d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f)
	);
}

void briefing::Destroy()
{
	//긳깏?긲귻깛긐됪몴귩됶뺳
	d3dg->CleanupTexture(TextureA);
	d3dg->CleanupTexture(TextureB);

	//봶똧됪몴귩됶뺳
	d3dg->CleanupTexture(gametitle);

	GameState->NextState();
}

//! @brief 긓깛긚긣깋긏?
maingame::maingame()
{
	SkyNumber = 0;
	DarkScreenFlag = false;
	FogFlag = true;
	mouse_rx = 0.0f;
	mouse_ry = 0.0f;
	view_rx = 0.0f;
	view_ry = 0.0f;
	add_camera_rx = 0.0f;
	add_camera_ry = 0.0f;
	ShowInfo_Debugmode = false;
	Camera_Debugmode = false;
	tag = false;
	radar = true;
	ShowFullMap = false;
	FullMapWorldR = 500.0f;
	wireframe = false;
	nomodel = false;
	CenterLine = false;
	Camera_Blind = true;
	Camera_F1mode = false;
	Camera_F2mode = 0;
	Camera_HOMEmode = GameConfig.GetShowArmFlag();
	Cmd_F5 = false;
	InvincibleID = -1;
	PlayerAI = false;
	AIstop = false;
	AINoFight = false;
	AIdebuginfoID = -1;
	start_framecnt = 0;
	end_framecnt = 0;
	EventStop = false;
	for(int i=0; i<TOTAL_EVENTLINE; i++){ EventSkip[i] = false; }
	GameSpeed = 1;
	message_id = -1;
	message_cnt = 0;
	redflash_flag = false;
	shieldflash_flag = false;

	damage_arc_timer = 0;
	damage_arc_rx = 0.0f;
	damage_arc_shield_flag = false;
	grenade_trajectory_flag = false;
	grenade_trajectory_weapon_id = ID_WEAPON_NONE;
	player_run_toggle_state = false;
	player_crouch_toggle_state = false;
	player_move_crouch_priority = false;

	ResetPlayerSkillState();

	time = 0;
	time_input = 0;
	time_process_object = 0;
	time_process_ai = 0;
	time_process_event = 0;
	time_sound = 0;
	time_render = 0;
	memcpy(&MainGameInfo, &GameInfoData, sizeof(GameInfo));

#ifdef ENABLE_DEBUGCONSOLE
	Show_Console = false;
	InfoConsoleData = NULL;
	NewCommand[0] = '\0';
	ScreenShot = 0;
#endif
}

//! @brief 긢귻긚긣깋긏?
maingame::~maingame()
{}

//! @brief 띆뷭궶궵귩?렑궥귡긢긫긞긏긾?긤궻긲깋긐먠믦
void maingame::SetShowInfoFlag(bool flag)
{
	ShowInfo_Debugmode = flag;
}

void maingame::SyncDrawCamera()
{
	// 현재 카메라 값을 2D/HUD/1인칭 무기 표시용 draw_camera에 맞춘다.
	draw_camera_x = camera_x;
	draw_camera_y = camera_y;
	draw_camera_z = camera_z;
	draw_camera_rx = camera_rx;
	draw_camera_ry = camera_ry;

}

void maingame::ProcessVisualTimers()
{
	human* player = ObjMgr.GetPlayerHumanObject();

	if (player != NULL) {
		if (player->GetHitEffectTimer() > 0) {
			player->DecreaseHitEffectTimer();
		}

		if (player->GetHitMarkerTimer() > 0) {
			player->DecreaseHitMarkerTimer();

			if (player->GetHitMarkerTimer() <= 0) {
				player->SetHeadshotFlag(false);
			}
		}
	}

	if (damage_arc_timer > 0) {
		damage_arc_timer--;

		if (damage_arc_timer <= 0) {
			damage_arc_shield_flag = false;
		}
	}
}

//! @brief 긒??궻렳뛱뫊뱗귩롦벦
int maingame::GetGameSpeed()
{
	return GameSpeed;
}

int maingame::Create()
{
	memcpy(&MainGameInfo, &GameInfoData, sizeof(GameInfo));

	damage_arc_timer = 0;
	damage_arc_rx = 0.0f;
	damage_arc_shield_flag = false;
	grenade_trajectory_flag = false;
	grenade_trajectory_weapon_id = ID_WEAPON_NONE;
	player_run_toggle_state = false;
	player_crouch_toggle_state = false;
	player_move_crouch_priority = false;

	ResetPlayerSkillState();

	char path[MAX_PATH];
	char bdata[MAX_PATH];
	char pdata[MAX_PATH];
	char pdata2[MAX_PATH];
	int blockflag, pointflag;
	bool collisionflag;

	//.bd1궴.pd1궻긲?귽깑긬긚귩땫귕귡
	if( MIFdata.GetFiletype() == false ){
		GameParamInfo.GetOfficialMission(MainGameInfo.selectmission_id, NULL, NULL, path, pdata2, &collisionflag, &DarkScreenFlag);

		strcpy(bdata, path);
		strcat(bdata, OFFICIALMISSION_BD1);
		strcpy(pdata, path);
		strcat(pdata, pdata2);
		strcat(pdata, ".pd1");
	}
	else{
		MIFdata.GetDatafilePath(bdata, pdata);
		collisionflag = MIFdata.GetCollisionFlag();
		DarkScreenFlag = MIFdata.GetScreenFlag();

		GetFileDirectory(bdata, path);
	}

	//믁돿룷븿귩벶귒뜛귔
	for(int i=0; i<MAX_ADDSMALLOBJECT; i++){
		Resource.CleanupAddSmallObject(i); // ★ [추가] 이전 미션의 소품 메모리 강제 해제
		Resource.LoadAddSmallObject(i, MIFdata.GetAddSmallobjectModelPath(i), MIFdata.GetAddSmallobjectTexturePath(i), MIFdata.GetAddSmallobjectSoundPath(i));
	}

	//긳깓긞긏긢??벶귒뜛귒
	blockflag = BlockData.LoadFiledata(bdata);

	//?귽깛긣긢??벶귒뜛귒
	pointflag = PointData.LoadFiledata(pdata);

	//긲?귽깑벶귒뜛귒궳긄깋?궕궇궯궫귞뭷뭚
	if( (blockflag != 0)||(pointflag != 0) ){
		//2bit : point data open failed
		//1bit : block data open failed
		return pointflag*2 + blockflag;
	}

	//긳깓긞긏긢??룊딖돸
	BlockData.CalculationBlockdata(DarkScreenFlag);
	d3dg->LoadMapdata(&BlockData, path);
	CollD.InitCollision(&BlockData);

	//?귽깛긣긢??룊딖돸
	ObjMgr.LoadPointData();

	// ==========================================================
	// ★ [추가할 코드] 맵 로딩 직후, 무기 랜덤 변경 함수 호출 ★
	// ==========================================================
	// GameParamInfo: 전역 변수로 선언된 파라미터 정보
	// Resource: 전역 변수로 선언된 리소스 관리자
	ObjMgr.RandomizeMapWeaponsByCustomGroup(&GameParamInfo, &Resource);

	//믁돿궻뱰궫귟뵽믦먠믦
	ObjMgr.SetAddCollisionFlag(collisionflag);

	//AI設定
	for (int i = 0; i < MAX_HUMAN; i++) {
		HumanAI[i].SetClass(&ObjMgr, i, &BlockData, &PointData, &GameParamInfo, &CollD, GameSound);
		HumanAI[i].Init();
		HumanAI[i].SetForceNoAI(ObjMgr.GetHumanForceNoAI(i));
	}


	//봶똧뗴벶귒뜛귒
	SkyNumber = MIFdata.GetSkynumber();
	Resource.CleanupSkyModelTexture(); // ★ [추가] 이전 미션의 하늘 메모리 강제 해제
	Resource.LoadSkyModelTexture(SkyNumber);

	//긖긂깛긤룊딖돸
	GameSound->InitWorldSound();

	//귽긹깛긣룊딖돸
	for(int i=0; i<TOTAL_EVENTLINE; i++){
		Event[i].SetClass(&PointData, &ObjMgr);
	}
	Event[0].Reset(TOTAL_EVENTENTRYPOINT_0);
	Event[1].Reset(TOTAL_EVENTENTRYPOINT_1);
	Event[2].Reset(TOTAL_EVENTENTRYPOINT_2);

	//긵깒귽깂?궻뛀궖귩롦벦
	ObjMgr.GetPlayerHumanObject()->GetRxRy(&mouse_rx, &mouse_ry);

	FogFlag = true;
	view_rx = 0.0f;
	view_ry = 0.0f;
	add_camera_rx = 0.0f;
	add_camera_ry = 0.0f;

	draw_camera_x = 0.0f;
	draw_camera_y = 0.0f;
	draw_camera_z = 0.0f;
	draw_camera_rx = 0.0f;
	draw_camera_ry = 0.0f;

	ShowInfo_Debugmode = false;
	tag = false;
	radar = true;
	ShowFullMap = false;

	// 미션이 새로 시작될 때 전체 지도 확대 상태를 현재 기본값으로 복구
	FullMapWorldR = 500.0f;

	wireframe = false;
	nomodel = false;
	CenterLine = false;
	Camera_Blind = true;
	Camera_F1mode = false;
	InvincibleID = -1;
	PlayerAI = false;
	AIstop = false;
	AINoFight = false;
	AIdebuginfoID = -1;
	framecnt = 0;
	start_framecnt = 0;
	end_framecnt = 0;
	EventStop = false;
	for(int i=0; i<TOTAL_EVENTLINE; i++){ EventSkip[i] = false; }
	GameSpeed = 1;
	message_id = -1;
	message_cnt = 0;
	redflash_flag = false;
	shieldflash_flag = false;
	MainGameInfo.missioncomplete = false;
	MainGameInfo.fire = 0;
	MainGameInfo.ontarget = 0;
	MainGameInfo.kill = 0;
	MainGameInfo.headshot = 0;

#ifdef ENABLE_DEBUGCONSOLE
	Show_Console = false;
	ScreenShot = 0;

	//긓깛??깑뾭룊딖돸
	InfoConsoleData = new ConsoleData [MAX_CONSOLELINES];
	for(int i=0; i<MAX_CONSOLELINES; i++){
		InfoConsoleData[i].color = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
		InfoConsoleData[i].textdata[0] = '\0';
	}
	AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Game Debug Console.");
	AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "        Command list >help");
	ConsoleTextBoxControl.InitTextBox("", MAX_CONSOLELEN - (int)strlen(CONSOLE_PROMPT), 0x01|0x04|0x20);
#endif

	//?긂긚긇??깑귩뭷돍귉댷벍
	inputCtrl->MoveMouseCenter();

	// 미션 시작 직후 이전 카메라 값이 튀지 않도록 초기화
	SyncDrawCamera();

	GameState->NextState();
	return 0;
}

int maingame::Recovery()
{
	char path[MAX_PATH];
	char bdata[MAX_PATH];	//???
	char pdata[MAX_PATH];	//???

	//.bd1궴.pd1궻긲?귽깑긬긚귩땫귕귡
	if( MIFdata.GetFiletype() == false ){
		GameParamInfo.GetOfficialMission(MainGameInfo.selectmission_id, NULL, NULL, path, NULL, NULL, NULL);
	}
	else{
		MIFdata.GetDatafilePath(bdata, pdata);
		GetFileDirectory(bdata, path);
	}

	//믁돿룷븿귩됶뺳곆벶귒뜛귔
	for(int i=0; i<MAX_ADDSMALLOBJECT; i++){
		Resource.CleanupAddSmallObject(i);
		Resource.LoadAddSmallObject(i, MIFdata.GetAddSmallobjectModelPath(i), MIFdata.GetAddSmallobjectTexturePath(i), MIFdata.GetAddSmallobjectSoundPath(i));
	}

	//긳깓긞긏긢??룊딖돸
	d3dg->LoadMapdata(&BlockData, path);

	//?귽깛긣긢??룊딖돸
	ObjMgr.Recovery();

	ResetPlayerSkillState();

		// ★ [추가] 재시작 시에도 무기 랜덤 변경 실행 ★
	ObjMgr.RandomizeMapWeaponsByCustomGroup(&GameParamInfo, &Resource);

	//봶똧뗴룊딖돸
	Resource.CleanupSkyModelTexture();
	Resource.LoadSkyModelTexture(SkyNumber);

	return 0;
}

//! @brief 벫믦?띿궻볺쀍귩?긃긞긏
bool maingame::CheckInputControl(int CheckKey, int mode)
{
	int KeyCode = OriginalkeycodeToDinputdef(GameConfig.GetKeycode(CheckKey));
	if( KeyCode == -1 ){
		if( mode == 0 ){
			return inputCtrl->CheckMouseButtonNowL();
		}
		if( mode == 1 ){
			return inputCtrl->CheckMouseButtonDownL();
		}
		//if( mode == 2 ){
			return inputCtrl->CheckMouseButtonUpL();
		//}
	}
	if( KeyCode == -2 ){
		if( mode == 0 ){
			return inputCtrl->CheckMouseButtonNowR();
		}
		if( mode == 1 ){
			return inputCtrl->CheckMouseButtonDownR();
		}
		//if( mode == 2 ){
			return inputCtrl->CheckMouseButtonUpR();
		//}
	}
	if( KeyCode == -3 ){
		int CodeL, CodeR;
		GetDoubleKeyCode(0, &CodeL, &CodeR);
		if( mode == 0 ){
			if( inputCtrl->CheckKeyNow(CodeL) ){
				return true;
			}
			return inputCtrl->CheckKeyNow(CodeR);
		}
		if( mode == 1 ){
			if( inputCtrl->CheckKeyDown(CodeL) ){
				return true;
			}
			return inputCtrl->CheckKeyDown(CodeR);
		}
		//if( mode == 2 ){
			if( inputCtrl->CheckKeyUp(CodeL) ){
				return true;
			}
			return inputCtrl->CheckKeyUp(CodeR);
		//}
	}
	if( KeyCode == -4 ){
		int CodeL, CodeR;
		GetDoubleKeyCode(1, &CodeL, &CodeR);
		if( mode == 0 ){
			if( inputCtrl->CheckKeyNow(CodeL) ){
				return true;
			}
			return inputCtrl->CheckKeyNow(CodeR);
		}
		if( mode == 1 ){
			if( inputCtrl->CheckKeyDown(CodeL) ){
				return true;
			}
			return inputCtrl->CheckKeyDown(CodeR);
		}
		//if( mode == 2 ){
			if( inputCtrl->CheckKeyUp(CodeL) ){
				return true;
			}
			return inputCtrl->CheckKeyUp(CodeR);
		//}
	}

	if( mode == 0 ){
		return inputCtrl->CheckKeyNow( OriginalkeycodeToDinputdef(GameConfig.GetKeycode(CheckKey)) );
	}
	if( mode == 1 ){
		return inputCtrl->CheckKeyDown( OriginalkeycodeToDinputdef(GameConfig.GetKeycode(CheckKey)) );
	}
	//if( mode == 2 ){
		return inputCtrl->CheckKeyUp( OriginalkeycodeToDinputdef(GameConfig.GetKeycode(CheckKey)) );
	//}
}

void maingame::Input()
{
	time = GetTimeMS();

	//긵깒귽깂?궻긏깋긚귩롦벦
	human *myHuman = ObjMgr.GetPlayerHumanObject();

	//긌?볺쀍귩롦벦
	inputCtrl->GetInputState(true);
	inputCtrl->MoveMouseCenter();

	//?긂긚궻댷벍쀊롦벦
	int x, y;
	float MouseSensitivity;
	inputCtrl->GetMouseMovement(&x, &y);

	//럨?궻댷벍쀊똶럁
	ScopeParameter sparam;
	GameParamInfo.GetScopeParam(myHuman->GetScopeMode(), &sparam);
	MouseSensitivity = DegreeToRadian(1) * sparam.MouseRange * GameConfig.GetMouseSensitivity();

	//?긂긚뵿?걁긆긵긘깈깛먠믦걂궕뾎뚼궶귞궽갂뵿?궥귡갃
	if( GameConfig.GetInvertMouseFlag() == true ){
		y *= -1;
	}

	if (inputCtrl->CheckKeyDown(GetEscKeycode())) {
		myHuman->NightVision = false;
		myHuman->CustomZoom = 1.0f;
		GameSpeed = 1;
		GameState->PushBackSpaceKey();
		return;
	}
	else if (inputCtrl->CheckKeyDown(GetFunctionKeycode(12))) {
		myHuman->NightVision = false;
		myHuman->CustomZoom = 1.0f;
		GameSpeed = 1;
		GameState->PushF12Key();
		return;
	}

	// M키(전체 지도) 토글 처리
// 레이더가 비활성화된 경우 전체 지도도 사용할 수 없다.
	if (GameConfig.GetRadarEnabledFlag() == false) {
		ShowFullMap = false;
	}
	else if (CheckInputControl(KEY_MAP, 1) == true) {
		ShowFullMap = !ShowFullMap;
	}

	// 전체 지도 표시 중에는 마우스 휠로 지도 확대/축소
	if (ShowFullMap == true) {
		int wheel = inputCtrl->GetMouseWheel();

		if (wheel != 0) {
			// wheel > 0 : 확대, 현재 기본 상태 쪽으로 복귀
			// wheel < 0 : 축소, 더 넓은 범위를 표시
			if (wheel > 0) {
				FullMapWorldR -= 100.0f;
			}
			else if (wheel < 0) {
				FullMapWorldR += 100.0f;
			}

			// 500.0f = 현재 전체 지도 상태, 즉 가장 확대된 상태
			if (FullMapWorldR < 500.0f) {
				FullMapWorldR = 500.0f;
			}

			// 최대로 축소했을 때 표시할 월드 범위
			if (FullMapWorldR > 2000.0f) {
				FullMapWorldR = 2000.0f;
			}

			inputCtrl->SetMouseWheel(0);
		}
	}


	//됪뽋궻UI빾뛛?띿궔?긃긞긏
	if( inputCtrl->CheckKeyDown( GetFunctionKeycode(2) ) ){
		if( Camera_F2mode == 2 ){
			Camera_F2mode = 0;
		}
		else{
			Camera_F2mode += 1;
		}
	}

#ifdef ENABLE_DEBUGCONSOLE
	if( Show_Console == false ){
#endif

		if( Camera_Debugmode == false ){	//믅륂긾?긤궶귞궽
			if( PlayerAI == false ){
				InputPlayer(myHuman, x, y, MouseSensitivity);
			}
		}
		else{								//긢긫긞긏긾?긤궶귞궽
			InputViewCamera(x, y, MouseSensitivity);
		}

#ifdef ENABLE_DEBUGCONSOLE
	}

	//긢긫긞긏뾭긓깛??깑궻?렑?띿궔?긃긞긏
	if( inputCtrl->CheckKeyDown( GetFunctionKeycode(11) ) ){
		if( Show_Console == false ){
			Show_Console = true;

			ConsoleTextBoxControl.SetTextBoxStr("");
		}
		else{
			Show_Console = false;
		}
	}

	if( Show_Console == true ){
		InputConsole();
	}
#endif

	time_input = GetTimeMS() - time;
}

bool maingame::ThrowPlayerImpactGrenade(human* myHuman)
{
	if (myHuman == NULL) { return false; }
	if (myHuman->GetHP() <= 0) { return false; }
	if (GetPlayerTemporarySkillWeaponAmmo() <= 0) { return false; }

	int PlayerID = ObjMgr.GetPlayerID();
	if (PlayerID < 0) { return false; }

	WeaponParameter ParamData;
	if (GameParamInfo.GetWeapon(ID_WEAPON_IMPACT_GRENADE, &ParamData) != 0) {
		return false;
	}

	float pos_x, pos_y, pos_z;
	float rotation_x, armrotation_y;
	int teamid;

	myHuman->GetPosData(&pos_x, &pos_y, &pos_z, NULL);
	myHuman->GetParamData(NULL, NULL, NULL, &teamid);
	myHuman->GetRxRy(&rotation_x, &armrotation_y);

	float crouch_y = 0.0f;
	if (myHuman->GetCrouchFlag() == true) {
		crouch_y = -2.0f;
	}

	float shot_y = pos_y + WEAPONSHOT_HEIGHT + crouch_y;

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

	float rx = rotation_x * -1 + (float)M_PI / 2.0f;
	float ry = armrotation_y;

	class grenade* newgrenade = ObjMgr.GetNewGrenadeObject();
	if (newgrenade == NULL) {
		return false;
	}

	int grenade_model = -1;
	int grenade_texture = -1;

	if (Resource.GetWeaponModelTexture(ID_WEAPON_IMPACT_GRENADE, &grenade_model, &grenade_texture) == 0) {
		newgrenade->SetModel(grenade_model, ParamData.size);
		newgrenade->SetTexture(grenade_texture);
	}

	newgrenade->SetPosData(muzzle_x, muzzle_y, muzzle_z, rx, ry);
	newgrenade->SetParamData(8.0f, teamid, PlayerID, 1.0f, ID_WEAPON_IMPACT_GRENADE, true);
	newgrenade->SetEnableFlag(true);

	// 충격 수류탄은 ObjMgr.ShotWeapon()을 거치지 않고 직접 생성되므로,
	// 일반 수류탄처럼 팔을 흔드는 투척 모션을 별도로 발생시킨다.
	myHuman->StartSkillWeaponShotMotion(ID_WEAPON_IMPACT_GRENADE);

	if (ParamData.soundvolume > 0) {
		GameSound->ShotWeapon(muzzle_x, muzzle_y, muzzle_z, ID_WEAPON_IMPACT_GRENADE, teamid, true);
	}

	return true;
}

bool maingame::IsPlayerHoldingGrenade(human* myHuman)
{
	return (GetPlayerCurrentGrenadeTrajectoryWeaponID(myHuman) != ID_WEAPON_NONE);
}

int maingame::GetPlayerCurrentGrenadeTrajectoryWeaponID(human* myHuman)
{
	if (myHuman == NULL) { return ID_WEAPON_NONE; }
	if (myHuman->GetHP() <= 0) { return ID_WEAPON_NONE; }

	if (IsPlayerTemporarySkillWeaponReady() == true) {
		int temp_weapon_id = GetPlayerTemporarySkillWeaponID();

		if (temp_weapon_id == ID_WEAPON_IMPACT_GRENADE) {
			return ID_WEAPON_IMPACT_GRENADE;
		}

		return ID_WEAPON_NONE;
	}

	int weapon_id = myHuman->GetMainWeaponTypeNO();

	if (weapon_id == ID_WEAPON_GRENADE) {
		return ID_WEAPON_GRENADE;
	}

	return ID_WEAPON_NONE;
}

void maingame::ClearGrenadeTrajectoryToggle()
{
	grenade_trajectory_flag = false;
	grenade_trajectory_weapon_id = ID_WEAPON_NONE;
}

void maingame::UpdateGrenadeTrajectoryAutoOff(human* myHuman)
{
	int current_grenade_weapon_id = GetPlayerCurrentGrenadeTrajectoryWeaponID(myHuman);

	if (current_grenade_weapon_id == ID_WEAPON_NONE) {
		ClearGrenadeTrajectoryToggle();
		return;
	}

	if ((grenade_trajectory_flag == true) &&
		(grenade_trajectory_weapon_id != current_grenade_weapon_id)) {
		ClearGrenadeTrajectoryToggle();
	}
}

bool maingame::ShouldRenderGrenadeTrajectory(human* myHuman)
{
	if (GetPlayerCurrentGrenadeTrajectoryWeaponID(myHuman) == ID_WEAPON_NONE) {
		return false;
	}

	if (grenade_trajectory_flag == false) {
		return false;
	}

	if ((Camera_Debugmode == true) || (Camera_F1mode == true)) {
		return false;
	}

	if ((IsPlayerSkillTargeting() == true) ||
		(myHuman->GetSkillRobotExecuteFlag() == true) ||
		(IsPlayerInstallSkillPending() == true) ||
		(player_skill_input_lock_timer > 0)) {
		return false;
	}

	return true;
}


//! @brief プレイヤ?（?作キャラ）の入力処理
//! @param myHuman プレイヤ?のクラス
//! @param mouse_x マウスのX座標
//! @param mouse_y マウスのY座標 
//! @param MouseSensitivity 視?の移動量調整
void maingame::InputPlayer(human* myHuman, int mouse_x, int mouse_y, float MouseSensitivity)
{
	if (myHuman == NULL) { return; }

	int PlayerID = ObjMgr.GetPlayerID();

	if (myHuman->GetHP() > 0) {

		// 플레이어 전용 스킬 입력
		bool skill_input_locked = TryUsePlayerSkill(myHuman);

		if (player_skill_input_lock_timer > 0) {
			player_skill_input_lock_timer--;
			skill_input_locked = true;
		}

		// 로봇 처형 프로토콜에서 좌클릭으로 처형한 뒤,
		// 좌클릭을 계속 누르고 있으면 입력 잠금 시간이 끝난 후 일반 총 발사로 이어질 수 있다.
		// 따라서 KEY_SHOT을 한 번 뗄 때까지 무기 입력으로 내려가지 않게 막는다.
		if (player_skill_block_shot_until_release == true) {
			if (CheckInputControl(KEY_SHOT, 0) == true) {
				skill_input_locked = true;
			}
			else {
				player_skill_block_shot_until_release = false;
			}
		}

		bool install_skill_pending = IsPlayerInstallSkillPending();

		// ================================
		// 조준 배율에 따른 자동 감도 조절
		// ================================
		float scoped_sensitivity = MouseSensitivity;

		int scopemode = myHuman->GetScopeMode();

		if (scopemode != 0) {
			if (scopemode == 7) {
				// 가변 배율 스코프
				float zoom = myHuman->CustomZoom;

				if (zoom < 1.5f) zoom = 1.5f;
				if (zoom > 10.0f) zoom = 10.0f;

				// 1.0x일 때는 감도 감소 없음
				// 2.0x 이상부터 서서히 감도 감소
				float zoom_sensitivity_scale = 2.0f / sqrtf(zoom);

				const float min_scope_sensitivity_ratio = 0.35f;
				if (zoom_sensitivity_scale < min_scope_sensitivity_ratio) {
					zoom_sensitivity_scale = min_scope_sensitivity_ratio;
				}

				scoped_sensitivity = MouseSensitivity * zoom_sensitivity_scale;
			}
			else {
				// 일반 고정 스코프만 감도 감소
				scoped_sensitivity = MouseSensitivity * 0.75f;
			}
		}

		//マウスによる向きを計算
		mouse_rx += mouse_x * scoped_sensitivity;
		//mouse_ry -= mouse_y * scoped_sensitivity;

		// [✨ R6S 스타일: 반동 제어 상쇄 및 충돌 방지 로직 ✨]
		float pull_down = (float)mouse_y * scoped_sensitivity;

		if (pull_down > 0.0f) { // 플레이어가 마우스를 아래로 내렸을 때
			float current_recoil = myHuman->GetRecoilYOffset();

			if (current_recoil > 0.0f) {
				// [핵심] 플레이어가 직접 제어 중일 때는 시스템 자동 회복(0.85f)이 끼어들지 못하게 막음
				myHuman->DelayRecoilRecovery();

				if (current_recoil >= pull_down) {
					myHuman->SetRecoilYOffset(current_recoil - pull_down);
					pull_down = 0.0f; // 에임을 직접 내리는 효과는 상쇄됨
				}
				else {
					pull_down -= current_recoil;
					myHuman->SetRecoilYOffset(0.0f);
				}
			}
		}

		// 상쇄되고 남은 마우스 이동량만 실제 베이스 시야에 적용
		mouse_ry -= pull_down;


		//キ??作による向きを計算
		if ((CheckInputControl(KEY_TURNUP, 0) == true) && (CheckInputControl(KEY_TURNDOWN, 0) == false)) { add_camera_ry += (INPUT_ARROWKEYS_ANGLE - add_camera_ry) * 0.2f; }
		else if ((CheckInputControl(KEY_TURNDOWN, 0) == true) && (CheckInputControl(KEY_TURNUP, 0) == false)) { add_camera_ry += (INPUT_ARROWKEYS_ANGLE * -1 - add_camera_ry) * 0.2f; }
		else { add_camera_ry = 0.0f; }
		if ((CheckInputControl(KEY_TURNLEFT, 0) == true) && (CheckInputControl(KEY_TURNRIGHT, 0) == false)) { add_camera_rx += (INPUT_ARROWKEYS_ANGLE * -1 - add_camera_rx) * 0.2f; }
		else if ((CheckInputControl(KEY_TURNRIGHT, 0) == true) && (CheckInputControl(KEY_TURNLEFT, 0) == false)) { add_camera_rx += (INPUT_ARROWKEYS_ANGLE - add_camera_rx) * 0.2f; }
		else { add_camera_rx = 0.0f; }
		mouse_rx += add_camera_rx;
		mouse_ry += add_camera_ry;

		if (mouse_ry > DegreeToRadian(70)) mouse_ry = DegreeToRadian(70);
		if (mouse_ry < DegreeToRadian(-70)) mouse_ry = DegreeToRadian(-70);


		//プレイヤ?（オブジェクト）の向きを設定
		myHuman->SetRxRy(mouse_rx, mouse_ry);


		//前後左右の移動（走り）?作か?ェック
		// 설치 대기 중에는 몸을 고정한다. 시점 회전은 위에서 이미 처리했으므로 그대로 허용된다.
		if (install_skill_pending == false) {
			if (CheckInputControl(KEY_MOVEFORWARD, 0)) {
				ObjMgr.MoveForward(PlayerID);
			}
			else {
				if (CheckInputControl(KEY_MOVEBACKWARD, 0)) {
					ObjMgr.MoveBack(PlayerID);
				}
			}
			if (CheckInputControl(KEY_MOVELEFT, 0)) {
				ObjMgr.MoveLeft(PlayerID);
			}
			else {
				if (CheckInputControl(KEY_MOVERIGHT, 0)) {
					ObjMgr.MoveRight(PlayerID);
				}
			}

			//歩き?作か?ェック
			if (CheckInputControl(KEY_WALK, 0)) {
				ObjMgr.MoveWalk(PlayerID);
			}
		}

		// 앉기 / 달리기 상태 갱신
		bool prev_crouching = myHuman->GetCrouchFlag();
		bool crouching = prev_crouching;
		bool running_input = false;

		if (install_skill_pending == false) {
			bool walk_hold = CheckInputControl(KEY_WALK, 0);

			bool run_hold = CheckInputControl(KEY_RUN, 0);
			bool run_down = CheckInputControl(KEY_RUN, 1);

			bool crouch_hold = CheckInputControl(KEY_CROUCH, 0);
			bool crouch_down = CheckInputControl(KEY_CROUCH, 1);

			if (GameConfig.GetRunToggleFlag() == true) {
				if (run_down == true) {
					player_run_toggle_state = !player_run_toggle_state;
					player_move_crouch_priority = false;
				}
				running_input = ((walk_hold == false) && (player_run_toggle_state == true));
			}
			else {
				player_run_toggle_state = false;

				if (run_down == true) {
					player_move_crouch_priority = false;
				}

				running_input = ((walk_hold == false) && (run_hold == true));
			}

			if (GameConfig.GetCrouchToggleFlag() == true) {
				if (crouch_down == true) {
					player_crouch_toggle_state = !player_crouch_toggle_state;
					player_move_crouch_priority = true;
				}
				crouching = player_crouch_toggle_state;
			}
			else {
				player_crouch_toggle_state = false;

				if (crouch_down == true) {
					player_move_crouch_priority = true;
				}

				crouching = crouch_hold;
			}

			// 둘 중 하나만 입력 중이면 우선순위도 자연스럽게 갱신한다.
			if ((running_input == true) && (crouching == false)) {
				player_move_crouch_priority = false;
			}
			else if ((running_input == false) && (crouching == true)) {
				player_move_crouch_priority = true;
			}
		}
		else {
			// 설치 대기 중에는 달리기를 강제로 끈다.
			// 앉기 상태는 기존 상태를 유지한다.
			player_run_toggle_state = false;
			running_input = false;
		}

		// 현재 위치 확인
		float px, py, pz;
		myHuman->GetPosData(&px, &py, &pz, NULL);

		// 서 있을 때 머리/상체가 들어갈 공간 검사
		bool can_stand = true;

		// 앉은 높이보다 위쪽, 서 있는 높이 근처에 블록이 있으면 일어서기 금지
		if (CollD.CheckALLBlockInside(px, py + HUMAN_HEIGHT - 1.0f, pz) == true) {
			can_stand = false;
		}
		if (CollD.CheckALLBlockInside(px + HUMAN_MAPCOLLISION_R, py + HUMAN_HEIGHT - 1.0f, pz) == true) {
			can_stand = false;
		}
		if (CollD.CheckALLBlockInside(px - HUMAN_MAPCOLLISION_R, py + HUMAN_HEIGHT - 1.0f, pz) == true) {
			can_stand = false;
		}
		if (CollD.CheckALLBlockInside(px, py + HUMAN_HEIGHT - 1.0f, pz + HUMAN_MAPCOLLISION_R) == true) {
			can_stand = false;
		}
		if (CollD.CheckALLBlockInside(px, py + HUMAN_HEIGHT - 1.0f, pz - HUMAN_MAPCOLLISION_R) == true) {
			can_stand = false;
		}

		// RUN / CROUCH가 동시에 입력되었을 때는 마지막 입력을 우선한다.
// 단, 낮은 천장 때문에 일어설 수 없으면 달리기를 막고 앉기를 유지한다.
		if ((crouching == true) && (running_input == true)) {
			if (player_move_crouch_priority == true) {
				// 달리기 중 앉기 입력: 달리기를 끄고 앉기 유지
				running_input = false;
				player_run_toggle_state = false;
			}
			else {
				// 앉기 중 달리기 입력: 일어설 수 있으면 앉기를 풀고 달리기
				if (can_stand == true) {
					crouching = false;
					player_crouch_toggle_state = false;
				}
				else {
					// 낮은 천장 아래에서는 일어설 수 없으므로 앉기 유지
					running_input = false;
					player_run_toggle_state = false;
					player_move_crouch_priority = true;
				}
			}
		}

		// 방패 장착 중에는 앉기 입력을 무시한다.
		// E 스킬 임시 방패와 F7로 소환한 실제 92번 방패를 모두 포함한다.
		// 단, 낮은 천장 아래에서는 충돌 방지를 위해 기존 앉기 유지 로직을 우선한다.
		if ((install_skill_pending == false) &&
			(IsPlayerShieldWeaponEquipped(myHuman) == true) &&
			(can_stand == true)) {
			crouching = false;
			player_crouch_toggle_state = false;
			player_move_crouch_priority = false;
		}
		// 앉기/서기 전환 순간에는 카메라 높이와 팔/무기 기준점이 동시에 바뀐다.
		// 144Hz 렌더 보간이 이전 높이와 새 높이를 섞으면 팔이 흔들려 보인다.
		if (prev_crouching != crouching) {
			SyncDrawCamera();
		}

		myHuman->SetCrouchFlag(crouching);

		//ジャンプ?作か?ェック
		// 앉은 상태에서도 점프 가능
		if ((install_skill_pending == false) &&
			(CheckInputControl(KEY_JUMP, 1) == true)) {
			ObjMgr.MoveJump(PlayerID);
		}

		float base_speed_multiplier = 1.0f;
		bool shield_equipped_for_move = IsPlayerShieldWeaponEquipped(myHuman);

		if (shield_equipped_for_move == true) {
			// 방패 장착 중 이동 속도.
			// 일반 방패 걷기: 0.80배
			// 방패 달리기: 방패 걷기 속도의 1.4배 = 0.80f * 1.4f
			// E 스킬 임시 방패와 F7로 소환한 실제 92번 방패를 모두 포함한다.
			if (crouching == true) {
				base_speed_multiplier = 0.45f * PLAYER_SKILL_SHIELD_MOVE_SPEED_MULT;
			}
			else if (running_input == true) {
				base_speed_multiplier = PLAYER_SKILL_SHIELD_MOVE_SPEED_MULT * 1.4f;
			}
			else {
				base_speed_multiplier = PLAYER_SKILL_SHIELD_MOVE_SPEED_MULT;
			}
		}
		else {
			if (crouching == true) {
				base_speed_multiplier = 0.45f; // 앉기 이동속도
			}
			else if (running_input == true) {
				base_speed_multiplier = 1.4f; // 달리기
			}
			else {
				base_speed_multiplier = 1.0f; // 일반 속도
			}
		}

		// 시민B 생존 본능: 기존 앉기/달리기 속도에 20% 추가 배율만 곱한다.
		if (myHuman->GetSkillSurvivalFlag() == true) {
			base_speed_multiplier *= 1.20f;
		}

		// 좀비 격노: 이동 속도 30% 증가
		if (myHuman->GetSkillRageFlag() == true) {
			base_speed_multiplier *= PLAYER_SKILL_RAGE_MOVE_SPEED_MULT;
		}

		// 시민A 철괴: 기존 앉기/달리기 속도에 90% 감속 배율을 곱한다.
		if (myHuman->GetSkillIronBodyFlag() == true) {
			base_speed_multiplier *= 0.10f;
		}

		if (install_skill_pending == true) {
			base_speed_multiplier = 0.0f;
		}

		myHuman->SpeedMultiplier = base_speed_multiplier;

		// 조준 확정형 스킬 중이거나 발동 직후 입력 잠금 중이면
// 좌클릭/우클릭이 총 발사, 스코프 전환, 수류탄 투척으로 이어지지 않게 막는다.
		if ((IsPlayerSkillTargeting() == true) ||
			(myHuman->GetSkillRobotExecuteFlag() == true) ||
			(install_skill_pending == true) ||
			(skill_input_locked == true) ||
			(player_skill_input_lock_timer > 0)) {
			return;
		}

		HumanParameter humandata;
		int id_param;
		bool zombie;
		int keymode;
		WeaponParameter weapon_paramdata; // [✨ 추가 ✨]

		//?ンビかどうか判定
		myHuman->GetParamData(&id_param, NULL, NULL, NULL);
		GameParamInfo.GetHuman(id_param, &humandata);
		if (humandata.type == 2) {
			zombie = true;
		}
		else {
			zombie = false;
		}

		// [✨ 수정 ✨] 점사 모드를 고려하여 발사 입력 방식 변경
		// Get Main Weapon Parameter
		int main_weapon_id = myHuman->GetMainWeaponTypeNO();
		if (main_weapon_id != ID_WEAPON_NONE) {
			GameParamInfo.GetWeapon(main_weapon_id, &weapon_paramdata);
		}
		else {
			weapon_paramdata.burstcnt = 1; // 무기가 없으면 단발 취급
		}

		UpdateGrenadeTrajectoryAutoOff(myHuman);

		bool isPlayerMinigun = (zombie == false && weapon_paramdata.burstcnt == 6);

		// Set keymode
		if (zombie == true) {
			// keymode = 1; // Press
			keymode = 0; // Hold (연사로 변경)
		}
		// [✨ 수정 ✨] 미니건(burstcnt == 6)을 완전 자동으로 인식하도록 조건 추가
		else if (weapon_paramdata.burstcnt == 0 || weapon_paramdata.burstcnt == 6) { // Full-auto or Minigun
			keymode = 0; // Hold
		}
		else { // Semi-auto or Burst
			keymode = 1; // Press
		}

		// -------------------------------------------------------------
// 미니건 예열 처리
// - 우클릭: 발사 없이 예열
// - 좌클릭: 예열 + 예열 완료 후 발사
// - 둘 다 떼면 서서히 감속
// -------------------------------------------------------------
		bool isMinigunBusy = false;

		if (isPlayerMinigun) {
			bool shotHold = CheckInputControl(KEY_SHOT, 0);
			bool spinHold = CheckInputControl(KEY_ZOOMSCOPE, 0);

			// 재장전 상태 확인
// 일반 탄창식 재장전: GetWeaponReloadCnt() > 0
// 셸 단위 재장전: GetShellReloadState() != SHELL_RELOAD_NONE
// 볼트/장전 후 딜레이: GetBoltActionTimer() > 0
			human::ShellReloadState reloadStateForMinigun = myHuman->GetShellReloadState();

			bool isReloadingForMinigun =
				(myHuman->GetWeaponReloadCnt() > 0) ||
				(reloadStateForMinigun != human::SHELL_RELOAD_NONE) ||
				(myHuman->GetBoltActionTimer() > 0);

			// 재장전 중에는 우클릭/좌클릭을 누르고 있어도 예열이 올라가지 않는다.
			// 이미 예열이 남아 있으면 UpdateMinigunSpin(false)에 의해 서서히 감소한다.
			bool spinInput = false;
			if (isReloadingForMinigun == false) {
				spinInput = (shotHold || spinHold);
			}

			int spinBefore = myHuman->GetMinigunSpinTimer();
			int spinTotal = myHuman->GetMinigunSpinTotalTime();

			myHuman->UpdateMinigunSpin(spinInput);

			int spinAfter = myHuman->GetMinigunSpinTimer();

			// 예열이 조금이라도 남아 있으면 미니건은 아직 바쁜 상태.
			// 즉, 최대 예열 상태에서 입력을 떼고 감속 중이어도
			// spinAfter가 0이 될 때까지 재장전/무기교체를 막을 수 있다.
			isMinigunBusy = (spinAfter > 0);

			float sx, sy, sz;
			int steamid;

			myHuman->GetPosData(&sx, &sy, &sz, NULL);
			myHuman->GetParamData(NULL, NULL, NULL, &steamid);
			sy += 16.0f;

			// 미니건 예열 시작음
// spinup_timer가 0에서 올라가기 시작하는 순간에만 1회 재생한다.
			if (spinBefore == 0 && spinAfter > 0 && spinInput == true) {
				GameSound->ReloadWeapon(sx, sy, sz, RELOAD_MINIGUN_SPIN_START, steamid, true);
			}

			// 미니건 예열 지속음
			// 최대 예열에 한 번 도달한 뒤부터,
			// spinup_timer가 0으로 떨어질 때까지 계속 재생한다.
			int spinSoundStart = spinTotal / 4;

			// 50% 이상 도달하면 지속음 상태를 켠다.
			if (spinBefore < spinSoundStart && spinAfter >= spinSoundStart) {
				myHuman->SetMinigunSpinLoopEnabled(true);
			}

			// 지속음 상태가 켜졌고, 아직 예열 타이머가 남아 있으면 반복음 재생
			if (myHuman->IsMinigunSpinLoopEnabled() && spinAfter > 0) {
				if (myHuman->ConsumeMinigunSpinSoundTick()) {
					GameSound->ReloadWeapon(sx, sy, sz, RELOAD_MINIGUN_SPIN, steamid, true);
				}
			}

			// 미니건 예열 종료음
			// 완전히 회전이 멈춘 순간에만 1회 재생한다.
			if (spinBefore > 0 && spinAfter == 0) {
				GameSound->ReloadWeapon(sx, sy, sz, RELOAD_MINIGUN_SPIN_END, steamid, true);
				myHuman->SetMinigunSpinLoopEnabled(false);
			}
		}

		// F7 등으로 실제 무기 슬롯에 92번 방패를 든 경우도 좌클릭 사격을 막는다.
// 임시 스킬 방패는 아래 임시 무기 분기에서 이미 처리된다.
		if ((IsPlayerTemporarySkillWeaponActive() == false) &&
			(IsPlayerShieldWeaponEquipped(myHuman) == true)) {
			// 방패는 공격/사격 기능이 없다.
			// 기존 발사 로직으로 내려가지 않게 이 프레임의 발사 처리를 끝낸다.
		}
		// 임시 스킬 무기를 들고 있으면 기존 총 발사를 가로채고 해당 스킬 무기를 사용한다.
		else if ((IsPlayerTemporarySkillWeaponActive() == true) ||
			(IsPlayerTemporarySkillWeaponFireLocked() == true) ||
			(IsPlayerTemporarySkillWeaponShotDelayed() == true)) {

			int temp_skill_weapon_id = GetPlayerTemporarySkillWeaponID();

			// CHANGING 중에는 기존 총 발사도, 임시 스킬 무기 사용도 하지 않는다.
			if (IsPlayerTemporarySkillWeaponChanging() == true) {
				// 아무 것도 하지 않음
			}
			else if (IsPlayerTemporarySkillWeaponReady() == true) {
				if (temp_skill_weapon_id == ID_WEAPON_IMPACT_GRENADE) {
					if ((IsPlayerTemporarySkillWeaponFireLocked() == true) ||
						(IsPlayerTemporarySkillWeaponShotDelayed() == true)) {
						// 마우스 버튼을 뗄 때까지 또는 다음 투척 딜레이가 끝날 때까지 추가 투척 방지
					}
					else if (CheckInputControl(KEY_SHOT, 1)) {
						if (ThrowPlayerImpactGrenade(myHuman) == true) {
							int remaining_ammo = ConsumePlayerTemporarySkillWeaponAmmo(1);

							SetPlayerTemporarySkillWeaponFireLock(true);

							// else if (shot_result == 2) {탄이 남아 있으면 기존 수류탄처럼 계속 들고 있게 하되,
							// Weapon[91].blazings 기준의 다음 투척 딜레이를 적용한다.
							if (remaining_ammo > 0) {
								StartPlayerTemporarySkillWeaponShotDelay();
								KeepPlayerTemporarySkillWeaponReadyAfterFire();
							}
							// 다 쓰면 기존 무기로 복귀한다.
							else {
								StartPlayerTemporarySkillWeaponReturn(myHuman, GetPlayerTemporarySkillWeaponReturnSlot());
							}
						}
						else {
							PlayPlayerSkillSound(SKILL_SOUND_COMMON_FAIL);
							StartPlayerTemporarySkillWeaponReturn(myHuman, GetPlayerTemporarySkillWeaponReturnSlot());
						}
					}
				}
				else if (temp_skill_weapon_id == ID_WEAPON_SHIELD) {
					// 방패는 공격/사격 기능이 없다.
					// 이 분기 자체가 기존 총 발사를 가로채므로 좌클릭은 조용히 무시된다.
				}
			}

			// 마우스 왼쪽 버튼을 뗐을 때 Impact grenade 다음 투척을 허용한다.
			if (CheckInputControl(KEY_SHOT, 0) == false) {
				SetPlayerTemporarySkillWeaponFireLock(false);
			}
		}
		// 셸 단위 재장전 중 발사 키를 누르면 재장전을 중단합니다.
		else if (myHuman->IsShellReloading()) {
			if (CheckInputControl(KEY_SHOT, 1)) { // 단발 입력(키를 눌렀다 떼는 순간)만 감지
				myHuman->InterruptShellReload();
			}
		}
		else {
			// 기존 발사 로직
			if (weapon_paramdata.burstcnt == 6) {
				if (CheckInputControl(KEY_SHOT, keymode)) {
					if (ObjMgr.ShotWeapon(PlayerID) == 1) {
						MainGameInfo.fire += 1;
					}
				}
			}
			else if (myHuman->GetBurstModeCnt() == 0) {
				if (CheckInputControl(KEY_SHOT, keymode)) {
					if (zombie == false) {
						int shot_result = ObjMgr.ShotWeapon(PlayerID);

						if (shot_result == 1) {
							MainGameInfo.fire += 1;
						}
						else if (shot_result == 2) {
							// 수류탄 투척 성공.
							// 탄이 남아 같은 수류탄을 계속 들고 있으면 토글 상태를 유지하고,
							// 마지막 수류탄이었다면 weapon::Shot()에서 무기가 제거되므로 자동 OFF된다.
							UpdateGrenadeTrajectoryAutoOff(myHuman);
						}
					}
					else {
						// 좀비 공격
						if (myHuman->GetBoltActionTimer() == 0) {
							float sx, sy, sz;
							int steamid;

							myHuman->GetPosData(&sx, &sy, &sz, NULL);
							myHuman->GetParamData(NULL, NULL, NULL, &steamid);
							sy += VIEW_HEIGHT;

							GameSound->ZombieAttack(sx, sy, sz, steamid, true);

							for (int i = 0; i < MAX_HUMAN; i++) {
								human* EnemyHuman = ObjMgr.GetHumanObject(i);
								if (ObjMgr.CheckZombieAttack(myHuman, EnemyHuman) == true) {
									ObjMgr.HitZombieAttack(myHuman, EnemyHuman);
								}
							}

							int zombie_attack_wait = 10;

							// 좀비 격노: 공격 속도 30% 증가 = 공격 대기시간 30% 감소
							if (myHuman->GetSkillRageFlag() == true) {
								zombie_attack_wait = (int)((float)zombie_attack_wait * PLAYER_SKILL_RAGE_FIRE_WAIT_MULT + 0.5f);

								if (zombie_attack_wait < 1) {
									zombie_attack_wait = 1;
								}
							}

							myHuman->SetBoltActionTimers(zombie_attack_wait, zombie_attack_wait);
						}
					}
				}
			}
		}

		// 발사 키를 누르고 있지 않을 때 점사 리셋을 시도합니다. (키를 누르고 있으면 리셋 안 됨)
		if (CheckInputControl(KEY_SHOT, 0) == false) {
			if (zombie == false && weapon_paramdata.burstcnt > 1) {
				myHuman->ResetBurstModeCnt();
			}

			// [✨ R6S 스타일 추가 ✨] 마우스 클릭을 떼는 순간 반동 대기 타이머를 즉시 0으로 만들어 빠릿하게 회복시킴
			if (zombie == false) {
				myHuman->ResetRecoilTimer();
			}
		}

		// リロード操作チェック
// 미니건 예열 중에는 재장전 금지
		if (CheckInputControl(KEY_RELOAD, 1)) {
			if (IsPlayerTemporarySkillWeaponVisible() == true) {
				StartPlayerImpactGrenadeReturn(myHuman, GetPlayerTemporarySkillWeaponReturnSlot());
			}
			else if (IsPlayerTemporarySkillWeaponChanging() == true) {
				// Impact grenade 복귀 중에는 숨겨진 기존 무기가 재장전되지 않도록 무시한다.
			}
			else if (isMinigunBusy == false) {
				ObjMgr.ReloadWeapon(PlayerID);
			}
		}

		// 武器の切り替え操作チェック
// 미니건 예열 중에는 무기 전환 금지
		if (CheckInputControl(KEY_SWITCHWEAPON, 1)) {
			if (IsPlayerTemporarySkillWeaponVisible() == true) {
				StartPlayerImpactGrenadeReturn(myHuman, -1);
			}
			else if (IsPlayerTemporarySkillWeaponChanging() == true) {
				// Impact grenade 복귀 중에는 추가 무기 전환을 무시한다.
			}
			else if (IsPlayerDP28SkillWeaponActive() == true) {
				// DP28 재장전 중에는 기존 무기로 복귀하지 않는다.
				// 재장전 중 복귀를 허용하면 복구된 기존 무기에 재장전 결과가 섞일 수 있다.
				if (IsPlayerCurrentWeaponReloading(myHuman) == true) {
					// 무기전환 입력은 조용히 무시한다.
				}
				else {
					// DP28 스킬 무기 상태에서는 기존 무기를 복구한 뒤 다음 슬롯으로 돌아간다.
					int return_slot = GetPlayerDP28SkillWeaponReturnSlot();

					if (return_slot < 0) {
						return_slot = 0;
					}

					int next_slot = return_slot + 1;

					if (next_slot >= TOTAL_HAVEWEAPON) {
						next_slot = 0;
					}

					ReturnPlayerDP28SkillWeapon(myHuman);
					myHuman->ForceSelectWeaponSlotForSkillReturn(next_slot, 5);
				}
			}
			else if (isMinigunBusy == false) {
				ObjMgr.ChangeHaveWeapon(PlayerID, -1);
			}
		}
		if (CheckInputControl(KEY_WEAPON1, 1)) {
			if (IsPlayerTemporarySkillWeaponVisible() == true) {
				StartPlayerImpactGrenadeReturn(myHuman, 0);
			}
			else if (IsPlayerTemporarySkillWeaponChanging() == true) {
				// Impact grenade 복귀 중에는 추가 무기 전환을 무시한다.
			}
			else if (IsPlayerDP28SkillWeaponActive() == true) {
				// DP28 재장전 중에는 기존 무기로 복귀하지 않는다.
				if (IsPlayerCurrentWeaponReloading(myHuman) == true) {
					// 무기전환 입력은 조용히 무시한다.
				}
				else {
					// DP28 스킬 무기 상태에서는 기존 무기를 복구한 뒤 1번 슬롯으로 돌아간다.
					ReturnPlayerDP28SkillWeapon(myHuman);
					myHuman->ForceSelectWeaponSlotForSkillReturn(0, 5);
				}
			}
			else if (isMinigunBusy == false) {
				ObjMgr.ChangeHaveWeapon(PlayerID, 0);
			}
		}
		if (CheckInputControl(KEY_WEAPON2, 1)) {
			if (IsPlayerTemporarySkillWeaponVisible() == true) {
				StartPlayerImpactGrenadeReturn(myHuman, 1);
			}
			else if (IsPlayerTemporarySkillWeaponChanging() == true) {
				// Impact grenade 복귀 중에는 추가 무기 전환을 무시한다.
			}
			else if (IsPlayerDP28SkillWeaponActive() == true) {
				// DP28 재장전 중에는 기존 무기로 복귀하지 않는다.
				if (IsPlayerCurrentWeaponReloading(myHuman) == true) {
					// 무기전환 입력은 조용히 무시한다.
				}
				else {
					// DP28 스킬 무기 상태에서는 기존 무기를 복구한 뒤 2번 슬롯으로 돌아간다.
					ReturnPlayerDP28SkillWeapon(myHuman);
					myHuman->ForceSelectWeaponSlotForSkillReturn(1, 5);
				}
			}
			else if (isMinigunBusy == false) {
				ObjMgr.ChangeHaveWeapon(PlayerID, 1);
			}
		}

		// 連射モード変更操作チェック
// 미니건 예열 중에는 무기 모드 변경도 막는다.
		if (CheckInputControl(KEY_SHOTMODE, 1)) {
			// Impact grenade 같은 임시 스킬 무기 상태에서는
			// 뒤에 숨겨진 기존 무기의 발사모드가 바뀌지 않도록 막는다.
			if (IsPlayerTemporarySkillWeaponActive() == true) {
				ClearGrenadeTrajectoryToggle();
			}
			else if (isMinigunBusy == false) {
				ObjMgr.ChangeWeaponID(PlayerID);
			}
		}

		// 武器の廃棄操作チェック
// 미니건 예열 중에는 무기 버리기도 금지
		if (CheckInputControl(KEY_DROPWEAPON, 1)) {
			// 스킬로 임시 장착한 무기는 버리지 못하게 한다.
			// Impact grenade 상태에서 버리기를 허용하면 뒤에 숨겨진 실제 무기가 버려질 수 있다.
			if (IsPlayerTemporarySkillWeaponActive() == true) {
				ClearGrenadeTrajectoryToggle();
			}
			else if (isMinigunBusy == false) {
				ObjMgr.DumpWeapon(PlayerID);
			}
		}

		// 스코프 / 수류탄 궤도 입력 처리
// - 수류탄을 들고 있으면 우클릭은 궤도 표시
// - 미니건은 기존처럼 우클릭 예열에 사용
// - 그 외 무기는 스코프 전환 또는 홀드 조준
		int current_grenade_trajectory_weapon_id = GetPlayerCurrentGrenadeTrajectoryWeaponID(myHuman);
		bool holding_grenade = (current_grenade_trajectory_weapon_id != ID_WEAPON_NONE);
		bool zoom_down = CheckInputControl(KEY_ZOOMSCOPE, 1);
		bool zoom_hold = CheckInputControl(KEY_ZOOMSCOPE, 0);

		if (holding_grenade == true) {
			// 수류탄을 들면 스코프와 동시에 켜지지 않도록 스코프를 해제한다.
			if (myHuman->GetScopeMode() != 0) {
				myHuman->SetDisableScope();
			}

			if (GameConfig.GetGrenadeTrajectoryToggleFlag() == true) {
				// TOGGLE: 현재 들고 있는 수류탄 무기에만 ON 상태를 묶는다.
				if (zoom_down == true) {
					if ((grenade_trajectory_flag == true) &&
						(grenade_trajectory_weapon_id == current_grenade_trajectory_weapon_id)) {
						ClearGrenadeTrajectoryToggle();
					}
					else {
						grenade_trajectory_flag = true;
						grenade_trajectory_weapon_id = current_grenade_trajectory_weapon_id;
					}
				}
			}
			else {
				// HOLD: 누르고 있는 동안만 표시한다.
				grenade_trajectory_flag = zoom_hold;
				grenade_trajectory_weapon_id = zoom_hold ? current_grenade_trajectory_weapon_id : ID_WEAPON_NONE;
			}
		}
		else {
			// 수류탄을 들고 있지 않으면 궤적 표시는 항상 해제한다.
			ClearGrenadeTrajectoryToggle();

			// 미니건 우클릭 예열은 스코프 설정의 영향을 받으면 안 된다.
			if (isPlayerMinigun == false) {
				if (GameConfig.GetScopeAimToggleFlag() == true) {
					// TOGGLE: 기존 방식
					if (zoom_down == true) {
						ObjMgr.ChangeScopeMode(PlayerID);
					}
				}
				else {
					// HOLD: 누르는 순간 스코프를 켜고, 떼면 끈다.
					if ((zoom_down == true) && (myHuman->GetScopeMode() == 0)) {
						ObjMgr.ChangeScopeMode(PlayerID);
					}

					if ((zoom_hold == false) && (myHuman->GetScopeMode() != 0)) {
						myHuman->SetDisableScope();
					}
				}
			}
		}

		if (myHuman->GetScopeMode() != 0) {
			int weapon_id = myHuman->GetMainWeaponTypeNO();
			WeaponParameter wp;
			bool canUseNVG = false;

			// 무기를 들고 있고, 그 무기 정보가 정상적으로 로드될 때
			if (weapon_id != -1 && GameParamInfo.GetWeapon(weapon_id, &wp) == 0) {
				// 스코프 번호가 4, 5, 7번일 때만 야간 투시경 사용 가능
				if (wp.scopemode == 4 || wp.scopemode == 5 || wp.scopemode == 6  || wp.scopemode == 7) {
					canUseNVG = true;
				}
			}

			// 2. N키 입력 처리 (스코프 조건이 맞을 때만 토글 허용)
			if (CheckInputControl(KEY_NIGHTVISION, 1)) {
				if (canUseNVG) {
					myHuman->NightVision = !myHuman->NightVision;

					// 직접 켰을 때만 소리 재생
					int nvg_snd = Resource.GetNVGSound();
					if (nvg_snd != -1) {
						float px, py, pz, prx;
						myHuman->GetPosData(&px, &py, &pz, &prx);
						int vol = (int)(GameConfig.GetVolume() * 100.0f);
						SoundCtrl.Play3DSound(nvg_snd, px, py, pz, vol);
					}
				}
			}

			// 3. 조건에 맞지 않는 무기로 바뀌면 자동으로 끄기
			if (canUseNVG == false) {
				myHuman->NightVision = false;
			}
		}

		if (ShowFullMap == false) {
			int current_scope = myHuman->GetScopeMode();

			if (current_scope != 0) {
				WeaponParameter wp;
				if (GameParamInfo.GetWeapon(myHuman->GetMainWeaponTypeNO(), &wp) == 0) {
					if (wp.scopemode == 7) {

						// 가변 배율 스코프는 조준하는 순간 최소 1.5x로 보정
						if (myHuman->CustomZoom < 1.5f) myHuman->CustomZoom = 1.5f;
						if (myHuman->CustomZoom > 10.0f) myHuman->CustomZoom = 10.0f;

						int wheel = inputCtrl->GetMouseWheel();
						if (wheel != 0) {
							if (wheel > 0) myHuman->CustomZoom += 0.5f;
							else if (wheel < 0) myHuman->CustomZoom -= 0.5f;

							if (myHuman->CustomZoom < 1.5f) myHuman->CustomZoom = 1.5f;
							if (myHuman->CustomZoom > 10.0f) myHuman->CustomZoom = 10.0f;

							inputCtrl->SetMouseWheel(0);
						}
					}
				}
			}
			else {
				inputCtrl->SetMouseWheel(0);
			}
		}
	}


	//긇긽깋?렑긾?긤빾뛛?띿궔?긃긞긏
	//긇긽깋?렑긾?긤빾뛛?띿궔?긃긞긏
	if (inputCtrl->CheckKeyDown(GetFunctionKeycode(1))) {
		if (Camera_F1mode == false) {
			Camera_F1mode = true;
			view_rx = 0.0f;
			view_ry = VIEW_F1MODE_ANGLE;
		}
		else {
			Camera_F1mode = false;
			view_rx = 0.0f;
			view_ry = 0.0f;
		}

		// 1인칭/3인칭 전환 순간에는 카메라 위치가 크게 바뀌므로
		// 이전 카메라와 새 카메라 사이를 보간하면 어색해진다.
		SyncDrawCamera();

	}

	//긇긽깋?띿
	if (Camera_F1mode == true) {
		if (inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x0C))) {	//NUM8
			view_ry -= INPUT_F1NUMKEYS_ANGLE;
		}
		if (inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x09))) {	//NUM5
			view_ry += INPUT_F1NUMKEYS_ANGLE;
		}
		if (inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x08))) {	//NUM4
			view_rx -= INPUT_F1NUMKEYS_ANGLE;
		}
		if (inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x0A))) {	//NUM6
			view_rx += INPUT_F1NUMKEYS_ANGLE;
		}
	}

	//?궞궞귏궳믅륂?띿똭
	//
	//?궞궞궔귞뿞땆똭

	//뿞땆갋뤵뤈궻?띿궔?긃긞긏
	if ((inputCtrl->CheckKeyNow(GetFunctionKeycode(5))) && (inputCtrl->CheckKeyNow(OriginalkeycodeToDinputdef(0x0F)))) {	// F5 + [ENTER]
		Cmd_F5 = true;
	}
	else {
		Cmd_F5 = false;
	}

	if (myHuman->GetHP() > 0) {

		//뿞땆갋뭙믁돿궻?띿궔?긃긞긏
		if (inputCtrl->CheckKeyNow(GetFunctionKeycode(6))) {
			if (inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x0F))) {		// [ENTER]
				ObjMgr.CheatAddBullet(PlayerID);
			}
		}

		//뿞땆갋븧딇빾뛛궻?띿궔?긃긞긏
		if (inputCtrl->CheckKeyNow(GetFunctionKeycode(7))) {
			if (inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x02))) {		// [겑]
				int id_param = myHuman->GetMainWeaponTypeNO();

				// 다음 무기 번호를 계산한다.
				if (id_param >= TOTAL_PARAMETERINFO_WEAPON - 1) { id_param = 0; }
				else { id_param += 1; }

				// 처형 프로토콜 전용 무기는 연출 전용이므로 F7 획득 목록에서 제외한다.
				if (id_param == ID_WEAPON_ROBOT_EXECUTE_RIFLE) {
					if (id_param >= TOTAL_PARAMETERINFO_WEAPON - 1) { id_param = 0; }
					else { id_param += 1; }
				}

				ObjMgr.CheatNewWeapon(PlayerID, id_param);
			}
			if (inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x03))) {		// [겏]
				int id_param = myHuman->GetMainWeaponTypeNO();

				// 이전 무기 번호를 계산한다.
				if (id_param <= 0) { id_param = TOTAL_PARAMETERINFO_WEAPON - 1; }
				else { id_param -= 1; }

				// 처형 프로토콜 전용 무기는 연출 전용이므로 F7 획득 목록에서 제외한다.
				if (id_param == ID_WEAPON_ROBOT_EXECUTE_RIFLE) {
					if (id_param <= 0) { id_param = TOTAL_PARAMETERINFO_WEAPON - 1; }
					else { id_param -= 1; }
				}

				ObjMgr.CheatNewWeapon(PlayerID, id_param);
			}
		}

	}

	//뿞땆갋릐빾뛛궻?띿궔?긃긞긏
	if (inputCtrl->CheckKeyNow(GetFunctionKeycode(8))) {
		int Player_HumanID;

		if (inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x02))) {		// [겑]
			//뙸띪궻긵깒귽깂?붥뜂귩롦벦
			Player_HumanID = ObjMgr.GetPlayerID();

			//렅궻릐귩똶럁
			Player_HumanID += 1;
			if (Player_HumanID >= MAX_HUMAN) { Player_HumanID = 0; }

			// 스킬 무기 상태에서 캐릭터를 바꾸면,
// 새 플레이어가 이전 플레이어의 스킬 무기 상태를 이어받는 문제가 생기므로 먼저 복구/취소한다.
			ReturnPlayerDP28SkillWeapon(myHuman);
			CancelPlayerTemporarySkillWeapon(myHuman);

			//뫮뤭긵깒귽깂?붥뜂귩밙뾭
			ObjMgr.SetPlayerID(Player_HumanID);

			//긵깒귽깂?궻뛀궖귩롦벦
			ObjMgr.GetPlayerHumanObject()->GetRxRy(&mouse_rx, &mouse_ry);

			//F1긾?긤렄궸긇긽깋궻뛀궖귩띋먠믦
			if (Camera_F1mode == true) {
				camera_rx = DegreeToRadian(90);
			}
		}
		if (inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x03))) {		// [겏]
			//뙸띪궻긵깒귽깂?붥뜂귩롦벦
			Player_HumanID = ObjMgr.GetPlayerID();

			//렅궻릐귩똶럁
			Player_HumanID -= 1;
			if (Player_HumanID < 0) { Player_HumanID = MAX_HUMAN - 1; }

			// 스킬 무기 상태에서 캐릭터를 바꾸면,
// 새 플레이어가 이전 플레이어의 스킬 무기 상태를 이어받는 문제가 생기므로 먼저 복구/취소한다.
			ReturnPlayerDP28SkillWeapon(myHuman);
			CancelPlayerTemporarySkillWeapon(myHuman);

			//뫮뤭긵깒귽깂?붥뜂귩밙뾭
			ObjMgr.SetPlayerID(Player_HumanID);

			//긵깒귽깂?궻뛀궖귩롦벦
			ObjMgr.GetPlayerHumanObject()->GetRxRy(&mouse_rx, &mouse_ry);

			//F1긾?긤렄궸긇긽깋궻뛀궖귩띋먠믦
			if (Camera_F1mode == true) {
				camera_rx = DegreeToRadian(90);
			}
		}
	}

	//뿞땆갋릐믁돿궻?띿궔?긃긞긏
	if (inputCtrl->CheckKeyNow(GetFunctionKeycode(9))) {
		if ((inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x00))) || (inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x01)))) {		// [겒]갋[겓]
			float x, y, z, r;
			int param, dataid, team;
			int selectweapon;
			weapon* weapon[TOTAL_HAVEWEAPON];
			int weapon_paramid[TOTAL_HAVEWEAPON];
			for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
				weapon[i] = NULL;
				weapon_paramid[i] = 0;
			}
			int id;

			//긵깒귽깂?궻띆뷭귘븧딇귩롦벦
			myHuman->GetPosData(&x, &y, &z, &r);
			myHuman->GetParamData(&param, &dataid, NULL, &team);
			myHuman->GetWeapon(&selectweapon, weapon, NULL, NULL);
			for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
				if (weapon[i] != NULL) {
					weapon[i]->GetParamData(&weapon_paramid[i], NULL, NULL);
				}
			}

			//긵깒귽깂?궻뽞궻멟궻띆뷭귩롦벦
			x += cosf(r * -1 + (float)M_PI / 2) * 10.0f;
			y += 5.0f;
			z += sinf(r * -1 + (float)M_PI / 2) * 10.0f;

			//릐귩믁돿
			id = ObjMgr.AddHumanIndex(x, y, z, r, param, team, weapon_paramid, 0, false);
			if (id >= 0) {
				ObjMgr.ChangeHaveWeapon(id, selectweapon);

				//AI귩먠믦
				HumanAI[id].Init();
				HumanAI[id].SetForceNoAI(false);
				if (inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x00))) {		// [겒]
					HumanAI[id].SetHoldTracking(PlayerID);
				}
				if (inputCtrl->CheckKeyDown(OriginalkeycodeToDinputdef(0x01))) {		// [겓]
					HumanAI[id].SetHoldWait(x, z, r);
				}
			}
		}
	}

	//뿞땆갋쁱?됪궻?띿궔?긃긞긏
	if (inputCtrl->CheckKeyDown(GetHomeKeycode())) {
		if (Camera_HOMEmode == true) {
			Camera_HOMEmode = true;
		}
		else {
			Camera_HOMEmode = true;
		}
	}
}

//! @brief 긲깏?긇긽깋?띿똭궻볺쀍룉뿚
//! @param mouse_x ?긂긚궻X띆뷭
//! @param mouse_y ?긂긚궻Y띆뷭 
//! @param MouseSensitivity 럨?궻댷벍쀊똶럁
void maingame::InputViewCamera(int mouse_x, int mouse_y, float MouseSensitivity)
{
	//?긂긚댷벍귩긇긽깋궻뛀궖궴궢궲밙뾭
	camera_rx -= mouse_x * MouseSensitivity;
	camera_ry -= mouse_y * MouseSensitivity;

	//긌??띿궸귝귡뛀궖귩똶럁
	if( CheckInputControl(KEY_TURNUP, 0) ){ camera_ry += INPUT_ARROWKEYS_ANGLE; }			// 뷭?갌[겒]
	if( CheckInputControl(KEY_TURNDOWN, 0) ){ camera_ry -= INPUT_ARROWKEYS_ANGLE; }			// 뷭?갌[겓]
	if( CheckInputControl(KEY_TURNLEFT, 0) ){ camera_rx += INPUT_ARROWKEYS_ANGLE; }			// 뷭?갌[겑]
	if( CheckInputControl(KEY_TURNRIGHT, 0) ){ camera_rx -= INPUT_ARROWKEYS_ANGLE; }		// 뷭?갌[겏]

	if( camera_ry > DegreeToRadian(70) ) camera_ry = DegreeToRadian(70);
	if( camera_ry < DegreeToRadian(-70) ) camera_ry = DegreeToRadian(-70);

	//댷벍쀊뙂믦
	float dist = VIEW_FREECAMERA_SCALE;
	if( CheckInputControl(KEY_SHOT, 0) ){
		dist *= 2;
	}

	//긌??띿궸귝귟긇긽깋띆뷭귩똶럁
	if( CheckInputControl(KEY_MOVEFORWARD, 0) ){
		camera_x += cosf(camera_rx)*cosf(camera_ry)*dist;
		camera_y += sinf(camera_ry)*dist;
		camera_z += sinf(camera_rx)*cosf(camera_ry)*dist;
	}
	if( CheckInputControl(KEY_MOVEBACKWARD, 0) ){
		camera_x -= cosf(camera_rx)*cosf(camera_ry)*dist;
		camera_y -= sinf(camera_ry)*dist;
		camera_z -= sinf(camera_rx)*cosf(camera_ry)*dist;
	}
	if( CheckInputControl(KEY_MOVELEFT, 0) ){
		camera_x += cosf(camera_rx + (float)M_PI/2)*dist;
		camera_z += sinf(camera_rx + (float)M_PI/2)*dist;
	}
	if( CheckInputControl(KEY_MOVERIGHT, 0) ){
		camera_x += cosf(camera_rx - (float)M_PI/2)*dist;
		camera_z += sinf(camera_rx - (float)M_PI/2)*dist;
	}
}

void maingame::Process()
{
	human* h = NULL;
	HumanParameter HParam;

	int PlayerID = ObjMgr.GetPlayerID();

	for (int i = 0; i < MAX_HUMAN; i++) {
		h = ObjMgr.GetHumanObject(i);
		int idParam = -1;
		h->GetParamData(&idParam, NULL, NULL, NULL);
		if (GameParamInfo.GetHuman(idParam, &HParam) != 0) { continue; }

		int oldHP = h->GetHP();
		int max_hp = HParam.hp;

		if (h->GetEnableFlag() == false) { continue; }
		if (h->GetDeadFlag() == true) { continue; }
		if (oldHP <= 0) { continue; }
		if (oldHP >= max_hp) { continue; }

		if (oldHP < max_hp) {
			if (framecnt % 300 == 0) {
				int regenAmount = 0;
				if (i == PlayerID) {
					// 플레이어는 config 파일의 회복량 적용
					regenAmount = GameConfig.GetHealthRegenAmount();
				}
				else {
					// AI는 기존 랜덤 회복량 적용
					regenAmount = GetRand(20) + 1;
				}
				h->SetHP(oldHP + regenAmount);
			}
		}

		int NewHP = h->GetHP();
		if (NewHP > max_hp) {
			h->SetHP(max_hp);
		}
	}
	human* myHuman = ObjMgr.GetPlayerHumanObject();

	// 렌더링 FPS와 무관하게, 시각 효과 타이머는 로직 프레임에서만 감소시킨다.
	ProcessVisualTimers();

	ProcessPlayerSkill(myHuman);

	// 스킬 임시 무기를 표시해야 하는 동안에는 실제 무기 슬롯은 유지하고,
// 플레이어 1인칭 손 모션/무기 렌더링만 임시 스킬 무기 기준으로 바꾼다.
// 반드시 ObjMgr.Process() 전에 설정해야 손 모양도 바뀐다.
	if (myHuman != NULL) {
		if (IsPlayerTemporarySkillWeaponVisible() == true) {
			myHuman->SetRenderOverrideWeaponID(GetPlayerTemporarySkillWeaponID());
		}
		else if ((myHuman->GetSkillRobotExecuteFlag() == true) ||
			(player_skill_robot_fire_queue_count > 0)) {
			// 로봇 처형 프로토콜 중에는 실제 무기 슬롯/탄약/UI는 유지하고,
// 렌더링용 무기만 처형 프로토콜 전용 Mk14로 고정한다.
			myHuman->SetRenderOverrideWeaponID(ID_WEAPON_ROBOT_EXECUTE_RIFLE);
		}
		else {
			myHuman->SetRenderOverrideWeaponID(-1);
		}
	}

	float ontarget;
	int kill, headshot;

	time = GetTimeMS();

	int cmdF5id;
	if (Cmd_F5 == true) {
		cmdF5id = ObjMgr.GetPlayerID();
	}
	else {
		cmdF5id = -1;
	}
	ObjMgr.Process(cmdF5id, false, DarkScreenFlag);

	ObjMgr.GetHumanShotInfo(ObjMgr.GetPlayerID(), &ontarget, &kill, &headshot);
	MainGameInfo.ontarget += ontarget;
	MainGameInfo.kill += kill;
	MainGameInfo.headshot += headshot;

	// 총알이 명중했다면, 새로 만든 '히트마커' 타이머를 설정합니다.
	if (ontarget > 0.0f)
	{
		myHuman->SetHitMarkerTimer(8);	// 8프레임(약 0.24초) 동안 효과 표시

		// 만약 헤드샷이 포함되어 있다면, 헤드샷 플래그를 true로 설정합니다.
		if (headshot > 0) {
			myHuman->SetHeadshotFlag(true);
		}
	}

	time_process_object = GetTimeMS() - time;

	time = GetTimeMS();
	if (AIstop == false) {
		int PlayerID = ObjMgr.GetPlayerID();
		for (int i = 0; i < MAX_HUMAN; i++) {
			if ((PlayerAI == false) && (i == PlayerID)) { continue; }

			human* h = ObjMgr.GetHumanObject(i);
			if (h == NULL) { continue; }
			if (h->GetEnableFlag() == false) { continue; }
			if (h->GetHP() <= 0) { continue; }

			HumanAI[i].Process();
		}
	}
	time_process_ai = GetTimeMS() - time;

	if (end_framecnt == 0) {
		int check = ObjMgr.CheckGameOverorComplete();

		if (check == 1) {
			end_framecnt += 1;
			MainGameInfo.missioncomplete = true;
		}

		if (check == 2) {
			end_framecnt += 1;
			MainGameInfo.missioncomplete = false;
		}
	}

	time = GetTimeMS();
	if (EventStop == false) {
		for (int i = 0; i < TOTAL_EVENTLINE; i++) {
			if (end_framecnt == 0) {
				EventParameter EventParam;

				EventParam.endcnt = end_framecnt;
				EventParam.complete = MainGameInfo.missioncomplete;
				EventParam.MessageID = message_id;
				EventParam.SetMessageID = false;

				Event[i].ProcessEventPoint(EventSkip[i], &EventParam);

				end_framecnt = EventParam.endcnt;
				MainGameInfo.missioncomplete = EventParam.complete;
				message_id = EventParam.MessageID;

				EventSkip[i] = false;

				if (EventParam.SetMessageID == true) {
					message_cnt = 0;
				}
			}
		}
	}

	if ((message_id != -1) && (message_cnt < (int)(TOTAL_EVENTENT_SHOWMESSEC * GAMEFPS))) {
		message_cnt += 1;
	}
	else {
		message_id = -1;
		message_cnt = 0;
	}
	time_process_event = GetTimeMS() - time;

	float x, y, z;
	myHuman->GetPosData(&x, &y, &z, NULL);

	if (PlayerAI == true) {
		ObjMgr.GetPlayerHumanObject()->GetRxRy(&mouse_rx, &mouse_ry);
	}

	if (Camera_Debugmode == true) {
		// (디버그 카메라 로직)
	}
	else if (myHuman->GetDeadFlag() == true) {
		float camera_ry2 = camera_ry;

		camera_ry2 = AngleNormalization(camera_ry2);

		float ry = camera_ry2 * 0.95f + DegreeToRadian(-89.0f) * 0.05f;
		float r = 3.12f;

		camera_rx += DegreeToRadian(1.0f);
		camera_ry = ry;
		camera_x = x + cosf(camera_rx) * r;
		camera_y = y + 33.3f;
		camera_z = z + sinf(camera_rx) * r;
	}
	else if (myHuman->GetDeadMotionFlag() == true) {
		// (사망 모션 중 카메라 로직)
	}
	else if (Camera_F1mode == true) {
		float crx, cry;
		float ccx, ccy, ccz;

		// 144Hz 렌더링에서는 카메라를 별도 보간하지 않으므로,
		// F1 카메라 내부의 0.8/0.2 보간도 제거한다.
		// 전환 직후 기존 카메라 각도가 섞이면 팔과 화면이 늦게 따라오는 흔들림이 생긴다.
		crx = view_rx + mouse_rx * -1 + (float)M_PI / 2;
		cry = view_ry + mouse_ry - (float)M_PI / 2;
		ccx = x - cosf(crx) * cosf(cry) * 3.0f;
		ccy = y + HUMAN_HEIGHT - 0.5f + sinf(cry * -1) * 2.5f;
		ccz = z - sinf(crx) * cosf(cry) * 3.0f;

		cry += (float)M_PI / 2;
		float dist;
		if (CollD.CheckALLBlockIntersectRay(ccx, ccy, ccz, cosf(crx) * cosf(cry) * -1, sinf(cry * -1), sinf(crx) * cosf(cry) * -1, NULL, NULL, &dist, VIEW_F1MODE_DIST) == true) {
			dist -= 1.0f;
		}
		else {
			dist = VIEW_F1MODE_DIST;
		}

		camera_x = ccx - cosf(crx) * cosf(cry) * (dist);
		camera_y = ccy + sinf(cry * -1) * dist;
		camera_z = ccz - sinf(crx) * cosf(cry) * (dist);
		camera_rx = crx;
		camera_ry = cry;
	}
	else {
		// [✨ 최종 수정 ✨]
		// 1. 순수한 마우스 시야각은 maingame의 변수(mouse_rx, mouse_ry)에서 직접 가져옵니다.
		// 2. 반동 값은 플레이어 객체에서 GetRecoilYOffset() 함수로 직접 가져옵니다.
		float current_recoil_y = myHuman->GetRecoilYOffset();
		float current_recoil_x = myHuman->GetRecoilXOffset();

		// 3. 위 두 값을 더해서 최종 카메라 각도를 계산합니다.
		float crx = view_rx + mouse_rx * -1 + (float)M_PI / 2 + current_recoil_x;
		float cry = view_ry + mouse_ry + current_recoil_y;

	

		// 앉기 시점 보정
		float view_height = VIEW_HEIGHT;
		if (myHuman->GetCrouchFlag() == true) {
			view_height -= 2.0f;
		}

		// 계산된 최종 각도로 카메라 위치와 방향을 설정합니다.
		camera_x = x + cosf(crx) * cosf(cry) * VIEW_DIST;
		camera_y = y + view_height + sinf(cry) * VIEW_DIST;
		camera_z = z + sinf(crx) * cosf(cry) * VIEW_DIST;
		camera_rx = crx;
		camera_ry = cry;
	}

	float hit_rx = 0.0f;
	bool shield_hit = false;
	bool silent_hit = false;

	redflash_flag = myHuman->CheckHit(&hit_rx, &shield_hit, &silent_hit);
	shieldflash_flag = shield_hit;

	if ((redflash_flag == true) && (silent_hit == false)) {
		damage_arc_rx = hit_rx;
		damage_arc_timer = 18;
		damage_arc_shield_flag = shield_hit;
	}

#ifdef ENABLE_DEBUGCONSOLE
	if ((Show_Console == true) && (end_framecnt == 0)) {
		ProcessConsole();
	}
#endif

	framecnt += 1;
	if (start_framecnt < (int)(2.0f * GAMEFPS)) {
		start_framecnt += 1;
	}
	if (end_framecnt == 1) {
		MainGameInfo.framecnt = framecnt;
		memcpy(&GameInfoData, &MainGameInfo, sizeof(GameInfo));
		end_framecnt += 1;
	}
	else if (end_framecnt > 0) {
		if (end_framecnt < (int)(4.0f * GAMEFPS)) {
			end_framecnt += 1;
		}
		else {
			GameInfoData.fire = MainGameInfo.fire;
			GameInfoData.ontarget = MainGameInfo.ontarget;
			GameInfoData.kill = MainGameInfo.kill;
			GameInfoData.headshot = MainGameInfo.headshot;

			GameState->PushMouseButton();
			GameSpeed = 1;
		}
	}
}

void maingame::Sound()
{
	time = GetTimeMS();

	//긵깒귽깂?궻???붥뜂롦벦
	int teamid;
	human *myHuman = ObjMgr.GetPlayerHumanObject();
	myHuman->GetParamData(NULL, NULL, NULL, &teamid);

	//긖긂깛긤귩띋맯
	GameSound->PlayWorldSound(camera_x, camera_y, camera_z, camera_rx, teamid);

	time_sound = GetTimeMS() - time;
}

void maingame::Render3D()
{
	time = GetTimeMS();


	int skymodel, skytexture;
	human* myHuman = ObjMgr.GetPlayerHumanObject();

	// [✨ 추가] 요청하신 플레이어 좌표 및 각도 변수 선언 및 가져오기
	float px, py, pz, prx, pry;
	myHuman->GetPosData(&px, &py, &pz, &prx);
	myHuman->GetRxRy(&prx, &pry);

	// [✨ 추가] 최종 시야각(View Angle) 계산 로직
	// 1. 기본 시야각 설정 (기존 38도)
	float final_viewangle = DegreeToRadian(65.0f);
	int current_scope = myHuman->GetScopeMode();   // 현재 조준 상태 (0이면 비조준)
	static float smooth_zoom = 1.5f;
	static int prev_scope = 0;
	static int prev_weapon = -1;

	int current_weapon = myHuman->GetMainWeaponTypeNO();

	if ((Camera_F1mode == false) && (Camera_Debugmode == false)) {
		// [핵심] 조준 중일 때만(current_scope != 0) 확대 로직을 실행합니다.
		if (current_scope != 0) {
			if (current_scope == 7) {
				float target_zoom = myHuman->CustomZoom;

				if (target_zoom < 1.5f) target_zoom = 1.5f;
				if (target_zoom > 10.0f) target_zoom = 10.0f;

				// 실제 값도 보정해 둠
				myHuman->CustomZoom = target_zoom;

				// 조준에 막 진입했거나 무기가 바뀐 순간에는 보간하지 않고 즉시 맞춤
				// 이걸 안 하면 1프레임 정도 FOV가 튀면서 화면이 흔들릴 수 있음
				if (prev_scope == 0 || prev_weapon != current_weapon) {
					smooth_zoom = target_zoom;
				}
				else {
					smooth_zoom += (target_zoom - smooth_zoom) * 0.22f;
				}

				final_viewangle = DegreeToRadian(65.0f) / smooth_zoom;
			}
			else {
				smooth_zoom += (1.0f - smooth_zoom) * 0.25f;

				ScopeParameter sparam;
				GameParamInfo.GetScopeParam(current_scope, &sparam);
				final_viewangle = sparam.ViewAngle;
			}
		}
		// 조준 중이 아니면(current_scope == 0) 위에서 설정한 65도(기본값)가 유지됩니다.
	}
	else {
		final_viewangle = DegreeToRadian(65.0f);
	}

	prev_scope = current_scope;
	prev_weapon = current_weapon;

	// 현재 카메라 기준으로 월드와 HUD를 그린다.
	SyncDrawCamera();

	d3dg->SetCamera(camera_x, camera_y, camera_z, camera_rx, camera_ry, final_viewangle);

	//긲긅긐궴긇긽깋귩먠믦
	bool currentFog = FogFlag;

	if (myHuman->NightVision == true && myHuman->GetScopeMode() != 0) {
		currentFog = false;  // 야간 투시경이 켜져 있으면 안개를 끔
	}

	// 좀비 격노 중에는 시야가 트이는 느낌을 주기 위해 안개를 끈다.
	if (myHuman->GetSkillRageFlag() == true) {
		currentFog = false;
	}

	// 요원A 마킹 샷: 1초 준비가 끝나고 실제 마킹이 유지되는 동안 안개를 끈다.
	if (player_skill_mark_timer > 0) {
		currentFog = false;
	}

	// 로봇 F: 에임핵 중에는 시야 확보를 위해 안개를 제거한다.
	if (player_skill_aimhack_timer > 0) {
		currentFog = false;
	}

	d3dg->SetFog(currentFog, SkyNumber);
	if ((Camera_F1mode == false) && (Camera_Debugmode == false)) {
		ScopeParameter sparam;
		int scopemode = myHuman->GetScopeMode();
		GameParamInfo.GetScopeParam(scopemode, &sparam);

	}
	else {
		ScopeParameter sparam;
		GameParamInfo.GetScopeParam(0, &sparam);

	}

	//긇긽깋띆뷭궸봶똧뗴귩?됪
	d3dg->SetWorldTransform(camera_x, camera_y, camera_z, 0.0f, 0.0f, 2.0f);
	Resource.GetSkyModelTexture(&skymodel, &skytexture);
	d3dg->RenderModel(skymodel, skytexture, DarkScreenFlag, false, false);

	//Z긫긞긲?귩룊딖돸
	d3dg->ResetZbuffer();

	if (CenterLine == true) {
		//뭷륲멄?렑걁긢긫긞긏뾭걂
		d3dg->RenderCenterline();
	}

	//?긞긵귩?됪
	d3dg->ResetWorldTransform();
	d3dg->RenderMapdata(wireframe);

	//긵깒귽깂?궻?됪뾎뼰궻뙂믦
	int DrawPlayer = 0;
	if ((Camera_F1mode == false) && (Camera_Debugmode == false) && (myHuman->GetHP() > 0)) {
		if (Camera_HOMEmode == false) {
			DrawPlayer = 1;
		}
		else {
			DrawPlayer = 2;
		}
	}



	//긆긳긙긃긏긣귩?됪
	// 오브젝트 보간도 전체 비활성화한다.
	ObjMgr.Render(camera_x, camera_y, camera_z, camera_rx, camera_ry, DrawPlayer, nomodel);

	// 시민A 응급 치료 성공 이펙트 표시
	RenderPlayerHealEffect(myHuman);

	// 요원B 동료 소환 성공 이펙트 표시
	RenderPlayerSummonEffect();

	// 요원B 생체장 지속 이펙트 표시
	RenderPlayerBioticField();

	// 대장 지점 지정 폭격 지속 범위 표시
	RenderPlayerArtilleryStrike();

	// 시민B 생존 본능 성공 이펙트 표시
	RenderPlayerSurvivalEffect(myHuman);

	// 시민B 긴급 회피 잔상 표시
	RenderPlayerDodgeTrail();

	// 군인 전투 강화 지속 이펙트 표시
	RenderPlayerBattleBoostEffect(myHuman);

	// 용병 전투 흡수 지속 이펙트 표시
	RenderPlayerBattleAbsorbEffect(myHuman);


	// 좀비 격노 오라 표시
	RenderPlayerRageAura(myHuman);

	// 대장 전술 스캔 핑 표시
	RenderTacticalScanPings(myHuman);

	// 요원A 마킹 샷 핑 표시
	RenderMarkedShotPing(myHuman);

	// 요원A 그림자 잔상에 반응한 적 핑 표시
	RenderShadowDecoyPings(myHuman);

	// 로봇 처형 프로토콜 마커 표시
	RenderRobotExecutePings(myHuman);

	// 경찰 제압 명령 표시
	RenderSuppressedEnemyPings(myHuman);

	// 좀비 갈고리 표시
	RenderPlayerHookSkill(myHuman);

	// 교란 신호 지속 파티클 표시
	RenderDistractionSignalEffect();

	// 교란 신호 발동 위치 표시
	RenderDistractionSignalMarker();

	// 테러리스트 지뢰 설치 위치 표시
	RenderPlayerTerroristMines();

	// 교란 신호 / 동료 소환 / 텔레포트 조준 미리보기
	RenderPlayerSkillTargetPreview(myHuman);
	// 비밀 요원 텔레포트 시전 중 지정 위치 표시
	RenderPlayerTeleportCastPreview();

	// 비밀 요원 텔레포트 시작/도착 이펙트
	RenderPlayerTeleportEffect();

	// 플레이어 수류탄 궤도 표시
	RenderGrenadeTrajectory();

	//AI긢긫긞긏륃뺪?렑
	if (AIdebuginfoID != -1) {
		if ((0 <= AIdebuginfoID) && (AIdebuginfoID < MAX_HUMAN)) {
			float posx, posy, posz, rx;
			int EnemyID;
			float mposx, mposz;
			int movemode;
			ObjMgr.GetHumanObject(AIdebuginfoID)->GetPosData(&posx, &posy, &posz, &rx);
			EnemyID = HumanAI[AIdebuginfoID].GetEnemyHumanID();
			HumanAI[AIdebuginfoID].GetMoveTargetPos(&mposx, &mposz, &movemode);

			d3dg->ResetWorldTransform();

			//?릐
			d3dg->Renderline(posx + 10.0f, posy, posz, posx - 10.0f, posy, posz, d3dg->GetColorCode(0.0f, 0.0f, 1.0f, 1.0f));
			d3dg->Renderline(posx, posy + 10.0f, posz, posx, posy - 10.0f, posz, d3dg->GetColorCode(0.0f, 0.0f, 1.0f, 1.0f));
			d3dg->Renderline(posx, posy, posz + 10.0f, posx, posy, posz - 10.0f, d3dg->GetColorCode(0.0f, 0.0f, 1.0f, 1.0f));

			//댷벍먩
			d3dg->Renderline(mposx + 10.0f, posy, mposz, mposx - 10.0f, posy, mposz, d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f));
			d3dg->Renderline(mposx, 5000.0f, mposz, mposx, -500.0f, mposz, d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f));
			d3dg->Renderline(mposx, posy, mposz + 10.0f, mposx, posy, mposz - 10.0f, d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f));

			if (EnemyID != -1) {
				ObjMgr.GetHumanObject(EnemyID)->GetPosData(&posx, &posy, &posz, &rx);

				//뛘똼뫮뤭
				d3dg->Renderline(posx + 3.0f, posy, posz + 3.0f, posx + 3.0f, posy, posz - 3.0f, d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f));
				d3dg->Renderline(posx + 3.0f, posy, posz - 3.0f, posx - 3.0f, posy, posz - 3.0f, d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f));
				d3dg->Renderline(posx - 3.0f, posy, posz - 3.0f, posx - 3.0f, posy, posz + 3.0f, d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f));
				d3dg->Renderline(posx - 3.0f, posy, posz + 3.0f, posx + 3.0f, posy, posz + 3.0f, d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f));
				d3dg->Renderline(posx + 3.0f, posy + HUMAN_HEIGHT, posz + 3.0f, posx + 3.0f, posy + HUMAN_HEIGHT, posz - 3.0f, d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f));
				d3dg->Renderline(posx + 3.0f, posy + HUMAN_HEIGHT, posz - 3.0f, posx - 3.0f, posy + HUMAN_HEIGHT, posz - 3.0f, d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f));
				d3dg->Renderline(posx - 3.0f, posy + HUMAN_HEIGHT, posz - 3.0f, posx - 3.0f, posy + HUMAN_HEIGHT, posz + 3.0f, d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f));
				d3dg->Renderline(posx - 3.0f, posy + HUMAN_HEIGHT, posz + 3.0f, posx + 3.0f, posy + HUMAN_HEIGHT, posz + 3.0f, d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f));
				d3dg->Renderline(posx + 3.0f, posy, posz + 3.0f, posx + 3.0f, posy + HUMAN_HEIGHT, posz + 3.0f, d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f));
				d3dg->Renderline(posx + 3.0f, posy, posz - 3.0f, posx + 3.0f, posy + HUMAN_HEIGHT, posz - 3.0f, d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f));
				d3dg->Renderline(posx - 3.0f, posy, posz - 3.0f, posx - 3.0f, posy + HUMAN_HEIGHT, posz - 3.0f, d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f));
				d3dg->Renderline(posx - 3.0f, posy, posz + 3.0f, posx - 3.0f, posy + HUMAN_HEIGHT, posz + 3.0f, d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f));
			}
		}
	}


	if (player_skill_aimhack_timer > 0) {
		// 로봇 F: 에임핵 화면 효과
		// 노란색/금색 계열로 타겟 분석 모드 느낌을 준다.
		// 너무 밝으면 +1을 제거하거나 Ambient 값을 낮추면 된다.
		d3dg->ScreenBrightness(GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight(), GameConfig.GetBrightness() + 1);
		d3dg->SetAmbientLight(0x00332200);
	}
	else if (myHuman->NightVision == true && myHuman->GetScopeMode() != 0) {
		d3dg->ScreenBrightness(GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight(), GameConfig.GetBrightness() + 2);
		d3dg->SetAmbientLight(0x0022FF22);
	}
	else {
		d3dg->ScreenBrightness(GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight(), GameConfig.GetBrightness());
		d3dg->SetAmbientLight(0x00000000);
	}


}

const char* maingame::GetControlGuideKeyName(int key_id)
{
	return GameConfig.GetOriginalkeycodeString(GameConfig.GetKeycode(key_id));
}

bool maingame::CanWeaponUseFireModeGuide(int weapon_id)
{
	if (weapon_id == ID_WEAPON_NONE) { return false; }
	if (weapon_id < 0) { return false; }

	WeaponParameter wp;
	if (GameParamInfo.GetWeapon(weapon_id, &wp) != 0) {
		return false;
	}

	if (wp.ChangeWeaponID == -1) {
		return false;
	}

	if (wp.ChangeWeaponID == weapon_id) {
		return false;
	}

	// 대상 무기 ID가 실제로 존재하는지도 확인한다.
	WeaponParameter next_wp;
	if (GameParamInfo.GetWeapon(wp.ChangeWeaponID, &next_wp) != 0) {
		return false;
	}

	return true;
}

bool maingame::CanPlayerUseNightVisionGuide(human* myHuman)
{
	if (myHuman == NULL) { return false; }
	if (myHuman->GetScopeMode() == 0) { return false; }

	int weapon_id = myHuman->GetMainWeaponTypeNO();

	WeaponParameter wp;
	if (GameParamInfo.GetWeapon(weapon_id, &wp) != 0) {
		return false;
	}

	if ((wp.scopemode == 4) ||
		(wp.scopemode == 5) ||
		(wp.scopemode == 6) ||
		(wp.scopemode == 7)) {
		return true;
	}

	return false;
}

bool maingame::CanPlayerUseVariableZoomGuide(human* myHuman)
{
	if (myHuman == NULL) { return false; }
	if (myHuman->GetScopeMode() == 0) { return false; }

	int weapon_id = myHuman->GetMainWeaponTypeNO();

	WeaponParameter wp;
	if (GameParamInfo.GetWeapon(weapon_id, &wp) != 0) {
		return false;
	}

	return (wp.scopemode == 7);
}

void maingame::DrawControlGuideLine(int x, int y, const char* action, const char* key, int action_color, int key_color)
{
	char key_text[64];

	if (action == NULL) { action = ""; }
	if (key == NULL) { key = ""; }

	sprintf(key_text, "[%s]", key);

	// 기능명 위치는 고정
	int action_x = x;

	// 키 위치도 고정
	// 이렇게 해야 SPACE, CTRL, MOUSE WHEEL처럼 글자 수가 달라도 흔들리지 않는다.
	int key_x = x + 118;

	int shadow_color = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f);

	// 그림자
	d3dg->Draw2DTextureDebugFontText(action_x + 1, y + 1, action, shadow_color);
	d3dg->Draw2DTextureDebugFontText(key_x + 1, y + 1, key_text, shadow_color);

	// 본문
	d3dg->Draw2DTextureDebugFontText(action_x, y, action, action_color);
	d3dg->Draw2DTextureDebugFontText(key_x, y, key_text, key_color);
}

void maingame::RenderPlayerControlGuide(human* myHuman, int display_weapon_id)
{
	if (myHuman == NULL) { return; }
	if (ShowFullMap == true) { return; }
	if (Camera_Debugmode == true) { return; }
	if (Camera_F1mode == true) { return; }

	struct ControlGuideSlot {
		const char* action;
		const char* key;
		bool visible;
		int action_color;
		int key_color;
	};

	const int GUIDE_SLOT_MAX = 12;

	ControlGuideSlot slot[GUIDE_SLOT_MAX];

	int normal_action_color = d3dg->GetColorCode(0.82f, 0.82f, 0.82f, 0.88f);
	int normal_key_color = d3dg->GetColorCode(1.00f, 1.00f, 1.00f, 0.95f);

	// 모드 변경은 다른 기본 조작보다 눈에 띄게 한다.
	int firemode_action_color = d3dg->GetColorCode(1.00f, 0.72f, 0.25f, 0.95f);
	int firemode_key_color = d3dg->GetColorCode(1.00f, 0.88f, 0.45f, 1.00f);

	// 상황에 따라 나타나는 조작
	int condition_action_color = d3dg->GetColorCode(0.45f, 0.90f, 1.00f, 0.92f);
	int condition_key_color = d3dg->GetColorCode(0.70f, 1.00f, 1.00f, 1.00f);

	for (int i = 0; i < GUIDE_SLOT_MAX; i++) {
		slot[i].action = "";
		slot[i].key = "";
		slot[i].visible = false;
		slot[i].action_color = normal_action_color;
		slot[i].key_color = normal_key_color;
	}

	// ------------------------------------------------------------
	// 고정 슬롯 배치
	// ------------------------------------------------------------
	// 0 : JUMP
	// 1 : WALK
	// 2 : RUN
	// 3 : CROUCH
	// 4 : RELOAD
	// 5 : MAP
	// 6 : FIRE MODE
	// 7 : TRAJECTORY
	// 8 : CONFIRM
	// 9 : CANCEL
	// 10: NIGHT VISION
	// 11: ZOOM
	//
	// 조건이 안 맞으면 해당 줄만 비우고, 다른 줄 위치는 절대 당기지 않는다.
	// ------------------------------------------------------------

	slot[0].action = "JUMP";
	slot[0].key = GetControlGuideKeyName(KEY_JUMP);
	slot[0].visible = true;

	slot[1].action = "WALK";
	slot[1].key = GetControlGuideKeyName(KEY_WALK);
	slot[1].visible = true;

	slot[2].action = "RUN";
	slot[2].key = GetControlGuideKeyName(KEY_RUN);
	slot[2].visible = true;

	slot[3].action = "CROUCH";
	slot[3].key = GetControlGuideKeyName(KEY_CROUCH);
	slot[3].visible = true;

	slot[4].action = "RELOAD";
	slot[4].key = GetControlGuideKeyName(KEY_RELOAD);
	slot[4].visible = true;

	if (GameConfig.GetRadarEnabledFlag() == true) {
		slot[5].action = "MAP";
		slot[5].key = GetControlGuideKeyName(KEY_MAP);
		slot[5].visible = true;
	}

	// 모드 변경 가능한 총기에서만 표시.
	// 단, 위치는 항상 6번 슬롯으로 고정한다.
	if (CanWeaponUseFireModeGuide(display_weapon_id) == true) {
		slot[6].action = "FIRE MODE";
		slot[6].key = GetControlGuideKeyName(KEY_SHOTMODE);
		slot[6].visible = true;
		slot[6].action_color = firemode_action_color;
		slot[6].key_color = firemode_key_color;
	}

	// 수류탄 궤도 표시
	if (GetPlayerCurrentGrenadeTrajectoryWeaponID(myHuman) != ID_WEAPON_NONE) {
		slot[7].action = "TRAJECTORY";
		slot[7].key = GetControlGuideKeyName(KEY_ZOOMSCOPE);
		slot[7].visible = true;
		slot[7].action_color = condition_action_color;
		slot[7].key_color = condition_key_color;
	}

	// 스킬 조준 중 확정/취소
	if (IsPlayerSkillTargeting() == true) {
		slot[8].action = "CONFIRM";
		slot[8].key = GetControlGuideKeyName(KEY_SHOT);
		slot[8].visible = true;
		slot[8].action_color = condition_action_color;
		slot[8].key_color = condition_key_color;

		slot[9].action = "CANCEL";
		slot[9].key = GetControlGuideKeyName(KEY_ZOOMSCOPE);
		slot[9].visible = true;
		slot[9].action_color = condition_action_color;
		slot[9].key_color = condition_key_color;
	}

	// 야간투시 가능 총기 + 조준 중일 때만 표시
	if (CanPlayerUseNightVisionGuide(myHuman) == true) {
		slot[10].action = "NIGHT VISION";
		slot[10].key = GetControlGuideKeyName(KEY_NIGHTVISION);
		slot[10].visible = true;
		slot[10].action_color = condition_action_color;
		slot[10].key_color = condition_key_color;
	}

	// 가변 줌 가능 총기 + 조준 중일 때만 표시
	if (CanPlayerUseVariableZoomGuide(myHuman) == true) {
		slot[11].action = "ZOOM";
		slot[11].key = "MOUSE WHEEL";
		slot[11].visible = true;
		slot[11].action_color = condition_action_color;
		slot[11].key_color = condition_key_color;
	}

	// ------------------------------------------------------------
// 기본 조작창은 항상 고정 위치/고정 크기로 출력한다.
// 조건부 조작은 기본 조작창 위쪽으로만 확장한다.
// ------------------------------------------------------------

	int line_h = 14;
	int padding = 6;

	int box_w = 230;

	int x = HUDA_WEAPON_POSX + 4;

	// 기본 조작 슬롯
	// 0 : JUMP
	// 1 : WALK
	// 2 : RUN
	// 3 : CROUCH
	// 4 : RELOAD
	// 5 : MAP
	const int BASE_SLOT_BEGIN = 0;
	const int BASE_SLOT_END = 5;
	const int BASE_LINE_COUNT = 6;

	// 조건부 조작 슬롯
	// 6 : FIRE MODE
	// 7 : TRAJECTORY
	// 8 : CONFIRM
	// 9 : CANCEL
	// 10: NIGHT VISION
	// 11: ZOOM
	const int CONDITION_SLOT_BEGIN = 6;
	const int CONDITION_SLOT_END = GUIDE_SLOT_MAX - 1;

	int condition_count = 0;

	for (int i = CONDITION_SLOT_BEGIN; i <= CONDITION_SLOT_END; i++) {
		if (slot[i].visible == true) {
			condition_count++;
		}
	}

	int bg_color = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.38f);
	int border_color = d3dg->GetColorCode(0.75f, 0.75f, 0.75f, 0.38f);

	// ------------------------------------------------------------
	// 1. 기본 조작창 위치 계산
	//    조건부 조작이 생겨도 이 위치는 변하지 않는다.
	// ------------------------------------------------------------

	int base_box_h = BASE_LINE_COUNT * line_h + padding * 2;
	int base_y = HUDA_WEAPON_POSY - base_box_h - 8;

	if (base_y < 8) {
		base_y = 8;
	}

	// ------------------------------------------------------------
	// 2. 조건부 조작창 출력
	//    조건부 조작은 기본 조작창 위쪽으로만 확장된다.
	// ------------------------------------------------------------

	if (condition_count > 0) {
		int condition_box_h = condition_count * line_h + padding * 2;
		int condition_y = base_y - condition_box_h - 2;

		if (condition_y < 8) {
			condition_y = 8;
		}

		d3dg->Draw2DBox(x, condition_y, x + box_w, condition_y + condition_box_h, bg_color);

		d3dg->Draw2DBox(x, condition_y, x + box_w, condition_y + 1, border_color);
		d3dg->Draw2DBox(x, condition_y + condition_box_h - 1, x + box_w, condition_y + condition_box_h, border_color);
		d3dg->Draw2DBox(x, condition_y, x + 1, condition_y + condition_box_h, border_color);
		d3dg->Draw2DBox(x + box_w - 1, condition_y, x + box_w, condition_y + condition_box_h, border_color);

		int draw_line = 0;

		for (int i = CONDITION_SLOT_BEGIN; i <= CONDITION_SLOT_END; i++) {
			if (slot[i].visible == false) {
				continue;
			}

			DrawControlGuideLine(
				x + padding,
				condition_y + padding + draw_line * line_h,
				slot[i].action,
				slot[i].key,
				slot[i].action_color,
				slot[i].key_color
			);

			draw_line++;
		}
	}

	// ------------------------------------------------------------
	// 3. 기본 조작창 출력
	//    항상 같은 크기와 같은 위치에 유지된다.
	// ------------------------------------------------------------

	d3dg->Draw2DBox(x, base_y, x + box_w, base_y + base_box_h, bg_color);

	d3dg->Draw2DBox(x, base_y, x + box_w, base_y + 1, border_color);
	d3dg->Draw2DBox(x, base_y + base_box_h - 1, x + box_w, base_y + base_box_h, border_color);
	d3dg->Draw2DBox(x, base_y, x + 1, base_y + base_box_h, border_color);
	d3dg->Draw2DBox(x + box_w - 1, base_y, x + box_w, base_y + base_box_h, border_color);

	for (int i = BASE_SLOT_BEGIN; i <= BASE_SLOT_END; i++) {
		if (slot[i].visible == false) {
			continue;
		}

		DrawControlGuideLine(
			x + padding,
			base_y + padding + i * line_h,
			slot[i].action,
			slot[i].key,
			slot[i].action_color,
			slot[i].key_color
		);
	}
}

void maingame::RenderMapControlGuide(int map_x, int map_y, int map_size)
{
	if (ShowFullMap == false) { return; }

	int line_h = 14;
	int padding = 6;
	int box_w = 230;
	int box_h = line_h * 2 + padding * 2;

	int x = map_x + 12;
	int y = map_y + 12;

	int bg_color = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.55f);
	int border_color = d3dg->GetColorCode(0.55f, 0.95f, 1.00f, 0.60f);
	int text_color = d3dg->GetColorCode(0.55f, 0.95f, 1.00f, 0.95f);

	d3dg->Draw2DBox(x, y, x + box_w, y + box_h, bg_color);

	d3dg->Draw2DBox(x, y, x + box_w, y + 1, border_color);
	d3dg->Draw2DBox(x, y + box_h - 1, x + box_w, y + box_h, border_color);
	d3dg->Draw2DBox(x, y, x + 1, y + box_h, border_color);
	d3dg->Draw2DBox(x + box_w - 1, y, x + box_w, y + box_h, border_color);

	DrawControlGuideLine(
		x + padding,
		y + padding,
		"CLOSE MAP",
		GetControlGuideKeyName(KEY_MAP),
		text_color,
		text_color
	);

	DrawControlGuideLine(
		x + padding,
		y + padding + line_h,
		"MAP ZOOM",
		"MOUSE WHEEL",
		text_color,
		text_color
	);
}

const char* maingame::GetPlayerInstallSkillCancelKeyName()
{
	int install_slot = PLAYER_SKILL_SLOT_PRIMARY;
	bool found = false;

	if (player_skill_distraction_install_pending == true) {
		install_slot = player_skill_distraction_install_slot;
		found = true;
	}
	else if (player_skill_terrorist_mine_install_pending == true) {
		install_slot = player_skill_terrorist_mine_install_slot;
		found = true;
	}
	else if (player_skill_summon_install_pending == true) {
		install_slot = player_skill_summon_install_slot;
		found = true;
	}
	else if (player_skill_shadow_decoy_install_pending == true) {
		install_slot = player_skill_shadow_decoy_install_slot;
		found = true;
	}
	else if (player_skill_biotic_field_install_pending == true) {
		install_slot = player_skill_biotic_field_install_slot;
		found = true;
	}
	else if (player_skill_artillery_strike_install_pending == true) {
		install_slot = player_skill_artillery_strike_install_slot;
		found = true;
	}

	if (found == false) {
		return "";
	}

	if (install_slot == PLAYER_SKILL_SLOT_SECONDARY) {
		return GetControlGuideKeyName(KEY_SKILL2);
	}

	return GetControlGuideKeyName(KEY_SKILL);
}

void maingame::RenderInstallSkillCancelGuide()
{
	if (IsPlayerInstallSkillPending() == false) { return; }
	if (ShowFullMap == true) { return; }
	if (Camera_Debugmode == true) { return; }
	if (Camera_F1mode == true) { return; }

	int sw = GameConfig.GetScreenWidth();
	int sh = GameConfig.GetScreenHeight();

	const char* action_text = "CANCEL";
	const char* key_name = GetPlayerInstallSkillCancelKeyName();

	if ((key_name == NULL) || (key_name[0] == '\0')) {
		return;
	}

	char key_text[64];
	sprintf(key_text, "[%s]", key_name);

	// 중앙 살짝 아래.
	// 너무 아래로 내리면 무기 HUD와 가까워지고,
	// 너무 위로 올리면 조준점과 겹치므로 화면 중앙 + 64 정도가 적당하다.
	int box_w = 280;
	int box_h = 44;
	int box_x = sw / 2 - box_w / 2;
	int box_y = sh / 2 + 256;

	int bg_color = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.55f);
	int border_color = d3dg->GetColorCode(0.70f, 1.00f, 1.00f, 0.75f);

	int action_color = d3dg->GetColorCode(0.45f, 0.90f, 1.00f, 1.00f);
	int key_color = d3dg->GetColorCode(1.00f, 1.00f, 1.00f, 1.00f);
	int shadow_color = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f);

	d3dg->Draw2DBox(box_x, box_y, box_x + box_w, box_y + box_h, bg_color);

	d3dg->Draw2DBox(box_x, box_y, box_x + box_w, box_y + 1, border_color);
	d3dg->Draw2DBox(box_x, box_y + box_h - 1, box_x + box_w, box_y + box_h, border_color);
	d3dg->Draw2DBox(box_x, box_y, box_x + 1, box_y + box_h, border_color);
	d3dg->Draw2DBox(box_x + box_w - 1, box_y, box_x + box_w, box_y + box_h, border_color);

	// 기능명과 키를 분리해서 그린다.
	// 키는 []로 감싸서 "눌러야 하는 버튼"이라는 느낌을 준다.
	int action_fw = 16;
	int action_fh = 20;
	int key_fw = 14;
	int key_fh = 20;
	int gap = 16;

	int action_w = (int)strlen(action_text) * action_fw;
	int key_w = (int)strlen(key_text) * key_fw;
	int total_w = action_w + gap + key_w;

	int text_x = sw / 2 - total_w / 2;
	int text_y = box_y + 12;

	int action_x = text_x;
	int key_x = action_x + action_w + gap;

	// 그림자
	d3dg->Draw2DTextureFontText(
		action_x + 1,
		text_y + 1,
		action_text,
		shadow_color,
		action_fw,
		action_fh
	);

	d3dg->Draw2DTextureFontText(
		key_x + 1,
		text_y + 1,
		key_text,
		shadow_color,
		key_fw,
		key_fh
	);

	// 본문
	d3dg->Draw2DTextureFontText(
		action_x,
		text_y,
		action_text,
		action_color,
		action_fw,
		action_fh
	);

	d3dg->Draw2DTextureFontText(
		key_x,
		text_y,
		key_text,
		key_color,
		key_fw,
		key_fh
	);
}

void maingame::Render2D()
{
	char str[256];
	unsigned char stru[256];
	float fps = GetFps(60);
	float effect;

	human* myHuman = ObjMgr.GetPlayerHumanObject();

	// 로봇 처형 프로토콜 화면 효과
	// 월드 렌더링 이후, HUD/조준선보다 먼저 그리는 것이 좋다.
	RenderRobotExecuteScreenEffect(myHuman);

	// 플레이어 오브젝트가 없으면 이후 HUD/조준경/무기/체력 표시를 처리할 수 없다.
	if (myHuman == NULL) {
		return;
	}

	// 조준 중일 때만 배율 표시
	if ((Camera_F1mode == false) && (Camera_Debugmode == false) && (myHuman->GetScopeMode() != 0)) {
		WeaponParameter wp;
		if (GameParamInfo.GetWeapon(myHuman->GetMainWeaponTypeNO(), &wp) == 0 && wp.scopemode == 7) {

			// ------------------------------------------------------------
// 가변 배율 스코프 UI
// - 작은 반투명 패널
// - 배율 숫자
// - 배율 단계 바
// - 조준점 기준 오른쪽 살짝 아래 배치
// - 내부 요소가 박스 밖으로 나가지 않도록 박스 기준 배치
// ------------------------------------------------------------
			char zoom_text[32];
			sprintf(zoom_text, "%.1fx", myHuman->CustomZoom);

			int sw = GameConfig.GetScreenWidth();
			int sh = GameConfig.GetScreenHeight();

			float ui_scale = (float)sh / 720.0f;
			if (ui_scale < 1.0f) ui_scale = 1.0f;
			if (ui_scale > 1.35f) ui_scale = 1.35f;

			// UI 크기
			int boxW = (int)(120 * ui_scale);
			int boxH = (int)(36 * ui_scale);

			if (boxW < 105) boxW = 105;
			if (boxH < 32) boxH = 32;
			if (boxW > 140) boxW = 140;
			if (boxH > 42) boxH = 42;

			// 조준점 기준 오른쪽 살짝 아래
			int centerX = sw / 2;
			int centerY = sh / 2;

			int posX = centerX + (int)(70 * ui_scale);
			int posY = centerY + (int)(80 * ui_scale);

			// 색상
			int bg_color = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.55f);
			int border_color = d3dg->GetColorCode(0.55f, 0.55f, 0.55f, 0.70f);
			int text_color = d3dg->GetColorCode(0.90f, 0.90f, 0.90f, 0.95f);
			int bar_on = d3dg->GetColorCode(0.75f, 0.75f, 0.75f, 0.85f);
			int bar_off = d3dg->GetColorCode(0.18f, 0.18f, 0.18f, 0.60f);

			// 배경
			d3dg->Draw2DBox(posX, posY, posX + boxW, posY + boxH, bg_color);

			// 테두리
			d3dg->Draw2DBox(posX, posY, posX + boxW, posY + 1, border_color);
			d3dg->Draw2DBox(posX, posY + boxH - 1, posX + boxW, posY + boxH, border_color);
			d3dg->Draw2DBox(posX, posY, posX + 1, posY + boxH, border_color);
			d3dg->Draw2DBox(posX + boxW - 1, posY, posX + boxW, posY + boxH, border_color);

			// ------------------------------------------------------------
			// 텍스트
			// ------------------------------------------------------------
			int padding = (int)(6 * ui_scale);
			if (padding < 5) padding = 5;

			int textY = posY + (int)(5 * ui_scale);
			if (textY < posY + 6) textY = posY + 6;

			int labelX = posX + padding;

			// 숫자는 박스 오른쪽 안쪽에 고정
			int zoomTextW = (int)(36 * ui_scale);
			if (zoomTextW < 34) zoomTextW = 34;

			int zoomX = posX + boxW - padding - zoomTextW;

			d3dg->Draw2DTextureDebugFontText(labelX, textY, "ZOOM", text_color);
			d3dg->Draw2DTextureDebugFontText(zoomX, textY, zoom_text, text_color);

			// ------------------------------------------------------------
			// 배율 단계 바
			// ------------------------------------------------------------
			float min_zoom = 1.5f;
			float max_zoom = 10.0f;

			// CustomZoom 최대값을 6.0f로 제한했다면 아래를 6.0f로 바꾸기
			// float max_zoom = 6.0f;

			float zoom_rate = (myHuman->CustomZoom - min_zoom) / (max_zoom - min_zoom);

			if (zoom_rate < 0.0f) zoom_rate = 0.0f;
			if (zoom_rate > 1.0f) zoom_rate = 1.0f;

			// ------------------------------------------------------------
// 배율 단계 바: 10칸
// ------------------------------------------------------------
			int totalBars = 10;
			int filledBars = (int)(zoom_rate * totalBars + 0.5f);

			int barAreaX = posX + padding;
			int barAreaY = posY + boxH - (int)(12 * ui_scale);

			int barAreaW = boxW - padding * 2;

			// 10칸이므로 간격은 작게 잡는 것이 좋음
			int barGap = (int)(2 * ui_scale);
			if (barGap < 1) barGap = 1;
			if (barGap > 3) barGap = 3;

			// 10개 바와 9개 간격이 박스 안에 들어가도록 자동 계산
			int barW = (barAreaW - barGap * (totalBars - 1)) / totalBars;
			int barH = (int)(4 * ui_scale);

			if (barW < 4) barW = 4;
			if (barH < 3) barH = 3;
			if (barH > 5) barH = 5;

			for (int i = 0; i < totalBars; i++) {
				int x1 = barAreaX + i * (barW + barGap);
				int y1 = barAreaY;
				int x2 = x1 + barW;
				int y2 = y1 + barH;

				if (i < filledBars) {
					d3dg->Draw2DBox(x1, y1, x2, y2, bar_on);
				}
				else {
					d3dg->Draw2DBox(x1, y1, x2, y2, bar_off);
				}
			}
		}
	}

	int selectweapon;
	weapon* weapon[TOTAL_HAVEWEAPON];
	int weapon_paramid[TOTAL_HAVEWEAPON];
	int lnbslist[TOTAL_HAVEWEAPON];
	int nbslist[TOTAL_HAVEWEAPON];
	WeaponParameter weapon_paramdata;
	for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
		weapon[i] = NULL;
		weapon_paramid[i] = 0;
		lnbslist[i] = 0;
		nbslist[i] = 0;
	}
	int lnbs = 0;
	int nbs = 0;
	int reloadcnt = 0;
	int total_reloadcnt = 0; // 전체 재장전 시간 변수 추가
	int selectweaponcnt = 0;
	int changeweaponidcnt = 0;
	int changeweaponid_total_time = 0;
	int weaponmodel, weapontexture;
	char weaponname[24 + 1];
	int hp;
	int param_scopemode;
	int param_WeaponP;
	int param_pellet;
	int ErrorRange;

	//各種設定やゲ??情報を取得
	myHuman->GetWeapon(&selectweapon, weapon, lnbslist, nbslist);
	for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
		if (selectweapon == i) {
			if (weapon[i] != NULL) {
				weapon[i]->GetParamData(&weapon_paramid[i], &lnbs, &nbs);
			}
			else {
				lnbs = lnbslist[i];
				nbs = nbslist[i];
			}
		}
		else {
			if (weapon[i] != NULL) {
				weapon[i]->GetParamData(&weapon_paramid[i], NULL, NULL);
			}
		}
	}
	changeweaponidcnt = myHuman->GetChangeWeaponIDCnt();
	changeweaponid_total_time = myHuman->GetChangeWeaponIDTotalTime();
	reloadcnt = myHuman->GetWeaponReloadCnt();
	total_reloadcnt = myHuman->GetTotalWeaponReloadCnt(); // 전체 재장전 시간 가져오기
	selectweaponcnt = myHuman->GetChangeHaveWeaponCnt() + myHuman->GetChangeWeaponIDCnt();

	if (IsPlayerTemporarySkillWeaponChanging() == true) {
		int temp_weapon_change_timer = GetPlayerTemporarySkillWeaponChangeTimer();

		if (temp_weapon_change_timer > selectweaponcnt) {
			selectweaponcnt = temp_weapon_change_timer;
		}
	}

	int display_hud_weapon_id = weapon_paramid[selectweapon];
	bool robot_execute_hud_active = false;

	// 스킬 임시 무기를 들고 있거나 꺼내는 중이면,
	// 실제 무기 슬롯은 유지하고 HUD 표시만 임시 스킬 무기로 바꾼다.
	if ((IsPlayerTemporarySkillWeaponReady() == true) ||
		(IsPlayerTemporarySkillWeaponChanging() == true)) {
		int temp_weapon_id = GetPlayerTemporarySkillWeaponID();

		if (temp_weapon_id != ID_WEAPON_NONE) {
			display_hud_weapon_id = temp_weapon_id;

			// 기존 총 HUD 방식처럼 장전 1 / 예비 탄 수 형태로 표시한다.
			lnbs = 1;
			nbs = GetPlayerTemporarySkillWeaponAmmo();

			if (nbs < 0) { nbs = 0; }
			if (lnbs > nbs) { lnbs = nbs; }
		}
	}

	// 로봇 처형 프로토콜 중에는 실제 무기/탄약은 유지하고,
	// HUD 이름과 장탄 표시만 처형 프로토콜 전용 값으로 바꾼다.
	if ((myHuman != NULL) &&
		((myHuman->GetSkillRobotExecuteFlag() == true) ||
			(player_skill_robot_fire_queue_count > 0))) {
		robot_execute_hud_active = true;
		display_hud_weapon_id = ID_WEAPON_ROBOT_EXECUTE_RIFLE;

		lnbs = player_skill_robot_execute_visual_ammo;
		nbs = player_skill_robot_execute_visual_ammo_max;

		if (lnbs < 0) { lnbs = 0; }
		if (nbs < 0) { nbs = 0; }
		if (lnbs > nbs) { lnbs = nbs; }
	}

	GameParamInfo.GetWeapon(display_hud_weapon_id, &weapon_paramdata);
	strncpy(weaponname, weapon_paramdata.name, sizeof(weaponname) - 1);
	weaponname[sizeof(weaponname) - 1] = '\0';
	hp = myHuman->GetHP();
	param_scopemode = weapon_paramdata.scopemode;
	param_WeaponP = weapon_paramdata.WeaponP;
	param_pellet = weapon_paramdata.pellet;
	ErrorRange = myHuman->GetGunsightErrorRange();

	float human_x, human_y, human_z, human_rx;
	myHuman->GetPosData(&human_x, &human_y, &human_z, &human_rx);

	//HPによる色の決定
	int statecolor;
	if (hp >= 100) {
		statecolor = d3dg->GetColorCode(0.0f, 1.0f, 0.0f, 1.0f);
	}
	else if (hp >= 50) {
		statecolor = d3dg->GetColorCode(1.0f / 50 * (100 - hp), 1.0f, 0.0f, 1.0f);
	}
	else if (hp > 0) {
		statecolor = d3dg->GetColorCode(1.0f, 1.0f / 50 * hp, 0.0f, 1.0f);
	}
	else {
		statecolor = d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f);
	}

	// 피격 플래시 표시
// 일반 피격은 빨강 전체 화면 플래시를 유지한다.
// 방패 방어 피격은 눈이 아프므로 전체 화면 하양 플래시를 표시하지 않는다.
	if ((redflash_flag == true) && (Camera_Debugmode == false)) {
		if (shieldflash_flag == false) {
			DWORD flash_color = d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 0.5f);
			d3dg->Draw2DBox(0, 0, GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight(), flash_color);
		}

		redflash_flag = false;
		shieldflash_flag = false;
	}

	// 로봇 F: 에임핵 화면 틴트
	// 노란색이 너무 강하면 시야가 탁해지므로 낮은 알파값을 사용한다.
	if ((player_skill_aimhack_timer > 0) && (Camera_Debugmode == false)) {
		DWORD aimhack_tint = d3dg->GetColorCode(1.0f, 0.82f, 0.0f, 0.07f);
		d3dg->Draw2DBox(0, 0, GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight(), aimhack_tint);
	}

	// [✨ 재장전 게이지 그리기 로직 ✨]
	if (reloadcnt > 0 && total_reloadcnt > 0) {
		float progress = 1.0f - ((float)reloadcnt / (float)(total_reloadcnt + 1));
		if (progress > 1.0f) progress = 1.0f;

		float cx = (float)GameConfig.GetScreenWidth() / 2;
		float cy = (float)GameConfig.GetScreenHeight() / 2;
		float radius = 30.0f + static_cast<float>(ErrorRange);
		int thickness = 5;
		// 매 프레임 2개의 DonutArc를 그리므로 과한 세그먼트는 프레임 드랍 원인이 된다.
		// 48이면 재장전 게이지는 충분히 부드럽고, 기존 100보다 드로우 부담이 낮다.
		int segments = 48;

		float start_angle = 0.0f;
		float end_angle = progress * 2.0f * (float)M_PI;

		// 배경 (어두운 회색)
		d3dg->Draw2DDonutArc(cx, cy, radius, 0.0f, 2.0f * (float)M_PI, thickness, segments, d3dg->GetColorCode(0.2f, 0.2f, 0.2f, 0.5f));
		// 진행 바 (흰색)
		d3dg->Draw2DDonutArc(cx, cy, radius, start_angle, end_angle, thickness, segments, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.8f));
	}

	if (myHuman->NightVision == true && myHuman->GetScopeMode() != 0) {
		int screenW = GameConfig.GetScreenWidth();  //
		int screenH = GameConfig.GetScreenHeight(); //
		d3dg->Draw2DBox(0, 0, screenW, screenH, 0x2200DD00);
	}

	bool hide_crosshair_for_robot_execute = false;

	if ((myHuman != NULL) && (myHuman->GetSkillRobotExecuteFlag() == true)) {
		hide_crosshair_for_robot_execute = true;
	}

	//スコ?プ?画
	if ((hide_crosshair_for_robot_execute == false) &&
		(Camera_F1mode == false) &&
		(Camera_Debugmode == false) &&
		(myHuman->GetScopeMode() != 0)) {
		ScopeParameter sparam;
		GameParamInfo.GetScopeParam(myHuman->GetScopeMode(), &sparam);

		if (((float)GameConfig.GetScreenWidth() / GameConfig.GetScreenHeight()) > 1.5f) {
			int swidth = (int)((float)GameConfig.GetScreenHeight() * 1.333f) + 1;
			d3dg->Draw2DTexture((GameConfig.GetScreenWidth() - swidth) / 2, 0, Resource.GetScopeTexture(), swidth, GameConfig.GetScreenHeight(), 1.0f);
			d3dg->Draw2DBox(0, 0, (GameConfig.GetScreenWidth() - swidth) / 2, GameConfig.GetScreenHeight(), d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f));
			d3dg->Draw2DBox(GameConfig.GetScreenWidth() - (GameConfig.GetScreenWidth() - swidth) / 2, 0, GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight(), d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f));
		}
		else {
			d3dg->Draw2DTexture(0, 0, Resource.GetScopeTexture(), GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight(), 1.0f);
		}

		if (sparam.ScopeGunsight == 1) {
			d3dg->Draw2DLine(GameConfig.GetScreenWidth() / 2 - 49, GameConfig.GetScreenHeight() / 2, GameConfig.GetScreenWidth() / 2 - 4, GameConfig.GetScreenHeight() / 2, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f));
			d3dg->Draw2DLine(GameConfig.GetScreenWidth() / 2 + 4, GameConfig.GetScreenHeight() / 2, GameConfig.GetScreenWidth() / 2 + 49, GameConfig.GetScreenHeight() / 2, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f));
			d3dg->Draw2DLine(GameConfig.GetScreenWidth() / 2, GameConfig.GetScreenHeight() / 2 - 49, GameConfig.GetScreenWidth() / 2, GameConfig.GetScreenHeight() / 2 - 4, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f));
			d3dg->Draw2DLine(GameConfig.GetScreenWidth() / 2, GameConfig.GetScreenHeight() / 2 + 4, GameConfig.GetScreenWidth() / 2, GameConfig.GetScreenHeight() / 2 + 49, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f));
			d3dg->Draw2DBox(GameConfig.GetScreenWidth() / 2 - 50, GameConfig.GetScreenHeight() / 2 - 1, GameConfig.GetScreenWidth() / 2 + 50, GameConfig.GetScreenHeight() / 2 + 1, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.5f));
			d3dg->Draw2DBox(GameConfig.GetScreenWidth() / 2 - 1, GameConfig.GetScreenHeight() / 2 - 50, GameConfig.GetScreenWidth() / 2 + 1, GameConfig.GetScreenHeight() / 2 + 50, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.5f));
		}
		if (sparam.ScopeGunsight == 2) {
			int w;
			if (((float)GameConfig.GetScreenWidth() / GameConfig.GetScreenHeight()) > 1.5f) {
				int swidth = (int)((float)GameConfig.GetScreenHeight() * 1.333f) + 1;
				w = 140 + (GameConfig.GetScreenWidth() - swidth) / 2;
			}
			else {
				w = 140;
			}
			d3dg->Draw2DLine(w, GameConfig.GetScreenHeight() / 2, GameConfig.GetScreenWidth() - w, GameConfig.GetScreenHeight() / 2, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f));
			d3dg->Draw2DLine(GameConfig.GetScreenWidth() / 2, 60, GameConfig.GetScreenWidth() / 2, GameConfig.GetScreenHeight() - 60, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f));
			d3dg->Draw2DBox(w, GameConfig.GetScreenHeight() / 2 - 1, GameConfig.GetScreenWidth() - w, GameConfig.GetScreenHeight() / 2 + 1, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.5f));
			d3dg->Draw2DBox(GameConfig.GetScreenWidth() / 2 - 1, 60, GameConfig.GetScreenWidth() / 2 + 1, GameConfig.GetScreenHeight() - 60, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.5f));
		}
		// 새로운 조준경 타입 3 (붉은 원 + 붉은 점)
		if (sparam.ScopeGunsight == 3) {
			int cx = GameConfig.GetScreenWidth() / 2;
			int cy = GameConfig.GetScreenHeight() / 2;
			DWORD color = d3dg->GetColorCode(1.0f, 0.1f, 0.1f, 0.75f); // 약간 투명한 밝은 빨간색

			// 중앙에 3x3 크기의 붉은 점을 그립니다.
			d3dg->Draw2DBox(cx - 1, cy - 1, cx + 2, cy + 2, color);

			// 중앙 점을 감싸는 붉은 원을 그립니다.
			// Draw2DCycle(중심 X, 중심 Y, 반지름, 색상)
			d3dg->Draw2DCycle(cx, cy, 18, color);
		}
		// Mil-Dot 조준점 + 중앙 원 + 숫자 눈금 (ID: 4)
		if (sparam.ScopeGunsight == 4) {
			int cx = GameConfig.GetScreenWidth() / 2;
			int cy = GameConfig.GetScreenHeight() / 2;
			DWORD line_color = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.9f); // 눈금 선 색상 (검은색)

			// 1. 얇은 기본 십자선을 그립니다.
			d3dg->Draw2DLine(cx, 0, cx, GameConfig.GetScreenHeight(), line_color); // 세로선
			d3dg->Draw2DLine(0, cy, GameConfig.GetScreenWidth(), cy, line_color);  // 가로선

			// 2. 중앙에 조준점보다 큰 검은 원을 추가합니다.
			int circle_radius = 20;
			DWORD circle_color = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.5f); // 반투명 검은색
			d3dg->Draw2DCycle(cx, cy, circle_radius, circle_color);
			d3dg->Draw2DCycle(cx, cy, circle_radius + 1, circle_color); // 두께를 위해 2번 그림

			// 3. 십자선 위에 눈금(점)과 숫자를 그립니다.
			int interval = 45; // [수정됨] 눈금 간격 대폭 증가 (기존 25 -> 45)
			char num_str[4];   // 숫자 저장용
			for (int i = 1; i <= 5; i++) { // 5번 반복하여 숫자 10까지 표시
				int offset = i * interval;
				int num_to_display = i * 2; // 2, 4, 6, 8, 10

				// 세로선 눈금 (위/아래 점)
				d3dg->Draw2DBox(cx - 1, cy - offset - 1, cx + 2, cy - offset + 2, line_color); // 위쪽 점
				d3dg->Draw2DBox(cx - 1, cy + offset - 1, cx + 2, cy + offset + 2, line_color); // 아래쪽 점

				// 가로선 눈금 (좌/우 점)
				d3dg->Draw2DBox(cx - offset - 1, cy - 1, cx - offset + 2, cy + 2, line_color); // 왼쪽 점
				d3dg->Draw2DBox(cx + offset - 1, cy - 1, cx + offset + 2, cy + 2, line_color); // 오른쪽 점

				sprintf(num_str, "%d", num_to_display);

				// 세로선 숫자 (상하)
				d3dg->Draw2DTextureFontText(cx + 8, cy - offset - 8, num_str, line_color, 8, 16); // 위쪽 숫자
				d3dg->Draw2DTextureFontText(cx + 8 + 1, cy - offset - 8, num_str, line_color, 8, 16); // (겹쳐 그리기)

				d3dg->Draw2DTextureFontText(cx + 8, cy + offset - 8, num_str, line_color, 8, 16); // 아래쪽 숫자
				d3dg->Draw2DTextureFontText(cx + 8 + 1, cy + offset - 8, num_str, line_color, 8, 16); // (겹쳐 그리기)

				// 가로선 숫자 (좌우)
				// 오른쪽 숫자
				d3dg->Draw2DTextureFontText(cx + offset + 4, cy + 8, num_str, line_color, 8, 16);
				d3dg->Draw2DTextureFontText(cx + offset + 4 + 1, cy + 8, num_str, line_color, 8, 16); // (겹쳐 그리기)

				// 왼쪽 숫자 (두 자리 수일 경우 X 위치 조정)
				if (num_to_display >= 10) {
					d3dg->Draw2DTextureFontText(cx - offset - 20, cy + 8, num_str, line_color, 8, 16);
					d3dg->Draw2DTextureFontText(cx - offset - 20 + 1, cy + 8, num_str, line_color, 8, 16); // (겹쳐 그리기)
				}
				else {
					d3dg->Draw2DTextureFontText(cx - offset - 12, cy + 8, num_str, line_color, 8, 16);
					d3dg->Draw2DTextureFontText(cx - offset - 12 + 1, cy + 8, num_str, line_color, 8, 16); // (겹쳐 그리기)
				}
			}

			// 4. 중심에 밝은 붉은 점을 추가합니다.
			DWORD red_dot_color = d3dg->GetColorCode(1.0f, 0.2f, 0.2f, 0.9f);
			d3dg->Draw2DBox(cx - 1, cy - 1, cx + 2, cy + 2, red_dot_color);
		}
		if (sparam.ScopeGunsight == 5) {
			int cx = GameConfig.GetScreenWidth() / 2;
			int cy = GameConfig.GetScreenHeight() / 2;

			DWORD color_red = d3dg->GetColorCode(1.0f, 0.2f, 0.2f, 0.9f); // 밝은 빨간색
			DWORD color_black = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.9f); // 검은색

			// 1. 중앙 쉐브론(^) 모양 그리기 (길이를 줄여 크기를 작게)
			int size = 18;       // 쉐브론의 크기
			int thickness = 5;   // 쉐브론의 두께
			for (int i = 0; i < thickness; i++) {
				d3dg->Draw2DLine(cx, cy + i, cx - size, cy + size + i, color_red);
				d3dg->Draw2DLine(cx, cy + i, cx + size, cy + size + i, color_red);
			}

			// [✨ 수정 ✨] interval 변수를 여기서 미리 선언합니다.
			int interval = 35;

			// 2. 수직 기준선 추가 (쉐브론 안쪽 꼭짓점과 이어지도록 설정)
			int v_thickness = 3;
			int v_start_y = cy + thickness;
			int v_end_y = cy + 300; // 숫자가 10까지 표시되므로 더 길게
			int y_pos_6 = v_start_y + (3 * interval); // 눈금 '6'의 Y 좌표 계산 (이제 정상 작동합니다)

			for (int i = -v_thickness / 2; i <= v_thickness / 2; i++) {
				// '6'까지 빨간색
				d3dg->Draw2DLine(cx + i, v_start_y, cx + i, y_pos_6, color_red);
				// '6' 아래로 검은색
				d3dg->Draw2DLine(cx + i, y_pos_6, cx + i, v_end_y, color_black);
			}

			// 3. 탄도 보정 눈금 및 숫자(짝수) 추가
			int num_lines = 5; // 2, 4, 6, 8, 10 표시

			for (int i = 1; i <= num_lines; i++) {
				int current_y = v_start_y + (interval * i);
				if (current_y > v_end_y) break;

				int line_width = (i == 1) ? 15 : 8;

				// 눈금 '6' 아래 ('8', '10')는 검은색, 위는 빨간색
				// i가 1, 2, 3일 때(2, 4, 6) 빨강 / i가 4, 5일 때(8, 10) 검정
				DWORD current_line_color = (i > 3) ? color_black : color_red;

				// 가로 눈금선 그리기
				d3dg->Draw2DLine(cx - line_width, current_y, cx + line_width, current_y, current_line_color);
				d3dg->Draw2DLine(cx - line_width, current_y + 1, cx + line_width, current_y + 1, current_line_color);

				// 숫자(일의 자리 짝수: 2, 4, 6, 8, 10) 텍스트 렌더링
				int display_num = i * 2;
				char numStr[16];
				snprintf(numStr, sizeof(numStr), "%d", display_num);

				// '8', '10' 숫자는 검은색, 나머지는 빨간색
				DWORD current_text_color = (i > 3) ? color_black : color_red;

				// 숫자를 굵게 하기 위해 X 좌표를 1픽셀 옮겨서 두 번 겹쳐 그리기
				d3dg->Draw2DTextureFontText(cx + line_width + 5, current_y - 6, numStr, current_text_color, 8, 16);
				d3dg->Draw2DTextureFontText(cx + line_width + 5 + 1, current_y - 6, numStr, current_text_color, 8, 16);
			}
		}
		if (sparam.ScopeGunsight == 6) {
			int cx = GameConfig.GetScreenWidth() / 2;
			int cy = GameConfig.GetScreenHeight() / 2;
			DWORD color = d3dg->GetColorCode(0.1f, 0.1f, 0.1f, 0.85f); // 어두운 검은색

			// [✨ 수정됨] 선의 굵기, 빈공간, 길이 변수 설정
			int main_line_thickness = 2; // 중심 십자선 두께 (전체적으로 약간 더 굵게 통일)
			int gap = 15;                // 중앙 점 주변 빈공간 (수치가 클수록 중앙이 넓어짐)
			int line_length = 150;       // 바깥쪽 선 길이 (기존 100에서 150으로 증가)

			// 1. 위쪽, 왼쪽, 오른쪽 십자선을 두껍고 길게 그립니다.
			d3dg->Draw2DBox(cx - (main_line_thickness / 2), cy - line_length, cx + (main_line_thickness / 2), cy - gap, color); // 위
			d3dg->Draw2DBox(cx - line_length, cy - (main_line_thickness / 2), cx - gap, cy + (main_line_thickness / 2), color); // 왼쪽
			d3dg->Draw2DBox(cx + gap, cy - (main_line_thickness / 2), cx + line_length, cy + (main_line_thickness / 2), color); // 오른쪽

			// BDC 간격 및 최대 Y좌표 미리 계산
			int bdc_interval = 25; // 눈금 간격
			int max_y_pos = cy + (10 * bdc_interval); // 숫자 10이 위치하는 Y 좌표

			// 2. 중앙 아래 세로선을 그립니다. (숫자 10까지만 내려가고 끊김)
			// [✨ 수정됨] 시작점을 cy - 10에서 cy + gap으로 변경하여 점을 침범하지 않게 수정
			// 두께 역시 main_line_thickness를 사용하여 상좌우 선과 동일하게 굵게 통일
			d3dg->Draw2DBox(cx - (main_line_thickness / 2), cy + gap, cx + (main_line_thickness / 2), max_y_pos + 1, color);

			// 3. BDC 눈금을 그립니다. (짝수 2, 4, 6, 8, 10만)
			for (int i = 2; i <= 10; i += 2) {
				int y_pos = cy + (i * bdc_interval);

				// 아래로 내려갈수록 너비가 좁아지게 계산
				int current_width = 24 - (i * 2);

				// 3픽셀 두께의 가로선
				d3dg->Draw2DLine(cx - current_width, y_pos - 1, cx + current_width, y_pos - 1, color);
				d3dg->Draw2DLine(cx - current_width, y_pos, cx + current_width, y_pos, color);
				d3dg->Draw2DLine(cx - current_width, y_pos + 1, cx + current_width, y_pos + 1, color);

				// 숫자(2, 4, 6, 8, 10) 텍스트 포맷팅
				char numStr[16];
				snprintf(numStr, sizeof(numStr), "%d", i);

				// 숫자를 눈금 우측에 배치하고 굵게(Bold) 두 번 겹쳐 그립니다.
				d3dg->Draw2DTextureFontText(cx + current_width + 5, y_pos - 8, numStr, color, 8, 16);
				d3dg->Draw2DTextureFontText(cx + current_width + 6, y_pos - 8, numStr, color, 8, 16);
			}

			// 4. 중앙 레드 닷
			DWORD red_dot_color = d3dg->GetColorCode(1.0f, 0.2f, 0.2f, 0.9f);
			d3dg->Draw2DBox(cx - 2, cy - 2, cx + 3, cy + 3, red_dot_color); // 3x3 크기의 점
		}
		// ✨ 새로 추가된 조준경 디자인 (ID: 7, 쉐브론) ✨
		if (sparam.ScopeGunsight == 7) {
			int cx = GameConfig.GetScreenWidth() / 2;
			int cy = GameConfig.GetScreenHeight() / 2;
			DWORD color = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.9f); // 조준선 색상 (검은색)
			DWORD red_dot_color = d3dg->GetColorCode(1.0f, 0.1f, 0.1f, 0.9f); // 중앙점 색상 (빨간색)
			int gap = 20;      // 중앙 공간
			int length = 80; // 십자선 길이

			// 2. 십자선 그리기 (중앙에 공간을 둠)
			d3dg->Draw2DLine(cx - gap, cy, cx - gap - length, cy, color); // 왼쪽
			d3dg->Draw2DLine(cx + gap, cy, cx + gap + length, cy, color); // 오른쪽
			d3dg->Draw2DLine(cx, cy - gap, cx, cy - gap - length, color); // 위쪽
			d3dg->Draw2DLine(cx, cy + gap, cx, cy + gap + length, color); // 아래쪽

			// 3. 중앙에 작은 점 추가
			d3dg->Draw2DBox(cx - 1, cy - 1, cx + 2, cy + 2, red_dot_color);
		}
	}


	//目隠し?画
	if ((Camera_Blind == true) && (Camera_Debugmode == false) && (hp > 0)) {
		int scopemode = myHuman->GetScopeMode();
		float adddist = 1.2f;
		ScopeParameter sparam;
		GameParamInfo.GetScopeParam(scopemode, &sparam);
		float addang = sparam.ViewAngle / 4;
		if (CollD.CheckALLBlockInside(camera_x + cosf(camera_rx) * cosf(camera_ry + addang) * adddist, camera_y + sinf(camera_ry + addang) * adddist, camera_z + sinf(camera_rx) * cosf(camera_ry + addang) * adddist) == true) {
			d3dg->Draw2DBox(0, 0, GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight() / 2, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f));
		}
		if (CollD.CheckALLBlockInside(camera_x + cosf(camera_rx) * cosf(camera_ry - addang) * adddist, camera_y + sinf(camera_ry - addang) * adddist, camera_z + sinf(camera_rx) * cosf(camera_ry - addang) * adddist) == true) {
			d3dg->Draw2DBox(0, GameConfig.GetScreenHeight() / 2, GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight(), d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f));
		}
		if (CollD.CheckALLBlockInside(camera_x + cosf(camera_rx + addang) * cosf(camera_ry) * adddist, camera_y + sinf(camera_ry) * adddist, camera_z + sinf(camera_rx + addang) * cosf(camera_ry) * adddist) == true) {
			d3dg->Draw2DBox(0, 0, GameConfig.GetScreenWidth() / 2, GameConfig.GetScreenHeight(), d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f));
		}
		if (CollD.CheckALLBlockInside(camera_x + cosf(camera_rx - addang) * cosf(camera_ry) * adddist, camera_y + sinf(camera_ry) * adddist, camera_z + sinf(camera_rx - addang) * cosf(camera_ry) * adddist) == true) {
			d3dg->Draw2DBox(GameConfig.GetScreenWidth() / 2, 0, GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight(), d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f));
		}
	}

#ifdef ENABLE_DEBUGCONSOLE
	//デバック用・ゲ??情報の?示
	if (ShowInfo_Debugmode == true) {
		float move_x, move_y, move_z;
		int keyflag;
		int block_id;
		myHuman->GetMovePos(&move_x, &move_y, &move_z);
		keyflag = myHuman->GetMoveFlag(false);
		myHuman->GetUnderBlock(&block_id, NULL);

		sprintf(str, "frame:%d   time %02d:%02d", framecnt, framecnt / (int)GAMEFPS / 60, framecnt / (int)GAMEFPS % 60);
		d3dg->Draw2DTextureDebugFontText(10 + 1, 10 + 1, str, d3dg->GetColorCode(0.1f, 0.1f, 0.1f, 1.0f));
		d3dg->Draw2DTextureDebugFontText(10, 10, str, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f));
		sprintf(str, "camera x:%.2f y:%.2f z:%.2f rx:%.2f ry:%.2f", camera_x, camera_y, camera_z, camera_rx, camera_ry);
		d3dg->Draw2DTextureDebugFontText(10 + 1, 30 + 1, str, d3dg->GetColorCode(0.1f, 0.1f, 0.1f, 1.0f));
		d3dg->Draw2DTextureDebugFontText(10, 30, str, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f));
		sprintf(str, "human[%d] x:%.2f y:%.2f z:%.2f rx:%.2f HP:%d MoveKey:0x%02X", ObjMgr.GetPlayerID(), human_x, human_y, human_z, human_rx, hp, keyflag);
		d3dg->Draw2DTextureDebugFontText(10 + 1, 50 + 1, str, d3dg->GetColorCode(0.1f, 0.1f, 0.1f, 1.0f));
		d3dg->Draw2DTextureDebugFontText(10, 50, str, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f));
		sprintf(str, "          move_x:%.2f move_y:%.2f move_z:%.2f BlockID:%d", move_x, move_y, move_z, block_id);
		d3dg->Draw2DTextureDebugFontText(10 + 1, 70 + 1, str, d3dg->GetColorCode(0.1f, 0.1f, 0.1f, 1.0f));
		d3dg->Draw2DTextureDebugFontText(10, 70, str, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f));
		sprintf(str, "Input:%02dms Object:%02dms AI:%02dms Event:%02dms Sound:%02dms Render:%02dms", time_input, time_process_object, time_process_ai, time_process_event, time_sound, time_render);
		d3dg->Draw2DTextureDebugFontText(10 + 1, 90 + 1, str, d3dg->GetColorCode(0.1f, 0.1f, 0.1f, 1.0f));
		d3dg->Draw2DTextureDebugFontText(10, 90, str, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f));
	}
#endif

	//ゲ??実行速度の?示
	sprintf(str, "fps:%.2f", fps);
	d3dg->Draw2DTextureFontText(GameConfig.GetScreenWidth() - strlen(str) * 10 - 5 + 1, 5 + 1, str, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f), 10, 14);
	d3dg->Draw2DTextureFontText(GameConfig.GetScreenWidth() - strlen(str) * 10 - 5, 5, str, d3dg->GetColorCode(1.0f, 0.5f, 0.0f, 1.0f), 10, 14);

	//HUD?示・モ?ドA
	if (Camera_F2mode == 0) {
		stru[0] = 0xB3;		stru[1] = 0xB4;		stru[2] = 0xB4;		stru[3] = 0xB4;		stru[4] = 0xB4;
		stru[5] = 0xB4;		stru[6] = 0xB4;		stru[7] = 0xB5;		stru[8] = '\0';
		d3dg->Draw2DTextureFontText(15, GameConfig.GetScreenHeight() - 105, (char*)stru, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.5f), 32, 32);
		for (int i = 0; stru[i] != 0x00; i++) { stru[i] += 0x10; }
		d3dg->Draw2DTextureFontText(15, GameConfig.GetScreenHeight() - 105 + 32, (char*)stru, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.5f), 32, 32);
		stru[0] = 0xB3;		stru[1] = 0xB4;		stru[2] = 0xB4;		stru[3] = 0xB6;		stru[4] = 0xB7;
		stru[5] = 0xB7;		stru[6] = 0xB7;		stru[7] = 0xB8;		stru[8] = 0xB9;		stru[9] = '\0';
		d3dg->Draw2DTextureFontText(15, GameConfig.GetScreenHeight() - 55, (char*)stru, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.5f), 32, 32);
		for (int i = 0; stru[i] != 0x00; i++) { stru[i] += 0x10; }
		d3dg->Draw2DTextureFontText(15, GameConfig.GetScreenHeight() - 55 + 32, (char*)stru, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.5f), 32, 32);

		stru[0] = 0xB0;
		for (int i = 1; i < HUDA_WEAPON_SIZEW - 1; i++) {
			stru[i] = 0xB1;
		}
		stru[HUDA_WEAPON_SIZEW - 1] = 0xB2;
		stru[HUDA_WEAPON_SIZEW] = '\0';

		d3dg->Draw2DTextureFontText(HUDA_WEAPON_POSX, HUDA_WEAPON_POSY, (char*)stru, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.5f), 32, 32);
		for (int i = 0; i < HUDA_WEAPON_SIZEW; i++) { stru[i] += 0x10; }
		for (int i = 1; i < HUDA_WEAPON_SIZEH - 1; i++) {
			d3dg->Draw2DTextureFontText(HUDA_WEAPON_POSX, HUDA_WEAPON_POSY + 32 * i, (char*)stru, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.5f), 32, 32);
		}
		for (int i = 0; i < HUDA_WEAPON_SIZEW; i++) { stru[i] += 0x10; }
		d3dg->Draw2DTextureFontText(HUDA_WEAPON_POSX, HUDA_WEAPON_POSY + 32 * (HUDA_WEAPON_SIZEH - 1), (char*)stru, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.5f), 32, 32);

		if (robot_execute_hud_active == true) {
			// 처형 프로토콜 중에는 실제 탄약이 아니라 스킬 전용 탄창을 현재/최대 형식으로 표시한다.
			sprintf((char*)stru, "%d/%d", lnbs, nbs);
		}
		else {
			sprintf((char*)stru, "A%d B%d", lnbs, (nbs - lnbs));

			for (int i = 0; i < (int)strlen((char*)stru); i++) {
				if (stru[i] == 'A') { stru[i] = 0xBB; }
				if (stru[i] == 'B') { stru[i] = 0xBA; }
			}
		}

		d3dg->Draw2DTextureFontText(
			25,
			GameConfig.GetScreenHeight() - 96,
			(char*)stru,
			d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f),
			23,
			24
		);

		// [✨ 핵심 수정 ✨]
		// 기존의 "FINE", "CAUTION" 등의 텍스트 대신, 정확한 체력 수치를 표시합니다.
		char hp_text[16];
		sprintf(hp_text, "%d", hp); // hp 변수 값을 문자열로 변환

		// "STATE" 텍스트는 그대로 표시
		d3dg->Draw2DTextureFontText(23, GameConfig.GetScreenHeight() - 45, "STATE", statecolor, 18, 24);

		// hp가 0보다 클 때는 변환된 숫자 문자열을, 0 이하일 때는 "DEAD"를 표시
		if (hp > 0) {
			d3dg->Draw2DTextureFontText(155, GameConfig.GetScreenHeight() - 45, hp_text, statecolor, 18, 24);
		}
		else {
			d3dg->Draw2DTextureFontText(155, GameConfig.GetScreenHeight() - 45, "DEAD", statecolor, 18, 24);
		}

#ifdef ENABLE_WEAPONNAME_SCALING
		int w, h;
		d3dg->GetFontTextSize(weaponname, 16, 20, 14, &w, &h);
		d3dg->Draw2DTextureFontText(HUDA_WEAPON_POSX + 9, HUDA_WEAPON_POSY + 4 + (20 - h) / 2, weaponname, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), w, h);
#else
		d3dg->Draw2DTextureFontText(HUDA_WEAPON_POSX + 9, HUDA_WEAPON_POSY + 4, weaponname, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), 16, 20);
#endif

		// 오른쪽 아래 3D 무기 모델 대신,
		// 실제 1번/2번 무기 슬롯 정보를 항상 표시한다.
		// 탄창 정보는 왼쪽 아래 HUD에 이미 있으므로 여기에는 표시하지 않는다.
		{
			int highlight_slot = selectweapon;

			// 스킬 임시 무기 사용 중에는 실제 복귀 슬롯을 밝게 표시한다.
			if (IsPlayerTemporarySkillWeaponVisible() == true) {
				int return_slot = GetPlayerTemporarySkillWeaponReturnSlot();

				if ((return_slot >= 0) && (return_slot < TOTAL_HAVEWEAPON)) {
					highlight_slot = return_slot;
				}
			}

			// DP28 스킬 무기 사용 중에도 원래 무기로 돌아갈 슬롯을 밝게 표시한다.
			if (IsPlayerDP28SkillWeaponActive() == true) {
				int return_slot = GetPlayerDP28SkillWeaponReturnSlot();

				if ((return_slot >= 0) && (return_slot < TOTAL_HAVEWEAPON)) {
					highlight_slot = return_slot;
				}
			}

			for (int i = 0; i < TOTAL_HAVEWEAPON; i++) {
				WeaponParameter slot_paramdata;
				char slot_weapon_name[32 + 1];
				char slot_text[48];

				strcpy(slot_weapon_name, "NONE");

				int slot_display_weapon_id = weapon_paramid[i];

				// DP28은 실제 선택 슬롯에 임시 삽입되므로,
				// HUD의 기존 무기 목록에서는 저장해둔 원래 무기 ID를 보여준다.
				if ((IsPlayerDP28SkillWeaponActive() == true) &&
					(i == GetPlayerDP28SkillWeaponReturnSlot())) {
					slot_display_weapon_id = GetPlayerDP28SkillStoredWeaponID();
				}

				if (GameParamInfo.GetWeapon(slot_display_weapon_id, &slot_paramdata) == 0) {
					strncpy(slot_weapon_name, slot_paramdata.name, sizeof(slot_weapon_name) - 1);
					slot_weapon_name[sizeof(slot_weapon_name) - 1] = '\0';
				}

				sprintf(slot_text, "[%d] %s", i + 1, slot_weapon_name);

				// 우측 무기 HUD 안에 들어가도록 글자 폭만 줄인다.
				int slot_font_width = 10;
				int slot_font_height = 14;
				const int slot_text_max_width = HUDA_WEAPON_SIZEW * 32 - 24;

				int slot_text_len = (int)strlen(slot_text);
				if ((slot_text_len > 0) && (slot_text_len * slot_font_width > slot_text_max_width)) {
					slot_font_width = slot_text_max_width / slot_text_len;
					if (slot_font_width < 8) { slot_font_width = 8; }
					if (slot_font_width > 10) { slot_font_width = 10; }
				}

				// 최소 폭 8로도 넘치면, 마지막 안전장치로 문자열 자체를 줄인다.
				while (((int)strlen(slot_text) * slot_font_width > slot_text_max_width) &&
					(strlen(slot_weapon_name) > 1)) {
					slot_weapon_name[strlen(slot_weapon_name) - 1] = '\0';
					sprintf(slot_text, "[%d] %s", i + 1, slot_weapon_name);
				}

				bool selected_slot = (i == highlight_slot);
				bool empty_slot = (strcmp(slot_weapon_name, "NONE") == 0);

				// 하이라이트 박스는 사용하지 않고 글자 색으로만 구분한다.
				// 선택 슬롯은 스킬 UI와 비슷한 노란색,
				// 비선택 슬롯은 읽기 쉬운 밝은 회색,
				// 빈 슬롯은 어두운 회색으로 표시한다.
				int slot_text_color;

				if (selected_slot == true) {
					slot_text_color = d3dg->GetColorCode(1.0f, 0.82f, 0.15f, 1.0f);
				}
				else if (empty_slot == true) {
					slot_text_color = d3dg->GetColorCode(0.45f, 0.45f, 0.45f, 0.75f);
				}
				else {
					slot_text_color = d3dg->GetColorCode(0.78f, 0.78f, 0.78f, 0.95f);
				}

				int row_y1 = HUDA_WEAPON_POSY + 31 + i * 21;

				d3dg->Draw2DTextureFontText(
					HUDA_WEAPON_POSX + 13,
					row_y1 + 2,
					slot_text,
					slot_text_color,
					slot_font_width,
					slot_font_height
				);
			}
		}

		// 무기 HUD 위에 현재 사용 가능한 조작키를 표시한다.
		RenderPlayerControlGuide(myHuman, display_hud_weapon_id);
	}

	// 플레이어 스킬 화면 효과
	RenderPlayerSkillScreenEffect(myHuman);

	// 플레이어 스킬 HUD
	RenderPlayerSkillHUD(myHuman);

	// 설치형 스킬 설치 대기 중에는 중앙 살짝 아래에 취소 안내를 강조 표시한다.
	RenderInstallSkillCancelGuide();

	//HUD?렑갋긾?긤B
	if( Camera_F2mode == 1 ){
		//됪뽋뢂귟궻멄
		d3dg->Draw2DLine(0, 0, GameConfig.GetScreenWidth()-1, 0, statecolor);
		d3dg->Draw2DLine(GameConfig.GetScreenWidth()-1, 0, GameConfig.GetScreenWidth()-1, GameConfig.GetScreenHeight()-1, statecolor);
		d3dg->Draw2DLine(0, 0, 0, GameConfig.GetScreenHeight()-1, statecolor);
		d3dg->Draw2DLine(0, GameConfig.GetScreenHeight()-1, GameConfig.GetScreenWidth()-1, GameConfig.GetScreenHeight()-1, statecolor);

		//븧딇뼹?렑
#ifdef ENABLE_WEAPONNAME_SCALING
		int w, h;
		d3dg->GetFontTextSize(weaponname, 16, 20, 14, &w, &h);

		d3dg->Draw2DBox(8, GameConfig.GetScreenHeight() - 32, 227, GameConfig.GetScreenHeight() - 7, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.3f));
		d3dg->Draw2DTextureFontText(10, GameConfig.GetScreenHeight() - 30 + (20 - h)/2, weaponname, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), w, h);
#else
		d3dg->Draw2DBox(8, GameConfig.GetScreenHeight() - 32, 227, GameConfig.GetScreenHeight() - 7, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.3f));
		d3dg->Draw2DTextureFontText(10, GameConfig.GetScreenHeight() - 30, weaponname, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 16, 20);
#endif
	}

	//귽긹깛긣긽긞긜?긙?렑
	if ((message_id != -1) && (message_cnt < (int)(TOTAL_EVENTENT_SHOWMESSEC * GAMEFPS))) {
		char messtr[MAX_POINTMESSAGEBYTE];
		PointData.GetMessageText(messtr, message_id);
		float effectA = 1.0f;
		if (message_cnt < (int)(0.2f * GAMEFPS)) { effectA = GetEffectAlpha(message_cnt, 1.0f, 0.2f, 0.0f, false); }
		if ((int)((TOTAL_EVENTENT_SHOWMESSEC - 0.2f) * GAMEFPS) < message_cnt) { effectA = GetEffectAlpha(message_cnt, 1.0f, 0.2f, (TOTAL_EVENTENT_SHOWMESSEC - 0.2f), true); }

		// 위치와 중앙 정렬은 기존 자막과 동일하게 유지하고,
		// 글씨 크기만 작은 시스템 폰트로 줄인다.
		d3dg->Draw2DMSFontTextCenterBriefingScaling(
			0 + 1,
			340 + 1,
			GameConfig.GetScreenWidth(),
			140,
			messtr,
			d3dg->GetColorCode(0.1f, 0.1f, 0.1f, effectA)
		);

		d3dg->Draw2DMSFontTextCenterBriefingScaling(
			0,
			340,
			GameConfig.GetScreenWidth(),
			140,
			messtr,
			d3dg->GetColorCode(1.0f, 1.0f, 1.0f, effectA)
		);
	}

#ifdef ENABLE_DEBUGCONSOLE
	//긢긫긞긏뾭갋긒??륃뺪궻?렑
	if( ShowInfo_Debugmode == true ){
		ObjMgr.RenderLog(HUDA_WEAPON_POSX, HUDA_WEAPON_POSY-60);
	}
#endif

	//깏깓?긤?렑
/// -----------------------------------------------------------
	// [통합] 재장전 및 볼트 액션 UI (원형 & 직선형 게이지 통합 관리)
	// -----------------------------------------------------------

	// 1. 필요한 변수들 미리 가져오기
	human::ShellReloadState reload_state = myHuman->GetShellReloadState();
	int bolt_timer = myHuman->GetBoltActionTimer();
	int bolt_total_time = myHuman->GetBoltActionTotalTime();

	// 미니건 예열 타이머
	int minigun_spin_timer = myHuman->GetMinigunSpinTimer();
	int minigun_spin_total_time = myHuman->GetMinigunSpinTotalTime();

	// 화면 중앙 좌표
	int cx = GameConfig.GetScreenWidth() / 2;
	int cy = GameConfig.GetScreenHeight() / 2;

	// 수류탄 궤도 표시 중에는 기존 조준점을 숨긴다.
// 단, 재장전 게이지/히트마커/태그 같은 HUD는 계속 표시한다.
	bool hide_crosshair_for_grenade = false;

	if (ShouldRenderGrenadeTrajectory(myHuman) == true) {
		hide_crosshair_for_grenade = true;
	}

	if ((myHuman != NULL) && (myHuman->GetSkillRobotExecuteFlag() == true)) {
		hide_crosshair_for_grenade = true;
	}

	// 2. 상태에 따른 그리기 분기
// 수류탄 궤도 표시 중에는 크로스헤어/중앙 조준 UI를 숨긴다.
	if (hide_crosshair_for_grenade == false) {

		// 우선순위 1: 탄약을 한 발씩 집어넣는 중 (원형 게이지)
		if (reload_state == human::SHELL_RELOAD_INSERTING) {
			// "RELOADING" 텍스트 표시
			d3dg->Draw2DTextureFontTextCenterScaling(3, 300 + 3, "RELOADING", d3dg->GetColorCode(0.2f, 0.2f, 0.2f, 1.0f), 32, 34);
			d3dg->Draw2DTextureFontTextCenterScaling(0, 300, "RELOADING", d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), 32, 34);

			float progress = 0.0f;
			int shell_timer = myHuman->GetShellReloadTimer();
			int shell_total_time = myHuman->GetShellReloadTimePerShell();

			if (shell_total_time > 0) {
				progress = 1.0f - static_cast<float>(shell_timer) / static_cast<float>(shell_total_time);
			}

			float arc_cx = static_cast<float>(cx);
			float arc_cy = static_cast<float>(cy);

			float radius = 30.0f + static_cast<float>(ErrorRange);
			int thickness = 5;
			int segments = 48;

			float start_angle = -3.14159265f / 2.0f;
			float end_angle = start_angle + progress * 2.0f * 3.14159265f;

			d3dg->Draw2DDonutArc(
				arc_cx,
				arc_cy,
				radius,
				0.0f,
				2.0f * 3.14159265f,
				thickness,
				segments,
				d3dg->GetColorCode(0.2f, 0.2f, 0.2f, 0.5f)
			);

			d3dg->Draw2DDonutArc(
				arc_cx,
				arc_cy,
				radius,
				start_angle,
				end_angle,
				thickness,
				segments,
				d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.8f)
			);
		}
		// 우선순위 2: 미니건 예열 게이지
		else if (minigun_spin_timer > 0 && minigun_spin_total_time > 0) {
			float progress = static_cast<float>(minigun_spin_timer) / static_cast<float>(minigun_spin_total_time);

			if (progress < 0.0f) { progress = 0.0f; }
			if (progress > 1.0f) { progress = 1.0f; }

			int bar_width = 100;
			int bar_height = 6;
			int bar_y_offset = 42 + ErrorRange;

			// 배경
			d3dg->Draw2DBox(
				cx - (bar_width / 2),
				cy + bar_y_offset,
				cx + (bar_width / 2),
				cy + bar_y_offset + bar_height,
				d3dg->GetColorCode(0.2f, 0.2f, 0.2f, 0.5f)
			);

			int fill_width = static_cast<int>(bar_width * progress);

			// 예열 진행바
			d3dg->Draw2DBox(
				cx - (bar_width / 2),
				cy + bar_y_offset,
				cx - (bar_width / 2) + fill_width,
				cy + bar_y_offset + bar_height,
				d3dg->GetColorCode(0.8f, 0.8f, 0.8f, 0.85f)
			);
		}
		// 우선순위 3: 볼트 액션 타이머가 돌아가는 중 (직선형 게이지)
		// -> 샷건 재장전 시작/끝 딜레이, 저격소총 볼트 당기기, 펌프 액션 등 모든 딜레이가 여기 포함됨
		else if (bolt_timer > 0 && bolt_total_time > 0) {
			// 필요하다면 여기서도 "RELOADING" 텍스트를 띄울 수 있음 (원하면 주석 해제)
			// if (reload_state != human::SHELL_RELOAD_NONE) {
			//     d3dg->Draw2DTextureFontTextCenterScaling(0, 300, "RELOADING", d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), 32, 34);
			// }

			float progress = 1.0f - ((float)bolt_timer / (float)bolt_total_time);

			int bar_width = 100;
			int bar_height = 6;

			// [핵심] ErrorRange(조준선 벌어짐)를 더해서 조준선과 겹치지 않게 함
			int bar_y_offset = 30 + ErrorRange;

			// 게이지 배경 (반투명 회색)
			d3dg->Draw2DBox(cx - (bar_width / 2), cy + bar_y_offset, cx + (bar_width / 2), cy + bar_y_offset + bar_height, d3dg->GetColorCode(0.2f, 0.2f, 0.2f, 0.5f));
			// 게이지 채워지는 부분 (흰색)
			int fill_width = (int)(bar_width * progress);
			d3dg->Draw2DBox(cx - (bar_width / 2), cy + bar_y_offset, cx - (bar_width / 2) + fill_width, cy + bar_y_offset + bar_height, d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.8f));
		}
		else if (changeweaponidcnt > 0 && changeweaponid_total_time > 0) {
			float progress = 1.0f - ((float)changeweaponidcnt / (float)changeweaponid_total_time);
			if (progress < 0.0f) progress = 0.0f;
			if (progress > 1.0f) progress = 1.0f;

			int bar_width = 100;
			int bar_height = 6;
			int bar_y_offset = 42 + ErrorRange;

			d3dg->Draw2DBox(
				cx - (bar_width / 2),
				cy + bar_y_offset,
				cx + (bar_width / 2),
				cy + bar_y_offset + bar_height,
				d3dg->GetColorCode(0.2f, 0.2f, 0.2f, 0.5f)
			);

			int fill_width = (int)(bar_width * progress);

			d3dg->Draw2DBox(
				cx - (bar_width / 2),
				cy + bar_y_offset,
				cx - (bar_width / 2) + fill_width,
				cy + bar_y_offset + bar_height,
				d3dg->GetColorCode(0.8f, 0.8f, 0.8f, 0.85f)
			);
		}

		else if (reloadcnt > 0) {
			// 일반 재장전: "RELOADING" 텍스트만 표시
			d3dg->Draw2DTextureFontTextCenterScaling(3, 300 + 3, "RELOADING", d3dg->GetColorCode(0.2f, 0.2f, 0.2f, 1.0f), 32, 34);
			d3dg->Draw2DTextureFontTextCenterScaling(0, 300, "RELOADING", d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), 32, 34);
		}

		//븧딇먛귟뫶궑?렑
		if (selectweaponcnt > 0) {
			d3dg->Draw2DTextureFontTextCenterScaling(3, 300 + 3, "CHANGING", d3dg->GetColorCode(0.2f, 0.2f, 0.2f, 1.0f), 32, 34);
			d3dg->Draw2DTextureFontTextCenterScaling(0, 300, "CHANGING", d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), 32, 34);
		}

		//뤖??렑
		if ((Camera_F1mode == false) && (param_WeaponP != 2) && (param_pellet > 0)) {
			if ((weapon[selectweapon] != NULL)) {
				int scopemode = myHuman->GetScopeMode();
				ScopeParameter sparam;
				GameParamInfo.GetScopeParam(param_scopemode, &sparam);

				if (((scopemode == 0) && (sparam.NoScopeGunsight == true)) || ((scopemode != 0) && (sparam.ScopeGunsight == 0))) {
					if (GameConfig.GetAnotherGunsightFlag()) {	//긆긵긘깈깛?
						//뤖?궻벁뼻뱗
						int cx = GameConfig.GetScreenWidth() / 2;
						int cy = GameConfig.GetScreenHeight() / 2;
						DWORD color = d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f);  // 노란색
						int offset = ErrorRange;

						d3dg->Draw2DBox(cx - 1, cy - 1, cx + 2, cy + 2, color);  // 가운데 점
						d3dg->Draw2DLine(cx - offset - 5, cy, cx - offset, cy, color);
						d3dg->Draw2DLine(cx + offset + 5, cy, cx + offset, cy, color);
						d3dg->Draw2DLine(cx, cy - offset - 5, cx, cy - offset, color);
						d3dg->Draw2DLine(cx, cy + offset + 5, cx, cy + offset, color);

						// 피격 대각선 X 효과 그리기
						human* player = ObjMgr.GetPlayerHumanObject();
						if (player && player->GetHitEffectTimer() > 0) {
							int cx = GameConfig.GetScreenWidth() / 2;
							int cy = GameConfig.GetScreenHeight() / 2;
							int offset = 20;

							DWORD hit_effect_color;

							if (player->IsShieldHitEffect() == true) {
								hit_effect_color = d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.85f);
							}
							else {
								hit_effect_color = d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 0.8f);
							}

							d3dg->Draw2DLine(cx - offset, cy - offset, cx + offset, cy + offset, hit_effect_color);
							d3dg->Draw2DLine(cx - offset, cy + offset, cx + offset, cy - offset, hit_effect_color);
						}
					}
					else { // 노스코프 크로스헤어 그리기
						if (hide_crosshair_for_grenade == false) {
							int cx = GameConfig.GetScreenWidth() / 2;
							int cy = GameConfig.GetScreenHeight() / 2;

							DWORD base_color;
							switch (GameConfig.GetCrosshairColor()) {
							case 1: base_color = d3dg->GetColorCode(1.0f, 0.0f, 0.0f, 1.0f); break; // RED
							case 2: base_color = d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 1.0f); break; // YELLOW
							case 3: base_color = d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f); break; // WHITE
							case 4: base_color = d3dg->GetColorCode(0.0f, 0.5f, 1.0f, 1.0f); break; // BLUE
							case 5: base_color = d3dg->GetColorCode(0.0f, 1.0f, 1.0f, 1.0f); break; // CYAN
							case 6: base_color = d3dg->GetColorCode(1.0f, 0.0f, 1.0f, 1.0f); break; // MAGENTA
							case 7: base_color = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f); break; // BLACK
							case 8: base_color = d3dg->GetColorCode(1.0f, 0.5f, 0.0f, 1.0f); break; // ORANGE
							case 9: base_color = d3dg->GetColorCode(1.0f, 0.4f, 0.7f, 1.0f); break; // PINK
							default: base_color = d3dg->GetColorCode(0.0f, 0.8f, 0.0f, 1.0f); break; // GREEN
							}

							int dynamic_gap = 6 + ErrorRange;

							// [✨ 테두리 효과 핵심 알고리즘] Outline이 켜져있으면 루프를 5번 돌려 그림자를 만듭니다.
							int passes = GameConfig.GetCrosshairOutline() ? 5 : 1;
							int dx[5] = { -1, 1, 0, 0, 0 }; // 좌, 우, 상, 하 방향 오프셋
							int dy[5] = { 0, 0, -1, 1, 0 };

							for (int p = 0; p < passes; p++) {
								DWORD color;
								int cx_off = cx;
								int cy_off = cy;

								if (passes == 5 && p < 4) {
									// 테두리를 그리는 1~4번째 패스: 약간 어긋난 위치에 검은색으로 그리기
									color = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f);
									cx_off += dx[p];
									cy_off += dy[p];
								}
								else {
									// 마지막 5번째 패스: 정중앙에 본래 색상으로 그리기
									color = base_color;
								}

								switch (GameConfig.GetCrosshairStyle()) {
								case 0: // CROSS
									d3dg->Draw2DBox(cx_off - dynamic_gap - 10, cy_off - 1, cx_off - dynamic_gap, cy_off + 1, color);
									d3dg->Draw2DBox(cx_off + dynamic_gap, cy_off - 1, cx_off + dynamic_gap + 10, cy_off + 1, color);
									d3dg->Draw2DBox(cx_off - 1, cy_off - dynamic_gap - 10, cx_off + 1, cy_off - dynamic_gap, color);
									d3dg->Draw2DBox(cx_off - 1, cy_off + dynamic_gap, cx_off + 1, cy_off + dynamic_gap + 10, color);
									break;

								case 1: // CIRCLE
									d3dg->Draw2DCycle(cx_off, cy_off, dynamic_gap + 4, color);
									d3dg->Draw2DCycle(cx_off, cy_off, dynamic_gap + 5, color);
									d3dg->Draw2DBox(cx_off - 1, cy_off - 1, cx_off + 1, cy_off + 1, color);
									break;

								case 2: // DOT
									d3dg->Draw2DBox(cx_off - 2, cy_off - 2, cx_off + 2, cy_off + 2, color);
									break;

								case 3: // CROSS + CIRCLE
									d3dg->Draw2DCycle(cx_off, cy_off, dynamic_gap + 8, color);
									d3dg->Draw2DBox(cx_off - dynamic_gap - 12, cy_off - 1, cx_off - dynamic_gap, cy_off + 1, color);
									d3dg->Draw2DBox(cx_off + dynamic_gap, cy_off - 1, cx_off + dynamic_gap + 12, cy_off + 1, color);
									d3dg->Draw2DBox(cx_off - 1, cy_off - dynamic_gap - 12, cx_off + 1, cy_off - dynamic_gap, color);
									d3dg->Draw2DBox(cx_off - 1, cy_off + dynamic_gap, cx_off + 1, cy_off + dynamic_gap + 12, color);
									d3dg->Draw2DBox(cx_off - 1, cy_off - 1, cx_off + 1, cy_off + 1, color);
									break;

								case 4: // CHEVRON (점처럼 위치 완전 고정, dynamic_gap 미사용)
									d3dg->Draw2DLine(cx_off, cy_off, cx_off - 10, cy_off + 10, color);
									d3dg->Draw2DLine(cx_off, cy_off, cx_off + 10, cy_off + 10, color);
									// 선을 굵게 만들기 위해 1픽셀 아래로 덧칠
									d3dg->Draw2DLine(cx_off, cy_off + 1, cx_off - 10, cy_off + 11, color);
									d3dg->Draw2DLine(cx_off, cy_off + 1, cx_off + 10, cy_off + 11, color);
									break;

								case 5: // X-SHAPE (선 굵기 추가)
									for (int t = 0; t <= 1; t++) {
										d3dg->Draw2DLine(cx_off - dynamic_gap - 8 + t, cy_off - dynamic_gap - 8, cx_off - dynamic_gap + t, cy_off - dynamic_gap, color);
										d3dg->Draw2DLine(cx_off + dynamic_gap + t, cy_off + dynamic_gap, cx_off + dynamic_gap + 8 + t, cy_off + dynamic_gap + 8, color);
										d3dg->Draw2DLine(cx_off - dynamic_gap - 8 + t, cy_off + dynamic_gap + 8, cx_off - dynamic_gap + t, cy_off + dynamic_gap, color);
										d3dg->Draw2DLine(cx_off + dynamic_gap + t, cy_off - dynamic_gap, cx_off + dynamic_gap + 8 + t, cy_off - dynamic_gap - 8, color);
									}
									d3dg->Draw2DBox(cx_off - 1, cy_off - 1, cx_off + 1, cy_off + 1, color);
									break;

								case 6: // SQUARE
									d3dg->Draw2DBox(cx_off - dynamic_gap - 4, cy_off - dynamic_gap - 4, cx_off + dynamic_gap + 4, cy_off - dynamic_gap - 2, color);
									d3dg->Draw2DBox(cx_off - dynamic_gap - 4, cy_off + dynamic_gap + 2, cx_off + dynamic_gap + 4, cy_off + dynamic_gap + 4, color);
									d3dg->Draw2DBox(cx_off - dynamic_gap - 4, cy_off - dynamic_gap - 4, cx_off - dynamic_gap - 2, cy_off + dynamic_gap + 4, color);
									d3dg->Draw2DBox(cx_off + dynamic_gap + 2, cy_off - dynamic_gap - 4, cx_off + dynamic_gap + 4, cy_off + dynamic_gap + 4, color);
									d3dg->Draw2DBox(cx_off - 1, cy_off - 1, cx_off + 1, cy_off + 1, color);
									break;

								case 7: // CROSS + DOT
									d3dg->Draw2DBox(cx_off - dynamic_gap - 10, cy_off - 1, cx_off - dynamic_gap, cy_off + 1, color);
									d3dg->Draw2DBox(cx_off + dynamic_gap, cy_off - 1, cx_off + dynamic_gap + 10, cy_off + 1, color);
									d3dg->Draw2DBox(cx_off - 1, cy_off - dynamic_gap - 10, cx_off + 1, cy_off - dynamic_gap, color);
									d3dg->Draw2DBox(cx_off - 1, cy_off + dynamic_gap, cx_off + 1, cy_off + dynamic_gap + 10, color);
									d3dg->Draw2DBox(cx_off - 1, cy_off - 1, cx_off + 1, cy_off + 1, color);
									break;

								case 8: // TRIANGLE (선 굵기 추가)
									for (int t = 0; t <= 1; t++) {
										d3dg->Draw2DLine(cx_off + t, cy_off - dynamic_gap - 8, cx_off - dynamic_gap - 8 + t, cy_off + dynamic_gap + 4, color);
										d3dg->Draw2DLine(cx_off - dynamic_gap - 8, cy_off + dynamic_gap + 4 + t, cx_off + dynamic_gap + 8, cy_off + dynamic_gap + 4 + t, color);
										d3dg->Draw2DLine(cx_off + dynamic_gap + 8 + t, cy_off + dynamic_gap + 4, cx_off + t, cy_off - dynamic_gap - 8, color);
									}
									d3dg->Draw2DBox(cx_off - 1, cy_off - 1, cx_off + 1, cy_off + 1, color);
									break;

								case 9: // T-SHAPE
									d3dg->Draw2DBox(cx_off - dynamic_gap - 10, cy_off - 1, cx_off - dynamic_gap, cy_off + 1, color);
									d3dg->Draw2DBox(cx_off + dynamic_gap, cy_off - 1, cx_off + dynamic_gap + 10, cy_off + 1, color);
									d3dg->Draw2DBox(cx_off - 1, cy_off + dynamic_gap, cx_off + 1, cy_off + dynamic_gap + 10, color);
									d3dg->Draw2DBox(cx_off - 1, cy_off - 1, cx_off + 1, cy_off + 1, color);
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	// 피격 방향 원호 표시
	if ((damage_arc_timer > 0) && (Camera_Debugmode == false)) {

		int cx = GameConfig.GetScreenWidth() / 2;
		int cy = GameConfig.GetScreenHeight() / 2;

		float radius = 46.0f;
		int thickness = 5;
		int segments = 32;

		float alpha = (float)damage_arc_timer / 18.0f;
		if (alpha < 0.0f) { alpha = 0.0f; }
		if (alpha > 1.0f) { alpha = 1.0f; }

		// 피격 방향을 현재 카메라 기준으로 변환
		float dir = draw_camera_rx - damage_arc_rx;

		// 화면 좌표계 보정
		float screen_angle = dir;

		// 원호 폭. 값이 클수록 넓게 표시됨
		float arc_width = (float)M_PI / 5.0f;

		float start_angle = screen_angle - arc_width;
		float end_angle = screen_angle + arc_width;

		DWORD color;

		if (damage_arc_shield_flag == true) {
			color = d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.85f * alpha);
		}
		else {
			color = d3dg->GetColorCode(1.0f, 0.1f, 0.05f, 0.85f * alpha);
		}

		d3dg->Draw2DDonutArc(
			(float)cx,
			(float)cy,
			radius,
			start_angle,
			end_angle,
			thickness,
			segments,
			color
		);
	}
	// '히트마커' 타이머를 확인하고, 그에 맞는 히트마커를 그립니다.
	human* player = ObjMgr.GetPlayerHumanObject();
	if (player && player->GetHitMarkerTimer() > 0)
	{
		int cx = GameConfig.GetScreenWidth() / 2;
		int cy = GameConfig.GetScreenHeight() / 2;

		if (player->IsHeadshot()) {
			// 헤드샷: 붉고 굵은 선
			int gap = 8;
			int length = 12;
			DWORD color = d3dg->GetColorCode(1.0f, 0.2f, 0.2f, 0.9f);
			d3dg->Draw2DLine(cx - gap, cy - gap, cx - gap - length, cy - gap - length, color);
			d3dg->Draw2DLine(cx - gap + 1, cy - gap, cx - gap - length + 1, cy - gap - length, color);
			d3dg->Draw2DLine(cx + gap, cy - gap, cx + gap + length, cy - gap - length, color);
			d3dg->Draw2DLine(cx + gap + 1, cy - gap, cx + gap + length + 1, cy - gap - length, color);
			d3dg->Draw2DLine(cx - gap, cy + gap, cx - gap - length, cy + gap + length, color);
			d3dg->Draw2DLine(cx - gap + 1, cy + gap, cx - gap - length + 1, cy + gap + length, color);
			d3dg->Draw2DLine(cx + gap, cy + gap, cx + gap + length, cy + gap + length, color);
			d3dg->Draw2DLine(cx + gap + 1, cy + gap, cx + gap + length + 1, cy + gap + length, color);
		}
		else {
			// 일반 피격: 흰색 선
			int gap = 8;
			int length = 12;
			DWORD color = d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 0.8f);
			d3dg->Draw2DLine(cx - gap, cy - gap, cx - gap - length, cy - gap - length, color);
			d3dg->Draw2DLine(cx + gap, cy - gap, cx + gap + length, cy - gap - length, color);
			d3dg->Draw2DLine(cx - gap, cy + gap, cx - gap - length, cy + gap + length, color);
			d3dg->Draw2DLine(cx + gap, cy + gap, cx + gap + length, cy + gap + length, color);
		}
	}

	if( tag == true ){
		int color;
		if (ObjMgr.GetObjectInfoTag(draw_camera_x, draw_camera_y, draw_camera_z, draw_camera_rx, draw_camera_ry, &color, str) == true) {
			d3dg->Draw2DTextureFontTextCenterScaling(1, 270 +1 , str, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f), 14, 18);
			d3dg->Draw2DTextureFontTextCenterScaling(0, 270, str, color, 14, 18);
		}
	}

#ifdef ENABLE_DEBUGCONSOLE
	//AI긢긫긞긏륃뺪?렑
	if( AIdebuginfoID != -1 ){
		if( (0 <= AIdebuginfoID)&&(AIdebuginfoID < MAX_HUMAN) ){
			float posx, posy, posz, rx;
			int hp;
			char modestr[32];
			int EnemyID;
			int AIlevel;
			float mposx, mposz;
			int movemode;
			pointdata ppdata;
			ObjMgr.GetHumanObject(AIdebuginfoID)->GetPosData(&posx, &posy, &posz, &rx);
			hp = ObjMgr.GetHumanObject(AIdebuginfoID)->GetHP();
			HumanAI[AIdebuginfoID].GetBattleMode(NULL, modestr);
			EnemyID = HumanAI[AIdebuginfoID].GetEnemyHumanID();
			AIlevel = HumanAI[AIdebuginfoID].GetAIlevel();
			HumanAI[AIdebuginfoID].GetMoveTargetPos(&mposx, &mposz, &movemode);
			HumanAI[AIdebuginfoID].GetPathPointData(&ppdata);

			sprintf(str, "AI debug info [ID:%d]", AIdebuginfoID);
			d3dg->Draw2DTextureDebugFontText(20 +1, 130 +1, str, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
			d3dg->Draw2DTextureDebugFontText(20, 130, str, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));
			sprintf(str, "(X:%.2f Y:%.2f Z:%.2f RX:%.2f HP:%d)", posx, posy, posz, rx, hp);
			d3dg->Draw2DTextureDebugFontText(20 +1, 150 +1, str, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
			d3dg->Draw2DTextureDebugFontText(20, 150, str, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));
			sprintf(str, "Mode:%s  TargetEnemyID:%d  AIlevel:%d", modestr, EnemyID, AIlevel);
			d3dg->Draw2DTextureDebugFontText(20 +1, 170 +1, str, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
			d3dg->Draw2DTextureDebugFontText(20, 170, str, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));
			sprintf(str, "PointPath:[%d][%d][%d][%d]", ppdata.p1, ppdata.p2, ppdata.p3, ppdata.p4);
			d3dg->Draw2DTextureDebugFontText(20 +1, 190 +1, str, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
			d3dg->Draw2DTextureDebugFontText(20, 190, str, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));
			sprintf(str, "MovePosX:%.2f  MovePosZ:%.2f  (MoveMode:%d)", mposx, mposz, movemode);
			d3dg->Draw2DTextureDebugFontText(20 +1, 210 +1, str, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
			d3dg->Draw2DTextureDebugFontText(20, 210, str, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));
		}
	}
#endif


	RenderRadar();


	//-----------------------------------

	//Z긫긞긲?귩룊딖돸
	d3dg->ResetZbuffer();


	// 오른쪽 아래 3D 무기 모델 표시는 제거한다.
// 현재 들고 있는 무기는 1인칭 모델로 이미 보이므로,
// 우측 HUD는 1번/2번 무기 슬롯 정보만 담당한다.

	//-----------------------------------

#ifdef ENABLE_DEBUGCONSOLE
	if( Show_Console == true ){
		if( ScreenShot > 0 ){
			ScreenShot += 1;
		}
		else{
			RenderConsole();
		}
	}
#endif

	//-----------------------------------


	//긚??긣렄궴뢎뿹렄궻긳깋긞긏귺긂긣먠믦
	if( start_framecnt < (int)(2.0f*GAMEFPS) ){
		effect = GetEffectAlpha(start_framecnt, 1.0f, 2.0f, 0.0f, true);
	}
	else if( end_framecnt > 0 ){
		effect = GetEffectAlpha(end_framecnt, 1.0f, 3.0f + 0.5f, 0.0f, false);
	}
	else{
		effect = 0.0f;
	}
	d3dg->Draw2DBox(0, 0, GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight(), d3dg->GetColorCode(0.0f,0.0f,0.0f,effect));

	//뢎뿹렄궻빒럻?렑
	if( end_framecnt > 0 ){
		if( end_framecnt < (int)(1.0f*GAMEFPS) ){
			effect = GetEffectAlpha(end_framecnt, 1.0f, 1.0f, 0.0f, false);
		}
		else if( end_framecnt > (int)(3.0f*GAMEFPS) ){
			effect = GetEffectAlpha(end_framecnt, 1.0f, 1.0f, 3.0f, true);
		}
		else{
			effect = 1.0f;
		}

		if( GameInfoData.missioncomplete == true ){
			d3dg->Draw2DTextureFontTextCenterScaling(0, 240 + 10, "objective complete", d3dg->GetColorCode(1.0f,0.5f,0.0f,effect), 28, 32);
		}
		else{
			d3dg->Draw2DTextureFontTextCenterScaling(0, 240 + 10, "mission failure", d3dg->GetColorCode(1.0f,0.0f,0.0f,effect), 28, 32);
		}
	}


	time_render = GetTimeMS() - time;
}

//! @brief 깒???궸?됪궥귡띆뷭궸빾듂
//! @param in_x 뗴듩 X띆뷭
//! @param in_y 뗴듩 Y띆뷭
//! @param in_z 뗴듩 Z띆뷭
//! @param RadarPosX 깒???궻?됪 X띆뷭걁뜺뤵딈?걂
//! @param RadarPosY 깒???궻?됪 Y띆뷭걁뜺뤵딈?걂
//! @param RadarSize 깒???궻?됪긖귽긛
//! @param RadarWorldR 깒???궸?귽깛긣궥귡떁뿣
//! @param out_x 2D X띆뷭 귩롷궚롦귡?귽깛?
//! @param out_y 2D X띆뷭 귩롷궚롦귡?귽깛?
//! @param local_y 깓?긇깑 Y띆뷭 귩롷궚롦귡?귽깛?걁NULL됀걂
//! @param check 똶럁궻뤙뿪
//! @return 맟뚻갌true?렪봲갌false
//! @attention check긲깋긐귩뾎뚼궸궥귡궴갂깒???궔귞둖귢귡궞궴궕뼻귞궔궸궶궯궫렄?궳똶럁귩뢎뿹궢갂false귩뺅궢귏궥갃
bool maingame::GetRadarPos(float in_x, float in_y, float in_z, int RadarPosX, int RadarPosY, int RadarSize, float RadarWorldR, int* out_x, int* out_y, float* local_y, bool check)
{
	bool outf = false;
	float x, y, z;
	float x2, z2;

	//긇긽깋궴궻떁뿣귩똶럁
	x = in_x - camera_x;
	y = in_y - camera_y;
	z = in_z - camera_z;

	//뗟궚귢궽룉뿚궥귡
	if( (check == false) || ((fabsf(x) < RadarWorldR*2)&&(fabsf(z) < RadarWorldR*2)&&(fabsf(y) < 80.0f)) ){
		// 레이더는 한 프레임 안에서 GetRadarPos()를 매우 많이 호출한다.
// camera_rx가 같으면 sin/cos를 다시 계산하지 않고 재사용한다.
		static bool radar_trig_cache_valid = false;
		static float radar_trig_cache_rx = 0.0f;
		static float radar_trig_cache_sin = 0.0f;
		static float radar_trig_cache_cos = 1.0f;

		if ((radar_trig_cache_valid == false) || (radar_trig_cache_rx != camera_rx)) {
			radar_trig_cache_rx = camera_rx;
			radar_trig_cache_sin = sinf(camera_rx);
			radar_trig_cache_cos = cosf(camera_rx);
			radar_trig_cache_valid = true;
		}

		float sin_camera = radar_trig_cache_sin;
		float cos_camera = radar_trig_cache_cos;

		x2 = sin_camera * x - cos_camera * z;
		z2 = -(cos_camera * x + sin_camera * z);

		//뢁귏귡궔뵽믦
		if( (check == false) || ((fabsf(x2) < RadarWorldR)&&(fabsf(z2) < RadarWorldR)) ){
			//?됪띆뷭귩똶럁
			static bool radar_scale_cache_valid = false;
			static int radar_scale_cache_size = 0;
			static float radar_scale_cache_world_r = 0.0f;
			static float radar_scale_cache_value = 1.0f;

			if ((radar_scale_cache_valid == false) ||
				(radar_scale_cache_size != RadarSize) ||
				(radar_scale_cache_world_r != RadarWorldR)) {
				radar_scale_cache_size = RadarSize;
				radar_scale_cache_world_r = RadarWorldR;
				radar_scale_cache_value = (RadarSize / 2.0f) / RadarWorldR;
				radar_scale_cache_valid = true;
			}

			*out_x = (int)(RadarPosX + RadarSize / 2 + x2 * radar_scale_cache_value);
			*out_y = (int)(RadarPosY + RadarSize / 2 + z2 * radar_scale_cache_value);
			if( local_y != NULL ){ *local_y = y; }
			outf = true;
		}
	}

	return outf;
}

bool maingame::IsRadarTargetVisibleToPlayer(human* radar_player, float target_x, float target_y, float target_z)
{
	if (radar_player == NULL) { return true; }

	float player_x, player_y, player_z;
	radar_player->GetPosData(&player_x, &player_y, &player_z, NULL);

	// 플레이어 눈높이에서 대상 눈높이로 레이를 쏜다.
	player_y += VIEW_HEIGHT;

	float ray_x = target_x - player_x;
	float ray_y = target_y - player_y;
	float ray_z = target_z - player_z;

	float ray_len = sqrtf(ray_x * ray_x + ray_y * ray_y + ray_z * ray_z);
	if (ray_len <= 1.0f) {
		return true;
	}

	ray_x /= ray_len;
	ray_y /= ray_len;
	ray_z /= ray_len;

	float dist = 0.0f;

	bool hit_wall = CollD.CheckALLBlockIntersectRay(
		player_x,
		player_y,
		player_z,
		ray_x,
		ray_y,
		ray_z,
		NULL,
		NULL,
		&dist,
		ray_len
	);

	// 플레이어와 대상 사이에 벽이 있으면 레이더에서 숨긴다.
	return (hit_wall == false);
}

bool maingame::IsRadarTargetVisibleToTeam(int teamid, float target_x, float target_y, float target_z)
{
	// 플레이어를 포함한 같은 팀 인간 중 한 명이라도 대상과 벽 없이 연결되면 표시한다.
	for (int i = 0; i < MAX_HUMAN; i++) {
		human* observer = ObjMgr.GetHumanObject(i);
		if (observer == NULL) { continue; }
		if (observer->GetEnableFlag() == false) { continue; }
		if (observer->GetDeadFlag() == true) { continue; }

		int observer_teamid = -1;
		observer->GetParamData(NULL, NULL, NULL, &observer_teamid);
		if (observer_teamid != teamid) { continue; }

		if (IsRadarTargetVisibleToPlayer(observer, target_x, target_y, target_z) == true) {
			return true;
		}
	}

	return false;
}

//! @brief 듗댲깒????렑
void maingame::RenderRadar()
{
	// config에서 레이더가 비활성화된 경우 미니 레이더와 전체 지도를 모두 표시하지 않는다.
	if (GameConfig.GetRadarEnabledFlag() == false) {
		ShowFullMap = false;
		return;
	}

	// 기존 레이더와 전체 맵이 모두 꺼져있으면 함수를 빠져나갑니다.
	if (radar == false && ShowFullMap == false) { return; }

	// [✨ 추가됨] 해상도 비례 스케일링을 위한 배율과 매크로 정의
	float ui_scale = (float)GameConfig.GetScreenHeight() / 1080.0f;
#define SCALE(x) ((int)((x) * ui_scale))
#define SCALE_F(x) ((x) * ui_scale)

	int sw = GameConfig.GetScreenWidth();
	int sh = GameConfig.GetScreenHeight();

	int RadarSize;
	int RadarPosX;
	int RadarPosY;
	float RadarWorldR;

	// 원래의 카메라 시야각과 위치(X, Z)를 임시로 저장해 둡니다.
	float save_camera_rx = camera_rx;
	float save_camera_x = camera_x;
	float save_camera_z = camera_z;

	if (ShowFullMap) {
		// 전체 지도 크기
		RadarSize = (int)(sh * 0.85f);
		RadarPosX = (sw - RadarSize) / 2;
		RadarPosY = (sh - RadarSize) / 2;

		// 500.0f가 현재 기본/최대 확대 상태
		// 값이 커질수록 더 넓은 범위를 보여주므로 화면상으로는 축소됨
		RadarWorldR = FullMapWorldR;

		if (RadarWorldR < 500.0f) {
			RadarWorldR = 500.0f;
		}
		if (RadarWorldR > 2000.0f) {
			RadarWorldR = 2000.0f;
		}

		// 전체 지도는 회전하지 않음
		camera_rx = 0.0f;
	}
	else {
		// [✨ 수정됨] 미니맵 모드 크기와 여백에도 스케일링 적용
		RadarSize = SCALE(200);
		RadarPosX = SCALE(10);
		RadarPosY = SCALE(10);
		RadarWorldR = 300.0f;
	}

	float ecr = DISTANCE_CHECKPOINT / RadarWorldR * (RadarSize / 2);

	// 프레임 배경 및 테두리 그리기
	d3dg->Draw2DBox(RadarPosX, RadarPosY, RadarPosX + RadarSize, RadarPosY + RadarSize, d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.6f));
	d3dg->Draw2DLine(RadarPosX, RadarPosY, RadarPosX + RadarSize, RadarPosY, d3dg->GetColorCode(0.0f, 0.8f, 0.0f, 1.0f));
	d3dg->Draw2DLine(RadarPosX + RadarSize, RadarPosY, RadarPosX + RadarSize, RadarPosY + RadarSize, d3dg->GetColorCode(0.0f, 0.8f, 0.0f, 1.0f));
	d3dg->Draw2DLine(RadarPosX + RadarSize, RadarPosY + RadarSize, RadarPosX, RadarPosY + RadarSize, d3dg->GetColorCode(0.0f, 0.8f, 0.0f, 1.0f));
	d3dg->Draw2DLine(RadarPosX, RadarPosY + RadarSize, RadarPosX, RadarPosY, d3dg->GetColorCode(0.0f, 0.8f, 0.0f, 1.0f));


	// 맵 지형 라인 그리기
	int bs = BlockData.GetTotaldatas();
	for (int i = 0; i < bs; i++) {
		blockdata bdata;
		int vid[4];
		int bvx[4], bvy[4];

		BlockData.Getdata(&bdata, i);

		if (CollD.CheckBlockAABB(i, camera_x - RadarWorldR * 2, camera_y - 1.0f, camera_z - RadarWorldR * 2, camera_x + RadarWorldR * 2, camera_y + 1.0f, camera_z + RadarWorldR * 2) == true) {
			for (int j = 0; j < 6; j++) {
				float angle = acosf(bdata.material[j].vy);
				if ((HUMAN_MAPCOLLISION_SLOPEANGLE < angle) && (angle < DegreeToRadian(120))) {
					blockdataface(j, &(vid[0]), NULL);
					for (int k = 0; k < 4; k++) {
						GetRadarPos(bdata.x[vid[k]], bdata.y[vid[k]], bdata.z[vid[k]], RadarPosX, RadarPosY, RadarSize, RadarWorldR, &(bvx[k]), &(bvy[k]), NULL, false);
					}
					int line_x1, line_y1, line_x2, line_y2;
					// 4변 그리기
					if (Get2DLineInBox(bvx[0], bvy[0], bvx[1], bvy[1], RadarPosX, RadarPosY, RadarPosX + RadarSize, RadarPosY + RadarSize, &line_x1, &line_y1, &line_x2, &line_y2) == true) {
						d3dg->Draw2DLine(line_x1, line_y1, line_x2, line_y2, d3dg->GetColorCode(0.8f, 0.8f, 0.8f, 1.0f));
					}
					if (Get2DLineInBox(bvx[1], bvy[1], bvx[2], bvy[2], RadarPosX, RadarPosY, RadarPosX + RadarSize, RadarPosY + RadarSize, &line_x1, &line_y1, &line_x2, &line_y2) == true) {
						d3dg->Draw2DLine(line_x1, line_y1, line_x2, line_y2, d3dg->GetColorCode(0.8f, 0.8f, 0.8f, 1.0f));
					}
					if (Get2DLineInBox(bvx[2], bvy[2], bvx[3], bvy[3], RadarPosX, RadarPosY, RadarPosX + RadarSize, RadarPosY + RadarSize, &line_x1, &line_y1, &line_x2, &line_y2) == true) {
						d3dg->Draw2DLine(line_x1, line_y1, line_x2, line_y2, d3dg->GetColorCode(0.8f, 0.8f, 0.8f, 1.0f));
					}
					if (Get2DLineInBox(bvx[3], bvy[3], bvx[0], bvy[0], RadarPosX, RadarPosY, RadarPosX + RadarSize, RadarPosY + RadarSize, &line_x1, &line_y1, &line_x2, &line_y2) == true) {
						d3dg->Draw2DLine(line_x1, line_y1, line_x2, line_y2, d3dg->GetColorCode(0.8f, 0.8f, 0.8f, 1.0f));
					}
				}
			}
		}
	}

	// 이벤트 포인트 그리기
	if (EventStop == false) {
		for (int i = 0; i < TOTAL_EVENTLINE; i++) {
			signed short int p4 = Event[i].GetNextP4();
			pointdata data;
			if (PointData.SearchPointdata(&data, 0x08, 0, 0, 0, p4, 0) != 0) {
				float y;
				int x_2d, y_2d;
				float alpha;
				if ((data.p1 == 13) || (data.p1 == 16)) {
					data.y += VIEW_HEIGHT;
					if (GetRadarPos(data.x, data.y, data.z, RadarPosX, RadarPosY, RadarSize, RadarWorldR, &x_2d, &y_2d, &y, true) == true) {
						if ((fabsf(y) < 40.0f)) { alpha = 1.0f; }
						else { alpha = 0.5f; }
						d3dg->Draw2DCycle(x_2d, y_2d, (int)ecr, d3dg->GetColorCode(1.0f, 0.5f, 0.0f, alpha));
					}
				}
			}
		}
	}

	// 플레이어 정보 가져오기
	int PlayerID = ObjMgr.GetPlayerID();
	human* radar_player = ObjMgr.GetHumanObject(PlayerID);

	if (radar_player == NULL) {
		camera_rx = save_camera_rx;
		return;
	}

	int myteamid = -1;
	radar_player->GetParamData(NULL, NULL, NULL, &myteamid);

	// 레이더 벽 투과 OFF일 때 같은 적을 Pass 1/2에서 각각 다시 Ray 검사하지 않도록
	// 이번 프레임의 표시 가능 여부를 한 번만 계산한다.
	bool radar_human_visible[MAX_HUMAN];
	for (int i = 0; i < MAX_HUMAN; i++) {
		radar_human_visible[i] = false;
	}

	for (int i = 0; i < MAX_HUMAN; i++) {
		human* thuman = ObjMgr.GetHumanObject(i);
		if (thuman == NULL) { continue; }
		if (thuman->GetEnableFlag() == false) { continue; }
		if (thuman->GetDeadFlag() == true) { continue; }

		int tteamid = -1;
		thuman->GetParamData(NULL, NULL, NULL, &tteamid);

		if ((i == PlayerID) ||
			(tteamid == myteamid) ||
			(GameConfig.GetRadarSeeThroughWallsFlag() == true)) {
			radar_human_visible[i] = true;
		}
		else {
			float tx, ty, tz;
			thuman->GetPosData(&tx, &ty, &tz, NULL);

			// 레이더 범위 밖의 적은 벽 투과 Ray 검사까지 할 필요가 없다.
			if ((fabsf(tx - camera_x) >= RadarWorldR * 2) ||
				(fabsf(tz - camera_z) >= RadarWorldR * 2) ||
				(fabsf((ty + VIEW_HEIGHT) - camera_y) >= 80.0f)) {
				continue;
			}

			radar_human_visible[i] = IsRadarTargetVisibleToTeam(myteamid, tx, ty + VIEW_HEIGHT, tz);
		}
	}

	// -------------------------------------------------------------------------
	// Pass 1: 부채꼴(Cone) 먼저 그리기 (배경 레이어)
	// -------------------------------------------------------------------------
	for (int i = 0; i < MAX_HUMAN; i++) {
		if (i == PlayerID) continue;

		human* thuman = ObjMgr.GetHumanObject(i);
		if (thuman->GetEnableFlag() == false) continue;
		if (thuman->GetDeadFlag() == true) continue;

		int tteamid = -1;
		thuman->GetParamData(NULL, NULL, NULL, &tteamid);

		float tx, ty, tz, trx;
		int x_2d, y_2d;
		float y, alpha;

		thuman->GetPosData(&tx, &ty, &tz, &trx);
		ty += VIEW_HEIGHT;

		if (radar_human_visible[i] == false) { continue; }

		if (GetRadarPos(tx, ty, tz, RadarPosX, RadarPosY, RadarSize, RadarWorldR, &x_2d, &y_2d, &y, true) == true) {
			if ((fabsf(y) < 40.0f)) alpha = 1.0f;
			else alpha = 0.5f;

			float cone_fov = DegreeToRadian(60.0f);
			int segments = 20;

			int current_ai_mode;
			HumanAI[i].GetBattleMode(&current_ai_mode, NULL);
			DWORD cone_color;
			float base_cone_alpha = 0.2f;

			if (current_ai_mode == AI_ACTION) {
				cone_color = d3dg->GetColorCode(1.0f, 0.2f, 0.2f, base_cone_alpha * alpha);
			}
			else if (current_ai_mode == AI_CAUTION) {
				cone_color = d3dg->GetColorCode(1.0f, 1.0f, 0.0f, base_cone_alpha * alpha);
			}
			else {
				cone_color = d3dg->GetColorCode(1.0f, 1.0f, 1.0f, base_cone_alpha * alpha);
			}

			// [✨ 수정됨] 부채꼴(시야)의 기본 길이도 해상도에 비례하도록 스케일링 적용
			float cone_length = SCALE_F(40.0f);
			float radar_center_x = RadarPosX + (RadarSize / 2.0f);
			float radar_center_y = RadarPosY + (RadarSize / 2.0f);
			float dist_x = (float)x_2d - radar_center_x;
			float dist_y = (float)y_2d - radar_center_y;
			float dist_to_enemy = sqrtf(dist_x * dist_x + dist_y * dist_y);
			float max_allowed_length = (RadarSize / 2.0f) - dist_to_enemy;

			if (cone_length > max_allowed_length) cone_length = max_allowed_length;
			if (cone_length < 0) cone_length = 0;

			float radar_angle = trx + camera_rx - (float)M_PI / 2;
			float start_angle = radar_angle - (cone_fov / 2.0f);
			float end_angle = radar_angle + (cone_fov / 2.0f);

			d3dg->Draw2DDonutArc((float)x_2d, (float)y_2d, cone_length, start_angle, end_angle, (int)cone_length, segments, cone_color);
		}
	}

	// -------------------------------------------------------------------------
	// Pass 2: 점(Dot/Box) 나중에 그리기 (전경 레이어)
	// -------------------------------------------------------------------------
	for (int i = 0; i < MAX_HUMAN; i++) {
		human* thuman = ObjMgr.GetHumanObject(i);
		if (thuman->GetEnableFlag() == false) continue;
		if (thuman->GetDeadFlag() == true) continue;

		float tx, ty, tz, trx;
		int tteamid;
		int x_2d, y_2d;
		float y, alpha;
		int color;

		thuman->GetPosData(&tx, &ty, &tz, &trx);
		thuman->GetParamData(NULL, NULL, NULL, &tteamid);
		ty += VIEW_HEIGHT;

		if (radar_human_visible[i] == false) { continue; }

		if (GetRadarPos(tx, ty, tz, RadarPosX, RadarPosY, RadarSize, RadarWorldR, &x_2d, &y_2d, &y, true) == true) {
			if ((fabsf(y) < 40.0f)) alpha = 1.0f;
			else alpha = 0.5f;

			if (PlayerID == i) color = d3dg->GetColorCode(1.0f, 1.0f, 0.0f, alpha);
			else if (tteamid == myteamid) color = d3dg->GetColorCode(0.0f, 0.5f, 1.0f, alpha);
			else color = d3dg->GetColorCode(1.0f, 0.0f, 0.5f, alpha);

			// [✨ 수정됨] 캐릭터를 표시하는 점의 크기 스케일링 적용 (최소 1픽셀 보장)
			int dot_r = SCALE(3);
			if (dot_r < 1) dot_r = 1;

			d3dg->Draw2DBox(x_2d - dot_r, y_2d - dot_r, x_2d + dot_r, y_2d + dot_r, color);

			if (PlayerID == i) {
				// 방향은 이미 맞춘 값 유지
				float radar_angle = trx + camera_rx + (float)M_PI;

				float dir_x = cosf(radar_angle);
				float dir_y = sinf(radar_angle);

				// 플레이어 네모 점 크기
				int player_dot_r = SCALE(3);
				if (player_dot_r < 2) player_dot_r = 2;

				// 바라보는 방향 선 길이
				int sight_len;
				if (ShowFullMap) {
					sight_len = SCALE(28);   // M 키 전체 지도에서는 조금 길게
				}
				else {
					sight_len = SCALE(18);   // 미니맵에서는 짧게
				}
				if (sight_len < 12) sight_len = 12;

				int sight_x = x_2d + (int)(dir_x * sight_len);
				int sight_y = y_2d + (int)(dir_y * sight_len);

				int sight_color = d3dg->GetColorCode(1.0f, 1.0f, 0.0f, 0.45f);

				// 1. 방향선 먼저 그림
				d3dg->Draw2DLine(x_2d, y_2d, sight_x, sight_y, sight_color);

				// 2. 플레이어 위치 네모 점을 그 위에 그림
				d3dg->Draw2DBox(
					x_2d - player_dot_r,
					y_2d - player_dot_r,
					x_2d + player_dot_r,
					y_2d + player_dot_r,
					color
				);
			}
			else {
				d3dg->Draw2DBox(x_2d - dot_r, y_2d - dot_r, x_2d + dot_r, y_2d + dot_r, color);
			}
		}
	}

	// 중요 아이템(캔/PC) 점 그리기
	for (int j = 0; j < MAX_SMALLOBJECT; j++) {
		smallobject* tsmallobject = ObjMgr.GetSmallObject(j);
		if (tsmallobject == NULL) continue;
		if (tsmallobject->GetEnableFlag() == false) continue;
		if (tsmallobject->GetHP() <= 0) continue;

		int paramID;
		tsmallobject->GetParamData(&paramID, NULL);

		bool isTargetObject = false;
		if (paramID == 0) isTargetObject = true;
		else if (paramID >= 1 && paramID <= 6) isTargetObject = true;
		else if (paramID == 9 || paramID == 10) isTargetObject = true;
		else if (paramID >= 11) isTargetObject = true;

		if (isTargetObject == false) continue;

		float tx, ty, tz;
		int x_2d, y_2d;
		float y, alpha;

		tsmallobject->GetPosData(&tx, &ty, &tz, NULL);
		ty += VIEW_HEIGHT;

		if (GetRadarPos(tx, ty, tz, RadarPosX, RadarPosY, RadarSize, RadarWorldR, &x_2d, &y_2d, &y, true) == true) {
			if ((fabsf(y) < 40.0f)) alpha = 0.8f;
			else alpha = 0.4f;

			DWORD dot_color = d3dg->GetColorCode(0.0f, 1.0f, 0.0f, alpha);

			// [✨ 수정됨] 아이템을 표시하는 점의 크기 스케일링 적용
			int item_dot_r = SCALE(2);
			if (item_dot_r < 1) item_dot_r = 1;

			d3dg->Draw2DBox(x_2d - item_dot_r, y_2d - item_dot_r, x_2d + item_dot_r, y_2d + item_dot_r, dot_color);
		}
	}

	// =========================================================================
	// [새로 추가할 부분] 무기 및 Case 점 그리기
	// =========================================================================
	for (int k = 0; k < MAX_WEAPON; k++) {
		weapon* tweapon = ObjMgr.GetWeaponObject(k);
		if (tweapon == NULL) continue;
		if (tweapon->GetEnableFlag() == false) continue; // 비활성화된 무기 제외

		// [✨ 핵심 추가 코드] 누군가 줍거나 인벤토리에 있어 숨겨진 무기는 레이더에서 제외
		if (tweapon->GetUsingFlag() == true) continue;

		// 무기의 종류(파라미터 ID) 가져오기
		int weapon_paramid = 0;
		tweapon->GetParamData(&weapon_paramid, NULL, NULL);

		float tx, ty, tz;
		int x_2d, y_2d;
		float y, alpha;

		tweapon->GetPosData(&tx, &ty, &tz, NULL);
		ty += VIEW_HEIGHT; // 레이더 고도 보정

		// 레이더 반경 안에 들어오는지 계산
		if (GetRadarPos(tx, ty, tz, RadarPosX, RadarPosY, RadarSize, RadarWorldR, &x_2d, &y_2d, &y, true) == true) {
			if ((fabsf(y) < 40.0f)) alpha = 0.8f;
			else alpha = 0.4f;

			DWORD dot_color;
			int item_dot_r; // ✨ 점의 크기(반지름)를 결정할 변수를 여기서 선언

			// ID가 15번(Case)이면 주황색, 크기는 기존처럼 크게 유지
			if (weapon_paramid == 15) {
				dot_color = d3dg->GetColorCode(1.0f, 0.5f, 0.0f, alpha); // 주황색
				item_dot_r = SCALE(2);
				if (item_dot_r < 1) item_dot_r = 1; // 최소 크기 보장
			}
			// 그 외 일반 무기는 보라색, 크기는 아주 작게(1픽셀) 변경
			else {
				dot_color = d3dg->GetColorCode(0.8f, 0.2f, 0.8f, alpha); // 보라색
				item_dot_r = 1; // ✨ 반지름을 0으로 주어 화면에 1픽셀짜리 점으로만 찍히게 함
			}

			// 점 그리기
			d3dg->Draw2DBox(x_2d - item_dot_r, y_2d - item_dot_r, x_2d + item_dot_r, y_2d + item_dot_r, dot_color);
		}
	}

	// =========================================================================
// 설치형 스킬 위치 표시
// 지뢰와 교란 신호는 같은 색의 작은 점으로 표시한다.
// =========================================================================
	{
		DWORD device_dot_color = d3dg->GetColorCode(0.0f, 1.0f, 0.85f, 0.95f);
		int device_dot_r = SCALE(2);
		if (device_dot_r < 1) {
			device_dot_r = 1;
		}

		if (player_skill_distraction_timer > 0) {
			float y;
			int x_2d, y_2d;

			if (GetRadarPos(
				player_skill_distraction_x,
				player_skill_distraction_y + VIEW_HEIGHT,
				player_skill_distraction_z,
				RadarPosX,
				RadarPosY,
				RadarSize,
				RadarWorldR,
				&x_2d,
				&y_2d,
				&y,
				true
			) == true) {
				d3dg->Draw2DBox(
					x_2d - device_dot_r,
					y_2d - device_dot_r,
					x_2d + device_dot_r,
					y_2d + device_dot_r,
					device_dot_color
				);
			}
		}

		for (int i = 0; i < MAX_TERRORIST_MINES; i++) {
			if (player_skill_terrorist_mine_active[i] == false) {
				continue;
			}

			float y;
			int x_2d, y_2d;

			if (GetRadarPos(
				player_skill_terrorist_mine_x[i],
				player_skill_terrorist_mine_y[i] + VIEW_HEIGHT,
				player_skill_terrorist_mine_z[i],
				RadarPosX,
				RadarPosY,
				RadarSize,
				RadarWorldR,
				&x_2d,
				&y_2d,
				&y,
				true
			) == true) {
				d3dg->Draw2DBox(
					x_2d - device_dot_r,
					y_2d - device_dot_r,
					x_2d + device_dot_r,
					y_2d + device_dot_r,
					device_dot_color
				);
			}
		}
	}

	// =========================================================================
// 미니맵 아래 정보 표시
// =========================================================================
	if ((ShowFullMap == false) && (GameConfig.GetRadarInfoHudFlag() == true)) {
		char hudstr[128];

		int aliveEnemy = 0;
		int totalEnemy = 0;

		// 플레이어 팀 번호 가져오기
		int myteamid = -1;
		human* playerHuman = ObjMgr.GetHumanObject(PlayerID);
		if (playerHuman != NULL) {
			playerHuman->GetParamData(NULL, NULL, NULL, &myteamid);
		}

		// 남은 적 수 계산
		for (int i = 0; i < MAX_HUMAN; i++) {
			human* thuman = ObjMgr.GetHumanObject(i);
			if (thuman == NULL) continue;
			if (thuman->GetEnableFlag() == false) continue;

			int teamid = -1;
			thuman->GetParamData(NULL, NULL, NULL, &teamid);

			if (teamid != myteamid) {
				totalEnemy++;
				if (thuman->GetDeadFlag() == false) {
					aliveEnemy++;
				}
			}
		}

		int textX = RadarPosX;
		int textY = RadarPosY + RadarSize + SCALE(8);
		
		int lineH = SCALE(18);
		if (lineH < 16) lineH = 16;

		int textDrawX = textX + SCALE(4);

		DWORD shadowColor = d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 1.0f);
		DWORD textColor = d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f);

		// 배경 박스
		d3dg->Draw2DBox(
			textX,
			textY - SCALE(4),
			textX + RadarSize,
			textY + lineH * 3 + SCALE(4),
			d3dg->GetColorCode(0.0f, 0.0f, 0.0f, 0.45f)
		);


		// 진행 시간
		sprintf(hudstr, "TIME  %02d:%02d",
			framecnt / (int)GAMEFPS / 60,
			framecnt / (int)GAMEFPS % 60
		);
		d3dg->Draw2DTextureDebugFontText(textDrawX + 1, textY + 1, hudstr, shadowColor);
		d3dg->Draw2DTextureDebugFontText(textDrawX, textY, hudstr, textColor);

		// 킬 수
		sprintf(hudstr, "KILL  %d", MainGameInfo.kill);
		d3dg->Draw2DTextureDebugFontText(textDrawX + 1, textY + lineH + 1, hudstr, shadowColor);
		d3dg->Draw2DTextureDebugFontText(textDrawX, textY + lineH, hudstr, textColor);

		// 남은 적 수
		sprintf(hudstr, "ENEMY %d / %d", aliveEnemy, totalEnemy);
		d3dg->Draw2DTextureDebugFontText(textDrawX + 1, textY + lineH * 2 + 1, hudstr, shadowColor);
		d3dg->Draw2DTextureDebugFontText(textDrawX, textY + lineH * 2, hudstr, textColor);
	}

	// 전체 지도를 펼친 상태에서는 지도 위에 지도 전용 조작 안내를 표시한다.
	if (ShowFullMap == true) {
		RenderMapControlGuide(RadarPosX, RadarPosY, RadarSize);
	}

	camera_rx = save_camera_rx;
}


#ifdef ENABLE_DEBUGCONSOLE

//! @brief 긢긫긞긏뾭긓깛??깑궸륷궫궶빒럻쀱귩믁돿
//! @param color 빒럻궻륡
//! @param str 믁돿궥귡빒럻쀱궻?귽깛?
//! @attention 륷궢궋빒럻쀱궼륂궸돷궔귞믁돿궠귢귏궥갃
//! @attention ?렑됀?뛱릶걁믦릶갌MAX_CONSOLELINES걂귩뤵됷귡뤾뜃갂띍룊궻뛱걁1뛱뽞걂귩랁룣궢1뛱궦궰궦귞궢궫뤵궳갂덇붥돷궻뛱궸믁돿궢귏궥갃
void maingame::AddInfoConsole(int color, const char *str)
{
	for(int i=0; i<MAX_CONSOLELINES; i++){
		if( InfoConsoleData[i].textdata[0] == NULL ){
			InfoConsoleData[i].color = color;
			strcpy(InfoConsoleData[i].textdata, str);
			return;
		}
	}

	for(int i=1; i<MAX_CONSOLELINES; i++){
		memcpy(&(InfoConsoleData[i-1]), &(InfoConsoleData[i]), sizeof(ConsoleData));
	}
	InfoConsoleData[MAX_CONSOLELINES-1].color = color;
	strcpy(InfoConsoleData[MAX_CONSOLELINES-1].textdata, str);
}

//! @brief 긓?깛긤궻뵽믦궓귝귂덙릶걁맢릶뭠걂귩롦벦
//! @param cmd 뵽믦궥귡긓?깛긤빒럻궻?귽깛?
//! @param num ?궑귞귢궫덙릶귩롷궚롦귡?귽깛?
//! @return 롦벦갌true?뵽믦둖갌false
bool maingame::GetCommandNum(const char *cmd, int *num)
{
	if( cmd == NULL ){ return false; }
	if( num == NULL ){ return false; }

	//긓?깛긤뼹귩뮧귊귡
	if( memcmp(NewCommand, cmd, strlen(cmd)) != 0 ){ return false; }
	if( NewCommand[strlen(cmd)] != ' ' ){ return false; }

	//걏긓?깛긤뼹_X걐빁궻빒럻릶궸묪궢궲궋귡궔궵궎궔
	if( strlen(cmd)+2 > strlen(NewCommand) ){ return false; }

	//릶럻궕?궑귞귢궲궋귡궔
	for(int i=(int)strlen(cmd)+1; NewCommand[i] != '\0'; i++){
		if( ((NewCommand[i] < '0')||('9' < NewCommand[i]))&&(NewCommand[i] != '+')&&(NewCommand[i] != '-') ){ return false; }
	}

	//?궑귞귢궫릶럻귩뮧귊귡
	*num = atoi(&(NewCommand[ strlen(cmd)+1 ]));
	return true;
}

//! @brief 긢긫긞긏뾭긓깛??깑궻볺쀍룉뿚
void maingame::InputConsole()
{
	char str[MAX_CONSOLELEN];
	NewCommand[0] = '\0';

	if( ConsoleTextBoxControl.ProcessTextBox(inputCtrl) == true ){
		//[ENTER]
		//긓깛??깑궸믁돿궢갂뼟룉뿚긓?깛긤궴궢궲뱋?
		strcpy(str, CONSOLE_PROMPT);
		strcat(str, ConsoleTextBoxControl.GetTextBoxStr());
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		strcpy(NewCommand, ConsoleTextBoxControl.GetTextBoxStr());

		ConsoleTextBoxControl.SetTextBoxStr("");
	}
}

//! @brief 긢긫긞긏뾭긓깛??깑궻긽귽깛룉뿚
void maingame::ProcessConsole()
{
	char str[MAX_CONSOLELEN];
	int id;

	//긓?깛긤깏긚긣
	if( strcmp(NewCommand, "help") == 0 ){
		AddInfoConsole(d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), "help          human        result          event            skip <NUM>");
		AddInfoConsole(d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), "config        mif          bd1    pd1      resinfo          info");
		AddInfoConsole(d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), "view          center       map    model    aiinfo <NUM>     tag     hitbox");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "radar         inmap        sky <NUM>       dark     fog     teleport <NUM>");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "player <NUM>  revive       kill <NUM>      treat <NUM>      nodamage <NUM>");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "newobj <NUM>  break <NUM>  delhuman <NUM>  delweapon <NUM>  delobj <NUM>");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "nofight       caution      ailevel <NUM>   ff       bot     stop      estop");
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "speed         window       ss              clear            ver       exit");
	}

	//릐궻뱷똶륃뺪
	if( strcmp(NewCommand, "human") == 0 ){
		int alivemyfriend = 0;
		int myfriend = 0;
		int aliveenemy = 0;
		int enemy = 0;
		int myteamid;

		//긵깒귽깂?궻???붥뜂귩롦벦
		ObjMgr.GetPlayerHumanObject()->GetParamData(NULL, NULL, NULL, &myteamid);

		for(int i=0; i<MAX_HUMAN; i++){
			int teamid;
			bool deadflag;
			human *thuman = ObjMgr.GetHumanObject(i);
			if( thuman->GetEnableFlag() == true ){
				//?뻊륉뫴궴???붥뜂귩롦벦
				deadflag = thuman->GetDeadFlag();
				thuman->GetParamData(NULL, NULL, NULL, &teamid);

				//긇긂깛긣
				if( teamid == myteamid ){
					myfriend += 1;
					if( deadflag == false ){ alivemyfriend += 1; }
				}
				else{
					enemy += 1;
					if( deadflag == false ){ aliveenemy += 1; }
				}
			}
		}

		sprintf(str, "Friend:%d/%d  Enemy:%d/%d  Total:%d/%d",
			alivemyfriend, myfriend, aliveenemy, enemy, alivemyfriend + aliveenemy, myfriend + enemy);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
	}

	//럃믦깏긗깑긣?렑
	if( strcmp(NewCommand, "result") == 0 ){
		int intontarget;
		float rate;

		//뼺뭷뿦똶럁
		intontarget = (int)floor(MainGameInfo.ontarget);
		if( MainGameInfo.fire == 0 ){
			rate = 0.0f;
		}
		else{
			rate = (float)intontarget / MainGameInfo.fire * 100;
		}

		sprintf(str, "Time %02d:%02d  /  Fired %d  /  On target %d(%0.2f)", framecnt/(int)GAMEFPS/60, framecnt/(int)GAMEFPS%60, MainGameInfo.fire, intontarget, MainGameInfo.ontarget);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		sprintf(str, "AR rate %.1f%%  /  Kill %d  /  HS %d", rate, MainGameInfo.kill, MainGameInfo.headshot);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
	}

	//귽긹깛긣?긚긏?렑
	if( strcmp(NewCommand, "event") == 0 ){
		for(int i=0; i<TOTAL_EVENTLINE; i++){
			signed short int p4 = Event[i].GetNextP4();
			pointdata data;
			if( PointData.SearchPointdata(&data, 0x08, 0, 0, 0, p4, 0) == 0 ){
				sprintf(str, "Event %d   No task.", i);
			}
			else{
				sprintf(str, "Event %d   [%d][%d][%d][%d]", i, data.p1, data.p2, data.p3, data.p4);
			}
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		}
	}

	//귽긹깛긣?긚긏긚긌긞긵
	if( GetCommandNum("skip", &id) == true ){
		if( (0 <= id)&&(id < TOTAL_EVENTLINE) ){
			signed short int p4 = Event[id].GetNextP4();
			pointdata data;
			if( PointData.SearchPointdata(&data, 0x08, 0, 0, 0, p4, 0) == 0 ){
				sprintf(str, "Event %d   No task.", id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
			else{
				EventSkip[id] = true;

				sprintf(str, "Skipped event [%d].", id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
				sprintf(str, " ([%d][%d][%d][%d])", data.p1, data.p2, data.p3, data.p4);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}

	//config궻륃뺪
	if( strcmp(NewCommand, "config") == 0 ){
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "[Basic Config]");

		//긌?긓?긤?렑
		char str2[8];
		char str3[MAX_CONSOLELEN];
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), " Keycode : ");
		for(int i=0; i*10<TOTAL_ControlKey; i++){
			strcpy(str, " ");
			for(int j=0; j<10; j++){
				if( i*10+j >= TOTAL_ControlKey ){ break; }
				sprintf(str2, " 0x%02X", GameConfig.GetKeycode(i*10+j));
				strcat(str, str2);
			}
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		}

		strcpy(str, " ");

		//?긂긚뒾뱗
		sprintf(str3, "MouseSensitivity : %2d           ", GameConfig.GetMouseSensitivity());
		strcat(str, str3);

		//됪뽋?렑긾?긤
		if( GameConfig.GetFullscreenFlag() == false ){
			strcat(str, "FullscreenFlag : false (window)");
		}
		else{
			strcat(str, "FullscreenFlag : true (fullscreen)");
		}

		//?렑
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		strcpy(str, " ");

		//뚼됈돶먠믦
		if( GameConfig.GetSoundFlag() == false ){
			strcat(str, "SoundFlag : false (OFF)         ");
		}
		else{
			strcat(str, "SoundFlag : true (ON)           ");
		}

		//뢯뙆먠믦
		if( GameConfig.GetBloodFlag() == false ){
			strcat(str, "BloodFlag : false (OFF)");
		}
		else{
			strcat(str, "BloodFlag : true (ON)");
		}

		//?렑
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		strcpy(str, " ");

		//됪뽋궻뼻귡궠먠믦
		sprintf(str3, "Brightness : %2d                 ", GameConfig.GetBrightness());
		strcat(str, str3);

		//?긂긚뵿?먠믦
		if( GameConfig.GetInvertMouseFlag() == false ){
			strcat(str, "InvertMouseFlag : false (OFF)");
		}
		else{
			strcat(str, "InvertMouseFlag : true (ON)");
		}

		//?렑
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		strcpy(str, " ");

		//긲깒??긚긌긞긵먠믦
		if( GameConfig.GetFrameskipFlag() == false ){
			strcat(str, "FrameskipFlag : false (OFF)     ");
		}
		else{
			strcat(str, "FrameskipFlag : true (ON)       ");
		}

		//빶궻뤖?귩럊뾭먠믦
		if( GameConfig.GetAnotherGunsightFlag() == false ){
			strcat(str, "AnotherGunsightFlag : false (OFF)");
		}
		else{
			strcat(str, "AnotherGunsightFlag : true (ON)");
		}

		//?렑
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		strcpy(str, " ");

		//긵깒귽깂?뼹?렑
		char namestr[MAX_PLAYERNAME+1];
		GameConfig.GetPlayerName(namestr);
		strcat(str, "PlayerName : ");
		strcat(str, namestr);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "[Add Config]");

		strcpy(str, " ");

		//뙻뚭먠믦
		sprintf(str3, "Language : %d     ", GameConfig.GetLanguage());
		strcat(str, str3);

		//됪뽋됶몴뱗
		sprintf(str3, "ScreenSize : %dx%d     ", GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight());
		strcat(str, str3);

		//돶쀊먠믦
		sprintf(str3, "MasterVolume : %d%%", (int)(GameConfig.GetVolume()*100));
		strcat(str, str3);

		//?렑
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
	}

	//MIF궻륃뺪?렑
	if( strcmp(NewCommand, "mif") == 0 ){
		bool AddonFlag;
		int MissionID = GameInfoData.selectmission_id;
		char str2[MAX_PATH];
		char str3[MAX_PATH];
		bool collisionflag, screenflag;

		if( GameInfoData.selectmission_id >= 0 ){ AddonFlag = false; }
		else{ AddonFlag = true; }

		//긶긞??
		if( AddonFlag == true ){ sprintf(str, "[Addon Mission]   (MissionID:%d)", MissionID); }
		else{ sprintf(str, "[Standard Mission]   (MissionID:%d)", MissionID); }
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//MIF긲?귽깑긬긚
		strcpy(str, "MIFpath : ");
		strcpy(str2, GameInfoData.mifpath);
		str2[(MAX_CONSOLELEN - strlen(str) - 1)] = '\0';
		strcat(str, str2);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//?긞긘깈깛렞빶뼹
		strcpy(str, "Name : ");
		if( AddonFlag == true ){ strcpy(str2, MIFdata.GetMissionName()); }
		else{ GameParamInfo.GetOfficialMission(MissionID, str2, NULL, NULL, NULL, NULL, NULL); }
		str2[(MAX_CONSOLELEN - strlen(str) - 1)] = '\0';
		strcat(str, str2);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//?긞긘깈깛맫렜뼹뤝
		strcpy(str, "FullName : ");
		if( AddonFlag == true ){ strcpy(str2, MIFdata.GetMissionFullname()); }
		else{ GameParamInfo.GetOfficialMission(MissionID, NULL, str2, NULL, NULL, NULL, NULL); }
		str2[(MAX_CONSOLELEN - strlen(str) - 1)] = '\0';
		strcat(str, str2);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//긳깓긞긏긢??긲?귽깑
		strcpy(str, "BD1file : ");
		if( AddonFlag == true ){
			MIFdata.GetDatafilePath(str2, str3);
			str2[(MAX_CONSOLELEN - strlen(str) - 1)] = '\0';
			strcat(str, str2);
		}
		else{
			GameParamInfo.GetOfficialMission(MissionID, NULL, NULL, str2, NULL, NULL, NULL);
			str2[(MAX_CONSOLELEN - strlen(str) - 8 - 1)] = '\0';
			strcat(str, str2);
			strcat(str, OFFICIALMISSION_BD1);
		}
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//?귽깛긣긢??긲?귽깑
		strcpy(str, "PD1file : ");
		if( AddonFlag == true ){
			MIFdata.GetDatafilePath(str3, str2);
			str2[(MAX_CONSOLELEN - strlen(str) - 1)] = '\0';
			strcat(str, str2);
		}
		else{
			GameParamInfo.GetOfficialMission(MissionID, NULL, NULL, str2, str3, NULL, NULL);
			strcat(str2, str3);
			str2[(MAX_CONSOLELEN - strlen(str) - 4 - 1)] = '\0';
			strcat(str, str2);
			strcat(str, ".pd1");
		}
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//믁돿룷븿륃뺪긲?귽깑
		strcpy(str, "AddOBJfile : ");
		strcpy(str2, MIFdata.GetAddSmallobjectFile());
		str2[(MAX_CONSOLELEN - strlen(str) - 1)] = '\0';
		strcat(str, str2);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//둫먠믦뭠궴Flag
		if( AddonFlag == true ){
			collisionflag = MIFdata.GetCollisionFlag();
			screenflag = MIFdata.GetScreenFlag();
		}
		else{
			GameParamInfo.GetOfficialMission(MissionID, NULL, NULL, NULL, NULL, &collisionflag, &screenflag);
		}
		sprintf(str, "Sky:%d    CollisionFlag:%d    DarkScreenFlag:%d", MIFdata.GetSkynumber(), (int)collisionflag, (int)screenflag);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
	}

	//긳깓긞긏긢??궻륃뺪
	if( strcmp(NewCommand, "bd1") == 0 ){
		bool AddonFlag;
		int MissionID = GameInfoData.selectmission_id;
		char str2[MAX_PATH];
		char str3[MAX_PATH];
		char fname1[_MAX_PATH];
		char fname2[_MAX_PATH];
		char flagstr1[4];
		char flagstr2[4];

		if( GameInfoData.selectmission_id >= 0 ){ AddonFlag = false; }
		else{ AddonFlag = true; }

		//긲?귽깑뼹?렑
		strcpy(str, "Filename : ");
		if( AddonFlag == true ){
			MIFdata.GetDatafilePath(str2, str3);
			str2[(MAX_CONSOLELEN - strlen(str) - 1)] = '\0';
			strcat(str, str2);
		}
		else{
			GameParamInfo.GetOfficialMission(MissionID, NULL, NULL, str2, NULL, NULL, NULL);
			str2[(MAX_CONSOLELEN - strlen(str) - 8 - 1)] = '\0';
			strcat(str, str2); 
			strcat(str, OFFICIALMISSION_BD1);
		}
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//뜃똶긳깓긞긏릶?렑
		sprintf(str, "TotalBlocks : %d", BlockData.GetTotaldatas());
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//?긞긵긡긏긚?긿궻벶귒뜛귒륉떟?렑
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Texture : ");
		for(int i=0; i<(TOTAL_BLOCKTEXTURE/2); i++){
			//긡긏긚?긿ID귩롦벦
			BlockData.GetTexture(fname1, i);
			BlockData.GetTexture(fname2, i + (TOTAL_BLOCKTEXTURE/2));
			
			//긡긏긚?긿궕벶귒뜛귏귢궲궋귡궔뵽믦
			if( d3dg->GetMapTextureID(i) == -1 ){ strcpy(flagstr1, "NG"); }
			else{ strcpy(flagstr1, "OK"); }
			if( d3dg->GetMapTextureID(i + (TOTAL_BLOCKTEXTURE/2)) == -1 ){ strcpy(flagstr2, "NG"); }
			else{ strcpy(flagstr2, "OK"); }

			//?렑
			sprintf(str, " %02d_%s %-31s %02d_%s %s", i, flagstr1, fname1, i + (TOTAL_BLOCKTEXTURE/2), flagstr2, fname2);
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		}
	}

	//?귽깛긣긢??궻륃뺪
	if( strcmp(NewCommand, "pd1") == 0 ){
		bool AddonFlag;
		int MissionID = GameInfoData.selectmission_id;
		char str2[MAX_PATH];
		char str3[MAX_PATH];
		pointdata pdata;
		int TotalPoints = PointData.GetTotaldatas();
		int HumanPoints = 0;
		int WeaponPoints = 0;
		int OjectPoints = 0;
		int HumaninfoPoints = 0;
		int PathPoints = 0;
		int EventPoints = 0;
		int info_paramid1 = 0;
		int info_paramid2 = 0;
		int info_textureid1 = 0;
		int info_textureid2 = 0;
		char fname1[_MAX_PATH];
		char fname2[_MAX_PATH];
		char flagstr1[4];
		char flagstr2[4];

		if( GameInfoData.selectmission_id >= 0 ){ AddonFlag = false; }
		else{ AddonFlag = true; }

		//긲?귽깑뼹?렑
		strcpy(str, "Filename : ");
		if( AddonFlag == true ){
			MIFdata.GetDatafilePath(str3, str2);
			str2[(MAX_CONSOLELEN - strlen(str) - 1)] = '\0';
			strcat(str, str2);
		}
		else{
			GameParamInfo.GetOfficialMission(MissionID, NULL, NULL, str2, str3, NULL, NULL);
			strcat(str2, str3);
			str2[(MAX_CONSOLELEN - strlen(str) - 4 - 1)] = '\0';
			strcat(str, str2);
			strcat(str, ".pd1");
		}
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//뜃똶?귽깛긣릶?렑
		sprintf(str, "TotalPoints : %d", TotalPoints);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//둫롰?귽깛긣릶귩릶궑궲?렑
		for(int i=0; i<TotalPoints; i++){
			if( PointData.Getdata(&pdata, i) != 0 ){ continue; }

			if( (pdata.p1 == POINT_P1TYPE_HUMAN)||(pdata.p1 == POINT_P1TYPE_HUMAN2) ){ HumanPoints += 1; }
			if( (pdata.p1 == POINT_P1TYPE_WEAPON)||(pdata.p1 == POINT_P1TYPE_RAND_WEAPON) ){ WeaponPoints += 1; }
			if( pdata.p1 == POINT_P1TYPE_SMALLOBJ ){ OjectPoints += 1; }
			if( pdata.p1 == POINT_P1TYPE_HUMANINFO ){ HumaninfoPoints += 1; }
			if( (pdata.p1 == POINT_P1TYPE_AIPATH)||(pdata.p1 == POINT_P1TYPE_RAND_AIPATH) ){ PathPoints += 1; }
			if( (10 <= pdata.p1)&&(pdata.p1 <= 19) ){ EventPoints += 1; }
		}
		sprintf(str, "HumanPoints : %-3d       WeaponPoints : %-3d    SmallOjectPoints : %-3d", HumanPoints, WeaponPoints, OjectPoints);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		sprintf(str, "HumaninfoPoints : %-3d   AIpathPoints : %-3d    EventPoints : %-3d", HumaninfoPoints, PathPoints, EventPoints);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//뼰뚼궶?귽깛긣릶귩땤럁궳땫귕귡
		sprintf(str, "InvalidPoints : %d", TotalPoints - (HumanPoints + WeaponPoints + OjectPoints + HumaninfoPoints + PathPoints + EventPoints));
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//릐궻긡긏긚?긿릶귩롦벦
		sprintf(str, "HumanTextures : %d/%d", Resource.GetHumanTextures(), MAX_LOADHUMANTEXTURE);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//릐륃뺪긡긏긚?긿?궻벶귒뜛귒륉떟?렑
		for(int i=0; i<(MAX_LOADHUMANTEXTURE/2); i++){
			//긡긏긚?긿ID귩롦벦
			Resource.GetHumanTextureInfo(i, &info_paramid1, &info_textureid1);
			Resource.GetHumanTextureInfo(i + (MAX_LOADHUMANTEXTURE/2), &info_paramid2, &info_textureid2);

			if( GameParamInfo.GetHumanTexturePath(info_paramid1, fname1) == 1 ){ strcpy(fname1, ""); }
			if( GameParamInfo.GetHumanTexturePath(info_paramid2, fname2) == 1 ){ strcpy(fname2, ""); }

			//긡긏긚?긿궕벶귒뜛귏귢궲궋귡궔뵽믦
			if( info_textureid1 == -1 ){ strcpy(flagstr1, "NG"); }
			else{ strcpy(flagstr1, "OK"); }
			if( info_textureid2 == -1 ){ strcpy(flagstr2, "NG"); }
			else{ strcpy(flagstr2, "OK"); }

			//?렑
			sprintf(str, " %02d_%s %-31s %02d_%s %s", i, flagstr1, fname1, i + (MAX_LOADHUMANTEXTURE/2), flagstr2, fname2);
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		}
	}

	//깏??긚륃뺪?렑
	if( strcmp(NewCommand, "resinfo") == 0 ){
		int human = 0;
		int weapon = 0;
		int smallobject = 0;
		int bullet = 0;
		int grenade = 0;
		int effect = 0;

		//긐깋긲귻긞긏
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "[Graphics]");
		sprintf(str, " model : %2d/%2d        texture : %2d/%2d", d3dg->GetTotalModels(), MAX_MODEL, d3dg->GetTotalTextures(), MAX_TEXTURE);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//긖긂깛긤
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "[Sound]");
		sprintf(str, " sound : %2d/%2d", SoundCtrl.GetTotalSounds(), MAX_LOADSOUND);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//긢??긲?귽깑
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "[Datafile]");
		sprintf(str, " blocks : %3d/%3d     points : %3d/%3d", BlockData.GetTotaldatas(), MAX_BLOCKS, PointData.GetTotaldatas(), MAX_POINTS);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);

		//긆긳긙긃긏긣릶
		ObjMgr.GetTotalObjects(&human, &weapon, &smallobject, &bullet, &grenade, &effect);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "[Object]");
		sprintf(str, " human : %3d/%3d      weapon : %3d/%3d      smallobject : %2d/%2d", human, MAX_HUMAN, weapon, MAX_WEAPON, smallobject, MAX_SMALLOBJECT);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		sprintf(str, " bullet : %3d/%3d     grenade : %3d/%3d     effect : %3d/%3d", bullet, MAX_BULLET, grenade, MAX_GRENADE, effect, MAX_EFFECT);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		sprintf(str, " sound : %3d/%3d", GameSound->GetTotalSoundList(), MAX_SOUNDMGR_LIST);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
	}

	//긢긫긞긏뾭빒럻궻?렑
	if( strcmp(NewCommand, "info") == 0 ){
		if( ShowInfo_Debugmode == false ){
			ShowInfo_Debugmode = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Show Debug information.");
		}
		else{
			ShowInfo_Debugmode = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Hide Debug information.");
		}
	}

	//긲깏?긇긽깋먛귟뫶궑
	if( strcmp(NewCommand, "view") == 0 ){
		if( Camera_Debugmode == false ){
			Camera_Debugmode = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Change FreeView mode.");
		}
		else{
			Camera_Debugmode = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Change PlayerView mode.");
		}

		//긢긫긞긏긾?긤둎럑렄궻긇긽깋먠믦
		if( Camera_Debugmode == true ){
			camera_x = 100.0f;
			camera_y = 100.0f;
			camera_z = -100.0f;
			camera_rx = DegreeToRadian(135);
			camera_ry = DegreeToRadian(-40);
		}
	}

	//3D궻뭷륲멄?렑
	if( strcmp(NewCommand, "center") == 0 ){
		if( CenterLine == false ){
			CenterLine = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Show World center position.");
		}
		else{
			CenterLine = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Hide World center position.");
		}
	}

	//?긞긵귩깗귽깂?긲깒??돸
	if( strcmp(NewCommand, "map") == 0 ){
		if( wireframe == false ){
			wireframe = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Draw map on the Wire frame.");
		}
		else{
			wireframe = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Draw map on the Normal.");
		}
	}

	//긾긢깑?됪긲깋긐먛귟뫶궑
	if( strcmp(NewCommand, "model") == 0 ){
		if( nomodel == false ){
			nomodel = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Disable render model.");
		}
		else{
			nomodel = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Enable render model.");
		}
	}

	// 히트박스 표시 토글
	if (strcmp(NewCommand, "hitbox") == 0) {
		if (ObjMgr.GetShowHitboxFlag() == false) {
			ObjMgr.SetShowHitboxFlag(true);
			AddInfoConsole(d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), "Show human hitbox.");
		}
		else {
			ObjMgr.SetShowHitboxFlag(false);
			AddInfoConsole(d3dg->GetColorCode(1.0f, 1.0f, 1.0f, 1.0f), "Hide human hitbox.");
		}
	}

	//AI륃뺪?렑
	if( GetCommandNum("aiinfo", &id) == true ){
		if( (0 <= id)&&(id < MAX_HUMAN) ){
			if( AIdebuginfoID == id ){
				//벏궣붥뜂궕럚믦궠귢궫귞궶귞갂뼰뚼돸
				AIdebuginfoID = -1;
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Disable AI Debug information.");
			}
			else{
				//륷궫궸릐귩럚믦
				AIdebuginfoID = id;
				sprintf(str, "Enable AI Debug information. Human[%d].", id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}
	else if( strcmp(NewCommand, "aiinfo") == 0 ){
		if( AIdebuginfoID != -1 ){
			//릐궕럚믦궠귢궲궋궫귞갂뼰뚼돸
			AIdebuginfoID = -1;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Disable AI Debug information.");
		}
	}

	//긆긳긙긃긏긣궻?긐귩?렑
	if( strcmp(NewCommand, "tag") == 0 ){
		if( tag == false ){
			tag = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Show object information.");
		}
		else{
			tag = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Hide object information.");
		}
	}

	//깒???귩?됪
	if( strcmp(NewCommand, "radar") == 0 ){
		if( radar == false ){
			radar = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Show Radar.");
		}
		else{
			radar = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Hide Radar.");
		}
	}

	//뽞덨궢귩?됪
	if( strcmp(NewCommand, "inmap") == 0 ){
		if( Camera_Blind == false ){
			Camera_Blind = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Blindfold in map.");
		}
		else{
			Camera_Blind = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Not blindfold in map.");
		}
	}

	//봶똧뗴궻빾뛛
	if( GetCommandNum("sky", &id) == true ){
		if( (0 <= id)&&(id <= 5) ){
			SkyNumber = id;

			//깏??긚띋?뭱
			Resource.CleanupSkyModelTexture();
			Resource.LoadSkyModelTexture(SkyNumber);

			sprintf(str, "Select SkyNumber %d.", id);
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		}
	}

	//됪뽋귩댠궘
	if( strcmp(NewCommand, "dark") == 0 ){
		char path[MAX_PATH];
		char bdata[MAX_PATH];
		char pdata[MAX_PATH];

		//긲깋긐먛귟뫶궑
		if( DarkScreenFlag == false ){
			DarkScreenFlag = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Enable DarkScreen Flag.");
		}
		else{
			DarkScreenFlag = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Disable DarkScreen Flag.");
		}

		//.bd1궻긲?귽깑긬긚귩땫귕귡
		if( MIFdata.GetFiletype() == false ){
			GameParamInfo.GetOfficialMission(MainGameInfo.selectmission_id, NULL, NULL, path, NULL, NULL, NULL);
		}
		else{
			MIFdata.GetDatafilePath(bdata, pdata);
			GetFileDirectory(bdata, path);
		}

		//긳깓긞긏긢??룊딖돸
		BlockData.CalculationBlockdata(DarkScreenFlag);
		d3dg->CleanupMapdata();
		d3dg->LoadMapdata(&BlockData, path);
	}

	//긲긅긐먛귟뫶궑
	if( strcmp(NewCommand, "fog") == 0 ){
		//긲깋긐먛귟뫶궑
		if( FogFlag == false ){
			FogFlag = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Enable Fog Flag.");
		}
		else{
			FogFlag = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Disable Fog Flag.");
		}
	}

	//긵깒귽깂?댷벍
	if( GetCommandNum("teleport", &id) == true ){
		if( (0 <= id)&&(id < MAX_HUMAN)&&(id != ObjMgr.GetPlayerID()) ){
			float x, y, z, rx;
			human *thuman = ObjMgr.GetHumanObject(id);

			//??긒긞긣궻띆뷭롦벦
			thuman->GetPosData(&x, &y, &z, NULL);

			//긵깒귽깂?댷벍
			ObjMgr.GetPlayerHumanObject()->GetPosData(NULL, NULL ,NULL, &rx);
			ObjMgr.GetPlayerHumanObject()->SetPosData(x + 5.0f, y + 5.0f, z + 5.0f, rx);

			sprintf(str, "Teleported player to Human[%d].", id);
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		}
	}

	//긵깒귽깂?먛귟뫶궑
	if (GetCommandNum("player", &id) == true) {
		if ((0 <= id) && (id < MAX_HUMAN) && (id != ObjMgr.GetPlayerID())) {
			human* oldPlayerHuman = ObjMgr.GetPlayerHumanObject();

			// 콘솔 명령으로 플레이어를 바꾸는 경우에도 스킬 무기 상태를 먼저 복구/취소한다.
			ReturnPlayerDP28SkillWeapon(oldPlayerHuman);
			CancelPlayerTemporarySkillWeapon(oldPlayerHuman);

			//뫮뤭긵깒귽깂?붥뜂귩밙뾭
			ObjMgr.SetPlayerID(id);

			//긵깒귽깂?궻뛀궖귩롦벦
			ObjMgr.GetPlayerHumanObject()->GetRxRy(&mouse_rx, &mouse_ry);

			//F1긾?긤렄궸긇긽깋궻뛀궖귩띋먠믦
			if( Camera_F1mode == true ){
				camera_rx = DegreeToRadian(90);
			}

			sprintf(str, "Changed player to Human[%d].", id);
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		}
	}

	//멣궲궻?롌귩멻맯궥귡
	if( strcmp(NewCommand, "revive") == 0 ){
		for(int i=0; i<MAX_HUMAN; i++){
			ObjMgr.HumanResuscitation(i);
		}
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "All human has resuscitation.");
	}

	//됷븳궥귡
	if( GetCommandNum("treat", &id) == true ){
		if( (0 <= id)&&(id < MAX_HUMAN) ){
			int param, hp;
			HumanParameter data;
			human *thuman = ObjMgr.GetHumanObject(id);
			
			//룊딖렄궻HP귩롦벦
			thuman->GetParamData(&param, NULL, NULL, NULL);
			GameParamInfo.GetHuman(param, &data);
			hp = data.hp;

			//됷븳
			if( thuman->SetHP(hp) == true ){
				sprintf(str, "Set the HP:%d to Human[%d].", data.hp, id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}

	//뼰밎돸
	if( GetCommandNum("nodamage", &id) == true ){
		if( (0 <= id)&&(id < MAX_HUMAN) ){
			if( InvincibleID == id ){
				//벏궣붥뜂궕럚믦궠귢궫귞궶귞갂뼰뚼돸
				InvincibleID = -1;
				ObjMgr.GetHumanObject(id)->SetInvincibleFlag(false);
				sprintf(str, "Not invincible Human[%d].", id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
			else{
				//딓궸묿궔궕럚믦궠귢궲궋궫귞갂딓궸럚믦궠귢궲궋귡릐귩뼰뚼돸
				if( InvincibleID != -1 ){
					ObjMgr.GetHumanObject(InvincibleID)->SetInvincibleFlag(false);
					sprintf(str, "Not invincible Human[%d].", InvincibleID);
					AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
				}

				//륷궫궸뼰밎륉뫴궻릐귩럚믦
				InvincibleID = id;
				ObjMgr.GetHumanObject(id)->SetInvincibleFlag(true);
				sprintf(str, "Invincible Human[%d].", id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}
	else if( strcmp(NewCommand, "nodamage") == 0 ){
		if( InvincibleID != -1 ){
			//릐궕럚믦궠귢궲궋궫귞갂뼰뚼돸
			ObjMgr.GetHumanObject(InvincibleID)->SetInvincibleFlag(false);
			sprintf(str, "Not invincible Human[%d].", InvincibleID);
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			InvincibleID = -1;
		}
	}

	//랤둙
	if( GetCommandNum("kill", &id) == true ){
		if( (0 <= id)&&(id < MAX_HUMAN) ){
			human *thuman = ObjMgr.GetHumanObject(id);
			if( thuman->GetEnableFlag() == true ){
				if( thuman->SetHP(0) == true ){
					sprintf(str, "Killed Human[%d].", id);
					AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
				}
			}
		}
	}

	//룷븿귩륷딮봹뭫
	if( GetCommandNum("newobj", &id) == true ){
		if( (0 <= id)&&(id < TOTAL_PARAMETERINFO_SMALLOBJECT) ){
			int dataid = ObjMgr.AddSmallObjectIndex(camera_x + cosf(camera_rx)*20.0f, camera_y, camera_z + sinf(camera_rx)*20.0f, camera_rx*-1, id, true);
			if( dataid != -1 ){
				sprintf(str, "Add SmallObject[%d].", dataid);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}

	//룷븿귩봨됹
	if( GetCommandNum("break", &id) == true ){
		if( (0 <= id)&&(id < MAX_SMALLOBJECT) ){
			smallobject *tsmallobject = ObjMgr.GetSmallObject(id);
			if( tsmallobject->GetEnableFlag() == true ){
				tsmallobject->Destruction();
				sprintf(str, "Broke SmallObject[%d].", id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}

	//릐귩랁룣
	if( GetCommandNum("delhuman", &id) == true ){
		if( (0 <= id)&&(id < MAX_HUMAN) ){
			human *thuman = ObjMgr.GetHumanObject(id);
			if( thuman->GetEnableFlag() == true ){

				//릐궕렃궯궲궋귡븧딇귖궥귊궲뼰뚼궸궥귡
				int dummy;
				class weapon *weaponlist[TOTAL_HAVEWEAPON];
				int lnbs[TOTAL_HAVEWEAPON];
				int nbs[TOTAL_HAVEWEAPON];
				thuman->GetWeapon(&dummy, weaponlist, NULL, NULL);
				for(int i=0; i<TOTAL_HAVEWEAPON; i++){
					if( weaponlist[i] != NULL ){
						weaponlist[i]->SetEnableFlag(false);
						weaponlist[i] = NULL;
					}
					lnbs[i] = 0;
					nbs[i] = 0;
				}
				thuman->SetWeapon(weaponlist, lnbs, nbs);

				thuman->SetEnableFlag(false);
				sprintf(str, "Delete human[%d].", id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}

	//븧딇귩랁룣
	if( GetCommandNum("delweapon", &id) == true ){
		if( (0 <= id)&&(id < MAX_WEAPON) ){
			weapon *tweapon = ObjMgr.GetWeaponObject(id);
			if( tweapon->GetEnableFlag() == true ){
				
				//릐궕궩궻븧딇귩렃궯궲궋귡궶귞갂듫쁀븊궚귩둖궥걁롆궲궠궧귡걂
				int dummy;
				class weapon *weaponlist[TOTAL_HAVEWEAPON];
				int lnbs[TOTAL_HAVEWEAPON];
				int nbs[TOTAL_HAVEWEAPON];
				for(int i=0; i<MAX_HUMAN; i++){
					human *thuman = ObjMgr.GetHumanObject(i);
					if( thuman->GetEnableFlag() == true ){
						thuman->GetWeapon(&dummy, weaponlist, lnbs, nbs);
						for(int j=0; j<TOTAL_HAVEWEAPON; j++){
							if( weaponlist[j] == tweapon ){
								weaponlist[j] = NULL;
							}
						}
						thuman->SetWeapon(weaponlist, lnbs, nbs);
					}
				}

				tweapon->SetEnableFlag(false);
				sprintf(str, "Delete weapon[%d].", id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}

	//룷븿귩랁룣
	if( GetCommandNum("delobj", &id) == true ){
		if( (0 <= id)&&(id < MAX_SMALLOBJECT) ){
			smallobject *tsmallobject = ObjMgr.GetSmallObject(id);
			if( tsmallobject->GetEnableFlag() == true ){
				tsmallobject->SetEnableFlag(false);
				sprintf(str, "Delete SmallObject[%d].", id);
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
			}
		}
	}

	//FF걁벏럐뱼궭걂뾎뚼돸
	if( strcmp(NewCommand, "ff") == 0 ){
		if( ObjMgr.GetFriendlyFireFlag(0) == false ){	//궴귟궇궑궦 No.0 궳뵽믦
			for(int i=0; i<MAX_HUMAN; i++){
				ObjMgr.SetFriendlyFireFlag(i, true);
			}
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Enable Friendly Fire.");
		}
		else{
			for(int i=0; i<MAX_HUMAN; i++){
				ObjMgr.SetFriendlyFireFlag(i, false);
			}
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Disable Friendly Fire.");
		}
	}

	//긵깒귽깂??띿귩AI돸
	if( strcmp(NewCommand, "bot") == 0 ){
		if( PlayerAI == false ){
			PlayerAI = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Player is AI control. (bot)");
		}
		else{
			PlayerAI = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Player is User control.");
		}
	}

	//AI귩뷄먰벉돸궠궧귡
	if( strcmp(NewCommand, "nofight") == 0 ){
		if( AINoFight == false ){
			AINoFight = true;
			for(int i=0; i<MAX_HUMAN; i++){
				HumanAI[i].SetNoFightFlag(true);
			}
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "AI had no Fight.");
		}
		else{
			AINoFight = false;
			for(int i=0; i<MAX_HUMAN; i++){
				HumanAI[i].SetNoFightFlag(false);
			}
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "AI had Fight.");
		}
	}

	//AI귩똸둀궠궧귡
	if( strcmp(NewCommand, "caution") == 0 ){
		for(int i=0; i<MAX_HUMAN; i++){
			HumanAI[i].SetCautionMode();
		}
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Set cautious AI.");
	}

	//AI깒긹깑먠믦
	if( GetCommandNum("ailevel", &id) == true ){
		if( id == -1 ){
			//AI깒긹깑귩긢긲긅깑긣뭠귉뽣궥
			for(int i=0; i<MAX_HUMAN; i++){
				HumanAI[i].SetAIlevel(-1);
			}
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Set all human AI-level to default.");
		}
		if( (0 <= id)&&(id < TOTAL_PARAMETERINFO_AILEVEL) ){
			//AI깒긹깑귩럚믦뭠귉뤵룕궖
			for(int i=0; i<MAX_HUMAN; i++){
				HumanAI[i].SetAIlevel(id);
			}
			sprintf(str, "Set all human to AI-level [%d].", id);
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		}
	}
	else if( strcmp(NewCommand, "ailevel") == 0 ){
		//AI깒긹깑귩긢긲긅깑긣뭠귉뽣궥
		for(int i=0; i<MAX_HUMAN; i++){
			HumanAI[i].SetAIlevel(-1);
		}
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Set all human AI-level to default.");
	}

	//AI궻룉뿚귩믠?
	if( strcmp(NewCommand, "stop") == 0 ){
		if( AIstop == false ){
			AIstop = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Stopped AI control.");
		}
		else{
			AIstop = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Started AI control.");
		}
	}

	/*
	//?긞긘깈깛묪맟
	if( strcmp(NewCommand, "comp") == 0 ){
		end_framecnt += 1;
		MainGameInfo.missioncomplete = true;
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Mission complete.");
	}

	//?긞긘깈깛렪봲
	if( strcmp(NewCommand, "fail") == 0 ){
		end_framecnt += 1;
		MainGameInfo.missioncomplete = false;
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Mission fail.");
	}
	*/

	//귽긹깛긣?귽깛긣궻룉뿚귩믠?
	if( strcmp(NewCommand, "estop") == 0 ){
		if( EventStop == false ){
			EventStop = true;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Stopped Event chains.");
		}
		else{
			EventStop = false;
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Started Event chains.");
		}
	}

	//긫?긙깈깛륃뺪롦벦
	if( strcmp(NewCommand, "ver") == 0 ){
		sprintf(str, "%s   Version:%s", GAMENAME, GAMEVERSION);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		sprintf(str, " Graphics : %s", GRAPHICS_CORE);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		sprintf(str, " Sound : %s", SOUND_CORE);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		sprintf(str, " Input : %s", INPUT_CORE);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
	}

	//긒??궻렳뛱뫊뱗
	if( strcmp(NewCommand, "speed") == 0 ){
		if( GameSpeed == 1 ){ GameSpeed = 2; }
		else if( GameSpeed == 2 ){ GameSpeed = 4; }
		else{ GameSpeed = 1; }

		sprintf(str, "Set GameSpeed x%d", GameSpeed);
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
	}

	//긂귻깛긤긂갋긲깑긚긏깏?깛먛귟뫶궑
	if( strcmp(NewCommand, "window") == 0 ){
		//뙸띪궻?렑긾?긤롦벦
		bool flag = d3dg->GetFullScreenFlag();

		if( flag == false ){ flag = true; }
		else{ flag = false; }

		if( ChangeWindowMode(WindowCtrl, d3dg, inputCtrl, this, flag) != 0 ){
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "[Error] Change failed.");
		}
		else{
			if( flag == true ){
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Changed FullScreen mode.");
			}
			else{
				AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Changed Window mode.");
			}
		}
	}

	//긚긏깏?깛긘깈긞긣귩랡뎓
	//?겍긓깛??깑됪뽋귩랁룣궥귡궫귕갂랡뎓귩2긲깒??뭯귞궧귡갃
	if( ScreenShot == 3 ){
		char fname[256];

		//긲?귽깑뼹귩뙂믦
		GetTimeName(fname);
		strcat(fname, ".bmp");

		//랡뎓갋뺎뫔
		if( d3dg->SaveScreenShot(fname) == true ){
			sprintf(str, "Saved Screenshot  (File:%s)", fname);
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), str);
		}
		else{
			AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "[Error] Save failed.");
		}

		ScreenShot = 0;
	}
	else{
		if( strcmp(NewCommand, "ss") == 0 ){
			ScreenShot = 1;
		}
	}

	//긓깛??깑귩긏깏귺
	if( strcmp(NewCommand, "clear") == 0 ){
		for(int i=0; i<MAX_CONSOLELINES; i++){
			InfoConsoleData[i].color = d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f);
			InfoConsoleData[i].textdata[0] = '\0';
		}
	}

	//긓깛??깑귩빧궣귡
	if( strcmp(NewCommand, "exit") == 0 ){
		AddInfoConsole(d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), "Closed debug console.");
		Show_Console = false;
	}

#ifdef _DEBUG
	//깏긜긞긣?띿
	if( strcmp(NewCommand, "f12") == 0 ){
		GameState->PushF12Key();
		GameSpeed = 1;
	}
#endif

	if( strcmp(NewCommand, "mikan") == 0 ){
		AddInfoConsole(d3dg->GetColorCode(1.0f,0.5f,0.0f,1.0f), "Hello! I'm [-_-;](mikan).");
		AddInfoConsole(d3dg->GetColorCode(1.0f,0.5f,0.0f,1.0f), "Thank you for playing OpenXOPS!");
	}
}

//! @brief 긢긫긞긏뾭긓깛??깑궻?렑룉뿚
void maingame::RenderConsole()
{
	//돷뭤
	d3dg->Draw2DBox(0, 0, GameConfig.GetScreenWidth(), (MAX_CONSOLELINES+1)*17 + 5 + 5, d3dg->GetColorCode(0.0f,0.0f,0.0f,0.75f));

	//?렑뭷궻빒럻
	for(int i=0; i<MAX_CONSOLELINES; i++){
		if( InfoConsoleData[i].textdata[0] != NULL ){
			d3dg->Draw2DTextureDebugFontText(5+1, i*17+5+1, InfoConsoleData[i].textdata, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
			d3dg->Draw2DTextureDebugFontText(5, i*17+5, InfoConsoleData[i].textdata, InfoConsoleData[i].color);
		}
	}

	//볺쀍뭷궻빒럻
	int cnt;
	char str[MAX_CONSOLELEN];
	for(cnt=0; cnt<MAX_CONSOLELINES; cnt++){
		if( InfoConsoleData[cnt].textdata[0] == NULL ){ break; }
	}
	strcpy(str, CONSOLE_PROMPT);
	strcat(str, ConsoleTextBoxControl.GetTextBoxStr());
	d3dg->Draw2DTextureDebugFontText(5+1, cnt*17+5+1, str, d3dg->GetColorCode(0.0f,0.0f,0.0f,1.0f));
	d3dg->Draw2DTextureDebugFontText(5, cnt*17+5, str, d3dg->GetColorCode(1.0f,1.0f,0.0f,1.0f));
}

#endif


void maingame::CleanupMissionRuntime()
{
	// 미션 종료/재시작 시 실행 속도와 진행 상태를 먼저 정상화
	GameSpeed = 1;
	AIstop = true;
	EventStop = true;

	// 월드 사운드 이벤트 초기화
	if (GameSound != NULL) {
		GameSound->InitWorldSound();
	}

	// AI 상태 초기화
	for (int i = 0; i < MAX_HUMAN; i++) {
		HumanAI[i].Init();
	}

	// 인물, 시체, 드랍 무기, 총알, 수류탄, 피, 이펙트, 소형 오브젝트 정리
	ObjMgr.Cleanup();

	// 맵 충돌 상태 초기화
	CollD.InitCollision(&BlockData);

	// 맵 지형 데이터 해제
	if (d3dg != NULL) {
		d3dg->CleanupMapdata();
	}

	// 추가 소형 오브젝트 리소스 해제
	for (int i = 0; i < MAX_ADDSMALLOBJECT; i++) {
		Resource.CleanupAddSmallObject(i);
	}

	// 하늘 리소스 해제
	Resource.CleanupSkyModelTexture();

	// 마우스/카메라/상태값 일부 초기화
	mouse_rx = 0.0f;
	mouse_ry = 0.0f;
	view_rx = 0.0f;
	view_ry = 0.0f;
	add_camera_rx = 0.0f;
	add_camera_ry = 0.0f;

	framecnt = 0;
	start_framecnt = 0;
	end_framecnt = 0;
	message_id = -1;
	message_cnt = 0;

	grenade_trajectory_flag = false;
	grenade_trajectory_weapon_id = ID_WEAPON_NONE;
	player_run_toggle_state = false;
	player_crouch_toggle_state = false;
	player_move_crouch_priority = false;

	ResetPlayerSkillState();
}

void maingame::Destroy()
{
	CleanupMissionRuntime();

#ifdef ENABLE_DEBUGCONSOLE
	if (InfoConsoleData != NULL) {
		delete[] InfoConsoleData;
		InfoConsoleData = NULL;
	}
	ConsoleTextBoxControl.DestroyTextBox();
#endif

	GameState->NextState();
}

//! @brief 긓깛긚긣깋긏?
result::result()
{}

//! @brief 긢귻긚긣깋긏?
result::~result()
{}

//! @brief 깏긗깑긣됪뽋궻2D?됪븫빁
void result::Render2D()
{
	char mname[64];
	char str[32];
	float effectA = GetEffectAlphaLoop(framecnt, 1.0f, 0.8f, true);
	int intontarget;
	float rate;


	//뼺뭷뿦똶럁
	intontarget = (int)floor(GameInfoData.ontarget);
	if( GameInfoData.fire == 0 ){
		rate = 0.0f;
	}
	else{
		rate = (float)intontarget / GameInfoData.fire * 100;
	}

	//긽긾갌봶똧됪몴궻?됪궼갂렔벍밒궸뛱귦귢귡갃

	//뚂믦빒럻?렑
	d3dg->Draw2DTextureFontTextCenterScaling(0, 40, "RESULT", d3dg->GetColorCode(1.0f,0.0f,1.0f,effectA), 50, 42);

	//?긞긘깈깛뼹귩롦벦궢?렑
	if( MIFdata.GetFiletype() == false ){
		GameParamInfo.GetOfficialMission(GameInfoData.selectmission_id, NULL, mname, NULL, NULL, NULL, NULL);
	}
	else{
		strcpy(mname, MIFdata.GetMissionFullname());
	}
	d3dg->Draw2DTextureFontTextCenterScaling(0, 100, mname, d3dg->GetColorCode(0.5f,0.5f,1.0f,1.0f), 18, 25);

	//?긞긘깈깛긏깏귺?궻뾎뼰
	if( GameInfoData.missioncomplete == true ){
		d3dg->Draw2DTextureFontTextCenterScaling(0, 150, "mission successful", d3dg->GetColorCode(0.0f,1.0f,0.0f,1.0f), 24, 32);
	}
	else{
		d3dg->Draw2DTextureFontTextCenterScaling(0, 150, "mission failure", d3dg->GetColorCode(1.0f,0.0f,0.0f,1.0f), 24, 32);
	}

	//뙅됈?렑
	sprintf(str, "Time  %dmin %dsec", GameInfoData.framecnt/(int)GAMEFPS/60, GameInfoData.framecnt/(int)GAMEFPS%60);
	d3dg->Draw2DTextureFontTextCenterScaling(0, 210, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 20, 32);
	sprintf(str, "Rounds fired  %d", GameInfoData.fire);
	d3dg->Draw2DTextureFontTextCenterScaling(0, 260, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 20, 32);
	sprintf(str, "Rounds on target  %d", intontarget);
	d3dg->Draw2DTextureFontTextCenterScaling(0, 310, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 20, 32);
	sprintf(str, "Accuracy rate  %.1f%%", rate);
	d3dg->Draw2DTextureFontTextCenterScaling(0, 360, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 20, 32);
	sprintf(str, "Kill  %d / HeadShot  %d", GameInfoData.kill, GameInfoData.headshot);
	d3dg->Draw2DTextureFontTextCenterScaling(0, 410, str, d3dg->GetColorCode(1.0f,1.0f,1.0f,1.0f), 20, 32);

}

//! @brief screen봦맯긏깋긚궻룊딖돸걁긏깋긚궻먠믦걂
void InitScreen(WindowControl *WindowCtrl, opening *Opening, mainmenu *MainMenu, briefing *Briefing, maingame *MainGame, result *Result)
{
	if( WindowCtrl == NULL ){ return; }
	if( Opening == NULL ){ return; }
	if( MainMenu == NULL ){ return; }
	if( Briefing == NULL ){ return; }
	if( MainGame == NULL ){ return; }
	if( Result == NULL ){ return; }

	Opening->SetClass(&GameState, WindowCtrl, &d3dg, &inputCtrl, &GameSound);
	MainMenu->SetClass(&GameState, WindowCtrl, &d3dg, &inputCtrl, &GameSound);
	Briefing->SetClass(&GameState, WindowCtrl, &d3dg, &inputCtrl);
	MainGame->SetClass(&GameState, WindowCtrl, &d3dg, &inputCtrl, &GameSound);
	Result->SetClass(&GameState, WindowCtrl, &d3dg, &inputCtrl);
}

//! @brief screen봦맯긏깋긚궻렳뛱
void ProcessScreen(
	WindowControl* WindowCtrl,
	opening* Opening,
	mainmenu* MainMenu,
	briefing* Briefing,
	maingame* MainGame,
	result* Result,
	unsigned int framecnt,
	bool do_logic,
	bool do_render
)
{
	if (WindowCtrl == NULL) { return; }
	if (Opening == NULL) { return; }
	if (MainMenu == NULL) { return; }
	if (Briefing == NULL) { return; }
	if (MainGame == NULL) { return; }
	if (Result == NULL) { return; }

	// 로직도 렌더도 하지 않는 호출이면 바로 종료
	if ((do_logic == false) && (do_render == false)) {
		return;
	}

	int error;

	switch (GameState.GetState()) {
		// 긆?긵긦깛긐 룊딖돸
	case STATE_CREATE_OPENING:
		if (do_logic == true) {
			error = Opening->Create();
			if (error != 0) {
				if ((error & 0x0001) != 0) {
					WindowCtrl->ErrorInfo("block data open failed");
				}
				if ((error & 0x0002) != 0) {
					WindowCtrl->ErrorInfo("point data open failed");
				}
				//WindowCtrl->CloseWindow();
				GameState.PushBackSpaceKey();
			}
		}
		break;

		// 긆?긵긦깛긐 렳뛱
	case STATE_NOW_OPENING:
		if (do_logic == true) {
			Opening->Input();
			Opening->Process();
			Opening->Sound();
		}

		if (do_render == true) {
			if (Opening->RenderMain() == true) {
				int ReturnCode;
				ReturnCode = ResetGame(WindowCtrl);
				if (ReturnCode == 2) {
					WindowCtrl->ErrorInfo("Reset궸렪봲궢귏궢궫");
					WindowCtrl->CloseWindow();
				}
				else if (ReturnCode == 0) {
					Opening->Recovery();
				}
			}
		}
		break;

		// 긆?긵긦깛긐 봯딙
	case STATE_DESTROY_OPENING:
		if (do_logic == true) {
			Opening->Destroy();
		}
		break;

		// 긽긦깄? 룊딖돸
	case STATE_CREATE_MENU:
		if (do_logic == true) {
			error = MainMenu->Create();
			if (error != 0) {
				if ((error & 0x0001) != 0) {
					WindowCtrl->ErrorInfo("block data open failed");
				}
				if ((error & 0x0002) != 0) {
					WindowCtrl->ErrorInfo("point data open failed");
				}
				WindowCtrl->CloseWindow();
			}
		}
		break;

		// 긽긦깄? 렳뛱
	case STATE_NOW_MENU:
		if (do_logic == true) {
			MainMenu->Input();
			MainMenu->Process();
			MainMenu->Sound();
		}

		if (do_render == true) {
			if (MainMenu->RenderMain() == true) {
				int ReturnCode;
				ReturnCode = ResetGame(WindowCtrl);
				if (ReturnCode == 2) {
					WindowCtrl->ErrorInfo("Reset궸렪봲궢귏궢궫");
					WindowCtrl->CloseWindow();
				}
				else if (ReturnCode == 0) {
					MainMenu->Recovery();
				}
			}
		}
		break;

		// 긽긦깄? 봯딙
	case STATE_DESTROY_MENU:
		if (do_logic == true) {
			MainMenu->Destroy();
		}
		break;

		// 긳깏?긲귻깛긐 룊딖돸
	case STATE_CREATE_BRIEFING:
		if (do_logic == true) {
			error = Briefing->Create();
			if (error == 1) {
				WindowCtrl->ErrorInfo("briefing data open failed");
				WindowCtrl->CloseWindow();
			}
		}
		break;

		// 긳깏?긲귻깛긐 렳뛱
	case STATE_NOW_BRIEFING:
		if (do_logic == true) {
			Briefing->Input();
			Briefing->Process();
		}

		if (do_render == true) {
			if (Briefing->RenderMain() == true) {
				int ReturnCode;
				ReturnCode = ResetGame(WindowCtrl);
				if (ReturnCode == 2) {
					WindowCtrl->ErrorInfo("Reset궸렪봲궢귏궢궫");
					WindowCtrl->CloseWindow();
				}
				else if (ReturnCode == 0) {
					Briefing->Recovery();
				}
			}
		}
		break;

		// 긳깏?긲귻깛긐 봯딙
	case STATE_DESTROY_BRIEFING:
		if (do_logic == true) {
			Briefing->Destroy();
		}
		break;

		// 긽귽깛긒?? 룊딖돸
	case STATE_CREATE_MAINGAME:
		if (do_logic == true) {
			error = MainGame->Create();
			if (error != 0) {
				if ((error & 0x0001) != 0) {
					WindowCtrl->ErrorInfo("block data open failed");
				}
				if ((error & 0x0002) != 0) {
					WindowCtrl->ErrorInfo("point data open failed");
				}
				WindowCtrl->CloseWindow();
			}
		}
		break;

		// 긽귽깛긒?? 렳뛱
	case STATE_NOW_MAINGAME:
		if (do_logic == true) {
			for (int i = 0; i < MainGame->GetGameSpeed(); i++) {
				MainGame->Input();
				MainGame->Process();
				MainGame->Sound();
			}
		}

		if (do_render == true) {
			if (MainGame->RenderMain() == true) {
				int ReturnCode;
				ReturnCode = ResetGame(WindowCtrl);
				if (ReturnCode == 2) {
					WindowCtrl->ErrorInfo("Reset궸렪봲궢귏궢궫");
					WindowCtrl->CloseWindow();
				}
				else if (ReturnCode == 0) {
					MainGame->Recovery();
				}
			}
		}
		break;

		// 긽귽깛긒?? 봯딙
	case STATE_DESTROY_MAINGAME:
		if (do_logic == true) {
			MainGame->Destroy();
		}
		break;

		// 깏긗깑긣 룊딖돸
	case STATE_CREATE_RESULT:
		if (do_logic == true) {
			error = Result->Create();
		}
		break;

		// 깏긗깑긣 렳뛱
	case STATE_NOW_RESULT:
		if (do_logic == true) {
			Result->Input();
			Result->Process();
		}

		if (do_render == true) {
			if (Result->RenderMain() == true) {
				int ReturnCode;
				ReturnCode = ResetGame(WindowCtrl);
				if (ReturnCode == 2) {
					WindowCtrl->ErrorInfo("Reset궸렪봲궢귏궢궫");
					WindowCtrl->CloseWindow();
				}
				else if (ReturnCode == 0) {
					Result->Recovery();
				}
			}
		}
		break;

		// 깏긗깑긣 봯딙
	case STATE_DESTROY_RESULT:
		if (do_logic == true) {
			Result->Destroy();
		}
		break;

		// 긒?? 뢎뿹
	case STATE_EXIT:
		if (do_logic == true) {
			WindowCtrl->CloseWindow();
		}
		break;

	default:
		break;
	}
}

//! @brief 긂귻깛긤긂갋긲깑긚긏깏?깛먛귟뫶궑
//! @return 맟뚻갌0?렪봲걁긒??뫏뛱걂갌1?렪봲걁긒??뢎뿹걂갌2
int ChangeWindowMode(WindowControl *WindowCtrl, D3DGraphics *d3dg, InputControl *inputCtrl, scene *RecoveryScene, bool fullscreen)
{
	if( WindowCtrl == NULL ){ return 2; }
	if( d3dg == NULL ){ return 2; }
	if( inputCtrl == NULL ){ return 2; }
	if( RecoveryScene == NULL ){ return 2; }

	int returncode = 0;
	bool ErrorFlag = false;

	//먛귟뫶궑룉뿚걁룊됷걂
	WindowCtrl->ChangeWindowMode(fullscreen);
	d3dg->SetFullScreenFlag(fullscreen);
	if( ResetGame(WindowCtrl) != 0 ){
		if( fullscreen == false ){
			//긲깑긚긏깏?깛곆긂귻깛긤긂궕렪봲궢궫귞갂긄깋?궴궢궲뢎뿹갃
			WindowCtrl->ErrorInfo("Reset궸렪봲궢귏궢궫");
			WindowCtrl->CloseWindow();
			ErrorFlag = true;
			returncode = 2;
		}
		else{
			//긂귻깛긤긂곆긲깑긚긏깏?깛궕렪봲궢궫귞갂긂귻깛긤긂긾?긤귉뽣궢궲긒??뫏뛱귩럫귒귡갃
			//?걁GPU궕럚믦됶몴뱗궻긲깑긚긏깏?깛궸뫮돒궢궲궶궋궴궔갋갋갎걂
			fullscreen = false;

			WindowCtrl->ChangeWindowMode(fullscreen);
			d3dg->SetFullScreenFlag(fullscreen);
			if( ResetGame(WindowCtrl) != 0 ){
				//뽣궢궲귖렪봲궥귡궶귞갂긄깋?궴궢궲뢎뿹갃
				WindowCtrl->ErrorInfo("Reset궸렪봲궢귏궢궫");
				WindowCtrl->CloseWindow();
				ErrorFlag = true;
				returncode = 2;
			}
			else{
				returncode = 1;
			}
		}
	}

	//먛귟뫶궑궸맟뚻궢궫귞갂됷븳궶궵궻뚣룉뿚갃
	if( ErrorFlag == false ){
		RecoveryScene->Recovery();

		//긌?볺쀍귩롦벦
		//?겍긢귻긚긵깒귽됶몴뱗궻빾돸궸귝귡?긂긚궻댷벍빁귩롆궲귡
		inputCtrl->GetInputState(true);
		inputCtrl->MoveMouseCenter();
	}

	return returncode;
}

void maingame::RenderGrenadeTrajectoryDot(float x, float y, float z, float size, int color)
{
	d3dg->Renderline(x - size, y, z, x + size, y, z, color);
	d3dg->Renderline(x, y - size, z, x, y + size, z, color);
	d3dg->Renderline(x, y, z - size, x, y, z + size, color);
}

void maingame::RenderGrenadeTrajectoryLine(float x1, float y1, float z1, float x2, float y2, float z2, int color)
{
	d3dg->Renderline(x1, y1, z1, x2, y2, z2, color);
}

bool maingame::IsGrenadeTrajectoryPointVisibleFromCamera(float x, float y, float z)
{
	float vx = x - camera_x;
	float vy = y - camera_y;
	float vz = z - camera_z;

	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	if (dist <= 0.001f) {
		return true;
	}

	vx /= dist;
	vy /= dist;
	vz /= dist;

	// 목표 지점 바로 앞까지만 검사한다.
	// 이렇게 해야 궤적 점이 벽 표면/충돌 지점에 있을 때 자기 자신 때문에 숨겨지는 일을 줄일 수 있다.
	float check_dist = dist - 0.8f;

	if (check_dist <= 0.1f) {
		return true;
	}

	int block_id = -1;
	int face = -1;
	float hit_dist = 0.0f;

	bool blocked = CollD.CheckALLBlockIntersectRay(
		camera_x,
		camera_y,
		camera_z,
		vx,
		vy,
		vz,
		&block_id,
		&face,
		&hit_dist,
		check_dist
	);

	return (blocked == false);
}

void maingame::RenderGrenadeTrajectoryVisibleDot(float x, float y, float z, float size, int color)
{
	if (IsGrenadeTrajectoryPointVisibleFromCamera(x, y, z) == false) {
		return;
	}

	RenderGrenadeTrajectoryDot(x, y, z, size, color);
}

void maingame::RenderGrenadeTrajectoryVisibleLine(float x1, float y1, float z1, float x2, float y2, float z2, int color)
{
	// 긴 선 하나를 그대로 검사하면 일부만 벽 뒤에 있는 경우 처리가 거칠 수 있다.
	// 그래서 짧은 조각으로 나누어, 보이는 조각만 그린다.
	const int split_count = 5;

	for (int i = 0; i < split_count; i++) {
		float t1 = (float)i / (float)split_count;
		float t2 = (float)(i + 1) / (float)split_count;

		float sx = x1 + (x2 - x1) * t1;
		float sy = y1 + (y2 - y1) * t1;
		float sz = z1 + (z2 - z1) * t1;

		float ex = x1 + (x2 - x1) * t2;
		float ey = y1 + (y2 - y1) * t2;
		float ez = z1 + (z2 - z1) * t2;

		float mx = (sx + ex) * 0.5f;
		float my = (sy + ey) * 0.5f;
		float mz = (sz + ez) * 0.5f;

		if (IsGrenadeTrajectoryPointVisibleFromCamera(mx, my, mz) == true) {
			RenderGrenadeTrajectoryLine(sx, sy, sz, ex, ey, ez, color);
		}
	}
}

void maingame::RenderGrenadeTrajectory()
{
	human* myHuman = ObjMgr.GetPlayerHumanObject();

	if (ShouldRenderGrenadeTrajectory(myHuman) == false) {
		return;
	}

	float pos_x, pos_y, pos_z;
	float rotation_x, armrotation_y;
	WeaponParameter ParamData;

	myHuman->GetPosData(&pos_x, &pos_y, &pos_z, NULL);
	myHuman->GetRxRy(&rotation_x, &armrotation_y);

	int grenade_weapon_id = GetPlayerCurrentGrenadeTrajectoryWeaponID(myHuman);

	if (grenade_weapon_id == ID_WEAPON_NONE) {
		return;
	}

	bool impact_grenade_trajectory = (grenade_weapon_id == ID_WEAPON_IMPACT_GRENADE);

	if (GameParamInfo.GetWeapon(grenade_weapon_id, &ParamData) != 0) {
		return;
	}

	float crouch_y = 0.0f;
	if (myHuman->GetCrouchFlag() == true) {
		crouch_y = -2.0f;
	}

	float shot_y = pos_y + WEAPONSHOT_HEIGHT + crouch_y;

	// 실제 수류탄 생성 위치와 맞추기 위해 ShotWeapon()과 같은 총구 위치 계산 사용
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

	// 실제 ShotWeapon()의 수류탄 발사각과 동일
	float rx = rotation_x * -1 + (float)M_PI / 2.0f;
	float ry = armrotation_y;

	// 실제 grenade::SetParamData(8.0f, ...)와 동일
	float move_x = cosf(rx) * cosf(ry) * 8.0f;
	float move_y = sinf(ry) * 8.0f;
	float move_z = sinf(rx) * cosf(ry) * 8.0f;

	float x = muzzle_x;
	float y = muzzle_y;
	float z = muzzle_z;

	int normal_color = d3dg->GetColorCode(0.2f, 1.0f, 0.2f, 0.75f);
	int bounce_color = d3dg->GetColorCode(1.0f, 0.7f, 0.1f, 0.95f);

	// 너무 길면 화면이 지저분하므로 45프레임 정도만 예측
	const int max_steps = 45;

	const int first_visible_step = 3; // 시작 부분 3스텝은 그리지 않음

	for (int i = 0; i < max_steps; i++) {
		float speed = sqrtf(move_x * move_x + move_y * move_y + move_z * move_z);
		if (speed <= 0.001f) {
			break;
		}

		float prev_x = x;
		float prev_y = y;
		float prev_z = z;

		int block_id = -1;
		int face = -1;
		float dist = 0.0f;

		bool hit_wall = CollD.CheckALLBlockIntersectRay(
			x,
			y,
			z,
			move_x / speed,
			move_y / speed,
			move_z / speed,
			&block_id,
			&face,
			&dist,
			speed
		);

		if (hit_wall == true) {
			float hx = x + (move_x / speed) * dist;
			float hy = y + (move_y / speed) * dist;
			float hz = z + (move_z / speed) * dist;

			// 충돌 지점까지 선으로 표시
// 단, 시작 직후의 선은 무기 모델과 겹치므로 숨김.
// 카메라에서 벽 뒤에 가려진 선 조각은 표시하지 않는다.
			if (i >= first_visible_step) {
				RenderGrenadeTrajectoryVisibleLine(prev_x, prev_y, prev_z, hx, hy, hz, normal_color);
			}

			// 충돌 지점도 카메라에서 보이는 경우에만 표시한다.
			RenderGrenadeTrajectoryVisibleDot(hx, hy, hz, 3.5f, bounce_color);

			// Impact grenade는 실제 투척체도 충돌 즉시 폭발하므로,
			// 궤적도 첫 충돌 지점 이후는 표시하지 않는다.
			if (impact_grenade_trajectory == true) {
				break;
			}

			// 실제 grenade::ProcessObject()의 반사 처리와 비슷하게 예측
			float angle, acceleration;
			float vx, vy, vz;

			CollD.AngleVector(block_id, face, move_x, move_y, move_z, &angle);
			CollD.ReflectVector(block_id, face, move_x, move_y, move_z, &vx, &vy, &vz);

			acceleration = angle * -1.0f * 0.2546f + 0.7f;

			move_x = vx * acceleration;
			move_y = vy * acceleration;
			move_z = vz * acceleration;

			move_x *= 0.98f;
			move_y = (move_y - 0.17f) * 0.98f;
			move_z *= 0.98f;

			float new_speed = sqrtf(move_x * move_x + move_y * move_y + move_z * move_z);
			if (new_speed <= 0.001f) {
				break;
			}

			// 충돌점에서 아주 조금 띄워서 다음 충돌 반복을 줄임
			x = hx + (move_x / new_speed) * 0.2f;
			y = hy + (move_y / new_speed) * 0.2f;
			z = hz + (move_z / new_speed) * 0.2f;
		}
		else {
			x += move_x;
			y += move_y;
			z += move_z;

			if (i >= first_visible_step) {
				RenderGrenadeTrajectoryVisibleLine(prev_x, prev_y, prev_z, x, y, z, normal_color);

				if (i == first_visible_step) {
					RenderGrenadeTrajectoryVisibleDot(prev_x, prev_y, prev_z, 1.2f, normal_color);
				}
			}

			move_x *= 0.98f;
			move_y = (move_y - 0.17f) * 0.98f;
			move_z *= 0.98f;
		}
	}
}