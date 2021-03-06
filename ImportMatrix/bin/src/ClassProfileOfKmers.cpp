#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string.h>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>
#include <omp.h>
#include "ClassEntropy.cpp"
#include "ClassAmeva.cpp"


//!
//! Class to deal with jellyfish results
//! author : Aubin Thomas
//!
class ProfileOfKmers{
public:
  map < uint64_t, vector<long double> > _data; //kmers freq
  uint64_t _nb_samples; //number of data
  uint64_t _size_kmer; // length of kmers
  uint64_t _size_cut; // number of kmers in memory
  uint64_t _total_size; // number of possible kmers
  vector <string> _list_files; //path to files
  vector <string> _list_files_name; //corresponding name
  vector<string> _list_files_group; //corresponding group

  ProfileOfKmers(uint64_t pnbsample, uint64_t psizekmer, uint64_t psizecut){
    _nb_samples = pnbsample;
    _size_kmer = psizekmer;
    _size_cut = psizecut;
    _total_size = powl(4, _size_kmer);
  }

  //! \fn void flushMemory()
  //! \brief erase the content od _data section of the object
  //!
  void flushMemory(){
    while (!_data.empty()){
       _data.erase(_data.begin());
    }
  }

  //! \fn uint64_t getIndice(string kmer, int64_t rank=0)
  //! \brief transform a sequence into an integer value that corresponds to {A,C,T,G} in base-4 algebra
  //! \param kmer (string) like "ATTACCAGAA"
  //! \param rank (integer) the initial rank to consider the parameter kmer, used through iterations. optional, default to 0
  //! \return (uint64_t) indice that corresponds to sequence
  //!
  uint64_t getIndice(string kmer, int64_t rank=0){
    if(kmer.size()==rank){
      return 0;
    }
    else{
      char character_c = kmer[kmer.size()-rank-1];
      if(character_c=='A'){
        return getIndice(kmer, rank+1)+0;
      }
      else{
        if(character_c=='C'){
          return getIndice(kmer, rank+1)+1*powl(4,rank);
        }
        else{
          if(character_c=='T'){
            return getIndice(kmer, rank+1)+2*powl(4,rank);
          }
          else{
            return getIndice(kmer, rank+1)+3*powl(4,rank);
          }
        }
      }
    }
  }

  //! \fn bool addFile(string pathfile, string namefile, string groupfile)
  //! \brief add file to be considered
  //! \param pathfile (string) path to a fasta file generated by JellyFish
  //! \param namefile (string) the name of the file (for output)
  //! \param groupfile (string) the group
  //! \return output : boolean (True in case of success)
  //!
  bool addFile(string pathfile, string namefile, string groupfile){
      if(_list_files.size()<_nb_samples){
        _list_files.push_back(pathfile);
        _list_files_name.push_back(namefile);
        _list_files_group.push_back(groupfile);
        return true;
      }
      else{
          return false;
      }
  }

  //! \fn bool addListOfFiles(string pathListOfFiles)
  //! \brief add file to object to be considered
  //! \param pathListOfFiles (string) file that lists several files in 3 columns path#name#group format
  //! \return (boolean) True in case of success, False otherwise
  //!
  bool addListOfFiles(string pathListOfFiles){
    ifstream file_ptr(pathListOfFiles.c_str(), ios::in);
    string path_c, name_c, group_c;
    bool state = true;
    while(file_ptr >> path_c >> name_c >> group_c){
      state = (state) and (addFile(path_c, name_c, group_c));
    }
    return state;
  }

