#ifndef PROGRAM_STATE_H

#define PROGRAM_STATE_H

#include <SFML/Graphics.hpp>
#include "charge.h"

enum class InteractionState {
	NONE,
	ACTION_WINDOW,
	PLACING,
	MOVING,
};

enum class InteractionType {
	NONE,
	PLACING_CHARGE
};

namespace State {
	inline InteractionState currentInteractionState;
	inline InteractionType currentInteraction;

	inline sf::Vector2f mousePos;

	inline Charge* hoveredCharge = nullptr;
	inline Charge* editingCharge = nullptr;
	inline sf::Vector2f movementOffset;
}

#endif