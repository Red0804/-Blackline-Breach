//! @file d3dgraphics-directx.cpp
//! @brief D3DGraphics긏깋긚궻믦?걁DirectX붎걂

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

#include "d3dgraphics.h"
#define ENABLE_MESH_OPTIMIZE

#if (GRAPHIC_ENGINE == 0)||(GRAPHIC_ENGINE == 1)

//! @brief 긓깛긚긣깋긏?
D3DGraphics::D3DGraphics()
{
	pD3D = NULL;
	pd3dDevice = NULL;
	aspect = 1.0f;
	fullscreenflag = false;
	LanguageMode = 0;
	for(int i=0; i<MAX_MODEL; i++){
		pmesh[i] = NULL;
		nummaterials[i] = 0;
	}
	for(int i=0; i<MAX_TEXTURE; i++){
		ptextures[i] = NULL;
	}

#if GRAPHIC_ENGINE == 1
	hVertexShader = NULL;
	hVSConstantTable = NULL;
	hVSWorldVertexPos = NULL;
	hVSWorldViewPos = NULL;
	hVSWorldViewProj = NULL;
	hVSDiffuse = NULL;
	pPixelShader = NULL;
	pPSConstantTable = NULL;
	hPSViewPos = NULL;
	hPSEnable2DRendering = NULL;
	hPSEnableTexture = NULL;
	hPSEnableFog = NULL;
	hPSFogColor = NULL;
	hPSFogStart = NULL;
	hPSFogEnd = NULL;
#endif

	BlockDataIF = NULL;
	bs = 0;
	for(int i=0; i<TOTAL_BLOCKTEXTURE; i++){
		mapTextureID[i] = -1;
	}
#ifdef BLOCKDATA_GPUMEMORY
	g_pVB = NULL;
#else
	ZeroMemory(g_pVertices, sizeof(VERTEXTXTA)*MAX_BLOCKS*6*4);
#endif

	for( int i=0; i<TOTAL_HAVEWEAPON; i++ ){
		HUD_myweapon_x[i] = 0.0f;
		HUD_myweapon_y[i] = 0.0f;
		HUD_myweapon_z[i] = 0.0f;
	}

	StartRenderFlag = false;
	EnableFogFlag = false;

	ptextsprite = NULL;
	pxmsfont = NULL;
	pxmssmallfont = NULL;
	pxmsfont_briefing = NULL;

	pxmsfont_ko = NULL;
	pxmssmallfont_ko = NULL;
	pxmsfont_jp = NULL;
	pxmssmallfont_jp = NULL;

	TextureFontFname[0] = '\0';
	TextureFont = -1;

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
//! 걁DirectX 9걂
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
	OutputLog.WriteLog(LOG_INIT, "Graphics", "DirectX");
#endif

	D3DPRESENT_PARAMETERS d3dpp;
	RECT rec;

	GetClientRect(WindowCtrl->GethWnd(), &rec);

	fullscreenflag = fullscreen;
	LanguageMode = language;

	//D3D9궻띿맟
	pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if( pD3D == NULL ){
		return 1;
	}

	// D3Dデバイスの作成
// fullscreenflag == true の場合も、排他フルスクリーンにはしない。
// 枠なしウィンドウを使うため、DirectX側は常に Windowed = TRUE にする。
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.BackBufferWidth = rec.right;
	d3dpp.BackBufferHeight = rec.bottom;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.FullScreen_RefreshRateInHz = 0;

	if( FAILED( pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, WindowCtrl->GethWnd(), D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice) ) ){
		if( FAILED( pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, WindowCtrl->GethWnd(), D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice) ) ){
			return 1;
		}
	}

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	//긡긏긚?긿긲긅깛긣뾭됪몴궻긲?귽깑뼹귩먠믦
	strcpy(TextureFontFname, TextureFontFilename);

	//?됪듫똚궻뤬띢궶먠믦
	if( InitSubset() != 0){
		return 1;
	}
	

#if GRAPHIC_ENGINE == 1
	HRESULT hr;
	LPD3DXBUFFER pCode;
	LPD3DXBUFFER pErrorMes;

	//뮯?긘긃???룊딖돸
	//hr = D3DXCompileShaderFromFile("DirectX_Shader01.fx", NULL, NULL, "VertexShader_Main", "vs_2_0", 0, &pCode, &pErrorMes, &hVSConstantTable);
	hr = D3DXCompileShaderFromResource(NULL, MAKEINTRESOURCE(HLSLFILE), NULL, NULL, "VertexShader_Main", "vs_2_0", 0, &pCode, &pErrorMes, &hVSConstantTable);
	if( FAILED(hr) ){
#ifdef _DEBUG
		//긘긃???긓깛긬귽깑렄궻긄깋??렑
		OutputDebugStringA( (const char*)pErrorMes->GetBufferPointer() );
		//WindowCtrl->ErrorInfo( (const char*)pErrorMes->GetBufferPointer() );
#endif
		return 1;
	}
	hr = pd3dDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(), &hVertexShader);
	pCode->Release();
	if( FAILED(hr) ){ return 1; }

	//뮯?긘긃???궻듫쁀빾릶
	hVSWorldVertexPos = hVSConstantTable->GetConstantByName(NULL, "WorldVertexPos");
	if( hVSWorldVertexPos == NULL ){ return 1; }
	hVSWorldViewPos = hVSConstantTable->GetConstantByName(NULL, "WorldViewPos");
	if( hVSWorldViewPos == NULL ){ return 1; }
	hVSWorldViewProj = hVSConstantTable->GetConstantByName(NULL, "WorldViewProj");
	if( hVSWorldViewProj == NULL ){ return 1; }
	hVSDiffuse = hVSConstantTable->GetConstantByName(NULL, "Diffuse");
	if( hVSDiffuse == NULL ){ return 1; }

	//긯긏긜깑긘긃???룊딖돸
	//hr = D3DXCompileShaderFromFile("DirectX_Shader01.fx", NULL, NULL, "PixelShader_Main", "ps_2_0", 0, &pCode, &pErrorMes, &pPSConstantTable);
	hr = D3DXCompileShaderFromResource(NULL, MAKEINTRESOURCE(HLSLFILE), NULL, NULL, "PixelShader_Main", "ps_2_0", 0, &pCode, &pErrorMes, &pPSConstantTable);
	if( FAILED(hr) ){
#ifdef _DEBUG
		//긘긃???긓깛긬귽깑렄궻긄깋??렑
		OutputDebugStringA( (const char*)pErrorMes->GetBufferPointer() );
		//WindowCtrl->ErrorInfo( (const char*)pErrorMes->GetBufferPointer() );
#endif
		return 1;
	}
	hr = pd3dDevice->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &pPixelShader);
	pCode->Release();
	if( FAILED(hr) ){ return 1; }

	//긯긏긜깑긘긃???궻듫쁀빾릶
	hPSViewPos = pPSConstantTable->GetConstantByName(NULL, "ViewPos");
	//if( hPSViewPos == NULL ){ return 1; }
	hPSEnable2DRendering = pPSConstantTable->GetConstantByName(NULL, "Enable2DRendering");
	//if( hPSEnable2DRendering == NULL ){ return 1; }
	hPSEnableTexture = pPSConstantTable->GetConstantByName(NULL, "EnableTexture");
	if( hPSEnableTexture == NULL ){ return 1; }
	hPSEnableFog = pPSConstantTable->GetConstantByName(NULL, "EnableFog");
	if( hPSEnableFog == NULL ){ return 1; }
	hPSFogColor = pPSConstantTable->GetConstantByName(NULL, "FogColor");
	if( hPSFogColor == NULL ){ return 1; }
	hPSFogStart = pPSConstantTable->GetConstantByName(NULL, "FogStart");
	if( hPSFogStart == NULL ){ return 1; }
	hPSFogEnd = pPSConstantTable->GetConstantByName(NULL, "FogEnd");
	if( hPSFogEnd == NULL ){ return 1; }

	D3DXMatrixIdentity(&VSWorldPosMatrix);
	D3DXMatrixIdentity(&VSViewPosMatrix);
	D3DXMatrixIdentity(&VSViewProjMatrix);
#endif


	//귺긚긻긏긣붶귩먠믦
	aspect = (float)rec.right / (float)rec.bottom;

	//?긂긚긇??깑귩뤑궥
	//ShowCursor(FALSE);


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

	return 0;
}

//! @brief 깏긜긞긣@n
//! 걁긂귻깛긤긂띍룷돸궔귞궻븳딞?궶궵걂
//! @param WindowCtrl WindowControl긏깋긚궻?귽깛?
//! @return 맟뚻갌0?뫲궭갌1?렪봲갌2
int D3DGraphics::ResetD3D(WindowControl *WindowCtrl)
{
	if( WindowCtrl == NULL ){ return 2; }

	//긲긅?긇긚귩렪궯궲궋귡궶귞뫲궫궧귡
	if( pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICELOST ){
		return 1;
	}

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CHECK, "Graphics", "DirectX lost");
#endif

	//깏??긚됶뺳
	CleanupD3Dresource();

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_INIT, "Graphics", "DirectX (Reset)");
#endif

	D3DPRESENT_PARAMETERS d3dpp;
	RECT rec;

	GetClientRect(WindowCtrl->GethWnd(), &rec);

	// D3DデバイスのReset
// 枠なしフルスクリーンでは、Reset時も常にウィンドウモードとして扱う。
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.BackBufferWidth = rec.right;
	d3dpp.BackBufferHeight = rec.bottom;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.FullScreen_RefreshRateInHz = 0;

	if( FAILED( pd3dDevice->Reset(&d3dpp) ) ){
		return 2;
	}

	//?됪듫똚궻뤬띢궶먠믦
	if( InitSubset() != 0){
		return 2;
	}

#ifdef ENABLE_DEBUGCONSOLE
	if( LoadDebugFontTexture() == false ){
		return 2;
	}
#endif

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_COMPLETE, "", "");
#endif

	return 0;
}

//! @brief 됶뺳
//! @attention ?듫릶궼렔벍밒궸뚁귂뢯궠귢귏궥궕갂뼻렑밒궸뚁귂뢯궥궞궴귖됀?궳궥갃
void D3DGraphics::DestroyD3D()
{
	if( (pd3dDevice == NULL)&&(pD3D == NULL) ){ return; }

	//깏??긚됶뺳
	CleanupD3Dresource();

#if GRAPHIC_ENGINE == 1
	if( pPSConstantTable != NULL ){
		pPSConstantTable->Release();
		pPSConstantTable = NULL;
	}
	if( pPixelShader != NULL ){
		pPixelShader->Release();
		pPixelShader = NULL;
	}
	if( hVSConstantTable != NULL ){
		hVSConstantTable->Release();
		hVSConstantTable = NULL;
	}
	if( hVertexShader != NULL ){
		hVertexShader->Release();
		hVertexShader = NULL;
	}
#endif

	if( pd3dDevice != NULL ){
		pd3dDevice->Release();
		pd3dDevice = NULL;
	}
	if( pD3D != NULL ){
		pD3D->Release();
		pD3D = NULL;
	}

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_CLEANUP, "Graphics", "DirectX");
#endif
}

