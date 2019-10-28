// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "debug_csr.h"

#include <iostream>

DebugCsr::DebugCsr(DebugRegAddr addr, DebugState *state)
    : state_(state),
      reg_addr_(addr) {}

bool DebugCsr::ProcessTransaction(DebugTransaction *trans) {
  if (!MatchAddr(trans->GetRegAddr())) {
    return true;
  }
  if (trans->GetRegOp() == kDebugOpWrite) {
    WriteReg(trans->GetWData());
    return true;
  }
  if (trans->GetRegOp() == kDebugOpRead) {
    if (trans->GetRData() != ReadReg()) {
      // TODO - RTL does weird things with this register
      if (trans->GetRegAddr() != kAbstractCS) {
        std::cout << "Error, read data mismatch" << std::endl
                  << "Expected rdata: " << std::hex << ReadReg()
                  << std::dec << std::endl;
        trans->Print();
        state_->PrintState();
        return false;
      }
    }
  }
  return true;
}

bool DebugCsr::MatchAddr(DebugRegAddr addr) {
  return (addr == reg_addr_);
}

void DebugCsr::WriteReg(uint32_t wdata) {
  if (state_->GetDMActive()) {
    reg_value_ = wdata;
  }
}

void DebugCsr::ResetReg() {
  reg_value_ = 0;
}

uint32_t DebugCsr::ReadReg() {
  return reg_value_;
}

uint32_t DMStatusReg::ReadReg() {
  // impebreak = 0
  uint32_t regval = (0x0 << 22);
  // allhavereset
  regval |= (state_->GetHaveReset() << 19);
  // anyhavereset
  regval |= (state_->GetHaveReset() << 18);
  // allresumeack
  regval |= (state_->GetResumeAck() << 17);
  // anyresumeack
  regval |= (state_->GetResumeAck() << 16);
  // allnonexistent
  regval |= (state_->GetNonExist() << 15);
  // anynonexistent
  regval |= (state_->GetNonExist() << 14);
  // allunavail
  regval |= (state_->GetUnavail() << 13);
  // anyunavail
  regval |= (state_->GetUnavail() << 12);
  // allrunning
  regval |= (state_->GetRunning() << 11);
  // anyrunning
  regval |= (state_->GetRunning() << 10);
  // allhalted
  regval |= (state_->GetHalted() << 9);
  // anyhalted
  regval |= (state_->GetHalted() << 8);
  // authenticated = 1 (not supported)
  regval |= (0x1 << 7);
  // authbusy = 0 (not supported)
  regval |= (0x0 << 6);
  // hasresethaltreq = 0
  regval |= (0x0 << 5);
  // confstrptrvalid = 0
  regval |= (0x0 << 4);
  // version = 2 (version 0.13)
  regval |= (0x2 << 0);
  return regval;
}

void DMControlReg::WriteReg(uint32_t wdata) {
  // dmactive - if 0, all writes are ignored
  if (state_->GetDMActive()) {
    // ndmreset - reset rest of system
    state_->SetNDMReset((wdata >> 1) & 0x1);
    // hartselhi
    uint32_t hartsel = ((wdata >> 6) & 0x3FF) << 10;
    // hartsello
    hartsel |= (wdata >> 16) & 0x3FF;
    state_->SetHartSel(hartsel);
  }
  // Set the new value of DMActive
  state_->SetDMActive(wdata & 0x1);
  // ackhavereset
  if ((wdata >> 28) & 0x1) {
    state_->SetHaveReset(false);
  }
  // resumereq
  if ((wdata >> 30) & 0x1) {
    state_->SetResumeAck(false);
  }
}

uint32_t DMControlReg::ReadReg() {
  // hart_reset - not implemented RAZ
  uint32_t regval = (0x0 << 29);
  // hasel - not supported RAZ
  regval |= (0x0 << 26);
  // hartsello
  regval |= ((state_->GetHartSel() & 0x3FF) << 16);
  // hartselhi
  regval |= (((state_->GetHartSel() >> 10) & 0x3FF) << 6);
  // ndmreset
  regval |= (state_->GetNDMReset() << 1);
  // dmactive
  regval |= state_->GetDMActive();
  return regval;
}

uint32_t HartInfoReg::ReadReg() {
  if (state_->GetNonExist()) {
    // TODO - RTL returns X. Check OK
    return 0;
  }
  // nscratch (fixed at 2)
  uint32_t retval = (0x2 << 20);
  // dataaccess (fixed at 1)
  retval |= (0x1 << 16);
  // datasize (fixed at 2)
  retval |= (0x2 << 12);
  // dataaddr (fixed at 0x380)
  retval |= 0x380;
  return retval;
}

void HaltSumReg::WriteReg(uint32_t wdata) {
  // do nothing
}

uint32_t AbstractCSReg::ReadReg() {
  // progbufsize (fixed at 8)
  uint32_t retval = (0x8 << 24);
  // busy flag
  retval |= (state_->GetBusyFlag() << 12);
  // cmderr flag
  retval |= (state_->GetCmdErr() << 8);
  // datacount (fixed at 2)
  retval |= 0x2;
  return retval;
}

uint32_t AbstractAutoReg::ReadReg() {
  // 8 progbuf entries
  uint32_t mask = 0xFF << 16;
  // 2 data entries
  mask |= 0x3;
  return reg_value_ & mask;
}

uint32_t AbstractCommand::ReadReg() {
  // RAZ register
  return 0;
}

uint32_t SBCSReg::ReadReg() {
  // sbversion = 1
  uint32_t retval = (0x1 << 29);
  // sbbusyerr
  retval |= (state_->GetSBBusyErr() << 22);
  // sbbusy
  retval |= (state_->GetSBBusy() << 21);
  // sbreadonaddr
  retval |= (state_->GetSBReadOnAddr() << 20);
  // sbaccess
  retval |= (state_->GetSBAccess() << 17);
  // sbautoincrement
  retval |= (state_->GetSBAutoInc() << 16);
  // sbreadondata
  retval |= (state_->GetSBReadOnData() << 15);
  // sberror
  retval |= (state_->GetSBError() << 12);
  // sbasize = 32
  retval |= (32 << 5);
  // sbaccess128
  retval |= (0x0 << 4);
  // sbaccess64
  retval |= (0x0 << 3);
  // sbaccess32
  retval |= (0x1 << 2);
  // sbaccess16
  retval |= (0x0 << 1);
  // sbaccess8
  retval |= (0x0 << 0);
  return retval;
}

void SBCSReg::WriteReg(uint32_t wdata) {
  if ((wdata >> 22) & 0x1) {
    state_->SetSBBusyError(false);
  }
  state_->SetSBReadOnAddr((wdata >> 20) & 0x1);
  state_->SetSBAccess((wdata >> 17) & 0x7);
  state_->SetSBAutoInc((wdata >> 16) & 0x1);
  state_->SetSBReadOnData((wdata >> 15) & 0x1);
  if ((wdata >> 12) & 0x7) {
    state_->SetSBError(0);
  }
}

uint32_t SBAddrReg::ReadReg() {
  return state_->GetSBAddr();
}

void SBAddrReg::WriteReg(uint32_t wdata) {
  state_->SetSBAddr(wdata);
}

uint32_t SBDataReg::ReadReg() {
  return state_->GetSBData();
}

void SBDataReg::WriteReg(uint32_t wdata) {
  state_->SetSBData(wdata);
}
