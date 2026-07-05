//! @file objectmanager.h
//! @brief ObjectManager긏깋긚궻먬뙻

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

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#define MAX_HUMAN 120			//!< 띍묈릐릶?걓뢣뾴걕갌AI깒긹깑뮧맢궻듫똚뤵갻24궻?릶갽궸궥귡궞궴갏
#define MAX_WEAPON 200			//!< 븧딇궻띍묈릶
#define MAX_SMALLOBJECT 40		//!< 룷븿궻띍묈릶

#define MAX_BULLET 128			//!< 뢤뭙궻띍묈릶
#define MAX_GRENADE 32			//!< 롨왮뭙궻띍묈릶
#define MAX_EFFECT 256			//!< 긄긲긃긏긣궻띍묈릶

#define WEAPONSHOT_HEIGHT (VIEW_HEIGHT)		//!< 뭙귩뵯롅궥귡뛼궠

#define TOTAL_WEAPON_AUTOBULLET 3			//!< 룊딖돸렄궸렔벍밒궸뺚궲귪궠귢귡뭙릶걁몧뭙릶궻돺?궔걂

#define HUMAN_PICKUPWEAPON_L -2.0f			//!< 븧딇귩뢉궎뛼궠걁돷걂
#define HUMAN_PICKUPWEAPON_H 16.0f			//!< 븧딇귩뢉궎뛼궠걁뤵걂
#define HUMAN_PICKUPWEAPON_R 3.8f			//!< 븧딇귩뢉궎떁뿣

#define HUMAN_HUMANCOLLISION_R 2.5f			//!< 릐벏럐궻뱰궫귟뵽믦궻뵾똞

#define HUMAN_BULLETCOLLISION_HEAD_H 2.9f	//!< 벆궻뱰궫귟뵽믦궻뛼궠
#define HUMAN_BULLETCOLLISION_HEAD_R 1.5f	//!< 벆궻뱰궫귟뵽믦궻뵾똞
#define HUMAN_BULLETCOLLISION_UP_H 7.0f		//!< 뤵뵾릆궻뱰궫귟뵽믦궻뛼궠
#define HUMAN_BULLETCOLLISION_UP_R 2.4f		//!< 뤵뵾릆궻뱰궫귟뵽믦궻뵾똞
#define HUMAN_BULLETCOLLISION_LEG_H 10.5f	//!< 뫉궻뱰궫귟뵽믦궻뛼궠
#define HUMAN_BULLETCOLLISION_LEG_R 2.6f	//!< 뫉궻뱰궫귟뵽믦궻뵾똞

#define SMALLOBJECT_COLLISIONSCALE 0.13f	//!< 룷븿뱰궫귟뵽믦궻?뿦

#define MAX_OBJECTMANAGER_LOGLEN 3						//!< 긆긳긙긃긏긣깓긐궻띍묈뛱릶
#define MAX_OBJECTMANAGER_LOGLINES 64					//!< 긆긳긙긃긏긣깓긐궻띍묈빒럻릶
#define MAX_OBJECTMANAGER_LOGCNT (int)(5.0f*GAMEFPS)	//!< 긆긳긙긃긏긣깓긐궻?렑긇긂깛긣릶

//#define HUMAN_DAMAGE_MULT_PLAYER_GET 0.05f 	//!< Damage multiplier of Player received
//#define HUMAN_DAMAGE_MULT_AI_GET 1.0f 		//!< Damage multiplier of AI received

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 3		//!< Select include file.
#endif
#include "main.h"

//! 긄긲긃긏긣?됪똶럁뾭?몾뫬
typedef struct
{
	int id;
	float dist;
	bool state_override;
	float x;
	float y;
	float z;
	float rotation;
	float size;
	float alpha;
} effectdata;

//! @brief 긆긳긙긃긏긣듖뿚긏깋긚
//! @details 둫긆긳긙긃긏긣궻룊딖돸갋똶럁갋?됪궶궵귩뛱궋듖뿚궢귏궥갃
class ObjectManager
{
	class human *HumanIndex;					//!< 릐긆긳긙긃긏긣
	class weapon *WeaponIndex;					//!< 븧딇긆긳긙긃긏긣
	class smallobject *SmallObjectIndex;		//!< 룷븿긆긳긙긃긏긣
	class bullet *BulletIndex;					//!< 뭙긆긳긙긃긏긣
	class grenade *GrenadeIndex;				//!< 롨왮뭙긆긳긙긃긏긣
	class effect *EffectIndex;					//!< 긄긲긃긏긣긆긳긙긃긏긣

