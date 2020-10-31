#pragma once
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

struct Data{
    // Path to instance
    std::string path;

    // Numero de vertice (n) e arcos (m)
    size_t n,m;

    // Index origem (s) e escoadouro (t) 
    int s, t;

    // u_ij custo para atravessar arco (i,j)
    std::vector<std::vector<int>> u;


    FILE* fp;
    Data(const char* path_to_instance);
    Data(std::string path_to_instance);
    ~Data();
    Data(){};

    void print();
    void generateDot();
};