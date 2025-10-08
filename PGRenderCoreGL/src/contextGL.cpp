#include <PGRenderCoreGL/contextGL.h>
#include <stdexcept>
#include <GL/glew.h>
#include <PGRenderCoreGL/bufferObjectGL.h>
#include <PGRenderCoreGL/shaderGL.h>
#include <PGRenderCoreGL/textureGL.h>
#include <PGRenderCoreGL/samplerGL.h>
#include <PGRenderCoreGL/pipelineGL.h>
#include <PGRenderCoreGL/renderTargetGL.h>
#include <PGRenderCoreGL/renderPassGL.h>

namespace PGRenderCore {

    ContextGL::ContextGL(const ContextDesc& desc) {
        if (!desc.nativeWindowHandle) {
            throw std::invalid_argument("nativeWindowHandle is null");
        }
        // Inicialización contexto OpenGL aquí si es necesario
    }

    ContextGL::~ContextGL() = default;

    std::shared_ptr<BufferObject> ContextGL::createBufferObject(const BufferObject::Desc& desc) {
        return std::make_shared<BufferObjectGL>(desc);
    }

    std::shared_ptr<Shader> ContextGL::createShader(const ShaderDesc& desc) {
        return std::make_shared<ShaderGL>(desc);
    }

    std::shared_ptr<Texture> ContextGL::createTexture(const Texture::Desc& desc) {
        return std::make_shared<TextureGL>(desc);
    }

    std::shared_ptr<Sampler> ContextGL::createSampler(const Sampler::Desc& desc) {
        return std::make_shared<SamplerGL>(desc);
    }

    std::shared_ptr<Pipeline> ContextGL::createPipeline(const Pipeline::Desc& desc) {
        return std::make_shared<PipelineGL>(desc);
    }

    std::shared_ptr<RenderTarget> ContextGL::createRenderTarget(const RenderTarget::Desc& desc) {
        return std::make_shared<RenderTargetGL>(desc);
    }

    std::shared_ptr<RenderPass> ContextGL::createRenderPass(const RenderPassDesc& desc) {
        return std::make_shared<RenderPassGL>(desc);
    }

    void ContextGL::bindVertexBuffer(const std::shared_ptr<BufferObject>& buffer) {
        auto glBuffer = std::dynamic_pointer_cast<BufferObjectGL>(buffer);
        if (!glBuffer) throw std::invalid_argument("Invalid buffer type for OpenGL");
        glBindBuffer(GL_ARRAY_BUFFER, glBuffer->nativeHandle());
    }

    void ContextGL::bindIndexBuffer(const std::shared_ptr<BufferObject>& buffer) {
        auto glBuffer = std::dynamic_pointer_cast<BufferObjectGL>(buffer);
        if (!glBuffer) throw std::invalid_argument("Invalid buffer type for OpenGL");
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glBuffer->nativeHandle());
    }

    void ContextGL::bindUniformBuffer(uint32_t slot, const std::shared_ptr<BufferObject>& ubo) {
        auto glBuffer = std::dynamic_pointer_cast<BufferObjectGL>(ubo);
        if (!glBuffer) throw std::invalid_argument("Invalid buffer type for OpenGL");
        glBindBufferBase(GL_UNIFORM_BUFFER, slot, glBuffer->nativeHandle());
    }

    void ContextGL::bindTexture(uint32_t slot, const std::shared_ptr<Texture>& texture) {
        auto glTex = std::dynamic_pointer_cast<TextureGL>(texture);
        if (!glTex) throw std::invalid_argument("Invalid texture type for OpenGL");
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(glTex->toGLTarget(), static_cast<uint32_t>(glTex->nativeHandle()));
    }

    void ContextGL::bindSampler(uint32_t slot, const std::shared_ptr<Sampler>& sampler) {
        auto glSampler = std::dynamic_pointer_cast<SamplerGL>(sampler);
        if (!glSampler) throw std::invalid_argument("Invalid sampler type for OpenGL");
        glBindSampler(slot, static_cast<uint32_t>(glSampler->nativeHandle()));
    }

    void ContextGL::setBlendMode(BlendMode mode) {
        switch (mode) {
        case BlendMode::None:
            glDisable(GL_BLEND);
            break;
        case BlendMode::AlphaBlend:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BlendMode::Additive:
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        default:
            glDisable(GL_BLEND);
            break;
        }
    }

    void ContextGL::setDepthFunc(DepthFunc func) {
        static const GLenum glFuncs[] = {
            GL_NEVER, GL_LESS, GL_LEQUAL, GL_GREATER,
            GL_GEQUAL, GL_EQUAL, GL_NOTEQUAL, GL_ALWAYS
        };
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(glFuncs[static_cast<int>(func)]);
    }

    void ContextGL::setCullMode(CullMode mode) {
        if (mode == CullMode::None) {
            glDisable(GL_CULL_FACE);
        }
        else {
            glEnable(GL_CULL_FACE);
            glCullFace(mode == CullMode::Back ? GL_BACK : GL_FRONT);
        }
    }

    void ContextGL::drawIndexed(size_t indexCount, size_t instanceCount) {
        if (instanceCount > 1) {
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(instanceCount));
        }
        else {
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);
        }
    }

    void ContextGL::present() {
        // Swap hecho en la plataforma de ventana (GLFW, SDL...)
    }

    void ContextGL::traceRays(std::shared_ptr<RayTracingPipeline>, std::shared_ptr<TopLevelAS>, uint32_t, uint32_t, uint32_t) {
        throw std::runtime_error("Ray tracing not supported in OpenGL 4.x backend");
    }

} // namespace PGRenderCore