	unsigned int framecnt;

	// 맵 전환 시 초기화되어야 하는 런타임 캐시
	int RuntimeLandingCooldown[MAX_HUMAN];
	int RuntimeFallingFrames[MAX_HUMAN];
	int RuntimeLastTexture[MAX_HUMAN];
	float RuntimeLastY[MAX_HUMAN];
	int RuntimeGroundGrace[MAX_HUMAN];
	int RuntimeFootstepTimer[MAX_HUMAN];	// Per-human footstep cadence; survives brief ground-detection gaps.

	// 고정 로직 틱 사이에서 상대 인물의 월드 위치만 보간한다.
	// 팔/다리 애니메이션과 실제 충돌/사격 좌표는 변경하지 않는다.
	float Human_RenderInterpolationAlpha;
	bool Human_RenderPreviousValid[MAX_HUMAN];
	float Human_RenderPreviousX[MAX_HUMAN];
	float Human_RenderPreviousY[MAX_HUMAN];
	float Human_RenderPreviousZ[MAX_HUMAN];
	float Human_RenderPreviousRX[MAX_HUMAN];
	bool Human_RenderPreviousCrouch[MAX_HUMAN];
	bool Human_RenderPreviousDead[MAX_HUMAN];
	bool Human_RenderPreviousDeadMotion[MAX_HUMAN];
	int Human_RenderPreviousParamID[MAX_HUMAN];
	int Human_RenderPreviousDataID[MAX_HUMAN];
	signed short int Human_RenderPreviousP4[MAX_HUMAN];
	int Human_RenderPreviousTeam[MAX_HUMAN];
	int Human_RenderPreviousGeneration[MAX_HUMAN];

	// Render-only interpolation for dropped world weapons.
	float Weapon_RenderInterpolationAlpha;
	bool Weapon_RenderPreviousValid[MAX_WEAPON];
	float Weapon_RenderPreviousX[MAX_WEAPON];
	float Weapon_RenderPreviousY[MAX_WEAPON];
	float Weapon_RenderPreviousZ[MAX_WEAPON];
	float Weapon_RenderPreviousRX[MAX_WEAPON];
	int Weapon_RenderPreviousParamID[MAX_WEAPON];
	int Weapon_RenderPreviousGeneration[MAX_WEAPON];

	// Render-only interpolation for destroyed/flying small objects.
	float SmallObject_RenderInterpolationAlpha;
	bool SmallObject_RenderPreviousValid[MAX_SMALLOBJECT];
	float SmallObject_RenderPreviousX[MAX_SMALLOBJECT];
	float SmallObject_RenderPreviousY[MAX_SMALLOBJECT];
	float SmallObject_RenderPreviousZ[MAX_SMALLOBJECT];
	float SmallObject_RenderPreviousRX[MAX_SMALLOBJECT];
	float SmallObject_RenderPreviousRY[MAX_SMALLOBJECT];
	int SmallObject_RenderPreviousParamID[MAX_SMALLOBJECT];
	signed short int SmallObject_RenderPreviousP4[MAX_SMALLOBJECT];
	int SmallObject_RenderPreviousGeneration[MAX_SMALLOBJECT];

	// Grenade render-only interpolation state. Collision positions stay unchanged.
	float Grenade_RenderInterpolationAlpha;
	bool Grenade_RenderPreviousValid[MAX_GRENADE];
	float Grenade_RenderPreviousX[MAX_GRENADE];
	float Grenade_RenderPreviousY[MAX_GRENADE];
	float Grenade_RenderPreviousZ[MAX_GRENADE];
	float Grenade_RenderPreviousRX[MAX_GRENADE];
	float Grenade_RenderPreviousRY[MAX_GRENADE];
	int Grenade_RenderPreviousWeaponID[MAX_GRENADE];
	int Grenade_RenderPreviousTeam[MAX_GRENADE];
	int Grenade_RenderPreviousHuman[MAX_GRENADE];
	int Grenade_RenderPreviousLifeCount[MAX_GRENADE];

