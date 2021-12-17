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

#include "extension/plugin_spec.hpp"
#include "extension/plugin_spec_p.hpp"

#include "extension/iplugin.hpp"
#include "extension/iplugin_p.hpp"
#include "extension/plugin_manager.hpp"

#include "utils/host_os_info.hpp"
#include "utils/string_utils.hpp"
#include "utils/wl_assert.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QPluginLoader>

#include <algorithm>

using namespace extension;
using namespace extension::internal;

utils::QHashValueType extension::qHash(const PluginDependency &value) { return qHash(value.name); }

bool PluginDependency::operator==(const PluginDependency &other) const {
  return name == other.name && version == other.version && type == other.type;
}

static QString typeString(PluginDependency::Type type) {
  switch (type) {
    case PluginDependency::OPTIONAL:
      return QString(", optional");
    case PluginDependency::REQUIRED:
    default:
      return QString();
  }
}

QString PluginDependency::toString() const { return name + " (" + version + typeString(type) + ")"; }

PluginSpec::PluginSpec() : d(std::make_unique<PluginSpecPrivate>(this)) {}

PluginSpec::~PluginSpec() {}

QString PluginSpec::name() const { return d->name; }

QString PluginSpec::version() const { return d->version; }

QString PluginSpec::compatVersion() const { return d->compat_version; }

QString PluginSpec::vendor() const { return d->vendor; }

QString PluginSpec::copyright() const { return d->copyright; }

QString PluginSpec::license() const { return d->license; }

QString PluginSpec::description() const { return d->description; }

QString PluginSpec::url() const { return d->url; }

QString PluginSpec::category() const { return d->category; }

QString PluginSpec::revision() const {
  const QJsonValue revision = metaData().value("Revision");
  if (revision.isString()) {
    return revision.toString();
  }
  return QString();
}

QRegularExpression PluginSpec::platformSpecification() const { return d->platform_specification; }

bool PluginSpec::isAvailableForHostPlatform() const {
  return d->platform_specification.pattern().isEmpty() ||
         d->platform_specification.match(PluginManager::platformName()).hasMatch();
}

bool PluginSpec::isRequired() const { return d->required; }

bool PluginSpec::isEnabledByDefault() const { return d->enabled_by_default; }

bool PluginSpec::isEnabledBySettings() const { return d->enabled_by_settings; }

bool PluginSpec::isEffectivelyEnabled() const {
  if (!isAvailableForHostPlatform()) {
    return false;
  }
  if (isForceEnabled() || isEnabledIndirectly()) {
    return true;
  }
  if (isForceDisabled()) {
    return false;
  }
  return isEnabledBySettings();
}

bool PluginSpec::isEnabledIndirectly() const { return d->enabled_indirectly; }

bool PluginSpec::isForceEnabled() const { return d->force_enabled; }

bool PluginSpec::isForceDisabled() const { return d->force_disabled; }

QVector<PluginDependency> PluginSpec::dependencies() const { return d->dependencies; }

QJsonObject PluginSpec::metaData() const { return d->meta_data; }

PluginSpec::PluginArgumentDescriptions PluginSpec::argumentDescriptions() const { return d->argument_descriptions; }

QString PluginSpec::location() const { return d->location; }

QString PluginSpec::filePath() const { return d->file_path; }

QStringList PluginSpec::arguments() const { return d->arguments; }

void PluginSpec::setArguments(const QStringList &args) { d->arguments = args; }

void PluginSpec::addArgument(const QString &arg) { d->arguments.push_back(arg); }

PluginSpec::State PluginSpec::state() const { return d->state; }

bool PluginSpec::hasError() const { return d->has_error; }

QString PluginSpec::errorString() const { return d->error_string; }

bool PluginSpec::provides(const QString &plugin_name, const QString &version) const {
  return d->provides(plugin_name, version);
}

IPlugin *PluginSpec::plugin() const { return d->plugin; }

QHash<PluginDependency, PluginSpec *> PluginSpec::dependencySpecs() const { return d->dependency_specs; }

