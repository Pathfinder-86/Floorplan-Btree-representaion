#include <iostream>
#include <string>
#include "design/circuit.h"
#include "optimization.h"
using namespace std;
int main(int argc, char **argv) {
    string case_name(argv[1]);    
    cout<<"Case "<<case_name<<endl;    
    Circuit::getInstance(case_name);
    // Test create circuit
    Circuit::get_circuit().print_design();
    Optimization optimization;
    optimization.random();    
    
    //optimization.SA_model(); 

    //optimization.GA_model();
    return 0;
}