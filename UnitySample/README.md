# Overview
This Unity project shows 

- how to visualize the AHAT (short-throw) depth camera video stream in Unity.
- how to reconstruct and visualize point cloud from AHAT depth camera data in Unity.

# Compatibility
- Unity 2019.4
- Visual Studio 2019

# Build
1. Open this folder in Unity.
2. Go to Build Settings, switch target platform to UWP.
3. In the Project tab, open `Scenes/PointCloudSample.unity`.
4. Hopefully, there is no error in the console. Go to Build Settings, change Target Device to HoloLens, Architecture to ARM64. Build the Unity project in a new folder (e.g. App folder).
5. Open `App/HL2ResearchModeUnitySample/Package.appxmanifest` with a text editor. Add `xmlns:rescap="http://schemas.microsoft.com/appx/manifest/foundation/windows10/restrictedcapabilities"` before the `IgnorableNamespaces` in Package tag (line 2). Add `<rescap:Capability Name="perceptionSensorsExperimental" />` in the Capabilities tag between `<uap2:Capability ... >` and `<DeviceCapability ... >`. This the same as enabling Research Mode on HoloLens 1.
6. Save the changes. Open `App/HL2ResearchModeUnitySample.sln`. Change the build type to Release-ARM64-Device(or Remote Machine). Build - Deploy.
7. Done!

# Note
- The app may not function properly the first time you open the deployed app when there are pop-up windows asking for permissions. You can simply grant the permissions, close the app and reopen it. Then everything should be fine.
- You need to restart the device (hold the power button for several seconds) each time the device hiberates after you opened an app that uses research mode functions. So if your app suddenly cannot get any sensor data, try restarting your device. Please let me know if you know how to solve this issue.