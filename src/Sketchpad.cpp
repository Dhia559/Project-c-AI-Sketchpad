#include "Sketchpad.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <windows.h>
#include <commdlg.h>
#include <cmath>

SketchpadWindow::SketchpadWindow()
    : window(sf::VideoMode::getDesktopMode(), "SFML Sketchpad - JuggernautXL Ragnarok", sf::Style::Fullscreen),
      currentColor(sf::Color::Black),
      brushSize(5.0f),
      isDrawing(false),
      currentToolIndex(0),
      currentToolType(PEN),
      isProcessing(false),
      showingEnhanced(false),
      isFullscreen(true),
      showingColorPicker(false),
      currentHue(0.0f),
      currentSaturation(1.0f),
      currentValue(0.0f),
      draggingHue(false),
      draggingSV(false),
      showingSizeControl(false),
      penSize(3.0f),
      brushSize2(15.0f),
      draggingPenSize(false),
      draggingBrushSize(false),
      showCustomCursor(false)
{
    auto mode = sf::VideoMode::getDesktopMode();

    canvas.create(mode.width, mode.height);
    canvas.clear(sf::Color::White);
    canvas.display();
    canvasSprite.setTexture(canvas.getTexture());

    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
    }
    statusText.setFont(font);
    statusText.setCharacterSize(28);
    statusText.setFillColor(sf::Color::White);
    statusText.setPosition(20.f, static_cast<float>(mode.height - 60));
    statusText.setStyle(sf::Text::Bold);

    // Initialize button labels
    for (int i = 0; i < 4; i++) {
        buttonLabels[i].setFont(font);
        buttonLabels[i].setCharacterSize(16);
        buttonLabels[i].setFillColor(sf::Color::White);
        buttonLabels[i].setStyle(sf::Text::Bold);
    }

    currentTool = new PenTool(currentColor, 3);

    float startX = 20.f, startY = 20.f, buttonSize = 60.f, spacing = 10.f;

    // ========== DRAWING TOOLS (indices 0-2) ==========
    toolbar.addButton(std::make_unique<Button>(sf::Vector2f{startX, startY}, sf::Vector2f{buttonSize, buttonSize},
        "resources/icons/pen.png", [this]() { switchTool(PEN); toolbar.setSelectedTool(0); }));

    toolbar.addButton(std::make_unique<Button>(sf::Vector2f{startX + (buttonSize + spacing) * 1.f, startY}, sf::Vector2f{buttonSize, buttonSize},
        "resources/icons/brush.png", [this]() { switchTool(BRUSH); toolbar.setSelectedTool(1); }));

    toolbar.addButton(std::make_unique<Button>(sf::Vector2f{startX + (buttonSize + spacing) * 2.f, startY}, sf::Vector2f{buttonSize, buttonSize},
        "resources/icons/eraser.png", [this]() { switchTool(ERASER); toolbar.setSelectedTool(2); }));

    // ========== COLOR PICKER BUTTON ==========
    float colorButtonX = startX + (buttonSize + spacing) * 3.f;
    toolbar.addButton(std::make_unique<Button>(
        sf::Vector2f{colorButtonX, startY},
        sf::Vector2f{buttonSize, buttonSize},
        currentColor,
        [this]() { toggleColorPicker(); }
    ));

    // Initialize current color display (will be drawn separately to update dynamically)
    currentColorDisplay.setSize(sf::Vector2f{buttonSize, buttonSize});
    currentColorDisplay.setPosition(sf::Vector2f{colorButtonX, startY});
    currentColorDisplay.setFillColor(currentColor);
    currentColorDisplay.setOutlineThickness(2.f);
    currentColorDisplay.setOutlineColor(sf::Color(200, 200, 200));

    // ========== SIZE CONTROL BUTTON ==========
    float sizeButtonX = startX + (buttonSize + spacing) * 4.f;
    toolbar.addButton(std::make_unique<Button>(
        sf::Vector2f{sizeButtonX, startY},
        sf::Vector2f{buttonSize, buttonSize},
        sf::Color(80, 80, 80),
        [this]() { toggleSizeControl(); }
    ));

    // Calculate position for action buttons
    float actionButtonX = startX + (buttonSize + spacing) * 5.f + 30.f;

    // ========== ACTION BUTTONS ==========
    toolbar.addButton(std::make_unique<Button>(
        sf::Vector2f{actionButtonX, startY},
        sf::Vector2f{buttonSize, buttonSize},
        sf::Color(180, 30, 30),
        [this]() { resetCanvas(); }
    ));
    buttonLabels[0].setString("CLEAR");
    buttonLabels[0].setPosition(actionButtonX + 5.f, startY + buttonSize + 5.f);

    toolbar.addButton(std::make_unique<Button>(
        sf::Vector2f{actionButtonX + (buttonSize + spacing), startY},
        sf::Vector2f{buttonSize, buttonSize},
        sf::Color(30, 180, 30),
        [this]() { saveCanvasAs(); }
    ));
    buttonLabels[1].setString("SAVE");
    buttonLabels[1].setPosition(actionButtonX + (buttonSize + spacing) + 8.f, startY + buttonSize + 5.f);

    toolbar.addButton(std::make_unique<Button>(
        sf::Vector2f{actionButtonX + (buttonSize + spacing) * 2.f, startY},
        sf::Vector2f{buttonSize, buttonSize},
        sf::Color(30, 100, 180),
        [this]() { openCanvas(); }
    ));
    buttonLabels[2].setString("OPEN");
    buttonLabels[2].setPosition(actionButtonX + (buttonSize + spacing) * 2.f + 8.f, startY + buttonSize + 5.f);

    toolbar.addButton(std::make_unique<Button>(
        sf::Vector2f{actionButtonX + (buttonSize + spacing) * 3.f, startY},
        sf::Vector2f{buttonSize, buttonSize},
        sf::Color(100, 50, 200),
        [this]() { enhanceWithAI(); }
    ));
    buttonLabels[3].setString("AI");
    buttonLabels[3].setPosition(actionButtonX + (buttonSize + spacing) * 3.f + 18.f, startY + buttonSize + 5.f);

    // Initialize color picker position (center of screen)
    colorPickerPosition = sf::Vector2f(static_cast<float>(mode.width) / 2.f - 150.f, static_cast<float>(mode.height) / 2.f - 150.f);

    // Initialize size control position
    sizeControlPosition = sf::Vector2f(static_cast<float>(mode.width) / 2.f - 150.f, static_cast<float>(mode.height) / 2.f - 100.f);

    // Initialize custom cursor
    customCursor.setFillColor(sf::Color::Transparent);
    customCursor.setOutlineThickness(2.f);
    customCursor.setOutlineColor(sf::Color::White);

    // Hide system cursor when drawing
    window.setMouseCursorVisible(true);
}

