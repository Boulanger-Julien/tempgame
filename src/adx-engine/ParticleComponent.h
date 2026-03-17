#pragma once
class ParticleComponent
{
public:
    IParticleSystemResource* gpuResource = nullptr;

    bool isInitialized = false;

    ~ParticleComponent() {
        if (gpuResource) {
            gpuResource->Release();
            delete gpuResource;
            gpuResource = nullptr;
        }
    }
};

