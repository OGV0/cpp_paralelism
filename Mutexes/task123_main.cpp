#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

list<int> l;

mutex mtx;

// Потік для додавання елементів
void AddToList(int startValue) {
    int currentValue = startValue;
    for (int i = 0; i < 10; ++i) {
        {
            lock_guard<mutex> lock(mtx);
            l.push_back(currentValue);
            cout << "[AddToList] Успішно додано елемент: " << currentValue << endl;
        }
        currentValue++;
        
        // Невелика затримка для наочності перемикання контексту між потоками
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

// Потік для перевірки наявності елемента
void ListContains(int targetValue) {
    for (int i = 0; i < 10; ++i) {
        bool found = false;
        {
            lock_guard<mutex> lock(mtx);
            for (int val : l) {
                if (val == targetValue) {
                    found = true;
                    break;
                }
            }
            cout << "[ListContains] Спроба " << (i + 1) 
                      << ": елемент " << targetValue 
                      << (found ? " ВХОДИТЬ" : " НЕ ВХОДИТЬ") << " до списку." << endl;
        }
        
        this_thread::sleep_for(chrono::milliseconds(12));
    }
}

int main() {
    int initialValue = 5; 

    
    thread t1(AddToList, initialValue);
    thread t2(ListContains, initialValue);

    
    t1.join();
    t2.join();

    return 0;
}