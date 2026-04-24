#include "../include/matrix_gf2/matrix.hpp"
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>
#include <iostream>

namespace matrix_gf2 {

// ============================================================================
// КОНСТРУКТОРЫ
// ============================================================================

/**
 * @brief Конструктор нулевой матрицы размера rows x cols
 * 
 * Создаёт матрицу размером rows строк на cols столбцов, заполненную нулевыми
 * элементами поля GF(p^m). Инициализирует параметры поля и внутреннее хранилище.
 */
Matrix::Matrix(size_t rows, size_t cols, uint32_t p, uint32_t m,
               const std::vector<uint32_t>& modulus)
    : rows_(rows), cols_(cols), p_(p), m_(m), modulus_(modulus) {
    // Выделяем память для всех строк матрицы
    data_.resize(rows);
    // Для каждой строки создаём нулевые элементы поля
    for (auto& row : data_) {
        row.resize(cols, GFElement(p, m, modulus));
    }
}

/**
 * @brief Конструктор из двумерного массива целых чисел
 * 
 * Создаёт матрицу из двумерного вектора целых чисел, где каждое число
 * преобразуется в элемент поля GF(p^m). Размеры определяются размерами входного массива.
 */
Matrix::Matrix(const std::vector<std::vector<uint32_t>>& data,
               uint32_t p, uint32_t m,
               const std::vector<uint32_t>& modulus)
    : rows_(data.size()), cols_(data.empty() ? 0 : data[0].size()),
      p_(p), m_(m), modulus_(modulus) {
    
    // Выделяем память для строк
    data_.resize(rows_);
    for (size_t i = 0; i < rows_; ++i) {
        data_[i].resize(cols_);
        // Преобразуем каждое целое число в элемент поля
        for (size_t j = 0; j < cols_ && j < data[i].size(); ++j) {
            data_[i][j] = GFElement(data[i][j], p, m, modulus);
        }
    }
}

/**
 * @brief Конструктор из двумерного массива элементов поля
 * 
 * Создаёт матрицу прямо из элементов поля GF(p^m). Параметры поля
 * извлекаются из первого элемента.
 */
Matrix::Matrix(const std::vector<std::vector<GFElement>>& data)
    : rows_(data.size()), cols_(data.empty() ? 0 : data[0].size()) {
    
    // Если матрица не пустая, извлекаем параметры поля из первого элемента
    if (!data.empty() && !data[0].empty()) {
        p_ = data[0][0].getP();
        m_ = data[0][0].getM();
        modulus_ = {1, 1};  // Значение по умолчанию для простых полей
    }
    
    // Копируем данные
    data_ = data;
}

// ============================================================================
// СТАТИЧЕСКИЕ МЕТОДЫ СОЗДАНИЯ
// ============================================================================

/**
 * @brief Создаёт единичную матрицу размера n x n
 * 
 * Единичная матрица - это матрица с единицами на главной диагонали
 * и нулями везде остальное. Используется в матричной алгебре.
 */
Matrix Matrix::identity(size_t n, uint32_t p, uint32_t m,
                       const std::vector<uint32_t>& modulus) {
    Matrix result(n, n, p, m, modulus);
    // Устанавливаем единицы на диагонали
    for (size_t i = 0; i < n; ++i) {
        result.data_[i][i] = GFElement(1, p, m, modulus);
    }
    return result;
}

/**
 * @brief Создаёт нулевую матрицу размера rows x cols
 * 
 * Нулевая матрица заполнена только нулями поля.
 * По сути - это просто обёртка над обычным конструктором.
 */
Matrix Matrix::zero(size_t rows, size_t cols, uint32_t p, uint32_t m,
                   const std::vector<uint32_t>& modulus) {
    return Matrix(rows, cols, p, m, modulus);
}

/**
 * @brief Создаёт случайную матрицу размера rows x cols
 * 
 * ��енерирует матрицу со случайными элементами поля GF(p^m).
 * Используется для тестирования и демонстрации.
 */
Matrix Matrix::random(size_t rows, size_t cols, uint32_t p, uint32_t m,
                     const std::vector<uint32_t>& modulus) {
    Matrix result(rows, cols, p, m, modulus);
    
    // Инициализируем генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Вычисляем максимальное значение элемента в поле (p^m)
    uint32_t max_val = 1;
    for (uint32_t i = 0; i < m; ++i) {
        max_val *= p;
    }
    
    // Создаём распределение на диапазон [0, p^m - 1]
    std::uniform_int_distribution<uint32_t> dis(0, max_val - 1);
    
    // Заполняем матрицу случайными элементами
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result.data_[i][j] = GFElement(dis(gen), p, m, modulus);
        }
    }
    return result;
}

