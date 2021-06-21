using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.InteropServices;

#if ENABLE_WINMD_SUPPORT
using HL2UnityPlugin;
#endif

public class ResearchModeVideoStream : MonoBehaviour
{
#if ENABLE_WINMD_SUPPORT
    HL2ResearchMode researchMode;
#endif

    TCPClient tcpClient;

    public GameObject depthPreviewPlane = null;
    private Material depthMediaMaterial = null;
    private Texture2D depthMediaTexture = null;
    private byte[] depthFrameData = null;

    public GameObject shortAbImagePreviewPlane = null;
    private Material shortAbImageMediaMaterial = null;
    private Texture2D shortAbImageMediaTexture = null;
    private byte[] shortAbImageFrameData = null;

    public GameObject longDepthPreviewPlane = null;
    private Material longDepthMediaMaterial = null;
    private Texture2D longDepthMediaTexture = null;
    private byte[] longDepthFrameData = null;

    public GameObject LFPreviewPlane = null;
    private Material LFMediaMaterial = null;
    private Texture2D LFMediaTexture = null;
    private byte[] LFFrameData = null;

    public GameObject RFPreviewPlane = null;
    private Material RFMediaMaterial = null;
    private Texture2D RFMediaTexture = null;
    private byte[] RFFrameData = null;


    public GameObject pointCloudRendererGo;
    public Color pointColor = Color.white;
    private PointCloudRenderer pointCloudRenderer;

    void Start()
    {
        depthMediaMaterial = depthPreviewPlane.GetComponent<MeshRenderer>().material;
        depthMediaTexture = new Texture2D(512, 512, TextureFormat.Alpha8, false);
        depthMediaMaterial.mainTexture = depthMediaTexture;

        shortAbImageMediaMaterial = shortAbImagePreviewPlane.GetComponent<MeshRenderer>().material;
        shortAbImageMediaTexture = new Texture2D(512, 512, TextureFormat.Alpha8, false);
        shortAbImageMediaMaterial.mainTexture = shortAbImageMediaTexture;

        longDepthMediaMaterial = longDepthPreviewPlane.GetComponent<MeshRenderer>().material;
        longDepthMediaTexture = new Texture2D(320, 288, TextureFormat.Alpha8, false);
        longDepthMediaMaterial.mainTexture = longDepthMediaTexture;

        LFMediaMaterial = LFPreviewPlane.GetComponent<MeshRenderer>().material;
        LFMediaTexture = new Texture2D(640, 480, TextureFormat.Alpha8, false);
        LFMediaMaterial.mainTexture = LFMediaTexture;

        RFMediaMaterial = RFPreviewPlane.GetComponent<MeshRenderer>().material;
        RFMediaTexture = new Texture2D(640, 480, TextureFormat.Alpha8, false);
        RFMediaMaterial.mainTexture = RFMediaTexture;

        pointCloudRenderer = pointCloudRendererGo.GetComponent<PointCloudRenderer>();

        tcpClient = GetComponent<TCPClient>();

#if ENABLE_WINMD_SUPPORT
        researchMode = new HL2ResearchMode();
        researchMode.InitializeDepthSensor();
        researchMode.InitializeLongDepthSensor();
        researchMode.InitializeSpatialCamerasFront();

        researchMode.SetPointCloudDepthOffset(0);

        // Depth sensor should be initialized in only one mode
        researchMode.StartDepthSensorLoop();
        //researchMode.StartLongDepthSensorLoop(); 
        
        researchMode.StartSpatialCamerasFrontLoop();
#endif
    }

