/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   World.h
 * Author: bbyington
 *
 * Created on August 29, 2016, 8:15 AM
 */

#ifndef RENDERING_WORLD_H
#define RENDERING_WORLD_H

#include "rendering/Camera.h"
#include "rendering/shaders/ShaderProgram.h"
#include "rendering/RootNode.h"

#include <memory>

namespace ShapeShifter {
namespace Rendering {

class World final{
public:
  World(std::unique_ptr<Camera> cam);
  World(const World& orig) = delete;
  World(World&& orig) = delete;
  World& operator=(const World& other) = delete;
  ~World() {};

  void SetRenderTree(std::unique_ptr<RootNode> root);
  Camera& camera();

  void Render() const;

private:
	std::unique_ptr<RootNode> root_;
  std::unique_ptr<Camera> camera_;
};

}} // ShapeShifter::Rendering

#endif /* RENDERING_WORLD_H */

