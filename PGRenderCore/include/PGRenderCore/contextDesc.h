#pragma once
#include <cstdint>

#pragma once

namespace PGRenderCore {

    /**
     * @brief Parámetros para la creación y configuración de un Context gráfico.
     *
     * Se usa para describir la ventana o superficie nativa y opciones
     * configurables al crear un contexto gráfico (OpenGL, Vulkan, Metal, etc.).
     */
    struct ContextDesc {
        /**
         * @brief Handle o puntero nativo a la ventana o superficie donde renderizar.
         *
         * - En Windows puede ser HWND.
         * - En Linux X11 puede ser Window.
         * - En macOS NSView* o CAMetalLayer*.
         * - En GLFW/SDL se obtiene con las funciones nativas.
         */
        void* nativeWindowHandle = nullptr;

        /**
         * @brief Ancho inicial deseado para el contexto (opcional).
         *
         * Usado para configurar buffers de presentación o swapchains.
         */
        uint32_t width = 0;

        /**
         * @brief Alto inicial deseado para el contexto (opcional).
         */
        uint32_t height = 0;

        /**
         * @brief Indica si se quiere soporte para Realidad Virtual (opcional).
         */
        bool enableVRSupport = false;

        /**
         * @brief Nivel de antialiasing deseado (ej: 0 = none, 4 = 4x MSAA).
         */
        uint32_t multisampleCount = 0;

        /**
         * @brief Otros flags o parámetros adicionales podrían añadirse aquí.
         */
    };

} // namespace 

