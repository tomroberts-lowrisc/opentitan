// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

package dmi_dpi;

  import "DPI-C"
  function void dmi_req_tick(
    input string name,
    input bit rst_ni,
    input bit [1:0] op,
    input bit [6:0] addr,
    input bit [31:0] data);

  import "DPI-C"
  function void dmi_resp_tick(
    input string name,
    input bit [1:0] resp,
    input bit [31:0] data);

endpackage

