
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

Visualizador de terreno usando shaders de teselación.

*/

constexpr float terrainSize = 64.f; // El terreno tiene 64x64 unidades
constexpr unsigned int TerrainLimitUnitTex = 55;  // Unidad de textura para las paredes del terreno
class MyRender : public Renderer {
public:
	MyRender() {};
	void setup(void) override;
	void render(void) override;
	void reshape(uint w, uint h) override;
	bool reload() override;

private:
	void buildGUI();
	std::shared_ptr<Program> program;
	std::shared_ptr<GLMatrices> mats;
	Model model;
	Texture2D heightMap;
	
	Model limits;
	Texture2D limitColors;
};

void buildLimits(Model& m, Texture2D &colors);

void MyRender::setup() {
	glClearColor(1.0f, 1.f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	auto m = std::make_shared<Mesh>();
	std::vector<glm::vec3> vs;
	vs.push_back(glm::vec3(-terrainSize/2, 0.0, terrainSize/2.0));
	vs.push_back(glm::vec3(terrainSize/2.0, 0.0, terrainSize/2.0));
	vs.push_back(glm::vec3(terrainSize/2.0, 0.0, -terrainSize/2.0));
	vs.push_back(glm::vec3(-terrainSize/2.0, 0.0, -terrainSize/2.0));
	m->addVertices(vs);

	std::vector<glm::vec2> tc;
	tc.push_back(glm::vec2(0.0f, 0.0f));
	tc.push_back(glm::vec2(1.0f, 0.0f));
	tc.push_back(glm::vec2(1.0f, 1.0f));
	tc.push_back(glm::vec2(0.0f, 1.0f));
	m->addTexCoord(0, tc);

	DrawArrays* dc = new DrawArrays(GL_PATCHES, 0, 4);
	dc->setVerticesPerPatch(4);
	m->addDrawCommand(dc);
	model.addMesh(m);

	mats = GLMatrices::build();

	// Cargamos la nueva textura desde un fichero
	heightMap.loadImage(App::assetsDir() + "images/heightmap.png");

	reload();

	auto camera = std::make_shared<WalkCameraHandler>(20.0f); // Altura inicial de la cámara
	camera->setWalkSpeed(5.0f);
	setCameraHandler(camera);

	buildLimits(limits, limitColors);
}

bool MyRender::reload() {

	program = std::make_shared<Program>();
	program->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
	program->addAttributeLocation(Mesh::VERTICES, "position");
	program->addAttributeLocation(Mesh::TEX_COORD0, "texCoord");
	program->loadFiles(App::exercisesDir() + "p7/shader");
	program->compile();
	program->use();

	GLint texUnitHeightMapLoc = program->getUniformLocation("texUnitHeightMap");
	glUniform1i(texUnitHeightMapLoc, 0);

	return true;
}

void MyRender::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix());
	program->use();
	heightMap.bind(GL_TEXTURE0);
	model.render();

	glEnable(GL_BLEND);
	TextureReplaceProgram::use();
	TextureReplaceProgram::setTextureUnit(TerrainLimitUnitTex);
	limits.render();
	glDisable(GL_BLEND);


	CHECK_GL();
}

void MyRender::reshape(uint w, uint h) {
	glViewport(0, 0, w, h);
	if (h == 0)
		h = 1;
	float ar = (float)w / h;
	mats->setMatrix(GLMatrices::PROJ_MATRIX,
		glm::perspective(glm::radians(60.0f), ar, 0.1f, 100.0f));
}
void buildLimits(Model& m, Texture2D &limitColors) {
	auto mesh = std::make_shared<Mesh>();
	auto s = terrainSize / 2; 
	float h = 20.f;
	std::vector<glm::vec3> vtcs =
	{
		{-s, h, -s}, {-s, 0, -s},
		{ s, h, -s}, { s, 0, -s},
		{ s, h,  s}, { s, 0,  s},
		{-s, h,  s}, {-s, 0,  s},
		{-s, h, -s}, {-s, 0, -s}
	};
	mesh->addVertices(vtcs);
	std::vector<float> texc = { 0, 0, 10, 10, 20, 20, 30, 30, 40, 40 };
	mesh->addTexCoord(0, texc);
	mesh->addDrawCommand(new DrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(vtcs.size())));
	m.addMesh(mesh);
	std::vector<uint8_t> colors{ 10, 10, 10, 180, 20, 20};
	limitColors.loadImageFromMemory(colors.data(), 2, 1, GL_RGB, GL_UNSIGNED_BYTE, GL_RGBA);
	limitColors.setMagFilter(GL_NEAREST);
	glBlendColor(0, 0, 0, 0.4f);
	glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
	limitColors.bind(GL_TEXTURE0 + TerrainLimitUnitTex);
}

int main(int argc, char* argv[]) {
	App& myApp = App::getInstance();
	myApp.setInitWindowSize(800, 600);
	myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER);
	myApp.getWindow().setRenderer(std::make_shared<MyRender>());
	return myApp.run();
}