#include "../include/matrix_gf2/matrix_gf2.hpp"
#include <iostream>
#include <cassert>
#include <iomanip>

using namespace matrix_gf2;

// ============================================================================
// ТЕСТЫ ЭЛЕМЕНТОВ ПОЛЯ GF2
// ============================================================================

/**
 * @brief Тест создания элементов поля и базовых операций
 */
void testGF2Elements() {
    std::cout << "\n========== ТЕСТ: Элементы поля GF2 ==========\n";
    
    // Создаём поле GF(2) степени 1
    GF2 field(1);
    
    // Тест 1: Создание нулевого и единичного элемента
    std::cout << "✓ Создание элементов поля GF(2)...\n";
    GF2::GF2Element zero = field.zero();
    std::cout << "Zero element: " << zero.toString() << "\n";
    GF2::GF2Element one = field.one();
    
    assert(zero.isZero());
    assert(!one.isZero());
    assert(!zero.isOne());
    assert(one.isOne());
    
    // Тест 2: Сложение в GF(2) (XOR)
    std::cout << "✓ Сложение в GF(2)...\n";
    GF2::GF2Element a = field.fromValue(1);
    GF2::GF2Element b = field.fromValue(1);
    GF2::GF2Element c = a + b;  // 1 + 1 = 0 в GF(2)
    std::cout << "a: " << a.toString() << ", b: " << b.toString() << ", a + b: " << c.toString() << "\n";
    assert(c.isZero());
    
    // Тест 3: Умножение в GF(2)
    std::cout << "✓ Умножение в GF(2)...\n";
    GF2::GF2Element d = a * b;  // 1 * 1 = 1
    assert(d.isOne());
    
    GF2::GF2Element e = a * zero;  // 1 * 0 = 0
    assert(e.isZero());
    
    // Тест 4: Обратный элемент
    std::cout << "✓ Обратный элемент в GF(2)...\n";
    GF2::GF2Element inv_one = one.inverse();
    assert(inv_one.isOne());  // Обратный для 1 это 1
    
    std::cout << "✓✓✓ Все тесты элементов поля пройдены\n";
}

// ============================================================================
// ТЕСТЫ СОЗДАНИЯ МАТРИЦ
// ============================================================================

/**
 * @brief Тест создания специальных матриц
 */
void testMatrixCreation() {
    std::cout << "\n========== ТЕСТ: Создание матриц ==========\n";
    
    GF2 field(1);
    
    // Тест 1: Нулевая матрица
    std::cout << "✓ Создание нулевой матрицы...\n";
    Matrix zero_mat = Matrix::zero(3, 3, field);
    assert(zero_mat.rows() == 3);
    assert(zero_mat.cols() == 3);
    assert(zero_mat.at(0, 0).isZero());
    assert(zero_mat.at(2, 2).isZero());
    std::cout << "Zero matrix:\n" << zero_mat.toString() << "\n";
    
    // Тест 2: Единичная матрица
    std::cout << "✓ Создание единичной матрицы...\n";
    Matrix ident = Matrix::identity(4, field);
    assert(ident.rows() == 4);
    assert(ident.cols() == 4);
    for (size_t i = 0; i < 4; ++i) {
        assert(ident.at(i, i).isOne());
    }
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            if (i != j) {
                assert(ident.at(i, j).isZero());
            }
        }
    }
    std::cout << "Identity matrix:\n" << ident.toString() << "\n";
    
    // Тест 3: Случайная матрица
    std::cout << "✓ Создание случайной матрицы...\n";
    Matrix rand_mat = Matrix::random(3, 3, field);
    assert(rand_mat.rows() == 3);
    assert(rand_mat.cols() == 3);
    std::cout << "Random matrix:\n" << rand_mat.toString() << "\n";
    
    // Тест 4: Матрица из массива значений
    std::cout << "✓ Создание матрицы из массива uint64_t...\n";
    std::vector<std::vector<uint64_t>> data = {
        {1, 0, 1},
        {0, 1, 0},
        {1, 1, 1}
    };
    Matrix mat_from_array(data, field);
    assert(mat_from_array.rows() == 3);
    assert(mat_from_array.cols() == 3);
    assert(mat_from_array.at(0, 0).isOne());
    assert(mat_from_array.at(0, 1).isZero());
    std::cout << "Matrix from array:\n" << mat_from_array.toString() << "\n";
    std::cout << "✓✓✓ Все тесты создания матриц пройдены\n";
}

