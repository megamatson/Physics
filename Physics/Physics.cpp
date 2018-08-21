#include "stdafx.h"

#include "Physics.h"

using namespace phys;

void Unit::init(const char* unit) {
	std::stringstream ss(unit);

	std::string token;

	bool numerator = true;
	while (ss.good()) {
		ss >> token;

		if (token == "") continue;

		if (token == "1")
			continue;

		if (token == "/") {
			numerator = false;
			continue;
		}


		auto caret = token.find('^');
		std::string cUnit = token.substr(0, caret);
		int exponent = 1;
		if (caret != std::string::npos) {
			std::stringstream number(token.substr(caret + 1));
			int exp;
			number >> exp;
			if (number.fail())
				throw Exception("Bad exponent in token " + token);
			exponent = exp;
		}

		if (exponent == 0)
			continue;

		if ((units[cUnit] += (numerator) ? exponent : -exponent) == 0)
			units.erase(cUnit);
	}
}

Unit Unit::operator *= (const Unit& u) {
	for (auto unit : u.units) {
		if ((units[unit.first] += unit.second) == 0)
			units.erase(unit.first);
	}

	return *this;
}

Unit Unit::operator * (const Unit& u) const {
	Unit ret;

	for (auto unit : units) {
		ret.units[unit.first] = unit.second;
	}

	for (auto unit : u.units) {
		if ((ret.units[unit.first] += unit.second) == 0)
			ret.units.erase(unit.first);
	}

	return ret;
}

Unit Unit::operator /= (const Unit& u) {
	for (auto unit : u.units) {
		if ((units[unit.first] -= unit.second) == 0)
			units.erase(unit.first);

	}

	return *this;
}

Unit Unit::operator / (const Unit& u) const {
	Unit ret;

	for (auto unit : units) {
		ret.units[unit.first] = unit.second;
	}

	for (auto unit : u.units) {
		if ((ret.units[unit.first] -= unit.second) == 0)
			ret.units.erase(unit.first);

	}

	return ret;
}

bool Unit::operator == (const Unit& u) const {
	for (auto thisUnit = units.begin(), thatUnit = u.units.begin(); ; thisUnit++, thatUnit++) {
		if (thisUnit == units.end() && thatUnit == u.units.end()) return true;

		if (thisUnit->first != thatUnit->first) return false;
		if (thisUnit->second != thatUnit->second) return false;
	}
}


std::ostream& phys::operator<<(std::ostream & o, const Unit & u) {
	bool outputNumerator = false;

	for (auto unit = u.units.begin(); unit != u.units.end(); unit++) {
		assert(unit->second != 0);
		if (unit->second < 0) continue;

		if (outputNumerator)
			o << ' ';
		else
			outputNumerator = true;

		o << unit->first;
		if (unit->second > 1)
			o << '^' << unit->second;
	}

	if (outputNumerator == false)
		o << '1';

	bool outputDenominator = false;

	for (auto unit = u.units.begin(); unit != u.units.end(); unit++) {
		assert(unit->second != 0);
		if (unit->second > 0) continue;

		if (!outputDenominator) {
			outputDenominator = true;
			o << " /";
		}

		o << ' ' << unit->first;

		if (unit->second < -1)
			o << '^' << -unit->second;
	}

	return o;
}