//! @brief ?됪듫똚궻띢븫먠믦
//! @attention 룊딖돸렄궸1뱗궬궚렳뛱궢궲궘궬궠궋갃
int D3DGraphics::InitSubset()
{
	//깋귽긣
	//pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(0,255,255,255) );
	pd3dDevice->LightEnable(0, FALSE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 긡긏긚?긿긲귻깑?귩럊궎
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//?긞긵?긞긵궻뤬띢깒긹깑 (LOD) 긫귽귺긚귩럚믦궥귡갃
	float LODBias = -0.2f;
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD)(&LODBias)) );

	//귺깑긲?갋긳깒깛긢귻깛긐귩뛱궎
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	//벁됡룉뿚귩뛱궎
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//귺깑긲?긡긚긣궸뫮돒궢궲궋귡궔?긃긞긏
	D3DCAPS9 Caps;
	pd3dDevice->GetDeviceCaps(&Caps);
	if( Caps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL ){
		//귺깑긲?긡긚긣먠믦
		//?뒶멣궸벁뼻궶긯긏긜깑궼?됪궢궶궋
		pd3dDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001);
		pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
		pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	}

	//?뱗긫긞긲?붶둹듫릶
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);

	//?깏긕깛궻뿞갋?
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


	//긡긌긚긣긚긵깋귽긣룊딖돸
	if( FAILED( D3DXCreateSprite( pd3dDevice, &ptextsprite ) ) ){
		return 1;
	}

	// 시스템 폰트 생성.
// SKILLS 화면에서 한국어/일본어를 DrawTextW로 출력할 수 있도록
// DEFAULT_CHARSET + Unicode 폰트 이름을 사용한다.
	if (FAILED(D3DXCreateFontW(
		pd3dDevice,
		(int)(((float)GameConfig.GetScreenHeight() / 480) * -20),
		0,
		FW_NORMAL,
		1,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"Dotum",
		&pxmsfont))
		) {
		return 1;
	}

	// 브리핑 본문 / 인게임 대사 자막 전용 폰트.
// 2번 버전의 Draw2DMSFontTextScaling과 같은 크기:
// 화면 높이 480 기준 -18을 현재 해상도에 맞게 스케일한다.
// 스킬 설명용 CJK 폰트(pxmsfont_ko / pxmssmallfont_ko 등)는 건드리지 않는다.
	int briefing_font_h = (int)(((float)GameConfig.GetScreenHeight() / 480.0f) * -18);

	if (FAILED(D3DXCreateFontW(
		pd3dDevice,
		briefing_font_h,
		0,
		FW_NORMAL,
		1,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"Dotum",
		&pxmsfont_briefing))
		) {
		return 1;
	}

	if (FAILED(D3DXCreateFontW(
		pd3dDevice,
		-14,
		0,
		FW_NORMAL,
		1,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"Dotum",
		&pxmssmallfont))
		) {
		return 1;
	}

	// SKILLS 화면 한국어/일본어 전용 폰트.
// 기존 pxmsfont는 해상도 기준이 480이라 너무 커질 수 있으므로,
// CJK 설명창에서는 별도 고정 크기 폰트를 사용한다.
	int cjk_large_h = -26;
	int cjk_small_h = -14;

	// 너무 작은 해상도에서는 약간 줄인다.
	if (GameConfig.GetScreenHeight() <= 720) {
		cjk_large_h = -23;
		cjk_small_h = -13;
	}

	// 한국어: 맑은 고딕
	if (FAILED(D3DXCreateFontW(
		pd3dDevice,
		cjk_large_h,
		0,
		FW_NORMAL,
		1,
		FALSE,
		HANGEUL_CHARSET,
		OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"Dotum",
		&pxmsfont_ko))
		) {
		return 1;
	}

	if (FAILED(D3DXCreateFontW(
		pd3dDevice,
		cjk_small_h,
		0,
		FW_NORMAL,
		1,
		FALSE,
		HANGEUL_CHARSET,
		OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"Dotum",
		&pxmssmallfont_ko))
		) {
		return 1;
	}

	// 일본어: Meiryo
	// 일본어 Windows 호환성을 위해 Malgun Gothic 대신 Meiryo를 사용한다.
	if (FAILED(D3DXCreateFontW(
		pd3dDevice,
		cjk_large_h,
		0,
		FW_NORMAL,
		1,
		FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"Meiryo",
		&pxmsfont_jp))
		) {
		return 1;
	}

	if (FAILED(D3DXCreateFontW(
		pd3dDevice,
		cjk_small_h,
		0,
		FW_NORMAL,
		1,
		FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"Meiryo",
		&pxmssmallfont_jp))
		) {
		return 1;
	}

	//긡긏긚?긿긲긅깛긣뾭됪몴귩롦벦
	TextureFont = LoadTexture(TextureFontFname, true, false);

	//긲긅긐궼띋뱗룊딖돸
	EnableFogFlag = false;

	return 0;
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

	if( pxmsfont != NULL ){
		pxmsfont->Release();
		pxmsfont = NULL;
	}
	if (pxmssmallfont != NULL) {
		pxmssmallfont->Release();
		pxmssmallfont = NULL;
	}

	if (pxmsfont_briefing != NULL) {
		pxmsfont_briefing->Release();
		pxmsfont_briefing = NULL;
	}

	if (pxmsfont_ko != NULL) {
		pxmsfont_ko->Release();
		pxmsfont_ko = NULL;
	}
	if (pxmssmallfont_ko != NULL) {
		pxmssmallfont_ko->Release();
		pxmssmallfont_ko = NULL;
	}
	if (pxmsfont_jp != NULL) {
		pxmsfont_jp->Release();
		pxmsfont_jp = NULL;
	}
	if (pxmssmallfont_jp != NULL) {
		pxmssmallfont_jp->Release();
		pxmssmallfont_jp = NULL;
	}

	if (ptextsprite != NULL) {
		ptextsprite->Release();
		ptextsprite = NULL;
	}

	CleanupMapdata();

	for(int i=0; i<MAX_MODEL; i++){
		CleanupModel(i);
	}
	for(int i=0; i<MAX_TEXTURE; i++){
		CleanupTexture(i);
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

	//뗴궋궲궋귡뾴멹귩뭈궥
	for(int i=0; i<MAX_MODEL; i++){
		if( pmesh[i] == NULL ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return -1; }

#ifdef ENABLE_PATH_DELIMITER_SLASH
	//긬긚뗦먛귟빒럻귩빾듂
	filename = ChangePathDelimiter(filename);
#endif

	// .x 파일을 버퍼에 읽어들임
	if (FAILED(D3DXLoadMeshFromX(
		filename,
		D3DXMESH_MANAGED,
		pd3dDevice,
		NULL,
		NULL,
		NULL,
		&nummaterials[id],
		&pmesh[id]
	))) {
		return -1;
	}

#ifdef ENABLE_MESH_OPTIMIZE
	// 메시 렌더링 최적화
	// DrawSubset() 비용을 줄이기 위해 정점 캐시 / 속성 정렬 / 압축 최적화를 적용한다.
	if (pmesh[id] != NULL) {
		DWORD faceCount = pmesh[id]->GetNumFaces();

		if (faceCount > 0) {
			DWORD* adjacency = new DWORD[faceCount * 3];

			if (adjacency != NULL) {
				if (SUCCEEDED(pmesh[id]->GenerateAdjacency(0.001f, adjacency))) {
					HRESULT hrOptimize = pmesh[id]->OptimizeInplace(
						D3DXMESHOPT_VERTEXCACHE |
						D3DXMESHOPT_ATTRSORT |
						D3DXMESHOPT_COMPACT,
						adjacency,
						NULL,
						NULL,
						NULL
					);

#ifdef ENABLE_DEBUGLOG
					if (FAILED(hrOptimize)) {
						OutputLog.WriteLog(LOG_ERROR, "Mesh optimize failed", filename);
					}
#endif
				}

				delete[] adjacency;
			}
		}
	}
#endif

#ifdef ENABLE_DEBUGLOG
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
	if( pmesh[idA] == NULL ){ return -1; }
	if( (idB < 0)||(MAX_MODEL <= idB) ){ return -1; }
	if( pmesh[idB] == NULL ){ return -1; }

	int idN = -1;
	int numvA, numvB;
	LPDIRECT3DVERTEXBUFFER9 pvbA, pvbB, pvbN;
	D3DXVECTOR3 *pVerticesA, *pVerticesB, *pVerticesN;
	int FVFsize;

	//뗴궋궲궋귡뾴멹귩뭈궥
	for(int i=0; i<MAX_MODEL; i++){
		if( pmesh[i] == NULL ){
			idN = i;
			break;
		}
	}
	if( idN == -1 ){ return -1; }

	//뮯?릶귩롦벦
	numvA = pmesh[idA]->GetNumVertices();
	numvB = pmesh[idB]->GetNumVertices();

	//뮯?릶궕벏궣궔궵궎궔뮧귊귡
	if( numvA != numvB ){ return -1; }

	//뮯?긢??귩긓긯?걁렳렲밒궸쀌덃둴뺎뾭궻???걂
	if( pmesh[idA]->CloneMeshFVF(pmesh[idA]->GetOptions(), pmesh[idA]->GetFVF(), pd3dDevice, &pmesh[idN]) != D3D_OK ){
		return -1;
	}

	//?긡깏귺깑륃뺪귩긓긯?
	nummaterials[idN] = nummaterials[idA];

	//긫긞긲??귩롦벦
	pmesh[idA]->GetVertexBuffer(&pvbA);
	pmesh[idB]->GetVertexBuffer(&pvbB);
	pmesh[idN]->GetVertexBuffer(&pvbN);

	//1뮯?궇궫귟궻긫귽긣릶롦벦
	FVFsize = D3DXGetFVFVertexSize(pmesh[idN]->GetFVF());

	//둫뮯?귩벶귒뢯궢똶럁
	for(int i=0; i<numvA; i++){
		pvbA->Lock(i*FVFsize, sizeof(D3DXVECTOR3), (void**)&pVerticesA, D3DLOCK_READONLY);
		pvbB->Lock(i*FVFsize, sizeof(D3DXVECTOR3), (void**)&pVerticesB, D3DLOCK_READONLY);
		pvbN->Lock(i*FVFsize, sizeof(D3DXVECTOR3), (void**)&pVerticesN, 0);

		//빟뗉돸
		pVerticesN->x = (pVerticesA->x + pVerticesB->x)/2;
		pVerticesN->y = (pVerticesA->y + pVerticesB->y)/2;
		pVerticesN->z = (pVerticesA->z + pVerticesB->z)/2;

		pvbA->Unlock();
		pvbB->Unlock();
		pvbN->Unlock();
	}

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_COMPLETE, "", idN);
#endif
	return idN;
}

//! @brief 벶귒뜛귒띙귒궻긾긢깑릶귩롦벦
//! @return 긾긢깑릶
int D3DGraphics::GetTotalModels()
{
	int cnt = 0;

	//럊뾭뭷궻뾴멹귩릶궑귡
	for(int i=0; i<MAX_MODEL; i++){
		if( pmesh[i] != NULL ){ cnt += 1; }
	}

	return cnt;
}