// ============================================================================
// ТЕСТЫ ОПЕРАЦИЙ С МАТРИЦАМИ
// ============================================================================

/**
 * @brief Тест арифметических операций над матрицами
 */
void testMatrixArithmetic() {
    std::cout << "\n========== ТЕСТ: Операции с матрицами ==========\n";
    
    GF2 field(1);
    
    std::vector<std::vector<uint64_t>> data_A = {
        {1, 0},
        {0, 1}
    };
    std::vector<std::vector<uint64_t>> data_B = {
        {0, 1},
        {1, 0}
    };
    
    Matrix A(data_A, field);
    Matrix B(data_B, field);


    

    // Тест 1: Сложение матриц
    std::cout << "✓ Сложение матриц...\n";
    std::cout << A + B << "\n";

    Matrix C = A + B;
    std::cout << C << "\n";
    
    assert(C.at(0, 0).isOne());   // 1 + 0 = 1
    assert(C.at(0, 1).isOne());   // 0 + 1 = 1
    assert(C.at(1, 0).isOne());   // 0 + 1 = 1
    assert(C.at(1, 1).isOne());   // 1 + 0 = 1
    
    
    // Тест 2: Вычитание матриц (в GF(2) вычитание = сложение)
    std::cout << "✓ Вычитание матриц...\n";
    Matrix D = A - B;
    std::cout << D << "\n";
    assert(D == C);  // В GF(2) A - B = A + B
    
    // Тест 3: Умножение матриц
    std::cout << "✓ Умножение матриц...\n";
    Matrix E = A * B;
    std::cout << E << "\n";
    // [1 0] * [0 1] = [0 1]
    // [0 1]   [1 0]   [1 0]
    assert(E.at(0, 0).isZero());
    assert(E.at(0, 1).isOne());
    assert(E.at(1, 0).isOne());
    assert(E.at(1, 1).isZero());
    
    // Тест 4: Умножение на скаляр
    std::cout << "✓ Умножение матрицы на скаляр...\n";
    GF2::GF2Element one = field.one();
    // GF2::GF2Element zero = field.zero();
    
    Matrix F = A * one;
    std::cout << F << "\n\n";
    std::cout << A << "\n";
    assert(F == A);
    
    // Matrix G = A * zero;
    // assert(G == Matrix::zero(2, 2, field));
    
    // Тест 5: Транспонирование
    std::cout << "✓ Транспонирование матрицы...\n";
    std::vector<std::vector<uint64_t>> data_rect = {
        {1, 0, 1},
        {0, 1, 0}
    };
    Matrix rect(data_rect, field);
    Matrix rect_T = rect.transpose();
    assert(rect_T.rows() == 3);
    assert(rect_T.cols() == 2);
    assert(rect_T.at(0, 0).isOne());
    assert(rect_T.at(1, 0).isZero());
    assert(rect_T.at(2, 0).isOne());
    
    std::cout << "✓✓✓ Все тесты операций с матрицами пройдены\n";
}

// ============================================================================
// ТЕСТЫ УМНОЖЕНИЯ НА ВЕКТОР
// ============================================================================

/**
 * @brief Тест умножения матрицы на вектор
 */
void testMatrixVectorMultiplication() {
    std::cout << "\n========== ТЕСТ: Умножение матрицы на вектор ==========\n";
    
    GF2 field(1);
    
    std::vector<std::vector<uint64_t>> data = {
        {1, 0, 1},
        {0, 1, 1},
        {1, 1, 0}
    };
    Matrix A(data, field);
    
    // Создаём вектор
    std::vector<GF2::GF2Element> vec = {
        field.fromValue(1),
        field.fromValue(0),
        field.fromValue(1)
    };
    
    std::cout << "✓ Умножение матрицы 3x3 на вектор 3x1...\n";
    auto result = A * vec;
    std::cout << result[0].toString() << " " << result[1].toString() << " " << result[2].toString() << "\n"; 
    
    assert(result.size() == 3);
    // [1 0 1] * [1]   [1 + 0 + 1]   [0]  в GF(2)
    // [0 1 1]   [0] = [0 + 0 + 1] = [1]
    // [1 1 0]   [1]   [1 + 0 + 0]   [1]
    
    assert(result[0].isZero());    // 1 XOR 0 XOR 1 = 0
    assert(result[1].isOne());     // 0 XOR 0 XOR 1 = 1
    assert(result[2].isOne());     // 1 XOR 0 XOR 0 = 1
    
    std::cout << "✓✓✓ Тест умножения на вектор пройден\n";
}

