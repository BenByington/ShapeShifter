/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   IndexedBuffers.h
 * Author: bbyington
 *
 * Created on October 3, 2016, 8:22 AM
 */

// TODO update all headers...

#ifndef TESTS_INDEXED_BUFFERS_H
#define TESTS_INDEXED_BUFFERS_H

#include "opengl/RenderNode.h"
#include "opengl/World.h"
#include "scenarios/Scenario.h"

#include <memory>

namespace ShapeShifter {
namespace scenarios {
namespace tests {

class IndexBuffers : public Scenario{
public:
  IndexBuffers() {};
  IndexBuffers(const IndexBuffers& orig) = delete;
  IndexBuffers(IndexBuffers&& orign) = delete;
  IndexBuffers& operator=(const IndexBuffers& other) = delete;
  virtual ~IndexBuffers() {};

  virtual std::unique_ptr<Opengl::World> Setup() override;

private:

};

}}} // ShapeShifter::scenarios::tests

#endif /* TESTS_INDEXED_BUFFERS_H */

