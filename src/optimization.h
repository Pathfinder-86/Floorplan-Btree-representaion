#ifndef OPTIMIZATION
#define OPTIMIZATION
#include <vector>
using namespace std;
class Optimization{
    public:    
        //Simulated_Annealing
        void SA_model();
        //Genetic
        void GA_model();
        // Random
        void random();
        // history
        vector<double> get_loss_history(){
            return loss_history;
        }        
    private:
        void output_loss();                
        vector<double> loss_history;    
        
};
#endif