  //! \fn void getAleaData()
  //! \brief pick random subset of kmers from the first file and output a matrix
  //!        by importing other files
  //!
  void getAleaData(){
      if(_list_files.size()>=1){
        string seq_read;
        uint32_t freq;
        uint32_t counter=0;
        //first open 1st file and get the total number of lines
        ifstream file_ptr(_list_files[0].c_str(), ios::in);
        if(!file_ptr){
          cerr << "getAleaData : Error while opening " << _list_files[0] << endl;
          exit(789);
        }
        while(file_ptr >> seq_read >> freq){
          counter++;
        }
        file_ptr.close();
        if(counter<_size_cut){
          cerr << "WARNING lower size of sample to file length" << endl;
          _size_cut = counter;
        }
        //establish probabilities to select lines
        srand(time(NULL));
        uint32_t rand_cutoff = (uint32_t) ((long double)((long double) _size_cut/(long double) counter)*1000000);
        cerr << "rand_cutoff=" << rand_cutoff << endl;
        //open the file, and get randomly some kmers
        uint32_t testl = 0;
        uint64_t effectiveSize = 0;
        ifstream file_ptr2(_list_files[0].c_str(), ios::in);
        while(file_ptr2 >> seq_read >> freq){
          testl++;
          int64_t aleaDecision = rand() % 1000000;
          if((effectiveSize<_size_cut) and (aleaDecision<=rand_cutoff)){
            effectiveSize++;
            uint64_t indice_read = getIndice(seq_read);
            _data[indice_read] = vector<long double>(_nb_samples,0);
            _data[indice_read][0] = freq;
          }
        }
        file_ptr2.close();
        //open the file, and get randomly some kmers if _size_cut is not obtained
        while(effectiveSize<_size_cut){
          ifstream file_ptr3(_list_files[0].c_str(), ios::in);
          while(file_ptr3 >> seq_read >> freq){
            int64_t aleaDecision = rand() % 1000000;
            if((effectiveSize<_size_cut) and (aleaDecision<=rand_cutoff)){
              uint64_t indice_read = getIndice(seq_read);
              if(_data.find(indice_read)!=_data.end()){
                effectiveSize++;
                _data[indice_read] = vector<long double>(_nb_samples,0);
                _data[indice_read][0] = freq;
              }
            }
          }
          file_ptr3.close();
        }
        //for each other file, search for kmer
        for(int64_t file_counter = 1; file_counter<_list_files.size(); file_counter++){
          cerr << _list_files[file_counter] << endl;
          ifstream file_ptr4(_list_files[file_counter].c_str(), ios::in);
          while(file_ptr4 >> seq_read >> freq){
            uint64_t indice_read = getIndice(seq_read);
            if(_data.find(indice_read)!=_data.end()){
              _data[indice_read][file_counter] = freq;
              //cerr << file_counter << ":\t" << indice_read << endl;
            }
          }
          file_ptr4.close();
        }
      }
  }

  //! \fn bool getMatrix(string outputfile)
  //! \brief output _data in tabulated texte file
  //! \param outputfile (string) the output of matrix file
  //! \return (boolean) True in case of success, False otherwise
  //! DEPRECATED
  bool getMatrix(string outputfile){
    ofstream ofs(outputfile.c_str(),ios::out);
    if(ofs){
      ofs << "kmer";
      for(uint64_t j=0; j<_list_files.size(); j++){
        ofs << "\t" << _list_files[j];
      }
      ofs << endl;
      for (map < uint64_t, vector<long double> >::iterator it=_data.begin(); it!=_data.end(); ++it){
        ofs << it->first;
        for(uint64_t j=0; j<_list_files.size(); j++){
          ofs << "\t" << _data[it->first][j];
        }
        ofs << endl;
      }
      return true;
    }
    else{
      return false;
    }
  }