//! @brief 긾긢깑긲?귽깑귩됶뺳
//! @param id 긾긢깑궻긢??붥뜂
void D3DGraphics::CleanupModel(int id)
{
	if( (id < 0)||(MAX_MODEL <= id) ){ return; }
	if( pmesh[id] != NULL ){
		pmesh[id]->Release();
		pmesh[id] = NULL;

#ifdef ENABLE_DEBUGLOG
		//깓긐궸뢯쀍
		OutputLog.WriteLog(LOG_CLEANUP, "Model", id);
#endif
	}
}

//! @brief 긡긏긚?긿귩벶귒뜛귔
//! @param filename 긲?귽깑뼹
//! @param texturefont 긡긏긚?긿긲긅깛긣긲깋긐
//! @param BlackTransparent 뜒귩벁됡궥귡
//! @return 맟뚻갌긡긏긚?긿궻긢??붥뜂걁0댥뤵걂?렪봲갌-1
int D3DGraphics::LoadTexture(const char* filename, bool texturefont, bool BlackTransparent)
{
	if( filename == NULL ){ return -1; }

	int id = -1;
	D3DXIMAGE_INFO info;
	int MipLevels;

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_LOAD, "Texture", filename);
#endif

	//뗴궋궲궋귡긢??붥뜂귩뭈궥
	for(int i=0; i<MAX_TEXTURE; i++){
		if( ptextures[i] == NULL ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return -1; }

#ifdef ENABLE_PATH_DELIMITER_SLASH
	//긬긚뗦먛귟빒럻귩빾듂
	filename = ChangePathDelimiter(filename);
#endif

	//긲?귽깑륃뺪귩롦벦
	if( D3DXGetImageInfoFromFile(filename, &info) != D3D_OK ){ return -1; }

	//?긞긵?긞긵깒긹깑귩먠믦
	if( texturefont == true ){
		MipLevels = 1;
	}
	else{
		MipLevels = 4;//D3DX_DEFAULT;
	}

	// 큰 텍스쳐 자동 축소
// 가로/세로 중 하나라도 1024를 넘으면, 비율을 유지하면서 긴 변을 1024로 맞춘다.
	const unsigned int SAFE_TEXTURE_SIZE = 1024;

	unsigned int loadWidth = info.Width;
	unsigned int loadHeight = info.Height;

	// 수정된 로직: 비율을 무시하고, 가로/세로 각각 1024를 넘으면 1024로 강제 고정
	if (loadWidth > SAFE_TEXTURE_SIZE) {
		loadWidth = SAFE_TEXTURE_SIZE;
	}
	if (loadHeight > SAFE_TEXTURE_SIZE) {
		loadHeight = SAFE_TEXTURE_SIZE;
	}

	//긡긏긚?긿귩벶귒뜛귔
	//긡긏긚?긿귩벶귒뜛귔
	if (BlackTransparent == false) {
		if (FAILED(D3DXCreateTextureFromFileEx(pd3dDevice, filename, loadWidth, loadHeight, MipLevels, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0x00000000, NULL, NULL, &ptextures[id]))) {
			return -1;
		}
	}
	else {
		// info.Width -> loadWidth, info.Height -> loadHeight 로 수정됨
		if (FAILED(D3DXCreateTextureFromFileEx(pd3dDevice, filename, loadWidth, loadHeight, MipLevels, 0, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, D3DCOLOR_ARGB(255, 0, 0, 0), NULL, NULL, &ptextures[id]))) {
			return -1;
		}
	}

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	//OutputLog.WriteLog(LOG_COMPLETE, "", id);

	char str[32];
	int width, height;
	GetTextureSize(id, &width, &height);
	sprintf(str, "ID갌%d (size:%dx%d)", id, width, height);
	OutputLog.WriteLog(LOG_COMPLETE, "", str);
#endif
	return id;
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
	int headersize = 54;
	int bufsize = headersize  + width*height*3;
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
		if( ptextures[i] == NULL ){
			id = i;
			break;
		}
	}
	if( id == -1 ){ return false; }

	//.bmp밯둎뾭쀌덃띿맟
	unsigned char *bmpdata = new unsigned char [bufsize];

	//.bmp긶긞??띿맟
	for(int i=0; i<headersize; i++){ bmpdata[i] = 0x00; }
	bmpdata[0x00] = 'B';
	bmpdata[0x01] = 'M';
	Set4ByteLittleEndian(&(bmpdata[0x02]), bufsize);
	bmpdata[0x0A] = (unsigned char)headersize;
	bmpdata[0x0E] = (unsigned char)(headersize - 14);
	Set4ByteLittleEndian(&(bmpdata[0x12]), width);
	Set4ByteLittleEndian(&(bmpdata[0x16]), height);
	bmpdata[0x1A] = 1;
	bmpdata[0x1C] = 24;
	bmpdata[0x1E] = 0;
	Set4ByteLittleEndian(&(bmpdata[0x22]), bufsize - headersize);

	datacnt = headersize;

	//6뛱빁궻긢??귩띿맟
	for(int cnt_y=5; cnt_y>=0; cnt_y--){
		for(int line_y=(charheight-1); line_y>=0; line_y--){
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
						bmpdata[datacnt + 0] = 255;
						bmpdata[datacnt + 1] = 255;
						bmpdata[datacnt + 2] = 255;
					}
					else{
						bmpdata[datacnt + 0] = 0;
						bmpdata[datacnt + 1] = 0;
						bmpdata[datacnt + 2] = 0;
					}

					datacnt += 3;
				}
			}
		}
	}

	//맕뚥긓?긤궕볺궯궲궋귡뤵궻2뛱 32빒럻빁궼갂뗴뿎궸궥귡갃
	for(int cnt_y=0; cnt_y<2; cnt_y++){
		for(int line_y=(charheight-1); line_y>=0; line_y--){
			for(int cnt_x=0; cnt_x<16; cnt_x++){
				for(int line_x=0; line_x<charwidth; line_x++){
					bmpdata[datacnt + 0] = 0;
					bmpdata[datacnt + 1] = 0;
					bmpdata[datacnt + 2] = 0;

					datacnt += 3;
				}
			}
		}
	}

	//.bmp긡긏긚?긿궴궢궲벶귒뜛귔
	if( FAILED( D3DXCreateTextureFromFileInMemoryEx(pd3dDevice, bmpdata, bufsize, width, height, 1, 0, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, D3DCOLOR_ARGB(255, 0, 0, 0), NULL, NULL, &ptextures[id]) ) ) {
		TextureDebugFont = -1;
		return false;
	}

	/*
	//.bmp궴궢궲뺎뫔궢궲귒귡
	FILE *fp;
	fp = fopen("debugfont-test.bmp", "wb");
	fwrite(bmpdata, sizeof(unsigned char), bufsize, fp);
	fclose(fp);
	*/

	//.bmp밯둎뾭쀌덃됶뺳
	delete [] bmpdata;

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
	if( ptextures[id] == NULL ){ return 1; }
	if( width == NULL ){ return 1; }
	if( height == NULL ){ return 1; }

	IDirect3DSurface9 *surface;
	D3DSURFACE_DESC desc;

	//긖?긲긃귽긚귩롦벦
	ptextures[id]->GetSurfaceLevel(0, &surface);

	//븴궴뛼궠귩롦벦
	surface->GetDesc(&desc);
	*width = desc.Width;
	*height = desc.Height;

	//긖?긲긃귽긚귩됶뺳
	surface->Release();

	return 0;
}

//! @brief 벶귒뜛귒띙귒궻긡긏긚?긿릶귩롦벦
//! @return 긡긏긚?긿릶
int D3DGraphics::GetTotalTextures()
{
	int cnt = 0;

	//럊뾭뭷궻뾴멹귩릶궑귡
	for(int i=0; i<MAX_TEXTURE; i++){
		if( ptextures[i] != NULL ){ cnt += 1; }
	}

	return cnt;
}

//! @brief 긡긏긚?긿귩됶뺳
//! @param id 긡긏긚?긿궻긢??붥뜂
void D3DGraphics::CleanupTexture(int id)
{
	if( (id < 0)||(MAX_TEXTURE <= id) ){ return; }
	if( ptextures[id] != NULL ){
		ptextures[id]->Release();
		ptextures[id] = NULL;

#ifdef ENABLE_DEBUGLOG
		//깓긐궸뢯쀍
		OutputLog.WriteLog(LOG_CLEANUP, "Texture", id);
#endif
	}
}

//! @brief 멣궲궻?됪룉뿚귩둎럑
//! @return 맟뚻갌0?렪봲갌1
//! @attention ?됪룉뿚궻띍룊궸뚁귂뢯궥뷠뾴궕궇귟귏궥갃
int D3DGraphics::StartRender()
{
	if( StartRenderFlag == true ){ return 1; }

	//쀌덃귩룊딖돸
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);

	if( SUCCEEDED( pd3dDevice->BeginScene() ) ){
		//Z긫긞긲?룊딖돸
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

#if GRAPHIC_ENGINE == 1
		//뮯?긘긃???궴긯긏긜깑긘긃???먠믦
		pd3dDevice->SetVertexShader(hVertexShader);
		pd3dDevice->SetPixelShader(pPixelShader);
#endif

		//띆뷭?깓뭤?궸깗?깑긤빾듂뛱쀱
		ResetWorldTransform();

		//?됪뭷궻긲깋긐귩뿧궲귡
		StartRenderFlag = true;
		return 0;
	}

	return 1;
}

//! @brief 멣궲궻?됪룉뿚귩뢎뿹
//! @return 맟뚻갌false?렪봲갌true
//! @attention ?됪룉뿚궻띍뚣궸뚁귂뢯궥뷠뾴궕궇귟귏궥갃
bool D3DGraphics::EndRender()
{
	//?됪뭷궶귞뢎뿹
	if( StartRenderFlag == true ){
		pd3dDevice->EndScene();
	}

	HRESULT hr = pd3dDevice->Present(NULL, NULL, NULL, NULL);

	//긲깋긐귩 false 궸
	StartRenderFlag = false;

	if( hr == D3DERR_DEVICELOST ){
		return true;
	}
	return false;
}

//! @brief Z긫긞긲?귩깏긜긞긣
void D3DGraphics::ResetZbuffer()
{
	//Z긫긞긲?귩덇뱗뼰뚼궸궢갂룊딖돸뚣갂띋뱗뾎뚼궸
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
}

//! @brief 깗?깑긤뗴듩귩뙱?걁0,0,0걂궸뽣궥?궶궵
void D3DGraphics::ResetWorldTransform()
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
#if GRAPHIC_ENGINE == 0
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
#else
	hVSConstantTable->SetMatrix(pd3dDevice, hVSWorldVertexPos, &matWorld);
	VSWorldPosMatrix = matWorld;
#endif
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
	D3DXMATRIX matWorld;
	D3DXMATRIX matWorld1, matWorld2, matWorld3, matWorld4, matWorld5;

	//뛱쀱귩띿맟
	D3DXMatrixTranslation(&matWorld1, x, y, z);
	D3DXMatrixRotationY(&matWorld2, rx);
	D3DXMatrixRotationX(&matWorld3, ry1);
	D3DXMatrixRotationZ(&matWorld4, ry2);
	D3DXMatrixScaling(&matWorld5, size, size, size);

	//똶럁
	matWorld = matWorld5 * matWorld4 * matWorld3 * matWorld2 * matWorld1;

	//밙뾭
