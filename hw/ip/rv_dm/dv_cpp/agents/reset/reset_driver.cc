// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "reset_driver.h"

extern "C" void rst_register_intf(std::string name, ResetDriver *intf);
extern "C" void rst_deregister_intf(std::string name);

ResetDriver::ResetDriver(std::string name)
    : reset_delay_(0), reset_count_(0), name_(name) {}

void ResetDriver::OnInitial(unsigned int seed, DVTest *test) {
  generator_.seed(seed);
  mode_ =  test->GetResetMode();
  if (mode_ == kDVResetOccasional) {
    reset_dist_ = std::uniform_int_distribution<int>(1000, 10000);
  } else {
    reset_dist_ = std::uniform_int_distribution<int>(100, 1000);
  }
  rst_register_intf(name_,this);
}

void ResetDriver::OnFinal() {
  rst_deregister_intf(name_);
}

void ResetDriver::DriveReset(unsigned char *rst_n) {
  if (reset_delay_) {
    --reset_delay_;
  } else if (mode_ != kDVResetOnce) {
    reset_delay_ = reset_dist_(generator_);
    reset_count_ = 0;
  }
  if (reset_count_ < kResetDuration) {
    ++reset_count_;
    *rst_n = 0;
  } else {
    *rst_n = 1;
  }
}

