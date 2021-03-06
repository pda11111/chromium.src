// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "android_webview/browser/aw_resource_context.h"

#include "content/public/browser/browser_thread.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_getter.h"

namespace android_webview {

AwResourceContext::AwResourceContext(net::URLRequestContextGetter* getter)
    : getter_(getter) {
  DCHECK(getter_);
}

AwResourceContext::~AwResourceContext() {
}

void AwResourceContext::SetExtraHeaders(
    const GURL& url, const std::string& headers) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
  if (!url.is_valid()) return;
  base::AutoLock scoped_lock(extra_headers_lock_);
  if (!headers.empty())
    extra_headers_[url.spec()] = headers;
  else
    extra_headers_.erase(url.spec());
}

std::string AwResourceContext::GetExtraHeaders(const GURL& url) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
  if (!url.is_valid()) return std::string();
  base::AutoLock scoped_lock(extra_headers_lock_);
  std::map<std::string, std::string>::iterator iter =
      extra_headers_.find(url.spec());
  return iter != extra_headers_.end() ? iter->second : std::string();
}

net::HostResolver* AwResourceContext::GetHostResolver() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
  return getter_->GetURLRequestContext()->host_resolver();
}

net::URLRequestContext* AwResourceContext::GetRequestContext() {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
  return getter_->GetURLRequestContext();
}

}  // namespace android_webview
