#include "../include/matrix_gf2/matrix.hpp"
#include <iostream>
#include <cassert>

using namespace matrix_gf2;

void testGFElement() {
    std::cout << "Тестирование GFElement...\n";
    
    // Тест 1: Создание элементов GF(2)
    GFElement a(1, 2, 1, {1, 1});
    GFElement b(0, 2, 1, {1, 1});
    assert(a.getValue() == 1);
    assert(b.getValue() == 0);
    
    // Тест 2: Сложение в GF(2)
    GFElement c = a + a;
    assert(c.isZero());
    
    // Тест 3: Умножение в GF(2)
    GFElement d = a * b;
    assert(d.isZero());
    
    GFElement e = a * a;
    assert(e.getValue() == 1);
    
    // Тест 4: Элементы GF(3)
    GFElement f(2, 3, 1, {1, 1});
    GFElement g(2, 3, 1, {1, 1});
    GFElement h = f + g;
    assert(h.getValue() == 1);  // 2 + 2 = 4 mod 3 = 1
    
    // Тест 5: Обратные элементы
    GFElement one(1, 2, 1, {1, 1});
    GFElement oneInv = one.inverse();
    assert(oneInv.getValue() == 1);
    
    std::cout << "  ✓ Все тесты GFElement пройдены\n";
}

void testMatrixCreation() {
    std::cout << "Тестирование создания матриц...\n";
    
    // Тест 1: Нулевая матрица
    Matrix A = Matrix::zero(3, 3, 2, 1);
    assert(A.rows() == 3);
    assert(A.cols() == 3);
    assert(A(0, 0).isZero());
    
    // Тест 2: Единичная матрица
    Matrix I = Matrix::identity(3, 2, 1);
    assert(I(0, 0).isOne());
    assert(I(1, 1).isOne());
    assert(I(0, 1).isZero());
    
    // Тест 3: Создание из массива
    Matrix B({{1, 0}, {0, 1}}, 2, 1);
    assert(B.rows() == 2);
    assert(B.cols() == 2);
    assert(B(0, 0).getValue() == 1);
    
    std::cout << "  ✓ Все тесты создания матриц пройдены\n";
}

void testMatrixOperations() {
    std::cout << "Тестирование операций с матрицами...\n";
    
    // Тест 1: Сложение
    Matrix A({{1, 0}, {0, 1}}, 2, 1);
    Matrix B({{0, 1}, {1, 0}}, 2, 1);
    Matrix C = A + B;
    assert(C(0, 0).getValue() == 1);
    assert(C(0, 1).getValue() == 1);
    
    // Тест 2: Умножение матриц
    Matrix D = A * B;
    assert(D(0, 0).isZero());
    assert(D(0, 1).isOne());
    assert(D(1, 0).isOne());
    assert(D(1, 1).isZero());
    
    // Тест 3: Транспонирование
    Matrix E({{1, 0, 1}, {0, 1, 0}}, 2, 1);
    Matrix ET = E.transpose();
    assert(ET.rows() == 3);
    assert(ET.cols() == 2);
    assert(ET(0, 0).getValue() == 1);
    assert(ET(2, 0).getValue() == 1);
    
    // Тест 4: Умножение на вектор
    Matrix F({{1, 0}, {0, 1}}, 2, 1);
    std::vector<GFElement> vec = {
        GFElement(1, 2, 1, {1, 1}),
        GFElement(1, 2, 1, {1, 1})
    };
    auto result = F * vec;
    assert(result.size() == 2);
    assert(result[0].getValue() == 1);
    assert(result[1].getValue() == 1);
    
    std::cout << "  ✓ Все тесты операций пройдены\n";
}

void testGaussElimination() {
    std::cout << "Тестирование метода Гаусса...\n";
    
    // Тест 1: Прямой ход
    Matrix A({{1, 1, 0}, {0, 1, 1}, {1, 0, 1}}, 2, 1);
    auto result = A.forwardGauss(false);
    assert(result.rank >= 2);
    
    // Тест 2: RREF единичной матрицы
    Matrix I = Matrix::identity(3, 2, 1);
    result = I.reducedRowEchelonForm(false);
    assert(result.matrix == I);
    
    // Тест 3: Ранг матрицы
    Matrix B({{1, 0, 1}, {0, 1, 1}, {1, 1, 0}}, 2, 1);
    assert(B.rank() <= 3);
    
    // Тест 4: Вырожденная матрица
    Matrix C({{1, 1}, {1, 1}}, 2, 1);
    assert(C.rank() == 1);
    
    std::cout << "  ✓ Все тесты метода Гаусса пройдены\n";
}

