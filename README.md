# Finite Element Simulation

# Features
## Extract the surface mesh from your tetrahedral mesh
  We extract surface meshes by identifying faces of our tetrahedral mesh which are not shared by another neighbor. The logic for this can be found in the `std::vector<Vector3i> getFaces(std::vector<Vector4i> tets)` function in `simulation.cpp`
 
## Compute and apply force due to gravity
  We use a force of Vector3d(0,-.1,0) as the downward gravitational pull. This force is multipled by the mass of each particle in the tet, which is computed based on the volume of each tet it is a part of and the density of the shape. To go into more detail, the mass of each tet is its `volume*density`. We accumlate `volume*density/4` to each particle in the tet. We apply the forces by computing the next velocity and position using RK4, which we will discuss later.
## Stress Forces
In order to compute stress due to strain and strain rate, we use to following set of equations (taken from lecture slides)

### Strain Rate

#### Stress Due to Strain Rate
![](https://github.com/brown-cs-224/simulation-cranberrymuffin/blob/master/images/stress_due_to_strain_rate.png)
#### Strain Rate
![](https://github.com/brown-cs-224/simulation-cranberrymuffin/blob/master/images/strain_rate.png)

### Strain

#### Stress Due to Strain
![](https://github.com/brown-cs-224/simulation-cranberrymuffin/blob/master/images/stress_due_to_strain.png)

#### Strain
![](https://github.com/brown-cs-224/simulation-cranberrymuffin/blob/master/images/strain.png)

### Deriving Common Terms
![](https://github.com/brown-cs-224/simulation-cranberrymuffin/blob/master/images/dx_du.png)
 
## Resolve collisions
  We hard code the location of our sphere obstacle and ground when computing collision forces. Collision forces are computer using the ‘penalty force’ method described in Section 3.3 of [O’Brien and Hodgins](http://graphics.berkeley.edu/papers/Obrien-GMA-1999-08/Obrien-GMA-1999-08.pdf). This can be summarized as the distance a particle in our soft body has collided with into a surface, multiplied by the normal of the point in the surface it has colided into multiplied by some constant.
 
## Midpoint Method
Our midpoint method is defined in the `void ParticleSystem::midpoint(std::vector<Vector3d> x0, std::vector<Vector3d> v0, double stepSize)` function in `particlesystem.cpp`. In order to invoke it, switch out the call to `RK4(x0, v0, stepSize)` (we will discuss this later) on line 320 of the file to `midpoint(x0, v0, stepSize)`. The midpoint method works by 1) updating forces for the current positions and velocities of our soft-body, 2) updating positions and velocities by a halfstep, 3) updating forces based on the new positions and velocites found in 2, 4) finally updating positions and velocities one last time using the values by a fullstep.
![](https://github.com/brown-cs-224/simulation-cranberrymuffin/blob/master/images/midpoint.png)

## Extra Features

### Cow Mesh!
![slack image](https://github.com/brown-cs-224/simulation-cranberrymuffin/blob/master/images/slack.png)
[Tet file available here](https://github.com/brown-cs-224/simulation-cranberrymuffin/blob/master/example-meshes/cow.mesh)

### A higher order integrator: Runge-Kutte 4 (RK4)
RK4 is defined in the `void ParticleSystem::RK4(std::vector<Vector3d> x0, std::vector<Vector3d> v0, double stepSize)` function in `particlesystem.cpp`. RK4 works by 1) updating forces for the current positions and velocities of our soft-body 2) updating positions and velocities by a halfstep 3) repeat steps 1 + 2, 4) update forces again, 5) update positions and velocities one last time using the values by a fullstep. 5) finally using the position and velicity values retrieved in steps 1 - 5, take a weighted average of them (for position and velocity respectively): `dx1 + (2.f * dx2) + (2.f * dx3) + dx4) * stepSize/6.f` and add this value to the initial positions and velocities.
![](https://github.com/brown-cs-224/simulation-cranberrymuffin/blob/master/images/rk4.png)

## Video
[![video](https://github.com/brown-cs-224/simulation-cranberrymuffin/blob/master/images/video_preview.png)](https://drive.google.com/file/d/1Zks9NfcZ4Jl-bH8_YytJiYfZ1vjWEqV3/view?usp=sharing)

The following video captures my simulation running using the RK4 integrator with:

1. The cow mesh colliding with the floor (0:00 - 0:52), (2:06 - 2:56)
2. The ellipsoid mesh colliding with the sphere obstacle and the floor (0:53 - 1:03), (1:19 - 2:06)
3. The sphere mesh colliding with the sphere obstacle and the floor (1:03 - 1:18)

