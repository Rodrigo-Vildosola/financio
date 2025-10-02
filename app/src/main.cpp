#include <eng/enginio.h>
#include <eng/core/entry_point.cpp>
#include <twsapi/EClientSocket.h>
#include "layer.h"

class ExampleApp : public eng::Application {
public:
    ExampleApp(eng::CommandLineArgs args)
        : eng::Application("My Example Application", args) 
    {
        push_layer(new ExampleLayer());
    }

    ~ExampleApp() {}

};

eng::Application* eng::create_application(eng::CommandLineArgs args) {
    return new ExampleApp(args);
}
