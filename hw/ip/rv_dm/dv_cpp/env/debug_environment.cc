// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "debug_environment.h"

#include <iostream>

DebugEnvironment::DebugEnvironment()
    : simctrl_(new SimCtrl()),
      dv_test_(new DVTest()),
      rst_driver_(new ResetDriver("rst_n")),
      trst_driver_(new ResetDriver("trst_n")),
      debug_scbd_(new DebugScoreboard(simctrl_)),
      dmi_mon_(new DMIMonitor(debug_scbd_,"dmi")),
      jtag_driver_(new JtagDriver(dv_test_,simctrl_,debug_scbd_,"jtag")) {}

DebugEnvironment::~DebugEnvironment() {
  delete jtag_driver_;
  delete dmi_mon_;
  delete trst_driver_;
  delete rst_driver_;
  delete simctrl_;
  delete debug_scbd_;
}

void DebugEnvironment::OnInitial(unsigned int seed, char *test) {
  if (!dv_test_->ReadTestConfig(test)) {
    simctrl_->RequestStop(false);
  }
  dv_test_->PrintConfig();
  std::cout << "Seed: " << seed << std::endl;
  rst_driver_->OnInitial(seed,dv_test_);
  trst_driver_->OnInitial(seed,dv_test_);
  jtag_driver_->OnInitial(seed);
  dmi_mon_->OnInitial();
}

void DebugEnvironment::OnFinal() {
  rst_driver_->OnFinal();
  trst_driver_->OnFinal();
  jtag_driver_->OnFinal();
  dmi_mon_->OnFinal();
}

void DebugEnvironment::GetStopReq(unsigned char *stop_req) {
  *stop_req = simctrl_->StopRequested();
}
