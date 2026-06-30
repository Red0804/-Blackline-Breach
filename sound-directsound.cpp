//! @file sound-directsound.cpp
//! @brief SoundControl긏깋긚궻믦?걁DirectSound붎걂

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

#if (SOUND_ENGINE == 1 || SOUND_ENGINE == 2)

//! @brief 긓깛긚긣깋긏?
SoundControl::SoundControl()
{
	pDSound = NULL;
	for(int i=0;i<MAX_LOADSOUND; i++){
		for(int j=0; j<MAX_SOUNDLISTS; j++){
			pDSBuffer[i][j] = NULL;
		}
	}
	p3DListener = NULL;
	mastervolume = 1.0f;
	volume_dBper = 1.0f;
}

//! @brief 긢귻긚긣깋긏?
SoundControl::~SoundControl()
{
	DestroySound();
}

//! @brief 룊딖돸
//! @param WindowCtrl WindowControl긏깋긚궻?귽깛?
//! @return 맟뚻갌0?렪봲갌1
int SoundControl::InitSound(WindowControl *WindowCtrl)
{
	if( WindowCtrl == NULL ){ return 1; }

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_INIT, "Sound", "DirectSound");
#endif

	//DirectSound긆긳긙긃긏긣귩맯맟
	if( FAILED( DirectSoundCreate8(NULL, &pDSound, NULL) ) ){
		return 1;
	}

	//떐뮧깒긹깑궻먠믦
	if( FAILED( pDSound->SetCooperativeLevel(WindowCtrl->GethWnd(), DSSCL_EXCLUSIVE) ) ){
		return 1;
	}

	//긵깋귽?깏 긫긞긲?귩띿맟궢갂깏긚긥?귽깛??긲긃귽긚귩롦벦궥귡
	LPDIRECTSOUNDBUFFER pPrimary;
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
	if( SUCCEEDED(pDSound->CreateSoundBuffer(&dsbd, &pPrimary, NULL)) ){
		pPrimary->QueryInterface(IID_IDirectSound3DListener8, (LPVOID *)&p3DListener);
		pPrimary->Release();
	}

	//긤긞긵깋?뚼됈귩뼰뚼궸
	p3DListener->SetDopplerFactor(DS3D_MINDOPPLERFACTOR, DS3D_IMMEDIATE);

	//깓?깑갋긆긲걁뙵릨뱗뜃궋걂먠믦
	p3DListener->SetRolloffFactor(0.01f, DS3D_IMMEDIATE);

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	return 0;
}

//! @brief 됶뺳
//! @attention ?듫릶궼렔벍밒궸뚁귂뢯궠귢귏궥궕갂뼻렑밒궸뚁귂뢯궥궞궴귖됀?궳궥갃
void SoundControl::DestroySound()
{
	if( pDSound == NULL ){ return; }

	for(int i=0;i<MAX_LOADSOUND; i++){
		for(int j=0; j<MAX_SOUNDLISTS; j++){
			if( pDSBuffer[i][j] != NULL ){
				pDSBuffer[i][j]->Release();
				pDSBuffer[i][j] = NULL;
			}
		}
	}
	if (p3DListener != NULL) {
		p3DListener->Release();
		p3DListener = NULL;
	}

	if( pDSound != NULL ){
		pDSound->Release();
		pDSound = NULL;
	}

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CLEANUP, "Sound", "DirectSound");
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
	if( mastervolume == 0.0f ){ volume_dBper = 0.0f; return; }
	if( mastervolume == 1.0f ){ volume_dBper = 1.0f; return; }
	volume_dBper = 1.0f - log10f(mastervolume)*-0.2f;
}

