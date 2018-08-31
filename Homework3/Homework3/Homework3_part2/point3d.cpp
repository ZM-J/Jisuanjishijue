#include "point3d.h"

point3d point3d::operator-(const point3d & b) const
{
	return point3d(x - b.x, y - b.y, z - b.z);
}

void point3d::operator+=(const point3d &b)
{
	x += b.x, y += b.y, z += b.z;
}

point3d point3d::operator/(const double c) const
{
	return point3d(x / c, y / c, z / c);
}

double abs(const point3d &p)
{
	return sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
}
