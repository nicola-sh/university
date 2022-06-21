#include "employee.h"

std::istream& operator>>(std::istream& in, employee& emp) {
    in >> emp.num >> emp.name >> emp.hours;
    return in;
}

std::ostream& operator<<(std::ostream& out, const employee& emp) {
    out << emp.num << ' ' << emp.name << ' ' << emp.hours;
    return out;
}
