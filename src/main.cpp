#include "Sketchpad.hpp"
#include <iostream>
#include <filesystem>

int main() {
    // Test if resources are accessible
    sf::Texture testTexture;
    if (testTexture.loadFromFile("resources/icons/pen.png")) {
        std::cout << "SUCCESS: Found resources/icons/pen.png" << std::endl;
    } else {
        std::cout << "FAILED: Could not find resources/icons/pen.png" << std::endl;
        std::cout << "Make sure 'resources' folder is in the same directory as your .exe file!" << std::endl;
    }

    // Create and run the sketchpad application
    // Note: Constructor parameters are not used since we use fullscreen mode
    SketchpadWindow app;
    app.run();

    return 0;
}