// ============================================================================
// ДОСТУП К ЭЛЕМЕНТАМ
// ============================================================================

/**
 * @brief Доступ к элементу матрицы по индексам (с проверкой границ)
 * 
 * Возвращает ссылку на элемент с индексами (i, j). Проверяет,
 * что индексы не выходят за границы матрицы.
 */
GFElement& Matrix::at(size_t i, size_t j) {
    if (i >= rows_ || j >= cols_) {
        throw std::out_of_range("Индекс вне границ матрицы");
    }
    return data_[i][j];
}

/**
 * @brief Константный доступ к элементу матрицы
 */
const GFElement& Matrix::at(size_t i, size_t j) const {
    if (i >= rows_ || j >= cols_) {
        throw std::out_of_range("Индекс вне границ матрицы");
    }
    return data_[i][j];
}

/**
 * @brief Оператор доступа с проверкой границ
 */
GFElement& Matrix::operator()(size_t i, size_t j) {
    return at(i, j);
}

/**
 * @brief Константный оператор доступа
 */
const GFElement& Matrix::operator()(size_t i, size_t j) const {
    return at(i, j);
}

// ============================================================================
// ОПЕРАЦИИ МЕЖДУ МАТРИЦАМИ
// ============================================================================

/**
 * @brief Сложение двух матриц
 * 
 * Складывает соответствующие элементы матриц по правилам поля.
 * Матрицы должны быть одинакового размера.
 */
Matrix Matrix::operator+(const Matrix& other) const {
    // Проверяем совместимость размеров
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::invalid_argument("Размеры матриц не совпадают");
    }
    
    // Создаём результирующую матрицу того же размера
    Matrix result(rows_, cols_, p_, m_, modulus_);
    
    // Складываем элементы
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result.data_[i][j] = data_[i][j] + other.data_[i][j];
        }
    }
    return result;
}

/**
 * @brief Вычитание двух матриц
 * 
 * Вычитает соответствующие элементы матриц по правилам поля.
 * Матрицы должны бы��ь одинакового размера.
 */
Matrix Matrix::operator-(const Matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::invalid_argument("Размеры матриц не совпадают");
    }
    
    Matrix result(rows_, cols_, p_, m_, modulus_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result.data_[i][j] = data_[i][j] - other.data_[i][j];
        }
    }
    return result;
}

/**
 * @brief Умножение двух матриц
 * 
 * Выполняет стандартное матричное умножение A * B.
 * Количество столбцов A должно равняться количеству строк B.
 * Результат имеет размер (rows_A x cols_B).
 */
Matrix Matrix::operator*(const Matrix& other) const {
    // Проверяем совместимость для умножения
    if (cols_ != other.rows_) {
        throw std::invalid_argument("Несовместимые размеры для умножения матриц");
    }
    
    // Результат имеет размер (rows_ x other.cols_)
    Matrix result(rows_, other.cols_, p_, m_, modulus_);
    
    // Выч��сляем каждый элемент результата
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < other.cols_; ++j) {
            // Инициализируем сумму нулём
            GFElement sum(p_, m_, modulus_);
            
            // Вычисляем скалярное произведение строки i на столбец j
            for (size_t k = 0; k < cols_; ++k) {
                sum += data_[i][k] * other.data_[k][j];
            }
            result.data_[i][j] = sum;
        }
    }
    return result;
}

/**
 * @brief Сложение с присваиванием (+=)
 */
Matrix& Matrix::operator+=(const Matrix& other) {
    *this = *this + other;
    return *this;
}

/**
 * @brief Вычитание с присваиванием (-=)
 */
Matrix& Matrix::operator-=(const Matrix& other) {
    *this = *this - other;
    return *this;
}

