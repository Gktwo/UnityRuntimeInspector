#include "pch.h"
#include "unity_explorer.h"
#include "../ui/language.h"
#include "methods/method_helpers.h"

bool UnityExplorer::initialize()
{
    if (m_initialized)
    {
        LOG_WARNING("[UnityExplorer] Already initialized");
        return true;
    }

    // LOG_INFO("[UnityExplorer] Initializing Unity Explorer...");

    try
    {
        // Test if UnityResolve is working by trying to get a basic assembly
        auto coreModule = UnityResolve::Get("UnityEngine.CoreModule.dll");
        if (!coreModule)
        {
            LOG_ERROR("[UnityExplorer] Failed to get UnityEngine.CoreModule.dll");
            return false;
        }

        LOG_INFO("[UnityExplorer] Found UnityEngine.CoreModule.dll");

        // Initialize component expanded states
        m_componentExpandedState["Transform"] = true;
        m_componentExpandedState["GameObject"] = true;

        m_initialized = true;
        refreshScene();

        LOG_INFO("[UnityExplorer] Unity Explorer initialized successfully!");
        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("[UnityExplorer] Exception during initialization: %s", e.what());
        return false;
    }
    catch (...)
    {
        LOG_ERROR("[UnityExplorer] Unknown exception during initialization");
        return false;
    }
}

void UnityExplorer::update()
{
    if (!m_initialized)
    {
        return;
    }

    // Auto-refresh check
    float currentTime = ImGui::GetTime();
    if (m_autoRefresh && (currentTime - m_lastRefreshTime > m_refreshInterval))
    {
        m_needsRefresh = true;
        m_lastRefreshTime = currentTime;
    }

    if (m_needsRefresh)
    {
        refreshScene();
        m_needsRefresh = false;
    }

    if (m_showSceneExplorer)
    {
        renderSceneExplorer();
    }

    if (m_showObjectInspector && m_selectedObject)
    {
        renderObjectInspector();
    }
}

void UnityExplorer::shutdown()
{
    LOG_INFO("[UnityExplorer] Shutting down...");

    m_rootObjects.clear();
    m_objectCache.clear();
    m_selectedObject = nullptr;
    m_componentExpandedState.clear();
    m_initialized = false;
}

