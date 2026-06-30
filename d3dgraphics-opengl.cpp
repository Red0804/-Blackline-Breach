//! @file d3dgraphics-opengl.cpp
//! @brief D3DGraphics긏깋긚궻믦?걁OpenGL붎걂

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

// ***** OpenGL core only *****
//
// libjpeg
//    Copyright (C) 1991-2014, Thomas G. Lane, Guido Vollbeding.
//    this software is based in part on the work of the Independent JPEG Group
//
// zlib
//    Copyright (C) 1995-2013 Jean-loup Gailly and Mark Adler
//
// libpng
//    Copyright (c) 1998-2014 Glenn Randers-Pehrson
//
// ****************************

#include "d3dgraphics.h"

#if GRAPHIC_ENGINE == 8

//! @brief 긓깛긚긣깋긏?
D3DGraphics::D3DGraphics()
{
	hWnd = NULL;
	hGLRC = NULL;
	width = 0;
	height = 0;
	fullscreenflag = false;
	LanguageMode = 0;
	SystemFont = NULL;
	SystemSmallFont = NULL;
	now_SystemFontUStr = new WCHAR [1];
	now_SystemFontUStr[0] = NULL;
	SystemFontListIdx = 0;
	SystemFontListIdxSize = 0;
	SystemFont_posz = 1.0f;
	now_textureid = -1;

	camera_x = 0.0f;
	camera_y = 0.0f;
	camera_z = 0.0f;
	camera_rx = 0.0f;
	camera_ry = 0.0f;
	viewangle = 0.0f;

	BlockDataIF = NULL;
	bs = 0;
	for(int i=0; i<TOTAL_BLOCKTEXTURE; i++){
		mapTextureID[i] = -1;
	}

	TextureFontFname[0] = '\0';
	TextureFont = -1;

	for( int i=0; i<TOTAL_HAVEWEAPON; i++ ){
		HUD_myweapon_x[i] = 0.0f;
		HUD_myweapon_y[i] = 0.0f;
		HUD_myweapon_z[i] = 0.0f;
	}

#ifdef ENABLE_DEBUGCONSOLE
	ZeroMemory(DebugFontData, sizeof(unsigned char)*96*16);
	TextureDebugFont = -1;
#endif
}

//! @brief 긢귻긚긣깋긏?
D3DGraphics::~D3DGraphics()
{
	DestroyD3D();
}

//! @brief 긲깑긚긏깏?깛긲깋긐먠믦
//! @param fullscreen 긲깑긚긏깏?깛긲깋긐
//! @attention 벏긲깋긐궼갂룊딖돸렄궻 InitD3D() 듫릶궳귖먠믦궳궖귏궥갃
void D3DGraphics::SetFullScreenFlag(bool fullscreen)
{
	fullscreenflag = fullscreen;
}

//! @brief 긲깑긚긏깏?깛긲깋긐롦벦
//! @return 긲깑긚긏깏?깛긲깋긐
bool D3DGraphics::GetFullScreenFlag()
{
	return fullscreenflag;
}

//! @brief 뙻뚭멗묖궻먠믦
//! @param mode 뙻뚭멗묖
//! @attention 벏먠믦뭠궼갂룊딖돸렄궻 InitD3D() 듫릶궳귖먠믦궳궖귏궥갃
void D3DGraphics::SetlanguageMode(int mode)
{
	LanguageMode = mode;
}

//! @brief 뙻뚭멗묖궻먠믦롦벦
//! @return 뙻뚭멗묖
int D3DGraphics::GetlanguageMode()
{
	return LanguageMode;
}

//! @brief 룊딖돸@n
//! 걁OpenGL 1.1걂
//! @param WindowCtrl WindowControl긏깋긚궻?귽깛?
//! @param TextureFontFilename 럊뾭궥귡긡긏긚?긿긲긅깛긣궻긲?귽깑뼹
//! @param fullscreen false갌긂귻깛긤긂?렑?true갌긲깑긚긏깏?깛뾭?렑
//! @param language 뙻뚭멗묖걁English/Japanese갌0갂English/Korean갌1걂
//! @return 맟뚻갌0?렪봲갌1
int D3DGraphics::InitD3D(WindowControl *WindowCtrl, const char *TextureFontFilename, bool fullscreen, int language)
{
	if( WindowCtrl == NULL ){ return 1; }
	if( TextureFontFilename == NULL ){ return 1; }

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_INIT, "Graphics", "OpenGL");
#endif

	hWnd = WindowCtrl->GethWnd();

	RECT prc;
	GetClientRect(hWnd, &prc);
	width = prc.right;
	height = prc.bottom;

	fullscreenflag = fullscreen;
	LanguageMode = language;

	// Borderless fullscreen.
// ディスプレイ解像度は変更しない。
// ウィンドウ側で WS_POPUP + モニター全体サイズにする。
	ChangeDisplaySettings(NULL, 0);
	/*
	//긲깑긚긏깏?깛돸
	if( fullscreen == true ){
		DEVMODE devmode;
		ZeroMemory(&devmode, sizeof(devmode));
		devmode.dmSize = sizeof(devmode);
		devmode.dmPelsWidth = width;
		devmode.dmPelsHeight = height;
		devmode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

		if( ChangeDisplaySettings(&devmode, CDS_TEST) != DISP_CHANGE_SUCCESSFUL ){
			return 1;
		}
		ChangeDisplaySettings(&devmode, CDS_FULLSCREEN);
	}
	*/



	HDC hDC;
	int pfdID;
	BOOL bResult;

	//긯긏긜깑긲긅??긞긣
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof (PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0,
		0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,
		0,
		0
	};

	//긢긫귽긚긓깛긡긌긚긣롦벦
	hDC = GetDC(hWnd);

	//긯긏긜깑긲긅??긞긣귩롦벦
	pfdID = ChoosePixelFormat(hDC, &pfd);	
	if( pfdID == 0 ){ return 1; }

	//긯긏긜깑긲긅??긞긣귩럚믦
	bResult = SetPixelFormat(hDC, pfdID, &pfd);
	if( bResult == FALSE ){ return 1; }

	//긓깛긡긌긚긣귩럚믦
	hGLRC = wglCreateContext(hDC);
	if( hGLRC == NULL ){ return 1; }

	//긢긫귽긚긓깛긡긌긚긣됶뺳
	ReleaseDC(hWnd, hDC);

	//긘긚긡?긲긅깛긣뾭댰
	if( LanguageMode == 1 ){
		// ---English/Korean---

		//긲긅깛긣뼹갌굃굍 긕긘긞긏?긖귽긛갌18
		SystemFont = CreateFont((int)(((float)GameConfig.GetScreenHeight() / 480) * 18), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, HANGEUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "Dotum");
		//긲긅깛긣뼹갌굃굍 긕긘긞긏?긖귽긛갌12
		SystemSmallFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, HANGEUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "Dotum");
	}
	else{
		// ---English/Japanese---

		//긲긅깛긣뼹갌굃굍 긕긘긞긏?긖귽긛갌18
		//SystemFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "굃굍 긕긘긞긏");
		SystemFont = CreateFont((int)(((float)GameConfig.GetScreenHeight() / 480) * 18), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "굃굍 긕긘긞긏");
		//긲긅깛긣뼹갌굃굍 긕긘긞긏?긖귽긛갌12
		SystemSmallFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "굃굍 긕긘긞긏");
	}

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	//긡긏긚?긿긲긅깛긣뾭됪몴궻긲?귽깑뼹귩먠믦
	strcpy(TextureFontFname, TextureFontFilename);

	//긡긏긚?긿긲긅깛긣뾭됪몴귩롦벦
	TextureFont = LoadTexture(TextureFontFname, true, false);


	//HUD?뙸띪렃궯궲궋귡븧딇귩?됪궥귡띆뷭
	HUD_myweapon_x[0] = GameConfig.GetScreenWidth() - 140.0f;
	HUD_myweapon_y[0] = GameConfig.GetScreenHeight() - 40.0f;
	HUD_myweapon_z[0] = 0.86f;

	//HUD??뷈궻븧딇귩?됪궥귡띆뷭
	HUD_myweapon_x[1] = GameConfig.GetScreenWidth() - 72.0f;
	HUD_myweapon_y[1] = GameConfig.GetScreenHeight() - 25.0f;
	HUD_myweapon_z[1] = 0.93f;


#ifdef ENABLE_DEBUGCONSOLE
	InitDebugFontData();
	if( LoadDebugFontTexture() == false ){
		return 1;
	}
#endif


#ifdef ENABLE_OPENGL_LIBJPEGPNG
	//libjpeg룊딖돸
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
#endif

	return 0;
}

//! @brief 깏긜긞긣@n
//! 걁긂귻깛긤긂띍룷돸궔귞궻븳딞?궶궵걂
//! @param WindowCtrl WindowControl긏깋긚궻?귽깛?
//! @return 맟뚻갌0?뫲궭갌1?렪봲갌2
int D3DGraphics::ResetD3D(WindowControl *WindowCtrl)
{
	if( WindowCtrl == NULL ){ return 2; }

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_INIT, "Graphics", "OpenGL (Reset)");
#endif

	//깏??긚됶뺳
	CleanupD3Dresource();

	if( hGLRC != NULL ){
		wglDeleteContext(hGLRC);
		hGLRC = NULL;
	}


	//  됶뺳룉뿚궞궞귏궳
	//?궞궞궔귞룊딖돸룉뿚

	hWnd = WindowCtrl->GethWnd();


	// Borderless fullscreen.
// Reset時もディスプレイ解像度は変更しない。
	ChangeDisplaySettings(NULL, 0);
	/*
	//긲깑긚긏깏?깛돸
	if( fullscreenflag == true ){
		DEVMODE devmode;
		ZeroMemory(&devmode, sizeof(devmode));
		devmode.dmSize = sizeof(devmode);
		devmode.dmPelsWidth = width;
		devmode.dmPelsHeight = height;
		devmode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

		if( ChangeDisplaySettings(&devmode, CDS_TEST) != DISP_CHANGE_SUCCESSFUL ){
			return 1;
		}
		ChangeDisplaySettings(&devmode, CDS_FULLSCREEN);
	}
	*/



	HDC hDC;
	int pfdID;
	BOOL bResult;

	//긯긏긜깑긲긅??긞긣
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof (PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0,
		0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,
		0,
		0
	};

	//긢긫귽긚긓깛긡긌긚긣롦벦
	hDC = GetDC(hWnd);

	//긯긏긜깑긲긅??긞긣귩롦벦
	pfdID = ChoosePixelFormat(hDC, &pfd);	
	if( pfdID == 0 ){ return 1; }

	//긯긏긜깑긲긅??긞긣귩럚믦
	bResult = SetPixelFormat(hDC, pfdID, &pfd);
	if( bResult == FALSE ){ return 1; }

	//긓깛긡긌긚긣귩럚믦
	hGLRC = wglCreateContext(hDC);
	if( hGLRC == NULL ){ return 1; }

	//긢긫귽긚긓깛긡긌긚긣됶뺳
	ReleaseDC(hWnd, hDC);

	//긘긚긡?긲긅깛긣뾭댰
	if( LanguageMode == 1 ){
		// ---English/Korean---

		//긲긅깛긣뼹갌굃굍 긕긘긞긏?긖귽긛갌18
		SystemFont = CreateFont((int)(((float)GameConfig.GetScreenHeight() / 480) * 18), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, HANGEUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "Dotum");
		//긲긅깛긣뼹갌굃굍 긕긘긞긏?긖귽긛갌12
		SystemSmallFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, HANGEUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "Dotum");
	}
	else{
		// ---English/Japanese---

		//긲긅깛긣뼹갌굃굍 긕긘긞긏?긖귽긛갌18
		//SystemFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "굃굍 긕긘긞긏");
		SystemFont = CreateFont((int)(((float)GameConfig.GetScreenHeight() / 480) * 18), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "굃굍 긕긘긞긏");
		//긲긅깛긣뼹갌굃굍 긕긘긞긏?긖귽긛갌12
		SystemSmallFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "굃굍 긕긘긞긏");
	}

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	//긡긏긚?긿긲긅깛긣뾭됪몴귩롦벦
	TextureFont = LoadTexture(TextureFontFname, true, false);

#ifdef ENABLE_DEBUGCONSOLE
	if( LoadDebugFontTexture() == false ){
		return 1;
	}
#endif

	return 0;
}

//! @brief 됶뺳
//! @attention ?듫릶궼렔벍밒궸뚁귂뢯궠귢귏궥궕갂뼻렑밒궸뚁귂뢯궥궞궴귖됀?궳궥갃
void D3DGraphics::DestroyD3D()
{
	if( hGLRC == NULL ){ return; }

	CleanupD3Dresource();

	if( hGLRC != NULL ){
		wglDeleteContext(hGLRC);
		hGLRC = NULL;
	}

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CLEANUP, "Graphics", "OpenGL");
#endif

#ifdef ENABLE_OPENGL_LIBJPEGPNG
	//libjpeg됶뺳
	jpeg_destroy_decompress(&cinfo);
#endif
}

//! @brief 긢긫귽긚궻깏??긚귩됶뺳
void D3DGraphics::CleanupD3Dresource()
{
	if( TextureFont != -1 ){
		CleanupTexture(TextureFont);
		TextureFont = -1;
	}

#ifdef ENABLE_DEBUGCONSOLE
	if( TextureDebugFont != -1 ){
		CleanupTexture(TextureDebugFont);
		TextureDebugFont = -1;
	}
#endif

	for(int i=0; i<MAX_MODEL; i++){
		CleanupModel(i);
	}
	for(int i=0; i<MAX_TEXTURE; i++){
		CleanupTexture(i);
	}

	if( SystemFont != NULL ){
		DeleteObject(SystemFont);
		SystemFont = NULL;
	}
	if( SystemSmallFont != NULL ){
		DeleteObject(SystemSmallFont);
		SystemSmallFont = NULL;
	}
	if( now_SystemFontUStr != NULL ){
		delete [] now_SystemFontUStr;
		now_SystemFontUStr = NULL;
	}
	if( SystemFontListIdx != 0 ){
		glDeleteLists(SystemFontListIdx, SystemFontListIdxSize);
		SystemFontListIdx = 0;
	}
}

