#include "b_tree.h"
#include <climits>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
using namespace std;
double rand01()
{
    return double(rand()%10000) / 10000.0;
}

void B_Tree :: init()
{    
    modules_N = Circuit::get_circuit().get_blockN();
    contour.resize(modules_N);
    nodes.resize(modules_N);
    nodes_root = 0;
    for(int i=0;i<modules_N;i++)
    {        
        nodes[i].id = i;
        nodes[i].parent = (i+1)/2-1;
        nodes[i].left = (2*i+1 < modules_N)? 2*i+1 : -1;
        nodes[i].right = (2*i+2 < modules_N)? 2*i+2 : -1;        
    }
    nodes[0].parent = -1;
    clear_contour();
    packing();
    double cost = getCost();
    keep_best(cost);
}
void B_Tree :: clear_contour()
{
    contour_root = -1;
}

void B_Tree ::packing(){
    clear_contour();
    packing(nodes_root);
}

void B_Tree ::packing(int ptr)
{
    // preorder
    if(ptr == -1)
        return;
    bool is_left = (ptr==nodes_root)? true : (ptr == nodes[nodes[ptr].parent].left)? true : false;
    place_module(ptr,nodes[ptr].parent,is_left);
    packing(nodes[ptr].left);
    packing(nodes[ptr].right);
}

void B_Tree :: place_module(int ptr,int parent,bool is_left)
{
    Module &module = Circuit::get_circuit().get_module(ptr);
    int w = module.getWidth();
    int h = module.getHeight();
    if(nodes[ptr].rotate)
        swap(w,h);

    if(parent==-1) // root of b_tree
    {
        contour_root = ptr;
        contour[ptr].front = -1;
        contour[ptr].back = -1;
        module.setX(0);
        module.setY(0);
        module.setRX(w);
        module.setRY(h);
        return;
    }

    int con_ptr; // trace on contour

    if(is_left) // place right
    {      
        module.setX( Circuit::get_circuit().get_module(parent).getRX());
        module.setRX(module.getX() + w);
        con_ptr = contour[parent].front;

        contour[parent].front = ptr;
        contour[ptr].back = parent;
        if(con_ptr==-1) // no obstacle in X axis
        {
            module.setY(0);
            module.setRY(h);
            contour[ptr].front = -1;
            return;
        }        
    }
    else // place top
    {
        module.setX(Circuit::get_circuit().get_module(parent).getX());
        module.setRX(module.getX() + w);
        con_ptr = parent;
        int n = contour[con_ptr].back;
        if(n==-1)  // first of the contour
        {
            contour_root = ptr;
            contour[ptr].back = -1;
        }
        else
        {
            contour[n].front = ptr;
            contour[ptr].back = n;
        }
    }
    
    // handle y and update contour 
    int min_y = -1,bx,by;

    for( ; con_ptr != -1 ; con_ptr = contour[con_ptr].front)
    {
        bx = Circuit::get_circuit().get_module(con_ptr).getRX();
        by = Circuit::get_circuit().get_module(con_ptr).getRY();
        min_y = max(min_y,by);
        if( bx >= module.getRX())
        {
            module.setY(min_y);
            module.setRY(min_y+h);
            if(bx > module.getRX())
            {
                contour[ptr].front = con_ptr;
                contour[con_ptr].back = ptr;
            }
            else
            {
                int n = contour[con_ptr].front;
                contour[ptr].front = n;
                if(n!=-1)
                    contour[n].back = ptr;
            }
            break;
        }
    }
    // last 
    if(con_ptr == -1)
    {
        module.setY( min_y );
        module.setRY( min_y + h);
        contour[ptr].front = -1;
    }
}

void B_Tree :: get_solution_outline(){
    // x,y,rx,ry
    outlines = {INT_MAX,INT_MAX,INT_MIN,INT_MIN};
    for(auto &node : nodes){
        int id = node.id;
        outlines[0] = min(outlines[0],Circuit::get_circuit().get_module(id).getX());
        outlines[1] = min(outlines[1],Circuit::get_circuit().get_module(id).getY());
        outlines[2] = max(outlines[2],Circuit::get_circuit().get_module(id).getRX());
        outlines[3] = max(outlines[3],Circuit::get_circuit().get_module(id).getRY());
    }
}

