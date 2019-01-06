/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BufferTypes.h
 * Author: bbyington
 *
 * Created on September 14, 2016, 4:00 PM
 */

#ifndef DATA_ABSTRACT_BUFFERTYPES_H
#define DATA_ABSTRACT_BUFFERTYPES_H

#include "data/BufferMapBase.h"
#include "rendering/LeafNode.h"

#include <cstdlib>
#include <type_traits>

namespace ShapeShifter {
namespace Data {

/*
 * This is a base class that provides a polymorphic handle to a buffer manager.
 * A buffer manager handles a data buffer that will be uploaded to the graphics
 * card for a shader to use.  To create a new custom buffer type, inherit from
 * the CRTP BaseManager class rather than this one.
 */
class AbstractManager {
public:
  AbstractManager(const AbstractManager&) = delete;
  AbstractManager(AbstractManager&&) = delete;
  AbstractManager& operator=(const AbstractManager&) = delete;
  AbstractManager& operator=(AbstractManager&&) = delete;

  virtual ~AbstractManager(){}

  // E.g. vertices require three floats while textures only require two.
  virtual size_t ElementsPerEntry() const = 0;
  virtual bool isFloating() const = 0;
  virtual std::string name() const = 0;

protected:
  // BaseManager should be the only deriving class.
  // @param idx: the layout index as defined by the shader program
  AbstractManager() {}
};

}} // ShapeShifter::Data

#endif /* DATA_ABSTRACT_BUFFERTYPES_H */

