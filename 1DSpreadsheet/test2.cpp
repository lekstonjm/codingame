#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <tuple>
#include <stack>

using namespace std;

struct Cell {
    char operation;
    pair<int,int> arg1;
    pair<int,int> arg2;
    int value;
    bool has_value;

    static pair<int,int> translate(const string &argument) {
        int index = -1;
        int value = 0;
        if (argument[0] =='$') {
            index = atoi(argument.substr(1).c_str()); 
        } else if(argument[0] != '_') {
            value = atoi(argument.c_str());
        }
        return {index, value};
    }

    void setOperation(const string &operation_,const string &argument1,const string &argument2) {
        operation = operation_[0];
        arg1 = translate(argument1);
        arg2 = translate(argument2);
        if (arg1.first == -1 && arg2.first == -1) {
            evaluate();    
        }
    }
    void evaluate() {
        switch(operation) {
            case 'V':
            value = arg1.second;
            break;
            case 'A':
            value = arg1.second + arg2.second;
            break;
            case 'S':
            value = arg1.second - arg2.second;
            break;
            case 'M':
            value = arg1.second * arg2.second;
            break;
        }
        has_value = true;
    }
};

struct Sheet {
    vector<Cell> cells;

    int getValue(int cell_index) {
        if (cells[cell_index].has_value) return cells[cell_index].value;
        stack<int> stack;
        stack.push(cell_index);
        auto deref = [&](pair<int,int> &argument)->bool {
            bool can_compute = true;
            if (argument.first >= 0) {
                if (!cells[argument.first].has_value) {
                    can_compute = false;
                    stack.push(argument.first);
                } else {
                    argument.second = cells[argument.first].value;
                }
            }
            return can_compute;
        };
        while (!stack.empty()) {
            Cell &current_cell = cells[stack.top()];
            bool can_compute = true;
            can_compute = can_compute && deref(current_cell.arg1);
            can_compute = can_compute && deref(current_cell.arg2);
            if (!can_compute) continue;
            current_cell.evaluate();
            stack.pop();
        }
        return cells[cell_index].value;
    }
};

int main()
{
    int N;
/*
    vector<tuple<string,string,string>> statements = {
        {"VALUE","3","_"},
        {"ADD","$0","4"}
    };
*/

    vector<tuple<string,string,string>> statements = {
    {"SUB","$33","$64"},
    {"ADD","$60","$60"},
    {"ADD","$61","$61"},
    {"SUB","$76","$80"},
    {"SUB","$25","$59"},
    {"ADD","$58","$28"},
    {"ADD","$88","$59"},
    {"ADD","$32","$32"},
    {"ADD","$83","$21"},
    {"ADD","$69","$39"},
    {"ADD","$57","$64"},
    {"ADD","$26","$26"},
    {"ADD","$1","$1"},
    {"SUB","$62","$68"},
    {"ADD","$73","$1"},
    {"ADD","$50","$27"},
    {"SUB","$24","$2"},
    {"ADD","$14","$12"},
    {"ADD","$10","$89"},
    {"SUB","$67","$35"},
    {"ADD","$58","$58"},
    {"ADD","$7","$7"},
    {"SUB","$0","$89"},
    {"ADD","$20","$20"},
    {"SUB","$43","$61"},
    {"SUB","$53","$11"},
    {"ADD","$37","$37"},
    {"ADD","$82","$47"},
    {"ADD","$90","$2"},
    {"ADD","$89","$89"},
    {"ADD","$85","$85"},
    {"SUB","$91","$47"},
    {"ADD","$69","$69"},
    {"SUB","$46","$86"},
    {"SUB","$42","$20"},
    {"ADD","$12","$12"},
    {"ADD","$56","$8"},
    {"ADD","$72","$72"},
    {"ADD","$9","$32"},
    {"ADD","$30","$77"},
    {"ADD","$80","$48"},
    {"ADD","$79","$81"},
    {"SUB","$16","$58"},
    {"SUB","$44","$56"},
    {"SUB","$63","$21"},
    {"ADD","$20","$5"},
    {"SUB","$49","$81"},
    {"ADD","$54","$54"},
    {"ADD","$29","$18"},
    {"SUB","$34","$23"},
    {"ADD","$47","$47"},
    {"SUB","$74","$32"},
    {"SUB","$17","$72"},
    {"SUB","$71","$26"},
    {"ADD","$59","$59"},
    {"ADD","$15","$68"},
    {"ADD","$21","$21"},
    {"ADD","$86","$41"},
    {"ADD","$2","$2"},
    {"ADD","$11","$11"},
    {"ADD","$80","$80"},
    {"ADD","$56","$56"},
    {"SUB","$31","$50"},
    {"SUB","$51","$7"},
    {"ADD","$86","$86"},
    {"ADD","$72","$35"},
    {"SUB","$75","$30"},
    {"SUB","$70","$12"},
    {"ADD","$50","$50"},
    {"ADD","$30","$30"},
    {"SUB","$84","$1"},
    {"SUB","$52","$37"},
    {"VALUE","1","_"},
    {"ADD","$40","$60"},
    {"SUB","$66","$69"},
    {"SUB","$13","$85"},
    {"SUB","$22","$29"},
    {"ADD","$55","$85"},
    {"ADD","$37","$65"},
    {"ADD","$23","$45"},
    {"ADD","$29","$29"},
    {"ADD","$23","$23"},
    {"ADD","$54","$6"},
    {"ADD","$38","$7"},
    {"SUB","$3","$60"},
    {"ADD","$68","$68"},
    {"ADD","$81","$81"},
    {"ADD","$78","$26"},
    {"ADD","$87","$11"},
    {"ADD","$64","$64"},
    {"ADD","$61","$36"},
    {"SUB","$4","$54"}
    };

    N = statements.size();
    //cin >> N; cin.ignore();
    Sheet sheet;
    sheet.cells.resize(N);
    for (int i = 0; i < N; i++) {
        string operation;
        string arg1;
        string arg2;
         tie(operation, arg1, arg2) = statements[i];
        //cin >> operation >> arg1 >> arg2; cin.ignore();
        sheet.cells[i].setOperation(operation, arg1, arg2); 
    }
    for (int i=0; i<N; i++) {
        cout << sheet.getValue(i) << endl;
    }
    return 0;
}