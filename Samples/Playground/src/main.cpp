#include "PlaygroundApp.h"

int main()
{
    mirras::AppSpecs appSpecs {
        .workingDirectory = "../../Samples/Playground",
        .backend = mirras::Renderer::Backend::OpenGL,
        .fixedUpdateRate = 60
    };

    mirras::WindowSpecs windowSpecs {
        .title = "Playground",
        .iconFilepath = "assets/icon.png",
        .width = 1366,
        .height = 768,
        .minWidth = 900,
        .minHeight = 500
    };

    PlaygroundApp app{appSpecs, windowSpecs};
    app.run();
}