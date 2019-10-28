// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "svdpi.h"
#include "jtag_driver.h"

#include <map>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

static std::map<std::string,JtagDriver*> intfs;

void jtag_register_intf(std::string name, JtagDriver *intf) {
  intfs.insert({name,intf});
}

void jtag_deregister_intf(std::string name) {
  intfs.erase(name);
}

void jtag_tick(
    const char *name,
    const svBit trst_n,
    svBit *testmode,
    svBit *tms,
    svBit *tdi,
    const svBit tdo) {

  auto ptr = intfs.find(name);
  if (ptr != intfs.end()) {
    ptr->second->GetIntfInputs(trst_n, tdo);
    ptr->second->OnClock();
    ptr->second->DriveIntfOutputs(testmode, tms, tdi);
  }
}

#ifdef __cplusplus
}
#endif
