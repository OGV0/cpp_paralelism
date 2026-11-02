#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <stdexcept>
#include <functional>
#include <string>

using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;


void multiplyColumnStripe(const Matrix& A, const Vector& x, Vector& local_y, int startCol, int endCol) {
    int rows = A.size();
    for (int i = 0; i < rows; ++i) {
        double sum = 0.0;
        for (int j = startCol; j < endCol; ++j) {
            sum += A[i][j] * x[j];
        }
        local_y[i] = sum; // Записуємо у локальний вектор потоку
    }
}

// Розбиття робимо по стовпцях
Vector Task2(const std::string& matrixFile, const std::string& vectorFile, int numThreads) {
    std::ifstream matIn(matrixFile);
    std::ifstream vecIn(vectorFile);

    if (!matIn.is_open() || !vecIn.is_open()) {
        throw std::runtime_error("Помилка відкриття файлів даних. Перевірте наявність 'matrix.txt' та 'vector.txt'.");
    }

    int rows, cols, vecSize;
    matIn >> rows >> cols;
    vecIn >> vecSize;

    if (cols != vecSize) {
        throw std::invalid_argument("Кількість стовпців матриці не дорівнює розміру вектора.");
    }

    Matrix A(rows, Vector(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matIn >> A[i][j];
        }
    }

    Vector x(vecSize);
    for (int i = 0; i < vecSize; ++i) {
        vecIn >> x[i];
    }

    // Вектор векторів для збереження часткових результатів кожного потоку
    std::vector<Vector> partial_y(numThreads, Vector(rows, 0.0));
    std::vector<std::thread> threads;

    // Розподіл стовпців між потоками
    int colsPerThread = cols / numThreads;
    int startCol = 0;

    for (int i = 0; i < numThreads; ++i) {
        int endCol = (i == numThreads - 1) ? cols : startCol + colsPerThread;
        threads.emplace_back(multiplyColumnStripe, std::cref(A), std::cref(x), std::ref(partial_y[i]), startCol, endCol);
        startCol = endCol;
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // Зводимо (редукуємо) часткові результати в один кінцевий вектор y
    Vector y(rows, 0.0);
    for (int i = 0; i < numThreads; ++i) {
        for (int j = 0; j < rows; ++j) {
            y[j] += partial_y[i][j];
        }
    }

    return y;
}

int main() {
    const std::string matrixFile = "matrix.txt";
    const std::string vectorFile = "vector.txt";

    // Визначаємо кількість апаратних потоків системи
    unsigned int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 2;

    try {
        std::cout << "Зчитування файлів та запуск обчислень (Task2 - по стовпцях) на " << numThreads << " потоках...\n";
        Vector result = Task2(matrixFile, vectorFile, numThreads);

        std::cout << "Результуючий вектор:\n";
        for (double val : result) {
            std::cout << val << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Сталася помилка: " << e.what() << "\n";
        return 1;
    }

    return 0;
}