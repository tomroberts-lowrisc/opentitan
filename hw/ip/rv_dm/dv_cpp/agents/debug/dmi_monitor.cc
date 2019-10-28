// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "dmi_monitor.h"

#include <iostream>
#include <memory>

extern "C" void dmi_register_intf(std::string name, DMIMonitor *intf);
extern "C" void dmi_deregister_intf(std::string name);

DMIMonitor::DMIMonitor(DebugScoreboard *scbd, std::string name)
    : scbd_(scbd),
      name_(name) {}

void DMIMonitor::OnInitial() {
  dmi_register_intf(name_, this);
}

void DMIMonitor::OnFinal() {
  dmi_deregister_intf(name_);
}

void DMIMonitor::CaptureDMIReq(unsigned char rst_ni, unsigned int op, unsigned int addr, unsigned int data) {
  if (!rst_ni) {
    while (!req_queue_.empty()) {
      req_queue_.pop();
    }
    scbd_->Reset();
    return;
  }
  auto trans = std::make_unique<DebugTransaction>();
  trans->SetJtagInstr(kJtagRegDMI);
  trans->SetReqOp((DebugOpType)op);
  trans->SetReqAddr((DebugRegAddr)addr);
  trans->SetReqWdata(data);
  req_queue_.push(std::move(trans));
}

void DMIMonitor::CaptureDMIResp(unsigned int resp, unsigned int data) {
  if (req_queue_.empty()) {
    std::cout << "Error: unexpected resp" << std::endl;
  } else {
    auto trans = std::move(req_queue_.front());
    req_queue_.pop();
    trans->SetReqError(resp);
    trans->SetReqRData(data);
    scbd_->PushDMITransaction(std::move(trans));
  }
}

