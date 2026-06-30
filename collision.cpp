//! @file collision.cpp
//! @brief Collisionクラスの定?

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

#include "collision.h"

//! @brief コンストラク?
Collision::Collision()
{
	BlockDataIF = NULL;
	cbdata = new Coll_Blockdata[MAX_BLOCKS];
	for(int i=0; i<MAX_BLOCKS; i++){
		cbdata[i].min_x = 0.0f;
		cbdata[i].min_y = 0.0f;
		cbdata[i].min_z = 0.0f;
		cbdata[i].max_x = 0.0f;
		cbdata[i].max_y = 0.0f;
		cbdata[i].max_z = 0.0f;
		cbdata[i].BoardBlock = false;
		cbdata[i].worldgroup = 0;
	}
}

//! @brief ディストラク?
Collision::~Collision()
{
	if( cbdata != NULL ){ delete [] cbdata; }
}

//! @brief ブロックデ??を取り込む
//! @param in_BlockDataIF ブロックデ??
int Collision::InitCollision(BlockDataInterface* in_BlockDataIF)
{
	int bs;
	blockdata data;
	int vID[4];

	if( in_BlockDataIF == NULL ){ return 1; }
	BlockDataIF = in_BlockDataIF;

	bs = BlockDataIF->GetTotaldatas();

	for(int i=0; i<bs; i++){
		cbdata[i].BoardBlock = false;
	}

	for(int i=0; i<bs; i++){
		float cx = 0.0f;
		float cy = 0.0f;
		float cz = 0.0f;
		float x, y, z, rx, ry, r;

		BlockDataIF->Getdata(&data, i);

		//中心を求める
		for(int j=0; j<8; j++){
			cx += data.x[j];
			cy += data.y[j];
			cz += data.z[j];
		}
		cx /= 8;
		cy /= 8;
		cz /= 8;

		//各頂?の座標を再計算
		for(int j=0; j<8; j++){
			//中心からの位置関係を算出
			x = data.x[j] - cx;
			y = data.y[j] - cy;
			z = data.z[j] - cz;
			rx = atan2f(z, x);
			ry = atan2f(y, sqrtf(x*x + z*z));
			r = sqrtf(x*x + y*y + z*z);

			//大きくする
			r += COLLISION_ADDSIZE;

			//座標を再計算
			cbdata[i].x[j] = cx + cosf(rx) * cosf(ry) * r;
			cbdata[i].y[j] = cy + sinf(ry) * r;
			cbdata[i].z[j] = cz + sinf(rx) * cosf(ry) * r;
		}
	}

	//判定用平面の中心?算出
	for(int i=0; i<bs; i++){
		for(int j=0; j<6; j++){
			blockdataface(j, &vID[0], NULL);

			cbdata[i].polygon_center_x[j] = 0.0f;
			cbdata[i].polygon_center_y[j] = 0.0f;
			cbdata[i].polygon_center_z[j] = 0.0f;

			//4頂?の中心?を算出
			for(int k=0; k<4; k++){
				cbdata[i].polygon_center_x[j] += cbdata[i].x[ vID[k] ];
				cbdata[i].polygon_center_y[j] += cbdata[i].y[ vID[k] ];
				cbdata[i].polygon_center_z[j] += cbdata[i].z[ vID[k] ];
			}
			cbdata[i].polygon_center_x[j] /= 4;
			cbdata[i].polygon_center_y[j] /= 4;
			cbdata[i].polygon_center_z[j] /= 4;
		}
	}

	//板状のブロックを検出
	for(int i=0; i<bs; i++){
		cbdata[i].BoardBlock = false;

		for(int j=0; j<8; j++){
			for(int k=j+1; k<8; k++){
				//一ヵ所でも頂?が同じなら、板状になっていると判定。
				if( (cbdata[i].x[j] == cbdata[i].x[k])&&(cbdata[i].y[j] == cbdata[i].y[k])&&(cbdata[i].z[j] == cbdata[i].z[k]) ){
					cbdata[i].BoardBlock = true;
					j = 8; k = 8;	//break
				}
			}
		}

		//ブロックの中心を算出
		float mx = 0.0f;
		float my = 0.0f;
		float mz = 0.0f;
		for(int j=0; j<8; j++){
			mx += cbdata[i].x[j];
			my += cbdata[i].y[j];
			mz += cbdata[i].z[j];
		}
		mx /= 8;
		my /= 8;
		mz /= 8;

		//ブロックの中心?に対して1面でも?面ならば、板状になっていると判定。
		for(int j=0; j<6; j++){
			if( CheckPolygonFront(i, j, mx, my, mz) == true ){
				cbdata[i].BoardBlock = true;
				break;
			}
		}
	}


	//ブロックAABB作成
	for(int i=0; i<bs; i++){
		GetBlockPosMINMAX(i, &cbdata[i].min_x, &cbdata[i].min_y, &cbdata[i].min_z, &cbdata[i].max_x, &cbdata[i].max_y, &cbdata[i].max_z);
	}

	//ブロックの空間分割グル?プを計算
	for(int i=0; i<bs; i++){
		cbdata[i].worldgroup = GetWorldGroup(cbdata[i].min_x, cbdata[i].min_z);
		if( GetWorldGroup(cbdata[i].max_x, cbdata[i].max_z) != cbdata[i].worldgroup ){
			cbdata[i].worldgroup = 0;
		}
	}

	return 0;
}

//! @brief ブロックの座標最大値・最小値を返す
//! @param id 判定するブロック番号
//! @param *min_x 最小 X座標を返す?イン?
//! @param *min_y 最小 Y座標を返す?イン?
//! @param *min_z 最小 Z座標を返す?イン?
//! @param *max_x 最大 X座標を返す?イン?
//! @param *max_y 最大 Y座標を返す?イン?
//! @param *max_z 最大 Z座標を返す?イン?
void Collision::GetBlockPosMINMAX(int id, float *min_x, float *min_y, float *min_z, float *max_x, float *max_y, float *max_z)
{
	*min_x = cbdata[id].x[0];
	*min_y = cbdata[id].y[0];
	*min_z = cbdata[id].z[0];
	*max_x = cbdata[id].x[0];
	*max_y = cbdata[id].y[0];
	*max_z = cbdata[id].z[0];
	for(int i=1; i<8; i++){
		if( *min_x > cbdata[id].x[i] ){ *min_x = cbdata[id].x[i]; }
		if( *min_y > cbdata[id].y[i] ){ *min_y = cbdata[id].y[i]; }
		if( *min_z > cbdata[id].z[i] ){ *min_z = cbdata[id].z[i]; }

		if( *max_x < cbdata[id].x[i] ){ *max_x = cbdata[id].x[i]; }
		if( *max_y < cbdata[id].y[i] ){ *max_y = cbdata[id].y[i]; }
		if( *max_z < cbdata[id].z[i] ){ *max_z = cbdata[id].z[i]; }
	}

	//計算誤差対策のため、わずかに大きめにする。
	*min_x -= COLLISION_ADDSIZE;
	*min_y -= COLLISION_ADDSIZE;
	*min_z -= COLLISION_ADDSIZE;
	*max_x += COLLISION_ADDSIZE;
	*max_y += COLLISION_ADDSIZE;
	*max_z += COLLISION_ADDSIZE;
}

//! @brief 空間分割のグル?プを算出
//! @param x X座標
//! @param z Z座標
//! @return グル?プ番号
//! @attention 「グル?プ番号」の概念は別途ドキュメントを参照
int Collision::GetWorldGroup(float x, float z)
{
	if( (x > 0)&&(z > 0) ){ return 1; }
	if( (x < 0)&&(z > 0) ){ return 2; }
	if( (x < 0)&&(z < 0) ){ return 3; }
	if( (x > 0)&&(z < 0) ){ return 4; }
	return 0;
}

//! @brief AABBによるブロックとの当たり判定
//! @param id 判定するブロック番号
//! @param min_x 物体の最少 X座標
//! @param min_y 物体の最少 Y座標
//! @param min_z 物体の最少 Z座標
//! @param max_x 物体の最大 X座標
//! @param max_y 物体の最大 Y座標
//! @param max_z 物体の最大 Z座標
//! @return 当たっている：true?当たっていない：false
//! @attention エラ?がある場合「当たっていない：false」が返されます。
bool Collision::CheckBlockAABB(int id, float min_x, float min_y, float min_z, float max_x, float max_y, float max_z)
{
	if( BlockDataIF == NULL ){ return false; }
	if( (id < 0)||(BlockDataIF->GetTotaldatas() <= id) ){ return false; }

	//板状のブロックは計算外
	if( cbdata[id].BoardBlock == true ){
		return false;
	}

	return CollideBoxAABB(min_x, min_y, min_z, max_x, max_y, max_z, cbdata[id].min_x, cbdata[id].min_y, cbdata[id].min_z, cbdata[id].max_x, cbdata[id].max_y, cbdata[id].max_z);
}

