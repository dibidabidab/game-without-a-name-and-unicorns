
#include "PaletteEditor.h"

#include <gu/profiler.h>
#include <imgui.h>

void PaletteEditor::drawGUI()
{
    gu::profiler::Zone z("palette editor");

    ImGui::SetNextWindowPos(ImVec2(530, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

    bool show = true;
    if (!ImGui::Begin("Palette Editor", &show, 0))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    std::map<std::string, std::vector<asset<Palette>>> effects {
            {"default", {
                asset<Palette>("palettes/default0"),
                asset<Palette>("palettes/default1")
            }},
            {"test", {
                asset<Palette>("palettes/default1"),
                asset<Palette>("palettes/default0")
            }}
    };
    if (selectedEffect.empty())
        selectedEffect = effects.begin()->first;

    if (ImGui::BeginCombo("effect", selectedEffect.c_str()))
    {
        for (auto &effect : effects)
        {
            bool is_selected = (selectedEffect == effect.first);
            if (ImGui::Selectable(effect.first.c_str(), is_selected))
                selectedEffect = effect.first;
            if (is_selected)
                ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
        }
        ImGui::EndCombo();
    }

    drawPalettes(effects.at(selectedEffect));

    ImGui::End();
    ImGui::ShowDemoWindow(&show);
}

void PaletteEditor::drawPalettes(std::vector<asset<Palette>> &palettes)
{
    if (ImGui::Button("Save"))
    {

    }
    ImGui::SameLine();
    if (ImGui::Button("Revert"))
    {
        for (auto &p : palettes)
            revert(p);
    }
    ImGui::Separator();

    ImGui::Text("Color name");
    ImGui::Separator();

    ImGui::Columns(2);
    for (int i = 0; i < palettes.at(0)->colors.size(); i++)
    {
        auto &firstName = palettes[0]->colors.at(i).first;
        vec3 firstColor = palettes[0]->colors.at(i).second;

        std::string label = "##" + std::to_string(i);

        char *ptr = new char[32]();
        memcpy(ptr, &firstName[0], firstName.size());
        if (ImGui::InputText(label.c_str(), ptr, 32))
            firstName = std::string(ptr);

        ImGui::NextColumn();
        for (auto &p : palettes)
        {
            if (p->colors.size() == i)
                p->colors.emplace_back(firstName, firstColor);

            if (p->colors.at(i).first != firstName)
                p->colors.at(i).first = firstName;

            std::string name = firstName + " (" + p.getLoadedAsset().shortPath + ")##" + std::to_string(i);
            std::string cName = name + "contextmenu";

            ImGui::ColorEdit3(name.c_str(), &p->colors.at(i).second[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            if (ImGui::BeginPopupContextItem(cName.c_str()))
            {
                if (ImGui::Selectable("Revert"))
                    revert(p, i);
                ImGui::EndPopup();
            }
            ImGui::SameLine();
        }
        ImGui::NextColumn();
    }
    ImGui::Columns(1);
    ImGui::Separator();
    if (ImGui::Button("Add"))
        palettes.at(0)->colors.emplace_back("untitled", vec3(0));

    ImGui::SameLine();
    if (ImGui::Button("Remove"))
        palettes.at(0)->colors.pop_back();
}

void PaletteEditor::revert(asset<Palette> &p)
{
    Palette original(p.getLoadedAsset().fullPath.c_str());
    p->colors = original.colors;
}

void PaletteEditor::revert(asset<Palette> &p, int colorIndex)
{
    Palette original(p.getLoadedAsset().fullPath.c_str());
    if (p->colors.size() > colorIndex && original.colors.size() > colorIndex)
        p->colors.at(colorIndex) = original.colors.at(colorIndex);
}
