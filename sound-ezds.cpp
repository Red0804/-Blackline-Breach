//! @file sound-directsound.cpp
//! @brief SoundControl긏깋긚궻믦?걁ezds.dll붎걂

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

#include "sound.h"

#if SOUND_ENGINE == 0

//! @brief 긓깛긚긣깋긏?
SoundControl::SoundControl()
{
	lib = NULL;
	DSver = NULL;
	DSinit = NULL;
	DSend = NULL;
	DSload = NULL;
	DSplay = NULL;
	DSrelease = NULL;

	//럊뾭띙귒긲깋긐귩룊딖돸
	for(int i=0; i<MAX_LOADSOUND; i++){
		useflag[i] = false;
	}

	camera_x = 0.0f;
	camera_y = 0.0f;
	camera_z = 0.0f;
	camera_rx = 0.0f;
	mastervolume = 1.0f;
	volume_max = 100;
}

//! @brief 긢귻긚긣깋긏?
SoundControl::~SoundControl()
{
	DestroySound();
}

//! @brief 룊딖돸@n
//! 걁DLL궻깓?긤갂룊딖돸듫릶궻렳뛱걂
//! @param WindowCtrl WindowControl긏깋긚궻?귽깛?
//! @return 맟뚻갌0?렪봲갌1
int SoundControl::InitSound(WindowControl *WindowCtrl)
{
	if( WindowCtrl == NULL ){ return 1; }

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_INIT, "Sound", "ezds.dll");
#endif

	if( lib != NULL ){
		return 1;
	}

	//DLL귩벶귒뜛귔
	lib = LoadLibrary("ezds.dll");
	if( lib == NULL ){
		return 1;
	}

	//듫릶귩뒆귟뱰궲
	DSver = GetProcAddress(lib, "DSver");
	DSinit = (FARPROCH)GetProcAddress(lib, "DSinit");
	DSend = GetProcAddress(lib, "DSend");
	DSload = (FARPROCCI)GetProcAddress(lib, "DSload");
	DSplay = (FARPROCIII)GetProcAddress(lib, "DSplay");
	DSrelease = (FARPROCI)GetProcAddress(lib, "DSrelease");

	if( DSinit != NULL ){
		//DLL룊딖돸귩렳뛱
		if( DSinit(WindowCtrl->GethWnd()) != 0 ){
#ifdef ENABLE_DEBUGLOG
			//깓긐궸뢯쀍
			OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif
			return 0;
		}
	}

	//DLL귩됶뺳
	FreeLibrary(lib);
	lib = NULL;

	//return 1;
	return 0;
}

//! @brief 됶뺳
//! @attention ?듫릶궼렔벍밒궸뚁귂뢯궠귢귏궥궕갂뼻렑밒궸뚁귂뢯궥궞궴귖됀?궳궥갃
void SoundControl::DestroySound()
{
	if( lib == NULL ){ return; }

	//럊뾭뭷궻긖긂깛긤긢??귩됶뺳
	for(int i=0; i<MAX_LOADSOUND; i++){
		if( useflag[i] == true ){
			if( DSrelease != NULL ){ DSrelease(i); }
			useflag[i] = false;
		}
	}

	//DLL귩뢎뿹
	if( DSend != NULL ){ DSend(); }

	//DLL귩됶뺳
	FreeLibrary(lib);
	lib = NULL;

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CLEANUP, "Sound", "ezds.dll");
#endif
}

//! @brief 띋맯돶쀊귩먠믦
//! @param volume 띋맯돶쀊?걁0.0걖뼰돶?1.0걖100걪걂
//! @attention 0.01걁1걪걂뼟뼖궼 0.00걁0걪걂궴궢궲룉뿚궠귢귏궥갃
void SoundControl::SetVolume(float volume)
{
	mastervolume = volume;
	if( mastervolume < 0.01f ){ mastervolume = 0.0f; }
	if( mastervolume > 1.00f ){ mastervolume = 1.0f; }

	//dB귩뛩뿶궢궲띍묈돶쀊귩뙂믦
	if( mastervolume == 0.0f ){ volume_max = -100; return; }
	if( mastervolume == 1.0f ){ volume_max = 100; return; }
	volume_max = 100 - (int)(log10f(mastervolume)*-0.2f*200);
}

