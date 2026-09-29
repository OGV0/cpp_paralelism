#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

list<int> l;


mutex mtx;


void AddToList(int value) {
    

    lock_guard<mutex> lock(mtx);
    l.push_back(value);
    cout << "[AddToList] Додано елемент: " << value << endl;
}


void ListContains(int targetValue, int attempt) {
    bool found = false;
    {
        lock_guard<mutex> lock(mtx);
        for (int val : l) {
            if (val == targetValue) {
                found = true;
                break;
            }
        }
        cout << "[ListContains] Спроба " << attempt 
                  << ": елемент " << targetValue 
                  << (found ? " ВХОДИТЬ" : " НЕ ВХОДИТЬ") << " до списку." << endl;
    }
}

int main() {
    int initialValue = 5; 
    int currentValue = initialValue;

    // Цикл для запуску та від'єднання 10 екземплярів кожного потоку
    for (int i = 0; i < 10; ++i) {
        // Створення та від'єднання потоку додавання (число кожного разу більше на 1)
        thread t1(AddToList, currentValue);
        t1.detach();
        currentValue++;

        // Створення та від'єднання потоку перевірки (перевіряється одне й те саме число)
        thread t2(ListContains, initialValue, i + 1);
        t2.detach();
    }

    // Затримка головного потоку, щоб дозволити фоновим (detached) потокам завершити виконання
    this_thread::sleep_for(chrono::milliseconds(500));

    return 0;
}