//! @brief AABBによる全てのブロックとの当たり判定
//! @param min_x 物体の最少 X座標
//! @param min_y 物体の最少 Y座標
//! @param min_z 物体の最少 Z座標
//! @param max_x 物体の最大 X座標
//! @param max_y 物体の最大 Y座標
//! @param max_z 物体の最大 Z座標
//! @return 当たっている：true?当たっていない：false
//! @attention エラ?がある場合「当たっていない：false」が返されます。
bool Collision::CheckALLBlockAABB(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z)
{
	if( BlockDataIF == NULL ){ return false; }

	int bs = BlockDataIF->GetTotaldatas();

	for(int i=0; i<bs; i++){
		if( CheckBlockAABB(i, min_x, min_y, min_z, max_x, max_y, max_z) == true ){ return true; }
	}

	return false;
}

//! @brief 特定の座標が、ブロックの面の?側か調べる
//! @param id 判定するブロック番号
//! @param face 判定する面番号
//! @param x X座標
//! @param y Y座標
//! @param z Z座標
//! @return ?向き：true?裏向き：false
//! @warning 絶対座標を指定する必要があります。ベクトルではありません。
//! @attention 各当たり判定の関数から自動的に呼ばれます。
bool Collision::CheckPolygonFront(int id, int face, float x, float y, float z)
{
	if( BlockDataIF == NULL ){ return false; }
	if( (id < 0)||(BlockDataIF->GetTotaldatas() <= id) ){ return false; }
	if( (face < 0)||(6 < face) ){ return false; }

	blockdata bdata;
	float vx, vy, vz;
	float d;

	BlockDataIF->Getdata(&bdata, id);

	//面の中心を基??に、座標のベクトルを求める
	vx = cbdata[id].polygon_center_x[face] - x;
	vy = cbdata[id].polygon_center_y[face] - y;
	vz = cbdata[id].polygon_center_z[face] - z;

	//内積
	d = bdata.material[face].vx*vx + bdata.material[face].vy*vy + bdata.material[face].vz*vz;

	if( d <= 0.0f ){
		return true;
	}
	return false;
}

//! @brief 特定の方向に対して、ブロックの面の?側か調べる
//! @param id 判定するブロック番号
//! @param face 判定する面番号
//! @param rx 横方向の角度
//! @return ?向き：true?裏向き：false
bool Collision::CheckPolygonFrontRx(int id, int face, float rx)
{
	if( BlockDataIF == NULL ){ return false; }
	if( (id < 0)||(BlockDataIF->GetTotaldatas() <= id) ){ return false; }
	if( (face < 0)||(6 < face) ){ return false; }

	blockdata bdata;
	float vx, vz;
	float d;

	BlockDataIF->Getdata(&bdata, id);

	//ベクトル算出
	vx = cosf(rx);
	vz = sinf(rx);

	//内積
	d = bdata.material[face].vx*vx + bdata.material[face].vz*vz;

	if( d <= 0.0f ){
		return true;
	}
	return false;
}

//! @brief ブロックの面とレイ（光線）の当たり判定
//! @param blockid 判定するブロック番号
//! @param face 判定する面番号
//! @param RayPos_x レイの位置（始?）を指定する X座標
//! @param RayPos_y レイの位置（始?）を指定する Y座標
//! @param RayPos_z レイの位置（始?）を指定する Z座標
//! @param RayDir_x レイのベクトルを指定する X成分
//! @param RayDir_y レイのベクトルを指定する Y成分
//! @param RayDir_z レイのベクトルを指定する Z成分
//! @param out_Dist 当たったブロックとの距離を受け取る?イン?
//! @return 当たっている：true?当たっていない：false
//! @warning RayPos（始?）と RayDir（ベクトル）を間違えないこと。
//! @attention 各当たり判定の関数から自動的に呼ばれます。
bool Collision::CheckIntersectTri(int blockid, int face, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, float *out_Dist)
{
	blockdata data;
	int vID[4];
	float d1, d2;
	float vx1, vy1, vz1;
	float dist;
	float x, y, z;
	float vx2, vy2, vz2;

	BlockDataIF->Getdata(&data, blockid);
	blockdataface(face, vID, NULL);

	//内積
	d1 = data.material[face].vx*RayDir_x + data.material[face].vy*RayDir_y + data.material[face].vz*RayDir_z;

	if( d1 >= 0.0f ){
		return false;		//面とレイが平行か、面に対してレイが逆向き
	}

	//面の中心を基??に、座標のベクトルを求める
	vx1 = RayPos_x - cbdata[blockid].polygon_center_x[face];
	vy1 = RayPos_y - cbdata[blockid].polygon_center_y[face];
	vz1 = RayPos_z - cbdata[blockid].polygon_center_z[face];

	//内積
	d2 = data.material[face].vx*vx1 + data.material[face].vy*vy1 + data.material[face].vz*vz1;		//面までの最短距離が求まる

	//交?までの距離と座標
	dist = 1.0f / (d1*-1) * d2;
	x = RayPos_x + RayDir_x * dist;
	y = RayPos_y + RayDir_y * dist;
	z = RayPos_z + RayDir_z * dist;


	//ブロック全体のAABBに入らなければ除外
	if( (x < cbdata[blockid].min_x)||(cbdata[blockid].max_x < x) ){ return false; }
	if( (y < cbdata[blockid].min_y)||(cbdata[blockid].max_y < y) ){ return false; }
	if( (z < cbdata[blockid].min_z)||(cbdata[blockid].max_z < z) ){ return false; }


	//以下、ブロック面の内側に交?があるか?ェック
	//??面を?成する各4辺との位置関係を算出し、面の?線と比較する。

	//外積
	vx2 = ((cbdata[blockid].y[ vID[1] ] - cbdata[blockid].y[ vID[0] ]) * (z - cbdata[blockid].z[ vID[0] ])) - ((y - cbdata[blockid].y[ vID[0] ]) * (cbdata[blockid].z[ vID[1] ] - cbdata[blockid].z[ vID[0] ]));
	vy2 = ((cbdata[blockid].z[ vID[1] ] - cbdata[blockid].z[ vID[0] ]) * (x - cbdata[blockid].x[ vID[0] ])) - ((z - cbdata[blockid].z[ vID[0] ]) * (cbdata[blockid].x[ vID[1] ] - cbdata[blockid].x[ vID[0] ]));
	vz2 = ((cbdata[blockid].x[ vID[1] ] - cbdata[blockid].x[ vID[0] ]) * (y - cbdata[blockid].y[ vID[0] ])) - ((x - cbdata[blockid].x[ vID[0] ]) * (cbdata[blockid].y[ vID[1] ] - cbdata[blockid].y[ vID[0] ]));

	//内積
	d1 = data.material[face].vx*vx2 + data.material[face].vy*vy2 + data.material[face].vz*vz2;		//ブロック面の?線との関係を算出

	if( d1 < 0.0f ){	//外側にあれば除外
		return false;
	}


	//外積
	vx2 = ((cbdata[blockid].y[ vID[2] ] - cbdata[blockid].y[ vID[1] ]) * (z - cbdata[blockid].z[ vID[1] ])) - ((y - cbdata[blockid].y[ vID[1] ]) * (cbdata[blockid].z[ vID[2] ] - cbdata[blockid].z[ vID[1] ]));
	vy2 = ((cbdata[blockid].z[ vID[2] ] - cbdata[blockid].z[ vID[1] ]) * (x - cbdata[blockid].x[ vID[1] ])) - ((z - cbdata[blockid].z[ vID[1] ]) * (cbdata[blockid].x[ vID[2] ] - cbdata[blockid].x[ vID[1] ]));
	vz2 = ((cbdata[blockid].x[ vID[2] ] - cbdata[blockid].x[ vID[1] ]) * (y - cbdata[blockid].y[ vID[1] ])) - ((x - cbdata[blockid].x[ vID[1] ]) * (cbdata[blockid].y[ vID[2] ] - cbdata[blockid].y[ vID[1] ]));

	//内積
	d1 = data.material[face].vx*vx2 + data.material[face].vy*vy2 + data.material[face].vz*vz2;		//ブロック面の?線との関係を算出

	if( d1 < 0.0f ){	//外側にあれば除外
		return false;
	}


	//外積
	vx2 = ((cbdata[blockid].y[ vID[3] ] - cbdata[blockid].y[ vID[2] ]) * (z - cbdata[blockid].z[ vID[2] ])) - ((y - cbdata[blockid].y[ vID[2] ]) * (cbdata[blockid].z[ vID[3] ] - cbdata[blockid].z[ vID[2] ]));
	vy2 = ((cbdata[blockid].z[ vID[3] ] - cbdata[blockid].z[ vID[2] ]) * (x - cbdata[blockid].x[ vID[2] ])) - ((z - cbdata[blockid].z[ vID[2] ]) * (cbdata[blockid].x[ vID[3] ] - cbdata[blockid].x[ vID[2] ]));
	vz2 = ((cbdata[blockid].x[ vID[3] ] - cbdata[blockid].x[ vID[2] ]) * (y - cbdata[blockid].y[ vID[2] ])) - ((x - cbdata[blockid].x[ vID[2] ]) * (cbdata[blockid].y[ vID[3] ] - cbdata[blockid].y[ vID[2] ]));

	//内積
	d1 = data.material[face].vx*vx2 + data.material[face].vy*vy2 + data.material[face].vz*vz2;		//ブロック面の?線との関係を算出

	if( d1 < 0.0f ){	//外側にあれば除外
		return false;
	}


	//外積
	vx2 = ((cbdata[blockid].y[ vID[0] ] - cbdata[blockid].y[ vID[3] ]) * (z - cbdata[blockid].z[ vID[3] ])) - ((y - cbdata[blockid].y[ vID[3] ]) * (cbdata[blockid].z[ vID[0] ] - cbdata[blockid].z[ vID[3] ]));
	vy2 = ((cbdata[blockid].z[ vID[0] ] - cbdata[blockid].z[ vID[3] ]) * (x - cbdata[blockid].x[ vID[3] ])) - ((z - cbdata[blockid].z[ vID[3] ]) * (cbdata[blockid].x[ vID[0] ] - cbdata[blockid].x[ vID[3] ]));
	vz2 = ((cbdata[blockid].x[ vID[0] ] - cbdata[blockid].x[ vID[3] ]) * (y - cbdata[blockid].y[ vID[3] ])) - ((x - cbdata[blockid].x[ vID[3] ]) * (cbdata[blockid].y[ vID[0] ] - cbdata[blockid].y[ vID[3] ]));

	//内積
	d1 = data.material[face].vx*vx2 + data.material[face].vy*vy2 + data.material[face].vz*vz2;		//ブロック面の?線との関係を算出

	if( d1 < 0.0f ){	//外側にあれば除外
		return false;
	}


	*out_Dist = dist;

	return true;
}