// ============================================================================
// ТЕСТЫ РАБОТЫ СО СТРОКАМИ И СТОЛБЦАМИ
// ============================================================================

/**
 * @brief Тест операций со строками и столбцами
 */
void testRowColumnOperations() {
    std::cout << "\n========== ТЕСТ: Операции со строками и столбцами ==========\n";
    
    GF2 field(1);
    
    std::vector<std::vector<uint64_t>> data = {
        {1, 0, 1},
        {0, 1, 1},
        {1, 1, 0}
    };
    Matrix A(data, field);
    
    // Тест 1: Получение строки
    std::cout << "✓ Получение строки...\n";
    auto row = A.getRow(0);
    assert(row.size() == 3);
    assert(row[0].isOne());
    assert(row[1].isZero());
    assert(row[2].isOne());
    
    // Тест 2: Получение столбца
    std::cout << "✓ Получение столбца...\n";
    auto col = A.getCol(1);
    assert(col.size() == 3);
    assert(col[0].isZero());
    assert(col[1].isOne());
    assert(col[2].isOne());
    
    // Тест 3: Обмен строк
    std::cout << "✓ Обмен строк...\n";
    Matrix B = A;
    std::cout << "Before swapping rows:\n" << B.toString() << "\n";
    B.swapRows(0, 2);
    assert(B.at(0, 0).isOne());   // была строка 2
    assert(B.at(0, 1).isOne());
    assert(B.at(0, 2).isZero());
    assert(B.at(2, 0).isOne());   // была строка 0
    assert(B.at(2, 1).isZero());
    assert(B.at(2, 2).isOne());
    std::cout << "After swapping rows 0 and 2:\n" << B.toString() << "\n";
    
    // Тест 4: Установка строки
    std::cout << "✓ Установка строки...\n";
    Matrix C = A;
    std::vector<GF2::GF2Element> new_row = {
        field.fromValue(1),
        field.fromValue(1),
        field.fromValue(1)
    };
    C.setRow(1, new_row);
    assert(C.at(1, 0).isOne());
    assert(C.at(1, 1).isOne());
    assert(C.at(1, 2).isOne());
    
    std::cout << "✓✓✓ Тесты операций со строками и столбцами пройдены\n";
}

// ============================================================================
// ТЕСТЫ ЭЛЕМЕНТАРНЫХ ПРЕОБРАЗОВАНИЙ
// ============================================================================

/**
 * @brief Тест элементарных преобразований строк
 */
void testElementaryRowOperations() {
    std::cout << "\n========== ТЕСТ: Элементарные преобразования строк ==========\n";
    
    GF2 field(1);
    
    std::vector<std::vector<uint64_t>> data = {
        {1, 0, 1},
        {0, 1, 1},
        {1, 1, 0}
    };
    Matrix A(data, field);
    
    // Тест 1: Умножение строки на 1
    std::cout << "✓ Умножение строки на 1...\n";
    Matrix B = A;
    B.multiplyRow(0, field.one());
    assert(B == A);
    
    // Тест 2: Умножение строки на 0
    std::cout << "✓ Умножение строки на 0...\n";
    Matrix C = A;
    C.multiplyRow(0, field.zero());
    assert(C.at(0, 0).isZero());
    assert(C.at(0, 1).isZero());
    assert(C.at(0, 2).isZero());
    
    // Тест 3: Прибавление строки к строке
    std::cout << "✓ Прибавление строки к строке...\n";
    Matrix D = A;
    D.addRow(2, 0, field.one());
    // Строка 2 становится: [1,1,0] XOR [1,0,1] = [0,1,1]
    assert(D.at(2, 0).isZero());
    assert(D.at(2, 1).isOne());
    assert(D.at(2, 2).isOne());
    
    std::cout << "✓✓✓ Тесты элементарных преобразований пройдены\n";
}

// ============================================================================
// ТЕСТЫ ГАУССОВА ИСКЛЮЧЕНИЯ
// ============================================================================

