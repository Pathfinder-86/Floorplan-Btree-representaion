#include "circuit.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
void Circuit::create_circuit()
{
    string case_folder = "../data/" + case_name + "/";
    string block_filename = case_folder + case_name + ".block";
    string net_filename = case_folder + case_name + ".nets";
    // read block
    ifstream block_file(block_filename);
    if (!block_file)
    {
        cout << "FILE " << block_filename << " not found" << endl;
    }
    stringstream ss;
    ss << block_file.rdbuf();
    block_file.close();
    create_module(ss.str());
    // read net
    ss.clear();
    ifstream net_file(net_filename);
    if (!net_file)
    {
        cout << "FILE " << net_filename << " not found" << endl;
    }

    ss << net_file.rdbuf();
    net_file.close();
    create_net(ss.str());
}

void Circuit::create_module(const string &file_txt)
{
    stringstream ss;
    ss << file_txt;
    string str;
    int id = 0;
    while (ss >> str)
    {        
        if (str.find("Outline") != string::npos)
        {
            ss >> outline_width >> outline_height;
        }
        else if (str.find("NumBlocks") != string::npos)
        {
            ss >> blockN;
        }
        else if (str.find("NumTerminals") != string::npos)
        {
            ss >> terminalN;
            moduleN = blockN + terminalN;
            modules.reserve(moduleN);
            module_id_string.reserve(moduleN);
            for(int i=0;i<blockN;i++){
                int w, h;
                ss >> str;
                ss >> w >> h;
                module_string_id[str] = id++;
                module_id_string.emplace_back(str);
                modules.emplace_back(Module(0, 0, w, h));
            }
            for(int i=0;i<terminalN;i++){
                ss >> str;
                module_string_id[str] = id++;
                module_id_string.emplace_back(str);
                // terminal
                ss >> str;
                int x, y;
                ss >> x >> y;                        
                modules.emplace_back(Module(x, y, 0, 0));
            }
        }
    }
}

void Circuit::create_net(const string &file_txt)
{
    stringstream ss;
    ss << file_txt;
    string str;
    while (ss)
    {
        ss >> str;
        if (str.find("NumNets") != string::npos)
        {
            ss >> netN;
            nets.reserve(netN);
        }
        else if (str.find("NetDegree") != string::npos)
        {
            int degree;
            ss >> degree;
            vector<int> net;
            net.reserve(degree);
            for (int i = 0; i < degree; i++)
            {
                ss >> str;
                int id = module_string_id[str];
                net.push_back(id);
            }
            nets.push_back(net);
        }
    }
}

string Circuit::get_module_name(int id) const
{
    string name;
    try
    {
        name = module_id_string.at(id);
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return name;
}

int Circuit::get_module_id(const string &name) const
{
    auto it = module_string_id.find(name);
    if (it != module_string_id.end())
    {
        return it->second;
    }
    else
    {
        cout << "ERROR module id not found: " << name << endl;
    }
}

Module & Circuit::get_module(int id)
{
    try
    {
        modules.at(id);
        return modules[id];
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }        
}

const Net & Circuit::get_net(int id) const
{    
    try
    {
        return nets.at(id);
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }    
}

void Circuit:: print_design(){
    ofstream file("../output/design_info");
    file<<"Module"<<endl;
    for(int i=0; i<modules.size();i++){
        Module &module = modules.at(i);
        file<<module_id_string[i]<<" "<<module.getX()<<" "<<module.getY()<<" "<<module.getWidth()<<" "<<module.getHeight()<<endl;
    }
    file<<"Net"<<endl;
    for(auto &net : nets){
        for(auto id : net){
            file<<id<<" ";
        }
        file<<endl;
    }

}

void Circuit:: output(string name)const{
    if(name.empty()){
        name = to_string(plot);
        plot++;
    }
    string out_filename = "../output/" + case_name + "/" + name;
    ofstream file(out_filename);
    file<<outline_width<<" "<<outline_height<<endl;
    for(int i=0;i<blockN;i++){
        file<<modules[i].getX()<<" "<<modules[i].getY()<<" "<<modules[i].getRX()<<" "<<modules[i].getRY()<<endl;
    }
    file.close();
}
