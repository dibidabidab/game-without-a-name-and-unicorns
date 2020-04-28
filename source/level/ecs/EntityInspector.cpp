
#include <gu/profiler.h>
#include <imgui.h>
#include <input/mouse_input.h>
#include <input/key_input.h>
#include "EntityInspector.h"
#include "components/Physics.h"


EntityInspector::EntityInspector(entt::registry *reg) : reg(reg)
{

}

void EntityInspector::findEntitiesWithComponent(const char *component, std::vector<entt::entity> &out)
{
    auto utils = ComponentUtils::getFor(component);
    if (!utils) return;
    for (int i = 0; i < reg->size(); i++)
        if (utils->entityHasComponent(getByIndex(i), reg))
            out.push_back(getByIndex(i));
}

entt::entity EntityInspector::getByIndex(int i)
{
    return reg->data()[i];
}

void EntityInspector::drawGUI(const Camera *cam, DebugLineRenderer &lineRenderer)
{
    templateToCreate.clear();
    if (!show) return;

    gu::profiler::Zone z("entity inspector");

    static bool pickEntity = false;
    if (pickEntity)
    {
        pickEntityGUI(pickEntity, cam, lineRenderer);
        return;
    }
    reg->view<Inspecting>().each([&](auto e, Inspecting &ins) {
        drawEntityInspectorGUI(e, ins);
    });

    ImGui::SetNextWindowPos(ImVec2(320, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200, 130), ImGuiCond_FirstUseEver);

    bool open = true;

    if (!ImGui::Begin("Entity inspector", &open))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }
    if (!open) show = false;

    ImGui::Text("%lu entities active", reg->size());
    pickEntity = ImGui::Button("Pick entity from screen");

    if (ImGui::Button("Create entity"))
        ImGui::OpenPopup("create_entity");

    if (ImGui::BeginPopup("create_entity"))
    {
        ImGui::Text("Template:");
        ImGui::Separator();

        for (auto &name : entityTemplates)
            if (ImGui::Selectable(name.c_str()))
                templateToCreate = name;
        ImGui::EndPopup();
    }

    ImGui::End();
}

void EntityInspector::pickEntityGUI(bool &pickEntity, const Camera *cam, DebugLineRenderer &lineRenderer)
{
    if (KeyInput::justPressed(GLFW_KEY_ESCAPE))
        pickEntity = false;

    vec2 p = cam->getCursorRayDirection() + cam->position;
    bool breakk = false;
    lineRenderer.axes(p, 10, vec3(1));

    reg->view<AABB>().each([&] (entt::entity e, AABB &box) {

        if (box.contains(p) && !breakk)
        {
            breakk = true;

            box.draw(lineRenderer, mu::Y);

            ImGui::SetTooltip("#%d", int(e));

            if (MouseInput::justPressed(GLFW_MOUSE_BUTTON_LEFT))
            {
                reg->assign_or_replace<Inspecting>(e);
                pickEntity = false;
            }
            return;
        }
        else box.draw(lineRenderer, mu::X);
    });
}

void EntityInspector::drawEntityInspectorGUI(entt::entity e, Inspecting &ins)
{
    if (!ins.show)
    {
        reg->remove<Inspecting>(e);
        return;
    }
    ImGui::SetNextWindowPos(ImVec2(MouseInput::mouseX, MouseInput::mouseY), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(500, 680), ImGuiCond_Once);

    std::string title("Entity #" + std::to_string(int(e)));
    if (!ImGui::Begin(title.c_str(), &ins.show, 0))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }
    if (ImGui::Button("Destroy entity"))
    {
        reg->destroy(e);
        ImGui::End();
        return;
    }

    // ---- COMPONENTS TREE -------

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2,2));
    ImGui::Columns(2);
    ImGui::Separator();

    ImGui::Text("Components:");
    ImGui::NextColumn();
    std::string addComponentPopupName = "add_component_" + std::to_string(int(e));
    if (ImGui::Button("Add"))
        ImGui::OpenPopup(addComponentPopupName.c_str());
    drawAddComponent(e, ins, addComponentPopupName.c_str());

    ImGui::SameLine();
    bool freezeAll = ImGui::Button("Freeze all");
    ImGui::SameLine();
    bool unfreezeAll = ImGui::Button("Unfreeze all");

    ImGui::NextColumn();

    for (auto componentName : ComponentUtils::getAllComponentTypeNames())
    {
        auto componentUtils = ComponentUtils::getFor(componentName);
        if (!componentUtils->entityHasComponent(e, reg)) continue;

        ImGui::PushID(componentName);
        ImGui::AlignTextToFramePadding();

        bool dontRemove = true;
        bool component_open = ImGui::CollapsingHeader(componentName, &dontRemove);
        if (!dontRemove)
        {
            componentUtils->removeComponent(e, reg);
            component_open = false;
        }
        ImGui::NextColumn();

        bool &freeze = ins.freezeComponent[componentName];

        if (freeze)
            componentUtils->setJsonComponent(
                    ins.frozenComponentContents[componentName], e, reg
            );

        if (freezeAll) freeze = true;
        else if (unfreezeAll) freeze = false;

        ImGui::Checkbox("freeze", &freeze);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Prevent game from updating this component while inspecting");

        ImGui::NextColumn();
        if (component_open)
            drawComponentFieldsTree(e, ins, componentName, componentUtils);

        if (freeze)
            componentUtils->getJsonComponent(
                 ins.frozenComponentContents[componentName], e, reg
            );

        ImGui::PopID();
    }
    ImGui::Columns(1);
    ImGui::PopStyleVar();

    ImGui::End();
}

void helpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

bool drawStringInput(std::string &str, Inspecting &ins, bool expandable=true, const char *label="")
{
    bool &multiline = ins.getState().multiline;

    ImGui::AlignTextToFramePadding();

    if (expandable)
    {
        if (!multiline && ImGui::Button(" v "))
            multiline = true;
        else if (multiline && ImGui::Button(" ^ "))
            multiline  = false;
    }
    ImGui::SameLine();

    const int extraBuffer = 1024;
    char *ptr = new char[str.length() + extraBuffer]();
    memcpy(ptr, &str[0], str.length());
    if (
            ((multiline && expandable) && ImGui::InputTextMultiline(label, ptr, str.length() + extraBuffer))
            ||
            (!(multiline && expandable) && ImGui::InputText(label, ptr, str.length() + extraBuffer))
            )
    {
        str = std::string(ptr);
        return ImGui::IsItemDeactivatedAfterEdit();
    }
    delete[] ptr;
    return false;
}

void drawJsonValue(json &value, Inspecting &ins, bool arrayPreview=true, bool readOnly=false)
{
    if (readOnly || (arrayPreview && value.is_array()))
    {
        std::string str = value.dump();
        ImGui::TextColored(ImVec4(.3, .5, 1, 1), "%s", str.c_str());
    }
    else if (value.is_number_float())
    {
        float v = value;
        if (ImGui::DragFloat(" ", &v))
            value = v;
        ImGui::SameLine();
        helpMarker("Drag or double-click");
    }
    else if (value.is_boolean())
    {
        bool v = value;
        if (ImGui::Checkbox(" ", &v))
            value = v;
    }
    else if (value.is_number_integer())
    {
        int v = value;
        if (ImGui::DragInt(" ", &v))
            value = v;
        ImGui::SameLine();
        helpMarker("Drag or double-click");
    }
    else if (value.is_string())
    {
        std::string v = value;
        drawStringInput(v, ins);
        value = v;
    }
}

void drawJsonTree(json &obj, Inspecting &ins, bool editStructure=true, bool readOnlyValues=false)
{
    int i = 0;
    int eraseI = -1;
    std::string eraseKey, addKey;
    json addJson;
    for (auto& [key, value] : obj.items()) { // also works for arrays: keys will be "0", "1", "2", etc.

        ins.currentPath.emplace_back(key);
        ImGui::PushID(i++);                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
        ImGui::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.

        std::string keyStr = key, keyLabel = obj.is_array() ? "[" + key + "]" : (editStructure ? "" : key);

        bool field_open = value.is_structured();
        if (field_open)
            field_open = ImGui::TreeNode("Object", "%s", keyLabel.c_str());
        else
            ImGui::TreeNodeEx(
                    "Field", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_None,
                    "%s", keyLabel.c_str()
            );

        if (editStructure)
        {
            ImGui::SameLine();
            if (ImGui::Button(" x "))
            {
                eraseI = i;
                eraseKey = key;
            }
            if (obj.is_object())
            {
                ImGui::SameLine();
                if (drawStringInput(keyStr, ins, false, "     "))
                {
                    eraseI = i;
                    eraseKey = key;
                    addKey = keyStr;
                    addJson = value;
                }
            }
        }

        ImGui::NextColumn();
        ImGui::AlignTextToFramePadding();

        drawJsonValue(value, ins, true, readOnlyValues);

        ImGui::NextColumn();

        if (field_open)
        {
            drawJsonTree(value, ins, true, readOnlyValues);
            ImGui::TreePop();
        }
        ImGui::PopID();
        ins.currentPath.pop_back();
    }
    if (editStructure)
    {
        ImGui::Separator();
        ImGui::Text("Add: ");
        if (obj.is_array())
        {
            ImGui::NextColumn();
            if (ImGui::Button("float")) obj.push_back(float(0));
            ImGui::SameLine();
            if (ImGui::Button("int")) obj.push_back(int(0));
            ImGui::SameLine();
            if (ImGui::Button("string")) obj.push_back("");
            ImGui::SameLine();
            if (ImGui::Button("bool")) obj.push_back(bool(false));
            ImGui::SameLine();
            if (ImGui::Button("array")) obj.push_back(json::array());
            ImGui::SameLine();
            if (ImGui::Button("object")) obj.push_back(json::object());
            ImGui::SameLine();
            ImGui::NextColumn();
        } else
        {
            std::string &newKey = ins.getState().newKey;
            drawStringInput(newKey, ins, false, "  ");

            ImGui::NextColumn();
            if (ImGui::Button("float")) obj[newKey] = float(0);
            ImGui::SameLine();
            if (ImGui::Button("int")) obj[newKey] = int(0);
            ImGui::SameLine();
            if (ImGui::Button("string")) obj[newKey] = "";
            ImGui::SameLine();
            if (ImGui::Button("bool")) obj[newKey] = bool(false);
            ImGui::SameLine();
            if (ImGui::Button("array")) obj[newKey] = json::array();
            ImGui::SameLine();
            if (ImGui::Button("object")) obj[newKey] = json::object();
            ImGui::SameLine();
            ImGui::NextColumn();
        }
        ImGui::Separator();
    }
    if (eraseI > -1)
    {
        if (obj.is_array()) obj.erase(eraseI - 1);
        else obj.erase(eraseKey);
    }
    if (!addKey.empty())
        obj[addKey] = addJson;
}

