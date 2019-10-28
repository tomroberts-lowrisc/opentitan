// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "svdpi.h"
#include "dmi_monitor.h"

#include <map>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

static std::map<std::string,DMIMonitor*> intfs;

void dmi_register_intf(std::string name, DMIMonitor *intf) {
  intfs.insert({name,intf});
}

void dmi_deregister_intf(std::string name) {
  intfs.erase(name);
}

void dmi_req_tick(
    const char *name,
    const char rst_ni,
    const unsigned int *op,
    const unsigned int *addr,
    const unsigned int *data) {

  auto ptr = intfs.find(name);
  if (ptr != intfs.end()) {
    ptr->second->CaptureDMIReq(
        rst_ni,
        *op,
        *addr,
        *data);
  }
}

void dmi_resp_tick(
    const char *name,
    const unsigned int *resp,
    const unsigned int *data) {

  auto ptr = intfs.find(name);
  if (ptr != intfs.end()) {
    ptr->second->CaptureDMIResp(
        *resp,
        *data);
  }
}

#ifdef __cplusplus
}
#endif