double B_Tree :: getCost(){
    auto outline = Circuit::get_circuit().get_outline();
    int outline_x = outline.first;
    int outline_y = outline.second;
    
    get_solution_outline();
    int sol_outline_x = outlines[2];
    int sol_outline_y = outlines[3];    
    // out of range penalty
    double out_of_range_cost = 0,area = 0,total_wire_length = 0;
    double out_of_range_y = 0, out_of_range_x = 0;
    // Horizontal 
    if(outline_y < sol_outline_y){  
        out_of_range_y = sol_outline_y * 1000000.0;
    }
    // Vertical
    if(outline_x < sol_outline_x){
        out_of_range_x = sol_outline_x * 1000000.0;
    }
    
    out_of_range_cost = out_of_range_x + out_of_range_y;
    
    // sol_area
    area = (double)sol_outline_x * sol_outline_y;

    // HPWL
    for(auto &it : Circuit::get_circuit().get_nets()){
        vector<int> limit = {INT_MAX,INT_MAX,INT_MIN,INT_MIN};
        for(auto nid : it){
            limit[0] = min(limit[0],Circuit::get_circuit().get_module(nid).getX());
            limit[1] = min(limit[1],Circuit::get_circuit().get_module(nid).getY());
            limit[2] = max(limit[2],Circuit::get_circuit().get_module(nid).getRX());
            limit[3] = max(limit[3],Circuit::get_circuit().get_module(nid).getRY());
        }
        total_wire_length += (limit[2]-limit[0]) + (limit[3]-limit[1]);
    }
    double total_cost = out_of_range_cost + 0.5 * total_wire_length + 0.5 * area;
    if(total_cost < cur_sol.cost){
        cout<<"cur_sol"<<cur_sol.cost<<" out_of_range_x:"<<out_of_range_x<<" out_of_range_y:"<<out_of_range_y<<endl;
    }
    return out_of_range_cost + 0.5 * total_wire_length + 0.5 * area;
}


// evaluate solution

double B_Tree :: evaluate(double degrade_ratio,double accept){
    double cost = getCost();
    bool accept_degrade = rand01() > accept;
    if(cost < best_sol.cost){
        keep_best(cost);
    }    
    else if(cost < cur_sol.cost)
    {
        keep_sol(cost);
    }
    else
    {
        if( accept_degrade && cost < cur_sol.cost * degrade_ratio){
            keep_sol(cost);
        }else{
            recover(cur_sol);
        }
    }    
    return cur_sol.cost;
}

void B_Tree ::keep_sol(double cost){
    cur_sol.nodes = nodes;
    cur_sol.nodes_root = nodes_root;
    cur_sol.cost = cost;    
}

void B_Tree ::keep_best(double cost)
{
    best_sol.nodes = nodes;
    best_sol.nodes_root = nodes_root;
    best_sol.cost = cost;
    Circuit::get_circuit().output("");
    cout<<"keep best "<<cost<<endl;
    keep_sol(cost);
}

void B_Tree ::recover_best()
{
    recover(best_sol);
    packing();
    check_out_of_range();
}


void B_Tree ::check_out_of_range(){
    auto outline = Circuit::get_circuit().get_outline();
    int outline_x = outline.first;
    int outline_y = outline.second;    
    get_solution_outline();
    int sol_outline_x = outlines[2];
    int sol_outline_y = outlines[3];
    if(outline_x < sol_outline_x || outline_y < sol_outline_y){
        cout<<outline_x << " " <<  sol_outline_x << " " << outline_y << " " <<sol_outline_y<<endl;
        cout<<"out of range FAIL"<<endl;
    }
}

void B_Tree :: recover(Solution &sol)
{
    nodes = sol.nodes;
    nodes_root = sol.nodes_root;
}