#if GRAPHIC_ENGINE == 0
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
#else
	hVSConstantTable->SetMatrix(pd3dDevice, hVSWorldVertexPos, &matWorld);
	VSWorldPosMatrix = matWorld;
#endif
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
	D3DXMATRIX matWorld;
	D3DXMATRIX matWorld1, matWorld2, matWorld3, matWorld4, matWorld5;

	//뛱쀱귩띿맟
	D3DXMatrixTranslation(&matWorld1, x, y, z);
	D3DXMatrixRotationY(&matWorld2, rx);
	D3DXMatrixRotationZ(&matWorld3, ry);
	D3DXMatrixRotationX(&matWorld4, rt);
	D3DXMatrixScaling(&matWorld5, size, size, size);

	//똶럁
	matWorld = matWorld5 * matWorld4 * matWorld3 * matWorld2 * matWorld1;

	//밙뾭
#if GRAPHIC_ENGINE == 0
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
#else
	hVSConstantTable->SetMatrix(pd3dDevice, hVSWorldVertexPos, &matWorld);
	VSWorldPosMatrix = matWorld;
#endif
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
	D3DXMATRIX matWorld;
	D3DXMATRIX matWorld1, matWorld2, matWorld3, matWorld4, matWorld5;

	//뛱쀱귩띿맟
	D3DXMatrixTranslation(&matWorld1, x, y, z);
	D3DXMatrixRotationY(&matWorld2, rx);
	D3DXMatrixRotationX(&matWorld3, ry);
	D3DXMatrixTranslation(&matWorld4, mx, my, mz);
	D3DXMatrixScaling(&matWorld5, size, size, size);

	//똶럁
	matWorld = matWorld5 * matWorld4 * matWorld3 * matWorld2 * matWorld1;

	//밙뾭
#if GRAPHIC_ENGINE == 0
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
#else
	hVSConstantTable->SetMatrix(pd3dDevice, hVSWorldVertexPos, &matWorld);
	VSWorldPosMatrix = matWorld;
#endif
}

//! @brief 깗?깑긤뗴듩귩룋렃궢궲궋귡븧딇귩?됪궥귡뤾룋궸먠믦
//! @param rotation 븧딇귩됷?궠궧귡
//! @param camera_rx 긇긽깋궻돘렡둷뱗
//! @param camera_ry 긇긽깋궻뢢렡둷뱗
//! @param rx 븧딇궻궻뢢렡둷뱗
//! @param size ?됪긖귽긛
//! @note rotation갋갋?true갌뙸띪렃궯궲궋귡븧딇궳궥갃?false갌?뷈궻븧딇궳궥갃걁rx 궼뼰럨궠귢귏궥걂
//! @todo 댧뭫귘긖귽긛궻뷊뮧맢
void D3DGraphics::SetWorldTransformPlayerWeapon(bool rotation, float camera_rx, float camera_ry, float rx, float size, float viewangle)
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matWorldV;
	D3DXMATRIXA16 matProj;
	D3DVIEWPORT9 pViewport;
	D3DXVECTOR3 p1;
	D3DXMATRIX matWorld1, matWorld2, matWorld3, matWorld4, matWorld5, matWorld6;

	if (rotation == true) {
		p1 = D3DXVECTOR3(HUD_myweapon_x[0], HUD_myweapon_y[0], HUD_myweapon_z[0]);
	}
	else {
		p1 = D3DXVECTOR3(HUD_myweapon_x[1], HUD_myweapon_y[1], HUD_myweapon_z[1]);
	}

	pd3dDevice->GetViewport(&pViewport);

#if GRAPHIC_ENGINE == 0
	//カメラ座標
	pd3dDevice->GetTransform(D3DTS_VIEW, &matWorldV);

	//カメラ設定（射影変換行列）viewangle
	//pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixPerspectiveFovLH(&matProj, viewangle, aspect, 1.0f, CLIPPINGPLANE_FAR);
#else
	//カメラ座標
	matWorldV = VSViewPosMatrix;

	//カメラ設定（射影変換行列）viewangle
	matProj = VSViewProjMatrix;
#endif

	D3DXMatrixIdentity(&matWorld);

	//スクリーン空間からオブジェクト空間にベクトルを射影
	D3DXVec3Unproject(&p1, &p1, &pViewport, &matProj, &matWorldV, &matWorld);

	//size = size * 0.3f;
	size = size * (0.0004f * GameConfig.GetScreenHeight() * GameConfig.GetScreenHeight() - 0.92f * GameConfig.GetScreenHeight() + 650.0f) / 1000.f;

	//行列計算
	D3DXMatrixTranslation(&matWorld1, p1.x, p1.y, p1.z);
	D3DXMatrixRotationY(&matWorld2, camera_rx * -1);
	D3DXMatrixRotationZ(&matWorld3, camera_ry);
	if (rotation == true) {
		D3DXMatrixRotationY(&matWorld5, rx);
	}
	else {
		D3DXMatrixRotationY(&matWorld5, D3DX_PI);
	}
	D3DXMatrixScaling(&matWorld6, size, size, size);

	matWorld = matWorld6 * matWorld5 * matWorld3 * matWorld2 * matWorld1;

	//適用
#if GRAPHIC_ENGINE == 0
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
#else
	hVSConstantTable->SetMatrix(pd3dDevice, hVSWorldVertexPos, &matWorld);
	VSWorldPosMatrix = matWorld;
#endif
}

//! @brief 깗?깑긤뗴듩궻띆뷭귩롦벦
//! @param *x x렡귩롷궚롦귡?귽깛?
//! @param *y y렡귩롷궚롦귡?귽깛?
//! @param *z z렡귩롷궚롦귡?귽깛?
void D3DGraphics::GetWorldTransformPos(float *x, float *y, float *z)
{
	if( (x == NULL)||(y == NULL)||(z == NULL) ){ return; }

	D3DXMATRIX matWorld;
#if GRAPHIC_ENGINE == 0
	pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
#else
	matWorld = VSWorldPosMatrix;
#endif
	*x = matWorld._41;
	*y = matWorld._42;
	*z = matWorld._43;
}

//! @brief 긲긅긐귩먠믦
//! @param enablefog 긲긅긐뾎뚼돸
//! @param skynumber 뗴궻붥뜂
void D3DGraphics::SetFog(bool enablefog, int skynumber)
{
	if( enablefog == true ){
		if( EnableFogFlag == false ){
			//긲긅긐
			float fog_st = 100;
			float fog_end = 800;
#if GRAPHIC_ENGINE == 0
			pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
			pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_RGBA(0, 0, 0, 0));
			pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
			pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
			pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&fog_st));
			pd3dDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD*)(&fog_end));
#else
			float FogColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
			pPSConstantTable->SetBool(pd3dDevice, hPSEnableFog, true);
			pPSConstantTable->SetFloatArray(pd3dDevice, hPSFogColor, FogColor, 4);
			pPSConstantTable->SetFloat(pd3dDevice, hPSFogStart, fog_st);
			pPSConstantTable->SetFloat(pd3dDevice, hPSFogEnd, fog_end);
#endif
		}

		D3DCOLOR skycolor;

		//뗴궻붥뜂궸귝귟륡귩뙂믦
		switch(skynumber){
			case 1: skycolor = D3DCOLOR_RGBA(64, 64+16, 64, 0); break;
			case 2: skycolor = D3DCOLOR_RGBA(16, 16, 16, 0); break;
			case 3: skycolor = D3DCOLOR_RGBA(0, 16, 32, 0); break;
			case 4: skycolor = D3DCOLOR_RGBA(32, 16, 16, 0); break;
			case 5: skycolor = D3DCOLOR_RGBA(64, 32, 32, 0); break;
			default: skycolor = D3DCOLOR_RGBA(0, 0, 0, 0); break;
		}

		//긲긅긐귩먠믦
#if GRAPHIC_ENGINE == 0
		pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, skycolor);
#else
		float FogColor[4] = {(float)((skycolor>>16)&0xFF)/255.0f, (float)((skycolor>>8)&0xFF)/255.0f, (float)(skycolor&0xFF)/255.0f, 1.0f};
		pPSConstantTable->SetFloatArray(pd3dDevice, hPSFogColor, FogColor, 4);
#endif

		EnableFogFlag = true;
	}
	else{
#if GRAPHIC_ENGINE == 0
		pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
#else
		pPSConstantTable->SetBool(pd3dDevice, hPSEnableFog, false);
#endif
		EnableFogFlag = false;
	}
}

//! @brief 긇긽깋걁럨?걂귩먠믦
//! @param camera_x 긇긽깋궻X띆뷭
//! @param camera_y 긇긽깋궻Y띆뷭
//! @param camera_z 긇긽깋궻Z띆뷭
//! @param camera_rx 긇긽깋궻돘렡둷뱗
//! @param camera_ry 긇긽깋궻뢢렡둷뱗
//! @param viewangle 럨뽰둷
//! @todo ?뤵/?돷귩뛀궘궴갂룋렃븧딇궻긾긢깑?됪궻뛀궖궕궓궔궢궘궶귡갃
void D3DGraphics::SetCamera(float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, float viewangle)
{
	float vUpVecF_x, vUpVecF_y, vUpVecF_z;
	D3DXMATRIX matWorld;
	D3DXMATRIXA16 matView;

	//camera_ry귩 -PI?PI 궻듩궸맫딮돸
	camera_ry = AngleNormalization(camera_ry);

	//긇긽깋궻뛀궖귩뙂믦
	if( fabsf(camera_ry) == D3DX_PI/2 ){
		//?뤵or?돷귩뛀궋궲궋귡궶귞궽갂긇긽깋궻뤵븫럚믦귩돘뛀궖궸궥귡갃
		vUpVecF_x = cosf(camera_rx); vUpVecF_y = 0.0f; vUpVecF_z = sinf(camera_rx);
	}
	else{
		if( fabsf(camera_ry) < D3DX_PI/2 ){
			vUpVecF_x = 0.0f; vUpVecF_y = 1.0f; vUpVecF_z = 0.0f;
		}
		else{
			vUpVecF_x = 0.0f; vUpVecF_y = -1.0f; vUpVecF_z = 0.0f;
		}
	}

	D3DXMatrixIdentity(&matWorld);
#if GRAPHIC_ENGINE == 0
	pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
#else
	hVSConstantTable->SetMatrix(pd3dDevice, hVSWorldVertexPos, &matWorld);
	VSWorldPosMatrix = matWorld;
#endif

	//긇긽깋띆뷭
	D3DXVECTOR3 vEyePt( camera_x, camera_y, camera_z );
	D3DXVECTOR3 vLookatPt( cosf(camera_rx)*cosf(camera_ry) + camera_x, sinf(camera_ry) + camera_y, sinf(camera_rx)*cosf(camera_ry) + camera_z );
	D3DXVECTOR3 vUpVec( vUpVecF_x, vUpVecF_y, vUpVecF_z );
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
#if GRAPHIC_ENGINE == 0
	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
#else
	hVSConstantTable->SetMatrix(pd3dDevice, hVSWorldViewPos, &matView);
	VSViewPosMatrix = matView;

	if( hPSViewPos != NULL ){
		float Camera[4] = {camera_x, camera_y, camera_z, 1.0f};
		pPSConstantTable->SetFloatArray(pd3dDevice, hPSViewPos, Camera, 4);
	}
#endif

	//긇긽깋먠믦걁롅뎓빾듂뛱쀱걂viewangle
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, viewangle, aspect, CLIPPINGPLANE_NEAR, CLIPPINGPLANE_FAR);
#if GRAPHIC_ENGINE == 0
	pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
