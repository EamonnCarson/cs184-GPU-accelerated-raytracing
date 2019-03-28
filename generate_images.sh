#!/bin/bash

# Part 1
build/pathtracer -t 8 -s 256 -l 4 -m 0 -r 480 360 -f docs/images/part1_spheres_0.png ./dae/sky/CBspheres.dae
build/pathtracer -t 8 -s 256 -l 4 -m 1 -r 480 360 -f docs/images/part1_spheres_1.png ./dae/sky/CBspheres.dae
build/pathtracer -t 8 -s 256 -l 4 -m 2 -r 480 360 -f docs/images/part1_spheres_2.png ./dae/sky/CBspheres.dae
build/pathtracer -t 8 -s 256 -l 4 -m 3 -r 480 360 -f docs/images/part1_spheres_3.png ./dae/sky/CBspheres.dae
build/pathtracer -t 8 -s 256 -l 4 -m 4 -r 480 360 -f docs/images/part1_spheres_4.png ./dae/sky/CBspheres.dae
build/pathtracer -t 8 -s 256 -l 4 -m 5 -r 480 360 -f docs/images/part1_spheres_5.png ./dae/sky/CBspheres.dae
build/pathtracer -t 8 -s 256 -l 4 -m 100 -r 480 360 -f docs/images/part1_spheres_100.png ./dae/sky/CBspheres.dae

# Part 2 (Partial)
build/pathtracer -t 8 -s 256 -l 1 -m 6 -r 480 360 -f docs/images/part2_dragon_005.png ./dae/sky/CBdragon_microfacet_au_005.dae
build/pathtracer -t 8 -s 256 -l 1 -m 6 -r 480 360 -f docs/images/part2_dragon_05.png ./dae/sky/CBdragon_microfacet_au_05.dae
build/pathtracer -t 8 -s 256 -l 1 -m 6 -r 480 360 -f docs/images/part2_dragon_25.png ./dae/sky/CBdragon_microfacet_au_25.dae
build/pathtracer -t 8 -s 256 -l 1 -m 6 -r 480 360 -f docs/images/part2_dragon_5.png ./dae/sky/CBdragon_microfacet_au.dae
