#ifndef CHARGE_H

#define CHARGE_H

#include <SFML/Graphics.hpp>
#include "configuration.h"

class Charge {
	public:

	float totalCharge = 1;

	virtual void MoveTo(sf::Vector2f position) = 0;
	virtual void Draw(sf::RenderWindow* window) = 0;
	virtual bool IsInside(sf::Vector2f point) = 0;
	virtual bool PhysicsIsInside(sf::Vector2f point) = 0;
	virtual void Hover() = 0;
	virtual void Unhover() = 0;

	virtual void EditedCharge() = 0;

	virtual sf::Vector2f GetPosition() = 0;
	virtual sf::Vector2f GetFieldAt(sf::Vector2f pos) = 0;
	virtual float GetPotentialAt(sf::Vector2f pos) = 0;

	Charge() : id_(generateId()) {}
	virtual ~Charge() = default;

	unsigned long id() const { return id_; }

	sf::Vector2f ToPhysicsPos(sf::Vector2f point) {
		return { point.x * conf::calculator::worldUnitsPerUnit, point.y * conf::calculator::worldUnitsPerUnit };
	}

	private:
	unsigned long id_;
	static unsigned long generateId() {
		static unsigned long counter = 0;
		return ++counter;       // atomic if multi-threading
	}



};

#endif