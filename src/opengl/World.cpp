/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   World.cpp
 * Author: bbyington
 *
 * Created on August 29, 2016, 8:15 AM
 */

#include "World.h"

namespace ShapeShifter {
namespace Opengl {

World::World(std::unique_ptr<ShaderProgram> prog, std::unique_ptr<Camera> cam)
  : program_(std::move(prog))
  , camera_(std::move(cam))
{}

void World::SetRenderTree(std::unique_ptr<RenderNode> root) {
  root_ = std::move(root);
}

Camera& World::camera() {
  return *camera_;
}

void World::Render() const {
  root_->RenderTree(*camera_, *program_);
}

}} // ShapeShifter::Opengl
