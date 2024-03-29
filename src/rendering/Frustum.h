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

#ifndef RENDERING_FRUSTUM_H
#define RENDERING_FRUSTUM_H

#include "math/Matrix4.h"

#include <memory>

namespace ShapeShifter::Rendering {

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
  virtual FrustumFar* fov(float f) = 0;
  virtual ~FrustumFov() {}
};
struct FrustumAspect {
  virtual FrustumFov* aspect(float a) = 0;
  virtual ~FrustumAspect() {}
};

struct FrustumBuilder : public detail::FrustumNear,
                        detail::FrustumFar,
                        detail::FrustumFov,
                        detail::FrustumAspect {
  virtual FrustumFov* aspect(float a) override;
  virtual FrustumFar* fov(float f) override;
  virtual FrustumNear* far(float f) override;
  virtual Frustum near(float near) override;

private:
  float far_;
  float fov_;
  float aspect_;
};

} // namespace detail

struct Frustum {
  Math::Matrix4 FrustTransform() const;

  static std::unique_ptr<detail::FrustumAspect> Build() {
    return std::unique_ptr<detail::FrustumAspect>(new detail::FrustumBuilder());
  }

  void SetAspect(const float aspect);
  void SetFov(const float fov);
  void SetFar(const float far);
  void SetNear(const float near);

  float aspect() const { return aspect_; }
  float fov() const { return fov_; }
  float far() const { return far_; }
  float near() const { return near_; }

private:
  friend class detail::FrustumBuilder;
  Frustum(float near, float far, float fov, float aspect);

  float near_;
  float far_;
  float fov_;
  float aspect_;
};

} // namespace ShapeShifter::Rendering

#endif /* RENDERING_FRUSTUM_H */
