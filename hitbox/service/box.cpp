#include "box.h"

namespace hitboxes {
	std::vector<BoxToDraw> box::boxesToDraw;
	std::vector<LineToDraw> line::linesToDraw;

	void box::DrawBoxes() {
		if (!boxesToDraw.empty())
		{
			for (const auto& box : boxesToDraw) {
				control::draw_box(box.min, box.max, box.alpha, box.thickness, box.color);
			}
			boxesToDraw.clear();
		}
	}

	bool box::foreground(range& r, ImColor color)
	{
		runable* invoke = runable::instance();

		FVector w1{ r.left, r.depth, r.top };
		FVector2D s1{};
		invoke->screen(w1, s1);

		FVector w3{ r.right, r.depth, r.bottom };
		FVector2D s3{};
		invoke->screen(w3, s3);

		ImVec2 rect_min(s1.X, s1.Y, nullptr);
		ImVec2 rect_max(s3.X, s3.Y, nullptr);

		boxesToDraw.push_back(BoxToDraw{ rect_min, rect_max, color, alpha, thickness });
		return true;
	}

	bool attack::resolve(object* obj, actions_entry entry, ActionLine pLine, description desc, bool passineffect) {
		for (uint32_t i = 0; i < pLine.capacity; i++) {
			ActionStructure action = pLine.actions[i];
			if (action.frame != obj->now)
				continue;

			Attack box = action.attack;
			AttackData atccs = obj->atks->atccs[box.DataID];
			if (desc.types.attack != atccs.types)
				continue;

			if (passineffect) {

				if (box.HitOrGuard && (obj->AttackHitorGuard | obj->AttackNotGuard) == 0)
					continue;
				else if (box.HitNotGuard && obj->AttackNotGuard == 0)
					continue;

				if (atccs.hitBackDir == HitBackDirID::IneffectiveBack && !obj->positive())
					continue;
				else if (atccs.hitBackDir == HitBackDirID::IneffectiveFront && obj->positive())
					continue;
				else if (atccs.hitBackDir == HitBackDirID::IneffectiveAll)
					continue;

				CategoryID enemyCategory = obj->enemycategory();
				if (atccs.IneffectiveStand && enemyCategory == CategoryID::Stand)
					continue;
				if (atccs.IneffectiveCrouch && enemyCategory == CategoryID::Crouch)
					continue;
				if (atccs.IneffectiveAir && enemyCategory == CategoryID::OnAir)
					continue;

				SubCategoryID enemySubCategory = obj->enemysubcategory();
				if (atccs.IneffectiveNotGuard && enemySubCategory != SubCategoryID::Defense)
					continue;
			}

			range r(obj, box.rect);
			foreground(r, desc.color);
		}
		return true;
	}

	bool body::resolve(object* obj, actions_entry entry, ActionLine pLine, description desc, bool passineffect) {
		for (uint32_t i = 0; i < pLine.capacity; i++) {
			ActionStructure action = pLine.actions[i];
			if (action.frame != obj->now)
				continue;

			range r(obj, action.collision.rect);
			foreground(r, desc.color);
		}
		return true;
	}

	bool hit::resolve(object* obj, actions_entry entry, ActionLine pLine, description desc, bool passineffect) {
		for (uint32_t i = 0; i < pLine.capacity; i++) {
			ActionStructure action = pLine.actions[i];
			Collision box = action.collision;
			if (desc.types.collision != box.Type || action.frame != obj->now)
				continue;

			if (box.Type == CollisionTypes::normal)
			{
				if (entry.CategoryID == CategoryID::OnAir && entry.SubCategoryID == SubCategoryID::BeAttack && obj->onairhitframe == 0)
					continue;
				if (obj->avoidhitframe)
					continue;
			}

			if (box.Type == CollisionTypes::ground && obj->fallenhitframe == 0)
				continue;

			if (box.Type == CollisionTypes::capture && obj->avoidthrowframe)
				continue;

			range r(obj, box.rect);
			foreground(r, desc.color);
		}
		return true;
	}