// ============================================================================
// УМНОЖЕНИЕ НА СКАЛЯР
// ============================================================================

/**
 * @brief Умножение матрицы на элемент поля (скаляр)
 * 
 * Умножает каждый элемент матрицы на заданный скаляр.
 */
Matrix Matrix::operator*(const GFElement& scalar) const {
    Matrix result(rows_, cols_, p_, m_, modulus_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result.data_[i][j] = data_[i][j] * scalar;
        }
    }
    return result;
}

/**
 * @brief Умножение матрицы на скаляр с присваиванием (*=)
 */
Matrix& Matrix::operator*=(const GFElement& scalar) {
    *this = *this * scalar;
    return *this;
}

// ============================================================================
// УМНОЖЕНИЕ НА ВЕКТОР
// ============================================================================

/**
 * @brief Умножение матрицы на вектор-столбец
 * 
 * Вычисляет произведение матрицы A на вектор v: A * v.
 * Размер вектора должен равняться числу столбцов матрицы.
 * Результат - вектор размера (rows_ x 1).
 */
std::vector<GFElement> Matrix::operator*(const std::vector<GFElement>& vec) const {
    // Проверяем совместимость
    if (vec.size() != cols_) {
        throw std::invalid_argument("Размер вектора не совпадает с количеством столбцов");
    }
    
    // Результат - вектор размера rows_
    std::vector<GFElement> result(rows_, GFElement(p_, m_, modulus_));
    
    // Для каждой строки матрицы вычисляем скалярное произведение на вектор
    for (size_t i = 0; i < rows_; ++i) {
        GFElement sum(p_, m_, modulus_);
        for (size_t j = 0; j < cols_; ++j) {
            sum += data_[i][j] * vec[j];
        }
        result[i] = sum;
    }
    return result;
}

// ============================================================================
// ОПЕРАЦИИ ТРАНСПОНИРОВАНИЯ И РАБОТА СО СТРОКАМИ/СТОЛБЦАМИ
// ============================================================================

/**
 * @brief Транспонирование матрицы
 * 
 * Возвращает новую матрицу, где строки стали столбцами и наоборот.
 * Если A имеет размер (m x n), то A^T имеет размер (n x m).
 */
Matrix Matrix::transpose() const {
    // Резу��ьтат имеет размер (cols_ x rows_)
    Matrix result(cols_, rows_, p_, m_, modulus_);
    
    // Копируем элементы с транспонированием
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result.data_[j][i] = data_[i][j];
        }
    }
    return result;
}

/**
 * @brief Получить строку матрицы по индексу
 */
std::vector<GFElement> Matrix::getRow(size_t i) const {
    if (i >= rows_) {
        throw std::out_of_range("Индекс строки вне границ");
    }
    return data_[i];
}

/**
 * @brief Получить столбец матрицы по индексу
 */
std::vector<GFElement> Matrix::getCol(size_t j) const {
    if (j >= cols_) {
        throw std::out_of_range("Индекс столбца вне границ");
    }
    // Собираем элементы столбца в вектор
    std::vector<GFElement> col(rows_);
    for (size_t i = 0; i < rows_; ++i) {
        col[i] = data_[i][j];
    }
    return col;
}

/**
 * @brief Установить строку матрицы
 */
void Matrix::setRow(size_t i, const std::vector<GFElement>& row) {
    if (i >= rows_) {
        throw std::out_of_range("Индекс строки вне границ");
    }
    if (row.size() != cols_) {
        throw std::invalid_argument("Размер строки не совпадает");
    }
    data_[i] = row;
}

/**
 * @brief Установить столбец матрицы
 */
void Matrix::setCol(size_t j, const std::vector<GFElement>& col) {
    if (j >= cols_) {
        throw std::out_of_range("Индекс столбца вне границ");
    }
    if (col.size() != rows_) {
        throw std::invalid_argument("Размер столбца не совпадает");
    }
    // Устанавливаем элементы столбца
    for (size_t i = 0; i < rows_; ++i) {
        data_[i][j] = col[i];
    }
}

/**
 * @brief Обмен двух строк матрицы местами
 */
void Matrix::swapRows(size_t i, size_t j) {
    if (i >= rows_ || j >= rows_) {
        throw std::out_of_range("Индекс строки вне границ");
    }
    std::swap(data_[i], data_[j]);
}

