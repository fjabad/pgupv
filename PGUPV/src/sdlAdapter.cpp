//
//  sdlAdaptor.cpp
//  PG
//
//  Created by Paco Abad on 19/9/15.
//
//

#include <SDL3/SDL.h>

#include <algorithm>
#include <sstream>
#include <string.h>
#include <gsl/gsl>

#include "log.h"
#include "HW.h"
#include "window.h"
#include "app.h"
#include "guipg.h"

using PGUPV::HW;

using PGUPV::GamepadHW;
using PGUPV::MouseMotionEvent;
using PGUPV::MouseButtonEvent;
using PGUPV::MouseWheelEvent;
using PGUPV::KeyboardEvent;
using PGUPV::JoystickMotionEvent;
using PGUPV::JoystickHatMotionEvent;
using PGUPV::JoystickButtonEvent;
using PGUPV::WindowResizedEvent;
using PGUPV::WindowClosedEvent;
using PGUPV::TextInputEvent;

using PGUPV::DialogType;
using PGUPV::Event;
using PGUPV::EventSourceHW;
using PGUPV::EventType;
using PGUPV::KeyCode;
using PGUPV::Window;
using PGUPV::WindowHW;


std::map<Uint32, Window*> WindowHW::windowIdToWindow;

// Values under EPSILON are considered 0
#define JOYSTICK_EPSILON 500
#define JOYSTICK_MAXIMUM 32767
#define JOYSTICK_ABS_MINIMUM 32768

struct SDLKeyCodeToPGUPVKeyCode {
	SDL_Keycode from;
	KeyCode to;
};

