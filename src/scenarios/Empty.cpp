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

namespace ShapeShifter {
namespace scenarios {

std::unique_ptr<Opengl::World> Empty::Setup() {

  // TODO factor this out.  There should be pre-set shader programs to select.
	std::vector<std::unique_ptr<Opengl::Shaders::Shader>> shaders;
	shaders.emplace_back(
	    new Opengl::Shaders::VertexShader("/Users/bbyington/ShapeShifter/shaders/vertex/BasicVertexShader.vert"));
	shaders.emplace_back(
	    new Opengl::Shaders::FragmentShader("/Users/bbyington/ShapeShifter/shaders/fragment/BasicFragmentShader.frag"));

  auto frust = Opengl::Frustum::Build()->aspect(1)->fov(.5)->far(300)->near(0.5);
  std::unique_ptr<Opengl::Camera> camera(new Opengl::Camera(frust, 2.5));
  std::unique_ptr<Opengl::World> world(new Opengl::World(std::move(camera)));
  return world;
}

}} // namespace ShapeShifter::scenarios