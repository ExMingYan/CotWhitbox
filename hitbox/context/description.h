#ifndef DESCRIPTION_H
#define DESCRIPTION_H

#include "imgui/imgui.h"
#include "entity/object.h"
#include <vector>

struct descript {
	std::vector<const char*> multititles;
	bool activity;
	bool open;
	ImColor color;
};

struct description {
	std::vector<const char*> multititles;
	bool activity;
	bool open;
	ImColor color;
	ActionLineID action;
	warpper types;
};

extern std::vector<description> descriptions;
void reload_description_colors();

#endif