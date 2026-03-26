#include "matrix_gf2/matrix.hpp"
#include <iostream>

using namespace matrix_gf2;

int main() {
    std::cout << "=== Примеры метода Гаусса ===\n\n";
    
    // Пример 1: Прямой ход Гаусса
    std::cout << "1. Прямой ход Гаусса (приведение к ступенчатому виду)\n";
    Matrix A({{1, 2, 3},
              {2, 4, 5},
              {3, 5, 6}}, 7, 1);
    
    std::cout << "Исходная матрица:\n" << A << "\n\n";
    
    auto result = A.forwardGauss(false);
    std::cout << "После прямого хода Гаусса:\n" << result.matrix << "\n";
    std::cout << "Ранг: " << result.rank << "\n";
    std::cout << "Столбцы с ведущими элементами: ";
    for (auto col : result.pivotCols) {
        std::cout << col << " ";
    }
    std::cout << "\n\n";
    
    // Пример 2: Приведение к RREF
    std::cout << "2. Приведение к приведённому ступенчатому виду (RREF)\n";
    Matrix B({{1, 0, 1, 0},
              {0, 1, 1, 0},
              {1, 1, 0, 1}}, 2, 1);
    
    std::cout << "Исходная матрица:\n" << B << "\n\n";
    
    result = B.reducedRowEchelonForm(false);
    std::cout << "После приведения к RREF:\n" << result.matrix << "\n";
    std::cout << "Ранг: " << result.rank << "\n\n";
    
    // Пример 3: Решение системы линейных уравнений
    std::cout << "3. Система линейных уравнений над GF(2)\n";
    std::cout << "   x + z = 1\n";
    std::cout << "   y + z = 0\n";
    std::cout << "   x + y = 1\n\n";
    
    Matrix sys({{1, 0, 1, 1},
                {0, 1, 1, 0},
                {1, 1, 0, 1}}, 2, 1);
    
    std::cout << "Расширенная матрица:\n" << sys << "\n\n";
    
    result = sys.reducedRowEchelonForm(false);
    std::cout << "После решения:\n" << result.matrix << "\n";
    std::cout << "Решение: x = " << result.matrix(0, 3) << ", "
              << "y = " << result.matrix(1, 3) << ", "
              << "z = " << result.matrix(2, 3) << "\n\n";
    
    // Пример 4: Большая матрица
    std::cout << "4. Большая матрица над GF(2)\n";
    Matrix large({{1, 1, 0, 1, 0},
                  {0, 1, 1, 0, 1},
                  {1, 0, 1, 1, 0},
                  {0, 1, 0, 1, 1}}, 2, 1);
    
    std::cout << "Исходная матрица:\n" << large << "\n\n";
    
    result = large.forwardGauss(false);
    std::cout << "После прямого хода:\n" << result.matrix << "\n";
    std::cout << "Ранг: " << result.rank << "\n\n";
    
    result = large.reducedRowEchelonForm(false);
    std::cout << "После приведения к RREF:\n" << result.matrix << "\n\n";
    
    // Пример 5: Матрица над GF(3)
    std::cout << "5. Матрица над GF(3)\n";
    Matrix gf3({{1, 2, 1},
                {2, 1, 2},
                {1, 1, 0}}, 3, 1);
    
    std::cout << "Исходная матрица:\n" << gf3 << "\n\n";
    
    result = gf3.reducedRowEchelonForm(false);
    std::cout << "После приведения к RREF:\n" << result.matrix << "\n";
    std::cout << "Ранг: " << result.rank << "\n\n";
    
    return 0;
}
