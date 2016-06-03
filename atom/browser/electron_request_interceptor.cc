// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "atom/browser/electron_request_interceptor.h"
#include "atom/browser/electron_single_send_message.h"
#include "atom/browser/electron_url_request_file_job.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/browser_thread.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_getter.h"
#include "net/url_request/url_request.h"

using content::BrowserThread;

namespace electron {

namespace {

const void* kURLRequestUserDataKey = &kURLRequestUserDataKey;

}  // namespace

ElectronRequestInterceptor::ElectronRequestInterceptor() {
}

ElectronRequestInterceptor::~ElectronRequestInterceptor() {
}

#if defined(OS_WIN)
BOOL StringToWString(const std::string &str,std::wstring &wstr)
{
    int nLen = (int)str.length();
    wstr.resize(nLen,L' ');

    int nResult = MultiByteToWideChar(CP_ACP,0,(LPCSTR)str.c_str(),nLen,(LPWSTR)wstr.c_str(),nLen);

    if (nResult == 0)
    {
        return FALSE;
    }

    return TRUE;
}
#endif


net::URLRequestJob* ElectronRequestInterceptor::MaybeInterceptRequest(
    net::URLRequest* request,
    net::NetworkDelegate* network_delegate) const {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  std::string filepath;
  SingletonSendMessage *singletonObj1 = SingletonSendMessage ::GetInstance();
  std::string url = request->url().spec();
  if(singletonObj1->getMatcher(url, &filepath)) {
    #if defined(OS_WIN)
    std::wstring path;
    StringToWString(filepath, path);
    #else
    std::string path = filepath;
    #endif
    const base::FilePath dest(path);
    return new net::URLRequestFileJob(request, network_delegate, dest, base::ThreadTaskRunnerHandle::Get());
  }
  if (url.find("file://") == 0 && url.find("/xcloud2-dist/") != std::string::npos) {
    #if defined(OS_WIN)
      int pos = 8;
    #else
      int pos = 7;
    #endif
    FILE* handle = fopen(url.substr(pos).c_str(), "r");
    if (!handle) {
      url += ".enc";
      handle = fopen(url.substr(pos).c_str(), "r");
      if (handle) {
        filepath = url.substr(pos);
        #if defined(OS_WIN)
        std::wstring path;
        StringToWString(filepath, path);
        #else
        std::string path = filepath;
        #endif
        const base::FilePath dest(path);
        return new net::ElectronURLRequestFileJob(request, network_delegate, dest, base::ThreadTaskRunnerHandle::Get());
      }
    }
  }
  return NULL;
}
}  // namespace electron
