// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/extensions/settings/settings_sync_util.h"

#include "base/values.h"
#include "base/json/json_writer.h"
#include "chrome/browser/sync/protocol/app_setting_specifics.pb.h"
#include "chrome/browser/sync/protocol/extension_setting_specifics.pb.h"

namespace extensions {

namespace settings_sync_util {

namespace {

void PopulateExtensionSettingSpecifics(
    const std::string& extension_id,
    const std::string& key,
    const Value& value,
    sync_pb::ExtensionSettingSpecifics* specifics) {
  specifics->set_extension_id(extension_id);
  specifics->set_key(key);
  {
    std::string value_as_json;
    base::JSONWriter::Write(&value, false, &value_as_json);
    specifics->set_value(value_as_json);
  }
}

void PopulateAppSettingSpecifics(
    const std::string& extension_id,
    const std::string& key,
    const Value& value,
    sync_pb::AppSettingSpecifics* specifics) {
  PopulateExtensionSettingSpecifics(
      extension_id, key, value, specifics->mutable_extension_setting());
}

}  // namespace

SyncData CreateData(
    const std::string& extension_id,
    const std::string& key,
    const Value& value,
    syncable::ModelType type) {
  sync_pb::EntitySpecifics specifics;

  switch (type) {
    case syncable::EXTENSION_SETTINGS:
      PopulateExtensionSettingSpecifics(
          extension_id,
          key,
          value,
          specifics.MutableExtension(sync_pb::extension_setting));
      break;

    case syncable::APP_SETTINGS:
      PopulateAppSettingSpecifics(
          extension_id,
          key,
          value,
          specifics.MutableExtension(sync_pb::app_setting));
      break;

    default:
      NOTREACHED();
  }

  return SyncData::CreateLocalData(extension_id + "/" + key, key, specifics);
}

SyncChange CreateAdd(
    const std::string& extension_id,
    const std::string& key,
    const Value& value,
    syncable::ModelType type) {
  return SyncChange(
      SyncChange::ACTION_ADD, CreateData(extension_id, key, value, type));
}

SyncChange CreateUpdate(
    const std::string& extension_id,
    const std::string& key,
    const Value& value,
    syncable::ModelType type) {
  return SyncChange(
      SyncChange::ACTION_UPDATE, CreateData(extension_id, key, value, type));
}

SyncChange CreateDelete(
    const std::string& extension_id,
    const std::string& key,
    syncable::ModelType type) {
  DictionaryValue no_value;
  return SyncChange(
      SyncChange::ACTION_DELETE, CreateData(extension_id, key, no_value, type));
}

}  // namespace settings_sync_util

}  // namespace extensions
