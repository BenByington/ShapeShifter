/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Scenario.h
 * Author: bbyington
 *
 * Created on August 29, 2016, 8:39 AM
 */

#ifndef SCENARIOS_SCENARIO_H
#define SCENARIOS_SCENARIO_H

#include "opengl/World.h"

#include <memory>

namespace ShapeShifter {
namespace scenarios {

class Scenario {
public:
  Scenario() {}
  Scenario(const Scenario& orig) = delete;
  Scenario(Scenario&& orig) = delete;
  Scenario& operator=(const Scenario& orig) = delete;
  virtual ~Scenario() {};

  virtual std::unique_ptr<Opengl::World> Setup() = 0;
private:

};

}} //namespace ShapeShifter::scenarios
#endif /* SCENARIOS_SCENARIO_H */