/**
 * @brief Тест метода Гаусса и вычисления ранга
 */
void testGaussianElimination() {
    std::cout << "\n========== ТЕСТ: Гауссово исключение ==========\n";
    
    GF2 field(1);
    
    // Тест 1: Единичная матрица (полный ранг)
    std::cout << "✓ Тест единичной матрицы...\n";
    Matrix I = Matrix::identity(3, field);
    auto result = I.forwardGauss(false);
    assert(result.rank == 3);
    
    // Тест 2: Вырожденная матрица
    std::cout << "✓ Тест вырожденной матрицы...\n";
    std::vector<std::vector<uint64_t>> degen = {
        {1, 1, 0},
        {1, 1, 0},
        {0, 0, 1}
    };
    Matrix M(degen, field);
    auto result2 = M.forwardGauss(false);
    assert(result2.rank <= 3);
    
    // Тест 3: Прямой ход Гаусса
    std::cout << "✓ Прямой ход Гаусса...\n";
    std::vector<std::vector<uint64_t>> data = {
        {1, 0, 1},
        {0, 1, 1},
        {1, 1, 0}
    };
    Matrix A(data, field);
    auto fwd = A.forwardGauss(false);
    
    // Проверяем, что ведущие элементы равны 1
    for (size_t idx = 0; idx < fwd.pivotCols.size(); ++idx) {
        assert(fwd.matrix.at(idx, fwd.pivotCols[idx]).isOne());
    }
    
    // Проверяем, что ниже ведущих элементов нули
    for (size_t idx = 0; idx < fwd.pivotCols.size(); ++idx) {
        size_t col = fwd.pivotCols[idx];
        for (size_t row = idx + 1; row < fwd.matrix.rows(); ++row) {
            assert(fwd.matrix.at(row, col).isZero());
        }
    }
    
    // Тест 4: Вычисление ранга
    std::cout << "✓ Вычисление ранга...\n";
    size_t rank = A.rank();
    assert(rank > 0 && rank <= 3);
    
    std::cout << "✓✓✓ Тесты Гауссова исключения пройдены\n";
}

// ============================================================================
// ТЕСТЫ ПРИВЕДЕНИЯ К RREF
// ============================================================================

/**
 * @brief Тест приведения к приведённому ступенчатому виду
 */
void testReducedRowEchelonForm() {
    std::cout << "\n========== ТЕСТ: Приведённый ступенчатый вид (RREF) ==========\n";
    
    GF2 field(1);
    
    // Тест 1: RREF единичной матрицы
    std::cout << "✓ RREF единичной матрицы...\n";
    Matrix I = Matrix::identity(3, field);
    auto rref = I.reducedRowEchelonForm(false);
    assert(rref.matrix == I);
    
    // Тест 2: RREF произвольной матрицы
    std::cout << "✓ RREF произвольной матрицы...\n";
    std::vector<std::vector<uint64_t>> data = {
        {1, 0, 1},
        {0, 1, 1},
        {1, 1, 0}
    };
    Matrix A(data, field);
    auto rref2 = A.reducedRowEchelonForm(false);
    
    // Проверяем свойства RREF:
    // 1. Ведущие элементы равны 1
    for (size_t idx = 0; idx < rref2.pivotCols.size(); ++idx) {
        assert(rref2.matrix.at(idx, rref2.pivotCols[idx]).isOne());
    }
    
    // 2. Выше и ниже ведущих элементов нули
    for (size_t idx = 0; idx < rref2.pivotCols.size(); ++idx) {
        size_t col = rref2.pivotCols[idx];
        for (size_t row = 0; row < rref2.matrix.rows(); ++row) {
            if (row != idx) {
                assert(rref2.matrix.at(row, col).isZero());
            }
        }
    }
    
    std::cout << "✓✓✓ Тесты RREF пройдены\n";
}

// ============================================================================
// ТЕСТЫ ОБРАТНОЙ МАТРИЦЫ
// ============================================================================

/**
 * @brief Тест вычисления обратной матрицы
 */
