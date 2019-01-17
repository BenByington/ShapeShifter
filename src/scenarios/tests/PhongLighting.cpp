
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   PhongLighting.cpp
 * Author: bbyington
 *
 * Created on August 29, 2016, 8:47 AM
 */

#include "scenarios/tests/PhongLighting.h"

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

// TODO clean up tests.  This is an unecessary replication, when all we want to do is swap out the shader program
std::unique_ptr<Rendering::World> PhongLighting::Setup() {

  using Math::Quaternion;
  using Math::Vector4;

  using namespace Rendering::Shaders;
  using namespace Rendering::Shaders::Programs;
  auto program = CreateShaderProgram<PhongVertexShader, PhongFragmentShader>();
  auto node1 = Rendering::CompatiblePureNode(*program);

  auto cube = node1->AddLeaf<Shapes::Cube>(.5f, .7f, .85f);
  cube->SetRotation(Quaternion(.5, 1, 1, 1));
  cube->SetTranslation(Vector4(.7, .6, -1.0f, 1));

  auto sphere = node1->AddLeaf<Shapes::Sphere>(0.2);

  auto node2 = Rendering::CompatiblePureNode(*program);
  auto manipulator = node2->AddChild(std::move(node1));
  manipulator->SetTranslation(Vector4(-.5, -.5, -2.5, 1));

  // TODO make separate test?  Add similar test to Squares2D?
  // Tests for setting the camera origin node.  Removing these extra
  // nodes along with removing the SetOriginNode call should not affect
  // the image

  // Undo single rotation
  auto dummy1 = Rendering::CompatiblePureNode(*program);
  manipulator = dummy1->AddChild(std::move(node2));
  manipulator->SetRotation(Quaternion(.9, 1, 3, 2));

  // Undo single translation
  auto dummy2 = Rendering::CompatiblePureNode(*program);
  dummy1->SetTranslation(Vector4(12, 93, 8, 1));
  dummy2->AddChild(std::move(dummy1));

  // Undo combined rot and trans
  auto dummy3 = Rendering::CompatiblePureNode(*program);
  dummy2->SetTranslation(Vector4(-1, 23, -5, 1));
  dummy2->SetRotation(Quaternion(3.1, 0, .3, 2));
  dummy3->AddChild(std::move(dummy2));

  // Undo combined rot and trans again, to help flush out cumulative issues
  auto dummy4 = Rendering::CompatiblePureNode(*program);
  dummy3->SetTranslation(Vector4(99, 2, 32, 1));
  dummy3->SetRotation(Quaternion(2.1, 5, .3, -2));
  dummy4->AddChild(std::move(dummy3));

  auto root = Rendering::CreateRootPtr(std::move(dummy4));
  root->SetOriginNode(manipulator);
  root->SetAmbientLight(0.15f);
  root->SetLightColor({.90f, .90f, .90f});
  root->SetLightNode(sphere);
  root->SetLightRelPos(Vector4(1,1,1,1));

  auto frust = Rendering::Frustum::Build()->aspect(1)->fov(.5)->far(300)->near(0.5);
  auto camera = std::make_unique<Rendering::Camera>(frust, 2.5);
  camera->ChangePosition(Vector4(0, 0, 0, 1.0f));

  auto tree = std::make_unique<Rendering::RenderingTree>(root, program);

  auto world = std::make_unique<Rendering::World>(std::move(camera));
  world->SetRenderTree(std::move(tree));
  return world;
}

}}}  // ShapeShifter::scenarios::tests
