/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Empty.h
 * Author: bbyington
 *
 * Created on August 31, 2016, 9:38 AM
 */

#ifndef SCENARIOS_EMPTY_H
#define SCENARIOS_EMPTY_H

#include "scenarios/Scenario.h"

#include <memory>

namespace ShapeShifter::scenarios {

class Empty : public Scenario{
public:
  Empty() {};
  Empty(const Empty& orig) = delete;
  Empty(Empty&& orign) = delete;
  Empty& operator=(const Empty& other) = delete;
  virtual ~Empty() {};

  virtual std::unique_ptr<Rendering::World> Setup() override;

private:

};

} // namespace ShapeShifter::scenarios
#endif /* SCENARIOS_EMPTY_H */

