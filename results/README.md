# Benchmark Results

This folder contains the various attempts and configurations used to
perform the benchmark process detailed in the accompanying paper.

## First and second runs

The first and second runs were run by using the PBS directives detailed
in the paper (one node, `place=pack:excl`).

## Third run

The third run deviates from the first two runs since its main goal was
that of benchmarking the performance on multiple nodes in a proper
distributed-memory setting. In order to do that, the PBS directives
included a `select:2` to select two nodes and used `place=scatter` to
make sure that the use of inter-node communications would be maximized.