void UnityExplorer::renderSceneExplorer()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;

    if (!ImGui::Begin("Scene Explorer", &m_showSceneExplorer, window_flags))
    {
        ImGui::End();
        return;
    }

    // Menu bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Show Inactive Objects", nullptr, &m_showInactiveObjects);
            ImGui::MenuItem("Auto Refresh", nullptr, &m_autoRefresh);
            ImGui::Separator();
            if (ImGui::MenuItem("Refresh Scene", "F5"))
            {
                m_needsRefresh = true;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options"))
        {
            ImGui::MenuItem("Expand All", nullptr, false, false); // TODO: Implement
            ImGui::MenuItem("Collapse All", nullptr, false, false); // TODO: Implement
            ImGui::Separator();
            ImGui::MenuItem("Copy Object Path", nullptr, false, false); // TODO: Implement
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    // Toolbar with better styling
    ImGui::BeginChild("Toolbar", ImVec2(0, 40), true);

    if (ImGui::Button("Refresh", ImVec2(80, 25)))
    {
        m_needsRefresh = true;
    }
    ImGui::SameLine();

    ImGui::Checkbox("Show Inactive", &m_showInactiveObjects);
    ImGui::SameLine();

    if (ImGui::Checkbox("Auto Refresh", &m_autoRefresh))
    {
        // Reset the refresh timer when auto-refresh is toggled
        m_lastRefreshTime = ImGui::GetTime();
    }
    ImGui::SameLine();

    // Status indicator
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);
            ImGui::Text("%s:", LANG("Status"));
    ImGui::SameLine();
    if (m_initialized)
    {
                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), LANG("● Connected"));
    }
            else
        {
            ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), LANG("● Disconnected"));
        }

    ImGui::SameLine();
    ImGui::Separator();
    ImGui::SameLine();

    // Auto-refresh status indicator
            if (m_autoRefresh)
        {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), LANG("● Auto-refresh: ON"));
        }
        else
        {
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), LANG("● Auto-refresh: OFF"));
        }

    ImGui::EndChild();

    // Search filter with better styling
    ImGui::Spacing();
            ImGui::Text("%s:", LANG("Search Objects"));
    ImGui::SameLine();
    helpMarker("Filter objects by name. Supports partial matches.");

    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputTextWithHint("##search", "Enter object name to filter...", m_searchBuffer, sizeof(m_searchBuffer)))
    {
        m_searchFilter = std::string(m_searchBuffer);
    }

    ImGui::Spacing();

    // Statistics bar
    ImGui::BeginChild("Stats", ImVec2(0, 30), true);
            ImGui::Text("%s: %zu", LANG("Root Objects"), m_rootObjects.size());
    ImGui::SameLine();
    ImGui::Separator();
    ImGui::SameLine();
            ImGui::Text("%s: %s", LANG("Selected"), m_selectedObject ? m_selectedObject->name.c_str() : LANG("None"));
    ImGui::SameLine();
    ImGui::Separator();
    ImGui::SameLine();
            ImGui::Text("%s: %s", LANG("Filtered"), m_searchFilter.empty() ? LANG("All") : LANG("Filtered"));
    ImGui::EndChild();

    ImGui::Spacing();

    // Hierarchy tree with better styling
            ImGui::Text("%s:", LANG("Scene Hierarchy"));
    ImGui::SameLine();
    helpMarker("Click objects to select them. Double-click to expand/collapse.");

    ImGui::BeginChild("Hierarchy", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

    if (m_rootObjects.empty())
    {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 50);
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), LANG("No objects found in scene"));
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), LANG("Make sure Unity Explorer is properly initialized"));
    }
    else
    {
        for (auto& rootObj : m_rootObjects)
        {
            if (!rootObj) continue;

            // Apply search filter
            if (!m_searchFilter.empty() && !matchesFilter(rootObj->name, m_searchFilter))
            {
                continue;
            }

            // Skip inactive objects if not showing them
            if (!m_showInactiveObjects && !rootObj->isActive)
            {
                continue;
            }

            renderGameObjectNode(rootObj);
        }
    }

    ImGui::EndChild();
    ImGui::End();
}

void UnityExplorer::renderGameObjectNode(std::shared_ptr<GameObjectNode> node)
{
    if (!node || !node->gameObject) return;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    if (node->children.empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (m_selectedObject == node)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    // Visual indicators for object state
    std::string displayName = node->name;
    std::string icon = "●"; // Default icon

    // Different icons based on object state
    if (!node->isActive)
    {
        icon = "○"; // Inactive object
    }
    else if (node->children.empty())
    {
        icon = "◆"; // Leaf object
    }
    else
    {
        icon = "▼"; // Parent object
    }

    // Color inactive objects differently
    if (!node->isActive)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
    }
    else if (m_selectedObject == node)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 1.0f, 1.0f));
    }

    // Render the tree node with icon
    bool nodeOpen = ImGui::TreeNodeEx(node.get(), flags, "%s %s", icon.c_str(), displayName.c_str());

    // Restore color
    if (!node->isActive || m_selectedObject == node)
    {
        ImGui::PopStyleColor();
    }

    // Handle selection
    if (ImGui::IsItemClicked())
    {
        m_selectedObject = node;
        LOG_INFO("[UnityExplorer] Selected object: %s", node->name.c_str());
    }

    // Context menu with better organization
    if (ImGui::BeginPopupContextItem())
    {
        ImGui::Text("%s: %s", LANG("GameObject"), node->name.c_str());
        ImGui::Separator();

        if (ImGui::MenuItem("Copy Name"))
        {
            // TODO: Implement copy to clipboard
        }
        if (ImGui::MenuItem("Copy Path"))
        {
            // TODO: Implement copy full path
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Focus in Inspector"))
        {
            // TODO: Implement focus functionality
        }
        if (ImGui::MenuItem("Delete Object", nullptr, false, false))
        {
            // TODO: Implement delete functionality
        }
        ImGui::EndPopup();
    }

    // Show children with better indentation
    if (nodeOpen)
    {
        ImGui::Indent(10.0f); // Add some indentation for children

        for (auto& child : node->children)
        {
            if (!child) continue;

            // Apply search filter to children
            if (!m_searchFilter.empty() && !matchesFilter(child->name, m_searchFilter))
            {
                continue;
            }

            // Skip inactive children if not showing them
            if (!m_showInactiveObjects && !child->isActive)
            {
                continue;
            }

            renderGameObjectNode(child);
        }

        ImGui::Unindent(10.0f);
        ImGui::TreePop();
    }
}

