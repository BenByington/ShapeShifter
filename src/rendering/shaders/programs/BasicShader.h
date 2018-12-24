/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   BasicShader.h
 * Author: bbyington
 *
 * Created on November 1, 2016, 4:26 PM
 */

#ifndef RENDERING_SHADERS_PROGRAMS_BASIC_SHADER_H
#define RENDERING_SHADERS_PROGRAMS_BASIC_SHADER_H

#include "data/ConcreteBufferManager.h"
#include "rendering/shaders/Shader.h"
#include "rendering/shaders/language/GLSLGeneratorBase.h"
#include "math/Quaternion.h"
#include "rendering/Camera.h"

// TODO don't really like this here
#include "rendering/PureNode.h"

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {
namespace Programs {

using Data::ColorManager;
using Data::VertexManager;

namespace detail {
struct ColorPass : InterfaceVariableBase<ColorPass, Language::Vec3> {
  ColorPass() = delete;

  using Base = InterfaceVariableBase<ColorPass, Language::Vec3>;
  using Base::InterfaceVariableBase;
  static constexpr const char* name() {
    return "theColor";
  }
  static constexpr bool smooth = true;
  Variable_T& theColor = Base::var;
};

struct OutputColor : InterfaceVariableBase<OutputColor, Language::Vec4> {
  OutputColor() = delete;

  using Base = InterfaceVariableBase<OutputColor, Language::Vec4>;
  using Base::InterfaceVariableBase;
  static constexpr const char* name() {
    return "outputColor";
  }
  static constexpr bool smooth = false;
  Variable_T& outputColor = Base::var;
};

// ISSUE: There needs to be some sort of constraint ensuring the second
//        template parameter here is consistent with the StorageType
//        defined in the inner class.
struct Transform : UniformVariableBase<Transform, Language::Mat4> {
  Transform() = delete;
  using Base = UniformVariableBase<Transform, Language::Mat4>;
  using Base::UniformVariableBase;
  static constexpr const char* name() {
    return "transform";
  }
  static constexpr bool smooth = false;
  Variable_T& transform = Base::var;

  class UniformManager {
  public:
    using StorageType = Math::Matrix4;
    UniformManager() : translation_(0,0,0,1) {}

    void SetRotation(const Math::Quaternion& rot) {
      rotation_ = rot;
    }
    void SetTranslation(const Math::Vector4& trans) {
      translation_ = trans;
    }

    Math::Matrix4 Data(const Camera& camera) const {
      auto mat = rotation_.RotationMatrix();
      mat.WriteColumn(3, translation_);
      return camera.ProjectionMatrix() * mat;
    }

    void Combine(const UniformManager& other) {
      SetTranslation(translation_ + rotation_.RotationMatrix() * other.translation_);
      SetRotation(rotation_*other.rotation_);
    }

    void CombineInverse(const UniformManager& other) {
      SetRotation(rotation_*other.rotation_.Inverse());
      SetTranslation(translation_ - rotation_.RotationMatrix() * other.translation_);
    }

    void Clone(const UniformManager& other) {
      rotation_ = other.rotation_;
      translation_ = other.translation_;
    }

  private:
    Math::Quaternion rotation_;
    Math::Vector4 translation_;
  };

  class UniformInitializer
  {
    public:
    UniformManager InitUniform() const {
      UniformManager ret{};
      for (auto m : path)
      {
        ret.CombineInverse(*m);
      }
      return ret;
    }

    template <typename T1, typename T2>
    void SetOriginNode(const CallableReferenceWrapper<T1, T2>& node)
    {
      const T2& ref = node.template Convert<T2>();
      const T2* ptr = &ref;
      while (ptr != nullptr) {
        auto * manager = dynamic_cast<const UniformManager*>(ptr);
        // TODO better error handling
        // TODO fix this ugliness with RootNode inheriting from stripped down
        // PureNode
        if (manager == nullptr) {
          assert(ptr->Parent() == nullptr);
          return;
        }
        path.push_back(manager);
        ptr = ptr->Parent();
      }
    }

    private:
      std::vector<const UniformManager*> path;
  };
};


}

class BasicVertexShader : public Language::GLSLVertexGeneratorBase<
    Pack<ColorManager, VertexManager>,
    Pack<detail::Transform>,
    Pack<detail::ColorPass>> {
  using Base = Language::GLSLVertexGeneratorBase<
      Pack<ColorManager, VertexManager>,
      Pack<detail::Transform>,
      Pack<detail::ColorPass>>;
public:
  BasicVertexShader(VariableFactory&& factory) : Base(std::move(factory)) {}
private:
  void DefineMain(const VariableFactory& factory) override;
};

class BasicFragmentShader : public Language::GLSLFragmentGeneratorBase<
    Pack<detail::ColorPass>,
    Pack<>,
    Pack<detail::OutputColor>> {
  using Base = Language::GLSLFragmentGeneratorBase<
      Pack<detail::ColorPass>,
      Pack<>,
      Pack<detail::OutputColor>>;
public:
  BasicFragmentShader(VariableFactory&& factory) : Base(std::move(factory)) {}
private:
  void DefineMain(const VariableFactory& factory) override;
};

}}}} // ShapeShifter::Rendering::Shaders::Programs

#endif /* RENDERING_SHADERS_PROGRAMS_BASIC_SHADER_H */