sf::Color SketchpadWindow::hsvToRgb(float h, float s, float v) {
    float c = v * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    float r = 0, g = 0, b = 0;

    if (h >= 0 && h < 60) {
        r = c; g = x; b = 0;
    } else if (h >= 60 && h < 120) {
        r = x; g = c; b = 0;
    } else if (h >= 120 && h < 180) {
        r = 0; g = c; b = x;
    } else if (h >= 180 && h < 240) {
        r = 0; g = x; b = c;
    } else if (h >= 240 && h < 300) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }

    return sf::Color(
        static_cast<sf::Uint8>((r + m) * 255),
        static_cast<sf::Uint8>((g + m) * 255),
        static_cast<sf::Uint8>((b + m) * 255)
    );
}

void SketchpadWindow::toggleColorPicker() {
    showingColorPicker = !showingColorPicker;
    if (showingColorPicker) {
        statusText.setString("Color Picker - Select your color | ESC to close");
    }
}

void SketchpadWindow::updateColorPicker() {
    // Update current color based on HSV values
    currentColor = hsvToRgb(currentHue, currentSaturation, currentValue);

    // Update the visual color display
    currentColorDisplay.setFillColor(currentColor);
}

void SketchpadWindow::toggleSizeControl() {
    showingSizeControl = !showingSizeControl;
    if (showingSizeControl) {
        statusText.setString("Adjust Pen/Brush Size | ESC to close");
    }
}

