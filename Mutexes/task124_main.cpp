#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

list<int> l;


mutex list_mutex;


void AddToList(int startValue) {
    int currentValue = startValue;
    for (int i = 0; i < 10; ++i) {
        {
            // Захист критичної секції: м'ютекс гарантує збереження структурного інваріанта списку
            lock_guard<mutex> lock(list_mutex);
            l.push_back(currentValue);
            cout << "[AddToList] Елемент " << currentValue << " успішно додано до списку." << endl;
        }
        currentValue++;
        
        // Затримка для наочності перемикання контексту між потоками
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

// Потік для перевірки наявності елемента у списку
void ListContains(int targetValue) {
    for (int i = 0; i < 10; ++i) {
        bool found = false;
        {
            // Захист критичної секції: читання відбувається лише над коректним станом списку
            lock_guard<mutex> lock(list_mutex);
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