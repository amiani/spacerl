//
// Created by amiani on 4/20/20.
//

#pragma once

#include "actor.h"
#include "action.h"

class Trainer {
public:
  Trainer(){};
  Trainer(std::shared_ptr<Actor>);
  virtual void addStep(const Observation& o, const Action& a, float r) =0;
  virtual void improve() =0;

protected:
  std::shared_ptr<Actor> actor;
  static std::vector<Tensor> getModuleParameters(jit::Module);
};
