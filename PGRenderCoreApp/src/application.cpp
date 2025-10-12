#include "Application.h"
#include <PGRenderCoreFactory/deviceFactory.h>
#include <iostream>
#include <stdexcept>

Application::Application(const Config& config)
	: m_config(config)
{
}

Application::~Application() {
	cleanup();
}

int Application::run() {
	try {
		initialize();

		if (!onInit()) {
			std::cerr << "Application initialization failed" << std::endl;
			return 1;
		}

		m_initialized = true;
		m_running = true;
		m_startTime = SDL_GetTicks();
		m_lastFrameTime = m_startTime;

		// Loop principal
		while (m_running) {
			processEvents();
			update();
			render();
			calculateFPS();
		}

		return 0;

	}
	catch (const std::exception& ex) {
		std::cerr << "Fatal error: " << ex.what() << std::endl;
		return 1;
	}
}

void Application::quit() {
	m_running = false;
}

void Application::addWindow(std::shared_ptr<WindowSDL> window)
{
	if (!window) {
		throw std::runtime_error("Cannot add null window");
	}

	if (windows.find(window->getWindowId()) != windows.end()) {
		throw std::runtime_error("Window with the same ID already exists");
	}

	window->onInit();
	windows[window->getWindowId()] = window;
}

WindowSDL* Application::findWindow(uint32_t windowId)
{
	auto it = windows.find(windowId);
	if (it == windows.end()) {
		return nullptr;
	}

	return it->second.get();
}

void Application::initialize() {
	// ===== INICIALIZAR SDL3 =====

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
	}

	std::cout << "SDL3 Version: " << SDL_GetVersion() << std::endl;
}


void Application::cleanup() {
	if (m_initialized) {
		onCleanup();
		m_initialized = false;
	}

	for (auto& w : windows) {
		w.second->onCleanup();
		w.second->m_context.reset();
		w.second->m_device.reset();

		if (w.second->m_window) {
			SDL_DestroyWindow(w.second->m_window);
			w.second->m_window = nullptr;
		}
		w.second->isOpen = false;
	}
	windows.clear();
	SDL_Quit();
}



void Application::processEvents() {
	SDL_Event event;



	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_EVENT_QUIT:
			m_running = false;
			break;

		case SDL_EVENT_WINDOW_RESIZED:
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
		{
			auto* w = findWindow(event.window.windowID);
			if (w) {
				w->onResize(event.window.data1, event.window.data2);
			}
		}
		break;

		case SDL_EVENT_KEY_DOWN:
			if (event.key.key == SDLK_ESCAPE) {
				m_running = false;
			}
			break;

		default:
			break;
		}

		// Pasar evento a la clase derivada
		onEvent(event);
	}
}

void Application::update() {
	// Calcular delta time
	uint64_t currentTime = SDL_GetTicks();
	m_deltaTime = (currentTime - m_lastFrameTime) / 1000.0f;
	m_lastFrameTime = currentTime;
	m_totalTime = (currentTime - m_startTime) / 1000.0f;

	// Limitar delta time para evitar saltos grandes
	if (m_deltaTime > 0.1f) {
		m_deltaTime = 0.1f;
	}

	for (auto& w : windows) {
		w.second->onUpdate(m_deltaTime);
	}
}

void Application::render() {
	for (auto& wm : windows) {
		auto w = wm.second.get();
		if (w->isOpen) {
			w->m_context->makeCurrent();
			w->onRender();
			w->m_context->swapBuffers();
		}
	}
	m_frameCount++;
}

void Application::calculateFPS() {
	m_fpsUpdateTimer += m_deltaTime;

	if (m_fpsUpdateTimer >= 1.0f) {
		m_fps = static_cast<float>(m_frameCount) / m_fpsUpdateTimer;
		m_frameCount = 0;
		m_fpsUpdateTimer = 0.0f;
	}
}

WindowSDL::WindowSDL(const Desc& config)
{
	// ===== CONFIGURAR ATRIBUTOS DE OPENGL (si se usa) =====

	if (config.backend == PGRenderCore::RenderBackend::OpenGL4) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		if (config.debugMode) {
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
		}
	}

	// ===== CREAR VENTANA =====

	SDL_WindowFlags windowFlags = SDL_WINDOW_OPENGL;

	if (config.fullscreen) {
		windowFlags = static_cast<SDL_WindowFlags>(windowFlags | SDL_WINDOW_FULLSCREEN);
	}

	if (config.resizable) {
		windowFlags = static_cast<SDL_WindowFlags>(windowFlags | SDL_WINDOW_RESIZABLE);
	}

	m_window = SDL_CreateWindow(
		config.title.c_str(),
		config.windowWidth,
		config.windowHeight,
		windowFlags
	);

	if (!m_window) {
		throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
	}

	// Obtener tamaño real de la ventana (puede diferir del solicitado)
	int width, height;
	SDL_GetWindowSize(m_window, &width, &height);
	m_windowWidth = static_cast<uint32_t>(width);
	m_windowHeight = static_cast<uint32_t>(height);

	std::cout << "Window created: " << m_windowWidth << "x" << m_windowHeight << std::endl;

	// ===== OBTENER HANDLES NATIVOS =====

	SDL_PropertiesID props = SDL_GetWindowProperties(m_window);

	void* nativeWindowHandle = nullptr;
	void* nativeDisplayHandle = nullptr;

#if defined(SDL_PLATFORM_WIN32)
	nativeWindowHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);

#elif defined(SDL_PLATFORM_LINUX)
	// X11
	if (SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_WINDOW_POINTER, nullptr)) {
		nativeWindowHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_WINDOW_POINTER, nullptr);
		nativeDisplayHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
	}
	// Wayland
	else if (SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr)) {
		nativeWindowHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
		nativeDisplayHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
	}

#elif defined(SDL_PLATFORM_MACOS)
	nativeWindowHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);

#endif

	if (!nativeWindowHandle) {
		throw std::runtime_error("Failed to get native window handle");
	}

	if (config.debugMode) {
		int temp;
		if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &temp) == false ||
			(temp & SDL_GL_CONTEXT_DEBUG_FLAG) == 0)
			throw std::runtime_error("Failed to create a debug GL context");
	}

	// ===== CREAR DEVICE Y CONTEXTO DE RENDERIZADO =====

	PGRenderCore::DeviceDesc deviceDesc;
	deviceDesc.backend = config.backend;
	deviceDesc.enableDebug = config.debugMode;
	m_device = PGRenderCore::createDevice(deviceDesc);

	PGRenderCore::Context::Desc contextDesc;
	contextDesc.nativeWindowHandle = nativeWindowHandle;
	contextDesc.nativeDisplayHandle = nativeDisplayHandle;
	contextDesc.width = m_windowWidth;
	contextDesc.height = m_windowHeight;
	contextDesc.enableDebug = config.debugMode;
	contextDesc.enableVSync = config.vSync;
	m_context = m_device->createContext(contextDesc);

	isOpen = true;

}

uint32_t WindowSDL::getWindowId() const
{
	return SDL_GetWindowID(m_window);
}
