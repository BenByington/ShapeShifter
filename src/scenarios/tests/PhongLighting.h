
#ifndef SCENARIOS_TESTS_PHONG_LIGHTING_H
#define SCENARIOS_TESTS_PHONG_LIGHTING_H

#include "rendering/PureNode.h"
#include "rendering/World.h"
#include "scenarios/Scenario.h"

#include <memory>

namespace ShapeShifter {
namespace scenarios {
namespace tests {

class PhongLighting : public Scenario{
public:
  PhongLighting() {};
  PhongLighting(const PhongLighting& orig) = delete;
  PhongLighting(PhongLighting&& orign) = delete;
  PhongLighting& operator=(const PhongLighting& other) = delete;
  virtual ~PhongLighting() {};

  virtual std::unique_ptr<Rendering::World> Setup() override;

private:

};

}}} // ShapeShifter::scenarios::tests

#endif /* SCENARIOS_TESTS_PHONG_LIGHTING_H */
