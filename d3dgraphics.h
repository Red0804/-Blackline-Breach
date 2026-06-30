//! @file d3dgraphics.h
//! @brief D3DGraphics긏깋긚궻먬뙻

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

#ifndef D3DGRAPHICS_H
#define D3DGRAPHICS_H

#define MAX_MODEL 200		//!< 띍묈긾긢깑릶
#define MAX_TEXTURE 200		//!< 띍묈긡긏긚?긿릶

#define CLIPPINGPLANE_NEAR 0.3f		//!< 뗟긏깏긞긵뽋걁곟?됪궥귡띍룷떁뿣걂
#define CLIPPINGPLANE_FAR 800.0f	//!< 돀긏깏긞긵뽋걁곟?됪궥귡띍묈떁뿣걂

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 1		//!< Select include file.
#endif
#include "main.h"

//! @brief 럊뾭궥귡긐깋긲귻긞긏긚긓귺궻멗묖
//! @details DirectX 9.0c걁뚂믦긬귽긵깋귽깛걂갌0?DirectX 9.0c걁긘긃???걂갌1?OpenGL 1.1갌8
//! @attention DirectX 9.0c궻긘긃???귩럊궎뤾뜃갂"resource.rc"귖뺂뢜궢궲궘궬궠궋갃
#define GRAPHIC_ENGINE 0

#if (GRAPHIC_ENGINE == 0)||(GRAPHIC_ENGINE == 1)

#define BLOCKDATA_GPUMEMORY	//!< @brief 긳깓긞긏긢??귩둰?궥귡긽긾깏?귩멗묖 @details 믦릶먬뙻뾎뚼갌GPU긽긾깏??믦릶먬뙻뼰뚼걁긓긽깛긣돸걂갌긽귽깛긽긾깏?

#pragma warning(disable:4819)		//VC++똸뜍뻞?
#include <d3dx9.h>
#pragma warning(default:4819)

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#if GRAPHIC_ENGINE == 0
 #define GRAPHICS_CORE "DirectX 9.0"				//!< 긫?긙깈깛?렑뾭륃뺪
#else
 #define GRAPHICS_CORE "DirectX 9.0 (Shader)"		//!< 긫?긙깈깛?렑뾭륃뺪
 #define HLSLFILE 201								//!< 깏??긚볙궻HLSL긲?귽깑
#endif

#elif GRAPHIC_ENGINE == 8

//#define ENABLE_OPENGL_LIBJPEGPNG		//!< OpenGL궸궓궋궲libjpeg갋libpng귩럊뾭궥귡

#include <ctype.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#ifdef ENABLE_OPENGL_LIBJPEGPNG
 #include "lib/libjpeg/jpeglib.h"
 #include "lib/zlib/zlib.h"
 #include "lib/libpng/png.h"

 //겍libjpeg.lib, zlib.lib, libpng.lib 궼렔멟궳뾭댰궥귡뷠뾴궕궇귟귏궥갃
 #pragma comment(lib, "lib/libjpeg/libjpeg.lib")
 #pragma comment(lib, "lib/zlib/zlib.lib")
 #pragma comment(lib, "lib/libpng/libpng.lib")

 //#pragma comment(lib, "legacy_stdio_definitions.lib")

 #pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")
 #pragma comment(linker, "/NODEFAULTLIB:libcpmt.lib")
#endif

#define GRAPHICS_CORE "OpenGL 1.1"		//!< 긫?긙깈깛?렑뾭륃뺪

#endif	//GRAPHIC_ENGINE

#if (GRAPHIC_ENGINE == 0)||(GRAPHIC_ENGINE == 1)

//! 3D?깏긕깛?됪뾭?몾뫬
struct VERTEXTXTA
{
	D3DXVECTOR3 position;
	DWORD       color;
	FLOAT       tu;
	FLOAT       tv;

	VERTEXTXTA()
		: position(0.0f, 0.0f, 0.0f),
		color(0xFFFFFFFF),
		tu(0.0f),
		tv(0.0f)
	{
	}
};

//! 2D?깏긕깛?됪뾭?몾뫬
typedef struct
{
	FLOAT    x;		//!< position
	FLOAT    y;		//!< position
	FLOAT    z;		//!< position
	FLOAT    rhw;	//!< vector
	D3DCOLOR color;	//!< color
	FLOAT    tu;	//!< texture coordinates
	FLOAT    tv;	//!< texture coordinates
} TLVERTX;

