#include "mapper.h"
#include "utils/logger.hpp"

bool mapper::initliaze()
{
	this->uworld = reinterpret_cast<UWorld**>(searcher::transform(searcher::search(".text", signatures::uworld)));
	if (this->uworld == nullptr) {
		return false;
	}

	this->Instance = reinterpret_cast<GameInstance**>(searcher::transform(searcher::search(".text", signatures::objects)));
	if (this->Instance == nullptr) {
		return false;
	}

	this->WorldToScreen = reinterpret_cast<WorldToScreenFunc>(searcher::search(".text", signatures::screen));
	if (this->WorldToScreen == nullptr) {
		return false;
	}

	this->isBattleReady = reinterpret_cast<IsBattleReady>(searcher::transform(searcher::search(".text", signatures::IsBattleReadyPattern) - 2));
	if (this->isBattleReady == nullptr) {
		return false;
	}

	this->isBattleEnd = reinterpret_cast<IsBattleEnd>(searcher::transform(searcher::search(".text", signatures::IsBattleEndPattern) - 2));
	if (this->isBattleEnd == nullptr) {
		return false;
	}

	return true;
}

bool mapper::checks() {
	auto Instance = *this->Instance;
	if (Instance == nullptr)
		return false;

	if (Instance->p1 == nullptr || Instance->p2 == nullptr)
		return false;

	if (!isBattleReady(Instance))
		return false;

	if (isBattleEnd(Instance))
		return false;

	return true;
}