// ============================================================================
// ЭЛЕМЕНТАРНЫЕ ПРЕОБРАЗОВАНИЯ СТР��К
// ============================================================================

/**
 * @brief Умножить строку на скаляр (элементарное преобразование)
 * 
 * Элементарное преобразование: каждый элемент строки i умножается на скаляр.
 */
void Matrix::multiplyRow(size_t i, const GFElement& scalar) {
    if (i >= rows_) {
        throw std::out_of_range("Индекс строки вне границ");
    }
    for (size_t j = 0; j < cols_; ++j) {
        data_[i][j] *= scalar;
    }
}

/**
 * @brief Прибавить к строке другую строку, умноженную на скаляр
 * 
 * Элементарное преобразование: к строке dest прибавляется строка src,
 * умноженная на скаляр. В формуле: R_dest := R_dest + scalar * R_src
 */
void Matrix::addRow(size_t dest, size_t src, const GFElement& scalar) {
    if (dest >= rows_ || src >= rows_) {
        throw std::out_of_range("Индекс строки вне границ");
    }
    for (size_t j = 0; j < cols_; ++j) {
        data_[dest][j] += data_[src][j] * scalar;
    }
}

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ ГАУССА
// ============================================================================

/**
 * @brief Найти ведущий элемент (pivot) в столбце, начиная с заданной строки
 * 
 * Ведущий элемент - это первый ненулевой элемент в столбце col,
 * начиная со строки startRow. Используется в методе Гаусса.
 * Возвращает индекс строки или nullopt, если ведущий элемент не найден.
 */
std::optional<size_t> Matrix::findPivot(const Matrix& mat, size_t col, size_t startRow) const {
    for (size_t i = startRow; i < mat.rows_; ++i) {
        // Ищем первый ненулевой элемент
        if (!mat.data_[i][col].isZero()) {
            return i;
        }
    }
    return std::nullopt;  // Ведущий элемент не найден
}

// ============================================================================
// ПУБЛИЧНЫЕ МЕТОДЫ ГАУССОВА ИСКЛЮЧЕНИЯ
// ============================================================================

/**
 * @brief Прямой ход Гаусса (приведение к ступенчатому виду)
 * 
 * Преобразует матрицу к ступенчатому виду (row echelon form).
 * Это подготовительный этап для других методов исключения.
 */
GaussResult Matrix::forwardGauss(bool educational) const {
    return gaussElimination(true, false, educational);
}

/**
 * @brief Обратный ход Гаусса (приведение к систематическому виду)
 * 
 * Преобразует матрицу к систематическому виду (back substitution).
 * Обычно выполняется после прямого хода.
 */
GaussResult Matrix::backwardGauss(bool educational) const {
    return gaussElimination(false, true, educational);
}

/**
 * @brief Полное гауссово исключение (приведение к RREF)
 * 
 * Приводит матрицу к приведённому ступенчатому виду (reduced row echelon form).
 * Это сочетание прямого и обратного ходов Гаусса.
 * RREF - это форма, где:
 *  1. На главной диагонали стоят единицы
 *  2. Остальн��е элементы в столбцах с единицами - это нули
 *  3. Строки отсортированы по положению ведущих элементов
 */
GaussResult Matrix::reducedRowEchelonForm(bool educational) const {
    return gaussElimination(true, true, educational);
}

// ============================================================================
// ОСНОВНОЙ АЛГОРИТМ ГАУССОВА ИСКЛЮЧЕНИЯ
// ============================================================================

/**
 * @brief Полная реализация гауссова исключения с образовательным режимом
 * 
 * Это главный метод для гауссова исключения.
 * Параметры:
 *  - forward: выполнять ли прямой ход (приведение к ступенчатому виду)
 *  - backward: выполнять ли обратный ход (приведение к RREF)
 *  - educational: выводить ли подробное объяснение каждого шага
 * 
 * Алгоритм прямого хода (forward):
 *  1. Для каждого столбца:
 *     а) Найти ведущий (ненулевой) э��емент
 *     б) Переместить его на диагональ (обмен строк)
 *     в) Нормализовать: сделать ведущий элемент равным 1
 *     г) Обнулить все элементы ниже ведущего
 */
