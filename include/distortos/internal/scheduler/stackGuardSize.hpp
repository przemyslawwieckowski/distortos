/**
 * \file
 * \brief stackGuardSize constant
 *
 * \author Copyright (C) 2017 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDE_DISTORTOS_INTERNAL_SCHEDULER_STACKGUARDSIZE_HPP_
#define INCLUDE_DISTORTOS_INTERNAL_SCHEDULER_STACKGUARDSIZE_HPP_

#include "distortos/distortosConfiguration.h"

#include <cstddef>

namespace distortos
{

namespace internal
{

/// size of "stack guard", bytes
constexpr size_t stackGuardSize {(CONFIG_STACK_GUARD_SIZE + CONFIG_ARCHITECTURE_STACK_ALIGNMENT - 1) /
		CONFIG_ARCHITECTURE_STACK_ALIGNMENT * CONFIG_ARCHITECTURE_STACK_ALIGNMENT};

}	// namespace internal

}	// namespace distortos

#endif	// INCLUDE_DISTORTOS_INTERNAL_SCHEDULER_STACKGUARDSIZE_HPP_
