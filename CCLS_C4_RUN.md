#	CCLS C4 RUN


```BASH
cd ~/github/ucsffrancislab/GECKO
mkdir CCLS
for normal in /francislab/data1/raw/CCLS/bam/G*bam; do
tumor=${normal/GM_/}
for bam in $tumor $normal ; do
echo $bam
base=$( basename $bam .recaled.bam )
ln -s $bam CCLS/${base}.bam
done
done
```



```BASH
cd ~/github/ucsffrancislab/GECKO/ImportMatrix
/bin/rm -rf ccls_import/ work/ results/

./main.pl decomposition --outdir ccls_import -B --reads '../CCLS/*bam' --kmersize 20 -resume
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

./main.pl importation --groupconfig ../Demo/microRNA_demo.conf --outdir ccls_import

./main.pl discretization --matrix ccls_import/rawimport/matrix/RAWmatrix.matrix –-outdir ccls_import
```

The above does not put the output in `ccls_import`.
Initially it had `-outdir` and not `--outdir`, but that wasn't the problem.
Not sure what's special about it.


```
mv results/discretization ccls_import/

./main.pl filter --matrix ccls_import/discretization/matrix/DISCRETmatrix.matrix  --outdir ccls_import

./main.pl real --matrixDiscrete ccls_import/filtering/matrix/FILTEREDmatrix.matrix --matrixRaw ccls_import/rawimport/matrix/RAWmatrix.matrix --outdir ccls_import/




cd ~/github/ucsffrancislab/GECKO/Gecko/algoGen/Producteurv2/utils

./transformIntoBinary ../../../../ImportMatrix/ccls_import/filtering/final/FILTEREDmatrix_RealCounts.matrix ../../../../ImportMatrix/ccls_import/filtering/final/FILTEREDmatrix_RealCounts.bin

mkdir ../../../../ImportMatrix/ccls_import/filtering/final/CutMatrix/

./indexBinary ../../../../ImportMatrix/ccls_import/filtering/final/FILTEREDmatrix_RealCounts.bin ../../../../ImportMatrix/ccls_import/filtering/final/CutMatrix/example.bin 1000
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






