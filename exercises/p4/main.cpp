
#include <glm/gtc/matrix_transform.hpp>

#include "PGUPV.h"
#include "GUI3.h"


using namespace PGUPV;

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using std::vector;


class MyRender : public Renderer {
public:
  MyRender() {};
  void setup(void) override;
  void render(void) override;
  void reshape(uint w, uint h) override;
  void update(uint64_t ms) override;
  bool reload() override;
private:
  std::shared_ptr<GLMatrices> mats;
  std::shared_ptr<Program> program;
  std::shared_ptr<TextureVideo> videoFeed;
  GLint texUnitLoc;
  void buildGUI();
  Rect sp{ 2.f, 2.f };
};


void MyRender::setup() {
  glEnable(GL_DEPTH_TEST);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  mats = GLMatrices::build();

  reload();

  auto sources = media::VideoDevice::getAvailableCameras();
  if (sources.empty()) {
	  videoFeed = std::make_shared<TextureVideo>(App::assetsDir() + "videos/bigbuck.mp4");
  }
  else {
	  videoFeed = std::make_shared<TextureVideo>(0);
  }
  videoFeed->bind(GL_TEXTURE0);
}


void MyRender::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  sp.render();

  CHECK_GL();
}


void MyRender::reshape(uint w, uint h) {
  glViewport(0, 0, w, h);
  if (program) {
	  program->use();
	  glUniform1i(program->getUniformLocation("height"), h);
  }
}


void MyRender::update(uint64_t ms) {
}

bool MyRender::reload()
{
	/* Este shader se encarga de calcular la iluminación, teniendo
	  en cuenta una textura */
	program = std::make_shared<Program>();
	program->addAttributeLocation(Mesh::VERTICES, "position");
	program->addAttributeLocation(Mesh::TEX_COORD0, "texCoord");
	program->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);

	program->loadFiles(App::exercisesDir() + "p4/textureReplace");
	if (!program->compile())
		return false;

	// Localización de los uniform (posición de la luz y unidad de textura)
	texUnitLoc = program->getUniformLocation("texUnit");
	// Comunicamos la unidad de textura al shader
	program->use();
	glUniform1i(texUnitLoc, 0);
	return true;
}


int main(int argc, char *argv[]) {
  App &myApp = App::getInstance();
  myApp.setInitWindowSize(1280, 720);
  myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER | PGUPV::MULTISAMPLE);
  myApp.getWindow().setRenderer(std::make_shared<MyRender>());
  return myApp.run();
}
