
#ifndef _RENDERER_H
#define _RENDERER_H 2011

#include "baseRenderer.h"
#include "events.h"
#include "camerahandler.h"
#include "glMatrices.h"

namespace PGUPV {

  // Tienes que crear una clase que derive de esta para incluir las instrucciones de dibujado
  // de la escena.
  // NO hagas el intercambio de buffers al final del frame. Lo hará la biblioteca (así puede dibujar
  // más cosas encima).
  class Renderer : public BaseRenderer {
  public:
    Renderer() : initialized(false) {};
    virtual ~Renderer() { release(); };
    // En esta función puedes inicializar tu escena, el estado de OpenGL, etc.
    // Sólo se llamará una vez, al principio
    virtual void setup() {};
    // Aquí es donde dibujas los objetos de tu escena, en el estado actual
    virtual void render() = 0;
    // Esta función te indica que han pasado 'ms' milisegundos. Así puedes
    // mover los objetos dinámicos de tu escena dependiendo de su velocidad
    virtual void update(uint64_t) {};
    // Esta función se llamará cuando el usuario cierre la ventana. Incluye aquí el código
    // para liberar los recursos reservados
    virtual void release() {};

    // GESTIÓN DE EVENTOS
    // Funciones a implementar obligatoriamente
    // Esta función se llamará cada vez que la ventana cambie de tamaño.
    virtual void reshape(uint w, uint h) = 0;

    // Funciones opcionales: 
    //		deben devolver true si han consumido el evento, y false en otro caso
    // Eventos relacionados con los botones del ratón.
    virtual bool mouse_button(const MouseButtonEvent &) { return false; };
    // Eventos relacionados con el movimiento del ratón
    virtual bool mouse_move(const MouseMotionEvent &) { return false; };
    // Eventos relacionados con la ruleta del ratón
    virtual bool mouse_wheel(const MouseWheelEvent &) { return false; };
    // Eventos de pulsación/liberación de teclas
    virtual bool keyboard(const KeyboardEvent &) { return false; };
    // Eventos de movimiento del joystick
    virtual bool joystick(const JoystickMotionEvent &) { return false; };
    // Eventos de movimiento del 'hat' del joystick
    virtual bool joystick_hat(const JoystickHatMotionEvent &) { return false; };
    // Eventos de pulsaciones de botones del joystick
    virtual bool joystick_button(const JoystickButtonEvent &) { return false; };

	// Recarga de shaders:
	// si mueves aquí el código de carga de un shader, al pulsar Ctrl+R podrás recargar
	// uno o varios shaders sin tener que parar la ejecución. Recuerda conectar los atributos,
	// UBO, calcular la localización de los uniforms, inicializar los uniforms que no cambian
	// por frame, etc.
	virtual bool reload() { return true; }

    // Uso interno
    void dispatchSetup() {
      if (!initialized) {
        setup();
        initialized = true;
      }
    }
  protected:
    bool initialized;
  };

};
#endif
