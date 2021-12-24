#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>
#include "imnodes.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>


int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "DFA analyzer");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);
    float scaleFactor = (float)sf::VideoMode::getDesktopMode().width / 1920;
    ImGui::GetStyle().ScaleAllSizes(scaleFactor);
    ImNodes::CreateContext();

    ImU32 defaultStateColor = IM_COL32(109, 109, 109, 255);
    ImU32 highlightStateColor = IM_COL32(140, 109, 109, 255);

    char sequence[1024] = "101";
    char currentSequence[1024] = "";
    char error[1024] = "";
    int currentState = -1;
    std::vector<std::pair<int, int>> links;
    int statesCount = 0;
    int prevStatesCount = -1;

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event = {};
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Once);
        ImGui::SetNextWindowSize({(float)window.getSize().x,
                                  (float)window.getSize().y}, ImGuiCond_Always);
        if (ImGui::Begin("Automata editor", nullptr, ImGuiWindowFlags_NoMove)) {
            ImGui::SetWindowFontScale(scaleFactor);
            ImGui::SliderInt("States Count", &statesCount, 0, 99);
            ImGui::Text("Current sequence: %s", currentSequence);
            ImGui::Text("%s", error);
            ImGui::InputText("Check sequence", sequence, IM_ARRAYSIZE(sequence));
            if (ImGui::Button("Start"))
            {
                strcpy(currentSequence, sequence);
                currentState = 100;
            }
            ImGui::SameLine();
            if (ImGui::Button("Next Step"))
            {
                int currentTransition = currentState * 4;
                bool crash = false;
                switch (currentSequence[0]) {
                    case '0':
                        currentTransition += 1;
                        break;
                    case '1':
                        currentTransition += 2;
                        break;
                    case '*':
                        currentTransition += 3;
                        break;
                    default:
                        crash = true;
                }
                if (!crash){
                    for (auto & link : links)
                    {
                        if (link.first == currentTransition)
                        {
                            std::cout << "Transition: " << link.first << " " << link.second << std::endl;
                            memmove(currentSequence, currentSequence+1, 1023);
                            currentState = link.second / 4;
                            if (currentState == 101)
                            {
                                if (currentSequence[0] == '\0') {
                                    sprintf(error, "We got final state!");
                                } else {
                                    sprintf(error, "We got final state, but here is something in sequence.");
                                }
                            }
                            break;
                        }
                        if (link == links.back())
                        {
                            sprintf(error, "No transitions, this sequence not defined by this DFA.");
                        }
                    }
                } else {
                    if (currentSequence[0] == '\0')
                    {
                        if(currentState != 101)
                            sprintf(error, "Sequence is empty. You are not in final state");
                    } else {
                        sprintf(error, "Wrong character: %c! Only 0, 1, * are allowed.", currentSequence[0]);
                    }
                }
            }

            ImNodes::BeginNodeEditor();
            if (currentState == 100)
            {
                ImNodes::PushColorStyle(
                        ImNodesCol_TitleBar, highlightStateColor);
            } else {
                ImNodes::PushColorStyle(
                        ImNodesCol_TitleBar, defaultStateColor);
            }
            ImNodes::BeginNode(100);
            ImNodes::BeginNodeTitleBar();
            ImGui::Text("q0");
            ImNodes::EndNodeTitleBar();
            ImNodes::BeginOutputAttribute(401);
            ImGui::Text("0");
            ImNodes::EndOutputAttribute();
            ImNodes::BeginOutputAttribute(402);
            ImGui::Text("1");
            ImNodes::EndOutputAttribute();
            ImNodes::BeginOutputAttribute(403);
            ImGui::Text("*");
            ImNodes::EndOutputAttribute();
            ImNodes::EndNode();

            if (currentState == 101)
            {
                ImNodes::PushColorStyle(
                        ImNodesCol_TitleBar, highlightStateColor);
            } else {
                ImNodes::PushColorStyle(
                        ImNodesCol_TitleBar, defaultStateColor);
            }

            ImNodes::BeginNode(101);
            ImNodes::BeginNodeTitleBar();
            ImGui::Text("qf");
            ImNodes::EndNodeTitleBar();
            ImNodes::BeginInputAttribute(404);
            ImNodes::EndInputAttribute();
            ImNodes::EndNode();

            for (int i = 0; i < statesCount; i++) {
                if (currentState == i)
                {
                    ImNodes::PushColorStyle(
                            ImNodesCol_TitleBar, highlightStateColor);
                } else {
                    ImNodes::PushColorStyle(
                            ImNodesCol_TitleBar, defaultStateColor);
                }
                ImNodes::BeginNode(i);
                ImNodes::BeginNodeTitleBar();
                ImGui::Text("q%d", i + 1);
                ImNodes::EndNodeTitleBar();
                ImNodes::BeginInputAttribute(i * 4 + 0);
                ImNodes::EndInputAttribute();
                ImNodes::BeginOutputAttribute(i * 4 + 1);
                ImGui::Text("0");
                ImNodes::EndOutputAttribute();
                ImNodes::BeginOutputAttribute(i * 4 + 2);
                ImGui::Text("1");
                ImNodes::EndOutputAttribute();
                ImNodes::BeginOutputAttribute(i * 4 + 3);
                ImGui::Text("*");
                ImNodes::EndOutputAttribute();
                ImNodes::EndNode();
            }

            for (int i = 0; i < links.size(); i++)
            {
                const std::pair<int, int> p = links[i];
                ImNodes::Link(i, p.first, p.second);
            }

            ImNodes::EndNodeEditor();
            int start_attr, end_attr;
            if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
            {
                std::cout << "Created: " << start_attr << " " << end_attr << std::endl;
                std::pair<int, int> linkToDelete;
                for (auto & link : links)
                {
                    if (link.first == start_attr)
                    {
                        linkToDelete = link;
                    }
                }
                links.erase(std::remove(links.begin(), links.end(), linkToDelete), links.end());
                links.emplace_back(start_attr, end_attr);
            }
        }
        ImGui::End();

        if (statesCount < prevStatesCount)
        {
            std::vector<std::pair<int, int>> pairsToDelete;
            for (auto & link : links)
            {
                if ((link.first >= statesCount*4 && link.first < 400) ||
                    (link.second >= statesCount*4 && link.second < 400))
                {
                    pairsToDelete.emplace_back(link);
                }
            }
            for (auto & link : pairsToDelete)
            {
                links.erase(std::remove(links.begin(), links.end(), link), links.end());
            }
        }

        prevStatesCount = statesCount;

        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }
    ImNodes::DestroyContext();
    ImGui::SFML::Shutdown();
}