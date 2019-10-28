// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef DMI_MONITOR_H_
#define DMI_MONITOR_H_

#include "debug_scoreboard.h"
#include "debug_transaction.h"

#include <memory>
#include <queue>
#include <string>

class DMIMonitor {
 public:
  DMIMonitor(DebugScoreboard *scbd, std::string name);

  void OnInitial();

  void OnFinal();

  void CaptureDMIReq(unsigned char rst_ni, unsigned int op, unsigned int addr, unsigned int data);
  void CaptureDMIResp(unsigned int resp, unsigned int data);

 private:

  std::queue<std::unique_ptr<DebugTransaction>> req_queue_;

  DebugScoreboard *scbd_;
  std::string name_;
};

#endif // DMI_MONITOR_H_
