/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Squares2D.cpp
 * Author: bbyington
 *
 * Created on August 29, 2016, 8:47 AM
 */

#include "programs/tests/Squares2D.h"

#include <cmath>
#include <memory>

namespace ShapeShifter {
namespace programs {
namespace tests {

std::unique_ptr<Opengl::World> Squares2D::Setup() {

	std::unique_ptr<Opengl::RenderNode> root(new Opengl::SquareTest2D());
  root->SetTranslation(Opengl::math::Vector4({-.5, -.5, -2.5, 1}));

  float pi = 4*std::atan(1.0f);

  std::shared_ptr<Opengl::RenderNode> second(new Opengl::SquareTest2D());
  second->SetRotation({-pi/2, 0 , 1, 0});
  root->AddChild(second);

  std::shared_ptr<Opengl::RenderNode> third(new Opengl::SquareTest2D());
  third->SetTranslation(Opengl::math::Vector4({0, 0 , -1.0, 1.0}));
  second->AddChild(third);

  std::shared_ptr<Opengl::RenderNode> fourth(new Opengl::SquareTest2D());
  fourth->SetRotation({pi/2, 1, 0, 0});
  third->AddChild(fourth);

  std::shared_ptr<Opengl::RenderNode> fifth(new Opengl::SquareTest2D());
  fifth->SetTranslation(Opengl::math::Vector4({0, 0 , -1.0, 1.0}));
  fourth->AddChild(fifth);

  std::shared_ptr<Opengl::RenderNode> sixth(new Opengl::SquareTest2D());
  sixth->SetRotation({pi/2, 0, 1, 0});
  sixth->SetTranslation(Opengl::math::Vector4({-1, 0 , 1, 1.0}));
  //fifth->AddChild(sixth);

  //TODO ownership is very weird here.  World needs a complete object.  Can
  //     provide mechanism to rotate/enable/disable nodes, but not add new
  //     ones or change point locations.
	root->UpdateData();

	std::vector<std::unique_ptr<Opengl::Shader>> shaders;
	shaders.emplace_back(
	    new Opengl::VertexShader("/Users/bbyington/ShapeShifter/shaders/vertex/BasicVertexShader.vert"));
	shaders.emplace_back(
	    new Opengl::FragmentShader("/Users/bbyington/ShapeShifter/shaders/fragment/BasicFragmentShader.frag"));
	std::unique_ptr<Opengl::ShaderProgram> program(new Opengl::ShaderProgram(std::move(shaders)));
  auto frust = Opengl::Frustum::Build()->aspect(1)->fov(.5)->far(300)->near(0.5);
  std::unique_ptr<Opengl::Camera> camera(new Opengl::Camera(frust, 2.5));
  camera->ChangePosition(Opengl::math::Vector4({0, 0, 0, 1.0f}));
  std::unique_ptr<Opengl::World> world(new Opengl::World(std::move(program), std::move(camera)));
  world->SetRenderTree(std::move(root));
  return world;
}


}}}  // ShapeShifter::programs::tests