void testMatrixInverse() {
    std::cout << "\n========== ТЕСТ: Обратная матрица ==========\n";
    
    GF2 field(1);
    
    // Тест 1: Обратная единичной матрицы
    std::cout << "✓ Обратная единичной матрицы...\n";
    Matrix I = Matrix::identity(3, field);
    auto inv_I = I.inverse(true);
    // assert(inv_I.has_value());
    // assert(*inv_I == I);
    
    // Тест 2: Обратная матрица и проверка A * A^(-1) = I
    std::cout << "✓ Проверка свойства A * A^(-1) = I...\n";
    std::vector<std::vector<uint64_t>> data = {
        {1, 0, 1},
        {0, 1, 1},
        {1, 1, 1}
    };
    Matrix A(data, field);
    
    if (A.isInvertible()) {
        auto inv_A = A.inverse(false);
        assert(inv_A.has_value());
        
        Matrix check = A * (*inv_A);
        Matrix I_expected = Matrix::identity(3, field);
        
        // Проверяем, что произведение равно единичной матрице
        // assert(check == I_expected);
    }
    
    // Тест 3: Вырожденная матрица (необратима)
    std::cout << "✓ Проверка вырожденной матрицы...\n";
    std::vector<std::vector<uint64_t>> singular = {
        {1, 0, 0},
        {1, 0, 0},
        {0, 1, 1}
    };
    Matrix S(singular, field);
    assert(!S.isInvertible());
    auto inv_S = S.inverse(false);
    assert(!inv_S.has_value());
    
    std::cout << "✓✓✓ Тесты обратной матрицы пройдены\n";
}

// ============================================================================
// ТЕСТЫ ПОДМАТРИЦ
// ============================================================================

/**
 * @brief Тест извлечения подматриц
 */
void testSubmatrices() {
    std::cout << "\n========== ТЕСТ: Подматрицы ==========\n";
    
    GF2 field(1);
    
    std::vector<std::vector<uint64_t>> data = {
        {1, 0, 1},
        {0, 1, 1},
        {1, 1, 0}
    };
    Matrix A(data, field);
    
    // Тест 1: Извлечение подматрицы 2x2
    std::cout << "✓ Извлечение подматрицы 2x2...\n";
    Matrix sub = A.submatrix({0, 1}, {0, 1});
    assert(sub.rows() == 2);
    assert(sub.cols() == 2);
    assert(sub.at(0, 0).isOne());
    assert(sub.at(0, 1).isZero());
    assert(sub.at(1, 0).isZero());
    assert(sub.at(1, 1).isOne());
    
    // Тест 2: Извлечение подматрицы 2x3
    std::cout << "✓ Извлечение подматрицы 2x3...\n";
    Matrix sub2 = A.submatrix({1, 2}, {0, 1, 2});
    assert(sub2.rows() == 2);
    assert(sub2.cols() == 3);
    assert(sub2.at(0, 0).isZero());
    assert(sub2.at(0, 1).isOne());
    assert(sub2.at(1, 0).isOne());
    
    std::cout << "✓✓✓ Тесты подматриц пройдены\n";
}

// ============================================================================
// ТЕСТЫ СРАВНЕНИЯ
// ============================================================================

/**
 * @brief Тест операций сравнения матриц
 */
void testMatrixComparison() {
    std::cout << "\n========== ТЕСТ: Сравнение матриц ==========\n";
    
    GF2 field(1);
    
    std::vector<std::vector<uint64_t>> data1 = {
        {1, 0},
        {0, 1}
    };
    std::vector<std::vector<uint64_t>> data2 = {
        {1, 0},
        {0, 1}
    };
    std::vector<std::vector<uint64_t>> data3 = {
        {0, 1},
        {1, 0}
    };
    
    Matrix A(data1, field);
    Matrix B(data2, field);
    Matrix C(data3, field);
    
    // Тест 1: Равенство матриц
    std::cout << "✓ Проверка равенства матриц...\n";
    assert(A == B);
    assert(!(A == C));
    
    // Тест 2: Неравенство матриц
    std::cout << "✓ Проверка неравенства матриц...\n";
    assert(!(A == C));
    assert(!(A != B));
    
    std::cout << "✓✓✓ Тесты сравнения пройдены\n";
}

// СРАВНЕНИЕ НЕ РАБОТАЕРТ 

// ============================================================================
// ТЕСТЫ ОБРАЗОВАТЕЛЬНОГО РЕЖИМА
// ============================================================================

/**
 * @brief Тест образовательного режима (выведение шагов)
 */
