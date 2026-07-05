//! @file main.cpp
//! @brief WinMain()듫릶궻믦?궓귝귂긡긚긣긵깓긐깋? 

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

//Doxygen먠믦긲?귽깑
#include "doxygen.h"

#include "main.h"

#include <direct.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
 #include <shlwapi.h>
 #pragma comment(lib, "Shlwapi.lib")
#endif

#ifdef ENABLE_DEBUGLOG
 //! 깓긐뢯쀍
 DebugLog OutputLog;
#endif

//! 긚긡?긣?긘깛
StateMachine GameState;

//! 긒??먠믦긢??
Config GameConfig;

//! 긽귽깛긂귻깛긤긂
WindowControl MainWindow;

// 렌더 FPS 제한용 고정밀 시간(ms)
// timeGetTime()은 ms 정수 단위라 120FPS의 8.333ms를 정확히 표현할 수 없다.
static double GetHighResolutionTimeMS()
{
	static bool initialized = false;
	static LARGE_INTEGER frequency;

	if (initialized == false) {
		if (QueryPerformanceFrequency(&frequency) == FALSE) {
			return (double)GetTimeMS();
		}
		initialized = true;
	}

	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	return ((double)counter.QuadPart * 1000.0) / (double)frequency.QuadPart;
}

// 실제 렌더가 시작되는 시각을 기준으로 보간 비율을 계산한다.
// Sleep()이나 렌더 준비 시간 때문에 오래된 alpha가 사용되는 것을 막는다.
static void UpdateRenderInterpolationAlpha(
	maingame* MainGame,
	double logic_accum,
	double last_time
)
{
	if (MainGame == NULL) {
		return;
	}

	double sample_time = GetHighResolutionTimeMS();

	double render_accum =
		logic_accum + (sample_time - last_time);

	if (render_accum < 0.0) {
		render_accum = 0.0;
	}

	float alpha =
		(float)(render_accum / LOGICFRAMEMS_D);

	if (alpha < 0.0f) {
		alpha = 0.0f;
	}

	if (alpha > 1.0f) {
		alpha = 1.0f;
	}

	MainGame->SetRenderInterpolationAlpha(alpha);
}

// 긴 대기 중에는 메인 루프로 돌아가 로직 처리를 계속한다.
// 렌더 마감 직전의 짧은 구간만 정밀하게 기다린다.
static bool WaitUntilRenderDeadline(double target_time)
{
	double now = GetHighResolutionTimeMS();
	double remain = target_time - now;

	if (remain <= 0.0) {
		return true;
	}

	if (remain > 2.0) {
		Sleep(1);
		return false;
	}

	if (remain > 0.30) {
		Sleep(0);
		return false;
	}

	// 최대 약 0.3ms만 바쁜 대기를 사용한다.
	// 전체 프레임 동안 바쁜 대기를 하지 않으므로 CPU 증가를 제한한다.
	while (GetHighResolutionTimeMS() < target_time) {
	}

	return true;
}

