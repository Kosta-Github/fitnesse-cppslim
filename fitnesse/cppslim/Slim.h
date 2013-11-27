//
// Copyright (C) 2013 by Konstantin (Kosta) Baumann. All Rights Reserved.
//

#pragma once

#include <cassert>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// since VS2012/VS2013 does not support the "noexcept" keyword which
// is essential for leveraging move-semantics for several
// STL containers and algorithms we use a dedicated macro for
// it for now (until VS also supports it; maybe VS2014?)
#if !defined(SLIM_NOEXCEPT)
#  if defined(_MSC_VER) && (_MSC_VER <= 1800)
#    define SLIM_NOEXCEPT throw()
#  else // defined(_MSC_VER) && (_MSC_VER <= 1800)
#    define SLIM_NOEXCEPT noexcept
#  endif // defined(_MSC_VER) && (_MSC_VER <= 1800)
#endif // !defined(SLIM_NOEXCEPT)

namespace slim { }