  //! \fn void getMassiveData(long double pthreshold = 0, string outputfile="", bool normalization = true)
  //! \brief takes all the kmers of all the files previously selected and makes a matrix
  //! \param pthreshold (long double) threshold to consider value as true signal
  //! \param outputfile (string) file of the matrix
  //! \param normalization (boolean) apply normalization by millions of kmers
  void getMassiveData(long double pthreshold = 0, string outputfile="", bool normalization = true){
    if(_list_files.size()>=1){
      _size_cut = 0;
      string seq_read;
      uint32_t freq;
      uint32_t counter=0;
      vector<uint64_t> listIDs;


      /// for each file
      for(int64_t file_counter = 0; file_counter<_list_files.size(); file_counter++){
        cerr << _list_files[file_counter] << "\t";

        ///loading data in memory
        ifstream file_ptr(_list_files[file_counter].c_str(), ios::in);
        if(!file_ptr){
          exit(12);
        }
        long double totalsum = 0; //used for normalization
        while(file_ptr >> seq_read >> freq){
              totalsum+=freq;
              if(freq > pthreshold){
                  uint64_t indice_read = getIndice(seq_read);
                  _data[indice_read] = vector<long double>(1,freq);
              }
        }
        totalsum/=1000000; //normalization by millions of kmers

        ///then open file to compare the kmers and write the join result into tmp file
        string outputfile_tmp = outputfile+"_str";
        ofstream file_oao(outputfile_tmp.c_str(), ios::out);

        string line;
        if(file_counter>0)
        {
          ifstream file_oai(outputfile.c_str(), ios::in);
          getline(file_oai, line);
          //headers
          for(uint64_t j = 0; j <= file_counter; j++){
            file_oao << "\t" << _list_files_name[j];
          }
          file_oao << endl;

          getline(file_oai, line);
          file_oao << "group";
          for(uint64_t j = 0; j <= file_counter; j++){
            file_oao << "\t" << _list_files_group[j];
          }
          file_oao << endl;

          while (getline(file_oai, line)){
              vector<string> content;
              boost::split(content,line,boost::is_any_of("\t"));
              uint64_t indice_c = strtoull(content[0].c_str(), NULL, 0);

              if(_data.find(indice_c)!=_data.end()){
                // the kmer has already been found
                file_oao << content[0];
                for(uint16_t i=1; i<content.size(); i++){
                  file_oao << "\t" << content[i];
                }
                if(!normalization){
                  file_oao << "\t" << _data[indice_c][0] << endl;
                }
                else{
                  file_oao << "\t" << _data[indice_c][0]/totalsum << endl;
                }
                _data.erase(indice_c); //delete the corresponding kmer from memory
              }
              else{
                // the kmer has not been found
                file_oao << content[0];
                for(uint16_t i=1; i<content.size(); i++){
                  file_oao << "\t" << content[i];
                }
                file_oao << "\t" << 0 << endl;
              }
          }
          file_oai.close();
        }
        else{
            file_oao << "\t" << _list_files_name[0] << endl;
            file_oao << "group";
            file_oao << "\t" << _list_files_group[0] << endl;
        }

        //add of new kmers
        for (map < uint64_t, vector<long double> >::iterator it=_data.begin(); it!=_data.end(); ++it){
          file_oao << it->first;
          for(uint64_t j=0; j<file_counter; j++){
            file_oao << "\t" << 0;
          }
          if(!normalization){
            file_oao << "\t" << _data[it->first][0] << endl;
          }
          else{
            file_oao << "\t" << _data[it->first][0]/totalsum << endl;
          }
        }
        file_oao.close();
        flushMemory();
        //move files
        int64_t resmv = rename(outputfile_tmp.c_str(), outputfile.c_str());
      } //for filecounter
    } //if _list_files
  } //getMassiveData

  //! \fn void discretizationMDL(vector <string> pgroups, string pInpFile, string pResFile, uint64_t psizeBuffer, bool pdebug)
  //! \brief from a matrix file, computes the discretization according MDL method
  //! \param pInpFile
  //! \param pResFile
  //! \param psizeBuffer is the size of the table to be parallelized (OpenMP)
  //! \param pdebug
  //!
  void discretizationMDL(string pInpFile, string pResFile, uint64_t psizeBuffer, bool pdebug){
    ifstream file_ai(pInpFile.c_str(), ios::in); //input matrix
    ofstream file_ao(pResFile.c_str(), ios::out); //output matrix
    string line;
    vector <string> buffer(psizeBuffer,""); //table of lines to be treated
    uint64_t sizeBuffer_c = 0;

    if(file_ai){
      string lineheader, linegroups;
      getline(file_ai, lineheader);
      getline(file_ai, linegroups);
      file_ao << lineheader << endl << linegroups << endl;

      while (getline(file_ai, line)){
          buffer[sizeBuffer_c] = line;
          sizeBuffer_c++;
          if(sizeBuffer_c==psizeBuffer){
            //bunch of psizeBuffer lines will be treated
            #pragma omp parallel for
            for(uint64_t vic = 0; vic < sizeBuffer_c; vic++){
              EntropyDecomposition ED = EntropyDecomposition(_nb_samples);
              vector<string> content;
              boost::split(content, buffer[vic], boost::is_any_of("\t"));
              string indice_c = content[0];

              vector<long double> dataset_value;
              for(uint32_t i =1; i<content.size(); i++){
                try{
                    dataset_value.push_back(stod(content[i]));
                }
                catch(exception const& e){
                    //cerr << "ERROR1 in discretizationMDL : " << e.what() << "\twith " << content[i] << " at line " << cptline << endl;
                    dataset_value.push_back(0.0);
                }

              }
              vector<string> result = ED.Discretization(dataset_value, _list_files_group, pdebug);
              buffer[vic] = indice_c;
              for(uint32_t ir =0; ir<result.size(); ir++){
                buffer[vic]+="\t"+result[ir];
              }
            }
            for(uint64_t vic = 0; vic < sizeBuffer_c; vic++){
              file_ao << buffer[vic] << endl;
            }
            sizeBuffer_c = 0;
          }
      }
      #pragma omp parallel for
      for(uint64_t vic = 0; vic < sizeBuffer_c; vic++){
        EntropyDecomposition ED = EntropyDecomposition(100);
        vector<string> content;
        boost::split(content, buffer[vic], boost::is_any_of("\t"));
        string indice_c = content[0];
        vector<long double> dataset_value;
        for(uint32_t i =1; i<content.size(); i++){
          dataset_value.push_back(stod(content[i]));
        }
        vector<string> result = ED.Discretization(dataset_value, _list_files_group, pdebug);
        buffer[vic] = indice_c;
        for(uint32_t ir =0; ir<result.size(); ir++){
          buffer[vic]+="\t"+result[ir];
        }
      }
      for(uint64_t vic = 0; vic < sizeBuffer_c; vic++){
        file_ao << buffer[vic] << endl;
      }
      sizeBuffer_c = 0;
    }
    file_ai.close();
    file_ao.close();
  }


