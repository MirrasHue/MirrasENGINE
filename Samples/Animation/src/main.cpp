#include "Animation.h"

int main()
{
    mirras::AppSpecs appSpecs {
        .workingDirectory = "../../Samples/Animation",
        .fixedUpdateRate = 60
    };

    mirras::WindowSpecs windowSpecs {
        .title = "Animation",
        .iconFilepath = "assets/icon.png",
        .width = 800,
        .height = 450
    };

    Animation app{appSpecs, windowSpecs};
    app.run();
}