void SketchpadWindow::updateToolSize() {
    // Update the current tool with new size
    if (currentToolType == PEN || currentToolType == BRUSH) {
        switchTool(currentToolType);
    }
}

void SketchpadWindow::drawSizeControl() {
    if (!showingSizeControl) return;

    // Semi-transparent background
    sf::RectangleShape backdrop(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
    backdrop.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(backdrop);

    // Main panel
    sf::RectangleShape panel(sf::Vector2f(300.f, 200.f));
    panel.setPosition(sizeControlPosition);
    panel.setFillColor(sf::Color(50, 50, 50));
    panel.setOutlineThickness(3.f);
    panel.setOutlineColor(sf::Color::White);
    window.draw(panel);

    // Title
    sf::Text title("Tool Size", font, 24);
    title.setPosition(sizeControlPosition.x + 10.f, sizeControlPosition.y + 10.f);
    title.setFillColor(sf::Color::White);
    window.draw(title);

    // Pen size slider
    sf::Text penLabel("Pen Size: " + std::to_string(static_cast<int>(penSize)) + "px", font, 18);
    penLabel.setPosition(sizeControlPosition.x + 20.f, sizeControlPosition.y + 60.f);
    penLabel.setFillColor(sf::Color::White);
    window.draw(penLabel);

    sf::RectangleShape penTrack(sf::Vector2f(250.f, 4.f));
    penTrack.setPosition(sizeControlPosition.x + 20.f, sizeControlPosition.y + 90.f);
    penTrack.setFillColor(sf::Color(100, 100, 100));
    window.draw(penTrack);

    float penSliderX = sizeControlPosition.x + 20.f + ((penSize - 1.f) / 19.f) * 250.f;
    sf::CircleShape penKnob(8.f);
    penKnob.setPosition(penSliderX - 8.f, sizeControlPosition.y + 90.f - 6.f);
    penKnob.setFillColor(sf::Color::White);
    window.draw(penKnob);

    // Brush size slider
    sf::Text brushLabel("Brush Size: " + std::to_string(static_cast<int>(brushSize2)) + "px", font, 18);
    brushLabel.setPosition(sizeControlPosition.x + 20.f, sizeControlPosition.y + 120.f);
    brushLabel.setFillColor(sf::Color::White);
    window.draw(brushLabel);

    sf::RectangleShape brushTrack(sf::Vector2f(250.f, 4.f));
    brushTrack.setPosition(sizeControlPosition.x + 20.f, sizeControlPosition.y + 150.f);
    brushTrack.setFillColor(sf::Color(100, 100, 100));
    window.draw(brushTrack);

    float brushSliderX = sizeControlPosition.x + 20.f + ((brushSize2 - 5.f) / 45.f) * 250.f;
    sf::CircleShape brushKnob(8.f);
    brushKnob.setPosition(brushSliderX - 8.f, sizeControlPosition.y + 150.f - 6.f);
    brushKnob.setFillColor(sf::Color::White);
    window.draw(brushKnob);
}

void SketchpadWindow::handleSizeControlClick(sf::Vector2i mousePos) {
    if (!showingSizeControl) return;

    float penTrackY = sizeControlPosition.y + 90.f;
    float brushTrackY = sizeControlPosition.y + 150.f;
    float trackX = sizeControlPosition.x + 20.f;
    float trackWidth = 250.f;

    // Check pen slider
    if (mousePos.x >= trackX && mousePos.x <= trackX + trackWidth &&
        mousePos.y >= penTrackY - 10.f && mousePos.y <= penTrackY + 10.f) {
        draggingPenSize = true;
        float relativeX = mousePos.x - trackX;
        penSize = 1.f + (relativeX / trackWidth) * 19.f;
        penSize = std::max(1.f, std::min(20.f, penSize));
        updateToolSize();
    }

    // Check brush slider
    if (mousePos.x >= trackX && mousePos.x <= trackX + trackWidth &&
        mousePos.y >= brushTrackY - 10.f && mousePos.y <= brushTrackY + 10.f) {
        draggingBrushSize = true;
        float relativeX = mousePos.x - trackX;
        brushSize2 = 5.f + (relativeX / trackWidth) * 45.f;
        brushSize2 = std::max(5.f, std::min(50.f, brushSize2));
        updateToolSize();
    }
}

void SketchpadWindow::handleSizeControlDrag(sf::Vector2i mousePos) {
    if (!showingSizeControl) return;

    float trackX = sizeControlPosition.x + 20.f;
    float trackWidth = 250.f;

    if (draggingPenSize) {
        float relativeX = mousePos.x - trackX;
        penSize = 1.f + (relativeX / trackWidth) * 19.f;
        penSize = std::max(1.f, std::min(20.f, penSize));
        updateToolSize();
    }

    if (draggingBrushSize) {
        float relativeX = mousePos.x - trackX;
        brushSize2 = 5.f + (relativeX / trackWidth) * 45.f;
        brushSize2 = std::max(5.f, std::min(50.f, brushSize2));
        updateToolSize();
    }
}

void SketchpadWindow::drawColorPicker() {
    if (!showingColorPicker) return;

    // Semi-transparent background
    sf::RectangleShape backdrop(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
    backdrop.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(backdrop);

    // Main color picker panel
    sf::RectangleShape panel(sf::Vector2f(300.f, 350.f));
    panel.setPosition(colorPickerPosition);
    panel.setFillColor(sf::Color(50, 50, 50));
    panel.setOutlineThickness(3.f);
    panel.setOutlineColor(sf::Color::White);
    window.draw(panel);

    // Title
    sf::Text title("Color Picker", font, 24);
    title.setPosition(colorPickerPosition.x + 10.f, colorPickerPosition.y + 10.f);
    title.setFillColor(sf::Color::White);
    window.draw(title);

    // Hue slider (vertical rainbow bar)
    float hueX = colorPickerPosition.x + 20.f;
    float hueY = colorPickerPosition.y + 50.f;
    float hueWidth = 30.f;
    float hueHeight = 250.f;

    for (int i = 0; i < 250; i++) {
        float h = (i / 250.f) * 360.f;
        sf::RectangleShape hueSegment(sf::Vector2f(hueWidth, 1.f));
        hueSegment.setPosition(hueX, hueY + i);
        hueSegment.setFillColor(hsvToRgb(h, 1.0f, 1.0f));
        window.draw(hueSegment);
    }

    // Hue slider indicator
    float hueIndicatorY = hueY + (currentHue / 360.f) * hueHeight;
    sf::RectangleShape hueIndicator(sf::Vector2f(hueWidth + 10.f, 3.f));
    hueIndicator.setPosition(hueX - 5.f, hueIndicatorY);
    hueIndicator.setFillColor(sf::Color::White);
    hueIndicator.setOutlineThickness(1.f);
    hueIndicator.setOutlineColor(sf::Color::Black);
    window.draw(hueIndicator);

    // Saturation/Value box
    float svX = colorPickerPosition.x + 70.f;
    float svY = colorPickerPosition.y + 50.f;
    float svSize = 200.f;

    // Draw saturation/value gradient
    for (int y = 0; y < static_cast<int>(svSize); y++) {
        for (int x = 0; x < static_cast<int>(svSize); x++) {
            float s = x / svSize;
            float v = 1.0f - (y / svSize);
            sf::RectangleShape pixel(sf::Vector2f(1.f, 1.f));
            pixel.setPosition(svX + x, svY + y);
            pixel.setFillColor(hsvToRgb(currentHue, s, v));
            window.draw(pixel);
        }
    }

    // S/V indicator
    float svIndicatorX = svX + currentSaturation * svSize;
    float svIndicatorY = svY + (1.0f - currentValue) * svSize;
    sf::CircleShape svIndicator(8.f);
    svIndicator.setPosition(svIndicatorX - 8.f, svIndicatorY - 8.f);
    svIndicator.setFillColor(sf::Color::Transparent);
    svIndicator.setOutlineThickness(3.f);
    svIndicator.setOutlineColor(sf::Color::White);
    window.draw(svIndicator);

    // Color preview
    sf::RectangleShape preview(sf::Vector2f(250.f, 30.f));
    preview.setPosition(colorPickerPosition.x + 20.f, colorPickerPosition.y + 310.f);
    preview.setFillColor(currentColor);
    preview.setOutlineThickness(2.f);
    preview.setOutlineColor(sf::Color::White);
    window.draw(preview);
}

void SketchpadWindow::handleColorPickerClick(sf::Vector2i mousePos) {
    if (!showingColorPicker) return;

    float hueX = colorPickerPosition.x + 20.f;
    float hueY = colorPickerPosition.y + 50.f;
    float hueWidth = 30.f;
    float hueHeight = 250.f;

    float svX = colorPickerPosition.x + 70.f;
    float svY = colorPickerPosition.y + 50.f;
    float svSize = 200.f;

    // Check if clicked on hue slider
    if (mousePos.x >= hueX && mousePos.x <= hueX + hueWidth &&
        mousePos.y >= hueY && mousePos.y <= hueY + hueHeight) {
        draggingHue = true;
        float relativeY = mousePos.y - hueY;
        currentHue = (relativeY / hueHeight) * 360.f;
        currentHue = std::max(0.f, std::min(360.f, currentHue));
        updateColorPicker();
    }

    // Check if clicked on S/V box
    if (mousePos.x >= svX && mousePos.x <= svX + svSize &&
        mousePos.y >= svY && mousePos.y <= svY + svSize) {
        draggingSV = true;
        float relativeX = mousePos.x - svX;
        float relativeY = mousePos.y - svY;
        currentSaturation = relativeX / svSize;
        currentValue = 1.0f - (relativeY / svSize);
        currentSaturation = std::max(0.f, std::min(1.f, currentSaturation));
        currentValue = std::max(0.f, std::min(1.f, currentValue));
        updateColorPicker();
    }
}

void SketchpadWindow::handleColorPickerDrag(sf::Vector2i mousePos) {
    if (!showingColorPicker) return;

    float hueX = colorPickerPosition.x + 20.f;
    float hueY = colorPickerPosition.y + 50.f;
    float hueHeight = 250.f;

    float svX = colorPickerPosition.x + 70.f;
    float svY = colorPickerPosition.y + 50.f;
    float svSize = 200.f;

    if (draggingHue) {
        float relativeY = mousePos.y - hueY;
        currentHue = (relativeY / hueHeight) * 360.f;
        currentHue = std::max(0.f, std::min(360.f, currentHue));
        updateColorPicker();
    }

    if (draggingSV) {
        float relativeX = mousePos.x - svX;
        float relativeY = mousePos.y - svY;
        currentSaturation = relativeX / svSize;
        currentValue = 1.0f - (relativeY / svSize);
        currentSaturation = std::max(0.f, std::min(1.f, currentSaturation));
        currentValue = std::max(0.f, std::min(1.f, currentValue));
        updateColorPicker();
    }
}

void SketchpadWindow::toggleFullscreen() {
    isFullscreen = !isFullscreen;
    sf::Image savedDrawing = canvas.getTexture().copyToImage();
    window.close();

    if (isFullscreen) {
        window.create(sf::VideoMode::getDesktopMode(), "SFML Sketchpad - JuggernautXL Ragnarok", sf::Style::Fullscreen);
        statusText.setString("Switched to Fullscreen (F11 = Windowed)");
    } else {
        window.create(sf::VideoMode(1280, 720), "SFML Sketchpad - JuggernautXL Ragnarok", sf::Style::Default);
        statusText.setString("Switched to Windowed (F11 = Fullscreen)");
    }

    canvas.create(window.getSize().x, window.getSize().y);
    canvas.clear(sf::Color::White);

    sf::Texture tempTexture;
    tempTexture.loadFromImage(savedDrawing);
    sf::Sprite tempSprite(tempTexture);

    float scaleX = static_cast<float>(canvas.getSize().x) / static_cast<float>(savedDrawing.getSize().x);
    float scaleY = static_cast<float>(canvas.getSize().y) / static_cast<float>(savedDrawing.getSize().y);
    float scale = std::min(scaleX, scaleY);

    tempSprite.setScale(scale, scale);
    tempSprite.setPosition(
        (static_cast<float>(canvas.getSize().x) - static_cast<float>(savedDrawing.getSize().x) * scale) / 2.f,
        (static_cast<float>(canvas.getSize().y) - static_cast<float>(savedDrawing.getSize().y) * scale) / 2.f
    );

    canvas.draw(tempSprite);
    canvas.display();
    canvasSprite.setTexture(canvas.getTexture());
}

void SketchpadWindow::enhanceWithAI() {
    if (isProcessing) return;
    isProcessing = true;
    statusText.setString("Capturing your drawing...");

    std::string projectPath = "C:\\Users\\mazen\\Desktop\\mazen\\c++\\aisketchpad\\";
    std::string drawingPath = projectPath + "drawing.png";

    sf::Image capture = canvas.getTexture().copyToImage();
    if (!capture.saveToFile(drawingPath)) {
        statusText.setString("Failed to save drawing.png");
        isProcessing = false;
        return;
    }

    statusText.setString("Running JuggernautXL Ragnarok AI... (please wait)");
    std::string result = callHuggingFaceAPI();

    if (result == "Success") {
        std::string outputPath = projectPath + "enhanced_output.png";
        if (enhancedTexture.loadFromFile(outputPath)) {
            enhancedSprite.setTexture(enhancedTexture);
            enhancedSprite.setScale(
                static_cast<float>(window.getSize().x) / static_cast<float>(enhancedTexture.getSize().x),
                static_cast<float>(window.getSize().y) / static_cast<float>(enhancedTexture.getSize().y)
            );
            showingEnhanced = true;
            statusText.setString("AI MASTERPIECE READY! | SPACE = toggle | ENTER = apply | ESC = cancel");
        } else {
            statusText.setString("Failed to load enhanced image");
        }
    } else {
        statusText.setString("AI failed - check console (Python/diffusers issue?)");
    }
    isProcessing = false;
}

std::string SketchpadWindow::callHuggingFaceAPI() {
    std::string projectPath = "C:\\Users\\mazen\\Desktop\\mazen\\c++\\aisketchpad\\";
    std::string scriptPath = projectPath + "enhance_local.py";
    std::string imagePath = projectPath + "drawing.png";
    std::string outputPath = projectPath + "enhanced_output.png";

    std::string command = "python \"" + scriptPath + "\" \"" + imagePath + "\"";
    std::cout << "\n=== JUGGERNAUTXL RAGNAROK ENHANCEMENT ===\n";
    std::cout << "Running: " << command << std::endl;

    int code = system(command.c_str());

    if (code == 0) {
        std::ifstream f(outputPath, std::ios::binary);
        if (f.is_open()) {
            char header[8] = {};
            f.read(header, 8);
            if (header[0] == '\x89' && header[1] == 'P' && header[2] == 'N' && header[3] == 'G') {
                std::cout << "SUCCESS: JuggernautXL Ragnarok generated a perfect image!\n" << std::endl;
                return "Success";
            }
        }
    }

    std::cout << "ERROR: Enhancement failed (code " << code << ")\n";
    return "Failed";
}

void SketchpadWindow::toggleView() {
    showingEnhanced = !showingEnhanced;
}

void SketchpadWindow::updateCustomCursor(sf::Vector2i mousePos) {
    // Only show custom cursor when in drawing area and not in menus
    if (mousePos.y > 100 && !showingColorPicker && !showingSizeControl && !showingEnhanced) {
        showCustomCursor = true;

        float cursorSize = 0.f;
        if (currentToolType == PEN) {
            cursorSize = penSize / 2.f;
        } else if (currentToolType == BRUSH) {
            cursorSize = brushSize2 / 2.f;
        } else if (currentToolType == ERASER) {
            cursorSize = 10.f; // Eraser size
        }

        customCursor.setRadius(cursorSize);
        customCursor.setPosition(
            static_cast<float>(mousePos.x) - cursorSize,
            static_cast<float>(mousePos.y) - cursorSize
        );

        // Set cursor color based on tool
        if (currentToolType == ERASER) {
            customCursor.setOutlineColor(sf::Color::Red);
        } else {
            customCursor.setOutlineColor(sf::Color::Black);
        }

        // Hide system cursor
        window.setMouseCursorVisible(false);
    } else {
        showCustomCursor = false;
        // Show system cursor
        window.setMouseCursorVisible(true);
    }
}

void SketchpadWindow::switchTool(ToolType type) {
    delete currentTool;
    currentToolType = type;
    switch(type) {
        case PEN:    currentTool = new PenTool(currentColor, penSize); break;
        case BRUSH:  currentTool = new BrushTool(currentColor, brushSize2); break;
        case ERASER: currentTool = new EraserTool(20); break;
    }
}

void SketchpadWindow::setColor(sf::Color color) {
    currentColor = color;
    if (currentToolType == PEN || currentToolType == BRUSH) {
        switchTool(currentToolType);
    }
}

void SketchpadWindow::run() {
    while (window.isOpen()) {
        processEvents();
        draw();
    }
}

void SketchpadWindow::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            if (showingColorPicker) {
                showingColorPicker = false;
                // Apply color to current tool
                if (currentToolType == PEN || currentToolType == BRUSH) {
                    switchTool(currentToolType);
                }
            } else if (showingSizeControl) {
                showingSizeControl = false;
                statusText.setString("Size control closed");
            } else if (showingEnhanced) {
                showingEnhanced = false;
                statusText.setString("Enhancement cancelled");
            } else {
                window.close();
            }
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i pos(event.mouseButton.x, event.mouseButton.y);

            if (showingColorPicker) {
                handleColorPickerClick(pos);
            } else if (showingSizeControl) {
                handleSizeControlClick(pos);
            } else if (pos.y <= 100) {
                int idx = -1;
                toolbar.handleClick(pos, idx);
            } else if (!showingEnhanced) {
                isDrawing = true;
                currentTool->onMouseDown(canvas, pos);
            }
        }

        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            draggingHue = false;
            draggingSV = false;
            draggingPenSize = false;
            draggingBrushSize = false;

            if (!showingColorPicker && !showingSizeControl) {
                toolbar.resetColors();
                isDrawing = false;
                currentTool->onMouseUp(canvas, sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            }
        }

        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2i pos(event.mouseMove.x, event.mouseMove.y);

            // Always update cursor position
            updateCustomCursor(pos);

            if (showingColorPicker && (draggingHue || draggingSV)) {
                handleColorPickerDrag(pos);
            } else if (showingSizeControl && (draggingPenSize || draggingBrushSize)) {
                handleSizeControlDrag(pos);
            } else if (isDrawing && !showingEnhanced && !showingColorPicker && !showingSizeControl) {
                toolbar.update(pos);
                currentTool->onMouseMove(canvas, pos);
            }
        }

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::R) {
                resetCanvas();
            }
            if (event.key.code == sf::Keyboard::S && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                saveCanvasAs();
            }
            else if (event.key.code == sf::Keyboard::S) {
                saveCanvas();
            }
            if (event.key.code == sf::Keyboard::O && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                openCanvas();
            }
            if (event.key.code == sf::Keyboard::A) {
                enhanceWithAI();
            }
            if (event.key.code == sf::Keyboard::F11) {
                toggleFullscreen();
            }
            if (event.key.code == sf::Keyboard::Space && showingEnhanced) {
                toggleView();
            }
            if (event.key.code == sf::Keyboard::Return && showingEnhanced) {
                sf::Image img = enhancedTexture.copyToImage();
                canvas.clear(sf::Color::White);
                sf::Texture tex;
                tex.loadFromImage(img);
                canvas.draw(sf::Sprite(tex));
                canvas.display();
                canvasSprite.setTexture(canvas.getTexture());
                showingEnhanced = false;
                statusText.setString("AI masterpiece applied to canvas!");
            }
        }
    }
}