#elif GRAPHIC_ENGINE == 8

//! 긾긢깑긢??궻뮯?둰??몾뫬
typedef struct
{
	float x;	//!< position
	float y;	//!< position
	float z;	//!< position
	float u;	//!< texture coordinates
	float v;	//!< texture coordinates
} MODELVDATA;

//! 긾긢깑긢???몾뫬
typedef struct
{
	bool useflag;			//!< 뾎뚼돸긲깋긐
	int polygons;			//!< ?깏긕깛릶
	float *VertexAry;		//!< 뮯?둰?봹쀱
	float *ColorAry;		//!< 륡둰?봹쀱
	float *ColorGrayAry;	//!< 륡둰?봹쀱걁댠궋걂
	float *TexCoordAry;		//!< 긡긏긚?긿띆뷭둰?봹쀱
} MODELDATA;

//! 긡긏긚?긿?몾뫬
typedef struct
{
	bool useflag;			//!< 뾎뚼돸긲깋긐
	int width;				//!< 븴
	int height;				//!< 뛼궠
	unsigned char *data;	//!< 렳긢??
} TEXTUREDATA;

#endif	//GRAPHIC_ENGINE


//! @brief 됪뽋?됪귩뛱궎긏깋긚
//! @details 됪뽋궻?됪??귘궩귢궸뮳먝듫쁀궥귡룉뿚귩뛱궋귏궥갃
//! @details 뗰뫬밒궸갂3D?됪갋2D?됪갋긾긢깑긲?귽깑귘긡긏긚?긿듖뿚?귩뛱궎??궕궇귟귏궥갃
class D3DGraphics
{

#if (GRAPHIC_ENGINE == 0)||(GRAPHIC_ENGINE == 1)

	LPDIRECT3D9 pD3D;					//!< DIRECT3D9궻?귽깛?
	LPDIRECT3DDEVICE9 pd3dDevice;		//!< DIRECT3DDEVICE9궻?귽깛?
	float aspect;						//!< 됪뽋궻귺긚긻긏긣붶
	bool fullscreenflag;				//!< 긲깑긚긏깏?깛?렑
	int LanguageMode;					//!< 뙻뚭멗묖
	LPD3DXMESH pmesh[MAX_MODEL];				//!< 걁X긲?귽깑뾭걂D3DXMESH궻?귽깛?
	DWORD nummaterials[MAX_MODEL];				//!< 걁X긲?귽깑뾭걂?긡깏귺깑릶
	LPDIRECT3DTEXTURE9 ptextures[MAX_TEXTURE];	//!< 긡긏긚?긿귩둰?

#if GRAPHIC_ENGINE == 1
	LPDIRECT3DVERTEXSHADER9	hVertexShader;			//!< 뮯?긘긃???
	LPD3DXCONSTANTTABLE hVSConstantTable;			//!< 뮯?긘긃???궻믦릶긡?긳깑
	D3DXHANDLE hVSWorldVertexPos;					//!< 뮯?긘긃???궻빾듂뛱쀱
	D3DXHANDLE hVSWorldViewPos;						//!< 뮯?긘긃???궻빾듂뛱쀱
	D3DXHANDLE hVSWorldViewProj;					//!< 뮯?긘긃???궻빾듂뛱쀱
	D3DXHANDLE hVSDiffuse;							//!< 뮯?긘긃???궻긢귻긲깄?긛륡
	LPDIRECT3DPIXELSHADER9 pPixelShader;			//!< 긯긏긜깑긘긃???
	LPD3DXCONSTANTTABLE pPSConstantTable;			//!< 긯긏긜깑긘긃???궻믦릶긡?긳깑
	D3DXHANDLE hPSViewPos;							//!< 긯긏긜깑긘긃???궻긇긽깋띆뷭
	D3DXHANDLE hPSEnable2DRendering;				//!< 긯긏긜깑긘긃???궻2D?됪긲깋긐
	D3DXHANDLE hPSEnableTexture;					//!< 긯긏긜깑긘긃???궻긡긏긚?긿긲깋긐
	D3DXHANDLE hPSEnableFog;						//!< 긯긏긜깑긘긃???궻긲긅긐뾎뚼긲깋긐
	D3DXHANDLE hPSFogColor;							//!< 긯긏긜깑긘긃???궻긲긅긐륡
	D3DXHANDLE hPSFogStart;							//!< 긯긏긜깑긘긃???궻긲긅긐궻둎럑?뱗
	D3DXHANDLE hPSFogEnd;							//!< 긯긏긜깑긘긃???궻긲긅긐궻뢎뿹?뱗
	D3DXMATRIX VSWorldPosMatrix;					//!< 뮯?긘긃???궸?궑궫빾듂뛱쀱
	D3DXMATRIX VSViewPosMatrix;						//!< 뮯?긘긃???궸?궑궫빾듂뛱쀱
	D3DXMATRIX VSViewProjMatrix;					//!< 뮯?긘긃???궸?궑궫빾듂뛱쀱
#endif

