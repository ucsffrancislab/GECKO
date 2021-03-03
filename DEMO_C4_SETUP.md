#	DEMO

Setup environment to run with minimal modifications.
Use a development node.

This software was initially committed in November 2018.
Some code is dated requiring older versions of pandas and scikit-learn,
most notably the use of ".ix" and ".from_dict".

Some code is very specific to the author's environment, notably the use of modules. Commented them out.

Changed uint to uint64_t in cpp files.

Added ${CPPFLAGS} ${LDFLAGS} to some Makefiles.

Added shebang line to some python scripts.

"flush" modification for python2 could now probably be undone.


```
Gecko/algoGen/ClassGeneticAlgPlotter.py
Gecko/algoGen/GA_sbatch_hpclr.sh
Gecko/algoGen/GA_sbatch_sauv.sh
Gecko/algoGen/GA_sbatch.sh
Gecko/algoGen/GA_sbatch_singularity.sh
Gecko/algoGen/multipleGeckoStart.py
Gecko/algoGen/nlyz_sbatch_hpclr.sh
Gecko/algoGen/nlyz_sbatch.sh
Gecko/algoGen/nlyz_sbatch_singularity.sh
Gecko/algoGen/occurencemerge.py
Gecko/algoGen/prod_client_script_C++_V3.sh
Gecko/algoGen/Producteurv2/ExtractNetwork.cpp
Gecko/algoGen/Producteurv2/Makefile
Gecko/algoGen/Producteurv2/utils/compile.sh
Gecko/algoGen/voting_nlyz_sbatch.sh
ImportMatrix/02_importation.nf
ImportMatrix/bin/src/anova_makefile
ImportMatrix/bin/src/compile.sh
ImportMatrix/bin/src/transformIntoMLformat.cpp
ImportMatrix/nextflow.config
```

Replace ".from_items" with ".from_dict" as suggested by deprecatation warning.


Replace ".ix" with ".iloc" or ".loc" or ".at" in Gecko/algoGen/ClassGeneticAlgPlotter.py and Gecko/algoGen/occurencemerge.py
Are all of my ix->iloc changes correct?
ix and iloc are not syntactically equivalent if the data frame index is an integer and the selection is a range.
ix[0:10] is not the same as iloc[0:10] as iloc is exclusive on the last position.
I think that it would have to be ix[0:10] ==? iloc[0:11].

I've undone the ix->iloc changes and don't get a warning anymore?







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



Install fastqc. As a module this will need done each time or added to the script.

```
module load fastqc
```

fastqc MAY need an X11 connection even for unused window
I added these lines to my `~/.ssh/config` for this connection
They have since been removed.

```
ForwardX11 yes
ForwardX11Trusted yes
```


May need to install fastq-dump from sra-tools
```
module load sratoolkit/2.10.9
```


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

make uninstall
```

Don't use this module. It seems to be old and sets some python2.7 things.
Or simply set PYTHONPATH to nothing.

```
module load mpi/openmpi-x86_64
export PYTHONPATH=""
```

NumPy 1.16.0 (release notes) – 14 Jan 2019. (20210213)

try
NumPy 1.15.0 (release notes) – 23 Jul 2018. (20210212b)


```
python3 -m pip install --upgrade --user awscli pip numpy scipy boto3 pyspark sklearn h5py mpi4py matplotlib seaborn IPython pandas==0.23.4 scikit-learn==0.20.1 cutadapt==1.18
```

```
/c4/home/gwendt/github/ucsffrancislab/GECKO/Gecko/algoGen/ClassGeneticAlgPlotter.py:481: FutureWarning: from_items     is deprecated. Please use DataFrame.from_dict(dict(items), ...) instead. DataFrame.from_dict(OrderedDict(items)) ma    y be used to preserve the key order.
```

scikit learn
```
/c4/home/gwendt/.local/lib/python3.6/site-packages/sklearn/utils/deprecation.py:125: FutureWarning: You are accessing a training score ('train_score'), which will not be available by default any more in 0.21. If you need training scores, please set return_train_score=True
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