void SketchpadWindow::draw() {
    window.clear(sf::Color(40, 40, 40));
    window.draw(showingEnhanced ? enhancedSprite : canvasSprite);
    window.setView(window.getDefaultView());
    toolbar.draw(window);

    // Draw the dynamic color display on top of the color button
    window.draw(currentColorDisplay);

    for (int i = 0; i < 4; i++) {
        window.draw(buttonLabels[i]);
    }

    if (showingColorPicker) {
        drawColorPicker();
    }

    if (showingSizeControl) {
        drawSizeControl();
    }

    // Draw custom cursor
    if (showCustomCursor) {
        window.draw(customCursor);
    }

    if (statusText.getString() != "") {
        window.draw(statusText);
    }
    window.display();
}

void SketchpadWindow::resetCanvas() {
    canvas.clear(sf::Color::White);
    canvas.display();
    canvasSprite.setTexture(canvas.getTexture());
    statusText.setString("Canvas cleared");
    showingEnhanced = false;
}

void SketchpadWindow::saveCanvas() {
    sf::Image img = canvas.getTexture().copyToImage();
    if (img.saveToFile("sketch.png")) {
        statusText.setString("Saved as sketch.png");
    } else {
        statusText.setString("Save failed!");
    }
}

std::string SketchpadWindow::openFileDialog(bool isSave) {
    OPENFILENAME ofn;
    char szFile[260] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = window.getSystemHandle();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "PNG Files\0*.png\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | (isSave ? OFN_OVERWRITEPROMPT : OFN_FILEMUSTEXIST);

    if (isSave) {
        ofn.lpstrTitle = "Save Drawing As";
        ofn.lpstrDefExt = "png";
        if (GetSaveFileName(&ofn) == TRUE) {
            return std::string(ofn.lpstrFile);
        }
    } else {
        ofn.lpstrTitle = "Open Drawing";
        if (GetOpenFileName(&ofn) == TRUE) {
            return std::string(ofn.lpstrFile);
        }
    }

    return "";
}

