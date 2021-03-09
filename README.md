# PollenGrain_indentation   [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4590379.svg)](https://doi.org/10.5281/zenodo.4590379)

To use the model, either install the provided MorphoMechanX package (part of the MorphoDynamX modeling software, precompiled for Linux Mint 19 and Ubuntu 20) or require the software here: www.morphomechanx.org (the software has been tested to run against the commmit version ba3c1c73). NOTE: you will need Cuda installed (https://docs.nvidia.com/cuda/index.html).

1) To install MorphoMechanX from package, download the one compatible with your distribution on your computer system. 

2) Use the one with OMP in the name unless you have a
decent double precision graphics card, such as an Geforce Titan or a Titan V. To run the pollen grain indentation model using Cuda (and so the GPU),
 the memory space on the GPU should be sufficiently big, 6GB are known to be enough.

3) If you want to use the Cuda version, install Cuda 11 from the Nvidia website (note that Linux Mint 19 is equivalent to Ubuntu 18)

4) To be able to run the models, some additional packages are required: 
 - libqt5opengl5-dev
 - libboost-all-dev
 - libgsl-dev
 - libthrust-dev
 - libtbb-dev

 The easiest way is to install them directly from terminal by tipying (you will need root privileges): sudo apt-get install "nameOfPackage"

5) Ater completing the installation of MorphoMechanX (and so MorphoDynamX), 
   - download the model package from this git repository (PollenGrain_indentation.tar.gz),
   - open a terminal and move to the folder where the package has been install and extract it (tar -zxvf PollenGrain_indentation.tar.gz "target directory")
   - move to the directory where you have extracted the model package 
   - type make clean
   - type make run
   
This will already load the model in the GUI and it will be ready to run. Follow the video tutorial attached to this GIT repository to learn how to run and edit the model parameters.
