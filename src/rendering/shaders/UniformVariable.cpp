/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   InterfaceVariableBase.cpp
 * Author: bbyington
 *
 * Created on October 24, 2016, 8:18 AM
 */

#include "rendering/shaders/UniformVariable.h"

using namespace ShapeShifter::Rendering::Shaders;
using namespace ShapeShifter::Rendering;
using namespace ShapeShifter;

// ISSUE set up tests and move these there, maybe make them runtime?
namespace {

struct HasSmooth {
    static constexpr bool smooth = true;
};

void Test() {

  class NoStorage {
  public:
    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    Math::Matrix4 Data(const Camera& camera) const {
      return camera.ProjectionMatrix();
    }

    void Combine(const NoStorage& other) {}
    void CombineInverse(const NoStorage& other) {}
    void Clone(const NoStorage& other) {}
  };
  static_assert(!UniformVarManager<NoStorage>);

  class NoDefaultConstruct {
  public:
    NoDefaultConstruct(int) {}
    using StorageType = Math::Matrix4;

    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    Math::Matrix4 Data(const Camera& camera) const {
      return camera.ProjectionMatrix();
    }

    void Combine(const NoDefaultConstruct& other) {}
    void CombineInverse(const NoDefaultConstruct& other) {}
    void Clone(const NoDefaultConstruct& other) {}
  };
  static_assert(!UniformVarManager<NoDefaultConstruct>);

  class NoData {
  public:
    using StorageType = Math::Matrix4;

    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    void Combine(const NoData& other) {}
    void CombineInverse(const NoData& other) {}
    void Clone(const NoData& other) {}
  };
  static_assert(!UniformVarManager<NoData>);

  class DataWrongArgs {
  public:
    using StorageType = Math::Matrix4;

    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    Math::Matrix4 Data(int) const {
      return Math::Matrix4{{}};
    }

    void Combine(const DataWrongArgs& other) {}
    void CombineInverse(const DataWrongArgs& other) {}
    void Clone(const DataWrongArgs& other) {}
  };
  static_assert(!UniformVarManager<DataWrongArgs>);

  class DataWrongReturn {
  public:
    using StorageType = Math::Matrix4;

    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    int Data(const Camera& camera) const {
        return 1;
    }

    void Combine(const DataWrongReturn& other) {}
    void CombineInverse(const DataWrongReturn& other) {}
    void Clone(const DataWrongReturn& other) {}
  };
  static_assert(!UniformVarManager<DataWrongReturn>);

  class DataNotConst {
  public:
    using StorageType = Math::Matrix4;

    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    Math::Matrix4 Data(const Camera& camera){
      return camera.ProjectionMatrix();
    }

    void Combine(const DataNotConst& other) {}
    void CombineInverse(const DataNotConst& other) {}
    void Clone(const DataNotConst& other) {}
  };
  static_assert(!UniformVarManager<DataNotConst>);

  class NoClone {
  public:
    using StorageType = Math::Matrix4;

    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    Math::Matrix4 Data(const Camera& camera) const {
      return camera.ProjectionMatrix();
    }

    void Combine(const NoClone& other) {}
    void CombineInverse(const NoClone& other) {}
  };
  static_assert(!UniformVarManager<NoClone>);

  class cloneWrongArgs {
  public:
    using StorageType = Math::Matrix4;

    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    Math::Matrix4 Data(const Camera& camera) const {
      return camera.ProjectionMatrix();
    }

    void Combine(int) {}
    void CombineInverse(const cloneWrongArgs& other) {}
    void Clone(const cloneWrongArgs& other) {}
  };
  static_assert(!UniformVarManager<cloneWrongArgs>);

  class CloneWrongReturn {
  public:
    using StorageType = Math::Matrix4;

    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    Math::Matrix4 Data(const Camera& camera) const {
      return camera.ProjectionMatrix();
    }

    void Combine(const CloneWrongReturn& other) {}
    void CombineInverse(const CloneWrongReturn& other) {}
    int Clone(const CloneWrongReturn& other) { return 1; }
  };
  static_assert(!UniformVarManager<CloneWrongReturn>);

  class NoCombine {
  public:
    using StorageType = Math::Matrix4;

    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    Math::Matrix4 Data(const Camera& camera) const {
      return camera.ProjectionMatrix();
    }

    void CombineInverse(const NoCombine& other) {}
    void Clone(const NoCombine& other) {}
  };
  static_assert(!UniformVarManager<NoCombine>);

  class CombineWrongArgs {
  public:
    using StorageType = Math::Matrix4;

    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    Math::Matrix4 Data(const Camera& camera) const {
      return camera.ProjectionMatrix();
    }

    void Combine(const CombineWrongArgs& other) {}
    void CombineInverse(const CombineWrongArgs& other) {}
    void Clone(int) {}
  };
  static_assert(!UniformVarManager<CombineWrongArgs>);

  class CombineWrongReturn {
  public:
    using StorageType = Math::Matrix4;

    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    Math::Matrix4 Data(const Camera& camera) const {
      return camera.ProjectionMatrix();
    }

    void Combine(const CombineWrongReturn& other) {}
    void CombineInverse(const CombineWrongReturn& other) {}
    int Clone(const CombineWrongReturn& other) { return 1;}
  };
  static_assert(!UniformVarManager<CombineWrongReturn>);

  class NoInverse {
  public:
    using StorageType = Math::Matrix4;

    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    Math::Matrix4 Data(const Camera& camera) const {
      return camera.ProjectionMatrix();
    }

    void Combine(const NoInverse& other) {}
    void Clone(const NoInverse& other) {}
  };
  static_assert(!UniformVarManager<NoInverse>);

  class InverseWrongArgs {
  public:
    using StorageType = Math::Matrix4;

    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    Math::Matrix4 Data(const Camera& camera) const {
      return camera.ProjectionMatrix();
    }

    void Combine(const InverseWrongArgs& other) {}
    void CombineInverse(int) {}
    void Clone(const InverseWrongArgs& other) {}
  };
  static_assert(!UniformVarManager<InverseWrongArgs>);

  class InverseWrongRet {
  public:
    using StorageType = Math::Matrix4;

    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    Math::Matrix4 Data(const Camera& camera) const {
      return camera.ProjectionMatrix();
    }

    void Combine(const InverseWrongRet& other) {}
    int CombineInverse(const InverseWrongRet& other) { return 1;}
    void Clone(const InverseWrongRet& other) {}
  };
  static_assert(!UniformVarManager<InverseWrongRet>);

  class GoodManager {
  public:
    using StorageType = Math::Matrix4;

    void SetRotation(const Math::Quaternion& rot) {}
    void SetTranslation(const Math::Vector4& trans) {}

    Math::Matrix4 Data(const Camera& camera) const {
      return camera.ProjectionMatrix();
    }

    void Combine(const GoodManager& other) {}
    void CombineInverse(const GoodManager& other) {}
    void Clone(const GoodManager& other) {}
  };
  static_assert(UniformVarManager<GoodManager>);

  struct NotInterface {
    using UniformManager = GoodManager;

    class UniformInitializer
    {
      public:
        UniformManager InitUniform() const { return {}; }
    };
  };
  static_assert(!UniformVariable<NotInterface>);

  struct NoManager : Language::Variable<Language::Mat4>, HasSmooth {
    using Base = Language::Variable<Language::Mat4>;
    NoManager() = delete;
    NoManager(VariableFactory& factory)
      : Base(factory.create<Language::Mat4>(name()))
    {}
    static constexpr const char* name() {
      return "transform";
    }

    class UniformInitializer
    {
      public:
        GoodManager InitUniform() const { return {}; }
    };
  };
  static_assert(!UniformVariable<NoManager>);

  struct NoInitializer : Language::Variable<Language::Mat4>, HasSmooth {
    using Base = Language::Variable<Language::Mat4>;
    NoInitializer() = delete;
    NoInitializer(VariableFactory& factory)
      : Base(factory.create<Language::Mat4>(name()))
    {}
    static constexpr const char* name() {
      return "transform";
    }

    using UniformManager = GoodManager;
  };
  static_assert(!UniformVariable<NoInitializer>);

  struct NoInitFunc : Language::Variable<Language::Mat4>, HasSmooth {
    using Base = Language::Variable<Language::Mat4>;
    NoInitFunc() = delete;
    NoInitFunc(VariableFactory& factory)
      : Base(factory.create<Language::Mat4>(name()))
    {}
    static constexpr const char* name() {
      return "transform";
    }

    using UniformManager = GoodManager;

    class UniformInitializer
    {
    };
  };
  static_assert(!UniformVariable<NoInitFunc>);

  struct GoodUniform : Language::Variable<Language::Mat4>, HasSmooth {
    using Base = Language::Variable<Language::Mat4>;
    GoodUniform() = delete;
    GoodUniform(VariableFactory& factory)
      : Base(factory.create<Language::Mat4>(name()))
    {}
    static constexpr const char* name() {
      return "transform";
    }

    using UniformManager = GoodManager;

    class UniformInitializer
    {
      public:
        UniformManager InitUniform() const { return {}; }
    };
  };
  static_assert(UniformVariable<GoodUniform>);
}

}