//! @brief 긇긽깋궻띆뷭궴둷뱗귩먠믦
//! @param x 긇긽깋궻X띆뷭
//! @param y 긇긽깋궻Y띆뷭
//! @param z 긇긽깋궻Z띆뷭
//! @param rx 긇긽깋궻X렡둷뱗?걁?뽵걂
//! @warning 뻽긲깒??뚁귂뢯궢궲갂띍륷궻긇긽깋띆뷭귩먠믦걁밙뾭걂궢궲궘궬궠궋갃
void SoundControl::SetCamera(float x, float y, float z, float rx)
{
	camera_x = x;
	camera_y = y;
	camera_z = z;
	camera_rx = rx;
}

//! @brief 긖긂깛긤귩벶귒뜛귔
//! @param filename 긲?귽깑뼹
//! @return 맟뚻갌0댥뤵궻긢??붥뜂?렪봲갌-1
int SoundControl::LoadSound(const char* filename)
{
	if( lib == NULL ){ return -1; }
	if( filename == NULL ){ return -1; }

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_LOAD, "Sound", filename);
#endif

	//럊뾭궢궲궋궶궋긢??붥뜂귩뭈궥
	for(int i=0; i<MAX_LOADSOUND; i++){
		if( useflag[i] == false ){

#ifdef ENABLE_PATH_DELIMITER_SLASH
			//긬긚뗦먛귟빒럻귩빾듂
			filename = ChangePathDelimiter(filename);
#endif

			//벶귒뜛귒귩럫귒귡
			if( DSload == NULL ){ return -1; }
			if( DSload(filename, i) == 0 ){ return -1; }

			//룷궠궋돶쀊궳덇뱗띋맯궢궲궓궘
			if( DSplay == NULL ){ return -1; }
			DSplay(i, -100, 0);

			//럊뾭뭷귩?궥긲깋긐귩긜긞긣
			useflag[i] = true;

#ifdef ENABLE_DEBUGLOG
			//깓긐궸뢯쀍
			OutputLog.WriteLog(LOG_COMPLETE, "", i);
#endif
			return i;
		}
	}

	return -1;
}

void SoundControl::StopSound(int id)
{
	// ezds.dll 쪽은 개별 재생 중지 API가 없으므로 아무것도 하지 않는다.
	// 현재 프로젝트 기본 SOUND_ENGINE == 2에서는 sound-directsound.cpp의 StopSound()가 사용된다.
}

//! @brief TEhĐi3DĐj
//! @param id f??ԍ
//! @param x XW

//! @brief 긖긂깛긤귩띋맯걁뷄3D띋맯DLL뚁귂뢯궢걂
//! @param id 긢??붥뜂
//! @param volume 띋맯?깏깄??
//! @param pan 긬깛걁뜺덭긫깋깛긚걂
//! @return 맟뚻갌1?3?렪봲갌0
//! @note 뾭뱑갌긵깒귽깂?렔릆궕뵯맯궥귡돶갋긒??뗴듩멣뫬궳뗉덇궸뼿귞궥돶갋BGM
int SoundControl::PlaySound(int id, int volume, int pan)
{
	if( lib == NULL ){ return 0; }
	if( (id < 0)||(MAX_LOADSOUND <= id) ){ return 0; }
	if( useflag[id] == false ){ return 0; }

	if( volume < -100 ){ volume = -100; }
	if( volume > 100 ){ volume = 100; }
	if( pan < -100 ){ pan = -100; }
	if( pan > 100 ){ pan = 100; }

	//긖긂깛긤귩띋맯
	if( DSplay == NULL ){ return 0; }
	//return DSplay(id, volume, pan);
	int volume2 = (int)(1.0f/200 * (volume + 100) * (volume_max + 100) - 100);
	return DSplay(id, volume2, pan);
}

