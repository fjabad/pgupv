#pragma once
#include "Application.h"
#include <PGRenderCore/Pipeline.h>
#include <PGRenderCore/VertexArray.h>
#include <PGRenderCore/BufferObject.h>
#include <PGRenderCore/RenderPass.h>
#include <memory>


class MyWindow1 : public WindowSDL {
public:
	MyWindow1(WindowSDL::Desc desc) : WindowSDL(std::move(desc)) {}
	bool onInit() override;
    void onUpdate(float deltaTime) override;
    void onRender() override;
    void onResize(uint32_t width, uint32_t height) override;
	void onCleanup() override;
	void onKey(const PGRenderCore::KeyEvent& event) override;
private:
    void createResources();
    void createRenderPass();


    // Recursos de renderizado
    std::shared_ptr<PGRenderCore::RenderPass> m_renderPass;
    std::shared_ptr<PGRenderCore::Pipeline> m_pipeline;
    std::shared_ptr<PGRenderCore::VertexArray> m_triangleVAO;
    std::shared_ptr<PGRenderCore::BufferObject> m_UBO;


    // Estado de la aplicación
    float m_rotationAngle = 0.0f;
    bool m_wireframe = false;
};

class MyWindow2 : public WindowSDL {
public:
	MyWindow2(WindowSDL::Desc desc) : WindowSDL(std::move(desc)) {}
    void onRender() override;
    void onResize(uint32_t width, uint32_t height) override;

private:
};


class MyApplication : public Application {
public:
    explicit MyApplication(const Config& config = Config());
    ~MyApplication() override;

protected:
    bool onInit() override;
    void onCleanup() override;
    void onEvent(const SDL_Event& event) override;

private:
    std::shared_ptr<WindowSDL> m_window1, m_window2;
};

