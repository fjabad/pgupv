#include "MyApplication.h"
#include <PGRenderCore/Shader.h>
#include <iostream>
#include <vector>
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

MyApplication::MyApplication(const Config& config)
    : Application(config)
{
}

MyApplication::~MyApplication() {
}

bool MyApplication::onInit() {
    std::cout << "Initializing MyApplication..." << std::endl;
    std::cout << "FPS will be displayed in console" << std::endl;
    std::cout << "Press SPACE to toggle wireframe" << std::endl;
    std::cout << "Press ESC to quit" << std::endl;

    WindowSDL::Desc window1Desc;
    window1Desc.debugMode = true;
    m_window1 = std::make_shared<MyWindow1>(window1Desc);
	addWindow(m_window1);


	WindowSDL::Desc window2Desc;
    window2Desc.windowWidth = window2Desc.windowHeight = 300;
	window2Desc.debugMode = true;
	m_window2 = std::make_shared<MyWindow2>(window1Desc);
	addWindow(m_window2);

    return true;
}

void MyApplication::onCleanup() {
    std::cout << "Cleaning up MyApplication..." << std::endl;
}

void MyApplication::onEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
        case SDLK_F:
            std::cout << "FPS: " << getFPS() << std::endl;
            break;

        default:
            break;
        }
    }
}

void MyWindow1::onUpdate(float deltaTime) {
    // Rotar triángulo
    m_rotationAngle += 45.0f * deltaTime; // 45 grados por segundo

    if (m_rotationAngle >= 360.0f) {
        m_rotationAngle -= 360.0f;
    }
}

void MyWindow1::onRender() {
    auto* context = getContext();

    // Comenzar render pass (clear automático)
    //PGRenderCore::RenderPassBeginInfo beginInfo;
    //beginInfo.renderAreaWidth = getWindowWidth();
    //beginInfo.renderAreaHeight = getWindowHeight();

    //context->beginRenderPass(m_renderPass, beginInfo);

    context->clear(PGRenderCore::ClearFlags::Color | PGRenderCore::ClearFlags::Depth, glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });
    // Vincular pipeline y dibujar
    context->bindPipeline(m_pipeline);
    context->bindVertexArray(m_triangleVAO);
    context->bindUniformBuffer(m_UBO, 2);

	auto world = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    m_UBO->update(&world[0][0], sizeof(glm::mat4));

    // TODO: Aquí establecerías uniforms con m_rotationAngle

	if (m_wireframe) {
		context->setPolygonMode(PGRenderCore::PolygonMode::Line);
	}
	else {
		context->setPolygonMode(PGRenderCore::PolygonMode::Fill);
	}

    context->draw(3); // 3 vértices = 1 triángulo

//    context->endRenderPass();
}


void MyWindow1::createRenderPass() {
    PGRenderCore::RenderPass::Desc rpDesc;

    rpDesc.clearColor = true;
	rpDesc.clearColorValue = { 0.1f, 0.1f, 0.15f, 1.0f };
    rpDesc.clearDepth = true;
    rpDesc.debugName = "MainRenderPass";

    m_renderPass = getContext()->createRenderPass(rpDesc);
}

void MyWindow1::onResize(uint32_t width, uint32_t height)
{
	auto* context = getContext();
    if (width == m_windowWidth && height == m_windowHeight) return;
	context->setViewport(0, 0, width, height);
    m_windowWidth = static_cast<uint32_t>(width);
    m_windowHeight = static_cast<uint32_t>(height);

    std::cout << "Window1 resized to " << width << "x" << height << std::endl;
}

void MyWindow1::onCleanup()
{
    m_triangleVAO.reset();
    m_pipeline.reset();
    m_renderPass.reset();
}

void MyWindow1::onKey(const PGRenderCore::KeyEvent& event)
{
    if (event.state == PGRenderCore::KeyState::Pressed) {
		if (event.keyCode == PGRenderCore::KeyCode::Space) {
            m_wireframe = !m_wireframe;
            std::cout << "Wireframe: " << (m_wireframe ? "ON" : "OFF") << std::endl;
            std::cout << "Space key pressed in Window1" << std::endl;
		}
	}
}