bool PluginSpec::requiresAny(const QSet<PluginSpec *> &plugins) const {
  return std::any_of(d->dependency_specs.keys().begin(), d->dependency_specs.keys().end(),
                     [this, &plugins](const PluginDependency &dep) {
                       return dep.type == PluginDependency::REQUIRED &&
                              plugins.contains(d->dependency_specs.value(dep));
                     });
}

void PluginSpec::setEnabledBySettings(bool value) { return d->setEnabledBySettings(value); }

PluginSpec *PluginSpec::read(const QString &file_path) {
  auto spec = new PluginSpec();
  if (!spec->d->read(file_path)) {
    delete spec;
    return nullptr;
  }
  return spec;
}

namespace {
const char PLUGIN_METADATA[] = "MetaData";
const char PLUGIN_NAME[] = "Name";
const char PLUGIN_VERSION[] = "Version";
const char PLUGIN_COMPATVERSION[] = "CompatVersion";
const char PLUGIN_REQUIRED[] = "Required";
const char PLUGIN_DISABLED_BY_DEFAULT[] = "DisabledByDefault";
const char VENDOR[] = "Vendor";
const char COPYRIGHT[] = "Copyright";
const char LICENSE[] = "License";
const char DESCRIPTION[] = "Description";
const char URL[] = "Url";
const char CATEGORY[] = "Category";
const char PLATFORM[] = "Platform";
const char DEPENDENCIES[] = "Dependencies";
const char DEPENDENCY_NAME[] = "Name";
const char DEPENDENCY_VERSION[] = "Version";
const char DEPENDENCY_TYPE[] = "Type";
const char DEPENDENCY_TYPE_SOFT[] = "optional";
const char DEPENDENCY_TYPE_HARD[] = "required";
const char ARGUMENTS[] = "Arguments";
const char ARGUMENT_NAME[] = "Name";
const char ARGUMENT_PARAMETER[] = "Parameter";
const char ARGUMENT_DESCRIPTION[] = "Description";
}  // namespace

PluginSpecPrivate::PluginSpecPrivate(PluginSpec *spec) : q_ptr(spec) {
  if (utils::HostOsInfo::isMacHost()) {
    loader.setLoadHints(QLibrary::ExportExternalSymbolsHint);
  }
}

bool PluginSpecPrivate::read(const QString &file_name) {
  qCDebug(pluginLog) << "\nReading meta data of " << file_name;
  name = version = compat_version = vendor = copyright = license = description = url = category = location = QString();

  state = PluginSpec::INVALID;
  has_error = false;
  error_string.clear();
  dependencies.clear();
  meta_data = QJsonObject();
  QFileInfo file_info(file_name);
  location = file_info.absolutePath();
  file_path = file_info.absoluteFilePath();
  loader.setFileName(file_path);
  if (loader.fileName().isEmpty()) {
    qCDebug(pluginLog) << "Cannot open file";
    return false;
  }

  if (!readMetaData(loader.metaData())) {
    return false;
  }

  state = PluginSpec::READ;
  return true;
}

void PluginSpecPrivate::setEnabledBySettings(bool value) { enabled_by_settings = value; }

void PluginSpecPrivate::setEnabledByDefault(bool value) { enabled_by_default = value; }

void PluginSpecPrivate::setForceEnabled(bool value) {
  force_enabled = value;
  if (value) {
    force_disabled = false;
  }
}

void PluginSpecPrivate::setForceDisabled(bool value) {
  if (value) {
    force_disabled = false;
  }
  force_disabled = value;
}

bool PluginSpecPrivate::reportError(const QString &error) {
  error_string = error;
  has_error = true;
  return true;
}

static inline QString msgValueMissing(const char *key) {
  return QCoreApplication::translate("PluginSpec", "\"%1\" is missing").arg(QLatin1String(key));
}

static inline QString msgValueIsNotAString(const char *key) {
  return QCoreApplication::translate("PluginSpec", "Value for key \"%1\" is not a string").arg(QLatin1String(key));
}

static inline QString msgValueIsNotABool(const char *key) {
  return QCoreApplication::translate("PluginSpec", "Value for key \"%1\" is not a bool").arg(QLatin1String(key));
}

