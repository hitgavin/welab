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

#include "utils/host_os_info.hpp"

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

PluginSpec::PluginSpec() : d_ptr(std::make_unique<PluginSpecPrivate>(this)) {}

PluginSpec::~PluginSpec() {}

QString PluginSpec::name() const { return d_ptr->name; }

QString PluginSpec::version() const { return d_ptr->version; }

QString PluginSpec::compatVersion() const { return d_ptr->compat_version; }

QString PluginSpec::vendor() const { return d_ptr->vendor; }

QString PluginSpec::copyright() const { return d_ptr->copyright; }

QString PluginSpec::license() const { return d_ptr->license; }

QString PluginSpec::description() const { return d_ptr->description; }

QString PluginSpec::url() const { return d_ptr->url; }

QString PluginSpec::category() const { return d_ptr->category; }

QString PluginSpec::revision() const {
  const QJsonValue revision = metaData().value("Revision");
  if (revision.isString()) {
    return revision.toString();
  }
  return QString();
}

QRegularExpression PluginSpec::platformSpecification() const { return d_ptr->platform_specification; }

bool PluginSpec::isAvailableForHostPlatform() const {
  return d_ptr->platform_specification.pattern().isEmpty() ||
         d_ptr->platform_specification.match(PluginManager::platformName()).hasMatch();
}

bool PluginSpec::isRequired() const { return d_ptr->required; }

bool PluginSpec::isEnabledByDefault() const { return d_ptr->enabled_by_default; }

bool PluginSpec::isEnabledBySettings() const { return d_ptr->enabled_by_settings; }

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

bool PluginSpec::isEnabledIndirectly() const { return d_ptr->enabled_indirectly; }

bool PluginSpec::isForceDisabled() const { return d_ptr->force_disabled; }

QVector<PluginDependency> PluginSpec::dependencies() const { return d_ptr->dependencies; }

QJsonObject PluginSpec::metaData() const { return d_ptr->meta_data; }

PluginSpec::PluginArgumentDescriptions PluginSpec::argumentDescriptions() const { return d_ptr->argument_descriptions; }

QString PluginSpec::location() const { return d_ptr->location; }

QString PluginSpec::filePath() const { return d_ptr->file_path; }

QStringList PluginSpec::arguments() const { return d_ptr->arguments; }

void PluginSpec::setArguments(const QStringList &args) { d_ptr->arguments = args; }

void PluginSpec::addArgument(const QString &arg) { d_ptr->arguments.push_back(arg); }

PluginSpec::State PluginSpec::state() const { return d_ptr->state; }

bool PluginSpec::hasError() const { return d_ptr->has_error; }

QString PluginSpec::errorString() const { return d_ptr->error_string; }

bool PluginSpec::provides(const QString &plugin_name, const QString &version) const {
  return d_ptr->provides(plugin_name, version);
}

std::shared_ptr<IPlugin> PluginSpec::plugin() const { return d_ptr->plugin; }

QHash<PluginDependency, std::shared_ptr<PluginSpec>> PluginSpec::dependencySpecs() const {
  return d_ptr->dependency_specs;
}

bool PluginSpec::requiresAny(const QSet<std::shared_ptr<PluginSpec>> &plugins) const {
  return std::any_of(d_ptr->dependency_specs.keys().begin(), d_ptr->dependency_specs.keys().end(),
                     [this, &plugins](const PluginDependency &dep) {
                       return dep.type == PluginDependency::REQUIRED &&
                              plugins.contains(d_ptr->dependency_specs.value(dep));
                     });
}

void PluginSpec::setEnabledBySettings(bool value) { return d_ptr->setEnabledBySettings(value); }

std::shared_ptr<PluginSpec> PluginSpec::read(const QString &file_path) {
  auto spec = std::make_shared<PluginSpec>();
  if (!spec->d_ptr->read(file_path)) {
    return std::shared_ptr<PluginSpec>();
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
const char DEPENDENCY_TYPE_TEST[] = "test";
const char ARGUMENTS[] = "Arguments";
const char ARGUMENT_NAME[] = "Name";
const char ARGUMENT_PARAMETER[] = "Parameter";
const char ARGUMENT_DESCRIPTION[] = "Description";
}  // namespace

PluginSpecPrivate::PluginSpecPrivate(std::shared_ptr<PluginSpec> spec) : q_ptr(spec) {
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
}