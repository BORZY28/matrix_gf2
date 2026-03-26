#include "matrix_gf2/matrix.hpp"
#include <iostream>

using namespace matrix_gf2;

int main() {
    std::cout << "=== Пример базовых операций с матрицами над GF(2) ===\n\n";
    
    // Создание матриц над GF(2)
    std::cout << "1. Создание матрицы 3x3 над GF(2):\n";
    Matrix A({{1, 1, 0},
              {0, 0, 0},
              {1, 1, 0}}, 2, 1);
    std::cout << "Матрица A:\n" << A << "\n\n";
    
    std::cout << "2. Создание матрицы 3x3 над GF(2):\n";
    Matrix B({{1, 1, 0},
              {0, 1, 0},
              {1, 0, 1}}, 2, 1);
    std::cout << "Матрица B:\n" << B << "\n\n";
    
    // Сложение матриц
    std::cout << "3. Сложение матриц A + B:\n";
    Matrix C = A + B;
    std::cout << C << "\n\n";
    
    // Умножение матриц
    std::cout << "4. Умножение матриц A * B:\n";
    Matrix D = A * B;
    std::cout << D << "\n\n";
    
    // Транспонирование
    std::cout << "5. Транспонирование матрицы A:\n";
    Matrix AT = A.transpose();
    std::cout << AT << "\n\n";
    
    // Умножение матрицы на вектор
    std::cout << "6. Умножение матрицы A на вектор [1, 0, 1]:\n";
    std::vector<GFElement> vec = {
        GFElement(1, 2, 1, {1, 1}),
        GFElement(0, 2, 1, {1, 1}),
        GFElement(1, 2, 1, {1, 1})
    };
    auto result = A * vec;
    std::cout << "Результат: [";
    for (size_t i = 0; i < result.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << result[i];
    }
    std::cout << "]\n\n";
    
    // Ранг матрицы
    std::cout << "7. Ранг матрицы A: " << A.rank() << "\n\n";
    
    // Единичная матрица
    std::cout << "8. Единичная матрица 4x4 над GF(2):\n";
    Matrix I = Matrix::identity(4, 2, 1);
    std::cout << I << "\n\n";

    std::cout << "9. Приведение гаусом матрицы 7x7 над GF(11^3):\n";
    Matrix I2 = Matrix::random(7, 7, 11, 3, {1, 0, 1, 4})
    std::cout << "Исходная матрица:\n" << I2 << "\n\n";
    
    auto result2 = I2.reducedRowEchelonForm(true);
    std::cout << "\nШаги преобразования:\n";
    for (size_t i = 0; i < result2.steps.size(); ++i) {
        std::cout << (i + 1) << ". " << result2.steps[i] << "\n";
    }
    std::cout << "После приведения :\n" << result2.matrix << "\n";
    std::cout << "Ранг: " << result2.rank << "\n\n";

    
    // Работа с GF(3)
    std::cout << "=== Работа с матрицами над GF(3) ===\n\n";
    Matrix A3({{1, 2, 0},
               {0, 1, 2},
               {2, 0, 1}}, 3, 1);
    std::cout << "Матрица над GF(3):\n" << A3 << "\n\n";
    
    std::cout << "Ранг: " << A3.rank() << "\n";
    std::cout << "Обратима: " << (A3.isInvertible() ? "да" : "нет") << "\n\n";
    
    return 0;
}
