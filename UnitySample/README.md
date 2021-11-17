# Overview
This Unity project shows 

- how to visualize the AHAT (short-throw) depth camera video stream and front spatial camera stream in Unity.
- how to reconstruct and visualize point cloud from AHAT depth camera data in Unity.
- how to send image to laptop using basic socket. (Python server script in `python` folder)
- how to obtain IMU data. (`Assets/Scenes/ImuViewSample.unity`)

# Compatibility
- Unity 2019.4*
- Visual Studio 2019

\* To use it in Unity 2020.1 - 2021.1,
- Open Unity project and install XRSDK (Project Settings-XR Plugin Management-install, then tick "Windows Mixed Reality")
- Select MixedRealityToolkit Gameobject in the Hierarchy. In the Inspector, change the mixed reality configuration profile to `New XRSDKConfigurationProfile` (or `DefaultXRSDKConfigurationProfile`).
- Point cloud sample not supported in Unity 2021.2 or later since OpenXR becomes the only supported pipeline with different way of obtaining the Unity world coordiante frame. Other functions shouldn't be influenced.

# Build
1. Open this folder in Unity.
2. Go to Build Settings, switch target platform to UWP.
3. In the Project tab, open `Scenes/PointCloudSample.unity`.
4. Hopefully, there is no error in the console. Go to Build Settings, change Target Device to HoloLens, Architecture to ARM64. Build the Unity project in a new folder (e.g. App folder).
5. Open `App/HL2ResearchModeUnitySample/Package.appxmanifest` with a text editor. Add `xmlns:rescap="http://schemas.microsoft.com/appx/manifest/foundation/windows10/restrictedcapabilities"` before the `IgnorableNamespaces` in Package tag (line 2). Add `<rescap:Capability Name="perceptionSensorsExperimental" />` in the Capabilities tag between `<uap2:Capability ... >` and `<DeviceCapability ... >`. This the same as enabling Research Mode on HoloLens 1.
6. If IMU is used, add `<DeviceCapability Name="backgroundSpatialPerception"/>` to DeviceCapability.
7. Save the changes. Open `App/HL2ResearchModeUnitySample.sln`. Change the build type to Release/Master-ARM64-Device(or Remote Machine). Build - Deploy.
8. Done!

# Note
- The app may not function properly the first time you open the deployed app when there are pop-up windows asking for permissions. You can simply grant the permissions, close the app and reopen it. Then everything should be fine.
- You need to restart the device (hold the power button for several seconds) each time the device hiberates after you opened an app that uses research mode functions. So if your app suddenly cannot get any sensor data, try restarting your device. Please let me know if you know how to solve this issue.