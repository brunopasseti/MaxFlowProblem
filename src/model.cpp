#pragma once
#include <model.h>

Model::Model(Data data){
    this->_data = data;
    this->_env = IloEnv();
    this->_model = IloModel(this->_env);

    // Prepare data adapting info from PFCM to PFM
    initData();
    addVariables();
    addObjFunction();
    addConstraints(); 
    initCplexFromModel();
}

void Model::addVariables(){
    x = IloArray< IloNumVarArray >(this->_env, _data.n);
    for(int i = 0; i < _data.n; i++) {
        IloNumVarArray vetor_i(this->_env, _data.n, 0, IloInfinity, ILOINT);
        this->x[i] = (vetor_i);
    }
    for(int i = 0; i < _data.n; i++) {
        for(int j = 0; j < _data.n; j++) {
            try {
                x[i][j].setName(std::string("x" + std::to_string(i) + "_" + std::to_string(j)).c_str());
            } catch(...) {
                std::cout << "DEU RUIM AQUI NO NOME" << std::endl;
            }
        }
    }

    for(int i = 0; i < _data.n; i++) {
        this->x[i][i].setUB(0);
    }
}
void Model::addObjFunction(){
    IloExpr FO(this->_env);
    for(int i = 0; i < _data.n; i++) {
        for(int j = 0; j < _data.n; j++) {

            if(i == j) continue;

            FO += (this->x[i][j] * this->c[i][j]);
        }
    }
    this->_model.add(IloMinimize(this->_env, FO));
}
void Model::addConstraints(){
    // (1) conservação de fluxo
    for(int i = 0; i < _data.n; i++){
        IloExpr fluxEntrada(this->_env);
        IloExpr fluxSaida(this->_env);
        for(int j = 0; j < _data.n; j++){
            // fluxSum += (this->x[i][j] - this->x[j][i]);
            fluxEntrada += x[j][i];
            fluxSaida += x[i][j];
        }
        IloConstraint c1 = (fluxSaida - fluxEntrada == this->d[i]);
        char c_name[32];
        sprintf(c_name, "c_1(%d)", i);
        c1.setName((char*) c_name);
        _model.add(c1);
        fluxEntrada.end();
        fluxSaida.end();
    }

    // (2) e (3) fluxo maximo
    for(int i = 0; i < _data.n; i++){
        for(int j = 0; j < _data.n; j++){
            if(i == j) continue;
            IloConstraint c2 = (x[i][j] <= _data.u[i][j]);
            IloConstraint c3 = (x[i][j] >= l[i][j]);
            char c_name[32];
            sprintf(c_name, "c_2(%d,%d)", i, j);
            c2.setName((char*) c_name);
            _model.add(c2);
            sprintf(c_name, "c_3(%d,%d)", i, j);
            c3.setName((char*) c_name);
            _model.add(c3);
        }

    }
}
void Model::initCplexFromModel(){
    this->_cplex = IloCplex(_model);
}
void Model::solve(){
    this->_cplex.exportModel("modelo.lp");
    try {
        this->_cplex.solve();
    } catch(IloException e) {
        std::cerr <<"Error" <<  ": " << e.getMessage() << std::endl;
    }
    std::cout << "status: " << this->_cplex.getStatus() << std::endl;
    for(int i = 0; i < _data.n; i++){
        for(int j = 0; j < _data.n; j++){
            if(i == j) continue;
            printf("%6.2f ", _cplex.getValue(x[i][j]));
        }
        puts("");
    }
}
void Model::initData(){
    l = std::vector<std::vector<int>>(_data.n, std::vector<int>(_data.n, 0));
    int F_barra = 0;
    for(int j = 0; j < _data.n; j++){
        F_barra += _data.u[_data.s][j];
    }
    printf("F_b: %d\n", F_barra);  
    // Primeiro, configure cij = 0 para todos os arcos existentes de modo a refletir a ausencia de custos no problema do fluxo maximo.
    c = std::vector<std::vector<int>>(_data.n, std::vector<int>(_data.n, 0));
    // Em segundo lugar, selecione uma quantidade F_barra, que e um limite superior seguro sobre o fluxo viavel maximo pela rede e depois
    // designa, respectivamente, uma oferta e uma demanda igual a ao no de suprimento e F_barra ao no de demanda. (Pelo fato de todos os 
    // demais nos serem nos de transbordo, eles automaticamente tem bi = 0.)
    d = std::vector<int>(_data.n, 0);
    d[_data.s] = F_barra;
    d[_data.t] = -F_barra;
    // Em terceiro lugar, adicionar um arco que vai diretamente do no de suprimento para o no de demanda e atribuir a ele um custo unitario
    // grande de cij = M, bem como uma capacidade de arco ilimitada (uij =  Infinity).
    c[_data.s][_data.t] = 1;
    _data.u[_data.s][_data.t] = std::numeric_limits<int>::max();
}
void Model::generateDot(){
    if(this->_cplex.getStatus() == IloAlgorithm::Status::Optimal){
        printf("digraph D {\n");
        for(int i = 0; i < _data.n; i++){
            if(i != _data.s && i != _data.t)
                printf("\t%d [shape=circle]\n", i+1);
            else if(i == _data.s)
                printf("\t%d [shape=triangle, color=green]\n", i+1);
            else if(i == _data.t)
                printf("\t%d [shape=triangle, color=red]\n", i+1);
        }
        for(int i = 0; i < _data.n; i++){
            for(int j = 0; j < _data.n; j++){
                if(i == j) continue;
                if(_cplex.getValue(x[i][j]) != 0)
                    printf("\t%i -> %i [label=%6.3f]\n", i+1, j+1, _cplex.getValue(x[i][j]));
            }
        }
        printf("}\n");
    }
}
