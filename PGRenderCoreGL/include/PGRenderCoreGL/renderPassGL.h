#pragma once
#include <PGRenderCore/renderPass.h>
#include <cstdint>

namespace PGRenderCore {

    class RenderPassGL : public RenderPass {
    public:
        explicit RenderPassGL(const RenderPassDesc& desc);
        ~RenderPassGL() override;

        const RenderPassDesc& getDesc() const override;
        uint64_t nativeHandle() const override;

        void begin() override;
        void end() override;

    private:
        RenderPassDesc m_desc;
        bool m_isActive = false;
    };

} // namespace PGRenderCore
