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

#ifndef WELAB__UTILS__HOST_OS_INFO_HPP_
#define WELAB__UTILS__HOST_OS_INFO_HPP_

#include "os_specific_aspects.hpp"
#include "utils_global.hpp"

#include <QString>

#ifdef Q_OS_WIN
#define WELAB_HOST_EXE_SUFFIX WELAB_WIN_EXE_SUFFIX
#else
#define WELAB_HOST_EXE_SUFFIX
#endif  // Q_OS_WIN

namespace utils {
class UTILS_EXPORT HostOsInfo {
public:
  static constexpr OsType hostOs() {
#if defined(Q_OS_WIN)
    return OS_TYPE_WINDOWS;
#elif defined(Q_OS_LINUX)
    return OS_TYPE_LINUX;
#elif defined(Q_OS_MAC)
    return OS_TYPE_MAC;
#elif defined(Q_OS_UNIX)
    return OS_TYPE_OTHER_UNIX;
#else
    return OS_TYPE_OTHER;
#endif
  }

  enum HostArchitecture {
    HOST_ARCHITECTURE_X86,
    HOST_ARCHITECTURE_AMD64,
    HOST_ARCHITECTURE_ITANIUM,
    HOST_ARCHITECTURE_ARM,
    HOST_ARCHITECTURE_UNKNOWN
  };
  static HostArchitecture hostArchitecture();

  static constexpr bool isWindowsHost() { return hostOs() == OS_TYPE_WINDOWS; }
  static constexpr bool isLinuxHost() { return hostOs() == OS_TYPE_LINUX; }
  static constexpr bool isMacHost() { return hostOs() == OS_TYPE_MAC; }
  static constexpr bool isAnyUnixHost() {
#ifdef Q_OS_UNIX
    return true;
#else
    return false;
#endif
  }

  static bool isRunningUnderRosetta();
  static QString withExecutableSuffix(const QString &executable) {
    return os_specific_aspects::withExecutableSuffix(hostOs(), executable);
  }
  static void setOverrideFileNameCaseSensitivity(Qt::CaseSensitivity sensitivity);
  static void unsetOverrideFileNameCaseSensitivity();

  static Qt::CaseSensitivity fileNameCaseSensitivity() {
    return use_override_file_name_case_sensitivity_ ? override_file_name_case_sensitivity_
                                                    : os_specific_aspects::fileNameCaseSensitivity(hostOs());
  }

  static QChar pathListSeparator() { return os_specific_aspects::pathListSeparator(hostOs()); }

  static Qt::KeyboardModifier controlModifier() { return os_specific_aspects::controlModifier(hostOs()); }

  static bool canCreateOpenGLContext(QString *error_msg);

private:
  static Qt::CaseSensitivity override_file_name_case_sensitivity_;
  static bool use_override_file_name_case_sensitivity_;
};
}  // namespace utils

#endif