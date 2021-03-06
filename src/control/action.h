//
// Created by amiani on 4/23/20.
//

#pragma once
#include <halley.hpp>
#include "chipmunk.hpp"
#include "torch/torch.h"

struct MixedAction {
  bool throttle = false;
  bool fire = false;
  cp::Vect target = cp::Vect();
  torch::Tensor tensor;
  torch::Tensor logProb;
};

enum Direction {
  LEFT,
  RIGHT,
  STRAIGHT
};

struct Action {
  bool throttle = false;
  bool fire = false;
  Direction direction = STRAIGHT;
  torch::Tensor tensor;
  inline static const int dim = 6;
};
