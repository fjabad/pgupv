#pragma once
#include "PGRenderCore/bufferObject.h"
#include <cstddef>

namespace PGRenderCore {

    /**
     * @brief Implementación de BufferObject para OpenGL.
     */
    class BufferObjectGL : public BufferObject {
    public:
        explicit BufferObjectGL(const Desc& desc);
        ~BufferObjectGL() override;

        void update(const void* data, size_t sizeBytes, size_t offset = 0) override;
        void* map(MapAccess access, size_t offset = 0, size_t sizeBytes = 0) override;
        void unmap() override;
        void read(void* dst, size_t sizeBytes, size_t offset = 0) override;

        size_t size() const override { return m_size; }
        Usage usage() const override { return m_usage; }
        Type bufferType() const override { return m_type; }
        bool isCpuReadable() const override { return m_cpuReadable; }
        bool isCpuWritable() const override { return m_cpuWritable; }
        bool isMapped() const override { return m_isMapped; }
        void* mappedPointer() const override { return m_mappedPtr; }

        uint32_t nativeHandle() const { return m_bufferId; }

    private:
        uint32_t m_bufferId = 0;
        size_t m_size = 0;
        Usage m_usage;
        Type m_type;
        bool m_cpuReadable = false;
        bool m_cpuWritable = true;
        bool m_isMapped = false;
        void* m_mappedPtr = nullptr;
    };

} // namespace PGRenderCore