static SDLKeyCodeToPGUPVKeyCode translationTable[]{
  {SDLK_UNKNOWN, KeyCode::Unknown},
  { SDLK_RETURN, KeyCode::Return },
  { SDLK_ESCAPE, KeyCode::Escape },
  { SDLK_BACKSPACE, KeyCode::BackSpace },
  { SDLK_TAB, KeyCode::Tab },
  { SDLK_SPACE, KeyCode::Space },
  { SDLK_EXCLAIM, KeyCode::Exclamation },
  { SDLK_DBLAPOSTROPHE, KeyCode::DblQuotes },
  { SDLK_HASH, KeyCode::Hash },
  { SDLK_PERCENT, KeyCode::Percent },
  { SDLK_DOLLAR, KeyCode::Dollar },
  { SDLK_AMPERSAND, KeyCode::Ampersand },
  { SDLK_APOSTROPHE, KeyCode::Quote },
  { SDLK_LEFTPAREN, KeyCode::LeftParen },
  { SDLK_RIGHTPAREN, KeyCode::RightParen },
  { SDLK_ASTERISK, KeyCode::Asterisk },
  { SDLK_PLUS, KeyCode::Plus },
  { SDLK_COMMA, KeyCode::Comma },
  { SDLK_MINUS, KeyCode::Minus },
  { SDLK_PERIOD, KeyCode::Period },
  { SDLK_SLASH, KeyCode::Slash },
  { SDLK_0, KeyCode::Alpha0 },
  { SDLK_1, KeyCode::Alpha1 },
  { SDLK_2, KeyCode::Alpha2 },
  { SDLK_3, KeyCode::Alpha3 },
  { SDLK_4, KeyCode::Alpha4 },
  { SDLK_5, KeyCode::Alpha5 },
  { SDLK_6, KeyCode::Alpha6 },
  { SDLK_7, KeyCode::Alpha7 },
  { SDLK_8, KeyCode::Alpha8 },
  { SDLK_9, KeyCode::Alpha9 },
  { SDLK_COLON, KeyCode::Colon },
  { SDLK_SEMICOLON, KeyCode::Semicolon },
  { SDLK_LESS, KeyCode::Less },
  { SDLK_EQUALS, KeyCode::Equals },
  { SDLK_GREATER, KeyCode::Greater },
  { SDLK_QUESTION, KeyCode::Question },
  { SDLK_AT, KeyCode::At },

  { SDLK_LEFTBRACKET, KeyCode::LeftBracket },
  { SDLK_BACKSLASH, KeyCode::BackSlash },
  { SDLK_RIGHTBRACKET, KeyCode::RightBracket },
  { SDLK_CARET, KeyCode::Caret },
  { SDLK_UNDERSCORE, KeyCode::Underscore },
  { SDLK_GRAVE, KeyCode::BackQuote },
  { SDLK_A, KeyCode::A },
  { SDLK_B, KeyCode::B },
  { SDLK_C, KeyCode::C },
  { SDLK_D, KeyCode::D },
  { SDLK_E, KeyCode::E },
  { SDLK_F, KeyCode::F },
  { SDLK_G, KeyCode::G },
  { SDLK_H, KeyCode::H },
  { SDLK_I, KeyCode::I },
  { SDLK_J, KeyCode::J },
  { SDLK_K, KeyCode::K },
  { SDLK_L, KeyCode::L },
  { SDLK_M, KeyCode::M },
  { SDLK_N, KeyCode::N },
  { SDLK_O, KeyCode::O },
  { SDLK_P, KeyCode::P },
  { SDLK_Q, KeyCode::Q },
  { SDLK_R, KeyCode::R },
  { SDLK_S, KeyCode::S },
  { SDLK_T, KeyCode::T },
  { SDLK_U, KeyCode::U },
  { SDLK_V, KeyCode::V },
  { SDLK_W, KeyCode::W },
  { SDLK_X, KeyCode::X },
  { SDLK_Y, KeyCode::Y },
  { SDLK_Z, KeyCode::Z },

  { SDLK_CAPSLOCK, KeyCode::CapsLock },
  { SDLK_F1, KeyCode::F1 },
  { SDLK_F2, KeyCode::F2 },
  { SDLK_F3, KeyCode::F3 },
  { SDLK_F4, KeyCode::F4 },
  { SDLK_F5, KeyCode::F5 },
  { SDLK_F6, KeyCode::F6 },
  { SDLK_F7, KeyCode::F7 },
  { SDLK_F8, KeyCode::F8 },
  { SDLK_F9, KeyCode::F9 },
  { SDLK_F10, KeyCode::F10 },
  { SDLK_F11, KeyCode::F11 },
  { SDLK_F12, KeyCode::F12 },

  { SDLK_PRINTSCREEN, KeyCode::PrintScreen },
  { SDLK_SCROLLLOCK, KeyCode::ScrollLock },
  { SDLK_PAUSE, KeyCode::Pause },
  { SDLK_INSERT, KeyCode::Insert },
  { SDLK_HOME, KeyCode::Home },
  { SDLK_PAGEUP, KeyCode::PageUp },
  { SDLK_DELETE, KeyCode::Delete },
  { SDLK_END, KeyCode::End },
  { SDLK_PAGEDOWN, KeyCode::PageDown },
  { SDLK_RIGHT, KeyCode::Right },
  { SDLK_LEFT, KeyCode::Left },
  { SDLK_DOWN, KeyCode::Down },
  { SDLK_UP, KeyCode::Up },

  { SDLK_LCTRL, KeyCode::LeftControl },
  { SDLK_RCTRL, KeyCode::RightControl },

  { SDLK_LSHIFT, KeyCode::LeftShift },
  { SDLK_RSHIFT, KeyCode::RightShift },

  { SDLK_LALT, KeyCode::LeftAlt },
  { SDLK_RALT, KeyCode::RightAlt },

  // Keypad 
{ SDLK_NUMLOCKCLEAR, KeyCode::NumLock },
{  SDLK_KP_DIVIDE, KeyCode::KeypadSlash },
{  SDLK_KP_MULTIPLY, KeyCode::KeypadAsterisk },
{  SDLK_KP_MINUS, KeyCode::KeypadMinus },
{  SDLK_KP_PLUS, KeyCode::KeypadPlus },
{  SDLK_KP_ENTER, KeyCode::KeypadEnter },
{  SDLK_KP_1, KeyCode::Keypad1 },
{  SDLK_KP_2, KeyCode::Keypad2 },
{  SDLK_KP_3, KeyCode::Keypad3 },
{  SDLK_KP_4, KeyCode::Keypad4 },
{  SDLK_KP_5, KeyCode::Keypad5 },
{  SDLK_KP_6, KeyCode::Keypad6 },
{  SDLK_KP_7, KeyCode::Keypad7 },
{  SDLK_KP_8, KeyCode::Keypad8 },
{  SDLK_KP_9, KeyCode::Keypad9 },
{  SDLK_KP_0, KeyCode::Keypad0 },
{  SDLK_KP_PERIOD, KeyCode::KeypadPeriod },
};


