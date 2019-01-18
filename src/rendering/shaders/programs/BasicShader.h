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

struct PosPass : InterfaceVariableBase<PosPass, Language::Vec3> {
  PosPass() = delete;

  using Base = InterfaceVariableBase<PosPass, Language::Vec3>;
  using Base::InterfaceVariableBase;
  static constexpr const char* name() {
    return "thePos";
  }
  static constexpr bool smooth = true;
  Variable_T& thePos = Base::var;
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
  using Required = Pack<>;

  struct UniformManager {
    using StorageType = Math::Matrix4;
    UniformManager() : translation_(0,0,0,1) {}

    void Rotation(const Math::Quaternion& rot) { rotation_ = rot; }
    const Math::Quaternion& Rotation() const { return rotation_; }
    
    void Translation(const Math::Vector4& trans) { translation_ = trans; }
    const Math::Vector4& Translation() const { return translation_; }

  private:
    Math::Quaternion rotation_;
    Math::Vector4 translation_;
  };

  struct UniformAccumulator : public UniformManager {

    Math::Matrix4 Data(const Camera& camera) const {
      auto mat = Rotation().RotationMatrix();
      mat.WriteColumn(3, Translation());
      return camera.ProjectionMatrix() * rootToCamera_ * mat;
    }

    Math::Matrix4 WorldTransform() const {
      auto mat = Rotation().RotationMatrix();
      mat.WriteColumn(3, Translation());
      return mat;
    }

    void Combine(const UniformManager& manager) {
      Translation(Translation() + Rotation().RotationMatrix() * manager.Translation());
      Rotation(Rotation()*manager.Rotation());
    }

    void CombineInverse(const UniformManager& manager) {
      Rotation(Rotation() * manager.Rotation().Inverse());
      Translation(Translation() - Rotation().RotationMatrix() * manager.Translation());
    }

    void Clone(const UniformAccumulator& other) {
      Rotation(other.Rotation());
      Translation(other.Translation());
      rootToCamera_ = other.rootToCamera_;
    }

    void RootToCamera(const Math::Matrix4& mat) { rootToCamera_ = mat; }

  private:
    Math::Matrix4 rootToCamera_ = Math::Matrix4::Identity();
  };

  struct UniformInitializer
  {
    void InitUniform(UniformAccumulator& accumulator) const {
      UniformAccumulator tmp{};
      for (auto m : path_)
      {
        tmp.CombineInverse(*m);
      }
      accumulator.RootToCamera(tmp.WorldTransform());
    }

    template <typename T1, typename T2>
    void SetCameraNode(const Util::MultiReferenceWrapper<T1, T2>& node)
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
  using Required = Pack<>;

  struct UniformManager {
    UniformManager(float v) : ambientAmp_{v} {}
    UniformManager() : ambientAmp_{1.0f} {}
    using StorageType = float;
  protected:
    float ambientAmp_ = 1.0f;
  };

  struct UniformAccumulator : public UniformManager {
    float Data(const Camera&) const { return ambientAmp_; }
    void Combine(const UniformManager&) {/*do nothing*/}
    void Clone(const UniformAccumulator& o) { ambientAmp_ = o.ambientAmp_; }
    void AmbientAmplitude(float a) { ambientAmp_ = a; }
  };

  struct UniformInitializer {
    void InitUniform(UniformAccumulator& accumulator ) const {
      accumulator.AmbientAmplitude(ambientAmp_);
    }
    void SetAmbientLight(float v) { ambientAmp_ = v; }
  private:
    float ambientAmp_ = 1.0f;
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
  using Required = Pack<>;

  struct UniformManager {
    UniformManager(const Math::Vector3& v) : lightColor_{v} {}
    UniformManager() : lightColor_{1.0f, 1.0f, 1.0f} {}
    using StorageType = Math::Vector3;
  protected:
    Math::Vector3 lightColor_;
  };

  struct UniformAccumulator : public UniformManager{
    Math::Vector3 Data(const Camera&) const { return lightColor_; }
    void Combine(const UniformManager&) {/*do nothing*/}
    void Clone(const UniformAccumulator& o) { lightColor_ = o.lightColor_; }
    void LightColor(const Math::Vector3& l) { lightColor_ = l; }
  };

