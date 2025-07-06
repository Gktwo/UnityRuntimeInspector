#pragma once
#include <UnityResolve.hpp>

namespace methods
{
    struct Application
    {
        static int GetTargetFrameRate()
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Application")->Get<
                    UnityResolve::Method>("get_targetFrameRate");
            if (method) return method->Invoke<int>();
            return 0;
        }

        static void SetTargetFrameRate(int value)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Application")->Get<
                    UnityResolve::Method>("set_targetFrameRate");
            if (method) method->Invoke<void>(value);
        }

        static UnityResolve::UnityType::String* GetProductName()
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Application")->Get<
                    UnityResolve::Method>("get_productName");
            if (method) return method->Invoke<UnityResolve::UnityType::String*>();
            return nullptr;
        }

        static UnityResolve::UnityType::String* GetVersion()
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Application")->Get<
                    UnityResolve::Method>("get_version");
            if (method) return method->Invoke<UnityResolve::UnityType::String*>();
            return nullptr;
        }

        static bool IsNotEqual(UnityResolve::UnityType::UnityObject* obj1,
                               UnityResolve::UnityType::UnityObject* obj2)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Object")->Get<
                    UnityResolve::Method>("op_Inequality");
            if (method) return method->Invoke<bool>(obj1, obj2);
            return false;
        }

        static bool IsNull(UnityResolve::UnityType::UnityObject* obj)
        {
            return obj == nullptr;
        }

        static int GetInstanceID(UnityResolve::UnityType::UnityObject* obj)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Object")->Get<
                    UnityResolve::Method>("GetInstanceID");
            if (method) return method->Invoke<int>(obj);
            return 0;
        }

        static bool GetIsPlaying()
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Application")->Get<
                    UnityResolve::Method>("get_isPlaying");
            if (method) return method->Invoke<bool>();
            return false;
        }

        static void Quit()
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Application")->Get<
                    UnityResolve::Method>("Quit");
            if (method) method->Invoke<void>();
        }

        static void Quit(int exitCode)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Application")->Get<
                    UnityResolve::Method>("Quit", {"System.Int32"});
            if (method) method->Invoke<void>(exitCode);
        }
    };

    struct Behaviour
    {
        static bool GetEnabled(UnityResolve::UnityType::Behaviour* behaviour)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Behaviour")->Get<
                    UnityResolve::Method>("get_enabled");
            if (method) return method->Invoke<bool>(behaviour);
            return false;
        }

        static void SetEnabled(UnityResolve::UnityType::Behaviour* behaviour, bool value)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Behaviour")->Get<
                    UnityResolve::Method>("set_enabled");
            if (method) method->Invoke<void>(behaviour, value);
        }

        static bool GetIsActiveAndEnabled(UnityResolve::UnityType::Behaviour* behaviour)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Behaviour")->Get<
                    UnityResolve::Method>("get_isActiveAndEnabled");
            if (method) return method->Invoke<bool>(behaviour);
            return false;
        }
    };

    struct MonoBehaviour
    {
        static void StartCoroutine(UnityResolve::UnityType::MonoBehaviour* monoBehaviour,
                                   UnityResolve::UnityType::String* methodName)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("MonoBehaviour")->Get<
                    UnityResolve::Method>("StartCoroutine", {"System.String"});
            if (method) method->Invoke<void>(monoBehaviour, methodName);
        }

        static void StopCoroutine(UnityResolve::UnityType::MonoBehaviour* monoBehaviour,
                                  UnityResolve::UnityType::String* methodName)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("MonoBehaviour")->Get<
                    UnityResolve::Method>("StopCoroutine", {"System.String"});
            if (method) method->Invoke<void>(monoBehaviour, methodName);
        }

        static void StopAllCoroutines(UnityResolve::UnityType::MonoBehaviour* monoBehaviour)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("MonoBehaviour")->Get<
                    UnityResolve::Method>("StopAllCoroutines");
            if (method) method->Invoke<void>(monoBehaviour);
        }

        static void Invoke(UnityResolve::UnityType::MonoBehaviour* monoBehaviour,
                           UnityResolve::UnityType::String* methodName, float time)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("MonoBehaviour")->Get<
                    UnityResolve::Method>("Invoke");
            if (method) method->Invoke<void>(monoBehaviour, methodName, time);
        }

        static void InvokeRepeating(UnityResolve::UnityType::MonoBehaviour* monoBehaviour,
                                    UnityResolve::UnityType::String* methodName, float time, float repeatRate)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("MonoBehaviour")->Get<
                    UnityResolve::Method>("InvokeRepeating");
            if (method) method->Invoke<void>(monoBehaviour, methodName, time, repeatRate);
        }

        static void CancelInvoke(UnityResolve::UnityType::MonoBehaviour* monoBehaviour)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("MonoBehaviour")->Get<
                    UnityResolve::Method>("CancelInvoke");
            if (method) method->Invoke<void>(monoBehaviour);
        }

        static void CancelInvoke(UnityResolve::UnityType::MonoBehaviour* monoBehaviour,
                                 UnityResolve::UnityType::String* methodName)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("MonoBehaviour")->Get<
                    UnityResolve::Method>("CancelInvoke", {"System.String"});
            if (method) method->Invoke<void>(monoBehaviour, methodName);
        }

        static bool IsInvoking(UnityResolve::UnityType::MonoBehaviour* monoBehaviour)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("MonoBehaviour")->Get<
                    UnityResolve::Method>("IsInvoking");
            if (method) return method->Invoke<bool>(monoBehaviour);
            return false;
        }

        static bool IsInvoking(UnityResolve::UnityType::MonoBehaviour* monoBehaviour,
                               UnityResolve::UnityType::String* methodName)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("MonoBehaviour")->Get<
                    UnityResolve::Method>("IsInvoking", {"System.String"});
            if (method) return method->Invoke<bool>(monoBehaviour, methodName);
            return false;
        }
    };

    struct Screen
    {
        static int GetWidth()
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Screen")->Get<
                    UnityResolve::Method>("get_width");
            if (method) return method->Invoke<int>();
            return 0;
        }

        static int GetHeight()
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Screen")->Get<
                    UnityResolve::Method>("get_height");
            if (method) return method->Invoke<int>();
            return 0;
        }

        static float GetDPI()
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Screen")->Get<
                    UnityResolve::Method>("get_dpi");
            if (method) return method->Invoke<float>();
            return 0.0f;
        }

        static void SetResolution(int width, int height, bool fullscreen)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Screen")->Get<
                    UnityResolve::Method>("SetResolution", {"System.Int32", "System.Int32", "System.Boolean"});
            if (method) method->Invoke<void>(width, height, fullscreen);
        }

        static void SetResolution(int width, int height, int fullscreenMode, int preferredRefreshRate = 0)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Screen")->Get<
                    UnityResolve::Method>("SetResolution",
                                          {"System.Int32", "System.Int32", "UnityEngine.FullScreenMode",
                                           "System.Int32"});
            if (method) method->Invoke<void>(width, height, fullscreenMode, preferredRefreshRate);
        }

        static bool GetFullScreen()
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Screen")->Get<
                    UnityResolve::Method>("get_fullScreen");
            if (method) return method->Invoke<bool>();
            return false;
        }

        static void SetFullScreen(bool fullscreen)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Screen")->Get<
                    UnityResolve::Method>("set_fullScreen");
            if (method) method->Invoke<void>(fullscreen);
        }
    };

    struct Input
    {
        static bool GetKey(int key)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.InputLegacyModule.dll")->Get("Input")->Get<
                    UnityResolve::Method>("GetKey", {"UnityEngine.KeyCode"});
            if (method) return method->Invoke<bool>(key);
            return false;
        }

        static bool GetKey(UnityResolve::UnityType::String* name)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.InputLegacyModule.dll")->Get("Input")->Get<
                    UnityResolve::Method>("GetKey", {"System.String"});
            if (method) return method->Invoke<bool>(name);
            return false;
        }

        static bool GetKeyDown(int key)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.InputLegacyModule.dll")->Get("Input")->Get<
                    UnityResolve::Method>("GetKeyDown", {"UnityEngine.KeyCode"});
            if (method) return method->Invoke<bool>(key);
            return false;
        }

        static bool GetKeyDown(UnityResolve::UnityType::String* name)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.InputLegacyModule.dll")->Get("Input")->Get<
                    UnityResolve::Method>("GetKeyDown", {"System.String"});
            if (method) return method->Invoke<bool>(name);
            return false;
        }

        static bool GetKeyUp(int key)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.InputLegacyModule.dll")->Get("Input")->Get<
                    UnityResolve::Method>("GetKeyUp", {"UnityEngine.KeyCode"});
            if (method) return method->Invoke<bool>(key);
            return false;
        }

        static bool GetKeyUp(UnityResolve::UnityType::String* name)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.InputLegacyModule.dll")->Get("Input")->Get<
                    UnityResolve::Method>("GetKeyUp", {"System.String"});
            if (method) return method->Invoke<bool>(name);
            return false;
        }

        static bool GetMouseButton(int button)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.InputLegacyModule.dll")->Get("Input")->Get<
                    UnityResolve::Method>("GetMouseButton");
            if (method) return method->Invoke<bool>(button);
            return false;
        }

        static bool GetMouseButtonDown(int button)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.InputLegacyModule.dll")->Get("Input")->Get<
                    UnityResolve::Method>("GetMouseButtonDown");
            if (method) return method->Invoke<bool>(button);
            return false;
        }

        static bool GetMouseButtonUp(int button)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.InputLegacyModule.dll")->Get("Input")->Get<
                    UnityResolve::Method>("GetMouseButtonUp");
            if (method) return method->Invoke<bool>(button);
            return false;
        }

        static UnityResolve::UnityType::Vector3 GetMousePosition()
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.InputLegacyModule.dll")->Get("Input")->Get<
                    UnityResolve::Method>("get_mousePosition");
            if (method) return method->Invoke<UnityResolve::UnityType::Vector3>();
            return {};
        }

        static float GetAxis(UnityResolve::UnityType::String* axisName)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.InputLegacyModule.dll")->Get("Input")->Get<
                    UnityResolve::Method>("GetAxis");
            if (method) return method->Invoke<float>(axisName);
            return 0.0f;
        }

        static float GetAxisRaw(UnityResolve::UnityType::String* axisName)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.InputLegacyModule.dll")->Get("Input")->Get<
                    UnityResolve::Method>("GetAxisRaw");
            if (method) return method->Invoke<float>(axisName);
            return 0.0f;
        }

        static bool GetButton(UnityResolve::UnityType::String* buttonName)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.InputLegacyModule.dll")->Get("Input")->Get<
                    UnityResolve::Method>("GetButton");
            if (method) return method->Invoke<bool>(buttonName);
            return false;
        }

        static bool GetButtonDown(UnityResolve::UnityType::String* buttonName)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.InputLegacyModule.dll")->Get("Input")->Get<
                    UnityResolve::Method>("GetButtonDown");
            if (method) return method->Invoke<bool>(buttonName);
            return false;
        }

        static bool GetButtonUp(UnityResolve::UnityType::String* buttonName)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.InputLegacyModule.dll")->Get("Input")->Get<
                    UnityResolve::Method>("GetButtonUp");
            if (method) return method->Invoke<bool>(buttonName);
            return false;
        }
    };

    struct Cursor
    {
        static void SetLockState(int lockState)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Cursor")->Get<
                    UnityResolve::Method>("set_lockState");
            if (method) method->Invoke<void>(lockState);
        }

        static int GetLockState()
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Cursor")->Get<
                    UnityResolve::Method>("get_lockState");
            if (method) return method->Invoke<int>();
            return 0;
        }

        static void SetVisible(bool visible)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Cursor")->Get<
                    UnityResolve::Method>("set_visible");
            if (method) method->Invoke<void>(visible);
        }

        static bool GetVisible()
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Cursor")->Get<
                    UnityResolve::Method>("get_visible");
            if (method) return method->Invoke<bool>();
            return true;
        }
    };

    struct Resources
    {
        template <typename T>
        static T Load(UnityResolve::UnityType::String* path)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Resources")->Get<
                    UnityResolve::Method>("Load", {"System.String"});
            if (method) return method->Invoke<T>(path);
            return T();
        }

        template <typename T>
        static T Load(UnityResolve::UnityType::String* path, UnityResolve::Class* type)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Resources")->Get<
                    UnityResolve::Method>("Load", {"System.String", "System.Type"});
            if (method) return method->Invoke<T>(path, type->GetType());
            return T();
        }

        template <typename T>
        static std::vector<T> LoadAll(UnityResolve::UnityType::String* path)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Resources")->Get<
                    UnityResolve::Method>("LoadAll", {"System.String"});
            if (method)
            {
                auto array = method->Invoke<UnityResolve::UnityType::Array<T>*>(path);
                return array ? array->ToVector() : std::vector<T>();
            }
            return {};
        }

        static void UnloadAsset(UnityResolve::UnityType::UnityObject* assetToUnload)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Resources")->Get<
                    UnityResolve::Method>("UnloadAsset");
            if (method) method->Invoke<void>(assetToUnload);
        }

        static void UnloadUnusedAssets()
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Resources")->Get<
                    UnityResolve::Method>("UnloadUnusedAssets");
            if (method) method->Invoke<void>();
        }
    };

    struct QualitySettings
    {
        static void SetVSyncCount(int value)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("QualitySettings")->Get<
                    UnityResolve::Method>("set_vSyncCount");
            if (method) method->Invoke<void>(value);
        }

        static int GetVSyncCount()
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("QualitySettings")->Get<
                    UnityResolve::Method>("get_vSyncCount");
            if (method) return method->Invoke<int>();
            return 0;
        }

        static void SetAnisotropicFiltering(int value)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("QualitySettings")->Get<
                    UnityResolve::Method>("set_anisotropicFiltering");
            if (method) method->Invoke<void>(value);
        }

        static void SetAntiAliasing(int value)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("QualitySettings")->Get<
                    UnityResolve::Method>("set_antiAliasing");
            if (method) method->Invoke<void>(value);
        }
    };

    struct GameObject
    {
        template <typename T>
        static bool TryGetComponent(UnityResolve::UnityType::GameObject* gameObject, T*& component)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("GameObject")->Get<
                    UnityResolve::Method>("TryGetComponent");
            if (method) return method->Invoke<bool>(gameObject, &component);
            return false;
        }

        template <typename T>
        static bool TryGetComponent(UnityResolve::UnityType::GameObject* gameObject, UnityResolve::Class* type,
                                    T*& component)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")
                         ->Get("GameObject")
                         ->Get<UnityResolve::Method>("TryGetComponent", {"System.Type", "UnityEngine.Component&"});
            if (method) return method->Invoke<bool>(gameObject, type->GetType(), &component);
            return false;
        }

        template <typename T>
        static T AddComponent(UnityResolve::UnityType::GameObject* gameObject)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("GameObject")->Get<
                    UnityResolve::Method>("AddComponent");
            if (method) return method->Invoke<T>(gameObject);
            return T();
        }

        template <typename T>
        static T AddComponent(UnityResolve::UnityType::GameObject* gameObject, UnityResolve::Class* type)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("GameObject")->Get<
                    UnityResolve::Method>("AddComponent", {"System.Type"});
            if (method) return method->Invoke<T>(gameObject, type->GetType());
            return T();
        }

        static void DestroyImmediate(UnityResolve::UnityType::GameObject* obj, bool allowDestroyingAssets = false)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("GameObject")->Get<
                    UnityResolve::Method>("DestroyImmediate", {"UnityEngine.Object", "System.Boolean"});
            if (method) method->Invoke<void>(obj, allowDestroyingAssets);
        }

        static UnityResolve::UnityType::GameObject* CreatePrimitive(int primitiveType)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("GameObject")->Get<
                    UnityResolve::Method>("CreatePrimitive");
            if (method) return method->Invoke<UnityResolve::UnityType::GameObject*>(primitiveType);
            return nullptr;
        }

        static void SetLayer(UnityResolve::UnityType::GameObject* gameObject, int layer)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("GameObject")->Get<
                    UnityResolve::Method>("set_layer");
            if (method) method->Invoke<void>(gameObject, layer);
        }

        static int GetLayer(UnityResolve::UnityType::GameObject* gameObject)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("GameObject")->Get<
                    UnityResolve::Method>("get_layer");
            if (method) return method->Invoke<int>(gameObject);
            return 0;
        }

        static void SetTag(UnityResolve::UnityType::GameObject* gameObject, UnityResolve::UnityType::String* tag)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("GameObject")->Get<
                    UnityResolve::Method>("set_tag");
            if (method) method->Invoke<void>(gameObject, tag);
        }

        static bool CompareTag(UnityResolve::UnityType::GameObject* gameObject, UnityResolve::UnityType::String* tag)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("GameObject")->Get<
                    UnityResolve::Method>("CompareTag");
            if (method) return method->Invoke<bool>(gameObject, tag);
            return false;
        }
    };

    struct Component
    {
        template <typename T>
        static bool TryGetComponent(UnityResolve::UnityType::Component* component, T*& result)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Component")->Get<
                    UnityResolve::Method>("TryGetComponent");
            if (method) return method->Invoke<bool>(component, &result);
            return false;
        }

        template <typename T>
        static bool TryGetComponent(UnityResolve::UnityType::Component* component, UnityResolve::Class* type,
                                    T*& result)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")
                         ->Get("Component")
                         ->Get<UnityResolve::Method>("TryGetComponent", {"System.Type", "UnityEngine.Component&"});
            if (method) return method->Invoke<bool>(component, type->GetType(), &result);
            return false;
        }

        template <typename T>
        static T GetComponent(UnityResolve::UnityType::Component* component)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Component")->Get<
                    UnityResolve::Method>("GetComponent");
            if (method) return method->Invoke<T>(component);
            return T();
        }

        template <typename T>
        static T GetComponent(UnityResolve::UnityType::Component* component, UnityResolve::Class* type)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Component")->Get<
                    UnityResolve::Method>("GetComponent", {"System.Type"});
            if (method) return method->Invoke<T>(component, type->GetType());
            return T();
        }

        template <typename T>
        static T GetComponentInChildren(UnityResolve::UnityType::Component* component, bool includeInactive = false)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Component")->Get<
                    UnityResolve::Method>("GetComponentInChildren", {"System.Boolean"});
            if (method) return method->Invoke<T>(component, includeInactive);
            return T();
        }

        template <typename T>
        static T GetComponentInParent(UnityResolve::UnityType::Component* component, bool includeInactive = false)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Component")->Get<
                    UnityResolve::Method>("GetComponentInParent", {"System.Boolean"});
            if (method) return method->Invoke<T>(component, includeInactive);
            return T();
        }

        static bool CompareTag(UnityResolve::UnityType::Component* component, UnityResolve::UnityType::String* tag)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Component")->Get<
                    UnityResolve::Method>("CompareTag");
            if (method) return method->Invoke<bool>(component, tag);
            return false;
        }

        static void SendMessage(UnityResolve::UnityType::Component* component,
                                UnityResolve::UnityType::String* methodName)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Component")->Get<
                    UnityResolve::Method>("SendMessage", {"System.String"});
            if (method) method->Invoke<void>(component, methodName);
        }

        static void SendMessageUpwards(UnityResolve::UnityType::Component* component,
                                       UnityResolve::UnityType::String* methodName)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Component")->Get<
                    UnityResolve::Method>("SendMessageUpwards", {"System.String"});
            if (method) method->Invoke<void>(component, methodName);
        }

        static void BroadcastMessage(UnityResolve::UnityType::Component* component,
                                     UnityResolve::UnityType::String* methodName)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Component")->Get<
                    UnityResolve::Method>("BroadcastMessage", {"System.String"});
            if (method) method->Invoke<void>(component, methodName);
        }
    };

    struct Transform
    {
        static void SetParent(UnityResolve::UnityType::Transform* transform, UnityResolve::UnityType::Transform* parent,
                              bool worldPositionStays = true)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform")->Get<
                    UnityResolve::Method>("SetParent", {"UnityEngine.Transform", "System.Boolean"});
            if (method) method->Invoke<void>(transform, parent, worldPositionStays);
        }

        static void SetParent(UnityResolve::UnityType::Transform* transform, UnityResolve::UnityType::Transform* parent)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform")->Get<
                    UnityResolve::Method>("SetParent", {"UnityEngine.Transform"});
            if (method) method->Invoke<void>(transform, parent);
        }

        static void DetachChildren(UnityResolve::UnityType::Transform* transform)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform")->Get<
                    UnityResolve::Method>("DetachChildren");
            if (method) method->Invoke<void>(transform);
        }

        static int GetSiblingIndex(UnityResolve::UnityType::Transform* transform)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform")->Get<
                    UnityResolve::Method>("GetSiblingIndex");
            if (method) return method->Invoke<int>(transform);
            return -1;
        }

        static void SetSiblingIndex(UnityResolve::UnityType::Transform* transform, int index)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform")->Get<
                    UnityResolve::Method>("SetSiblingIndex");
            if (method) method->Invoke<void>(transform, index);
        }

        static void SetAsFirstSibling(UnityResolve::UnityType::Transform* transform)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform")->Get<
                    UnityResolve::Method>("SetAsFirstSibling");
            if (method) method->Invoke<void>(transform);
        }

        static void SetAsLastSibling(UnityResolve::UnityType::Transform* transform)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform")->Get<
                    UnityResolve::Method>("SetAsLastSibling");
            if (method) method->Invoke<void>(transform);
        }

        static UnityResolve::UnityType::Transform* Find(UnityResolve::UnityType::Transform* transform,
                                                        UnityResolve::UnityType::String* name)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform")->Get<
                    UnityResolve::Method>("Find");
            if (method) return method->Invoke<UnityResolve::UnityType::Transform*>(transform, name);
            return nullptr;
        }

        static bool IsChildOf(UnityResolve::UnityType::Transform* transform, UnityResolve::UnityType::Transform* parent)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform")->Get<
                    UnityResolve::Method>("IsChildOf");
            if (method) return method->Invoke<bool>(transform, parent);
            return false;
        }

        static UnityResolve::UnityType::Vector3 InverseTransformPoint(UnityResolve::UnityType::Transform* transform,
                                                                      UnityResolve::UnityType::Vector3 position)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform")->Get<
                    UnityResolve::Method>("InverseTransformPoint");
            if (method) return method->Invoke<UnityResolve::UnityType::Vector3>(transform, position);
            return {};
        }

        static UnityResolve::UnityType::Vector3 InverseTransformDirection(UnityResolve::UnityType::Transform* transform,
                                                                          UnityResolve::UnityType::Vector3 direction)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform")->Get<
                    UnityResolve::Method>("InverseTransformDirection");
            if (method) return method->Invoke<UnityResolve::UnityType::Vector3>(transform, direction);
            return {};
        }

        static UnityResolve::UnityType::Vector3 TransformDirection(UnityResolve::UnityType::Transform* transform,
                                                                   UnityResolve::UnityType::Vector3 direction)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform")->Get<
                    UnityResolve::Method>("TransformDirection");
            if (method) return method->Invoke<UnityResolve::UnityType::Vector3>(transform, direction);
            return {};
        }

        static void Translate(UnityResolve::UnityType::Transform* transform,
                              UnityResolve::UnityType::Vector3 translation)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform")->Get<
                    UnityResolve::Method>("Translate", {"UnityEngine.Vector3"});
            if (method) method->Invoke<void>(transform, translation);
        }

        static void Rotate(UnityResolve::UnityType::Transform* transform, UnityResolve::UnityType::Vector3 eulers,
                           int relativeTo = 0)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform")->Get<
                    UnityResolve::Method>("Rotate", {"UnityEngine.Vector3", "UnityEngine.Space"});
            if (method) method->Invoke<void>(transform, eulers, relativeTo);
        }

        static void RotateAround(UnityResolve::UnityType::Transform* transform, UnityResolve::UnityType::Vector3 point,
                                 UnityResolve::UnityType::Vector3 axis, float angle)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Transform")->Get<
                    UnityResolve::Method>("RotateAround");
            if (method) method->Invoke<void>(transform, point, axis, angle);
        }
    };

    struct Camera
    {
        static void SetClearFlags(UnityResolve::UnityType::Camera* camera, int clearFlags)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Camera")->Get<UnityResolve::Method>(
                    "set_clearFlags");
            if (method) method->Invoke<void>(camera, clearFlags);
        }

        static int GetClearFlags(UnityResolve::UnityType::Camera* camera)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Camera")->Get<UnityResolve::Method>(
                    "get_clearFlags");
            if (method) return method->Invoke<int>(camera);
            return 0;
        }

        static void SetCullingMask(UnityResolve::UnityType::Camera* camera, int mask)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Camera")->Get<UnityResolve::Method>(
                    "set_cullingMask");
            if (method) method->Invoke<void>(camera, mask);
        }

        static int GetCullingMask(UnityResolve::UnityType::Camera* camera)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Camera")->Get<UnityResolve::Method>(
                    "get_cullingMask");
            if (method) return method->Invoke<int>(camera);
            return 0;
        }

        static void SetNearClipPlane(UnityResolve::UnityType::Camera* camera, float nearClip)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Camera")->Get<UnityResolve::Method>(
                    "set_nearClipPlane");
            if (method) method->Invoke<void>(camera, nearClip);
        }

        static float GetNearClipPlane(UnityResolve::UnityType::Camera* camera)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Camera")->Get<UnityResolve::Method>(
                    "get_nearClipPlane");
            if (method) return method->Invoke<float>(camera);
            return 0.0f;
        }

        static void SetFarClipPlane(UnityResolve::UnityType::Camera* camera, float farClip)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Camera")->Get<UnityResolve::Method>(
                    "set_farClipPlane");
            if (method) method->Invoke<void>(camera, farClip);
        }

        static float GetFarClipPlane(UnityResolve::UnityType::Camera* camera)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Camera")->Get<UnityResolve::Method>(
                    "get_farClipPlane");
            if (method) return method->Invoke<float>(camera);
            return 0.0f;
        }

        static void SetOrthographic(UnityResolve::UnityType::Camera* camera, bool orthographic)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Camera")->Get<UnityResolve::Method>(
                    "set_orthographic");
            if (method) method->Invoke<void>(camera, orthographic);
        }

        static bool GetOrthographic(UnityResolve::UnityType::Camera* camera)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Camera")->Get<UnityResolve::Method>(
                    "get_orthographic");
            if (method) return method->Invoke<bool>(camera);
            return false;
        }

        static void SetOrthographicSize(UnityResolve::UnityType::Camera* camera, float size)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Camera")->Get<UnityResolve::Method>(
                    "set_orthographicSize");
            if (method) method->Invoke<void>(camera, size);
        }

        static float GetOrthographicSize(UnityResolve::UnityType::Camera* camera)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Camera")->Get<UnityResolve::Method>(
                    "get_orthographicSize");
            if (method) return method->Invoke<float>(camera);
            return 0.0f;
        }

        static void Render(UnityResolve::UnityType::Camera* camera)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Camera")->Get<
                    UnityResolve::Method>("Render");
            if (method) method->Invoke<void>(camera);
        }
    };

    struct Object
    {
        static void DontDestroyOnLoad(UnityResolve::UnityType::UnityObject* target)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Object")->Get<UnityResolve::Method>(
                    "DontDestroyOnLoad");
            if (method) method->Invoke<void>(target);
        }

        static void DestroyImmediate(UnityResolve::UnityType::UnityObject* obj, bool allowDestroyingAssets = false)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Object")->Get<UnityResolve::Method>(
                    "DestroyImmediate", {"UnityEngine.Object", "System.Boolean"});
            if (method) method->Invoke<void>(obj, allowDestroyingAssets);
        }

        template <typename T>
        static T FindObjectOfType(bool includeInactive = false)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Object")->Get<UnityResolve::Method>(
                    "FindObjectOfType", {"System.Boolean"});
            if (method) return method->Invoke<T>(includeInactive);
            return T();
        }

        template <typename T>
        static std::vector<T> FindObjectsOfType(bool includeInactive = false)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Object")->Get<UnityResolve::Method>(
                    "FindObjectsOfType", {"System.Boolean"});
            if (method)
            {
                auto array = method->Invoke<UnityResolve::UnityType::Array<T>*>(includeInactive);
                return array ? array->ToVector() : std::vector<T>();
            }
            return {};
        }

        static bool IsEqual(UnityResolve::UnityType::UnityObject* x, UnityResolve::UnityType::UnityObject* y)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Object")->Get<UnityResolve::Method>(
                    "op_Equality");
            if (method) return method->Invoke<bool>(x, y);
            return x == y;
        }

        static bool IsNotEqual(UnityResolve::UnityType::UnityObject* x, UnityResolve::UnityType::UnityObject* y)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Object")->Get<UnityResolve::Method>(
                    "op_Inequality");
            if (method) return method->Invoke<bool>(x, y);
            return x != y;
        }

        static bool IsNull(UnityResolve::UnityType::UnityObject* obj)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Object")->Get<UnityResolve::Method>(
                    "op_Equality");
            if (method) return method->Invoke<bool>(obj, nullptr);
            return false;
        }

        static bool IsNotNull(UnityResolve::UnityType::UnityObject* obj)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Object")->Get<UnityResolve::Method>(
                    "op_Inequality");
            if (method) return method->Invoke<bool>(obj, nullptr);
            return true;
        }

        static int GetInstanceID(UnityResolve::UnityType::UnityObject* obj)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Object")->Get<UnityResolve::Method>(
                    "GetInstanceID");
            if (method) return method->Invoke<int>(obj);
            return 0;
        }

        static UnityResolve::UnityType::String* GetName(UnityResolve::UnityType::UnityObject* obj)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Object")->Get<UnityResolve::Method>(
                    "get_name");
            if (method) return method->Invoke<UnityResolve::UnityType::String*>(obj);
            return nullptr;
        }

        static void SetName(UnityResolve::UnityType::UnityObject* obj, UnityResolve::UnityType::String* name)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Object")->Get<UnityResolve::Method>(
                    "set_name");
            if (method) method->Invoke<void>(obj, name);
        }

        static UnityResolve::UnityType::String* ToString(UnityResolve::UnityType::UnityObject* obj)
        {
            static UnityResolve::Method* method;
            if (!method)
                method = UnityResolve::Get("UnityEngine.CoreModule.dll")->Get("Object")->Get<UnityResolve::Method>(
                    "ToString");
            if (method) return method->Invoke<UnityResolve::UnityType::String*>(obj);
            return nullptr;
        }
    };
}
