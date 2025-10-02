#include <eng/enginio.h>
#include <eng/core/entry_point.cpp>
#include <twsapi/EClientSocket.h>

#include "financio/root_layer.h"

class ExampleApp : public eng::Application {
public:
    ExampleApp(eng::CommandLineArgs args) : eng::Application("My Example Application", args) 
    {
        push_layer(new RootLayer());
    }

    ~ExampleApp() {}

};

eng::Application* eng::create_application(eng::CommandLineArgs args) {
    return new ExampleApp(args);
}