//! @brief 긾긢깑긲?귽깑귩벶귒뜛귔걁.x걂
//! @param filename 긲?귽깑뼹
//! @return 맟뚻갌긾긢깑궻긢??붥뜂걁0댥뤵걂?렪봲갌-1
int D3DGraphics::LoadModel(const char* filename)
{
	if( filename == NULL ){ return -1; }

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_LOAD, "Model", filename);
#endif

	int id = -1;
	FILE *fp;
	char buf[256];
	char str[256];

	int vertexs = 0;
	MODELVDATA *vertex = NULL;
	int polygons = 0;
	int *index = NULL;
	int in_vertexs, in_polygons;
	MODELVDATA *old_vertex;
	int *old_index;

	char stroks[] = " ;,";		//뗦먛귡빒럻쀱

	//뗴궋궲궋귡긢??붥뜂귩뭈궥
	for(int i=0; i<MAX_MODEL; i++){
		if( pmodel[i].useflag == false ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return -1; }

	//긲?귽깑귩벶귒뜛귔
	fp = fopen(filename, "r");
	if( fp == NULL ){ return -1; }

	//?긙긞긏긓?긤롦벦
	fgets(buf, 256, fp);
	buf[ strlen("xof 0302txt") ] = '\0';
	if( strcmp(buf, "xof 0302txt") != 0 ){
		fclose( fp );
		return -1;		//X긲?귽깑궳궶궋
	}

	while( fgets(buf, 256, fp) != NULL ){
		strcpy(str, buf);
		str[ strlen("Mesh") ] = '\0';
		if( strcmp(str, "Mesh") == 0 ){
			
			fgets(buf, 256, fp);
			in_vertexs = atoi(buf);

			if( vertexs == 0 ){
				//1궰뽞궻긽긞긘깄긢??궶귞궽갂쀌덃귩띿맟궥귡궬궚갃
				vertex = new MODELVDATA [in_vertexs];
			}
			else{
				//2궰뽞궻댥?궶귞갂쀌덃귩둴뺎궢뮳궢궲긓긯?궢갂뚀궋쀌덃궼랁룣갃
				old_vertex = vertex;
				vertex = new MODELVDATA [vertexs+in_vertexs];
				memcpy(vertex, old_vertex, sizeof(MODELVDATA)*vertexs);
				delete [] old_vertex;
			}

			for(int i=0; i<in_vertexs; i++){
				fgets(buf, 256, fp);
				vertex[i+vertexs].x = (float)atof( strtok(buf, stroks) ) * -1;
				vertex[i+vertexs].y = (float)atof( strtok(NULL, stroks) );
				vertex[i+vertexs].z = (float)atof( strtok(NULL, stroks) );
			}

			fgets(buf, 256, fp);

			fgets(buf, 256, fp);
			in_polygons = atoi(buf);

			if( polygons == 0 ){
				//1궰뽞궻귽깛긢긞긏긚긢??궶귞궽갂쀌덃귩띿맟궥귡궬궚갃
				index = new int [in_polygons*5];
			}
			else{
				//2궰뽞궻댥?궶귞갂쀌덃귩둴뺎궢뮳궢궲긓긯?궢갂뚀궋쀌덃궼랁룣갃
				old_index = index;
				index = new int [(polygons+in_polygons)*5];
				memcpy(index, old_index, sizeof(int)*polygons*5);
				delete [] old_index;
			}

			for(int i=0; i<in_polygons; i++){
				fgets(buf, 256, fp);
				index[(i+polygons)*5] = atoi( strtok(buf, stroks) );
				for(int j=0; j<index[(i+polygons)*5]; j++){
					index[(i+polygons)*5 + j + 1] = atoi( strtok(NULL, stroks) ) + vertexs;
				}
			}

			while( fgets(buf, 256, fp) != NULL ){
				strcpy(str, buf);
				str[ strlen(" MeshTextureCoords") ] = '\0';
				if( strcmp(str, " MeshTextureCoords") == 0 ){

					fgets(buf, 256, fp);
					if( atoi(buf) != in_vertexs ){ break; }

					for(int i=0; i<in_vertexs; i++){
						fgets(buf, 256, fp);
						vertex[i+vertexs].u = (float)atof( strtok(buf, stroks) );
						vertex[i+vertexs].v = (float)atof( strtok(NULL, stroks) );
					}

					break;
				}
			}

			vertexs += in_vertexs;
			polygons += in_polygons;
		}
	}

	//긲?귽깑긪깛긤깑귩됶뺳
	fclose( fp );

	float *VertexAry = new float [polygons*6*3];
	float *ColorAry = new float [polygons*6*4];
	float *ColorGrayAry = new float [polygons*6*4];
	float *TexCoordAry = new float [polygons*6*2];
	int vid;
	int cnt = 0;

	for(int i=0; i<polygons; i++){
		if( index[i*5] == 3 ){
			//랳둷?
			vid = index[i*5+1];
			VertexAry[0 + cnt*3] = vertex[vid].x;
			VertexAry[1 + cnt*3] = vertex[vid].y;
			VertexAry[2 + cnt*3] = vertex[vid].z;
			TexCoordAry[0 + cnt*2] = vertex[vid].u;
			TexCoordAry[1 + cnt*2] = vertex[vid].v;

			VertexAry[3 + cnt*3] = vertex[vid].x;
			VertexAry[4 + cnt*3] = vertex[vid].y;
			VertexAry[5 + cnt*3] = vertex[vid].z;
			TexCoordAry[2 + cnt*2] = vertex[vid].u;
			TexCoordAry[3 + cnt*2] = vertex[vid].v;

			vid = index[i*5+3];
			VertexAry[6 + cnt*3] = vertex[vid].x;
			VertexAry[7 + cnt*3] = vertex[vid].y;
			VertexAry[8 + cnt*3] = vertex[vid].z;
			TexCoordAry[4 + cnt*2] = vertex[vid].u;
			TexCoordAry[5 + cnt*2] = vertex[vid].v;

			vid = index[i*5+2];
			VertexAry[9 + cnt*3] = vertex[vid].x;
			VertexAry[10 + cnt*3] = vertex[vid].y;
			VertexAry[11 + cnt*3] = vertex[vid].z;
			TexCoordAry[6 + cnt*2] = vertex[vid].u;
			TexCoordAry[7 + cnt*2] = vertex[vid].v;

			VertexAry[12 + cnt*3] = vertex[vid].x;
			VertexAry[13 + cnt*3] = vertex[vid].y;
			VertexAry[14 + cnt*3] = vertex[vid].z;
			TexCoordAry[8 + cnt*2] = vertex[vid].u;
			TexCoordAry[9 + cnt*2] = vertex[vid].v;

			VertexAry[15 + cnt*3] = vertex[vid].x;
			VertexAry[16 + cnt*3] = vertex[vid].y;
			VertexAry[17 + cnt*3] = vertex[vid].z;
			TexCoordAry[10 + cnt*2] = vertex[vid].u;
			TexCoordAry[11 + cnt*2] = vertex[vid].v;

			cnt += 6;
		}
		else{
			//럏둷?
			vid = index[i*5+1];
			VertexAry[0 + cnt*3] = vertex[vid].x;
			VertexAry[1 + cnt*3] = vertex[vid].y;
			VertexAry[2 + cnt*3] = vertex[vid].z;
			TexCoordAry[0 + cnt*2] = vertex[vid].u;
			TexCoordAry[1 + cnt*2] = vertex[vid].v;

			VertexAry[3 + cnt*3] = vertex[vid].x;
			VertexAry[4 + cnt*3] = vertex[vid].y;
			VertexAry[5 + cnt*3] = vertex[vid].z;
			TexCoordAry[2 + cnt*2] = vertex[vid].u;
			TexCoordAry[3 + cnt*2] = vertex[vid].v;

			vid = index[i*5+4];
			VertexAry[6 + cnt*3] = vertex[vid].x;
			VertexAry[7 + cnt*3] = vertex[vid].y;
			VertexAry[8 + cnt*3] = vertex[vid].z;
			TexCoordAry[4 + cnt*2] = vertex[vid].u;
			TexCoordAry[5 + cnt*2] = vertex[vid].v;

			vid = index[i*5+2];
			VertexAry[9 + cnt*3] = vertex[vid].x;
			VertexAry[10 + cnt*3] = vertex[vid].y;
			VertexAry[11 + cnt*3] = vertex[vid].z;
			TexCoordAry[6 + cnt*2] = vertex[vid].u;
			TexCoordAry[7 + cnt*2] = vertex[vid].v;

			vid = index[i*5+3];
			VertexAry[12 + cnt*3] = vertex[vid].x;
			VertexAry[13 + cnt*3] = vertex[vid].y;
			VertexAry[14 + cnt*3] = vertex[vid].z;
			TexCoordAry[8 + cnt*2] = vertex[vid].u;
			TexCoordAry[9 + cnt*2] = vertex[vid].v;

			VertexAry[15 + cnt*3] = vertex[vid].x;
			VertexAry[16 + cnt*3] = vertex[vid].y;
			VertexAry[17 + cnt*3] = vertex[vid].z;
			TexCoordAry[10 + cnt*2] = vertex[vid].u;
			TexCoordAry[11 + cnt*2] = vertex[vid].v;

			cnt += 6;
		}
	}

	if( polygons > 0 ){
		//륡륃뺪봹쀱귩뾭댰
		ColorAry[0] = 1.0f;
		ColorAry[1] = 1.0f;
		ColorAry[2] = 1.0f;
		ColorAry[3] = 1.0f;
		for(int i=1; i<cnt; i++){
			memcpy(&(ColorAry[i*4]), ColorAry, sizeof(float)*4);
		}
		ColorGrayAry[0] = 0.8f;
		ColorGrayAry[1] = 0.8f;
		ColorGrayAry[2] = 0.8f;
		ColorGrayAry[3] = 1.0f;
		for(int i=1; i<cnt; i++){
			memcpy(&(ColorGrayAry[i*4]), ColorGrayAry, sizeof(float)*4);
		}
	}

	delete [] vertex;
	delete [] index;

	pmodel[id].useflag = true;
	pmodel[id].polygons = polygons;
	pmodel[id].VertexAry = VertexAry;
	pmodel[id].ColorAry = ColorAry;
	pmodel[id].ColorGrayAry = ColorGrayAry;
	pmodel[id].TexCoordAry = TexCoordAry;

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_COMPLETE, "", id);
#endif
	return id;
}

//! @brief 긾긢깑긲?귽깑궻뭷듩긢??귩띿맟걁긾?긲귻깛긐걂
//! @param idA 긾긢깑A궻긢??붥뜂
//! @param idB 긾긢깑B궻긢??붥뜂
//! @return 맟뚻갌륷궢궋긾긢깑궻긢??붥뜂걁0댥뤵걂?렪봲갌-1
//! @attention 긾긢깑A궴긾긢깑B궼갂뮯?릶갋?깏긕깛릶갋귽깛긢긞긏긚궕벏궣궳궇귡뷠뾴궕궇귟귏궥갃
//! @attention 궩귢궪귢궻긾긢깑긢??궕맫궢궘궶궋궔 뮯?릶궕댶궶귡뤾뜃갂렳뛱궸렪봲궢귏궥갃
int D3DGraphics::MorphingModel(int idA, int idB)
{
#ifdef ENABLE_DEBUGLOG
	char str[128];
	sprintf(str, "Create morphing model  ID : %d and %d", idA, idB);

	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_LOAD, "Model", str);
#endif

	//긢??궕맫궢궋궔뮧귊귡
	if( (idA < 0)||(MAX_MODEL <= idA) ){ return -1; }
	if( pmodel[idA].useflag == false ){ return -1; }
	if( (idB < 0)||(MAX_MODEL <= idB) ){ return -1; }
	if( pmodel[idB].useflag == false ){ return -1; }

	int idN = -1;
	int numpA, numpB;

	//뗴궋궲궋귡긢??붥뜂귩뭈궥
	for(int i=0; i<MAX_MODEL; i++){
		if( pmodel[i].useflag == false ){
			idN = i;
			break;
		}
	}
	if( idN == -1 ){ return -1; }

	//?깏긕깛릶귩롦벦
	numpA = pmodel[idA].polygons;
	numpB = pmodel[idB].polygons;

	//?깏긕깛릶궕벏궣궔궵궎궔뮧귊귡
	if( numpA != numpB ){ return -1; }

	float *VertexAry = new float [numpA*6*3];
	float *ColorAry = new float [numpA*6*4];
	float *ColorGrayAry = new float [numpA*6*4];
	float *TexCoordAry = new float [numpA*6*2];

	//둫뮯?귩벶귒뢯궢똶럁
	for(int i=0; i<numpA*6; i++){
		VertexAry[i*3 + 0] = (pmodel[idA].VertexAry[i*3 + 0] + pmodel[idB].VertexAry[i*3 + 0])/2;
		VertexAry[i*3 + 1] = (pmodel[idA].VertexAry[i*3 + 1] + pmodel[idB].VertexAry[i*3 + 1])/2;
		VertexAry[i*3 + 2] = (pmodel[idA].VertexAry[i*3 + 2] + pmodel[idB].VertexAry[i*3 + 2])/2;
	}

	//?긡깏귺깑륃뺪귩긓긯?
	for(int i=0; i<numpA*6; i++){
		ColorAry[i*4 + 0] = pmodel[idA].ColorAry[i*4 + 0];
		ColorAry[i*4 + 1] = pmodel[idA].ColorAry[i*4 + 1];
		ColorAry[i*4 + 2] = pmodel[idA].ColorAry[i*4 + 2];
		ColorAry[i*4 + 3] = pmodel[idA].ColorAry[i*4 + 3];
		ColorGrayAry[i*4 + 0] = pmodel[idA].ColorGrayAry[i*4 + 0];
		ColorGrayAry[i*4 + 1] = pmodel[idA].ColorGrayAry[i*4 + 1];
		ColorGrayAry[i*4 + 2] = pmodel[idA].ColorGrayAry[i*4 + 2];
		ColorGrayAry[i*4 + 3] = pmodel[idA].ColorGrayAry[i*4 + 3];
		TexCoordAry[i*2 + 0] = pmodel[idA].TexCoordAry[i*2 + 0];
		TexCoordAry[i*2 + 1] = pmodel[idA].TexCoordAry[i*2 + 1];
	}

	pmodel[idN].useflag = true;
	pmodel[idN].polygons = numpA;
	pmodel[idN].VertexAry = VertexAry;
	pmodel[idN].ColorAry = ColorAry;
	pmodel[idN].ColorGrayAry = ColorGrayAry;
	pmodel[idN].TexCoordAry = TexCoordAry;

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_COMPLETE, "", idN);
#endif
	return idN;
}

//! @brief 긾긢깑긲?귽깑귩됶뺳
//! @param id 긾긢깑궻긢??붥뜂
void D3DGraphics::CleanupModel(int id)
{
	if( (id < 0)||(MAX_MODEL <= id) ){ return; }
	if( pmodel[id].useflag == false ){ return; }

	delete pmodel[id].VertexAry;
	delete pmodel[id].ColorAry;
	delete pmodel[id].ColorGrayAry;
	delete pmodel[id].TexCoordAry;
	pmodel[id].useflag = false;

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CLEANUP, "Model", id);
#endif
}

//! @brief 벶귒뜛귒띙귒궻긾긢깑릶귩롦벦
//! @return 긾긢깑릶
int D3DGraphics::GetTotalModels()
{
	int cnt = 0;

	//럊뾭뭷궻뾴멹귩릶궑귡
	for(int i=0; i<MAX_MODEL; i++){
		if( pmodel[i].useflag == true ){ cnt += 1; }
	}

	return cnt;
}

//! @brief 긡긏긚?긿긲긅??긞긣귩둮뮗럔궳뵽믦
//! @param filename 긲?귽깑뼹
//! @param nowformat 뙸띪궻뵽빶뭠
//! @return 륷궫궶뵽빶뭠
int D3DGraphics::CheckTextureFileExtension(const char* filename, int nowformat)
{
	char filename2[MAX_PATH];

	//긲?귽깑뼹귩룷빒럻귉빾듂걁둮뮗럔뵽믦뾭걂
	for(int i=0; i<(int)strlen(filename); i++){
		filename2[i] = (char)tolower(filename[i]);
	}
	filename2[ strlen(filename) ] = '\0';

	//둮뮗럔궳긲?귽깑긲긅??긞긣귩뵽믦
	for(int i=(int)strlen(filename2)-1; i>0; i--){
		if( filename2[i] == '.' ){
			if( strcmp(&(filename2[i]), ".bmp") == 0 ){
				return 1;
			}
			if( strcmp(&(filename2[i]), ".dds") == 0 ){
				return 2;
			}
			if( strcmp(&(filename2[i]), ".jpeg") == 0 ){
				return 3;
			}
			if( strcmp(&(filename2[i]), ".jpg") == 0 ){
				return 3;
			}
			if( strcmp(&(filename2[i]), ".jpe") == 0 ){
				return 3;
			}
			if( strcmp(&(filename2[i]), ".png") == 0 ){
				return 4;
			}
		}
	}

	return nowformat;
}

//! @brief 긡긏긚?긿긲긅??긞긣귩긲?귽깑긶긞??궳뵽믦
//! @param filename 긲?귽깑뼹
//! @param nowformat 뙸띪궻뵽빶뭠
//! @return 륷궫궶뵽빶뭠
int D3DGraphics::CheckTextureFileTypeFlag(const char* filename, int nowformat)
{
	FILE *fp;
	unsigned char header[4];

	//긲?귽깑귩벶귒뜛귔
	fp = fopen(filename, "rb");
	if( fp == NULL ){ return false; }

	//긶긞??귩벶귔
	fread(header, 1, 4, fp);

	//긲?귽깑긪깛긤깑귩됶뺳
	fclose( fp );

	if( (header[0x00] == 'B')&&(header[0x01] == 'M') ){															//.bmp
		return 1;
	}
	if( (header[0x00] == 'D')&&(header[0x01] == 'D')&&(header[0x02] == 'S') ){									//.dds
		return 2;
	}
	if( (header[0x00] == 0xFF)&&(header[0x01] == 0xD8) ){														//.jpg
		return 3;
	}
	if( (header[0x00] == 0x89)&&(header[0x01] == 'P')&&(header[0x02] == 'N')&&(header[0x03] == 'G') ){			//.png?겍?뱰궼8긫귽긣
		return 4;
	}

	return nowformat;
}

