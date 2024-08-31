#include "MirrasENGINE.h"
#include "MirrasENGINE/MainEntryPoint.h"

#include "EditorLayer.h"

namespace mirras
{
    class EditorApp : public App
    {
    public:
        EditorApp(const AppSpecs& appSpecs, const WindowSpecs& windowSpecs) :
            App{appSpecs, windowSpecs}
        {
            addLayer(std::make_unique<EditorLayer>());
        }
    };

    std::unique_ptr<App> createClientApp()
    {
        AppSpecs appSpecs {.name = "Editor",
                           .updateRate = 60};

        WindowSpecs windowSpecs {.title = "MirrasENGINE Editor",
                                 .minWidth = 600,
                                 .minHeight = 400};

        return std::make_unique<EditorApp>(appSpecs, windowSpecs);
    }
}