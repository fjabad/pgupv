#include "PGRenderCoreGL/deviceGL.h"
#include <stdexcept>
#include <GL/glew.h>
#include "PGRenderCoreGL/contextGL.h"

namespace PGRenderCore {

DeviceGL::DeviceGL() {
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        throw std::runtime_error("Failed to initialize GLEW");
    }
    // Comprobaciones adicionales de versión/ extensiones aquí
}

DeviceGL::~DeviceGL() = default;

std::unique_ptr<Context> DeviceGL::createContext(const ContextDesc& desc) {
    if (desc.nativeWindowHandle == nullptr) {
        throw std::invalid_argument("nativeWindowHandle is null");
    }
    return std::make_unique<ContextGL>(desc);
}

DebugManager* DeviceGL::getDebugManager() {
    return nullptr;
}

} // namespace PGRenderCore