HW::HW() {
	// Inicializar SDL
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK))
		ERRT("Unable to initialize SDL");

	// Ordenar la tabla de traducción de códigos de tecla para poder usar luego una búsqueda binaria
	const size_t sizeTranslationElem = sizeof(translationTable[0]);
	std::qsort(translationTable, (sizeof(translationTable) / sizeTranslationElem), sizeTranslationElem, [](const void* a, const void* b)
		{
			const SDLKeyCodeToPGUPVKeyCode* arg1 = static_cast<const SDLKeyCodeToPGUPVKeyCode*>(a);
			const SDLKeyCodeToPGUPVKeyCode* arg2 = static_cast<const SDLKeyCodeToPGUPVKeyCode*>(b);

			if (arg1->from < arg2->from) return -1;
			if (arg1->from > arg2->from) return 1;
			return 0;
		});
}

std::string HW::getLibVersionString() {

	auto compiled = SDL_VERSION;
	auto current = SDL_GetVersion();

	std::string version = "SDL: Runtime: " +
		std::to_string(current) +
		" Compiled: " +
		std::to_string(compiled);
	return version;
}

HW::~HW() {

}

void HW::moveMousePointerInWindow(float x, float y) {
	SDL_WarpMouseInWindow(nullptr, x, y);
}

#if SDL_VERSION_ATLEAST(2, 0, 4)
void HW::moveMousePointerTo(float x, float y) {
	SDL_WarpMouseGlobal(x, y);
}
#endif

GamepadHW::GamepadHW(size_t gamepad) {
	gameController = SDL_OpenJoystick(gsl::narrow<int>(gamepad));
	if (gameController == nullptr) {
		ERRT(std::string("No se ha podido usar el joystick. Error SDL: ") + SDL_GetError());
	}
	else {
		INFO(std::string("Joystick (") + getName() + "): " +
			std::to_string(getNumAxes()) + " ejes, " +
			std::to_string(getNumButtons()) + " botones, " +
			std::to_string(getNumHats()) + " hats, " +
			std::to_string(getNumBalls()) + " trackballs");
	}
}

GamepadHW::~GamepadHW() {
	if (gameController) {
		INFO(std::string("Destruyendo el joystick ") + SDL_GetJoystickName(gameController));
		SDL_CloseJoystick(gameController);
	}
}

std::string GamepadHW::getName() const {
	return std::string(SDL_GetJoystickName(gameController));
}

size_t GamepadHW::getNumAxes() const {
	return SDL_GetNumJoystickAxes(gameController);
}

size_t GamepadHW::getNumButtons() const {
	return SDL_GetNumJoystickButtons(gameController);
}

size_t GamepadHW::getNumHats() const {
	return SDL_GetNumJoystickHats(gameController);
}

size_t GamepadHW::getNumBalls() const {
	return SDL_GetNumJoystickBalls(gameController);
}

size_t HW::getNumGamepads() {
	int count;
	SDL_GetJoysticks(&count);
	return count;
}

uint64_t HW::currentMillis() {
	return SDL_GetTicks();
}

void HW::sleep(uint ms) {
	SDL_Delay(ms);
}

void HW::terminate(void) {
	SDL_Quit();
}


int HW::showMessageBox(DialogType type, const std::string& title, const std::string& body) {
	Uint32 sdlType;
	switch (type) {
	case DialogType::DLG_WARNING:
		sdlType = SDL_MESSAGEBOX_WARNING;
		break;
	case DialogType::DLG_INFORMATION:
		sdlType = SDL_MESSAGEBOX_INFORMATION;
		break;
	default:
		sdlType = SDL_MESSAGEBOX_ERROR;
		break;

	}
	int buttonid;
	SDL_MessageBoxData messageBoxData;
	messageBoxData.flags = sdlType;
	messageBoxData.title = title.c_str();
	messageBoxData.message = body.c_str();

	static const SDL_MessageBoxButtonData btnErr[] = {
	  { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "Depurar" },
	  { 0, 100, "Ver log" },
	  { 0, 101, "Terminar" }
	};

	static const SDL_MessageBoxButtonData btn[] = {
	  { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "Cerrar" }
	};

	if (type == DialogType::DLG_ERROR) {
		messageBoxData.buttons = btnErr;
		messageBoxData.numbuttons = sizeof(btnErr) / sizeof(btnErr[0]);
	}
	else {
		messageBoxData.buttons = btn;
		messageBoxData.numbuttons = sizeof(btn) / sizeof(btn[0]);
	}

	messageBoxData.colorScheme = nullptr;
	messageBoxData.window = nullptr;

	SDL_ShowMessageBox(&messageBoxData, &buttonid);
	return buttonid;
	// SDL_ShowSimpleMessageBox(sdlType, title.c_str(), body.c_str(), nullptr);
}



