#include "dispatcher.h"
#include "utils/logger.hpp"

namespace hitboxes {
	context contexts[] = {
		{ActionLineID::Body, new body},
		{ActionLineID::Hit, new hit},
		{ActionLineID::Attack, new attack},
	};

	bool dispatcher::dispatch() {
		auto mapping = mapper::instance();
		if (!mapping->checks())
			return false;

		auto objs = *mapping->objs;
		auto p1 = objs->p1;
		auto p2 = objs->p2;

		for (description& desc : descriptions) {
			if (!desc.activity)
				continue;

			if (configurs::dp1) {
				actions_entry entry = p1->acts->entry[p1->action];
				for (int i = 0; i < entry.LineCount && entry.Line != nullptr; i++) {
					ActionLine pLine = entry.Line[i];
					if (pLine.LineID != desc.action)
						continue;

					int index = (int)pLine.LineID - 3;
					if (index < 0 || index > 3)
						continue;

					hitboxes::box* box = contexts[index].instance;
					box->alpha = configurs::alpha;
					box->thickness = configurs::thickness;
					box->resolve(p1, entry, pLine, desc, configurs::passineffective);
				}
			}

			if (configurs::dp2) {
				actions_entry entry = p2->acts->entry[p2->action];
				for (int i = 0; i < entry.LineCount && entry.Line != nullptr; i++) {
					ActionLine pLine = entry.Line[i];
					if (pLine.LineID != desc.action)
						continue;

					int index = (int)pLine.LineID - 3;
					if (index < 0 || index > 3)
						continue;

					hitboxes::box* box = contexts[index].instance;
					box->alpha = configurs::alpha;
					box->thickness = configurs::thickness;
					box->resolve(p2, entry, pLine, desc, configurs::passineffective);
				}
			}

			auto list = objs->subobjects->props_list;
			for (; list != nullptr; list = list->next) {
				projectile* props = list->props;
				if (props->isInvalidate())
					continue;
				actions_entry props_entry = props->acts->entry[props->action];
				for (int i = 0; i < props_entry.LineCount; i++) {
					ActionLine pLine = props_entry.Line[i];
					if (pLine.LineID != desc.action)
						continue;

					int index = (int)pLine.LineID - 3;
					if (index < 0 || index > 3)
						continue;

					if (configurs::dp1 && props->owner == p1) {
						hitboxes::box* box = contexts[index].instance;
						box->alpha = configurs::alpha;
						box->thickness = configurs::thickness;
						box->propresolve(props, props_entry, pLine, desc, configurs::passineffective);
					}

					if (configurs::dp2 && props->owner == p2) {
						hitboxes::box* box = contexts[index].instance;
						box->alpha = configurs::alpha;
						box->thickness = configurs::thickness;
						box->propresolve(props, props_entry, pLine, desc, configurs::passineffective);
					}
				}
			}
		}

		for (size_t i = 0 ; i < configurs::distancedesc.size(); i++)
		{
			auto disdesc = configurs::distancedesc[i];
			if (disdesc.activity)
			{
				if (configurs::dp1)
				{
					hitboxes::line lineDrawer;
					lineDrawer.alpha = configurs::alpha;
					lineDrawer.thickness = configurs::thickness;
					lineDrawer.resolve(p1, disdesc, i);
				}

				if (configurs::dp2)
				{
					hitboxes::line lineDrawer;
					lineDrawer.alpha = configurs::alpha;
					lineDrawer.thickness = configurs::thickness;
					lineDrawer.resolve(p2, disdesc, i);
				}
			}
		}
		return true;
	}
}
