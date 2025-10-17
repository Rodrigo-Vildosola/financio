#include "financio/ui/panels/chart_panel.h"

#include "eng/ui/implot.h"


namespace app {

void ChartPanel::on_draw() {
    if (ImPlot::BeginPlot("Price History")) {
        static float xs[100], ys[100];
        for (int i = 0; i < 100; ++i) { xs[i] = i; ys[i] = sinf(i * 0.1f); }
        ImPlot::PlotLine("Demo", xs, ys, 100);
        ImPlot::EndPlot();
    }
}

} // namespace app
