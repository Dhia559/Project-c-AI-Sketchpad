#pragma once
#include "Tool.hpp"

class EraserTool : public Tool {
private:
    float size;

    void eraseAt(sf::RenderTexture& canvas, sf::Vector2i pos) {
        sf::CircleShape dot(size);
        dot.setFillColor(sf::Color::White);
        dot.setPosition(pos.x - size, pos.y - size);
        canvas.draw(dot);
    }

public:
    EraserTool(float s = 20) : size(s) {}

    void onMouseDown(sf::RenderTexture& canvas, sf::Vector2i pos) override {
        eraseAt(canvas, pos);
        lastPos = pos;
        hasLastPos = true;
        canvas.display();
    }

    void onMouseMove(sf::RenderTexture& canvas, sf::Vector2i pos) override {
        if (hasLastPos) {
            // Interpolate between last position and current position
            interpolate(canvas, lastPos, pos, [this, &canvas](sf::Vector2i p) {
                eraseAt(canvas, p);
            });
        } else {
            eraseAt(canvas, pos);
        }
        lastPos = pos;
        hasLastPos = true;
        canvas.display();
    }

    void onMouseUp(sf::RenderTexture&, sf::Vector2i) override {
        hasLastPos = false;
    }
};
