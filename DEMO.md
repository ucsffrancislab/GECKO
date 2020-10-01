#	DEMO

My notes and changes to run the demo on my laptop.


```
export CFLAGS="-I${HOME}/.local/include -I/opt/local/include ${CFLAGS}"
export CPPFLAGS="-I${HOME}/.local/include -I/opt/local/include ${CPPFLAGS}"
export LDFLAGS="-L${HOME}/.local/lib -L/opt/local/lib ${LDFLAGS}"
export LD_LIBRARY_PATH="/opt/local/lib:${LD_LIBRARY_PATH}"
```


Install mpirun
I use MacPorts. Don't forget to select it.
```
...
sudo port install mpich-devel

sudo port select mpi mpich-devel-mp-fortran
```


Install mpi4py python package.
(I actually installed with MacPorts)
```
pip install --user mpi4py
```



Install boost, if you don't have it already.

```
wget https://dl.bintray.com/boostorg/release/1.74.0/source/boost_1_74_0.tar.gz
tar xvfz boost_1_74_0.tar.gz
cd boost_1_74_0
./bootstrap.sh --prefix=$HOME/.local
./b2
./b2 headers
./b2 install
```



Install fastqc
```
...
```



fastqc needs an X11 connection even for unused window
I added these lines to my `~/.ssh/config` for this connection
```
	ForwardX11 yes
	ForwardX11Trusted yes
```



Install fastq-dump from sra-tools
```
...
```





Download demo data and compile first, then process.



```
cd ~/github/ucsffrancislab/GECKO

cd Demo
sh createdataset.sh
cd ..

cd ImportMatrix
curl -s https://get.nextflow.io | bash 

cd bin/src
```

edited compile.sh and anova_makefile to include CFLAGS, CPPFLAGS and LDFLAGS

change uint to uint64_t in transformIntoMLformat.cpp

(compiled on head node for cluster test)

```
./compile.sh
cd ../../
```

Edited nextflow.config to change executor and softPath

Also, limit the queue size so doesn't overload the server raising ...
```
  Error occurred during initialization of VM
  java.lang.OutOfMemoryError: unable to create new native thread
```


```
cp nextflow.config.example nextflow.config

cd ../Gecko/algoGen/Producteurv2/utils

./compile.sh

cd .. 
```

( you should be in Gecko/algoGen/Producteurv2 after this command)

Edited Makefile

```
make clean
make
cd ../../../ImportMatrix

```



Now process





```
./main.pl decomposition --singleEnd --outdir demo_import --reads '../Demo/*fastq' --kmersize 20
```

Takes a while. 

Then cleanup.

```
rm work/* -rf
```

Change paths in ../Demo/microRNA_demo.conf


```
cp ../Demo/microRNA_demo.conf.example ../Demo/microRNA_demo.conf

./main.pl importation --groupconfig ../Demo/microRNA_demo.conf --outdir demo_import

./main.pl discretization --matrix demo_import/rawimport/matrix/RAWmatrix.matrix –-outdir demo_import
```


The above does not put the output in `demo_import`.
Initially it had `-outdir` and not `--outdir`, but that wasn't the problem.
Not sure what's special about it.


```
mv results/discretization demo_import/

./main.pl filter --matrix demo_import/discretization/matrix/DISCRETmatrix.matrix  --outdir demo_import

./main.pl real --matrixDiscrete demo_import/filtering/matrix/FILTEREDmatrix.matrix --matrixRaw demo_import/rawimport/matrix/RAWmatrix.matrix --outdir demo_import/




cd ../Gecko/algoGen/Producteurv2/utils

./transformIntoBinary ../../../../ImportMatrix/demo_import/filtering/final/FILTEREDmatrix_RealCounts.matrix ../../../../ImportMatrix/demo_import/filtering/final/FILTEREDmatrix_RealCounts.bin

mkdir ../../../../ImportMatrix/demo_import/filtering/final/CutMatrix/

./indexBinary ../../../../ImportMatrix/demo_import/filtering/final/FILTEREDmatrix_RealCounts.bin ../../../../ImportMatrix/demo_import/filtering/final/CutMatrix/example.bin 1000






./prod_client_script_C++_V3.sh ../../Demo/configGA_microRNA_demo.conf 20
```


That seemed to be working but was crushing my laptop so I killed the process.

Next to try on a single isolated node on the cluster.