static std::string decipherSDLWindowFlags(Uint64 flags) {
	std::ostringstream os;

#define FLAG(F) if (flags & F) os << #F " ";
	FLAG(SDL_WINDOW_FULLSCREEN)
	FLAG(SDL_WINDOW_OPENGL)
	FLAG(SDL_WINDOW_OCCLUDED)
	FLAG(SDL_WINDOW_HIDDEN)
	FLAG(SDL_WINDOW_BORDERLESS)
	FLAG(SDL_WINDOW_RESIZABLE)
	FLAG(SDL_WINDOW_MINIMIZED)
	FLAG(SDL_WINDOW_MAXIMIZED)
	FLAG(SDL_WINDOW_MOUSE_GRABBED)
	FLAG(SDL_WINDOW_INPUT_FOCUS)
	FLAG(SDL_WINDOW_MOUSE_FOCUS)
	FLAG(SDL_WINDOW_EXTERNAL)
	FLAG(SDL_WINDOW_MODAL)
	FLAG(SDL_WINDOW_HIGH_PIXEL_DENSITY)
	FLAG(SDL_WINDOW_MOUSE_CAPTURE)
	FLAG(SDL_WINDOW_MOUSE_RELATIVE_MODE)
	FLAG(SDL_WINDOW_ALWAYS_ON_TOP)
	FLAG(SDL_WINDOW_UTILITY)
	FLAG(SDL_WINDOW_TOOLTIP)
	FLAG(SDL_WINDOW_POPUP_MENU)
	FLAG(SDL_WINDOW_KEYBOARD_GRABBED)
	FLAG(SDL_WINDOW_VULKAN)
	FLAG(SDL_WINDOW_METAL)
	FLAG(SDL_WINDOW_TRANSPARENT)
	FLAG(SDL_WINDOW_NOT_FOCUSABLE)
	return os.str();
}

static std::string decipherSDLGLContextFlags(int flags) {
	std::ostringstream os;

	if (flags & SDL_GL_CONTEXT_DEBUG_FLAG)
		os << "SDL_GL_CONTEXT_DEBUG_FLAG ";
	if (flags & SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG)
		os << "SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG ";
	if (flags & SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG)
		os << "SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG ";
	if (flags & SDL_GL_CONTEXT_RESET_ISOLATION_FLAG)
		os << "SDL_GL_CONTEXT_RESET_ISOLATION_FLAG ";
	return os.str();
}


static std::string decipherWindowProperties(std::shared_ptr<WindowHW> window) {
	std::ostringstream os;

	if (window->hasDoubleBuffer()) os << "DOUBLE_BUFFER ";
	if (window->hasStereo()) os << "STEREO ";
	return os.str();
}



std::shared_ptr<WindowHW> WindowHW::createWindow(Window* container, const std::string& title, uint flags, uint posx,
	uint posy, uint width, uint height, uint glMajor, uint glMinor, bool glCompatibility) {
	// Request the opengl context. SDL doesn't have the ability to choose which
	// profile at this time of writing, but it should default to the core profile

	auto window = std::shared_ptr<WindowHW>(new WindowHW());
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glMajor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glMinor);
	if (glCompatibility) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	}
	else {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE);
	}

	window->reqGLVersion = "GL_VERSION_" + std::to_string(glMajor) + "_" + std::to_string(glMinor);

	/* Turn on double buffering with a 24bit Z buffer.
	* You may need to change this to 16 or 32 for your system */
	if (flags & DOUBLE_BUFFER)
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if (flags & DEPTH_BUFFER)
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	else
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
	if (flags & STENCIL_BUFFER)
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	if (flags & MULTISAMPLE) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	}
	if (flags & RGBA)
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	if (flags & STEREO) {
		SDL_GL_SetAttribute(SDL_GL_STEREO, 1);
		flags |= FULLSCREEN;
	}
	// We always request a debug context when compiling in Debug. When compiling in
	// Release, you have to explicitly request it