//! @brief ブロックに埋まっていないか調べる
//! @param blockid 判定するブロック番号
//! @param x X座標
//! @param y Y座標
//! @param z Z座標
//! @param worldgroup 空間のグル?プを利用して計算省略を試みる（true：有効・計算省略?false：無効・完全検索）
//! @param *planeid ?にある面番号（NULL可）
//! @return 埋っている：true?埋っていない：false
//! @warning *planeid が返す?面（0?5）は、複数の面が該当する場合でも、最初に見つけた1面のみ返します。
bool Collision::CheckBlockInside(int blockid, float x, float y, float z, bool worldgroup, int *planeid)
{
	if( BlockDataIF == NULL ){ return false; }
	if( (blockid < 0)||(BlockDataIF->GetTotaldatas() <= blockid) ){ return false; }

	//板状のブロックは計算外
	if( cbdata[blockid].BoardBlock == true ){ return false; }


	//判定の粗削り
	if( worldgroup == true ){

		//空間分割
		if( cbdata[blockid].worldgroup != 0 ){
			//観測?の空間のグル?プを取得
			int worldgroup = GetWorldGroup(x, z);

			if( worldgroup != 0 ){
				//空間のグル?プが違えば計算外
				if( cbdata[blockid].worldgroup != worldgroup ){
					return false;
				}
			}
		}

		//範囲で検索
		if( (x < cbdata[blockid].min_x)||(cbdata[blockid].max_x < x) ){ return false; }
		if( (y < cbdata[blockid].min_y)||(cbdata[blockid].max_y < y) ){ return false; }
		if( (z < cbdata[blockid].min_z)||(cbdata[blockid].max_z < z) ){ return false; }
	}


	//6面から見て全て裏面かどうか
	for(int i=0; i<6; i++){
		if( CheckPolygonFront(blockid, i, x, y, z) == true ){
			if( planeid != NULL ){ *planeid = i; }
			return false;	//?面ならば終了
		}
	}

	return true;
}

//! @brief 全てのブロックに埋まっていないか調べる
//! @param x X座標
//! @param y Y座標
//! @param z Z座標
//! @return 埋っている：true?埋っていない：false
bool Collision::CheckALLBlockInside(float x, float y, float z)
{
	if( BlockDataIF == NULL ){ return false; }

	int bs = BlockDataIF->GetTotaldatas();

	for(int i=0; i<bs; i++){
		if( CheckBlockInside(i, x, y, z, true, NULL) == true ){ return true; }
	}

	return false;
}

//! @brief ブロックとレイ（光線）の当たり判定
//! @param blockid 判定するブロック番号
//! @param RayPos_x レイの位置（始?）を指定する X座標
//! @param RayPos_y レイの位置（始?）を指定する Y座標
//! @param RayPos_z レイの位置（始?）を指定する Z座標
//! @param RayDir_x レイのベクトルを指定する X成分
//! @param RayDir_y レイのベクトルを指定する Y成分
//! @param RayDir_z レイのベクトルを指定する Z成分
//! @param face 当たったブロックの面番号（0?5）を受け取る?イン??（NULL可）
//! @param Dist 当たったブロックとの距離を受け取る?イン?
//! @param maxDist 判定を行う最大距離?（0.0 未満で無効・無限）
//! @return 当たっている：true?当たっていない：false
//! @warning RayPos（始?）と RayDir（ベクトル）を間違えないこと。
//! @warning 判定を行う最大距離を指定しないと、パフォ??ンスが大幅に低下します。
//! @attention レイの始?から裏側になるブロックの面は無視されます。厚さを?ロに変?させた板状のブロックも無視します。
//! @attention また、レイが複数のブロックに当たる場合は、レイの始?から一番近い判定を返します。
bool Collision::CheckBlockIntersectRay(int blockid, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, int *face, float *Dist, float maxDist)
{
	if( BlockDataIF == NULL ){ return false; }
	if( (blockid < 0)||(BlockDataIF->GetTotaldatas() <= blockid) ){ return false; }
	if( (RayDir_x == 0.0f)&&(RayDir_y == 0.0f)&&(RayDir_z == 0.0f) ){ return false; }

	float pDist;
	float min_pDist = FLT_MAX;
	int min_blockface = -1;
	float rmin_x = 0.0f;
	float rmin_y = 0.0f;
	float rmin_z = 0.0f;
	float rmax_x = 0.0f;
	float rmax_y = 0.0f;
	float rmax_z = 0.0f;
	int worldgroupA = 0;
	int worldgroupB = 0;

	//板状のブロックは計算外
	if( cbdata[blockid].BoardBlock == true ){
		if( face != NULL ){ *face = 0; }
		*Dist = 0.0f;
		return false;
	}

	if( maxDist > 0.0f ){
		//レイのAABBを作る
		GetAABBRay(RayPos_x, RayPos_y, RayPos_z, RayDir_x, RayDir_y, RayDir_z, maxDist, &rmin_x, &rmin_y, &rmin_z, &rmax_x, &rmax_y, &rmax_z);
	}

	if( maxDist > 0.0f ){
		//始?と終?の空間グル?プを取得
		worldgroupA = GetWorldGroup(RayPos_x, RayPos_z);
		worldgroupB = GetWorldGroup(RayPos_x + RayDir_x * maxDist, RayPos_z + RayDir_z * maxDist);
	}

	if( maxDist > 0.0f ){
		if( cbdata[blockid].worldgroup != 0 ){
			//始?と終?が、空間のグル?プから出ていなければ
			if( (worldgroupA != 0)&&(worldgroupA == worldgroupB) ){

				//空間のグル?プが違えば計算外
				if( cbdata[blockid].worldgroup == worldgroupA ){		//worldgroupA == worldgroupB
					//境界?ックス同士で判定
					if( CollideBoxAABB(cbdata[blockid].min_x, cbdata[blockid].min_y, cbdata[blockid].min_z, cbdata[blockid].max_x, cbdata[blockid].max_y, cbdata[blockid].max_z, rmin_x, rmin_y, rmin_z, rmax_x, rmax_y, rmax_z) == false ){
						//当たってなければ、このブロックは調べない。
						if( face != NULL ){ *face = 0; }
						*Dist = 0.0f;
						return false;
					}
				}
			}
		}
	}

	//境界?ックスとレイで判定
	if( CollideAABBRay(cbdata[blockid].min_x, cbdata[blockid].min_y, cbdata[blockid].min_z, cbdata[blockid].max_x, cbdata[blockid].max_y, cbdata[blockid].max_z, RayPos_x, RayPos_y, RayPos_z, RayDir_x, RayDir_y, RayDir_z, NULL, -1.0f) == false ){
		//当たってなければ、このブロックは調べない。
		if( face != NULL ){ *face = 0; }
		*Dist = 0.0f;
		return false;
	}

	//各?リゴン単位で判定
	for(int i=0; i<6; i++){
		if( CheckPolygonFront(blockid, i, RayPos_x, RayPos_y, RayPos_z) == true ){
			if( CheckIntersectTri(blockid, i, RayPos_x, RayPos_y, RayPos_z, RayDir_x, RayDir_y, RayDir_z, &pDist) == true ){
				if( min_pDist > pDist ){
					min_pDist = pDist;
					min_blockface = i;
				}
			}
		}
	}

	//見つけた距離が最大距離を超えていれば、判定を無効に。
	if( maxDist >= 0.0f ){
		if( min_pDist > maxDist ){
			min_blockface = -1;
		}
	}

	//見つからなければ、?イン?に適当な数字を入れて返す。
	if( min_blockface == -1 ){
		if( face != NULL ){ *face = 0; }
		*Dist = 0.0f;
		return false;
	}

	//計算結果を入れて返す
	if( face != NULL ){ *face = min_blockface; }
	*Dist = min_pDist;
	return true;
}

