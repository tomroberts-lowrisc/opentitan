// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "debug_csrs.h"

DebugCsrs::DebugCsrs(SimCtrl *sc)
    : simctrl_(sc),
      state_(new DebugState) {
  // Sweep the address range an instantiate registers that are implemented
  for (int i = 0x4; i < 0x40; ++i) {
   DebugRegAddr reg = static_cast<DebugRegAddr>(i);
    switch (reg) {
      // 2 Abstract Data registers
      case kAbstractData0 : register_map_.push_back(std::make_unique<DebugCsr>(reg,state_)); break;
      case kAbstractData1 : register_map_.push_back(std::make_unique<DebugCsr>(reg,state_)); break;
      case kDMControl : register_map_.push_back(std::make_unique<DMControlReg>(reg,state_)); break;
      case kDMStatus : register_map_.push_back(std::make_unique<DMStatusReg>(reg,state_)); break;
      case kHartInfo : register_map_.push_back(std::make_unique<HartInfoReg>(reg,state_)); break;
      case kHaltSum0 : register_map_.push_back(std::make_unique<HaltSumReg>(reg,state_)); break;
      case kHaltSum1 : register_map_.push_back(std::make_unique<HaltSumReg>(reg,state_)); break;
      case kHaltSum2 : register_map_.push_back(std::make_unique<HaltSumReg>(reg,state_)); break;
      case kHaltSum3 : register_map_.push_back(std::make_unique<HaltSumReg>(reg,state_)); break;
      case kAbstractCS : register_map_.push_back(std::make_unique<AbstractCSReg>(reg,state_)); break;
      case kCommand : register_map_.push_back(std::make_unique<AbstractCommand>(reg,state_)); break;
      // 8 progbuf entries
      case kProgBuf0 : register_map_.push_back(std::make_unique<DebugCsr>(reg,state_)); break;
      case kProgBuf1 : register_map_.push_back(std::make_unique<DebugCsr>(reg,state_)); break;
      case kProgBuf2 : register_map_.push_back(std::make_unique<DebugCsr>(reg,state_)); break;
      case kProgBuf3 : register_map_.push_back(std::make_unique<DebugCsr>(reg,state_)); break;
      case kProgBuf4 : register_map_.push_back(std::make_unique<DebugCsr>(reg,state_)); break;
      case kProgBuf5 : register_map_.push_back(std::make_unique<DebugCsr>(reg,state_)); break;
      case kProgBuf6 : register_map_.push_back(std::make_unique<DebugCsr>(reg,state_)); break;
      case kProgBuf7 : register_map_.push_back(std::make_unique<DebugCsr>(reg,state_)); break;
      case kAbstractAuto : register_map_.push_back(std::make_unique<AbstractAutoReg>(reg,state_)); break;
      case kSBCS : register_map_.push_back(std::make_unique<SBCSReg>(reg,state_)); break;
      // 1 address and data registers (32 bit)
      case kSBAddress0 : register_map_.push_back(std::make_unique<SBAddrReg>(reg,state_)); break;
      case kSBData0 : register_map_.push_back(std::make_unique<SBDataReg>(reg,state_)); break;
      default :;
    }
  }
}

DebugCsrs::~DebugCsrs() {
  delete state_;
}

void DebugCsrs::Reset() {
  state_->Reset();
  for (auto it = register_map_.begin(); it != register_map_.end(); ++it) {
    (*it)->ResetReg();
  }
}

void DebugCsrs::NewTransaction(DebugTransaction *trans) {
  for (auto it = register_map_.begin(); it != register_map_.end(); ++it) {
    if (!(*it)->ProcessTransaction(trans)) {
      simctrl_->RequestStop(false);
    }
  }
}