#if _DEBUG
	if (glMajor > 4 || (glMajor == 4 && glMinor >= 3))
		flags |= DEBUG;
#endif
	if (flags & DEBUG)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	window->_mainwindow = SDL_CreateWindow(title.c_str(), width, height,
		SDL_WINDOW_OPENGL | ((flags & FULLSCREEN) ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_RESIZABLE));
	if (!(window->_mainwindow)) {
		ERRT("Unable to create window for " + window->reqGLVersion + ". SDL Error: " +
			SDL_GetError());
	}

	SDL_SetWindowPosition(window->_mainwindow, posx, posy);

	/* Create our opengl context and attach it to our window */
	window->_maincontext = SDL_GL_CreateContext(window->_mainwindow);
	if (!window->_maincontext) {
		SDL_DestroyWindow(window->_mainwindow);
		window->_mainwindow = nullptr;
		ERRT("Error creando contexto " + window->reqGLVersion + ". SDL Error: " +
			SDL_GetError() +
			"Selecciona 'Depurar' para intentar obtener una versión de OpenGL menor"
			"\nCon App::setPrefGLVersion puedes pedir la versión que se pedirá por defecto");
	}

	// Obtain the real capabitilies that we got
	int temp;
	if (!SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &temp))
		window->gotDoubleBuffer = (temp != 0);

	if (!SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &temp))
		window->gotDebugContext = (temp & SDL_GL_CONTEXT_DEBUG_FLAG);

	if (!SDL_GL_GetAttribute(SDL_GL_STEREO, &temp))
		window->gotStereo = (temp != 0);

	if (flags & FULLSCREEN)
		window->setFullScreen(true);

	INFO("Window Properties: " + decipherWindowProperties(window));
	auto got = SDL_GetWindowFlags(window->_mainwindow);
	INFO("SDL Window flags: " + decipherSDLWindowFlags(got));
	int contextFlags;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &contextFlags);
	INFO("SDL Context flags: " + decipherSDLGLContextFlags(contextFlags));

	windowIdToWindow[SDL_GetWindowID(window->_mainwindow)] = container;

	PGUPV::GUILib::initGUI(window->_mainwindow, &(window->_maincontext), "#version 410");
	if (App::getInstance().isForgetGUIState())
		PGUPV::GUILib::forgetState();

	return window;
}

void WindowHW::swapBuffers() {
	SDL_GL_SwapWindow(_mainwindow);
}

void WindowHW::setFullScreen(bool fs) {
	if (fs == _fullscreen)
		return;
	if (fs) {
		SDL_SetWindowFullscreen(_mainwindow, SDL_WINDOW_FULLSCREEN);
	}
	else {
		SDL_SetWindowFullscreen(_mainwindow, 0);
	}
	_fullscreen = fs;
}


WindowHW::~WindowHW() {

	windowIdToWindow.erase(SDL_GetWindowID(_mainwindow));

	/* Delete our opengl context, destroy our window, and shutdown SDL */
	if (_maincontext) {
		GUILib::shutdown();
		SDL_GL_DestroyContext(_maincontext);
		_maincontext = nullptr;
	}
	if (_mainwindow) {
		SDL_DestroyWindow(_mainwindow);
		_mainwindow = nullptr;
	}
}


void WindowHW::captureMouse() {
	if (!SDL_SetWindowRelativeMouseMode(_mainwindow, true)) {
		ERR("Capturing the mouse is not supported");
	}
}
void WindowHW::releaseMouse() {
	SDL_SetWindowRelativeMouseMode(_mainwindow, false);
}

void WindowHW::setMousePosition(float x, float y) {
	SDL_WarpMouseInWindow(_mainwindow, x, y);
}

void WindowHW::showMouseCursor(bool show) {
	if (show)
		SDL_ShowCursor();
	else
		SDL_HideCursor();
}

void WindowHW::setTitle(const std::string& title) {
	SDL_SetWindowTitle(_mainwindow, title.c_str());
}

