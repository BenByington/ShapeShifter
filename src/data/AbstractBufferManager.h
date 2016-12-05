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

#include "data/MixedDataMapBase.h"
#include "rendering/RenderNode.h"

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

  // WARNING: Only one of these will be valid at any time.  Having them both
  // here is an interface cludge, but at this level of abstraction we can't
  // know if this buffer managed float data or integral.  Sometimes all Managers
  // are stored together and sometimes they are already separated by the data
  // type they handle, so perhaps a template parameter should be added to
  // this class forcing segregation.
  /*
   * Uses the supplied RenderNode to populate the supplied VectorSlice with data.
   * It is intended that the CRTP BaseManager be the only implementation of this
   * function, and it will handle the dispatch to make sure the correct function
   * in the RenderNode gets called.
   */
  virtual void FillData(VectorSlice<float>& data, Rendering::RenderNode* node) = 0;
  virtual void FillData(VectorSlice<uint32_t>& data, Rendering::RenderNode* node) = 0;

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

