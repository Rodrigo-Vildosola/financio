#include <terra/terra.h>
#include <terra/core/entry_point.h>
#include "layer.h"

class ExampleGame : public terra::Application {
public:
    ExampleGame(terra::CommandLineArgs args)
        : terra::Application("My Example Game", args) 
    {
        push_layer(new ExampleLayer());
    }

    ~ExampleGame() {}

};

terra::Application* terra::create_application(terra::CommandLineArgs args) {
    return new ExampleGame(args);
}
