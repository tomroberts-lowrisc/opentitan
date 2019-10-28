// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef DEBUG_SCOREBOARD_H_
#define DEBUG_SCOREBOARD_H_

#include "debug_csrs.h"
#include "debug_transaction.h"
#include "simctrl.h"

#include <memory>
#include <queue>

/**
 * Class to compare outputs of DUT and model
 */
class DebugScoreboard {
 public:
  DebugScoreboard(SimCtrl *sc);
  ~DebugScoreboard();
  /**
   * Method to push a new Jtag transaction into the scoreboard
   */
  void PushJtagTransaction(std::unique_ptr<DebugTransaction> trans);

  /**
   * Method to push a new DMI transaction into the scoreboard
   */
  void PushDMITransaction(std::unique_ptr<DebugTransaction> trans);

  /**
   * Reset the request queues on an hardware reset
   */
  void Reset();

 private:

  void ProcessJtagQueue();
  void CheckJtagCmd(DebugTransaction *trans);

  std::queue<std::unique_ptr<DebugTransaction>> jtag_queue_;
  std::queue<std::unique_ptr<DebugTransaction>> dmi_queue_;
  SimCtrl *simctrl_;
  DebugCsrs *register_model_;

};

#endif // DEBUG_SCOREBOARD_H_
