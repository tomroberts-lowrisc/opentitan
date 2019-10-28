// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "jtag_driver.h"

#include <iostream>

extern "C" void jtag_register_intf(std::string name, JtagDriver *intf);
extern "C" void jtag_deregister_intf(std::string name);

JtagDriver::JtagDriver(DVTest *test, SimCtrl *sc, DebugScoreboard *scbd, std::string name)
    : test_(test),
      simctrl_(sc),
      scbd_(scbd),
      debug_driver_(new DebugDriver(&generator_,test_)),
      name_(name) {}

JtagDriver::~JtagDriver() {
  delete debug_driver_;
}

void JtagDriver::OnInitial(unsigned int seed) {
  generator_.seed(seed);
  // 50% jtag pause states
  pause_dist_ = std::uniform_int_distribution<int>(0, 1);
  // Weighted dist 95% don't jump (since it causes errors)
  jump_dist_ = std::discrete_distribution<int>({95, 5});
  // Weighted dist since short delays cause errors
  delay_dist_ = std::discrete_distribution<int>({1, 1, 2, 2, 3, 3, 10, 20, 30});

  signal_state_ = kSignalIdle;
  transaction_state_ = kTransactionIdle;
  transaction_done_ = false;
  transaction_delay_ = 0;
  transactions_driven_ = 0;
  error_flag_ = false;
  jump_to_idle_ = false;

  testmode_ = false;
  tdi_ = false;
  tms_ = false;

  jtag_register_intf(name_,this);
}

void JtagDriver::OnFinal() {
  jtag_deregister_intf(name_);
  std::cout << "[JTAG Driver] Drove: " << transactions_driven_
            << " transactions" << std::endl;
}

void JtagDriver::GetIntfInputs(bool rst_n, bool tdo) {
  if (!rst_n) {
    Reset();
  } else {
    tdo_ = tdo;
  }
}

void JtagDriver::DriveIntfOutputs(unsigned char *testmode, unsigned char *tms, unsigned char *tdi) {
  *testmode = testmode_;
  *tms = tms_;
  *tdi = tdi_;
}

void JtagDriver::SetupNewTransaction() {
  current_trans_ = debug_driver_->GetNewTransaction(error_flag_);
  error_flag_ = false;
  instr_addr_ = current_trans_->GetJtagInstr();
  register_read_ = (instr_addr_ == kJtagRegDMI) && (current_trans_->GetRegOp() == kDebugOpRead);
  shift_data_ = current_trans_->GetJtagData();
  shift_data_ |= (uint64_t)current_trans_->GetRegOp();
  data_width_ = current_trans_->GetWidth();
}

bool JtagDriver::RandomWait() {
  return pause_dist_(generator_);
}

bool JtagDriver::RandomJump() {
  return jump_dist_(generator_);
}

bool JtagDriver::RandomDelayDone() {
  if (transaction_delay_ == 0) {
    transaction_delay_ = delay_dist_(generator_);
    return true;
  } else {
    --transaction_delay_;
    return false;
  }
}

void JtagDriver::Reset() {
  signal_state_ = kSignalIdle;
  transaction_state_ = kTransactionIdle;
  transaction_done_ = false;
  error_flag_ = false;
  jump_to_idle_ = false;
  tms_ = false;
  tdi_ = false;
  // randomize testmode on every reset
  //testmode_ = RandomJump();
}

void JtagDriver::ProcessTransactionState() {
  switch (transaction_state_) {
    case kTransactionIdle :
      SetupNewTransaction();
      transaction_state_ = kTransactionInstr;
      break;
    case kTransactionInstr :
      if (transaction_done_) {
        transaction_state_ = kTransactionDataWr;
        transaction_done_ = false;
        error_flag_ = false;
      }
      break;
    case kTransactionDataWr :
      if (transaction_done_) {
        transaction_done_ = false;
        ++transactions_driven_;
        if (register_read_) {
          transaction_state_ = kTransactionDataRd;
          shift_data_ = current_trans_->GetJtagData();
        } else {
          transaction_state_ = kTransactionIdle;
          if (current_trans_->GetJtagInstr() == kJtagRegDMI) {
            error_flag_ = register_rdata_ & 3;
            current_trans_->SetReqRData((uint32_t)(register_rdata_ >> 2));
          } else {
            current_trans_->SetReqRData((uint32_t)register_rdata_);
          }
          current_trans_->SetReqError(error_flag_);
          scbd_->PushJtagTransaction(std::move(current_trans_));
        }
      }
      break;
    case kTransactionDataRd :
      if (transaction_done_) {
        transaction_done_ = false;
        if (register_rdata_ & 3) {
          error_flag_ = true;
        }
        current_trans_->SetReqError(error_flag_);
        current_trans_->SetReqRData((uint32_t)(register_rdata_ >> 2));
        scbd_->PushJtagTransaction(std::move(current_trans_));
        transaction_state_ = kTransactionIdle;
      }
      break;
  }
}

