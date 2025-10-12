#pragma once
#include <PGRenderCore/Device.h>
#include <PGRenderCore/Context.h>

#include <SDL3/SDL.h>
#include <memory>
#include <string>
#include <cstdint>
#include <PGRenderCoreFactory/deviceFactory.h>


class WindowSDL {
public:
	struct Desc {
		std::string title = "PGRenderCore Window";
		uint32_t windowWidth = 1280;
		uint32_t windowHeight = 720;
		bool fullscreen = false;
		bool resizable = true;
		bool vSync = true;
		bool debugMode = false;
		PGRenderCore::RenderBackend backend = PGRenderCore::RenderBackend::OpenGL4;
	};

	/**
	 * @brief Constructor.
	 * @param config Configuración de la aplicación.
	 */
	explicit WindowSDL(const Desc& config = Desc());

	/**
	 * @brief Destructor virtual.
	 */
	virtual ~WindowSDL() = default;

	/**
	 * @brief Obtiene el contexto de renderizado.
	 */
	PGRenderCore::Context* getContext() const { return m_context.get(); }

	/**
	 * @brief Obtiene el device de renderizado.
	 */
	PGRenderCore::Device* getDevice() const { return m_device.get(); }

	/**
	 * @brief Obtiene la ventana SDL.
	 */
	SDL_Window* getWindow() const { return m_window; }

	/**
	 * @brief Obtiene el ancho actual de la ventana.
	 */
	uint32_t getWindowWidth() const { return m_windowWidth; }

	/**
	 * @brief Obtiene el alto actual de la ventana.
	 */
	uint32_t getWindowHeight() const { return m_windowHeight; }

	virtual bool onInit() { return true; }

	/**
 * @brief Callback de actualización lógica (override en clases derivadas).
 * Se llama una vez por frame con delta time.
 * @param deltaTime Tiempo transcurrido desde el último frame (segundos).
 */
	virtual void onUpdate(float deltaTime) {}

	/**
	 * @brief Callback de renderizado (override en clases derivadas).
	 * Se llama una vez por frame.
	 */
	virtual void onRender() {}

	/**
	 * @brief Callback de redimensionamiento de ventana.
	 * @param width Nuevo ancho.
	 * @param height Nuevo alto.
	 */
	virtual void onResize(uint32_t width, uint32_t height) = 0;

	virtual void onCleanup() {}
	virtual uint32_t getWindowId() const;
protected:
	// SDL
	SDL_Window* m_window = nullptr;
	uint32_t m_windowWidth = 0;
	uint32_t m_windowHeight = 0;

	// Desc
	Desc m_desc;

	// PGRenderCore
	std::unique_ptr<PGRenderCore::Device> m_device;
	std::unique_ptr<PGRenderCore::Context> m_context;

	bool isOpen = false;

	friend class Application;
};

/**
 * @brief Clase base para aplicaciones que usan PGRenderCore con SDL3.
 * Gestiona ventana, eventos, contexto de renderizado y loop principal.
 */
class Application {
public:
	/**
	 * @brief Configuración de la aplicación.
	 */
	struct Config {
		std::string title = "PGRenderCore Application";
	};

	/**
	 * @brief Constructor.
	 * @param config Configuración de la aplicación.
	 */
	explicit Application(const Config& config = Config());

	/**
	 * @brief Destructor virtual.
	 */
	virtual ~Application();

	/**
	 * @brief Ejecuta el loop principal de la aplicación.
	 * @return Código de salida (0 = éxito).
	 */
	int run();

	/**
	 * @brief Solicita el cierre de la aplicación.
	 */
	void quit();


	/**
	 * @brief Obtiene el tiempo transcurrido desde el último frame (segundos).
	 */
	float getDeltaTime() const { return m_deltaTime; }

	/**
	 * @brief Obtiene el tiempo total desde el inicio (segundos).
	 */
	float getTotalTime() const { return m_totalTime; }

	/**
	 * @brief Obtiene los FPS actuales.
	 */
	float getFPS() const { return m_fps; }

	void addWindow(std::shared_ptr<WindowSDL> window);

protected:
	/**
	 * @brief Callback de inicialización (override en clases derivadas).
	 * Se llama una vez después de crear ventana y contexto.
	 * @return true si la inicialización fue exitosa.
	 */
	virtual bool onInit() { return true; }

	/**
	 * @brief Callback de limpieza (override en clases derivadas).
	 * Se llama antes de destruir ventana y contexto.
	 */
	virtual void onCleanup() {}

	/**
	 * @brief Callback de evento SDL (override en clases derivadas).
	 * @param event Evento SDL a procesar.
	 */
	virtual void onEvent(const SDL_Event& event) {}

private:

	WindowSDL* findWindow(uint32_t windowId);

	void initialize();
	void cleanup();
	void processEvents();
	void update();
	void render();
	void calculateFPS();

	// Configuración
	Config m_config;

	std::unordered_map<uint32_t, std::shared_ptr<WindowSDL>> windows;

	// Estado de la aplicación
	bool m_running = false;
	bool m_initialized = false;

	// Timing
	uint64_t m_lastFrameTime = 0;
	uint64_t m_startTime = 0;
	float m_deltaTime = 0.0f;
	float m_totalTime = 0.0f;

	// FPS counter
	float m_fps = 0.0f;
	uint32_t m_frameCount = 0;
	float m_fpsUpdateTimer = 0.0f;
};
