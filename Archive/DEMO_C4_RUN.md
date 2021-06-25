#	DEMO C4 RUN


Prep work run on a development node.


Now process.
May need to load modules if in new shell environment.

May need an older version of numpy
/c4/home/gwendt/.local/lib/python3.6/site-packages/numpy/lib/function_base.py:2559: RuntimeWarning: invalid value encountered in true_divide
  c /= stddev[:, None]
/c4/home/gwendt/.local/lib/python3.6/site-packages/numpy/lib/function_base.py:2560: RuntimeWarning: invalid value encountered in true_divide
  c /= stddev[None, :]




```
cd ~/github/ucsffrancislab/GECKO/ImportMatrix
/bin/rm -rf demo_import/ work/ results/

./main.pl decomposition --singleEnd --outdir demo_import --reads '../Demo/*fastq' --kmersize 20
```

Takes a while. 

Then cleanup.

```
rm work/* -rf
```

Change paths in ../Demo/microRNA_demo.conf
nextflow.config



```
#cp ../Demo/microRNA_demo.conf.example ../Demo/microRNA_demo.conf

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




cd ~/github/ucsffrancislab/GECKO/Gecko/algoGen/Producteurv2/utils

./transformIntoBinary ../../../../ImportMatrix/demo_import/filtering/final/FILTEREDmatrix_RealCounts.matrix ../../../../ImportMatrix/demo_import/filtering/final/FILTEREDmatrix_RealCounts.bin

mkdir ../../../../ImportMatrix/demo_import/filtering/final/CutMatrix/

./indexBinary ../../../../ImportMatrix/demo_import/filtering/final/FILTEREDmatrix_RealCounts.bin ../../../../ImportMatrix/demo_import/filtering/final/CutMatrix/example.bin 1000
```


Need to add shebang line to multipleGeckoStart.py
Also chmod +x multipleGeckoStart.py

If sbatch is run from a login node, GA_sbatch.sh will silently fail.
Not entirely clear why, but think because something isn't loaded and not clear which just yet.
Will investigate.
Likely openmpi
Also need the "SBATCH --export=NONE" line.

```
cd ~/github/ucsffrancislab/GECKO/Gecko/algoGen

/bin/rm -rf ../../Demo/DemoGeneticAlgResultDir/ slurm-?????.out GECKO_?????.* __pycache__/ plot_analysis_?????.* 

sbatch multipleGeckoStart.py ../../Demo/configGA_microRNA_demo.conf 20
```

Job is submitted. Waiting ...







-----





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
>>> import pandas as pd
>>> df = pd.DataFrame(dict(X=range(4))
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

```
>>> import pandas as pd
>>> df = pd.DataFrame(dict(X=range(4),Y=range(4,8)))
>>> df
   X  Y
0  0  4
1  1  5
2  2  6
3  3  7
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




