#include <iostream>
#include <future>
#include <cmath>
#include <chrono>
#include <string>

// Функція для знаходження n-го простого числа (не найоптимальніша, 
// але добре підходить для створення навантаження на процесор)
long long findNthPrime(int n) {
    if (n <= 0) return 0;
    int count = 0;
    long long num = 1;
    while (count < n) {
        num++;
        bool isPrime = true;
        for (long long i = 2; i * i <= num; i++) {
            if (num % i == 0) {
                isPrime = false;
                break;
            }
        }
        if (isPrime) count++;
    }
    return num;
}

// Функція для проведення тесту з певною політикою запуску
void runTest(int n, std::launch policy, const std::string& policyName) {
    std::cout << "\n========================================\n";
    std::cout << "Запуск з політикою: " << policyName << "\n";
    std::cout << "========================================\n";

    // Починаємо вимірювати загальний час виконання блоку
    auto startTotalTime = std::chrono::high_resolution_clock::now();

    // Створюємо об'єкт std::async із заданою політикою
    std::future<long long> primeFuture = std::async(policy, findNthPrime, n);

    std::cout << "Обчислення " << n << "-го простого числа розпочато...\n\n";
    
    std::cout << "Поки йде обчислення, виберіть математичну функцію для числа " << n << ":\n";
    std::cout << "1 - Корінь квадратний (sqrt)\n";
    std::cout << "2 - Синус (sin)\n";
    std::cout << "3 - Натуральний логарифм (ln)\n";
    std::cout << "Ваш вибір: ";

    int choice;
    std::cin >> choice;

    switch (choice) {
        case 1: std::cout << ">> sqrt(" << n << ") = " << std::sqrt(n) << "\n"; break;
        case 2: std::cout << ">> sin(" << n << ") = " << std::sin(n) << "\n"; break;
        case 3: std::cout << ">> ln(" << n << ") = " << std::log(n) << "\n"; break;
        default: std::cout << ">> Невірний вибір функції.\n"; break;
    }

    std::cout << "\nОчікування на завершення обчислення простого числа (виклик .get())...\n";

    // Вимірюємо час, який програма проводить безпосередньо заблокованою на методі .get()
    auto waitStart = std::chrono::high_resolution_clock::now();
    
    long long prime = primeFuture.get(); // Отримання результату
    
    auto waitEnd = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> waitDuration = waitEnd - waitStart;
    std::chrono::duration<double> totalDuration = waitEnd - startTotalTime;

    std::cout << "\nРЕЗУЛЬТАТ: " << n << "-те просте число = " << prime << "\n";
    std::cout << "Час очікування на .get(): " << waitDuration.count() << " секунд\n";
    std::cout << "Загальний час виконання:  " << totalDuration.count() << " секунд\n";
}

int main() {
    std::cout << "Введіть номер n для пошуку n-го простого числа (наприклад, 100000 або 1000000): ";
    int n;
    std::cin >> n;

    if (n <= 0) {
        std::cout << "Число має бути більшим за 0.\n";
        return 1;
    }

    // 1. Тест із відкладеним запуском (deferred)
    runTest(n, std::launch::deferred, "std::launch::deferred");

    // 2. Тест із асинхронним запуском (async)
    runTest(n, std::launch::async, "std::launch::async");

    return 0;
}