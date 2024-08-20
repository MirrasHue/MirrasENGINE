#pragma once

#include "Core/Application.h"

int main()
{
    mirras::Log::init();

    try
    {
        auto app = mirras::createClientApp();
        app->run();
    }
    catch(const std::exception& e)
    {
        throw;
    }

    return 0;
}