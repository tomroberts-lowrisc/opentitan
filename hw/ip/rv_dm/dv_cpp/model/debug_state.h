// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef DEBUG_STATE_H_
#define DEBUG_STATE_H_

#include <stdint.h>

/**
 * Internal debug module state
 */
class DebugState {
 public:
  void Reset();
  void PrintState();
  // Getters
  bool GetHaveReset();
  bool GetResumeAck();
  bool GetNonExist();
  bool GetUnavail();
  bool GetRunning();
  bool GetHalted();
  uint32_t GetHartSel();
  bool GetNDMReset();
  bool GetDMActive();
  bool GetBusyFlag();
  uint8_t GetCmdErr();
  bool GetSBBusyErr();
  bool GetSBBusy();
  bool GetSBReadOnAddr();
  uint8_t GetSBAccess();
  bool GetSBAutoInc();
  bool GetSBReadOnData();
  uint8_t GetSBError();
  uint32_t GetSBAddr();
  uint32_t GetSBData();
  // Setters
  void SetNDMReset(bool newval);
  void SetDMActive(bool newval);
  void SetHartSel(uint32_t newval);
  void SetHaveReset(bool newval);
  void SetResumeAck(bool newval);
  void SetSBBusyError(bool newval);
  void SetSBReadOnAddr(bool newval);
  void SetSBAccess(uint8_t newval);
  void SetSBAutoInc(bool newval);
  void SetSBReadOnData(bool newval);
  void SetSBError(uint8_t newval);
  void SetSBAddr(uint32_t newval);
  void SetSBData(uint32_t newval);
 private:
  // DMStatus
  // NOTE hart slection masks not supported all* == any*
  bool have_reset_[PARAM_NRHARTS];
  bool resume_ack_[PARAM_NRHARTS];
  bool unavailable_[PARAM_NRHARTS];
  bool halted_[PARAM_NRHARTS];
  // DMControl
  uint32_t hartsel_;
  bool ndm_reset_;
  bool dm_active_;
  // Abstract cmd
  bool busy_;
  uint8_t cmderr_;
  // system bus
  bool sb_busy_err_;
  bool sb_busy_;
  bool sb_read_on_addr_;
  uint8_t sb_access_;
  bool sb_auto_inc_;
  bool sb_read_on_data_;
  uint8_t sb_error_;
  uint32_t sb_addr_;
  uint32_t sb_data_;
};

#endif

