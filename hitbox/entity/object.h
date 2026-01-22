#pragma once
#include <cstddef>
#include <stdint.h>
#include "action.h"

#define ACTIONSOFFSET 0x38
#define ATTACKSOFFSET 0x68
#define ENEMYOFFSET 0x90
#define ZOFFSET 0xA8
#define XOFFOFFSET 0xDC
#define ZOFFOFFSET 0xE4
#define TOWARDOFFSET 0x154
#define ACTIONOFFSET 0x170
#define NOWOFFSET 0x17C
#define HITFLAGOFFSET 0x490
#define CBASOFFSET 0x4B8
#define ONAIRHITFRAMEOFFSET 0x8BC
#define AVOIDHITFRAMEOFFSET 0x8E0

struct CBAS {
	int32_t padding0[0x7];
	int32_t ActionCount;
	int32_t padding1[0x6];
	float ADistance;
	float BDistance;
	float CDistance;
	float DDistance;
	int32_t padding2[0x2A];
	int32_t* actiontable;
};
static_assert(offsetof(CBAS, actiontable) == 0xF0, "CBAS size error");

_declspec(align(8))
struct object
{
	int64_t padding0[CalcFillSize(ACTIONSOFFSET, 0, sizeof(int64_t), 0)];
	actions* acts;
	int64_t padding1[CalcFillSize(ATTACKSOFFSET, ACTIONSOFFSET, sizeof(int64_t))];
	attacks* atks;
	int64_t padding2[CalcFillSize(ENEMYOFFSET, ATTACKSOFFSET, sizeof(int64_t))];
	object* enemy;
	object* owner;
	float x;
	float y;
	float z;
	int32_t padding3[CalcFillSize(XOFFOFFSET, ZOFFSET, sizeof(int32_t), sizeof(float))];
	float xoff;
	float yoff;
	float zoff;
	int32_t padding4[CalcFillSize(TOWARDOFFSET, ZOFFOFFSET, sizeof(int32_t), sizeof(float))];
	int32_t toward;
	int32_t padding5[CalcFillSize(ACTIONOFFSET, TOWARDOFFSET, sizeof(int32_t), sizeof(int32_t))];
	int32_t action;
	int32_t padding6[CalcFillSize(NOWOFFSET, ACTIONOFFSET, sizeof(int32_t), sizeof(int32_t))];
	int32_t now;
	int32_t padding7[CalcFillSize(HITFLAGOFFSET, NOWOFFSET, sizeof(int32_t), sizeof(int32_t))];
	uint32_t hasAttack : 1;
	uint32_t AttackHitorGuard : 1;
	uint32_t AttackNotGuard : 1;
	int32_t padding8[CalcFillSize(CBASOFFSET, HITFLAGOFFSET, sizeof(int32_t), sizeof(int32_t))];
	CBAS* cbas;
	int32_t padding9[CalcFillSize(ONAIRHITFRAMEOFFSET, CBASOFFSET, sizeof(int32_t))];
	int32_t onairhitframe;
	int32_t fallenhitframe;
	int32_t paddingA[CalcFillSize(AVOIDHITFRAMEOFFSET, ONAIRHITFRAMEOFFSET, sizeof(int32_t))];
	int32_t avoidhitframe;
	int32_t avoidthrowframe;

	inline bool left() {
		return this->toward == 0;
	}

	inline CategoryID enemycategory() {
		return this->enemy->acts->entry[this->enemy->action].CategoryID;
	}

	inline SubCategoryID enemysubcategory() {
		return this->enemy->acts->entry[this->enemy->action].SubCategoryID;
	}

	inline bool positive() {
		if (!left())
			return this->x >= this->enemy->x;
		else
			return this->x <= this->enemy->x;
	}
};
static_assert(offsetof(object, avoidhitframe) == AVOIDHITFRAMEOFFSET, "object size error");

struct warpper {
	AttackType attack;
	CollisionTypes collision;

	warpper(AttackType types);
	warpper(CollisionTypes types);
	bool operator== (warpper b);
	bool operator!= (warpper b);
};
