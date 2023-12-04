#include <PGUPV.h>

using namespace PGUPV;

/* 
Rellena las funciones setup y render tal y como se explica en el enunciado de la práctica.
¡Cuidado! NO uses las llamadas de OpenGL directamente (glGenVertexArrays, glBindBuffer, etc.).
Usa las clases Model y Mesh de PGUPV.
*/

class MyRender : public Renderer {
public:
	void setup(void);
	void render(void);
	void reshape(uint w, uint h);
};

void MyRender::setup() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

void MyRender::render() {
	glClear(GL_COLOR_BUFFER_BIT);

}

void MyRender::reshape(uint w, uint h) {
	glViewport(0, 0, w, h);
}

int main(int argc, char *argv[]) {
	App &myApp = App::getInstance();
	myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER);
	myApp.getWindow().setRenderer(std::make_shared<MyRender>());
	return myApp.run();
}