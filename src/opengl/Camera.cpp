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

namespace ShapeShifter {
namespace Opengl {

Camera::Camera(const Frustum& frust) : frust_(frust) {
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

math::Matrix4 Camera::ProjectionMatrix() const {
  math::Matrix4 rot = rotation_.RotationMatrix();
  // TODO this write column is too brittle for adding translation.  Find more
  // robust solution
  rot.WriteColumn(3, translation_);
  return frust_.FrustTransform() * rot;
}

}} // ShapeShifter::Opengl
