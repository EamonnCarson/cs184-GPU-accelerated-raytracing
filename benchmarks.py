import time
import os
import statistics
from collections import OrderedDict

num_threads = 1
num_trials = 8
dry_run = False

params = [(1, 4), (4, 4), (16, 4), (64, 4),
          (1, 32), (4, 32), (16, 32), (64, 32)]

# files = [('bunny', '../dae/sky/CBbunny.dae'),
#          ('dragon', '../dae/sky/dragon.dae'),
#          ('spheres', '../dae/sky/CBspheres_lambertian.dae')]
files = [('spheres', '../dae/sky/CBspheres_lambertian.dae')]

output = []
stats = OrderedDict()

for tag, name in files:
    for s, l in params:
        output_name = '{}_{}_{}.png'.format(tag, s, l)
        output.append(output_name)
        times = []
        for i in range(8):
            start = time.time()
            if not dry_run:
                os.system('./pathtracer -t {} -s {} -l {} -f {} {}'.format(num_threads, s, l, output_name, name))
            else:
                print('./pathtracer -t {} -s {} -l {} -f {} {}'.format(num_threads, s, l, output_name, name))
            runtime = time.time() - start
            times.append(runtime)
            output.append(str(runtime))
        stats[output_name] = (str(statistics.mean(times)), str(statistics.median(times)), str(statistics.variance(times)))

output.append('name mean median var')
for name in stats:
    output.append("{} {}".format(name," ".join(stats[name])))

with open("benchmark_log.txt", "w") as log:
    log.write("\n".join(output))


