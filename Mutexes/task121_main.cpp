#include <iostream>
#include <thread>

using namespace std;

void Thread1() {
    cout << 1 << endl;
}

void Thread2() {
    cout << 2 << endl;
}

int main() {
   
    jthread t1(Thread1);
    jthread t2(Thread2);

    

    return 0;
}