//! @brief 긡긏긚?긿귩벶귒뜛귔
//! @param filename 긲?귽깑뼹
//! @param texturefont 긡긏긚?긿긲긅깛긣긲깋긐
//! @param BlackTransparent 뜒귩벁됡궥귡
//! @return 맟뚻갌긡긏긚?긿궻긢??붥뜂걁0댥뤵걂?렪봲갌-1
int D3DGraphics::LoadTexture(const char* filename, bool texturefont, bool BlackTransparent)
{
	//뼟럊뾭덙릶뫮랉
	UNREFERENCED_PARAMETER(texturefont);

	if( filename == NULL ){ return -1; }

	int id = -1;
	int format = 0;

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_LOAD, "Texture", filename);
#endif

	//뗴궋궲궋귡긢??붥뜂귩뭈궥
	for(int i=0; i<MAX_TEXTURE; i++){
		if( ptextures[i].useflag == false ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return -1; }

	//귏궦둮뮗럔궳긲?귽깑긲긅??긞긣귩뵽믦
	format = CheckTextureFileExtension(filename, format);

	//긲?귽깑긶긞??궻륃뺪궳귖둴봃
	format = CheckTextureFileTypeFlag(filename, format);

	//뫮돒궢궲궶궋긲긅??긞긣
	if( format == 0 ){ return -1; }

	if( format == 1 ){	// .bmp
		if( LoadBMPTexture(filename, BlackTransparent, &(ptextures[id])) == false ){
			return -1;
		}
	}
	if( format == 2 ){	// .dds
		if( LoadDDSTexture(filename, BlackTransparent, &(ptextures[id])) == false ){
			return -1;
		}
	}
	if( format == 3 ){	// .jpeg
		if( LoadJPEGTexture(filename, BlackTransparent, &(ptextures[id])) == false ){
			return -1;
		}
	}
	if( format == 4 ){	// .png
		if( LoadPNGTexture(filename, BlackTransparent, &(ptextures[id])) == false ){
			return -1;
		}
	}


	//긡긏긚?긿뾎뚼
	glEnable(GL_TEXTURE_2D);

	HDC hDC;
	hDC = GetDC(hWnd);
	wglMakeCurrent(hDC, hGLRC);
	glGenTextures(1, &(textureobjname[id]));
	ReleaseDC(hWnd, hDC);

	glBindTexture(GL_TEXTURE_2D, textureobjname[id]);

	//OpenGL궸긜긞긣
	int width = ptextures[id].width;
	int height = ptextures[id].height;
	unsigned char *data = ptextures[id].data;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//?긞긵?긞긵먠믦
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//뤸럁뜃맟
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//긡긏긚?긿뼰뚼
	glDisable(GL_TEXTURE_2D);

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	//OutputLog.WriteLog(LOG_COMPLETE, "", id);

	char str[32];
	sprintf(str, "ID갌%d (size:%dx%d)", id, width, height);
	OutputLog.WriteLog(LOG_COMPLETE, "", str);
#endif

	return id;


	/*
	unsigned char *data = new unsigned char [16*4];

	data[0*4 + 0] = 255;	data[0*4 + 1] = 255;	data[0*4 + 2] = 255;	data[0*4 + 3] = 255;
	data[1*4 + 0] = 0;		data[1*4 + 1] = 0;		data[1*4 + 2] = 0;		data[1*4 + 3] = 255;
	data[2*4 + 0] = 255;	data[2*4 + 1] = 255;	data[2*4 + 2] = 255;	data[2*4 + 3] = 255;
	data[3*4 + 0] = 0;		data[3*4 + 1] = 0;		data[3*4 + 2] = 0;		data[3*4 + 3] = 255;
	data[4*4 + 0] = 255;	data[4*4 + 1] = 0;		data[4*4 + 2] = 0;		data[4*4 + 3] = 255;
	data[5*4 + 0] = 0;		data[5*4 + 1] = 255;	data[5*4 + 2] = 0;		data[5*4 + 3] = 255;
	data[6*4 + 0] = 0;		data[6*4 + 1] = 0;		data[6*4 + 2] = 255;	data[6*4 + 3] = 255;
	data[7*4 + 0] = 0;		data[7*4 + 1] = 0;		data[7*4 + 2] = 0;		data[7*4 + 3] = 255;
	data[8*4 + 0] = 128;	data[8*4 + 1] = 0;		data[8*4 + 2] = 0;		data[8*4 + 3] = 255;
	data[9*4 + 0] = 0;		data[9*4 + 1] = 128;	data[9*4 + 2] = 0;		data[9*4 + 3] = 255;
	data[10*4 + 0] = 0;		data[10*4 + 1] = 0;		data[10*4 + 2] = 128;	data[10*4 + 3] = 255;
	data[11*4 + 0] = 0;		data[11*4 + 1] = 0;		data[11*4 + 2] = 0;		data[11*4 + 3] = 255;
	data[12*4 + 0] = 255;	data[12*4 + 1] = 255;	data[12*4 + 2] = 0;		data[12*4 + 3] = 255;
	data[13*4 + 0] = 255;	data[13*4 + 1] = 0;		data[13*4 + 2] = 255;	data[13*4 + 3] = 255;
	data[14*4 + 0] = 0;		data[14*4 + 1] = 255;	data[14*4 + 2] = 255;	data[14*4 + 3] = 255;
	data[15*4 + 0] = 255;	data[15*4 + 1] = 255;	data[15*4 + 2] = 255;	data[15*4 + 3] = 255;

	ptextures[id].data = data;
	ptextures[id].width = 4;
	ptextures[id].height = 4;

	ptextures[id].useflag = true;

	return id;
	*/
}

//! @brief BMP긲?귽깑귩벶귒뜛귔
//! @param filename 긲?귽깑뼹
//! @param BlackTransparent 뜒귩벁됡궥귡
//! @param ptexture 롷궚롦귡TEXTUREDATA?몾뫬궻?귽깛?
//! @return 맟뚻갌true?렪봲갌false
//! @attention 믅륂궼 LoadTexture()듫릶 궔귞뚁귂궬궥궞궴갃
bool D3DGraphics::LoadBMPTexture(const char* filename, bool BlackTransparent, TEXTUREDATA *ptexture)
{
	FILE *fp;
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int width, height;
	unsigned int index;

	//긲?귽깑귩벶귒뜛귔
	fp = fopen(filename, "rb");
	if( fp == NULL ){ return false; }

	//긶긞??귩벶귔
	fread(header, 1, 54, fp);

	if( (header[0x00] != 'B')||(header[0x01] != 'M') ){
		fclose(fp);
		return false;		//.bmp궳궼궶궋
	}

	// 긫귽긣봹쀱궔귞맢릶귩벶귒뜛귔
	dataPos = *(int*)&(header[0x0E]) + 14;
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	index = *(int*)&(header[0x1C]);

	//렳긢??궻먩벆귏궳댷벍
	fseek(fp, dataPos, SEEK_SET);

	unsigned char *data = new unsigned char [width*height*4];

	//둫긯긏긜깑4긮긞긣궶귞갂16륡긬깒긞긣긾?긤
	if( index == 4 ){
		unsigned char pixel;
		unsigned char pixelH;
		unsigned char pixelL;
		unsigned char *pallet = new unsigned char [16*4];
		fread(pallet, 1, 16*4, fp);

		for(int h=height-1; h>=0; h--){
			for(int w=0; w<((int)width/2); w++){
				fread(&pixel, 1, 1, fp);
				pixelH = (pixel >> 4)&0x0F;
				pixelL = pixel&0x0F;

				data[(h*width+w*2)*4 + 0] = pallet[pixelH*4 + 2];
				data[(h*width+w*2)*4 + 1] = pallet[pixelH*4 + 1];
				data[(h*width+w*2)*4 + 2] = pallet[pixelH*4 + 0];
				data[(h*width+w*2)*4 + 3] = 255;

				data[(h*width+w*2+1)*4 + 0] = pallet[pixelL*4 + 2];
				data[(h*width+w*2+1)*4 + 1] = pallet[pixelL*4 + 1];
				data[(h*width+w*2+1)*4 + 2] = pallet[pixelL*4 + 0];
				data[(h*width+w*2+1)*4 + 3] = 255;

				if( BlackTransparent == true ){
					//뜒궶귞궽벁됡궥귡
					if( (data[(h*width+w*2)*4 + 0] == 0)&&(data[(h*width+w*2)*4 + 1] == 0)&&(data[(h*width+w*2)*4 + 2] == 0) ){
						data[(h*width+w*2)*4 + 3] = 0;
					}
					if( (data[(h*width+w*2+1)*4 + 0] == 0)&&(data[(h*width+w*2+1)*4 + 1] == 0)&&(data[(h*width+w*2+1)*4 + 2] == 0) ){
						data[(h*width+w*2+1)*4 + 3] = 0;
					}
				}
			}

			if( (((int)width/2)%4) != 0 ){
				//4궻?릶궸궶귡귝궎궸긬긢귻깛긐
				unsigned char dummy[3];
				fread(&dummy, 1, 4 - (((int)width/2)%4), fp);
			}
		}

		delete []pallet;
	}

	//둫긯긏긜깑8긮긞긣궶귞갂256륡긬깒긞긣긾?긤
	if( index == 8 ){
		unsigned char pixel;
		unsigned char *pallet = new unsigned char [256*4];
		fread(pallet, 1, 256*4, fp);

		for(int h=height-1; h>=0; h--){
			for(int w=0; w<(int)width; w++){
				fread(&pixel, 1, 1, fp);

				data[(h*width+w)*4 + 0] = pallet[pixel*4 + 2];
				data[(h*width+w)*4 + 1] = pallet[pixel*4 + 1];
				data[(h*width+w)*4 + 2] = pallet[pixel*4 + 0];
				data[(h*width+w)*4 + 3] = 255;

				if( BlackTransparent == true ){
					//뜒궶귞궽벁됡궥귡
					if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
						data[(h*width+w)*4 + 3] = 0;
					}
				}
			}

			if( (width%4) != 0 ){
				//4궻?릶궸궶귡귝궎궸긬긢귻깛긐
				unsigned char dummy[3];
				fread(&dummy, 1, 4 - (width%4), fp);
			}
		}

		delete []pallet;
	}

	//둫긯긏긜깑24긮긞긣궶귞갂긲깑긇깋?
	if( index == 24 ){
		unsigned char pixel[3];
		for(int h=height-1; h>=0; h--){
			for(int w=0; w<(int)width; w++){
				fread(&pixel, 1, 3, fp);

				data[(h*width+w)*4 + 0] = pixel[2];
				data[(h*width+w)*4 + 1] = pixel[1];
				data[(h*width+w)*4 + 2] = pixel[0];
				data[(h*width+w)*4 + 3] = 255;

				if( BlackTransparent == true ){
					//뜒궶귞궽벁됡궥귡
					if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
						data[(h*width+w)*4 + 3] = 0;
					}
				}
			}

			if( (width%4) != 0 ){
				//4궻?릶궸궶귡귝궎궸긬긢귻깛긐
				unsigned char dummy[3];
				fread(&dummy, 1, 4 - (width%4), fp);
			}
		}
	}

	//둫긯긏긜깑32긮긞긣궶귞갂긲깑긇깋?
	if( index == 32 ){
		unsigned char pixel[4];
		for(int h=height-1; h>=0; h--){
			for(int w=0; w<(int)width; w++){
				fread(&pixel, 1, 4, fp);

				data[(h*width+w)*4 + 0] = pixel[2];
				data[(h*width+w)*4 + 1] = pixel[1];
				data[(h*width+w)*4 + 2] = pixel[0];
				data[(h*width+w)*4 + 3] = 255;

				if( BlackTransparent == true ){
					//뜒궶귞궽벁됡궥귡
					if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
						data[(h*width+w)*4 + 3] = 0;
					}
				}
			}

			//긽긾갌1긯긏긜깑걖4긫귽긣 궶귞궽갂긬긢귻깛긐븉뾴갃
		}
	}

	//긲?귽깑긪깛긤깑귩됶뺳
	fclose( fp );

	//?몾뫬궸묆볺
	ptexture->data = data;
	ptexture->width = width;
	ptexture->height = height;

	ptexture->useflag = true;

	return true;
}

//! @brief DDS긲?귽깑귩벶귒뜛귔
//! @param filename 긲?귽깑뼹
//! @param BlackTransparent 뜒귩벁됡궥귡
//! @param ptexture 롷궚롦귡TEXTUREDATA?몾뫬궻?귽깛?
//! @return 맟뚻갌true?렪봲갌false
//! @attention 믅륂궼 LoadTexture()듫릶 궔귞뚁귂궬궥궞궴갃
bool D3DGraphics::LoadDDSTexture(const char* filename, bool BlackTransparent, TEXTUREDATA *ptexture)
{
	FILE *fp;
	unsigned char header[124+4];
	unsigned int width, height;
	unsigned int index;
	unsigned int flag;
	unsigned int Caps;

	//긲?귽깑귩벶귒뜛귔
	fp = fopen(filename, "rb");
	if( fp == NULL ){ return false; }

	//긶긞??귩벶귔
	fread(header, 1, 124+4, fp);

	if( (header[0x00] != 'D')||(header[0x01] != 'D')||(header[0x02] != 'S')||(header[0x03] != ' ') ){
		fclose(fp);
		return false;		//.dds궳궼궶궋
	}

	// 긫귽긣봹쀱궔귞맢릶귩벶귒뜛귔
	width = *(int*)&(header[0x10]);
	height = *(int*)&(header[0x0C]);
	index = *(int*)&(header[0x58]);
	flag = *(int*)&(header[0x08]);
	Caps = *(int*)&(header[0x70]);

	if( (index != 16)&&(index != 32) ){
		fclose(fp);
		return false;		//뫮돒궢궲궶궋긲긅??긞긣
	}

	if( (flag & 0x00020000)&&(Caps & 0x00400000) ){		//DDSD_MIPMAPCOUNT갋DDSCAPS_MIPMAP
		//?긞긵?긞긵륃뺪벶귒뜛귒걁???걂
		fread(header, 1, 128, fp);
	}

	unsigned char *data = new unsigned char [width*height*4];

	for(int h=0; h<(int)height; h++){
		for(int w=0; w<(int)width; w++){
			unsigned char pixel[4];
			fread(&pixel, 1, index/8, fp);

			if( index == 16 ){		//둫긯긏긜깑16긮긞긣
				data[(h*width+w)*4 + 0] = (pixel[1]<<4)&0xF0;
				data[(h*width+w)*4 + 1] = pixel[0]&0xF0;
				data[(h*width+w)*4 + 2] = (pixel[0]<<4)&0xF0;
				data[(h*width+w)*4 + 3] = pixel[1]&0xF0;
			}
			if( index == 32 ){		//둫긯긏긜깑32긮긞긣
				data[(h*width+w)*4 + 0] = pixel[2];
				data[(h*width+w)*4 + 1] = pixel[1];
				data[(h*width+w)*4 + 2] = pixel[0];
				data[(h*width+w)*4 + 3] = pixel[3];
			}

			if( BlackTransparent == true ){
				//뜒궶귞궽벁됡궥귡
				if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
					data[(h*width+w)*4 + 3] = 0;
				}
			}
		}
	}

	//긲?귽깑긪깛긤깑귩됶뺳
	fclose( fp );

	//?몾뫬궸묆볺
	ptexture->data = data;
	ptexture->width = width;
	ptexture->height = height;

	ptexture->useflag = true;

	return true;
}

