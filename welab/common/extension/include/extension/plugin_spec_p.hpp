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

#ifndef WELAB__EXTENSION__PLUGIN_SPEC_P_HPP_
#define WELAB__EXTENSION__PLUGIN_SPEC_P_HPP_

#include "plugin_spec.hpp"
#include "iplugin.hpp"

#include <QJsonObject>
#include <QObject>
#include <QPluginLoader>
#include <QRegularExpression>
#include <QStringList>
#include <QVector>
#include <QXmlStreamReader>

namespace extension {

class IPlugin;
class PluginManager;

namespace internal {

class EXTENSION_EXPORT PluginSpecPrivate : public QObject {
  Q_OBJECT
 public:
  PluginSpecPrivate(std::shared_ptr<PluginSpec> spec);

  bool read(const QString &file_name);
  bool provides(const QString &plugin_name, const QString &version);
  bool resolveDependencies(const QVector<std::shared_ptr<PluginSpec>> &specs);
  bool loadLibrary();
  bool initializePlugin();
  bool initializeExtensions();
  bool delayedInitialize();
  IPlugin::ShutdownFlag stop();
  void kill();

  void setEnabledBySettings(bool value);
  void setEnabledByDefault(bool value);
  void setForceEnabled(bool value);
  void setForceDisabled(bool value);

  QVector<std::shared_ptr<PluginSpec>> enableDependenciesIndirectly();
  bool readMetaData(const QJsonObject &plugin_meta_data);

  static bool isValidVerion(const QString &version);
  static int versionCompare(const QString &version1, const QString version2);

  QPluginLoader loader;

  QString name;
  QString version;
  QString compat_version;
  bool required = false;
  bool hidden_by_default = false;
  bool enabled_by_default = true;
  QString vendor;
  QString copyright;
  QString license;
  QString description;
  QString url;
  QString category;
  QRegularExpression platform_specification;
  QVector<PluginDependency> dependencies;
  QJsonObject meta_data;
  bool enabled_by_settings = true;
  bool enabled_indirectly = false;
  bool force_enabled = false;
  bool force_disabled = false;

  QString location;
  QString file_path;
  QStringList arguments;

  QHash<PluginDependency, std::shared_ptr<PluginSpec>> dependency_specs;
  PluginSpec::PluginArgumentDescriptions argument_descriptions;
  std::shared_ptr<IPlugin> plugin = nullptr;

  PluginSpec::State state = PluginSpec::INVALID;
  bool has_error = false;
  QString error_string;

 private:
  std::weak_ptr<PluginSpec> q_ptr;

  bool reportError(const QString &error);
  static const QRegularExpression &versionRegExp();
};

}  // namespace internal

}  // namespace extension

#endif