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
            addLayer(instantiate<EditorLayer>());
        }
    };

    single_ref<App> createClientApp()
    {
        AppSpecs appSpecs {
            .name = "Editor",
            .workingDirectory = "../../MirrasENGINE/Editor",
            .backend = Renderer::Backend::OpenGL,
            .updateRate = 60
        };

        WindowSpecs windowSpecs {
            .title = "MirrasENGINE Editor",
            .iconFilepath = "Assets/Icons/icon.png",
            .width = 1600,
            .height = 900,
            .minWidth = 1280,
            .minHeight = 720
        };

        return instantiate<EditorApp>(appSpecs, windowSpecs);
    }
} // namespace mirras
