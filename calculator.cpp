#include "charge.h"
#include "calculator.h"
#include "renderingRegistry.h"
#include <SFML/Graphics.hpp>
#include "configuration.h"
#include "programState.h"
#include <iostream>
#include <limits>
#include <imgui-SFML.h>
#include <imgui.h>


std::vector<float> finePotentialGrid;
std::vector<sf::Vector2f> fineElectricGrid;

int imgDimsX;
int imgDimsY;
int numValsX = 0;
int numValsY = 0;


sf::Vector2f ToPhysicsPos(sf::Vector2f& point) {
	return { point.x * conf::calculator::worldUnitsPerUnit, point.y * conf::calculator::worldUnitsPerUnit };
}

sf::Color ColorFromDirection(sf::Vector2f& pos) {
	if (pos.lengthSquared() == 0)
		return sf::Color::Black;
	auto norm = pos.normalized();
	return sf::Color(255*(0.5*(1+norm.x)), 255*(0.5*(1+norm.y)), 255, 255);
	//return sf::Color(255 * norm.x, 255 * norm.y, 255);
}

sf::Color ColorFromFloat(float f) {
	float s = 1.f / (1.f + exp(-3*f + 2));
	return sf::Color(s*255, s * 255, s * 255);
}

void CalculateGrids() {
	sf::Vector2f startPos = { 0, 0 };
	int stepSize = conf::calculator::worldUnitsPerVal;
	
	numValsX = conf::dimensions::windowDims.x / stepSize;
	numValsY = conf::dimensions::windowDims.y / stepSize;

	imgDimsX = numValsX * stepSize;
	imgDimsY = numValsY * stepSize;

	sf::Vector2u imgDims({ static_cast<uint32_t>(imgDimsX), static_cast<uint32_t>(imgDimsY) });

	std::shared_ptr<sf::Image> retImg = std::make_shared<sf::Image>(imgDims);

	finePotentialGrid = std::vector<float>(imgDimsX*imgDimsY);
	fineElectricGrid = std::vector<sf::Vector2f>(imgDimsX * imgDimsY);

	std::vector<float> potentialGrid(numValsX * numValsY);
	std::vector<sf::Vector2f> electricGrid(numValsX * numValsY);


	float highestPixelPot = -std::numeric_limits<float>::max();
	float lowestPixelPot = std::numeric_limits<float>::max();

	float longestSqrVector = 0;
	
	for (int x = 0; x < numValsX; x++) {
		for (int y = 0; y < numValsY; y++) {
			int currentGridIndex = numValsX * y + x;
			sf::Vector2f pos = (startPos + sf::Vector2f({ static_cast<float>(stepSize * x), static_cast<float>(stepSize * y) }));

			for (auto charge : chargeRegistry) {
				if (!charge->PhysicsIsInside(pos)) {
					potentialGrid[currentGridIndex] += charge->GetPotentialAt(ToPhysicsPos(pos));
					electricGrid[currentGridIndex] += charge->GetFieldAt(ToPhysicsPos(pos));
				}
			}
			//interpolation
			if (x == 0 || y == 0)
				continue;
			//i00 i10
			//i01 i11
			
			
			float f00 = potentialGrid[numValsX * (y - 1) + x - 1];
			float f10 = potentialGrid[numValsX * (y - 1) + x];
			float f01 = potentialGrid[numValsX * y + x - 1];
			float f11 = potentialGrid[currentGridIndex];

			sf::Vector2f v00 = electricGrid[numValsX * (y - 1) + x - 1];
			sf::Vector2f v10 = electricGrid[numValsX * (y - 1) + x];
			sf::Vector2f v01 = electricGrid[numValsX * y + x - 1];
			sf::Vector2f v11 = electricGrid[currentGridIndex];

			float offsetNormalised = 1.f / float(stepSize);
			for (int sub_x = 0; sub_x < stepSize; sub_x++) {
				for (int sub_y = 0; sub_y < stepSize; sub_y++) {

					float ix = sub_x * offsetNormalised;
					float iy = sub_y * offsetNormalised;

					float pot_val = (1 - iy) * ((1 - ix) * f00 + ix * f01) + iy * ((1 - ix) * f10 + ix * f11);
					sf::Vector2f field_val = (1.f - iy) * ((1 - ix) * v00 + ix * v01) + iy * ((1.f - ix) * v10 + ix * v11);
					if (pot_val > highestPixelPot)
						highestPixelPot = pot_val;
					if (pot_val < lowestPixelPot)
						lowestPixelPot = pot_val;
					if (field_val.lengthSquared() > longestSqrVector)
						longestSqrVector = field_val.lengthSquared();
					finePotentialGrid[numValsX * stepSize * (stepSize * (y - 1) + sub_y) + stepSize * (x - 1) + sub_x] = pot_val;
					fineElectricGrid[numValsX * stepSize * (stepSize * (y - 1) + sub_y) + stepSize * (x - 1) + sub_x] = field_val;
					
				}
			}
			
		}
	}



	std::cout << "highest pixel val: " << highestPixelPot << "\n";
	std::cout << "lowest pixel val: " << lowestPixelPot << "\n";
	float pixelPotRange = highestPixelPot - lowestPixelPot;
	std::cout << "range: " << pixelPotRange << "\n";

	for (int x = 0; x < imgDimsX; x++) {
		for (int y = 0; y < imgDimsY; y++) {
			float normalizedVal = (finePotentialGrid[imgDimsX * y + x] - lowestPixelPot) / pixelPotRange;
			float sigmoid = 1.f / (1.f + exp(-8.f * (normalizedVal - 0.5f)));
			normalizedVal = floor(sigmoid * 50.0f) / 49.0f;
			retImg->setPixel({ static_cast<uint32_t>(x), static_cast<uint32_t>(y) }, sf::Color(normalizedVal * 255, normalizedVal * 255, normalizedVal * 255));
		}
	}

	State::currentFieldImage = retImg;
	State::currentFieldTexture = std::make_shared<sf::Texture>();
	if (!State::currentFieldTexture->loadFromImage(*retImg)) {
		std::cout << "loading into image failed" << "\n";
	}
	State::currentFieldSprite = std::make_shared<sf::Sprite>(*State::currentFieldTexture);
}