void UnityExplorer::renderObjectInspector()
{
    if (!ImGui::Begin("Object Inspector", &m_showObjectInspector))
    {
        ImGui::End();
        return;
    }

    if (!m_selectedObject || !m_selectedObject->gameObject)
    {
        // Empty state with better styling
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 50);
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), LANG("No object selected"));
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), LANG("Select an object in the Scene Explorer to inspect it"));
        ImGui::End();
        return;
    }

    auto gameObject = m_selectedObject->gameObject;
    auto transform = m_selectedObject->transform;

    // Header section with object info
    ImGui::BeginChild("InspectorHeader", ImVec2(0, 60), true);
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "%s: %s", LANG("Inspecting"), m_selectedObject->name.c_str());
            ImGui::Text("%s: %s", LANG("Type"), LANG("GameObject"));
        ImGui::Text("%s: 0x%p", LANG("Address"), gameObject);
    ImGui::EndChild();

    ImGui::Spacing();

    // GameObject info section
    renderGameObjectInfo(gameObject);

    ImGui::Spacing();

    // Transform component (always present) with better styling
    if (transform)
    {
        bool transformOpen = m_componentExpandedState["Transform"];
        if (ImGui::CollapsingHeader("Transform", transformOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0))
        {
            m_componentExpandedState["Transform"] = true;
            ImGui::Indent(10.0f);
            renderTransformComponent(transform);
            ImGui::Unindent(10.0f);
        }
        else
        {
            m_componentExpandedState["Transform"] = false;
        }
    }

    // Get and render other components with better organization
    try
    {
        // Try to get specific component types we know about
        auto camera = gameObject->GetComponent<UnityResolve::UnityType::Camera*>(
            GET_CLASS("UnityEngine.CoreModule.dll", "Camera"));
        if (camera)
        {
            bool cameraOpen = m_componentExpandedState["Camera"];
            if (ImGui::CollapsingHeader("Camera", cameraOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0))
            {
                m_componentExpandedState["Camera"] = true;
                ImGui::Indent(10.0f);
                renderCameraComponent(camera);
                ImGui::Unindent(10.0f);
            }
            else
            {
                m_componentExpandedState["Camera"] = false;
            }
        }

        auto renderer = gameObject->GetComponent<UnityResolve::UnityType::Renderer*>(
            GET_CLASS("UnityEngine.CoreModule.dll", "Renderer"));
        if (renderer)
        {
            bool rendererOpen = m_componentExpandedState["Renderer"];
            if (ImGui::CollapsingHeader("Renderer", rendererOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0))
            {
                m_componentExpandedState["Renderer"] = true;
                ImGui::Indent(10.0f);
                renderRendererComponent(renderer);
                ImGui::Unindent(10.0f);
            }
            else
            {
                m_componentExpandedState["Renderer"] = false;
            }
        }

        auto rigidbody = gameObject->GetComponent<UnityResolve::UnityType::Rigidbody*>(
            GET_CLASS("UnityEngine.PhysicsModule.dll", "Rigidbody"));
        if (rigidbody)
        {
            bool rigidbodyOpen = m_componentExpandedState["Rigidbody"];
            if (ImGui::CollapsingHeader("Rigidbody", rigidbodyOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0))
            {
                m_componentExpandedState["Rigidbody"] = true;
                ImGui::Indent(10.0f);
                renderRigidbodyComponent(rigidbody);
                ImGui::Unindent(10.0f);
            }
            else
            {
                m_componentExpandedState["Rigidbody"] = false;
            }
        }

        auto collider = gameObject->GetComponent<UnityResolve::UnityType::Collider*>(
            GET_CLASS("UnityEngine.PhysicsModule.dll", "Collider"));
        if (collider)
        {
            bool colliderOpen = m_componentExpandedState["Collider"];
            if (ImGui::CollapsingHeader("Collider", colliderOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0))
            {
                m_componentExpandedState["Collider"] = true;
                ImGui::Indent(10.0f);
                renderColliderComponent(collider);
                ImGui::Unindent(10.0f);
            }
            else
            {
                m_componentExpandedState["Collider"] = false;
            }
        }
    }
    catch (...)
    {
        ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), LANG("Error retrieving components"));
    }

    ImGui::End();
}

