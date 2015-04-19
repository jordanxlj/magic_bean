/*
 * Copyright 2015 jordanxlj
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "no_op_event_processor.h"

namespace magic_bean {

NoOpEventProcessor::~NoOpEventProcessor() {
  sequence_.reset();
}

SequencePtr NoOpEventProcessor::GetSequence() {
  return sequence_;
}

void NoOpEventProcessor::Halt() {
  running_.store(false, std::memory_order_acquire);
}

bool NoOpEventProcessor::IsRunning() {
  return running_.load(std::memory_order_relaxed);
}

void NoOpEventProcessor::Run() {
  bool running = false;
  if(!running_.compare_exchange_strong(running, true))
    throw std::runtime_error("thread is already running.");
}

} //end namespace