//! @brief JPEG긲?귽깑귩벶귒뜛귔
//! @param filename 긲?귽깑뼹
//! @param BlackTransparent 뜒귩벁됡궥귡
//! @param ptexture 롷궚롦귡TEXTUREDATA?몾뫬궻?귽깛?
//! @return 맟뚻갌true?렪봲갌false
//! @attention 믅륂궼 LoadTexture()듫릶 궔귞뚁귂궬궥궞궴갃
bool D3DGraphics::LoadJPEGTexture(const char* filename, bool BlackTransparent, TEXTUREDATA *ptexture)
{
#ifdef ENABLE_OPENGL_LIBJPEGPNG
	FILE *fp;
	JSAMPARRAY img;
	unsigned int width, height;

	//긲?귽깑귩벶귒뜛귔
	fp = fopen(filename, "rb");
	if( fp == NULL ){ return false; }
	jpeg_stdio_src(&cinfo, fp);

	//긬깋긽??궻먠믦
	jpeg_read_header(&cinfo, true);

	//밯둎둎럑
	jpeg_start_decompress(&cinfo);

	//쀌덃둴뺎
	img = (JSAMPARRAY)new JSAMPROW [cinfo.output_height];
	for(int i=0; i<(int)cinfo.output_height; i++){
		img[i] = (JSAMPROW)new JSAMPLE [cinfo.output_width * cinfo.out_color_components];
	}

	//밯둎
	while( cinfo.output_scanline < cinfo.output_height ) {
		jpeg_read_scanlines(&cinfo, img + cinfo.output_scanline, cinfo.output_height - cinfo.output_scanline);
	}

	//밯둎뢎뿹
	jpeg_finish_decompress(&cinfo);


	//긲?귽깑긪깛긤깑귩됶뺳
	fclose( fp );


	// 긫귽긣봹쀱궔귞맢릶귩벶귒뜛귔
	width = (int)cinfo.output_width;
	height = (int)cinfo.output_height;

	unsigned char *data = new unsigned char [width*height*4];

	for(int h=0; h<(int)height; h++){
		//1깋귽깛빁롦벦
		JSAMPROW p = img[h];

		for(int w=0; w<(int)width; w++){
			data[(h*width+w)*4 + 0] = p[w*3 + 0];
			data[(h*width+w)*4 + 1] = p[w*3 + 1];
			data[(h*width+w)*4 + 2] = p[w*3 + 2];
			data[(h*width+w)*4 + 3] = 255;

			if( BlackTransparent == true ){
				//뜒궶귞궽벁됡궥귡
				if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
					data[(h*width+w)*4 + 3] = 0;
				}
			}

		}
	}

	//쀌덃됶뺳
	for(int i=0; i<(int)cinfo.output_height; i++){
		delete [] img[i];
	}
	delete [] img;

	//?몾뫬궸묆볺
	ptexture->data = data;
	ptexture->width = width;
	ptexture->height = height;

	ptexture->useflag = true;

	return true;
#else
	return false;
#endif
}

//! @brief PNG긲?귽깑귩벶귒뜛귔
//! @param filename 긲?귽깑뼹
//! @param BlackTransparent 뜒귩벁됡궥귡
//! @param ptexture 롷궚롦귡TEXTUREDATA?몾뫬궻?귽깛?
//! @return 맟뚻갌true?렪봲갌false
//! @attention 믅륂궼 LoadTexture()듫릶 궔귞뚁귂궬궥궞궴갃
bool D3DGraphics::LoadPNGTexture(const char* filename, bool BlackTransparent, TEXTUREDATA *ptexture)
{
#ifdef ENABLE_OPENGL_LIBJPEGPNG
	FILE *fp;
	png_byte sig[4];
	png_structp pPng;
    png_infop pInfo;
	unsigned int width, height;
	bool pallet;

	//긲?귽깑귩벶귒뜛귔
	fp = fopen(filename, "rb");
	if( fp == NULL ){ return false; }

	//PNG긲?귽깑궔뵽믦
	fread(sig, 4, 1, fp);
	if( png_sig_cmp(sig, 0, 4) != 0 ){
		fclose(fp);
		return false;	//.png궳궼궶궋
	}

	//?몾뫬귩룊딖돸
	pPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    pInfo = png_create_info_struct(pPng);

	//륃뺪귩롦벦
	png_init_io(pPng, fp);
	png_set_sig_bytes(pPng, 4);
	png_read_info(pPng, pInfo);

	//긡긏긚?긿궻묈궖궠귩롦벦
	width = png_get_image_width(pPng, pInfo);
	height =  png_get_image_height(pPng, pInfo);

	//귽깛??깒긚뺴렜궔뵽믦
	if( png_get_interlace_type(pPng, pInfo) != PNG_INTERLACE_NONE ){
		png_destroy_read_struct(&pPng, &pInfo, (png_infopp)NULL);
		fclose(fp);
		return false;	//귽깛??깒긚궸궼뫮돒궢궶궋갃
	}

	//긮긞긣?뱗뵽믦
	if( png_get_bit_depth(pPng, pInfo) != 8 ){
		png_destroy_read_struct(&pPng, &pInfo, (png_infopp)NULL);
		fclose(fp);
		return false;	//?뱗궕8긮긞긣댥둖궼뫮돒궢궲궶궋갃
	}

	//긇깋??귽긵뵽믦
	if( (png_get_color_type(pPng, pInfo) == PNG_COLOR_TYPE_GRAY)||(png_get_color_type(pPng, pInfo) == PNG_COLOR_TYPE_GRAY_ALPHA) ){
		png_destroy_read_struct(&pPng, &pInfo, (png_infopp)NULL);
		fclose(fp);
		return false;	//긐깒?긚긑?깑궸궼뫮돒궢궲궶궋갋갋궯궴럙귦귢귡궻궳룣둖갃걁뼟뙚뤪걂
	}

	//긇깋??귽긵롦벦
	if( png_get_color_type(pPng, pInfo) == PNG_COLOR_TYPE_PALETTE ){
		pallet = true;
	}
	else{
		pallet = false;
	}

	unsigned char *data = new unsigned char [width*height*4];

	//귺깑긲??긿깛긨깑귩룊딖돸
	png_set_add_alpha(pPng, 0xff, PNG_FILLER_AFTER);

	if( pallet == false ){
		// tRNS?긿깛긏궕궇귢궽갂귺깑긲??긿깛긨깑궸빾듂
		if( png_get_valid(pPng, pInfo, PNG_INFO_tRNS) ){
			png_set_tRNS_to_alpha(pPng);
		}

		//1깋귽깛빁궻띿떾쀌덃귩둴뺎
		png_bytep buf = new png_byte[width*4];

		for(int h=0; h<(int)height; h++){
			//1깋귽깛빁롦벦
			png_read_row(pPng, buf, NULL);

			for(int w=0; w<(int)width; w++){
				data[(h*width+w)*4 + 0] = buf[w*4 + 0];
				data[(h*width+w)*4 + 1] = buf[w*4 + 1];
				data[(h*width+w)*4 + 2] = buf[w*4 + 2];
				data[(h*width+w)*4 + 3] = buf[w*4 + 3];

				if( BlackTransparent == true ){
					//뜒궶귞궽벁됡궥귡
					if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
						data[(h*width+w)*4 + 3] = 0;
					}
				}
			}
		}

		//1깋귽깛빁궻띿떾쀌덃귩됶뺳
		delete [] buf;
	}
	else{
		png_colorp palette;
		int num;

		//긬깒긞긣롦벦
		png_get_PLTE(pPng, pInfo, &palette, &num);

		//1깋귽깛빁궻띿떾쀌덃귩둴뺎
		png_bytep buf = new png_byte[width];

		for(int h=0; h<(int)height; h++){
			//1깋귽깛빁롦벦
			png_read_row(pPng, buf, NULL);

			for(int w=0; w<(int)width; w++){
				data[(h*width+w)*4 + 0] = palette[ buf[w] ].red;
				data[(h*width+w)*4 + 1] = palette[ buf[w] ].green;
				data[(h*width+w)*4 + 2] = palette[ buf[w] ].blue;
				data[(h*width+w)*4 + 3] = 255;

				if( BlackTransparent == true ){
					//뜒궶귞궽벁됡궥귡
					if( (data[(h*width+w)*4 + 0] == 0)&&(data[(h*width+w)*4 + 1] == 0)&&(data[(h*width+w)*4 + 2] == 0) ){
						data[(h*width+w)*4 + 3] = 0;
					}
				}
			}
		}

		//1깋귽깛빁궻띿떾쀌덃귩됶뺳
		delete [] buf;
	}

	//됶뺳
	png_read_end(pPng, NULL);
	png_destroy_read_struct(&pPng, &pInfo, (png_infopp)NULL);

	//긲?귽깑긪깛긤깑귩됶뺳
	fclose( fp );

	//?몾뫬궸묆볺
	ptexture->data = data;
	ptexture->width = width;
	ptexture->height = height;

	ptexture->useflag = true;

	return true;
#else
	return false;
#endif
}

#ifdef ENABLE_DEBUGCONSOLE
//! @brief 긢긫긞긏뾭긲긅깛긣귩벶귒뜛귔
//! @return 맟뚻갌true?렪봲갌false
//! @attention 궞궻듫릶귩뚁귂뢯궥멟궸갂InitDebugFontData()듫릶귩렳뛱궢궲궘궬궠궋갃
bool D3DGraphics::LoadDebugFontTexture()
{
	int charwidth = 8;
	int charheight = 16;
	int width = charwidth * 16;
	int height = charheight * 8;

	int datacnt = 0;
	int id = -1;

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_LOAD, "Texture", "DebugFontTexture");
#endif

	//딓궸벶귒뜛귏귢궲궋귡궶귞렪봲
	if( TextureDebugFont != -1 ){ return false; }

	//뗴궋궲궋귡긢??붥뜂귩뭈궥
	for(int i=0; i<MAX_TEXTURE; i++){
		if( ptextures[i].useflag == false ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return false; }

	unsigned char *data = new unsigned char [width*height*4];

	//맕뚥긓?긤궕볺궯궲궋귡2뛱 32빒럻빁궼갂뗴뿎궸궥귡갃
	for(int cnt_y=0; cnt_y<2; cnt_y++){
		for(int line_y=0; line_y<charheight; line_y++){
			for(int cnt_x=0; cnt_x<16; cnt_x++){
				for(int line_x=0; line_x<charwidth; line_x++){
					data[datacnt + 0] = 0;
					data[datacnt + 1] = 0;
					data[datacnt + 2] = 0;
					data[datacnt + 3] = 0;

					datacnt += 4;
				}
			}
		}
	}

	//6뛱빁궻긢??귩띿맟
	for(int cnt_y=0; cnt_y<6; cnt_y++){
		for(int line_y=0; line_y<charheight; line_y++){
			for(int cnt_x=0; cnt_x<16; cnt_x++){
				for(int line_x=(charwidth-1); line_x>=0; line_x--){
					unsigned char mask;

					//긮긞긣뵽믦뾭?긚긏띿맟
					switch(line_x){
						case 0: mask = 0x01; break;
						case 1: mask = 0x02; break;
						case 2: mask = 0x04; break;
						case 3: mask = 0x08; break;
						case 4: mask = 0x10; break;
						case 5: mask = 0x20; break;
						case 6: mask = 0x40; break;
						case 7: mask = 0x80; break;
						default: mask = 0x00;		//럷렳뤵긄깋?
					}

					//둤뱰긮긞긣궕1궶귞뵏갂0궶귞뜒갃
					if( (DebugFontData[cnt_y*16 + cnt_x][line_y] & mask) != 0 ){
						data[datacnt + 0] = 255;
						data[datacnt + 1] = 255;
						data[datacnt + 2] = 255;
						data[datacnt + 3] = 255;
					}
					else{
						data[datacnt + 0] = 0;
						data[datacnt + 1] = 0;
						data[datacnt + 2] = 0;
						data[datacnt + 3] = 0;
					}

					datacnt += 4;
				}
			}
		}
	}

	//긡긏긚?긿뾎뚼
	glEnable(GL_TEXTURE_2D);

	HDC hDC;
	hDC = GetDC(hWnd);
	wglMakeCurrent(hDC, hGLRC);
	glGenTextures(1, &(textureobjname[id]));
	ReleaseDC(hWnd, hDC);

	glBindTexture(GL_TEXTURE_2D, textureobjname[id]);

	//OpenGL궸긜긞긣
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//?긞긵?긞긵먠믦
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//뤸럁뜃맟
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//긡긏긚?긿뼰뚼
	glDisable(GL_TEXTURE_2D);


	//?몾뫬궸묆볺
	ptextures[id].data = data;
	ptextures[id].width = width;
	ptextures[id].height = height;

	ptextures[id].useflag = true;


#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	//OutputLog.WriteLog(LOG_COMPLETE, "", id);

	char str[32];
	sprintf(str, "ID갌%d (size:%dx%d)", id, width, height);
	OutputLog.WriteLog(LOG_COMPLETE, "", str);
#endif

	//긡긏긚?긿ID먠믦
	TextureDebugFont = id;
	return true;
}
#endif

//! @brief 긡긏긚?긿궻긖귽긛귩롦벦
//! @param id 긡긏긚?긿궻긢??붥뜂
//! @param width 븴귩롷궚롦귡?귽깛?
//! @param height 뛼궠귩롷궚롦귡?귽깛?
//! @return 맟뚻갌0?렪봲갌1
//! @attention 긖?긲긃귽긚궻긖귽긛귩롦벦궢귏궥갃GPU궸깓?긤궠귢궫긖귽긛궳궇귟갂긡긏긚?긿걁뙸븿걂궴댶궶귡뤾뜃궕궇귟귏궥갃
int D3DGraphics::GetTextureSize(int id, int *width, int *height)
{
	//뼰뚼궶긢??붥뜂궕럚믦궠귢궲궋궫귞갂룉뿚궧궦뺅궥갃
	if( id == -1 ){ return 1; }
	if( ptextures[id].useflag == false ){ return 1; }
	if( width == NULL ){ return 1; }
	if( height == NULL ){ return 1; }

	*width = ptextures[id].width;
	*height = ptextures[id].height;

	return 0;
}

//! @brief 긡긏긚?긿귩럚믦
//! @param TextureID 긡긏긚?긿궻긢??붥뜂
void D3DGraphics::SetTexture(int TextureID)
{
	if( now_textureid == TextureID ){
		return;
	}

	//OpenGL궸긜긞긣
	glBindTexture(GL_TEXTURE_2D, textureobjname[TextureID]);

	now_textureid = TextureID;
}

//! @brief 긡긏긚?긿귩됶뺳
//! @param id 긡긏긚?긿궻긢??붥뜂
void D3DGraphics::CleanupTexture(int id)
{
	if( (id < 0)||(MAX_TEXTURE <= id) ){ return; }
	if( ptextures[id].useflag == false ){ return; }

	delete ptextures[id].data;
	glDeleteTextures(1, &(textureobjname[id]));
	ptextures[id].useflag = false;

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CLEANUP, "Texture", id);
#endif
}

//! @brief 벶귒뜛귒띙귒궻긡긏긚?긿릶귩롦벦
//! @return 긡긏긚?긿릶
int D3DGraphics::GetTotalTextures()
{
	int cnt = 0;

	//럊뾭뭷궻뾴멹귩릶궑귡
	for(int i=0; i<MAX_TEXTURE; i++){
		if( ptextures[i].useflag == true ){ cnt += 1; }
	}

	return cnt;
}

//! @brief 멣궲궻?됪룉뿚귩둎럑
//! @return 맟뚻갌0?렪봲갌1
//! @attention ?됪룉뿚궻띍룊궸뚁귂뢯궥뷠뾴궕궇귟귏궥갃
int D3DGraphics::StartRender()
{
	HDC hDC;

	hDC = BeginPaint(hWnd, &Paint_ps);

	//긓깛긡긌긚긣귩럚믦
	wglMakeCurrent(hDC, hGLRC);

	//룊딖돸
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//뜫뜃룉뿚걁벁됡뾎뚼돸걂
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//귺깑긲?긡긚긣
	glAlphaFunc(GL_GREATER, 0.0f);
	glEnable(GL_ALPHA_TEST);

	//2D긘긚긡?긲긅깛긣Z띆뷭룊딖돸
	SystemFont_posz = 1.0f;

	return 0;
}

//! @brief 멣궲궻?됪룉뿚귩뢎뿹
//! @return 맟뚻갌false?렪봲갌true
//! @attention ?됪룉뿚궻띍뚣궸뚁귂뢯궥뷠뾴궕궇귟귏궥갃
bool D3DGraphics::EndRender()
{
	glFlush();

	wglMakeCurrent(NULL, NULL);

	EndPaint(hWnd, &Paint_ps);

	return false;
}

//! @brief Z긫긞긲?귩깏긜긞긣
void D3DGraphics::ResetZbuffer()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