GaussResult Matrix::gaussElimination(bool forward, bool backward, bool educational) const {
    // Если образовательный режим отключен, используем более быстрый вариант
    if (!educational) {
        return gaussEliminationCore(forward, backward);
    }

    // Инициализируем результат копией исходной матрицы
    GaussResult result(*this);
    size_t currentRow = 0;
    
    // ========== ПРЯМОЙ ХОД ==========
    if (forward) {
        // Проходим по каждому столбцу
        for (size_t col = 0; col < cols_ && currentRow < rows_; ++col) {
            // Ищем ведущий элемент в этом столбце
            auto pivotRow = findPivot(result.matrix, col, currentRow);
            
            // Если ведущий элемент не найден, переходим к следующему столбцу
            if (!pivotRow.has_value()) {
                result.steps.push_back("Столбец " + std::to_string(col) + 
                                         ": все элементы ниже строки " + 
                                         std::to_string(currentRow) + " равны нулю");
                continue;
            }
            
            // Если ведущий элемент не на текущей строке, обмениваем строки
            if (pivotRow.value() != currentRow) {
                result.matrix.swapRows(currentRow, pivotRow.value());
                result.steps.push_back("Шаг: меняем местами строки " + 
                                         std::to_string(currentRow) + " и " + 
                                         std::to_string(pivotRow.value()) +
                                         " (нашли ведущий элемент в столбце " + 
                                         std::to_string(col) + ")");
            }
            
            // Запоминаем столбец с ведущим элементом
            result.pivotCols.push_back(col);
            
            // Нормализуем строку: делаем ведущий элемент равным 1
            GFElement pivot = result.matrix.data_[currentRow][col];
            if (!pivot.isOne()) {
                GFElement pivotInv = pivot.inverse();
                result.matrix.multiplyRow(currentRow, pivotInv);
                std::ostringstream oss;
                oss << "Шаг: умножаем строку " << currentRow 
                    << " на " << pivotInv 
                    << " (делаем ведущий элемент равным 1)";
                result.steps.push_back(oss.str());
            }
            
            // Обнуляем все элементы НИЖЕ ведущего
            for (size_t row = currentRow + 1; row < rows_; ++row) {
                if (!result.matrix.data_[row][col].isZero()) {
                    // Вычисляем множитель для обнуления элемента
                    GFElement factor = -result.matrix.data_[row][col];
                    result.matrix.addRow(row, currentRow, factor);
                    std::ostringstream oss;
                    oss << "Шаг: прибавляем к строке " << row 
                        << " строку " << currentRow 
                        << ", умноженную на " << factor
                        << " (обнуляем элемент [" << row << "," << col << "])";
                    result.steps.push_back(oss.str());
                }
            }
            
            currentRow++;
            result.rank++;
        }
        
        result.steps.push_back("Прямой ход завершён. Ранг матрицы: " + 
                                 std::to_string(result.rank));
    }
    
    // ========== ОБРАТНЫЙ ХОД ==========
    if (backward && result.rank > 0) {
        // Добавляем информацию о начале обратного хода
        if (forward) {
            result.steps.push_back("Начинаем обратный ход (приведение к RREF)");
        }
        
        // Если не был выполнен прямой ход, нужно найти ведущие столбцы
        if (!forward) {
            result.pivotCols.clear();
            for (size_t row = 0; row < rows_; ++row) {
                for (size_t col = 0; col < cols_; ++col) {
                    if (!result.matrix.data_[row][col].isZero()) {
                        result.pivotCols.push_back(col);
                        result.rank++;
                        break;
                    }
                }
            }
        }
        
        // Обнуляем все элементы НАД ведущими элементами
        for (int pivotIdx = static_cast<int>(result.pivotCols.size()) - 1; 
             pivotIdx >= 0; --pivotIdx) {
            size_t pivotRow = static_cast<size_t>(pivotIdx);
            size_t pivotCol = result.pivotCols[pivotIdx];
            
            // Проходим по всем строкам выше текущей
            for (int row = static_cast<int>(pivotRow) - 1; row >= 0; --row) {
                if (!result.matrix.data_[row][pivotCol].isZero()) {
                    // Вычисляем множитель для обнуления элемента
                    GFElement factor = -result.matrix.data_[row][pivotCol];
                    result.matrix.addRow(row, pivotRow, factor);
                    std::ostringstream oss;
                    oss << "Шаг: прибавляем к строке " << row 
                        << " строку " << pivotRow 
                        << ", умноженную на " << factor
                        << " (обнуляем элемент [" << row << "," << pivotCol << "])";
                    result.steps.push_back(oss.str());
                }
            }
        }
        
        result.steps.push_back("Обратный ход завершён. Матрица приведена к RREF");
    }
    
    return result;
}

