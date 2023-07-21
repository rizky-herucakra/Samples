#pragma once
#include <RavEngine/GameObject.hpp>
#include <RavEngine/BuiltinMaterials.hpp>
#include <RavEngine/Texture.hpp>

struct FlagMatUniforms {
    float time = 0;
};

struct FlagMat : public RavEngine::Material{
    FlagMat(std::string_view secondShader = "flag") : Material("flag", secondShader, RavEngine::MaterialConfig{
        .vertConfig = RavEngine::defaultVertexConfig,
        .colorBlendConfig = RavEngine::defaultColorBlendConfig,
        .bindings = {
            {
                .binding = 0,
                .type = RGL::BindingType::Sampler,
                .stageFlags = RGL::BindingVisibility::Fragment,
            },
             {
                .binding = 1,
                .type = RGL::BindingType::SampledImage,
                .stageFlags = RGL::BindingVisibility::Fragment,
            },
            {
                .binding = 2,
                .type = RGL::BindingType::StorageBuffer,
                .stageFlags = RGL::BindingVisibility::Vertex
            }
        },
        .pushConstantSize = sizeof(FlagMatUniforms)
        }) {}
};

struct FlagMatInst : public RavEngine::MaterialInstance{
    FlagMatInst(Ref<FlagMat> f) : MaterialInstance(f){
        SetAlbedoTexture(RavEngine::Texture::Manager::defaultTexture);
    }
    void SetTime(float time) {
        pushConstantData.time = time;
    }
    void SetAlbedoTexture(Ref<RavEngine::Texture> texture) {
        textureBindings[1] = texture;
    }

    virtual const RGL::untyped_span GetPushConstantData() const override {
        return pushConstantData;
    }
private:
    FlagMatUniforms pushConstantData;
};


struct Flagpole : public RavEngine::GameObject{
    void Create();
    struct entry{
        std::string name;
        Ref<FlagMatInst> matInst;
    };
    RavEngine::Vector<entry> flags;
    
    void SwitchToFlag(uint16_t idx);
    Ref<RavEngine::MaterialInstance> GetCurrentMaterial();
};
