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

#include "data/ConcreteBufferManager.h"
#include "rendering/PureNode.h"
#include "rendering/World.h"
#include "scenarios/Scenario.h"

#include <memory>

namespace ShapeShifter::scenarios::tests {

class Squares2D : public Scenario {
public:
  Squares2D(){};
  Squares2D(const Squares2D& orig) = delete;
  Squares2D(Squares2D&& orign) = delete;
  Squares2D& operator=(const Squares2D& other) = delete;
  virtual ~Squares2D(){};

  virtual std::unique_ptr<Rendering::World> Setup() override;

private:
};

namespace detail {

/**
 * Simple test class that will draw a shaded square in the center of the screen
 */
class SquareTest2D : public Rendering::LeafNode<Data::VertexManager, Data::ColorManager> {
public:
  SquareTest2D() = default;
  virtual ~SquareTest2D() {}

private:
  using BufferIndex = Data::BufferIndex;
  virtual BufferIndex ExclusiveNodeDataCount() const override;
  virtual void FillVertexData(Data::VectorSlice<float>& data) const override;
  virtual void FillColorData(Data::VectorSlice<float>& data) const override;
  virtual void FillIndexData(Data::VectorSlice<uint32_t>&) const override {}
  virtual void DrawSelf() const override;
};

} // namespace detail

} // namespace ShapeShifter::scenarios::tests

#endif /* SCENARIOS_TESTS_SQUARES2D_H */
