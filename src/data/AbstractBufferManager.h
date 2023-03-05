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

#include "data/VectorSlice.h"
#include "rendering/shaders/InterfaceVariable.h"

#include <cstdlib>
#include <type_traits>

namespace ShapeShifter {
namespace Data {

/*
 * This is a base class that provides a polymorphic handle to a buffer
 * manager.  A buffer manager handles a data buffer that will be
 * uploaded to the graphics card for a shader to use.  To create a new
 * custom buffer type, one must both inherit from this class, as well
 * as implement the BufferManager concept defined below inherit from
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

// Concept for the BufferManager type.  This enforces all the
// constraints that cannot be sensibly inserted into the virtual
// AbstractManager interface
template <typename Child>
concept BufferManager = requires(Child c) {
    requires std::derived_from<Child, AbstractManager>;

    // Implementation must work with floats or unsigned ints
    typename Child::Type;
    requires std::same_as<typename Child::Type, float>
          || std::same_as<typename Child::Type, uint32_t>;

    // any buffer manager defined must supply an inner class named
    // Interface that has a FillData function.  It is assumed but not
    // enforced that this FillData function will in turn dispatch to a
    // uniquely named virtual function that RenderNode children
    // classes will implement.  e.g. A RenderNode that supports the
    // VertexManager will extend the VertexManager::Interface class
    // and be forced to implement the virtual FillVertexData()
    // function.
    typename Child::Interface;
    requires requires (typename Child::Interface& i,
                       VectorSlice<typename Child::Type>& v) {
        { i.FillData(v) } -> std::same_as<void>;
    };

    typename Child::Variable;
    requires Rendering::Shaders::InterfaceVariable<typename Child::Variable>;
};

}} // ShapeShifter::Data

#endif /* DATA_ABSTRACT_BUFFERTYPES_H */

