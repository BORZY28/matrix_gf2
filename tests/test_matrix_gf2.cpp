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
    size_t rank = B.rank();
    assert(rank <= 3);
    
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
        Matrix I_check = Matrix::identity(3, 2, 1);
        
        bool isIdentity = true;
        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                if (i == j && !check(i, j).isOne()) isIdentity = false;
                if (i != j && !check(i, j).isZero()) isIdentity = false;
            }
        }
        assert(isIdentity);
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

void testGF4() {
    std::cout << "Тестирование операций над GF(4) = GF(2^2)...\n";
    
    // GF(4): p=2, m=2, неприводимый полином x^2 + x + 1 = {1, 1, 1}
    std::vector<uint32_t> mod = {1, 1, 1};
    
    // Тест 1: Обратный элемент в GF(4)
    // alpha = x имеет обратный alpha+1 = x+1, т.к. x*(x+1) = x^2+x ≡ 1 (mod x^2+x+1) в GF(2)
    GFElement alpha({0, 1}, 2, 2, mod);   // x
    GFElement inv_alpha = alpha.inverse(); // должно быть x+1 = {1, 1}
    GFElement expected({1, 1}, 2, 2, mod);
    assert(inv_alpha == expected);
    
    // Тест 2: Умножение alpha * inv_alpha = 1
    GFElement product = alpha * inv_alpha;
    assert(product.isOne());
    
    // Тест 3: Прямой ход Гаусса над GF(4)
    // value=2 -> alpha=x={0,1}, value=1 -> 1={1,0}
    // Матрица [ alpha  1 ]  =  [ x    1 ]
    //         [  1    1 ]     [ 1    1 ]
    Matrix A({{2, 1}, {1, 1}}, 2, 2, mod);
    auto result = A.forwardGauss();
    assert(result.rank == 2);
    
    // Тест 4: RREF над GF(4)
    auto rref = A.reducedRowEchelonForm();
    assert(rref.rank == 2);
    // Проверяем, что ведущие элементы равны 1
    for (size_t pivotIdx = 0; pivotIdx < rref.pivotCols.size(); ++pivotIdx) {
        assert(rref.matrix(pivotIdx, rref.pivotCols[pivotIdx]).isOne());
    }
    
    // Тест 5: Обратная матрица над GF(4)
    assert(A.isInvertible());
    auto invA = A.inverse();
    assert(invA.has_value());
    Matrix check = A * (*invA);
    Matrix I = Matrix::identity(2, 2, 2, mod);
    assert(check == I);
    
    // Тест 6: Учебный режим над GF(4) (не должен зависать)
    auto edu_result = A.forwardGauss(true);
    assert(!edu_result.steps.empty());
    
    std::cout << "  ✓ Все тесты GF(4) пройдены\n";
}

void testGF11sq() {
    std::cout << "Тестирование операций над GF(11^2)...\n";

    // GF(11^2): p=11, m=2, неприводимый полином x^2 + x + 1 = {1, 1, 1}
    std::vector<uint32_t> mod = {1, 1, 1};

    // Тест 1: Обратный элемент в GF(11^2)
    // (5x+8)^{-1} = 10x+5, т.к. (5x+8)*(10x+5) ≡ 1 (mod x^2+x+1) над GF(11)
    GFElement elem({8, 5}, 11, 2, mod);       // 5x + 8
    GFElement expected({5, 10}, 11, 2, mod);  // 10x + 5
    GFElement inv = elem.inverse();
    assert(inv == expected);

    // Тест 2: Проверка elem * inv == 1
    GFElement product = elem * inv;
    assert(product.isOne());

    // Тест 3: Прямой ход Гаусса над матрицей 2x2 над GF(11^2)
    // Значения 15=4+11=x+4 и 9 выбраны так, что матрица невырожденная
    // и ведущий элемент первой строки не равен 1 — проверяем нормализацию.
    // Проверяем, что ведущие элементы равны 1 и ниже них — нули
    Matrix A({{15, 8}, {5, 9}}, 11, 2, mod);
    auto result = A.forwardGauss(false);
    assert(result.rank == 2);
    for (size_t pivotIdx = 0; pivotIdx < result.pivotCols.size(); ++pivotIdx) {
        assert(result.matrix(pivotIdx, result.pivotCols[pivotIdx]).isOne());
        for (size_t row = pivotIdx + 1; row < A.rows(); ++row) {
            assert(result.matrix(row, result.pivotCols[pivotIdx]).isZero());
        }
    }

    // Тест 4: Прямой ход Гаусса (учебный режим) над матрицей 5x5 над GF(11^2) — не должен зависать
    Matrix B = Matrix::random(5, 5, 11, 2, mod);
    auto edu = B.forwardGauss(true);
    assert(!edu.steps.empty());
    // Проверяем корректность: ведущие элементы == 1, нули ниже
    for (size_t pivotIdx = 0; pivotIdx < edu.pivotCols.size(); ++pivotIdx) {
        assert(edu.matrix(pivotIdx, edu.pivotCols[pivotIdx]).isOne());
        for (size_t row = pivotIdx + 1; row < B.rows(); ++row) {
            assert(edu.matrix(row, edu.pivotCols[pivotIdx]).isZero());
        }
    }

    std::cout << "  ✓ Все тесты GF(11^2) пройдены\n";
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
        testGF4();
        testGF11sq();
        testSubmatrix();
        testRowOperations();
        
        std::cout << "\n✓✓✓ ВСЕ ТЕСТЫ УСПЕШНО ПРОЙДЕНЫ ✓✓✓\n\n";
        return 0;
    } catch (const std::exception& e) {
        std::cout << "\n✗✗✗ ОШИБКА: " << e.what() << " ✗✗✗\n\n";
        return 1;
    }
}
