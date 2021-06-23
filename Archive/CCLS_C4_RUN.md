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

Nextflow uses a Java VM so needs to be from a non-login node ...


```BASH
cd ~/github/ucsffrancislab/GECKO/ImportMatrix
#/bin/rm -rf ccls_import/ work/ results/

./main.pl decomposition --outdir ccls_import -B --reads '../CCLS/*bam' --kmersize 20 -resume
```

Takes quite a while. 

Then cleanup.


```
rm work/* -rf
```



```
find ccls_import -type f -exec chmod a-w {} \;

./main.pl importation --groupconfig ../CCLS/microRNA_demo.conf --outdir ccls_import -resume

#	That was 2 steps and the join failed.
#	Does not "resume" so calling explicitly on n17.
#	We'll see if there is enough memory.

#	nohup /c4/home/gwendt/github/ucsffrancislab/GECKO/ImportMatrix/bin/JoinKMers ccls_import/rawimport/matrix/RAWmatrix.matrix ccls_import/rawimport/matrix/subgroupaa.conf.matrix ccls_import/rawimport/matrix/subgroupab.conf.matrix &

#	kinda worked but doesn't look right.
#	trying again. All good now. All 10 columns.

./main.pl discretization --matrix ccls_import/rawimport/matrix/RAWmatrix.matrix –-outdir ccls_import
```

The above does not put the output in `ccls_import`.
Initially it had `-outdir` and not `--outdir`, but that wasn't the problem.
Not sure what's special about it.


```
mv results/discretization ccls_import/
```

This takes FOREVER! Set to 299 hours and still doesn't finish.
Only uses 1 CPU at about 15%.

du -sh ccls_import/discretization/matrix/DISCRETmatrix.matrix
102G	ccls_import/discretization/matrix/DISCRETmatrix.matrix

du -sh work/
9.2G	work/


```
nohup ./main.pl filter --matrix ccls_import/discretization/matrix/DISCRETmatrix.matrix  --outdir ccls_import -resume
```

















```
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


