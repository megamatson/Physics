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

Unit Unit::sqrt() const {
	Unit ret;

	for (auto unit : units) {
		if (unit.second % 2 != 0)
			throw Exception("Unit does not have a power that is a multiple of two! (" + unit.first + "^" + std::to_string(unit.second) + ")");
		ret.units [unit.first] = unit.second / 2;
	}

	return ret;
}

std::string Unit::toString() const {
	std::stringstream ss;
	bool outputNumerator = false;

	for (auto unit = units.begin(); unit != units.end(); unit++) {
		assert(unit->second != 0);
		if (unit->second < 0) continue;

		if (outputNumerator)
			ss << ' ';
		else
			outputNumerator = true;

		ss << unit->first;
		if (unit->second > 1)
			ss << '^' << unit->second;
	}

	if (outputNumerator == false)
		ss << '1';

	bool outputDenominator = false;

	for (auto unit = units.begin(); unit != units.end(); unit++) {
		assert(unit->second != 0);
		if (unit->second > 0) continue;

		if (!outputDenominator) {
			outputDenominator = true;
			ss << " /";
		}

		ss << ' ' << unit->first;

		if (unit->second < -1)
			ss << '^' << -unit->second;
	}

	return ss.str();
}


Force phys::elecForce(const Charge& e1, const Charge& e2, const Distance& d) {
	return cnstQ::K * e1 * e2 * d.unitV() / (d * d);
}

ForceM phys::elecForce(const Charge& e1, const Charge& e2, const DistanceM& d) {
	return (cnstQ::K * e1 * e2 / (d * d)).magnitude();
}

Force phys::gravForce(const Mass& m1, const Mass& m2, const Distance& d) {
	return cnstQ::G * m1 * m2 / (d * d) * d.unitV();
}

ForceM phys::gravForce(const Mass& m1, const Mass& m2, const DistanceM& d) {
	return cnstQ::G * m1 * m2 / (d * d);
}

VelocityM phys::circularVelocity(const DistanceM& r, const AccelerationM& a) { return (r * a).sqrt(); }

std::string phys::Quantity::toString() const {
	std::stringstream ss;
	ss << quantity << ' ' << unit;
	return ss.str();
}
