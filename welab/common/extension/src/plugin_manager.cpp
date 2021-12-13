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

#include "extension/plugin_manager.hpp"
#include "extension/plugin_manager_p.hpp"

#include "extension/iplugin.hpp"
#include "extension/plugin_spec.hpp"
#include "extension/plugin_spec_p.hpp"

#include "utils/host_os_info.hpp"
#include "utils/wl_assert.hpp"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QGuiApplication>
#include <QLibrary>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QMetaProperty>
#include <QPushButton>
#include <QSysInfo>
#include <QTextStream>
#include <QTimer>
#include <QWriteLocker>

#include <functional>
#include <memory>

Q_LOGGING_CATEGORY(pluginLog, "wl.extension", QtWarningMsg)

const char C_IGNORED_PLUGIINS[] = "Plugins/Ignored";
const char C_FORCEENABLED_PLUGINS[] = "Plugins/ForceEnabled";
const int DELAYED_INITIALIZE_INTERVAL = 20;  // ms

using namespace utils;

namespace extension {
using namespace internal;

static std::unique_ptr<internal::PluginManagerPrivate> d = nullptr;
static PluginManager *object = nullptr;

PluginManager *PluginManager::instance() { return object; }

PluginManager::PluginManager() {
  object = this;
  d = std::make_unique<PluginManagerPrivate>(this);
}

PluginManager::~PluginManager() {}

QString PluginManager::pluginIID() { return QString(""); }

void PluginManager::setPluginIID(const QString &) {}

static inline QString getPlatformName() {
  if (HostOsInfo::isMacHost()) {
    return QLatin1String("OS X");
  } else if (HostOsInfo::isAnyUnixHost()) {
    return QLatin1String(HostOsInfo::isLinuxHost() ? "Linux" : "Unix");
  } else if (HostOsInfo::isWindowsHost()) {
    return QLatin1String("Windows");
  }

  return QLatin1String("Unknown");
}

QString PluginManager::platformName() {
  static const QString result = getPlatformName() + " (" + QSysInfo::prettyProductName() + ')';
  return result;
}

}  // namespace extension