# HoloLens2-ResearchMode-Unity
Unity Plugin for using research mode functionality in HoloLens 2. Modified based on [HoloLens2ForCV](https://github.com/microsoft/HoloLens2ForCV).

![Depth Map Example](https://github.com/petergu684/HoloLens2-ResearchMode-Unity/blob/master/DepthMapExample.jpg)

Skeleton to wrap HoloLens 2 research mode api into Windows Runtime extension. 
To use it in Unity, just build the project (ARM64/Release) and move the .dll and .winmd file in 'HL2UnityPlugin\ARM64\Release\HL2UnityPlugin' into Plugins folder in Unity Assets.

The reconstructed point cloud still has the offset problem as is described [here](https://github.com/microsoft/HoloLens2ForCV/issues/12) for object beyond 1m.

Feel free to modify the code according to your own need. So far I have only implemented functions related to AHAT camera.

Still under development...
