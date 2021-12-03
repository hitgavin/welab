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

#ifndef WELAB__UTILS__OS_SPECIFIC_ASPECTS_HPP_
#define WELAB__UTILS__OS_SPECIFIC_ASPECTS_HPP_

#include <QString>

#include <algorithm>

#define WELAB_WIN_EXE_SUFFIX ".exe"

namespace utils {

enum OsType { OS_TYPE_WINDOWS, OS_TYPE_LINUX, OS_TYPE_MAC, OS_TYPE_OTHER_UNIX, OS_TYPE_OTHER };

namespace os_specific_aspects {
inline QString withExecutableSuffix(OsType os_type, const QString &executable) {
  QString final_name = executable;
  if (os_type == OS_TYPE_WINDOWS) {
    final_name += QLatin1String(WELAB_WIN_EXE_SUFFIX);
  }

  return final_name;
}

inline Qt::CaseSensitivity fileNameCaseSensitivity(OsType os_type) {
  return os_type == OS_TYPE_WINDOWS || os_type == OS_TYPE_MAC ? Qt::CaseInsensitive : Qt::CaseSensitive;
}

inline QChar pathListSeparator(OsType os_type) { return QLatin1Char(os_type == OS_TYPE_WINDOWS ? ';' : ':'); }

inline Qt::KeyboardModifier controlModifier(OsType os_type) {
  return os_type == OS_TYPE_MAC ? Qt::MetaModifier : Qt::ControlModifier;
}

inline QString pathWithNativeSeparators(OsType os_type, const QString &path_name) {
  if (os_type == OS_TYPE_WINDOWS) {
    const int pos = path_name.indexOf('/');
    if (pos >= 0) {
      QString n = path_name;
      std::replace(std::begin(n) + pos, std::end(n), '/', '\\');
      return n;
    } else {
      const int pos = path_name.indexOf('\\');
      if (pos >= 0) {
        QString n = path_name;
        std::replace(std::begin(n) + pos, std::end(n), '\\', '/');
        return n;
      }
    }
  }
}

}  // namespace os_specific_aspects

}  // namespace utils

#endif