void UnityExplorer::renderGameObjectInfo(UnityResolve::UnityType::GameObject* gameObject)
{
    if (!gameObject) return;

    bool gameObjectOpen = m_componentExpandedState["GameObject"];
    if (ImGui::CollapsingHeader("GameObject", gameObjectOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0))
    {
        m_componentExpandedState["GameObject"] = true;
        ImGui::Indent(10.0f);

        try
        {
            // Basic properties section
            ImGui::Text("%s:", LANG("Basic Properties"));
            ImGui::Separator();

            // Name
            std::string name = getSafeString(gameObject->GetName());
            ImGui::Text("%s: %s", LANG("Name"), name.c_str());

            // Tag
            std::string tag = getSafeString(gameObject->GetTag());
            ImGui::Text("%s: %s", LANG("Tag"), tag.c_str());

            ImGui::Spacing();

            // State properties section
            ImGui::Text("%s:", LANG("State Properties"));
            ImGui::Separator();

            // Active state with colored indicators
            bool activeSelf = gameObject->GetActiveSelf();
            bool activeInHierarchy = gameObject->GetActiveInHierarchy();

            ImGui::Text("%s: ", LANG("Active Self"));
            ImGui::SameLine();
            textColored(activeSelf ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.8f, 0.2f, 0.2f, 1.0f),
                        "%s %s", activeSelf ? "●" : "○", activeSelf ? "True" : "False");

            ImGui::Text("%s: ", LANG("Active in Hierarchy"));
            ImGui::SameLine();
            textColored(activeInHierarchy ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.8f, 0.2f, 0.2f, 1.0f),
                        "%s %s", activeInHierarchy ? "●" : "○", activeInHierarchy ? "True" : "False");

            // Static
            bool isStatic = gameObject->GetIsStatic();
            ImGui::Text("%s: ", LANG("Static"));
            ImGui::SameLine();
            textColored(isStatic ? ImVec4(0.8f, 0.8f, 0.2f, 1.0f) : ImVec4(0.6f, 0.6f, 0.6f, 1.0f),
                        "%s %s", isStatic ? "●" : "○", isStatic ? "True" : "False");

            ImGui::Spacing();

            // Technical info section
            ImGui::Text("Technical Information:");
            ImGui::Separator();

            // Address info
            ImGui::Text("Address: 0x%p", gameObject);

            // // Layer info
            // try
            // {
            //     int layer = gameObject->GetLayer();
            //     ImGui::Text("Layer: %d", layer);
            // }
            // catch (...)
            // {
            //     ImGui::Text("Layer: <Error reading>");
            // }
        }
        catch (...)
        {
            ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "Error reading GameObject data");
        }

        ImGui::Unindent(10.0f);
    }
    else
    {
        m_componentExpandedState["GameObject"] = false;
    }
}

