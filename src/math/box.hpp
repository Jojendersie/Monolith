#pragma once

#include "fixedpoint.hpp"

namespace Math {

	struct WorldBox {
		FixVec3 min;
		FixVec3 max;
	};

} // namespace Math