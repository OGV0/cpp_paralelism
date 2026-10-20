#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;
int i = 0; // Змінна, про яку йдеться в завданні

// Функція для потоків Thread1, Thread2 та Thread3
void worker_thread(int id) {
    std::unique_lock<std::mutex> lock(mtx);
    
    // Потік очікує на сигнал (ready стане true)
    cv.wait(lock, []{ return ready; });
    
    // Вивід повідомлення після пробудження
    std::cout << "Повідомлення з потоку " << id << std::endl;
}

// Функція для потоку Notify
void notify_thread() {
    // Очікування на протязі 100 мілісекунд
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true; // Встановлюємо прапорець, щоб уникнути хибних пробуджень
        i = 1;        // Присвоюємо змінній i значення 1
    }
    
    // Пробуджуємо лише один з очікуючих потоків
    cv.notify_one();
}

int main() {
    // Створюємо три потоки
    std::thread Thread1(worker_thread, 1);
    std::thread Thread2(worker_thread, 2);
    std::thread Thread3(worker_thread, 3);
    
    // Створюємо потік Notify
    std::thread Notify(notify_thread);
    
    // Головний потік дочікується виконання потоку Notify
    Notify.join();
    
    // Даємо трохи часу пробудженому потоку, щоб він встиг вивести повідомлення
    // на екран до того, як головний потік завершить програму.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Оскільки ми пробудили лише ОДИН потік (notify_one), два інші
    // назавжди залишаться в стані очікування. Щоб програма могла коректно 
    // завершитися (інакше виклик join() для них зависне), ми їх від'єднуємо.
    Thread1.detach();
    Thread2.detach();
    Thread3.detach();
    
    return 0;
}