/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   IndexBuffers.cpp
 * Author: bbyington
 *
 * Created on August 29, 2016, 8:47 AM
 */

#include "scenarios/tests/IndexBuffers.h"

#include "math/Quaternion.h"
#include "rendering/PureNode.h"
#include "rendering/shaders/ShaderProgram.h"
#include "rendering/shaders/programs/BasicShader.h"
#include "shapes/Cube.h"
#include "shapes/Sphere.h"

#include <cassert>
#include <cmath>
#include <memory>

namespace ShapeShifter {
namespace scenarios{
namespace tests {

std::unique_ptr<Rendering::World> IndexBuffers::Setup() {

  using Math::Quaternion;
  using Math::Vector4;

  auto cube = std::make_unique<Shapes::Cube>(.5f, .7f, .85f);
  cube->SetRotation(Quaternion(.5, 1, 1, 1));
  cube->SetTranslation(Vector4(.7, .2, -.4, 1));

  auto sphere = std::make_unique<Shapes::Sphere>(0.2);

  auto pure = std::make_unique<Rendering::PureNode<
      Data::VertexManager,
      Data::ColorManager
  >>();
  pure->AddChild(std::move(sphere));
  pure->AddChild(std::move(cube));

  // TODO make this more succinct
  using namespace Rendering::Shaders;
  using namespace Rendering::Shaders::Programs;
  auto vert = std::make_unique<VertexShader<BasicVertexShader>>();
  auto frag = std::make_unique<FragmentShader<BasicFragmentShader>>();
  using ShaderProgram = Rendering::Shaders::ShaderProgram<
      Data::VertexManager,
      Data::ColorManager>;
	auto program = std::make_shared<ShaderProgram>(
      std::move(vert), std::move(frag));

	auto root = std::make_unique<Rendering::RootNode>(std::move(pure),  program);
  root->SetTranslation(Vector4(-.5, -.5, -2.5, 1));

  auto frust = Rendering::Frustum::Build()->aspect(1)->fov(.5)->far(300)->near(0.5);
  auto camera = std::make_unique<Rendering::Camera>(frust, 2.5);
  camera->ChangePosition(Vector4(0, 0, 0, 1.0f));

  auto world = std::make_unique<Rendering::World>(std::move(camera));
  world->SetRenderTree(std::move(root));
  return world;
}

}}}  // ShapeShifter::scenarios::tests