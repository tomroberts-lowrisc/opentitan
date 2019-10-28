// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "debug_state.h"

#include <iostream>

void DebugState::Reset() {
  // TODO - wrong reset
  for (int i = 0; i < PARAM_NRHARTS; ++i) {
    have_reset_[i] = true;
  }

  hartsel_ = 0;
  ndm_reset_ = false;
  dm_active_ = false;
  busy_ = false;
  cmderr_ = 0;
  sb_busy_err_ = false;
  sb_busy_ = false;
  sb_read_on_addr_ = false;
  sb_access_ = 0x2;
  sb_auto_inc_ = false;
  sb_read_on_data_ = false;
  sb_error_ = 0;
  sb_addr_ = 0;
  sb_data_ = 0;
}

void DebugState::PrintState() {
  std::cout << "Debug module internal state:" << std::endl
            << "Hartsel:   " << std::hex << hartsel_ << std::endl
            << "DM active: " << dm_active_ << std::endl
            << "SB busy:   " << sb_busy_ << std::endl
            << "SB berr:   " << sb_busy_err_ << std::endl
            << "SB err:    " << sb_error_ << std::dec << std::endl;
}

// Getters
bool DebugState::GetHaveReset() {
  if (hartsel_ < PARAM_NRHARTS) {
    return have_reset_[hartsel_];
  }
  return false;
}

bool DebugState::GetResumeAck() {
  if (hartsel_ < PARAM_NRHARTS) {
    return resume_ack_[hartsel_];
  }
  return false;
}

bool DebugState::GetNonExist() {
  return hartsel_ >= PARAM_NRHARTS;
}

bool DebugState::GetUnavail() {
  if (hartsel_ < PARAM_NRHARTS) {
    return unavailable_[hartsel_];
  }
  return false;
}

bool DebugState::GetRunning() {
  if (hartsel_ < PARAM_NRHARTS) {
    return !halted_[hartsel_];
  }
  return true;
}

bool DebugState::GetHalted() {
  if (hartsel_ < PARAM_NRHARTS) {
    return halted_[hartsel_];
  }
  return false;
}

uint32_t DebugState::GetHartSel() {
  return hartsel_;
}

bool DebugState::GetNDMReset() {
  return ndm_reset_;
}

bool DebugState::GetDMActive() {
  return dm_active_;
}

bool DebugState::GetBusyFlag() {
  return busy_;
}

uint8_t DebugState::GetCmdErr() {
  return cmderr_;
}

bool DebugState::GetSBBusyErr() {
  return sb_busy_err_;
}

bool DebugState::GetSBBusy() {
  return sb_busy_;
}

bool DebugState::GetSBReadOnAddr() {
  return sb_read_on_addr_;
}

uint8_t DebugState::GetSBAccess() {
  return sb_access_;
}

bool DebugState::GetSBAutoInc() {
  return sb_auto_inc_;
}

bool DebugState::GetSBReadOnData() {
  return sb_read_on_data_;
}

uint8_t DebugState::GetSBError() {
  return sb_error_;
}

uint32_t DebugState::GetSBAddr() {
  // TODO - RTL returns 0 rather than actual value?
  return sb_busy_ ? 0 : sb_addr_;
}

uint32_t DebugState::GetSBData() {
  if (sb_busy_) {
    sb_busy_err_ = true;
    // TODO - RTL returns 0 rather than actual value?
    return 0;
  }
  if (sb_read_on_data_ && !sb_busy_err_) {
    sb_busy_ = true;
  }
  return sb_addr_;
}

// Setters
void DebugState::SetNDMReset(bool newval) {
  ndm_reset_ = newval;
}

void DebugState::SetDMActive(bool newval) {
  dm_active_ = newval;
  if (!newval) {
    Reset();
  }
}

void DebugState::SetHartSel(uint32_t newval) {
  hartsel_ = newval;
}

void DebugState::SetHaveReset(bool newval) {
  if (hartsel_ < PARAM_NRHARTS) {
    have_reset_[hartsel_] = newval;
  }
}

void DebugState::SetResumeAck(bool newval) {
  if (hartsel_ < PARAM_NRHARTS) {
    resume_ack_[hartsel_] = newval;
  }
}

void DebugState::SetSBBusyError(bool newval) {
  sb_busy_err_ = newval;
}

void DebugState::SetSBReadOnAddr(bool newval) {
  sb_read_on_addr_ = newval;
}

void DebugState::SetSBAccess(uint8_t newval) {
  sb_access_ = newval;
}

void DebugState::SetSBAutoInc(bool newval) {
  sb_auto_inc_ = newval;
}

void DebugState::SetSBReadOnData(bool newval) {
  sb_read_on_data_ = newval;
}

void DebugState::SetSBError(uint8_t newval) {
  sb_error_ = newval;
}

void DebugState::SetSBAddr(uint32_t newval) {
  if (sb_busy_) {
    sb_busy_err_ = true;
    return;
  }
  sb_addr_ = newval;
  if (!sb_busy_err_ && sb_read_on_addr_) {
    sb_busy_ = true;
  }
}

void DebugState::SetSBData(uint32_t newval) {
  if (sb_busy_) {
    sb_busy_err_ = true;
    return;
  }
  if (!sb_busy_ && !sb_busy_err_) {
    sb_data_ = newval;
    sb_busy_ = true;
  }
}

