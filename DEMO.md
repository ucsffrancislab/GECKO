#	DEMO

My notes and changes to run the demo on my laptop.



boost and mpirun required (I used MacPorts)

don't forget to select the installed version

sudo port select mpi mpich-devel-mp-fortran



fastqc

sra-tools (fastq-dump)

mpi4py




```
export CFLAGS="-I/Users/jakewendt/.local/include -I/opt/local/include"
export CPPFLAGS="-I/Users/jakewendt/.local/include -I/opt/local/include"
export LDFLAGS="-L/Users/jakewendt/.local/lib -L/opt/local/lib"
export LD_LIBRARY_PATH="/opt/local/lib:${LD_LIBRARY_PATH}"

cd ~/github/ucsffrancislab/GECKO


cd DEMO
sh createdataset.sh
cd ..



cd ImportMatrix
curl -s https://get.nextflow.io | bash 


cd bin/src
```

edited compile.sh and anova_makefile to include CFLAGS, CPPFLAGS and LDFLAGS

change uint to uint64_t in transformIntoMLformat.cpp

```
./compile.sh
cd ../../
```

edited nextflow.config to change executor and softPath

```
perl main.pl decomposition --singleEnd --outdir demo_import --reads '../Demo/*fastq' --kmersize 20
```


Takes a while. 

Then cleanup.










```
rm work/* -rf
```

Change paths in ../Demo/microRNA_demo.conf


```
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

./compile.sh

./transformIntoBinary ../../../../ImportMatrix/demo_import/filtering/final/FILTEREDmatrix_RealCounts.matrix ../../../../ImportMatrix/demo_import/filtering/final/FILTEREDmatrix_RealCounts.bin

mkdir ../../../../ImportMatrix/demo_import/filtering/final/CutMatrix/

./indexBinary ../../../../ImportMatrix/demo_import/filtering/final/FILTEREDmatrix_RealCounts.bin ../../../../ImportMatrix/demo_import/filtering/final/CutMatrix/example.bin 1000





cd .. 
```

( you should be in Gecko/algoGen/Producteurv2 after this command)

Edited Makefile

```
make clean
make
cd ..

./prod_client_script_C++_V3.sh ../../Demo/configGA_microRNA_demo.conf 20
```


That seemed to be working but was crushing my laptop so I killed the process.

Next to try on a single isolated node on the cluster.