//! @brief 깗?깑긤뗴듩귩뙱?걁0,0,0걂궸뽣궥?궶궵
void D3DGraphics::ResetWorldTransform()
{
	float camera_y_flag;

	if( fabsf(camera_ry) <= (float)M_PI/2 ){
		camera_y_flag = 1.0f;
	}
	else{
		camera_y_flag = -1.0f;
	}

	//?뤵or?돷귩뛀궋궲궋귡궶귞궽갂긇긽깋궻뤵븫럚믦귩뵿?궥귡갃겍DirectX궴럅뾩귩뜃귦궧귡궫귕
	if( fabsf(camera_ry) == (float)M_PI/2 ){
		camera_y_flag *= -1;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(viewangle*(180.0f/(float)M_PI), (float)width/height, CLIPPINGPLANE_NEAR, CLIPPINGPLANE_FAR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera_x*-1, camera_y, camera_z, camera_x*-1 + cosf(camera_rx*-1 + (float)M_PI)*cosf(camera_ry), camera_y + sinf(camera_ry), camera_z + sinf(camera_rx*-1 + (float)M_PI)*cosf(camera_ry), 0.0f, camera_y_flag, 0.0f);
}

//! @brief 깗?깑긤뗴듩궻띆뷭갋둷뱗갋둮묈뿦귩먠믦
//! @param x X띆뷭
//! @param y Y띆뷭
//! @param z Z띆뷭
//! @param rx 돘렡둷뱗
//! @param ry 뢢렡둷뱗
//! @param size 둮묈뿦
void D3DGraphics::SetWorldTransform(float x, float y, float z, float rx, float ry, float size)
{
	SetWorldTransform(x, y, z, rx, ry, 0.0f, size);
}

//! @brief 깗?깑긤뗴듩궻띆뷭갋둷뱗갋둮묈뿦귩먠믦
//! @param x X띆뷭
//! @param y Y띆뷭
//! @param z Z띆뷭
//! @param rx 돘렡둷뱗
//! @param ry1 뢢렡둷뱗
//! @param ry2 뢢렡둷뱗
//! @param size 둮묈뿦
void D3DGraphics::SetWorldTransform(float x, float y, float z, float rx, float ry1, float ry2, float size)
{
	ResetWorldTransform();

	glMatrixMode(GL_MODELVIEW);

	glTranslatef(x*-1, y, z);
	glRotatef(rx*-1*(180.0f/(float)M_PI), 0.0f, 1.0f, 0.0f);
	glRotatef(ry1*(180.0f/(float)M_PI), 1.0f, 0.0f, 0.0f);
	glRotatef(ry2*-1*(180.0f/(float)M_PI), 0.0f, 0.0f, 1.0f);
	glScalef(size, size, size);
}

//! @brief 깗?깑긤뗴듩궻띆뷭갋둷뱗갋둮묈뿦귩먠믦걁긄긲긃긏긣뾭걂
//! @param x X띆뷭
//! @param y Y띆뷭
//! @param z Z띆뷭
//! @param rx 돘렡둷뱗
//! @param ry 뢢렡둷뱗
//! @param rt 됷?둷뱗
//! @param size 둮묈뿦
void D3DGraphics::SetWorldTransformEffect(float x, float y, float z, float rx, float ry, float rt, float size)
{
	ResetWorldTransform();

	glMatrixMode(GL_MODELVIEW);

	glTranslatef(x*-1, y, z);
	glRotatef(rx*-1*(180.0f/(float)M_PI), 0.0f, 1.0f, 0.0f);
	glRotatef(ry*-1*(180.0f/(float)M_PI), 0.0f, 0.0f, 1.0f);
	glRotatef(rt*(180.0f/(float)M_PI), 1.0f, 0.0f, 0.0f);
	glScalef(size, size, size);
}

//! @brief 깗?깑긤뗴듩귩릐궕븧딇귩렃궰뤾룋궸먠믦
//! @param x X띆뷭
//! @param y Y띆뷭
//! @param z Z띆뷭
//! @param mx 롨뙰귩뙱?궸궢궫 긾긢깑궻X띆뷭
//! @param my 롨뙰귩뙱?궸궢궫 긾긢깑궻Y띆뷭
//! @param mz 롨뙰귩뙱?궸궢궫 긾긢깑궻Z띆뷭
//! @param rx 돘렡둷뱗
//! @param ry 뢢렡둷뱗
//! @param size 둮묈뿦
void D3DGraphics::SetWorldTransformHumanWeapon(float x, float y, float z, float mx, float my, float mz, float rx, float ry, float size)
{
	ResetWorldTransform();

	glMatrixMode(GL_MODELVIEW);

	glTranslatef(x*-1, y, z);
	glRotatef(rx*-1*(180.0f/(float)M_PI), 0.0f, 1.0f, 0.0f);
	glRotatef(ry*(180.0f/(float)M_PI), 1.0f, 0.0f, 0.0f);
	glTranslatef(mx*-1, my, mz);
	glScalef(size, size, size);
}

//! @brief 깗?깑긤뗴듩귩룋렃궢궲궋귡븧딇귩?됪궥귡뤾룋궸먠믦
//! @param rotation 븧딇귩됷?궠궧귡
//! @param camera_rx 긇긽깋궻돘렡둷뱗
//! @param camera_ry 긇긽깋궻뢢렡둷뱗
//! @param rx 븧딇궻궻뢢렡둷뱗
//! @param size ?됪긖귽긛
//! @note rotation갋갋?true갌뙸띪렃궯궲궋귡븧딇궳궥갃?false갌?뷈궻븧딇궳궥갃걁rx 궼뼰럨궠귢귏궥걂
//! @todo 댧뭫귘긖귽긛궻뷊뮧맢
void D3DGraphics::SetWorldTransformPlayerWeapon(bool rotation, float camera_rx, float camera_ry, float rx, float size)
{
	float screenX, screenY, screenZ;
	double modelview[16];
	double projection[16];
	int viewport[4];
	double objX, objY, objZ;

	if( rotation == true ){
		screenX = HUD_myweapon_x[0];
		screenY = GameConfig.GetScreenHeight() - HUD_myweapon_y[0];
		screenZ = HUD_myweapon_z[0];
	}
	else{
		screenX = HUD_myweapon_x[1];
		screenY = GameConfig.GetScreenHeight() - HUD_myweapon_y[1];
		screenZ = HUD_myweapon_z[1];
	}

	ResetWorldTransform();

	glMatrixMode(GL_MODELVIEW);

	//긾긢깑긮깄?뛱쀱갋벁럨뱤뎓뛱쀱갋긮깄???긣귩롦벦
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	//긚긏깏?깛띆뷭궔귞긆긳긙긃긏긣띆뷭귩땫귕귡
	gluUnProject(screenX, screenY, screenZ, modelview, projection, viewport, &objX, &objY, &objZ);

	//size = size * 0.3f;
	size = size * (0.0004f*GameConfig.GetScreenHeight()*GameConfig.GetScreenHeight() - 0.92f*GameConfig.GetScreenHeight() + 650.0f) / 1000.f;

	//뛱쀱똶럁
	glTranslated(objX, objY, objZ);
	glRotatef(camera_rx*(180.0f/(float)M_PI), 0.0f, 1.0f, 0.0f);
	glRotatef(camera_ry*-1*(180.0f/(float)M_PI), 0.0f, 0.0f, 1.0f);
	if( rotation == true ){
		glRotatef(rx*-1*(180.0f/(float)M_PI), 0.0f, 1.0f, 0.0f);
	}
	else{
		glRotatef(180, 0.0f, 1.0f, 0.0f);
	}
	glScalef(size, size, size);
}

//! @brief 깗?깑긤뗴듩궻띆뷭귩롦벦
//! @param *x x렡귩롷궚롦귡?귽깛?
//! @param *y y렡귩롷궚롦귡?귽깛?
//! @param *z z렡귩롷궚롦귡?귽깛?
void D3DGraphics::GetWorldTransformPos(float *x, float *y, float *z)
{
	if( (x == NULL)||(y == NULL)||(z == NULL) ){ return; }

	GLfloat model[16];
	glMatrixMode(GL_MODELVIEW);
	glGetFloatv(GL_MODELVIEW_MATRIX, model);
	*x = model[12];
	*y = model[13];
	*z = model[14];
}

//! @brief 긲긅긐귩먠믦
//! @param enablefog 긲긅긐뾎뚼돸
//! @param skynumber 뗴궻붥뜂
void D3DGraphics::SetFog(bool enablefog, int skynumber)
{
	if( enablefog == true ){
		float fogColor[4];

		//뗴궻붥뜂궸귝귟륡귩뙂믦
		switch(skynumber){
			case 1:		fogColor[0] = 0.25f;	fogColor[1] = 0.25f+0.0625;	fogColor[2] = 0.25f;	fogColor[3] = 1.0f;		break;
			case 2:		fogColor[0] = 0.0625;	fogColor[1] = 0.0625;		fogColor[2] = 0.0625;	fogColor[3] = 1.0f;		break;
			case 3:		fogColor[0] = 0.0f;		fogColor[1] = 0.0625;		fogColor[2] = 0.125;	fogColor[3] = 1.0f;		break;
			case 4:		fogColor[0] = 0.125;	fogColor[1] = 0.0625;		fogColor[2] = 0.0625;	fogColor[3] = 1.0f;		break;
			case 5:		fogColor[0] = 0.25f;	fogColor[1] = 0.125;		fogColor[2] = 0.125;	fogColor[3] = 1.0f;		break;
			default:	fogColor[0] = 0.0f;		fogColor[1] = 0.0f;			fogColor[2] = 0.0f;		fogColor[3] = 1.0f;		break;
		}

		float fog_st = 100;
		float fog_end = 800;
		glFogi(GL_FOG_MODE, GL_LINEAR);
		glFogfv(GL_FOG_COLOR, fogColor);
		glHint(GL_FOG_HINT, GL_NICEST);
		glFogf(GL_FOG_START, fog_st);
		glFogf(GL_FOG_END, fog_end);

		glEnable(GL_FOG);
	}
	else{
		glDisable(GL_FOG);
	}
}

//! @brief 긇긽깋걁럨?걂귩먠믦
//! @param in_camera_x 긇긽깋궻X띆뷭
//! @param in_camera_y 긇긽깋궻Y띆뷭
//! @param in_camera_z 긇긽깋궻Z띆뷭
//! @param in_camera_rx 긇긽깋궻돘렡둷뱗
//! @param in_camera_ry 긇긽깋궻뢢렡둷뱗걁-꺨?꺨댥볙걂
//! @param in_viewangle 럨뽰둷
//! @todo ?뤵/?돷귩뛀궘궴갂룋렃븧딇궻긾긢깑?됪궻뛀궖궕궓궔궢궘궶귡갃
void D3DGraphics::SetCamera(float in_camera_x, float in_camera_y, float in_camera_z, float in_camera_rx, float in_camera_ry, float in_viewangle)
{
	glViewport(0, 0, width, height);
	
	camera_x = in_camera_x;
	camera_y = in_camera_y;
	camera_z = in_camera_z;
	camera_rx = in_camera_rx;
	camera_ry = in_camera_ry;
	viewangle = in_viewangle;

	ResetWorldTransform();
}

//! @brief ?긞긵긢??귩롦귟뜛귔
//! @param in_BlockDataIF 긳깓긞긏긢??
//! @param directory 긳깓긞긏긢??궕뫔띪궥귡긢귻깒긏긣깏
void D3DGraphics::LoadMapdata(BlockDataInterface* in_BlockDataIF, const char *directory)
{
	//긳깓긞긏긢??궕럚믦궠귢궲궋궶궚귢궽갂룉뿚궢궶궋갃
	if( in_BlockDataIF == NULL ){ return; }
	if( directory == NULL ){ return; }

	char fname[MAX_PATH];
	char fnamefull[MAX_PATH];

	//긏깋긚귩먠믦
	BlockDataIF = in_BlockDataIF;

	//긳깓긞긏릶귩롦벦
	bs = BlockDataIF->GetTotaldatas();

	//긡긏긚?긿벶귒뜛귒
	for(int i=0; i<TOTAL_BLOCKTEXTURE; i++){
		//긡긏긚?긿뼹귩롦벦
		BlockDataIF->GetTexture(fname, i);

		if( strcmp(fname, "") == 0 ){	//럚믦궠귢궲궋궶궚귢궽갂룉뿚궢궶궋
			mapTextureID[i] = -1;
		}
		else{
			//걏긢귻깒긏긣깏?긲?귽깑뼹걐귩맯맟궢갂벶귒뜛귔
			strcpy(fnamefull, directory);
			strcat(fnamefull, fname);
			mapTextureID[i] = LoadTexture(fnamefull, false, false);
		}
	}
}

//! @brief ?긞긵긢??귩?됪
//! @param wireframe 깗귽깂?긲깒???렑
void D3DGraphics::RenderMapdata(bool wireframe)
{
	//긳깓긞긏긢??궕벶귒뜛귏귢궲궋궶궚귢궽갂룉뿚궢궶궋갃
	if( BlockDataIF == NULL ){ return; }

	blockdata data;
	int textureID;
	int vID[4];
	int uvID[4];
	float *VertexAry = new float [bs*6 * 6*3];
	float *ColorAry = new float [bs*6 * 6*4];
	float *TexCoordAry = new float [bs*6 * 6*2];

	if( wireframe == true ){
		int linecolor = GetColorCode(0.0f,1.0f,0.0f,1.0f);

		//깗귽깂?긲깒???렑
		for(int i=0; i<bs; i++){
			BlockDataIF->Getdata(&data, i);
			Renderline(data.x[0], data.y[0], data.z[0], data.x[1], data.y[1], data.z[1], linecolor);
			Renderline(data.x[1], data.y[1], data.z[1], data.x[2], data.y[2], data.z[2], linecolor);
			Renderline(data.x[2], data.y[2], data.z[2], data.x[3], data.y[3], data.z[3], linecolor);
			Renderline(data.x[3], data.y[3], data.z[3], data.x[0], data.y[0], data.z[0], linecolor);
			Renderline(data.x[4], data.y[4], data.z[4], data.x[5], data.y[5], data.z[5], linecolor);
			Renderline(data.x[5], data.y[5], data.z[5], data.x[6], data.y[6], data.z[6], linecolor);
			Renderline(data.x[6], data.y[6], data.z[6], data.x[7], data.y[7], data.z[7], linecolor);
			Renderline(data.x[7], data.y[7], data.z[7], data.x[4], data.y[4], data.z[4], linecolor);
			Renderline(data.x[0], data.y[0], data.z[0], data.x[4], data.y[4], data.z[4], linecolor);
			Renderline(data.x[1], data.y[1], data.z[1], data.x[5], data.y[5], data.z[5], linecolor);
			Renderline(data.x[2], data.y[2], data.z[2], data.x[6], data.y[6], data.z[6], linecolor);
			Renderline(data.x[3], data.y[3], data.z[3], data.x[7], data.y[7], data.z[7], linecolor);
		}
		return;
	}

	//봹쀱뾎뚼돸
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	for(textureID=0; textureID<TOTAL_BLOCKTEXTURE; textureID++){
		int cnt = 0;

		//긡긏긚?긿궕맫륂궸벶귒뜛귕궲궋궶궚귢궽먠믦
		if( mapTextureID[textureID] == -1 ){
			//긡긏긚?긿뼰뚼
			glDisable(GL_TEXTURE_2D);
		}
		else if( ptextures[ mapTextureID[textureID] ].useflag == false ){
			//긡긏긚?긿뼰뚼
			glDisable(GL_TEXTURE_2D);
		}
		else{
			//긡긏긚?긿뾎뚼
			glEnable(GL_TEXTURE_2D);

			//긡긏긚?긿귩긜긞긣
			SetTexture(mapTextureID[textureID]);
		}

		for(int i=0; i<bs; i++){
			//긢??롦벦
			BlockDataIF->Getdata(&data, i);

			for(int j=0; j<6; j++){
				//긡긏긚?긿궻긢??붥뜂귩롦벦
				int ID = data.material[j].textureID;

				if( textureID == ID ){
					//뽋궻뮯?긢??궻듫쁀븊궚귩롦벦
					blockdataface(j, &vID[0], &uvID[0]);

					//뮯?봹쀱귩뾭댰
					VertexAry[0 + cnt*18] = data.x[ vID[1] ]*-1;		VertexAry[1 + cnt*18] = data.y[ vID[1] ];	VertexAry[2 + cnt*18] = data.z[ vID[1] ];
					VertexAry[3 + cnt*18] = data.x[ vID[1] ]*-1;		VertexAry[4 + cnt*18] = data.y[ vID[1] ];	VertexAry[5 + cnt*18] = data.z[ vID[1] ];
					VertexAry[6 + cnt*18] = data.x[ vID[0] ]*-1;		VertexAry[7 + cnt*18] = data.y[ vID[0] ];	VertexAry[8 + cnt*18] = data.z[ vID[0] ];
					VertexAry[9 + cnt*18] = data.x[ vID[2] ]*-1;		VertexAry[10 + cnt*18] = data.y[ vID[2] ];	VertexAry[11 + cnt*18] = data.z[ vID[2] ];
					VertexAry[12 + cnt*18] = data.x[ vID[3] ]*-1;		VertexAry[13 + cnt*18] = data.y[ vID[3] ];	VertexAry[14 + cnt*18] = data.z[ vID[3] ];
					VertexAry[15 + cnt*18] = data.x[ vID[3] ]*-1;		VertexAry[16 + cnt*18] = data.y[ vID[3] ];	VertexAry[17 + cnt*18] = data.z[ vID[3] ];

					//륡륃뺪봹쀱귩뾭댰
					ColorAry[0 + cnt*24] = data.material[j].shadow;
					ColorAry[1 + cnt*24] = data.material[j].shadow;
					ColorAry[2 + cnt*24] = data.material[j].shadow;
					ColorAry[3 + cnt*24] = 1.0f;
					for(int k=1; k<6; k++){
						memcpy(&(ColorAry[k*4 + cnt*24]), &(ColorAry[cnt*24]), sizeof(float)*4);
					}

					//UV띆뷭봹쀱귩뾭댰
					TexCoordAry[0 + cnt*12] = data.material[j].u[ uvID[1] ];	TexCoordAry[1 + cnt*12] = data.material[j].v[ uvID[1] ];
					TexCoordAry[2 + cnt*12] = data.material[j].u[ uvID[1] ];	TexCoordAry[3 + cnt*12] = data.material[j].v[ uvID[1] ];
					TexCoordAry[4 + cnt*12] = data.material[j].u[ uvID[0] ];	TexCoordAry[5 + cnt*12] = data.material[j].v[ uvID[0] ];
					TexCoordAry[6 + cnt*12] = data.material[j].u[ uvID[2] ];	TexCoordAry[7 + cnt*12] = data.material[j].v[ uvID[2] ];
					TexCoordAry[8 + cnt*12] = data.material[j].u[ uvID[3] ];	TexCoordAry[9 + cnt*12] = data.material[j].v[ uvID[3] ];
					TexCoordAry[10 + cnt*12] = data.material[j].u[ uvID[3] ];	TexCoordAry[11 + cnt*12] = data.material[j].v[ uvID[3] ];

					cnt += 1;
				}
			}
		}

		//?됪
		glVertexPointer(3, GL_FLOAT, 0, VertexAry);
		glColorPointer(4, GL_FLOAT, 0, ColorAry);
		glTexCoordPointer(2, GL_FLOAT, 0, TexCoordAry);
		glDrawArrays(GL_TRIANGLE_STRIP, 1, 6*cnt-2);
	}

	//봹쀱뼰뚼돸
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	delete [] VertexAry;
	delete [] ColorAry;
	delete [] TexCoordAry;
}

//! @brief ?긞긵긡긏긚?긿귩롦벦
//! @param id 긡긏긚?긿붥뜂
//! @return 긡긏긚?긿궻긢??붥뜂걁렪봲갌-1걂
int D3DGraphics::GetMapTextureID(int id)
{
	if( (id < 0)||(TOTAL_BLOCKTEXTURE <= id ) ){ return -1; }
	return mapTextureID[id];
}

//! @brief ?긞긵긢??귩됶뺳
void D3DGraphics::CleanupMapdata()
{
	//긡긏긚?긿귩됶뺳
	for(int i=0; i<TOTAL_BLOCKTEXTURE; i++){
		CleanupTexture(mapTextureID[i]);
	}

	bs = 0;

	BlockDataIF = NULL;
}

//! @brief 긾긢깑긲?귽깑귩?됪
//! @param id_model 긾긢깑궻긢??붥뜂
//! @param id_texture 긡긏긚?긿궻긢??붥뜂
//! @param darkflag 긾긢깑귩댠궘궥귡
//! @param HUDmode HUD?렑뾭긾긢깑궸먠믦
//! @param nomodel 긾긢깑?됪뼰뚼돸
void D3DGraphics::RenderModel(int id_model, int id_texture, bool darkflag, bool HUDmode, bool nomodel, bool disable_culling, float alpha, float model_brightness)
{
	UNREFERENCED_PARAMETER(alpha);
	UNREFERENCED_PARAMETER(model_brightness);
	if (id_model == -1 || pmodel[id_model].useflag == false) { return; }

	if (nomodel == true) {
		Renderline(1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, GetColorCode(1.0f, 0.5f, 0.0f, 1.0f));
		Renderline(0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, GetColorCode(1.0f, 0.5f, 0.0f, 1.0f));
		Renderline(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, GetColorCode(1.0f, 0.5f, 0.0f, 1.0f));
		return;
	}

	if (disable_culling) {
		glDisable(GL_CULL_FACE);
	}

	if (id_texture == -1 || ptextures[id_texture].useflag == false) {
		glDisable(GL_TEXTURE_2D);
	}
	else {
		glEnable(GL_TEXTURE_2D);
		SetTexture(id_texture);
	}

	float* ColorAry = darkflag ? pmodel[id_model].ColorGrayAry : pmodel[id_model].ColorAry;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, pmodel[id_model].VertexAry);
	glColorPointer(4, GL_FLOAT, 0, ColorAry);
	glTexCoordPointer(2, GL_FLOAT, 0, pmodel[id_model].TexCoordAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 1, pmodel[id_model].polygons * 6 - 2);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if (disable_culling) {
		glEnable(GL_CULL_FACE);
	}
}