void drawFieldsTree(
        json &valuesArray, const SerializableStructInfo *info, Inspecting &ins,
        bool readOnly=false, bool forceEditReadOnly=false
)
{
    for (int i = 0; i < info->nrOfFields; i++)
    {
        auto fieldName = info->fieldNames[i];
        auto fieldTypeName = info->fieldTypeNames[i];

        ImGui::PushID(fieldName);                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
        ImGui::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.

        json &value = valuesArray[i];

        bool field_open = value.is_structured();
        if (field_open)
            field_open = ImGui::TreeNode("Object", "%s", fieldName);
        else
            ImGui::TreeNodeEx(
                    "Field", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_None,
                    "%s", fieldName
            );

        ImGui::SameLine();
        ImGui::TextColored(ImVec4(.3, .5, .7, 1), "%s", fieldTypeName);

        ImGui::NextColumn();
        ImGui::AlignTextToFramePadding();

        auto subInfo = SerializableStructInfo::getFor(fieldTypeName);

        static std::string finalTypeBegin = "final<";
        bool subReadOnly = !forceEditReadOnly
                            && (readOnly
                                || std::string(fieldTypeName).compare(0, finalTypeBegin.length(), finalTypeBegin) == 0
                            );

        drawJsonValue(value, ins, !subInfo, subReadOnly);

        ImGui::NextColumn();
        if (field_open)
        {
            ins.currentPath.emplace_back(fieldName);
            if (subInfo)
                drawFieldsTree(value, subInfo, ins, subReadOnly);
            else drawJsonTree(
                    value, ins,
                    !info->structFieldIsFixedSize[i],
                    subReadOnly
            );
            ins.currentPath.pop_back();
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}

void EntityInspector::drawComponentFieldsTree(entt::entity e, Inspecting &ins, const char *componentName, const ComponentUtils *componentUtils)
{
    auto info = SerializableStructInfo::getFor(componentName);
    json valuesArray;
    componentUtils->getJsonComponent(valuesArray, e, reg);

    assert(ins.currentPath.empty());
    ins.currentPath.emplace_back(componentName);
    drawFieldsTree(valuesArray, info, ins);
    ins.currentPath.pop_back();

    try {
        componentUtils->setJsonComponent(valuesArray, e, reg);
    } catch (std::exception& e) {
        std::cerr << "Exception after editing component in inspector:\n" << e.what() << '\n';
    }
}

void EntityInspector::drawAddComponent(entt::entity e, Inspecting &ins, const char *popupName)
{
    if (ImGui::BeginPopup(popupName))
    {
        ImGui::Text("Component Type:");
        ImGui::Separator();

        for (auto typeName : ComponentUtils::getAllComponentTypeNames())
        {
            auto utils = ComponentUtils::getFor(typeName);
            if (utils->entityHasComponent(e, reg))
                continue;
            if (ImGui::Selectable(typeName))
            {
                ins.addingComponentTypeName = typeName;
                ins.addingComponentJson = utils->construct();
            }
        }
        ImGui::EndPopup();
    }

    if (ins.addingComponentTypeName == NULL) return;

    ImGui::SetNextWindowPos(ImVec2(MouseInput::mouseX - 200, MouseInput::mouseY - 15), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);

    std::string title = "Adding " + std::string(ins.addingComponentTypeName) + " to entity #" + std::to_string(int(e));
    bool open = true;
    ImGui::Begin(title.c_str(), &open);

    ImGui::Text("[Warning]:\nadding components (with wrong values) could crash the game!");
    ImGui::Columns(2);
    ImGui::Separator();

    drawFieldsTree(ins.addingComponentJson, SerializableStructInfo::getFor(ins.addingComponentTypeName), ins, false);

    ImGui::Columns(1);
    ImGui::Separator();

    if (ImGui::Button("Add"))
    {
        open = false;

        auto utils = ComponentUtils::getFor(ins.addingComponentTypeName);
        try
        {
            utils->addComponent(ins.addingComponentJson, e, reg);
        } catch (std::exception& e) {
            std::cerr << "Exception while trying to add component in inspector:\n" << e.what() << '\n';
        }
    }
    ImGui::End();
    if (!open)
        ins.addingComponentTypeName = NULL;
}
