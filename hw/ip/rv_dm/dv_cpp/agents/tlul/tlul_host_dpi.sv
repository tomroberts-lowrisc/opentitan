// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

package tlul_host_dpi;

  // TLUL host driver
  import "DPI-C"
  function void tlul_host_addr_tick(
    input string name,
    input bit a_ready,
    output bit a_valid,
    output bit [31:0] a_address,
    output bit [2:0] a_opcode,
    output bit [3:0] a_mask,
    output bit [1:0] a_size,
    output bit [7:0] a_source,
    output bit [31:0] a_data,
    output bit d_ready,
    input bit d_valid,
    input bit [2:0] d_opcode,
    input bit [2:0] d_size,
    input bit [7:0] d_source,
    input bit d_error,
    input bit [31:0] d_data);

endpackage
