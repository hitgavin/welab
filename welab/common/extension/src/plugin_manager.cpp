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

void PluginManager::addObject(QObject *obj) { d->addObject(obj); }

void PluginManager::removeObject(QObject *obj) { d->removeObject(obj); }

QVector<QObject *> PluginManager::allObjects() { return d->all_objects; }

QReadWriteLock *PluginManager::listLock() { return &d->lock; }

void PluginManager::loadPlugins() { d->loadPlugins(); }

bool PluginManager::hasError() {
  return std::any_of(plugins().begin(), plugins().end(), [](PluginSpec *spec) {
    // only show errors on startup if plugin is enabled.
    return spec->hasError() && spec->isEffectivelyEnabled();
  });
}

const QStringList PluginManager::allErrors() {
  QStringList sl;
  QVector<PluginSpec *> ps;
  std::copy_if(plugins().begin(), plugins().end(), ps.begin(),
               [](const PluginSpec *spec) { return spec->hasError() && spec->isEffectivelyEnabled(); });
  std::transform(ps.begin(), ps.end(), sl.begin(),
                 [](const PluginSpec *spec) { return spec->name().append(": ").append(spec->errorString()); });
}

const QSet<PluginSpec *> PluginManager::pluginsRequiringPlugin(PluginSpec *spec) {
  QSet<PluginSpec *> depending_plugins({spec});
  for (PluginSpec *spec : d->loadQueue()) {
    if (spec->requiresAny(depending_plugins)) {
      depending_plugins.insert(spec);
    }
  }
  depending_plugins.remove(spec);
  return depending_plugins;
}

const QSet<PluginSpec *> PluginManager::pluginsRequiredByPlugin(PluginSpec *spec) {
  QSet<PluginSpec *> recursive_dependencies;
  recursive_dependencies.insert(spec);
  std::queue<PluginSpec *> queue;
  queue.push(spec);

  while (!queue.empty()) {
    PluginSpec *check_spec = queue.front();
    queue.pop();
    const QHash<PluginDependency, PluginSpec *> deps = check_spec->dependencySpecs();
    for (auto dep_it = deps.cbegin(), end = deps.cend(); dep_it != end; ++dep_it) {
      if (dep_it.key().type != PluginDependency::REQUIRED) {
        continue;
      }
      PluginSpec *dep_spec = dep_it.value();
      if (!recursive_dependencies.contains(dep_spec)) {
        recursive_dependencies.insert(dep_spec);
        queue.push(dep_spec);
      }
    }
  }
  recursive_dependencies.remove(spec);
  return recursive_dependencies;
}

void PluginManager::shutdown() { d->shutdown(); }

static QString filled(const QString &s, int min) { return s + QString(qMax(0, min - s.size()), ' '); }

QString PluginManager::systemInformation() {
  QString result;
  return result;
}

QStringList PluginManager::pluginPaths() { return d->plugin_paths; }

void PluginManager::setPluginPaths(const QStringList &paths) { d->setPluginPaths(paths); }

QString PluginManager::pluginIID() { return d->plugin_iid; }

void PluginManager::setPluginIID(const QString &iid) { d->plugin_iid = iid; }

void PluginManager::setSettings(WlSettings *settings) { d->setSettings(settings); }

void PluginManager::setGlobalSettings(WlSettings *settings) { d->setGlobalSettings(settings); }

WlSettings *PluginManager::settings() { return d->settings; }

WlSettings *PluginManager::globalSettings() { return d->global_settings; }

void PluginManager::writeSettings() { d->writeSettings(); }

QStringList PluginManager::arguments() { return d->arguments; }

QStringList PluginManager::argumentsForRestart() { d->arguments_for_restart; }

const QVector<PluginSpec *> PluginManager::plugins() { return d->plugin_specs; }

QHash<QString, QVector<PluginSpec *>> PluginManager::pluginCollections() { return d->plugin_categories; }

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