// ============================================================================
// БЫСТРАЯ РЕАЛИЗАЦИЯ ГАУССОВА ИСКЛЮЧЕНИЯ БЕЗ ОБЪЯСНЕНИЙ
// ============================================================================

/**
 * @brief Быстрая реализация гауссова исключения (без вывода шагов)
 * 
 * Выполняет гауссово исключение без подробного объяснения.
 * Используется, когда нужна скорость вычисления.
 */
GaussResult Matrix::gaussEliminationCore(bool forward, bool backward) const {
    GaussResult result(*this);
    size_t currentRow = 0;
    
    // Прямой ход
    if (forward) {
        for (size_t col = 0; col < cols_ && currentRow < rows_; ++col) {
            // Ищем ведущий элемент
            auto pivotRow = findPivot(result.matrix, col, currentRow);
            
            if (!pivotRow.has_value()) {
                continue;  // Переходим к следующему столбцу
            }
            
            // Обмениваем строки, если нужно
            if (pivotRow.value() != currentRow) {
                result.matrix.swapRows(currentRow, pivotRow.value());
            }
            
            result.pivotCols.push_back(col);
            
            // Нормализуем ведущий элемент
            GFElement pivot = result.matrix.data_[currentRow][col];
            if (!pivot.isOne()) {
                result.matrix.multiplyRow(currentRow, pivot.inverse());
            }
            
            // Обнуляем элементы ниже
            for (size_t row = currentRow + 1; row < rows_; ++row) {
                if (!result.matrix.data_[row][col].isZero()) {
                    result.matrix.addRow(row, currentRow, -result.matrix.data_[row][col]);
                }
            }
            
            currentRow++;
            result.rank++;
        }
    }
    
    // Обратный ход
    if (backward && result.rank > 0) {
        // Если не был выполнен прямой ход, найдём ведущие столбцы
        if (!forward) {
            result.pivotCols.clear();
            for (size_t row = 0; row < rows_; ++row) {
                for (size_t col = 0; col < cols_; ++col) {
                    if (!result.matrix.data_[row][col].isZero()) {
                        result.pivotCols.push_back(col);
                        result.rank++;
                        break;
                    }
                }
            }
        }
        
        // Обнуляем элементы над ведущими
        for (int pivotIdx = static_cast<int>(result.pivotCols.size()) - 1; 
             pivotIdx >= 0; --pivotIdx) {
            size_t pivotRow = static_cast<size_t>(pivotIdx);
            size_t pivotCol = result.pivotCols[pivotIdx];
            
            for (int row = static_cast<int>(pivotRow) - 1; row >= 0; --row) {
                if (!result.matrix.data_[row][pivotCol].isZero()) {
                    result.matrix.addRow(row, pivotRow, -result.matrix.data_[row][pivotCol]);
                }
            }
        }
    }
    
    return result;
}

// ============================================================================
// РАНГ МАТРИЦЫ И ОБРАТИМОСТЬ
// ============================================================================

/**
 * @brief Вычисление ранга матрицы
 * 
 * Ранг матрицы - это количество линейно независимых строк (или столбцов).
 * Эквивалентно количеству ведущих элементов в ступенчатом виде.
 */
size_t Matrix::rank() const {
    // Приводим матрицу к ступенчатому виду и возвращаем ранг
    auto result = forwardGauss(false);
    return result.rank;
}

/**
 * @brief Проверка обратимости матрицы
 * 
 * Матрица обратима (имеет обратную) тогда и только тогда, когда:
 *  1. Она квадратная (rows == cols)
 *  2. Её ранг равен её размеру (ранг == количество строк)
 */