  //! \fn void discretizationAmeva(vector <string> pgroups, string pInpFile, string pResFile, uint64_t psizeBuffer, bool pdebug)
  //! \brief from a matrix file, computes the discretization according MDL method
  //! \param pInpFile
  //! \param pResFile
  //! \param psizeBuffer is the size of the table to be parallelized (OpenMP)
  //! \param pdebug
  //!
  void discretizationAmeva(string pInpFile, string pResFile, uint64_t psizeBuffer, bool pdebug){
    ifstream file_ai(pInpFile.c_str(), ios::in); //input matrix
    ofstream file_ao(pResFile.c_str(), ios::out); //output matrix
    string line;
    vector <string> buffer(psizeBuffer,""); //table of lines to be treated
    uint64_t sizeBuffer_c = 0;

    vector<string> list_files_group_categories;
    for(uint64_t i = 0; i < _list_files_group.size(); i++){
        string cat_c = _list_files_group[i];
        bool statecmp = false;
        for(uint64_t j = 0; j < list_files_group_categories.size(); j++){
            if(cat_c.compare(list_files_group_categories[j])==0){
                statecmp = true;
                j = list_files_group_categories.size();
            }
        }
        if(!statecmp){
            list_files_group_categories.push_back(cat_c);
        }
    }


    if(file_ai){
      string lineheader, linegroups;
      getline(file_ai, lineheader);
      getline(file_ai, linegroups);
      file_ao << lineheader << endl << linegroups << endl;

      while (getline(file_ai, line)){
          buffer[sizeBuffer_c] = line;
          sizeBuffer_c++;
          if(sizeBuffer_c==psizeBuffer){
            //bunch of psizeBuffer lines will be treated
            #pragma omp parallel for
            for(uint64_t vic = 0; vic < sizeBuffer_c; vic++){
              AmevaDecomposition ED = AmevaDecomposition(_nb_samples);
              vector<string> content;
              boost::split(content, buffer[vic], boost::is_any_of("\t"));
              string indice_c = content[0];

              vector<long double> dataset_value;
              for(uint32_t i =1; i<content.size(); i++){
                try{
                    dataset_value.push_back(stod(content[i]));
                }
                catch(exception const& e){
                    //cerr << "ERROR1 in discretizationMDL : " << e.what() << "\twith " << content[i] << " at line " << cptline << endl;
                    dataset_value.push_back(0.0);
                }

              }
              vector<string> result = ED.Discretization(dataset_value, _list_files_group, list_files_group_categories, pdebug);
              buffer[vic] = indice_c;
              for(uint32_t ir =0; ir<result.size(); ir++){
                buffer[vic]+="\t"+result[ir];
              }
            }
            for(uint64_t vic = 0; vic < sizeBuffer_c; vic++){
              file_ao << buffer[vic] << endl;
            }
            sizeBuffer_c = 0;
          }
      }
      #pragma omp parallel for
      for(uint64_t vic = 0; vic < sizeBuffer_c; vic++){
        EntropyDecomposition ED = EntropyDecomposition(100);
        vector<string> content;
        boost::split(content, buffer[vic], boost::is_any_of("\t"));
        string indice_c = content[0];
        vector<long double> dataset_value;
        for(uint32_t i =1; i<content.size(); i++){
          dataset_value.push_back(stod(content[i]));
        }
        vector<string> result = ED.Discretization(dataset_value, _list_files_group, pdebug);
        buffer[vic] = indice_c;
        for(uint32_t ir =0; ir<result.size(); ir++){
          buffer[vic]+="\t"+result[ir];
        }
      }
      for(uint64_t vic = 0; vic < sizeBuffer_c; vic++){
        file_ao << buffer[vic] << endl;
      }
      sizeBuffer_c = 0;
    }
    file_ai.close();
    file_ao.close();
  }





