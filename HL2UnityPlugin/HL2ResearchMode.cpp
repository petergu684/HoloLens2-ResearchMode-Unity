#include "pch.h"
#include "HL2ResearchMode.h"
#include "HL2ResearchMode.g.cpp"

extern "C"
HMODULE LoadLibraryA(
    LPCSTR lpLibFileName
);

static ResearchModeSensorConsent camAccessCheck;
static HANDLE camConsentGiven;

using namespace DirectX;

namespace winrt::HL2UnityPlugin::implementation
{
    void HL2ResearchMode::InitializeDepthSensor() 
    {
        // Load Research Mode library
        camConsentGiven = CreateEvent(nullptr, true, false, nullptr);
        HMODULE hrResearchMode = LoadLibraryA("ResearchModeAPI");
        HRESULT hr = S_OK;

        if (hrResearchMode)
        {
            typedef HRESULT(__cdecl* PFN_CREATEPROVIDER) (IResearchModeSensorDevice** ppSensorDevice);
            PFN_CREATEPROVIDER pfnCreate = reinterpret_cast<PFN_CREATEPROVIDER>(GetProcAddress(hrResearchMode, "CreateResearchModeSensorDevice"));
            if (pfnCreate)
            {
                winrt::check_hresult(pfnCreate(&m_pSensorDevice));
            }
            else
            {
                winrt::check_hresult(E_INVALIDARG);
            }
        }

        //IResearchModeSensorDevicePerception* pSensorDevicePerception;
        
        size_t sensorCount = 0;

        //hr = CreateResearchModeSensorDevice(&pSensorDevice);
        winrt::check_hresult(m_pSensorDevice->QueryInterface(IID_PPV_ARGS(&m_pSensorDeviceConsent)));
        winrt::check_hresult(m_pSensorDeviceConsent->RequestCamAccessAsync(HL2ResearchMode::CamAccessOnComplete));
        
        m_pSensorDevice->DisableEyeSelection();

        winrt::check_hresult(m_pSensorDevice->GetSensorCount(&sensorCount));
        m_sensorDescriptors.resize(sensorCount);
        winrt::check_hresult(m_pSensorDevice->GetSensorDescriptors(m_sensorDescriptors.data(), m_sensorDescriptors.size(), &sensorCount));

        for (auto sensorDescriptor : m_sensorDescriptors)
        {
            if (sensorDescriptor.sensorType == DEPTH_AHAT)
            {
                winrt::check_hresult(m_pSensorDevice->GetSensor(sensorDescriptor.sensorType, &m_depthSensor));
                winrt::check_hresult(m_depthSensor->QueryInterface(IID_PPV_ARGS(&m_pDepthCameraSensor)));
                winrt::check_hresult(m_pDepthCameraSensor->GetCameraExtrinsicsMatrix(&m_depthCameraPose));
                break;
            }
        }
    }

    void HL2ResearchMode::StartDepthSensorLoop() 
    {
        //std::thread th1([this] {this->DepthSensorLoopTest(); });
        m_pDepthUpdateThread = new std::thread(HL2ResearchMode::DepthSensorLoop, this);
    }

