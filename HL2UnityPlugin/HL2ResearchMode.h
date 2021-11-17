#pragma once
#include "HL2ResearchMode.g.h"
#include "ResearchModeApi.h"
#include "TimeConverter.h"
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
        HL2ResearchMode();

        UINT16 GetCenterDepth();
        int GetDepthBufferSize();
        int GetLongDepthBufferSize();
        hstring PrintDepthResolution();
        hstring PrintDepthExtrinsics();
		hstring PrintLFResolution();
		hstring PrintLFExtrinsics();
		hstring PrintRFResolution();
		hstring PrintRFExtrinsics();

        void InitializeDepthSensor();
        void InitializeLongDepthSensor();
        void InitializeSpatialCamerasFront();
        void InitializeAccelSensor();
        void InitializeGyroSensor();
        void InitializeMagSensor();

        void StartDepthSensorLoop(bool reconstructPointCloud = true);
        void StartLongDepthSensorLoop(bool reconstructPointCloud = true);
        void StartSpatialCamerasFrontLoop();
        void StartAccelSensorLoop();
        void StartGyroSensorLoop();
        void StartMagSensorLoop();

        void StopAllSensorDevice();

        bool DepthMapTextureUpdated();
        bool ShortAbImageTextureUpdated();
        bool LongAbImageTextureUpdated();
        bool PointCloudUpdated();
        bool LongThrowPointCloudUpdated();
        bool LongDepthMapTextureUpdated();
		bool LFImageUpdated();
		bool RFImageUpdated();
        bool AccelSampleUpdated();
        bool GyroSampleUpdated();
        bool MagSampleUpdated();

        void SetReferenceCoordinateSystem(Windows::Perception::Spatial::SpatialCoordinateSystem refCoord);
        void SetPointCloudRoiInSpace(float centerX, float centerY, float centerZ, float boundX, float boundY, float boundZ);
        void SetPointCloudDepthOffset(uint16_t offset);

        com_array<uint16_t> GetDepthMapBuffer();
        com_array<uint8_t> GetDepthMapTextureBuffer();
        com_array<uint16_t> GetShortAbImageBuffer();
        com_array<uint8_t> GetShortAbImageTextureBuffer();
        com_array<uint16_t> GetLongAbImageBuffer();
        com_array<uint8_t> GetLongAbImageTextureBuffer();
        com_array<uint16_t> GetLongDepthMapBuffer();
        com_array<uint8_t> GetLongDepthMapTextureBuffer();
        com_array<uint8_t> GetLFCameraBuffer(int64_t& ts);
        com_array<uint8_t> GetRFCameraBuffer(int64_t& ts);
        com_array<uint8_t> GetLRFCameraBuffer(int64_t& ts_left, int64_t& ts_right);

        com_array<float> GetAccelSample();
        com_array<float> GetGyroSample();
        com_array<float> GetMagSample();

        com_array<float> GetPointCloudBuffer();
        com_array<float> GetLongThrowPointCloudBuffer();
        com_array<float> GetCenterPoint();
        std::mutex mu;

    private:
        float* m_pointCloud = nullptr;
        int m_pointcloudLength = 0;
        float* m_longThrowPointCloud = nullptr;
        int m_longThrowPointcloudLength = 0;
        UINT16* m_depthMap = nullptr;
        UINT8* m_depthMapTexture = nullptr;
        UINT16* m_shortAbImage = nullptr;
        UINT8* m_shortAbImageTexture = nullptr;
        UINT16* m_longAbImage = nullptr;
        UINT8* m_longAbImageTexture = nullptr;
        UINT16* m_longDepthMap = nullptr;
        UINT8* m_longDepthMapTexture = nullptr;

		UINT8* m_LFImage = nullptr;
		UINT8* m_RFImage = nullptr;
        float* m_accelSample = nullptr;
        float* m_gyroSample = nullptr;
        float* m_magSample = nullptr;

        IResearchModeSensor* m_depthSensor = nullptr;
        IResearchModeCameraSensor* m_pDepthCameraSensor = nullptr;
        IResearchModeSensor* m_longDepthSensor = nullptr;
        IResearchModeCameraSensor* m_pLongDepthCameraSensor = nullptr;
        IResearchModeSensor* m_LFSensor = nullptr;
        IResearchModeCameraSensor* m_LFCameraSensor = nullptr;
        IResearchModeSensor* m_RFSensor = nullptr;
        IResearchModeCameraSensor* m_RFCameraSensor = nullptr;
        IResearchModeSensor* m_accelSensor = nullptr;
        IResearchModeSensor* m_gyroSensor = nullptr;
        IResearchModeSensor* m_magSensor = nullptr;
        ResearchModeSensorResolution m_depthResolution;
        ResearchModeSensorResolution m_longDepthResolution;
        ResearchModeSensorResolution m_LFResolution;
        ResearchModeSensorResolution m_RFResolution;
        IResearchModeSensorDevice* m_pSensorDevice = nullptr;
        std::vector<ResearchModeSensorDescriptor> m_sensorDescriptors;
        IResearchModeSensorDeviceConsent* m_pSensorDeviceConsent = nullptr;
        Windows::Perception::Spatial::SpatialLocator m_locator = 0;
        Windows::Perception::Spatial::SpatialCoordinateSystem m_refFrame = nullptr;
        std::atomic_int m_depthBufferSize = 0;
        std::atomic_int m_longDepthBufferSize = 0;
        std::atomic_int m_LFbufferSize = 0;
        std::atomic_int m_RFbufferSize = 0;
        std::atomic_uint16_t m_centerDepth = 0;
        float m_centerPoint[3]{ 0,0,0 };

        std::atomic_bool m_depthSensorLoopStarted = false;
        std::atomic_bool m_longDepthSensorLoopStarted = false;
        std::atomic_bool m_spatialCamerasFrontLoopStarted = false;
        std::atomic_bool m_accelSensorLoopStarted = false;
        std::atomic_bool m_gyroSensorLoopStarted = false;
        std::atomic_bool m_magSensorLoopStarted = false;

        std::atomic_bool m_depthMapTextureUpdated = false;
        std::atomic_bool m_shortAbImageTextureUpdated = false;
        std::atomic_bool m_longAbImageTextureUpdated = false;
        std::atomic_bool m_longDepthMapTextureUpdated = false;
        std::atomic_bool m_pointCloudUpdated = false;
        std::atomic_bool m_longThrowPointCloudUpdated = false;
        std::atomic_bool m_useRoiFilter = false;
		std::atomic_bool m_LFImageUpdated = false;
		std::atomic_bool m_RFImageUpdated = false;
        std::atomic_bool m_accelSampleUpdated = false;
        std::atomic_bool m_gyroSampleUpdated = false;
        std::atomic_bool m_magSampleUpdated = false;

        std::atomic_bool m_reconstructShortThrowPointCloud = false;
        std::atomic_bool m_reconstructLongThrowPointCloud = false;

        float m_roiBound[3]{ 0,0,0 };
        float m_roiCenter[3]{ 0,0,0 };
        static void DepthSensorLoop(HL2ResearchMode* pHL2ResearchMode);
        static void LongDepthSensorLoop(HL2ResearchMode* pHL2ResearchMode);
        static void SpatialCamerasFrontLoop(HL2ResearchMode* pHL2ResearchMode);
        static void AccelSensorLoop(HL2ResearchMode* pHL2ResearchMode);
        static void GyroSensorLoop(HL2ResearchMode* pHL2ResearchMode);
        static void MagSensorLoop(HL2ResearchMode* pHL2ResearchMode);
        static void CamAccessOnComplete(ResearchModeSensorConsent consent);
        static void ImuAccessOnComplete(ResearchModeSensorConsent consent);
        std::string MatrixToString(DirectX::XMFLOAT4X4 mat);
        DirectX::XMFLOAT4X4 m_depthCameraPose;
        DirectX::XMMATRIX m_depthCameraPoseInvMatrix;
        DirectX::XMFLOAT4X4 m_longDepthCameraPose;
        DirectX::XMMATRIX m_longDepthCameraPoseInvMatrix;
        DirectX::XMFLOAT4X4 m_LFCameraPose;
        DirectX::XMMATRIX m_LFCameraPoseInvMatrix;
        DirectX::XMFLOAT4X4 m_RFCameraPose;
        DirectX::XMMATRIX m_RFCameraPoseInvMatrix;
        std::thread* m_pDepthUpdateThread;
        std::thread* m_pLongDepthUpdateThread;
        std::thread* m_pSpatialCamerasFrontUpdateThread;
        std::thread* m_pAccelUpdateThread;
        std::thread* m_pGyroUpdateThread;
        std::thread* m_pMagUpdateThread;
        static long long checkAndConvertUnsigned(UINT64 val);
        static DirectX::XMMATRIX HL2ResearchMode::SpatialLocationToDxMatrix(Windows::Perception::Spatial::SpatialLocation location);
        struct DepthCamRoi {
            float kRowLower = 0.2;
            float kRowUpper = 0.55;
            float kColLower = 0.3;
            float kColUpper = 0.7;
            UINT16 depthNearClip = 200; // Unit: mm
            UINT16 depthFarClip = 800;
        } depthCamRoi;
        UINT16 m_depthOffset = 0;

        TimeConverter m_converter;

        struct Frame {
            UINT64 timestamp; // QPC 
            int64_t timestamp_ft; // FileTime
            UINT8* image = nullptr;
        };

        struct SpatialCameraFrame {
            Frame LFFrame;
            Frame RFFrame;
        } m_lastSpatialFrame;
    };
}
namespace winrt::HL2UnityPlugin::factory_implementation
{
    struct HL2ResearchMode : HL2ResearchModeT<HL2ResearchMode, implementation::HL2ResearchMode>
    {
    };
}
