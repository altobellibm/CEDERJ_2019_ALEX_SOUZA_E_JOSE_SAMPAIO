#include <iostream>
#include <chrono>

#undef NDEBUG
#include <assert.h>

#include <fstream>

#define EIGEN_RUNTIME_NO_MALLOC // Define this symbol to enable runtime tests for allocations


#include "dual_scaling.h"
#include "leitorbasenumerica.h"


typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;

const double LIMITE_INFERIOR_DELTA = 1e-8;
typedef double real_type;
typedef double index;

typedef Eigen::Matrix<real_type, Eigen::Dynamic, Eigen::Dynamic>	dynamic_size_matrix;
typedef Eigen::Matrix<real_type, Eigen::Dynamic, 1>					column_vector_type;
typedef Eigen::Matrix<real_type, 1, Eigen::Dynamic>					row_vector_type;

typedef ds::multiple_choice_data<dynamic_size_matrix>               mcd;

BASE_NUM getMatrix(std::string& _strFile){

   BASE_NUM base = BASE_NUM();
   if(!LeitorBaseNumerico::obterDadoArquivo(_strFile, base)){
       std::cout << "Erro read file!" << std::endl;
       return base;
   }
   return base;
}


int main(int argc, char* argv[]){

    if(argc != 2)
        return 1;

    std::string strFile = argv[1];
    std::string strPrefixFile = strFile.substr(strFile.find_last_of("/")+1,strFile.find("."));

    //dualscaling atributes dados multiplas escolhas
    dynamic_size_matrix x_normed, x_projected;
    dynamic_size_matrix y_normed, y_projected;
    row_vector_type rho, delta, fc;
    column_vector_type fr;

    auto t0 = Time::now();
     auto dado = getMatrix(strFile);
     auto matriz = mcd(dado.getMatrix());
    auto t1 = Time::now();

    fsec fs = t1 -t0;


    //contigency table
    std::cout << "started Dual Scaling " << endl;
    t0 = Time::now();
      ds::dual_scaling(matriz, x_normed, y_normed, x_projected, y_projected, rho, delta, fc, fr, LIMITE_INFERIOR_DELTA);
    t1 = Time::now();

    fsec calculo = t1 -t0;

    std::ofstream myfile;
      myfile.open (strPrefixFile + "_multiple_choice_tempoProcessamentoCPU.txt");
      myfile << "Nome arquivo: " << strFile << std::endl;
      myfile << "Matriz formato: [" << dado.m_transacoes.size() << "," << dado.m_cabecalho.size() << std::endl;
      myfile << "Tempo criar matriz entrada (s):" << fs.count() << std::endl;
      myfile << "Tempo calcular em CPU (s):" << calculo.count() << std::endl;
    myfile.close();

    myfile.open(strPrefixFile + "_Xprojected.txt");
        myfile << x_projected;
    myfile.close();

    myfile.open(strPrefixFile + "_Yprojected.txt");
        myfile << y_projected;
    myfile.close();

    myfile.open(strPrefixFile + "_fc.txt");
        myfile << fc;
    myfile.close();

    myfile.open(strPrefixFile + "_fr.txt");
        myfile << fr;
    myfile.close();

    myfile.open(strPrefixFile + "_rho.txt");
        myfile << rho;
    myfile.close();

    myfile.open(strPrefixFile + "_delta.txt");
        myfile << delta;
    myfile.close();

    myfile.open(strPrefixFile + "_dicionario.txt");
        for (auto it = dado.m_cabecalhoIdReal.begin(); it != dado.m_cabecalhoIdReal.end(); it++)
            myfile << it->first << " " << it->second << std::endl;
    myfile.close();

    std::cout << "Finished Dual Scaling" << std::endl;

    return 0;
}
