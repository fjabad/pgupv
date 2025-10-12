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

PGRenderCore::KeyCode translateKeyCode(SDL_Keycode sdlKey) {
	switch (sdlKey) {
	case SDLK_UNKNOWN: return PGRenderCore::KeyCode::Unknown;
	case SDLK_A: return PGRenderCore::KeyCode::A;
	case SDLK_B: return PGRenderCore::KeyCode::B;
	case SDLK_C: return PGRenderCore::KeyCode::C;
	case SDLK_D: return PGRenderCore::KeyCode::D;
	case SDLK_E: return PGRenderCore::KeyCode::E;
	case SDLK_F: return PGRenderCore::KeyCode::F;
	case SDLK_G: return PGRenderCore::KeyCode::G;
	case SDLK_H: return PGRenderCore::KeyCode::H;
	case SDLK_I: return PGRenderCore::KeyCode::I;
	case SDLK_J: return PGRenderCore::KeyCode::J;
	case SDLK_K: return PGRenderCore::KeyCode::K;
	case SDLK_L: return PGRenderCore::KeyCode::L;
	case SDLK_M: return PGRenderCore::KeyCode::M;
	case SDLK_N: return PGRenderCore::KeyCode::N;
	case SDLK_O: return PGRenderCore::KeyCode::O;
	case SDLK_P: return PGRenderCore::KeyCode::P;
	case SDLK_Q: return PGRenderCore::KeyCode::Q;
	case SDLK_R: return PGRenderCore::KeyCode::R;
	case SDLK_S: return PGRenderCore::KeyCode::S;
	case SDLK_T: return PGRenderCore::KeyCode::T;
	case SDLK_U: return PGRenderCore::KeyCode::U;
	case SDLK_V: return PGRenderCore::KeyCode::V;
	case SDLK_W: return PGRenderCore::KeyCode::W;
	case SDLK_X: return PGRenderCore::KeyCode::X;
	case SDLK_Y: return PGRenderCore::KeyCode::Y;
	case SDLK_Z: return PGRenderCore::KeyCode::Z;

	case SDLK_0: return PGRenderCore::KeyCode::Num0;
	case SDLK_1: return PGRenderCore::KeyCode::Num1;
	case SDLK_2: return PGRenderCore::KeyCode::Num2;
	case SDLK_3: return PGRenderCore::KeyCode::Num3;
	case SDLK_4: return PGRenderCore::KeyCode::Num4;
	case SDLK_5: return PGRenderCore::KeyCode::Num5;
	case SDLK_6: return PGRenderCore::KeyCode::Num6;
	case SDLK_7: return PGRenderCore::KeyCode::Num7;
	case SDLK_8: return PGRenderCore::KeyCode::Num8;
	case SDLK_9: return PGRenderCore::KeyCode::Num9;
	case SDLK_ESCAPE: return PGRenderCore::KeyCode::Escape;
	case SDLK_RETURN: return PGRenderCore::KeyCode::Return;
	case SDLK_TAB: return PGRenderCore::KeyCode::Tab;
	case SDLK_BACKSPACE: return PGRenderCore::KeyCode::Backspace;
	case SDLK_INSERT: return PGRenderCore::KeyCode::Insert;
	case SDLK_DELETE: return PGRenderCore::KeyCode::Delete;
	case SDLK_RIGHT: return PGRenderCore::KeyCode::Right;
	case SDLK_LEFT: return PGRenderCore::KeyCode::Left;
	case SDLK_DOWN: return PGRenderCore::KeyCode::Down;
	case SDLK_UP: return PGRenderCore::KeyCode::Up;
	case SDLK_PAGEUP: return PGRenderCore::KeyCode::PageUp;
	case SDLK_PAGEDOWN: return PGRenderCore::KeyCode::PageDown;
	case SDLK_HOME: return PGRenderCore::KeyCode::Home;
	case SDLK_END: return PGRenderCore::KeyCode::End;
	case SDLK_CAPSLOCK: return PGRenderCore::KeyCode::CapsLock;
	case SDLK_SCROLLLOCK: return PGRenderCore::KeyCode::ScrollLock;
	case SDLK_NUMLOCKCLEAR: return PGRenderCore::KeyCode::NumLock;
	case SDLK_PRINTSCREEN: return PGRenderCore::KeyCode::PrintScreen;
	case SDLK_PAUSE: return PGRenderCore::KeyCode::Pause;
	case SDLK_F1: return PGRenderCore::KeyCode::F1;
	case SDLK_F2: return PGRenderCore::KeyCode::F2;
	case SDLK_F3: return PGRenderCore::KeyCode::F3;
	case SDLK_F4: return PGRenderCore::KeyCode::F4;
	case SDLK_F5: return PGRenderCore::KeyCode::F5;
	case SDLK_F6: return PGRenderCore::KeyCode::F6;
	case SDLK_F7: return PGRenderCore::KeyCode::F7;
	case SDLK_F8: return PGRenderCore::KeyCode::F8;
	case SDLK_F9: return PGRenderCore::KeyCode::F9;
	case SDLK_F10: return PGRenderCore::KeyCode::F10;
	case SDLK_F11: return PGRenderCore::KeyCode::F11;
	case SDLK_F12: return PGRenderCore::KeyCode::F12;
	case SDLK_LCTRL: return PGRenderCore::KeyCode::LeftControl;
	case SDLK_LSHIFT: return PGRenderCore::KeyCode::LeftShift;
	case SDLK_LALT: return PGRenderCore::KeyCode::LeftAlt;
	case SDLK_LGUI: return PGRenderCore::KeyCode::LeftSuper;
	case SDLK_RCTRL: return PGRenderCore::KeyCode::RightControl;
	case SDLK_RSHIFT: return PGRenderCore::KeyCode::RightShift;
	case SDLK_RALT: return PGRenderCore::KeyCode::RightAlt;
	case SDLK_RGUI: return PGRenderCore::KeyCode::RightSuper;
	case SDLK_SPACE: return PGRenderCore::KeyCode::Space;
	default:
		return PGRenderCore::KeyCode::Unknown;
	}
}