//! @brief 긇긽깋궻띆뷭궴둷뱗귩먠믦
//! @param x 긇긽깋궻X띆뷭
//! @param y 긇긽깋궻Y띆뷭
//! @param z 긇긽깋궻Z띆뷭
//! @param rx 긇긽깋궻X렡둷뱗?걁?뽵걂
//! @warning 뻽긲깒??뚁귂뢯궢궲갂띍륷궻긇긽깋띆뷭귩먠믦걁밙뾭걂궢궲궘궬궠궋갃
void SoundControl::SetCamera(float x, float y, float z, float rx)
{
	p3DListener->SetPosition(x, y, z, DS3D_IMMEDIATE);
	p3DListener->SetOrientation(cosf(rx), 0.0f, sinf(rx), 0.0f, 1.0f, 0.0f, DS3D_IMMEDIATE);
}

//! @brief 긖긂깛긤귩벶귒뜛귔
//! @param filename 긲?귽깑뼹
//! @return 맟뚻갌0댥뤵궻긢??붥뜂?렪봲갌-1
//! @attention 2?긿깛긨깑걁긚긡깒긆걂긢??궕럚믦궠귢궫뤾뜃갂덭뫀궻긢??궬궚롦벦궠귢갂뜺뫀궻긢??궼뼰럨궠귢귏궥갃
int SoundControl::LoadSound(const char* filename)
{
	if( pDSound == NULL ){ return -1; }
	if( filename == NULL ){ return -1; }

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_LOAD, "Sound", filename);
#endif

	//둎궋궲궋귡붥뜂귩뭈궥
	int id=0;
	for(id=0; id<MAX_LOADSOUND; id++){
		if( pDSBuffer[id][0] == NULL ){ break; }
	}
	if( id == MAX_LOADSOUND ){ return -1; }

	WAVEFORMATEX* pwfex;
	int WavSize = 0;
	int Wavoffset = 0;
	bool d2channels = false;

	//Wave긲?귽깑궻륃뺪귩롦벦
	if( CheckSoundFile(filename, &WavSize, &Wavoffset, &pwfex) == true ){
		return -1;
	}
	pwfex->cbSize = 0;

	//뷄뫮돒긲긅??긞긣궶귞렪봲
	if( (pwfex->wFormatTag != WAVE_FORMAT_PCM)&&(pwfex->wFormatTag != WAVE_FORMAT_IEEE_FLOAT) ){ return -1; }
	if( (pwfex->nChannels != 1)&&(pwfex->nChannels != 2) ){ return -1; }
	if( (pwfex->wBitsPerSample != 8)&&(pwfex->wBitsPerSample != 16)
		&&(pwfex->wBitsPerSample != 24)&&(pwfex->wBitsPerSample != 32) ){ return -1; }

	//귖궢긚긡깒긆긢??궶귞
	if( pwfex->nChannels == 2 ){
		d2channels = true;		//긲깋긐귩먠믦
		WavSize /= 2;			//긖귽긛귩뵾빁궸

		//긾긩깋깑궴궢궲떗맕밒궸띋똶럁
		pwfex->nChannels = 1;
		pwfex->nAvgBytesPerSec /= 2;
		pwfex->nBlockAlign /= 2;
	}

	// DirectSound긜긇깛?깏?긫긞긲??띿맟
	DSBUFFERDESC dsbd;  
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME;
	dsbd.dwBufferBytes = WavSize;
	dsbd.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
	dsbd.lpwfxFormat = pwfex;
	if( FAILED( pDSound->CreateSoundBuffer(&dsbd, &pDSBuffer[id][0], NULL) ) ){
		return -1;	//긜긇깛?깏긫긞긲??띿맟렪봲
	}

	//깓긞긏궥귡
	VOID* pBuffer = NULL;
	DWORD dwBufferSize = 0; 
	if( FAILED(pDSBuffer[id][0]->Lock(0, WavSize, &pBuffer, &dwBufferSize, NULL, NULL, 0) ) ){
		return -1;	//긫긞긲?궻깓긞긏렪봲
	}

	//덇렄쀌덃귩둴뺎
	BYTE* pWavData;
	int dwSize = dwBufferSize;
	if( dwSize > WavSize ){ dwSize = WavSize; }
	if( d2channels == true ){
		dwSize = dwSize * 2;
	}
	pWavData = new BYTE[dwSize];
	if( pWavData == NULL ){
		//WAVE긲?귽깑귩벶귒뜛귔긽긾깏?궕둴뺎궳궖궶궋
		return -1;
	}

