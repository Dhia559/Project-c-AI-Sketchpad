#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <iostream>

class Button {
private:
    sf::RectangleShape shape;
    sf::Sprite iconSprite;
    sf::Texture iconTexture;
    sf::Text label;
    sf::Font* font;
    std::function<void()> onClick;
    bool hasIcon;
    bool isHovered;
    bool isSelected;  // Move this BEFORE the colors to match initialization order

    // Colors
    sf::Color normalColor;
    sf::Color hoverColor;
    sf::Color clickColor;
    sf::Color selectedColor;

public:
    Button(sf::Vector2f pos, sf::Vector2f size, const std::string& iconPath, std::function<void()> callback)
        : font(nullptr), hasIcon(false), isHovered(false), isSelected(false) {  // Initialize in declaration order

        // Modern colors
        normalColor = sf::Color(255, 255, 255);      // Dark gray
        hoverColor = sf::Color(62, 62, 66);       // Lighter gray
        clickColor = sf::Color(0, 122, 204);      // Blue accent
        selectedColor = sf::Color(0, 122, 204);   // Blue for selected

        shape.setPosition(pos);
        shape.setSize(size);
        shape.setFillColor(normalColor);
        shape.setOutlineThickness(0);

        // Load icon
        if (!iconTexture.loadFromFile(iconPath)) {
            std::cerr << "ERROR: Could not load icon: " << iconPath << std::endl;
            shape.setFillColor(sf::Color::Red);
            onClick = callback;
            return;
        }

        std::cout << "SUCCESS: Loaded " << iconPath << std::endl;
        hasIcon = true;
        iconSprite.setTexture(iconTexture);

        // scale icon to fit button
        float targetSize = std::min(size.x, size.y) * 0.6f;
        float scaleX = targetSize / iconTexture.getSize().x;
        float scaleY = targetSize / iconTexture.getSize().y;
        float scale = std::min(scaleX, scaleY);
        iconSprite.setScale(scale, scale);

        // center icon
        iconSprite.setPosition(
            pos.x + (size.x - iconTexture.getSize().x * iconSprite.getScale().x) / 2,
            pos.y + (size.y - iconTexture.getSize().y * iconSprite.getScale().y) / 2
        );

        onClick = callback;
    }

    // Constructor for color buttons (no icon)
    Button(sf::Vector2f pos, sf::Vector2f size, sf::Color color, std::function<void()> callback)
        : font(nullptr), hasIcon(false), isHovered(false), isSelected(false) {

        normalColor = color;
        hoverColor = sf::Color(
            std::min(color.r + 30, 255),
            std::min(color.g + 30, 255),
            std::min(color.b + 30, 255)
        );
        clickColor = sf::Color::White;
        selectedColor = color;

        shape.setPosition(pos);
        shape.setSize(size);
        shape.setFillColor(normalColor);
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color(200, 200, 200));

        onClick = callback;
    }

    void setSelected(bool selected) {
        isSelected = selected;
        if (selected) {
            shape.setOutlineThickness(3);
            shape.setOutlineColor(sf::Color(0, 122, 204));
        } else {
            shape.setOutlineThickness(0);
        }
    }

    void update(sf::Vector2i mousePos) {
        bool wasHovered = isHovered;
        isHovered = shape.getGlobalBounds().contains(mousePos.x, mousePos.y);

        if (isSelected) {
            return; // Don't change color if selected
        }

        // Only change color if hover state changed
        if (isHovered && !wasHovered) {
            shape.setFillColor(hoverColor);
        } else if (!isHovered && wasHovered) {
            shape.setFillColor(normalColor);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        if (hasIcon) {
            window.draw(iconSprite);
        }
    }

    void handleClick(sf::Vector2i mousePos) {
        if (shape.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            onClick();
        }
    }

    void resetColor() {
        if (isSelected) {
            shape.setFillColor(selectedColor);
        } else if (!isHovered) {
            shape.setFillColor(normalColor);
        } else {
            shape.setFillColor(hoverColor);
        }
    }
};