  //! \fn void RemoveNonInformativeKmers(string pInpFile, string pResFile, long double pSymmetricUncertaintyThreshold, bool pdebug){
  //! \brief
  //! \param pInpFile
  //! \param pResFile
  //! \param pSymmetricUncertaintyThreshold
  //! \param pdebug
  //!
  void RemoveNonInformativeKmers(string pInpFile, string pResFile, long double pSymmetricUncertaintyThreshold, bool pdebug){
    ifstream file_ai(pInpFile.c_str(), ios::in);
    string line;
    string header1, header2;
    //first get only the kmers that give information (at least 1 level of discretization) & creating workspace
    vector<string> data;
    vector<bool> dataindic;
    uint64_t numKMers = 0;
    if(file_ai){
      getline(file_ai, header1);
      getline(file_ai, header2);
      while (getline(file_ai, line)){
        vector<string> content;
        boost::split(content, line, boost::is_any_of("\t"));
        for(uint32_t ci =1; ci<content.size(); ci++){
          if(content[ci].compare("0")!=0){
            numKMers++;
            ci=content.size();
            data.push_back(line);
            dataindic.push_back(true);
          }
        }
      }
      file_ai.close();
    }
    else{
      exit(773);
    }
    cerr << numKMers << " kmers passed the class filter" << endl;

    //looks if kmers are informative
    EntropyDecomposition ED = EntropyDecomposition(_nb_samples);
    #pragma omp parallel for
    for(uint64_t li = 0; li < numKMers; li++){
      //cerr << li << endl;
      uint64_t lic = li+1;
      vector<string> content1;
      boost::split(content1, data[li], boost::is_any_of("\t"));
      while(lic<numKMers){
        vector<string> content2;
        boost::split(content2, data[lic], boost::is_any_of("\t"));
        if(ED.SymmetricUncertainty(content1, content2, false)>pSymmetricUncertaintyThreshold){
          dataindic[li] = false;
          lic = numKMers;
        }
        lic++;
      }
    }

    //output the result
    ofstream file_ao(pResFile.c_str(), ios::out);
    file_ao << header1 << endl << header2 << endl;
    uint64_t kw =0;
    for(uint64_t li = 0; li < numKMers; li++){
      if(dataindic[li]){
        file_ao << data[li] << endl;
        kw++;
      }
    }
    file_ao.close();
    cerr << kw << " kmers finally written" << endl;
  }





