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

namespace ShapeShifter::Rendering {

World::World(std::unique_ptr<Camera> cam)
  : camera_(std::move(cam))
{}

void World::SetRenderTree(std::unique_ptr<RenderingTree> tree) {
  tree_ = std::move(tree);
}

Camera& World::camera() {
  return *camera_;
}

void World::Render() const {
  if (tree_) {
    tree_->Render(*camera_);
  }
}

} // ShapeShifter::Rendering
