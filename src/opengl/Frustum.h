/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Frustum.h
 * Author: bbyington
 *
 * Created on August 11, 2016, 6:41 PM
 */

#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "opengl/math/Matrix4.h"

#include <memory>

namespace ShapeShifter {
namespace Opengl {

struct Frustum;

namespace detail {

struct FrustumNear {
  virtual Frustum near(float n) = 0;
  virtual ~FrustumNear() {}
};
struct FrustumFar {
  virtual FrustumNear* far(float f) = 0;
  virtual ~FrustumFar() {}
};
struct FrustumFov {
  virtual FrustumFar*  fov(float f) = 0;
  virtual ~FrustumFov() {}
};
struct FrustumAspect {
  virtual FrustumFov*  aspect(float a) = 0;
  virtual ~FrustumAspect() {}
};

struct FrustumBuilder : public detail::FrustumNear, detail::FrustumFar, detail::FrustumFov, detail::FrustumAspect {
  virtual FrustumFov* aspect(float a) override;
  virtual FrustumFar* fov(float f) override;
  virtual FrustumNear* far(float f) override;
  virtual Frustum near(float near) override;

private:
  float far_;
  float fov_;
  float aspect_;
};

}

struct Frustum {
  const float near;
  const float far;
  const float fov;
  const float aspect;

  math::Matrix4 FrustTransform() const;

  static std::unique_ptr<detail::FrustumAspect> Build() {
    return std::unique_ptr<detail::FrustumAspect>(new detail::FrustumBuilder());
  }

};

}} // ShapeShifter::Opengl

#endif /* FRUSTUM_H */

