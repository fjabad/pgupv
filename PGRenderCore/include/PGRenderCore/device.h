#pragma once

#include <memory>
namespace PGRenderCore {
    class Device {
    public:
        virtual std::unique_ptr<class Context> createContext(const struct ContextDesc&) = 0;
        virtual class DebugManager* getDebugManager() = 0;
        virtual ~Device() = default;
    };
}
