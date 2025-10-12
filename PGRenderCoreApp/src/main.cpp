#include "MyApplication.h"

int main(int argc, char* argv[]) {
    Application::Config config;
    config.title = "PGRenderCore + SDL3 Example";

    MyApplication app(config);
    return app.run();
}
