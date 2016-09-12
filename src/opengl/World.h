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

#ifndef WORLD_H
#define WORLD_H

#include "opengl/Camera.h"
#include "opengl/shaders/ShaderProgram.h"
#include "opengl/RenderNode.h"

#include <memory>

namespace ShapeShifter {
namespace Opengl {

class World final{
public:
  World(std::unique_ptr<Shaders::ShaderProgram> prog, std::unique_ptr<Camera> cam);
  World(const World& orig) = delete;
  World(World&& orig) = delete;
  World& operator=(const World& other) = delete;
  ~World() {};

  // TODO break this typing.  Should accept generic root.
  void SetRenderTree(std::unique_ptr<RootNode<SupportedBuffers::COLORS>> root);
  Camera& camera();

  void Render() const;

private:
	std::unique_ptr<RootNode<SupportedBuffers::COLORS>> root_;
  std::unique_ptr<Shaders::ShaderProgram> program_;
  std::unique_ptr<Camera> camera_;
};

}} // ShapeShifter::Opengl

#endif /* WORLD_H */

