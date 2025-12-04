#ifndef CONFIG_H
#define CONFIG_H

#include <SFML/Graphics.hpp>

namespace conf {
	namespace colors {
		inline sf::Color backgroundCol = sf::Color(50, 50, 50);
		inline sf::Color positiveColor = sf::Color(0, 0, 200);
		inline sf::Color negativeColor = sf::Color(200, 0, 0);
		inline sf::Color standardLineColor = sf::Color(255, 255, 255);
		inline sf::Color hoverOutlineColor = sf::Color(20, 20, 20);
	}

	namespace dimensions {
		inline const sf::Vector2u windowDims = { 1600, 900 };
		inline float pointChargeRadius = 13.f;
		inline float pointChargeOutlineThickness = 2.f;
	}

	namespace bounds {
		inline float minMaxCharge = 10.f;
	}

	namespace calculator {
		inline int worldUnitsPerVal = 2;
		inline float worldUnitsPerUnit = 100.f;
		inline const float coulombConstant = 1.f;
	}
}


#endif