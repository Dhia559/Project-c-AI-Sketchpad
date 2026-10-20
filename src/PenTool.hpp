#pragma once
#include "Tool.hpp"

class PenTool : public Tool {
private:
    sf::Color color;
    float size;

    void drawCircleAt(sf::RenderTexture& canvas, sf::Vector2i pos) {
        sf::CircleShape dot(size);
        dot.setFillColor(color);
        dot.setPosition(pos.x - size, pos.y - size);
        canvas.draw(dot);
    }

public:
    PenTool(sf::Color c = sf::Color::Black, float s = 3) : color(c), size(s) {}

    void onMouseDown(sf::RenderTexture& canvas, sf::Vector2i pos) override {
        drawCircleAt(canvas, pos);
        lastPos = pos;
        hasLastPos = true;
        canvas.display();
    }

    void onMouseMove(sf::RenderTexture& canvas, sf::Vector2i pos) override {
        if (hasLastPos) {
            // Interpolate between last position and current position
            interpolate(canvas, lastPos, pos, [this, &canvas](sf::Vector2i p) {
                drawCircleAt(canvas, p);
            });
        } else {
            drawCircleAt(canvas, pos);
        }
        lastPos = pos;
        hasLastPos = true;
        canvas.display();
    }

    void onMouseUp(sf::RenderTexture&, sf::Vector2i) override {
        hasLastPos = false;
    }
};
