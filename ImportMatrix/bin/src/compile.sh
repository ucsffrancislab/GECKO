g++ ${CPPFLAGS} ${LDFLAGS} -std=c++0x -O3 -fopenmp -o indexingKmers IndexingKMers.cpp
g++ ${CPPFLAGS} ${LDFLAGS} -std=c++0x -O3 -fopenmp RemoveNonInformativeKmers.cpp -o NIK
g++ ${CPPFLAGS} ${LDFLAGS} -std=c++0x -O3 -fopenmp JoinMultiImportationFiles.cpp -o JoinKMers
g++ ${CPPFLAGS} ${LDFLAGS} -std=c++0x -O3 -fopenmp JoinUltraBigMatrix.cpp -o JoinKmersUltra
g++ ${CPPFLAGS} ${LDFLAGS} -std=c++0x -O3 -fopenmp transformIntoMLformat.cpp -o transformIntoML
g++ ${CPPFLAGS} ${LDFLAGS} -std=c++0x -O3 -fopenmp CutMatrixByClass.cpp -o CutMatrixByClass
g++ ${CPPFLAGS} ${LDFLAGS} -std=c++0x -O3 MLformatToBinary.cpp -lm -lz -o transformIntoBinary
g++ ${CPPFLAGS} ${LDFLAGS} -std=c++0x -O3 MLformatToBinaryRemoveGroup.cpp -lm -lz -o transformIntoBinaryRemoveGroup
make -f ./anova_makefile
mv anovaFilter indexingKmers NIK JoinKmersUltra JoinKMers transformIntoML transformIntoBinary transformIntoBinaryRemoveGroup CutMatrixByClass ../.