static inline QString msgValueIsNotAObjectArray(const char *key) {
  return QCoreApplication::translate("PluginSpec", "Value for key \"%1\" is not an array of objects")
      .arg(QLatin1String(key));
}

static inline QString msgValueIsNotAMultilineString(const char *key) {
  return QCoreApplication::translate("PluginSpec", "Value for key \"%1\" is not a string and not an array of strings")
      .arg(QLatin1String(key));
}

static inline QString msgInvalidFormat(const char *key, const QString &content) {
  return QCoreApplication::translate("PluginSpec", "Value \"%2\" for key \"%1\" has invalid format")
      .arg(QLatin1String(key), content);
}

bool PluginSpecPrivate::readMetaData(const QJsonObject &plugin_meta_data) {
  qCDebug(pluginLog) << "MetaData:" << QJsonDocument(plugin_meta_data).toJson();
  QJsonValue value;
  value = plugin_meta_data.value(QLatin1String("IID"));
  if (!value.isString()) {
    qCDebug(pluginLog) << "Not a plugin (no string IID found)";
    return false;
  }
  if (value.toString() != PluginManager::pluginIID()) {
    qCDebug(pluginLog) << "Plugin ignored (IID does not match)";
    return false;
  }

  value = plugin_meta_data.value(QLatin1String(PLUGIN_METADATA));
  if (!value.isObject()) {
    return reportError(tr("Plugin meta data not found"));
  }
  meta_data = value.toObject();
  // plugin name
  value = meta_data.value(QLatin1String(PLUGIN_NAME));
  if (value.isUndefined()) {
    return reportError(msgValueMissing(PLUGIN_NAME));
  }
  if (!value.isString()) {
    return reportError(msgValueIsNotAString(PLUGIN_NAME));
  }
  name = value.toString();
  // plugin version
  value = meta_data.value(QLatin1String(PLUGIN_VERSION));
  if (value.isUndefined()) {
    return reportError(msgValueMissing(PLUGIN_VERSION));
  }
  if (!value.isString()) {
    return reportError(msgValueIsNotAString(PLUGIN_VERSION));
  }
  version = value.toString();
  if (!isValidVerion(version)) {
    return reportError(msgInvalidFormat(PLUGIN_VERSION, version));
  }
  // compat version
  value = meta_data.value(QLatin1String(PLUGIN_COMPATVERSION));
  if (!value.isUndefined() && !value.isString()) {
    return reportError(msgValueIsNotAString(PLUGIN_COMPATVERSION));
  }
  compat_version = value.toString(version);
  if (!value.isUndefined() && !isValidVerion(compat_version)) {
    return reportError(msgInvalidFormat(PLUGIN_COMPATVERSION, compat_version));
  }
  // plugin required
  value = meta_data.value(QLatin1String(PLUGIN_REQUIRED));
  if (!value.isUndefined() && !value.isBool()) {
    return reportError(msgValueIsNotABool(PLUGIN_REQUIRED));
  }
  required = value.toBool(false);
  qCDebug(pluginLog) << "required =" << required;
  // plugin enabled by default
  value = meta_data.value(QLatin1String(PLUGIN_DISABLED_BY_DEFAULT));
  if (!value.isUndefined() && !value.isBool()) {
    return reportError(msgValueIsNotABool(PLUGIN_DISABLED_BY_DEFAULT));
  }
  enabled_by_default = !value.toBool(false);
  qCDebug(pluginLog) << "enabled_by_default =" << enabled_by_default;

  enabled_by_settings = enabled_by_default;
  // plugin vendor
  value = meta_data.value(QLatin1String(VENDOR));
  if (!value.isUndefined() && !value.isString()) {
    return reportError(msgValueIsNotAString(VENDOR));
  }
  vendor = value.toString();
  // copyright
  value = meta_data.value(QLatin1String(COPYRIGHT));
  if (!value.isUndefined() && !value.isString()) {
    return reportError(msgValueIsNotAString(COPYRIGHT));
  }
  copyright = value.toString();
  // description
  value = meta_data.value(QLatin1String(DESCRIPTION));
  if (!value.isUndefined() && !utils::readMultiLineString(value, &description)) {
    return reportError(msgValueIsNotAString(DESCRIPTION));
  }
  // url
  value = meta_data.value(QLatin1String(URL));
  if (!value.isUndefined() && !value.isString()) {
    return reportError(msgValueIsNotAString(URL));
  }
  url = value.toString();
  // category
  value = meta_data.value(QLatin1String(CATEGORY));
  if (!value.isUndefined() && !value.isString()) {
    return reportError(msgValueIsNotAString(CATEGORY));
  }
  category = value.toString();
  // license
  value = meta_data.value(QLatin1String(LICENSE));
  if (!value.isUndefined() && !utils::readMultiLineString(value, &license)) {
    return reportError(msgValueIsNotAMultilineString(LICENSE));
  }
  // platform
  value = meta_data.value(QLatin1String(PLATFORM));
  if (!value.isUndefined() && !value.isString()) {
    return reportError(msgValueIsNotAString(PLATFORM));
  }
  const QString platform_spec = value.toString().trimmed();
  if (!platform_spec.isEmpty()) {
    platform_specification.setPattern(platform_spec);
    if (!platform_specification.isValid()) {
      return reportError(
          tr("Invalid platform specification \"%1\": %2").arg(platform_spec, platform_specification.errorString()));
    }
  }
  // dependencies
  value = meta_data.value(QLatin1String(DEPENDENCIES));
  if (!value.isUndefined() && !value.isArray()) {
    return reportError(msgValueIsNotAObjectArray(DEPENDENCIES));
  }
  if (!value.isUndefined()) {
    const QJsonArray array = value.toArray();
    for (const QJsonValue &v : array) {
      if (!v.isObject()) {
        return reportError(msgValueIsNotAObjectArray(DEPENDENCIES));
      }
      QJsonObject dependency_object = v.toObject();
      PluginDependency dep;
      value = dependency_object.value(QLatin1String(DEPENDENCY_NAME));
      if (value.isUndefined()) {
        return reportError(tr("Dependency: %1").arg(msgValueMissing(DEPENDENCY_NAME)));
      }
      if (!value.isString()) {
        return reportError(tr("Dependency: %1").arg(msgValueIsNotAString(DEPENDENCY_NAME)));
      }
      dep.name = value.toString();
      value = dependency_object.value(QLatin1String(DEPENDENCY_VERSION));
      if (!value.isUndefined() && !value.isString()) {
        return reportError(tr("Dependency: %1").arg(msgValueIsNotAString(DEPENDENCY_VERSION)));
      }
      dep.version = value.toString();
      if (!isValidVerion(dep.version)) {
        return reportError(tr("Dependency: %1").arg(msgInvalidFormat(DEPENDENCY_VERSION, dep.version)));
      }
      dep.type = PluginDependency::REQUIRED;
      value = dependency_object.value(QLatin1String(DEPENDENCY_TYPE));
      if (!value.isUndefined() && !value.isString()) {
        return reportError(tr("Dependency: %1").arg(msgValueIsNotAString(DEPENDENCY_TYPE)));
      }
      if (!value.isUndefined()) {
        const QString type_value = value.toString();
        if (type_value.toLower() == QLatin1String(DEPENDENCY_TYPE_HARD)) {
          dep.type = PluginDependency::REQUIRED;
        } else if (type_value.toLower() == QLatin1String(DEPENDENCY_TYPE_SOFT)) {
          dep.type = PluginDependency::OPTIONAL;
        } else {
          return reportError(tr("Dependency: \"%1\" must be \"%2\" or \"%3\" (is \"%4\").")
                                 .arg(QLatin1String(DEPENDENCY_TYPE), QLatin1String(DEPENDENCY_TYPE_HARD),
                                      QLatin1String(DEPENDENCY_TYPE_SOFT), type_value));
        }
      }
      dependencies.append(dep);
    }
  }

  // arguments
  value = meta_data.value(QLatin1String(ARGUMENTS));
  if (!value.isUndefined() && !value.isArray()) {
    return reportError(msgValueIsNotAObjectArray(ARGUMENTS));
  }
  if (!value.isUndefined()) {
    const QJsonArray array = value.toArray();
    for (const QJsonValue &v : array) {
      if (!v.isObject()) {
        return reportError(msgValueIsNotAObjectArray(ARGUMENTS));
      }
      QJsonObject argument_object = v.toObject();
      PluginArgumentDescription arg;
      value = argument_object.value(QLatin1String(ARGUMENT_NAME));
      if (value.isUndefined()) {
        return reportError(tr("Argument: %1").arg(msgValueMissing(ARGUMENT_NAME)));
      }
      if (!value.isString()) {
        return reportError(tr("Argument: %1").arg(msgValueIsNotAString(ARGUMENT_NAME)));
      }
      arg.name = value.toString();
      if (arg.name.isEmpty()) {
        return reportError(tr("Argument: \"%1\" is empty").arg(QLatin1String(ARGUMENT_NAME)));
      }
      value = argument_object.value(QLatin1String(ARGUMENT_DESCRIPTION));
      if (!value.isUndefined() && !value.isString()) {
        return reportError(tr("Argument: %1").arg(msgValueIsNotAString(ARGUMENT_DESCRIPTION)));
      }
      arg.description = value.toString();
      value = argument_object.value(QLatin1String(ARGUMENT_PARAMETER));
      if (!value.isUndefined() && !value.isString()) {
        return reportError(tr("Argument: %1").arg(msgValueIsNotAString(ARGUMENT_PARAMETER)));
      }
      arg.parameter = value.toString();
      argument_descriptions.append(arg);
      qCDebug(pluginLog) << "Argument:" << arg.name << "Parameter:" << arg.parameter
                         << "Description:" << arg.description;
    }
  }

  return true;
}

