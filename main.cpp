
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include "configuration.h"
#include "eventHandling.h"
#include "interactionHandling.h"
#include "imguiDefinitions.h"
#include "renderingRegistry.h"

int main()
{
    
    sf::ContextSettings settings = sf::ContextSettings();
    settings.antiAliasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(conf::dimensions::windowDims), "Electric Field Visualisation", sf::State::Windowed, settings);

    if (!ImGui::SFML::Init(window)) {
        std::cout << "ImGUI failed to initialize" << std::endl;
        return -1;
    }


    sf::Clock deltaClock;
    while (window.isOpen())
    {

        ImGui::SFML::Update(window, deltaClock.restart());

        State::mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));


        DefineImGuiWindows();
        HandleEvents(&window);
        CurrentAction();



        window.clear(conf::colors::backgroundCol);
        if (State::currentFieldSprite)
            window.draw(*State::currentFieldSprite);

        Draw(&window);

        ImGui::SFML::Render(window);
        window.display();
    }
}