bool Matrix::isInvertible() const {
    // Квадратная ли матрица?
    if (rows_ != cols_) {
        return false;
    }
    // Полный ли ранг?
    return rank() == rows_;
}

// ============================================================================
// ВЫЧИСЛЕНИЕ ОБРАТНОЙ МАТРИЦЫ
// ============================================================================

/**
 * @brief Вычисление обратной матрицы
 * 
 * Использует метод Гаусса-Жордана на расширенной матрице [A | I].
 * Если матрица обратима, в результате получаем [I | A^(-1)].
 * 
 * Алгоритм:
 *  1. Создаём расширенную матрицу [A | I]
 *  2. Приводим к RREF методом Гаусса-Жордана
 *  3. Если слева получилась единичная матрица, то справа - обратная
 *  4. Иначе матрица необратима
 */
std::optional<Matrix> Matrix::inverse(bool educational) const {
    // Проверяем, что матрица квадратная
    if (rows_ != cols_) {
        if (educational) {
            std::cout << "Матрица не квадратная, обратная не существует\n";
        }
        return std::nullopt;
    }
    
    // Создаём расширенную матрицу [A | I]
    // Левая половина - исходная матрица, правая половина - единичная матрица
    Matrix augmented(rows_, 2 * cols_, p_, m_, modulus_);
    
    // Копируем исходную матрицу в левую половину
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            augmented.data_[i][j] = data_[i][j];
        }
    }
    
    // Копируем единичную матрицу в правую половину
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            if (i == j) {
                augmented.data_[i][cols_ + j] = GFElement(1, p_, m_, modulus_);
            } else {
                augmented.data_[i][cols_ + j] = GFElement(p_, m_, modulus_);
            }
        }
    }
    
    if (educational) {
        std::cout << "Расширенная матрица [A | I]:\n" << augmented << "\n\n";
    }
    
    // Приводим к RREF
    auto result = augmented.reducedRowEchelonForm(educational);
    
    if (educational) {
        std::cout << "\nПосле приведения к RREF:\n" << result.matrix << "\n";
        for (const auto& step : result.steps) {
            std::cout << step << "\n";
        }
    }
    
    // Проверяем, что получили единичную матрицу слева
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            // На диагонали должны быть единицы
            bool shouldBeOne = (i == j);
            bool isOne = result.matrix.data_[i][j].isOne();
            bool isZero = result.matrix.data_[i][j].isZero();
            
            if (shouldBeOne && !isOne) {
                // На диагонали должна быть единица, но её нет
                if (educational) {
                    std::cout << "\nМатрица необратима (ранг < " << rows_ << ")\n";
                }
                return std::nullopt;
            }
            if (!shouldBeOne && !isZero) {
                // Вне диагонали должны быть нули, но они не нули
                if (educational) {
                    std::cout << "\nМатрица необратима (не удалось получить единичную матрицу слева)\n";
                }
                return std::nullopt;
            }
        }
    }
    
    // Матрица обратима! Извлекаем правую половину (это обратная матрица)
    Matrix inv(rows_, cols_, p_, m_, modulus_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            inv.data_[i][j] = result.matrix.data_[i][cols_ + j];
        }
    }
    
    if (educational) {
        std::cout << "\nОбратная матрица найдена!\n";
    }
    
    return inv;
}

// ============================================================================
// ПОИСК ОБРАТИМОЙ ПОДМАТРИЦЫ
// ============================================================================

/**
 * @brief Поиск максимальной обратимой подматрицы
 * 
 * Ищет наибольшую подматрицу (выбирая подмножества строк и столбцов),
 * которая является обратимой.
 * 
 * Используется перебор всех комбинаций:
 *  - Сн��чала ищем подматрицу максимально возможного размера
 *  - Если не найдена, уменьшаем размер и повторяем
 *  - Процесс продолжается, пока не найдём обратимую подматрицу
 */