#ifdef ENABLE_PATH_DELIMITER_SLASH
	//긬긚뗦먛귟빒럻귩빾듂
	filename = ChangePathDelimiter(filename);
#endif

	//봥?긢??귩롦귟뜛귔
	FILE* fp;
	fp = fopen(filename, "rb");
	fseek(fp, Wavoffset, SEEK_SET);
	fread(pWavData, 1, dwSize, fp);
	fclose(fp);

	//덇렄쀌덃궔귞긜긇깛?깏긫긞긲??귉긓긯?
	if( d2channels == false ){
		//긾긩깋깑긢??궶귞1긫귽긣궱궰긓긯?궥귡
		for(int i=0; i<dwSize; i++){
			((BYTE*)pBuffer)[i] = pWavData[i];
		}
	}
	else{
		//긖깛긵깏깛긐긫귽긣릶귩롦벦
		int samplingbits = pwfex->wBitsPerSample;

		if( samplingbits == 8 ){
			int byte = 0;
			for(int i=0; i<dwSize; i+=2){
				/*
				int mix = (pWavData[i] + pWavData[i + 1])/2;
				((BYTE*)pBuffer)[byte] = (BYTE)mix;
				*/

				//긚긡깒긆긢??궶귞갂뜺뫀궻긢??궬궚둰?
				((BYTE*)pBuffer)[byte] = (BYTE)pWavData[i];
				byte += 1;
			}
		}
		if( samplingbits == 16 ){
			int byte = 0;
			for(int i=0; i<dwSize; i+=4){
				/*
				//int mix = (int)(Get2ByteBigEndian(&(pWavData[i + 0])) + Get2ByteBigEndian(&(pWavData[i + 2])))/2;
				int mix = (int)Get2ByteBigEndian(&(pWavData[i + 0]));
				((BYTE*)pBuffer)[byte+0] = (BYTE)((mix>>8)&0x00FF);
				((BYTE*)pBuffer)[byte+1] = (BYTE)(mix)&0x00FF;
				byte += 2;
				*/

				//긚긡깒긆긢??궶귞갂뜺뫀궻긢??궬궚둰?
				((BYTE*)pBuffer)[byte+0] = (BYTE)pWavData[i + 0];
				((BYTE*)pBuffer)[byte+1] = (BYTE)pWavData[i + 1];
				byte += 2;
			}
		}
		if( samplingbits == 24 ){
			int byte = 0;
			for(int i=0; i<dwSize; i+=6){
				//긚긡깒긆긢??궶귞갂뜺뫀궻긢??궬궚둰?
				((BYTE*)pBuffer)[byte+0] = (BYTE)pWavData[i + 0];
				((BYTE*)pBuffer)[byte+1] = (BYTE)pWavData[i + 1];
				((BYTE*)pBuffer)[byte+2] = (BYTE)pWavData[i + 2];
				byte += 3;
			}
		}
		if( samplingbits == 32 ){
			int byte = 0;
			for(int i=0; i<dwSize; i+=8){
				//긚긡깒긆긢??궶귞갂뜺뫀궻긢??궬궚둰?
				((BYTE*)pBuffer)[byte+0] = (BYTE)pWavData[i + 0];
				((BYTE*)pBuffer)[byte+1] = (BYTE)pWavData[i + 1];
				((BYTE*)pBuffer)[byte+2] = (BYTE)pWavData[i + 2];
				((BYTE*)pBuffer)[byte+3] = (BYTE)pWavData[i + 3];
				byte += 4;
			}
		}
	}

	//깓긞긏됶룣
	pDSBuffer[id][0]->Unlock(pBuffer, dwBufferSize, NULL, 0);

	//덇렄쀌덃귩됶뺳
	delete [] pWavData;

	//긲긅??긞긣륃뺪됶뺳
	delete pwfex;

	//긜긇깛?깏긫긞긲??궻긓긯?귩띿맟
	for(int i=1; i<MAX_SOUNDLISTS; i++){
		if( pDSound->DuplicateSoundBuffer(pDSBuffer[id][0], &(pDSBuffer[id][i])) != DS_OK ){
			CleanupSound(id);
			return -1;
		}
	}

	//돶쀊띍룷궳덇뱗띋맯궢궲궓궘
	pDSBuffer[id][0]->SetVolume(DSBVOLUME_MIN);
	pDSBuffer[id][0]->Play(NULL, 0, NULL);

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_COMPLETE, "", id);
#endif

	return id;
}

