#pragma once
#include "utility.h"

class point3d {
public:
	double x, y, z;
	point3d(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
	point3d operator - (const point3d &) const;
	void operator += (const point3d&);
	point3d operator /(const double)const;
	friend double abs(const point3d &);
};