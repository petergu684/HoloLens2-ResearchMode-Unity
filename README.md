# HoloLens2-ResearchMode-Unity
Unity Plugin for using research mode functionality in HoloLens 2. Modified based on [HoloLens2ForCV](https://github.com/microsoft/HoloLens2ForCV).

![Depth Map Example](https://github.com/petergu684/HoloLens2-ResearchMode-Unity/blob/master/DepthMapExample.jpg)

Skeleton to wrap HoloLens 2 research mode api into Windows Runtime extension. 

To use it in Unity,
- Build this project (ARM64,Release) and copy the .dll and .winmd files in `HL2UnityPlugin\ARM64\Release\HL2UnityPlugin` into `Assets/Plugins/WSA/ARM64` folder of your Unity project.
- Change the architecture in your Unity build settings to be ARM64.
- After building the visual studio solution from Unity, go to `App/[Project name]/Package.appxmanifest` and add the restricted capability to the manifest file. (Same as what you would do to enable research mode on HoloLens 1, reference: http://akihiro-document.azurewebsites.net/post/hololens_researchmode2/)
- Save the changes and deploy the solution to your HoloLens 2.


## Note:
- The reconstructed point cloud still has the offset problem as is described [here](https://github.com/microsoft/HoloLens2ForCV/issues/12) for object beyond 1m.
- To visualize the depth image, you need a grayscale shader applied to your preview plane. Example: [grayscale shader](https://github.com/qian256/HoloLensARToolKit/blob/master/HoloLensARToolKit/Assets/Sample/Grayscale.shader).
- For point cloud, current implementation only returns the reconstructed point cloud as a float array (in the format of x,y,z,x,y,z,...). If you want to visualize it, I find [this project](https://github.com/MarekKowalski/LiveScan3D-Hololens) is a good example.
- This project is mainly to show how to use Reseach Mode in Unity. I only provided implementation on AHAT camera image visualization and point cloud reconstruction (based on depth map of AHAT camera). Feel free to modify the code according to your own need.
- If you need some sample script to get started, you can refer to [here](https://github.com/petergu684/HoloLens2-ResearchMode-Unity/issues/1#issuecomment-689762815).