#else
	hVSConstantTable->SetMatrix(pd3dDevice, hVSWorldViewProj, &matProj);
	VSViewProjMatrix = matProj;
#endif
}

//! @brief ?긞긵긢??귩롦귟뜛귔
//! @param in_BlockDataIF 긳깓긞긏긢??
//! @param directory 긳깓긞긏긢??궕뫔띪궥귡긢귻깒긏긣깏
void D3DGraphics::LoadMapdata(BlockDataInterface* in_BlockDataIF, const char *directory)
{
	//긳깓긞긏긢??궕럚믦궠귢궲궋궶궚귢궽갂룉뿚궢궶궋갃
	if( in_BlockDataIF == NULL ){ return; }
	if( directory == NULL ){ return; }

	CleanupMapdata();

	char fname[MAX_PATH];
	char fnamefull[MAX_PATH];
	//int bs;
	blockdata data;
	int vID[4];
	int uvID[4];

	//긏깋긚귩먠믦
	BlockDataIF = in_BlockDataIF;

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

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	OutputLog.WriteLog(LOG_LOAD, "Mapdata", "(Vertex data)");
#endif

#ifdef BLOCKDATA_GPUMEMORY
	VERTEXTXTA* pVertices;

	//긳깓긞긏릶귩롦벦
	bs = BlockDataIF->GetTotaldatas();

	//긳깓긞긏릶빁궻긫긞긲??귩띿맟
	pd3dDevice->CreateVertexBuffer(bs*6*4*sizeof(VERTEXTXTA), 0, D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1, D3DPOOL_DEFAULT, &g_pVB, NULL);

	for(int i=0; i<bs; i++){
		//긢??귩롦벦
		BlockDataIF->Getdata(&data, i);

		for(int j=0; j<6; j++){
			//뽋궻뮯?긢??궻듫쁀븊궚귩롦벦
			blockdataface(j, &vID[0], &uvID[0]);

			//GPU귩깓긞긏걁1뽋빁걂
			g_pVB->Lock((i*6+j)*4*sizeof(VERTEXTXTA), 4*sizeof(VERTEXTXTA), (void**)&pVertices, 0);

			//뮯?띆뷭갋UV띆뷭갋륡귩먠믦
			pVertices[0].position = D3DXVECTOR3( data.x[ vID[1] ], data.y[ vID[1] ], data.z[ vID[1] ] );
			pVertices[0].tu       = data.material[j].u[ uvID[1] ];
			pVertices[0].tv       = data.material[j].v[ uvID[1] ];
			pVertices[1].position = D3DXVECTOR3( data.x[ vID[2] ], data.y[ vID[2] ], data.z[ vID[2] ] );
			pVertices[1].tu       = data.material[j].u[ uvID[2] ];
			pVertices[1].tv       = data.material[j].v[ uvID[2] ];
			pVertices[2].position = D3DXVECTOR3( data.x[ vID[0] ], data.y[ vID[0] ], data.z[ vID[0] ] );
			pVertices[2].tu       = data.material[j].u[ uvID[0] ];
			pVertices[2].tv       = data.material[j].v[ uvID[0] ];
			pVertices[3].position = D3DXVECTOR3( data.x[ vID[3] ], data.y[ vID[3] ], data.z[ vID[3] ] );
			pVertices[3].tu       = data.material[j].u[ uvID[3] ];
			pVertices[3].tv       = data.material[j].v[ uvID[3] ];
			for(int k=0; k<4; k++){
				pVertices[k].color = D3DCOLOR_COLORVALUE(data.material[j].shadow, data.material[j].shadow, data.material[j].shadow, 1.0f);
			}

			//GPU궻깓긞긏귩됶룣
			g_pVB->Unlock();
		}
	}
#else
	//긳깓긞긏릶귩롦벦
	bs = BlockDataIF->GetTotaldatas();

	for(int i=0; i<bs; i++){
		//긢??귩롦벦
		BlockDataIF->Getdata(&data, i);

		for(int j=0; j<6; j++){
			//뽋궻뮯?긢??궻듫쁀븊궚귩롦벦
			blockdataface(j, vID, uvID);

			//뮯?띆뷭갋UV띆뷭갋륡귩먠믦
			g_pVertices[i][j][0].position = D3DXVECTOR3( data.x[ vID[1] ], data.y[ vID[1] ], data.z[ vID[1] ] );
			g_pVertices[i][j][0].tu       = data.material[j].u[ uvID[1] ];
			g_pVertices[i][j][0].tv       = data.material[j].v[ uvID[1] ];
			g_pVertices[i][j][1].position = D3DXVECTOR3( data.x[ vID[2] ], data.y[ vID[2] ], data.z[ vID[2] ] );
			g_pVertices[i][j][1].tu       = data.material[j].u[ uvID[2] ];
			g_pVertices[i][j][1].tv       = data.material[j].v[ uvID[2] ];
			g_pVertices[i][j][2].position = D3DXVECTOR3( data.x[ vID[0] ], data.y[ vID[0] ], data.z[ vID[0] ] );
			g_pVertices[i][j][2].tu       = data.material[j].u[ uvID[0] ];
			g_pVertices[i][j][2].tv       = data.material[j].v[ uvID[0] ];
			g_pVertices[i][j][3].position = D3DXVECTOR3( data.x[ vID[3] ], data.y[ vID[3] ], data.z[ vID[3] ] );
			g_pVertices[i][j][3].tu       = data.material[j].u[ uvID[3] ];
			g_pVertices[i][j][3].tv       = data.material[j].v[ uvID[3] ];
			for(int k=0; k<4; k++){
				g_pVertices[i][j][k].color = D3DCOLOR_COLORVALUE(data.material[j].shadow, data.material[j].shadow, data.material[j].shadow, 1.0f);
			}
		}
	}
#endif

#ifdef ENABLE_DEBUGLOG
	//깓긐궸뢯쀍
	//OutputLog.WriteLog(LOG_COMPLETE, "", id);

	char str[32];
	sprintf(str, "Blocks:%d, Vertices:%d", bs, bs*6*4);
	OutputLog.WriteLog(LOG_COMPLETE, "", str);
#endif
}

//! @brief ?긞긵긢??귩?됪
//! @param wireframe 깗귽깂?긲깒???렑
void D3DGraphics::RenderMapdata(bool wireframe)
{
	//긳깓긞긏긢??궕벶귒뜛귏귢궲궋궶궚귢궽갂룉뿚궢궶궋갃
	if( BlockDataIF == NULL ){ return; }

	blockdata data;
	int textureID;

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

	//?뱗긫긞긲?붶둹듫릶귩먠믦
	//pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);


#ifdef BLOCKDATA_GPUMEMORY
	//긢??먠믦
	pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(VERTEXTXTA));

	for(textureID=0; textureID<TOTAL_BLOCKTEXTURE; textureID++){
		//긡긏긚?긿궕맫륂궸벶귒뜛귕궲궋궶궚귢궽먠믦
#if GRAPHIC_ENGINE == 0
		if( mapTextureID[textureID] == -1 ){
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
			pd3dDevice->SetTexture(0, NULL);
		}
		else if( ptextures[ mapTextureID[textureID] ] == NULL ){
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
			pd3dDevice->SetTexture(0, NULL);
		}
		else{
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
			pd3dDevice->SetTexture(0, ptextures[mapTextureID[textureID]] );
		}
#else
		if( hPSEnable2DRendering != NULL ){ pPSConstantTable->SetBool(pd3dDevice, hPSEnable2DRendering, false); }
		if( mapTextureID[textureID] == -1 ){
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
			pPSConstantTable->SetBool(pd3dDevice, hPSEnableTexture, false);
			pd3dDevice->SetTexture(0, NULL);
		}
		else if( ptextures[ mapTextureID[textureID] ] == NULL ){
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
			pPSConstantTable->SetBool(pd3dDevice, hPSEnableTexture, false);
			pd3dDevice->SetTexture(0, NULL);
		}
		else{
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
			pPSConstantTable->SetBool(pd3dDevice, hPSEnableTexture, true);
			pd3dDevice->SetTexture(0, ptextures[mapTextureID[textureID]] );
		}
#endif

		for(int i=0; i<bs; i++){
			//긢??롦벦
			BlockDataIF->Getdata(&data, i);

			for(int j=0; j<6; j++){
				//긡긏긚?긿궻긢??붥뜂귩롦벦
				int ID = data.material[j].textureID;

				if( textureID == ID ){
					//뽋귩?됪
					pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (i*6+j)*4, 2);
				}
			}
		}
	}
#else
	//긢??귩먠믦
	pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);

	for(textureID=0; textureID<TOTAL_BLOCKTEXTURE; textureID++){
		//긡긏긚?긿궕맫륂궸벶귒뜛귕궲궋궶궚귢궽먠믦
#if GRAPHIC_ENGINE == 0
		if( mapTextureID[textureID] == -1 ){
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
			pd3dDevice->SetTexture(0, NULL);
		}
		else if( ptextures[ mapTextureID[textureID] ] == NULL ){
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
			pd3dDevice->SetTexture(0, NULL);
		}
		else{
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
			pd3dDevice->SetTexture(0, ptextures[mapTextureID[textureID]] );
		}
#else
		if( hPSEnable2DRendering != NULL ){ pPSConstantTable->SetBool(pd3dDevice, hPSEnable2DRendering, false); }
		if( mapTextureID[textureID] == -1 ){
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
			pPSConstantTable->SetBool(pd3dDevice, hPSEnableTexture, false);
			pd3dDevice->SetTexture(0, NULL);
		}
		else if( ptextures[ mapTextureID[textureID] ] == NULL ){
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
			pPSConstantTable->SetBool(pd3dDevice, hPSEnableTexture, false);
			pd3dDevice->SetTexture(0, NULL);
		}
		else{
			pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
			pPSConstantTable->SetBool(pd3dDevice, hPSEnableTexture, true);
			pd3dDevice->SetTexture(0, ptextures[mapTextureID[textureID]] );
		}
#endif

		for(int i=0; i<bs; i++){
			//긢??롦벦
			BlockDataIF->Getdata(&data, i);

			for(int j=0; j<6; j++){
				//긡긏긚?긿궻긢??붥뜂귩롦벦
				int ID = data.material[j].textureID;

				if( textureID == ID ){
					//뽋귩?됪
					pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_pVertices[i][j], sizeof(VERTEXTXTA));
				}
			}
		}
	}
