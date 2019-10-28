// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "svdpi.h"
#include "debug_environment.h"

#ifdef __cplusplus
extern "C" {
#endif

DebugEnvironment *env;

void env_initial(unsigned int *seed, char *test) {
  env = new DebugEnvironment();
  env->OnInitial(*seed,test);
}

void env_tick(svBit *stop_req) {
  env->GetStopReq(stop_req);
}

void env_final() {
  env->OnFinal();
  delete env;
}

#ifdef __cplusplus
}
#endif
