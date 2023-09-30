#include "optimization.h"
#include "b_tree.h"
#include "design/circuit.h"
#include <iostream>
using namespace std;
void Optimization :: random(){
    B_Tree btree;
    btree.init();    
    // iterations
    for(int i=0;i<20000;i++)
    {    
        //cout<<i<<" iteration"<<endl;
        btree.perturb();
        btree.packing();
        double cost = btree.evaluate(1.01,0.97);
        loss_history.push_back(cost);
        /*if(i%1000==0){
            Circuit::get_circuit().output(to_string(i/1000));
        }*/
    }
    btree.recover_best();    
    Circuit::get_circuit().output("best");
    output_loss();
}

void Optimization :: SA_model(){

}

void Optimization :: GA_model(){

}

void Optimization :: output_loss(){
    string case_name = Circuit::get_circuit().getCaseName();
    string output_filename = "../loss/" + case_name + "/loss";
    ofstream file(output_filename);
    for(auto loss : loss_history){
        file<< loss <<endl;
    }
}

