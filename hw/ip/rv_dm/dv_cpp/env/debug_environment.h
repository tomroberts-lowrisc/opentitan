// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef DEBUG_ENVIRONMENT_H_
#define DEBUG_ENVIRONMENT_H_

#include "debug_scoreboard.h"
#include "dmi_monitor.h"
#include "jtag_driver.h"
#include "reset_driver.h"
#include "simctrl.h"
#include "dv_test.h"

#include <random>

/**
 * Class to contain all tb components
 */
class DebugEnvironment {

 public:
  DebugEnvironment();
  ~DebugEnvironment();

  /**
   * Initial setup call
   */
  void OnInitial(unsigned int seed, char *test);

  /**
   * Method to be called at the end of simulation
   */
  void OnFinal();

  /**
   * Method to be called on every JTAG clock cycle
   */
  void GetStopReq(unsigned char *stop_req);

 private:

  SimCtrl *simctrl_;
  DVTest *dv_test_;
  ResetDriver *rst_driver_;
  ResetDriver *trst_driver_;
  DebugScoreboard *debug_scbd_;
  DMIMonitor *dmi_mon_;
  JtagDriver *jtag_driver_;

};

#endif  // DEBUG_ENVIRONMENT_H_
