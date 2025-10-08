#pragma once
#include <memory>
#include <PGRenderCore/device.h>
#include <PGRenderCore/context.h>

namespace PGRenderCore {

    /**
     * @brief Implementación de Device para OpenGL 4.
     */
    class DeviceGL : public Device {
    public:
        DeviceGL();
        ~DeviceGL() override;

        std::unique_ptr<Context> createContext(const ContextDesc& desc) override;
        DebugManager* getDebugManager() override;

    private:
        // Puedes añadir aquí miembros privados si hace falta
    };

} // namespace PGRenderCore