//! @brief 긖긂깛긤귩띋맯걁3D띋맯걂
//! @param id 긢??붥뜂
//! @param x 돶뙶궻X띆뷭
//! @param y 돶뙶궻Y띆뷭
//! @param z 돶뙶궻Z띆뷭
//! @param volume 띋맯?깏깄??
//! @return 맟뚻갌1?3?렪봲갌0
//! @note 뾭뱑갌먥뫮밒궶댧뭫귩렃궭떁뿣궸귝귟뙵릨궥귡갂덇붗밒궶뚼됈돶갃
int SoundControl::Play3DSound(int id, float x, float y, float z, int volume)
{
	if( (id < 0)||(MAX_LOADSOUND <= id) ){ return 0; }
	if( useflag[id] == false ){ return 0; }

	if( volume < -100 ){ volume = -100; }
	if( volume > 100 ){ volume = 100; }

	float dist;
	int playvolume;
	int pan = 0;
	
	//떁뿣궸귝귡띋맯돶쀊뙂믦
	if( CheckSourceDist(x, y, z, false, &dist) == false ){
		return 0;
	}
	playvolume = CalculationVolume(volume, dist, false);

	/*
	//뜺덭궻긬깛걁띋맯긫깋깛긚걂궻뙂믦
	float vx = x - camera_x;
	float vz = z - camera_z;
	float rx = (atan2f(vz, vx) - camera_rx) * -1;
	rx = AngleNormalization(rx);
	pan = (int)((float)10 / M_PI * rx);
	*/

	//DLL뚁귂뢯궢
	return PlaySound(id, playvolume, pan);
}

//! @brief 벶귒뜛귒띙귒궻긖긂깛긤릶귩롦벦
//! @return 긖긂깛긤릶
int SoundControl::GetTotalSounds()
{
	int cnt = 0;

	//럊뾭뭷궻뾴멹귩릶궑귡
	for(int i=0; i<MAX_LOADSOUND; i++){
		if( useflag[i] == true ){ cnt += 1; }
	}

	return cnt;
}

//! @brief 긖긂깛긤귩됶뺳
//! @param id 긢??붥뜂
void SoundControl::CleanupSound(int id)
{
	if( lib == NULL ){ return; }
	if( (id < 0)||(MAX_LOADSOUND <= id) ){ return; }
	if( useflag[id] == false ){ return; }

	//럊뾭뭷궻긖긂깛긤긢??귩됶뺳
	if( DSrelease != NULL ){ DSrelease(id); }

	//럊뾭뭷긲깋긐귩됶룣
	useflag[id] = false;

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CLEANUP, "Sound", id);
#endif
}

//! @brief 돶뙶궴궻떁뿣귩뮧귊귡
//! @param x 돶뙶궻X띆뷭
//! @param y 돶뙶궻Y띆뷭
//! @param z 돶뙶궻Z띆뷭
//! @param snear 뗟떁뿣돶뙶
//! @param out_dist 떁뿣
//! @return 뾎뚼걁볙걂갌true?뼰뚼걁둖걂갌false
bool SoundControl::CheckSourceDist(float x, float y, float z, bool snear, float *out_dist)
{
	float dx, dy, dz, dist;
	int max_dist;

	if( snear == false ){
		max_dist = MAX_SOUNDDIST;
	}
	else{
		max_dist = 30;
	}

	dx = camera_x - x;
	dy = camera_y - y;
	dz = camera_z - z;
	dist = dx*dx + dy*dy + dz*dz;
	if( dist > max_dist * max_dist ){
		*out_dist = 0.0f;
		return false;
	}

	*out_dist = sqrtf(dist);
	return true;
}

//! @brief 돶쀊귩똶럁
//! @param MaxVolume 돶뙶궻띍묈돶쀊
//! @param dist 돶뙶궴궻떁뿣
//! @param snear 뗟떁뿣돶뙶
int SoundControl::CalculationVolume(int MaxVolume, float dist, bool snear)
{
	int max_dist;
	if( snear == false ){
		max_dist = MAX_SOUNDDIST;
	}
	else{
		max_dist = 30;
	}
	return (int)( (float)MaxVolume/max_dist*dist*-1 + MaxVolume );
}

#endif	//SOUND_ENGINE