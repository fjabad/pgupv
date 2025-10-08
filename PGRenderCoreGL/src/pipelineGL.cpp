#include "PGRenderCoreGL/pipelineGL.h"
#include "PGRenderCoreGL/shaderGL.h"

#include <GL/glew.h>
#include <stdexcept>
#include <iostream>

namespace PGRenderCore {

    PipelineGL::PipelineGL(const Desc& desc)
        : m_desc(desc), m_programId(0)
    {
        if (!m_desc.vertexShader || !m_desc.fragmentShader) {
            throw std::invalid_argument("Vertex and Fragment shaders are required");
        }

        m_programId = static_cast<ProgramHandle>(m_desc.vertexShader->nativeHandle());
        // En esta implementación simplificada asumimos que vertexShader y fragmentShader comparten el mismo ID
        // En un pipeline real, aquí se debería compilar y linkar el programa con ambos shaders

        if (m_programId == 0) {
            throw std::runtime_error("Invalid shader program ID");
        }

        applyBlendMode();
        applyDepthFunc();
        applyCullMode();
    }

    PipelineGL::~PipelineGL() {
        // No se elimina el programa aquí, pues se asume gestión externa por ShaderGL
    }

    void PipelineGL::applyBlendMode() const {
        switch (m_desc.blendMode) {
        case BlendMode::None:
            glDisable(GL_BLEND);
            break;
        case BlendMode::AlphaBlend:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BlendMode::Additive:
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        case BlendMode::Multiply:
            glEnable(GL_BLEND);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            break;
        case BlendMode::Custom:
            // Aquí se podría implementar blend personalizado
            glDisable(GL_BLEND);
            break;
        }
    }

    void PipelineGL::applyDepthFunc() const {
        static const GLenum glFuncs[] = {
            GL_NEVER, GL_LESS, GL_LEQUAL, GL_GREATER,
            GL_GEQUAL, GL_EQUAL, GL_NOTEQUAL, GL_ALWAYS
        };
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(glFuncs[static_cast<int>(m_desc.depthFunc)]);
    }

    void PipelineGL::applyCullMode() const {
        if (m_desc.cullMode == CullMode::None) {
            glDisable(GL_CULL_FACE);
        }
        else {
            glEnable(GL_CULL_FACE);
            glCullFace(m_desc.cullMode == CullMode::Back ? GL_BACK : GL_FRONT);
        }
    }

    void PipelineGL::applyPolygonMode() const {
        auto toGLMode = [](PolygonMode mode) -> GLenum {
            switch (mode) {
            case PolygonMode::Fill: return GL_FILL;
            case PolygonMode::Line: return GL_LINE;
            case PolygonMode::Point: return GL_POINT;
            default: return GL_FILL;
            }
        };
        glPolygonMode(GL_FRONT, toGLMode(m_desc.polygonMode.front));
        glPolygonMode(GL_BACK, toGLMode(m_desc.polygonMode.back));
    }

    uint64_t PipelineGL::nativeHandle() const {
        return m_programId;
    }

    const Pipeline::Desc& PipelineGL::getDesc() const {
        return m_desc;
    }

} // namespace PGRenderCore

