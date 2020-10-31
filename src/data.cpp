#pragma once
#include <data.h>


Data::~Data(){}
Data::Data(const char* path_to_instance){
    this->path = std::string(path_to_instance);
    this->fp = fopen(path_to_instance, "r");
    if(!fp){
        printf("Failed to open file %s!\n", path_to_instance);
        exit(-1);
    }
    fscanf(fp, "%zu\n%zu\n%d\n%d\n", &n, &m, &s, &t);
    s-=1;
    t-=1;
    this->u = std::vector<std::vector<int>>(n, std::vector<int>(n, 0));
    int _i = 0, _j = 0, _u_ij = 0;
    for(int e = 0; e < this->m; e++){
        fscanf(fp, "%d %d %d\n", &_i, &_j, &_u_ij);
        this->u[_i-1][_j-1] = _u_ij;
    }
    fclose(this->fp);
}
void Data::print(){
    printf("n: %zu, m: %zu, s:%d, t:%d\n", n, m, s+1, t+1);
    puts("c_ij:");
    for(int i = 0; i < this->n; i++){
        for(int j = 0; j < this->n; j++){
            if(u[i][j])
                printf("%3i ", u[i][j]);
            else
                printf("%3c ", '-');
        }
        puts("");
    }
}
void Data::generateDot(){
    printf("digraph D {\n");
    for(int i = 0; i < this->n; i++){
        if(i != s && i != t)
            printf("\t%d [shape=circle]\n", i+1);
        else if(i == s)
            printf("\t%d [shape=triangle, color=green]\n", i+1);
        else if(i == t)
            printf("\t%d [shape=triangle, color=red]\n", i+1);
    }
    for(int i = 0; i < this->n; i++){
        for(int j = 0; j < this->n; j++){
            if(u[i][j])
                printf("\t%i -> %i [label=%d]\n", i+1, j+1, u[i][j]);
        }
    }
    printf("}\n");
}