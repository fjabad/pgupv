
#include <glm/gtc/matrix_transform.hpp>

#include "PGUPV.h"
#include "GUI3.h"

using namespace PGUPV;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

using std::vector;


/*

Simulando un volumen de recorte cónicocon un shader de geometría

*/

#define INITIAL_RADIUS 0.25f
#define INITIAL_HEIGHT 0.5f
#define INITIAL_AXIS glm::vec3(0.0f, 1.0f, 0.0f)

class MyRender : public Renderer {
public:
  MyRender()
    :
    conePosWCS(0.8f, 0.8f, 0.0f), // Posición en el S.C. del mundo
    coneModel(1.0f, 1e-5f, 1.0f),
    // Este objeto interpola linealmente entre las dos primeras variables,
    // la tercera es la duración de un ciclo y la cuarta el tipo de
    // movimiento
    // Se usa para calcular la posición del cono en cada momento.
    li(conePosWCS, vec3(-0.6, -0.4, 0), 4.0, PING_PONG) {};
  void setup(void) override;
  void render(void) override;
  void reshape(uint w, uint h) override;
  void update(uint64_t ms) override;
  bool reload() override;

private:
  std::shared_ptr<GLMatrices> mats;
  Axes axes;
  std::shared_ptr<Program> program;
  GLint lightPosLoc;
  vec4 lightPosWCS;
  GLint conePosLoc;
  vec3 conePosWCS; // Posición de la base del cono en el S.C. del mundo

  Cylinder coneModel;
  std::shared_ptr<Scene> bunny;
  LinearInterpolator<vec3> li;

  std::shared_ptr<FloatSliderWidget> coneHeight, coneRadius;
  std::shared_ptr<DirectionWidget> coneAxis; // Eje del cono en el S.C. del mundo
  void buildGUI();
};

void MyRender::update(uint64_t ms) {
  static uint64_t total = 0;

  total += ms;
  conePosWCS = li.interpolate(total / 1000.0);
}

bool MyRender::reload() {
	/* Este shader se encarga de calcular la iluminación (sólo componente
	 * difusa)*/
	program = std::make_shared<Program>();
	program->addAttributeLocation(Mesh::VERTICES, "position");
	program->addAttributeLocation(Mesh::NORMALS, "normal");
	program->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
	program->loadFiles(App::exercisesDir() + "p6/p6");
	program->compile();

	// Posiciones de las variables uniform
	lightPosLoc = program->getUniformLocation("lightpos");
	conePosLoc = program->getUniformLocation("conePos");

	// Color del objeto
	program->use();
	GLint diffuseLoc = program->getUniformLocation("diffuseColor");
	glUniform4f(diffuseLoc, 0.8f, 0.8f, 0.8f, 1.0f);

	// Reconectar los widgets con el programa
	destroyPanels();
	buildGUI();

	return true;
}

void MyRender::setup() {
  glClearColor(1.f, 1.f, 1.f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  // Inicialización de variables de estado
  // Posición de la luz en el S.C. del mundo
  lightPosWCS = vec4(2.f, 2.f, 2.f, 1.f);

  bunny = FileLoader::load(App::assetsDir() + "models/bunny.obj");

  mats = GLMatrices::build();
  setCameraHandler(std::make_shared<OrbitCameraHandler>());

  reload();

  App::getInstance().getWindow().showGUI();
}

void MyRender::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mat4 viewMatrix = getCamera().getViewMatrix();
  mats->setMatrix(GLMatrices::VIEW_MATRIX, viewMatrix);

  program->use();
  // Posición de la fuente en el S.C. de la cámara
  vec4 lp = viewMatrix * lightPosWCS;
  glUniform3f(lightPosLoc, lp.x, lp.y, lp.z);

  // Posición de la base del cono en el espacio del mundo
  glUniform3fv(conePosLoc, 1, &conePosWCS.x);

  // Dibujando el objeto
  mats->pushMatrix(GLMatrices::MODEL_MATRIX);
  mats->translate(GLMatrices::MODEL_MATRIX, 0.3f, 0.3f, 0.0f);
  mats->scale(GLMatrices::MODEL_MATRIX, vec3(1.2f / bunny->maxDimension()));
  mats->translate(GLMatrices::MODEL_MATRIX, -bunny->center());
  bunny->render();
  mats->popMatrix(GLMatrices::MODEL_MATRIX);

  // Un shader sencillo para dibujar el color definido por cada vértice
  ConstantIllumProgram::use();

  // Dibujamos los ejes de coordenadas
  axes.render();

  // Dibujamos el cono en alámbrico
  mats->pushMatrix(GLMatrices::MODEL_MATRIX);
  mats->translate(GLMatrices::MODEL_MATRIX, conePosWCS);

  GLStateCapturer<PolygonModeState> restorePolygonModeState;
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  auto t = coneAxis->get();
  auto yp1 = t.y + 1.0f;
  mats->multMatrix(GLMatrices::MODEL_MATRIX, glm::mat4(
    t.y + (t.z * t.z / yp1), -t.x, -t.z * t.x / yp1, 0.0f,
    t.x, t.y, t.z, 0.0f,
    -t.z*t.x / yp1, -t.z, t.y + (t.x*t.x / yp1), 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  ));
  vec3 cylScale(coneRadius->get());
  cylScale.y = coneHeight->get();
  mats->scale(GLMatrices::MODEL_MATRIX, cylScale);
  mats->translate(GLMatrices::MODEL_MATRIX, 0.0f, 0.5f, 0.0f);
  coneModel.render();
  mats->popMatrix(GLMatrices::MODEL_MATRIX);

  CHECK_GL();
}

void MyRender::reshape(uint w, uint h) {
  glViewport(0, 0, w, h);
  if (h == 0)
    h = 1;
  float ar = (float)w / h;
  mats->setMatrix(GLMatrices::PROJ_MATRIX,
    glm::perspective(glm::radians(60.0f), ar, .1f, 100.0f));
  mats->loadIdentity(GLMatrices::MODEL_MATRIX);
}

void MyRender::buildGUI() {
  // Un Panel representa un conjunto de widgets agrupados en una ventana, con un título
  auto panel = addPanel("Cono");

  // Podemos darle un tamaño y una posición, aunque no es necesario
  panel->setPosition(5, 50);
  panel->setSize(150, 240);

  coneRadius = std::make_shared<FloatSliderWidget>("Radio", INITIAL_RADIUS, 0.001f, 2.0f,
    program, "coneRadius");
  panel->addWidget(coneRadius);

  coneHeight = std::make_shared<FloatSliderWidget>("Altura", INITIAL_HEIGHT, 0.001f, 2.0f,
    program, "coneHeight");
  panel->addWidget(coneHeight);

  coneAxis = std::make_shared<DirectionWidget>("Eje", INITIAL_AXIS, program, "coneAxis");
  coneAxis->setCamera(getCameraHandler()->getCameraPtr());
  panel->addWidget(coneAxis);

}

int main(int argc, char *argv[]) {
  App &myApp = App::getInstance();
  myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER |
    PGUPV::MULTISAMPLE);
  myApp.getWindow().setRenderer(std::make_shared<MyRender>());
  return myApp.run();
}