//! @brief WinMain()듫릶
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
)
{
	//뼟럊뾭덙릶뫮랉
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	char path[MAX_PATH];

#ifdef ENABLE_AUTOLOADMIF
	bool NoBriefingFlag = false;
	int gamemode = 0;
#endif

	//뿉릶룊딖돸
	InitRand();

	//렳뛱긲?귽깑궻궇귡뤾룋귩갂긇깒깛긣긢귻깒긏긣깏궸궥귡갃
	GetFileDirectory(__argv[0], path);

	if (_chdir(path) != 0) {
		MessageBoxA(
			NULL,
			"Failed to change the current directory to the executable folder.",
			GAMENAME,
			MB_OK | MB_ICONERROR
		);
		return 1;
	}

#ifdef _DEBUG
	//긽긾깏깏?긏궻뙚뢯
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	char str[255];
	strcpy(str, "[Information]\nThe compiler is the Debug mode.\nIf release the software, Switch compiler to Release mode.");
	MessageBox(NULL, str, GAMENAME, MB_OK);
#endif

	//덙릶룉뿚
	//긽긾갌Windows듏떕궶귞갂lpCmdLine덙릶궴궔GetCommandLine()듫릶궳귖롦귢귏궥궚궵궺갋갋갃
	strcpy(path, "");
	if( __argc > 1 ){
		for(int param=1; param<__argc; param++){
#ifdef ENABLE_DEBUGLOG
			//"Log"궕?궑귞귢궲궋궫귞갂깓긐뢯쀍귩뾎뚼돸
			if( CheckCommandParameter(__argv[param], "-Log") == true ){
				//깓긐뢯쀍귩뾎뚼돸
				OutputLog.EnableOutputLog();

				MainWindow.ErrorInfo("Enable Debug Log...");
				continue;
			}
#endif

#ifdef ENABLE_AUTOLOADMIF
			//"Dir"궕?궑귞귢궲궋궫귞갂긇깒깛긣긢귻깒긏긣깏댷벍
			if (CheckCommandParameter(__argv[param], "-Dir") == true) {
				if ((param + 1) < __argc) {
					const char* requested_directory = __argv[param + 1];

					if (_chdir(requested_directory) != 0) {
						MessageBoxA(
							NULL,
							"Failed to change to the directory specified by -Dir.",
							GAMENAME,
							MB_OK | MB_ICONERROR
						);
						return 1;
					}

					param += 1;
				}
				else {
					MessageBoxA(
						NULL,
						"The -Dir option requires a directory path.",
						GAMENAME,
						MB_OK | MB_ICONERROR
					);
					return 1;
				}

				continue;
			}

			//"NoBriefing"궕?궑귞귢궲궋궫귞갂걁.mif럚믦렄궼걂긳깏?긲귻깛긐귩뤙뿪궥귡
			if( CheckCommandParameter(__argv[param], "-NoBriefing") == true ){
				NoBriefingFlag = true;
				continue;
			}

			//궩귢댥둖궻덙릶궶귞궽둮뮗럔귩뵽믦궢궲귒귡
			if( CheckFileExtension(__argv[param], ".mif") == true ){
				//.mif궳궇귢궽긲?귽깑긬긚궴궢궲딯돬
				strcpy(path, __argv[param]);
			}
#endif
		}

#ifdef ENABLE_AUTOLOADMIF
		//돺궔궢귞궻.mif궕럚믦궠귢궲궋귡궶귞궽갂긳깏?긲귻깛긐긲깋긐뵿뎕
		if( path[0] != '\0' ){
			if( NoBriefingFlag == false ){ gamemode = 1; }
			else{ gamemode = 2; }
		}
#endif
	}

#ifdef ENABLE_DEBUGLOG
	//긲?귽깑띿맟
	OutputLog.MakeLog();

	//깇?긗?듏떕귩뢯쀍
	GetOperatingEnvironment();

	char infostr[64];

	/*
	//깓긐궸뢯쀍
	sprintf(infostr, "Total Parameters : %d", __argc);
	OutputLog.WriteLog(LOG_CHECK, "Parameter", infostr);
	for(int i=0; i<__argc; i++){
		sprintf(infostr, "Parameter[%d]", i);
		OutputLog.WriteLog(LOG_CHECK, infostr, __argv[i]);
	}
	*/

	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CHECK, "Startup", "Start program entry point");
#endif

#ifdef _DEBUG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CHECK, "Startup", "Visual C++ debug mode");
#endif

#ifdef _DEBUG
	char path2[MAX_PATH];

	if (_getcwd(path2, MAX_PATH) != NULL) {
		sprintf_s(
			infostr,
			sizeof(infostr),
			"EXE directory path length : %u",
			static_cast<unsigned int>(strlen(path2))
		);

		OutputLog.WriteLog(
			LOG_CHECK,
			"Environment",
			infostr
		);
	}
	else {
		OutputLog.WriteLog(
			LOG_CHECK,
			"Environment",
			"Failed to get the current directory"
		);
	}
