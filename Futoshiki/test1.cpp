#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
const char *test1 = 
R"(3
1 0
   
0 0)";
const char *test2 =
R"(3
0>0
   
0<0)";
const char *test3 =
R"(3
0 0
^ v
0 0)";

const char *test4 = 
R"(9
0>3 5 1 0
         
0 2 0 5 0
        ^
0 0 0 0 0
         
0 5 0 3 0
         
0 4 3 2<0)";




struct Constraint {
    int value;
    char bottom;
    char right;
};

void parseConstraints(int line_index, const string& text, vector<vector<Constraint>> &constraints) {
    int column_index = 0; 
    istringstream text_reader(text);
    text_reader >> std::noskipws;
    while(!text_reader.eof()) {
        int value;
        char constraint;
        if (line_index % 2==0) {
            if (column_index % 2 == 0) {
                text_reader >> value;
                constraints[line_index/2][column_index/2].value = value;
            } else {
                text_reader >> constraint;
                if (constraint == ' ') constraint = 0;
                constraints[line_index/2][column_index/2].right = constraint;
            }
        } else {
            if (column_index % 2 == 0) {
                text_reader >> constraint;
                if (constraint == ' ') constraint = 0;
                constraints[line_index/2][column_index/2].bottom = constraint;
            } else {
                while(text_reader.peek() == ' ') { text_reader.ignore(); }
            }        
        }
        column_index++;
    } 
}

void intializeGrid(vector<vector<int>> &grid) {
    int size = grid.size();
    for (int line = 0; line < size; line++) {
        for (int column = 0; column < size; column++) {
            grid[line][column] = (column + line) % size + 1;
        }
    }
}


int main() {
    istringstream strin(test1);
    istream *in_ptr = &cin;
    in_ptr = (istream*) &strin;

    int size;
    *in_ptr >> size; (*in_ptr).ignore();
     cerr << size << endl;

    int grid_size = size/2 + 1;
    cerr << grid_size <<endl;
    vector<vector<Constraint>> constraints; 
    constraints.resize(grid_size,vector<Constraint>(grid_size));
    for (int i = 0; i < size; i++) {
        string line;
        getline(*in_ptr, line);
        cerr << line << endl;
        parseConstraints(i,line,constraints);
    }
    vector<vector<int>> grid;
    grid.resize(grid_size,vector<int>(grid_size));
    intializeGrid(grid);
    // Write an answer using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;
    for (int line = 0; line < grid.size(); line++) {
        for (int column = 0; column < grid.size(); column++) {
            if (column > 0) {cout << " ";}
            cout << grid[line][column];
        }
        cout << endl;        
    }
}