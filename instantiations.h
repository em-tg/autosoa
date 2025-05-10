#define FIELDS \
	x(int, a) \
	x(int, b) \
	x(void *, whatever)

#define id(n) my_ ## n
#include "soa_vec.h"
