#include "MirrasENGINE.h"
#include "MirrasENGINE/MainEntryPoint.h"

#include "PlaygroundLayer.h"
#include "SceneExample.h"

//using namespace mirras;

class PlaygroundApp : public mirras::App
{
public:
    PlaygroundApp(const mirras::AppSpecs& appSpecs, const mirras::WindowSpecs& windowSpecs) :
        App{appSpecs, windowSpecs}
    {
        //addLayer(mirras::instantiate<PlaygroundLayer>());
        addLayer(mirras::instantiate<SceneExample>());
    }
};

mirras::single_ref<mirras::App> mirras::createClientApp()
{
    mirras::AppSpecs appSpecs {
        .name = "Playground",
        .workingDirectory = "../../Samples/Playground",
        .backend = Renderer::Backend::OpenGL,
        .updateRate = 60
    };

    mirras::WindowSpecs windowSpecs {
        .title = "MirrasENGINE Playground",
        .width = 1366,
        .height = 768,
        .minWidth = 900,
        .minHeight = 500
    };

    return instantiate<PlaygroundApp>(appSpecs, windowSpecs);
}