PGRenderCore::KeyState translateKeyState(uint32_t sdlEventType) {
	switch (sdlEventType) {
	case SDL_EVENT_KEY_DOWN:
		return PGRenderCore::KeyState::Pressed;
	case SDL_EVENT_KEY_UP:
		return PGRenderCore::KeyState::Released;
	default:
		return PGRenderCore::KeyState::Unknown;
	}
}

PGRenderCore::KeyEvent translateEvent(const SDL_Event& sdlEvent) {
	PGRenderCore::KeyEvent keyEvent;
	keyEvent.keyCode = translateKeyCode(sdlEvent.key.key);
	

	if (sdlEvent.key.mod & SDL_KMOD_SHIFT) keyEvent.modifiers |= PGRenderCore::KeyModifier::Shift;
	if (sdlEvent.key.mod & SDL_KMOD_CTRL) keyEvent.modifiers |= PGRenderCore::KeyModifier::Control;
	if (sdlEvent.key.mod & SDL_KMOD_ALT) keyEvent.modifiers |= PGRenderCore::KeyModifier::Alt;
	if (sdlEvent.key.mod & SDL_KMOD_GUI) keyEvent.modifiers |= PGRenderCore::KeyModifier::Super;

	keyEvent.state = translateKeyState(sdlEvent.type);
	return keyEvent;
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
			switch (event.key.key) {
			case SDLK_ESCAPE:
				m_running = false;
				break;
			case SDLK_F:
				std::cout << "FPS: " << getFPS() << "  " << event.key.windowID << std::endl;
				break;
			default:
				auto w = findWindow(event.key.windowID);
				w->onKey(translateEvent(event));
				break;
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
