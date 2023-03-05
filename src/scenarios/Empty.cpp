/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Empty.cpp
 * Author: bbyington
 *
 * Created on August 31, 2016, 9:38 AM
 */

#include "Empty.h"

namespace ShapeShifter::scenarios {

std::unique_ptr<Rendering::World> Empty::Setup() {

  auto&& frust = Rendering::Frustum::Build()->aspect(1)->fov(.5)->far(300)->near(0.5);
  auto camera = std::unique_ptr<Rendering::Camera>(new Rendering::Camera(frust, 2.5));
  auto world = std::unique_ptr<Rendering::World>(new Rendering::World(std::move(camera)));
  return world;
}

} // namespace ShapeShifter::scenarios