#endif

	//?뱗긫긞긲?붶둹듫릶귩뙰궸뽣궥
	//pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
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
	// g_pVB == NULL 일 때의 강제 종료(return)를 제거합니다.
	if ((bs == 0) && (BlockDataIF == NULL)) { return; }

	// 텍스처를 해제하고 배열을 확실하게 초기화합니다.
	for (int i = 0; i < TOTAL_BLOCKTEXTURE; i++) {
		CleanupTexture(mapTextureID[i]);
		mapTextureID[i] = -1; // ★ [추가] 텍스처 ID 초기화
	}

#ifdef BLOCKDATA_GPUMEMORY
	// 버텍스 버퍼 해제
	if (g_pVB != NULL) {
		g_pVB->Release();
		g_pVB = NULL;
	}
#endif
	bs = 0;

	BlockDataIF = NULL;

#ifdef ENABLE_DEBUGLOG
	// 로그 출력
	OutputLog.WriteLog(LOG_CLEANUP, "Mapdata", "걁Vertex data걂");
#endif
}

//! @brief 긾긢깑긲?귽깑귩?됪
//! @param id_model 긾긢깑궻긢??붥뜂
//! @param id_texture 긡긏긚?긿궻긢??붥뜂
//! @param darkflag 긾긢깑귩댠궘궥귡
//! @param HUDmode HUD?렑뾭긾긢깑궸먠믦
//! @param nomodel 긾긢깑?됪뼰뚼돸
void D3DGraphics::RenderModel(int id_model, int id_texture, bool darkflag, bool HUDmode, bool nomodel, bool disable_culling, float alpha, float model_brightness, bool alpha_zwrite)
{
	if (id_model == -1 || pmesh[id_model] == NULL) { return; }

	if (nomodel == true) {
		Renderline(1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, GetColorCode(1.0f, 0.5f, 0.0f, 1.0f));
		Renderline(0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, GetColorCode(1.0f, 0.5f, 0.0f, 1.0f));
		Renderline(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, GetColorCode(1.0f, 0.5f, 0.0f, 1.0f));
		return;
	}

	if (disable_culling) {
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}

	if (alpha < 0.0f) { alpha = 0.0f; }
	if (alpha > 1.0f) { alpha = 1.0f; }

	bool alpha_model = (alpha < 0.999f);

	if (alpha_model == true) {
		// 기본 반투명 모델은 기존처럼 Z-write를 끈다.
		// 단, 그림자 분신처럼 같은 모델 내부에서 머리/몸통 렌더 우선순위가 꼬이는 경우에는
		// alpha_zwrite=true로 깊이값을 쓰게 해서 가까운 표면이 우선 보이게 한다.
		if (alpha_zwrite == true) {
			pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		}
		else {
			pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		}

		pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, alpha));
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	}

	float Brightness = darkflag ? 0.8f : 1.0f;

	if (model_brightness >= 0.0f) {
		if (model_brightness < 0.0f) { model_brightness = 0.0f; }
		if (model_brightness > 1.0f) { model_brightness = 1.0f; }

		Brightness = model_brightness;
	}

	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Emissive = D3DXCOLOR(Brightness, Brightness, Brightness, 1.0f);
	pd3dDevice->SetMaterial(&mtrl);

	if (id_texture == -1 || ptextures[id_texture] == NULL) {
		pd3dDevice->SetTexture(0, NULL);
	}
	else {
		pd3dDevice->SetTexture(0, ptextures[id_texture]);
	}

	for (int i = 0; i < (signed)nummaterials[id_model]; i++) {
		pmesh[id_model]->DrawSubset(i);
	}

	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	if (alpha_model == true) {
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

		// RenderModel 종료 후에는 항상 기존 기본 상태로 복구한다.
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}

	if (disable_culling) {
		pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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

	if( nomodel == true ){
		Renderline(0.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, GetColorCode(0.75f,0.75f,0.75f,1.0f));
		Renderline(0.0f, -0.5f, -0.5f, 0.0f, -0.5f, 0.5f, GetColorCode(0.75f,0.75f,0.75f,1.0f));
		Renderline(0.0f, -0.5f, 0.5f, 0.0f, 0.5f, 0.5f, GetColorCode(0.75f,0.75f,0.75f,1.0f));
		Renderline(0.0f, 0.5f, 0.5f, 0.0f, 0.5f, -0.5f, GetColorCode(0.75f,0.75f,0.75f,1.0f));
		return;
	}

	VERTEXTXTA BoardVertices[4];

	//뮯?띆뷭갋UV띆뷭갋륡/벁뼻뱗귩먠믦
	BoardVertices[0].position = D3DXVECTOR3(0.0f, 0.5f, -0.5f);
	BoardVertices[0].tu       = 1.0f;
	BoardVertices[0].tv       = 0.0f;
	BoardVertices[1].position = D3DXVECTOR3(0.0f, -0.5f, -0.5f);
	BoardVertices[1].tu       = 1.0f;
	BoardVertices[1].tv       = 1.0f;
	BoardVertices[2].position = D3DXVECTOR3(0.0f, 0.5f, 0.5f);
	BoardVertices[2].tu       = 0.0f;
	BoardVertices[2].tv       = 0.0f;
	BoardVertices[3].position = D3DXVECTOR3(0.0f, -0.5f, 0.5f);
	BoardVertices[3].tu       = 0.0f;
	BoardVertices[3].tv       = 1.0f;
	for(int i=0; i<4; i++){
		BoardVertices[i].color = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, alpha);
	}

	//귺깑긲?긳깒깛긤귩먠믦
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	//긡긏긚?긿궴긢???렜귩먠믦궢?됪
#if GRAPHIC_ENGINE == 1
	if( hPSEnable2DRendering != NULL ){ pPSConstantTable->SetBool(pd3dDevice, hPSEnable2DRendering, false); }
	pPSConstantTable->SetBool(pd3dDevice, hPSEnableTexture, true);
#endif
	pd3dDevice->SetTexture(0, ptextures[id_texture]);
	pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, BoardVertices, sizeof(VERTEXTXTA));

	//귺깑긲?긳깒깛긤귩뙰궸뽣궥
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
}

//! @brief 긄긲긃긏긣?됪뾭먠믦
void D3DGraphics::StartEffectRender()
{
	//?뱗긫긞긲?붶둹듫릶 뼰뚼돸
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
}

//! @brief 긄긲긃긏긣?됪뾭먠믦귩됶룣
void D3DGraphics::EndEffectRender()
{
	//?뱗긫긞긲?붶둹듫릶 뾎뚼돸
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
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
	Draw2DBox(0, 0, Width, Height, D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,alpha));
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
	VERTEXTXTA mv[2];

	mv[0].position = D3DXVECTOR3(x1, y1, z1);
	mv[1].position = D3DXVECTOR3(x2, y2, z2);
	for(int i=0; i<2; i++){
		mv[i].color = color;
		mv[i].tu = 0.0f;
		mv[i].tv = 0.0f;
	}

#if GRAPHIC_ENGINE == 1
	if( hPSEnable2DRendering != NULL ){ pPSConstantTable->SetBool(pd3dDevice, hPSEnable2DRendering, false); }
	pPSConstantTable->SetBool(pd3dDevice, hPSEnableTexture, false);
#endif
	pd3dDevice->SetTexture(0, NULL);
	pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, mv, sizeof(VERTEXTXTA));
}

//! @brief 2D 긘긚긡?긲긅깛긣궸귝귡긡긌긚긣?렑귩둎럑
//! @attention DirectX궻 ID3DXSprite 귩룊딖돸궢궲궋귏궥갃
void D3DGraphics::Start2DMSFontTextRender()
{
	ptextsprite->Begin(D3DXSPRITE_ALPHABLEND);
}

//! @brief 빒럻귩?렑걁긘긚긡?긲긅깛긣럊뾭걂
//! @param x x띆뷭
//! @param y y띆뷭
//! @param str 빒럻쀱?걁둂뛱긓?긤갌됀걂
//! @param color 륡
//! @warning <b>?렑궼뷄륂궸믟뫊궳궥갃</b>됪뽋볙궳돺뱗귖뚁귂뢯궥궴긬긲긅??깛긚궸뎓떯궢귏궥갃
//! @warning 걏둂뛱긓?긤귩뒋뾭궢덇뱗궸?렑궥귡걐걏볷?뚭궕뷠뾴궶궋빒럻궼긡긏긚?긿긲긅깛긣귩뒋뾭궥귡걐궶궵궻뫮돒귩뛳궣궲궘궬궠궋갃
//! @attention DirectX궻 ID3DXSprite 귩럊뾭궢갂긘긚긡?긲긅깛긣궳?렑궢궲궋귏궥갃
//! @attention 긲긅깛긣궻롰쀞귘긖귽긛궼뚂믦궳궥갃?빒럻귩볫뢣궸뢣궺궲뿧뫬뒾귩뢯궠궶궋궴뙥궸궘궘궶귟귏궥갃
void D3DGraphics::Draw2DMSFontText(int x, int y, const char *str, int color)
{
	//if( ptextsprite == NULL ){ return; }
	if( str == NULL ){ return; }

	//긡긌긚긣긚긵깋귽긣룊딖돸
	Start2DMSFontTextRender();

	//딈?띆뷭귩먠믦
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	ptextsprite->SetTransform(&matWorld);

	//빒럻귩?렑
	RECT rc = {x, y, 0, 0};
	pxmsfont->DrawText(ptextsprite, str, -1, &rc, DT_NOCLIP, color);

	//긡긌긚긣긚긵깋귽긣됶뺳
	End2DMSFontTextRender();
}

