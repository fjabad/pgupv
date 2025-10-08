#pragma once

namespace PGRenderCore {

    /**
     * @brief Modos de blending para mezclar colores en framebuffer.
     */
    enum class BlendMode {
        None,           // Sin blending, sobrescribe directamente
        AlphaBlend,     // Mezcla alfa (src alpha, 1-src alpha)
        Additive,       // Suma colores (emisión, fuego, glow)
        Multiply,       // Multiplica colores (sombra, oscurecido)
        Custom          // Permite definir blend states específicos (backend)
    };

    /**
     * @brief Funciones de comparación usadas en test de profundidad (depth test).
     */
    enum class DepthFunc {
        Never,          // Nunca pasa (descarta todo)
        Less,           // Pasa si nuevo valor menor que almacenado (default típico)
        LessEqual,      // Pasa si nuevo valor menor o igual
        Greater,        // Pasa si nuevo valor mayor que almacenado
        GreaterEqual,   // Pasa si nuevo valor mayor o igual
        Equal,          // Igualdad exacta
        NotEqual,       // Diferente
        Always          // Siempre pasa (sin test)
    };

    /**
     * @brief Modos de culling para descartar polígonos según orientación.
     */
    enum class CullMode {
        None,           // No se descarta nada
        Front,          // Culling caras frontales
        Back            // Culling caras traseras (default típico)
    };

    /**
     * @brief Referencia a las caras de un polígono.
     */
    enum class PolygonFace {
        Front,          // Cara frontal
        Back,           // Cara traseras
        FrontAndBack    // Ambas caras
    };

    /**
     * @brief Modos de dibujado de los polígonos.
     */
    enum class PolygonMode {
        Fill,           // Relleno
        Line,           // Sólo aristas
        Point           // Sólo vértices
    };

} // namespace PGRenderCore