    void HL2ResearchMode::DepthSensorLoop(HL2ResearchMode* pHL2ResearchMode)
    {
        if (!pHL2ResearchMode->m_depthSensorLoopStarted)
        {
            pHL2ResearchMode->m_depthSensorLoopStarted = true;
        }
        else {
            return;
        }

        pHL2ResearchMode->m_depthSensor->OpenStream();
        
        try 
        {
            while (pHL2ResearchMode->m_depthSensorLoopStarted)
            {
                IResearchModeSensorFrame* pDepthSensorFrame = nullptr;
                ResearchModeSensorResolution resolution;
                pHL2ResearchMode->m_depthSensor->GetNextBuffer(&pDepthSensorFrame);
                // process sensor frame
                pDepthSensorFrame->GetResolution(&resolution);
                pHL2ResearchMode->m_resolution = resolution;

                IResearchModeSensorDepthFrame* pDepthFrame = nullptr;
                winrt::check_hresult(pDepthSensorFrame->QueryInterface(IID_PPV_ARGS(&pDepthFrame)));

                size_t outBufferCount = 0;
                const UINT16* pDepth = nullptr;
                pDepthFrame->GetBuffer(&pDepth, &outBufferCount);
                pHL2ResearchMode->m_bufferSize = outBufferCount;

                //uint8_t* pDepthTexture = new uint8_t[pHL2ResearchMode->m_bufferSize];
                auto pDepthTexture = std::make_unique<uint8_t[]>(pHL2ResearchMode->m_bufferSize);

                for (UINT i = 0; i < resolution.Height; i++)
                {
                    for (UINT j = 0; j < resolution.Width; j++)
                    {
                        auto idx = resolution.Width * i + j;
                        UINT16 depth = pDepth[idx];
                        depth = (depth > 1000) ? 0 : depth;

                        // reconstruct point cloud
                        float xy[2] = { 0, 0 };
                        float uv[2] = {j, i};
                        pHL2ResearchMode->m_pDepthCameraSensor->MapImagePointToCameraUnitPlane(uv, xy);
                        auto pointOnUnitPlane = XMFLOAT3(xy[0], xy[1], 1);
                        auto tempPoint = (float)depth / 1000 * XMVector3Normalize(XMLoadFloat3(&pointOnUnitPlane));
                        
                        // save texture for visualization
                        if (depth == 0) { pDepthTexture.get()[idx] = 0; }
                        else { pDepthTexture.get()[idx] = (uint8_t)((float)depth / 1000 * 255); }

                        // save the depth of center pixel
                        if (i == (UINT)(0.5 * resolution.Height) && j == (UINT)(0.5 * resolution.Width))
                        {
                            pHL2ResearchMode->m_centerDepth = depth;
                            //pHL2ResearchMode->m_pointCloud
                        }
                    }
                }

                if (!pHL2ResearchMode->m_depthMap)
                {
                    OutputDebugString(L"Create Space for depth map...\n");
                    pHL2ResearchMode->m_depthMap = new UINT16[outBufferCount];
                }
                {
                    std::lock_guard<std::mutex> l(pHL2ResearchMode->mu);
                    memcpy(pHL2ResearchMode->m_depthMap, pDepth, outBufferCount * sizeof(UINT16));
                }

                if (!pHL2ResearchMode->m_depthMapTexture)
                {
                    OutputDebugString(L"Create Space for depth map texture...\n");
                    pHL2ResearchMode->m_depthMapTexture = new UINT8[outBufferCount];
                }
                {
                    std::lock_guard<std::mutex> l(pHL2ResearchMode->mu);
                    memcpy(pHL2ResearchMode->m_depthMapTexture, pDepthTexture.get(), outBufferCount * sizeof(UINT8));
                }
                pHL2ResearchMode->m_depthMapTextureUpdated = true;

                pDepthTexture.reset();

                // release space
                if (pDepthFrame) {
                    pDepthFrame->Release();
                }
                if (pDepthSensorFrame)
                {
                    pDepthSensorFrame->Release();
                }
                
            }
        }
        catch (...)  {}
        pHL2ResearchMode->m_depthSensor->CloseStream();
    }

    void HL2ResearchMode::CamAccessOnComplete(ResearchModeSensorConsent consent)
    {
        camAccessCheck = consent;
        SetEvent(camConsentGiven);
    }

    inline UINT16 HL2ResearchMode::GetCenterDepth() {return m_centerDepth;}

    inline int HL2ResearchMode::GetBufferSize() { return m_bufferSize; }

    inline bool HL2ResearchMode::DepthMapTextureUpdated() { return m_depthMapTextureUpdated; }

    hstring HL2ResearchMode::PrintResolution()
    {
        std::string res_c_ctr = std::to_string(m_resolution.Height) + "x" + std::to_string(m_resolution.Width) + "x" + std::to_string(m_resolution.BytesPerPixel);
        return winrt::to_hstring(res_c_ctr);// m_resolution.Width
    }