bool PluginSpecPrivate::provides(const QString &plugin_name, const QString &plugin_version) const {
  if (QString::compare(plugin_name, name, Qt::CaseInsensitive) != 0) {
    return false;
  }

  return (versionCompare(version, plugin_version) >= 0) && (versionCompare(compat_version, plugin_version) <= 0);
}

const QRegularExpression &PluginSpecPrivate::versionRegExp() {
  static const QRegularExpression reg("^([0-9]+)(?:[.]([0-9]))?(?:[.]([0-9]+))?(?:_([0-9]+))?$");
  return reg;
}

bool PluginSpecPrivate::isValidVerion(const QString &version) { return versionRegExp().match(version).hasMatch(); }

int PluginSpecPrivate::versionCompare(const QString &version1, const QString &version2) {
  const QRegularExpressionMatch match1 = versionRegExp().match(version1);
  const QRegularExpressionMatch match2 = versionRegExp().match(version2);

  if (!match1.hasMatch()) {
    return 0;
  }
  if (!match2.hasMatch()) {
    return 0;
  }
  for (int i = 0; i < 4; ++i) {
    const int number1 = match1.captured(i + 1).toInt();
    const int number2 = match2.captured(i + 1).toInt();
    if (number1 < number2) {
      return -1;
    }
    if (number1 > number2) {
      return 1;
    }
  }
  return 0;
}

