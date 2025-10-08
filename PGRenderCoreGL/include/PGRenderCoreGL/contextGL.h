#pragma once
#include <memory>
#include <PGRenderCore/context.h>
#include <PGRenderCore/contextDesc.h>

namespace PGRenderCore {

    class BufferObjectGL;
    class ShaderGL;
    class TextureGL;
    class SamplerGL;
    class PipelineGL;
    class RenderTargetGL;
    class RenderPassGL;

    /**
     * @brief Implementación OpenGL 4 del Context.
     */
    class ContextGL : public Context {
    public:
        explicit ContextGL(const ContextDesc& desc);
        ~ContextGL() override;

        std::shared_ptr<BufferObject> createBufferObject(const BufferObject::Desc& desc) override;
        std::shared_ptr<Shader> createShader(const ShaderDesc& desc) override;
        std::shared_ptr<Texture> createTexture(const Texture::Desc& desc) override;
        std::shared_ptr<Sampler> createSampler(const Sampler::Desc& desc) override;
        std::shared_ptr<Pipeline> createPipeline(const Pipeline::Desc& desc) override;
        std::shared_ptr<RenderTarget> createRenderTarget(const RenderTarget::Desc& desc) override;
        std::shared_ptr<RenderPass> createRenderPass(const RenderPassDesc& desc) override;

        void bindVertexBuffer(const std::shared_ptr<BufferObject>& buffer) override;
        void bindIndexBuffer(const std::shared_ptr<BufferObject>& buffer) override;
        void bindUniformBuffer(uint32_t slot, const std::shared_ptr<BufferObject>& ubo) override;
        void bindTexture(uint32_t slot, const std::shared_ptr<Texture>& texture) override;
        void bindSampler(uint32_t slot, const std::shared_ptr<Sampler>& sampler) override;

        void setBlendMode(BlendMode mode) override;
        void setDepthFunc(DepthFunc func) override;
        void setCullMode(CullMode mode) override;

        void drawIndexed(size_t indexCount, size_t instanceCount = 1) override;
        void present() override;

        void traceRays(std::shared_ptr<RayTracingPipeline> pipeline,
            std::shared_ptr<TopLevelAS> tlas,
            uint32_t width, uint32_t height, uint32_t depth = 1) override;

    private:
        // Miembros internos para estado, contexto nativo, etc.
    };

} // namespace PGRenderCore