	// Render-only interpolation for shell casings, smoke, blood and moving particles.
	float Effect_RenderInterpolationAlpha;
	bool Effect_RenderPreviousValid[MAX_EFFECT];
	float Effect_RenderPreviousX[MAX_EFFECT];
	float Effect_RenderPreviousY[MAX_EFFECT];
	float Effect_RenderPreviousZ[MAX_EFFECT];
	float Effect_RenderPreviousRotation[MAX_EFFECT];
	float Effect_RenderPreviousSize[MAX_EFFECT];
	float Effect_RenderPreviousAlpha[MAX_EFFECT];
	int Effect_RenderPreviousGeneration[MAX_EFFECT];

	// 이펙트 슬롯 탐색 위치도 맵 전환 시 초기화
	int RuntimeEffectLastIndex;
	int RuntimeMapEffectLastIndex;

	void ResetRuntimeState();

	float *Human_ontarget;	//!< 뼺뭷릶
	int *Human_kill;		//!< ?궢궫밎궻릶
	int *Human_headshot;	//!< 밎궻벆븫궸뼺뭷궢궫릶
	bool *Human_ShotFlag;	//!< 뵯뺺긲깋긐걁?긛깑긲깋긞긘깄뾭걂
	int *Human_YakkyouCnt;	//!< 뵯뺺렄궻뽶娥긇긂깛긣
	class BulletObjectHumanIndex *BulletObj_HumanIndex;		//!< 뭙뫮릐뵽믦뾭긆긳긙긃긏긣
	int *Human_ActionSoundTimer;
	int *Human_ActionSoundID;
	int *Human_ShellReloadPrevState;
	int *Human_ShellReloadPrevLoad;
	int* Human_MinigunSpinSoundTimer;   // AI 미니건 회전음 반복 방지용

	// AI가 방패로 너무 오래 버티지 않도록,
	// 방패 방어 횟수/누적 방어 피해가 일정 이상이면 총으로 전환한다.
	int* Human_ShieldBlockSwitchCount;
	int* Human_ShieldBlockSwitchDamage;
	int* Human_ShieldBlockSwitchCooldown;

	bool* Human_ForceNoAI;

	bool AddCollisionFlag;	//!< 믁돿궻뱰궫귟뵽믦
	bool *FriendlyFire;		//!< FF걁벏럐뱼궭걂뾎뚼돸

	int Player_HumanID;		//!< 긵깒귽깂?궕?띿궥귡릐긆긳긙긃긏긣궻ID

	bool *Human_FrameTextureRefresh;	//!< 릐궻긡긏긚?긿깏긲깒긞긘깄긲깋긐걁긡긏긚?긿딼몾릐듩걁볞릐듩걂뾭걂

	ParameterInfo *GameParamInfo;		//!< 긒??궻먠믦뭠
	D3DGraphics *d3dg;					//!< ?됪긏깋긚
	ResourceManager *Resource;			//!< 깏??긚듖뿚
	BlockDataInterface *BlockData;		//!< 긳깓긞긏긢??듖뿚긏깋긚
	PointDataInterface *PointData;		//!< ?귽깛긣긢??듖뿚긏깋긚
	Collision *CollD;					//!< 뱰궫귟뵽믦듖뿚긏깋긚
	SoundManager *GameSound;			//!< 긒??뚼됈돶듖뿚긏깋긚
	MIFInterface *MIFdata;				//!< MIF긓깛긣깓?깑긏깋긚

	class ObjectManagerLog* ObjectLog;

	bool ShowHitboxFlag;

	// Debug weapon / muzzle tuning state.
	bool DebugMuzzleAdjustFlag;
	bool DebugBulletTraceFlag;
	bool DebugInfiniteAmmoFlag;
	bool DebugNoReloadFlag;
	bool DebugNoSpreadFlag;
	bool DebugNoRecoilFlag;
	float DebugMuzzleOffsetX[TOTAL_PARAMETERINFO_WEAPON];
	float DebugMuzzleOffsetY[TOTAL_PARAMETERINFO_WEAPON];
	float DebugMuzzleOffsetZ[TOTAL_PARAMETERINFO_WEAPON];

