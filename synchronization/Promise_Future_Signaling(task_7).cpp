#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <cmath>

// Допоміжна функція для перевірки, чи є число простим
bool isPrime(int num) {
    if (num <= 1) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

// Функція для пошуку k-го простого числа
int findKthPrime(int k) {
    int count = 0;
    int num = 1;
    while (count < k) {
        num++;
        if (isPrime(num)) {
            count++;
        }
    }
    return num;
}

// Перший потік
void thread1_func(int n, 
                  std::promise<int>&& prom_n, 
                  std::promise<bool>&& prom_signal, 
                  std::promise<int>&& prom_n10) 
{
    // 1. Знайти n-те просте число
    int nth_prime = findKthPrime(n);
    
    // Передати n-те просте число головному потоку
    prom_n.set_value(nth_prime);
    
    // 2. Сигналізувати другому потоку (встановити true)
    prom_signal.set_value(true);
    
    // 3. Знайти (n * 10)-те просте число
    int n10th_prime = findKthPrime(n * 10);
    
    // Передати (n * 10)-те просте число головному потоку
    prom_n10.set_value(n10th_prime);
}

// Другий потік
void thread2_func(int n, std::future<bool>&& fut_signal) {
    // Очікування сигналу від першого потоку
    bool start_work = fut_signal.get(); 
    
    if (start_work) {
        // Чекаємо 2 секунди
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Виводимо квадратний корінь від числа n
        // std::endl використано для коректного скидання буфера виводу при конкурентному виконанні
        std::cout << "[Потік 2] Квадратний корінь з " << n << " дорівнює: " << std::sqrt(n) << std::endl;
    }
}

int main() {
    int n;
    std::cout << "Введіть число n: ";
    if (!(std::cin >> n) || n <= 0) {
        std::cerr << "Помилка вводу. n має бути додатнім цілим числом." << std::endl;
        return 1;
    }

    // Створення об'єктів promise та відповідних їм future
    std::promise<int> prom_n;
    std::future<int> fut_n = prom_n.get_future();

    std::promise<bool> prom_signal;
    std::future<bool> fut_signal = prom_signal.get_future();

    std::promise<int> prom_n10;
    std::future<int> fut_n10 = prom_n10.get_future();

    // Запуск потоків (promise і future передаються за допомогою std::move, оскільки вони не копіюються)
    std::thread t1(thread1_func, n, std::move(prom_n), std::move(prom_signal), std::move(prom_n10));
    std::thread t2(thread2_func, n, std::move(fut_signal));

    // Отримання та вивід n-го простого числа (головний потік блокується, поки t1 не викличе set_value)
    std::cout << "[Головний потік] " << n << "-те просте число: " << fut_n.get() << std::endl;

    // Отримання та вивід (n*10)-го простого числа
    std::cout << "[Головний потік] " << (n * 10) << "-те просте число: " << fut_n10.get() << std::endl;

    // Очікування завершення потоків
    t1.join();
    t2.join();

    return 0;
}