// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

// Top level testbench module
// Instantiates the DUT and calls DPI functions
module testbench #(
  parameter int                 NrHarts = 1,
  parameter logic [31:0]        IdcodeValue = 32'h 0000_0001
) (
  inout  wire               clk_i,       // clock
  inout  wire               in_rst_ni    // asynchronous reset active low, connect PoR
                                          // here, not the system reset

);

  // reset driven by DPI
  logic dpi_rst_ni;
  logic dpi_trst_ni;

  // combined reset
  logic rst_ni;
  logic trst_ni;

  assign rst_ni = in_rst_ni & dpi_rst_ni;
  assign trst_ni = in_rst_ni & dpi_trst_ni;

  logic               tck_i;
  logic               testmode_i;
  logic               ndmreset_o;  // non-debug module reset
  logic               dmactive_o;  // debug module is active
  logic [NrHarts-1:0] debug_req_o; // async debug request
  logic [NrHarts-1:0] unavailable_i; // communicate whether the hart is unavailable
                                     // (e.g.: power down)

  // TODO !!!
  assign unavailable_i = '0;

  // bus device with debug memory, for an execution based technique
  tlul_pkg::tl_h2d_t tl_d_i;
  tlul_pkg::tl_d2h_t tl_d_o;

  // bus host, for system bus accesses
  tlul_pkg::tl_h2d_t  tl_h_o;
  tlul_pkg::tl_d2h_t  tl_h_i;

  // JTAG signals
  logic               tms_i;           // JTAG test mode select pad
  logic               td_i;            // JTAG test data input pad
  logic               td_o;            // JTAG test data output pad
  logic               tdo_oe_o;        // Data out output enable

  //------------------
  // DUT instantiation
  //------------------
  rv_dm #(
    .NrHarts (NrHarts),
    .IdcodeValue (IdcodeValue)
  ) i_rv_dm (.*);

  //-----------------
  // Clock generation
  //-----------------
`ifndef VERILATOR
  logic in_clk_i;
  initial begin
    in_clk_i = 1'b0;
    while (1) begin
      #10
      in_clk_i = !in_clk_i;
    end
  end

  initial begin
    tck_i = 1'b0;
    while (1) begin
      #15
      tck_i = !tck_i;
    end
  end

  assign clk_i = in_clk_i;
  assign in_rst_ni = 1'b1;
`else
  // Sync clocking for verilator
  assign tck_i = clk_i;
`endif


  //----------
  // DPI calls
  //----------

  bit stop_simulation;
  bit [31:0] seed;
  string test;

  initial begin
    if (!$value$plusargs ("ntb_random_seed=%d", seed)) begin
      seed = 0;
    end
    if (!$value$plusargs ("test=%s", test)) begin
      test = "";
    end
    env_dpi::env_initial(seed,test);
  end

  final begin
    env_dpi::env_final();
  end

  always_ff @(posedge clk_i) begin
    env_dpi::env_tick(stop_simulation);
    rst_dpi::rst_tick("rst_n",dpi_rst_ni);
    rst_dpi::rst_tick("trst_n",dpi_trst_ni);
    if (stop_simulation) begin
      $finish();
    end
  end

  always_ff @(posedge tck_i or negedge tck_i or negedge trst_ni) begin
    // testmode == 0 -> posedge clocking
    // testmode == 1 -> negedge clocking
    if (!trst_ni || (testmode_i != tck_i)) begin
      jtag_dpi::jtag_tick("jtag",
                          trst_ni,
                          testmode_i,
                          tms_i,
                          td_i,
                          td_o);
    end
  end

  always_ff @(posedge clk_i or negedge rst_ni) begin
    if ((i_rv_dm.dmi_req_valid && i_rv_dm.dmi_req_ready) || !rst_ni) begin
      dmi_dpi::dmi_req_tick("dmi",
                            rst_ni,
                            i_rv_dm.dmi_req.op,
                            i_rv_dm.dmi_req.addr,
                            i_rv_dm.dmi_req.data);
    end

    if (i_rv_dm.dmi_rsp_valid && i_rv_dm.dmi_rsp_ready && rst_ni) begin
      dmi_dpi::dmi_resp_tick("dmi",
                             i_rv_dm.dmi_rsp.resp,
                             i_rv_dm.dmi_rsp.data);
    end
  end

endmodule
