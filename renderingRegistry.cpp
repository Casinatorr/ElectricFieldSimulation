#include "renderingRegistry.h"

void RegisterCharge(std::shared_ptr<Charge>& charge) {
	chargeRegistry.push_back(charge);
}

void UnregisterCharge(std::shared_ptr<Charge>& charge) {
	chargeRegistry.erase(std::remove(chargeRegistry.begin(), chargeRegistry.end(), charge), chargeRegistry.end());
}


void Draw(sf::RenderWindow* window) {
	for (auto c : chargeRegistry) {
		c->Draw(window);
	}
}