//! @brief 全てのブロックとレイ（光線）の当たり判定
//! @param RayPos_x レイの位置（始?）を指定する X座標
//! @param RayPos_y レイの位置（始?）を指定する Y座標
//! @param RayPos_z レイの位置（始?）を指定する Z座標
//! @param RayDir_x レイのベクトルを指定する X成分
//! @param RayDir_y レイのベクトルを指定する Y成分
//! @param RayDir_z レイのベクトルを指定する Z成分
//! @param id 当たったブロックのIDを受け取る?イン??（NULL可）
//! @param face 当たったブロックの面番号（0?5）を受け取る?イン??（NULL可）
//! @param Dist 当たったブロックとの距離を受け取る?イン?
//! @param maxDist 判定を行う最大距離?（0.0 未満で無効・無限）
//! @return 当たっている：true?当たっていない：false
//! @warning RayPos（始?）と RayDir（ベクトル）を間違えないこと。
//! @warning 判定を行う最大距離を指定しないと、パフォ??ンスが大幅に低下します。
//! @warning 使い方は CheckALLBlockIntersectDummyRay()関数 と類似していますが、同関数より高精度で低速です。
//! @attention レイの始?から裏側になるブロックの面は無視されます。厚さを?ロに変?させた板状のブロックも無視します。
//! @attention また、レイが複数のブロックに当たる場合は、<b>レイの始?から一番近い判定</b>を返します。
bool Collision::CheckALLBlockIntersectRay(float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, int *id, int *face, float *Dist, float maxDist)
{
	if( BlockDataIF == NULL ){ return false; }
	if( (RayDir_x == 0.0f)&&(RayDir_y == 0.0f)&&(RayDir_z == 0.0f) ){ return false; }

	int bs = BlockDataIF->GetTotaldatas();
	float pDist;
	float min_pDist = FLT_MAX;
	int min_blockid = -1;
	int min_blockface = -1;
	float rmin_x = 0.0f;
	float rmin_y = 0.0f;
	float rmin_z = 0.0f;
	float rmax_x = 0.0f;
	float rmax_y = 0.0f;
	float rmax_z = 0.0f;
	int worldgroupA = 0;
	int worldgroupB = 0;

	if( maxDist > 0.0f ){
		//レイのAABBを作る
		GetAABBRay(RayPos_x, RayPos_y, RayPos_z, RayDir_x, RayDir_y, RayDir_z, maxDist, &rmin_x, &rmin_y, &rmin_z, &rmax_x, &rmax_y, &rmax_z);
	}

	if( maxDist > 0.0f ){
		//始?と終?の空間グル?プを取得
		worldgroupA = GetWorldGroup(RayPos_x, RayPos_z);
		worldgroupB = GetWorldGroup(RayPos_x + RayDir_x * maxDist, RayPos_z + RayDir_z * maxDist);
	}
	
	for(int i=0; i<bs; i++){
		//板状のブロックは計算外
		if( cbdata[i].BoardBlock == true ){ continue; }

		if( maxDist > 0.0f ){
			if( cbdata[i].worldgroup != 0 ){
				//始?と終?が、空間のグル?プから出ていなければ
				if( (worldgroupA != 0)&&(worldgroupA == worldgroupB) ){

					//空間のグル?プが違えば計算外
					if( cbdata[i].worldgroup == worldgroupA ){		//worldgroupA == worldgroupB

						//境界?ックス同士で判定
						if( CollideBoxAABB(cbdata[i].min_x, cbdata[i].min_y, cbdata[i].min_z, cbdata[i].max_x, cbdata[i].max_y, cbdata[i].max_z, rmin_x, rmin_y, rmin_z, rmax_x, rmax_y, rmax_z) == false ){
							continue;	//当たってなければ、このブロックは調べない。
						}

					}
				}
			}
		}

		//境界?ックスとレイで判定
		if( CollideAABBRay(cbdata[i].min_x, cbdata[i].min_y, cbdata[i].min_z, cbdata[i].max_x, cbdata[i].max_y, cbdata[i].max_z, RayPos_x, RayPos_y, RayPos_z, RayDir_x, RayDir_y, RayDir_z, NULL, -1.0f) == false ){
			continue;		//当たってなければ、このブロックは調べない。
		}

		//各?リゴン単位で判定
		for(int j=0; j<6; j++){
			if( CheckPolygonFront(i, j, RayPos_x, RayPos_y, RayPos_z) == true ){
				if( CheckIntersectTri(i, j, RayPos_x, RayPos_y, RayPos_z, RayDir_x, RayDir_y, RayDir_z, &pDist) == true ){
					if( min_pDist > pDist ){
						min_pDist = pDist;
						min_blockid = i;
						min_blockface = j;
					}
				}
			}
		}
	}

	//見つけた距離が最大距離を超えていれば、判定を無効に。
	if( maxDist >= 0.0f ){
		if( min_pDist > maxDist ){
			min_blockid = -1;
		}
	}

	//見つからなければ、?イン?に適当な数字を入れて返す。
	if( min_blockid == -1 ){
		if( id != NULL ){ *id = 0; }
		if( face != NULL ){ *face = 0; }
		*Dist = 0.0f;
		return false;
	}

	//計算結果を入れて返す
	if( id != NULL ){ *id = min_blockid; }
	if( face != NULL ){ *face = min_blockface; }
	*Dist = min_pDist;
	return true;
}

//! @brief 全てのブロックと衝突しているか判定
//! @param RayPos_x 始? X座標
//! @param RayPos_y 始? Y座標
//! @param RayPos_z 始? Z座標
//! @param RayDir_x ベクトル X成分
//! @param RayDir_y ベクトル Y成分
//! @param RayDir_z ベクトル Z成分
//! @param id 当たったブロックのIDを受け取る?イン??（NULL可）
//! @param face 当たったブロックの面番号（0?5）を受け取る?イン??（NULL可）
//! @param Dist ???変数の?イン?（常に 0.0f を返す）
//! @param maxDist 判定を行う最大距離?（0.0 未満指定 不可）
//! @return 当たっている：true?当たっていない：false
//! @warning 始?と ベクトルを間違えないこと。
//! @warning 使い方は CheckALLBlockIntersectRay()関数 と類似していますが、同関数より高速で低精度です。
//! @attention レイの始?から裏側になるブロックの面は無視されます。厚さを?ロに変?させた板状のブロックも無視します。
//! @attention また、レイが複数のブロックに当たる場合は、<b>一番最初に発見した判定</b>を返します。
bool Collision::CheckALLBlockIntersectDummyRay(float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, int *id, int *face, float *Dist, float maxDist)
{
	if( BlockDataIF == NULL ){ return false; }
	if( maxDist <= 0.0f ){ return false; }

	float RayPos2_x = RayPos_x + RayDir_x * maxDist/2;
	float RayPos2_y = RayPos_y + RayDir_y * maxDist/2;
	float RayPos2_z = RayPos_z + RayDir_z * maxDist/2;
	float RayPos3_x = RayPos_x + RayDir_x * maxDist;
	float RayPos3_y = RayPos_y + RayDir_y * maxDist;
	float RayPos3_z = RayPos_z + RayDir_z * maxDist;
	
	if( (id == NULL)&&(face == NULL) ){
		for(int i=0; i<MAX_BLOCKS; i++){
			//終了時?
			if( CheckBlockInside(i, RayPos3_x, RayPos3_y, RayPos3_z, true, NULL) == true ){
				*Dist = 0.0f;
				return true;
			}

			//中間時?
			if( CheckBlockInside(i, RayPos2_x, RayPos2_y, RayPos2_z, true, NULL) == true ){
				*Dist = 0.0f;
				return true;
			}
		}

		*Dist = 0.0f;
		return false;
	}

	for(int i=0; i<MAX_BLOCKS; i++){
		int surface;

		//開始地?
		CheckBlockInside(i, RayPos_x, RayPos_y, RayPos_z, false, &surface);

		//終了時?
		if( CheckBlockInside(i, RayPos3_x, RayPos3_y, RayPos3_z, true, &surface) == true ){
			if( id != NULL ){ *id = i; }
			if( face != NULL ){ *face = surface; }
			*Dist = 0.0f;
			return true;
		}

		//中間時?
		if( CheckBlockInside(i, RayPos2_x, RayPos2_y, RayPos2_z, true, &surface) == true ){
			if( id != NULL ){ *id = i; }
			if( face != NULL ){ *face = surface; }
			*Dist = 0.0f;
			return true;
		}
	}

	if( id != NULL ){ *id = 0; }
	if( face != NULL ){ *face = 0; }
	*Dist = 0.0f;
	return false;
}

//! @brief ブロックに沿って移動するベクトルを求める
void Collision::ScratchVector(int id, int face, float in_vx, float in_vy, float in_vz, float *out_vx, float *out_vy, float *out_vz)
{
	if( BlockDataIF == NULL ){ return; }
	if( (id < 0)||(BlockDataIF->GetTotaldatas() <= id) ){ return; }
	if( (face < 0)||(6 < face) ){ return; }

	blockdata bdata;
	BlockDataIF->Getdata(&bdata, id);

	//内積
	float Dot = in_vx * bdata.material[face].vx + in_vy * bdata.material[face].vy + in_vz * bdata.material[face].vz;

	*out_vx = in_vx - Dot * bdata.material[face].vx;
	*out_vy = in_vy - Dot * bdata.material[face].vy;
	*out_vz = in_vz - Dot * bdata.material[face].vz;
}

