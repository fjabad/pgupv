#pragma once

#include <memory>
#include "stateConstants.h"

namespace PGRenderCore {

    /**
     * @brief Configuración de pipeline gráfico.
     *
     * Agrupa shaders, estado de blending, culling, test de profundidad, etc.
     */
    class Pipeline {
    public:
        /**
         * @brief Descripción para la creación del pipeline.
         */
        struct Desc {
            std::shared_ptr<class Shader> vertexShader;
            std::shared_ptr<class Shader> fragmentShader;
            enum class BlendMode blendMode = BlendMode::None;
            enum class DepthFunc depthFunc = DepthFunc::Less;
            enum class CullMode cullMode = CullMode::Back;

            struct PolygonModeDesc {
                PolygonMode front = PolygonMode::Fill;
                PolygonMode back = PolygonMode::Fill;
            } polygonMode;
            // Se pueden añadir más configuraciones: rasterizer, maschering, multicore, etc.
        };

        virtual ~Pipeline() = default;

        /**
         * @brief Devuelve la descripción usada para crear este pipeline.
         */
        virtual const Desc& getDesc() const = 0;

        /**
         * @brief Retorna un identificador nativo o handle interno.
         *        Para depuración, integración o validación.
         */
        virtual uint64_t nativeHandle() const = 0;
    };

} // namespace PGRenderCore
