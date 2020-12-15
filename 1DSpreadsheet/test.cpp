#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <future>
#include <iostream>
#include <thread>
#include <utility>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int getValue(vector<shared_future<int>> &futures, const string &argument) {
    if (argument[0] == '$') { 
        return futures[atoi(argument.c_str())].get();
    } else if (argument[0] != '_') {
        return atoi(argument.c_str());
    } else {
        return 0;
    }
}
void doOperation(promise<int> && promise, vector<shared_future<int>>& futures, string operator_name, string argument1, string argument2) {
    int value1=getValue(futures, argument1);
    int value2=getValue(futures, argument2);
    int result = 0;
    switch (operator_name[0]) {
    case 'V':
        result = value1;  
        break;
    case 'A':
        result = value1 + value2;
        break;
    case 'S':
        result = value1 - value2;
        break;
    case 'M':
        result = value1 * value2;
        break;
    }
    promise.set_value(result);
}


int main()
{
    /*
    vector<tuple<string,string,string>> statements =
    {
        {"VALUE","3","_"},
        {"ADD","$0","4"}
    };
    */
    int N;
    cin >> N; cin.ignore();
    //auto N = statements.size();
    vector<promise<int>> promises(N);
    vector<shared_future<int>> futures(N);
    vector<thread> threads(N);
    for (int i=0;i<N;i++){
        futures[i] = promises[i].get_future();
    }
    for (int i = 0; i < N; i++) {
        string operation;
        string arg1;
        string arg2;
        //tie(operation, arg1, arg2) = statements[i];
        cin >> operation >> arg1 >> arg2; cin.ignore();
        threads[i] = thread(doOperation,move(promises[i]),ref(futures),operation, arg1,arg2);
    }
    for (int i=0;i<N;i++) {
        cout << futures[i].get() << endl;
    }
}