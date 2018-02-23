# hubalign-py
Python wrapper of [HubAlign](https://github.com/hashemifar/HubAlign) that outputs cross-species gene similarity scores.

### Setup

#### Dependencies

The methods and experiments are written in Python 3. We recommend using Conda to manage dependencies, which you can do directly using the provided `environment.yml` file:

    conda env create -f environment.yml
    source activate hubalign-py-env

#### HubAlign

HubAlign requires a C++ compiler.

1. Compile HubAlign:

        g++ -o bin/HubAlign src/HubAlign.cpp src/Alignment.cpp src/Network.cpp

2. Then test:

        ./HubAlign example/Test1.tab example/Test2.tab -l 0.1 -a 0.7 -d 10 -b example/Test1_Test2.bitscore


###
