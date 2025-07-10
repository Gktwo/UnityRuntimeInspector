#pragma once

class UnityExplorer
{
public:
    struct GameObjectNode
    {
        UnityResolve::UnityType::GameObject* gameObject = nullptr;
        UnityResolve::UnityType::Transform* transform = nullptr;
        std::string name;
        std::vector<std::shared_ptr<GameObjectNode>> children;
        bool expanded = false;
        int depth = 0;
        bool isActive = true;
    };

private:
    // UI State
    bool m_showSceneExplorer = true;
    bool m_showObjectInspector = true;
    bool m_showInactiveObjects = false;
    bool m_autoRefresh = false;
    bool m_initialized = false;

    // Search and filtering
    char m_searchBuffer[256] = "";
    std::string m_searchFilter;

    // Scene data
    std::vector<std::shared_ptr<GameObjectNode>> m_rootObjects;
    std::shared_ptr<GameObjectNode> m_selectedObject = nullptr;

    // Caching and performance
    bool m_needsRefresh = true;
    std::unordered_map<void*, std::shared_ptr<GameObjectNode>> m_objectCache;
    float m_lastRefreshTime = 0.0f;
    float m_refreshInterval = 1.0f;

    // Inspector state
    std::unordered_map<std::string, bool> m_componentExpandedState;

public:
    bool initialize();
    void update();
    void shutdown();

    // Public interface
    bool isVisible() const { return m_showSceneExplorer || m_showObjectInspector; }
    bool isAutoRefreshEnabled() const { return m_autoRefresh; }
    void setAutoRefresh(bool enabled) { m_autoRefresh = enabled; }

    void setVisible(bool visible)
    {
        m_showSceneExplorer = visible;
        m_showObjectInspector = visible;
    }

    void toggleVisibility() { setVisible(!isVisible()); }

private:
    // Scene Explorer
    void renderSceneExplorer();
    void refreshScene();
    void buildHierarchy();
    void renderGameObjectNode(std::shared_ptr<GameObjectNode> node);
    std::shared_ptr<GameObjectNode> createGameObjectNode(UnityResolve::UnityType::GameObject* go, int depth = 0);

    // Object Inspector
    void renderObjectInspector();
    void renderGameObjectInfo(UnityResolve::UnityType::GameObject* gameObject);
    void renderTransformComponent(UnityResolve::UnityType::Transform* transform);
    void renderGenericComponent(UnityResolve::UnityType::Component* component, const std::string& typeName);
    void renderCameraComponent(UnityResolve::UnityType::Camera* camera);
    void renderRendererComponent(UnityResolve::UnityType::Renderer* renderer);
    void renderRigidbodyComponent(UnityResolve::UnityType::Rigidbody* rigidbody);
    void renderColliderComponent(UnityResolve::UnityType::Collider* collider);

    std::string getComponentTypeName(UnityResolve::UnityType::Component* component);

    // Utility functions
    std::string getSafeString(UnityResolve::UnityType::String* unityStr);
    std::string formatVector3(const UnityResolve::UnityType::Vector3& vec);
    std::string formatQuaternion(const UnityResolve::UnityType::Quaternion& quat);
    std::string formatColor(const UnityResolve::UnityType::Color& color);
    bool matchesFilter(const std::string& name, const std::string& filter);
    void renderVector3Field(const char* label, const UnityResolve::UnityType::Vector3& vec, bool readOnly = true);
    void renderQuaternionField(const char* label, const UnityResolve::UnityType::Quaternion& quat,
                               bool readOnly = true);
    void renderColorField(const char* label, const UnityResolve::UnityType::Color& color, bool readOnly = true);

    // ImGui helpers
    void helpMarker(const char* desc);
    void textColored(ImVec4 color, const char* fmt, ...);
};
