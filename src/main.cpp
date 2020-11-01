#include <stdio.h>
#include <stdlib.h>
#include <data.h>
#include <model.h>

int main(int argc, char const *argv[]){
    Data data(argv[1]);
    // data.print();
    // data.generateDot();
    Model model(data);
    model.solve();
    // model.generateDot();
    return 0;
}