void SketchpadWindow::saveCanvasAs() {
    std::string filepath = openFileDialog(true);

    if (!filepath.empty()) {
        if (filepath.length() < 4 || filepath.substr(filepath.length() - 4) != ".png") {
            filepath += ".png";
        }

        sf::Image img = canvas.getTexture().copyToImage();
        if (img.saveToFile(filepath)) {
            statusText.setString("Saved to " + filepath);
            std::cout << "Saved to: " << filepath << std::endl;
        } else {
            statusText.setString("Failed to save!");
        }
    } else {
        statusText.setString("Save cancelled");
    }
}

void SketchpadWindow::openCanvas() {
    std::string filepath = openFileDialog(false);

    if (!filepath.empty()) {
        sf::Image img;
        if (img.loadFromFile(filepath)) {
            canvas.clear(sf::Color::White);

            sf::Texture loadedTexture;
            loadedTexture.loadFromImage(img);
            sf::Sprite loadedSprite(loadedTexture);

            float scaleX = static_cast<float>(canvas.getSize().x) / static_cast<float>(img.getSize().x);
            float scaleY = static_cast<float>(canvas.getSize().y) / static_cast<float>(img.getSize().y);
            float scale = std::min(scaleX, scaleY);

            loadedSprite.setScale(scale, scale);

            loadedSprite.setPosition(
                (static_cast<float>(canvas.getSize().x) - static_cast<float>(img.getSize().x) * scale) / 2.f,
                (static_cast<float>(canvas.getSize().y) - static_cast<float>(img.getSize().y) * scale) / 2.f
            );

            canvas.draw(loadedSprite);
            canvas.display();
            canvasSprite.setTexture(canvas.getTexture());

            statusText.setString("Opened: " + filepath);
            std::cout << "Opened: " << filepath << std::endl;
        } else {
            statusText.setString("Failed to open file!");
        }
    } else {
        statusText.setString("Open cancelled");
    }
}
