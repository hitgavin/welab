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

#ifndef WELAB__EXTENSION__PLUGIN_MANAGER_HPP_
#define WELAB__EXTENSION__PLUGIN_MANAGER_HPP_

#include "extension/extension_global.hpp"

#include "utils/wl_settings.hpp"

#include <QObject>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QTextStream;
QT_END_NAMESPACE

namespace extension {

class IPlugin;
class PluginSpec;

namespace internal {
class PluginManagerPrivate;
}  // namespace internal

class EXTENSION_EXPORT PluginManager : public QObject {
  Q_OBJECT
public:
  struct ProcessData {
    QString executable;
    QStringList args;
    QString working_path;
    QString settings_path;
  };

  static PluginManager *instance();

  PluginManager();
  ~PluginManager() override;

  // Object pool operations
  static void addObject(QObject *obj);
  static void removeObject(QObject *obj);
  static QVector<QObject *> allObjects();
  static QReadWriteLock *listLock();

  // This is useful for soft dependencies using pure interface
  template <typename T>
  static T *getObject() {
    QReadLocker lock(listLock());
    const QVector<QObject *> all = allObjects();
    for (QObject *obj : all) {
      if (T *result = qobject_cast<T *>(obj)) {
        return result;
      }
    }

    return nullptr;
  }

  template <typename T, typename Predicate>
  static T *getObject(Predicate predicate) {
    QReadLocker lock(listLock());
    const QVector<QObject *> all = allObjects();
    for (QObject *obj : all) {
      if (T *result = qobject_cast<T *>(obj)) {
        if (predicate(result)) {
          return result;
        }
      }
    }

    return nullptr;
  }

  static QObject *getObjectByName(const QString &name);

  // Plugin operations
  static QVector<PluginSpec *> loadQueue();
  static void loadPlugins();
  static QStringList pluginPaths();
  static void setPluginPaths(const QStringList &paths);
  static QString pluginIID();
  static void setPluginIID(const QString &iid);
  static const QVector<PluginSpec *> plugins();
  static bool hasError();
  static const QStringList allErrors();
  static const QSet<PluginSpec *> pluginsRequiringPlugin(PluginSpec *spec);
  static const QSet<PluginSpec *> pluginsRequiredByPlugin(PluginSpec *spec);
  static void checkForProblematicPlugins();

  // Settings
  static void setSettings(utils::WlSettings *settings);
  static utils::WlSettings *settings();
  static void setGlobalSettings(utils::WlSettings *settings);
  static utils::WlSettings *globalSettings();
  static void writeSettings();

  // command line arguments
  static QStringList arguments();
  static QStringList argumentsForRestart();
  static bool parseOptions(const QStringList &args, const QMap<QString, bool> &app_options,
                           QMap<QString, QString> *found_app_options, QString *error_string);
  static void formatOptions(QTextStream &str, int option_indentation, int description_identation);
  static void formatPluginOptions(QTextStream &str, int option_indentation, int description_identation);
  static void formatPluginVersions(QTextStream &str);

  static QString serializedArguments();

  static void setWelabProcessData(const ProcessData &data);
  static ProcessData welabProcessData();

  static void profilingReport(const char *what, const PluginSpec *spec = nullptr);
  static QString platformName();
  static bool isInitializationDone();

  static void remoteArguments(const QString &serialized_arguments, QObject *socket);
  static void shutdown();

  static QString systemInformation();

signals:
  void objectAdded(QObject *obj);
  void aboutToRemoveObject(QObject *obj);

  void pluginsChanged();
  void initializationDone();

  friend class internal::PluginManagerPrivate;
};
}  // namespace extension

#endif