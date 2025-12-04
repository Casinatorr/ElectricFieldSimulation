#include "charge.h"
#include "calculator.h"
#include "renderingRegistry.h"
#include <SFML/Graphics.hpp>
#include "configuration.h"


std::vector<sf::Vector2f> grid;
int numValsX = 0;
int numValsY = 0;

void SetGridVal(int x, int y, sf::Vector2f val) {
	int index = numValsX * y + x;
	grid[index] = val;
}

sf::Vector2f GetGridVal(int x, int y) {
	int index = numValsX * y + x;
	return grid[index];
}

float AddGridVal(int x, int y, sf::Vector2f val) {
	int index = numValsX * y + x;
	grid[index] += val;
}

sf::Vector2f ToPhysicsPos(sf::Vector2f point) {
	return { point.x * conf::calculator::worldUnitsPerUnit, point.y * conf::calculator::worldUnitsPerUnit };
}

void CalulculateGrid() {


	sf::Vector2f startPos = { 0, 0 };
	int offset = conf::calculator::worldUnitsPerVal;
	numValsX = conf::dimensions::windowDims.x / offset;
	numValsY = conf::dimensions::windowDims.y / offset;

	sf::Image retImg(sf::Vector2u({numValsX * offset, numValsY * offset}));

	grid = std::vector<sf::Vector2f>(numValsX * numValsY, {0, 0 });
	for (auto c : chargeRegistry) {
		for (int x = 0; x < numValsX; x++) {
			for (int y = 0; y < numValsY; y++) {
				sf::Vector2f pos = (startPos + sf::Vector2f({ offset * x, offset * y }));
				if (!c->IsInside(pos))
					AddGridVal(x, y, c->GetFieldAt(ToPhysicsPos(pos)));

				if (x != 0 && y != 0) {
					//interpolation magic
					float offsetNormalised = 1.f / float(offset);
				}
			}
		}
	}
}

