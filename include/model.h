#pragma once
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <ilcplex/ilocplex.h> 
#include <data.h>

struct Model{
    std::vector<std::vector<int>> l, c;
    std::vector<int> d;

    IloArray<IloNumVarArray> x;
    
    IloModel _model;
    IloCplex _cplex;
    IloEnv _env;
    Data _data;

    Model(Data data);
    void solve();
    void generateDot();
private:
    void addObjFunction();
    void addVariables();
    void addConstraints(); 
    void initCplexFromModel();
    void initData();
};