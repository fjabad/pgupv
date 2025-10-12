#pragma once

#include <PGRenderCore/Context.h>
#include <PGRenderCore/BufferObject.h>
#include <PGRenderCore/Shader.h>
#include <PGRenderCore/Pipeline.h>
#include <memory>

class Renderer {
public:
    explicit Renderer(PGRenderCore::Context* context);
    ~Renderer();

    void render();

private:
    void setupResources();

    PGRenderCore::Context* m_context;
    std::shared_ptr<PGRenderCore::BufferObject> m_vertexBuffer;
    std::shared_ptr<PGRenderCore::BufferObject> m_indexBuffer;
	std::shared_ptr<PGRenderCore::VertexArray> m_vertexArray;
    std::shared_ptr<PGRenderCore::Shader> m_shader;
    std::shared_ptr<PGRenderCore::Pipeline> m_pipeline;
};