#endif

	//긢??긲긅깑??긃긞긏
#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_LOAD, "Directory", "data folder");
	if( CheckDirectory("data") == true ){
		OutputLog.WriteLog(LOG_COMPLETE, "", "");
	}
	else{
		OutputLog.WriteLog(LOG_ERROR, "", "");
	}
#endif
#ifdef ENABLE_CHECKDATADIR
	if( CheckDirectory("data") == false ){
		MainWindow.ErrorInfo("Data directory not exist.\n\nPlease download and copy \"X operations 0.96\".");
		return 1;
	}
#endif

	//먠믦긲?귽깑벶귒뜛귒
	if( GameConfig.LoadFile("config.dat") == 1 ){
#ifndef ENABLE_AUTOCREATECONFIG
		MainWindow.ErrorInfo("config data open failed");
		return 1;
#else
		//긢긲긅깑긣뭠귩뵿뎕궢갂먠믦긲?귽깑궸뺎뫔궥귡
		GameConfig.SetDefaultConfig();
		if( GameConfig.SaveFile("config.dat") == 1 ){
			MainWindow.ErrorInfo("config data save failed");
			return 1;
		}
#endif
	}

	//믁돿먠믦귩벶귒뜛귔
#ifdef ENABLE_ADDCONFIG
	GameConfig.CreateExtConfig("config-blackline-breach.ini");
	GameConfig.LoadExtFile("config-blackline-breach.ini");
#else
	GameConfig.LoadExtFile(NULL);
#endif

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	sprintf(infostr, "Display resolution : %d x %d", GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight());
	OutputLog.WriteLog(LOG_CHECK, "Environment", infostr);
	if( GameConfig.GetFullscreenFlag() == false ){
		OutputLog.WriteLog(LOG_CHECK, "Environment", "Window mode : Window");
	}
	else{
		OutputLog.WriteLog(LOG_CHECK, "Environment", "Window mode : Fullscreen");
	}
	sprintf(infostr, "Game language mode : %d", GameConfig.GetLanguage());
	OutputLog.WriteLog(LOG_CHECK, "Environment", infostr);
#endif

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CHECK, "Startup", "Start game initialize");
#endif

	//긂귻깛긤긂룊딖돸
	MainWindow.SetParam(hPrevInstance, nCmdShow);
	MainWindow.InitWindow(GAMENAME, GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight(), GameConfig.GetFullscreenFlag());

	//딈?밒궶룊딖돸룉뿚
