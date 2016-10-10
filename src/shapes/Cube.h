/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Cube.h
 * Author: bbyington
 *
 * Created on August 31, 2016, 4:07 PM
 */

#ifndef SHAPES_CUBE_H
#define SHAPES_CUBE_H

#include "opengl/RenderNode.h"

namespace ShapeShifter {
namespace Shapes {

class Cube final : public Opengl::TypedRenderNode<
    Opengl::Data::SupportedBufferFlags::COLORS |
    Opengl::Data::SupportedBufferFlags::INDICES
>{

  using BufferIndex = Opengl::Data::BufferIndex;
public:
  Cube(float sx, float sy, float sz);
  Cube(const Cube& orig) = delete;
  Cube(Cube&& orig) = delete;
  Cube& operator=(const Cube& orig) = delete;

  ~Cube() {}
private:

  virtual BufferIndex ExclusiveNodeDataCount() const override;
	virtual void FillVertexData(Opengl::Data::VectorSlice<float>& data) const override;
	virtual void FillColorData(Opengl::Data::VectorSlice<float>& data) const override;
	virtual void FillIndexData(Opengl::Data::VectorSlice<uint32_t>& data) const override;
  virtual void DrawSelf() const override;

  float sx_;
  float sy_;
  float sz_;
};

}} // ShapeShifter::Shapes

 #endif /*SHAPES_CUBE_H */

