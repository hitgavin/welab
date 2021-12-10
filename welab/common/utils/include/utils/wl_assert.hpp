/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2021, gavin.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/
#ifndef WELAB__UTILS__WL_ASSERT_HPP_
#define WELAB__UTILS__WL_ASSERT_HPP_

#include "utils_global.hpp"

namespace utils {
UTILS_EXPORT void writeAssertLocation(const char *msg);
UTILS_EXPORT void dumpBacktrace(int max_depth);
}  // namespace utils

#define WL_ASSERT_STRINGIFY_HELPER(x) #x
#define WL_ASSERT_STRINGIFY(x) WL_ASSERT_STRINGIFY_HELPER(x)
// clang-format off
#define WL_ASSERT_STRING(cond) \
  ::utils::writeAssertLocation("\"" cond"\" in file " __FILE__ ", line " WL_ASSERT_STRINGIFY(__LINE__))

#define WL_ASSERT(cond, action) if (Q_LIKELY(cond)) {} else { WL_ASSERT_STRING(#cond); action; } do {} while(0)
#define WL_CHECK(cond) if (Q_LIKELY(cond)) {} else { WL_ASSERT_STRING(#cond); } do {} while(0)
#define WL_GUARD(cond) ((Q_LIKELY(cond)) ? true : (WL_ASSERT_STRING(#cond), false))
// clang-format on
#endif