#ifdef ENABLE_AUTOLOADMIF
	if( InitGame(&MainWindow, gamemode, path) ){
#else
	if( InitGame(&MainWindow, 0, "") ){
#endif
		return 1;
	}

	opening Opening;
	mainmenu MainMenu;
	briefing Briefing;
	maingame MainGame;
	result Result;
	InitScreen(&MainWindow, &Opening, &MainMenu, &Briefing, &MainGame, &Result);

#ifdef ENABLE_AUTOLOADMIF
	//긽귽깛긒??궻귒?렑궥귡긾?긤궶귞궽갂뷭?궳긢긫긞긏?렑귩뾎뚼궸궥귡갃
	if( gamemode == 2 ){ MainGame.SetShowInfoFlag(true); }
#endif


#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CHECK, "Startup", "Start main loop");
#endif

	unsigned int framecnt = 0;

	// 고정 로직 업데이트용 누적 시간
	// 렌더와 로직 보간이 같은 고해상도 시간축을 사용하게 한다.
// GetTimeMS()의 1ms 단위 흔들림이 120FPS 이동 보간에 섞이는 것을 막는다.
	double last_time = GetHighResolutionTimeMS();
	double logic_accum = 0.0;

	// 렌더 제한용 다음 예정 시간
	// 120FPS는 8.333ms이므로 unsigned int ms로는 정확히 제한할 수 없다.
	double next_render_time = 0.0;
	int last_render_fps = -1;
	bool last_app_active =
		(GetForegroundWindow() == MainWindow.GethWnd());

	// Sleep(1)의 정밀도를 올림
	timeBeginPeriod(1);

	for (int flag = 0; flag != -1; flag = MainWindow.CheckMainLoop()) {
		if (flag == 1) {
			double nowtime = GetHighResolutionTimeMS();
			bool app_active =
				(GetForegroundWindow() == MainWindow.GethWnd());

			// Alt+Tab 전환 직후에는 이전 보간/마우스/렌더 마감 시간을 이어 쓰지 않는다.
			// 특히 복귀 첫 Render3D()가 커서를 중앙에 놓기 전에 실행되면
			// 창을 다시 선택한 위치가 큰 시야 회전으로 해석될 수 있다.
			if (app_active != last_app_active) {
				last_app_active = app_active;
				last_time = nowtime;
				logic_accum = 0.0;
				next_render_time = nowtime;
				MainGame.HandleApplicationFocusChange(app_active);
			}

			double elapsed = nowtime - last_time;
			last_time = nowtime;

			// 타이머 역행이나 Alt+Tab 뒤 큰 누적 시간을 방지한다.
			if (elapsed < 0.0) {
				elapsed = 0.0;
			}

			if (elapsed > 250.0) {
				elapsed = 250.0;
			}

			logic_accum += elapsed;

			int logic_steps = 0;

			// 게임 로직은 기존처럼 30ms 단위로만 실행한다.
			while ((logic_accum >= LOGICFRAMEMS_D) && (logic_steps < MAX_LOGIC_STEPS_PER_LOOP)) {
				ProcessScreen(
					&MainWindow,
					&Opening,
					&MainMenu,
					&Briefing,
					&MainGame,
					&Result,
					framecnt,
					true,   // do_logic
					false   // do_render
				);

				framecnt++;
				logic_accum -= LOGICFRAMEMS_D;
				logic_steps++;
			}

			// 너무 밀리면 게임이 무한히 따라잡으려 하지 않도록 버린다.
			if (logic_steps >= MAX_LOGIC_STEPS_PER_LOOP) {
				logic_accum = 0.0;
			}

			
			int render_fps = GameConfig.GetRenderFpsLimit();
			double render_interval_ms = 0.0;

			if (render_fps > 0) {
				// 120FPS = 8.333ms, 144FPS = 6.944ms.
				// 정수 ms로 올림 처리하면 120FPS가 9ms가 되어 111FPS로 떨어진다.
				render_interval_ms = 1000.0 / (double)render_fps;
			}

			double render_nowtime = GetHighResolutionTimeMS();

			// 옵션에서 RENDER FPS 값을 바꾼 직후에는 렌더 스케줄을 다시 맞춘다.
			if (last_render_fps != render_fps) {
				last_render_fps = render_fps;
				next_render_time = render_nowtime;
			}

			if (render_interval_ms <= 0.0) {
				// 무제한 FPS에서도 실제 렌더 직전 시각으로 보간값을 맞춘다.
				UpdateRenderInterpolationAlpha(
					&MainGame,
					logic_accum,
					last_time
				);

				ProcessScreen(
					&MainWindow,
					&Opening,
					&MainMenu,
					&Briefing,
					&MainGame,
					&Result,
					framecnt,
					false,		// do_logic
					true		// do_render
				);
			}
			else {
				if (next_render_time <= 0.0) {
					next_render_time = render_nowtime;
				}

				bool render_due =
					(render_nowtime >= next_render_time);

				if (render_due == false) {
					render_due =
						WaitUntilRenderDeadline(next_render_time);
				}

				if (render_due == true) {
					// 정밀 대기가 끝난 실제 렌더 시각을 기준으로
					// 플레이어·카메라·상대 인물 보간값을 갱신한다.
					UpdateRenderInterpolationAlpha(
						&MainGame,
						logic_accum,
						last_time
					);

					ProcessScreen(
						&MainWindow,
						&Opening,
						&MainMenu,
						&Briefing,
						&MainGame,
						&Result,
						framecnt,
						false,		// do_logic
						true		// do_render
					);

					double render_end_time =
						GetHighResolutionTimeMS();

					// 현재 시간에 다시 맞추는 것이 아니라,
					// 누적된 목표 시각을 기준으로 다음 프레임을 잡는다.
					next_render_time += render_interval_ms;

					// Alt+Tab, 디버그 중단, 긴 렌더링 이후에는
					// 밀린 프레임을 연속으로 출력하지 않는다.
					if ((render_end_time - next_render_time) >
						(render_interval_ms * 4.0)) {

						next_render_time =
							render_end_time + render_interval_ms;
					}
				}
			}
		}
	}

	timeEndPeriod(1);

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CHECK, "Exit", "Exit main loop");
#endif


	//딈?밒궶됶뺳룉뿚
	CleanupGame();

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CHECK, "Exit", "Exit program entry point");
#endif
	return 0;
}

