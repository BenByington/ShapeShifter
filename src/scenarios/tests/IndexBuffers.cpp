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

#include <cassert>
#include <cmath>
#include <memory>

namespace ShapeShifter {
namespace scenarios{
namespace tests {

std::unique_ptr<Opengl::World> IndexBuffers::Setup() {

  using detail::Dummy;

  auto pi = 4*std::atan(1.0f);

  auto sixth = std::make_unique<Dummy>();

  // Could set the rotation/translation of sixth before adding it to fifth, but
  // this way we make sure that when adding a child, we get back a pointer
  // that allows us to manipulate the node (but not add/remove points or entire
  // new nodes)
  auto fifth = std::make_unique<Dummy>();
  auto manipulator = fifth->AddChild(std::move(sixth));
  manipulator->SetRotation({pi/2, 0, 1, 0});
  manipulator->SetTranslation(Opengl::math::Vector4({-1, 0 , 1, 1.0}));

  auto fourth = std::make_unique<Dummy>();
  manipulator = fourth->AddChild(std::move(fifth));
  manipulator->SetTranslation(Opengl::math::Vector4({0, 0 , -1.0, 1.0}));

  auto third = std::make_unique<Dummy>();
  manipulator = third->AddChild(std::move(fourth));
  manipulator->SetRotation({pi/2, 1, 0, 0});

  auto second = std::make_unique<Dummy>();
  manipulator = second->AddChild(std::move(third));
  manipulator->SetTranslation(Opengl::math::Vector4({0, 0 , -1.0, 1.0}));

	auto first = std::make_unique<Dummy>();
  manipulator = first->AddChild(std::move(second));
  manipulator->SetRotation({-pi/2, 0 , 1, 0});

	auto vert = std::make_unique<Opengl::Shaders::VertexShader>(
	    "/Users/bbyington/ShapeShifter/shaders/vertex/BasicVertexShader.vert");
	auto frag = std::make_unique<Opengl::Shaders::FragmentShader>(
	    "/Users/bbyington/ShapeShifter/shaders/fragment/BasicFragmentShader.frag");
	auto program = std::make_shared<Opengl::Shaders::ShaderProgram>(
      std::move(vert), std::move(frag));

	auto root = std::make_unique<Opengl::RootNode>(std::move(first),  program);
  root->SetTranslation(Opengl::math::Vector4({-.5, -.5, -2.5, 1}));

  auto frust = Opengl::Frustum::Build()->aspect(1)->fov(.5)->far(300)->near(0.5);
  auto camera = std::make_unique<Opengl::Camera>(frust, 2.5);
  camera->ChangePosition(Opengl::math::Vector4({0, 0, 0, 1.0f}));

  auto world = std::make_unique<Opengl::World>(std::move(camera));
  world->SetRenderTree(std::move(root));
  return world;
}

namespace detail {

size_t Dummy::ExclusiveNodeVertexCount() const { return 4; }

void Dummy::FillColorData(std::vector<float>& data, size_t start_vert) const {
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

void Dummy::FillVertexData(std::vector<float>& data, size_t start_vert) const {
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

void Dummy::DrawSelf() const {
  glDrawArrays (GL_TRIANGLE_STRIP, start_vertex(), this->ExclusiveNodeVertexCount());
}

}

}}}  // ShapeShifter::scenarios::tests