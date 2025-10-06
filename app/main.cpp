#include <eng/enginio.h>
#include <eng/core/entry_point.cpp>

#include "financio/root_layer.h"

class FinancioApp : public eng::Application {
public:
    FinancioApp(eng::CommandLineArgs args) : eng::Application("Financio App", args) 
    {
        push_layer(new app::RootLayer());
    }

    ~FinancioApp() {}
};

eng::Application* eng::create_application(eng::CommandLineArgs args) {
    return new FinancioApp(args);
}