//! @brief 긓?깛긤덙릶뵽믦
//! @param param 뙰궻빒럻쀱
//! @param cmd 뵽믦궥귡덙릶
//! @return 둤뱰갌true?뷄둤뱰갌false
//! @warning cmd덙릶궻띍룊궻덇빒럻궼뼰럨궠귢귏궥갃
//! @attention 렅궻덙릶귩뵽믦궢귏궥갃걚/[덙릶]갂-[덙릶]갂/[덙릶걁룷빒럻걂]갂-[덙릶걁룷빒럻걂]
bool CheckCommandParameter(const char *param, const char *cmd)
{
	if( param == NULL ){ return false; }
	if( cmd == NULL ){ return false; }
	if( strlen(param) >= 16 ){ return false; }
	if( strlen(cmd) >= 16 ){ return false; }
	if( strlen(param) != strlen(cmd) ){ return false; }

	char str[16];
	strcpy(str, cmd);

	str[0] = '/';
	if( strcmp(param, str) == 0 ){ return true; }
	str[0] = '-';
	if( strcmp(param, str) == 0 ){ return true; }

	for(int i=0; i<(int)strlen(str); i++){ str[i] = (char)tolower((int)(str[i])); }

	str[0] = '/';
	if( strcmp(param, str) == 0 ){ return true; }
	str[0] = '-';
	if( strcmp(param, str) == 0 ){ return true; }

	return false;
}

//! @brief 둷뱗걁깋긙귺깛걂맫딮돸
//! @param r 둷뱗
//! @return 맫딮돸뚣궻둷뱗
//! @note -PI?PI 궻듩궸맫딮돸궢귏궥갃
float AngleNormalization(float r)
{
	for(; r > (float)M_PI;    r -= (float)M_PI*2){}
	for(; r < (float)M_PI*-1; r += (float)M_PI*2){}
	return r;
}

//! @brief ?귽깛긣긬깋긽???궻뭠귩unsigned int?궸빾듂
//! @param value 뭠
//! @return 맢릶뭠
unsigned int PointParamToInt(signed short int value)
{
	return (unsigned int)((unsigned char)value);
}

//! @brief 2긫귽긣궸릶뭠먠믦걁깏긣깑긄깛긢귻귺깛걂
//! @param outdata 2긫귽긣궻?귽깛?
//! @param value 릶뭠
void Set2ByteLittleEndian(unsigned char *outdata, unsigned int value)
{
	if( outdata == NULL ){ return; }

	outdata[0] = (unsigned char)(( value >> 0 ) & 0x00FF);
	outdata[1] = (unsigned char)(( value >> 8 ) & 0x00FF);
}

