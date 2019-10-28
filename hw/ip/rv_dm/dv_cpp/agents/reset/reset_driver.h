// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef RESET_DRIVER_H_
#define RESET_DRIVER_H_

#include "dv_test.h"

#include <random>
#include <string>

class ResetDriver {
 public:
  ResetDriver(std::string name);

  void OnInitial(unsigned int seed, DVTest *test);

  void OnFinal();

  void DriveReset(unsigned char *rst_n);

 private:
  DVTestResetMode mode_;
  static const unsigned int kResetDuration = 3;
  unsigned int reset_delay_;
  unsigned int reset_count_;
  std::string name_;

  std::default_random_engine generator_;
  std::uniform_int_distribution<int> reset_dist_;
};

#endif // RESET_DRIVER_H_
