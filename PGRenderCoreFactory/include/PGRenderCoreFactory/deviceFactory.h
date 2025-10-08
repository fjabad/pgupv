#pragma once
#include <PGRenderCore/device.h>
#include <memory>
#include <string>
#include <vector>

namespace PGRenderCore {

/**
 * @brief Enumeración de backends gráficos soportados.
 */
enum class RenderBackend {
    OpenGL4,      ///< OpenGL 4.x
    Vulkan,       ///< Vulkan API
    Metal,        ///< Metal (macOS/iOS)
    DirectX12,    ///< DirectX 12
    Auto          ///< Selección automática según plataforma
};

/**
 * @brief Descriptor para la creación del Device.
 */
struct DeviceDesc {
    RenderBackend backend = RenderBackend::Auto;
    void* nativeWindowHandle = nullptr;  ///< Handle nativo de ventana (opcional para inicialización)
    bool enableDebug = false;            ///< Habilitar validación/debug
    bool enableVSync = true;             ///< Habilitar sincronización vertical
};

/**
 * @brief Factory para crear dispositivos gráficos según backend.
 * Solo compila soporte para backends habilitados en CMake.
 */
class DeviceFactory {
public:
    /**
     * @brief Crea un Device según el descriptor especificado.
     * @param desc Descriptor con backend y configuración.
     * @return Unique pointer al Device creado.
     * @throws std::runtime_error si el backend no está soportado o disponible.
     */
    static std::unique_ptr<Device> createDevice(const DeviceDesc& desc);

    /**
     * @brief Obtiene lista de backends disponibles en esta compilación.
     * @return Vector con nombres de backends soportados.
     */
    static std::vector<std::string> getAvailableBackends();

    /**
     * @brief Verifica si un backend específico está disponible.
     * @param backend Backend a verificar.
     * @return true si está disponible, false en caso contrario.
     */
    static bool isBackendAvailable(RenderBackend backend);

private:
    static RenderBackend selectAutomaticBackend();
};

// Función de conveniencia global
std::unique_ptr<Device> createDevice(const DeviceDesc& desc);

} // namespace PGRenderCore
