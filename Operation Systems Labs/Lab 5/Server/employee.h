#pragma once

#include <iostream>

struct employee {
	int num;
	char name[20];
	double hours;

	friend std::istream& operator>>(std::istream& in, employee& emp);
	friend std::ostream& operator<<(std::ostream& out, const employee& emp);
};