//! @brief ブロックに反射するベクトルを求める
void Collision::ReflectVector(int id, int face, float in_vx, float in_vy, float in_vz, float *out_vx, float *out_vy, float *out_vz)
{
	if( BlockDataIF == NULL ){ return; }
	if( (id < 0)||(BlockDataIF->GetTotaldatas() <= id) ){ return; }
	if( (face < 0)||(6 < face) ){ return; }

	blockdata bdata;
	BlockDataIF->Getdata(&bdata, id);

	//内積
	float Dot = in_vx * bdata.material[face].vx + in_vy * bdata.material[face].vy + in_vz * bdata.material[face].vz;

	*out_vx = in_vx - 2.0f * Dot * bdata.material[face].vx;
	*out_vy = in_vy - 2.0f * Dot * bdata.material[face].vy;
	*out_vz = in_vz - 2.0f * Dot * bdata.material[face].vz;
}

//! @brief ブロックに対するベクトルの進入角度を求める
//! @return 成功：true?失敗：false
//! @attention ?向き：0.0?PI/2（0.0度?90.0度）、裏向き：-0.0?-PI/2（-0.0度?-90.0度）
bool Collision::AngleVector(int id, int face, float in_vx, float in_vy, float in_vz, float *out_angle)
{
	if( BlockDataIF == NULL ){ return false; }
	if( (id < 0)||(BlockDataIF->GetTotaldatas() <= id) ){ return false; }
	if( (face < 0)||(6 < face) ){ return false; }
	if( out_angle == NULL ){ return false; }

	if( (in_vx == 0.0f)&&(in_vy == 0.0f)&&(in_vz == 0.0f) ){ return false; }

	blockdata bdata;
	BlockDataIF->Getdata(&bdata, id);

	VectorNormalization(&in_vx, &in_vy, &in_vz);

	//内積
	float Dot = in_vx * bdata.material[face].vx + in_vy * bdata.material[face].vy + in_vz * bdata.material[face].vz;

	//角度を求める
	*out_angle = asinf(Dot) * -1;

	return true;
}

//! @brief ブロックに対するベクトルのなす角を求める
//! @return 成功：true?失敗：false
//! @attention ベクトルのなす角：0.0?PI（0度?180度）
bool Collision::AngleVector2(int id, int face, float in_vx, float in_vy, float in_vz, float *out_angle)
{
	if( BlockDataIF == NULL ){ return false; }
	if( (id < 0)||(BlockDataIF->GetTotaldatas() <= id) ){ return false; }
	if( (face < 0)||(6 < face) ){ return false; }
	if( out_angle == NULL ){ return false; }

	if( (in_vx == 0.0f)&&(in_vy == 0.0f)&&(in_vz == 0.0f) ){ return false; }

	blockdata bdata;
	BlockDataIF->Getdata(&bdata, id);

	//内積
	float Dot = in_vx * bdata.material[face].vx + in_vy * bdata.material[face].vy + in_vz * bdata.material[face].vz;

	//裏向きなら除外
	if( Dot >= 0.0f ){ return false; }

	//なす角を求める
	*out_angle = acosf(Dot);

	return true;
}

//! @brief レイのAABBを求める
//! @param RayPos_x 始? X座標
//! @param RayPos_y 始? Y座標
//! @param RayPos_z 始? Z座標
//! @param RayDir_x ベクトル X成分
//! @param RayDir_y ベクトル Y成分
//! @param RayDir_z ベクトル Z成分
//! @param maxDist 判定を行う最大距離
//! @param *min_x 最小 X座標を返す?イン?
//! @param *min_y 最小 Y座標を返す?イン?
//! @param *min_z 最小 Z座標を返す?イン?
//! @param *max_x 最大 X座標を返す?イン?
//! @param *max_y 最大 Y座標を返す?イン?
//! @param *max_z 最大 Z座標を返す?イン?
void GetAABBRay(float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, float maxDist, float *min_x, float *min_y, float *min_z, float *max_x, float *max_y, float *max_z)
{
	float rmin_x, rmin_y, rmin_z, rmax_x, rmax_y, rmax_z;

	//レイのAABBを作る
	rmin_x = RayPos_x + RayDir_x * maxDist;
	rmin_y = RayPos_y + RayDir_y * maxDist;
	rmin_z = RayPos_z + RayDir_z * maxDist;
	rmax_x = rmin_x;
	rmax_y = rmin_y;
	rmax_z = rmin_z;
	if( rmin_x > RayPos_x ){ rmin_x = RayPos_x; }
	if( rmin_y > RayPos_y ){ rmin_y = RayPos_y; }
	if( rmin_z > RayPos_z ){ rmin_z = RayPos_z; }
	if( rmax_x < RayPos_x ){ rmax_x = RayPos_x; }
	if( rmax_y < RayPos_y ){ rmax_y = RayPos_y; }
	if( rmax_z < RayPos_z ){ rmax_z = RayPos_z; }

	//計算誤差対策のため、わずかに大きめにする。
	rmin_x -= COLLISION_ADDSIZE;
	rmin_y -= COLLISION_ADDSIZE;
	rmin_z -= COLLISION_ADDSIZE;
	rmax_x += COLLISION_ADDSIZE;
	rmax_y += COLLISION_ADDSIZE;
	rmax_z += COLLISION_ADDSIZE;

	*min_x = rmin_x;
	*min_y = rmin_y;
	*min_z = rmin_z;
	*max_x = rmax_x;
	*max_y = rmax_y;
	*max_z = rmax_z;
}

//! @brief AABBによる当たり判定
//! @param box1_min_x 物体Aの最少 X座標
//! @param box1_min_y 物体Aの最少 Y座標
//! @param box1_min_z 物体Aの最少 Z座標
//! @param box1_max_x 物体Aの最大 X座標
//! @param box1_max_y 物体Aの最大 Y座標
//! @param box1_max_z 物体Aの最大 Z座標
//! @param box2_min_x 物体Bの最少 X座標
//! @param box2_min_y 物体Bの最少 Y座標
//! @param box2_min_z 物体Bの最少 Z座標
//! @param box2_max_x 物体Bの最大 X座標
//! @param box2_max_y 物体Bの最大 Y座標
//! @param box2_max_z 物体Bの最大 Z座標
//! @return 当たっている：true?当たっていない：false
//! @attention エラ?がある場合「当たっていない：false」が返されます。
bool CollideBoxAABB(float box1_min_x, float box1_min_y, float box1_min_z, float box1_max_x, float box1_max_y, float box1_max_z, float box2_min_x, float box2_min_y, float box2_min_z, float box2_max_x, float box2_max_y, float box2_max_z)
{
	//エラ?対策
	if( box1_min_x > box1_max_x ){ return false; }
	if( box1_min_y > box1_max_y ){ return false; }
	if( box1_min_z > box1_max_z ){ return false; }
	if( box2_min_x > box2_max_x ){ return false; }
	if( box2_min_y > box2_max_y ){ return false; }
	if( box2_min_z > box2_max_z ){ return false; }

	if(
		(box1_min_x < box2_max_x)&&(box1_max_x > box2_min_x)&&
		(box1_min_y < box2_max_y)&&(box1_max_y > box2_min_y)&&
		(box1_min_z < box2_max_z)&&(box1_max_z > box2_min_z)
	){
		return true;
	}

	return false;
}

//! @brief ?柱同士の当たり判定
//! @param c1_x ?柱1 底辺のx座標
//! @param c1_y ?柱1 底辺のy座標
//! @param c1_z ?柱1 底辺のz座標
//! @param c1_r ?柱1 の半径
//! @param c1_h ?柱1 の高さ
//! @param c2_x ?柱2 底辺のx座標
//! @param c2_y ?柱2 底辺のy座標
//! @param c2_z ?柱2 底辺のz座標
//! @param c2_r ?柱2 の半径
//! @param c2_h ?柱2 の高さ
//! @param *angle ?柱1からみた接触角度を受け取る?イン?（NULL可）
//! @param *length ?柱1からみた接触距離を受け取る?イン?（NULL可）
//! @return 当たっている：true?当たっていない：false
bool CollideCylinder(float c1_x, float c1_y, float c1_z, float c1_r, float c1_h, float c2_x, float c2_y, float c2_z, float c2_r, float c2_h, float *angle, float *length)
{
	//先にお手軽な高さで判定
	if( (c1_y < c2_y + c2_h)&&(c1_y + c1_h > c2_y) ){

		//距離で判定
		float x = c1_x - c2_x;
		float z = c1_z - c2_z;
		float caser = x*x + z*z;
		float minr = (c1_r+c2_r) * (c1_r+c2_r);
		if( caser < minr ){
			if( angle != NULL ){ *angle = atan2f(z, x); }
			if( length != NULL ){ *length = sqrtf(minr) - sqrtf(caser); }
			return true;
		}
	}

	return false;
}