void MyWindow1::createResources() {
    auto* context = getContext();

    // ===== CREAR GEOMETRÍA DE TRIÁNGULO =====

    struct Vertex {
        float position[3];
        float color[3];
    };

    std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // Rojo
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Verde
        {{ 0.0f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}  // Azul
    };

    PGRenderCore::BufferObject::Desc bufferDesc;
    bufferDesc.type = PGRenderCore::BufferType::Vertex;
    bufferDesc.usage = PGRenderCore::BufferUsage::Static;
    bufferDesc.size = vertices.size() * sizeof(Vertex);
    bufferDesc.data = vertices.data();
    bufferDesc.debugName = "TriangleVertexBuffer";

    auto vertexBuffer = context->createBufferObject(bufferDesc);

    // ===== CREAR VERTEX ARRAY =====

    auto layout = PGRenderCore::VertexLayoutBuilder()
        .addBufferBinding(0, sizeof(Vertex), false)
        .addAttribute(0, PGRenderCore::VertexAttributeType::Float3, 0, 0)  // position
        .addAttribute(1, PGRenderCore::VertexAttributeType::Float3, 0, 12) // color
        .build();

    PGRenderCore::VertexArray::Desc vaoDesc;
    vaoDesc.layout = layout;
    vaoDesc.vertexBuffers.push_back(vertexBuffer);

    m_triangleVAO = context->createVertexArray(vaoDesc);

    // UBO

	PGRenderCore::BufferObject::Desc uboDesc;
	uboDesc.debugName = "TriangleUBO";
	uboDesc.type = PGRenderCore::BufferType::Uniform;
	uboDesc.usage = PGRenderCore::BufferUsage::Dynamic;
	uboDesc.size = sizeof(glm::mat4); // Matriz 4x4
	uboDesc.data = nullptr; // Sin datos iniciales

	m_UBO = context->createBufferObject(uboDesc);

    // ===== CREAR SHADERS =====

    PGRenderCore::Program::Desc shaderDesc;

    shaderDesc.stages.push_back({
        PGRenderCore::ShaderStage::Vertex,
        R"(
            #version 460 core
            layout(location = 0) in vec3 aPosition;
            layout(location = 1) in vec3 aColor;
            
            layout (std140, binding = 2) uniform MatrixBlock
            {
                mat4 modelMatrix;
            };
            out vec3 vColor;
            
            void main() {
                gl_Position = modelMatrix * vec4(aPosition, 1.0);
                vColor = aColor;
            }
        )"
        });

    shaderDesc.stages.push_back({
        PGRenderCore::ShaderStage::Fragment,
        R"(
            #version 460 core
            in vec3 vColor;
            out vec4 FragColor;
            
            void main() {
                FragColor = vec4(vColor, 1.0);
            }
        )"
        });

    auto shader = context->createProgram(shaderDesc);

    if (!shader->compile()) {
        throw std::runtime_error("Shader compilation failed");
    }

    // ===== CREAR PIPELINE =====

    PGRenderCore::Pipeline::Desc pipelineDesc;
    pipelineDesc.program = shader;
    pipelineDesc.cullMode = PGRenderCore::CullMode::None;
    pipelineDesc.depthState.depthTestEnabled = true;
    pipelineDesc.debugName = "TrianglePipeline";

    m_pipeline = context->createPipeline(pipelineDesc);
}

bool MyWindow1::onInit()
{
    try {
        createRenderPass();
        createResources();

        // Configurar viewport inicial
        getContext()->setViewport(0, 0, getWindowWidth(), getWindowHeight());

        return true;

    }
    catch (const std::exception& ex) {
        std::cerr << "Initialization failed: " << ex.what() << std::endl;
        return false;
    }

}

void MyWindow2::onRender()
{
    auto* context = getContext();
    context->clear(PGRenderCore::ClearFlags::Color, glm::vec4{ 0, 0, 1, 1 });
    //context->swapBuffers();
}

void MyWindow2::onResize(uint32_t width, uint32_t height)
{
    auto* context = getContext();
    if (width == m_windowWidth && height == m_windowHeight) return;
    context->setViewport(0, 0, width, height);
    m_windowWidth = static_cast<uint32_t>(width);
    m_windowHeight = static_cast<uint32_t>(height);

    std::cout << "Window2 resized to " << width << "x" << height << std::endl;
}
