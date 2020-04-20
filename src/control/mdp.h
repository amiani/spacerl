#pragma once

#include "chipmunk.hpp"
#include "torch/torch.h"
#include <halley.hpp>
using namespace Halley;

struct EntityData {
  cp::Vect position;
  float rotation;
  cp::Vect velocity;
  float health;
  std::array<float, 6> toBlob() const;
  EntityData normalize() const;
  static const cp::Vect spaceSize;
  static const cp::Vect maxVelocity;
  static const int maxHealth = 1000;
};

struct Observation {
  bool terminal;
  EntityData self;
  //EntityData enemies[1];
  std::vector<EntityData> detectedBodies;
  //EntityData allies[1];
  torch::Tensor toTensor() const;
  static const int dim = 6*31;
};

struct Action {
  bool throttle = false;
  bool fire = false;
  cp::Vect target = cp::Vect();
  torch::Tensor logProb;
  torch::Tensor toTensor();
};


class TrajBatch {
public:
  torch::Tensor observations;
  //torch::Tensor actions;
  torch::Tensor actionLogProbs;
  torch::Tensor rewards;
  void addTrajectory(std::vector<std::shared_ptr<Observation>>&,
                    std::vector<std::shared_ptr<Action>>&,
                    std::vector<float>&);
  int getNumTrajectories() { return numTrajectories; }
private:
    int numTrajectories = 0;
    static void padAndStack(torch::Tensor& batchTensor, torch::Tensor& newTensor, int dims);
};