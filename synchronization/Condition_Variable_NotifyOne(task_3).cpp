#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;
int i = 0; 


void worker_thread(int id) {
    std::unique_lock<std::mutex> lock(mtx);
    
    // Потік очікує на сигнал (ready стане true)
    cv.wait(lock, []{ return ready; });
    
    
    std::cout << "Повідомлення з потоку " << id << std::endl;
}


void notify_thread() {
    
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
    
    std::thread Thread1(worker_thread, 1);
    std::thread Thread2(worker_thread, 2);
    std::thread Thread3(worker_thread, 3);
    
    
    std::thread Notify(notify_thread);
    
    // Головний потік дочікується виконання потоку Notify
    Notify.join();
 
    // Даємо трохи часу пробудженому потоку, щоб він встиг вивести повідомлення
    // на екран до того, як головний потік завершить програму.
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 

    Thread1.detach();
    Thread2.detach();
    Thread3.detach();
    
    return 0;
}