//! @brief 긖긂깛긤귩띋맯걁뷄3D띋맯걂
//! @param id 긢??붥뜂
//! @param volume 띋맯?깏깄??
//! @param pan 걙뼰뚼걚
//! @return 맟뚻갌1?렪봲갌0
//! @note 뾭뱑갌긵깒귽깂?렔릆궕뵯맯궥귡돶갋긒??뗴듩멣뫬궳뗉덇궸뼿귞궥돶갋BGM
int SoundControl::PlaySound(int id, int volume, int pan)
{
	if( pDSound == NULL ){ return 0; }
	if( (id < 0)||(MAX_LOADSOUND <= id) ){ return 0; }
	if( pDSBuffer[id][0] == NULL ){ return 0; }

	DWORD status = 0;

	for(int i=0; i<MAX_SOUNDLISTS; i++){
		//띋맯륉떟귩롦벦
		pDSBuffer[id][i]->GetStatus(&status);

		//긖긂깛긤궕믠?뭷궶귞궽
		if( (status & DSBSTATUS_PLAYING) == 0x00 ){
			LPDIRECTSOUND3DBUFFER pDS3DBuffer;
			if( FAILED(pDSBuffer[id][i]->QueryInterface(IID_IDirectSound3DBuffer8, (VOID**)&pDS3DBuffer)) ){
				//IDirectSound3DBuffer8귩롦벦궳궖궶궋
				return 0;
			}

			//3D띋맯귩뼰뚼
			pDS3DBuffer->SetMode(DS3DMODE_DISABLE, DS3D_IMMEDIATE);

			//?깏깄??귩먠믦궢띋맯
			pDSBuffer[id][i]->SetVolume( GetDSVolume(volume) );
			pDSBuffer[id][i]->Play(NULL, 0, NULL);
			pDS3DBuffer->Release();
			return 1;
		}
	}

	return 0;
}

int SoundControl::PlaySoundPriority(int id, int volume, int pan)
{
	if (pDSound == NULL) { return 0; }
	if ((id < 0) || (MAX_LOADSOUND <= id)) { return 0; }
	if (pDSBuffer[id][0] == NULL) { return 0; }

	//  Ϲ   ۸ ãƺ.
	if (PlaySound(id, volume, pan) != 0) {
		return 1;
	}

	//  Դٴ    ۰   ̶ .
	// ÷̾ ѼҸ   ǹǷ 0 ۸  Ѵ.
	LPDIRECTSOUND3DBUFFER pDS3DBuffer;
	if (FAILED(pDSBuffer[id][0]->QueryInterface(IID_IDirectSound3DBuffer8, (VOID**)&pDS3DBuffer))) {
		return 0;
	}

	pDSBuffer[id][0]->Stop();
	pDSBuffer[id][0]->SetCurrentPosition(0);

	// ÷̾ ڽ ѼҸ 2D 
	pDS3DBuffer->SetMode(DS3DMODE_DISABLE, DS3D_IMMEDIATE);

	pDSBuffer[id][0]->SetVolume(GetDSVolume(volume));
	pDSBuffer[id][0]->Play(NULL, 0, NULL);

	pDS3DBuffer->Release();

	return 1;
}

