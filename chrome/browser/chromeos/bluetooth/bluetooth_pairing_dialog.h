// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_BLUETOOTH_BLUETOOTH_PAIRING_DIALOG_H_
#define CHROME_BROWSER_CHROMEOS_BLUETOOTH_BLUETOOTH_PAIRING_DIALOG_H_

#include <string>

#include "base/compiler_specific.h"
#include "base/values.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/web_dialogs/web_dialog_delegate.h"

namespace device {
class BluetoothDevice;
}

namespace chromeos {

// Bluetooth device pairing dialog shown form ash tray bubble.
class BluetoothPairingDialog : public ui::WebDialogDelegate {
 public:
  BluetoothPairingDialog(gfx::NativeWindow parent_window,
                         const device::BluetoothDevice* device);
  virtual ~BluetoothPairingDialog();

  void Show();

 protected:
  // ui::WebDialogDelegate implementation.
  virtual ui::ModalType GetDialogModalType() const override;
  virtual base::string16 GetDialogTitle() const override;
  virtual GURL GetDialogContentURL() const override;
  virtual void GetWebUIMessageHandlers(
      std::vector<content::WebUIMessageHandler*>* handlers) const override;
  virtual void GetDialogSize(gfx::Size* size) const override;
  virtual std::string GetDialogArgs() const override;
  // NOTE: This function deletes this object at the end.
  virtual void OnDialogClosed(const std::string& json_retval) override;
  virtual void OnCloseContents(
      content::WebContents* source, bool* out_close_dialog) override;
  virtual bool ShouldShowDialogTitle() const override;
  virtual bool HandleContextMenu(
      const content::ContextMenuParams& params) override;

 private:
  gfx::NativeWindow parent_window_;
  base::DictionaryValue device_data_;

  DISALLOW_COPY_AND_ASSIGN(BluetoothPairingDialog);
};

}  // namespace chromeos

#endif  // CHROME_BROWSER_CHROMEOS_BLUETOOTH_BLUETOOTH_PAIRING_DIALOG_H_
