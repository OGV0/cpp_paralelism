#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>

// Глобальні змінні
std::mutex mtx;
std::condition_variable cv;
int i = 0; // Ініціалізація нулем згідно з умовою

// Функція для 3-х потоків
void Waits(int id) {
    std::unique_lock<std::mutex> lock(mtx);
    // Вивід повідомлення перед початком очікування
    std::cout << "Потік " << id << " входить у стан очікування.\n";
    
    // Очікування з лямбда-функцією
    cv.wait(lock, [] { return i == 1; });
    
    // Повідомлення після успішного пробудження
    std::cout << "Потік " << id << ": очікування завершене.\n";
}

// Функція пробудження
void Awake() {
    // Спить 100 мілісекунд
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "\n[Awake]: Починає повідомляти решту потоків (1-й notify_all).\n";
    cv.notify_all();
    
    // Спить ще 100 мілісекунд
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Присвоює змінній значення 1
    {
        std::lock_guard<std::mutex> lock(mtx);
        i = 1;
    }
    
    std::cout << "[Awake]: Повторне повідомлення, змінна i = 1 (2-й notify_all).\n\n";
    cv.notify_all();
}

int main() {
    // Запуск трьох потоків Waits
    std::thread t1(Waits, 1);
    std::thread t2(Waits, 2);
    std::thread t3(Waits, 3);
    
    // Запуск потоку Awake
    std::thread awake_thread(Awake);
    
    // Головний потік чекає на виконання потоку Awake
    awake_thread.join();
    
    // Також необхідно дочекатися завершення потоків Waits, 
    // щоб уникнути помилки при завершенні програми (std::terminate)
    t1.join();
    t2.join();
    t3.join();
    
    std::cout << "Головний потік завершив роботу.\n";
    return 0;
}