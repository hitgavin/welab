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

#ifndef WELAB__EXTENSION__PLUGIN_MANAGER_P_HPP_
#define WELAB__EXTENSION__PLUGIN_MANAGER_P_HPP_

#include "extension/plugin_spec.hpp"
#include "extension/plugin_manager.hpp"

#include <QElapsedTimer>
#include <QMutex>
#include <QObject>
#include <QReadWriteLock>
#include <QScopedPointer>
#include <QSet>
#include <QStringList>
#include <QWaitCondition>

#include <atomic>
#include <queue>

QT_BEGIN_NAMESPACE
class QTime;
class QTimer;
class QEventLoop;
QT_END_NAMESPACE

namespace extension {
class PluginManager;

namespace internal {
class PluginSpecPrivate;

class EXTENSION_EXPORT PluginManagerPrivate : public QObject {
  Q_OBJECT
public:
  PluginManagerPrivate(PluginManager *plugin_manager);
  ~PluginManagerPrivate() override;
  // Object pool operations
  void addObject(QObject *obj);
  void removeObject(QObject *obj);

  // Plugin operations
  void checkForProblematicPlugins();
  void loadPlugins();
  void shutdown();
  void setPluginPaths(const QStringList &paths);
  const QVector<extension::PluginSpec *> loadQueue();
  void loadPlugin(PluginSpec *spec, PluginSpec::State dest_state);
  void resolveDependendies();
  void enableDependenciesIndirectly();
  void initProfiling();
  void profilingSummary() const;
  void profilingReport(const char *what, const PluginSpec *spec = nullptr);
  void setSettings(utils::WlSettings *settings);
  void setGlobalSettings(utils::WlSettings *settings);
  void readSettings();
  void writeSettings();

  // Look in argument descriptions of the specs for the option.
  PluginSpec *pluginForOption(const QString &option, bool *requires_argument) const;
  PluginSpec *pluginByName(const QString &name) const;

  QHash<QString, QVector<PluginSpec *>> plugin_categories;
  QVector<PluginSpec *> plugin_specs;
  QStringList plugin_paths;
  QString plugin_iid;
  QVector<QObject *> all_objects;
  // Plugins/Ignored from install settings
  QStringList default_disabled_plugins;
  // Plugins/ForceEnabled from install settings
  QStringList default_enabled_plugins;
  QStringList disabled_plugins;
  QStringList force_enabled_plugins;
  // delayed initialization
  QTimer *delayed_initialize_timer = nullptr;
  std::queue<PluginSpec *> delayed_initialize_queue;
  // asynchronous shutdown
  // plugins that have requested async shutdown
  QSet<PluginSpec *> asynchronous_plugins;
  // used for async shutdown
  QEventLoop *shutdown_event_loop = nullptr;

  QStringList arguments;
  QStringList arguments_for_restart;
  QScopedPointer<QElapsedTimer> profile_timer;
  QHash<const PluginSpec *, int> profile_total;
  int profile_elapsed_ms = 0;
  unsigned profiling_verbosity = 0;
  utils::WlSettings *settings = nullptr;
  utils::WlSettings *global_settings = nullptr;

  mutable QReadWriteLock lock;

  bool is_initialization_done = false;
  bool enable_crash_check = true;

  QHash<QString, std::function<bool()>> scenarios;
  QString requested_scenario;
  std::atomic_bool is_scenario_running = false;
  std::atomic_bool is_scenario_finished = false;
  bool scenario_fully_initialized = false;
  QMutex scenario_mutex;
  QWaitCondition scenario_wait_condation;

  PluginManager::ProcessData welab_process_data;

private:
  void nextDelayedInitialize();
  void asyncShutdownFinished();
  void readPluginPaths();
  bool loadQueue(PluginSpec *spec, QVector<extension::PluginSpec *> &queue,
                 QVector<extension::PluginSpec *> &circularity_check_queue);
  void stopAll();
  void deleteAll();

  PluginManager *q;
};
}  // namespace internal
}  // namespace extension

#endif