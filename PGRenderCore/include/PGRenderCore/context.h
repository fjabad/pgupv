#pragma once

#pragma once
#include <memory>
#include <cstddef>
#include <cstdint>

#include "bufferObject.h"
#include "texture.h"
#include "sampler.h"
#include "pipeline.h"
#include "renderTarget.h"

namespace PGRenderCore {

    enum class BlendMode;
    enum class DepthFunc;
    enum class CullMode;

    class Shader;
    class RenderPass;
    class RayTracingPipeline;
    class TopLevelAS;

    /**
     * @brief Interfaz abstracta para el contexto gráfico.
     *
     * Representa un entorno de ejecución gráfico, con soporte para
     * creación y gestión de recursos, binding, configuración de estados,
     * dibujo y operaciones de trazado de rayos.
     */
    class Context {
    public:
        virtual ~Context() = default;

        // --- Creación de recursos ---

        virtual std::shared_ptr<BufferObject> createBufferObject(const struct BufferObject::Desc& desc) = 0;
        virtual std::shared_ptr<Shader> createShader(const struct ShaderDesc& desc) = 0;
        virtual std::shared_ptr<Texture> createTexture(const struct Texture::Desc& desc) = 0;
        virtual std::shared_ptr<Sampler> createSampler(const struct Sampler::Desc& desc) = 0;
        virtual std::shared_ptr<Pipeline> createPipeline(const struct Pipeline::Desc& desc) = 0;
        virtual std::shared_ptr<RenderTarget> createRenderTarget(const struct RenderTarget::Desc& desc) = 0;
        virtual std::shared_ptr<RenderPass> createRenderPass(const struct RenderPassDesc& desc) = 0;

        // --- Binding de recursos para el pipeline activo ---

        /**
         * @brief Bindea un buffer de vértices para uso durante draw calls.
         */
        virtual void bindVertexBuffer(const std::shared_ptr<BufferObject>& buffer) = 0;

        /**
         * @brief Bindea un buffer de índices para uso durante draw calls indexados.
         */
        virtual void bindIndexBuffer(const std::shared_ptr<BufferObject>& buffer) = 0;

        /**
         * @brief Bindea un buffer uniforme (UBO) a un slot de shader.
         * @param slot Número del binding slot en el shader.
         * @param ubo Buffer uniforme a bindear.
         */
        virtual void bindUniformBuffer(uint32_t slot, const std::shared_ptr<BufferObject>& ubo) = 0;

        /**
         * @brief Bindea una textura a un slot para uso en shader.
         */
        virtual void bindTexture(uint32_t slot, const std::shared_ptr<Texture>& texture) = 0;

        /**
         * @brief Bindea un sampler a un slot para uso en shader.
         */
        virtual void bindSampler(uint32_t slot, const std::shared_ptr<Sampler>& sampler) = 0;

        // --- Configuración de estados gráficos ---

        virtual void setBlendMode(BlendMode mode) = 0;
        virtual void setDepthFunc(DepthFunc func) = 0;
        virtual void setCullMode(CullMode mode) = 0;

        // --- Comandos de dibujo y presentación ---

        /**
         * @brief Ejecuta un draw call indexado con un número de índices e instancias.
         */
        virtual void drawIndexed(size_t indexCount, size_t instanceCount = 1) = 0;

        /**
         * @brief Presenta la imagen renderizada en pantalla (swapchain).
         */
        virtual void present() = 0;

        // --- Soporte para trazado de rayos por hardware ---

        /**
         * @brief Dispara un comando de trazado de rayos usando pipeline y aceleración.
         * @param pipeline Pipeline de trazado que define shaders raytracing.
         * @param scene Estructura de aceleración superior (TLAS).
         * @param width Anchura de imagen a generar.
         * @param height Altura de imagen a generar.
         * @param depth Profundidad (usa 1 para 2D).
         */
        virtual void traceRays(std::shared_ptr<RayTracingPipeline> pipeline,
            std::shared_ptr<TopLevelAS> scene,
            uint32_t width,
            uint32_t height,
            uint32_t depth = 1) = 0;
    };

} // namespace PGRenderCore
