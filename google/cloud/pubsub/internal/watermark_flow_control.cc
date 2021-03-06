// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "google/cloud/pubsub/internal/watermark_flow_control.h"

namespace google {
namespace cloud {
namespace pubsub_internal {
inline namespace GOOGLE_CLOUD_CPP_PUBSUB_NS {

bool WatermarkFlowControl::MaybeAdmit(std::size_t size) {
  std::lock_guard<std::mutex> lk(mu_);
  if (overflow_ || IsFull()) return false;
  current_size_ += size;
  ++current_count_;
  if (IsFull()) overflow_ = true;
  return true;
}

bool WatermarkFlowControl::Release(std::size_t size) {
  std::lock_guard<std::mutex> lk(mu_);
  if (current_count_ > 0) --current_count_;
  if (size < current_size_) {
    current_size_ -= size;
  } else {
    current_size_ = 0;
  }
  if (HasCapacity()) overflow_ = false;
  return !overflow_ && !IsFull();
}

}  // namespace GOOGLE_CLOUD_CPP_PUBSUB_NS
}  // namespace pubsub_internal
}  // namespace cloud
}  // namespace google
