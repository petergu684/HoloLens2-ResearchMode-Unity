#pragma once
#include "HL2ResearchMode.g.h"
#include "ResearchModeApi.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <wchar.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <cmath>
#include <DirectXMath.h>
#include <vector>
#include<winrt/Windows.Perception.Spatial.h>
#include<winrt/Windows.Perception.Spatial.Preview.h>

namespace winrt::HL2UnityPlugin::implementation
{
    struct HL2ResearchMode : HL2ResearchModeT<HL2ResearchMode>
    {
        HL2ResearchMode() = default;

        UINT16 GetCenterDepth();
        int GetBufferSize();
        hstring PrintResolution();
        hstring PrintDepthExtrinsics();
        void InitializeDepthSensor();
        void StartDepthSensorLoop();
        void StopAllSensorDevice();
        bool DepthMapTextureUpdated();
        bool PointCloudUpdated();
        void SetReferenceCoordinateSystem(Windows::Perception::Spatial::SpatialCoordinateSystem refCoord);
        com_array<uint16_t> GetDepthMapBuffer();
        com_array<uint8_t> GetDepthMapTextureBuffer();
        com_array<float> GetPointCloudBuffer();
        com_array<float> GetCenterPoint();
        std::mutex mu;
        
        
    private:
        float* m_pointCloud = nullptr;
        int m_pointcloudLength = 0;
        UINT16* m_depthMap = nullptr;
        UINT8* m_depthMapTexture = nullptr;
        IResearchModeSensor* m_depthSensor = nullptr;
        IResearchModeCameraSensor* m_pDepthCameraSensor = nullptr;
        ResearchModeSensorResolution m_resolution;
        IResearchModeSensorDevice* m_pSensorDevice = nullptr;
        std::vector<ResearchModeSensorDescriptor> m_sensorDescriptors;
        IResearchModeSensorDeviceConsent* m_pSensorDeviceConsent = nullptr;
        Windows::Perception::Spatial::SpatialLocator m_locator = 0;
        Windows::Perception::Spatial::SpatialCoordinateSystem m_refFrame = nullptr;
        std::atomic_int m_bufferSize = 0;
        std::atomic_uint16_t m_centerDepth = 0;
        float m_centerPoint[3];
        std::atomic_bool m_depthSensorLoopStarted = false;
        std::atomic_bool m_depthMapTextureUpdated = false;
        std::atomic_bool m_pointCloudUpdated = false;
        static void DepthSensorLoop(HL2ResearchMode* pHL2ResearchMode);
        static void CamAccessOnComplete(ResearchModeSensorConsent consent);
        std::string MatrixToString(DirectX::XMFLOAT4X4 mat);
        DirectX::XMFLOAT4X4 m_depthCameraPose;
        DirectX::XMMATRIX m_depthCameraPoseInvMatrix;
        std::thread* m_pDepthUpdateThread;
        static long long checkAndConvertUnsigned(UINT64 val);
        struct DepthCamRoi {
            float kRowLower = 0.2;
            float kRowUpper = 0.5;
            float kColLower = 0.3;
            float kColUpper = 0.7;
            UINT16 depthNearClip = 200; // Unit: mm
            UINT16 depthFarClip = 800;
        } depthCamRoi;
    };
}
namespace winrt::HL2UnityPlugin::factory_implementation
{
    struct HL2ResearchMode : HL2ResearchModeT<HL2ResearchMode, implementation::HL2ResearchMode>
    {
    };
}
