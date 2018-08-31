#include "utility.h"

int sgn(double x)
{
	if (fabs(x) < eps) {
		return 0;
	}
	else if (x < 0) {
		return -1;
	}
	else {
		return 1;
	}
}