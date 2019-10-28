// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "tlul_host_driver.h"

extern "C" void tlul_host_register_intf(std::string name, TLULHostIntf *intf);
extern "C" void tlul_host_deregister_intf(std::string name);

TLULHostDriver::TLULHostDriver(DebugScoreboard *scbd, std::string name)
    : scbd_(scbd),
      trans_outstanding_(0),
      trans_issued_(false),
      name_(name),
      addr_delay_(0),
      data_delay_(0) {}

void TLULHostDriver::OnInitial() {
  generator_ = std::default_random_engine(0);
  addr_delay_dist_ = std::uniform_int_distribution<int>(0, 10);
  data_delay_dist_ = std::uniform_int_distribution<int>(0, 10);
  intf_ = new TLULHostIntf;
  tlul_host_register_intf(name_,intf_);
}

void TLULHostDriver::OnFinal() {
  tlul_host_deregister_intf(name_);
  delete intf_;
}

void TLULHostDriver::OnClock() {
  if (intf_ == nullptr) {
    return;
  }
  ProcessAddress();
  ProcessData();
}

void TLULHostDriver::ProcessAddress() {
  if (intf_->a_ready) {
    trans_issued_ = false;
  }
  if (!trans_issued_) {
    if (!tqueue_.empty() && !RandomAddrDelay()) {
      IssueTransaction();
    }
  }
}

void TLULHostDriver::ProcessData() {
  if (intf_->d_valid) {
    if (!RandomDataDelay()) {
      CaptureData();
    }
  }
}

void TLULHostDriver::IssueTransaction() {
  TLULAddr *trans = tqueue_.front();
  tqueue_.pop();
  intf_->a_valid = 1;
  intf_->a_opcode = trans->a_opcode;
  intf_->a_address = trans->a_address;
  intf_->a_mask = trans->a_mask;
  intf_->a_size = trans->a_size;
  intf_->a_source = trans->a_source;
  intf_->a_data = trans->a_data;
  ++trans_outstanding_;
  trans_issued_ = true;
}

void TLULHostDriver::CaptureData() {
  TLULData *trans = new TLULData();
  intf_->d_ready = 1;
  trans->d_opcode = intf_->d_opcode;
  trans->d_data = intf_->d_data;
  trans->d_size = intf_->d_size;
  trans->d_source = intf_->d_source;
  trans->d_error = intf_->d_error;
  if (trans_outstanding_ == 0) {
    // error
  } else {
    --trans_outstanding_;
  }
  delete trans;
}

bool TLULHostDriver::RandomAddrDelay() {
  if (addr_delay_ == 0) {
    addr_delay_ = addr_delay_dist_(generator_);
    return false;
  } else {
    --addr_delay_;
    return true;
  }
}

bool TLULHostDriver::RandomDataDelay() {
  if (data_delay_) {
    --data_delay_;
    return true;
  } else {
    data_delay_ = data_delay_dist_(generator_);
    return false;
  }
}

