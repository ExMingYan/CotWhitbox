#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <iostream>
#include <windows.h>

#include "description.h"

#include "imgui/imgui.h"

#include "utils/inirw.h"

namespace configurs {
	extern ImFont* mainfont;
	extern ImFont* icon_awesmoe;
	extern ImFont* icon_brands;
	extern bool dp1;
	extern bool dp2;
	extern bool passineffective;
	extern float alpha;
	extern float thickness;
	extern int language;
	extern std::vector<const char*> passname;
	extern std::vector<const char*> alphaname;
	extern std::vector<const char*> thicknessname;
	extern std::vector<descript> distancedesc;
	extern std::vector<const char*> languages;
	void GetIniConfig();
}

#endif // CONFIGURATION_H
