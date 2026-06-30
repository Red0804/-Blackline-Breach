//! @file ai.h
//! @brief AIcontrolÉNÉâÉXÇÃêÈåæ

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

#ifndef AI_H
#define AI_H

#define AI_TOTALHUMAN_SCALE (MAX_HUMAN/24)		//!< 24êlÇ†ÇΩÇËÇÃç≈ëÂêlêîÇÃ?ó¶

#define AI_ADDTURNRAD DegreeToRadian(0.8f)	//!< AIÇÃê˘âÒÅiâÒ?Åj?óÕ
#define AI_SEARCH_RX DegreeToRadian(110)	//!< ìGÇî≠å©Ç∑ÇÈâ°é≤äpìx
#define AI_SEARCH_RY DegreeToRadian(60)		//!< ìGÇî≠å©Ç∑ÇÈècé≤äpìx
#define AI_ZOMBIEATTACK_ARMRY DegreeToRadian(-15)		//!< ?ÉìÉrçUåÇéûÇÃòrÇÃäpìx

#define AI_ARRIVALDIST_PATH 5.0f			//!< ÉpÉXÇ…ìûíBÇµÇΩÇ∆îªíËÇ∑ÇÈãóó£
#define AI_ARRIVALDIST_TRACKING 18.0f		//!< í«îˆëŒè€Ç…ìûíBÇµÇΩÇ∆îªíËÇ∑ÇÈãóó£
#define AI_ARRIVALDIST_WALKTRACKING 24.0f	//!< í«îˆëŒè€Ç÷ÅiëñÇÁÇ∏Ç…Åjï‡Ç¢ÇƒãﬂÇ√Ç≠ãóó£
#define AI_CHECKJUMP_HEIGHT 0.3f			//!< ëOÇ…ÉuÉçÉbÉNÇ™ñ≥Ç¢Ç©îªíËÇ∑ÇÈçÇÇ≥
#define AI_CHECKJUMP_DIST 2.0f				//!< ëOÇ…ÉuÉçÉbÉNÇ™ñ≥Ç¢Ç©îªíËÇ∑ÇÈãóó£
#define AI_CHECKJUMP2_DIST 10.0f			//!< ëOÇ…ÉuÉçÉbÉNÇ™ñ≥Ç¢Ç©îªíËÇ∑ÇÈãóó£ÅióDêÊìIÇ»ëñÇËÅj
#define AI_CHECKBULLET 20.0f				//!< íeÇ™ãﬂÇ≠Çí Ç¡ÇΩÇ∆îªíËÇ∑ÇÈãóó£
#define AI_CHECKSHORTATTACK_DIST 200.0f		//!< ãﬂãóó£çUåÇÇ∆îªíËÇ∑ÇÈãóó£

#ifndef H_LAYERLEVEL
 #define H_LAYERLEVEL 3		//!< Select include file.
#endif
#include "main.h"

//! @brief AIêßå‰ÇçsÇ§ÉNÉâÉX
//! @details ÉpÉXÇ…ÇÊÇÈà⁄ìÆÅAéãñÏÇ…Ç¢ÇÈìGÇ÷ÇÃîFéØÇ‚çUåÇÅAïêäÌÇÃégÇ¢ï™ÇØ?Ç»Ç«ÇÃAIÇÃêßå‰ëSî ÇçsÇ¢Ç‹Ç∑ÅB
class AIcontrol
{
	class ObjectManager *ObjMgr;		//!< ObjectManagerÉNÉâÉXÇÃ?ÉCÉì?
	class BlockDataInterface *blocks;	//!< ÉuÉçÉbÉNÉf??Çä«óùÇ∑ÇÈÉNÉâÉXÇ÷ÇÃ?ÉCÉì?
	class ParameterInfo *Param;		//!< ê›íËílÇä«óùÇ∑ÇÈÉNÉâÉXÇ÷ÇÃ?ÉCÉì?
	class Collision *CollD;				//!< ìñÇΩÇËîªíËÇä«óùÇ∑ÇÈÉNÉâÉXÇ÷ÇÃ?ÉCÉì?
	class SoundManager *GameSound;		//!< ÉQ??å¯â âπçƒê∂ÉNÉâÉXÇ÷ÇÃ?ÉCÉì?

