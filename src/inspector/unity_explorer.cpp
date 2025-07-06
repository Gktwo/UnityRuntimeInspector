#include "pch.h"
#include "unity_explorer.h"

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
    if (currentTime - m_lastRefreshTime > m_refreshInterval)
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
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::MenuItem("Show Inactive Objects", nullptr, &m_showInactiveObjects);
            ImGui::Separator();
            if (ImGui::MenuItem("Refresh Scene", "F5"))
            {
                m_needsRefresh = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // Toolbar
    if (ImGui::Button("Refresh"))
    {
        m_needsRefresh = true;
    }
    ImGui::SameLine();
    ImGui::Checkbox("Show Inactive", &m_showInactiveObjects);

    // Search filter
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputTextWithHint("##search", "Search objects...", m_searchBuffer, sizeof(m_searchBuffer)))
    {
        m_searchFilter = std::string(m_searchBuffer);
    }

    ImGui::Separator();

    // Status
    ImGui::Text("Root Objects: %zu | Selected: %s",
                m_rootObjects.size(),
                m_selectedObject ? m_selectedObject->name.c_str() : "None");

    ImGui::Separator();

    // Hierarchy tree
    ImGui::BeginChild("Hierarchy", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

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

    // Color inactive objects differently
    if (!node->isActive)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
    }

    std::string displayName = node->name;
    bool nodeOpen = ImGui::TreeNodeEx(node.get(), flags, "%s", displayName.c_str());

    if (!node->isActive)
    {
        ImGui::PopStyleColor();
    }

    // Handle selection
    if (ImGui::IsItemClicked())
    {
        m_selectedObject = node;
        LOG_INFO("[UnityExplorer] Selected object: %s", node->name.c_str());
    }

    // Context menu
    if (ImGui::BeginPopupContextItem())
    {
        ImGui::Text("GameObject: %s", node->name.c_str());
        ImGui::Separator();
        ImGui::MenuItem("Copy Name", nullptr, false, false); // Placeholder
        ImGui::MenuItem("Copy Path", nullptr, false, false); // Placeholder
        ImGui::EndPopup();
    }

    // Show children
    if (nodeOpen)
    {
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
        ImGui::Text("No object selected");
        ImGui::Text("Select an object in the Scene Explorer to inspect it.");
        ImGui::End();
        return;
    }

    auto gameObject = m_selectedObject->gameObject;
    auto transform = m_selectedObject->transform;

    // GameObject info
    renderGameObjectInfo(gameObject);

    ImGui::Separator();

    // Transform component (always present)
    if (transform)
    {
        bool transformOpen = m_componentExpandedState["Transform"];
        if (ImGui::CollapsingHeader("Transform", transformOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0))
        {
            m_componentExpandedState["Transform"] = true;
            renderTransformComponent(transform);
        }
        else
        {
            m_componentExpandedState["Transform"] = false;
        }
    }

    // Get and render other components
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
                renderCameraComponent(camera);
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
                renderRendererComponent(renderer);
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
                renderRigidbodyComponent(rigidbody);
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
                renderColliderComponent(collider);
            }
            else
            {
                m_componentExpandedState["Collider"] = false;
            }
        }
    }
    catch (...)
    {
        ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "Error retrieving components");
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

        try
        {
            // Name
            std::string name = getSafeString(gameObject->GetName());
            ImGui::Text("Name: %s", name.c_str());

            // Tag
            std::string tag = getSafeString(gameObject->GetTag());
            ImGui::Text("Tag: %s", tag.c_str());

            // Active state
            bool activeSelf = gameObject->GetActiveSelf();
            bool activeInHierarchy = gameObject->GetActiveInHierarchy();

            ImGui::Text("Active Self: ");
            ImGui::SameLine();
            textColored(activeSelf ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1), "%s", activeSelf ? "True" : "False");

            ImGui::Text("Active in Hierarchy: ");
            ImGui::SameLine();
            textColored(activeInHierarchy ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1), "%s",
                        activeInHierarchy ? "True" : "False");

            // Static
            bool isStatic = gameObject->GetIsStatic();
            ImGui::Text("Static: %s", isStatic ? "True" : "False");

            // Address info
            ImGui::Separator();
            ImGui::Text("Address: 0x%p", gameObject);
        }
        catch (...)
        {
            ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "Error reading GameObject data");
        }
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
        // World Transform
        ImGui::Text("World Transform:");

        auto position = transform->GetPosition();
        renderVector3Field("Position", position);

        auto rotation = transform->GetRotation();
        auto eulerAngles = rotation.ToEuler();
        renderVector3Field("Rotation", eulerAngles);
        renderQuaternionField("Rotation (Quat)", rotation);

        auto scale = transform->GetLocalScale();
        renderVector3Field("Scale", scale);

        ImGui::Separator();

        // Local Transform
        ImGui::Text("Local Transform:");

        auto localPos = transform->GetLocalPosition();
        renderVector3Field("Local Position", localPos);

        auto localRot = transform->GetLocalRotation();
        auto localEuler = localRot.ToEuler();
        renderVector3Field("Local Rotation", localEuler);

        auto localScale = transform->GetLocalScale();
        renderVector3Field("Local Scale", localScale);

        ImGui::Separator();

        // Hierarchy info
        ImGui::Text("Hierarchy:");

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
        }
        else
        {
            ImGui::Text("Parent: None (Root)");
        }
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
    ImGui::Text("(%.3f, %.3f, %.3f)", vec.x, vec.y, vec.z);
}

void UnityExplorer::renderQuaternionField(const char* label, const UnityResolve::UnityType::Quaternion& quat,
                                          bool readOnly)
{
    ImGui::Text("%s:", label);
    ImGui::SameLine();
    ImGui::Text("(%.3f, %.3f, %.3f, %.3f)", quat.x, quat.y, quat.z, quat.w);
}

void UnityExplorer::renderColorField(const char* label, const UnityResolve::UnityType::Color& color, bool readOnly)
{
    ImGui::Text("%s:", label);
    ImGui::SameLine();
    ImVec4 colorVec(color.r, color.g, color.b, color.a);
    ImGui::ColorButton("##color", colorVec, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
    ImGui::SameLine();
    ImGui::Text("(%.3f, %.3f, %.3f, %.3f)", color.r, color.g, color.b, color.a);
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