void SoundControl::StopSound(int id)
{
	if (pDSound == NULL) { return; }
	if ((id < 0) || (MAX_LOADSOUND <= id)) { return; }
	if (pDSBuffer[id][0] == NULL) { return; }

	DWORD status = 0;

	for (int i = 0; i < MAX_SOUNDLISTS; i++) {
		if (pDSBuffer[id][i] == NULL) { continue; }

		pDSBuffer[id][i]->GetStatus(&status);

		if ((status & DSBSTATUS_PLAYING) != 0x00) {
			pDSBuffer[id][i]->Stop();
			pDSBuffer[id][i]->SetCurrentPosition(0);
		}
	}
}

//! @brief TEhĐi3DĐj
//! @param id 긢??붥뜂
//! @param x 돶뙶궻X띆뷭
//! @param y 돶뙶궻Y띆뷭
//! @param z 돶뙶궻Z띆뷭
//! @param volume 띋맯?깏깄??
//! @return 맟뚻갌1?렪봲갌0
//! @note 뾭뱑갌먥뫮밒궶댧뭫귩렃궭떁뿣궸귝귟뙵릨궥귡갂덇붗밒궶뚼됈돶갃
int SoundControl::Play3DSound(int id, float x, float y, float z, int volume)
{
	if( pDSound == NULL ){ return 0; }
	if( (id < 0)||(MAX_LOADSOUND <= id) ){ return 0; }
	if( pDSBuffer[id][0] == NULL ){ return 0; }

#if SOUND_ENGINE == 1

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

	return PlaySound(id, playvolume, pan);

#else	//#if SOUND_ENGINE

	DWORD status = 0;

	for(int i=0; i<MAX_SOUNDLISTS; i++){
		//띋맯륉떟귩롦벦
		pDSBuffer[id][i]->GetStatus(&status);

		//긖긂깛긤궕믠?뭷궶귞궽
		if( (status & DSBSTATUS_PLAYING) == 0x00 ){

			LPDIRECTSOUND3DBUFFER pDS3DBuffer;
			if( FAILED(pDSBuffer[id][i]->QueryInterface(IID_IDirectSound3DBuffer8, (VOID**)&pDS3DBuffer)) ){
				//IDirectSound3DBuffer8귩롦벦궳궖궶궋
				return 0;
			}

			//띍룷떁뿣궴띍묈떁뿣궻먠믦
			pDS3DBuffer->SetMinDistance(1, DS3D_IMMEDIATE);
			pDS3DBuffer->SetMaxDistance(MAX_SOUNDDIST, DS3D_IMMEDIATE);

			//돶뙶궻띆뷭귩먠믦
			pDS3DBuffer->SetPosition(x, y, z, DS3D_IMMEDIATE);

			//3D띋맯귩뾎뚼
			pDS3DBuffer->SetMode(DS3DMODE_NORMAL, DS3D_IMMEDIATE);

			//?깏깄??귩먠믦궢띋맯
			pDSBuffer[id][i]->SetVolume( GetDSVolume(volume) );
			pDSBuffer[id][i]->Play(NULL, 0, NULL);
			pDS3DBuffer->Release();
			return 1;
		}
	}

	return 0;

#endif	//#if SOUND_ENGINE
}