	class AIMoveNavi *MoveNavi;			//!< êlà⁄ìÆä«óùÉNÉâÉXÇÃ?ÉCÉì?
	class AIObjectDriver *ObjDriver;	//!< êlà⁄ìÆâÒ?êßå‰ÉNÉâÉXÇÃ?ÉCÉì?

	int AIlevel;		//!< AIÉåÉxÉã
	int battlemode;		//!< êÌì¨ÉÇ?Éh
	bool NoFight;		//!< îÒêÌì¨âªÉtÉâÉO
	bool ForceNoAI;
	float posx;			//!< Xç¿ïW
	float posy;			//!< Yç¿ïW
	float posz;			//!< Zç¿ïW
	float rx;			//!< Xé≤âÒ?äpìx
	float ry;			//!< Yé≤âÒ?äpìx
	float cautionback_posx;		//!< åxâ˙å„Ç…ñﬂÇÈXç¿ïW
	float cautionback_posz;		//!< åxâ˙å„Ç…ñﬂÇÈZç¿ïW
	bool FaceCaution_flag;		//!< åxâ˙íÜÇ…ì¡íËÇÃïÒçêÇå¸Ç≠ÉtÉâÉO
	float FaceCaution_rx;		//!< åxâ˙íÜÇ…å¸Ç≠ï˚å¸
	float total_move;			//!< çáåvà⁄ìÆó 
	int waitcnt;				//!< éûä‘ë“ÇøÉJÉEÉìÉg
	int movejumpcnt;			//!< ÉWÉÉÉìÉvîªíËÉJÉEÉìÉg
	int gotocnt;				//!< à⁄ìÆÉJÉEÉìÉg
	int cautioncnt;
	int actioncnt;
	bool longattack;
	bool EventWeaponShot;

	// AI æ…±‚ ¡¶æÓ
	int crouchcnt;

	AIParameter* LevelParam;

	bool CheckTargetPos(bool back);
	void MoveTarget(bool back);
	void MoveTarget2(bool back);
	void MoveRandom();
	void TurnSeen();
	bool StopSeen();
	bool MoveJump();
	void Action();
	bool ActionCancel();
	int HaveWeapon();
	void CancelMoveTurn();
	int ControlWeapon();
	void ControlCrouch();
	int ThrowGrenade();
	void ArmAngle();
	int SearchEnemy();
	int SearchShortEnemy();
	bool CheckLookEnemy(int id, float search_rx, float search_ry, float maxDist, float *out_minDist);
	bool CheckLookEnemy(class human* thuman, float search_rx, float search_ry, float maxDist, float *out_minDist);
	bool CheckCorpse(int id);
	void MovePath();
	bool ActionMain();
	bool CautionMain();
	bool NormalMain();

	int ctrlid;					//!< é©ï™é©êgÅiêßå‰ëŒè€ÅjÇÃêlî‘çÜ
	class human *ctrlhuman;		//!< é©ï™é©êgÅiêßå‰ëŒè€ÅjÇÃhumanÉNÉâÉX
	class human *enemyhuman;	//!< çUåÇëŒè€ÇÃhumanÉNÉâÉX

public:
	AIcontrol(class ObjectManager *in_ObjMgr = NULL, int in_ctrlid = -1, class BlockDataInterface *in_blocks = NULL, class PointDataInterface *in_Points = NULL, class ParameterInfo *in_Param = NULL, class Collision *in_CollD = NULL, class SoundManager *in_GameSound = NULL);
	~AIcontrol();
	void SetClass(class ObjectManager *in_ObjMgr, int in_ctrlid, class BlockDataInterface *in_blocks, class PointDataInterface *in_Points, class ParameterInfo *in_Param, class Collision *in_CollD, class SoundManager *in_GameSound);
	void Init();
	void SetAIlevel(int level);
	int GetAIlevel();

