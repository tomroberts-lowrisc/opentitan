// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef DEBUG_DRIVER_H_
#define DEBUG_DRIVER_H_

#include "debug_sequencer.h"

#include <memory>
#include <queue>
#include <random>

/**
 * Class to drive random debug transactions
 */
class DebugDriver {

 public:
  DebugDriver(std::default_random_engine *gen, DVTest *test);
  ~DebugDriver();

  /**
   * Return a new transaction
   */
  std::unique_ptr<DebugTransaction> GetNewTransaction(bool error_flag_);

 private:

  std::queue<std::unique_ptr<DebugTransaction>> trans_queue_;
  std::default_random_engine *generator_;
  DebugSequencer *sequencer_;
};

#endif // DEBUG_DRIVER_