//! @brief 球体と?の当たり判定
//! @param s_x 球体のx座標
//! @param s_y 球体のy座標
//! @param s_z 球体のz座標
//! @param s_r 球体の半径
//! @param p_x X座標
//! @param p_y Y座標
//! @param p_z Z座標
//! @return 当たっている：true?当たっていない：false
bool CollideSphereInside(float s_x, float s_y, float s_z, float s_r, float p_x, float p_y, float p_z)
{
	float x, y, z, d;

	//距離を算出
	x = s_x - p_x;
	y = s_y - p_y;
	z = s_z - p_z;
	d = x*x + y*y + z*z;

	//距離が半径より近い
	if( d < s_r*s_r ){
		return true;
	}

	return false;
}

//! @brief 球体とレイ（光線）の当たり判定
//! @param s_x 球体のx座標
//! @param s_y 球体のy座標
//! @param s_z 球体のz座標
//! @param s_r 球体の半径
//! @param RayPos_x レイの位置（始?）を指定する X座標
//! @param RayPos_y レイの位置（始?）を指定する Y座標
//! @param RayPos_z レイの位置（始?）を指定する Z座標
//! @param RayDir_x レイのベクトルを指定する X成分
//! @param RayDir_y レイのベクトルを指定する Y成分
//! @param RayDir_z レイのベクトルを指定する Z成分
//! @param Dist 当たった球体との距離を受け取る?イン?
//! @param maxDist 判定を行う最大距離?（0.0 未満で無効・無限）
//! @return 当たっている：true?当たっていない：false
//! @warning RayPos（始?）と RayDir（ベクトル）を間違えないこと。
//! @warning 判定を行う最大距離を指定しないと、パフォ??ンスが大幅に低下します。
//! @todo レイの始?が球体の外側でかつ球体と逆向きの場合、正しく判定できない？
bool CollideSphereRay(float s_x, float s_y, float s_z, float s_r, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, float *Dist, float maxDist)
{
	if( maxDist > 0.0f ){
		float pmin_x, pmin_y, pmin_z, pmax_x, pmax_y, pmax_z;

		//レイのAABBを作る
		GetAABBRay(RayPos_x, RayPos_y, RayPos_z, RayDir_x, RayDir_y, RayDir_z, maxDist, &pmin_x, &pmin_y, &pmin_z, &pmax_x, &pmax_y, &pmax_z);

		//境界?ックス同士で判定
		if( CollideBoxAABB(s_x - s_r, s_y - s_r, s_z - s_r, s_x + s_r, s_y + s_r, s_z + s_r, pmin_x, pmin_y, pmin_z, pmax_x, pmax_y, pmax_z) == false ){
			return false;
		}
	}

	float x, y, z, d;
	float MinDist, RayDist, RDist;

	//?とレイ始?の距離
	x = s_x - RayPos_x;
	y = s_y - RayPos_y;
	z = s_z - RayPos_z;
	d = sqrtf(x*x + y*y + z*z);

	//レイ始?が半径より近い（＝めり込んでいる）
	if( d < s_r ){
		*Dist = 0.0f;
		return true;
	}

	//?（球体の中心）とレイの最短距離を求める
	MinDist = DistancePosRay(s_x, s_y, s_z, RayPos_x, RayPos_y, RayPos_z, RayDir_x, RayDir_y, RayDir_z);

	if( MinDist <= s_r ){
		RayDist = sqrtf(d*d - MinDist*MinDist);		//（レイ始?から）?に最も近づく距離
		RDist = sqrtf(s_r*s_r - MinDist*MinDist);	//（?半径から）?に最も近づく距離

		*Dist = RayDist - RDist;	//レイ視?最短 - 半径最短 = レイ視?から半径までの最短

		//判定を行う最大距離よりも遠ければ、当たってないことに
		if( (maxDist > 0.0f)&&(maxDist < *Dist) ){
			return false;
		}
		return true;
	}

	return false;
}

//! @brief AABBとレイ（光線）の当たり判定
//! @param box_min_x 物体の最少 X座標
//! @param box_min_y 物体の最少 Y座標
//! @param box_min_z 物体の最少 Z座標
//! @param box_max_x 物体の最大 X座標
//! @param box_max_y 物体の最大 Y座標
//! @param box_max_z 物体の最大 Z座標
//! @param RayPos_x レイの位置（始?）を指定する X座標
//! @param RayPos_y レイの位置（始?）を指定する Y座標
//! @param RayPos_z レイの位置（始?）を指定する Z座標
//! @param RayDir_x レイのベクトルを指定する X成分
//! @param RayDir_y レイのベクトルを指定する Y成分
//! @param RayDir_z レイのベクトルを指定する Z成分
//! @param Dist 当たったAABBとの距離を受け取る?イン?
//! @param maxDist 判定を行う最大距離?（0.0 未満で無効・無限）
//! @return 当たっている：true?当たっていない：false
//! @warning RayPos（始?）と RayDir（ベクトル）を間違えないこと。
//! @warning 判定を行う最大距離を指定しないと、パフォ??ンスが大幅に低下します。
bool CollideAABBRay(float box_min_x, float box_min_y, float box_min_z, float box_max_x, float box_max_y, float box_max_z, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, float *Dist, float maxDist)
{
	if( box_min_x > box_max_x ){ return false; }
	if( box_min_y > box_max_y ){ return false; }
	if( box_min_z > box_max_z ){ return false; }

	//レイの始?がAABBの内側に入っていれば、既に当たっている
	if( (box_min_x <= RayPos_x)&&(RayPos_x <= box_max_x)&&(box_min_y <= RayPos_y)&&(RayPos_y <= box_max_y)&&(box_min_z <= RayPos_z)&&(RayPos_z <= box_max_z) ){
		if( Dist != NULL ){ *Dist = 0.0f; }
		return true;
	}

	if( maxDist > 0.0f ){
		float pmin_x, pmin_y, pmin_z, pmax_x, pmax_y, pmax_z;

		//レイのAABBを作る
		GetAABBRay(RayPos_x, RayPos_y, RayPos_z, RayDir_x, RayDir_y, RayDir_z, maxDist, &pmin_x, &pmin_y, &pmin_z, &pmax_x, &pmax_y, &pmax_z);

		//境界?ックス同士で判定
		if( CollideBoxAABB(box_min_x, box_min_y, box_min_z, box_max_x, box_max_y, box_max_z, pmin_x, pmin_y, pmin_z, pmax_x, pmax_y, pmax_z) == false ){
			return false;
		}
	}

	//ベクトルを正規化
	VectorNormalization(&RayDir_x, &RayDir_y, &RayDir_z);

	//各座標を配列に格?
	float box_min[] = {box_min_x, box_min_y, box_min_z};
	float box_max[] = {box_max_x, box_max_y, box_max_z};
	float RayPos[] = {RayPos_x, RayPos_y, RayPos_z};
	float RayDir[] = {RayDir_x, RayDir_y, RayDir_z};
	float Ray_min[3];
	float Ray_max[3];
	float Ray_tmin, Ray_tmax;

	//X・Y・Zの3軸分の処理
	for(int axis=0; axis<3; axis++){
		if( ( (RayPos[axis] < box_min[axis])||(box_max[axis] < RayPos[axis]) )&&(RayDir[axis] == 0.0f) ){
			//内側に入っていないのに ベクトルの方向が 0 なら、既にAABBの外
			return false;
		}
		else{
			//内側に入っていないが、ベクトルが方向を持つなら?
			float t1, t2;

			//AABBをベクトルが突き抜ける交?までの距離を取得
			t1 = (box_min[axis] - RayPos[axis]) / RayDir[axis];
			t2 = (box_max[axis] - RayPos[axis]) / RayDir[axis];

			//交?までの距離が最小・最大で逆なら、入れ替える。
			if( t1 > t2 ){
				float temp = t1; t1 = t2; t2 = temp;
			}

			//軸の情報として記?
			Ray_min[axis] = t1;
			Ray_max[axis] = t2;

			if( (box_min[axis] <= RayPos[axis])&&(RayPos[axis] <= box_max[axis]) ){
				//内側に入っていれば、そのまま軸の情報として記?
				Ray_min[axis] = 0.0f;
			}

			//距離が?イナス（＝ベクトル逆方向）ならAABBとは あたらない
			if( Ray_min[axis] < 0.0f ){ return false; }
			if( Ray_max[axis] < 0.0f ){ return false; }
		}
	}

	//各軸で、最も遠い‘最小距離’と最も近い‘最大距離’を算出
	Ray_tmin = Ray_min[0];
	Ray_tmax = Ray_max[0];
	if( Ray_tmin < Ray_min[1] ){ Ray_tmin = Ray_min[1]; }
	if( Ray_tmax > Ray_max[1] ){ Ray_tmax = Ray_max[1]; }
	if( Ray_tmin < Ray_min[2] ){ Ray_tmin = Ray_min[2]; }
	if( Ray_tmax > Ray_max[2] ){ Ray_tmax = Ray_max[2]; }

	//最小距離と最大距離の関係が正しければ?
	if( (Ray_tmax - Ray_tmin) > 0 ){
		//判定を行う最大距離より遠ければ、判定無効。
		if( maxDist > 0.0f ){
			if( Ray_tmin > maxDist ){ return false; }
		}

		//距離を代入し返す
		if( Dist != NULL ){ *Dist = Ray_tmin; }
		return true;
	}

	return false;
}

