#ifndef CIRCUIT
#define CIRCUIT
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
using namespace std;

#define Net vector<int>

struct Module{
    Module(int x,int y,int width,int height):x(x),y(y),width(width),height(height),rotate(false){
        rx = x + width, ry = y + height;
    };
    public:
        bool getRotate()const{return rotate;}
        int getWidth()const{return width;}
        int getHeight()const{return height;}
        int getX()const{return x;}
        int getY()const{return y;}
        int getRX()const{return rx;}
        int getRY()const{return ry;}

        void setRotate(bool value){this->rotate = value;}
        void setWidth(int value){this->width = value;}
        void setHeight(int value){this->height = value;}
        void setX(int value){this->x = value;}
        void setY(int value){this->y = value;}
        void setRX(int value){this->rx = value;}
        void setRY(int value){this->ry = value;}
    private:
        int x,y;
        int rx,ry;
        bool rotate;
        int width, height;    
};

class Circuit
{
    private:
        string case_name;
        int outline_width, outline_height;
        int blockN,terminalN,netN,moduleN;
        mutable int plot;
        
        unordered_map<string,int> module_string_id;
        vector<string> module_id_string;
        vector<Module> modules;
        vector<vector<int>> nets;
        Circuit(string case_name) : case_name(case_name),plot(0){
            create_circuit();
        };
    public:
        Circuit()=delete;        
        void create_circuit();
        string getCaseName(){return case_name;}
        static Circuit& get_circuit(){
            return getInstance("");
        }
        static Circuit& getInstance(std::string case_name) {
            static Circuit instance(case_name);
            return instance;
        }
        string get_module_name(int id) const;
        int get_module_id(const string &) const;        
        Module & get_module(int);
        const Net& get_net(int) const;
        int get_blockN() const {return blockN;};
        int get_netN() const {return netN;};
        int get_terminalN() const {return terminalN;};
        const vector<Net>& get_nets() const{
            return nets;
        };
        const vector<Module>& get_modules() const{
            return modules;
        };

        void create_module(const string &);
        void create_net(const string &);
        void print_design();
        pair<int,int> get_outline(){
            return {outline_width,outline_height};
        }
        void output(string) const;
        ~Circuit(){};
};

extern Circuit circuit;

#endif