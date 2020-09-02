
#include "PaletteEditor.h"
#include "../game/Game.h"

#include <gu/profiler.h>
#include <imgui.h>

static bool saved = true;

void PaletteEditor::drawGUI(uint currentlyRendered)
{
    static auto allowClose = gu::canClose += [] {
        return saved;
    };

    const char *unsavedModalTitle = "Palette Editor: closing unsaved";

    if (gu::shouldClose() && !saved)
    {
        ImGui::Begin(unsavedModalTitle, NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
        ImGui::Text("You've made unsaved changes in the Palette Editor!");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0)))
            saved = true; // kinda dirty hack

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            gu::setShouldClose(false);
            show = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::End();
    }

    if (!show) return;
    gu::profiler::Zone z("palette editor");

    ImGui::SetNextWindowPos(ImVec2(730, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);

    bool open = true;
    if (!ImGui::Begin("Palette Editor", &open, !saved ? ImGuiWindowFlags_UnsavedDocument : 0))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }
    if (!open) show = false;

    if (selectedEffect.empty())
        selectedEffect = Game::palettes->effects.at(0).name;

    ImGui::TextDisabled("Currently rendered effect:");
    ImGui::SameLine();
    auto currentlyRenderedStr = Game::palettes->effects.at(currentlyRendered).name;
    auto c = selectedEffect == currentlyRenderedStr ? ImVec4(1, 1, 1, 1) : ImVec4(1, 0, .1, 1);
    ImGui::TextColored(c, "%s", currentlyRenderedStr.c_str());

    ImGui::Separator();

    if (ImGui::BeginCombo("Edit effect", selectedEffect.c_str()))
    {
        for (auto &effect : Game::palettes->effects)
        {
            bool is_selected = (selectedEffect == effect.name);
            if (ImGui::Selectable(effect.name.c_str(), is_selected))
                selectedEffect = effect.name;
            if (is_selected)
                ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
        }
        ImGui::EndCombo();
    }

    if (drawPalettes(Game::palettes->effects.at(Game::palettes->effectIndex(selectedEffect))))
    {
        saved = false;
        Game::palettes->changedByEditor = true;
    }

    ImGui::End();
}

bool PaletteEditor::drawPalettes(Palettes3D::Effect &effect)
{
    bool changed = false;
    if (ImGui::Button("Save"))
    {
        for (auto &p : effect.lightLevels)
            p->save(p.getLoadedAsset().fullPath.c_str());
        saved = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Revert"))
    {
        for (auto &p : effect.lightLevels)
            revert(p);
        changed = true;
        saved = true;
    }
    ImGui::Separator();

    ImGui::Columns(2);

    ImGui::Text("#  Color name");

    ImGui::NextColumn();

    ImGui::Text("Light level 0,1,2 & Bloom");

    ImGui::NextColumn();

    for (int i = 0; i < effect.lightLevels[0]->colors.size(); i++)
    {
        auto &firstName = effect.lightLevels[0]->colors.at(i).first;
        vec3 firstColor = effect.lightLevels[0]->colors.at(i).second;

        ImGui::Text("%d", i);
        ImGui::SameLine();

        std::string label = "##" + std::to_string(i);

        constexpr size_t maxNameLen = 32;
        char *ptr = new char[maxNameLen]();
        memcpy(ptr, &firstName[0], min(maxNameLen, firstName.size()));
        if (ImGui::InputText(label.c_str(), ptr, 32))
            firstName = std::string(ptr);
        delete[] ptr;

        ImGui::NextColumn();
        for (auto &p : effect.lightLevels)
        {
            if (p->colors.size() == i)
                p->colors.emplace_back(firstName, firstColor);

            if (p->colors.at(i).first != firstName)
                p->colors.at(i).first = firstName;

            std::string name = firstName + " (" + p.getLoadedAsset().shortPath + ")##" + std::to_string(i);
            std::string cName = name + "contextmenu";

            if (ImGui::ColorEdit3(name.c_str(), &p->colors.at(i).second[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
                changed = true;

            if (ImGui::BeginPopupContextItem(cName.c_str()))
            {
                if (ImGui::Selectable("Revert"))
                {
                    revert(p, i);
                    changed = true;
                }
                ImGui::EndPopup();
            }
            ImGui::SameLine();
        }
        ImGui::NextColumn();
    }
    ImGui::Columns(1);
    ImGui::Separator();
    if (ImGui::Button("Add"))
        effect.lightLevels[0]->colors.emplace_back("untitled", vec3(0));

    ImGui::SameLine();
    if (ImGui::Button("Remove"))
        effect.lightLevels[0]->colors.pop_back();

    return changed;
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