bool PluginSpecPrivate::resolveDependencies(const QVector<PluginSpec *> &specs) {
  if (has_error) {
    return false;
  }
  if (state == PluginSpec::RESOLVED) {
    // Go back, so we just re-resolve
    state = PluginSpec::READ;
  }
  if (state != PluginSpec::READ) {
    error_string = QCoreApplication::translate("PluginSpec", "Resolving dependencies failed because state != READ");
    has_error = true;
    return false;
  }
  QHash<PluginDependency, PluginSpec *> resolved_dependencies;
  foreach (const PluginDependency &dependency, dependencies) {
    auto it = std::find_if(specs.begin(), specs.end(), [&dependency](PluginSpec *spec) {
      return spec->provides(dependency.name, dependency.version);
    });
    if (it == specs.end()) {
      if (dependency.type == PluginDependency::REQUIRED) {
        has_error = true;
        if (!error_string.isEmpty()) {
          error_string.append(QLatin1Char('\n'));
        }
        error_string.append(QCoreApplication::translate("PluginSpec", "Could not resolve dependency '%1(%2)'")
                                .arg(dependency.name)
                                .arg(dependency.version));
      }
      continue;
    }
    resolved_dependencies.insert(dependency, *it);
  }

  if (has_error) {
    return false;
  }
  dependency_specs = resolved_dependencies;

  state = PluginSpec::RESOLVED;

  return true;
}

