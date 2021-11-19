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

#ifndef WELAB__CONSOLE_BRIDGE__CONSOLE_BRIDGE_EXPORT_HPP_
#define WELAB__CONSOLE_BRIDGE__CONSOLE_BRIDGE_EXPORT_HPP_

#include <macros/class_forward.hpp>
#include <macros/declare_ptr.hpp>

// clang-format off
#ifdef CONSOLE_BRIDGE_STATIC_DEFINE
#  define CONSOLE_BRIDGE_DLLAPI
#  define CONSOLE_BRIDGE_NO_EXPORT
#else
#  ifndef CONSOLE_BRIDGE_DLLAPI
#    ifdef console_bridge_EXPORTS
        /* We are building this library */
#      define CONSOLE_BRIDGE_DLLAPI __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define CONSOLE_BRIDGE_DLLAPI __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef CONSOLE_BRIDGE_NO_EXPORT
#    define CONSOLE_BRIDGE_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef CONSOLE_BRIDGE_DEPRECATED
#  define CONSOLE_BRIDGE_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef CONSOLE_BRIDGE_DEPRECATED_EXPORT
#  define CONSOLE_BRIDGE_DEPRECATED_EXPORT CONSOLE_BRIDGE_DLLAPI CONSOLE_BRIDGE_DEPRECATED
#endif

#ifndef CONSOLE_BRIDGE_DEPRECATED_NO_EXPORT
#  define CONSOLE_BRIDGE_DEPRECATED_NO_EXPORT CONSOLE_BRIDGE_NO_EXPORT CONSOLE_BRIDGE_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define CONSOLE_BRIDGE_NO_DEPRECATED
#endif

// clang-format on

#endif