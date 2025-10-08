#include "PGRenderCoreFactory/deviceFactory.h"
#include <stdexcept>
#include <sstream>

// Incluir backends habilitados según compilación condicional
#ifdef PGRENDERCORE_OPENGL_SUPPORT
    #include <PGRenderCoreGL/deviceGL.h>
#endif

#ifdef PGRENDERCORE_VULKAN_SUPPORT
    #include <PGRenderCoreVulkan/DeviceVK.h>
#endif

#ifdef PGRENDERCORE_METAL_SUPPORT
    #include <PGRenderCoreMetal/DeviceMetal.h>
#endif

#ifdef PGRENDERCORE_D3D12_SUPPORT
    #include <PGRenderCoreD3D12/DeviceD3D12.h>
#endif

namespace PGRenderCore {

std::unique_ptr<Device> DeviceFactory::createDevice(const DeviceDesc& desc) {
    RenderBackend selectedBackend = desc.backend;

    // Selección automática si se solicita
    if (selectedBackend == RenderBackend::Auto) {
        selectedBackend = selectAutomaticBackend();
    }

    // Verificar disponibilidad
    if (!isBackendAvailable(selectedBackend)) {
        std::ostringstream oss;
        oss << "Requested render backend is not available in this build. ";
        oss << "Available backends: ";
        auto available = getAvailableBackends();
        for (size_t i = 0; i < available.size(); ++i) {
            oss << available[i];
            if (i < available.size() - 1) oss << ", ";
        }
        throw std::runtime_error(oss.str());
    }

    // Crear el device según el backend seleccionado
    switch (selectedBackend) {
#ifdef PGRENDERCORE_OPENGL_SUPPORT
        case RenderBackend::OpenGL4:
            return std::make_unique<DeviceGL>();
#endif

#ifdef PGRENDERCORE_VULKAN_SUPPORT
        case RenderBackend::Vulkan:
            return std::make_unique<DeviceVK>();
#endif

#ifdef PGRENDERCORE_METAL_SUPPORT
        case RenderBackend::Metal:
            return std::make_unique<DeviceMetal>();
#endif

#ifdef PGRENDERCORE_D3D12_SUPPORT
        case RenderBackend::DirectX12:
            return std::make_unique<DeviceD3D12>();
#endif

        default:
            throw std::runtime_error("Unknown or unsupported render backend");
    }
}

std::vector<std::string> DeviceFactory::getAvailableBackends() {
    std::vector<std::string> backends;

#ifdef PGRENDERCORE_OPENGL_SUPPORT
    backends.push_back("OpenGL4");
#endif

#ifdef PGRENDERCORE_VULKAN_SUPPORT
    backends.push_back("Vulkan");
#endif

#ifdef PGRENDERCORE_METAL_SUPPORT
    backends.push_back("Metal");
#endif

#ifdef PGRENDERCORE_D3D12_SUPPORT
    backends.push_back("DirectX12");
#endif

    return backends;
}

bool DeviceFactory::isBackendAvailable(RenderBackend backend) {
    switch (backend) {
        case RenderBackend::OpenGL4:
#ifdef PGRENDERCORE_OPENGL_SUPPORT
            return true;
#else
            return false;
#endif

        case RenderBackend::Vulkan:
#ifdef PGRENDERCORE_VULKAN_SUPPORT
            return true;
#else
            return false;
#endif

        case RenderBackend::Metal:
#ifdef PGRENDERCORE_METAL_SUPPORT
            return true;
#else
            return false;
#endif

        case RenderBackend::DirectX12:
#ifdef PGRENDERCORE_D3D12_SUPPORT
            return true;
#else
            return false;
#endif

        case RenderBackend::Auto:
            return true;  // Auto siempre está "disponible" (selecciona automáticamente)

        default:
            return false;
    }
}

RenderBackend DeviceFactory::selectAutomaticBackend() {
    // Prioridad por plataforma
#if defined(_WIN32) || defined(_WIN64)
    // Windows: preferir DirectX 12, luego Vulkan, luego OpenGL
    #ifdef PGRENDERCORE_D3D12_SUPPORT
        return RenderBackend::DirectX12;
    #elif defined(PGRENDERCORE_VULKAN_SUPPORT)
        return RenderBackend::Vulkan;
    #elif defined(PGRENDERCORE_OPENGL_SUPPORT)
        return RenderBackend::OpenGL4;
    #endif

#elif defined(__APPLE__)
    // macOS/iOS: preferir Metal, luego OpenGL
    #ifdef PGRENDERCORE_METAL_SUPPORT
        return RenderBackend::Metal;
    #elif defined(PGRENDERCORE_OPENGL_SUPPORT)
        return RenderBackend::OpenGL4;
    #endif

#elif defined(__linux__)
    // Linux: preferir Vulkan, luego OpenGL
    #ifdef PGRENDERCORE_VULKAN_SUPPORT
        return RenderBackend::Vulkan;
    #elif defined(PGRENDERCORE_OPENGL_SUPPORT)
        return RenderBackend::OpenGL4;
    #endif

#else
    // Plataforma desconocida: intentar OpenGL como fallback universal
    #ifdef PGRENDERCORE_OPENGL_SUPPORT
        return RenderBackend::OpenGL4;
    #endif
#endif

    // Si llegamos aquí, no hay backend disponible
    throw std::runtime_error("No render backend available for automatic selection");
}

// Función de conveniencia global
std::unique_ptr<Device> createDevice(const DeviceDesc& desc) {
    return DeviceFactory::createDevice(desc);
}

} // namespace PGRenderCore
