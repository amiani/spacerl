#pragma once

#include "src/control/observation.h"
#include "torch/script.h"
#include "src/control/distributions/normal.h"
#include "src/utils.h"
#include "action.h"

class Actor {
public:
  Actor() {};
  virtual Action act(const Observation& o) =0;
};