	class BlockDataInterface* BlockDataIF;		//!< 벶귒뜛귪궬긳깓긞긏긢??귩둰?궥귡긏깋긚귉궻?귽깛?
	int bs;										//!< 긳깓긞긏릶
	int mapTextureID[TOTAL_BLOCKTEXTURE];		//!< 긡긏긚?긿붥뜂
#ifdef BLOCKDATA_GPUMEMORY
	LPDIRECT3DVERTEXBUFFER9 g_pVB;				//!< 뮯?륃뺪걁GPU듖뿚걂
#else
	VERTEXTXTA g_pVertices[MAX_BLOCKS][6][4];	//!< 뮯?륃뺪걁CPU듖뿚걂
#endif

	float HUD_myweapon_x[TOTAL_HAVEWEAPON];		//!< HUD궻븧딇?됪 X띆뷭
	float HUD_myweapon_y[TOTAL_HAVEWEAPON];		//!< HUD궻븧딇?됪 Y띆뷭
	float HUD_myweapon_z[TOTAL_HAVEWEAPON];		//!< HUD궻븧딇?됪 Z띆뷭

	bool StartRenderFlag;			//!< StartRender()듫릶 렳뛱뭷귩?궥긲깋긐
	bool EnableFogFlag;				//!< 긲긅긐럊뾭뭷귩?궥긲깋긐

	LPD3DXSPRITE ptextsprite;			//!< 2D긡긏긚?긿긚긵깋귽긣
	LPD3DXFONT pxmsfont;
	LPD3DXFONT pxmssmallfont;

	// 브리핑 본문 / 인게임 대사 자막 전용.
	// 기존 시스템 폰트와 같은 계열이지만 크기만 줄인 폰트.
	LPD3DXFONT pxmsfont_briefing;

	// SKILLS 화면 CJK 표시용 시스템 폰트.
	// 기존 pxmsfont와 분리하여 한국어/일본어 폰트를 따로 사용한다.
	LPD3DXFONT pxmsfont_ko;
	LPD3DXFONT pxmssmallfont_ko;
	LPD3DXFONT pxmsfont_jp;
	LPD3DXFONT pxmssmallfont_jp;
	char TextureFontFname[_MAX_PATH];	//!< 긡긏긚?긿긲긅깛긣궻긲?귽깑뼹
	int TextureFont;					//!< 긡긏긚?긿긲긅깛긣궻긡긏긚?긿ID

	int InitSubset();
	void CleanupD3Dresource();
	void Start2DMSFontTextRender();
	void End2DMSFontTextRender();
	void Start2DRender();
	void End2DRender();

#elif GRAPHIC_ENGINE == 8

	HWND hWnd;				//!< 긂귻깛긤긂긪깛긤깑
	HGLRC hGLRC;			//!< OpenGL궻긓깛긡긌긚긣
	int width;				//!< 븴
	int height;				//!< 뛼궠
	bool fullscreenflag;	//!< 긲깑긚긏깏?깛?렑
	int LanguageMode;		//!< 뙻뚭멗묖
	PAINTSTRUCT Paint_ps;					//!< BeginPaint()듫릶궴EndPaint()듫릶뾭
	MODELDATA pmodel[MAX_MODEL];			//!< 긾긢깑긢??귩둰?
	TEXTUREDATA ptextures[MAX_TEXTURE];		//!< 긡긏긚?긿귩둰?
	GLuint textureobjname[MAX_TEXTURE];		//!< 긡긏긚?긿긆긳긙긃긏긣
	HFONT SystemFont;				//!< 긘긚긡?긲긅깛긣뾭궻?뿚긲긅깛긣
	HFONT SystemSmallFont;			//!< 긘긚긡?긲긅깛긣뾭궻?뿚긲긅깛긣걁룷궠궋걂
	WCHAR *now_SystemFontUStr;		//!< 뙸띪?렑뭷궻긘긚긡?긲긅깛긣궸귝귡빒럻쀱걁Unicode걂
	GLuint SystemFontListIdx;		//!< 긘긚긡?긲긅깛긣궻긢귻긚긵깒귽깏긚긣
	int SystemFontListIdxSize;		//!< 긘긚긡?긲긅깛긣궻긢귻긚긵깒귽깏긚긣궻긖귽긛
	float SystemFont_posz;			//!< 긘긚긡?긲긅깛긣궻Z띆뷭
	int now_textureid;		//!< 뙸띪먠믦뭷궻긡긏긚?긿붥뜂

