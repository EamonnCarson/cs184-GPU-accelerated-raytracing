# CS 184: Computer Graphics and Imaging, Spring 2019
## Project 3-2: Pathtracer (2)
###### William Sheu, cs184-adw

## Overview

For this project, I expanded on the pathtracer from the previous part by adding
support for mirror, glass, and microfacet materials, as well as support for
environment lights and depth of field.

## Part 1: Mirror and Glass Materials

![](images/part1_spheres_0.png)  
Spheres rendered with max ray depth of 0. Only the light source can be seen.

![](images/part1_spheres_1.png)  
Max ray depth of 1: We can see the walls now, but we don't have enough bounces
for the spheres to have any color.

![](images/part1_spheres_2.png)  
Max ray depth of 2: We see the ceiling, and we can see the environment from the
previous image reflected on the mirror sphere. We start to see the light source
in the glass sphere.

![](images/part1_spheres_3.png)  
Max ray depth of 3: We see the scene from the previous image reflected in the
mirror sphere, and the scene from 2 images ago refracted in the glass
sphere. Notice that we can now see the walls in the glass sphere.

![](images/part1_spheres_4.png)  
Max ray depth of 4: We see the scene from the previous image reflected in the
mirror sphere, and the scene from 2 images ago refracted in the glass
sphere. Notice that the glass sphere reflection on the mirror sphere is now lit
up, and the light in the glass sphere's shadow.

![](images/part1_spheres_5.png)  
Max ray depth of 5: We see the scene from the previous image reflected in the
mirror sphere, and the scene from 2 images ago refracted in the glass
sphere. Note the light in the glass sphere's shadow now showing up in its
reflection on the mirror sphere.

![](images/part1_spheres_100.png)  
Max ray depth of 100: Not too different from the previous render; just brighter
as the light bouncces more.

## Part 2: Microfacet Materials

![](images/part2_dragon_005.png)  
Dragon with alpha of 0.005: Very glossy surface

![](images/part2_dragon_05.png)  
Dragon with alpha of 0.05: Fairly glossy surface

![](images/part2_dragon_25.png)  
Dragon with alpha of 0.25: Fairly matte surface, but still slightly glosssy

![](images/part2_dragon_5.png)  
Dragon with alpha of 0.5: Very matte surface

![](images/part2_bunny_hemisphere.png)  
Bunny rendered with hemisphere sampling - very noisy render

![](images/part2_bunny_importance.png)  
Bunny rendered with importance sampling - same render time, but much less noisy
render

![](images/part2_dragon_ag.png)  
Dragon rendered with silver as the material instead of gold

## Part 3: Environment Light

For environment lighting, we replace our black background with an environment
map. We sample from this environment for lighting when rays don't intersect any
primitives in the scene, so that the illumination and colors from the
environment are visible in our render. This lets us render scenes in various
environments without having to model the environments.

![](images/grace.png)  
Grace EXR used to render the following images

![](images/grace_probability.png)  
Probability debug image for grace.exr

![](images/part3_bunny_uniform.png)  
Bunny with uniform sampling - extremely noisy render

![](images/part3_bunny_importance.png)  
Bunny with importance sampling - very clear render

![](images/part3_bunny_microfacet_uniform.png)  
Microfacet bunny with uniform sampling - extremely noisy render

![](images/part3_bunny_microfacet_importance.png)  
Microfacet bunny with importance sampling - much less noisy render

## Part 4: Depth of Field

With a pinhole camera model, every point in the scene is in focus. With a
thin-lens camera model, there is a focal plane where things are in focus, but
due to the way we sample from the lens, every other point will not be in focus.

![](images/part4_dragon_closest.png)  
Dragon rendered with focal distance at the nose

![](images/part4_dragon_closer.png)  
Dragon rendered with focal distance at the horns

![](images/part4_dragon_farther.png)  
Dragon rendered with focal distance around the middle

![](images/part4_dragon_farthest.png)  
Dragon rendered with focal distance at the tail

![](images/part4_dragon_smallest.png)  
Dragon rendered with focal distance at the nose, and a tiny aperture size

![](images/part4_dragon_smaller.png)  
Dragon rendered with focal distance at the nose, and a small aperture size

![](images/part4_dragon_larger.png)  
Dragon rendered with focal distance at the nose, and a medium aperture size

![](images/part4_dragon_largest.png)  
Dragon rendered with focal distance at the nose, and a large aperture size
