#include "MirrasENGINE.h"
#include "MirrasENGINE/MainEntryPoint.h"

class PlaygroundApp : public mirras::App
{
public:
    PlaygroundApp(mirras::AppSpecs& specs) :
        App{specs} {}
};

std::unique_ptr<mirras::App> mirras::createClientApp()
{
    mirras::AppSpecs specs {.title = "Playground",
                            .width = 1280,
                            .height = 720};

    return std::make_unique<PlaygroundApp>(specs);
}
