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

  typedef Opengl::TypedRenderNode<Opengl::SupportedBufferFlags::COLORS> TypedRenderNode;
  using detail::SquareTest2D;

	auto first = std::make_shared<SquareTest2D>();
  first->SetTranslation(Opengl::math::Vector4({-.5, -.5, -2.5, 1}));

  auto pi = 4*std::atan(1.0f);

  auto second = std::make_shared<SquareTest2D>();
  second->SetRotation({-pi/2, 0 , 1, 0});
  first->AddChild(second);

  auto third = std::make_shared<SquareTest2D>();
  third->SetTranslation(Opengl::math::Vector4({0, 0 , -1.0, 1.0}));
  second->AddChild(third);

  auto fourth = std::make_shared<SquareTest2D>();
  fourth->SetRotation({pi/2, 1, 0, 0});
  third->AddChild(fourth);

  auto fifth = std::make_shared<SquareTest2D>();
  fifth->SetTranslation(Opengl::math::Vector4({0, 0 , -1.0, 1.0}));
  fourth->AddChild(fifth);

  auto sixth = std::make_shared<SquareTest2D>();
  sixth->SetRotation({pi/2, 0, 1, 0});
  sixth->SetTranslation(Opengl::math::Vector4({-1, 0 , 1, 1.0}));
  //fifth->AddChild(sixth);

	auto vert = std::make_unique<Opengl::Shaders::VertexShader>(
	    "/Users/bbyington/ShapeShifter/shaders/vertex/BasicVertexShader.vert");
	auto frag = std::make_unique<Opengl::Shaders::FragmentShader>(
	    "/Users/bbyington/ShapeShifter/shaders/fragment/BasicFragmentShader.frag");
	auto program = std::make_shared<Opengl::Shaders::ShaderProgram>(
      std::move(vert), std::move(frag));

  //TODO ownership is very weird here.  World needs a complete object.  Can
  //     provide mechanism to rotate/enable/disable nodes, but not add new
  //     ones or change point locations.
	auto root = std::make_unique<Opengl::RootNode>(first,  program);
	root->UpdateData();

  auto frust = Opengl::Frustum::Build()->aspect(1)->fov(.5)->far(300)->near(0.5);
  auto camera = std::make_unique<Opengl::Camera>(frust, 2.5);
  camera->ChangePosition(Opengl::math::Vector4({0, 0, 0, 1.0f}));

  auto world = std::make_unique<Opengl::World>(std::move(camera));
  world->SetRenderTree(std::move(root));
  return world;
}

namespace detail {

size_t SquareTest2D::ExclusiveNodeVertexCount() const { return 4; }

void SquareTest2D::FillColorData(std::vector<float>& data, size_t start_vert) const {
  size_t start_idx = start_vert * floats_per_color;
	data[start_idx+0] = 1.0;
	data[start_idx+1] = 0.0;
	data[start_idx+2] = 0.0;

	data[start_idx+3] = 1.0;
	data[start_idx+4] = 1.0;
	data[start_idx+5] = 0.0;

	data[start_idx+6] = 0.0;
	data[start_idx+7] = 0.0;
	data[start_idx+8] = 1.0;

	data[start_idx+9] = 0.0;
	data[start_idx+10] = 1.0;
	data[start_idx+11] = 0.0;
}

void SquareTest2D::FillVertexData(std::vector<float>& data, size_t start_vert) const {
  size_t start_idx = start_vert * floats_per_vert_;
	data[start_idx+0] = 0;
	data[start_idx+1] = 0;
	data[start_idx+2] = 0;

	data[start_idx+3] = 0;
	data[start_idx+4] = 1;
	data[start_idx+5] = 0;

	data[start_idx+6]  =  1;
	data[start_idx+7] = 0;
	data[start_idx+8] = 0;

	data[start_idx+9] =  1;
	data[start_idx+10] =  1;
	data[start_idx+11] = 0;
}

void SquareTest2D::DrawSelf() const {
  glDrawArrays (GL_TRIANGLE_STRIP, start_vertex(), this->ExclusiveNodeVertexCount());
}

}

}}}  // ShapeShifter::scenarios::tests