//! @brief ?柱と?の当たり判定
//! @param c_x ?柱 底辺のx座標
//! @param c_y ?柱 底辺のy座標
//! @param c_z ?柱 底辺のz座標
//! @param c_r ?柱 の半径
//! @param c_h ?柱 の高さ
//! @param p_x X座標
//! @param p_y Y座標
//! @param p_z Z座標
//! @return 当たっている：true?当たっていない：false
bool CollideCylinderInside(float c_x, float c_y, float c_z, float c_r, float c_h, float p_x, float p_y, float p_z)
{
	float x, z, d;


	// X-Z平面で?として処理する

	//距離を算出
	x = c_x - p_x;
	z = c_z - p_z;
	d = x*x + z*z;

	//距離が半径より離れている時?で当たらない
	if( d > c_r*c_r ){
		return false;
	}


	//Y軸のみAABBと同様の処理

	if( (c_y <= p_y)&&(p_y <= (c_y+c_h)) ){
		//Y軸でも内側に入っていれば、当たっている
		return true;
	}

	return false;
}

//! @brief ?柱とレイ（光線）の当たり判定
//! @param c_x ?柱 底辺のx座標
//! @param c_y ?柱 底辺のy座標
//! @param c_z ?柱 底辺のz座標
//! @param c_r ?柱 の半径
//! @param c_h ?柱 の高さ
//! @param RayPos_x レイの位置（始?）を指定する X座標
//! @param RayPos_y レイの位置（始?）を指定する Y座標
//! @param RayPos_z レイの位置（始?）を指定する Z座標
//! @param RayDir_x レイのベクトルを指定する X成分
//! @param RayDir_y レイのベクトルを指定する Y成分
//! @param RayDir_z レイのベクトルを指定する Z成分
//! @param Dist 当たった?柱との距離を受け取る?イン?
//! @param maxDist 判定を行う最大距離?（0.0fを超える値）
//! @return 当たっている：true?当たっていない：false
//! @warning RayPos（始?）と RayDir（ベクトル）を間違えないこと。
//! @attention Y軸方向へ垂直に立つ?柱です。向きは変えられません。
bool CollideCylinderRay(float c_x, float c_y, float c_z, float c_r, float c_h, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z, float *Dist, float maxDist)
{
	float x, z, d;
	float cMinDist, cRayDist, cRDist;
	float RayXZ_min, RayXZ_max;
	float Ray_min[2];
	float Ray_max[2];
	float Ray_tmin, Ray_tmax;


	// X-Z平面で?として処理する

	//?とレイ始?の距離
	x = c_x - RayPos_x;
	z = c_z - RayPos_z;
	d = sqrtf(x*x + z*z);

	//?（?柱の中心）とレイの最短距離を求める
	cMinDist = DistancePosRay(c_x, 0.0f, c_z, RayPos_x, 0.0f, RayPos_z, RayDir_x, 0.0f, RayDir_z);

	//最短距離が半径より離れている時?で当たらない
	if( cMinDist > c_r ){
		return false;
	}

	cRayDist = sqrtf(d*d - cMinDist*cMinDist);		//（レイ始?から）?に最も近づく距離
	cRDist = sqrtf(c_r*c_r - cMinDist*cMinDist);	//（?半径から）?に最も近づく距離

	if( d < c_r ){
		RayXZ_min = 0;		//始?が?の中なら距離?ロ
	}
	else{
		RayXZ_min = cRayDist - cRDist;	//レイ視?最短 - 半径最短 = レイ視?から半径までの最短
	}

	//?とレイ終?の距離
	x = c_x - RayPos_x + RayDir_x*maxDist;
	z = c_z - RayPos_z + RayDir_x*maxDist;
	d = sqrtf(x*x + z*z);

	if( d < c_r ){
		RayXZ_max = maxDist;		//終?が?の中なら最大距離
	}
	else{
		RayXZ_max = cRayDist + cRDist;
	}

	//Y軸と比べる前に、Y軸と比較できるよう変換?（底辺の値を斜辺へ変換）
	float Ray_ry = atan2f(RayDir_y, sqrtf(RayDir_x*RayDir_x + RayDir_z*RayDir_z));
	Ray_min[0] = RayXZ_min / cosf(Ray_ry);
	Ray_max[0] = RayXZ_max / cosf(Ray_ry);


	//Y軸のみAABBと同様の処理

	if( ( (RayPos_y < c_y)||((c_y+c_h) < RayPos_y) )&&(RayDir_y == 0.0f) ){
		//内側に入っていないのに ベクトルの方向が 0 なら、既に?柱の外
		return false;
	}

	//内側に入っていないが、ベクトルが方向を持つなら?
	float t1, t2;

	//?柱をベクトルが突き抜ける交?までの距離を取得
	t1 = (c_y - RayPos_y) / RayDir_y;
	t2 = ((c_y+c_h) - RayPos_y) / RayDir_y;

	//交?までの距離が最小・最大で逆なら、入れ替える。
	if( t1 > t2 ){
		float temp = t1; t1 = t2; t2 = temp;
	}

	//軸の情報として記?
	Ray_min[1] = t1;
	Ray_max[1] = t2;

	if( (c_y <= RayPos_y)&&(RayPos_y <= (c_y+c_h)) ){
		//内側に入っていれば、そのまま軸の情報として記?
		Ray_min[1] = 0.0f;
	}

	//距離が?イナス（＝ベクトル逆方向）なら?柱とは あたらない
	if( Ray_min[1] < 0.0f ){ return false; }
	if( Ray_max[1] < 0.0f ){ return false; }


	//両計算を合わせる

	//各軸で、最も遠い‘最小距離’と最も近い‘最大距離’を算出
	Ray_tmin = Ray_min[0];
	Ray_tmax = Ray_max[0];
	if( Ray_tmin < Ray_min[1] ){ Ray_tmin = Ray_min[1]; }
	if( Ray_tmax > Ray_max[1] ){ Ray_tmax = Ray_max[1]; }

	//最小距離と最大距離の関係が正しければ?
	if( (Ray_tmax - Ray_tmin) > 0 ){
		//判定を行う最大距離より遠ければ、判定無効。
		if( maxDist > 0.0f ){
			if( Ray_tmin > maxDist ){ return false; }
		}

		//距離を代入し返す
		if( Dist != NULL ){ *Dist = Ray_tmin; }
		return true;
	}

	return false;
}

//! @brief ?とレイの最短距離を求める
//! @param Pos_x ?のX座標
//! @param Pos_y ?のY座標
//! @param Pos_z ?のZ座標
//! @param RayPos_x レイの位置（始?）を指定する X座標
//! @param RayPos_y レイの位置（始?）を指定する Y座標
//! @param RayPos_z レイの位置（始?）を指定する Z座標
//! @param RayDir_x レイのベクトルを指定する X成分
//! @param RayDir_y レイのベクトルを指定する Y成分
//! @param RayDir_z レイのベクトルを指定する Z成分
//! @return 最短距離
//! @warning RayPos（始?）と RayDir（ベクトル）を間違えないこと。
//! @attention レイの方向は考慮されますが、レイの長さは考慮されません。
float DistancePosRay(float Pos_x, float Pos_y, float Pos_z, float RayPos_x, float RayPos_y, float RayPos_z, float RayDir_x, float RayDir_y, float RayDir_z)
{
	float x1, y1, z1;
	float x2, y2, z2;
	float x3, y3, z3;
	float Dot;

	x1 = Pos_x - RayPos_x;
	y1 = Pos_y - RayPos_y;
	z1 = Pos_z - RayPos_z;
	x2 = RayDir_x;
	y2 = RayDir_y;
	z2 = RayDir_z;

	//内積
	Dot = x1 * x2 + y1 * y2 + z1 * z2;

	//レイのベクトルが逆方向なら
	if( Dot < 0.0f ){
		return sqrtf(x1*x1 + y1*y1 + z1*z1);
	}

	//外積
	x3 = y1 * z2 - z1 * y2;
	y3 = z1 * x2 - x1 * z2;
	z3 = x1 * y2 - y1 * x2;

	return sqrtf(x3*x3 + y3*y3 + z3*z3) / sqrtf(RayDir_x*RayDir_x + RayDir_y*RayDir_y + RayDir_z*RayDir_z);
}

