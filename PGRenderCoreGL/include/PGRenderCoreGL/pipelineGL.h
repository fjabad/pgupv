#pragma once
#include <PGRenderCore/pipeline.h>
#include <cstdint>

namespace PGRenderCore {

    // Alias opaco para identificador OpenGL de programa shader
    using ProgramHandle = uint32_t;

    class PipelineGL : public Pipeline {
    public:
        explicit PipelineGL(const Desc& desc);
        ~PipelineGL() override;

        const Desc& getDesc() const override;
        uint64_t nativeHandle() const override;

    private:
        Desc m_desc;
        ProgramHandle m_programId;

        void applyBlendMode() const;
        void applyDepthFunc() const;
        void applyCullMode() const;
        void applyPolygonMode() const;

        void validateProgram() const;
    };
} // namespace PGRenderCore

