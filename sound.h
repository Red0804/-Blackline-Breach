//! @file sound.h
//! @brief SoundControl긏깋긚궻먬뙻

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

#ifndef SOUND2_H
#define SOUND2_H

#define MAX_LOADSOUND 256		//!< 사운드를 읽어올 수 있는 최대 수
#define MAX_SOUNDLISTS 24		// 같은 사운드를 동시에 겹쳐 재생할 수 있는 DirectSound 보조 버퍼 수.
#define MAX_SOUNDDIST 335		//!< 사운드를 재생하는 최대 거리

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 1		//!< Select include file.
#endif
#include "main.h"
#include <windows.h>

#define SOUND_ENGINE 2		//!< @brief 긖긂깛긤궻띋맯깋귽긳깋깏귩멗묖 @details ezds.dll갌0?DirectSound걁2D띋맯걂갌1?DirectSound걁3D띋맯걂갌2

#if SOUND_ENGINE == 0
 typedef int (*FARPROCH)(HWND);					//!< DLL Parameter
 typedef int (*FARPROCCI)(const char*, int);	//!< DLL Parameter
 typedef int (*FARPROCIII)(int, int, int);		//!< DLL Parameter
 typedef int (*FARPROCI)(int);					//!< DLL Parameter

 #define SOUND_CORE "ezds"					//!< 긫?긙깈깛?렑뾭륃뺪
#else	//#if SOUND_ENGINE
 #include <dsound.h>
 #include <mmsystem.h>
 #pragma comment(lib, "dsound.lib")
 #pragma comment(lib, "dxguid.lib")
 #pragma comment(lib, "winmm.lib")
 #ifndef WAVE_FORMAT_IEEE_FLOAT
  #define WAVE_FORMAT_IEEE_FLOAT 3
 #endif

 #if SOUND_ENGINE == 1
  #define SOUND_CORE "DirectSound(Monaural)"	//!< 긫?긙깈깛?렑뾭륃뺪
 #else
  #define SOUND_CORE "DirectSound(Stereo)"	//!< 긫?긙깈깛?렑뾭륃뺪
 #endif
#endif	//#if SOUND_ENGINE

//! @brief 긖긂깛긤귩띋맯궥귡긏깋긚
//! @details 긖긂깛긤궻벶귒뜛귒궔귞띋맯귏궳귩듖뿚궢귏궥갃
//! @details 랷뛩럱뿿갌ezds.dll긲?귽깑(ver:0.4)됶먏륃뺪?http://openxops.net/filesystem-ezds04.php
class SoundControl
{
#if SOUND_ENGINE == 0
	HINSTANCE lib;		//!< DLL긲?귽깑궻귽깛긚?깛긚
	FARPROC DSver;		//!< DSver()
	FARPROCH DSinit;	//!< DSinit()
	FARPROC DSend;		//!< DSend()
	FARPROCCI DSload;	//!< DSload()
	FARPROCIII DSplay;	//!< DSplay()
	FARPROCI DSrelease;	//!< DSrelease()
	bool useflag[MAX_LOADSOUND];	//!< 럊뾭뭷궻긖긂깛긤붥뜂귩듖뿚궥귡봹쀱

	float camera_x;			//!< 긇긽깋띆뷭
	float camera_y;			//!< 긇긽깋띆뷭
	float camera_z;			//!< 긇긽깋띆뷭
	float camera_rx;		//!< 긇긽깋X렡둷뱗
	float mastervolume;		//!< 돶쀊(긬?긜깛긣)
	int volume_max;			//!< 띍묈돶쀊


	bool CheckSourceDist(float x, float y, float z, bool near, float *out_dist);
	int CalculationVolume(int MaxVolume, float dist, bool near);
#else	//#if SOUND_ENGINE
	LPDIRECTSOUND8 pDSound;					//!< DIRECTSOUND8궻?귽깛?
	LPDIRECTSOUNDBUFFER pDSBuffer[MAX_LOADSOUND][MAX_SOUNDLISTS];	//!< 긜긇깛?깏?긫긞긲??
	LPDIRECTSOUND3DLISTENER8 p3DListener;	//!< 깏긚긥?
	float mastervolume;						//!< 돶쀊(긬?긜깛긣)
	float volume_dBper;						//!< 띍묈돶쀊

	bool CheckSoundFile(const char* filename, int *filesize, int *fileoffset, WAVEFORMATEX** pwfex);
	int GetDSVolume(int volume);
#if SOUND_ENGINE == 1
	bool CheckSourceDist(float x, float y, float z, bool near, float *out_dist);
	int CalculationVolume(int MaxVolume, float dist, bool near);
#endif
#endif	//#if SOUND_ENGINE

public:
	SoundControl();
	~SoundControl();
	int InitSound(WindowControl *WindowCtrl);
	void DestroySound();
	void SetVolume(float volume);
	void SetCamera(float x, float y, float z, float rx);
	int LoadSound(const char* filename);
	int PlaySound(int id, int volume, int pan);
	int PlaySoundPriority(int id, int volume, int pan);
	void StopSound(int id);
	int Play3DSound(int id, float x, float y, float z, int volume);
	int Play3DSoundEx(int id, float x, float y, float z, int volume, float minDist, float maxDist);
	int GetTotalSounds();
	void CleanupSound(int id);
};

#endif