bool IsInsideGrid(int x, int y) {
	return 0 <= x && x < imgDimsX && 0 <= y && y < imgDimsY;
}

int NearestGridIndex(sf::Vector2f pos) {
	int x = round(pos.x);
	int y = round(pos.y);
	if (IsInsideGrid(x, y))
		return imgDimsX * y + x;
	return -1;
}

sf::Vector2f GradientPerpendicular(int gridIndex) {
	sf::Vector2f field = fineElectricGrid[gridIndex];
	return sf::Vector2f(-field.y, field.x);
}

sf::Vector2f EquipotNextOffset(int gridIndex) {
	auto perp = GradientPerpendicular(gridIndex);
	//step size
	//we try fixed, 2px 
	if (perp.lengthSquared() == 0)
		return sf::Vector2f({ -1, -1 });
	return perp.normalized() * 2.f;
}

int FineGridIndex(int x, int y) {
	return y * imgDimsX + x;
}

sf::Vector2f FineGridPosition(int index) {
	int x = index % imgDimsX;
	int y = index / imgDimsX;
	return { float(x), float(y) };
}

sf::Vector2i FineGridPositionDisc(int index) {
	int x = index % imgDimsX;
	int y = index / imgDimsX;
	return { x, y };
}

float Dot(sf::Vector2f a, sf::Vector2f b) {
	return a.x * b.x + a.y * b.y;
}

bool IsOnGridEdge(sf::Vector2i pos) {
	return (pos.x % (imgDimsX - 1) == 0) || (pos.y % (imgDimsY - 1) == 0);
}

//maybe use arrays
std::vector<int> GetNeighborIndices(int gridIndex) {
	return {gridIndex-1, gridIndex+1, gridIndex+imgDimsX, gridIndex-imgDimsX, gridIndex+imgDimsX-1, gridIndex+imgDimsX+1, gridIndex-imgDimsX-1, gridIndex-imgDimsX+1};
}


void DrawThickLine(sf::RenderWindow* window, sf::Vector2f start, sf::Vector2f end, float thickness, sf::Color color) {
	// Compute direction and length
	sf::Vector2f direction = end - start;
	float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	if (length == 0)
		return;

	// Normalize direction
	sf::Vector2f unitDir = direction / length;

	// Compute angle in degrees
	float angle = std::atan2(direction.y, direction.x);

	// Create rectangle
	sf::RectangleShape line(sf::Vector2f(length, thickness));
	line.setPosition(start);
	line.setRotation(sf::radians(angle));
	line.setFillColor(color);

	// Center thickness around the line
	line.setOrigin({ 0.f, thickness / 2.f });

	window->draw(line);
}


