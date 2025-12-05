#include "eventHandling.h"

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include "programState.h"
#include "interactionHandling.h"
#include <iostream>
#include "renderingRegistry.h"

void HandleEscapeKey(sf::RenderWindow* window) {
    //Escape

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift))
        window->close();

    if (State::currentInteractionState == InteractionState::PLACING) {
        //exit placement
        CancelCurrentAction();
    }
}

void HandleLeftClick(sf::RenderWindow* window) {
    if (State::currentInteractionState == InteractionState::PLACING) {
        //place down
        PlaceChargeDown();
    }
    else if (State::currentInteractionState == InteractionState::NONE) {
        //check for hovering

        if (State::hoveredCharge) {
            //Click charge
            ClickChargeAction();
        }
    }
}




void HandleHovering(sf::RenderWindow* window, bool update) {
    for (auto c : chargeRegistry) {
        if (c->IsInside(State::mousePos)) {
            //hover

            if (State::hoveredCharge)
                State::hoveredCharge->Unhover();
            State::hoveredCharge = c.get();
            if (update) c->Hover();
        }
        else if (c.get() == State::hoveredCharge) {
            State::hoveredCharge = nullptr;
            c->Unhover();
        }
    }
}

void HandleEvents(sf::RenderWindow* window) {

    while (const std::optional event = window->pollEvent())
    {
        ImGui::SFML::ProcessEvent(*window, event.value());
        if (event->is<sf::Event::Closed>())
            window->close();

        if (event->is<sf::Event::KeyPressed>()) {
            sf::Keyboard::Key keyboardCode = event->getIf<sf::Event::KeyPressed>()->code;
            

            if (keyboardCode == sf::Keyboard::Key::Escape) {
                HandleEscapeKey(window);
            }

            if (keyboardCode == sf::Keyboard::Key::Q) {
                //place a new charge
                PlaceChargeAction();
            }

            if (keyboardCode == sf::Keyboard::Key::C) {
                RecalculateAction();
            }
        }

        if (event->is<sf::Event::MouseButtonPressed>()) {
            sf::Mouse::Button btn = event->getIf<sf::Event::MouseButtonPressed>()->button;
            if (btn == sf::Mouse::Button::Left) {
                HandleLeftClick(window);
            }
        }

        if (event->is<sf::Event::MouseButtonReleased>()) {
            sf::Mouse::Button btn = event->getIf<sf::Event::MouseButtonReleased>()->button;
            if (btn == sf::Mouse::Button::Left) {
                //check if clicked charge
                if (State::editingCharge) {
                    //stop movement action
                    State::currentInteractionState = InteractionState::ACTION_WINDOW;
                }
            }
        }
    }

    HandleHovering(window, State::currentInteractionState == InteractionState::NONE || State::currentInteractionState == InteractionState::ACTION_WINDOW);

    if (State::editingCharge && State::currentInteractionState == InteractionState::MOVING) {
        State::editingCharge->MoveTo(State::mousePos + State::movementOffset);
    }
}