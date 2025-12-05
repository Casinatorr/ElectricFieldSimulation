#include "charge.h"
#include "calculator.h"
#include "renderingRegistry.h"
#include <SFML/Graphics.hpp>
#include "configuration.h"
#include "programState.h"
#include <iostream>


std::vector<sf::Vector2f> grid;
int numValsX = 0;
int numValsY = 0;

void SetGridVal(int x, int y, sf::Vector2f& val) {
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
	return 0;
}

sf::Vector2f ToPhysicsPos(sf::Vector2f& point) {
	return { point.x * conf::calculator::worldUnitsPerUnit, point.y * conf::calculator::worldUnitsPerUnit };
}

sf::Color ColorFromDirection(sf::Vector2f& pos) {
	if (pos.lengthSquared() == 0)
		return sf::Color::Black;
	auto norm = pos.normalized();
	return sf::Color(255*(0.5*(1+norm.x)), 255*(0.5*(1+norm.y)), 255, 255*pos.length()/(1e-7));
}

void CalulculateGrid() {

	sf::Vector2f startPos = { 0, 0 };
	int offset = conf::calculator::worldUnitsPerVal;
	numValsX = conf::dimensions::windowDims.x / offset;
	numValsY = conf::dimensions::windowDims.y / offset;

	std::shared_ptr<sf::Image> retImg = std::make_shared<sf::Image>(sf::Vector2u({static_cast<uint32_t>(numValsX * offset), static_cast<uint32_t>(numValsY * offset)}));

	grid = std::vector<sf::Vector2f>(numValsX * numValsY, {0, 0 });
	float highestVal = 0;
	for (auto c : chargeRegistry) {
		for (int x = 0; x < numValsX; x++) {
			for (int y = 0; y < numValsY; y++) {
				sf::Vector2f pos = (startPos + sf::Vector2f({ static_cast<float>(offset * x), static_cast<float>(offset * y )}));
				if (!c->IsInside(pos))
					AddGridVal(x, y, c->GetFieldAt(ToPhysicsPos(pos)));

				if (x != 0 && y != 0) {
					//interpolation magic
					float offsetNormalised = 1.f / float(offset);
					for (int sub_x = 0; sub_x < offset; sub_x++) {
						for (int sub_y = 0; sub_y < offset; sub_y++) {

							float ix = sub_x * offsetNormalised;
							float iy = sub_y * offsetNormalised;
							sf::Vector2f val = (1.f - iy) * ((1.f - ix) * GetGridVal(x - 1, y - 1) + ix * GetGridVal(x, y-1)
								+ iy*((1-ix)*GetGridVal(x-1, y) + ix*GetGridVal(x, y)));
							retImg->setPixel({ static_cast<uint32_t>(offset * x + sub_x), static_cast<uint32_t>(offset * y + sub_y) }, ColorFromDirection(val));
							if (val.length() > highestVal)
								highestVal = val.length();
						}
					}
				}
			}
		}
	}
	std::cout << "highest value: " << highestVal << "\n";

	State::currentFieldImage = retImg;
	State::currentFieldTexture = std::make_shared<sf::Texture>();
	State::currentFieldTexture->loadFromImage(*retImg);
	State::currentFieldSprite = std::make_shared<sf::Sprite>(*State::currentFieldTexture);

}

