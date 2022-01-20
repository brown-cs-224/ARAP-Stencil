# Assignment 4: As-Rigid-As-Possible Surface Modeling (ARAP)


**Released:** 3/18/22

**Due:** 4/4/22 @ 11:59pm EST

In this assignment, you will implement a system for user-interactive deformation of 3D meshes. In your system, mesh vertices can be re-positioned by clicking and dragging. Your system will update the rest of the mesh in response to these user interactions such that it moves as-rigidly-as-possible (i.e. the deformation it exhibits is close to a rigid transformation). The end result is a deformation that looks physically-plausible, as if the mesh has an underlying rig / skeletal armature. To achieve this goal, you will need to formulate the deformation process as an optimization problem which alternates between estimating the best-fit rigid transformation for each mesh element and solving a sparse linear system to find new mesh vertex positions.


## Relevant Reading

- The lecture slides!
- [As-Rigid-As-Possible Surface Modeling](https://igl.ethz.ch/projects/ARAP/arap_web.pdf) on the mathematics behind the ARAP algorithm.


## Requirements

This assignment is out of **100 points**.

Your code must implement at least the following features:

* Initialize the shape **(X points)**
  * Determine one-ring neighbor of each vertices.
  * Calculate the weight **w** for each vertices.
* Iterate between 
    * Determining the rotation transformation **R** for the moved point **p'** from original point **p** **(Y points)**.
    * Optimize the position **p'** given **p** and **R** so that the energy term is minimized **(Z points)**
        * Precompute matrix **L** from **p** and **w**. 
        * Compute **b** from initial position **p**, rotation matrix **p**, and constraint **c**.
* Video (described below) **(T points)**
* README **(W points)**

Note that for simplicity, we only require the code to work on **manifold** meshes. Successfully implementing all of the requirements results in a total of **A/100 points**.
To score **100/100** (or more!), you’ll need to implement some extra features.

### Video

You must submit at least one video demonstrating your system in action. The video(s) must demonstrate all of the features you have implemented (including any extra features).

There are a few different ways you might go about making such videos:

* Screen capture an OpenGL rendering of your program, e.g. using the interactive viewer code that we provide below (see “Resources”).
* Export frame-by-frame meshes from your program and use your path tracer from Assignment 1 to render them.
* Use some other modeling/animation/rendering software to render exported meshes (e.g. Maya, Blender).

Particularly creative and/or nicely-rendered animations may receive extra credit.
Please use a standard format and codec for your video files (e.g. .mp4 with the H264 codec).
To turn a set of frame images into a video, you can use [FFMPEG](https://hamelot.io/visualization/using-ffmpeg-to-convert-a-set-of-images-into-a-video/).

### README

You must also submit a Markdown README file. This file should describe how to run your program (e.g. what command line arguments are needed?)

This file should also list all of the features your code implements.

Finally, it should describe what features are demonstrated by the video(s) you’ve submitted. You should embed the videos into the README file.

### Extra Features
Each of the following features that you implement will earn you extra points. The features are ordered roughly by difficulty of implementation.

* Multithreading
* Other form of parallelism
* Improved initial guess
* Improved Interactivity 
* Multi-resolution


**Any extra features you implement must be mentioned in your README and demonstrated in your video (you can submit multiple videos to show off different features, if that’s easier).**

### Resources

Feel free to use this stencil code to get started.

This starter project is quite barebones: it provides code to load .obj files into a simple mesh representation (list of vertices and list of faces), as well as code to a simple interactive 3D viewer for visualizing (and dynamically updating) meshes. You’ll need to implement everything else: the data structure you’ll use to support efficient update.

The starter project also contains some .obj files you can use to test your code. There are also tons of 3D model files available on the internet. 

### Submission Instructions

Submit your branch of the Github classroom repository to the ARAP assignment.

## About the code

You'll want to look at src/arap.cpp to get started, as that's the only file you need to change (although you'll probably make several of your own new files, too).
You also might want to look at src/view.cpp, if you're interested in adding new interactivity/controls to the program.

Speaking of controls: the controls offered by the starter code are:
 * Move Camera: WASD
 * Look around: Click and hold mouse and drag
 * Toggle orbit mode: C (changes the camera from a first-person view to an orbiting camera a la what the Maya editor does)
 * Toggle between displaying the surface mesh and a wireframe of the full tet mesh: T


## Example Video