//! @brief 붋귩?됪
//! @param id_texture 긡긏긚?긿궻긢??붥뜂
//! @param alpha 벁뼻뱗?걁0.0?1.0?0.0갌뒶멣벁뼻걂
//! @param nomodel 붋궻?됪뼰뚼돸
void D3DGraphics::RenderBoard(int id_texture, float alpha, bool nomodel)
{
	//긡긏긚?긿궕먠믦궠귢궲궋궶궚귢궽갂룉뿚궢궶궋갃
	if( id_texture == -1 ){ return; }
	if( ptextures[id_texture].useflag == false ){ return; }

	if( nomodel == true ){
		Renderline(0.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, GetColorCode(0.75f,0.75f,0.75f,1.0f));
		Renderline(0.0f, -0.5f, -0.5f, 0.0f, -0.5f, 0.5f, GetColorCode(0.75f,0.75f,0.75f,1.0f));
		Renderline(0.0f, -0.5f, 0.5f, 0.0f, 0.5f, 0.5f, GetColorCode(0.75f,0.75f,0.75f,1.0f));
		Renderline(0.0f, 0.5f, 0.5f, 0.0f, 0.5f, -0.5f, GetColorCode(0.75f,0.75f,0.75f,1.0f));
		return;
	}

	float VertexAry[4*3];
	float ColorAry[4*4];
	float TexCoordAry[4*2];

	//긡긏긚?긿뾎뚼
	glEnable(GL_TEXTURE_2D);

	//긡긏긚?긿귩긜긞긣
	SetTexture(id_texture);

	//뮯?봹쀱귩뾭댰
	VertexAry[0] = 0.0f;	VertexAry[1] = 0.5f;	VertexAry[2] = 0.5f;
	VertexAry[3] = 0.0f;	VertexAry[4] = -0.5f;	VertexAry[5] = 0.5f;
	VertexAry[6] = 0.0f;	VertexAry[7] = 0.5f;	VertexAry[8] = -0.5f;
	VertexAry[9] = 0.0f;	VertexAry[10] = -0.5f;	VertexAry[11] = -0.5f;

	//륡륃뺪봹쀱귩뾭댰
	ColorAry[0] = 1.0f;
	ColorAry[1] = 1.0f;
	ColorAry[2] = 1.0f;
	ColorAry[3] = alpha;
	for(int i=1; i<4; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(float)*4);
	}

	//UV띆뷭봹쀱귩뾭댰
	TexCoordAry[0] = 1.0f;	TexCoordAry[1] = 0.0f;
	TexCoordAry[2] = 1.0f;	TexCoordAry[3] = 1.0f;
	TexCoordAry[4] = 0.0f;	TexCoordAry[5] = 0.0f;
	TexCoordAry[6] = 0.0f;	TexCoordAry[7] = 1.0f;

	//봹쀱뾎뚼돸
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//?됪
	glVertexPointer(3, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_FLOAT, 0, ColorAry);
	glTexCoordPointer(2, GL_FLOAT, 0, TexCoordAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//봹쀱뼰뚼돸
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

//! @brief 긄긲긃긏긣?됪뾭먠믦
void D3DGraphics::StartEffectRender()
{
	glDisable(GL_DEPTH_TEST);
}

//! @brief 긄긲긃긏긣?됪뾭먠믦귩됶룣
void D3DGraphics::EndEffectRender()
{
	glEnable(GL_DEPTH_TEST);
}

//! @brief 됪뽋궻뼻귡궠귩먠믦
//! @param Width 븴
//! @param Height 뛼궠
//! @param Brightness 됪뽋궻뼻귡궠?걁0 궳븉빾갂1 댥뤵궳뼻귡궠궻뱗뜃궋걂
void D3DGraphics::ScreenBrightness(int Width, int Height, int Brightness)
{
	//뼻귡궠븉빾궶귞룉뿚궢궶궋걁똹쀊돸걂
	if( Brightness == 0 ){ return; }

	//벁뼻뱗귩먠믦궢갂?됪
	float alpha = 0.02f * Brightness;
	Draw2DBox(0, 0, Width, Height, GetColorCode(1.0f,1.0f,1.0f,alpha));
}

//! @brief 걓긢긫긞긏뾭걕뭷륲멄?렑
void D3DGraphics::RenderCenterline()
{
	ResetWorldTransform();
	Renderline(100.0f, 0.0f, 0.0f, -100.0f, 0.0f, 0.0f, GetColorCode(1.0f,0.0f,0.0f,1.0f));
	Renderline(0.0f, 100.0f, 0.0f, 0.0f, -100.0f, 0.0f, GetColorCode(0.0f,1.0f,0.0f,1.0f));
	Renderline(0.0f, 0.0f, 100.0f, 0.0f, 0.0f, -100.0f, GetColorCode(0.0f,0.0f,1.0f,1.0f));
}

//! @brief 걓긢긫긞긏뾭걕멄?렑
void D3DGraphics::Renderline(float x1, float y1, float z1, float x2, float y2, float z2, int color)
{
	float VertexAry[2*3];
	unsigned char ColorAry[2*4];

	//긡긏긚?긿뼰뚼
	glDisable(GL_TEXTURE_2D);

	//뮯?봹쀱귩뾭댰
	VertexAry[0] = (float)x1*-1;	VertexAry[1] = (float)y1;	VertexAry[2] = (float)z1;
	VertexAry[3] = (float)x2*-1;	VertexAry[4] = (float)y2;	VertexAry[5] = (float)z2;

	//륡륃뺪봹쀱귩뾭댰
	Set4ByteBigEndian(&(ColorAry[0]), color);
	memcpy(&(ColorAry[4]), ColorAry, sizeof(unsigned char)*4);

	//봹쀱뾎뚼돸
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//?렑
	glVertexPointer(3, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glDrawArrays(GL_LINE_STRIP, 0, 2);

	//봹쀱뼰뚼돸
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

//! @brief 띍귖뮮궋뛱궻빒럻릶귩롦벦
//! @param str 빒럻쀱?걁둂뛱긓?긤갌됀걂
//! @return 빒럻릶
//! @attention ?깑?긫귽긣빒럻궼 1빒럻궇궫귟 2 궴궢궲긇긂깛긣궠귢귏궥갃
int D3DGraphics::StrMaxLineLen(const char *str)
{
	int maxlen = 0;
	int cnt = 0;

	for(int i=0; i<(int)strlen(str); i++){
		if( str[i] == '\n' ){
			if( maxlen < cnt ){
				maxlen = cnt;
			}
			cnt = 0;
		}
		else{
			cnt += 1;
		}
	}

	if( maxlen < cnt ){
		maxlen = cnt;
	}

	return maxlen;
}

//! @brief 빒럻귩?렑걁긘긚긡?긲긅깛긣럊뾭걂
//! @param x x띆뷭
//! @param y y띆뷭
//! @param str 빒럻쀱?걁둂뛱긓?긤갌됀걂
//! @param color 륡
//! @warning ?듫릶궼1긲깒??듩궳100됷귏궳궢궔뚁귂뢯궧귏궧귪갃걁OpenGL긓귺궻귒걂
//! @warning <b>?렑궼뷄륂궸믟뫊궳궥갃</b>됪뽋볙궳돺뱗귖뚁귂뢯궥궴긬긲긅??깛긚궸뎓떯궢귏궥갃
//! @warning걏둂뛱긓?긤귩뒋뾭궢덇뱗궸?렑궥귡걐걏볷?뚭궕뷠뾴궶궋빒럻궼긡긏긚?긿긲긅깛긣귩뒋뾭궥귡걐궶궵궻뫮돒귩뛳궣궲궘궬궠궋갃
//! @attention 긲긅깛긣궻롰쀞귘긖귽긛궼뚂믦궳궥갃?빒럻귩볫뢣궸뢣궺궲뿧뫬뒾귩뢯궠궶궋궴뙥궸궘궘궶귟귏궥갃
//! @todo 1빒럻뽞궕뙁궚귡뤾뜃궕궇귡갃
void D3DGraphics::Draw2DMSFontText(int x, int y, const char *str, int color)
{
	if( str == NULL ){ return; }

	int len = (int)strlen(str);
	WCHAR *ustr;

	y += 18;

	Start2DRender();
	glEnable(GL_DEPTH_TEST);

	//긡긏긚?긿뼰뚼
	glDisable(GL_TEXTURE_2D);

	//Unicode빒럻쀱귉빾듂
	ustr = new WCHAR [len+1];
	MultiByteToWideChar(CP_ACP,	0, str, -1, ustr, len + 1);

	//륷궫궶빒럻쀱궶귞갂깏??긚귩띿귟뮳궥
	if( lstrcmpW(ustr, now_SystemFontUStr) != 0 ){
		GLuint listIdx;
		HDC hDC;

		//뚀궋긢??귩랁룣
		glDeleteLists(SystemFontListIdx, SystemFontListIdxSize);
		delete [] now_SystemFontUStr;

		//긢긫귽긚긓깛긡긌긚긣먠믦
		hDC = GetDC(hWnd);
		wglMakeCurrent(hDC, hGLRC);
		SelectObject(hDC, SystemFont);

		//긢귻긚긵깒귽깏긚긣귩띿맟
		listIdx = glGenLists(len);
		wglUseFontBitmapsW(hDC, ustr[0], 1, listIdx);	//???
		for(int i=0; i<lstrlenW(ustr); i++){
			wglUseFontBitmapsW(hDC, ustr[i], 1, listIdx+i);
		}

		//긢긫귽긚긓깛긡긌긚긣봯딙
		ReleaseDC(hWnd, hDC);

		//먠믦귩딯?
		now_SystemFontUStr = new WCHAR [len+1];
		lstrcpyW(now_SystemFontUStr, ustr);
		SystemFontListIdx = listIdx;
		SystemFontListIdxSize = len;
	}

	//띆뷭궴륡귩먠믦
	glBitmap(0, 0, 0, 0, 10, 0, NULL);
	glRasterPos3f((float)x, (float)y, SystemFont_posz);
	glColor4ub((color>>24)&0xFF, (color>>16)&0xFF, (color>>8)&0xFF, color&0xFF);

	for(int i=0; i<lstrlenW(ustr); i++){
		if( ustr[i] == '\n' ){
			//둂뛱궥귡
			y += 19;
			glRasterPos3f((float)x, (float)y, SystemFont_posz);
		}
		else{
			//긢귻긚긵깒귽깏긚긣?됪
			glCallList(SystemFontListIdx + i);
		}
	}

	SystemFont_posz -= 0.01f;

	//Unicode빒럻쀱궻봯딙
	delete [] ustr;

	//glDisable(GL_DEPTH_TEST);
	End2DRender();
}

//! @brief 빒럻귩?렑걁긘긚긡?긲긅깛긣럊뾭걂걓긚긑?깏깛긐??븊궖걕
//! @param x x띆뷭
//! @param y y띆뷭
//! @param str 빒럻쀱?걁둂뛱긓?긤갌됀걂
//! @param color 륡
//! @attention 640x480몜뱰궻됶몴뱗궔귞뙸됶몴뱗귉긚긑?깏깛긐궢귏궥갃궩귢댥둖궼Draw2DMSFontText()듫릶궴벏뱳궳궥갃
void D3DGraphics::Draw2DMSFontTextScaling(int x, int y, const char *str, int color)
{
	if( str == NULL ){ return; }

	float scaling_x = (float)GameConfig.GetScreenHeight() / 480;//(float)GameConfig.GetScreenWidth() / 640;
	float scaling_y = (float)GameConfig.GetScreenHeight() / 480;

	Draw2DMSFontText((int)(scaling_x * x), (int)(scaling_y * y), str, color);
}

//! @brief 빒럻귩뭷돍뫓궑궳?렑걁긘긚긡?긲긅깛긣럊뾭걂
//! @param x x띆뷭
//! @param y y띆뷭
//! @param w 돘궻묈궖궠
//! @param h 뢢궻묈궖궠
//! @param str 빒럻쀱?걁둂뛱긓?긤갌됀걂
//! @param color 륡
void D3DGraphics::Draw2DMSFontTextCenter(int x, int y, int w, int h, const char *str, int color)
{
	//뼟럊뾭덙릶뫮랉
	UNREFERENCED_PARAMETER(h);

	if( str == NULL ){ return; }

	int fonthalfsize = (int)(((float)GameConfig.GetScreenHeight() / 480) * 9);

	//Draw2DMSFontText(x + (w/2 - (StrMaxLineLen(str)*9/2)), y, str, color);
	Draw2DMSFontText(x + (w/2 - (StrMaxLineLen(str)*fonthalfsize/2)), y, str, color);
}

//! @brief 빒럻귩뭷돍뫓궑궳?렑걁긘긚긡?긲긅깛긣럊뾭걂걓긚긑?깏깛긐??븊궖걕
//! @param x x띆뷭
//! @param y y띆뷭
//! @param w 돘궻묈궖궠
//! @param h 뢢궻묈궖궠
//! @param str 빒럻쀱?걁둂뛱긓?긤갌됀걂
//! @param color 륡
//! @attention 640x480몜뱰궻됶몴뱗궔귞뙸됶몴뱗귉긚긑?깏깛긐궢귏궥갃궩귢댥둖궼Draw2DMSFontTextCenter()듫릶궴벏뱳궳궥갃
void D3DGraphics::Draw2DMSFontTextCenterScaling(int x, int y, int w, int h, const char *str, int color)
{
	if( str == NULL ){ return; }

	float scaling_x = (float)GameConfig.GetScreenHeight() / 480;//(float)GameConfig.GetScreenWidth() / 640;
	float scaling_y = (float)GameConfig.GetScreenHeight() / 480;

	Draw2DMSFontTextCenter((int)(scaling_x * x), (int)(scaling_y * y), w, h, str, color);
}

//! @brief 빒럻귩?렑걁긘긚긡?긲긅깛긣럊뾭갂룷궠궋걂
//! @param x x띆뷭
//! @param y y띆뷭
//! @param str 빒럻쀱?걁둂뛱긓?긤갌됀걂
//! @param color 륡
//! @note 딈?럅뾩궼Draw2DMSFontText()궴벏뱳궳궥궕갂빒럻궕룷궠궘궶귟귏궥갃
//! @todo 벏덇빒럻쀱귩믅륂긲긅깛긣긖귽긛궳뢯궥궴긫긐궕뢯귡궔귖궢귢궶궋갃걁뾴뙚뤪걂
void D3DGraphics::Draw2DMSSmallFontText(int x, int y, const char *str, int color)
{
	if( str == NULL ){ return; }

	int len = (int)strlen(str);
	WCHAR *ustr;

	y += 12;

	Start2DRender();
	glEnable(GL_DEPTH_TEST);

	//긡긏긚?긿뼰뚼
	glDisable(GL_TEXTURE_2D);

	//Unicode빒럻쀱귉빾듂
	ustr = new WCHAR [len+1];
	MultiByteToWideChar(CP_ACP,	0, str, -1, ustr, len + 1);

	//륷궫궶빒럻쀱궶귞갂깏??긚귩띿귟뮳궥
	if( lstrcmpW(ustr, now_SystemFontUStr) != 0 ){
		GLuint listIdx;
		HDC hDC;

		//뚀궋긢??귩랁룣
		glDeleteLists(SystemFontListIdx, SystemFontListIdxSize);
		delete [] now_SystemFontUStr;

		//긢긫귽긚긓깛긡긌긚긣먠믦
		hDC = GetDC(hWnd);
		wglMakeCurrent(hDC, hGLRC);
		SelectObject(hDC, SystemSmallFont);

		//긢귻긚긵깒귽깏긚긣귩띿맟
		listIdx = glGenLists(len);
		wglUseFontBitmapsW(hDC, ustr[0], 1, listIdx);	//???
		for(int i=0; i<lstrlenW(ustr); i++){
			wglUseFontBitmapsW(hDC, ustr[i], 1, listIdx+i);
		}

		//긢긫귽긚긓깛긡긌긚긣봯딙
		ReleaseDC(hWnd, hDC);

		//먠믦귩딯?
		now_SystemFontUStr = new WCHAR [len+1];
		lstrcpyW(now_SystemFontUStr, ustr);
		SystemFontListIdx = listIdx;
		SystemFontListIdxSize = len;
	}

	//띆뷭궴륡귩먠믦
	glBitmap(0, 0, 0, 0, 10, 0, NULL);
	glRasterPos3f((float)x, (float)y, SystemFont_posz);
	glColor4ub((color>>24)&0xFF, (color>>16)&0xFF, (color>>8)&0xFF, color&0xFF);

	for(int i=0; i<lstrlenW(ustr); i++){
		if( ustr[i] == '\n' ){
			//둂뛱궥귡
			y += 12;
			glRasterPos3f((float)x, (float)y, SystemFont_posz);
		}
		else{
			//긢귻긚긵깒귽깏긚긣?됪
			glCallList(SystemFontListIdx + i);
		}
	}

	SystemFont_posz -= 0.01f;

	//Unicode빒럻쀱궻봯딙
	delete [] ustr;

	//glDisable(GL_DEPTH_TEST);
	End2DRender();
}

//! @brief 2D?됪뾭먠믦
void D3DGraphics::Start2DRender()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

//! @brief 긡긏긚?긿긲긅깛긣긖귽긛똶럁
//! @param str 빒럻쀱?걁둂뛱긓?긤갌<b>븉됀</b>걂
//! @param base_w 긢긲긅깑긣궻덇빒럻궻븴
//! @param base_h 긢긲긅깑긣궻덇빒럻궻뛼궠
//! @param limitlen 긢긲긅깑긣궻빒럻릶
//! @param w 덇빒럻궻븴귩롦벦궥귡?귽깛?
//! @param h 덇빒럻궻뛼궠귩롦벦궥귡?귽깛?
void D3DGraphics::GetFontTextSize(const char *str, int base_w, int base_h, int limitlen, int *w, int *h)
{
	*w = base_w;
	*h = base_h;
	if( str == NULL ){ return; }
	if( limitlen <= 0 ){ return; }

	if( (int)strlen(str) <= limitlen ){ return; }

	float percent = (float)limitlen / strlen(str);

	*w = (int)((float)base_w * percent);
	*h = (int)((float)base_h * percent);
	return;
}

//! @brief 빒럻귩?됪걁긡긏긚?긿긲긅깛긣럊뾭걂
//! @param x x띆뷭
//! @param y y띆뷭
//! @param str 빒럻쀱?걁둂뛱긓?긤갌<b>븉됀</b>걂
//! @param color 륡
//! @param fontwidth 덇빒럻궻븴
//! @param fontheight 덇빒럻궻뛼궠
//! @attention 빒럻귩볫뢣궸뢣궺궲뿧뫬뒾귩뢯궠궶궋궴뙥궸궘궘궶귟귏궥갃
void D3DGraphics::Draw2DTextureFontText(int x, int y, const char *str, int color, int fontwidth, int fontheight)
{
	//긡긏긚?긿긲긅깛긣궻롦벦궸렪봲궢궲궋귢궽갂룉뿚궢궶궋
	if( TextureFont == -1 ){ return; }
	if( str == NULL ){ return; }

	int strlens = (int)strlen(str);

	float *VertexAry = new float [strlens*6*2];
	unsigned char *ColorAry = new unsigned char [strlens*6*4];
	float *TexCoordAry = new float [strlens*6*2];

	//2D?됪뾭먠믦귩밙뾭
	Start2DRender();

	int w;
	float font_u, font_v;
	float t_u, t_v;

	//1빒럻궻UV띆뷭귩똶럁
	font_u = 1.0f / 16;
	font_v = 1.0f / 16;

	//긡긏긚?긿뾎뚼
	glEnable(GL_TEXTURE_2D);

	//긡긏긚?긿귩긜긞긣
	SetTexture(TextureFont);

	//봹쀱뾎뚼돸
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//륡륃뺪봹쀱귩뾭댰
	Set4ByteBigEndian(&(ColorAry[0]), color);
	for(int i=1; i<strlens*6; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(unsigned char)*4);
	}

	// ?궑귞귢궫빒럻릶빁깑?긵
	for(int i=0; i<strlens; i++){
		//UV띆뷭귩똶럁
		w = str[i];
		if( w < 0 ){ w += 256; }
		t_u = (w % 16) * font_u;
		t_v = (w / 16) * font_v;

		VertexAry[0 + i*12] = (float)x + i*fontwidth;			VertexAry[1 + i*12] = (float)y;
		VertexAry[2 + i*12] = (float)x + i*fontwidth;			VertexAry[3 + i*12] = (float)y;
		VertexAry[4 + i*12] = (float)x + fontwidth + i*fontwidth;	VertexAry[5 + i*12] = (float)y;
		VertexAry[6 + i*12] = (float)x + i*fontwidth;			VertexAry[7 + i*12] = (float)y + fontheight;
		VertexAry[8 + i*12] = (float)x + fontwidth + i*fontwidth;	VertexAry[9 + i*12] = (float)y + fontheight;
		VertexAry[10 + i*12] = (float)x + fontwidth + i*fontwidth;	VertexAry[11 + i*12] = (float)y + fontheight;
		TexCoordAry[0 + i*12] = t_u;		TexCoordAry[1 + i*12] = t_v;
		TexCoordAry[2 + i*12] = t_u;		TexCoordAry[3 + i*12] = t_v;
		TexCoordAry[4 + i*12] = t_u + font_u;	TexCoordAry[5 + i*12] = t_v;
		TexCoordAry[6 + i*12] = t_u;		TexCoordAry[7 + i*12] = t_v + font_v;
		TexCoordAry[8 + i*12] = t_u + font_u;	TexCoordAry[9 + i*12] = t_v + font_v;
		TexCoordAry[10 + i*12] = t_u + font_u;	TexCoordAry[11 + i*12] = t_v + font_v;
	}

	//?됪
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glTexCoordPointer(2, GL_FLOAT, 0, TexCoordAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 1, strlens*6-2);

	//봹쀱뼰뚼돸
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//2D?됪뾭먠믦귩됶룣
	End2DRender();

	delete [] VertexAry;
	delete [] ColorAry;
	delete [] TexCoordAry;
}

//! @brief 빒럻귩?렑걁긡긏긚?긿긲긅깛긣럊뾭걂걓긚긑?깏깛긐??븊궖걕
//! @param x x띆뷭
//! @param y y띆뷭
//! @param str 빒럻쀱?걁둂뛱긓?긤갌<b>븉됀</b>걂
//! @param color 륡
//! @param fontwidth 덇빒럻궻븴
//! @param fontheight 덇빒럻궻뛼궠
//! @attention 640x480몜뱰궻됶몴뱗궔귞뙸됶몴뱗귉긚긑?깏깛긐궢귏궥갃궩귢댥둖궼Draw2DTextureFontText()듫릶궴벏뱳궳궥갃
void D3DGraphics::Draw2DTextureFontTextScaling(int x, int y, const char *str, int color, int fontwidth, int fontheight)
{
	if( str == NULL ){ return; }

	float scaling_x = (float)GameConfig.GetScreenHeight() / 480;//(float)GameConfig.GetScreenWidth() / 640;
	float scaling_y = (float)GameConfig.GetScreenHeight() / 480;
	
	Draw2DTextureFontText((int)(scaling_x * x), (int)(scaling_y * y), str, color, (int)(scaling_x * fontwidth), (int)(scaling_x * fontheight));
}

//! @brief 뭷돍딋궧궸빒럻귩?렑걁긡긏긚?긿긲긅깛긣럊뾭걂
//! @param x x띆뷭궻돿럁빁
//! @param y y띆뷭
//! @param str 빒럻쀱?걁둂뛱긓?긤갌<b>븉됀</b>걂
//! @param color 륡
//! @param fontwidth 덇빒럻궻븴
//! @param fontheight 덇빒럻궻뛼궠
//! @attention 빒럻귩볫뢣궸뢣궺궲뿧뫬뒾귩뢯궠궶궋궴뙥궸궘궘궶귟귏궥갃
void D3DGraphics::Draw2DTextureFontTextCenter(int x, int y, const char *str, int color, int fontwidth, int fontheight)
{
	if( str == NULL ){ return; }

	Draw2DTextureFontText((GameConfig.GetScreenWidth() - strlen(str)*fontwidth)/2 + x, y, str, color, fontwidth, fontheight);
}

//! @brief 뭷돍딋궧궸빒럻귩?렑걁긡긏긚?긿긲긅깛긣럊뾭걂걓긚긑?깏깛긐??븊궖걕
//! @param x x띆뷭궻돿럁빁
//! @param y y띆뷭
//! @param str 빒럻쀱?걁둂뛱긓?긤갌<b>븉됀</b>걂
//! @param color 륡
//! @param fontwidth 덇빒럻궻븴
//! @param fontheight 덇빒럻궻뛼궠
//! @attention 640x480몜뱰궻됶몴뱗궔귞뙸됶몴뱗귉긚긑?깏깛긐궢귏궥갃궩귢댥둖궼Draw2DTextureFontTextCenter()듫릶궴벏뱳궳궥갃
void D3DGraphics::Draw2DTextureFontTextCenterScaling(int x, int y, const char *str, int color, int fontwidth, int fontheight)
{
	if( str == NULL ){ return; }

	float scaling_x = (float)GameConfig.GetScreenHeight() / 480;//(float)GameConfig.GetScreenWidth() / 640;
	float scaling_y = (float)GameConfig.GetScreenHeight() / 480;
	
	Draw2DTextureFontTextCenter((int)(scaling_x * x), (int)(scaling_y * y), str, color, (int)(scaling_x * fontwidth), (int)(scaling_x * fontheight));
}

#ifdef ENABLE_DEBUGCONSOLE
//! @brief 빒럻귩?렑걁긢긫긞긏뾭긲긅깛긣럊뾭걂
//! @param x x띆뷭
//! @param y y띆뷭
//! @param str 빒럻쀱?걁둂뛱긓?긤갌<b>븉됀</b>걂
//! @param color 륡
//! @attention 덇빒럻궻븴궓귝귂뛼궠궼 8x16 뚂믦궳궥갃
//! @attention 빒럻귩볫뢣궸뢣궺궲뿧뫬뒾귩뢯궠궶궋궴뙥궸궘궘궶귟귏궥갃
void D3DGraphics::Draw2DTextureDebugFontText(int x, int y, const char *str, int color)
{
	if( str == NULL ){ return; }

	int fontwidth = 8;
	int fontheight = 16;

	//긡긏긚?긿긲긅깛긣궻롦벦궸렪봲궢궲궋귢궽갂룉뿚궢궶궋
	if( TextureDebugFont == -1 ){ return; }

	int strlens = (int)strlen(str);

	float *VertexAry = new float [strlens*6*2];
	unsigned char *ColorAry = new unsigned char [strlens*6*4];
	float *TexCoordAry = new float [strlens*6*2];

	//2D?됪뾭먠믦귩밙뾭
	Start2DRender();

	int w;
	float font_u, font_v;
	float t_u, t_v;

	//1빒럻궻UV띆뷭귩똶럁
	font_u = 1.0f / 16;
	font_v = 1.0f / 8;

	//긡긏긚?긿뾎뚼
	glEnable(GL_TEXTURE_2D);

	//긡긏긚?긿귩긜긞긣
	SetTexture(TextureDebugFont);

	//봹쀱뾎뚼돸
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//륡륃뺪봹쀱귩뾭댰
	Set4ByteBigEndian(&(ColorAry[0]), color);
	for(int i=1; i<strlens*6; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(unsigned char)*4);
	}

	// ?궑귞귢궫빒럻릶빁깑?긵
	for(int i=0; i<strlens; i++){
		//UV띆뷭귩똶럁
		w = str[i];
		if( w < 0 ){ w = ' '; }
		t_u = (w % 16) * font_u;
		t_v = (w / 16) * font_v;

		VertexAry[0 + i*12] = (float)x + i*fontwidth;			VertexAry[1 + i*12] = (float)y;
		VertexAry[2 + i*12] = (float)x + i*fontwidth;			VertexAry[3 + i*12] = (float)y;
		VertexAry[4 + i*12] = (float)x + fontwidth + i*fontwidth;	VertexAry[5 + i*12] = (float)y;
		VertexAry[6 + i*12] = (float)x + i*fontwidth;			VertexAry[7 + i*12] = (float)y + fontheight;
		VertexAry[8 + i*12] = (float)x + fontwidth + i*fontwidth;	VertexAry[9 + i*12] = (float)y + fontheight;
		VertexAry[10 + i*12] = (float)x + fontwidth + i*fontwidth;	VertexAry[11 + i*12] = (float)y + fontheight;
		TexCoordAry[0 + i*12] = t_u;		TexCoordAry[1 + i*12] = t_v;
		TexCoordAry[2 + i*12] = t_u;		TexCoordAry[3 + i*12] = t_v;
		TexCoordAry[4 + i*12] = t_u + font_u;	TexCoordAry[5 + i*12] = t_v;
		TexCoordAry[6 + i*12] = t_u;		TexCoordAry[7 + i*12] = t_v + font_v;
		TexCoordAry[8 + i*12] = t_u + font_u;	TexCoordAry[9 + i*12] = t_v + font_v;
		TexCoordAry[10 + i*12] = t_u + font_u;	TexCoordAry[11 + i*12] = t_v + font_v;
	}

	//?됪
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glTexCoordPointer(2, GL_FLOAT, 0, TexCoordAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 1, strlens*6-2);

	//봹쀱뼰뚼돸
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//2D?됪뾭먠믦귩됶룣
	End2DRender();

	delete [] VertexAry;
	delete [] ColorAry;
	delete [] TexCoordAry;
}
#endif

//! @brief 멄귩?됪
//! @param x1 럑?궻 x띆뷭
//! @param y1 럑?궻 y띆뷭
//! @param x2 뢎?궻 x띆뷭
//! @param y2 뢎?궻 y띆뷭
//! @param color 륡
void D3DGraphics::Draw2DLine(int x1, int y1, int x2, int y2, int color)
{
	float VertexAry[2*2];
	unsigned char ColorAry[2*4];

	//2D?됪뾭먠믦귩밙뾭
	Start2DRender();

	//긡긏긚?긿뼰뚼
	glDisable(GL_TEXTURE_2D);

	//뮯?봹쀱귩뾭댰
	VertexAry[0] = (float)x1;	VertexAry[1] = (float)y1;
	VertexAry[2] = (float)x2;	VertexAry[3] = (float)y2;

	//륡륃뺪봹쀱귩뾭댰
	Set4ByteBigEndian(&(ColorAry[0]), color);
	memcpy(&(ColorAry[4]), ColorAry, sizeof(unsigned char)*4);

	//봹쀱뾎뚼돸
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//?됪
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glDrawArrays(GL_LINE_STRIP, 0, 2);

	//봹쀱뼰뚼돸
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	//2D?됪뾭먠믦귩됶룣
	End2DRender();
}

//! @brief ?걁16둷?걂귩?됪
//! @param x 뭷륲궻 x띆뷭
//! @param y 뭷륲궻 y띆뷭
//! @param r 뵾똞
//! @param color 륡
void D3DGraphics::Draw2DCycle(int x, int y, int r, int color)
{
	float VertexAry[(16+1)*2];
	unsigned char ColorAry[(16+1)*4];

	//2D?됪뾭먠믦귩밙뾭
	Start2DRender();

	//긡긏긚?긿뼰뚼
	glDisable(GL_TEXTURE_2D);

	//뮯?띆뷭귩먠믦
	for(int i=0; i<16+1; i++){
		float x2, y2;
		x2 = (float)x + cosf((float)M_PI*2/16 * i) * r;
		y2 = (float)y + sinf((float)M_PI*2/16 * i) * r;
		VertexAry[i*2] = x2;	VertexAry[i*2+1] = y2;
	}

	//륡륃뺪봹쀱귩뾭댰
	Set4ByteBigEndian(&(ColorAry[0]), color);
	for(int i=1; i<16+1; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(unsigned char)*4);
	}

	//봹쀱뾎뚼돸
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//?됪
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glDrawArrays(GL_LINE_STRIP, 0, 16+1);

	//봹쀱뼰뚼돸
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	//2D?됪뾭먠믦귩됶룣
	End2DRender();
}