std::optional<SubmatrixInfo> Matrix::findInvertibleSubmatrix() const {
    // Максимальный возможный размер подматрицы
    size_t maxSize = std::min(rows_, cols_);
    
    // Ищем обратимую подматрицу, начиная с максимального размера
    for (size_t size = maxSize; size >= 1; --size) {
        // Генерируем все комбинации строк размера 'size'
        std::vector<size_t> rowIndices;
        for (size_t i = 0; i < rows_; ++i) {
            rowIndices.push_back(i);
        }
        
        // Используем битовый вектор для перебора комбинаций строк
        std::vector<bool> rowSelector(rows_, false);
        std::fill(rowSelector.begin(), rowSelector.begin() + size, true);
        
        do {
            // Собираем выбранные индексы строк
            std::vector<size_t> selectedRows;
            for (size_t i = 0; i < rows_; ++i) {
                if (rowSelector[i]) {
                    selectedRows.push_back(i);
                }
            }
            
            // Генерируем все комбинации столбцов размера 'size'
            std::vector<bool> colSelector(cols_, false);
            std::fill(colSelector.begin(), colSelector.begin() + size, true);
            
            do {
                // Собираем выбранные индексы столбцов
                std::vector<size_t> selectedCols;
                for (size_t j = 0; j < cols_; ++j) {
                    if (colSelector[j]) {
                        selectedCols.push_back(j);
                    }
                }
                
                // Проверяем подматрицу на обратимость
                Matrix sub = submatrix(selectedRows, selectedCols);
                if (sub.isInvertible()) {
                    // Нашли обратимую подматрицу!
                    SubmatrixInfo info(sub);
                    info.rows = selectedRows;
                    info.cols = selectedCols;
                    return info;
                }
                
            } while (std::prev_permutation(colSelector.begin(), colSelector.end()));
            
        } while (std::prev_permutation(rowSelector.begin(), rowSelector.end()));
    }
    
    // Обратимая подматрица не найдена
    return std::nullopt;
}

// ============================================================================
// ИЗВЛЕЧЕНИЕ ПОДМАТРИЦЫ
// ============================================================================

/**
 * @brief Извлечение подматрицы по заданным индексам строк и столбцов
 * 
 * Создаёт новую матрицу, содержащую только элементы с индексами,
 * указанными в rowIndices и colIndices.
 */
Matrix Matrix::submatrix(const std::vector<size_t>& rowIndices,
                        const std::vector<size_t>& colIndices) const {
    // Создаём результирующую матрицу нужного размера
    Matrix result(rowIndices.size(), colIndices.size(), p_, m_, modulus_);
    
    // Копируем выбранные элементы
    for (size_t i = 0; i < rowIndices.size(); ++i) {
        for (size_t j = 0; j < colIndices.size(); ++j) {
            result.data_[i][j] = data_[rowIndices[i]][colIndices[j]];
        }
    }
    return result;
}

// ============================================================================
// ОПЕРАЦИИ СРАВНЕНИЯ
// ============================================================================

/**
 * @brief Проверка равенства двух матриц
 * 
 * Две матрицы равны, если они имеют одинаковые размеры и все их
 * соответствующие элементы равны.
 */
bool Matrix::operator==(const Matrix& other) const {
    // Сравниваем размеры
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        return false;
    }
    
    // Сравниваем все элементы
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            if (data_[i][j] != other.data_[i][j]) {
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief Проверка неравенства двух матриц
 */
bool Matrix::operator!=(const Matrix& other) const {
    return !(*this == other);
}

// ============================================================================
// ВЫВОД МАТРИЦЫ
// ============================================================================

/**
 * @brief Преобразование матрицы в строку
 */
std::string Matrix::toString() const {
    std::ostringstream oss;
    oss << *this;
    return oss.str();
}

/**
 * @brief Оператор вывода матрицы в поток
 * 
 * Форматирует матрицу для вывода в консоль или файл:
 *  - Каждая строка начинается с [
 *  - Элементы разделены пробелами
 *  - Каждая строка заканчивается ]
 *  - Строки выводятся одна под другой
 */
std::ostream& operator<<(std::ostream& os, const Matrix& mat) {
    for (size_t i = 0; i < mat.rows_; ++i) {
        os << "[";
        for (size_t j = 0; j < mat.cols_; ++j) {
            if (j > 0) os << " ";
            os << std::setw(4) << mat.data_[i][j];
        }
        os << " ]";
        if (i < mat.rows_ - 1) os << "\n";
    }
    return os;
}

} // namespace matrix_gf2
