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

#ifndef SCENARIOS_TESTS_SQUARES2D_H
#define SCENARIOS_TESTS_SQUARES2D_H

#include "opengl/RenderNode.h"
#include "opengl/World.h"
#include "scenarios/Scenario.h"

#include <memory>

namespace ShapeShifter {
namespace scenarios {
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

namespace detail {

/**
 * Simple test class that will draw a shaded square in the center of the screen
 */
class SquareTest2D : public Opengl::TypedRenderNode<Opengl::Data::SupportedBufferFlags::COLORS> {
public:
	SquareTest2D() = default;
	virtual ~SquareTest2D() {}
private:
  using BufferIndex = Opengl::Data::BufferIndex;
	virtual BufferIndex ExclusiveNodeDataCount() const override;
	virtual void FillVertexData(Opengl::Data::VectorSlice<float>& data) const override;
	virtual void FillColorData(Opengl::Data::VectorSlice<float>& data) const override;
	virtual void DrawSelf() const override;
};

}

}}} // ShapeShifter::scenarios::tests

#endif /* SCENARIOS_TESTS_SQUARES2D_H */

