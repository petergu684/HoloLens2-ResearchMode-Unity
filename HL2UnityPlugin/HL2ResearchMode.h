#pragma once
#include "HL2ResearchMode.g.h"
#include "ResearchModeApi.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <cmath>
#include <DirectXMath.h>

namespace winrt::HL2UnityPlugin::implementation
{
    struct HL2ResearchMode : HL2ResearchModeT<HL2ResearchMode>
    {
        HL2ResearchMode() = default;

        UINT16 GetCenterDepth();
        int GetBufferSize();
        hstring PrintResolution();
        void InitializeDepthSensor();
        void StartDepthSensorLoop();
        
        void StartDepthSensorOnce();
        void PauseSensorsAndFreeSpace();
        void StopAllSensorDevice();
        bool DepthMapTextureUpdated();
        com_array<uint16_t> GetDepthMapBuffer();
        com_array<uint8_t> GetDepthMapTextureBuffer();
        std::mutex mu;
        
    private:
        float* m_pointCloud = nullptr;
        UINT16* m_depthMap = nullptr;
        UINT8* m_depthMapTexture = nullptr;
        IResearchModeSensor* m_depthSensor = nullptr;
        IResearchModeCameraSensor* m_pDepthCameraSensor = nullptr;
        ResearchModeSensorResolution m_resolution;
        IResearchModeSensorDevice* m_pSensorDevice;
        std::vector<ResearchModeSensorDescriptor> m_sensorDescriptors;
        IResearchModeSensorDeviceConsent* m_pSensorDeviceConsent;
        std::atomic_int m_bufferSize = 0;
        std::atomic_uint16_t m_centerDepth = 0;
        std::atomic_bool m_depthSensorLoopStarted = false;
        std::atomic_bool m_depthMapTextureUpdated = true;
        static void DepthSensorLoop(HL2ResearchMode* pHL2ResearchMode);
        static void CamAccessOnComplete(ResearchModeSensorConsent consent);
        DirectX::XMFLOAT4X4 m_depthCameraPose;
        std::thread* m_pDepthUpdateThread;
    };
}
namespace winrt::HL2UnityPlugin::factory_implementation
{
    struct HL2ResearchMode : HL2ResearchModeT<HL2ResearchMode, implementation::HL2ResearchMode>
    {
    };
}
