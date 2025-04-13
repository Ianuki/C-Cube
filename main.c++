#include <iostream>
#include <SFML/Graphics.hpp>
#include "vector3.hpp"

const int FOCAL_LENGTH = 300;

vector3<float> block_position(0, 0, 0);
vector3<float> block_size(35, 35, 35);

vector3<float> light_direction(0, 0, 1);
vector3<float> camera_position(0, 0, -150);
vector3<float> camera_rotation(0, 0, 0);

std::vector<vector3<float>> points = {
    vector3<float>(1, -1, -1),
    vector3<float>(-1, -1, -1),
    vector3<float>(1, 1, -1),
    vector3<float>(-1, 1, -1),
    vector3<float>(1, -1, 1),
    vector3<float>(-1, -1, 1),
    vector3<float>(1, 1, 1),
    vector3<float>(-1, 1, 1) 
};

// Point Indices
struct face { 
    int a, b, c; 
    face(int _a, int _b, int _c) { a = _a; b = _b; c = _c; }
};

std::vector<face> faces = {
    face(0, 1, 2),
    face(3, 2, 1),
    face(6, 5, 4),
    face(5, 6, 7),
    face(0, 2, 4),
    face(6, 4, 2),
    face(1, 5, 7),
    face(7, 3, 1),

    face(4, 1, 0),
    face(1, 4, 5),
    face(2, 3, 6),
    face(7, 6, 3),
};

/*
    face(4, 5, 6),
    face(7, 6, 5),
    face(0, 2, 4),
    face(6, 4, 2),
    face(7, 5, 1),
    face(1, 3, 7),
    face(0, 1, 4),
    face(1, 4, 5),
    face(2, 3, 6),
    face(3, 6, 7),

    vector3<float> normal = (a - c).cross((b - c));
    float light_factor = normal.normalize().dot(light_direction) * -255;
    convex.setFillColor(sf::Color(light_factor, light_factor, light_factor));
*/

sf::Vector2f project_point(vector3<float> point, sf::Vector2u window_size) {
    return sf::Vector2f(
        (point[0] * block_size[0]) * FOCAL_LENGTH / ((point[2] * block_size[2]) - camera_position[2]) + window_size.x / 2,
        (point[1] * block_size[1]) * FOCAL_LENGTH / ((point[2] * block_size[2]) - camera_position[2]) + window_size.y / 2
    );
}

vector3<float> rotate_point_y(vector3<float> point, float rotation) {
    return vector3<float>(
        point[0] * cos(rotation) + point[2] * sin(rotation),
        point[1],
        point[0] * -sin(rotation) + point[2] * cos(rotation)
    );
}

vector3<float> rotate_point_x(vector3<float> point, float rotation) {
    return vector3<float>(
        point[0],
        point[1] * cos(rotation) + point[2] * -sin(rotation),
        point[1] * sin(rotation) + point[2] * cos(rotation)
    );
}

int main(int argc, char** argv) {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8.0;
    sf::RenderWindow window(sf::VideoMode(), "a fucking cube", sf::Style::Fullscreen, settings);
    window.setFramerateLimit(60);

    float rotation = 0;

    while (window.isOpen()) {
        window.clear(sf::Color::Black);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseWheelMoved) {
                camera_position += vector3<float>(0, 0, 10 * -event.mouseWheel.delta);
            }
        }

        sf::ConvexShape convex;
        convex.setPointCount(3);
        for (face quad : faces) {
            vector3<float> a = rotate_point_y(points[quad.a], rotation);
            vector3<float> b = rotate_point_y(points[quad.b], rotation);
            vector3<float> c = rotate_point_y(points[quad.c], rotation);
            a = rotate_point_x(a, rotation / 2);
            b = rotate_point_x(b, rotation / 2);
            c = rotate_point_x(c, rotation / 2);

            convex.setFillColor(sf::Color(quad.a * 20, quad.b * 30, quad.c * 20));
            convex.setPoint(0, project_point(a, window.getSize()));
            convex.setPoint(1, project_point(b, window.getSize()));
            convex.setPoint(2, project_point(c, window.getSize()));

            vector3<float> normal = (a - c).cross((b - c));
            float light_factor = normal.normalize().dot(light_direction);
            convex.setFillColor(sf::Color(light_factor * -255, light_factor * -255, light_factor * -255));

            if (light_factor < 0) {
                window.draw(convex);
            }
        }
        
        rotation += 0.025;

        window.display();
    }

    return 0;
}