int SoundControl::Play3DSoundEx(int id, float x, float y, float z, int volume, float minDist, float maxDist)
{
	if (pDSound == NULL) { return 0; }
	if ((id < 0) || (MAX_LOADSOUND <= id)) { return 0; }
	if (pDSBuffer[id][0] == NULL) { return 0; }

#if SOUND_ENGINE == 1

	// SOUND_ENGINE == 1은 현재 사용하지 않지만, 안전하게 기존 방식으로 처리
	return Play3DSound(id, x, y, z, volume);

#else

	DWORD status = 0;

	for (int i = 0; i < MAX_SOUNDLISTS; i++) {
		pDSBuffer[id][i]->GetStatus(&status);

		if ((status & DSBSTATUS_PLAYING) == 0x00) {

			LPDIRECTSOUND3DBUFFER pDS3DBuffer;
			if (FAILED(pDSBuffer[id][i]->QueryInterface(IID_IDirectSound3DBuffer8, (VOID**)&pDS3DBuffer))) {
				return 0;
			}

			if (minDist < 1.0f) {
				minDist = 1.0f;
			}
			if (maxDist < minDist) {
				maxDist = minDist;
			}

			pDS3DBuffer->SetMinDistance(minDist, DS3D_IMMEDIATE);
			pDS3DBuffer->SetMaxDistance(maxDist, DS3D_IMMEDIATE);

			pDS3DBuffer->SetPosition(x, y, z, DS3D_IMMEDIATE);
			pDS3DBuffer->SetMode(DS3DMODE_NORMAL, DS3D_IMMEDIATE);

			pDSBuffer[id][i]->SetVolume(GetDSVolume(volume));
			pDSBuffer[id][i]->Play(NULL, 0, NULL);
			pDS3DBuffer->Release();
			return 1;
		}
	}

	return 0;

#endif
}

//! @brief 벶귒뜛귒띙귒궻긖긂깛긤릶귩롦벦
//! @return 긖긂깛긤릶
int SoundControl::GetTotalSounds()
{
	int cnt = 0;

	//럊뾭뭷궻뾴멹귩릶궑귡
	for(int i=0; i<MAX_LOADSOUND; i++){
		if( pDSBuffer[i][0] != NULL ){ cnt += 1; }
	}

	return cnt;
}

//! @brief 긖긂깛긤귩됶뺳
//! @param id 긢??붥뜂
void SoundControl::CleanupSound(int id)
{
	if( pDSound == NULL ){ return; }
	if( (id < 0)||(MAX_LOADSOUND <= id) ){ return; }
	if( pDSBuffer[id][0] == NULL ){ return; }

	DWORD status = 0;

	for(int i=0; i<MAX_SOUNDLISTS; i++){
		//띋맯륉떟귩롦벦
		pDSBuffer[id][i]->GetStatus(&status);

		//긖긂깛긤궕띋맯뭷궶귞궽믠?궥귡
		if( (status & DSBSTATUS_PLAYING) != 0x00 ){
			pDSBuffer[id][i]->Stop();
		}

		//뫮뤭궻긜긇깛?깏긫긞긲??귩됶뺳
		if( pDSBuffer[id][i] != NULL ){ pDSBuffer[id][i]->Release(); }
		pDSBuffer[id][i] = NULL;
	}

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CLEANUP, "Sound", id);
#endif
}

