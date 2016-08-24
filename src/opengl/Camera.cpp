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
namespace Opengl {

Camera::Camera(const Frustum& frust, float look_depth)
  : frust_(frust),
    look_depth_(look_depth) {
  // TODO consider if look_depth should be rolled tino frustum to avoid
  // insane values.
}

Camera::~Camera() {
}

void Camera::ChangePitchUp(float rads) {
  rotation_ = math::Quaternion(-rads, 1, 0, 0)*rotation_;
}

void Camera::ChangeRollLeft(float rads) {
  rotation_ = math::Quaternion(-rads, 0, 0, 1)*rotation_;
}

void Camera::ChangeYawLeft(float rads) {
  rotation_ = math::Quaternion(-rads, 0, 1, 0)*rotation_;
}

void Camera::ChangePosition(const math::Vector4& trans) {
  translation_ = translation_ - trans;
}

void Camera::PivotAroundLook(const std::pair<float,float>& start, const std::pair<float,float>& end) {
  if (start == end) return;
  float proj_width = look_depth_*tan(frust_.aspect());
  // TODO make this configurable?
  // TODO this might behave oddly with tall narrow windows
  float ball_rad = proj_width;
  float ball_rad2 = ball_rad * ball_rad;

  auto convert  = [&](const std::pair<float, float>& in) {
    math::Vector3 coord;
    coord[0] = in.first*proj_width;
    coord[1] = in.second * proj_width / frust_.aspect();
    float rad2 = coord[0]*coord[0] + coord[1]*coord[1];
    float rad = sqrt(rad2);

    if (rad2 < ball_rad2/2) {
      coord[2] = sqrt(ball_rad2 - rad2);
    } else {
      coord[2] = ball_rad2/2 / rad;
    }
    return coord;
  };

  math::Vector3 first = convert(start);
  math::Vector3 second = convert(end);

  auto rot_dir = first.cross(second);
  rot_dir.Normalize();
  float dot = first.dot(second);
  float theta = acos(dot / first.Magnitude() / second.Magnitude());

  math::Quaternion q(theta, rot_dir);
  rotation_ = q * rotation_;
  translation_[2] += look_depth_;
  translation_ = q.RotationMatrix()*translation_;
  translation_[2] -= look_depth_;

}

math::Matrix4 Camera::ProjectionMatrix() const {
  math::Matrix4 rot = rotation_.RotationMatrix();
  // TODO this write column is too brittle for adding translation.  Find more
  // robust solution
  rot.WriteColumn(3, translation_);
  return frust_.FrustTransform() * rot;
}

void Camera::SetAspectRatio(float aspect) {
  //TODO validation
  frust_.SetAspect(aspect);
}


}} // ShapeShifter::Opengl
