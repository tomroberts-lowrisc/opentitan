// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

package jtag_dpi;

  import "DPI-C"
  function void jtag_tick(
    input string name,
    input bit trst_n,
    output bit testmode,
    output bit tms,
    output bit tdi,
    input bit tdo);

endpackage

