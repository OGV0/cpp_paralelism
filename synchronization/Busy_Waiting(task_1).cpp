#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <chrono>
#include <cmath>


std::queue<int> data_queue;
bool data_ready = false;
std::mutex mtx;


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
        std::unique_lock<std::mutex> lock(mtx);
        data_queue.push(num);
    }

    // Після завершення вводу встановлюємо прапорець
    std::unique_lock<std::mutex> lock(mtx);
    data_ready = true;
}


void DataProcessing() {
    // Очікування завершення введення даних (з використанням polling)
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        if (data_ready) {
            break; // Якщо дані готові, виходимо з циклу очікування
        }
        lock.unlock(); // Звільняємо м'ютекс перед засинанням, щоб не блокувати інший потік
        
        // Засинаємо на 100 мілісекунд
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Обробка даних: вилучення з черги та перевірка на простоту
    std::unique_lock<std::mutex> lock(mtx);
    std::cout << "Прості числа з черги: ";
    bool found_prime = false;
    
    while (!data_queue.empty()) {
        int current_num = data_queue.front();
        data_queue.pop();
        
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