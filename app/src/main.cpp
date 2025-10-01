#include <eng/enginio.h>
#include <eng/core/entry_point.h>
#include "layer.h"

class ExampleGame : public eng::Application {
public:
    ExampleGame(eng::CommandLineArgs args)
        : eng::Application("My Example Game", args) 
    {
        push_layer(new ExampleLayer());
    }

    ~ExampleGame() {}

};

eng::Application* eng::create_application(eng::CommandLineArgs args) {
    return new ExampleGame(args);
}
