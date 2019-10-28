// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "debug_transaction.h"

#include <iostream>

DebugTransaction::DebugTransaction()
    : jtag_dist_(std::discrete_distribution<int>({1,1,5,92,1})),
      op_dist_(std::discrete_distribution<int>({1,50,49})),
      reg_dist_(std::uniform_int_distribution<int>(0x4,0x40)),
      wdata_dist_(std::uniform_int_distribution<int>(0x0,0xFFFFFFFF)) {}

void DebugTransaction::SetupErrorClear() {
  jtag_reg_ = kJtagRegDTMCS;
  debug_op_ = kDebugOpNOP;
  // bit[16] = DMI reset
  debug_wdata_ = 0x10000;
  debug_error_ = false;
  debug_rdata_ = 0;
}

void DebugTransaction::Randomize(std::default_random_engine *gen) {
  // jtag_dist_ returns an index into the enum
  jtag_reg_ = IndexToTransType(jtag_dist_(*gen));
  // Randomize all fields since the transaction might be changed
  // afterwards as part of a sequence
  debug_reg_ = static_cast<DebugRegAddr>(reg_dist_(*gen));
  debug_op_ = static_cast<DebugOpType>(op_dist_(*gen));
  debug_wdata_ = wdata_dist_(*gen);
  debug_error_ = false;
  debug_rdata_ = 0;
}

void DebugTransaction::SetJtagInstr(JtagTransType instr) {
  jtag_reg_ = instr;
}

void DebugTransaction::SetReqOp(DebugOpType op) {
  debug_op_ = op;
}

void DebugTransaction::SetReqAddr(DebugRegAddr addr) {
  debug_reg_ = addr;
}

void DebugTransaction::SetReqWdata(uint32_t wdata) {
  debug_wdata_ = wdata;
}

void DebugTransaction::SetReqRData(uint32_t rdata) {
  debug_rdata_ = rdata;
}

void DebugTransaction::SetReqError(bool error) {
  debug_error_ = error;
}

JtagTransType DebugTransaction::GetJtagInstr() {
  return jtag_reg_;
}

DebugRegAddr DebugTransaction::GetRegAddr() {
  return debug_reg_;
}

uint32_t DebugTransaction::GetWData() {
  return debug_wdata_;
}

uint64_t DebugTransaction::GetJtagData() {
  if (jtag_reg_ == kJtagRegDMI) {
    return (((uint64_t)debug_reg_ << JTAG_DATA_W) |
            debug_wdata_) << DM_OP_W;
  } else {
    return debug_wdata_;
  }
}

uint32_t DebugTransaction::GetRData() {
  return debug_rdata_;
}

DebugOpType DebugTransaction::GetRegOp() {
  return debug_op_;
}

bool DebugTransaction::GetError() {
  return debug_error_;
}

uint32_t DebugTransaction::GetWidth() {
  return (jtag_reg_ == kJtagRegDMI) ? (JTAG_DATA_W + DM_ADDR_W + DM_OP_W) :
                                      JTAG_DATA_W;
}

bool DebugTransaction::CompareTransaction(DebugTransaction *trans) {
  // If the reference transaction (DMI) transmitted an error,
  // this transaction must have recorded an error
  if (trans->GetError() && !GetError()) {
    return false;
  }
  // If this transaction receives an error for some other reason
  // the fields can't be checked
  if (GetError()) {
    return true;
  }
  if (trans->GetRegAddr() != GetRegAddr()) {
    return false;
  }
  if (trans->GetRegOp() != GetRegOp()) {
    return false;
  }
  if (debug_op_ == kDebugOpWrite) {
    if (trans->GetWData() != GetWData()) {
      return false;
    }
  }
  if (debug_op_ == kDebugOpRead) {
    if (trans->GetRData() != GetRData()) {
      return false;
    }
  }
  return true;
}

JtagTransType DebugTransaction::IndexToTransType(int i) {
  switch (i) {
    case 0 : return kJtagRegBypass;
    case 1 : return kJtagRegIDCode;
    case 2 : return kJtagRegDTMCS;
    case 3 : return kJtagRegDMI;
    default : return kJtagRegBP;
  }
}

std::string DebugTransaction::DebugOpAsString(DebugOpType op) {
  switch (op) {
    case kDebugOpNOP   : return "Debug op NOP";
    case kDebugOpRead  : return "Debug op Read";
    case kDebugOpWrite : return "Debug op Write";
    default            : return "Unknown operation!";
  }
}

std::string DebugTransaction::JtagReqAsString(JtagTransType reg) {
  switch (reg) {
    case kJtagRegBypass : return "Jtag Bypass 0";
    case kJtagRegIDCode : return "Jtag ID code";
    case kJtagRegDTMCS  : return "Jtag DTMCS register";
    case kJtagRegDMI    : return "Jtag DMI register";
    case kJtagRegBP     : return "Jtag Bypass 31";
    default             : return "Unknown register!";
  }
}

std::string DebugTransaction::DebugRegAsString(DebugRegAddr reg) {
  switch (reg) {
    case kAbstractData0 : return "Abstract data 0";
    case kAbstractData1 : return "Abstract data 1";
    case kDMControl : return "DM Control";
    case kDMStatus : return "DM Status";
    case kHartInfo : return "Hart Info";
    case kHaltSum0 : return "Halt Summary 0";
    case kHaltSum1 : return "Halt Summary 1";
    case kHaltSum2 : return "Halt Summary 2";
    case kHaltSum3 : return "Halt Summary 3";
    case kAbstractCS : return "Abstract Control / Status";
    case kCommand : return "Abstract Command";
    case kProgBuf0 : return "Program Buffer 0";
    case kProgBuf1 : return "Program Buffer 1";
    case kProgBuf2 : return "Program Buffer 2";
    case kProgBuf3 : return "Program Buffer 3";
    case kProgBuf4 : return "Program Buffer 4";
    case kProgBuf5 : return "Program Buffer 5";
    case kProgBuf6 : return "Program Buffer 6";
    case kProgBuf7 : return "Program Buffer 7";
    case kAbstractAuto : return "Abstract Autoexec";
    case kSBCS : return "System Bus Control / Status";
    // 2 address and data registers
    case kSBAddress0 : return "System Bus Address 0";
    case kSBAddress1 : return "System Bus Address 1";
    case kSBData0 : return "System Bus Data 0";
    case kSBData1 : return "System Bus Data 1";
    default : return "Unknown / unimplemented register: " + std::to_string(reg);
  }
}

void DebugTransaction::Print() {
  std::cout << "Jtag type: " << JtagReqAsString(jtag_reg_)
            << std::hex << std::endl;
  if (jtag_reg_ == kJtagRegDMI) {
    std::cout << "Reg addr:  " << DebugRegAsString(debug_reg_) << std::endl
              << "Reg op:    " << DebugOpAsString(debug_op_) << std::endl
              << "Reg wdata: " << debug_wdata_ << std::endl
              << "Reg rdata: " << debug_rdata_ << std::endl
              << "Error:     " << (debug_error_ ? "Yes" : "No") << std::endl;
  }
  std::cout << std::dec << std::endl;
}
