// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "debug_driver.h"

#include <iostream>

DebugDriver::DebugDriver(std::default_random_engine *gen, DVTest *test)
    : generator_(gen),
      sequencer_(new DebugSequencer(generator_, test, &trans_queue_)) {}

DebugDriver::~DebugDriver() {
  delete sequencer_;
}

std::unique_ptr<DebugTransaction> DebugDriver::GetNewTransaction(bool error_flag) {
  if (trans_queue_.empty()) {
    sequencer_->NewSequence();
    // Clear the error once the current sequence has completed
    if (error_flag) {
      auto trans = std::make_unique<DebugTransaction>();
      trans->SetupErrorClear();
      return trans;
    }
  }
  if (trans_queue_.empty()) {
    sequencer_->NewSequence();
  }
  auto trans = std::move(trans_queue_.front());
  trans_queue_.pop();
  return trans;
}

