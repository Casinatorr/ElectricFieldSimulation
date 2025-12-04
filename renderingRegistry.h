#ifndef RENDERING_REGISTRY_H

#define RENDERING_REGISTRY_H

#include <SFML/Graphics.hpp>
#include "charge.h"

inline std::vector<std::shared_ptr<Charge>> chargeRegistry;

void RegisterCharge(std::shared_ptr<Charge>& charge);

void UnregisterCharge(std::shared_ptr<Charge>& charge);
void Draw(sf::RenderWindow* window);

#endif