//! @brief 럏둷?귩?됪
//! @param x1 뜺뤵궻 x띆뷭
//! @param y1 뜺뤵궻 y띆뷭
//! @param x2 덭돷궻 x띆뷭
//! @param y2 덭돷궻 y띆뷭
//! @param color 륡
void D3DGraphics::Draw2DBox(int x1, int y1, int x2, int y2, int color)
{
	float VertexAry[4*2];
	unsigned char ColorAry[4*4];

	//2D?됪뾭먠믦귩밙뾭
	Start2DRender();

	//긡긏긚?긿뼰뚼
	glDisable(GL_TEXTURE_2D);

	//뮯?봹쀱귩뾭댰
	VertexAry[0] = (float)x1;	VertexAry[1] = (float)y1;
	VertexAry[2] = (float)x2;	VertexAry[3] = (float)y1;
	VertexAry[4] = (float)x1;	VertexAry[5] = (float)y2;
	VertexAry[6] = (float)x2;	VertexAry[7] = (float)y2;

	//륡륃뺪봹쀱귩뾭댰
	Set4ByteBigEndian(&(ColorAry[0]), color);
	for(int i=1; i<4; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(unsigned char)*4);
	}

	//봹쀱뾎뚼돸
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//?됪
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, ColorAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//봹쀱뼰뚼돸
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	//2D?됪뾭먠믦귩됶룣
	End2DRender();
}

