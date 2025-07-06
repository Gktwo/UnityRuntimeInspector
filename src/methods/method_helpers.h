#pragma once

#include <UnityResolve.hpp>

#define GET_CLASS(module, className) \
	UnityResolve::Get(module)->Get(className)