void CalculateEquipotLines(sf::Vector2f pos, sf::RenderWindow* window) {
	sf::Vector2f currentPos = pos;

	std::vector<sf::Vector2f> points;


	int numPoints = 5000;

	int gridIndex = NearestGridIndex(currentPos);



	if (gridIndex < 0)
		return;

	float potential = finePotentialGrid[gridIndex];

	points.push_back(currentPos);
	int lastGridIndex = -1;

	//right side
	for (int i = 0; i < numPoints; i++) {
		auto discPos = FineGridPositionDisc(gridIndex);
		if (IsOnGridEdge(discPos))
			break;

		auto neighbors = GetNeighborIndices(gridIndex);
		float bestScore = std::numeric_limits<float>::max();
		int bestIndex = -1;
		auto perpGradient = GradientPerpendicular(gridIndex);
		for (auto nIndex : neighbors) {
			if (nIndex == lastGridIndex)
				continue;
			auto nPot = finePotentialGrid[nIndex];
			float deltaV = fabs(nPot - potential);
			auto offset = FineGridPosition(nIndex) - currentPos;
			float score = conf::calculator::deltaVWeighting*deltaV - 1.f * (Dot(offset.normalized(), perpGradient));
			if (score < bestScore) {
				bestIndex = nIndex;
				bestScore = score;
			}
		}
		lastGridIndex = gridIndex;
		gridIndex = bestIndex;
		currentPos = FineGridPosition(gridIndex);
		points.push_back(currentPos);
	}

	std::vector<sf::Vector2f> smoothed;
	for (size_t i = 1; i < points.size() - 1; i++) {
		sf::Vector2f avg = (points[i - 1] + points[i] + points[i + 1]) / 3.f;
		smoothed.push_back(avg);
	}

	if (smoothed.size() < 2)
		return;

	for (int i = 0; i < smoothed.size() - 1; i++) {
		DrawThickLine(window, smoothed[i], smoothed[i + 1], 2.f, sf::Color::White);
	}

	/*
	sf::VertexArray va(sf::PrimitiveType::LineStrip, smoothed.size());
	for (size_t i = 0; i < smoothed.size(); i++) {
		va[i].position = smoothed[i];
		va[i].color = sf::Color::White;
	}
	window->draw(va);
	*/
}

/*
void CalculateEquipotLines(sf::Vector2f pos, sf::RenderWindow* window) {
	sf::Vector2f currentPos = pos;

	std::vector<sf::Vector2f> points;

	int numPoints = 5000;

	int gridIndex = NearestGridIndex(currentPos);



	if (gridIndex < 0)
		return;

	float potential = finePotentialGrid[gridIndex];

	points.push_back(currentPos);

	float constexpr firstStepDst = 2.f;
	float constexpr secondStepMaxDst = 2.f;
	
	for (int i = 0; i < numPoints; i++) {
		auto gradientPerp = GradientPerpendicular(gridIndex);
		if (gradientPerp.lengthSquared() == 0)
			break;
		auto firstIter = currentPos + gradientPerp.normalized() * firstStepDst;
		int newGridIndex = NearestGridIndex(firstIter);
		if (newGridIndex < 0)
			break;
		float deltaV = potential - finePotentialGrid[newGridIndex];
		sf::Vector2f newGradient = fineElectricGrid[newGridIndex];

		//deltaV < 0 => -gradient
		//deltaV > 0 => gradient
		float gradientLen = newGradient.length();
		auto secondIter = firstIter;
		if (gradientLen != 0) {
			float secondStep = deltaV / gradientLen;

			if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_F)) {
				secondIter += newGradient.normalized() * secondStep;
				std::cout << deltaV << "\n";
			}
		}
		gridIndex = NearestGridIndex(secondIter);
		if (gridIndex < 0)
			break;
		currentPos = secondIter;
		points.push_back(currentPos);
	}
	
	sf::VertexArray va(sf::PrimitiveType::LineStrip, points.size());
	for (size_t i = 0; i < points.size(); i++) {
		va[i].position = points[i];
		va[i].color = sf::Color::White;
	}
	window->draw(va);

	
}
*/


