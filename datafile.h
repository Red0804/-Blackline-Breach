//! @file datafile.h
//! @brief デ??管理クラスの宣言

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

#ifndef DATAFILE_H
#define DATAFILE_H

#define MAX_BLOCKS 160			//!< 最大ブロック数
#define TOTAL_BLOCKTEXTURE 10	//!< 合計テクス?ャ数

//#define MAX_POINTS 200
#define MAX_POINTS 256			//!< 最大?イント数

//! @brief 追加小物の数
//! @attention 設定値を増やす場合、MAX_MODEL・MAX_TEXTURE・MAX_LOADSOUNDの値も適切に見直すこと
//! @note 追加小物設定フ?イルの下部に、続けて次の追加小物の設定値を記載します。ただし、各追加小物の設定値の間に???行（"//"推奨）を?んでください。
#define MAX_ADDSMALLOBJECT 1

#define MAX_POINTMESSAGES 16			//!< .msgフ?イルの最大メッセ?ジ数
#define MAX_POINTMESSAGEBYTE (71+2)		//!< .msgフ?イル 1行あたりの最大バイト数

#define LIGHT_RX DegreeToRadian(190)	//!< ライトの横軸角度（ブロックの陰影計算用）
#define LIGHT_RY DegreeToRadian(120)	//!< ライトの縦軸角度（ブロックの陰影計算用）

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 1		//!< Select include file.
#endif
#include "main.h"

#include <ctype.h>
#include <sys/stat.h>

//! ブロックデ??用?造体?（blockdata?造体で使用）
typedef struct
{
	int textureID;				//!< textureID
	float u[4];					//!< textureUV-U
	float v[4];					//!< textureUV-V
	float center_x;				//!< center
	float center_y;				//!< center
	float center_z;				//!< center
	float vx;					//!< normal vector 
	float vy;					//!< normal vector 
	float vz;					//!< normal vector 
	float shadow;				//!< shadow 
} b_material;
//! ブロックデ??用?造体
typedef struct
{
	int id;						//!< DataID
	float x[8];					//!< Position
	float y[8];					//!< Position
	float z[8];					//!< Position
	b_material material[6];		//!< Material data
} blockdata;

//! ?イントデ??用?造体
typedef struct
{
	int id;						//!< DataID
	float x;					//!< Position
	float y;					//!< Position
	float z;					//!< Position
	float r;					//!< Rotation
	signed short int p1;		//!< Param
	signed short int p2;		//!< Param
	signed short int p3;		//!< Param
	signed short int p4;		//!< Param
} pointdata;

//?イントデ??のP1の種類番号
#define POINT_P1TYPE_HUMAN			1	//!< Param1 type
#define POINT_P1TYPE_WEAPON			2	//!< Param1 type
#define POINT_P1TYPE_AIPATH			3	//!< Param1 type
#define POINT_P1TYPE_HUMANINFO		4	//!< Param1 type
#define POINT_P1TYPE_SMALLOBJ		5	//!< Param1 type
#define POINT_P1TYPE_HUMAN2			6	//!< Param1 type
#define POINT_P1TYPE_RAND_WEAPON	7	//!< Param1 type
#define POINT_P1TYPE_RAND_AIPATH	8	//!< Param1 type

//! 追加小物用の?造体
typedef struct
{
	char modelpath[_MAX_PATH];		//!< モデルデ??パス
	char texturepath[_MAX_PATH];	//!< テクス?ャパス
	int decide;						//!< 当たり判定の大きさ
	int hp;							//!< 耐久力
	char soundpath[_MAX_PATH];		//!< サウンドデ??パス
	int jump;						//!< 飛び具合
} addsmallobject;

//! @brief デ??を管理するクラス（基底クラス）
//! @details ゲ??のデ??を、フ?イルから読み込み処理するクラス群の基底クラスです。
class DataInterface
{
protected:
	int datas;		//!< デ??数

public:
	DataInterface();
	~DataInterface();
	virtual int LoadFiledata(const char *fname);
	virtual int GetTotaldatas();
	virtual int Getdata(void *out_data, int id);
};

//! @brief ブロックデ??を管理するクラス
//! @details ?ップデ??として使う、ブロックデ??フ?イルを管理します。
//! @details フ?イルの読み込みのほか、?ップへの影（各面の明るさ?現）も計算します。
class BlockDataInterface : public DataInterface
{
	blockdata *data;								//!< ブロックデ??を?す?造体
	char texture[TOTAL_BLOCKTEXTURE][_MAX_PATH];	//!< テクス?ャ名

public:
	BlockDataInterface();
	~BlockDataInterface();
	int LoadFiledata(const char *fname);
	void CalculationBlockdata(bool darkflag);
	int GetTexture(char *fname, int id);
	int Getdata(blockdata *out_data, int id);
};

bool blockdataface(int faceID, int* vID, int* uvID);