	void SetForceNoAI(bool flag);
	bool GetForceNoAI();
	void SetHoldWait(float px, float pz, float rx);
	void SetHoldTracking(int id);
	void SetCautionMode();
	void SetNoFightFlag(bool flag);
	void GetBattleMode(int *mode, char *modestr);
	int GetEnemyHumanID();
	void GetMoveTargetPos(float *posx, float *posz, int *movemode);
	void GetPathPointData(pointdata *out_data);
	void Process();
};

//! @brief êlà⁄ìÆä«óùÉNÉâÉX
//! @details í èÌÉÇ?ÉhÇ…Ç®Ç¢ÇƒÅAêlÇÃà⁄ìÆêÊÇåàíËÇ∑ÇÈÉNÉâÉXÇ≈Ç∑ÅBAIcontrolÉNÉâÉXì‡Ç≈égópÇµÇ‹Ç∑ÅB
class AIMoveNavi
{
	class ObjectManager *ObjMgr;		//!< ObjectManagerÉNÉâÉXÇÃ?ÉCÉì?
	int ctrlid;							//!< é©ï™é©êgÅiêßå‰ëŒè€ÅjÇÃêlî‘çÜ
	class PointDataInterface *Points;	//!< ?ÉCÉìÉgÉf??Çä«óùÇ∑ÇÈÉNÉâÉXÇ÷ÇÃ?ÉCÉì?

	int movemode;			//!< à⁄ìÆÉÇ?Éh
	bool hold;				//!< à⁄ìÆÉpÉXÇì«Ç‹Ç»Ç¢
	int path_pointid;		//!< ÉpÉXÇÃ?ÉCÉìÉgÉf??î‘çÜ
	int target_humanid;		//!< ??ÉQÉbÉgÇ…Ç∑ÇÈêlÇÃÉf??î‘çÜ
	float target_posx;		//!< ??ÉQÉbÉgÇÃXç¿ïW
	float target_posz;		//!< ??ÉQÉbÉgÇÃZç¿ïW
	float target_rx;		//!< ??ÉQÉbÉgÇÃêÖïΩäpìx

public:
	AIMoveNavi(class ObjectManager *in_ObjMgr = NULL, int in_ctrlid = -1, class PointDataInterface *in_Points = NULL);
	~AIMoveNavi();
	void SetClass(class ObjectManager *in_ObjMgr, int in_ctrlid, class PointDataInterface *in_Points);
	void Init();
	bool MovePathNowState();
	bool MovePathNextState();
	void SetHoldWait(float px, float pz, float rx);
	void SetHoldTracking(int id);
	int GetMoveMode();
	bool GetRun2();
	int GetTargetHumanID();
	void GetPathPointData(pointdata *out_data);
	void GetTargetPos(float *posx, float *posz, float *rx, int *out_movemode, int *out_pointmode);
};

//! @brief êlà⁄ìÆâÒ?êßå‰ÉNÉâÉX
//! @details êlÇÃà⁄ìÆêßå‰Ç∆âÒ?êßå‰ÇÉtÉâÉOä«óùÇ∑ÇÈÉNÉâÉXÇ≈Ç∑ÅBAIcontrolÉNÉâÉXì‡Ç≈égópÇµÇ‹Ç∑ÅB
class AIObjectDriver
{
	class ObjectManager *ObjMgr;		//!< ObjectManagerÉNÉâÉXÇÃ?ÉCÉì?
	int ctrlid;				//!< é©ï™é©êgÅiêßå‰ëŒè€ÅjÇÃêlî‘çÜ

