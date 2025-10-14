// // Example showing the simplified layer system usage
// #include "eng/core/application.h"
// // #include "eng/core/layer.h"
// #include <iostream>

// using namespace eng;

// // Example layer for a GUI application
// class MyGameLayer {
// public:
//     void on_attach() {
//         // Initialize layer resources
//         std::cout << "Game layer attached!\n";
//     }
    
//     void on_detach() {
//         // Clean up layer resources
//         std::cout << "Game layer detached!\n";
//     }
    
//     void on_update(Timestep ts) {
//         // Update game logic
//         std::cout << "Game layer update: " << ts << "s\n";
//     }
    
//     void on_physics_update(Timestep fixed_ts) {
//         // Fixed timestep physics
//         std::cout << "Game layer physics: " << fixed_ts << "s\n";
//     }
    
//     void on_ui_render() {
//         // Render UI elements
//         std::cout << "Game layer UI render\n";
//     }
    
//     void on_event(Event& e) {
//         // Handle events
//         std::cout << "Game layer event\n";
//     }
// };

// // Example layer for a headless application
// class MyHeadlessLayer {
// public:
//     void on_attach() {
//         std::cout << "Headless layer attached!\n";
//     }
    
//     void on_detach() {
//         std::cout << "Headless layer detached!\n";
//     }
    
//     void on_update(Timestep ts) {
//         // std::cout << "Headless layer update: " << ts << "s\n";
//     }
    
//     void on_physics_update(Timestep fixed_ts) {
//         std::cout << "Headless layer physics: " << fixed_ts << "s\n";
//     }

//     // void on_ui_render() {
//     //     // Render UI elements
//     //     std::cout << "Game layer UI render\n";
//     // }
    
//     // void on_event(Event& e) {
//     //     // Handle events
//     //     std::cout << "Game layer event\n";
//     // }
//     // Note: No on_ui_render or on_event for headless
// };

// // Example hooks for GUI application
// struct GuiHooks {
//     static void init() {
//         std::cout << "Initializing GUI platform...\n";
//         // Initialize window, OpenGL/Vulkan, etc.
//     }
    
//     static void shutdown() {
//         std::cout << "Shutting down GUI platform...\n";
//         // Clean up platform resources
//     }
    
//     static void begin_frame() {
//         // Clear screen, begin rendering
//         std::cout << "Begin frame\n";
//     }
    
//     static void end_frame() {
//         // Swap buffers, present
//         std::cout << "End frame\n";
//     }
    
//     static void pump_platform() {
//         // Handle window events, input, etc.
//         std::cout << "Pump platform events\n";
//     }
// };

// // Example hooks for headless application
// struct HeadlessHooks {
//     static void init() {
//         std::cout << "Initializing headless platform...\n";
//     }
    
//     static void shutdown() {
//         std::cout << "Shutting down headless platform...\n";
//     }
    
//     static void begin_frame() {
//         // No-op for headless
//     }
    
//     static void end_frame() {
//         // No-op for headless
//     }
    
//     static void pump_platform() {
//         // Handle signals, timers, etc.
//         std::cout << "Pump headless events\n";
//     }
// };

// int main() {
//     // GUI Application example
//     std::cout << "=== GUI Application Example ===\n";
//     Application<GuiConfig, GuiHooks> gui_app;
    
//     // Add layers to GUI app
//     auto& game_layer = gui_app.push_root_layer<MyGameLayer>();
//     auto& overlay = gui_app.push_overlay<MyGameLayer>();
    
//     // Run for a few iterations (in real app this would be the main loop)
//     // gui_app.run(); // This would run indefinitely
    
//     std::cout << "\n=== Headless Application Example ===\n";
//     Application<HeadlessConfig, HeadlessHooks> headless_app;
    
//     // Add layers to headless app
//     auto& headless_layer = headless_app.push_root_layer<MyHeadlessLayer>();
    
//     // Run for a few iterations
//     headless_app.run(); // This would run indefinitely
    
//     return 0;
// }
