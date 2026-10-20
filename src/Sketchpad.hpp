#pragma once
#include <SFML/Graphics.hpp>
#include "Tool.hpp"
#include "PenTool.hpp"
#include "BrushTool.hpp"
#include "EraserTool.hpp"
#include "Toolbar.hpp"
#include <string>

class SketchpadWindow {
public:
    SketchpadWindow();
    void run();

private:
    sf::RenderWindow window;
    sf::RenderTexture canvas;
    sf::Sprite canvasSprite;

    sf::Color currentColor;
    float brushSize;
    bool isDrawing;
    Tool* currentTool;
    Toolbar toolbar;
    int currentToolIndex;
    enum ToolType { PEN, BRUSH, ERASER };
    ToolType currentToolType;

    // AI enhancement
    bool isProcessing;
    sf::Text statusText;
    sf::Font font;
    sf::Texture enhancedTexture;
    sf::Sprite enhancedSprite;
    bool showingEnhanced;

    // Fullscreen toggle
    bool isFullscreen;

    // Button labels for action buttons (CLEAR, SAVE, OPEN, AI)
    sf::Text buttonLabels[4];

    // Current color display button (will update with selected color)
    sf::RectangleShape currentColorDisplay;

    // Color picker
    sf::RectangleShape colorPickerBox;
    sf::RectangleShape hueSlider;
    sf::RectangleShape saturationBox;
    sf::RectangleShape colorPreview;
    bool showingColorPicker;
    float currentHue;
    float currentSaturation;
    float currentValue;
    sf::Vector2f colorPickerPosition;
    bool draggingHue;
    bool draggingSV;

    // Brush size control
    bool showingSizeControl;
    sf::Vector2f sizeControlPosition;
    float penSize;
    float brushSize2;
    bool draggingPenSize;
    bool draggingBrushSize;

    // Custom cursor
    sf::CircleShape customCursor;
    bool showCustomCursor;

    void processEvents();
    void draw();
    void resetCanvas();
    void saveCanvas();
    void saveCanvasAs();
    void openCanvas();
    void setColor(sf::Color color);
    void switchTool(ToolType type);
    void enhanceWithAI();
    std::string callHuggingFaceAPI();
    void toggleView();
    void toggleFullscreen();
    std::string openFileDialog(bool isSave);

    // Color picker functions
    void toggleColorPicker();
    void updateColorPicker();
    void drawColorPicker();
    sf::Color hsvToRgb(float h, float s, float v);
    void handleColorPickerClick(sf::Vector2i mousePos);
    void handleColorPickerDrag(sf::Vector2i mousePos);

    // Size control functions
    void toggleSizeControl();
    void drawSizeControl();
    void handleSizeControlClick(sf::Vector2i mousePos);
    void handleSizeControlDrag(sf::Vector2i mousePos);
    void updateToolSize();
    void updateCustomCursor(sf::Vector2i mousePos);
};
