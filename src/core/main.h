#pragma once

struct UnityModuleBackendInfo
{
    void* module;
    UnityResolve::Mode mode;
};

class Main
{
public:
    void run();

private:
    UnityModuleBackendInfo getUnityBackend();
    bool initializeUnity();
};