	float camera_x;			//!< 긇긽깋띆뷭
	float camera_y;			//!< 긇긽깋띆뷭
	float camera_z;			//!< 긇긽깋띆뷭
	float camera_rx;		//!< 긇긽깋됷?둷뱗
	float camera_ry;		//!< 긇긽깋됷?둷뱗
	float viewangle;		//!< 긇긽깋궻럨뽰둷

	class BlockDataInterface* BlockDataIF;		//!< 벶귒뜛귪궬긳깓긞긏긢??귩둰?궥귡긏깋긚귉궻?귽깛?
	int bs;										//!< 긳깓긞긏릶
	int mapTextureID[TOTAL_BLOCKTEXTURE];		//!< 긡긏긚?긿붥뜂

	char TextureFontFname[_MAX_PATH];	//!< 긡긏긚?긿긲긅깛긣궻긲?귽깑뼹
	int TextureFont;				//!< 긡긏긚?긿긲긅깛긣궻긡긏긚?긿ID

	float HUD_myweapon_x[TOTAL_HAVEWEAPON];		//!< HUD_A궻븧딇?됪 X띆뷭
	float HUD_myweapon_y[TOTAL_HAVEWEAPON];		//!< HUD_A궻븧딇?됪 Y띆뷭
	float HUD_myweapon_z[TOTAL_HAVEWEAPON];		//!< HUD_A궻븧딇?됪 Z띆뷭

#ifdef ENABLE_OPENGL_LIBJPEGPNG
	jpeg_decompress_struct cinfo;	//!< libjpeg
	jpeg_error_mgr jerr;			//!< libjpeg
#endif

	void CleanupD3Dresource();
	int CheckTextureFileExtension(const char* filename, int nowformat);
	int CheckTextureFileTypeFlag(const char* filename, int nowformat);
	bool LoadBMPTexture(const char* filename, bool BlackTransparent, TEXTUREDATA *ptexture);
	bool LoadDDSTexture(const char* filename, bool BlackTransparent, TEXTUREDATA *ptexture);
	bool LoadJPEGTexture(const char* filename, bool BlackTransparent, TEXTUREDATA *ptexture);
	bool LoadPNGTexture(const char* filename, bool BlackTransparent, TEXTUREDATA *ptexture);
	void SetTexture(int TextureID);
	int StrMaxLineLen(const char *str);
	void Start2DRender();
	void End2DRender();

#endif	//GRAPHIC_ENGINE

#ifdef ENABLE_DEBUGCONSOLE
	unsigned char DebugFontData[96][16];	//!< 긢긫긞긏뾭긲긅깛긣긢??둰?봹쀱
	int TextureDebugFont;					//!< 긢긫긞긏뾭긲긅깛긣궻긡긏긚?긿ID
	void InitDebugFontData();
	bool LoadDebugFontTexture();
#endif

public:
	D3DGraphics();
	~D3DGraphics();
	void SetFullScreenFlag(bool fullscreen);
	bool GetFullScreenFlag();
	void SetlanguageMode(int mode);
	int GetlanguageMode();
	int InitD3D(WindowControl *WindowCtrl, const char *TextureFontFilename, bool fullscreen, int language);
	int ResetD3D(WindowControl *WindowCtrl);
	void DestroyD3D();
	int LoadModel(const char* filename);
	int MorphingModel(int idA, int idB);
	int GetTotalModels();
	void CleanupModel(int id);
	int LoadTexture(const char* filename, bool texturefont, bool BlackTransparent);
	int GetTextureSize(int id, int *width, int *height);
	int GetTotalTextures();
	void CleanupTexture(int id);
	int StartRender();
	bool EndRender();
	void ResetZbuffer();
	void ResetWorldTransform();
	void SetWorldTransform(float x, float y, float z, float rx, float ry, float size);
	void SetWorldTransform(float x, float y, float z, float rx, float ry1, float ry2, float size);
	void SetWorldTransformEffect(float x, float y, float z, float rx, float ry, float rt, float size);
	void SetWorldTransformHumanWeapon(float x, float y, float z, float mx, float my, float mz, float rx, float ry, float size);
	void SetWorldTransformPlayerWeapon(bool rotation, float camera_rx, float camera_ry, float rx, float size, float viewangle);
	void GetWorldTransformPos(float *x, float *y, float *z);
	void SetFog(bool enablefog, int skynumber);
	void SetCamera(float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, float viewangle);
	void LoadMapdata(class BlockDataInterface* in_BlockDataIF, const char *directory);
	void RenderMapdata(bool wireframe);
	int GetMapTextureID(int id);
	void CleanupMapdata();
	void RenderModel(int id_model, int id_texture, bool darkflag, bool HUDmode, bool nomodel, bool disable_culling = false, float alpha = 1.0f, float model_brightness = -1.0f, bool alpha_zwrite = false);
	void RenderBoard(int id_texture, float alpha, bool nomodel);
	void StartEffectRender();
	void EndEffectRender();
	void ScreenBrightness(int Width, int Height, int Brightness);
	void RenderCenterline();
	void Renderline(float x1, float y1, float z1, float x2, float y2, float z2, int color);
	void Draw2DMSFontText(int x, int y, const char* str, int color);
	void Draw2DMSFontTextScaling(int x, int y, const char* str, int color);
	void Draw2DMSFontTextCenter(int x, int y, int w, int h, const char* str, int color);
	void Draw2DMSFontTextCenterScaling(int x, int y, int w, int h, const char* str, int color);
	void Draw2DMSSmallFontText(int x, int y, const char* str, int color);
	// 기존 시스템 폰트와 같은 계열이지만 크기만 줄인 브리핑/자막 전용 출력
	void Draw2DMSFontTextBriefingScaling(int x, int y, const char* str, int color);
	void Draw2DMSFontTextCenterBriefingScaling(int x, int y, int w, int h, const char* str, int color);