	bool attack::propresolve(projectile* obj, actions_entry entry, ActionLine pLine, description desc, bool passineffect) {
		for (uint32_t i = 0; i < pLine.capacity; i++) {
			ActionStructure action = pLine.actions[i];
			if (action.frame != obj->now)
				continue;

			Attack box = action.attack;
			AttackData atccs = obj->atks->atccs[box.DataID];
			if (desc.types.attack != atccs.types)
				continue;

			if (passineffect) {
				if (atccs.hitBackDir == HitBackDirID::IneffectiveBack && !obj->positive())
					continue;
				else if (atccs.hitBackDir == HitBackDirID::IneffectiveFront && obj->positive())
					continue;
				else if (atccs.hitBackDir == HitBackDirID::IneffectiveAll)
					continue;

				CategoryID enemyCategory = obj->enemycategory();
				if (atccs.IneffectiveStand && enemyCategory == CategoryID::Stand)
					continue;
				if (atccs.IneffectiveCrouch && enemyCategory == CategoryID::Crouch)
					continue;
				if (atccs.IneffectiveAir && enemyCategory == CategoryID::OnAir)
					continue;

				SubCategoryID enemySubCategory = obj->enemysubcategory();
				if (atccs.IneffectiveNotGuard && enemySubCategory != SubCategoryID::Defense)
					continue;
			}

			range r(obj, box.rect);
			foreground(r, desc.color);
		}
		return true;
	}

	bool body::propresolve(projectile* obj, actions_entry entry, ActionLine pLine, description desc, bool passineffect) {
		for (uint32_t i = 0; i < pLine.capacity; i++) {
			ActionStructure action = pLine.actions[i];
			if (action.frame != obj->now)
				continue;

			range r(obj, action.collision.rect);
			foreground(r, desc.color);
		}
		return true;
	}

	bool hit::propresolve(projectile* obj, actions_entry entry, ActionLine pLine, description desc, bool passineffect) {
		for (uint32_t i = 0; i < pLine.capacity; i++) {
			ActionStructure action = pLine.actions[i];
			Collision box = action.collision;
			if (desc.types.collision != box.Type || action.frame != obj->now)
				continue;

			range r(obj, box.rect);
			foreground(r, desc.color);
		}
		return true;
	}

	void line::DrawLines() {
		if (!linesToDraw.empty())
		{
			for (const auto& line : linesToDraw) {
				control::draw_line(line.start, line.end, line.alpha, line.thickness, line.color);
			}
			linesToDraw.clear();
		}
	}

	bool line::GetEnemyBodyWidth(object* obj, float& outX)
	{
		object* enemy = obj->enemy;
		actions_entry entry = enemy->acts->entry[enemy->action];
		for (size_t i = 0; i < entry.LineCount; i++)
		{
			ActionLine line = entry.Line[i];
			if (line.LineID == ActionLineID::Body)
			{
				for (size_t j = 0; j < line.capacity; j++)
				{
					ActionStructure actiondata = line.actions[j];
					if (actiondata.frame != enemy->now)
						continue;
					float width = actiondata.collision.rect.w / 2;
					outX += obj->left() ? -width : width;
					return true;
				}
			}
		}
		return false;
	}

	bool line::foreground(float x, float depth, ImColor color)
	{
		runable* invoke = runable::instance();
		FVector w1{ x * 10, depth, 240.0f };
		FVector2D s1;
		invoke->screen(w1, s1);

		FVector w2{ x * 10, depth };
		FVector2D s2;
		invoke->screen(w2, s2);

		ImVec2 p1(s1.X, s1.Y, nullptr);
		ImVec2 p2(s2.X, s2.Y, nullptr);
		linesToDraw.push_back(LineToDraw{ p1, p2, color, alpha, thickness });
		return true;
	}

	bool line::resolve(object* obj, descript desc, size_t index)
	{
		if (obj->action != 0)
			return false;
		float x = obj->x;
		switch (index)
		{
		case 0:
			x += obj->left() ? obj->cbas->ADistance : -(obj->cbas->ADistance);
			break;
		case 1:
			x += obj->left() ? obj->cbas->BDistance : -(obj->cbas->BDistance);
			break;
		case 2:
			x += obj->left() ? obj->cbas->CDistance : -(obj->cbas->CDistance);
			break;
		case 3:
			x += obj->left() ? obj->cbas->DDistance : -(obj->cbas->DDistance);
			break;
		}
		foreground(x, 0.0f, desc.color);
		return true;
	}
};
