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

}} // ShapeShifter::Opengl