void WindowHW::getSize(uint& w, uint& h) {
	int width, height;
	SDL_GetWindowSize(_mainwindow, &width, &height);
	assert(width > 0 && height > 0);
	w = width;
	h = height;
}


Window* WindowHW::getWindowFromId(Uint32 id) {
	return windowIdToWindow[id];
}

void WindowHW::initGUIRender() {
	GUILib::startFrame(_mainwindow);
}

void WindowHW::maximize() {
	SDL_MaximizeWindow(_mainwindow);
}

void WindowHW::minimize() {
	SDL_MinimizeWindow(_mainwindow);
}

void WindowHW::getWindowPos(int& x, int& y) {
	SDL_GetWindowPosition(_mainwindow, &x, &y);
}

void WindowHW::restore() {
	SDL_RestoreWindow(_mainwindow);
}


bool translateKeyboardEvent(const SDL_Event& in, KeyboardEvent& out) {

	// Ignoramos la repetición (sólo nos interesan los eventos de pulsación y liberación)
	if (in.key.repeat) return false;

	auto w = WindowHW::getWindowFromId(in.key.windowID);
	if (w == nullptr) return false;

	out.wsrc = w;
	out.type = EventType::KeyboardEvent;
	out.state = (in.key.down ?
		PGUPV::ButtonState::Pressed :
		PGUPV::ButtonState::Released);

	out.mod = in.key.mod;

	const size_t nelem = (sizeof(translationTable) / sizeof(translationTable[0]));
	size_t low = 0, high = nelem - 1;
	out.key = PGUPV::KeyCode::Unknown;
	while (low <= high)
	{
		size_t midpoint = low + (high - low) / 2;
		if (in.key.key == translationTable[midpoint].from)
		{
			out.key = translationTable[midpoint].to;
			break;
		}
		else if (in.key.key < translationTable[midpoint].from)
			high = midpoint - 1;
		else
			low = midpoint + 1; //when key is > array[midpoint]
	}
	return true;
}

bool translateMouseButtonEvent(const SDL_Event& in, MouseButtonEvent& out) {

	auto w = WindowHW::getWindowFromId(in.key.windowID);
	if (w == nullptr) return false;

	out.wsrc = w;
	out.type = EventType::MouseButtonEvent;
	out.state = (in.button.down ? PGUPV::ButtonState::Pressed : PGUPV::ButtonState::Released);
	out.x = in.button.x;
	out.y = in.button.y;
	out.button = in.button.button;
	return true;
}

bool translateMouseMotionEvent(const SDL_Event& in, MouseMotionEvent& out) {
	auto w = WindowHW::getWindowFromId(in.key.windowID);
	if (w == nullptr) return false;

	out.wsrc = w;
	out.type = EventType::MouseMotionEvent;
	out.state = static_cast<uint8_t>(in.motion.state);
	out.x = in.motion.x;
	out.y = in.motion.y;
	out.xrel = in.motion.xrel;
	out.yrel = in.motion.yrel;
	return true;
}

bool translateMouseWheelEvent(const SDL_Event& in, MouseWheelEvent& out) {
	auto w = WindowHW::getWindowFromId(in.key.windowID);
	if (w == nullptr) return false;

	out.wsrc = w;
	out.type = EventType::MouseWheelEvent;
	out.x = in.wheel.x;
	out.y = in.wheel.y;
	return true;
}

bool translateJoyAxisMotionEvent(const SDL_Event& in, JoystickMotionEvent& out) {
	out.type = EventType::JoystickMotionEvent;
	out.joystickId = in.jaxis.which;
	out.axis = in.jaxis.axis;
	if (abs(in.jaxis.value) < JOYSTICK_EPSILON)
		out.value = 0;
	else if (in.jaxis.value > 0)
		out.value = static_cast<float>(in.jaxis.value) / JOYSTICK_MAXIMUM;
	else
		out.value = static_cast<float>(in.jaxis.value) / JOYSTICK_ABS_MINIMUM;
	return true;
}

