# CS 184: Computer Graphics and Imaging, Spring 2019
## Final Project Milestone Report: GPU Accelerated Pathtracing
###### Authors: William Sheu, Eric Zhou, Eamonn Carson

## Current Accomplishments
Currently we have ported the basic engine of project 3-2 over to the OpenCL framework.
Specifically, we have:
1. Set up our OpenCL libraries and gotten our code to work on Ubuntu and Windows (OpenCL is deprecated in OSX and isn't compiling properly, so we have abandoned OSX compatibility for now) with both Intel and NVIDIA graphics chips.
2. Implemented basic raytracing for triangles and spheres. There is no lighting implemented yet, so we just display the value of the surface normal at the intersection for each pixel.
3. Implemented traversal of a bounding volume hierarchy on GPU. We considered this a necessary component in order to compete with CPU based raytracing methods. BVH construction was kept on the CPU, but we needed to flatten the BVH so that it would be laid out contiguously in mermory in order to interface with the OpenCl kernel.

## Progress Relative to Original Timeline
The original timeline that we presented went as such:
- 4/9-4/16: Gain familiarity with OpenCL
- 4/16-4/23: Port over the computationally intensive parts of Project 3 to use OpenCL instead
- 4/23-4/30: Finish porting, begin running benchmarks
- 4/30-5/9: Work on final presentation. If there is remaining time, work on stretch goals.

Our current progress (today is 4/30) is that we are still about a bit less than halfway through the porting step.
As such, we are approximately one week behind schedule.
We will likely not have to abandon any of our original goals; however, it is unlikely that we will be able to achieve any stretch goals, and only will have time to create a basic implementation of a GPU raytracer.

Things that remain to be done are:
1. Finish porting project 3-2 to use OpenCL
    - Implement direct lighting
    - Implement materials (micro-facet, mirrors, diffuse, glass)
    - Implement indirect lighting
2. Benchmark our code
3. Write final report

## Reflection on Progress
Overall we have a fair bit of work set out for us; however, it also appears that our velocity is high enough (because William is a wizard) that completing all tasks on time is feasible. 

The implementation of direct lighting will be of middling difficulty, since the operation is by nature embarrassingly parallel (independent samples from independent points on independent light source). 

The hardest part that will be coming to us is definitely going to be the implementation of the implementation of indirect lighting, since the recursive structure of that operation will be difficult to replicate because its a long sequential sequential process as opposed to the short parallelize-able processes that OpenCL is really meant for.

However, afterwards the work will be fairly easy and distributable: materials are likely going to be almost identical to those that we've implemented already (we'd just port them to the OpenCL architecture), and benchmarks and the final report are not particularly difficult.

## Updated Work Plan
Depending on how squeezed we are for time, we may need to stop work on importance sampling and only have uniform sampling, and we may need to skip some types of materials (micro-facet for example). Other than that there are no major changes to our work plan.

## Slide show Presentation
[Click this link to see the milestone slide show](https://docs.google.com/presentation/d/e/2PACX-1vSz2fW5cA52-eeCSBh4bAUiQwhRjFbWnK33cN2306zHP4m0o4lz3Jas4tF-UhsGEfcCku5vgNWJobxN/pub?start=false&loop=false&delayms=30000)

## Video Presentation
[Click this link to see the milestone video](https://youtu.be/IRanldDhsLI)
