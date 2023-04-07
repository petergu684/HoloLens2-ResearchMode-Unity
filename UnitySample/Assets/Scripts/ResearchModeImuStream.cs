using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System;
using System.Runtime.InteropServices;

#if ENABLE_WINMD_SUPPORT
using HL2UnityPlugin;
#endif

public class ResearchModeImuStream : MonoBehaviour
{
#if ENABLE_WINMD_SUPPORT
    HL2ResearchMode researchMode;
#endif
    private float[] accelSampleData = null;
    private Vector3 accelVector;

    private float[] gyroSampleData = null;
    private Vector3 gyroEulerAngle;

    private float[] magSampleData = null;

    public Text AccelText = null;
    public Text GyroText = null;
    public Text MagText = null;

    public ImuVisualize RefImuVisualize = null;

    void Start()
    {
#if ENABLE_WINMD_SUPPORT
        researchMode = new HL2ResearchMode();
        researchMode.InitializeAccelSensor();
        researchMode.InitializeGyroSensor();
        researchMode.InitializeMagSensor();

        researchMode.StartAccelSensorLoop();
        researchMode.StartGyroSensorLoop();
        researchMode.StartMagSensorLoop();
#endif
    }
    void LateUpdate()
    {
#if ENABLE_WINMD_SUPPORT
        // update Accel Sample
        if (researchMode.AccelSampleUpdated())
        {
            accelSampleData = researchMode.GetAccelSample();
            if (accelSampleData.Length == 3)
            {
                AccelText.text = $"Accel : {accelSampleData[0]:F3}, {accelSampleData[1]:F3}, {accelSampleData[2]:F3}";
            }
        }

        // update Gyro Sample
        if (researchMode.GyroSampleUpdated())
        {
            gyroSampleData = researchMode.GetGyroSample();
            if (gyroSampleData.Length == 3)
            {
                GyroText.text = $"Gyro  : {gyroSampleData[0]:F3}, {gyroSampleData[1]:F3}, {gyroSampleData[2]:F3}";
            }
        }

        // update Mag Sample
        if (researchMode.MagSampleUpdated())
        {
            magSampleData = researchMode.GetMagSample();
            if (magSampleData.Length == 3)
            {
                MagText.text = $"Mag   : {magSampleData[0]:F3}, {magSampleData[1]:F3}, {magSampleData[2]:F3}";
            }
        }
#endif
        // Convert to Vector3
        accelVector = CreateAccelVector(accelSampleData);
        gyroEulerAngle = CreateGyroEulerAngle(gyroSampleData);

        // Visualize corrected values
        RefImuVisualize.AccelVector = accelVector * 0.1f;
        RefImuVisualize.GyroEulorAngle = gyroEulerAngle * 30.0f;
    }

    private Vector3 CreateAccelVector(float[] accelSample)
    {
        Vector3 vector = Vector3.zero;
        if ((accelSample?.Length ?? 0) == 3)
        {
            // Positive directions
            //  accelSample[0] : Down direction
            //  accelSample[1] : Back direction
            //  accelSample[2] : Right direction
            vector = new Vector3(
                accelSample[2],
                -1.0f * accelSample[0],
                -1.0f * accelSample[1]
                );
        }
        return vector;
    }

    private Vector3 CreateGyroEulerAngle(float[] gyroSample)
    {
        Vector3 vector = Vector3.zero;
        if ((gyroSample?.Length ?? 0) == 3)
        {
            // Axis of rotation
            //  gyroSample[0] : Unity Y axis(Plus)
            //  gyroSample[1] : Unity Z axis(Plus)
            //  gyroSample[2] : Unity X axis(Plus)
            vector = new Vector3(
                gyroSample[2],
                gyroSample[0],
                gyroSample[1]
                );
        }
        return vector;
    }

    public void StopSensorsEvent()
    {
#if ENABLE_WINMD_SUPPORT
        researchMode.StopAllSensorDevice();
#endif
    }

    private void OnApplicationFocus(bool focus)
    {
        if (!focus) StopSensorsEvent();
    }
}