// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef JTAG_DRIVER_H_
#define JTAG_DRIVER_H_

#include "debug_driver.h"
#include "debug_scoreboard.h"
#include "debug_types.h"
#include "debug_transaction.h"
#include "simctrl.h"

#include <memory>
#include <random>
#include <string>

// Transaction state for JTAG operations
enum JtagTransactionState : int {
  kTransactionIdle,
  kTransactionInstr,
  kTransactionDataWr,
  kTransactionDataRd
};

// JTAG states (From IEEE 1149.1)
enum JtagSignalState : int {
  kSignalReset,
  kSignalIdle,
  kSignalSelectDR,
  kSignalSelectIR,
  kSignalCaptureDR,
  kSignalCaptureIR,
  kSignalShiftIR,
  kSignalShiftDR,
  kSignalExit1IR,
  kSignalExit1DR,
  kSignalPauseIR,
  kSignalPauseDR,
  kSignalExit2IR,
  kSignalExit2DR,
  kSignalUpdateIR,
  kSignalUpdateDR
};

/**
 * Class to drive protocol level JTAG signals
 */
class JtagDriver {

 public:
  JtagDriver(DVTest *test, SimCtrl *sc, DebugScoreboard *scbd, std::string name);
  ~JtagDriver();

  /**
   * Initial setup call
   */
  void OnInitial(unsigned int seed);

  /**
   * Method to be called at the end of simulation
   */
  void OnFinal();

  /**
   * Method to be called on every JTAG clock cycle
   */
  void OnClock();

  /**
   * Capture interface signals
   */
  void GetIntfInputs(bool rst_n, bool tdo);

  /**
   * Drive interface outputs
   */
  void DriveIntfOutputs(unsigned char *testmode, unsigned char *tms, unsigned char *tdi);

 private:
  /**
   * Return true/false randomly to insert random wait states
   */
  bool RandomWait();

  /**
   * Return true/false to randomly jump through states
   */
  bool RandomJump();

  /**
   * Return true once random delay completed
   */
  bool RandomDelayDone();

  /**
   * Called on a reset signal
   */
  void Reset();

  /**
   * Advance the transaction-layer state machine
   */
  void ProcessTransactionState();

  /**
   * Advance the signal-layer state machine
   */
  void ProcessSignalState();

  /**
   * Fetch a new transaction from the debug driver
   */
  void SetupNewTransaction();

  std::default_random_engine generator_;
  std::discrete_distribution<int> jump_dist_;
  std::uniform_int_distribution<int> pause_dist_;
  std::discrete_distribution<int> delay_dist_;

  JtagSignalState signal_state_;
  JtagTransactionState transaction_state_;
  std::unique_ptr<DebugTransaction> current_trans_;

  DVTest *test_;
  SimCtrl *simctrl_;
  DebugScoreboard *scbd_;
  DebugDriver *debug_driver_;
  std::string name_;

  uint32_t instr_addr_;
  uint64_t shift_data_;
  uint64_t register_rdata_;
  uint32_t data_count_;
  uint32_t data_width_;
  uint32_t transaction_delay_;
  bool transaction_done_;
  bool register_read_;
  bool error_flag_;
  bool jump_to_idle_;

  bool testmode_;
  bool tdi_;
  bool tdo_;
  bool tms_;
  int transactions_driven_;
};

#endif  // JTAG_DRIVER_