	int moveturn_mode;		//!< à⁄ìÆï˚å¸Ç‚âÒ?ï˚å¸ÇÃÉtÉâÉO
	float addrx;			//!< Xé≤âÒ?äpâ¡ë¨ìx
	float addry;			//!< Yé≤âÒ?äpâ¡ë¨ìx

public:
	AIObjectDriver(class ObjectManager *in_ObjMgr = NULL, int in_ctrlid = -1);
	~AIObjectDriver();
	void SetClass(class ObjectManager *in_ObjMgr, int in_ctrlid);
	void Init();
	void ResetMode();
	void SetModeFlag(int flag);
	void DelModeFlag(int flag);
	bool GetModeFlag(int flag);
	void ControlObject();
};

//! AIÇÃêßå‰ÉÇ?ÉhÇ?Ç∑íËêî
//! @warning íËêîÇïœçXÇ∑ÇÈèÍçáÅAïKóvÇ…âûÇ∂ÇƒGetBattleMode()ä÷êîÇÃï∂éöóÒèoóÕèàóùÇ‡ïœçXÇ∑ÇÈÇ±Ç∆ÅB
enum AImode
{
	AI_DEAD = 0,	//!< ?ñSÇµÇƒÇ¢ÇÈêl
	AI_ACTION,		//!< êÌì¨íÜÇÃêl
	AI_CAUTION,		//!< åxâ˙íÜÇÃêl
	AI_NORMAL,		//!< í èÌÇÃÉÇ?Éh

	AI_WALK,		//!< à⁄ìÆÉpÉXÇ…ÇÊÇ¡Çƒï‡Ç¢ÇƒÇ¢ÇÈêl
	AI_RUN,			//!< à⁄ìÆÉpÉXÇ…ÇÊÇ¡ÇƒëñÇ¡ÇƒÇ¢ÇÈêl
	AI_WAIT,		//!< ÉpÉXÇ…ÇÊÇ¡Çƒë“?ÇµÇƒÇ¢ÇÈêl
	AI_STOP_5SEC,	//!< ÉpÉXÇ…ÇÊÇ¡Çƒéûä‘ë“ÇøÇÇµÇƒÇ¢ÇÈêlÅi5ïbÅj
	AI_TRACKING,	//!< ì¡íËÇÃêlÇí«îˆÇ∑ÇÈ
	AI_GRENADE,		//!< éËû÷íeÇìäÇ∞ÇÈ
	AI_RUN2,		//!< óDêÊìIÇ»ëñÇË
	AI_RANDOM,		//!< ÉâÉì??ÉpÉXèàóùíÜ
	AI_NULL			//!< ÉpÉXÇ»Çµ
};

//! AIÇÃà⁄ìÆÉÇ?ÉhÇ?Ç∑íËêî
enum AIMoveNaviFlag
{
	AI_NAVI_MOVE_NULL,
	AI_NAVI_MOVE_WALK,
	AI_NAVI_MOVE_RUN,
	AI_NAVI_MOVE_RUN2,
	AI_NAVI_MOVE_TRACKING,

	AI_NAVI_POINT_NULL,
	AI_NAVI_POINT_WAIT,
	AI_NAVI_POINT_STOP_5SEC,
	AI_NAVI_POINT_TRACKING,
	AI_NAVI_POINT_GRENADE
};

//! AIÇÃ?çÏÉÇ?ÉhÇ?Ç∑íËêî
enum AIcontrolFlag
{
	AI_CTRL_MOVEFORWARD = 0x0001,
	AI_CTRL_MOVEBACKWARD = 0x0002,
	AI_CTRL_MOVELEFT = 0x0004,
	AI_CTRL_MOVERIGHT = 0x0008,
	AI_CTRL_MOVEWALK = 0x0010,
	AI_CTRL_TURNUP = 0x0100,
	AI_CTRL_TURNDOWN = 0x0200,
	AI_CTRL_TURNLEFT = 0x0400,
	AI_CTRL_TURNRIGHT = 0x0800
};

#endif