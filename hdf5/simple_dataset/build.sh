#!/bin/bash
export PHDF5=/home/peyrardj/local/hdf5-1.8.18/ ; 
export PATH=${PHDF5}/bin:$PATH
export LD_LIBRARY_PATH=${PHDF5}/lib:$LD_LIBRARY_PATH
mpicc -I${PHDF5}/include -L${PHDF5}/lib -lhdf5 dataset.c 		-o dataset
mpicc -I${PHDF5}/include -L${PHDF5}/lib -lhdf5 hyperslab_by_row.c 	-o hyperslab_by_row