	bool DebugLastTraceValid;
	bool DebugLastMuzzleBlocked;
	float DebugLastSafeX;
	float DebugLastSafeY;
	float DebugLastSafeZ;
	float DebugLastMuzzleX;
	float DebugLastMuzzleY;
	float DebugLastMuzzleZ;
	float DebugLastAimOriginX;
	float DebugLastAimOriginY;
	float DebugLastAimOriginZ;
	float DebugLastAimTargetX;
	float DebugLastAimTargetY;
	float DebugLastAimTargetZ;
	float DebugLastActualStartX;
	float DebugLastActualStartY;
	float DebugLastActualStartZ;
	float DebugLastActualTargetX;
	float DebugLastActualTargetY;
	float DebugLastActualTargetZ;
	float DebugLastBlockHitX;
	float DebugLastBlockHitY;
	float DebugLastBlockHitZ;

	bool GetWeaponMuzzleWorldPos(int humanid, int weapon_paramid, float* x, float* y, float* z);
	bool CheckMuzzleObstruction(
		float safe_x,
		float safe_y,
		float safe_z,
		float muzzle_x,
		float muzzle_y,
		float muzzle_z,
		float* hit_x,
		float* hit_y,
		float* hit_z
	);
	bool ResolveMuzzleSafeOrigin(
		float preferred_x,
		float preferred_y,
		float preferred_z,
		float aim_dir_x,
		float aim_dir_y,
		float aim_dir_z,
		float* safe_x,
		float* safe_y,
		float* safe_z
	);
	void MaintainDebugAmmoState();
	void RenderDebugWeaponData();
	void RenderDebugPointCross(float x, float y, float z, float size, int color);
	bool GetCurrentPlayerWeaponObject(weapon** out_weapon, int* out_weapon_id);

	void RenderDebugHumanHitbox();
	void RenderDebugCylinder(float x, float y, float z, float r, float h, int color);
	void RenderDebugShieldBox(float x, float y, float z, float rx, int color, int front_color);
	bool GetHumanRenderPosition(int id, float* x, float* y, float* z);
	bool GetHumanRenderYawOffset(int id, float* yaw_offset);
	bool GetGrenadeRenderTransform(int id, float* x, float* y, float* z, float* rx, float* ry);
	bool GetWeaponRenderTransform(int id, float* x, float* y, float* z, float* rx);
	bool GetSmallObjectRenderTransform(int id, float* x, float* y, float* z, float* rx, float* ry);


