#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <future>
#include <vector>
#include <string>
#include <stdexcept>


uint64_t find_nth_prime(int n) {
    if (n <= 0) return 0;
    int count = 0;
    uint64_t num = 1;
    while (count < n) {
        num++;
        bool is_prime = true;
        for (uint64_t i = 2; i * i <= num; ++i) {
            if (num % i == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) count++;
    }
    return num;
}

int main() {
    
    std::deque<std::packaged_task<uint64_t()>> tasks_deque;
    std::deque<int> n_deque;

    // Примітиви синхронізації
    std::mutex mtx;
    std::condition_variable cv;
    bool stop_processing = false;

    // Функція незалежного потоку
    auto worker = [&]() {
        while (true) {
            std::packaged_task<uint64_t()> task;
            int current_n = 0;

            {
                // Забезпечення безпечного доступу до деків
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [&]() { 
                    return !tasks_deque.empty() || stop_processing; 
                });

                // Якщо завдань немає і отримано сигнал завершення — виходимо
                if (stop_processing && tasks_deque.empty()) {
                    break;
                }

                // Вилучення завдання та його номера n
                task = std::move(tasks_deque.front());
                tasks_deque.pop_front();

                current_n = n_deque.front();
                n_deque.pop_front();
            }

            // Виконання обчислення n-го простого числа
            task(); 
        }
    };

    // Запуск незалежного потоку
    std::thread worker_thread(worker);
    
    // Вектор для збереження ф'ючерсів (щоб потім вивести результати)
    std::vector<std::pair<int, std::future<uint64_t>>> results;

    std::cout << "Введіть номер n для обчислення n-го простого числа." << std::endl;
    std::cout << "Введіть 'stop' або 'q' для завершення вводу." << std::endl;
    
    std::string input;
    while (true) {
        std::cout << "> ";
        std::cin >> input;

        if (input == "stop" || input == "q") {
            break;
        }

        try {
            int n = std::stoi(input);
            if (n <= 0) {
                std::cout << "Помилка: n має бути додатним числом." << std::endl;
                continue;
            }

            // Створення завдання (packaged_task)
            std::packaged_task<uint64_t()> task([n]() { return find_nth_prime(n); });
            
            // Збереження future для отримання результату в головному потоці
            results.push_back({n, task.get_future()});

            // Додавання завдання та n у деки з використанням м'ютексу
            {
                std::lock_guard<std::mutex> lock(mtx);
                tasks_deque.push_back(std::move(task));
                n_deque.push_back(n);
            }
            
            // Сповіщаємо робочий потік, що з'явилося нове завдання
            cv.notify_one();

        } catch (const std::exception&) {
            std::cout << "Некоректний ввід. Введіть число або стоп-символ." << std::endl;
        }
    }

    // Сигналізуємо незалежному потоку про завершення роботи
    {
        std::lock_guard<std::mutex> lock(mtx);
        stop_processing = true;
    }
    cv.notify_all();

    // Головний потік очікує коректного завершення роботи незалежного потоку
    worker_thread.join();

 
    std::cout << "\n=== РЕЗУЛЬТАТИ ОБЧИСЛЕНЬ ===" << std::endl;
    for (auto& res : results) {
        // res.second.get() блокував би виконання, якби обчислення ще тривало,
        // але оскільки ми викликали join(), всі потоки вже завершили роботу.
        std::cout << res.first << "-те просте число: " << res.second.get() << std::endl;
    }

    return 0;
}