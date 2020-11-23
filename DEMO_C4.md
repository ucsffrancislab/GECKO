#	DEMO

My notes and changes to run the demo on my laptop.


```
export CFLAGS="-I${HOME}/.local/include ${CFLAGS}"
export CPPFLAGS="-I${HOME}/.local/include ${CPPFLAGS}"
export LDFLAGS="-L${HOME}/.local/lib ${LDFLAGS}"
export LD_LIBRARY_PATH="${HOME}/.local/lib:${LD_LIBRARY_PATH}"
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


Install cutadapt

```
pip install --user cutadapt
```


Install fastqc

```
module load fastqc
```

fastqc MAY need an X11 connection even for unused window
I added these lines to my `~/.ssh/config` for this connection

```
ForwardX11 yes
ForwardX11Trusted yes
```


May need to install fastq-dump from sra-tools


Install jellyfish

```
wget https://github.com/gmarcais/Jellyfish/releases/download/v2.3.0/jellyfish-linux
mv jellyfish-linux ~/.local/bin/
chmod +x ~/.local/bin/jellyfish
```




Install mpirun ( there is an mpi module that perhaps I could have used? )

```
wget https://download.open-mpi.org/release/open-mpi/v4.0/openmpi-4.0.5.tar.gz
tar xfvz openmpi-4.0.5.tar.gz
cd openmpi-4.0.5
./configure --prefix=$HOME/.local
make
make all install
```



```
pip install --upgrade --user awscli pip numpy scipy boto3 pyspark sklearn h5py pandas mpi4py matplotlib seaborn
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
// This is nice. Can run directly on n38 without running out of processes
executor{
  queueSize=30
}


process{
    //executor = 'slurm'
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
```


Make sure that prod_client_script_C++_V3.sh uses python3 when calling clientNNc++.py


Don't crush a shared server. Specify OMP_NUM_THREADS or all processors will be used.


```
cd ../..

OMP_NUM_THREADS=24 ./prod_client_script_C++_V3.sh ../../Demo/configGA_microRNA_demo.conf 20
```

That ran, rather quickly. Looks like it finished. Not sure what / where the results are.


Or can I run ... on C4

Need to add shebang line to multipleGeckoStart.py
Also chmod +x multipleGeckoStart.py


```
/bin/rm -rf ../../Demo/DemoGeneticAlgResultDir/ slurm-?????.out GECKO_?????.* __pycache__/ plot_analysis_?????.* 

sbatch multipleGeckoStart.py ../../Demo/configGA_microRNA_demo.conf 20
```

Job is submitted. Waiting?




Several scripts load non-existant modules.
Commented all out.
Rerunning
Add matplotlib
Rerunning
Add seaborn

Replace ".ix" with ".iloc" or ".loc"

Replace ".from_items" with ....
```
/c4/home/gwendt/github/ucsffrancislab/GECKO/Gecko/algoGen/ClassGeneticAlgPlotter.py:481: FutureWarning: from_items is deprecated. Please use DataFrame.from_dict(dict(items), ...) instead. DataFrame.from_dict(OrderedDict(items)) may be used to preserve the key order.
```



Lowering pandas version (like had to with pyspark and MetaGO)
```
pip uninstall pandas
pip install --user pandas==0.23
```


Undid .ix and .from_items changes

redid from_items to from_dict change


python3 has the flush=True option. python2 would need it removed and followed by sys.stdout.flush()


lower sklearn to ~2 years ago (0.20.0)
```
pip uninstall scikit-learn
pip install --user scikit-learn==0.23
```

WORKING!!!

```
/c4/home/gwendt/.local/lib/python3.6/site-packages/sklearn/externals/joblib/externals/cloudpickle/cloudpickle.py:47: Deprecation
Warning: the imp module is deprecated in favour of importlib; see the module's documentation for alternative uses
  import imp
no display found. Using non-interactive Agg backend
```

upgrade sklearn to newest working version?


scikit-learn==0.20 - 
```
/c4/home/gwendt/.local/lib/python3.6/site-packages/sklearn/externals/joblib/externals/cloudpickle/cloudpickle.py:47: DeprecationWarning: the imp module is deprecated in favour of importlib; see the module's documentation for alternative uses
 import imp
```

scikit-learn==0.21???


```
pip uninstall pandas
pip uninstall scikit-learn

pip install --upgrade --user awscli pip cutadapt numpy scipy boto3 pyspark sklearn h5py mpi4py matplotlib seaborn IPython pandas==0.23.4 scikit-learn==0.20.1

cd ~/github/ucsffrancislab/GECKO/Gecko/algoGen && /bin/rm -rf __pycache__/ ../../Demo/DemoGeneticAlgResultDir GECKO_?????.??? plot_analysis_?????.??? slurm-?????.out 

sbatch multipleGeckoStart.py ../../Demo/configGA_microRNA_demo.conf 20
```

ix and iloc are not the same IF THE INDEX IS INTEGER


```
import pandas as pd
df = pd.DataFrame(dict(X=range(4),Y=range(4,4)))
>>> df
   X
0  0
1  1
2  2
3  3
>>> df.ix[0:1]
   X
0  0
1  1
>>> df.iloc[0:1]
   X
0  0
>>> df.loc[0:1]
   X
0  0
1  1
```




scikit-learn 0.21 ....

```
Traceback (most recent call last):
  File "plotter_for_eachhistorylog.py", line 337, in <module>
    MLev.MLevaluation.generateNlzReportClassifierQuality( os.path.join(fi, "fig/BestIndiv10.csvforextractkm.count_SampleMat.csv"), extendsavepath="/indiv"+str(idindiv+1)+"/", nrot=0, hideNeurons=int(nbkmer/2), kmByIndiv=nbkmer, idindiv=idindiv, eliminate=False,classfierType=classfierType)
  File "/c4/home/gwendt/github/ucsffrancislab/GECKO/Gecko/algoGen/MLevaluation.py", line 751, in generateNlzReportClassifierQuality
    slcrossres.loc[clstype, :] = MLevaluator.sklearcrossval(clf=model, idindiv=idindiv,nrot=nrot)
  File "/c4/home/gwendt/github/ucsffrancislab/GECKO/Gecko/algoGen/MLevaluation.py", line 188, in sklearcrossval
    print("Train score mean = {}".format(np.mean(res["train_score"])))
KeyError: 'train_score'
```



This was created in November 2018. Some newer software is proving incompatible.

pandas 0.23.4

scikit-learn 0.20.1




