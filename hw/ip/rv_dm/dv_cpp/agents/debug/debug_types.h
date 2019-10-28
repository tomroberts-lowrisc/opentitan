// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef DEBUG_TYPES_H_
#define DEBUG_TYPES_H_

#define JTAG_INSTR_W 5
#define JTAG_DATA_W 32
#define DM_ADDR_W 7
#define DM_OP_W 2

// Jtag registers (from RISC-V Debug Specification 0.13.2)
enum JtagTransType : int {
  kJtagRegBypass = 0,
  kJtagRegIDCode = 1,
  kJtagRegDTMCS  = 16,
  kJtagRegDMI    = 17,
  kJtagRegBP     = 31
};

// Debug registers (from RISC-V Debug Specification 0.13.2)
enum DebugRegAddr : int {
  kAbstractData0 = 0x4,
  kAbstractData1 = 0x5,
  kAbstractData2 = 0x6,
  kAbstractData3 = 0x7,
  kAbstractData4 = 0x8,
  kAbstractData5 = 0x9,
  kAbstractData6 = 0xA,
  kAbstractData7 = 0xB,
  kAbstractData8 = 0xC,
  kAbstractData9 = 0xD,
  kAbstractData10 = 0xE,
  kAbstractData11 = 0xF,
  kDMControl = 0x10,
  kDMStatus = 0x11,
  kHartInfo = 0x12,
  kHaltSum1 = 0x13,
  kHAWindowSel  = 0x14,
  kHAWindow     = 0x15,
  kAbstractCS   = 0x16,
  kCommand      = 0x17,
  kAbstractAuto = 0x18,
  kDevTreeAddr0 = 0x19,
  kDevTreeAddr1 = 0x1A,
  kDevTreeAddr2 = 0x1B,
  kDevTreeAddr3 = 0x1C,
  kNextDM       = 0x1D,
  kProgBuf0     = 0x20,
  kProgBuf1     = 0x21,
  kProgBuf2     = 0x22,
  kProgBuf3     = 0x23,
  kProgBuf4     = 0x24,
  kProgBuf5     = 0x25,
  kProgBuf6     = 0x26,
  kProgBuf7     = 0x27,
  kProgBuf8     = 0x28,
  kProgBuf9     = 0x29,
  kProgBuf10    = 0x2A,
  kProgBuf11    = 0x2B,
  kProgBuf12    = 0x2C,
  kProgBuf13    = 0x2D,
  kProgBuf14    = 0x2E,
  kProgBuf15    = 0x2F,
  kAuthData     = 0x30,
  kHaltSum2     = 0x34,
  kHaltSum3     = 0x35,
  kSBAddress3   = 0x37,
  kSBCS         = 0x38,
  kSBAddress0   = 0x39,
  kSBAddress1   = 0x3A,
  kSBAddress2   = 0x3B,
  kSBData0      = 0x3C,
  kSBData1      = 0x3D,
  kSBData2      = 0x3E,
  kSBData3      = 0x3F,
  kHaltSum0     = 0x40
};

enum DebugOpType : int {
  kDebugOpNOP = 0,
  kDebugOpRead = 1,
  kDebugOpWrite = 2
};

enum JtagDriverState : int {
  kJtagDriverReady = 0,
  kJtagDriverError = 1,
  kJtagDriverBusy = 2
};

#endif // DEBUG_TYPES_H_
