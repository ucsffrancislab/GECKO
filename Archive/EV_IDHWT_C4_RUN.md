#	EV IDH WT C4 RUN

DO NOT USE --export=None with sbatch HERE. IT WILL FAIL.

export sbatch="sbatch --mail-user=George.Wendt@ucsf.edu --mail-type=FAIL "
export GECKO=/c4/home/gwendt/github/ucsffrancislab/GECKO
export GDATA=${GECKO}/ImportMatrix/ev_IDHWT_import
export k=15
date=$( date "+%Y%m%d%H%M%S" )



ONE TIME FOR SETUP
( "\042" is the octal code for a double quote and does not muck with syntax highlighting. )
( Could also use "\x22" which is the ASCII code for a double quote. )
( "\x27" is the code for a single quote, FYI )

```
paste <( ls -1 ${GDATA}/jellyfish/text/SFHH005?.ojf.tab && ls -1 ${GDATA}/jellyfish/text/SFHH005??.ojf.tab ) <( tail -n +2 /francislab/data1/raw/20210428-EV/metadata.csv | awk 'BEGIN{FS="\042"}{print $2}' ) > ${GECKO}/EV_IDHWT_metadata.conf

vi ${GECKO}/EV_IDHWT_metadata.conf

awk 'BEGIN{FS=OFS="\t"}{print $1,NR,$2}' ${GECKO}/EV_IDHWT_metadata.conf > tmp
mv tmp ${GECKO}/EV_IDHWT_metadata.conf
```

metadata.conf is a IDH WT/nonWT file


Next time, try to submit these commands to the queue
If submitting works, could set some dependencies and make this a proper script.
Will it require specifying where to run given that none of this is in the PATH


date=$( date "+%Y%m%d%H%M%S" )


Use fastq 
Lexogen or Diagenode?
bbduk{1,2,3} or cutadapt{1,2,3}?

lte30 or gt30 or all ( try lte30 then do all )


```
cd ${GECKO}
mkdir EV_IDHWT
for f in /francislab/data1/working/20210428-EV/20210518-preprocessing/output/SFHH005*.cutadapt2.fastq.gz ; do
echo $f
base=$( basename $f .cutadapt2.fastq.gz )
ln -s $f EV_IDHWT/${base}.fastq.gz
done
```


Nextflow uses a Java VM so needs to be from a non-login dev node ( or as a job on the cluster )



```BASH
cd ${GECKO}/ImportMatrix
mkdir /francislab/data2/users/gwendt/github/ucsffrancislab/GECKO/ImportMatrix/ev_IDHWT_import
ln -s /francislab/data2/users/gwendt/github/ucsffrancislab/GECKO/ImportMatrix/ev_IDHWT_import
#/bin/rm -rf ev_IDHWT_import/* work/* results/*

${sbatch} --job-name=decomposition --time=9999 --ntasks=4 --mem=30G --output=${GDATA}/decomposition.${date}.txt ${GECKO}/ImportMatrix/main.pl decomposition --singleEnd --outdir ${GDATA} --reads '/c4/home/gwendt/github/ucsffrancislab/GECKO/EV_IDHWT/*.fastq.gz' --kmersize ${k}
```

Can take quite a while depending on file size. 

Then cleanup.


```
rm work/* -rf
```



```
cd ${GECKO}/ImportMatrix
find ev_IDHWT_import -type f -exec chmod a-w {} \;

${sbatch} --job-name=importation --time=9999 --ntasks=4 --mem=30G --output=${GDATA}/importation.${date}.txt ${GECKO}/ImportMatrix/main.pl importation --groupconfig ${GECKO}/EV_IDHWT_metadata.conf --outdir ${GDATA}

${sbatch} --job-name=discretization --time=9999 --ntasks=4 --mem=30G --output=${GDATA}/discretization.${date}.txt ${GECKO}/ImportMatrix/main.pl discretization --matrix ${GDATA}/rawimport/matrix/RAWmatrix.matrix –-outdir ${GDATA}
```

The above does not put the output in `ev_IDHWT_import`.
Initially it had `-outdir` and not `--outdir`, which was incorrect, but not the problem.
Not sure what is special about this step.

This next step (filter) can take days depending on data size so ALWAYS run with nohup (or submit to queue).


