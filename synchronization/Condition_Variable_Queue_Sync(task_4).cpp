#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable> 
#include <chrono>
#include <cmath>


std::queue<int> data_queue;
bool data_ready = false;
std::mutex mtx;
std::condition_variable cv; 


bool is_prime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}


void DataPreparation() {
    int num;
    std::cout << "Введіть цілі числа (для завершення введіть будь-яку літеру, наприклад 'q'):\n";
    
    // Зчитуємо числа, поки користувач вводить коректні цілі числа
    while (std::cin >> num) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            data_queue.push(num);
        }
        cv.notify_one(); // Сповіщаємо потік обробки, що в черзі з'явилися нові дані
    }

    // Після завершення вводу встановлюємо прапорець
    {
        std::unique_lock<std::mutex> lock(mtx);
        data_ready = true;
    }
    cv.notify_one(); // Сповіщаємо про завершення вводу (на випадок, якщо черга порожня)
}


void DataProcessing() {
    std::cout << "Прості числа з черги: ";
    bool found_prime = false;
    
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        
        // Очікування даних з використанням умовної змінної.
        // Лямбда-функція перевіряє чергу на пустоту (та чи не завершився ввід).
        cv.wait(lock, [] { return !data_queue.empty() || data_ready; });
        
        // Якщо черга порожня і користувач завершив ввід — виходимо з циклу
        if (data_queue.empty() && data_ready) {
            break;
        }
        
        // Вилучення елемента з черги
        int current_num = data_queue.front();
        data_queue.pop();
        
        // Звільняємо м'ютекс перед обчисленнями, щоб не блокувати потік підготовки
        lock.unlock(); 
        
        if (is_prime(current_num)) {
            std::cout << current_num << " ";
            found_prime = true;
        }
    }
    
    if (!found_prime) {
        std::cout << "відсутні.";
    }
    std::cout << std::endl;
}

int main() {
 
    std::thread prep_thread(DataPreparation);
    prep_thread.detach();

 
    std::thread proc_thread(DataProcessing);
    
 
    proc_thread.join();

    return 0;
}