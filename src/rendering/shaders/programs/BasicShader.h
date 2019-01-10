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
#include "util/MultiReferenceWrapper.h"

namespace ShapeShifter {
namespace Rendering {
namespace Shaders {
namespace Programs {

using Data::ColorManager;
using Data::VertexManager;
using Data::NormalManager;

// TODO organize this so it scales
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

struct NormalPass : InterfaceVariableBase<NormalPass, Language::Vec3> {
  NormalPass() = delete;

  using Base = InterfaceVariableBase<NormalPass, Language::Vec3>;
  using Base::InterfaceVariableBase;
  static constexpr const char* name() {
    return "theNormal";
  }
  static constexpr bool smooth = true;
  Variable_T& theNormal = Base::var;
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
      for (auto m : path_)
      {
        ret.CombineInverse(*m);
      }
      return ret;
    }

    template <typename T1, typename T2>
    void SetOriginNode(const Util::MultiReferenceWrapper<T1, T2>& node)
    {
      static_assert(std::is_base_of<UniformManager, T2>::value,
                    "SetOriginNode called node without Transform Uniform");

      const auto& path = node.template Convert<T2>()->PathToRoot();
      path_ = std::vector<const UniformManager*>(path.begin(), path.end());
    }

    private:
      std::vector<const UniformManager*> path_;
  };
};

struct AmbientLight : UniformVariableBase<AmbientLight, Language::Float> {

  AmbientLight() = delete;
  using Base = UniformVariableBase<AmbientLight, Language::Float>;
  using Base::UniformVariableBase;
  static constexpr const char* name() {
    return "ambientLight";
  }
  static constexpr bool smooth = false;
  Variable_T& ambientLight = Base::var;

  struct UniformManager {
    UniformManager(float v) : val_{v} {}
    UniformManager() : val_{1.0f} {}
    using StorageType = float;
    float Data(const Camera&) const { return val_; }
    void Combine(const UniformManager& o) {/*do nothing*/}
    void Clone(const UniformManager& o) { val_ = o.val_; }
  private:
    float val_ = 1.0f;
  };
  struct UniformInitializer {
    UniformManager InitUniform() const { return UniformManager(val_); }
    void SetAmbientLight(float v) { val_ = v; }
  private:
    float val_ = 1.0f;
  };
};

struct LightColor : UniformVariableBase<LightColor, Language::Vec3> {

  LightColor() = delete;
  using Base = UniformVariableBase<LightColor, Language::Vec3>;
  using Base::UniformVariableBase;
  static constexpr const char* name() {
    return "lightColor";
  }
  static constexpr bool smooth = false;
  Variable_T& lightColor = Base::var;

  struct UniformManager {
    UniformManager(const Math::Vector3& v) : val_{v} {}
    UniformManager() : val_{1.0f, 1.0f, 1.0f} {}
    using StorageType = Math::Vector3;
    Math::Vector3 Data(const Camera&) const { return val_; }
    void Combine(const UniformManager& o) {/*do nothing*/}
    void Clone(const UniformManager& o) { val_ = o.val_; }
  private:
    Math::Vector3 val_;
  };
  struct UniformInitializer {
    UniformManager InitUniform() const { return UniformManager(val_); }
    void SetLightColor(const Math::Vector3& v) { val_ = v; }
  private:
    Math::Vector3 val_ {1.0f, 1.0f, 1.0f};
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

class PhongVertexShader : public Language::GLSLVertexGeneratorBase<
    Pack<ColorManager, VertexManager, NormalManager>,
    Pack<detail::Transform>,
    Pack<detail::ColorPass, detail::NormalPass>> {
  using Base = Language::GLSLVertexGeneratorBase<
      Pack<ColorManager, VertexManager, NormalManager>,
      Pack<detail::Transform>,
      Pack<detail::ColorPass, NormalPass>>;
public:
  PhongVertexShader(VariableFactory&& factory) : Base(std::move(factory)) {}
private:
  void DefineMain(const VariableFactory& factory) override;
};

class PhongFragmentShader : public Language::GLSLFragmentGeneratorBase<
    Pack<detail::ColorPass, detail::NormalPass>,
    Pack<detail::AmbientLight, detail::LightColor>,
    Pack<detail::OutputColor>> {
  using Base = Language::GLSLFragmentGeneratorBase<
      Pack<detail::ColorPass, detail::NormalPass>,
      Pack<detail::AmbientLight, detail::LightColor>,
      Pack<detail::OutputColor>>;
public:
  PhongFragmentShader(VariableFactory&& factory) : Base(std::move(factory)) {}
private:
  void DefineMain(const VariableFactory& factory) override;
};

}}}} // ShapeShifter::Rendering::Shaders::Programs

#endif /* RENDERING_SHADERS_PROGRAMS_BASIC_SHADER_H */

