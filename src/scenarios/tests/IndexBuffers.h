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

#ifndef SCENARIOS_TESTS_INDEXED_BUFFERS_H
#define SCENARIOS_TESTS_INDEXED_BUFFERS_H

#include "rendering/RenderNode.h"
#include "rendering/World.h"
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

  virtual std::unique_ptr<Rendering::World> Setup() override;

private:

};

}}} // ShapeShifter::scenarios::tests

#endif /* SCENARIOS_TESTS_INDEXED_BUFFERS_H */

