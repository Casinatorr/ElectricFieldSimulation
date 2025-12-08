#ifndef CALCULATOR_H

#define CALCULATOR_H

#include <SFML/Graphics.hpp>
#include <array>

void CalculateGrids();

void CalculateEquipotLines(sf::Vector2f pos, sf::RenderWindow* window);
#endif