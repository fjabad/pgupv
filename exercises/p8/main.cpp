

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

Implementando las técnicas del bump mapping y el parallax mapping.

*/

class MyRender : public Renderer {
public:
	MyRender()
		: ci(vec4(0.0, 0.0, 1.0, 1.0), vec4(1.5, 0.0, 1.0, 1.0),
			vec3(0.0, 0.0, 1.0), 4),
		tower{ 0.5f, 0.5f, 1.f },
		lightBulb(.05f, 5, 5, vec4(1.0f, 1.0f, 0.0f, 1.0f)) {}
	void setup(void) override;
	void render(void) override;
	void reshape(uint w, uint h) override;

private:
	std::shared_ptr<GLMatrices> mats;
	std::shared_ptr<UBOLightSources> lights;
	std::shared_ptr<Program> program;
	Axes axes;
	Rect wall;
	Cylinder tower;
	std::shared_ptr<CheckBoxWidget> showAxis;
	CircularInterpolator ci;
	Sphere lightBulb;
	void buildGUI();
};

void MyRender::setup() {
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	/* Tendrás que implementar el bump mapping y el parallax mapping en este
	 * shader */
	program = std::make_shared<Program>();
	program->addAttributeLocation(Mesh::VERTICES, "position");
	program->addAttributeLocation(Mesh::NORMALS, "normal");
	program->addAttributeLocation(Mesh::TEX_COORD0, "texCoord");
	program->addAttributeLocation(Mesh::TANGENTS, "tangent");

	mats = GLMatrices::build();
	program->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
	lights = UBOLightSources::build();
	program->connectUniformBlock(lights, UBO_LIGHTS_BINDING_INDEX);

	LightSourceParameters lsp(glm::vec4(.2, .2, .2, 1.0),
		vec4(0.8, 0.7, 0.7, 1.0), vec4(1.0, 1.0, 1.0, 1.0),
		vec4(0.0, 0.0, 1.0, 0.0));
	lights->setLightSource(0, lsp);

	program->loadFiles(App::exercisesDir() + "p8/p8");
	program->compile();

	// Cargamos las texturas desde fichero
	auto tcolor = std::make_shared<Texture2D>();
	tcolor->loadImage(App::assetsDir() + "images/rocas-color.png");
	auto tbrillo = std::make_shared<Texture2D>();
	tbrillo->loadImage(App::assetsDir() + "images/rocas-gloss.png");
	auto tnormales = std::make_shared<Texture2D>();
	tnormales->loadImage(App::assetsDir() + "images/rocas-normales.png");

	// Asociamos las texturas a un material
	auto material = std::make_shared<Material>("bump-mapping");
	material->setDiffuseTexture(tcolor);
	material->setSpecularTexture(tbrillo);
	material->setNormalMapTexture(tnormales);

	// Asignamos el material a todas las mallas
	wall.accept([material](Mesh& m) { m.setMaterial(material); });

	// Ajustamos las coordenadas de textura del cilindro para que haya tres copias en la dirección original
	// (si no, los ladrillos quedan estirados horizontalmente)
	auto& towerM = tower.getMesh(0); // sólo hay una malla
	auto tc = towerM.getTexCoords();  // obtenemos sus coordenadas de textura
	for (auto& t : tc) {
		t *= glm::vec2{ 3, 1 };   // las modificamos
	}
	towerM.addTexCoord(0, tc);   // las volvemos a añadir

	buildGUI();
	App::getInstance().getWindow().showGUI();

	setCameraHandler(std::make_shared<OrbitCameraHandler>(1.5f));
}

void MyRender::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 viewMatrix = getCamera().getViewMatrix();
	mats->setMatrix(GLMatrices::VIEW_MATRIX, viewMatrix);

	// Activar el shader que aplica el bump mapping
	program->use();

	LightSourceParameters light = lights->getLightSource(0);
	// Posición de la luz (coord. mundo)
	light.positionWorld = ci.interpolate(App::getInstance().getAppTime());
	// Posición de la luz (coord. cámara)
	light.positionEye = viewMatrix * light.positionWorld;
	lights->setLightSource(0, light);

	mats->pushMatrix(GLMatrices::MODEL_MATRIX);

	mats->translate(GLMatrices::MODEL_MATRIX, -1.5, 0, 0);
	tower.render();

	mats->translate(GLMatrices::MODEL_MATRIX, 1, 0, 0);
	wall.render();

	mats->translate(GLMatrices::MODEL_MATRIX, 1, 0, 0);
	wall.render();

	mats->translate(GLMatrices::MODEL_MATRIX, 1, 0, 0);
	tower.render();

	mats->popMatrix(GLMatrices::MODEL_MATRIX);

	ConstantIllumProgram::use();

	if (showAxis->get()) {
		// Dibujamos los ejes de coordenadas
		axes.render();
	}
	// Dibujamos la posición de la luz
	mats->pushMatrix(GLMatrices::MODEL_MATRIX);
	mats->setMatrix(GLMatrices::MODEL_MATRIX,
		glm::translate(glm::mat4(1.0f), vec3(light.positionWorld)));
	lightBulb.render();
	mats->popMatrix(GLMatrices::MODEL_MATRIX);

	CHECK_GL();
}

void MyRender::reshape(uint w, uint h) {
	glViewport(0, 0, w, h);
	mats->setMatrix(GLMatrices::PROJ_MATRIX, getCamera().getProjMatrix());
}

void MyRender::buildGUI() {
	auto panel = addPanel("Bump mapping");
	panel->setPosition(580, 10);
	panel->setSize(200, 80);

	panel->addWidget(std::make_shared<CheckBoxWidget>("Parallax", false, program, "useParallax"));
	showAxis = std::make_shared<CheckBoxWidget>("Mostrar ejes", false);
	panel->addWidget(showAxis);


}

int main(int argc, char* argv[]) {
	App& myApp = App::getInstance();
	myApp.setInitWindowSize(800, 600);
	myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER |
		PGUPV::MULTISAMPLE);
	myApp.getWindow().setRenderer(std::make_shared<MyRender>());
	return myApp.run();
}
