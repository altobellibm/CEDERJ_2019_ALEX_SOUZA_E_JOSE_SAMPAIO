#include <iostream>
#include <chrono>

#undef NDEBUG
#include <assert.h>

#define EIGEN_RUNTIME_NO_MALLOC // Define this symbol to enable runtime tests for allocations

#include "leitorbasenumerica.h"

vector<string> split( string s, char c = ' '){
    vector<string> v;

    istringstream iss(s);
    string token;

    while (getline(iss, token, c))
        v.push_back(token);

    return v;
}

vector<int> splitInt( string s, char c = ' '){
    vector<int> v;
    vector<string> resul = split(s,c);

    for(auto item : resul)
        v.push_back(atoi(item.c_str()));

    return v;
}

string vecto_to_string( vector<int> v, char c = ' '){
    string str_v;
    for(auto item : v)
        str_v = str_v + to_string(item) + ",";
    str_v.pop_back();

    return str_v;
}

int frequencia(vector<int>& item, Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>& matrix, int nTransactions ){
    int freq = 0;
    for (int linha = 0; linha < nTransactions; linha++){
        bool bcontem = true;
        for (auto id : item) {
            if (matrix(linha, id -1) != 1)
                bcontem = false;
        }
        if (bcontem)
            freq++;
    }
    return freq;
}

void criarNovoArquivoRegras(string _arquivo_regras, BASE_NUM& base, string file_out){

    auto matrix = base.getMatrix();

    std::ofstream m_fileOutput(file_out);

    std::ifstream myfile(_arquivo_regras);

    if (! myfile.is_open()) {
        std::cout << "Nao abriu o arquivo!" << std::endl;
        return;
    }

    int posi = 0;
    std::string line;
    while (getline(myfile, line)){
        auto pos = line.find("==>");
        string strAntecedente = line.substr(0,pos-1);
        auto antecedente = splitInt(strAntecedente);
        //sortedOriginalOrder(antecedente);

        auto posfim = line.find("#SUP:");
        string strConsequente = line.substr(pos+4, posfim - (pos+5));
        auto consequente = splitInt(strConsequente);
        //sortedOriginalOrder(consequente);

        pos = posfim+5;
        posfim = line.find("#CONF:");
        string suporte = line.substr(pos+1,posfim - (pos + 2));
        int sup = atoi(suporte.c_str());
        pos = posfim + 5;
        string confianca = line.substr(pos+2);

        int freqA = frequencia(antecedente,matrix,base.getSizeTransation());
        int freqC = frequencia(consequente,matrix,base.getSizeTransation());

         m_fileOutput << vecto_to_string(antecedente) + " " + vecto_to_string(consequente) + " " + std::to_string((float)sup/(float)base.getSizeTransation()) + " "
                         + confianca  + " " + std::to_string((float)freqA/(float)base.getSizeTransation()) + " " + std::to_string((float)freqC/(float)base.getSizeTransation()) << std::endl;

         if (posi % 1000 == 0)

                std::cout << "Regra = " << posi++ << std::endl;
    }

    myfile.close();

}


int main(int argc, char* argv[]){

    if (argc < 2){
        std::cout << "parameter not found." << std::endl;
        return 1;
    }

    std::string strFile = argv[1];
    std::string strFileOut = argv[3];
    std::string strFileregra = argv[2];

    std::cout << "file = " << strFile << std::endl;
    std::cout << "arquivo regras saida" << strFileOut << std::endl;
    std::cout << "arquivo regras " << strFileregra << std::endl;


    BASE_NUM base = BASE_NUM();
    if(!LeitorBaseNumerico::obterDadoArquivo(strFile, base)){
        std::cout << "Erro read file!" << std::endl;
        return 1;
    }

   std::cout << "leu file" <<std::endl;
   criarNovoArquivoRegras(strFileregra, base, strFileOut);


    return 0;
}