//! @brief 럏둷?귩?됪걓긚긑?깏깛긐??븊궖걕
//! @param x1 뜺뤵궻 x띆뷭
//! @param y1 뜺뤵궻 y띆뷭
//! @param x2 덭돷궻 x띆뷭
//! @param y2 덭돷궻 y띆뷭
//! @param color 륡
//! @attention 640x480몜뱰궻됶몴뱗궔귞뙸됶몴뱗귉긚긑?깏깛긐궢귏궥갃궩귢댥둖궼Draw2DBox()듫릶궴벏뱳궳궥갃
void D3DGraphics::Draw2DBoxScaling(int x1, int y1, int x2, int y2, int color)
{
	float scaling_x = (float)GameConfig.GetScreenHeight() / 480;//(float)GameConfig.GetScreenWidth() / 640;
	float scaling_y = (float)GameConfig.GetScreenHeight() / 480;

	Draw2DBox((int)(scaling_x * x1), (int)(scaling_y * y1), (int)(scaling_x * x2), (int)(scaling_y * y2), color);
}

// [✨ 이 함수가 없으므로, 여기에 전체를 추가하세요 ✨]
void D3DGraphics::Draw2DDonutArc(float cx, float cy, float radius, float start_angle, float end_angle, int thickness, int segments, int color)
{
	Start2DRender();
	glDisable(GL_TEXTURE_2D);

	int num_vertices = (segments + 1) * 2;
	float* vertex_array = new float[num_vertices * 2];
	unsigned char* color_array = new unsigned char[num_vertices * 4];
	if (!vertex_array || !color_array) {
		if (vertex_array) delete[] vertex_array;
		if (color_array) delete[] color_array;
		End2DRender();
		return;
	}

	float theta_step = (end_angle - start_angle) / segments;
	unsigned char r = (unsigned char)((color >> 24) & 0xFF);
	unsigned char g = (unsigned char)((color >> 16) & 0xFF);
	unsigned char b = (unsigned char)((color >> 8) & 0xFF);
	unsigned char a = (unsigned char)(color & 0xFF);

	for (int i = 0; i <= segments; ++i)
	{
		float angle = start_angle + i * theta_step;
		float cos_a = cosf(angle - (float)M_PI / 2);
		float sin_a = sinf(angle - (float)M_PI / 2);

		// 안쪽 정점
		vertex_array[i * 4 + 0] = cx + (radius - thickness / 2) * cos_a;
		vertex_array[i * 4 + 1] = cy + (radius - thickness / 2) * sin_a;

		// 바깥쪽 정점
		vertex_array[i * 4 + 2] = cx + (radius + thickness / 2) * cos_a;
		vertex_array[i * 4 + 3] = cy + (radius + thickness / 2) * sin_a;

		// 색상 설정
		for (int j = 0; j < 2; ++j) {
			color_array[(i * 2 + j) * 4 + 0] = r;
			color_array[(i * 2 + j) * 4 + 1] = g;
			color_array[(i * 2 + j) * 4 + 2] = b;
			color_array[(i * 2 + j) * 4 + 3] = a;
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, vertex_array);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, color_array);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, num_vertices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	delete[] vertex_array;
	delete[] color_array;

	End2DRender();
}

//! @brief 됪몴귩?됪
//! @param x x띆뷭
//! @param y y띆뷭
//! @param id 긡긏긚?긿궻긢??붥뜂
//! @param width 븴
//! @param height 뛼궠
//! @param alpha 벁뼻뱗걁0.0?1.0걂
void D3DGraphics::Draw2DTexture(int x, int y, int id, int width, int height, float alpha)
{
	//뼰뚼궶긡긏긚?긿붥뜂귩럚믦궠귢궲궋귢궽룉뿚궢궶궋
	if( id == -1 ){ return; }

	float VertexAry[4*2];
	float ColorAry[4*4];
	float TexCoordAry[4*2];

	//2D?됪뾭먠믦귩밙뾭
	Start2DRender();

	//긡긏긚?긿뾎뚼
	glEnable(GL_TEXTURE_2D);

	//긡긏긚?긿귩긜긞긣
	SetTexture(id);

	//뮯?봹쀱귩뾭댰
	VertexAry[0] = (float)x;		VertexAry[1] = (float)y;
	VertexAry[2] = (float)x+width;	VertexAry[3] = (float)y;
	VertexAry[4] = (float)x;		VertexAry[5] = (float)y+height;
	VertexAry[6] = (float)x+width;	VertexAry[7] = (float)y+height;

	//륡륃뺪봹쀱귩뾭댰
	ColorAry[0] = 1.0f;
	ColorAry[1] = 1.0f;
	ColorAry[2] = 1.0f;
	ColorAry[3] = alpha;
	for(int i=1; i<4; i++){
		memcpy(&(ColorAry[i*4]), ColorAry, sizeof(float)*4);
	}

	//UV띆뷭봹쀱귩뾭댰
	TexCoordAry[0] = 0.0f;	TexCoordAry[1] = 0.0f;
	TexCoordAry[2] = 1.0f;	TexCoordAry[3] = 0.0f;
	TexCoordAry[4] = 0.0f;	TexCoordAry[5] = 1.0f;
	TexCoordAry[6] = 1.0f;	TexCoordAry[7] = 1.0f;

	//봹쀱뾎뚼돸
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//?됪
	glVertexPointer(2, GL_FLOAT, 0, VertexAry);
	glColorPointer(4, GL_FLOAT, 0, ColorAry);
	glTexCoordPointer(2, GL_FLOAT, 0, TexCoordAry);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//봹쀱뼰뚼돸
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//2D?됪뾭먠믦귩됶룣
	End2DRender();
}

//! @brief 됪몴귩?됪걓긚긑?깏깛긐??븊궖걕
//! @param x x띆뷭
//! @param y y띆뷭
//! @param id 긡긏긚?긿궻긢??붥뜂
//! @param width 븴
//! @param height 뛼궠
//! @param alpha 벁뼻뱗걁0.0?1.0걂
//! @attention 640x480몜뱰궻됶몴뱗궔귞뙸됶몴뱗귉긚긑?깏깛긐궢귏궥갃궩귢댥둖궼Draw2DTexture()듫릶궴벏뱳궳궥갃
void D3DGraphics::Draw2DTextureScaling(int x, int y, int id, int width, int height, float alpha)
{
	float scaling_x = (float)GameConfig.GetScreenHeight() / 480;//(float)GameConfig.GetScreenWidth() / 640;
	float scaling_y = (float)GameConfig.GetScreenHeight() / 480;

	Draw2DTexture((int)(scaling_x * x), (int)(scaling_y * y), id, (int)(scaling_x * width), (int)(scaling_y * height), alpha);
}

//! @brief 2D?됪뾭먠믦귩됶룣
void D3DGraphics::End2DRender()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

//! @brief 됪뽋궻긚긏깏?깛긘깈긞긣귩뺎뫔
//! @param filename 긲?귽깑뼹
//! @return 맟뚻갌true?렪봲갌false
bool D3DGraphics::SaveScreenShot(const char* filename)
{
	if( filename == NULL ){ return false; }

	HDC hDC;
	FILE *fp;
	unsigned char header[54];
	unsigned char pixel[3];

	unsigned char *dataBuffer = new unsigned char [GameConfig.GetScreenWidth() * GameConfig.GetScreenHeight() * 3];

	//긢긫귽긚긓깛긡긌긚긣먠믦
	hDC = GetDC(hWnd);
	wglMakeCurrent(hDC, hGLRC);

	//긫긞긲?궸둰?
	glReadPixels(0, 0, GameConfig.GetScreenWidth(), GameConfig.GetScreenHeight(), GL_RGB, GL_UNSIGNED_BYTE, dataBuffer);

	//긲?귽깑귩뺎뫔궥귡
	fp = fopen(filename, "wb");
	if( fp == NULL ){
		delete [] dataBuffer;
		return false;		//긲?귽깑궕뺎뫔궳궖궶궋
	}

	//긶긞??귩맯맟
	for(int i=0; i<54; i++){ header[i] = 0x00; }
	header[0x00] = 'B';
	header[0x01] = 'M';
	Set4ByteLittleEndian(&(header[0x02]), 54 + GameConfig.GetScreenWidth()*GameConfig.GetScreenHeight()*3);
	header[0x0A] = 54;
	header[0x0E] = 54 - 14;
	Set4ByteLittleEndian(&(header[0x12]), GameConfig.GetScreenWidth());
	Set4ByteLittleEndian(&(header[0x16]), GameConfig.GetScreenHeight());
	header[0x1A] = 1;
	header[0x1C] = 24;
	header[0x1E] = 0;
	Set4ByteLittleEndian(&(header[0x22]), GameConfig.GetScreenWidth()*GameConfig.GetScreenHeight()*3);

	//긶긞??귩룕궖뜛귔
	fwrite(header, 1, 54, fp);

	for(int h=0; h<GameConfig.GetScreenHeight(); h++){
		for(int w=0; w<GameConfig.GetScreenWidth(); w++){
			pixel[2] = dataBuffer[(h*GameConfig.GetScreenWidth()+w)*3 + 0];
			pixel[1] = dataBuffer[(h*GameConfig.GetScreenWidth()+w)*3 + 1];
			pixel[0] = dataBuffer[(h*GameConfig.GetScreenWidth()+w)*3 + 2];

			fwrite(&pixel, 1, 3, fp);
		}

		if( (GameConfig.GetScreenWidth()%4) != 0 ){
			//4궻?릶궸궶귡귝궎궸긬긢귻깛긐
			unsigned char dummy[3];
			dummy[0] = 0x00;
			dummy[1] = 0x00;
			dummy[2] = 0x00;
			fwrite(&dummy, 1, 4 - (GameConfig.GetScreenWidth()%4), fp);
		}
	}

	//긲?귽깑긪깛긤깑귩됶뺳
	fclose(fp);

	delete [] dataBuffer;

	ReleaseDC(hWnd, hDC);

	return true;
}

//! @brief 긇깋?긓?긤귩롦벦
//! @param red 먗걁0.0f?1.0f걂
//! @param green 쀎걁0.0f?1.0f걂
//! @param blue 먃걁0.0f?1.0f걂
//! @param alpha 벁뼻뱗걁0.0f?1.0f걂
//! @return 긇깋?긓?긤
int D3DGraphics::GetColorCode(float red, float green, float blue, float alpha)
{
	unsigned char red2, green2, blue2, alpha2;
	red2 = (unsigned char)(red*255);
	green2 = (unsigned char)(green*255);
	blue2 = (unsigned char)(blue*255);
	alpha2 = (unsigned char)(alpha*255);

	return (red2 << 24) | (green2 << 16) | (blue2 << 8) | alpha2;
}

#endif	//GRAPHIC_ENGINE