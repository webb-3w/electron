// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ELECTRON_URL_REQUEST_URL_REQUEST_FILE_JOB_H_
#define ELECTRON_URL_REQUEST_URL_REQUEST_FILE_JOB_H_

#include "net/url_request/url_request_file_job.h"

namespace net {

// A request job that handles reading file URLs
class ElectronURLRequestFileJob : public URLRequestFileJob {
 public:
  ElectronURLRequestFileJob(URLRequest* request,
                    NetworkDelegate* network_delegate,
                    const base::FilePath& file_path,
                    const scoped_refptr<base::TaskRunner>& file_task_runner);

  void OnSeekComplete(int64_t result) override;
  void OnReadComplete(IOBuffer* buf, int result) override;

 private:
  ~ElectronURLRequestFileJob();
  int64_t read_length_;
};

}  // namespace net

#endif  // NET_URL_REQUEST_URL_REQUEST_FILE_JOB_H_