void UnityExplorer::renderTransformComponent(UnityResolve::UnityType::Transform* transform)
{
    if (!transform) return;

    try
    {
        // World Transform section
        ImGui::Text("World Transform:");
        ImGui::Separator();

        auto position = transform->GetPosition();
        renderVector3Field("Position", position);

        auto rotation = transform->GetRotation();
        auto eulerAngles = rotation.ToEuler();
        renderVector3Field("Rotation (Euler)", eulerAngles);
        renderQuaternionField("Rotation (Quaternion)", rotation);

        auto scale = transform->GetLocalScale();
        renderVector3Field("Scale", scale);

        ImGui::Spacing();

        // Local Transform section
        ImGui::Text("Local Transform:");
        ImGui::Separator();

        auto localPos = transform->GetLocalPosition();
        renderVector3Field("Local Position", localPos);

        auto localRot = transform->GetLocalRotation();
        auto localEuler = localRot.ToEuler();
        renderVector3Field("Local Rotation (Euler)", localEuler);
        renderQuaternionField("Local Rotation (Quaternion)", localRot);

        auto localScale = transform->GetLocalScale();
        renderVector3Field("Local Scale", localScale);

        ImGui::Spacing();

        // Hierarchy info section
        ImGui::Text("Hierarchy Information:");
        ImGui::Separator();

        int childCount = transform->GetChildCount();
        ImGui::Text("Children: %d", childCount);

        auto parent = transform->GetParent();
        if (parent)
        {
            auto parentGO = parent->GetGameObject();
            if (parentGO)
            {
                std::string parentName = getSafeString(parentGO->GetName());
                ImGui::Text("Parent: %s", parentName.c_str());
            }
            else
            {
                ImGui::Text("Parent: <Unknown GameObject>");
            }
        }
        else
        {
            ImGui::Text("Parent: None (Root Object)");
        }

        // Sibling info
        // try
        // {
        //     int siblingIndex = transform->GetSiblingIndex();
        //     ImGui::Text("Sibling Index: %d", siblingIndex);
        // }
        // catch (...)
        // {
        //     ImGui::Text("Sibling Index: <Error reading>");
        // }

        ImGui::Spacing();

        // Technical info section
        ImGui::Text("Technical Information:");
        ImGui::Separator();
        ImGui::Text("Transform Address: 0x%p", transform);
    }
    catch (...)
    {
        ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "Error reading Transform data");
    }
}

void UnityExplorer::renderCameraComponent(UnityResolve::UnityType::Camera* camera)
{
    if (!camera) return;

    try
    {
        float fov = camera->GetFoV();
        ImGui::Text("Field of View: %.2f°", fov);

        float depth = camera->GetDepth();
        ImGui::Text("Depth: %.2f", depth);

        // Camera type and other properties would go here
        ImGui::Text("Address: 0x%p", camera);
    }
    catch (...)
    {
        ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "Error reading Camera data");
    }
}

void UnityExplorer::renderRendererComponent(UnityResolve::UnityType::Renderer* renderer)
{
    if (!renderer) return;

    try
    {
        auto bounds = renderer->GetBounds();
        ImGui::Text("Bounds Center: %s", formatVector3(bounds.m_vCenter).c_str());
        ImGui::Text("Bounds Extents: %s", formatVector3(bounds.m_vExtents).c_str());

        ImGui::Text("Address: 0x%p", renderer);
    }
    catch (...)
    {
        ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "Error reading Renderer data");
    }
}