    bool startRealtimePreview = true;
    void LateUpdate()
    {
#if ENABLE_WINMD_SUPPORT
        // update depth map texture
        if (startRealtimePreview && researchMode.DepthMapTextureUpdated())
        {
            byte[] frameTexture = researchMode.GetDepthMapTextureBuffer();
            if (frameTexture.Length > 0)
            {
                if (depthFrameData == null)
                {
                    depthFrameData = frameTexture;
                }
                else
                {
                    System.Buffer.BlockCopy(frameTexture, 0, depthFrameData, 0, depthFrameData.Length);
                }

                depthMediaTexture.LoadRawTextureData(depthFrameData);
                depthMediaTexture.Apply();
            }
        }
        // update short-throw AbImage texture
        if (startRealtimePreview && researchMode.ShortAbImageTextureUpdated())
        {
            byte[] frameTexture = researchMode.GetShortAbImageTextureBuffer();
            if (frameTexture.Length > 0)
            {
                if (shortAbImageFrameData == null)
                {
                    shortAbImageFrameData = frameTexture;
                }
                else
                {
                    System.Buffer.BlockCopy(frameTexture, 0, shortAbImageFrameData, 0, shortAbImageFrameData.Length);
                }

                shortAbImageMediaTexture.LoadRawTextureData(shortAbImageFrameData);
                shortAbImageMediaTexture.Apply();
            }
        }
        // update long depth map texture
        //if (researchMode.LongDepthMapTextureUpdated())
        //{
        //    byte[] frameTexture = researchMode.GetLongDepthMapTextureBuffer();
        //    if (frameTexture.Length > 0)
        //    {
        //        if (longDepthFrameData == null)
        //        {
        //            longDepthFrameData = frameTexture;
        //        }
        //        else
        //        {
        //            System.Buffer.BlockCopy(frameTexture, 0, longDepthFrameData, 0, longDepthFrameData.Length);
        //        }

        //        longDepthMediaTexture.LoadRawTextureData(longDepthFrameData);
        //        longDepthMediaTexture.Apply();
        //    }
        //}

        // update LF camera texture
        if (startRealtimePreview && researchMode.LFImageUpdated())
        {
            byte[] frameTexture = researchMode.GetLFCameraBuffer();
            if (frameTexture.Length > 0)
            {
                if (LFFrameData == null)
                {
                    LFFrameData = frameTexture;
                }
                else
                {
                    System.Buffer.BlockCopy(frameTexture, 0, LFFrameData, 0, LFFrameData.Length);
                }

                LFMediaTexture.LoadRawTextureData(LFFrameData);
                LFMediaTexture.Apply();
            }
        }
        // update RF camera texture
        if (startRealtimePreview && researchMode.RFImageUpdated())
        {
            byte[] frameTexture = researchMode.GetRFCameraBuffer();
            if (frameTexture.Length > 0)
            {
                if (RFFrameData == null)
                {
                    RFFrameData = frameTexture;
                }
                else
                {
                    System.Buffer.BlockCopy(frameTexture, 0, RFFrameData, 0, RFFrameData.Length);
                }

                RFMediaTexture.LoadRawTextureData(RFFrameData);
                RFMediaTexture.Apply();
            }
        }

        // Update point cloud
        if (renderPointCloud)
        {
            float[] pointCloud = researchMode.GetPointCloudBuffer();
            if (pointCloud.Length > 0)
            {
                int pointCloudLength = pointCloud.Length / 3;
                Vector3[] pointCloudVector3 = new Vector3[pointCloudLength];
                for (int i = 0; i < pointCloudLength; i++)
                {
                    pointCloudVector3[i] = new Vector3(pointCloud[3 * i], pointCloud[3 * i + 1], pointCloud[3 * i + 2]);
                }
                //Debug.LogError("Point Cloud Size: " + pointCloudVector3.Length.ToString());
                pointCloudRenderer.Render(pointCloudVector3, pointColor);

            }
        }
#endif
    }


    #region Button Event Functions
    public void TogglePreviewEvent()
    {
        startRealtimePreview = !startRealtimePreview;
    }

    bool renderPointCloud = true;
    public void TogglePointCloudEvent()
    {
        renderPointCloud = !renderPointCloud;
        if (renderPointCloud)
        {
            pointCloudRendererGo.SetActive(true);
        }
        else
        {
            pointCloudRendererGo.SetActive(false);
        }
    }

    public void StopSensorsEvent()
    {
#if ENABLE_WINMD_SUPPORT
        researchMode.StopAllSensorDevice();
#endif
        startRealtimePreview = false;
    }

    public void SaveAHATSensorDataEvent()
    {
#if ENABLE_WINMD_SUPPORT
        var depthMap = researchMode.GetDepthMapBuffer();
        var AbImage = researchMode.GetShortAbImageBuffer();
#if WINDOWS_UWP
        tcpClient.SendUINT16Async(depthMap, AbImage);
#endif
#endif
    }
    #endregion
    private void OnApplicationFocus(bool focus)
    {
        if (!focus) StopSensorsEvent();
    }
}