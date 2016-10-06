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

#ifndef CUBE_H
#define CUBE_H

#include "opengl/RenderNode.h"

namespace ShapeShifter {
namespace Shapes {

class Cube final : public Opengl::TypedRenderNode<Opengl::SupportedBufferFlags::COLORS>{
public:
  Cube(float sx, float sy, float sz);
  Cube(const Cube& orig) = delete;
  Cube(Cube&& orig) = delete;
  Cube& operator=(const Cube& orig) = delete;

  ~Cube() {}
private:

  virtual size_t ExclusiveNodeVertexCount() const override;
	virtual void FillVertexData(Opengl::VectorSlice<float>& data) const override;
	virtual void FillColorData(Opengl::VectorSlice<float>& data) const override;
  virtual void DrawSelf() const override;

  float sx_;
  float sy_;
  float sz_;
};

}} // ShapeShifter::Shapes

#endif /* CUBE_H */

