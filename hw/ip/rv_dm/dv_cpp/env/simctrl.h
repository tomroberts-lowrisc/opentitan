// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef SIMCTRL_H_
#define SIMCTRL_H_

class SimCtrl {
 public:
  SimCtrl();

  void RequestStop(bool success);

  bool StopRequested();

 private:
  bool stop_request_;
  bool test_success_;
};

#endif // SIMCTRL_H_