//! @brief Wave긲?귽깑궻륃뺪귩뮧귊귡
//! @param filename 긲?귽깑뼹
//! @param filesize 긲?귽깑긖귽긛귩롷궚롦귡?귽깛?
//! @param fileoffset 긢??긆긲긜긞긣귩롷궚롦귡?귽깛?
//! @param pwfex WAVEFORMATEX?귩롷궚롦귡볫뢣?귽깛?
//! @return 맟뚻갌false?렪봲갌true
bool SoundControl::CheckSoundFile(const char* filename, int *filesize, int *fileoffset, WAVEFORMATEX** pwfex)
{
	HMMIO hMmio = NULL;
	MMCKINFO ckInfo;
	MMCKINFO riffckInfo;
	PCMWAVEFORMAT pcmWaveFormat; 
	bool errorflag;

	hMmio = mmioOpen((LPSTR)filename, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if( hMmio == NULL ){
		//긲?귽깑궕궶궋
		return true;
	}

	ZeroMemory(&riffckInfo, sizeof(MMCKINFO));

	//WAVE긲?귽깑궔둴봃
	errorflag = true;
	if( mmioDescend( hMmio, &riffckInfo, NULL, 0 ) == MMSYSERR_NOERROR ){
		if( (mmioFOURCC('R','I','F','F') == riffckInfo.ckid) && (mmioFOURCC('W','A','V','E') == riffckInfo.fccType) ){
			errorflag = false;
		}
	}
	if( errorflag == true ){
		mmioClose(hMmio, MMIO_FHOPEN);
		return true;	//WAVE긲?귽깑궳궶궋
	}

	//긲긅??긞긣륃뺪롦벦
	*pwfex = NULL;
	ckInfo.ckid = mmioFOURCC('f','m','t',' ');
	if( mmioDescend(hMmio, &ckInfo, &riffckInfo, MMIO_FINDCHUNK) == MMSYSERR_NOERROR ){
		if( mmioRead(hMmio, (HPSTR) &pcmWaveFormat, sizeof(pcmWaveFormat)) == sizeof(pcmWaveFormat) ){
			if( (pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM)||(pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_IEEE_FLOAT) ){
				if( (pcmWaveFormat.wf.nChannels == 1)||(pcmWaveFormat.wf.nChannels == 2) ){
					*pwfex = new WAVEFORMATEX;
					if( *pwfex != NULL ){
						memcpy( *pwfex, &pcmWaveFormat, sizeof(pcmWaveFormat) );
						//pwfex->cbSize = 0;
					}
				}
			}
		}
	}
	if( *pwfex == NULL ){
		mmioClose(hMmio, MMIO_FHOPEN);
		return true;	//긲?귽깑긲긅??긞긣궕뫮돒궢궲궶궋
	}
	if( mmioAscend(hMmio, &ckInfo, 0) != MMSYSERR_NOERROR ){
		delete pwfex;
		mmioClose(hMmio, MMIO_FHOPEN);
		return true;	//?긿깛긏맕뚥렪봲
	}

	//긢??쀌덃귩뮧귊귡
	ckInfo.ckid = mmioFOURCC('d','a','t','a');
	if( mmioDescend(hMmio, &ckInfo, &riffckInfo, MMIO_FINDCHUNK) != MMSYSERR_NOERROR ){
		mmioClose(hMmio, MMIO_FHOPEN);
		return true;	//긲?귽깑긲긅??긞긣궕뫮돒궢궲궶궋
	}

	mmioClose(hMmio, MMIO_FHOPEN);

	*filesize = ckInfo.cksize;
	*fileoffset = ckInfo.dwDataOffset;
	return false;
}

//! @brief SetVolume()뾭?1/100 dB (긢긘긹깑) 귩똶럁
//! @param volume 돶쀊걁-100?100걂
//! @return 1/100 dB (긢긘긹깑).
int SoundControl::GetDSVolume(int volume)
{
	if( volume <= -100 ){ return DSBVOLUME_MIN; }
	if( volume >=  100 ){ return DSBVOLUME_MAX; }

	//float volume2 = 1.0f/200 * (volume + 100);
	float volume2 = (1.0f/200 * (volume + 100)) * volume_dBper;
	int retn = (int)((DSBVOLUME_MIN-DSBVOLUME_MAX) * (1.0f - volume2));
	return retn;
}

#if SOUND_ENGINE == 1
//! @brief 돶뙶궴궻떁뿣귩뮧귊귡
//! @param x 돶뙶궻X띆뷭
//! @param y 돶뙶궻Y띆뷭
//! @param z 돶뙶궻Z띆뷭
//! @param snear 뗟떁뿣돶뙶
//! @param out_dist 떁뿣
//! @return 뾎뚼걁볙걂갌true?뼰뚼걁둖걂갌false
bool SoundControl::CheckSourceDist(float x, float y, float z, bool snear, float *out_dist)
{
	D3DXVECTOR3 camera;
	float dx, dy, dz, dist;
	int max_dist;

	p3DListener->GetPosition(&camera);

	if( snear == false ){
		max_dist = MAX_SOUNDDIST;
	}
	else{
		max_dist = 30;
	}

	dx = camera.x - x;
	dy = camera.y - y;
	dz = camera.z - z;
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
#endif

#endif	//SOUND_ENGINE