#ifndef BTREE_H
#define BTREE_H
#include "design/circuit.h"
#include <cfloat>

struct Node{
  int id,parent,left,right; 
  bool rotate = false;
  bool isLeaf(){ return (left==-1 && right==-1); }
};
 
struct Contour{  
  int front,back;
};

struct Solution{
    int nodes_root;
    vector<Node> nodes;
    double cost;
    Solution(): cost(DBL_MAX) {}
    void clear(){cost = DBL_MAX,nodes.clear();}
};

class B_Tree{
  public:
    B_Tree(){};
    void init();
    void perturb();
    void packing();
    double evaluate(double degrade_ratio=1.05,double accept_rate=2.0);
    void recover_best();
    void check_out_of_range();
  private:    
    void packing(int root);
    void place_module(int mod,int abut,bool is_left=true);
    void clear_contour();

    // SA operations
    void swap_node(Node &n1, Node &n2);
    void insert_node(Node &parent,Node &node);
    void delete_node(Node &node);
    
    // evaluation
    double getCost();
    void get_solution_outline();
    void keep_sol(double);
    void keep_best(double);
    void recover(Solution &);
  private:
    int modules_N;
    int contour_root;
    vector<int> outlines;
    vector<Contour> contour;
    int nodes_root;
    vector<Node> nodes;    
    Solution best_sol, cur_sol;
    
};

#endif
