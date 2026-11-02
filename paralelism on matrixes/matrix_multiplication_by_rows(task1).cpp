#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <stdexcept>
#include <functional>
#include <string>

using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;


void multiplyStripe(const Matrix& A, const Vector& x, Vector& y, int startRow, int endRow) {
    for (int i = startRow; i < endRow; ++i) {
        double sum = 0.0;
        for (size_t j = 0; j < x.size(); ++j) {
            sum += A[i][j] * x[j];
        }
        y[i] = sum;
    }
}

// розбиття робим по рядках
Vector Task1(const std::string& matrixFile, const std::string& vectorFile, int numThreads) {
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

    Vector y(rows, 0.0);
    std::vector<std::thread> threads;

    int rowsPerThread = rows / numThreads;
    int startRow = 0;

    for (int i = 0; i < numThreads; ++i) {
        int endRow = (i == numThreads - 1) ? rows : startRow + rowsPerThread;
        threads.emplace_back(multiplyStripe, std::cref(A), std::cref(x), std::ref(y), startRow, endRow);
        startRow = endRow;
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
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
        std::cout << "Зчитування файлів та запуск обчислень на " << numThreads << " потоках...\n";
        Vector result = Task1(matrixFile, vectorFile, numThreads);

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