	// 작은 시스템 폰트를 640x480 기준 좌표로 출력
	void Draw2DMSSmallFontTextScaling(int x, int y, const char* str, int color);

	// 작은 시스템 폰트를 가운데 정렬로 출력
	void Draw2DMSSmallFontTextCenter(int x, int y, int w, int h, const char* str, int color);

	// 작은 시스템 폰트를 640x480 기준 좌표 + 가운데 정렬로 출력
	void Draw2DMSSmallFontTextCenterScaling(int x, int y, int w, int h, const char* str, int color);

	// UTF-8 문자열을 UTF-16으로 변환한 뒤 DrawTextW로 출력한다.
	// SKILLS 화면의 한국어/일본어 표시용.
	// language: 0 = Korean, 1 = English/Default, 2 = Japanese
	void Draw2DMSFontTextUTF8(int x, int y, const char* str, int color, bool smallfont, int language);
	void GetFontTextSize(const char *str, int base_w, int base_h, int limitlen, int *w, int *h);
	void Draw2DTextureFontText(int x, int y, const char *str, int color, int fontwidth, int fontheight);
	void Draw2DTextureFontTextScaling(int x, int y, const char *str, int color, int fontwidth, int fontheight);
	void Draw2DTextureFontTextCenter(int x, int y, const char *str, int color, int fontwidth, int fontheight);
	void Draw2DTextureFontTextCenterScaling(int x, int y, const char *str, int color, int fontwidth, int fontheight);

	void SetAmbientLight(int color);	//!< 주변광 설정
#ifdef ENABLE_DEBUGCONSOLE
	void Draw2DTextureDebugFontText(int x, int y, const char *str, int color);
#endif
	void Draw2DLine(int x1, int y1, int x2, int y2, int color);
	void Draw2DCycle(int x, int y, int r, int color);
	void Draw2DBox(int x1, int y1, int x2, int y2, int color);
	void Draw2DDonutArc(float cx, float cy, float radius, float start_angle, float end_angle, int thickness, int segments, int color); // [✨ 이 줄을 추가하세요 ✨]
	void Draw2DBoxScaling(int x1, int y1, int x2, int y2, int color);
	void Draw2DTexture(int x, int y, int id, int width, int height, float alpha);
	void Draw2DTextureScaling(int x, int y, int id, int width, int height, float alpha);
	bool SaveScreenShot(const char *fname);
	int GetColorCode(float red, float green, float blue, float alpha);
};

#endif