  struct UniformInitializer {
    void InitUniform(UniformAccumulator& accumulator) const {
      accumulator.LightColor(lightColor_);
    }
    void SetLightColor(const Math::Vector3& v) { lightColor_ = v; }
  private:
    Math::Vector3 lightColor_ {1.0f, 1.0f, 1.0f};
  };
};

struct LightPos : UniformVariableBase<LightPos, Language::Vec3> {
  LightPos() = delete;
  using Base = UniformVariableBase<LightPos, Language::Vec3>;
  using Base::UniformVariableBase;
  static constexpr const char* name() {
    return "lightPos";
  }
  static constexpr bool smooth = false;
  Variable_T& lightPos = Base::var;

  using Required = Pack<Transform>;
  struct UniformManager {
    using StorageType = Math::Vector3;
    UniformManager() : worldPos_(0,0,0,0) {}
    UniformManager(const Math::Vector4& v) : worldPos_(v) {}

  protected:
    Math::Vector4 worldPos_;
  };

  struct UniformAccumulator : public UniformManager {
    Math::Vector3 Data(const Camera& camera) const {
      const auto& tmp = transRef_->Translation();
      auto tmp3 = transRef_->Rotation().Inverse().RotationMatrix() * (worldPos_ - tmp); 
      return Math::Vector3(tmp3[0], tmp3[1], tmp3[2]);
    }

    void Combine(const UniformManager&) {}
    void Clone(const UniformAccumulator& other) {
      worldPos_ = other.worldPos_;
    }

    void SetSiblings(const Util::MultiReferenceWrapper<UniformAccumulator, Transform::UniformAccumulator>& siblings) {
      // TODO this is ugly
      transRef_ = &(*siblings.template Convert<Transform::UniformAccumulator>());
      assert(transRef_ != nullptr);
    }
    void WorldPos(const Math::Vector4& p) { worldPos_ = p; }
  private:
    const Transform::UniformAccumulator* transRef_ = nullptr;
  };

  struct UniformInitializer
  {
    void InitUniform(UniformAccumulator& accumulator ) const {
      Transform::UniformAccumulator trans{};
      for (auto m : path_)
      {
        trans.Combine(*m);
      }
      accumulator.WorldPos(trans.Rotation().RotationMatrix() * relPos_ + trans.Translation());
    }

    template <typename T1, typename T2>
    void SetLightNode(const Util::MultiReferenceWrapper<T1, T2>& node)
    {
      static_assert(std::is_base_of<UniformManager, T2>::value,
                    "SetLightNode called node without LightPos Uniform");

      const auto& path = node.template Convert<T2>()->PathToRoot();
      path_ = std::vector<const Transform::UniformManager*>(path.rbegin(), path.rend());
    }
    void SetLightRelPos(const Math::Vector4& v) { relPos_ = v; }

  private:
    std::vector<const Transform::UniformManager*> path_;
    Math::Vector4 relPos_ = Math::Vector4(0,0,0,0);
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
    Pack<detail::ColorPass, detail::NormalPass, detail::PosPass>> {
  using Base = Language::GLSLVertexGeneratorBase<
      Pack<ColorManager, VertexManager, NormalManager>,
      Pack<detail::Transform>,
      Pack<detail::ColorPass, NormalPass, detail::PosPass>>;
public:
  PhongVertexShader(VariableFactory&& factory) : Base(std::move(factory)) {}
private:
  void DefineMain(const VariableFactory& factory) override;
};

class PhongFragmentShader : public Language::GLSLFragmentGeneratorBase<
    Pack<detail::ColorPass, detail::NormalPass, detail::PosPass>,
    Pack<detail::AmbientLight, detail::LightColor, detail::LightPos>,
    Pack<detail::OutputColor>> {
  using Base = Language::GLSLFragmentGeneratorBase<
      Pack<detail::ColorPass, detail::NormalPass, detail::PosPass>,
      Pack<detail::AmbientLight, detail::LightColor, detail::LightPos>,
      Pack<detail::OutputColor>>;
public:
  PhongFragmentShader(VariableFactory&& factory) : Base(std::move(factory)) {}
private:
  void DefineMain(const VariableFactory& factory) override;
};

}}}} // ShapeShifter::Rendering::Shaders::Programs

#endif /* RENDERING_SHADERS_PROGRAMS_BASIC_SHADER_H */

