#ifndef POINT_CHARGE_H

#define POINT_CHARGE_H

#include "charge.h"
#include <SFML/Graphics.hpp>

class PointCharge : public Charge {
	public:
	void SetCharge(float newCharge);
	float GetCharge();

	PointCharge(float _charge);

	void Draw(sf::RenderWindow* window) override;
	void MoveTo(sf::Vector2f position) override;
	bool IsInside(sf::Vector2f point) override;
	bool PhysicsIsInside(sf::Vector2f point) override;
	void Hover() override;
	void Unhover() override;
	void EditedCharge() override;
	sf::Vector2f GetPosition() override;
	sf::Vector2f GetFieldAt(sf::Vector2f pos) override;
	float GetPotentialAt(sf::Vector2f pos) override;

	private:
	float charge;
	sf::CircleShape body;
};

#endif