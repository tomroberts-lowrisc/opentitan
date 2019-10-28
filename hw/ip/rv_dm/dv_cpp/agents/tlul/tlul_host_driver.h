// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef TLUL_HOST_DRIVER_H_
#define TLUL_HOST_DRIVER_H_

#include "debug_scoreboard.h"
#include "tlul_types.h"

#include <random>
#include <string>

class TLULHostDriver {

 public:
  TLULHostDriver(DebugScoreboard *scbd, std::string name);

  void OnClock();
  void OnInitial();
  void OnFinal();

 private:

  void ProcessAddress();
  void ProcessData();

  void IssueTransaction();
  void CaptureData();

  bool RandomAddrDelay();
  bool RandomDataDelay();

  DebugScoreboard *scbd_;
  std::queue<TLULAddr*> tqueue_;
  int trans_outstanding_;
  bool trans_issued_;
  TLULHostIntf *intf_;
  std::string name_;
  int addr_delay_;
  int data_delay_;

  std::default_random_engine generator_;
  std::uniform_int_distribution<int> addr_delay_dist_;
  std::uniform_int_distribution<int> data_delay_dist_;
};

#endif // TLUL_HOST_DRIVER_H_
