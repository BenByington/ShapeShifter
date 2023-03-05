/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Shader.cpp
 * Author: bbyington
 *
 * Created on July 29, 2016, 8:38 AM
 */

#include "rendering/shaders/RawShader.h"

#include <cassert>
#include <cstdlib>
#include <string>
#include <map>
#include <sstream>

namespace ShapeShifter::Rendering::Shaders::detail {
std::map<std::string, size_t> ParseLayouts(const std::string& data) {
  std::map<std::string, size_t> ret;
  auto stream = std::istringstream(data);
  auto line = std::string{};
  while (std::getline(stream, line)) {
    if (line.find("layout") != std::string::npos && line.find(" in ") != std::string::npos) {
      // Issue: Create classes for writing opengl code within C++
      auto start = line.find('(');
      auto end = line.find(')');
      assert(start != std::string::npos);
      assert(end != std::string::npos);
      std::istringstream temp(line.substr(start+1, end-start-1));
      std::string trash;
      char op;
      size_t location;
      temp >> trash >> op >> location;

      start = line.find("vec3");
      end = line.find(";");
      assert(start != std::string::npos);
      assert(end != std::string::npos);
      auto name = line.substr(start+5, end-start-5);

      ret[name] = location;
    }
  }
  return ret;
}
} // ShapeShifter::Rendering::Shaders
