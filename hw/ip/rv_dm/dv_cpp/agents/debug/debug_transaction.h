// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef DEBUG_TRANSACTION_H_
#define DEBUG_TRANSACTION_H_

#include "debug_types.h"

#include <random>
#include <stdint.h>
#include <string>

/**
 * Class encapsulating a debug transaction
 */
class DebugTransaction {
 public:

  DebugTransaction();

  /**
   * Randomize class fields to new values
   */
  void Randomize(std::default_random_engine *gen);

  /*
   * Set this transaction up to clear DMI errors (not random)
   */
  void SetupErrorClear();

  /**
   * Print the details of this transaction
   */
  void Print();

  /**
   * Get functions
   */
  JtagTransType GetJtagInstr();

  uint32_t GetWData();

  uint64_t GetJtagData();

  uint32_t GetRData();

  DebugRegAddr GetRegAddr();

  DebugOpType GetRegOp();

  bool GetError();

  uint32_t GetWidth();

  /**
   * Set functions
   */
  void SetJtagInstr(JtagTransType instr);

  void SetReqOp(DebugOpType op);

  void SetReqAddr(DebugRegAddr addr);

  void SetReqWdata(uint32_t wdata);

  void SetReqRData(uint32_t rdata);

  void SetReqError(bool error);

  /**
   * Compare this transaction to a reference
   */
  bool CompareTransaction(DebugTransaction *trans);

 private:

  JtagTransType IndexToTransType(int i);
  std::string DebugOpAsString(DebugOpType op);
  std::string JtagReqAsString(JtagTransType reg);
  std::string DebugRegAsString(DebugRegAddr reg);

  JtagTransType jtag_reg_;
  DebugRegAddr  debug_reg_;
  DebugOpType debug_op_;
  uint32_t debug_wdata_;
  uint32_t debug_rdata_;
  bool debug_error_;

  std::discrete_distribution<int> jtag_dist_;
  std::discrete_distribution<int> op_dist_;
  std::uniform_int_distribution<int> reg_dist_;
  std::uniform_int_distribution<int> wdata_dist_;
};

#endif // DEBUG_TRANSACTION_H_
