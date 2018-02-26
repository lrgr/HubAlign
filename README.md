# HubAlign Python wrapper
<img src='https://travis-ci.org/lrgr/HubAlign.svg?branch=master' >
Python wrapper of [HubAlign](https://github.com/hashemifar/HubAlign) that outputs cross-species gene similarity scores.

### Setup

#### Dependencies

The methods and experiments are written in Python 3. We recommend using Conda to manage dependencies, which you can do directly using the provided `environment.yml` file:

    conda env create -f environment.yml
    source activate hubalign-py-env

Note that HubAlign requires a C++ compiler. Consequently, we include the Conda [`gxx_linux_64`](https://anaconda.org/anaconda/gxx_linux-64) as a dependency in the `environment.yml`. You may need to update or remove this dependency if you are on a non-Linux system.
