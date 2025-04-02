#pragma once
#include <stdint.h>
#include "object.h"
#include "props.h"

struct ccmd {
	uint8_t padding0[0x28];			//0x0
	int32_t cancelflag;				//0x28
	struct {
		int32_t commandcount;		//0x0
		uint32_t padding0;			//0x4
		struct {
			uint32_t command;		//0x0
			bool presspksametime;	//0x4
			uint8_t padding1[0xB];	//0x5
			int32_t chargeframe;	//0x10
			uint32_t padding2[3];	//0x14
		} *commands;				//0x8
	} *commandtable;				//0x30
	struct {
		uint8_t padding0[0x10];		//0x0
		int32_t type;				//0x10
		uint32_t padding1[2];		//0x14
		int32_t prriority;			//0x1C
		uint64_t padding2;			//0x20
		uint32_t needpower;			//0x28
		uint32_t padding3[2];		//0x2C
		uint32_t groundlimit : 2;	//0x34
		uint32_t padding4 : 2;
		uint32_t onlyreverse : 1;
		uint32_t padding5 : 1;
		uint32_t onlyrunning : 1;
		uint32_t padding6 : 3;
		uint32_t autoturning : 1;
		uint32_t padding7 : 2;
		uint32_t onlygc : 1;
		uint32_t padding8 : 1;
		uint32_t spglimit : 3;
		uint32_t overheatlimit : 2;
		uint32_t padding9 : 1;
		uint32_t multilinelimit : 1;
		int32_t actionnumber;		//0x38
	} *moveinfo;
};

struct ccmdptrnext
{
	uint8_t _0x0[0x78];				//0x0
	struct {
		ccmd* ptrlist;				//0x0
	} *next;						//0x78
	uint8_t _0x80[0x38];			//0x80
	int32_t count;					//0xB8
};

#define NEXT2OFFSET 0x4A0

struct ccmdptr
{
	uint8_t _0x0[0x10];				//0x0
	struct {
		uint8_t _0x0[NEXT2OFFSET];	//0x0
		struct {
			ccmdptrnext* commonptr;	//0x0
			uint8_t _0x8[0x30];		//0x8
			ccmdptrnext* p1ptr;		//0x38
			uint8_t _0x40[0x68];	//0x40
			ccmdptrnext* p2ptr;		//0xA8
		} *next;					//0x498
	} *next;						//0x10
};

struct subobject
{
	char _0x0[0x20];				//0x0
	ccmdptr* ccmd;					//0x20
	char _0x28[0x10];				//0x28
	projectile_list* props_list;	//0x38
};

#define SUBOBJOFFSET 0x75C48
#define BLACKOUTOFFSET 0x75FD0
#define READYOFFSET 0x85448

_declspec(align(8))
struct objects
{
	void* padding0[0x3];								//0x0
	object* p1;											//0x18
	object* p2;											//0x20
	uint8_t padding1[SUBOBJOFFSET - 0x28];				//0x28
	subobject* subobjects;
	uint8_t paddingblackout[BLACKOUTOFFSET - SUBOBJOFFSET - 0x8];
	int32_t blackout;
	uint8_t paddingready[READYOFFSET - BLACKOUTOFFSET - 0x4];
	int32_t ready;
	uint32_t paddingend[0x5];
	bool isBattleEnd;
};
static_assert(offsetof(objects, ready) == READYOFFSET, "Wrong offset for objects::ready");
