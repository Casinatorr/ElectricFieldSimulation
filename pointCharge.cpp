#include "pointCharge.h"

#include "configuration.h"
#include <SFML/Graphics.hpp>

void PointCharge::SetCharge(float _charge) {
	charge = _charge;
}
float PointCharge::GetCharge() { return charge; }

PointCharge::PointCharge(float _charge) {
	body = sf::CircleShape(conf::dimensions::pointChargeRadius);
	body.setFillColor(sf::Color::White);
	body.setOrigin({ conf::dimensions::pointChargeRadius, conf::dimensions::pointChargeRadius });
	body.setOutlineColor(conf::colors::hoverOutlineColor);
	charge = _charge;
}

void PointCharge::Draw(sf::RenderWindow* window) {
	window->draw(body);
}

void PointCharge::MoveTo(sf::Vector2f position) {
	body.setPosition(position);
}

bool PointCharge::IsInside(sf::Vector2f point) {
	return (point - body.getPosition()).lengthSquared() <= conf::dimensions::pointChargeRadius * conf::dimensions::pointChargeRadius;
}

void PointCharge::Hover() {
	body.setOutlineThickness(conf::dimensions::pointChargeOutlineThickness);
}

void PointCharge::Unhover() {
	body.setOutlineThickness(0);
}

sf::Color HSVtoRGB(float h, float s, float v)
{

	float c = v * s;                       // chroma
	float x = c * (1 - fabs(fmod(h / 60.0f, 2) - 1));
	float m = v - c;

	float r, g, b;

	if (h < 60) { r = c; g = x; b = 0; }
	else if (h < 120) { r = x; g = c; b = 0; }
	else if (h < 180) { r = 0; g = c; b = x; }
	else if (h < 240) { r = 0; g = x; b = c; }
	else if (h < 300) { r = x; g = 0; b = c; }
	else { r = c; g = 0; b = x; }

	return sf::Color(
		static_cast<uint8_t>(255 * (r + m)),
		static_cast<uint8_t>(255 * (g + m)),
		static_cast<uint8_t>(255 * (b + m))
	);
}

sf::Color ColorByCharge(float charge) {
	float normalizedCharge = charge / conf::bounds::minMaxCharge;
	float hue = 0;
	if (normalizedCharge < 0) {
		hue = 240.f;
	}
	else {
		hue = 0.f;
	}
	return HSVtoRGB(hue, sqrtf(fabs(normalizedCharge)), 1.0);
}

void PointCharge::EditedCharge() {
	body.setFillColor(ColorByCharge(totalCharge));
}

sf::Vector2f PointCharge::GetPosition() {
	return body.getPosition();
}

sf::Vector2f PointCharge::GetFieldAt(sf::Vector2f pos) {
	sf::Vector2f dir = pos - ToPhysicsPos(body.getPosition());
	float sqrDst = dir.lengthSquared();
	return dir.normalized() * totalCharge* conf::calculator::coulombConstant / sqrDst;
}