void UnityExplorer::renderRigidbodyComponent(UnityResolve::UnityType::Rigidbody* rigidbody)
{
    if (!rigidbody) return;

    try
    {
        auto velocity = rigidbody->GetVelocity();
        renderVector3Field("Velocity", velocity);

        bool detectCollisions = rigidbody->GetDetectCollisions();
        ImGui::Text("Detect Collisions: %s", detectCollisions ? "True" : "False");

        ImGui::Text("Address: 0x%p", rigidbody);
    }
    catch (...)
    {
        ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "Error reading Rigidbody data");
    }
}

void UnityExplorer::renderColliderComponent(UnityResolve::UnityType::Collider* collider)
{
    if (!collider) return;

    try
    {
        auto bounds = collider->GetBounds();
        ImGui::Text("Bounds Center: %s", formatVector3(bounds.m_vCenter).c_str());
        ImGui::Text("Bounds Extents: %s", formatVector3(bounds.m_vExtents).c_str());

        ImGui::Text("Address: 0x%p", collider);
    }
    catch (...)
    {
        ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "Error reading Collider data");
    }
}

void UnityExplorer::refreshScene()
{
    try
    {
        LOG_INFO("[UnityExplorer] Refreshing scene...");

        m_rootObjects.clear();
        m_objectCache.clear();

        buildHierarchy();

        LOG_INFO("[UnityExplorer] Scene refreshed. Found %zu root objects.", m_rootObjects.size());
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("[UnityExplorer] Exception during scene refresh: %s", e.what());
    } catch (...)
    {
        LOG_ERROR("[UnityExplorer] Unknown exception during scene refresh");
    }
}

void UnityExplorer::buildHierarchy()
{
    auto gameObjectClass = GET_CLASS("UnityEngine.CoreModule.dll", "GameObject");
    if (!gameObjectClass)
    {
        LOG_ERROR("[UnityExplorer] Could not find GameObject class");
        return;
    }

    auto allGameObjects = gameObjectClass->FindObjectsByType<UnityResolve::UnityType::GameObject*>();
    LOG_INFO("[UnityExplorer] Found %zu GameObjects total", allGameObjects.size());

    // Build the hierarchy
    std::unordered_map<UnityResolve::UnityType::Transform*, std::shared_ptr<GameObjectNode>> transformToNode;

    // First pass: create all nodes
    for (auto* go : allGameObjects)
    {
        if (!go) continue;

        auto node = createGameObjectNode(go);
        if (node && node->transform)
        {
            transformToNode[node->transform] = node;
        }
    }

    // Second pass: build parent-child relationships
    for (auto& [transform, node] : transformToNode)
    {
        try
        {
            auto parent = transform->GetParent();
            if (parent && transformToNode.contains(parent))
            {
                // This is a child
                transformToNode[parent]->children.push_back(node);
                node->depth = transformToNode[parent]->depth + 1;
            }
            else
            {
                // This is a root object
                m_rootObjects.push_back(node);
            }
        }
        catch (...)
        {
            // If we can't get parent info, treat as root
            m_rootObjects.push_back(node);
        }
    }

    // Sort root objects by name for consistency
    std::sort(m_rootObjects.begin(), m_rootObjects.end(),
              [](const std::shared_ptr<GameObjectNode>& a, const std::shared_ptr<GameObjectNode>& b)
              {
                  return a->name < b->name;
              });
}

std::shared_ptr<UnityExplorer::GameObjectNode> UnityExplorer::createGameObjectNode(
    UnityResolve::UnityType::GameObject* go, int depth)
{
    if (!go) return nullptr;

    auto node = std::make_shared<GameObjectNode>();
    node->gameObject = go;
    node->depth = depth;

    try
    {
        node->transform = go->GetTransform();
        node->isActive = go->GetActiveSelf();

        // Get name safely
        if (auto nameStr = go->GetName())
        {
            node->name = getSafeString(nameStr);
        }
        else
        {
            node->name = "Unknown GameObject";
        }

        // Cache the node
        m_objectCache[go] = node;
    }
    catch (...)
    {
        node->name = "Error reading GameObject";
        node->isActive = false;
    }

    return node;
}