```
mv ${GECKO}/ImportMatrix/results/discretization ${GDATA}/

${sbatch} --job-name=filter --time=9999 --ntasks=8 --mem=61G --output=${GDATA}/filter.${date}.txt ${GECKO}/ImportMatrix/main.pl filter --matrix ${GDATA}/discretization/matrix/DISCRETmatrix.matrix --outdir ${GDATA}


${sbatch} --job-name=real --time=999 --ntasks=8 --mem=61G --output=${GDATA}/real.${date}.txt ${GECKO}/ImportMatrix/main.pl real --matrixDiscrete ${GDATA}/filtering/matrix/FILTEREDmatrix.matrix --matrixRaw ${GDATA}/rawimport/matrix/RAWmatrix.matrix --outdir ${GDATA}


cd ${GECKO}/Gecko/algoGen/Producteurv2/utils

${sbatch} --job-name=transformIntoBinary --time=999 --ntasks=8 --mem=61G --output=${GDATA}/transformIntoBinary.${date}.txt --wrap="${GECKO}/Gecko/algoGen/Producteurv2/utils/transformIntoBinary ${GDATA}/filtering/final/FILTEREDmatrix_RealCounts.matrix ${GDATA}/filtering/final/FILTEREDmatrix_RealCounts.bin"

mkdir ${GDATA}/filtering/final/CutMatrix/

${sbatch} --job-name=indexBinary --time=999 --ntasks=8 --mem=61G --output=${GDATA}/indexBinary.${date}.txt --wrap="${GECKO}/Gecko/algoGen/Producteurv2/utils/indexBinary ${GDATA}/filtering/final/FILTEREDmatrix_RealCounts.bin ${GDATA}/filtering/final/CutMatrix/example.bin 1000"
```




```
cd ${GECKO}/Gecko/algoGen

/bin/rm -rf ../../Demo/DemoGeneticAlgResultDir/ slurm-*.out GECKO_*.* __pycache__/ plot_analysis_*.* 

find ${GDATA}/ -type f -exec chmod a-w {} \;


cd ${GECKO}/Gecko/algoGen

mkdir /francislab/data1/users/gwendt/github/ucsffrancislab/GECKO/EV_IDHWT
mkdir /francislab/data1/users/gwendt/github/ucsffrancislab/GECKO/EV_IDHWT/GeneticAlgResultDir
ln -s /francislab/data1/users/gwendt/github/ucsffrancislab/GECKO/EV_IDHWT/GeneticAlgResultDir ${GECKO}/EV_IDHWT/
${sbatch} --job-name=multipleGeckoStart --time=9999 --ntasks=4 --mem=30G --output=${GECKO}/EV_IDHWT/GeneticAlgResultDir/multipleGeckoStart.${date}.txt ${GECKO}/Gecko/algoGen/multipleGeckoStart.py ${GECKO}/EV_IDHWT/GA.conf ${k}

```

Job is submitted. Waiting ...




If running many, wrap it in a loop.

```


cd ${GECKO}/Gecko/algoGen

date=$( date "+%Y%m%d%H%M%S" )
#for i in $( seq 0 9 ) ; do

i=5
mkdir /francislab/data1/users/gwendt/github/ucsffrancislab/GECKO/EV_IDHWT/GeneticAlgResult${i}Dir
ln -s /francislab/data1/users/gwendt/github/ucsffrancislab/GECKO/EV_IDHWT/GeneticAlgResult${i}Dir ${GECKO}/EV_IDHWT/

cp ${GECKO}/EV_IDHWT/GA.conf ${GECKO}/EV_IDHWT/GA${i}.conf
vi ${GECKO}/EV_IDHWT/GA${i}.conf

${sbatch} --job-name=${i}multipleGeckoStart --time=999 --ntasks=4 --mem=30G --output=${GECKO}/EV_IDHWT/GeneticAlgResult${i}Dir/multipleGeckoStart.${date}.txt ${GECKO}/Gecko/algoGen/multipleGeckoStart.py ${GECKO}/EV_IDHWT/GA${i}.conf ${k}

#done

```



```
tar --directory /francislab/data1/users/gwendt/github/ucsffrancislab/GECKO/ cvf - /francislab/data1/users/gwendt/github/ucsffrancislab/GECKO/EV_IDHWT | gzip > 20210615-EV_IDHWT-Gecko.tar.gz

tar -cv --directory /francislab/data1/users/gwendt/github/ucsffrancislab/GECKO/ -f - EV_IDHWT | gzip > 20210615-EV_IDHWT-Gecko.tar.gz



tar -cv --directory /francislab/data1/users/gwendt/github/ucsffrancislab/GECKO/ -f - EV_IDHWT/GeneticAlgResult4Dir | gzip > 20210616-EV_IDHWT-Gecko.tar.gz

BOX="https://dav.box.com/dav/Francis _Lab_Share"

curl -netrc -T 20210616-EV_IDHWT-Gecko.tar.gz "${BOX}/"
```
