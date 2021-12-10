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

#ifndef WELAB__EXTENSION__PLUGIN_SPEC_HPP_
#define WELAB__EXTENSION__PLUGIN_SPEC_HPP_

#include "extension_global.hpp"
#include "macros/declare_private.hpp"
#include "utils/porting.hpp"

#include <QHash>
#include <QString>
#include <QVector>

#include <memory>

QT_BEGIN_NAMESPACE
class QRegularExpression;
QT_END_NAMESPACE

namespace extension {

namespace internal {

class OptionsParser;
class PluginSpecPrivate;
class PluginManagerPrivate;

}  // namespace internal

class IPlugin;

/**
 * @struct PluginDependency
 * @brief The PluginDependency class contains the name and required compatible
 * version number of a plugin's dependency.
 *
 * This reflects the data of a dependency object in the plugin's meta data.
 * The name and version are used to resolve the dependency. That is, a plugin
 * with the given name and plugin \c {compatibility version <= dependency version <= plugin version}
 *
 * See also extension::IPlugin for more information about plugin dependencies version matching
 */
struct EXTENSION_EXPORT PluginDependency {
  enum Type {
    REQUIRED,  ///< Dependency needs to be there
    OPTIONAL   ///< Dependency is not necessarily needed.
  };

  PluginDependency() : type(REQUIRED) {}

  bool operator==(const PluginDependency &other) const;
  QString toString() const;

  /** @brief String identifier of the plugin */
  QString name;
  /** @brief Version string that a plugin must match to fill this dependency. */
  QString version;
  /** @brief Defines whether the dependency is required or optional. */
  Type type;
};

utils::QHashValueType qHash(const extension::PluginDependency &value);

struct EXTENSION_EXPORT PluginArgumentDescription {
  QString name;
  QString parameter;
  QString description;
};

/**
 * @class PluginSpec
 * @brief The PluginSpec class contains the information of the plugin's embedded meta data
 * and information about the plugin's current state.
 *
 * The plugin spec is also filled with more information as the plugin goes through
 * its loading process (see PluginSpec::State).
 * If an error occurs, the plugin spec is the place to look for the error details.
 */
class EXTENSION_EXPORT PluginSpec {
public:
  enum State {
    /// Starting point: Even the plugin meta data was not read.
    INVALID,
    /// The plugin meta data has been successfully read, and its information is available via the PluginSpec.
    READ,
    /// The dependencies given in the description file have been successfully found, and are available via
    /// the dependencySpecs() function.
    RESOLVED,
    /// The plugin's library is loaded and the plugin instance created(available through plugin()).
    LOADED,
    /// The plugin instance's IPlugin::initialize() function has been called and returned a success value.
    INITIALIZED,
    /// The plugin's dependencies are successfully initialized and extensionsInitialized has been called. The loading
    /// process is complete.
    RUNNING,
    /// The plugin has been shut down, i.e. the plugin's IPlugin::aboutToShutdown() function has been called.
    STOPPED,
    /// The plugin instance has been deleted.
    DELETED
  };

  ~PluginSpec();
  // Information from json file, valid after 'READ' state is reached
  QString name() const;
  QString version() const;
  QString compatVersion() const;
  QString vendor() const;
  QString copyright() const;
  QString license() const;
  QString description() const;
  QString url() const;
  QString category() const;
  QString revision() const;
  QRegularExpression platformSpecification() const;
  bool isAvailableForHostPlatform() const;
  bool isRequired() const;
  bool isEnabledByDefault() const;
  bool isEnabledBySettings() const;
  bool isEffectivelyEnabled() const;
  bool isEnabledIndirectly() const;
  bool isForceEnabled() const;
  bool isForceDisabled() const;
  QVector<PluginDependency> dependencies() const;
  QJsonObject metaData() const;

  using PluginArgumentDescriptions = QVector<PluginArgumentDescription>;
  PluginArgumentDescriptions argumentDescriptions() const;

  // Other information, valid after 'READ' state is reached
  QString location() const;
  QString filePath() const;

  QStringList arguments() const;
  void setArguments(const QStringList &args);
  void addArgument(const QString &arg);

  bool provides(const QString &plugin_name, const QString &version) const;

  // dependency specs, valid after 'RESOLVED' state is reached
  QHash<PluginDependency, PluginSpec *> dependencySpecs() const;
  bool requiresAny(const QSet<PluginSpec *> &plugins) const;

  // linked plugin instance, valid after 'LOADED' state is reached
  IPlugin *plugin() const;

  // State
  State state() const;
  bool hasError() const;
  QString errorString() const;

  void setEnabledBySettings(bool value);

  static PluginSpec *read(const QString &file_path);

private:
  PluginSpec();

  WELAB_DECLARE_PRIVATE_NS(internal, PluginSpec);
  friend class internal::OptionsParser;
  friend class internal::PluginManagerPrivate;
  friend class internal::PluginSpecPrivate;
};

}  // namespace extension

#endif