std::string UnityExplorer::getComponentTypeName(UnityResolve::UnityType::Component* component)
{
    if (!component) return "Unknown";

    try
    {
        if (auto type = component->GetType())
        {
            if (auto nameStr = type->GetFullName())
            {
                return getSafeString(nameStr);
            }
        }
    }
    catch (...)
    {
    }

    return "Component";
}

std::string UnityExplorer::getSafeString(UnityResolve::UnityType::String* unityStr)
{
    if (!unityStr) return "";

    try
    {
        return unityStr->ToString();
    }
    catch (...)
    {
        return "Error reading string";
    }
}

std::string UnityExplorer::formatVector3(const UnityResolve::UnityType::Vector3& vec)
{
    char buffer[128];
    sprintf_s(buffer, sizeof(buffer), "(%.3f, %.3f, %.3f)", vec.x, vec.y, vec.z);
    return std::string(buffer);
}

std::string UnityExplorer::formatQuaternion(const UnityResolve::UnityType::Quaternion& quat)
{
    char buffer[128];
    sprintf_s(buffer, sizeof(buffer), "(%.3f, %.3f, %.3f, %.3f)", quat.x, quat.y, quat.z, quat.w);
    return std::string(buffer);
}

std::string UnityExplorer::formatColor(const UnityResolve::UnityType::Color& color)
{
    char buffer[128];
    sprintf_s(buffer, sizeof(buffer), "(%.3f, %.3f, %.3f, %.3f)", color.r, color.g, color.b, color.a);
    return std::string(buffer);
}

bool UnityExplorer::matchesFilter(const std::string& name, const std::string& filter)
{
    if (filter.empty()) return true;

    std::string lowerName = name;
    std::string lowerFilter = filter;

    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), tolower);
    std::transform(lowerFilter.begin(), lowerFilter.end(), lowerFilter.begin(), tolower);

    return lowerName.find(lowerFilter) != std::string::npos;
}

void UnityExplorer::renderVector3Field(const char* label, const UnityResolve::UnityType::Vector3& vec, bool readOnly)
{
    ImGui::Text("%s:", label);
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);

    // Color code the values for better readability
    ImGui::Text("X: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "%.3f", vec.x);
    ImGui::SameLine();

    ImGui::Text("Y: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%.3f", vec.y);
    ImGui::SameLine();

    ImGui::Text("Z: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "%.3f", vec.z);
}

void UnityExplorer::renderQuaternionField(const char* label, const UnityResolve::UnityType::Quaternion& quat,
                                          bool readOnly)
{
    ImGui::Text("%s:", label);
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);

    // Color code the values for better readability
    ImGui::Text("X: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "%.3f", quat.x);
    ImGui::SameLine();

    ImGui::Text("Y: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%.3f", quat.y);
    ImGui::SameLine();

    ImGui::Text("Z: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "%.3f", quat.z);
    ImGui::SameLine();

    ImGui::Text("W: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.5f, 1.0f), "%.3f", quat.w);
}

void UnityExplorer::renderColorField(const char* label, const UnityResolve::UnityType::Color& color, bool readOnly)
{
    ImGui::Text("%s:", label);
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);

    ImVec4 colorVec(color.r, color.g, color.b, color.a);
    ImGui::ColorButton("##color", colorVec, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
    ImGui::SameLine();

    // Color code the RGBA values
    ImGui::Text("R: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "%.3f", color.r);
    ImGui::SameLine();

    ImGui::Text("G: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%.3f", color.g);
    ImGui::SameLine();

    ImGui::Text("B: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "%.3f", color.b);
    ImGui::SameLine();

    ImGui::Text("A: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%.3f", color.a);
}

void UnityExplorer::helpMarker(const char* desc)
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

void UnityExplorer::textColored(ImVec4 color, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ImGui::TextColoredV(color, fmt, args);
    va_end(args);
}