//! @brief ?イントデ??を管理するクラス
//! @details ?ッションデ??として使う、?イントデ??フ?イルを管理します。
//! @details フ?イルの読み込みのほか、?イントの検索??もあります。
class PointDataInterface : public DataInterface
{
	pointdata *data;							//!< ブロックデ??を?す?造体
	char *text[MAX_POINTMESSAGES];				//!< イベントメッセ?ジ

	int LoadMSGFiledata(char *fname);

public:
	PointDataInterface();
	~PointDataInterface();
	int LoadFiledata(const char *fname);
	int Getdata(pointdata *out_data, int id);
	int SetParam(int id, signed short int p1, signed short int p2, signed short int p3, signed short int p4);
	int GetMessageText(char *str, int id);
	int SearchPointdata(int* id, unsigned char pmask, signed short int p1, signed short int p2, signed short int p3, signed short int p4, int offset = 0);
	int SearchPointdata(pointdata *out_data, unsigned char pmask, signed short int p1, signed short int p2, signed short int p3, signed short int p4, int offset = 0);
};

//! @brief MIFを管理するクラス
//! @details MIF（MISSION INFORMATION FILE）を管理します。
//! @details 標??ッションのブリ?フィングフ?イル（.txt）にも対応しています。
class MIFInterface : public DataInterface
{
	bool mif;								//!< フ?イル?式が .mif
	char mission_name[24];					//!< ?ッション識別名
	char mission_fullname[64];				//!< ?ッション正式名称
	char blockfile_path[_MAX_PATH];			//!< ブロックデ??フ?イル
	char pointfile_path[_MAX_PATH];			//!< ?イントデ??フ?イル
	int skynumber;							//!< 背景空のID?（なし：0）
	char picturefileA_path[_MAX_PATH];		//!< ブリ?フィング画像A
	char picturefileB_path[_MAX_PATH];		//!< ブリ?フィング画像B（追加分）
	char addsmallobject_path[_MAX_PATH];	//!< 追加小物情報フ?イルへのパス
	char briefingtext[816];					//!< ブリ?フィング文章・?文
	bool collision;					//!< 当たり判定を多めに行う
	bool screen;					//!< 画面を暗めにする
	addsmallobject *AddSmallObjectData;		//!< 追加小物の情報

	int LoadDefaultTextFiledata(const char *fname);
	int LoadMissionInfoFiledata(const char *fname);
	int LoadAddSmallObjectFiledata(const char *fname);
	bool ChangeExePathToFullPath(char *dir, char *fname);

public:
	MIFInterface();
	~MIFInterface();
	int LoadFiledata(const char *fname, bool addfile);
	bool GetFiletype();
	char* GetMissionName();
	char* GetMissionFullname();
	void GetDatafilePath(char *blockfile, char *pointfile);
	int GetSkynumber();
	void GetPicturefilePath(char *picturefileA, char *picturefileB);
	char* GetBriefingText();
	bool GetCollisionFlag();
	bool GetScreenFlag();
	char* GetAddSmallobjectFile();
	char* GetAddSmallobjectModelPath(int id);
	char* GetAddSmallobjectTexturePath(int id);
	int GetAddSmallobjectDecide(int id);
	int GetAddSmallobjectHP(int id);
	char* GetAddSmallobjectSoundPath(int id);
	int GetAddSmallobjectJump(int id);
};

//! @brief ADDONリストを管理するクラス
//! @details 特定のディレクトリに入った.mifをADDONリストとして管理します。
class AddonList : public DataInterface
{
	char filename[MAX_ADDONLIST][_MAX_PATH];	//!< .mifフ?イル名
	char mission_name[MAX_ADDONLIST][24];		//!< ?ッション識別名

	void GetMIFlist(const char *dir);
	void GetMissionName(const char *dir);
	void Sort();

public:
	AddonList();
	~AddonList();
	int LoadFiledata(const char *dir);
	char *GetMissionName(int id);
	char *GetFileName(int id);
};

//! @brief INIフ?イルを管理するクラス
//! @details INIフ?イルを管理します。
class INIFileInterface
{
	FILE *inifp;	//!< フ?イル?イン?

public:
	INIFileInterface();
	~INIFileInterface();
	bool LoadINIFile(const char *fname);
	int GetINIFileString(const char *sectionname, const char *keyname, const char *defaultvalue, char *valuestr, int strbuflen);
	int GetINIFileInt(const char *sectionname, const char *keyname, int defaultvalue, int *errorcode);
	float GetINIFileFloat(const char *sectionname, const char *keyname, float defaultvalue, int *errorcode);
	bool GetINIFileBool(const char *sectionname, const char *keyname, bool defaultvalue, int *errorcode);
	void ReleaseINIFile();
};

int DeleteLinefeed(char *str);
bool CheckFullPath(const char *path);
void GetFileDirectory(const char *path, char *dir);
bool CheckFileExtension(const char *filepath, const char *checkstr);
bool CheckDirectory(const char *dir);

#endif