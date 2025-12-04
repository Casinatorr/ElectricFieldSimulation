#include "interactionHandling.h"
#include "programState.h"
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include "pointCharge.h"
#include <SFML/Graphics.hpp>
#include "renderingRegistry.h"


void PlaceChargeAction() {
	State::currentInteractionState = InteractionState::ACTION_WINDOW;
	ImGui::OpenPopup("PlaceChargeMenu");
}


std::shared_ptr<Charge> currentPlacingCharge;

void PlaceChargeActionActive() {
	if (!currentPlacingCharge) {
		//how
		return;
	}
	currentPlacingCharge->MoveTo(State::mousePos);
}


void CurrentAction() {
	switch (State::currentInteraction) {
		case InteractionType::PLACING_CHARGE:
			PlaceChargeActionActive();
			break;
	}
}


void InitializePointCharge() {
	currentPlacingCharge = std::make_shared<PointCharge>(0.f);
	RegisterCharge(currentPlacingCharge);
	State::currentInteraction = InteractionType::PLACING_CHARGE;
	State::currentInteractionState = InteractionState::PLACING;
}

void PlaceChargeDown() {
	//place charge

	State::currentInteractionState = InteractionState::NONE;
	State::currentInteraction = InteractionType::NONE;

	
	State::editingCharge = currentPlacingCharge.get();
	ImGui::OpenPopup("ChargeClicked");
	currentPlacingCharge = nullptr;
}



void CancelCurrentAction() {
	if (State::currentInteractionState == InteractionState::PLACING) {
		UnregisterCharge(currentPlacingCharge);
		currentPlacingCharge = nullptr;
	}
	State::currentInteractionState = InteractionState::NONE;
	State::currentInteraction = InteractionType::NONE;
}

void ClickChargeAction() {
	State::currentInteractionState = InteractionState::MOVING; 
	State::editingCharge = State::hoveredCharge;
	State::movementOffset = State::editingCharge->GetPosition() - State::mousePos;
	ImGui::OpenPopup("ChargeClicked");
}