QVector<PluginSpec *> PluginSpecPrivate::enableDependenciesIndirectly() {
  if (!q_ptr->isEffectivelyEnabled()) {
    return {};
  }
  QVector<PluginSpec *> enabled;
  for (auto it = dependency_specs.cbegin(), end = dependency_specs.cend(); it != end; ++it) {
    if (it.key().type != PluginDependency::REQUIRED) {
      continue;
    }
    PluginSpec *dependency_spec = it.value();
    if (!dependency_spec->isEffectivelyEnabled()) {
      dependency_spec->d->enabled_indirectly = true;
      enabled << dependency_spec;
    }
  }
  return enabled;
}

bool PluginSpecPrivate::loadLibrary() {
  if (has_error) {
    return false;
  }
  if (state != PluginSpec::RESOLVED) {
    if (state == PluginSpec::LOADED) {
      return true;
    }
    error_string = QCoreApplication::translate("PluginSpec", "Loading the library failed because state != RESOLVED");
    has_error = true;
    return false;
  }
  if (!loader.load()) {
    has_error = true;
    error_string = QDir::toNativeSeparators(file_path) + QString::fromLatin1(": ") + loader.errorString();
    return false;
  }

  IPlugin *plugin_object = qobject_cast<IPlugin *>(loader.instance());
  if (!plugin_object) {
    has_error = true;
    error_string = QCoreApplication::translate("PluginSpec", "Plugin is not valid (does not derive from IPlugin)");
    loader.unload();
    return false;
  }
  state = PluginSpec::LOADED;
  plugin = plugin_object;
  plugin->d->plugin_spec = q_ptr;
  return true;
}

bool PluginSpecPrivate::initializePlugin() {
  if (has_error) {
    return false;
  }
  if (state != PluginSpec::LOADED) {
    if (state == PluginSpec::INITIALIZED) {
      return true;
    }
    error_string = QCoreApplication::translate("PluginSpec", "Initializing the plugin failed because state != LOADED");
    has_error = true;
    return false;
  }
  if (!plugin) {
    error_string = QCoreApplication::translate("PluginSpec", "Internal error: have no plugin instance to initialize");
    has_error = true;
    return false;
  }
  QString err;
  if (!plugin->initialize(arguments, &err)) {
    error_string = QCoreApplication::translate("PluginSpec", "Plugin initialization failed: %1").arg(err);
    has_error = true;
    return false;
  }
  state = PluginSpec::INITIALIZED;
  return true;
}

bool PluginSpecPrivate::initializeExtensions() {
  if (has_error) {
    return false;
  }
  if (state != PluginSpec::INITIALIZED) {
    if (state == PluginSpec::RUNNING) {
      return true;
    }
    error_string =
        QCoreApplication::translate("PluginSpec", "Cannot perform extensionsInitialized because state != INITIALIZED");
    has_error = true;
    return false;
  }
  if (!plugin) {
    error_string = QCoreApplication::translate(
        "PluginSpec", "Internal error: have no plugin instance to perform extensionsInitialized");
    has_error = true;
    return false;
  }
  plugin->extensionsInitialized();
  state = PluginSpec::RUNNING;

  return true;
}

bool PluginSpecPrivate::delayedInitialize() {
  if (has_error) {
    return false;
  }
  if (state != PluginSpec::RUNNING) {
    return false;
  }
  if (!plugin) {
    error_string = QCoreApplication::translate("PluginSpec",
                                               "Internal error: have no plugin instance to perform delayedInitialize");
    has_error = true;
    return false;
  }
  return plugin->delayedInitialize();
}

IPlugin::ShutdownFlag PluginSpecPrivate::stop() {
  if (!plugin) {
    return IPlugin::SYNCHRONOUS_SHUTDOWN;
  }
  state = PluginSpec::STOPPED;
  return plugin->aboutToShutdown();
}

void PluginSpecPrivate::kill() {
  if (!plugin) {
    return;
  }
  delete plugin;
  plugin = nullptr;
  state = PluginSpec::DELETED;
}