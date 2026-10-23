#pragma once

// Primary Arithvex C++ header. Existing MathEngine headers and symbols remain
// available for callers that depend on the original internal namespace.
#include "MathEngine.h"

// Source-compatible project namespace. A namespace alias keeps type identity
// and exported ABI behavior unchanged.
namespace Arithvex = MathEngine;
