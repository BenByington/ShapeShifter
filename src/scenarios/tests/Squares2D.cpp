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

#include "rendering/shaders/ShaderProgram.h"

#include <cassert>
#include <cmath>
#include <memory>

namespace ShapeShifter {
namespace scenarios{
namespace tests {

std::unique_ptr<Rendering::World> Squares2D::Setup() {

  using detail::SquareTest2D;
  using Math::Vector4;

  auto pi = 4*std::atan(1.0f);

  auto sixth = std::make_unique<SquareTest2D>();

  // Could set the rotation/translation of sixth before adding it to fifth, but
  // this way we make sure that when adding a child, we get back a pointer
  // that allows us to manipulate the node (but not add/remove points or entire
  // new nodes)
  auto fifth = std::make_unique<SquareTest2D>();
  auto manipulator = fifth->AddChild(std::move(sixth));
  manipulator->SetRotation({pi/2, 0, 1, 0});
  manipulator->SetTranslation(Vector4(-1, 0 , 1, 1.0));

  auto fourth = std::make_unique<SquareTest2D>();
  manipulator = fourth->AddChild(std::move(fifth));
  manipulator->SetTranslation(Vector4(0, 0 , -1.0, 1.0));

  auto third = std::make_unique<SquareTest2D>();
  manipulator = third->AddChild(std::move(fourth));
  manipulator->SetRotation({pi/2, 1, 0, 0});

  auto second = std::make_unique<SquareTest2D>();
  manipulator = second->AddChild(std::move(third));
  manipulator->SetTranslation(Vector4(0, 0 , -1.0, 1.0));

	auto first = std::make_unique<SquareTest2D>();
  manipulator = first->AddChild(std::move(second));
  manipulator->SetRotation({-pi/2, 0 , 1, 0});

	auto vert = std::make_unique<Rendering::Shaders::VertexShader>(
	    "/Users/bbyington/ShapeShifter/shaders/vertex/BasicVertexShader.vert");
	auto frag = std::make_unique<Rendering::Shaders::FragmentShader>(
	    "/Users/bbyington/ShapeShifter/shaders/fragment/BasicFragmentShader.frag");
  // TODO find more succinct way to express ShaderProgram type
  using ShaderProgram = Rendering::Shaders::ShaderProgram<Data::ColorManager, Data::VertexManager>;
	auto program = std::make_shared<ShaderProgram>(
      std::move(vert), std::move(frag));

	auto root = std::make_unique<Rendering::RootNode>(std::move(first),  program);
  root->SetTranslation(Vector4(-.5, -.5, -2.5, 1));

  auto frust = Rendering::Frustum::Build()->aspect(1)->fov(.5)->far(300)->near(0.5);
  auto camera = std::make_unique<Rendering::Camera>(frust, 2.5);
  camera->ChangePosition(Vector4(0, 0, 0, 1.0f));

  auto world = std::make_unique<Rendering::World>(std::move(camera));
  world->SetRenderTree(std::move(root));
  return world;
}

namespace detail {

using Data::BufferIndex;
BufferIndex SquareTest2D::ExclusiveNodeDataCount() const {
  auto ret = BufferIndex();
  ret.vertex_ = 4;
  return ret;
}

void SquareTest2D::FillColorData(Data::VectorSlice<float>& data) const {
	data[0] = 1.0;
	data[1] = 0.0;
	data[2] = 0.0;

	data[3] = 1.0;
	data[4] = 1.0;
	data[5] = 0.0;

	data[6] = 0.0;
	data[7] = 0.0;
	data[8] = 1.0;

	data[9] = 0.0;
	data[10] = 1.0;
	data[11] = 0.0;
}

void SquareTest2D::FillVertexData(Data::VectorSlice<float>& data) const {
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;

	data[3] = 0;
	data[4] = 1;
	data[5] = 0;

	data[6]  =  1;
	data[7] = 0;
	data[8] = 0;

	data[9] =  1;
	data[10] =  1;
	data[11] = 0;
}

void SquareTest2D::DrawSelf() const {
  glDrawArrays (
      GL_TRIANGLE_STRIP,
      start().vertex_,
      ExclusiveNodeDataCount().vertex_
  );
}

}

}}}  // ShapeShifter::scenarios::tests