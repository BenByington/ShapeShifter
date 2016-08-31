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

#include "scenarios/tests/Squares2D.h"

#include <cassert>
#include <cmath>
#include <memory>

namespace ShapeShifter {
namespace scenarios{
namespace tests {

std::unique_ptr<Opengl::World> Squares2D::Setup() {

	std::unique_ptr<Opengl::RenderNode> root(new detail::SquareTest2D());
  root->SetTranslation(Opengl::math::Vector4({-.5, -.5, -2.5, 1}));

  float pi = 4*std::atan(1.0f);

  std::shared_ptr<Opengl::RenderNode> second(new detail::SquareTest2D());
  second->SetRotation({-pi/2, 0 , 1, 0});
  root->AddChild(second);

  std::shared_ptr<Opengl::RenderNode> third(new detail::SquareTest2D());
  third->SetTranslation(Opengl::math::Vector4({0, 0 , -1.0, 1.0}));
  second->AddChild(third);

  std::shared_ptr<Opengl::RenderNode> fourth(new detail::SquareTest2D());
  fourth->SetRotation({pi/2, 1, 0, 0});
  third->AddChild(fourth);

  std::shared_ptr<Opengl::RenderNode> fifth(new detail::SquareTest2D());
  fifth->SetTranslation(Opengl::math::Vector4({0, 0 , -1.0, 1.0}));
  fourth->AddChild(fifth);

  std::shared_ptr<Opengl::RenderNode> sixth(new detail::SquareTest2D());
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

namespace detail {

size_t SquareTest2D::ExclusiveBufferSizeRequired() const { return 12; }

void SquareTest2D::FillColorData(std::vector<float>& data, size_t start) const {
	data[start+0] = 1.0;
	data[start+1] = 0.0;
	data[start+2] = 0.0;

	data[start+3] = 1.0;
	data[start+4] = 1.0;
	data[start+5] = 0.0;

	data[start+6] = 0.0;
	data[start+7] = 0.0;
	data[start+8] = 1.0;

	data[start+9] = 0.0;
	data[start+10] = 1.0;
	data[start+11] = 0.0;
}

void SquareTest2D::FillVertexData(std::vector<float>& data, size_t start) const {
	data[start+0] = 0;
	data[start+1] = 0;
	data[start+2] = 0;

	data[start+3] = 0;
	data[start+4] = 1;
	data[start+5] = 0;

	data[start+6]  =  1;
	data[start+7] = 0;
	data[start+8] = 0;

	data[start+9] =  1;
	data[start+10] =  1;
	data[start+11] = 0;
}

void SquareTest2D::DrawSelf() const {
	assert(start_vertex() % 3 == 0);
	assert(this->ExclusiveBufferSizeRequired() % 3 == 0);
  glDrawArrays (GL_TRIANGLE_STRIP, start_vertex()/3, this->ExclusiveBufferSizeRequired()/3);
}

}

}}}  // ShapeShifter::scenarios::tests