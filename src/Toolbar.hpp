#pragma once
#include "Button.hpp"
#include <vector>
#include <memory>

class Toolbar {
private:
    std::vector<std::unique_ptr<Button>> buttons;
    int selectedToolIndex;
    int selectedColorIndex;

public:
    Toolbar() : selectedToolIndex(-1), selectedColorIndex(-1) {}

    void addButton(std::unique_ptr<Button> b) {
        buttons.push_back(std::move(b));
    }

    void setSelectedTool(int index) {
        // Determine if this is a tool (0-2) or color (3-10)
        if (index >= 3 && index <= 10) {
            // It's a color - deselect previous color only
            if (selectedColorIndex >= 3 && selectedColorIndex <= 10) {
                buttons[selectedColorIndex]->setSelected(false);
            }
            selectedColorIndex = index;
            buttons[index]->setSelected(true);
        }
        else if (index >= 0 && index < 3) {
            // It's a tool - deselect previous tool only
            if (selectedToolIndex >= 0 && selectedToolIndex < 3) {
                buttons[selectedToolIndex]->setSelected(false);
            }
            selectedToolIndex = index;
            buttons[index]->setSelected(true);
        }
    }

    void update(sf::Vector2i mousePos) {
        for (auto& b : buttons)
            b->update(mousePos);
    }

    void draw(sf::RenderWindow& window) {
        // Draw toolbar background
        sf::RectangleShape toolbarBg({window.getSize().x, 108.f});
        toolbarBg.setPosition(0, 0);
        toolbarBg.setFillColor(sf::Color(30, 30, 30));
        window.draw(toolbarBg);

        for (auto& b : buttons)
            b->draw(window);
    }

    void handleClick(sf::Vector2i pos, int& clickedIndex) {
        clickedIndex = -1;
        for (int i = 0; i < buttons.size(); i++) {
            buttons[i]->handleClick(pos);
        }
    }

    void resetColors() {
        for (auto& b : buttons)
            b->resetColor();
    }
};
