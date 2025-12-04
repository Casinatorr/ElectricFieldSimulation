#ifndef IMGUI_DEFS_H

#define IMGUI_DEFS_H

#include <imgui-SFML.h>
#include <imgui.h>
#include "programState.h"
#include "interactionHandling.h"
#include "renderingRegistry.h"

const char* to_string(InteractionType type) {
	switch (type) {
		case InteractionType::NONE:   return "NONE";
		case InteractionType::PLACING_CHARGE: return "PLACING CHARGE";
	
	}
	return "UNKNOWN";
}

const char* to_string(InteractionState state) {
	switch (state) {
		case InteractionState::NONE:          return "NONE";
		case InteractionState::ACTION_WINDOW: return "ACTION_WINDOW";
		case InteractionState::PLACING:		  return "PLACING";
	}
	return "UNKNOWN";
}

void DoubleTextLabel(const char* a, const char* b) {
	ImGui::Text(a);
	ImGui::SameLine();
	ImGui::Text(b);
}

std::string chargeIDText(Charge* c) {
	if (c)
		return std::to_string(c->id());
	return "NONE";
}

void DebugWindow() {
	ImGui::Begin("DEBUG_VARIABLES");
	DoubleTextLabel("Interaction State: ", to_string(State::currentInteractionState));
	DoubleTextLabel("Interaction Type: ", to_string(State::currentInteraction));
	DoubleTextLabel("Registered Charges: ", std::to_string(chargeRegistry.size()).c_str());

	DoubleTextLabel("Hovered Charge: ", chargeIDText(State::hoveredCharge).c_str());
	DoubleTextLabel("Clicked Charge: ", chargeIDText(State::editingCharge).c_str());
	ImGui::End();
}

float floatVal = 0;

void DefineImGuiWindows() {

	//variables window
	DebugWindow();

	if (ImGui::BeginPopup("PlaceChargeMenu")) {
		if (ImGui::MenuItem("Point Charge")) {
			InitializePointCharge();
		}
		if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
			ImGui::CloseCurrentPopup();
			State::currentInteractionState = InteractionState::NONE;
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("ChargeClicked")) {
		if (ImGui::SliderFloat("Charge", &State::editingCharge->totalCharge, -conf::bounds::minMaxCharge, conf::bounds::minMaxCharge)) {
			State::editingCharge->EditedCharge();
		}

		ImGui::EndPopup();
	}

	if (!ImGui::IsPopupOpen("ChargeClicked") && State::editingCharge) {
		//not editing anymore
		State::editingCharge = nullptr;
		State::currentInteractionState = InteractionState::NONE;
		if (State::hoveredCharge) {
			ClickChargeAction();
		}
	}
}

#endif