// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef DEBUG_CSRS_H_
#define DEBUG_CSRS_H_

#include "debug_csr.h"
#include "debug_state.h"
#include "debug_transaction.h"
#include "simctrl.h"

#include <memory>
#include <vector>

/**
 * Class storing Debug Module CSRs
 */
class DebugCsrs {
 public:
  DebugCsrs(SimCtrl *sc);
  ~DebugCsrs();

  /**
   * Process a new transaction issued by the debug agent
   */
  void NewTransaction(DebugTransaction *trans);

  /**
   * Reset all registers
   */
  void Reset();

 private:
  SimCtrl *simctrl_;
  DebugState *state_;
  std::vector<std::unique_ptr<DebugCsr>> register_map_;
};

#endif // DEBUG_CSRS_H_