	int AddHumanIndex(pointdata data, pointdata infodata, bool RandomFlag);
	int AddWeaponIndex(pointdata data);
	int AddSmallObjectIndex(pointdata data);
	void SetHumanBlood(float x, float y, float z, int damage, bool flowing);
	void SetShieldBlockBulletEffect(float x, float y, float z, int damage, float shield_forward_x, float shield_forward_z);
	bool CollideHuman(human *in_humanA, human *in_humanB);
	bool CollideBullet(bullet *in_bullet);
	void HitBulletMap(float x, float y, float z, int teamID);
	void HitBulletShieldOnly(int HitHuman_id, float x, float y, float z, int visual_damage, int Shothuman_id);
	void AddAIShieldBlockSwitchScore(int HitHuman_id, int shield_damage);
	void HitBulletHuman(int HitHuman_id, int Hit_id, float x, float y, float z, float brx, int attacks, int humanid, float ontargetcnt, bool shield_blocked = false, bool bullet_silencer = false);
	void HitBulletSmallObject(int HitSmallObject_id, float x, float y, float z, int attacks, int teamID);
	bool GrenadeExplosion(grenade* in_grenade, float range_scale = 1.0f);
	void DeadEffect(human *in_human);
	bool CollideBlood(effect *in_effect, int *id, int *face, float *pos_x, float *pos_y, float *pos_z);
	void PickupWeapon(human *in_human, weapon *in_weapon);
	void CleanupPointDataToObject();
	int SortEffect(float camera_x, float camera_y, float camera_z, effectdata data[]);
	void ShotWeaponEffect(int humanid);
	void ShotWeaponYakkyou(int humanid);

public:
	ObjectManager();
	~ObjectManager();
	void SetClass(ParameterInfo *in_GameParamInfo, D3DGraphics *in_d3dg, ResourceManager *in_Resource, BlockDataInterface *in_BlockData, PointDataInterface *in_PointData, Collision *in_CollD, SoundManager *in_GameSound, MIFInterface *in_MIFdata);
	int AddHumanIndex(float px, float py, float pz, float rx, int CharacterID, int TeamID, int WeaponID[], signed short int param4, bool RandomFlag);
	void ClearHumanFrameTextureRefresh(int id);
	void ApplyHumanUpperModelIndex(int human_id, int upper_model_index);
	int AddVisualWeaponIndex(int WeaponID, bool loadbullet);
	int AddSmallObjectIndex(float px, float py, float pz, float rx, int paramID, bool MapColl);
	int AddEffect(float pos_x, float pos_y, float pos_z, float move_x, float move_y, float move_z, float addmove_y, float rotation, float addrotation, float size, float addsize, int texture, int brightness, int addbrightness, float alpha, float addalpha, int settype, int count);
	int AddMapEffect(int id, int face, float pos_x, float pos_y, float pos_z, float size, float rotation, int texture, int brightness, int addbrightness, float alpha, float addalpha, int count);
	void LoadPointData();
	void Recovery();
	void SetAddCollisionFlag(bool flag);
	bool GetFriendlyFireFlag(int id);
	void SetFriendlyFireFlag(int id, bool flag);
	int GetPlayerID();
	void SetPlayerID(int id);
	human* GetHumanObject(int id);
	human* GetPlayerHumanObject();
	int GetHumanObjectID(human* object);
	bool GetHumanVisualRenderPosition(int id, float* x, float* y, float* z);
	bool GetHumanForceNoAI(int id);
	void SetHumanRenderInterpolationAlpha(float alpha);
	void CaptureHumanRenderInterpolationState();
	void ResetHumanRenderInterpolation();
	void SetGrenadeRenderInterpolationAlpha(float alpha);
	void CaptureGrenadeRenderInterpolationState();
	void ResetGrenadeRenderInterpolation();
	void SetEffectRenderInterpolationAlpha(float alpha);
	void CaptureEffectRenderInterpolationState();
	void ResetEffectRenderInterpolation();
	void SetWeaponRenderInterpolationAlpha(float alpha);
	void CaptureWeaponRenderInterpolationState();
	void ResetWeaponRenderInterpolation();
	void SetSmallObjectRenderInterpolationAlpha(float alpha);
	void CaptureSmallObjectRenderInterpolationState();
	void ResetSmallObjectRenderInterpolation();
	bool GetEffectRenderState(int id, float* x, float* y, float* z, float* rotation, float* size, float* alpha);
	weapon* GetWeaponObject(int id);
	smallobject* GetSmallObject(int id);
	bullet* GetBulletObject(int id);
	int GetBulletObjectID(bullet* object);
	bullet* GetNewBulletObject();
	grenade* GetNewGrenadeObject();
	human* SearchHuman(signed short int p4);
	smallobject* SearchSmallobject(signed short int p4);
	void GetTotalObjects(int *HumanCnt, int *WeaponCnt, int *SmallobjectCnt, int *BulletCnt, int *GrenadeCnt, int *EffectCnt);
	void MoveForward(int human_id);
	void MoveBack(int human_id);
	void MoveLeft(int human_id);
	void MoveRight(int human_id);
	void MoveWalk(int human_id);
	void MoveJump(int human_id);
	int ShotWeapon(int human_id);
	void PlayWeaponShotVisualEffect(int human_id, bool include_yakkyou);
	bool ReloadWeapon(int human_id);
	void ChangeHaveWeapon(int human_id, int id = -1);
	int ChangeWeaponID(int human_id);
	bool DumpWeapon(int human_id);
	void ChangeScopeMode(int human_id);
	bool CheatAddBullet(int human_id);
	bool CheatNewWeapon(int human_id, int new_weaponID);
	bool CheckZombieAttack(human* MyHuman, human* EnemyHuman);
	bool IsZombieAttackBlockedByShield(human* MyHuman, human* EnemyHuman, bool play_effect);
	bool CheckShieldHookBlockSegment(
		human* hitHuman,
		human* hookHuman,
		float start_x,
		float start_y,
		float start_z,
		float end_x,
		float end_y,
		float end_z,
		bool play_effect
	);
	void HitZombieAttack(human* MyHuman, human* EnemyHuman);

