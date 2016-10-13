/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   TypedRenderNode.cpp
 * Author: bbyington
 *
 * Created on October 13, 2016, 3:52 PM
 */

#include "rendering/TypedRenderNode.h"

namespace ShapeShifter {
namespace Rendering {

// ISSUE: Set up type list to automatically instantiate explicit templates
template class TypedRenderNode<Data::SupportedBufferFlags::COLORS>;

}} // ShapeShifter::Rendering