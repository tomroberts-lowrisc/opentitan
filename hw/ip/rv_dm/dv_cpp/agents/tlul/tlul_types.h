// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef TLUL_TYPES_H_
#define TLUL_TYPES_H_

#include <stdint.h>

enum TLULOperation : int {
  kOpPutFull = 0,
  kOpPutPartial = 1,
  kOpGet = 4
};

enum TLULRespType : int {
  kRespAck = 0,
  kRespAckData = 1
};

/**
 * Struct to describe TLUL address transactions
 */
struct TLULAddr {

 public:

  uint32_t a_address;
  uint8_t a_mask;
  TLULOperation a_opcode;
  uint32_t a_data;
  uint8_t a_size;
  uint8_t a_source;

};

/**
 * Struct to describe TLUL data transactions
 */
struct TLULData {

 public:

  TLULRespType d_opcode;
  uint32_t d_data;
  uint8_t d_size;
  uint8_t d_source;
  bool d_error;
};

struct TLULHostIntf : public TLULAddr, TLULData {
  bool a_valid;
  bool a_ready;
  bool d_valid;
  bool d_ready;
};


#endif // TLUL_TYPES_H_
