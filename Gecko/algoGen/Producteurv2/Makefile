install: farmer2

clean:
		rm farmer2
		rm sampledatamat
	

farmer2: ClassGeneticAlg.cpp
		g++ ${CPPFLAGS} ${LDFLAGS} -std=c++11 -lboost_system -static-libstdc++ -g -fopenmp -lm -lz -O3 ClassGeneticAlg.cpp -o farmer2
	        
		g++ ${CPPFLAGS} ${LDFLAGS} -std=c++11 -lboost_system -static-libstdc++ -g -fopenmp -Wall -O3 sampledatamat.cpp -o sampledatamat 

#g++ -std=c++0x -O3 ClassMatrixAccess.cpp -lm -lz 
