// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MESSAGE_PUMP_UV_H_
#define BASE_MESSAGE_PUMP_UV_H_
#pragma once

#include "base/message_pump.h"
#include "base/time.h"
#include "base/base_export.h"
#include "third_party/libuv/include/uv.h"

namespace base {

class BASE_EXPORT MessagePumpUV : public MessagePump {
 public:
  MessagePumpUV();
  virtual ~MessagePumpUV();

  // MessagePump methods:
  virtual void Run(Delegate* delegate) OVERRIDE;
  virtual void Quit() OVERRIDE;
  virtual void ScheduleWork() OVERRIDE;
  virtual void ScheduleDelayedWork(const TimeTicks& delayed_work_time) OVERRIDE;

 private:
  // This flag is set to false when Run should return.
  bool keep_running_;

  struct uv_async_s wakeup_event_;

  TimeTicks delayed_work_time_;

  DISALLOW_COPY_AND_ASSIGN(MessagePumpUV);
};

}  // namespace base

#endif  // BASE_MESSAGE_PUMP_UV_H_