//! @brief 4긫귽긣궸릶뭠먠믦걁깏긣깑긄깛긢귻귺깛걂
//! @param outdata 4긫귽긣궻?귽깛?
//! @param value 릶뭠
void Set4ByteLittleEndian(unsigned char *outdata, unsigned int value)
{
	if( outdata == NULL ){ return; }

	outdata[0] = (unsigned char)(( value >> 0 ) & 0x000000FF);
	outdata[1] = (unsigned char)(( value >> 8 ) & 0x000000FF);
	outdata[2] = (unsigned char)(( value >> 16 ) & 0x000000FF);
	outdata[3] = (unsigned char)(( value >> 24 ) & 0x000000FF);
}

//! @brief 2긫귽긣궸릶뭠먠믦걁긮긞긐긄깛긢귻귺깛걂
//! @param outdata 2긫귽긣궻?귽깛?
//! @param value 릶뭠
void Set2ByteBigEndian(unsigned char *outdata, unsigned int value)
{
	if( outdata == NULL ){ return; }

	outdata[0] = (unsigned char)(( value >> 8 ) & 0x00FF);
	outdata[1] = (unsigned char)(( value >> 0 ) & 0x00FF);
}

//! @brief 4긫귽긣궸릶뭠먠믦걁긮긞긐긄깛긢귻귺깛걂
//! @param outdata 4긫귽긣궻?귽깛?
//! @param value 릶뭠
void Set4ByteBigEndian(unsigned char *outdata, unsigned int value)
{
	if( outdata == NULL ){ return; }

	outdata[0] = (unsigned char)(( value >> 24 ) & 0x000000FF);
	outdata[1] = (unsigned char)(( value >> 16 ) & 0x000000FF);
	outdata[2] = (unsigned char)(( value >> 8 ) & 0x000000FF);
	outdata[3] = (unsigned char)(( value >> 0 ) & 0x000000FF);
}

//! @brief 2긫귽긣궻릶뭠롦벦걁깏긣깑긄깛긢귻귺깛걂
//! @param indata 2긫귽긣궻?귽깛?
//! @return 릶뭠
unsigned int Get2ByteLittleEndian(unsigned char *indata)
{
	if( indata == NULL ){ return 0; }

	return (unsigned int)( ((indata[1]<<8)&0xFF00) + ((indata[0]<<0)&0x00FF) );
}

//! @brief 4긫귽긣궻릶뭠롦벦걁깏긣깑긄깛긢귻귺깛걂
//! @param indata 4긫귽긣궻?귽깛?
//! @return 릶뭠
unsigned int Get4ByteLittleEndian(unsigned char *indata)
{
	if( indata == NULL ){ return 0; }

	return (unsigned int)( ((indata[3]<<24)&0xFF000000) + ((indata[2]<<16)&0x00FF0000) + ((indata[1]<<8)&0x0000FF00) + ((indata[0]<<0)&0x000000FF) );
}

//! @brief 2긫귽긣궻릶뭠롦벦걁긮긞긐긄깛긢귻귺깛걂
//! @param indata 2긫귽긣궻?귽깛?
//! @return 릶뭠
unsigned int Get2ByteBigEndian(unsigned char *indata)
{
	if( indata == NULL ){ return 0; }

	return (unsigned int)( ((indata[0]<<8)&0xFF00) + ((indata[1]<<0)&0x00FF) );
}

//! @brief 4긫귽긣궻릶뭠롦벦걁긮긞긐긄깛긢귻귺깛걂
//! @param indata 4긫귽긣궻?귽깛?
//! @return 릶뭠
unsigned int Get4ByteBigEndian(unsigned char *indata)
{
	if( indata == NULL ){ return 0; }

	return (unsigned int)( ((indata[0]<<24)&0xFF000000) + ((indata[1]<<16)&0x00FF0000) + ((indata[2]<<8)&0x0000FF00) + ((indata[3]<<0)&0x000000FF) );
}
