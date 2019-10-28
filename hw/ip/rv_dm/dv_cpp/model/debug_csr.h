// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef DEBUG_CSR_H_
#define DEBUG_CSR_H_

#include "debug_state.h"
#include "debug_transaction.h"
#include "debug_types.h"

#include <stdint.h>

/**
 * Base class for Debug CSRs
 */
class DebugCsr {

 public:

  DebugCsr(DebugRegAddr addr, DebugState *state);
  virtual ~DebugCsr() = default;

  /**
   * Check for an address match
   */
  virtual bool MatchAddr(DebugRegAddr addr);

  /**
   * Write this register with new data
   */
  virtual void WriteReg(uint32_t wdata);

  /**
   * Set this register to its reset value
   */
  virtual void ResetReg();

  /**
   * Return the current register contents
   */
  virtual uint32_t ReadReg();

  /**
   * Process an incoming transaction
   * return: success / fail
   */
  virtual bool ProcessTransaction(DebugTransaction *trans);

 protected:
  DebugState *state_;
  uint32_t reg_value_;

 private:
  DebugRegAddr reg_addr_;
};

class DMStatusReg : public DebugCsr {
  using DebugCsr::DebugCsr;
 public:
  virtual uint32_t ReadReg();
};

class DMControlReg : public DebugCsr {
  using DebugCsr::DebugCsr;
 public:
  virtual void WriteReg(uint32_t wdata);
  virtual uint32_t ReadReg();
};

class HartInfoReg : public DebugCsr {
  using DebugCsr::DebugCsr;
 public:
  virtual uint32_t ReadReg();
};

class HaltSumReg : public DebugCsr {
  using DebugCsr::DebugCsr;
 public:
  virtual void WriteReg(uint32_t wdata);
};

class AbstractCSReg : public DebugCsr {
  using DebugCsr::DebugCsr;
 public:
  virtual uint32_t ReadReg();
};

class AbstractAutoReg : public DebugCsr {
  using DebugCsr::DebugCsr;
 public:
  virtual uint32_t ReadReg();
};

class AbstractCommand : public DebugCsr {
  using DebugCsr::DebugCsr;
  virtual uint32_t ReadReg();
};

class SBCSReg : public DebugCsr {
  using DebugCsr::DebugCsr;
  virtual uint32_t ReadReg();
  virtual void WriteReg(uint32_t wdata);
};

class SBAddrReg : public DebugCsr {
  using DebugCsr::DebugCsr;
  virtual uint32_t ReadReg();
  virtual void WriteReg(uint32_t wdata);
};

class SBDataReg : public DebugCsr {
  using DebugCsr::DebugCsr;
  virtual uint32_t ReadReg();
  virtual void WriteReg(uint32_t wdata);
};

#endif // DEBUG_CSR_H_
