#	EV C4 RUN

ONE TIME FOR SETUP
```
paste <( ls -1 /c4/home/gwendt/github/ucsffrancislab/GECKO/ImportMatrix/ev_import/jellyfish/text/SFHH005?.ojf.tab && ls -1 /c4/home/gwendt/github/ucsffrancislab/GECKO/ImportMatrix/ev_import/jellyfish/text/SFHH005??.ojf.tab ) <( tail -n +2 /francislab/data1/raw/20210428-EV/metadata.csv | awk -F\" '{print $2}' ) > /c4/home/gwendt/github/ucsffrancislab/GECKO/EV_metadata.conf

vi /c4/home/gwendt/github/ucsffrancislab/GECKO/EV_metadata.conf

awk 'BEGIN{FS=OFS="\t"}{print $1,NR,$2}' /c4/home/gwendt/github/ucsffrancislab/GECKO/EV_metadata.conf > tmp
mv tmp /c4/home/gwendt/github/ucsffrancislab/GECKO/EV_metadata.conf
```

metadata.conf is a GBM/nonGBM file


Next time, try to submit these commands to the queue
If submitting works, could set some dependencies and make this a proper script.
Will it require specifying where to run given that none of this is in the PATH

sbatch="sbatch --mail-user=George.Wendt@ucsf.edu --mail-type=FAIL "
date=$( date "+%Y%m%d%H%M%S" )


Use fastq 
Lexogen or Diagenode?
bbduk{1,2,3} or cutadapt{1,2,3}?

lte30 or gt30 or all ( try lte30 then do all )


```
cd ~/github/ucsffrancislab/GECKO
mkdir EV
for f in /francislab/data1/working/20210428-EV/20210518-preprocessing/output/SFHH005*.cutadapt2.lte30.fastq.gz ; do
echo $f
base=$( basename $f .cutadapt2.lte30.fastq.gz )
ln -s $f EV/${base}.fastq.gz
done
```


Nextflow uses a Java VM so needs to be from a non-login dev node ( or as a job on the cluster )



```BASH
cd ~/github/ucsffrancislab/GECKO/ImportMatrix
#/bin/rm -rf ev_import/* work/* results/*

${sbatch} --job-name=decomposition --time=999 --ntasks=4 --mem=30G --output=${PWD}/ev_import/decomposition.${date}.txt ${PWD}/main.pl decomposition --singleEnd --outdir ${PWD}/ev_import --reads '/c4/home/gwendt/github/ucsffrancislab/GECKO/EV/*.fastq.gz' --kmersize 15

#./main.pl decomposition --singleEnd --outdir ev_import --reads '../EV/*.fastq.gz' --kmersize 15 -resume
```

Can take quite a while depending on file size. 

Then cleanup.


```
rm work/* -rf
```



```
find ev_import -type f -exec chmod a-w {} \;

#./main.pl importation --groupconfig ../EV/metadata3.conf --outdir ev_import -resume

${sbatch} --job-name=importation --time=999 --ntasks=4 --mem=30G --output=${PWD}/ev_import/importation.${date}.txt ${PWD}/main.pl importation --groupconfig /c4/home/gwendt/github/ucsffrancislab/GECKO/EV_metadata.conf --outdir ${PWD}/ev_import


#./main.pl discretization --matrix ev_import/rawimport/matrix/RAWmatrix.matrix –-outdir ev_import

${sbatch} --job-name=discretization --time=999 --ntasks=4 --mem=30G --output=${PWD}/ev_import/discretization.${date}.txt ${PWD}/main.pl discretization --matrix ${PWD}/ev_import/rawimport/matrix/RAWmatrix.matrix –-outdir ${PWD}/ev_import
```

The above does not put the output in `ev_import`.
Initially it had `-outdir` and not `--outdir`, which was incorrect, but not the problem.
Not sure what's special about this step.

This next step can take days depending on data size so ALWAYS run with nohup (or submit to queue).


```
mv results/discretization ev_import/

#nohup ./main.pl filter --matrix ev_import/discretization/matrix/DISCRETmatrix.matrix --outdir ev_import -resume &
tail -f nohup.out

${sbatch} --job-name=filter --time=9999 --ntasks=8 --mem=61G --output=${PWD}/ev_import/filter.${date}.txt ${PWD}/main.pl filter --matrix ${PWD}/ev_import/discretization/matrix/DISCRETmatrix.matrix --outdir ${PWD}/ev_import


${sbatch} --job-name=real --time=999 --ntasks=8 --mem=61G --output=${PWD}/ev_import/real.${date}.txt ${PWD}/main.pl real --matrixDiscrete ${PWD}/ev_import/filtering/matrix/FILTEREDmatrix.matrix --matrixRaw ${PWD}/ev_import/rawimport/matrix/RAWmatrix.matrix --outdir ${PWD}/ev_import/


cd ~/github/ucsffrancislab/GECKO/Gecko/algoGen/Producteurv2/utils

${sbatch} --job-name=transformIntoBinary --time=999 --ntasks=8 --mem=61G --output=${PWD}/ev_import/transformIntoBinary.${date}.txt --wrap="/c4/home/gwendt/github/ucsffrancislab/GECKO/Gecko/algoGen/Producteurv2/utils/transformIntoBinary /c4/home/gwendt/github/ucsffrancislab/GECKO/ImportMatrix/ev_import/filtering/final/FILTEREDmatrix_RealCounts.matrix /c4/home/gwendt/github/ucsffrancislab/GECKO/ImportMatrix/ev_import/filtering/final/FILTEREDmatrix_RealCounts.bin"

mkdir ~/github/ucsffrancislab/GECKO/ImportMatrix/ev_import/filtering/final/CutMatrix/

${sbatch} --job-name=indexBinary --time=999 --ntasks=8 --mem=61G --output=${PWD}/ev_import/indexBinary.${date}.txt --wrap="/c4/home/gwendt/github/ucsffrancislab/GECKO/Gecko/algoGen/Producteurv2/utils/indexBinary /c4/home/gwendt/github/ucsffrancislab/GECKO/ImportMatrix/ev_import/filtering/final/FILTEREDmatrix_RealCounts.bin /c4/home/gwendt/github/ucsffrancislab/GECKO/ImportMatrix/ev_import/filtering/final/CutMatrix/example.bin 1000"
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



cd ~/github/ucsffrancislab/GECKO/Gecko/algoGen


date=$( date "+%Y%m%d%H%M%S" )
for i in $( seq 0 9 ) ; do
#mkdir /c4/home/gwendt/github/ucsffrancislab/GECKO/EV/GeneticAlgResult${i}Dir/
${sbatch} --job-name=${i}multipleGeckoStart --time=999 --ntasks=4 --mem=30G --output=/c4/home/gwendt/github/ucsffrancislab/GECKO/EV/GeneticAlgResult${i}Dir/multipleGeckoStart.${date}.txt /c4/home/gwendt/github/ucsffrancislab/GECKO/Gecko/algoGen/multipleGeckoStart.py /c4/home/gwendt/github/ucsffrancislab/GECKO/EV/GA${i}.conf 15
done

```

Job is submitted. Waiting ...






```

tar cvf - EV | gzip > 20210606-EV-Gecko.tar.gz

BOX="https://dav.box.com/dav/Francis _Lab_Share"

curl -netrc -T 20210606-EV-Gecko.tar.gz "${BOX}/"
```