//! @brief 線分と線分の当たり判定（2D）
//! @param A1x 線分Aの始? X座標
//! @param A1y 線分Aの始? Y座標
//! @param A2x 線分Aの終? X座標
//! @param A2y 線分Aの終? Y座標
//! @param B1x 線分Bの始? X座標
//! @param B1y 線分Bの始? Y座標
//! @param B2x 線分Bの終? X座標
//! @param B2y 線分Bの終? Y座標
//! @param out_x 交?の X座標 を受け取る?イン?（NULL可）
//! @param out_y 交?の Y座標 を受け取る?イン?（NULL可）
//! @return 交差する：true?交差しない：false
bool Collide2DLine(int A1x, int A1y, int A2x, int A2y, int B1x, int B1y, int B2x, int B2y, int *out_x, int *out_y)
{
	//線分のベクトルを求める
	int Avx = A2x - A1x;
	int Avy = A2y - A1y;
	int Bvx = B2x - B1x;
	int Bvy = B2y - B1y;

	float v1_v2 = (float)(Avx * Bvy - Avy * Bvx);	//外積
	if( v1_v2 == 0.0f ){
		return false;	//平行
	}

	float vx = (float)(B1x - A1x);
	float vy = (float)(B1y - A1y);
	float v_v1 = vx * Avy - vy * Avx;	//外積
	float v_v2 = vx * Bvy - vy * Bvx;	//外積
	float t1 = v_v2 / v1_v2;
	float t2 = v_v1 / v1_v2;

	if( (t1 <= 0)||(1 <= t1)||(t2 <= 0)||(1 <= t2) ){
		return false;	//交差してない
	}

	if( out_x != NULL ){ *out_x = (int)(A1x + Avx * t1); }
	if( out_y != NULL ){ *out_y = (int)(A1y + Avy * t1); }

	return true;
}

//! @brief 四角?に収まる線分を求める（2D）
//! @param line_x1 線分の始? X座標
//! @param line_y1 線分の始? Y座標
//! @param line_x2 線分の終? X座標
//! @param line_y2 線分の終? Y座標
//! @param box_x1 四角?の左上 X座標
//! @param box_y1 四角?の左上 Y座標
//! @param box_x2 四角?の右下 X座標
//! @param box_y2 四角?の右下 Y座標
//! @param out_line_x1 四角?に収まる 線分の始? X座標 を受け取る?イン?
//! @param out_line_y1 四角?に収まる 線分の始? Y座標 を受け取る?イン?
//! @param out_line_x2 四角?に収まる 線分の終? X座標 を受け取る?イン?
//! @param out_line_y2 四角?に収まる 線分の終? Y座標 を受け取る?イン?
//! @return 有効（?画する）：true?無効（?画しない）：false
//! @warning 引数は必ず「box_x1 < box_x2」かつ「box_x1 < box_x2」にすること
//! @note 簡易レ???の?ップ?画用
bool Get2DLineInBox(int line_x1, int line_y1, int line_x2, int line_y2, int box_x1, int box_y1, int box_x2, int box_y2, int *out_line_x1, int *out_line_y1, int *out_line_x2, int *out_line_y2)
{
	//四角?指定が異常
	if( (box_x1 >= box_x2)||(box_y1 >= box_y2) ){ return false; }

	//上下左右の空間にあるなら、的外れ
	if( (line_x1 < box_x1)&&(line_x2 < box_x1) ){ return false; }
	if( (line_y1 < box_y1)&&(line_y2 < box_y1) ){ return false; }
	if( (box_x2 < line_x1)&&(box_x2 < line_x2) ){ return false; }
	if( (box_y2 < line_y1)&&(box_y2 < line_y2) ){ return false; }

	//既に四角?に収まる
	if( (box_x1 <= line_x1)&&(line_x1 <= box_x2)&&(box_y1 <= line_y1)&&(line_y1 <= box_y2) ){
		if( (box_x1 <= line_x2)&&(line_x2 <= box_x2)&&(box_y1 <= line_y2)&&(line_y2 <= box_y2) ){
			*out_line_x1 = line_x1;
			*out_line_y1 = line_y1;
			*out_line_x2 = line_x2;
			*out_line_y2 = line_y2;
			return true;
		}
	}

	int x, y;

	//上辺
	if( Collide2DLine(box_x1, box_y1, box_x2, box_y1, line_x1, line_y1, line_x2, line_y2, &x, &y) == true ){
		//始?が四角?の内側なら終?を、違えば（＝終?が内側）始?を書き換える。
		if( (box_x1 <= line_x1)&&(line_x1 <= box_x2)&&(box_y1 <= line_y1)&&(line_y1 <= box_y2) ){
			line_x2 = x;
			line_y2 = y;
		}
		else{
			line_x1 = x;
			line_y1 = y;
		}
	}
	//右辺
	if( Collide2DLine(box_x2, box_y1, box_x2, box_y2, line_x1, line_y1, line_x2, line_y2, &x, &y) == true ){
		//始?が四角?の内側なら終?を、違えば（＝終?が内側）始?を書き換える。
		if( (box_x1 <= line_x1)&&(line_x1 <= box_x2)&&(box_y1 <= line_y1)&&(line_y1 <= box_y2) ){
			line_x2 = x;
			line_y2 = y;
		}
		else{
			line_x1 = x;
			line_y1 = y;
		}
	}
	//下辺
	if( Collide2DLine(box_x2, box_y2, box_x1, box_y2, line_x1, line_y1, line_x2, line_y2, &x, &y) == true ){
		//始?が四角?の内側なら終?を、違えば（＝終?が内側）始?を書き換える。
		if( (box_x1 <= line_x1)&&(line_x1 <= box_x2)&&(box_y1 <= line_y1)&&(line_y1 <= box_y2) ){
			line_x2 = x;
			line_y2 = y;
		}
		else{
			line_x1 = x;
			line_y1 = y;
		}
	}
	//左辺
	if( Collide2DLine(box_x1, box_y2, box_x1, box_y1, line_x1, line_y1, line_x2, line_y2, &x, &y) == true ){
		//始?が四角?の内側なら終?を、違えば（＝終?が内側）始?を書き換える。
		if( (box_x1 <= line_x1)&&(line_x1 <= box_x2)&&(box_y1 <= line_y1)&&(line_y1 <= box_y2) ){
			line_x2 = x;
			line_y2 = y;
		}
		else{
			line_x1 = x;
			line_y1 = y;
		}
	}

	//改めて四角?に収まるか確認
	if( (box_x1 <= line_x1)&&(line_x1 <= box_x2)&&(box_y1 <= line_y1)&&(line_y1 <= box_y2) ){
		if( (box_x1 <= line_x2)&&(line_x2 <= box_x2)&&(box_y1 <= line_y2)&&(line_y2 <= box_y2) ){
			*out_line_x1 = line_x1;
			*out_line_y1 = line_y1;
			*out_line_x2 = line_x2;
			*out_line_y2 = line_y2;
			return true;
		}
	}

	return false;
}

//! @brief 観測?から対象?への 距離判定・角度算出
//! @param pos_x 観測?のX座標
//! @param pos_y 観測?のY座標
//! @param pos_z 観測?のZ座標
//! @param rx 観測?の水平角度
//! @param ry 観測?の垂直角度
//! @param target_x 対象?のX座標
//! @param target_y 対象?のY座標
//! @param target_z 対象?のZ座標
//! @param checkdist 判定距離（0.0f以下で判定無効）
//! @param out_rx 対象?への水平角度（π?-π）を受け取る?イン?（NULL可）
//! @param out_ry 対象?への垂直角度を受け取る?イン?（NULL可）
//! @param out_dist2 対象?への距離<b>の二乗</b>を受け取る?イン?（NULL可）
//! @return 成功：true 失敗：false
//! @warning out_dist2は距離の<b>二乗</b>です。必要に応じて改めて sqrtf()関数 などを用いてください。
//! @attention 引数 checkdist に有効な距離を?えた場合は、観測?から対象?への距離判定も行います。指定された距離より離れている場合、角度を計算せずに false を返します。
//! @attention 逆に、引数 checkdist に0.0f以下を?えた場合、距離による判定を行いません。関数は常に true を返します。
bool CheckTargetAngle(float pos_x, float pos_y, float pos_z, float rx, float ry, float target_x, float target_y, float target_z, float checkdist, float *out_rx, float *out_ry, float *out_dist2)
{
	float x, y, z;
	float dist2 = 0.0f;
	float angleX, angleY;

	x = target_x - pos_x;
	y = target_y - pos_y;
	z = target_z - pos_z;

	if( (checkdist > 0.0f)||(out_dist2 != NULL) ){
		dist2 = (x*x + y*y + z*z);
	}

	if( checkdist > 0.0f ){
		if( dist2 > checkdist * checkdist ){
			return false;
		}
	}

	if( out_rx != NULL ){
		angleX = atan2f(z, x) - rx;
		angleX = AngleNormalization(angleX);

		*out_rx = angleX;
	}
	if( out_ry != NULL ){
		angleY = atan2f(y, sqrtf(x*x + z*z))  - ry;

		*out_ry = angleY;
	}
	if( out_dist2 != NULL ){ *out_dist2 = dist2; }

	return true;
}

//! @brief ベクトル正規化
//! @param vx vxの?イン?
//! @param vy vyの?イン?
//! @param vz vzの?イン?
//! @return 正規化前のベクトルの長さ
float VectorNormalization(float *vx, float *vy, float *vz)
{
	float r = sqrtf((*vx) * (*vx) + (*vy) * (*vy) + (*vz) * (*vz));
	if( r > 0.0f ){
		*vx /= r;
		*vy /= r;
		*vz /= r;
	}
	return r;
}