//! @brief UTF-8 문자열을 시스템 폰트로 출력
//! @details 한글/일본어처럼 텍스처 폰트로 표현할 수 없는 문자열용.
//!          str은 UTF-8로 저장된 문자열이어야 한다.
void D3DGraphics::Draw2DMSFontTextUTF8(int x, int y, const char* str, int color, bool smallfont, int language)
{
	if (str == NULL) { return; }
	if (ptextsprite == NULL) { return; }

	LPD3DXFONT usefont = NULL;

	// language: 0 = Korean, 1 = English/Default, 2 = Japanese
	if (language == 2) {
		usefont = smallfont ? pxmssmallfont_jp : pxmsfont_jp;
	}
	else {
		usefont = smallfont ? pxmssmallfont_ko : pxmsfont_ko;
	}

	// 전용 폰트 생성에 실패했을 때의 예비 처리
	if (usefont == NULL) {
		usefont = smallfont ? pxmssmallfont : pxmsfont;
	}

	if (usefont == NULL) { return; }

	UINT codepage = CP_UTF8;
	DWORD flags = MB_ERR_INVALID_CHARS;

	int wlen = MultiByteToWideChar(codepage, flags, str, -1, NULL, 0);

	if (wlen <= 0) {
		codepage = CP_ACP;
		flags = 0;
		wlen = MultiByteToWideChar(codepage, flags, str, -1, NULL, 0);
	}

	if (wlen <= 0) { return; }

	WCHAR* wstr = new WCHAR[wlen];
	MultiByteToWideChar(codepage, flags, str, -1, wstr, wlen);

	Start2DMSFontTextRender();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	ptextsprite->SetTransform(&matWorld);

	RECT rc = { x, y, 0, 0 };
	usefont->DrawTextW(ptextsprite, wstr, -1, &rc, DT_NOCLIP, color);

	End2DMSFontTextRender();

	delete[] wstr;
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
//! @warning <b>?렑궼뷄륂궸믟뫊궳궥갃</b>됪뽋볙궳돺뱗귖뚁귂뢯궥궴긬긲긅??깛긚궸뎓떯궢귏궥갃
//! @warning 걏둂뛱긓?긤귩뒋뾭궢덇뱗궸?렑궥귡걐걏볷?뚭궕뷠뾴궶궋빒럻궼긡긏긚?긿긲긅깛긣귩뒋뾭궥귡걐궶궵궻뫮돒귩뛳궣궲궘궬궠궋갃
//! @attention DirectX궻 ID3DXSprite 귩럊뾭궢갂긘긚긡?긲긅깛긣궳?렑궢궲궋귏궥갃
//! @attention 긲긅깛긣궻롰쀞귘긖귽긛궼뚂믦궳궥갃?빒럻귩볫뢣궸뢣궺궲뿧뫬뒾귩뢯궠궶궋궴뙥궸궘궘궶귟귏궥갃
void D3DGraphics::Draw2DMSFontTextCenter(int x, int y, int w, int h, const char *str, int color)
{
	//if( ptextsprite == NULL ){ return; }
	if( str == NULL ){ return; }

	//긡긌긚긣긚긵깋귽긣룊딖돸
	Start2DMSFontTextRender();

	//딈?띆뷭귩먠믦
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	ptextsprite->SetTransform(&matWorld);

	//빒럻귩?렑
	RECT rc = {x, y, x+w, y+h};
	pxmsfont->DrawText(ptextsprite, str, -1, &rc, DT_CENTER, color);

	//긡긌긚긣긚긵깋귽긣됶뺳
	End2DMSFontTextRender();
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

void D3DGraphics::Draw2DMSFontTextBriefingScaling(int x, int y, const char* str, int color)
{
	if (str == NULL) { return; }
	if (pxmsfont_briefing == NULL) { return; }

	float scaling_x = (float)GameConfig.GetScreenHeight() / 480;
	float scaling_y = (float)GameConfig.GetScreenHeight() / 480;

	int draw_x = (int)(scaling_x * x);
	int draw_y = (int)(scaling_y * y);

	Start2DMSFontTextRender();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	ptextsprite->SetTransform(&matWorld);

	RECT rc = { draw_x, draw_y, 0, 0 };
	pxmsfont_briefing->DrawText(ptextsprite, str, -1, &rc, DT_NOCLIP, color);

	End2DMSFontTextRender();
}

void D3DGraphics::Draw2DMSFontTextCenterBriefingScaling(int x, int y, int w, int h, const char* str, int color)
{
	if (str == NULL) { return; }
	if (pxmsfont_briefing == NULL) { return; }

	float scaling_x = (float)GameConfig.GetScreenHeight() / 480;
	float scaling_y = (float)GameConfig.GetScreenHeight() / 480;

	int draw_x = (int)(scaling_x * x);
	int draw_y = (int)(scaling_y * y);

	Start2DMSFontTextRender();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	ptextsprite->SetTransform(&matWorld);

	// 2번 버전의 Draw2DMSFontTextCenterScaling과 동일하게
	// x, y만 스케일하고 w, h는 그대로 사용한다.
	RECT rc = { draw_x, draw_y, draw_x + w, draw_y + h };
	pxmsfont_briefing->DrawText(ptextsprite, str, -1, &rc, DT_CENTER, color);

	End2DMSFontTextRender();
}

//! @brief 빒럻귩?렑걁긘긚긡?긲긅깛긣럊뾭갂룷궠궋걂
//! @param x x띆뷭
//! @param y y띆뷭
//! @param str 빒럻쀱?걁둂뛱긓?긤갌됀걂
//! @param color 륡
//! @note 딈?럅뾩궼Draw2DMSFontText()궴벏뱳궳궥궕갂빒럻궕룷궠궘궶귟귏궥갃
void D3DGraphics::Draw2DMSSmallFontText(int x, int y, const char *str, int color)
{
	//if( ptextsprite == NULL ){ return; }
	if( str == NULL ){ return; }

	//긡긌긚긣긚긵깋귽긣룊딖돸
	Start2DMSFontTextRender();

	//딈?띆뷭귩먠믦
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	ptextsprite->SetTransform(&matWorld);

	//빒럻귩?렑
	RECT rc = {x, y, 0, 0};
	pxmssmallfont->DrawText(ptextsprite, str, -1, &rc, DT_NOCLIP, color);

	//긡긌긚긣긚긵깋귽긣됶뺳
	End2DMSFontTextRender();
}

void D3DGraphics::Draw2DMSSmallFontTextScaling(int x, int y, const char* str, int color)
{
	if (str == NULL) { return; }

	float scaling_x = (float)GameConfig.GetScreenHeight() / 480;
	float scaling_y = (float)GameConfig.GetScreenHeight() / 480;

	Draw2DMSSmallFontText((int)(scaling_x * x), (int)(scaling_y * y), str, color);
}

void D3DGraphics::Draw2DMSSmallFontTextCenter(int x, int y, int w, int h, const char* str, int color)
{
	if (str == NULL) { return; }

	Start2DMSFontTextRender();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	ptextsprite->SetTransform(&matWorld);

	RECT rc = { x, y, x + w, y + h };
	pxmssmallfont->DrawText(ptextsprite, str, -1, &rc, DT_CENTER, color);

	End2DMSFontTextRender();
}

void D3DGraphics::Draw2DMSSmallFontTextCenterScaling(int x, int y, int w, int h, const char* str, int color)
{
	if (str == NULL) { return; }

	float scaling_x = (float)GameConfig.GetScreenHeight() / 480;
	float scaling_y = (float)GameConfig.GetScreenHeight() / 480;

	Draw2DMSSmallFontTextCenter((int)(scaling_x * x), (int)(scaling_y * y), w, h, str, color);
}

//! @brief 2D 긘긚긡?긲긅깛긣궸귝귡긡긌긚긣?렑귩뢎뿹
//! @attention DirectX궻 ID3DXSprite 귩됶뺳궢궲궋귏궥갃
void D3DGraphics::End2DMSFontTextRender()
{
	ptextsprite->End();
}

//! @brief 2D?됪뾭먠믦
void D3DGraphics::Start2DRender()
{
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	//?뱗긫긞긲?붶둹듫릶귩먠믦
	pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

	//긲긅긐귩덇렄밒궸뼰뚼궸궥귡
#if GRAPHIC_ENGINE == 0
	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
#else
	pPSConstantTable->SetBool(pd3dDevice, hPSEnableFog, false);
#endif
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

//! @brief 빒럻귩?렑걁긡긏긚?긿긲긅깛긣럊뾭걂
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

	//2D?됪뾭먠믦귩밙뾭
	Start2DRender();

	int w;
	float font_u, font_v;
	float t_u, t_v;
	TLVERTX pBoxVertices[4];

	//1빒럻궻UV띆뷭귩똶럁
	font_u = 1.0f / 16;
	font_v = 1.0f / 16;

	//깗?깑긤띆뷭귩뙱?궸뽣궥
	ResetWorldTransform();

	//긡긏긚?긿귩긲긅깛긣긡긏긚?긿궸먠믦
#if GRAPHIC_ENGINE == 1
	if( hPSEnable2DRendering != NULL ){ pPSConstantTable->SetBool(pd3dDevice, hPSEnable2DRendering, true); }
	pPSConstantTable->SetBool(pd3dDevice, hPSEnableTexture, true);
#endif
	pd3dDevice->SetTexture( 0, ptextures[TextureFont] );

	//긢???렜귩먠믦
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

	// ?궑귞귢궫빒럻릶빁깑?긵
	for(int i=0; i<(int)strlen(str); i++){
		//UV띆뷭귩똶럁
		w = str[i];
		if( w < 0 ){ w += 256; }
		t_u = (w % 16) * font_u;
		t_v = (w / 16) * font_v;

		//뮯?띆뷭갋UV띆뷭갋륡귩먠믦
		pBoxVertices[0].x = (float)x + i*fontwidth;
		pBoxVertices[0].y = (float)y;
		pBoxVertices[0].tu = t_u;
		pBoxVertices[0].tv = t_v;
		pBoxVertices[1].x = (float)x + fontwidth + i*fontwidth;
		pBoxVertices[1].y = (float)y;
		pBoxVertices[1].tu = t_u + font_u;
		pBoxVertices[1].tv = t_v;
		pBoxVertices[2].x = (float)x + i*fontwidth;
		pBoxVertices[2].y = (float)y + fontheight;
		pBoxVertices[2].tu = t_u;
		pBoxVertices[2].tv = t_v + font_v;
		pBoxVertices[3].x = (float)x + fontwidth + i*fontwidth;
		pBoxVertices[3].y = (float)y + fontheight;
		pBoxVertices[3].tu = t_u + font_u;
		pBoxVertices[3].tv = t_v + font_v;
		for(int j=0; j<4; j++){
			pBoxVertices[j].z = 0.0f;
			pBoxVertices[j].rhw = 1.0f;
			pBoxVertices[j].color = color;
		}

		//?렑
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pBoxVertices, sizeof(TLVERTX));
	}

	//2D?됪뾭먠믦귩됶룣
	End2DRender();
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

	//2D?됪뾭먠믦귩밙뾭
	Start2DRender();

	// 긡긏긚?긿긲귻깑?뼰뚼돸
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	int w;
	float font_u, font_v;
	float t_u, t_v;
	TLVERTX pBoxVertices[4];

	//1빒럻궻UV띆뷭귩똶럁
	font_u = 1.0f / 16;
	font_v = 1.0f / 8;

	//깗?깑긤띆뷭귩뙱?궸뽣궥
	ResetWorldTransform();

	//긡긏긚?긿귩긲긅깛긣긡긏긚?긿궸먠믦
#if GRAPHIC_ENGINE == 1
	if( hPSEnable2DRendering != NULL ){ pPSConstantTable->SetBool(pd3dDevice, hPSEnable2DRendering, true); }
	pPSConstantTable->SetBool(pd3dDevice, hPSEnableTexture, true);
#endif
	pd3dDevice->SetTexture( 0, ptextures[TextureDebugFont] );

	//긢???렜귩먠믦
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

	// ?궑귞귢궫빒럻릶빁깑?긵
	for(int i=0; i<(int)strlen(str); i++){
		//UV띆뷭귩똶럁
		w = str[i];
		if( w < 0 ){ w = ' '; }
		t_u = (w % 16) * font_u;
		t_v = (w / 16) * font_v;

		//뮯?띆뷭갋UV띆뷭갋륡귩먠믦
		pBoxVertices[0].x = (float)x + i*fontwidth;
		pBoxVertices[0].y = (float)y;
		pBoxVertices[0].tu = t_u;
		pBoxVertices[0].tv = t_v;
		pBoxVertices[1].x = (float)x + fontwidth + i*fontwidth;
		pBoxVertices[1].y = (float)y;
		pBoxVertices[1].tu = t_u + font_u;
		pBoxVertices[1].tv = t_v;
		pBoxVertices[2].x = (float)x + i*fontwidth;
		pBoxVertices[2].y = (float)y + fontheight;
		pBoxVertices[2].tu = t_u;
		pBoxVertices[2].tv = t_v + font_v;
		pBoxVertices[3].x = (float)x + fontwidth + i*fontwidth;
		pBoxVertices[3].y = (float)y + fontheight;
		pBoxVertices[3].tu = t_u + font_u;
		pBoxVertices[3].tv = t_v + font_v;
		for(int j=0; j<4; j++){
			pBoxVertices[j].z = 0.0f;
			pBoxVertices[j].rhw = 1.0f;
			pBoxVertices[j].color = color;
		}

		//?렑
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pBoxVertices, sizeof(TLVERTX));
	}

	//긡긏긚?긿긲귻깑?뾎뚼돸
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//2D?됪뾭먠믦귩됶룣
	End2DRender();
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
	TLVERTX pLineVertices[2];

	//2D?됪뾭먠믦귩밙뾭
	Start2DRender();

	//깗?깑긤띆뷭귩뙱?궸뽣궥
	ResetWorldTransform();

	//뮯?띆뷭궴륡궶궵귩먠믦
	pLineVertices[0].x = (float)x1;
	pLineVertices[0].y = (float)y1;
	pLineVertices[1].x = (float)x2;
	pLineVertices[1].y = (float)y2;
	for(int i=0; i<2; i++){
		pLineVertices[i].z = 0.0f;
		pLineVertices[i].rhw = 1.0f;
		pLineVertices[i].color = color;
		pLineVertices[i].tu = 0.0f;
		pLineVertices[i].tv = 0.0f;
	}

#if GRAPHIC_ENGINE == 1
	if( hPSEnable2DRendering != NULL ){ pPSConstantTable->SetBool(pd3dDevice, hPSEnable2DRendering, true); }
	pPSConstantTable->SetBool(pd3dDevice, hPSEnableTexture, false);
#endif
	pd3dDevice->SetTexture(0, NULL);

	//긢???렜귩먠믦궢갂?됪갃
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, pLineVertices, sizeof(TLVERTX));

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
	TLVERTX pLineVertices[16+1];

	//2D?됪뾭먠믦귩밙뾭
	Start2DRender();

	//깗?깑긤띆뷭귩뙱?궸뽣궥
	ResetWorldTransform();

	//뮯?띆뷭궴륡궶궵귩먠믦
	for(int i=0; i<16+1; i++){
		pLineVertices[i].x = (float)x + cosf(DegreeToRadian((360.0f/16.0f)) * i) * r;
		pLineVertices[i].y = (float)y + sinf(DegreeToRadian((360.0f/16.0f)) * i) * r;

		pLineVertices[i].z = 0.0f;
		pLineVertices[i].rhw = 1.0f;
		pLineVertices[i].color = color;
		pLineVertices[i].tu = 0.0f;
		pLineVertices[i].tv = 0.0f;
	}

