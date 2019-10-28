// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "debug_scoreboard.h"

#include <iostream>

DebugScoreboard::DebugScoreboard(SimCtrl *sc)
    : simctrl_(sc),
      register_model_(new DebugCsrs(sc)) {}

DebugScoreboard::~DebugScoreboard() {
  delete register_model_;
}

void DebugScoreboard::Reset() {
  while (!jtag_queue_.empty()) {
    jtag_queue_.pop();
  }
  while (!dmi_queue_.empty()) {
    dmi_queue_.pop();
  }
  register_model_->Reset();
}

void DebugScoreboard::PushJtagTransaction(std::unique_ptr<DebugTransaction> trans) {
  jtag_queue_.push(std::move(trans));
  ProcessJtagQueue();
}

void DebugScoreboard::PushDMITransaction(std::unique_ptr<DebugTransaction> trans) {
  dmi_queue_.push(std::move(trans));
}

void DebugScoreboard::ProcessJtagQueue() {
  while (!jtag_queue_.empty()) {
    auto jtag_trans = jtag_queue_.front().get();
    // JTAG TAP command
    if (jtag_trans->GetJtagInstr() != kJtagRegDMI) {
      CheckJtagCmd(jtag_trans);
      jtag_queue_.pop();
      continue;
    }
    // NOP does nothing
    if (jtag_trans->GetRegOp() == kDebugOpNOP) {
      jtag_queue_.pop();
      continue;
    }
    // JTAG error cannot be checked
    if (jtag_trans->GetError()) {
      if (!dmi_queue_.empty()) {
        dmi_queue_.pop();
      }
      jtag_queue_.pop();
      continue;
    }
    if (!dmi_queue_.empty()) {
      auto dmi_trans = std::move(dmi_queue_.front());
      dmi_queue_.pop();
      if (!jtag_trans->CompareTransaction(dmi_trans.get())) {
        std::cout << "Error, JTAG/DMI mismatch" << std::endl;
        std::cout << "JTAG transaction:" << std::endl;
        jtag_trans->Print();
        std::cout << "DMI transaction:" << std::endl;
        dmi_trans->Print();
        simctrl_->RequestStop(false);
      }
      jtag_queue_.pop();
      register_model_->NewTransaction(dmi_trans.get());
    } else {
      // DMI queue is empty, don't process any more JTAG transactions
      return;
    }
  }
}

void DebugScoreboard::CheckJtagCmd(DebugTransaction *trans) {
  switch (trans->GetJtagInstr()) {
    case kJtagRegIDCode :
      if (trans->GetRData() != PARAM_IDCODEVALUE) {
        std::cout << "Error: unexpected IDCODE: " << std::hex
                  << trans->GetRData() << std::dec << std::endl;
        simctrl_->RequestStop(false);
      }
      break;
    case kJtagRegDTMCS :
      // TODO - check higher bits?
      if ((trans->GetRData() & 0x3FF) != 0x71) {
        std::cout << "Error: unexpected DTMCS: " << std::hex
                  << trans->GetRData() << std::dec << std::endl;
        simctrl_->RequestStop(false);
      }
      break;
    default :; // Bypass (do nothing)
  }
}