void JtagDriver::ProcessSignalState() {
  // Standard JTAG state machine
  switch (signal_state_) {
    case kSignalReset :
      if (RandomJump()) {
        tms_ = true;
      } else {
        tms_ = false;
        signal_state_ = kSignalIdle;
      }
      break;
    case kSignalIdle :
      if (RandomDelayDone()) {
        tms_ = true;
        signal_state_ = kSignalSelectDR;
      }
      break;
    case kSignalSelectDR :
      if ((transaction_state_ == kTransactionInstr) ||
          (transaction_state_ == kTransactionIdle)) {
        tms_ = true;
        signal_state_ = kSignalSelectIR;
      } else {
        tms_ = false;
        signal_state_ = kSignalCaptureDR;
      }
      break;
    case kSignalCaptureDR :
      if (RandomJump()) {
        tms_ = true;
        jump_to_idle_ = true;
        signal_state_ = kSignalExit1DR;
      } else {
        tms_ = false;
        signal_state_ = kSignalShiftDR;
        register_rdata_ = 0;
        data_count_ = data_width_;
      }
      break;
    case kSignalShiftDR :
      tdi_ = shift_data_ & 0x1;
      shift_data_ >>= 1;
      register_rdata_ |= ((uint64_t)tdo_ << data_width_);
      register_rdata_ >>= 1;
      --data_count_;
      if (data_count_ == 0) {
        tms_ = true;
        signal_state_ = kSignalExit1DR;
      }
      break;
    case kSignalExit1DR :
      register_rdata_ |= ((uint64_t)tdo_ << data_width_);
      register_rdata_ >>= 1;
      if (RandomWait()) {
        tms_ = false;
        signal_state_ = kSignalPauseDR;
      } else {
        tms_ = true;
        signal_state_ = kSignalUpdateDR;
      }
      break;
    case kSignalPauseDR :
      if (RandomWait()) {
        tms_ = false;
      } else {
        tms_ = true;
        signal_state_ = kSignalExit2DR;
      }
      break;
    case kSignalExit2DR :
      // TODO RTL cannot handle repeated shift state
      if (jump_to_idle_ && RandomJump()) {
        tms_ = false;
        jump_to_idle_ = false;
        data_count_ = data_width_;
        signal_state_ = kSignalShiftDR;
        register_rdata_ = 0;
        shift_data_ = current_trans_->GetJtagData();
        if (transaction_state_ == kTransactionDataWr) {
          shift_data_ |= (uint64_t)current_trans_->GetRegOp();
        }
      } else {
        tms_ = true;
        signal_state_ = kSignalUpdateDR;
      }
      break;
    case kSignalUpdateDR :
      transaction_done_ = !jump_to_idle_;
      jump_to_idle_ = false;
      if (RandomJump()) {
        tms_ = true;
        signal_state_ = kSignalSelectDR;
      } else {
        tms_ = false;
        signal_state_ = kSignalIdle;
      }
      break;
    case kSignalSelectIR :
      if (RandomJump()) {
        tms_ = true;
        signal_state_ = kSignalReset;
      } else {
        tms_ = false;
        signal_state_ = kSignalCaptureIR;
      }
      break;
    case kSignalCaptureIR :
      if (RandomJump()) {
        tms_ = true;
        jump_to_idle_ = true;
        signal_state_ = kSignalExit1IR;
      } else {
        tms_ = false;
        signal_state_ = kSignalShiftIR;
        data_count_ = JTAG_INSTR_W;
      }
      break;
    case kSignalShiftIR :
      tdi_ = instr_addr_ & 0x1;
      --data_count_;
      instr_addr_ >>= 1;
      if (data_count_ == 0) {
        tms_ = true;
        signal_state_ = kSignalExit1IR;
      }
      break;
    case kSignalExit1IR :
      if (RandomWait()) {
        tms_ = false;
        signal_state_ = kSignalPauseIR;
      } else {
        tms_ = true;
        signal_state_ = kSignalUpdateIR;
      }
      break;
    case kSignalPauseIR :
      if (RandomWait()) {
        tms_ = false;
      } else {
        tms_ = true;
        signal_state_ = kSignalExit2IR;
      }
      break;
    case kSignalExit2IR :
      if (RandomJump()) {
        tms_ = false;
        jump_to_idle_ = false;
        signal_state_ = kSignalShiftIR;
        data_count_ = JTAG_INSTR_W;
        instr_addr_ = current_trans_->GetJtagInstr();
      } else {
        tms_ = true;
        signal_state_ = kSignalUpdateIR;
      }
      break;
    case kSignalUpdateIR :
      if (transaction_state_ == kTransactionInstr) {
        tms_ = true;
        signal_state_ = kSignalSelectDR;
        transaction_done_ = !jump_to_idle_;
        jump_to_idle_ = false;
      } else {
        tms_ = false;
        signal_state_ = kSignalIdle;
      }
      break;
  }
}

void JtagDriver::OnClock() {
  if (transactions_driven_ >= 100000) {
    std::cout << "Stop" << std::endl;
    simctrl_->RequestStop(true);
  }
  ProcessTransactionState();
  ProcessSignalState();
}