bool translateJoyHatMotionEvent(const SDL_Event& in, JoystickHatMotionEvent& out) {
	out.type = EventType::JoystickHatMotionEvent;
	out.joystickId = in.jhat.which;
	out.hatId = in.jhat.hat;
	switch (in.jhat.value) {
	case SDL_HAT_LEFTUP:
		out.position = PGUPV::LeftUp;
		break;
	case SDL_HAT_UP:
		out.position = PGUPV::Up;
		break;
	case SDL_HAT_RIGHTUP:
		out.position = PGUPV::RightUp;
		break;
	case SDL_HAT_LEFT:
		out.position = PGUPV::Left;
		break;
	case SDL_HAT_CENTERED:
		out.position = PGUPV::Centered;
		break;
	case SDL_HAT_RIGHT:
		out.position = PGUPV::Right;
		break;
	case SDL_HAT_LEFTDOWN:
		out.position = PGUPV::LeftDown;
		break;
	case SDL_HAT_DOWN:
		out.position = PGUPV::Down;
		break;
	case SDL_HAT_RIGHTDOWN:
		out.position = PGUPV::RightDown;
		break;
	}
	return true;
}

bool translateJoyButtonEvent(const SDL_Event& in, JoystickButtonEvent& out) {
	out.type = EventType::JoystickButtonEvent;
	out.joystickId = in.jbutton.which;
	out.button = in.jbutton.button;
	out.state = (in.jbutton.down ? PGUPV::ButtonState::Pressed : PGUPV::ButtonState::Released);;
	return true;
}


bool translateTextInputEvent(const SDL_Event& in, TextInputEvent& out) {
	auto w = WindowHW::getWindowFromId(in.key.windowID);
	if (w == nullptr) return false;

	out.type = EventType::TextInputEvent;
	out.wsrc = w;
#ifdef _WIN32
	strcpy_s(out.text, sizeof(out.text), in.text.text);
#else
	strcpy(out.text, in.text.text);
#endif
	return true;
}


bool translateWindowResizedEvent(const SDL_Event& in, WindowResizedEvent& out) {
	auto w = WindowHW::getWindowFromId(in.key.windowID);
	if (w == nullptr) return false;

	assert(in.window.data1 >= 0 && in.window.data2 >= 0 && in.window.data1 <= 16384 && in.window.data2 <= 16384);
	out.type = EventType::WindowResizedEvent;
	out.wsrc = w;
	out.width = in.window.data1;
	out.height = in.window.data2;
	return true;
}

bool translateWindowCloseEvent(const SDL_Event& in, WindowClosedEvent& out) {
	auto w = WindowHW::getWindowFromId(in.key.windowID);
	if (w == nullptr) return false;

	out.type = EventType::WindowClosedEvent;
	out.wsrc = w;
	return true;
}


bool translateSDLEventToPGUPV(const SDL_Event& in, PGUPV::Event& out) {
	switch (in.type) {
	case SDL_EVENT_KEY_UP:
	case SDL_EVENT_KEY_DOWN:
		return translateKeyboardEvent(in, (KeyboardEvent&)out);
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	case SDL_EVENT_MOUSE_BUTTON_UP:
		return translateMouseButtonEvent(in, (MouseButtonEvent&)out);
	case SDL_EVENT_MOUSE_MOTION:
		return translateMouseMotionEvent(in, (MouseMotionEvent&)out);
	case SDL_EVENT_MOUSE_WHEEL:
		return translateMouseWheelEvent(in, (MouseWheelEvent&)out);
	case SDL_EVENT_JOYSTICK_AXIS_MOTION:
		return translateJoyAxisMotionEvent(in, (JoystickMotionEvent&)out);
	case SDL_EVENT_JOYSTICK_HAT_MOTION:
		return translateJoyHatMotionEvent(in, (JoystickHatMotionEvent&)out);
	case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
	case SDL_EVENT_JOYSTICK_BUTTON_UP:
		return translateJoyButtonEvent(in, (JoystickButtonEvent&)out);
	case SDL_EVENT_TEXT_INPUT:
		return translateTextInputEvent(in, (TextInputEvent&)out);
	case SDL_EVENT_WINDOW_RESIZED:
		return translateWindowResizedEvent(in, (WindowResizedEvent&)out);
	case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
		return translateWindowCloseEvent(in, (WindowClosedEvent&)out);
	case SDL_EVENT_QUIT:
		out.quit.type = EventType::QuitEvent;
		return true;
	default:
		break;
	}
	return false;
}

bool EventSourceHW::getEvent(Event& pgEvent) {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (!GUILib::processEvent(&e))
			if (translateSDLEventToPGUPV(e, pgEvent))
				return true;
	}
	return false;
}
