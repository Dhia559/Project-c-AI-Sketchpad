#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <functional>
class Tool {
protected:
    sf::Vector2i lastPos;
    bool hasLastPos;

public:
    Tool() : lastPos(0, 0), hasLastPos(false) {}
    virtual ~Tool() {}

    virtual void onMouseDown(sf::RenderTexture& canvas, sf::Vector2i pos) = 0;
    virtual void onMouseMove(sf::RenderTexture& canvas, sf::Vector2i pos) = 0;
    virtual void onMouseUp(sf::RenderTexture& canvas, sf::Vector2i pos) = 0;

    // Helper function to interpolate between two points
    void interpolate(sf::RenderTexture& canvas, sf::Vector2i from, sf::Vector2i to,
                     std::function<void(sf::Vector2i)> drawFunc) {
        float dx = to.x - from.x;
        float dy = to.y - from.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        int steps = static_cast<int>(distance / 2.0f); // Draw every 2 pixels
        if (steps < 1) steps = 1;

        for (int i = 0; i <= steps; i++) {
            float t = static_cast<float>(i) / steps;
            sf::Vector2i pos(
                from.x + static_cast<int>(dx * t),
                from.y + static_cast<int>(dy * t)
            );
            drawFunc(pos);
        }
    }
};
