// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/message_center/fake_message_center.h"
#include "ui/message_center/notification_list.h"

namespace message_center {

FakeMessageCenter::FakeMessageCenter() {
}

FakeMessageCenter::~FakeMessageCenter() {
}

void FakeMessageCenter::AddObserver(MessageCenterObserver* observer) {
}

void FakeMessageCenter::RemoveObserver(MessageCenterObserver* observer) {
}

void FakeMessageCenter::SetDelegate(Delegate* delegate) {
}

size_t FakeMessageCenter::NotificationCount() const {
  return 0u;
}

size_t FakeMessageCenter::UnreadNotificationCount() const {
  return 0u;
}

bool FakeMessageCenter::HasPopupNotifications() const {
  return false;
}

bool FakeMessageCenter::HasNotification(const std::string& id) {
  return false;
}

bool FakeMessageCenter::IsQuietMode() const {
  return false;
}

bool FakeMessageCenter::HasClickedListener(const std::string& id) {
  return false;
}

const NotificationList::Notifications& FakeMessageCenter::GetNotifications() {
  return empty_notifications_;
}

NotificationList::PopupNotifications
    FakeMessageCenter::GetPopupNotifications() {
  return NotificationList::PopupNotifications();
}

void FakeMessageCenter::AddNotification(scoped_ptr<Notification> notification) {
}

void FakeMessageCenter::UpdateNotification(
    const std::string& old_id,
    scoped_ptr<Notification> new_notification) {}

void FakeMessageCenter::RemoveNotification(const std::string& id,
                                           bool by_user) {
}

void FakeMessageCenter::RemoveAllNotifications(bool by_user) {
}

void FakeMessageCenter::SetNotificationIcon(const std::string& notification_id,
                                            const gfx::Image& image) {
}

void FakeMessageCenter::SetNotificationImage(const std::string& notification_id,
                                             const gfx::Image& image) {
}

void FakeMessageCenter::SetNotificationButtonIcon(
    const std::string& notification_id,
    int button_index,
    const gfx::Image& image) {
}

void FakeMessageCenter::DisableNotificationsByExtension(const std::string& id) {
}

void FakeMessageCenter::DisableNotificationsByUrl(const std::string& id) {
}

void FakeMessageCenter::ShowNotificationSettings(const std::string& id) {
}

NotifierSettingsDelegate* FakeMessageCenter::ShowNotificationSettingsDialog(
    gfx::NativeView context) {
  return NULL;
}

void FakeMessageCenter::ExpandNotification(const std::string& id) {
}

void FakeMessageCenter::ClickOnNotification(const std::string& id) {
}

void FakeMessageCenter::ClickOnNotificationButton(const std::string& id,
                                                  int button_index) {
}

void FakeMessageCenter::MarkSinglePopupAsShown(const std::string& id,
                                               bool mark_notification_as_read) {
}

void FakeMessageCenter::DisplayedNotification(const std::string& id) {
}

void FakeMessageCenter::SetQuietMode(bool in_quiet_mode) {
}

void FakeMessageCenter::EnterQuietModeWithExpire(
    const base::TimeDelta& expires_in) {
}

void FakeMessageCenter::SetMessageCenterVisible(bool visible) {
}

}  // namespace message_center