#if GRAPHIC_ENGINE == 1
	if( hPSEnable2DRendering != NULL ){ pPSConstantTable->SetBool(pd3dDevice, hPSEnable2DRendering, true); }
	pPSConstantTable->SetBool(pd3dDevice, hPSEnableTexture, false);
#endif
	pd3dDevice->SetTexture(0, NULL);

	//긢???렜귩먠믦궢갂?됪갃
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 16, pLineVertices, sizeof(TLVERTX));

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
	TLVERTX pBoxVertices[4];

	//2D?됪뾭먠믦귩밙뾭
	Start2DRender();

	//깗?깑긤띆뷭귩뙱?궸뽣궥
	ResetWorldTransform();

	//뮯?띆뷭궴륡궶궵귩먠믦
	pBoxVertices[0].x = (float)x1;
	pBoxVertices[0].y = (float)y1;
	pBoxVertices[1].x = (float)x2;
	pBoxVertices[1].y = (float)y1;
	pBoxVertices[2].x = (float)x1;
	pBoxVertices[2].y = (float)y2;
	pBoxVertices[3].x = (float)x2;
	pBoxVertices[3].y = (float)y2;
	for(int i=0; i<4; i++){
		pBoxVertices[i].z = 0.0f;
		pBoxVertices[i].rhw = 1.0f;
		pBoxVertices[i].color = color;
		pBoxVertices[i].tu = 0.0f;
		pBoxVertices[i].tv = 0.0f;
	}

#if GRAPHIC_ENGINE == 1
	if( hPSEnable2DRendering != NULL ){ pPSConstantTable->SetBool(pd3dDevice, hPSEnable2DRendering, true); }
	pPSConstantTable->SetBool(pd3dDevice, hPSEnableTexture, false);
#endif
	pd3dDevice->SetTexture(0, NULL);

	//긢???렜귩먠믦궢갂?됪갃
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pBoxVertices, sizeof(TLVERTX));

	//2D?됪뾭먠믦귩됶룣
	End2DRender();
}

// 4개의 정점으로 구성된 사각형을 그립니다.
void D3DGraphics::Draw2DDonutArc(float cx, float cy, float radius, float start_angle, float end_angle, int thickness, int segments, int color)
{
	if (StartRenderFlag == false) { return; }

	// segments는 최소 1 이상이어야 삼각형 스트립을 만들 수 있음.
	if (segments < 1) { return; }

	// 너무 큰 값 방지. 실사용은 20~100 정도면 충분함.
	if (segments > 360) { segments = 360; }

	if (radius <= 0.0f) { return; }
	if (thickness <= 0) { return; }

	Start2DRender();
	ResetWorldTransform();

	int num_vertices = (segments + 1) * 2;
	TLVERTX* vertices = new TLVERTX[num_vertices];
	if (!vertices) {
		End2DRender();
		return;
	}

	float theta_step = (end_angle - start_angle) / (float)segments;

	for (int i = 0; i <= segments; ++i)
	{
		int inner_index = i * 2;
		int outer_index = inner_index + 1;

		float angle = start_angle + i * theta_step;
		float cos_a = cosf(angle - (float)M_PI / 2);
		float sin_a = sinf(angle - (float)M_PI / 2);

		float inner_radius = radius - ((float)thickness / 2.0f);
		float outer_radius = radius + ((float)thickness / 2.0f);

		vertices[inner_index].x = cx + inner_radius * cos_a;
		vertices[inner_index].y = cy + inner_radius * sin_a;
		vertices[inner_index].z = 0.0f;
		vertices[inner_index].rhw = 1.0f;
		vertices[inner_index].color = color;
		vertices[inner_index].tu = 0.0f;
		vertices[inner_index].tv = 0.0f;

		vertices[outer_index].x = cx + outer_radius * cos_a;
		vertices[outer_index].y = cy + outer_radius * sin_a;
		vertices[outer_index].z = 0.0f;
		vertices[outer_index].rhw = 1.0f;
		vertices[outer_index].color = color;
		vertices[outer_index].tu = 0.0f;
		vertices[outer_index].tv = 0.0f;
	}

#if GRAPHIC_ENGINE == 1
	if (hPSEnable2DRendering != NULL) { pPSConstantTable->SetBool(pd3dDevice, hPSEnable2DRendering, true); }
	pPSConstantTable->SetBool(pd3dDevice, hPSEnableTexture, false);
#endif

	pd3dDevice->SetTexture(0, NULL);

	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, num_vertices - 2, vertices, sizeof(TLVERTX));

	delete[] vertices;

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

	TLVERTX pBoxVertices[4];

	//2D?됪뾭먠믦귩밙뾭
	Start2DRender();

	//깗?깑긤띆뷭귩뙱?궸뽣궥
	ResetWorldTransform();

	//뮯?띆뷭갋UV띆뷭갋륡귩먠믦
	pBoxVertices[0].x = (float)x;
	pBoxVertices[0].y = (float)y;
	pBoxVertices[0].tu = 0.0f;
	pBoxVertices[0].tv = 0.0f;
	pBoxVertices[1].x = (float)x + width;
	pBoxVertices[1].y = (float)y;
	pBoxVertices[1].tu = 1.0f;
	pBoxVertices[1].tv = 0.0f;
	pBoxVertices[2].x = (float)x;
	pBoxVertices[2].y = (float)y + height;
	pBoxVertices[2].tu = 0.0f;
	pBoxVertices[2].tv = 1.0f;
	pBoxVertices[3].x = (float)x + width;
	pBoxVertices[3].y = (float)y + height;
	pBoxVertices[3].tu = 1.0f;
	pBoxVertices[3].tv = 1.0f;
	for(int i=0; i<4; i++){
		pBoxVertices[i].z = 0.0f;
		pBoxVertices[i].rhw = 1.0f;
		pBoxVertices[i].color = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,alpha);
	}

	//긡긏긚?긿궴긢???렜귩먠믦궢갂?됪
#if GRAPHIC_ENGINE == 1
	if( hPSEnable2DRendering != NULL ){ pPSConstantTable->SetBool(pd3dDevice, hPSEnable2DRendering, true); }
	pPSConstantTable->SetBool(pd3dDevice, hPSEnableTexture, true);
#endif
	pd3dDevice->SetTexture( 0, ptextures[id] );
	pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pBoxVertices, sizeof(TLVERTX));

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
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	//?뱗긫긞긲?붶둹듫릶귩뙰궸뽣궥
	pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	//긲긅긐귩뙰궸뽣궥
	if( EnableFogFlag == true ){
#if GRAPHIC_ENGINE == 0
		pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
#else
		pPSConstantTable->SetBool(pd3dDevice, hPSEnableFog, true);
#endif
	}
}

//! @brief 됪뽋궻긚긏깏?깛긘깈긞긣귩뺎뫔
//! @param filename 긲?귽깑뼹
//! @return 맟뚻갌true?렪봲갌false
bool D3DGraphics::SaveScreenShot(const char* filename)
{
	if( filename == NULL ){ return false; }

	LPDIRECT3DSURFACE9 pSurface = NULL;
	HRESULT hr;

	//긖?긲긃?긚귩띿맟궢갂됪뽋귩롦벦
	pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);

	//긖?긲긃귽긚귩됪몴궸뢯쀍
	hr = D3DXSaveSurfaceToFile(filename, D3DXIFF_BMP, pSurface, NULL, NULL);

	//됶뺳
	pSurface->Release();

	if( hr == D3D_OK ){
		return true;
	}
	return false;
}

//! @brief 긇깋?긓?긤귩롦벦
//! @param red 먗걁0.0f?1.0f걂
//! @param green 쀎걁0.0f?1.0f걂
//! @param blue 먃걁0.0f?1.0f걂
//! @param alpha 벁뼻뱗걁0.0f?1.0f걂
//! @return 긇깋?긓?긤
int D3DGraphics::GetColorCode(float red, float green, float blue, float alpha)
{
	return D3DCOLOR_COLORVALUE(red, green, blue, alpha);
}

#endif	//GRAPHIC_ENGINE

void D3DGraphics::SetAmbientLight(int color)
{
	// DirectX 9의 전역 조명 밝기를 설정합니다.
	pd3dDevice->SetRenderState(D3DRS_AMBIENT, (D3DCOLOR)color);
}