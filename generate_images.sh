#!/bin/bash

# Part 1
build/pathtracer -t 8 -s 256 -l 4 -m 0 -b 0 -r 480 360 -f docs/images/part1_spheres_0.png ./dae/sky/CBspheres.dae
build/pathtracer -t 8 -s 256 -l 4 -m 1 -b 0 -r 480 360 -f docs/images/part1_spheres_1.png ./dae/sky/CBspheres.dae
build/pathtracer -t 8 -s 256 -l 4 -m 2 -b 0 -r 480 360 -f docs/images/part1_spheres_2.png ./dae/sky/CBspheres.dae
build/pathtracer -t 8 -s 256 -l 4 -m 3 -b 0 -r 480 360 -f docs/images/part1_spheres_3.png ./dae/sky/CBspheres.dae
build/pathtracer -t 8 -s 256 -l 4 -m 4 -b 0 -r 480 360 -f docs/images/part1_spheres_4.png ./dae/sky/CBspheres.dae
build/pathtracer -t 8 -s 256 -l 4 -m 5 -b 0 -r 480 360 -f docs/images/part1_spheres_5.png ./dae/sky/CBspheres.dae
build/pathtracer -t 8 -s 256 -l 4 -m 100 -b 0 -r 480 360 -f docs/images/part1_spheres_100.png ./dae/sky/CBspheres.dae

# Part 2 (Partial)
build/pathtracer -t 8 -s 256 -l 1 -m 6 -b 0 -r 480 360 -f docs/images/part2_dragon_005.png ./dae/sky/CBdragon_microfacet_au_005.dae
build/pathtracer -t 8 -s 256 -l 1 -m 6 -b 0 -r 480 360 -f docs/images/part2_dragon_05.png ./dae/sky/CBdragon_microfacet_au_05.dae
build/pathtracer -t 8 -s 256 -l 1 -m 6 -b 0 -r 480 360 -f docs/images/part2_dragon_25.png ./dae/sky/CBdragon_microfacet_au_25.dae
build/pathtracer -t 8 -s 256 -l 1 -m 6 -b 0 -r 480 360 -f docs/images/part2_dragon_5.png ./dae/sky/CBdragon_microfacet_au.dae

# Part 3
build/pathtracer -t 8 -s 256 -l 4 -m 8 -r 480 360 -c cam1.txt -b 0.1 -d 2.3 -f docs/images/part4_dragon_closest.png ./dae/sky/CBdragon_microfacet_ag.dae
build/pathtracer -t 8 -s 256 -l 4 -m 8 -r 480 360 -c cam1.txt -b 0.1 -d 2.53 -f docs/images/part4_dragon_closer.png ./dae/sky/CBdragon_microfacet_ag.dae
build/pathtracer -t 8 -s 256 -l 4 -m 8 -r 480 360 -c cam1.txt -b 0.1 -d 2.76 -f docs/images/part4_dragon_farther.png ./dae/sky/CBdragon_microfacet_ag.dae
build/pathtracer -t 8 -s 256 -l 4 -m 8 -r 480 360 -c cam1.txt -b 0.1 -d 3.0 -f docs/images/part4_dragon_farthest.png ./dae/sky/CBdragon_microfacet_ag.dae

build/pathtracer -t 8 -s 256 -l 4 -m 8 -r 480 360 -c cam1.txt -b 0.01 -d 2.3 -f docs/images/part4_dragon_smallest.png ./dae/sky/CBdragon_microfacet_ag.dae
build/pathtracer -t 8 -s 256 -l 4 -m 8 -r 480 360 -c cam1.txt -b 0.05 -d 2.3 -f docs/images/part4_dragon_smaller.png ./dae/sky/CBdragon_microfacet_ag.dae
build/pathtracer -t 8 -s 256 -l 4 -m 8 -r 480 360 -c cam1.txt -b 0.1 -d 2.3 -f docs/images/part4_dragon_larger.png ./dae/sky/CBdragon_microfacet_ag.dae
build/pathtracer -t 8 -s 256 -l 4 -m 8 -r 480 360 -c cam1.txt -b 0.2 -d 2.3 -f docs/images/part4_dragon_largest.png ./dae/sky/CBdragon_microfacet_ag.dae