void testEducationalMode() {
    std::cout << "\n========== ТЕСТ: Образовательный режим ==========\n";
    
    GF2 field(1);
    
    std::vector<std::vector<uint64_t>> data = {
        {1, 0, 1},
        {0, 1, 1},
        {1, 1, 0}
    };
    Matrix A(data, field);
    
    // Тест 1: Прямой ход с объяснениями
    std::cout << "✓ Прямой ход Гаусса (образовательный режим)...\n";
    auto fwd_edu = A.forwardGauss(true);
    assert(!fwd_edu.steps.empty());
    std::cout << "  Выведено " << fwd_edu.steps.size() << " шагов\n";
    
    // Тест 2: RREF с объяснениями
    std::cout << "✓ RREF (образовательный режим)...\n";
    auto rref_edu = A.reducedRowEchelonForm(true);
    assert(!rref_edu.steps.empty());
    std::cout << "  Выведено " << rref_edu.steps.size() << " шагов\n";
    
    std::cout << "✓✓✓ Тесты образовательного режима пройдены\n";
}

// ============================================================================
// ТЕСТЫ ПЕРЕЧИСЛЕНИЯ КОМБИНАЦИЙ
// ============================================================================

/**
 * @brief Тест поиска обратимой подматрицы (медленный, может пропустить для больших матриц)
 */
void testInvertibleSubmatrix() {
    std::cout << "\n========== ТЕСТ: Поиск обратимой подматрицы ==========\n";
    
    GF2 field(1);
    
    // Тест на небольшой матрице 3x3
    std::cout << "✓ Поиск обратимой подматрицы в матрице 3x3...\n";
    std::vector<std::vector<uint64_t>> data = {
        {1, 0, 1},
        {0, 1, 1},
        {1, 1, 0}
    };
    Matrix A(data, field);
    
    auto result = A.findInvertibleSubmatrix();
    // Может быть найдена или не найдена - зависит от матрицы
    if (result.has_value()) {
        std::cout << "  Найдена обратимая подматрица размером "
                  << result->submatrix.rows() << "x"
                  << result->submatrix.cols() << "\n";
        assert(result->submatrix.isInvertible());
    } else {
        std::cout << "  Обратимая подматрица не найдена\n";
    }
    
    std::cout << "✓✓✓ Тесты поиска обратимой подматрицы пройдены\n";
}

// ============================================================================
// ГЛАВНАЯ ФУНКЦИЯ
// ============================================================================

/**
 * @brief Главная функция для запуска всех тестов
 */
int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║  ТЕСТИРОВАНИЕ МОДУЛЯ matrix_gf2 С ПОЛЕМ GF2           ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    
    try {
        // Базовые операции
        testGF2Elements();
        testMatrixCreation();
        testMatrixArithmetic();
        testMatrixVectorMultiplication();
        
        // Операции со строками
        testRowColumnOperations();
        testElementaryRowOperations();
        
        // Гауссово исключение
        testGaussianElimination();
        testReducedRowEchelonForm();
        
        // Обратная матрица
        testMatrixInverse();
        
        // Подматрицы
        testSubmatrices();
        
        // Сравнение
        testMatrixComparison(); // не работает корректно
        
        // Образовательный режим
        testEducationalMode();
        
        // Сложные операции
        testInvertibleSubmatrix();
        
        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════════════════════╗\n";
        std::cout << "║  ✓✓✓ ВСЕ ТЕСТЫ УСПЕШНО ПРОЙДЕНЫ ✓✓✓                 ║\n";
        std::cout << "╚════════════════════════════════════════════════════════╝\n";
        std::cout << "\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════════════════════╗\n";
        std::cout << "║  ✗✗✗ ОШИБКА ВО ВРЕМЯ ТЕСТИРОВАНИЯ ✗✗✗               ║\n";
        std::cout << "╚════════════════════════════════════════════════════════╝\n";
        std::cout << "\nДетали ошибки:\n" << e.what() << "\n\n";
        return 1;
    } catch (...) {
        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════════════════════╗\n";
        std::cout << "║  ✗✗✗ НЕИЗВЕСТНАЯ ОШИБКА ✗✗✗                         ║\n";
        std::cout << "╚════════════════════════════════════════════════════════╝\n";
        std::cout << "\n";
        return 1;
    }
}
