#include "matrix_gf2/matrix.hpp"
#include <iostream>

using namespace matrix_gf2;

int main() {
    std::cout << "=== Примеры нахождения обратной матрицы ===\n\n";
    
    // Пример 1: Обратимая матрица над GF(2)
    std::cout << "1. Обратимая матрица 3x3 над GF(2)\n";
    Matrix A({{1, 0, 1},
              {0, 1, 1},
              {1, 1, 1}}, 2, 1);
    
    std::cout << "Исходная матрица A:\n" << A << "\n\n";
    
    std::cout << "Обратима: " << (A.isInvertible() ? "да" : "нет") << "\n";
    std::cout << "Ранг: " << A.rank() << "\n\n";
    
    auto invA = A.inverse(false);
    if (invA) {
        std::cout << "Обратная матрица A^(-1):\n" << *invA << "\n\n";
        
        // Проверка
        Matrix check = A * (*invA);
        std::cout << "Проверка A * A^(-1):\n" << check << "\n\n";
    }
    
    // Пример 2: Необратимая матрица
    std::cout << "2. Необратимая матрица (вырожденная)\n";
    Matrix B({{1, 0, 1},
              {0, 1, 1},
              {1, 1, 0}}, 2, 1);
    
    std::cout << "Матрица B:\n" << B << "\n\n";
    std::cout << "Обратима: " << (B.isInvertible() ? "да" : "нет") << "\n";
    std::cout << "Ранг: " << B.rank() << "\n\n";
    
    // Пример 3: Матрица над GF(3)
    std::cout << "3. Обратимая матрица над GF(3)\n";
    Matrix C({{1, 2, 0},
              {0, 1, 2},
              {2, 0, 1}}, 3, 1);
    
    std::cout << "Матрица C:\n" << C << "\n\n";
    std::cout << "Обратима: " << (C.isInvertible() ? "да" : "нет") << "\n\n";
    
    auto invC = C.inverse(false);
    if (invC) {
        std::cout << "Обратная матрица C^(-1):\n" << *invC << "\n\n";
        
        Matrix check = C * (*invC);
        std::cout << "Проверка C * C^(-1):\n" << check << "\n\n";
    }
    
    // Пример 4: Единичная матрица (сама себе обратная)
    std::cout << "4. Единичная матрица\n";
    Matrix I = Matrix::identity(3, 2, 1);
    std::cout << "Единичная матрица:\n" << I << "\n\n";
    
    auto invI = I.inverse(false);
    if (invI) {
        std::cout << "Обратная матрица:\n" << *invI << "\n\n";
    }
    
    // Пример 5: Поиск обратимой подматрицы
    std::cout << "5. Поиск обратимой подматрицы\n";
    Matrix D({{1, 0, 1, 0},
              {0, 1, 1, 0},
              {1, 1, 0, 1}}, 2, 1);
    
    std::cout << "Матрица D (не квадратная):\n" << D << "\n\n";
    
    auto subInfo = D.findInvertibleSubmatrix();
    if (subInfo) {
        std::cout << "Найдена обратимая подматрица!\n";
        std::cout << "Строки: ";
        for (auto r : subInfo->rows) std::cout << r << " ";
        std::cout << "\nСтолбцы: ";
        for (auto c : subInfo->cols) std::cout << c << " ";
        std::cout << "\n\nПодматрица:\n" << subInfo->submatrix << "\n\n";
        
        auto invSub = subInfo->submatrix.inverse(false);
        if (invSub) {
            std::cout << "Обратная подматрица:\n" << *invSub << "\n\n";
        }
    } else {
        std::cout << "Обратимая подматрица не найдена\n\n";
    }
    
    // Пример 6: Матрица 2x2
    std::cout << "6. Простая матрица 2x2 над GF(2)\n";
    Matrix E({{1, 1},
              {1, 0}}, 2, 1);
    
    std::cout << "Матрица E:\n" << E << "\n\n";
    
    auto invE = E.inverse(false);
    if (invE) {
        std::cout << "Обратная матрица E^(-1):\n" << *invE << "\n\n";
        
        Matrix check = E * (*invE);
        std::cout << "Проверка E * E^(-1):\n" << check << "\n\n";
    }
    
    return 0;
}
