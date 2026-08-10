// Protocol Buffers - Google's data interchange format
// Copyright 2023 Google Inc.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "google/protobuf/thread.h"

#if defined(_POSIX_THREADS)
#include <pthread.h>
#endif

#include <cstddef>
#include <thread>  // NOLINT

#include "absl/base/config.h"
#include "absl/base/optimization.h"
#include "absl/functional/function_ref.h"
#include "absl/types/optional.h"


namespace google {
namespace protobuf {
namespace internal {

absl::optional<StackInfo> GetCurrentStackInfo() {
  static thread_local absl::optional<StackInfo> info{};
  if (ABSL_PREDICT_TRUE(info.has_value())) return info;

#if defined(_POSIX_THREADS)
  pthread_attr_t attr;
  if (pthread_getattr_np(pthread_self(), &attr) == 0) {
    void* base_ptr;
    size_t size;
    if (pthread_attr_getstack(&attr, &base_ptr, &size) == 0) {
      info = StackInfo{base_ptr, size};
    }
  }
  pthread_attr_destroy(&attr);
#endif  // _POSIX_THREADS

  return info;
}

absl::optional<size_t> GetEstimatedThreadStackRemaining() {
  void* estimated_sp;
#if ABSL_HAVE_BUILTIN(__builtin_frame_address)
  estimated_sp = __builtin_frame_address(0);
#else
  estimated_sp = &estimated_sp;
#endif

  auto stack_info = GetCurrentStackInfo();
  if (!stack_info.has_value()) return absl::nullopt;

  ptrdiff_t estimated_available = static_cast<char*>(estimated_sp) -
                                  static_cast<char*>(stack_info->base_ptr);
  if (ABSL_PREDICT_TRUE(estimated_available > 0 &&
                        static_cast<size_t>(estimated_available) <
                            stack_info->size)) {
    return estimated_available;
  }
  return absl::nullopt;
}

void RunSyncInSeparateThread(absl::FunctionRef<void()> work) {
  std::thread(work).join();
}

}  // namespace internal
}  // namespace protobuf
}  // namespace google