	// 플레이어 스킬용 피해 처리
// private HitBulletHuman()을 외부에서 안전하게 사용하기 위한 public 래퍼
	void SkillDamageHuman(int HitHuman_id, int Hit_id, float x, float y, float z, float brx, int attacks, int humanid, float ontargetcnt);

	// 플레이어 스킬용 지뢰 폭발 처리
	// 기존 수류탄 폭발 피해/이펙트/폭발음을 재사용하기 위한 public 래퍼
	bool ExplodeSkillMine(float x, float y, float z, int teamid, int humanid, int weapon_paramid, float range_scale = 1.0f);

	bool HumanResuscitation(int id);
	int CheckGameOverorComplete();
	bool GetObjectInfoTag(float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, int *color, char *infostr);
	int Process(int cmdF5id, bool demomode, bool screen);
	bool GetHumanShotInfo(int id, float *ontarget, int *kill, int *headshot);
	void Render(float camera_x, float camera_y, float camera_z, float camera_rx, float camera_ry, int HidePlayer, bool NoModel);
	void RenderLog(int x, int y);
	void SetShowHitboxFlag(bool flag);
	bool GetShowHitboxFlag();

	void SetDebugMuzzleAdjustFlag(bool flag);
	bool GetDebugMuzzleAdjustFlag();
	bool AdjustDebugMuzzle(int axis, float amount);
	bool ResetDebugMuzzle();
	bool GetDebugCurrentWeaponInfo(
		int* weapon_id,
		float* flash_x,
		float* flash_y,
		float* flash_z,
		float* world_x,
		float* world_y,
		float* world_z,
		bool* blocked
	);
	void SetDebugBulletTraceFlag(bool flag);
	bool GetDebugBulletTraceFlag();
	void SetDebugInfiniteAmmoFlag(bool flag);
	bool GetDebugInfiniteAmmoFlag();
	void SetDebugNoReloadFlag(bool flag);
	bool GetDebugNoReloadFlag();
	void SetDebugNoSpreadFlag(bool flag);
	bool GetDebugNoSpreadFlag();
	void SetDebugNoRecoilFlag(bool flag);
	bool GetDebugNoRecoilFlag();
	bool FillDebugCurrentMagazine();
	// [추가됨] 무기 무작위 변경 함수 선언
	void RandomizeMapWeaponsByCustomGroup(class ParameterInfo* Param, class ResourceManager* Resource);
	void Cleanup();

};

//! @brief 뭙뫮릐뵽믦뾭긏깋긚
//! @details 뭙궴릐궻뱰궫귟뵽믦궻뾎뼰귩듖뿚궥귡긏깋긚궳궥갃ObjectManager긏깋긚볙궳럊뾭궢귏궥갃
class BulletObjectHumanIndex
{
	bool *HumanIndex;		//!< 깏긚긣

public:
	BulletObjectHumanIndex();
	~BulletObjectHumanIndex();
	void Init();
	bool GetIndexFlag(int id);
	void SetIndexFlag(int id);
};

//! @brief 긆긳긙긃긏긣깓긐긏깋긚
//! @details 긆긳긙긃긏긣듖뿚긏깋긚 ObjectManager 궻깓긐귩딯?갋?렑궢귏궥갃
class ObjectManagerLog
{
	D3DGraphics *d3dg;							//!< ?됪긏깋긚

	int TextCnt[MAX_OBJECTMANAGER_LOGLEN];		//!< 긲깒??릶
	char *TextStr[MAX_OBJECTMANAGER_LOGLEN];	//!< 빒럻쀱
	int TextColor[MAX_OBJECTMANAGER_LOGLEN];	//!< 빒럻쀱궻륡

	bool AddTextLog(int cnt, const char *str, int color);

public:
	ObjectManagerLog();
	~ObjectManagerLog();
	void SetClass(D3DGraphics *in_d3dg);
	void ClearLog();
	void InfoLog(const char *str);
	void AddHuman(int humanID, int TeamID, int PlayerTeamID);
	void DiedHuman(int ShothumanID, int HitHumanID, int ShothumanTeamID, int HitHumanTeamID, int PlayerTeamID);
	void ReviveHuman(int humanID, int TeamID, int PlayerTeamID);
	void AddSmallObject(int objID);
	void BreakSmallObject(int objID);
	void Process();
	void Render(int x, int y);
};

#endif