void B_Tree ::perturb()
{
    //cout<<"perturb"<<endl;
    int p,n,s;
    double swap_rate = 0.5;
    double rotate_rate = 0.5;
    s = rand()% modules_N;
    n = rand()% modules_N;
    if(rotate_rate > rand01()){
        nodes[s].rotate = !nodes[s].rotate;
    }

    if( swap_rate > rand01())
    {
        do{
            p = rand()%modules_N;               
        }while(p==n ||nodes[n].parent==p||nodes[p].parent==n);
        swap_node(nodes[p],nodes[n]);
    }
    else
    {
        do{
            p = rand()%modules_N;               
        }while(p==n);     
        delete_node(nodes[n]);
        insert_node(nodes[p],nodes[n]);        
    }
}

void B_Tree :: swap_node(Node &n1,Node &n2)
{
    // update nodes' children
    //cout<<"swap_node"<<endl;
    if(n1.left != -1)    
        nodes[n1.left].parent = n2.id;
    if(n1.right != -1)
        nodes[n1.right].parent = n2.id;
    if(n2.left != -1)
        nodes[n2.left].parent = n1.id;
    if(n2.right != -1)
        nodes[n2.right].parent = n1.id;
    
    // update nodes' parents
    if(n1.parent != -1) // not root
    {
        if(nodes[n1.parent].left == n1.id) // n1 is left child of its parent
            nodes[n1.parent].left = n2.id;
        else
           nodes[n1.parent].right = n2.id; 
    }
    else
        nodes_root = n2.id;

    if(n2.parent != -1) // not root
    {
        if(nodes[n2.parent].left == n2.id) // n1 is left child of its parent
            nodes[n2.parent].left = n1.id;
        else
           nodes[n2.parent].right = n1.id; 
    }
    else
        nodes_root = n1.id;
    swap(n1.left,n2.left);
    swap(n1.right,n2.right);
    swap(n1.parent,n2.parent);
}
bool rand_bool()
{
    return (rand()%2==0);
}

void B_Tree :: insert_node(Node &parent, Node &node)
{
    //cout<<"insert_node"<<endl;
    node.parent = parent.id;
    bool egde = rand_bool();
    if(egde)
    {
        node.left = parent.left;
        node.right = -1;
        if(parent.left != -1)
            nodes[parent.left].parent = node.id;
        parent.left = node.id;
    }
    else
    {
        node.left = -1;
        node.right = parent.right;
        if(parent.right != -1)
            nodes[parent.right].parent = node.id;
        parent.right = node.id;
    }
}

void B_Tree :: delete_node(Node &node)
{
    //cout<<"delete_node"<<endl;
    int child = -1;  // pull which child
    int subchild = -1;
    int subparent = -1;
    if(!node.isLeaf())   // update new node points to original node's child
    {
        bool left = rand_bool();
        if(node.left == -1) left = false;
        if(node.right == -1) left = true;

        if(left)
        {
            child = node.left;
            if(node.right != -1)
            {
                subchild  = nodes[child].right;
                subparent = node.right;                
                nodes[node.right].parent =child;
                nodes[child].right = node.right;
            }            
        }
        else
        {
            child = node.right;
            if(node.left!= -1)
            {
                subchild  = nodes[child].left;  // ?
                subparent = node.left;                
                nodes[node.left].parent =child;
                nodes[child].left = node.left;
            }
        }     
        nodes[child].parent = node.parent;   
    }

    if(node.parent == -1) // root    update parent to new child
        nodes_root = child;
    else
    {
        if(node.id == nodes[node.parent].left) // left child
            nodes[node.parent].left = child;
        else
            nodes[node.parent].right = child;
    }

    // subtree    
    if(subchild != -1)
    {
        Node &sc = nodes[subchild];
        int count = 1;
        while(1)
        {
            count++;
            Node &p = nodes[subparent];
            if(p.left == -1 || p.right == -1)
            {
                sc.parent = p.id;
                if(p.left ==-1)
                    p.left = sc.id;
                else
                    p.right = sc.id;
                break;
            }
            else            
                subparent = (rand_bool())? p.left : p.right;        
        }       
    }
}