    // start once, for debugging purpose
    void HL2ResearchMode::StartDepthSensorOnce() 
    {
        m_depthSensor->OpenStream();
        
        try
        {
            IResearchModeSensorFrame* pDepthSensorFrame = nullptr;
            ResearchModeSensorResolution resolution;
            m_depthSensor->GetNextBuffer(&pDepthSensorFrame);
            // process sensor frame
            pDepthSensorFrame->GetResolution(&resolution);
            m_resolution = resolution;

            IResearchModeSensorDepthFrame* pDepthFrame = nullptr;
            winrt::check_hresult(pDepthSensorFrame->QueryInterface(IID_PPV_ARGS(&pDepthFrame)));

            size_t outBufferCount = 0;
            const UINT16* pDepth = nullptr;
            pDepthFrame->GetBuffer(&pDepth, &outBufferCount);
            m_bufferSize = outBufferCount;
            
            uint8_t* pDepthTexture = new uint8_t[m_bufferSize];

            for (UINT i = 0; i < resolution.Height; i++)
            {
                for (UINT j = 0; j < resolution.Width; j++)
                {
                    auto idx = resolution.Width * i + j;
                    UINT16 depth = pDepth[idx];

                    depth = (depth > 1000) ? 0 : depth;
                    if (depth == 0) 
                    {
                        pDepthTexture[idx] = 0;
                    }
                    else
                    {
                        pDepthTexture[idx] = (uint8_t)((float)depth / 1000 * 255);
                    }
                    
                    if (i == (UINT)(0.5 * resolution.Height) && j == (UINT)(0.5 * resolution.Width))
                    {
                        m_centerDepth = depth;
                    }
                }
            }

            if (!m_depthMap)
            {
                m_depthMap = new UINT16[outBufferCount];
            }
            {
                std::lock_guard<std::mutex> l(mu);
                memcpy(m_depthMap, pDepth, outBufferCount * sizeof(UINT16));
            }

            if (!m_depthMapTexture)
            {
                m_depthMapTexture = new UINT8[outBufferCount];
            }
            {
                std::lock_guard<std::mutex> l(mu);
                memcpy(m_depthMapTexture, pDepthTexture, outBufferCount * sizeof(UINT8));
            }

            delete[] pDepthTexture;
            // release space
            if (pDepthFrame) 
            {
                pDepthFrame->Release();
            }
            if (pDepthSensorFrame)
            {
                pDepthSensorFrame->Release();
            }
            
        }
        catch (...)
        {
        }
        m_depthSensor->CloseStream();
    }

    void HL2ResearchMode::PauseSensorsAndFreeSpace()
    {
        m_depthSensorLoopStarted = false;
        //m_pDepthUpdateThread->join();
        if (m_depthMap) 
        {
            delete[] m_depthMap;
            m_depthMap = nullptr;
        }
        if (m_depthMapTexture) 
        {
            delete[] m_depthMapTexture;
            m_depthMapTexture = nullptr;
        }
    }

    void HL2ResearchMode::StopAllSensorDevice()
    {
        m_depthSensor->CloseStream();
        m_depthSensor->Release();
        m_pSensorDevice->Release();
        m_pSensorDeviceConsent->Release();
    }

    com_array<uint16_t> HL2ResearchMode::GetDepthMapBuffer()
    {
        std::lock_guard<std::mutex> l(mu);
        auto tempBuffer = com_array<UINT16>(m_depthMap, m_depthMap + m_bufferSize);
        
        return tempBuffer;
    }

    com_array<uint8_t> HL2ResearchMode::GetDepthMapTextureBuffer()
    {
        std::lock_guard<std::mutex> l(mu);
        com_array<UINT8> tempBuffer = com_array<UINT8>(std::move_iterator(m_depthMapTexture), std::move_iterator(m_depthMapTexture + m_bufferSize));

        m_depthMapTextureUpdated = false;
        return tempBuffer;
    }

    
}
