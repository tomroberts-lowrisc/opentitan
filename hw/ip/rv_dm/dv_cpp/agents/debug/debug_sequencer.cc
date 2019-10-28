// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "debug_sequencer.h"

DebugSequencer::DebugSequencer(std::default_random_engine *gen, DVTest *test, std::queue<std::unique_ptr<DebugTransaction>> *trans_queue)
    : generator_(gen),
      test_(test),
      random_seq_(std::discrete_distribution<int>({90,10})),
      trans_queue_(trans_queue) {}

void DebugSequencer::NewSequence() {
  // Get test config
  DVCSRMode mode = test_->GetCSRMode();
  // Periodically generate random sequence regardless of config
  if ((mode == kDVCSRRandom) || random_seq_(*generator_)) {
    RandomSequence();
  } else {
    // Generate sequences depending on config
    switch (mode) {
      case kDVCSRBusAccess :
        RandomBusSequence();
        break;
      case kDVCSRAbsCmd :
        RandomAbsSequence();
        break;
      case kDVCSRStress :
        RandomStressSequence();
      default :;
        // Do nothing
    }
  }
}

void DebugSequencer::RandomSequence() {
  // Generate sequence length
  seq_length_ = RandomSeqLength();
  // Generate sequence items
  auto trans = std::make_unique<DebugTransaction>();
  trans->Randomize(generator_);
  trans_queue_->push(std::move(trans));
  for (int i = 1; i < seq_length_; ++i) {
    auto trans = std::make_unique<DebugTransaction>();
    trans->Randomize(generator_);
    trans_queue_->push(std::move(trans));
  }
}

void DebugSequencer::RandomStressSequence() {
  // Generate sequence length
  seq_length_ = RandomSeqLength();
  // Generate sequence items
  auto trans = std::make_unique<DebugTransaction>();
  trans->Randomize(generator_);
  trans_queue_->push(std::move(trans));
  for (int i = 1; i < seq_length_; ++i) {
    auto trans = std::make_unique<DebugTransaction>();
    trans->Randomize(generator_);
    trans->SetJtagInstr(trans_queue_->front()->GetJtagInstr());
    trans->SetReqAddr(trans_queue_->front()->GetRegAddr());
    trans_queue_->push(std::move(trans));
  }
}

void DebugSequencer::RandomBusSequence() {
}

void DebugSequencer::RandomAbsSequence() {
}

int DebugSequencer::RandomSeqLength() {
  std::uniform_int_distribution<int> length(2,10);
  return length(*generator_);
}