  //! \fn void discretizationWithR(vector <string> pgroups, string pInpFile, string pResFile, uint64_t psizeBuffer, bool pdebug)
  //! \brief from a matrix file, computes the discretization according MDL method
  //! \param pInpFile
  //! \param pResFile
  //! \param psizeBuffer is the size of the table to be parallelized (OpenMP)
  //! \param pdebug
  //!
  void discretizationWithR(string pInpFile, string pResFile, uint64_t psizeBuffer, bool pdebug){
    ifstream file_ai(pInpFile.c_str(), ios::in); //input matrix
    ofstream file_ao(pResFile.c_str(), ios::out); //output matrix

    string line;
    vector <string> buffer(psizeBuffer,""); //table of lines to be treated
    uint64_t sizeBuffer_c = 0;



    if(file_ai and file_ao){

      if(pdebug) cout << "file opening OK" << endl;
      string acceptes="abcdefghijklmnopqrstuvwyz";
      acceptes+="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
      acceptes+="0123456789";
      string lineheader, linegroups; //first line : the names
      getline(file_ai, lineheader); //2nd line : the groups
      getline(file_ai, linegroups);
      file_ao << lineheader << endl << linegroups << endl;
      if(pdebug) cout << "lineheader = " << lineheader << endl << "linegroups = " << linegroups << endl;

      // Creation id aleatoire pour fichiers temporaires
      string randomKeychain="";
      srand(time(0));
      size_t randtaille = (rand()%12)+6;
      for(size_t randi=0; randi<randtaille; randi++)
      {
          size_t posrand = rand()%62;
          randomKeychain+=acceptes[posrand];
      }
      if(pdebug) cout << "randomKeyChain = " << randomKeychain << endl;
      string randomInputFile = "tmp_input_"+randomKeychain;
      string randomOutputFile = "tmp_output_"+randomKeychain;
      uint64_t dimTableLigne = _list_files_group.size(); // le nombre de ligne (en colonne on met les psizeBuffer kmers)


      while (getline(file_ai, line)){
        buffer[sizeBuffer_c] = line;
        sizeBuffer_c++;

        if(sizeBuffer_c==psizeBuffer){
          //a bunch of sizeBuffer_c lines will be treated

          // Creation structure memoire
          vector< vector<long double> > resultTable (dimTableLigne, vector<long double> (sizeBuffer_c,0.0));
          //long double resultTable[dimTableLigne][sizeBuffer_c]; // va contenir les comptages
          string resultTableIds[sizeBuffer_c]; // va contenir les KMers ID
          if(pdebug){
            cout << "dimTableLigne = " << dimTableLigne << endl;
            cout << "sizeBuffer_c = " << sizeBuffer_c << endl;
          }
          for(uint64_t vic = 0; vic < sizeBuffer_c; vic++){
            vector<string> content;
            boost::split(content, buffer[vic], boost::is_any_of("\t"));
            string indice_c = content[0];
            resultTableIds[vic] = indice_c;

            for(uint64_t i =1; i<content.size(); i++){
              try{
                resultTable[i-1][vic] = stold(content[i]);
              }
              catch(exception const& e){
                  cerr << "ERROR in discretizationWithR : " << e.what() << "\twith " << content[i] << endl;
                  resultTable[i-1][vic] = 0.0;
              }
            }
          }

          // Remplissage fichier temporaire (aot) dans un format importable en R
          ofstream file_aot(randomInputFile.c_str(), ios::out); //as output temporary
          if(file_aot){
            file_aot << "KMer" << resultTableIds[0];
            for(uint64_t icolumn = 1; icolumn < sizeBuffer_c; icolumn++){
              file_aot << "\tKMer" << resultTableIds[icolumn];
            }
            file_aot << "\tgroups" << endl;
            for(uint64_t iline = 0; iline < dimTableLigne ; iline++){
              file_aot << resultTable[iline][0];
              for(uint64_t icolumn = 1; icolumn < sizeBuffer_c; icolumn++){
                file_aot << "\t" << resultTable[iline][icolumn];
              }
              file_aot << "\tgroup_" << _list_files_group[iline] << endl;
            }
            file_aot.close();
          } //if(file_aot)
          else{
            cerr << "Error while opening " << randomInputFile << endl;
            exit(27);
          }


          // appel au script R pour la discretisation
          string command = "Rscript script_R_discretization.r "+randomInputFile+" "+randomOutputFile;
          if(pdebug) cout << "command : " << command << endl;
          int repint = system(command.c_str());


          // import du resultat et rangement dans buffer
          ifstream file_ait(randomOutputFile.c_str(), ios::in); //as input temporary
          if(file_ait){
            getline(file_ait, line); //on saute la ligne des ID de Kmers
            for(uint64_t iline = 0; iline < dimTableLigne ; iline++){
              vector<string> content;
              getline(file_ait, line);
              boost::split(content, line, boost::is_any_of("\t"));
              for(uint64_t icolumn = 0; icolumn < content.size()-1; icolumn++){
                resultTable[iline][icolumn] = stold(content[icolumn]);
              }
            }
            file_ait.close();
          }
          else{
            cerr << "Error while opening " << randomOutputFile << endl;
            exit(27);
          }

          // ecriture du buffer
          for(uint64_t i = 0; i < sizeBuffer_c; i++){
            file_ao << resultTableIds[i];
            for(uint64_t j = 0; j < dimTableLigne ; j++){
              file_ao << "\t" << resultTable[j][i];
            }
            file_ao << endl;
          }

          sizeBuffer_c = 0;

        } //if(sizeBuffer_c==psizeBuffer)


      } //while (getline(file_ai, line))
      // the bunch of last sizeBuffer_c lines will be treated




    } //if(file_ai and file_ao)
    else{
      cerr << "Error while opening " << pInpFile << " and "  << pResFile << endl;
      exit(54);
    }

  } //discretizationWithR







}; //class