/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Sphere.h
 * Author: bbyington
 *
 * Created on August 31, 2016, 4:07 PM
 */

#ifndef SPHERE_H
#define SPHERE_H

#include "opengl/RenderNode.h"

#include <cmath>

namespace ShapeShifter {
namespace Shapes {

class Sphere final : public Opengl::TypedRenderNode<
    Opengl::SupportedBufferFlags::COLORS |
    Opengl::SupportedBufferFlags::INDICES
>{
  constexpr static const auto phi = (1.0f + std::sqrt(5.0f))/2.0f;
public:
  Sphere(float radius);
  Sphere(const Sphere& orig) = delete;
  Sphere(Sphere&& orig) = delete;
  Sphere& operator=(const Sphere& orig) = delete;

  ~Sphere() {}
private:

  virtual size_t ExclusiveNodeVertexCount() const override;
  virtual size_t ExclusiveNodeIndexCount() const override;
	virtual void FillVertexData(Opengl::Data::VectorSlice<float>& data) const override;
	virtual void FillColorData(Opengl::Data::VectorSlice<float>& data) const override;
	virtual void FillIndexData(Opengl::Data::VectorSlice<uint32_t>& data) const override;
  virtual void DrawSelf() const override;

  // FIXME: This isn't quite the radius...
  float radius_;
};

}} // ShapeShifter::Shapes

#endif /* SPHERE_H */

