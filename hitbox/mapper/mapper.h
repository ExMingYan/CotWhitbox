#ifndef MAPPER_H
#define MAPPER_H

#include "utils/searcher.h"
#include "utils/singleton.hpp"
#include "entity/objects.h"
#include "unreal_engine/World.h"

#define FASTCALL __fastcall*

namespace signatures {
	static std::string uworld = "48 * * * * * * 48 8B * * * 48 8B C7 48 8B * * * 48 * * * 5E C3";
	static std::string screen = "40 55 53 56 57 41 56 48 8D * * * * * * 48 * * * * * * 48 * * * \
								 * * * 48 33 C4 48 89 * * * * * 45 0F B6 F1 49 8B D8 48 8B F2 48 \
								 8B F9 48 85 C9 0F * * * * * E8 * * * * 48 85 C0 0F * * * * * 48 \
								 8B * * 48 85 D2 0F * * * * * 48 8B 08 4C 8D * * 0F 57 C0";
	static std::string objects = "48 8B * * * * * E8 * * * * 48 85 C0 74 * 48 8B * 48 8B * FF * \
								 * * * * 48 85 C0 74 * 8B 00 48 83 C4 * C3";
	static std::string IsBattleReadyPattern = "E8 ? ? ? ? 84 C0 74 ? 8B 83 ? ? ? ? 85";
	static std::string IsBattleEndPattern = "E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 8B 8B ? ? ? ? 85";
}

class mapper : public singleton<mapper> {
private:
	typedef bool(*WorldToScreenFunc)(void* Player, const FVector& WorldPosition, FVector2D& ScreenPosition, bool bPlayerViewportRelative);
	typedef bool (FASTCALL IsBattleReady)(GameInstance* instance);
	typedef bool (FASTCALL IsBattleEnd)(GameInstance* instance);
public:
	UWorld** uworld;
	GameInstance** Instance;
	WorldToScreenFunc WorldToScreen;
	IsBattleReady isBattleReady;
	IsBattleEnd isBattleEnd;

public:
	bool initliaze();
	bool checks();
};

#endif // MAPPER_H