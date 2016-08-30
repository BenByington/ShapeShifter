/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Squares2D.h
 * Author: bbyington
 *
 * Created on August 29, 2016, 8:47 AM
 */

#ifndef SQUARES2D_H
#define SQUARES2D_H

#include "opengl/World.h"
#include "programs/Scenario.h"

#include <memory>

namespace ShapeShifter {
namespace programs {
namespace tests {

class Squares2D : public Scenario{
public:
  Squares2D() {};
  Squares2D(const Squares2D& orig) = delete;
  Squares2D(Squares2D&& orign) = delete;
  Squares2D& operator=(const Squares2D& other) = delete;
  virtual ~Squares2D() {};

  virtual std::unique_ptr<Opengl::World> Setup() override;

private:

};

}}} // ShapeShifter::programs::tests

#endif /* SQUARES2D_H */

