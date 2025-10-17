#include <eng/enginio.h>
#include <eng/runtime/entry_windowed.cpp>

#include "eng/runtime/application.h"
#include "financio/root_layer.h"

class FinancioApp final : public eng::Application {
public:
    FinancioApp(eng::CommandLineArgs args) : eng::Application("Financio App", args) 
    {
        push_layer(new app::RootLayer());
    }

    ~FinancioApp() {}
};

eng::ApplicationBase* eng::create_application(eng::CommandLineArgs args) {
    return new FinancioApp(args);
}
