#pragma once
#include <cstddef>
#include <stdint.h>
#include "action.h"

enum class spgpos : uint8_t
{
	none,
	front,
	middle,
	last,
	whole
};

#define TOWARDOFFSET 0x154
#define ACTIONOFFSET 0x170
#define NOWFRAOFFSET 0x17C
#define NEXTACOFFSET 0x194
#define HITFLAGOFFSET 0x490
#define CBASOFFSET 0x4B8
#define HPOFFSET	 0x718
#define POWEROFFSET  (HPOFFSET + sizeof(int32_t) * 2)
#define GUARDOFFSET  0x734
#define SPGOFFSET	 0x854
#define OVERHEATOFFSET 0x868
#define REVOFFSET 0x880

#define FILLSIZE(offset1, offset2) ((offset2 - offset1 - sizeof(int32_t)) >> 2)

#define ACTIONFILL FILLSIZE(TOWARDOFFSET, ACTIONOFFSET)
#define NOWFRAFILL FILLSIZE(ACTIONOFFSET, NOWFRAOFFSET)
#define NEXTACFILL FILLSIZE(NOWFRAOFFSET, NEXTACOFFSET)
#define HITFLAGFILL FILLSIZE(NEXTACOFFSET, HITFLAGOFFSET)
#define CBASFILL FILLSIZE(HITFLAGOFFSET, CBASOFFSET)
#define HPFILL FILLSIZE(CBASOFFSET, HPOFFSET)
#define GUARDFILL FILLSIZE(POWEROFFSET, GUARDOFFSET)
#define SPGFILL FILLSIZE(GUARDOFFSET, SPGOFFSET)
#define OVERHEATFILL FILLSIZE(SPGOFFSET, OVERHEATOFFSET)

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
	int64_t padding0[0x7];
	actions* acts;
	int64_t padding1[0x5];
	attacks* atcs;
	int64_t padding2[0x4];
	object* enemy;
	object* owner;
	float x;
	float y;
	float z;
	int32_t padding3[0xC];
	float xoff;
	float yoff;
	float zoff;
	int32_t padding4[0x1B];
	int32_t toward;
	int32_t padding5[ACTIONFILL];
	int32_t action;
	int32_t padding6[NOWFRAFILL];
	int32_t now;
	int32_t padding7[NEXTACFILL];
	int32_t nextaction;
	int32_t paddingHITFLAG[HITFLAGFILL];
	uint32_t hasAttack : 1;
	uint32_t AttackHitorGuard : 1;
	uint32_t AttackNotGuard : 1;
	int32_t paddingI[CBASFILL];
	CBAS* cbas;
	int32_t padding8[HPFILL - 1];
	int32_t hp;
	int32_t displayhp;
	int32_t power;
	int32_t padding9[GUARDFILL];
	int32_t guard;
	int32_t paddingA[SPGFILL];
	spgpos spg;
	uint8_t paddingB[0x3];
	int32_t paddingC[OVERHEATFILL];
	bool overheat;
	uint8_t paddingD[0x3];
	int32_t revcovercd;
	int64_t maxrev;
	int64_t minrev;
	int64_t rev;
	int64_t paddingE;
	int64_t revcoverspeed;
	int32_t paddingG[0x7];
	int32_t onairhitframe;
	int32_t fallenhitframe;
	uint32_t paddingH[0x8];
	int32_t avoidhitframe;
	int32_t avoidthrowframe;

	inline bool left() {
		return this->toward == 0;
	}

	inline CategoryID enemycategory() {
		return this->enemy->acts->entry[this->enemy->action].iCategoryID;
	}

	inline SubCategoryID enemysubcategory() {
		return this->enemy->acts->entry[this->enemy->action].iSubCategoryID;
	}

	inline bool positive() {
		if (!left())
			return this->x >= this->enemy->x;
		else
			return this->x <= this->enemy->x;
	}
};
static_assert(sizeof(object) == 0x8E8, "object size error");

struct warpper {
	AttackType attack;
	CollisionTypes collision;

	warpper(AttackType types);
	warpper(CollisionTypes types);
	bool operator== (warpper b);
	bool operator!= (warpper b);
};
