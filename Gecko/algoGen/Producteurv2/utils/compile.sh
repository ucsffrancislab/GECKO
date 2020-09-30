g++ ${CPPFLAGS} ${LDFLAGS} -std=c++11 -lboost_system -static-libstdc++ -g -lm -lz -O3 IndexBinary.cpp -o indexBinary
g++ ${CPPFLAGS} ${LDFLAGS} -std=c++11 -lboost_system -static-libstdc++ -g -lm -lz -O3 MLformatToBinary.cpp -o transformIntoBinary
