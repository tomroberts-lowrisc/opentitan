// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef DEBUG_SEQUENCE_H_
#define DEBUG_SEQUENCE_H_

#include "debug_transaction.h"
#include "dv_test.h"

#include <memory>
#include <queue>
#include <random>

enum DebugSequenceType : int {
  kSeqRandom,
  kSeqSameCSR,
  kSeqIncrCSR
};

class DebugSequencer {
 public:
  DebugSequencer(std::default_random_engine *gen, DVTest *test, std::queue<std::unique_ptr<DebugTransaction>> *trans_queue);
  void NewSequence();
 private:
  int RandomSeqLength();
  void RandomSequence();
  void RandomStressSequence();
  void RandomBusSequence();
  void RandomAbsSequence();

  int seq_length_;
  std::default_random_engine *generator_;
  DVTest *test_;
  std::discrete_distribution<int> random_seq_;
  std::queue<std::unique_ptr<DebugTransaction>> *trans_queue_;
};

#endif // DEBUG_SEQUENCE_H_
