// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_PASSWORD_MANAGER_CONTENT_COMMON_CREDENTIAL_MANAGER_TYPES_H_
#define COMPONENTS_PASSWORD_MANAGER_CONTENT_COMMON_CREDENTIAL_MANAGER_TYPES_H_

#include <string>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/strings/string16.h"
#include "url/gurl.h"

namespace autofill {
struct PasswordForm;
}

namespace blink {
class WebCredential;
};

namespace password_manager {

// Limit the size of the federations array that we pass to the browser to
// something reasonably sane.
const size_t kMaxFederations = 50u;

enum CredentialType {
  CREDENTIAL_TYPE_EMPTY = 0,
  CREDENTIAL_TYPE_LOCAL,
  CREDENTIAL_TYPE_FEDERATED,
  CREDENTIAL_TYPE_LAST = CREDENTIAL_TYPE_FEDERATED
};

struct CredentialInfo {
  CredentialInfo();
  explicit CredentialInfo(const blink::WebCredential& credential);
  explicit CredentialInfo(const autofill::PasswordForm& form);
  ~CredentialInfo();

  CredentialType type;

  // An identifier (username, email address, etc). Corresponds to
  // WebCredential's id property.
  base::string16 id;

  // An user-friendly name ("Jane Doe"). Corresponds to WebCredential's name
  // property.
  base::string16 name;

  // The address of a user's avatar. Corresponds to WebCredential's avatar
  // property.
  GURL avatar;

  // Corresponds to WebLocalCredential's password property.
  base::string16 password;

  // Corresponds to WebFederatedCredential's federation property, which is an
  // origin serialized as a URL (e.g. "https://example.com/").
  GURL federation;
};

// Create a new autofill::PasswordForm object based on |info|, valid in the
// context of |origin|.
scoped_ptr<autofill::PasswordForm> CreatePasswordFormFromCredentialInfo(
    const CredentialInfo& info,
    const GURL& origin);

}  // namespace password_manager

#endif  // COMPONENTS_PASSWORD_MANAGER_CONTENT_COMMON_CREDENTIAL_MANAGER_TYPES_H_
