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

    public GameObject previewPlane = null;
    private Material mediaMaterial = null;
    private Texture2D mediaTexture = null;
    private byte[] frameData = null;

    public GameObject pointCloudRendererGo;
    public Color pointColor = Color.white;
    private PointCloudRenderer pointCloudRenderer;

    void Start()
    {
        mediaMaterial = previewPlane.GetComponent<MeshRenderer>().material;
        mediaTexture = new Texture2D(512, 512, TextureFormat.Alpha8, false);
        mediaMaterial.mainTexture = mediaTexture;
        pointCloudRenderer = pointCloudRendererGo.GetComponent<PointCloudRenderer>();

#if ENABLE_WINMD_SUPPORT
        IntPtr WorldOriginPtr = UnityEngine.XR.WSA.WorldManager.GetNativeISpatialCoordinateSystemPtr();
        var unityWorldOrigin = Marshal.GetObjectForIUnknown(WorldOriginPtr) as Windows.Perception.Spatial.SpatialCoordinateSystem;
        
        researchMode = new HL2ResearchMode();
        researchMode.InitializeDepthSensor();
        researchMode.SetReferenceCoordinateSystem(unityWorldOrigin);
        researchMode.SetPointCloudDepthOffset(0);
        Debug.Log("Successfuly initialize sensors");
        researchMode.StartDepthSensorLoop();
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
                if (frameData == null)
                {
                    frameData = frameTexture;
                }
                else
                {
                    System.Buffer.BlockCopy(frameTexture, 0, frameData, 0, frameData.Length);
                }

                mediaTexture.LoadRawTextureData(frameData);
                mediaTexture.Apply();
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
        startRealtimePreview = false;
#endif
    }

    private void OnApplicationQuit()
    {
        StopSensorsEvent();
    }
}