
#ifndef GAME_IMGUISTYLE_H
#define GAME_IMGUISTYLE_H

#include <imgui.h>
#include "../game/Game.h"

namespace
{
    static u8vec3 Black(0), White(255);

    static ImVec4 GetColor(u8vec3 rgb) { return ImVec4(rgb.r / 255.f, rgb.g / 255.f, rgb.b / 255.f, 1.f); }

    static ImVec4 Darken(ImVec4 c, float p)			{ return ImVec4(fmax(0.f, c.x - 1.0f * p), fmax(0.f, c.y - 1.0f * p), fmax(0.f, c.z - 1.0f *p), c.w); }
    static ImVec4 Lighten(ImVec4 c, float p)		{ return ImVec4(fmax(0.f, c.x + 1.0f * p), fmax(0.f, c.y + 1.0f * p), fmax(0.f, c.z + 1.0f *p), c.w); }

    static ImVec4 Disabled(ImVec4 c)	{	return Darken(c, 0.4f); 	}
    static ImVec4 Hovered(ImVec4 c)		{	return Lighten(c, 0.2f);	}
    static ImVec4 Active(ImVec4 c)		{	return Lighten(ImVec4(c.x, c.y, c.z, 1.0f),0.1f); }
    static ImVec4 Collapsed(ImVec4 c)	{	return Darken(c, 0.2f);		}
}

inline void setImGuiStyleAndConfig()
{
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
    /**
     * Color scheme taken from https://github.com/ocornut/imgui/issues/707#issuecomment-512669512
     */
    ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/ruda/Ruda-Bold.ttf", 15.5f);
    ImGui::GetIO().Fonts->AddFontDefault();
    auto &style = ImGui::GetStyle();
    style.FrameRounding = style.GrabRounding = style.ChildRounding = style.PopupRounding = 4.0f;

    style.WindowTitleAlign.x = .5f;

    ImVec4* colors = style.Colors;

    auto
        TextColor = Game::settings.graphics.imGuiThemeColor_text,
        BackGroundColor = Game::settings.graphics.imGuiThemeColor_background,
        MainColor = Game::settings.graphics.imGuiThemeColor_main,
        MainAccentColor = Game::settings.graphics.imGuiThemeColor_mainAccent,
        HighlightColor = Game::settings.graphics.imGuiThemeColor_highLight;

    colors[ImGuiCol_Text]					= GetColor(TextColor);
    colors[ImGuiCol_TextDisabled]			= Disabled(colors[ImGuiCol_Text]);
    colors[ImGuiCol_WindowBg]				= GetColor(BackGroundColor);
    colors[ImGuiCol_ChildBg]				= ImVec4(0, 0, 0, .3);
    colors[ImGuiCol_PopupBg]				= GetColor(BackGroundColor);
    colors[ImGuiCol_Border]					= Lighten(GetColor(BackGroundColor),0.1f);
    colors[ImGuiCol_BorderShadow]			= GetColor(Black);
    colors[ImGuiCol_FrameBg]				= GetColor(MainAccentColor);
    colors[ImGuiCol_FrameBgHovered]			= Hovered(colors[ImGuiCol_FrameBg]);
    colors[ImGuiCol_FrameBgActive]			= Active(colors[ImGuiCol_FrameBg]);
    colors[ImGuiCol_TitleBg]				= GetColor(BackGroundColor);
    colors[ImGuiCol_TitleBgActive]			= Active(colors[ImGuiCol_TitleBg]);
    colors[ImGuiCol_TitleBgCollapsed]		= Collapsed(colors[ImGuiCol_TitleBg]);
    colors[ImGuiCol_MenuBarBg]				= Darken(GetColor(BackGroundColor), 0.2f);
    colors[ImGuiCol_ScrollbarBg]			= Lighten(GetColor(BackGroundColor), 0.1f);
    colors[ImGuiCol_ScrollbarGrab]			= Lighten(GetColor(BackGroundColor), 0.3f);
    colors[ImGuiCol_ScrollbarGrabHovered]	= Hovered(colors[ImGuiCol_ScrollbarGrab]);
    colors[ImGuiCol_ScrollbarGrabActive]	= Active(colors[ImGuiCol_ScrollbarGrab]);
    colors[ImGuiCol_CheckMark]				= GetColor(HighlightColor);
    colors[ImGuiCol_SliderGrab]				= GetColor(HighlightColor);
    colors[ImGuiCol_SliderGrabActive]		= Active(colors[ImGuiCol_SliderGrab]);
    colors[ImGuiCol_Button]					= GetColor(MainColor);
    colors[ImGuiCol_ButtonHovered]			= Hovered(colors[ImGuiCol_Button]);
    colors[ImGuiCol_ButtonActive]			= Active(colors[ImGuiCol_Button]);
    colors[ImGuiCol_Header]					= GetColor(MainAccentColor);
    colors[ImGuiCol_HeaderHovered]			= Hovered(colors[ImGuiCol_Header]);
    colors[ImGuiCol_HeaderActive]			= Active(colors[ImGuiCol_Header]);
    colors[ImGuiCol_Separator]				= colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered]		= Hovered(colors[ImGuiCol_Separator]);
    colors[ImGuiCol_SeparatorActive]		= Active(colors[ImGuiCol_Separator]);
    colors[ImGuiCol_ResizeGrip]				= GetColor(MainColor);
    colors[ImGuiCol_ResizeGripHovered]		= Hovered(colors[ImGuiCol_ResizeGrip]);
    colors[ImGuiCol_ResizeGripActive]		= Active(colors[ImGuiCol_ResizeGrip]);
    colors[ImGuiCol_Tab]					= GetColor(MainColor);
    colors[ImGuiCol_TabHovered]				= Hovered(colors[ImGuiCol_Tab]);
    colors[ImGuiCol_TabActive]				= Active(colors[ImGuiCol_Tab]);
    colors[ImGuiCol_TabUnfocused]			= colors[ImGuiCol_Tab];
    colors[ImGuiCol_TabUnfocusedActive]		= colors[ImGuiCol_TabActive];
    colors[ImGuiCol_PlotLines]				= GetColor(HighlightColor);
    colors[ImGuiCol_PlotLinesHovered]		= Hovered(colors[ImGuiCol_PlotLines]);
    colors[ImGuiCol_PlotHistogram]			= GetColor(HighlightColor);
    colors[ImGuiCol_PlotHistogramHovered]	= Hovered(colors[ImGuiCol_PlotHistogram]);
    colors[ImGuiCol_TextSelectedBg]			= GetColor(HighlightColor);
    colors[ImGuiCol_DragDropTarget]			= GetColor(HighlightColor);
    colors[ImGuiCol_NavHighlight]			= GetColor(White);
    colors[ImGuiCol_NavWindowingHighlight]	= GetColor(White);
    colors[ImGuiCol_NavWindowingDimBg]		= GetColor(White);
    colors[ImGuiCol_ModalWindowDimBg]		= ImVec4(0, 0, 0, .3);
}

#endif //GAME_IMGUISTYLE_H
