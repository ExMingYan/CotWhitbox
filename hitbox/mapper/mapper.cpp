#include "mapper.h"
#include "utils/logger.hpp"

bool mapper::initliaze()
{
	this->uworld = reinterpret_cast<UWorld**>(searcher::transform(searcher::search(".text", signatures::uworld)));
	if (this->uworld == nullptr) {
		return false;
	}

	this->objs = reinterpret_cast<objects**>(searcher::transform(searcher::search(".text", signatures::objects)));
	if (this->objs == nullptr) {
		return false;
	}

	this->WorldToScreen = reinterpret_cast<bool(*)(void* Player, const FVector & WorldPosition, FVector2D & ScreenPosition, bool bPlayerViewportRelative)>(searcher::search(".text", signatures::screen));
	if (this->WorldToScreen == nullptr) {
		return false;
	}
	return true;
}

bool mapper::checks() {
	auto objs = *this->objs;
	if (objs == nullptr)
		return false;

	if (objs->p1 == nullptr || objs->p2 == nullptr)
		return false;

	auto ready = objs->ready;
	if (ready <= 2)
		return false;

	auto blackout = objs->blackout;
	if (blackout != 0)
		return false;

	return !objs->isBattleEnd;
}
