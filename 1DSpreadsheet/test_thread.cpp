#include <string>
#include <iostream>
#include <thread>
#include <future>

using namespace std;

void task1(promise<int> && promise, vector<shared_future<int>> &futures, int arg, string msg){
    cerr << "task1 says: " << msg;
    promise.set_value(arg);
}

int main(){
    int N=10;
    vector<promise<int>> promises(N);
    vector<shared_future<int>> futures(N);
    for (int i=0; i<N; i++) {
        futures[i] = promises[i].get_future();
    }
    vector<thread> threads(N);
    for (int i = 0; i<10; i++) {
        threads[i] = thread(task1, move(promises[i]), futures, i, "Hello");
    }
    for(auto f:futures) { cout << f.get() << endl; }
}