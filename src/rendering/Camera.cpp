/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Camera.cpp
 * Author: bbyington
 *
 * Created on August 11, 2016, 6:37 PM
 */

#include "Camera.h"

#include "math/Vector3.h"

#include <cmath>
#include <iostream>

namespace ShapeShifter {
namespace Rendering {

Camera::Camera(const Frustum& frust, float look_depth)
  : frust_(frust),
    look_depth_(look_depth) {
}

Camera::~Camera() {
}

void Camera::ChangePitchUp(float rads) {
  rotation_ = Math::Quaternion(-rads, 1, 0, 0)*rotation_;
}

void Camera::ChangeRollLeft(float rads) {
  rotation_ = Math::Quaternion(-rads, 0, 0, 1)*rotation_;
}

void Camera::ChangeYawLeft(float rads) {
  rotation_ = Math::Quaternion(-rads, 0, 1, 0)*rotation_;
}

void Camera::ChangePosition(const Math::Vector4& trans) {
  translation_ = translation_ - trans;
}

void Camera::PivotAroundLook(const std::pair<float,float>& start, const std::pair<float,float>& end) {
  if (start == end) return;
  auto proj_width = look_depth_*tan(frust_.aspect());
  // ISSUE Add camera controls so this can be configurable.  Currently things
  // are a bit odd when there is a very large or very small aspect ratio
  auto ball_rad = proj_width;
  auto ball_rad2 = ball_rad * ball_rad;

  auto convert  = [&](const std::pair<float, float>& in) {
    auto coord = Math::Vector3 {};
    coord[0] = in.first*proj_width;
    coord[1] = in.second * proj_width / frust_.aspect();
    auto rad2 = coord[0]*coord[0] + coord[1]*coord[1];
    auto rad = sqrt(rad2);

    if (rad2 < ball_rad2/2) {
      coord[2] = sqrt(ball_rad2 - rad2);
    } else {
      coord[2] = ball_rad2/2 / rad;
    }
    return coord;
  };

  auto first = convert(start);
  auto second = convert(end);

  auto rot_dir = first.cross(second);
  rot_dir.Normalize();
  auto dot = first.dot(second);
  auto theta = acos(dot / first.Magnitude() / second.Magnitude());

  auto q = Math::Quaternion(theta, rot_dir);
  rotation_ = q * rotation_;
  translation_[2] += look_depth_;
  translation_ = q.RotationMatrix()*translation_;
  translation_[2] -= look_depth_;

}

Math::Matrix4 Camera::ProjectionMatrix() const {
  auto rot = rotation_.RotationMatrix();
  rot.WriteColumn(3, translation_);
  return frust_.FrustTransform() * rot;
}

void Camera::SetAspectRatio(float aspect) {
  frust_.SetAspect(aspect);
}


}} // ShapeShifter::Rendering
