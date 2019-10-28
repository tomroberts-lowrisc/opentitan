// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "svdpi.h"
#include "tlul_types.h"

#include <map>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

static std::map<std::string,TLULHostIntf*> intfs;

void tlul_host_register_intf(std::string name, TLULHostIntf *intf) {
  intfs.insert({name,intf});
}

void tlul_host_deregister_intf(std::string name) {
  intfs.erase(name);
}

void tlul_host_addr_tick(
    const char *name,
    // Addr channel
    const svBit a_ready,
    svBit *a_valid,
    unsigned int *a_address,
    unsigned int *a_opcode,
    unsigned int *a_mask,
    unsigned int *a_size,
    unsigned int *a_source,
    unsigned int *a_data,
    // Data channel
    svBit *d_ready,
    const svBit d_valid,
    unsigned int *d_opcode,
    unsigned int *d_size,
    unsigned int *d_source,
    const svBit d_error,
    unsigned int *d_data) {

  auto ptr = intfs.find(name);
  if (ptr != intfs.end()) {
    // Addr channel
    ptr->second->a_ready = a_ready;
    *a_valid = ptr->second->a_valid;
    *a_address = ptr->second->a_address;
    *a_opcode = ptr->second->a_opcode;
    *a_mask = ptr->second->a_mask;
    *a_size = ptr->second->a_size;
    *a_source = ptr->second->a_source;
    *a_data = ptr->second->a_data;
    // Data channel
    *d_ready = ptr->second->d_ready;
    ptr->second->d_valid = d_valid;
    ptr->second->d_opcode = static_cast<TLULRespType>(*d_opcode);
    ptr->second->d_data = *d_data;
    ptr->second->d_size = *d_size;
    ptr->second->d_source = *d_source;
    ptr->second->d_error = d_error;
  }
}

#ifdef __cplusplus
}
#endif
