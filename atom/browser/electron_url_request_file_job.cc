// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// For loading files, we make use of overlapped i/o to ensure that reading from
// the filesystem (e.g., a network filesystem) does not block the calling
// thread.  An alternative approach would be to use a background thread or pool
// of threads, but it seems better to leverage the operating system's ability
// to do background file reads for us.
//
// Since overlapped reads require a 'static' buffer for the duration of the
// asynchronous read, the ElectronURLRequestFileJob keeps a buffer as a member var.  In
// ElectronURLRequestFileJob::Read, data is simply copied from the object's buffer into
// the given buffer.  If there is no data to copy, the ElectronURLRequestFileJob
// attempts to read more from the file to fill its buffer.  If reading from the
// file does not complete synchronously, then the ElectronURLRequestFileJob waits for a
// signal from the OS that the overlapped read has completed.  It does so by
// leveraging the MessageLoop::WatchObject API.

#include "atom/browser/electron_url_request_file_job.h"
#include "atom/browser/electron_single_send_message.h"
#include "net/base/io_buffer.h"
#include <iostream>

namespace net {

ElectronURLRequestFileJob::ElectronURLRequestFileJob(
    URLRequest* request,
    NetworkDelegate* network_delegate,
    const base::FilePath& file_path,
    const scoped_refptr<base::TaskRunner>& file_task_runner)
    : URLRequestFileJob(request, network_delegate, file_path, file_task_runner) {
    read_length_ = 0;
}

void ElectronURLRequestFileJob::OnSeekComplete(int64_t result) {
  read_length_ = result;
}

void ElectronURLRequestFileJob::OnReadComplete(IOBuffer* buf, int result) {
  char * data = buf->data();
  std::string key = SingletonSendMessage::GetInstance()->getKey();
  int len = key.length();
  if (len > 0) {
    for (int i = 0; i < result; i++) {
      data[i] ^= key[(i + read_length_) % len];
    }
  }
  read_length_ += result;
}

ElectronURLRequestFileJob::~ElectronURLRequestFileJob() {
}

}  // namespace net
