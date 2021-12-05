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

#include "utils/host_os_info.hpp"

#include <QCoreApplication>

#if !defined(QT_NO_OPENGL) && defined(QT_GUI_LIB)
#include <QOpenGLContext>
#endif

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

#ifdef Q_OS_MACOS
#include <sys/sysctl.h>
#endif

using namespace utils;

Qt::CaseSensitivity HostOsInfo::override_file_name_case_sensitivity_ = Qt::CaseSensitive;
bool HostOsInfo::use_override_file_name_case_sensitivity_ = false;

#ifdef Q_OS_WIN
static WORD hostProcessorArchitecture() {
  SYSTEM_INFO info;
  GetNativeSystemInfo(&info);
  return info.wProcessorArchitecture;
}
#endif

HostOsInfo::HostArchitecture HostOsInfo::hostArchitecture() {
#ifdef Q_OS_WIN
  static const WORD processorArchitecture = hostProcessorArchitecture();
  switch (processorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64:
      return HostOsInfo::HOST_ARCHITECTURE_AMD64;
    case PROCESSOR_ARCHITECTURE_INTEL:
      return HostOsInfo::HOST_ARCHITECTURE_X86;
    case PROCESSOR_ARCHITECTURE_IA64:
      return HostOsInfo::HOST_ARCHITECTURE_ITANIUM;
    case PROCESSOR_ARCHITECTURE_ARM:
      return HostOsInfo::HOST_ARCHITECTURE_ARM;
    default:
      return HostOsInfo::HOST_ARCHITECTURE_UNKNOWN;
  }
#else
  return HostOsInfo::HOST_ARCHITECTURE_UNKNOWN;
#endif
}

void HostOsInfo::setOverrideFileNameCaseSensitivity(Qt::CaseSensitivity sensitivity) {
  use_override_file_name_case_sensitivity_ = true;
  override_file_name_case_sensitivity_ = sensitivity;
}

void HostOsInfo::unsetOverrideFileNameCaseSensitivity() { use_override_file_name_case_sensitivity_ = false; }

bool HostOsInfo::canCreateOpenGLContext(QString *error_msg) {
#if defined(QT_NO_OPENGL) || !defined(QT_GUI_LIB)
  Q_UNUSED(error_msg);
  return false;
#else
  static const bool can_create = QOpenGLContext().create();
  if (!can_create) {
    *error_msg = QCoreApplication::translate("utils::HostOsInfo", "Cannot create OpenGL context.");
  }
  return can_create;
#endif
}