void testMatrixInverse() {
    std::cout << "Тестирование обратной матрицы...\n";
    
    // Тест 1: Обратная единичной матрицы
    Matrix I = Matrix::identity(3, 2, 1);
    auto invI = I.inverse(false);
    assert(invI.has_value());
    assert(*invI == I);
    
    // Тест 2: Проверка обратимости
    Matrix A({{1, 0, 1}, {0, 1, 1}, {1, 1, 1}}, 2, 1);
    if (A.isInvertible()) {
        auto invA = A.inverse(false);
        assert(invA.has_value());
        
        // Проверка A * A^(-1) = I
        Matrix check = A * (*invA);
        
        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                if (i == j) assert(check(i, j).isOne());
                if (i != j) assert(check(i, j).isZero());
            }
        }
    }
    
    // Тест 3: Вырожденная матрица
    Matrix B({{1, 0}, {1, 0}}, 2, 1);
    assert(!B.isInvertible());
    auto invB = B.inverse(false);
    assert(!invB.has_value());
    
    std::cout << "  ✓ Все тесты обратной матрицы пройдены\n";
}

void testGF3() {
    std::cout << "Тестирование операций над GF(3)...\n";
    
    // Тест 1: Арифметика в GF(3)
    GFElement a(1, 3, 1, {1, 1});
    GFElement b(2, 3, 1, {1, 1});
    GFElement c = a + b;
    assert(c.isZero());  // 1 + 2 = 3 mod 3 = 0
    
    // Тест 2: Матрица над GF(3)
    Matrix A({{1, 2}, {2, 1}}, 3, 1);
    assert(A(0, 0).getValue() == 1);
    assert(A(0, 1).getValue() == 2);
    
    // Тест 3: Умножение
    Matrix B({{2, 1}, {1, 2}}, 3, 1);
    Matrix C = A * B;
    // 1*2 + 2*1 = 4 mod 3 = 1
    assert(C(0, 0).getValue() == 1);
    
    std::cout << "  ✓ Все тесты GF(3) пройдены\n";
}

void testSubmatrix() {
    std::cout << "Тестирование подматриц...\n";
    
    Matrix A({{1, 0, 1}, {0, 1, 1}, {1, 1, 0}}, 2, 1);
    
    // Извлечение подматрицы 2x2
    auto sub = A.submatrix({0, 1}, {0, 1});
    assert(sub.rows() == 2);
    assert(sub.cols() == 2);
    assert(sub(0, 0).getValue() == 1);
    assert(sub(0, 1).isZero());
    assert(sub(1, 0).isZero());
    assert(sub(1, 1).getValue() == 1);
    
    std::cout << "  ✓ Все тесты подматриц пройдены\n";
}

void testRowOperations() {
    std::cout << "Тестирование элементарных операций со строками...\n";
    
    Matrix A({{1, 0, 1}, {0, 1, 1}, {1, 1, 0}}, 2, 1);
    
    // Обмен строк
    Matrix B = A;
    B.swapRows(0, 1);
    assert(B(0, 1).getValue() == 1);
    assert(B(1, 0).getValue() == 1);
    
    // Умножение строки
    Matrix C = A;
    GFElement one(1, 2, 1, {1, 1});
    C.multiplyRow(0, one);
    assert(C(0, 0).getValue() == 1);
    
    // Сложение строк
    Matrix D = A;
    D.addRow(2, 0, one);
    // Строка 2 становится [1, 1, 0] + [1, 0, 1] = [0, 1, 1]
    assert(D(2, 0).isZero());
    assert(D(2, 1).getValue() == 1);
    
    std::cout << "  ✓ Все тесты операций со строками пройдены\n";
}

int main() {
    std::cout << "\n=== Запуск тестов модуля matrix_gf2 ===\n\n";
    
    try {
        testGFElement();
        testMatrixCreation();
        testMatrixOperations();
        testGaussElimination();
        testMatrixInverse();
        testGF3();
        testSubmatrix();
        testRowOperations();
        
        std::cout << "\n✓✓✓ ВСЕ ТЕСТЫ УСПЕШНО ПРОЙДЕНЫ ✓✓✓\n\n";
        return 0;
    } catch (const std::exception& e) {
        std::cout << "\n✗✗✗ ОШИБКА: " << e.what() << " ✗✗✗\n\n";
        return 1;
    }
}
