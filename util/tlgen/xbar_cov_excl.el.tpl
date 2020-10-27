// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0
//
// xbar_cov_excl.el generated by `tlgen.py` tool
<%
  import math
  # or all start & end addresses to get toggle bits
  def get_device_addr_toggle_bits(dev_name):
    for device in xbar.devices:
      if device.name == dev_name:
        for i in range(len(device.addr_range)):
          if i == 0:
            toggle_bits = device.addr_range[i][0]
          else:
            toggle_bits ^= device.addr_range[i][0]
        for addr in device.addr_range:
          toggle_bits |= addr[1] - addr[0]

        return toggle_bits
    log.error("Invalid dev_name: {}".format(dev_name))

  num_hosts = len(xbar.hosts)
  if num_hosts > 1:
    host_unr_source_bits = math.ceil(math.log2(num_hosts))
  else:
    host_unr_source_bits = 0
%>\

ANNOTATION: "[UNSUPPORTED]"
MODULE: prim_fifo_sync
Branch 2 "2323268504" "(!rst_ni)" (1) "(!rst_ni) 0,1,-,-"
Branch 3 "3736627057" "(!rst_ni)" (1) "(!rst_ni) 0,1,-,-"

% for i in range(2, num_hosts + 1):
ANNOTATION: "[UNR]"
MODULE: prim_arbiter_ppc ( parameter N=${i},DW=102,EnDataPort=1,EnReqStabA=0 ) 
Condition 2 "175047464" "(valid_o && ((!ready_i))) 1 -1" (2 "10")
%endfor
ANNOTATION: "[NON_RTL]"
MODULE: uvm_pkg
Assert \uvm_reg_map::do_write .unnamed$$_0.unnamed$$_1 "assertion"
Assert \uvm_reg_map::do_read .unnamed$$_0.unnamed$$_1 "assertion"

INSTANCE: tb.dut
ANNOTATION: "[UNSUPPORTED]"
Block 1 "0" "assign unused_scanmode = scanmode_i;"

ANNOTATION: "[UNSUPPORTED]"
Toggle scanmode_i

ANNOTATION: "[UNR] these device address bits are always 0"
% for device in xbar.devices:
<%
  toggle_bits = get_device_addr_toggle_bits(device.name)
%>\
  % for i in range(32):
    % if toggle_bits % 2 == 0:
Toggle tl_${device.name}_o.a_address [${i}]
    % endif
<%
    toggle_bits = toggle_bits >> 1
%>\
  % endfor
% endfor

% for i in range(host_unr_source_bits):
  % for host in xbar.hosts:
Toggle tl_${host.name}_i.a_source [${7 - i}]
Toggle tl_${host.name}_o.d_source [${7 - i}]
  % endfor
  % for device in xbar.devices:
Toggle tl_${device.name}_o.a_source [${7 - i}]
Toggle tl_${device.name}_i.d_source [${7 - i}]
  % endfor
% endfor

% for host in xbar.hosts:
Toggle tl_${host.name}_i.a_param
Toggle tl_${host.name}_o.d_param

Toggle tl_${host.name}_o.d_user
Toggle tl_${host.name}_i.a_user.rsvd1
Toggle tl_${host.name}_i.a_user.parity
Toggle tl_${host.name}_i.a_user.parity_en

Toggle tl_${host.name}_o.d_opcode [2:1]
% endfor
% for device in xbar.devices:
Toggle tl_${device.name}_o.a_param
Toggle tl_${device.name}_i.d_param

Toggle tl_${device.name}_i.d_user
Toggle tl_${device.name}_o.a_user.rsvd1
Toggle tl_${device.name}_o.a_user.parity
Toggle tl_${device.name}_o.a_user.parity_en

Toggle tl_${device.name}_i.d_opcode [2:1]
% endfor