#include "MirrasENGINE.h"
#include "MirrasENGINE/MainEntryPoint.h"

#include "EditorLayer.h"

namespace mirras
{
    class EditorApp : public App
    {
    public:
        EditorApp(AppSpecs& specs) :
            App{specs}
        {
            addLayer(std::make_unique<EditorLayer>());
        }
    };

    std::unique_ptr<App> createClientApp()
    {
        AppSpecs specs {.title = "MirrasENGINE Editor",
                        .width = 1280,
                        .height = 720};

        return std::make_unique<EditorApp>(specs);
    }
}