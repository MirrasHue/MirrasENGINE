#include "Benchmark.h"

int main()
{
    mirras::AppSpecs appSpecs {
        .workingDirectory = "../../Samples/Benchmark",
        .fixedUpdateRate = 60
    };

    mirras::WindowSpecs windowSpecs {
        .title = "Benchmark",
        .iconFilepath = "assets/icon.png",
        .maximized = true,
        .VSync = false
    };

    Benchmark app{appSpecs, windowSpecs};
    app.run();
}
