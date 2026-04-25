#include "../include/matrix_gf2/matrix_gf2.hpp"
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
 * элементами поля GF2. Инициализирует объект поля и внутреннее хранилище.
 * 
 * @param rows Количество строк матрицы
 * @param cols Количество столбцов матрицы
 * @param field Объект поля GF2, над которым работает матрица
 */
Matrix::Matrix(size_t rows, size_t cols, const GF2& field)
    : rows_(rows), cols_(cols), field_(field) {
    // Выделяем память для всех строк матрицы
    data_.resize(rows);
    // Для каждой строки создаём нулевые элементы поля
    for (auto& row : data_) {
        row.resize(cols, field_.zero());
    }
}

/**
 * @brief Конструктор из двумерного массива значений типа uint64_t
 * 
 * Создаёт матрицу из двумерного вектора целых чисел (uint64_t), где каждое число
 * преобразуется в элемент поля GF2. Размеры матрицы определяются размерами входного массива.
 * Использование uint64_t обеспечивает совместимость с представлением элементов поля GF2.
 * 
 * @param data Двумерный вектор значений uint64_t
 * @param field Объект поля GF2 для преобразования значений
 */
Matrix::Matrix(const std::vector<std::vector<uint64_t>>& data, const GF2& field)
    : rows_(data.size()), cols_(data.empty() ? 0 : data[0].size()), field_(field) {
    
    // Выделяем память для строк матрицы
    data_.resize(rows_);
    for (size_t i = 0; i < rows_; ++i) {
        data_[i].resize(cols_);
        // Преобразуем каждое целое число в элемент поля через field_.fromValue()
        for (size_t j = 0; j < cols_ && j < data[i].size(); ++j) {
            data_[i][j] = field_.fromValue(data[i][j]);
        }
    }
}

/**
 * @brief Конструктор из двумерного массива элементов поля GF2
 * 
 * Создаёт матрицу прямо из элементов поля GF2::GF2Element. Параметры поля
 * извлекаются из первого элемента матрицы. Это наиболее эффективный способ
 * создания матрицы, когда элементы уже в правильном формате.
 * 
 * @param data Двумерный вектор элементов поля GF2::GF2Element
 */
Matrix::Matrix(const std::vector<std::vector<GF2::GF2Element>>& data)
    : rows_(data.size()), cols_(data.empty() ? 0 : data[0].size()) {
    
    // Если матрица не пустая, извлекаем объект поля из первого элемент��
    if (!data.empty() && !data[0].empty()) {
        field_ = data[0][0].field();
    } else {
        // Для пустой матрицы создаём поле степени 1 по умолчанию
        field_ = GF2(1);
    }
    
    // Копируем данные матрицы
    data_ = data;
}

// ============================================================================
// СТАТИЧЕСКИЕ МЕТОДЫ СОЗДАНИЯ СПЕЦИАЛЬНЫХ МАТРИЦ
// ============================================================================

/**
 * @brief Создаёт единичную матрицу размера n x n
 * 
 * Единичная матрица - это квадратная матрица с единицами на главной диагонали
 * и нулями везде остальное. В матричной алгебре обозначается как I или E.
 * Для любой матрицы A: A * I = I * A = A
 * 
 * @param n Размер единичной матрицы (n x n)
 * @param field Объект поля GF2
 * @return Единичная матрица размера (n x n) над полем field
 */
Matrix Matrix::identity(size_t n, const GF2& field) {
    Matrix result(n, n, field);
    // Устанавливаем единицы на диагонали: [i, i] = 1
    for (size_t i = 0; i < n; ++i) {
        result.data_[i][i] = field.one();
    }
    return result;
}

/**
 * @brief Создаёт нулевую матрицу размера rows x cols
 * 
 * Нулевая матрица заполнена только нулями поля. Это нейтральный элемент
 * для операции сложения матриц: A + O = A для любой матрицы A.
 * По сути это просто вызов обычного конструктора.
 * 
 * @param rows Количество строк
 * @param cols Количество столбцов
 * @param field Объект поля GF2
 * @return Нулевая матрица размера (rows x cols)
 */
Matrix Matrix::zero(size_t rows, size_t cols, const GF2& field) {
    return Matrix(rows, cols, field);
}

/**
 * @brief Создаёт случайную матрицу размера rows x cols
 * 
 * Генерирует матрицу со случайными элементами поля GF2. Для каждой позиции
 * матрицы выбирается случайный элемент поля. Используется для тестирования
 * и демонстрации работы алгоритмов.
 * 
 * @param rows Количество строк
 * @param cols Количество столбцов
 * @param field Объект поля GF2
 * @return Случайная матрица размера (rows x cols) с элементами из field
 */
Matrix Matrix::random(size_t rows, size_t cols, const GF2& field) {
    Matrix result(rows, cols, field);
    
    // Инициализируем генератор случайных чисел
    std::random_device rd;
    std::mt19937_64 gen(rd());
    
    // Заполняем матрицу случайными элементами поля
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            // gf2::randomElement - функция из gf2.h для генерации случайного элемента
            result.data_[i][j] = gf2::randomElement(field, gen);
        }
    }
    return result;
}

// ============================================================================
// ДОСТУП К ЭЛЕМЕНТАМ МАТРИЦЫ
// ============================================================================

/**
 * @brief Доступ к элементу матрицы по индексам (с проверкой границ)
 * 
 * Возвращает неконстантную ссылку на элемент с индексами (i, j).
 * Проверяет, что индексы не выходят за границы матрицы.
 * Использует 0-базированную индексацию.
 * 
 * @param i Индекс строки (0 <= i < rows)
 * @param j Индекс столбца (0 <= j < cols)
 * @return Ссылка на элемент [i, j]
 * @throw std::out_of_range если индекс вне границ
 */
GF2::GF2Element& Matrix::at(size_t i, size_t j) {
    if (i >= rows_ || j >= cols_) {
        throw std::out_of_range("Matrix index out of bounds");
    }
    return data_[i][j];
}

/**
 * @brief Константный доступ к элементу матрицы
 * 
 * Возвращает константную ссылку на элемент. Используется, когда
 * матрица константна или когда нужно убедиться, что элемент не будет изменён.
 * 
 * @param i Индекс строки
 * @param j Индекс столбца
 * @return Константная ссылка на элемент [i, j]
 * @throw std::out_of_range если индекс вне границ
 */
const GF2::GF2Element& Matrix::at(size_t i, size_t j) const {
    if (i >= rows_ || j >= cols_) {
        throw std::out_of_range("Matrix index out of bounds");
    }
    return data_[i][j];
}

/**
 * @brief Оператор доступа к элементу матрицы (неконстантный)
 * 
 * Просто вызывает at(i, j). Позволяет использовать синтаксис матрица(i, j)
 * вместо матрица.at(i, j).
 */
GF2::GF2Element& Matrix::operator()(size_t i, size_t j) {
    return at(i, j);
}

/**
 * @brief Оператор доступа к элементу матрицы (константный)
 */
const GF2::GF2Element& Matrix::operator()(size_t i, size_t j) const {
    return at(i, j);
}

// ============================================================================
// ОПЕРАЦИИ МЕЖДУ МАТРИЦАМИ
// ============================================================================

/**
 * @brief Сложение двух матриц
 * 
 * Складывает соответствующие элементы матриц по правилам поля GF2.
 * В поле GF(2) сложение эквивалентно операции XOR для элементов.
 * Матрицы должны быть одинакового размера.
 * 
 * @param other Вторая матрица для сложения
 * @return Новая матрица, содержащая сумму элементов
 * @throw std::invalid_argument если размеры матриц не совпадают
 */
Matrix Matrix::operator+(const Matrix& other) const {
    // Проверяем совместимость размеров
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::invalid_argument("Matrix dimensions do not match");
    }
    
    // Создаём результирующую матрицу того же размера
    Matrix result(rows_, cols_, field_);
    
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
 * Вычитает соответствующие элементы матриц по правилам поля GF2.
 * В поле GF(2) вычитание эквивалентно сложению (так как -a = a в GF(2)).
 * Матрицы должны быть одинакового размера.
 * 
 * @param other Вторая матрица
 * @return Новая матрица, содержащая разность элементов
 * @throw std::invalid_argument если размеры матриц не совпадают
 */
Matrix Matrix::operator-(const Matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::invalid_argument("Matrix dimensions do not match");
    }
    
    Matrix result(rows_, cols_, field_);
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
 * Количество столбцов первой матрицы должно равняться количеству строк второй матрицы.
 * Результат имеет размер (rows_A x cols_B).
 * Элемент [i, j] результата = скалярное произведение i-й строки A на j-й столбец B.
 * 
 * @param other Вторая матрица для умножения
 * @return Новая матрица размера (rows x other.cols)
 * @throw std::invalid_argument если cols != other.rows
 */
Matrix Matrix::operator*(const Matrix& other) const {
    // Проверяем совместимость для умножения
    if (cols_ != other.rows_) {
        throw std::invalid_argument("Incompatible matrix dimensions for multiplication");
    }
    
    // Результат имеет размер (rows_ x other.cols_)
    Matrix result(rows_, other.cols_, field_);
    
    // Вычисляем каждый элемент результата
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < other.cols_; ++j) {
            // Инициализируем сумму нулём
            GF2::GF2Element sum = field_.zero();
            
            // Вычисляем скалярное произведение строки i на столбец j
            for (size_t k = 0; k < cols_; ++k) {
                sum = sum + (data_[i][k] * other.data_[k][j]);
            }
            result.data_[i][j] = sum;
        }
    }
    return result;
}


/**
 * @brief Сложение с присваиванием (+=)
 * 
 * Эквивалентно this = *this + other
 * Модифицирует текущую матрицу на месте.
 * 
 * @param other Вторая матрица
 * @return Ссылка на изменённую матрицу
 */
Matrix& Matrix::operator+=(const Matrix& other) {
    *this = *this + other;
    return *this;
}

/**
 * @brief Вычитание с присваиванием (-=)
 * 
 * Эквивалентно this = *this - other
 * Модифицирует текущую матрицу на месте.
 * 
 * @param other Вторая матрица
 * @return Ссылка на изменённую матрицу
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
 * Умножает каждый элемент матрицы на заданный скаляр (элемент поля).
 * Используется для масштабирования матрицы.
 * 
 * @param scalar Элемент поля, на который умножать
 * @return Новая матрица, где каждый элемент умножен на scalar
 */
Matrix Matrix::operator*(const GF2::GF2Element& scalar) const {
    Matrix result(rows_, cols_, field_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result.data_[i][j] = data_[i][j] * scalar;
        }
    }
    return result;
}

/**
 * @brief Умножение матрицы на скаляр с присваиванием (*=)
 * 
 * Модифицирует текущую матрицу на месте.
 * 
 * @param scalar Элемент поля
 * @return Ссылка на изменённую матрицу
 */
Matrix& Matrix::operator*=(const GF2::GF2Element& scalar) {
    *this = *this * scalar;
    return *this;
}


// ============================================================================
// ОПЕРАЦИИ СРАВНЕНИЯ
// ============================================================================

/**
 * @brief Проверка равенства двух матриц
 * 
 * Две матрицы равны, если они имеют одинаковые размеры и все их
 * соответствующие э��ементы равны.
 * 
 * @param other Вторая матрица для сравнения
 * @return true если матрицы равны, false иначе
 */
bool Matrix::operator==(const Matrix& other) const {
    // Сравниваем размеры
    if (rows_ != other.rows_ || cols_ != other.cols_) { // вот тут заменил && на || так как было сравнение не правильное, но пока не помогло дальше
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
// УМНОЖЕНИЕ НА ВЕКТОР
// ============================================================================

/**
 * @brief Умножение матрицы на вектор-столбец
 * 
 * Вычисляет произведение матрицы A на вектор v: A * v.
 * Размер век��ора должен равняться числу столбцов матрицы.
 * Результат - вектор размера (rows x 1).
 * Элемент i результата = скалярное произведение i-й строки матрицы на вектор.
 * 
 * @param vec Вектор для умножения (размер должен быть = cols)
 * @return Новый вектор размера rows с результатом умножения
 * @throw std::invalid_argument если размер вектора != cols
 */
std::vector<GF2::GF2Element> Matrix::operator*(const std::vector<GF2::GF2Element>& vec) const {
    // Проверяем совместимость
    if (vec.size() != cols_) {
        throw std::invalid_argument("Vector size does not match matrix columns");
    }
    
    // Результат - вектор размера rows_
    std::vector<GF2::GF2Element> result(rows_, field_.zero());
    
    // Для каждой строки матрицы вычисляем скалярное произведение на вектор
    for (size_t i = 0; i < rows_; ++i) {
        GF2::GF2Element sum = field_.zero();
        for (size_t j = 0; j < cols_; ++j) {
            sum = sum + (data_[i][j] * vec[j]);
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
 * Элемент [i, j] транспонированной матрицы = элемент [j, i] исходной матрицы.
 * 
 * @return Новая матрица размера (cols x rows)
 */
Matrix Matrix::transpose() const {
    // Результат имеет размер (cols_ x rows_)
    Matrix result(cols_, rows_, field_);
    
    // Копируем элементы с транспонированием
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result.data_[j][i] = data_[i][j];
        }
    }
    return result;
}

/**
 * @brief Получить строку матрицы по ин��ексу
 * 
 * Возвращает копию i-й строки матрицы в виде вектора.
 * 
 * @param i Индекс строки
 * @return Вектор элементов i-й строки
 * @throw std::out_of_range если индекс вне границ
 */
std::vector<GF2::GF2Element> Matrix::getRow(size_t i) const {
    if (i >= rows_) {
        throw std::out_of_range("Row index out of bounds");
    }
    return data_[i];
}

/**
 * @brief Получить столбец матрицы по индексу
 * 
 * Возвращает копию j-го столбца матрицы в виде вектора.
 * 
 * @param j Индекс столбца
 * @return Вектор элементов j-го столбца
 * @throw std::out_of_range если индекс вне границ
 */
std::vector<GF2::GF2Element> Matrix::getCol(size_t j) const {
    if (j >= cols_) {
        throw std::out_of_range("Column index out of bounds");
    }
    // Собираем элементы столбца в вектор
    std::vector<GF2::GF2Element> col(rows_);
    for (size_t i = 0; i < rows_; ++i) {
        col[i] = data_[i][j];
    }
    return col;
}

/**
 * @brief Установить строку матрицы
 * 
 * Заменяет i-ю строку матрицы новой строкой из вектора.
 * Размер вектора должен совпадать с количеством столбцов.
 * 
 * @param i Индекс строки
 * @param row Новая строка (вектор элементов)
 * @throw std::out_of_range если индекс вне границ
 * @throw std::invalid_argument если размер не совпадает
 */
void Matrix::setRow(size_t i, const std::vector<GF2::GF2Element>& row) {
    if (i >= rows_) {
        throw std::out_of_range("Row index out of bounds");
    }
    if (row.size() != cols_) {
        throw std::invalid_argument("Row size does not match");
    }
    data_[i] = row;
}

/**
 * @brief Установить столбец матрицы
 * 
 * Заменяет j-й столбец матрицы новым столбцом из вектора.
 * Размер вектора должен совпадать с количеством строк.
 * 
 * @param j Индекс столбца
 * @param col Новый столбец (вектор элементов)
 * @throw std::out_of_range если индекс вне границ
 * @throw std::invalid_argument если размер не совпадает
 */
void Matrix::setCol(size_t j, const std::vector<GF2::GF2Element>& col) {
    if (j >= cols_) {
        throw std::out_of_range("Column index out of bounds");
    }
    if (col.size() != rows_) {
        throw std::invalid_argument("Column size does not match");
    }
    // Устанавливаем элементы столбца
    for (size_t i = 0; i < rows_; ++i) {
        data_[i][j] = col[i];
    }
}

/**
 * @brief Обмен двух строк матрицы местами
 * 
 * Элементарное преобразование строк: строки i и j меняются местами.
 * Используется в методе Гаусса для выбора ведущего элемента.
 * 
 * @param i Индекс первой строки
 * @param j Индекс второй строки
 * @throw std::out_of_range если индекс вне границ
 */
void Matrix::swapRows(size_t i, size_t j) {
    if (i >= rows_ || j >= rows_) {
        throw std::out_of_range("Row index out of bounds");
    }
    std::swap(data_[i], data_[j]);
}

// ============================================================================
// ЭЛЕМЕНТАРНЫЕ ПРЕ��БРАЗОВАНИЯ СТРОК
// ============================================================================

/**
 * @brief Умножить строку на скаляр
 * 
 * Элементарное преобразование типа R_i := c * R_i.
 * Каждый элемент строки i умножается на скаляр c.
 * Используется в методе Гаусса для нормализации ведущего элемента.
 * 
 * @param i Индекс строки
 * @param scalar Элемент поля, на который умножать
 * @throw std::out_of_range если индекс вне границ
 */
void Matrix::multiplyRow(size_t i, const GF2::GF2Element& scalar) {
    if (i >= rows_) {
        throw std::out_of_range("Row index out of bounds");
    }
    for (size_t j = 0; j < cols_; ++j) {
        data_[i][j] = data_[i][j] * scalar;
    }
}

/**
 * @brief Прибавить к строке другую строку, умноженную на скаляр
 * 
 * Элементарное преобразование типа R_dest := R_dest + c * R_src.
 * К строке dest прибавляется строка src, умноженная на скаляр.
 * Это основное преобразован��е, используемое в методе Гаусса для обнуления элементов.
 * 
 * @param dest Индекс строки, к которой прибавляем
 * @param src Индекс строки, которую прибавляем
 * @param scalar Элемент поля, на который умножать строку src
 * @throw std::out_of_range если индекс вне границ
 */
void Matrix::addRow(size_t dest, size_t src, const GF2::GF2Element& scalar) {
    if (dest >= rows_ || src >= rows_) {
        throw std::out_of_range("Row index out of bounds");
    }
    for (size_t j = 0; j < cols_; ++j) {
        data_[dest][j] = data_[dest][j] + (data_[src][j] * scalar);
    }
}

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ ГАУССОВА ИСКЛЮЧЕНИЯ
// ============================================================================

/**
 * @brief Найти ведущий элемент (pivot) в столбце
 * 
 * Ведущий элемент - это первый ненулевой элемент в столбце col,
 * начиная со строки startRow и ниже. Используется в методе Га��сса.
 * 
 * Алгоритм:
 *  1. Ищем в столбце col, начиная со строки startRow
 *  2. Возвращаем индекс первого ненулевого элемента
 *  3. Если нет ненулевых элементов, возвращаем nullopt
 * 
 * @param mat Матрица для поиска
 * @param col Индекс столбца
 * @param startRow Начальная строка для поиска
 * @return Индекс строки с ведущим элементом или nullopt
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
 * Прео��разует матрицу к ступенчатому виду (row echelon form).
 * Результат: матрица имеет вид лестницы с ведущими элементами на диагонали.
 * Это подготовительный этап для других методов исключения.
 * 
 * @param educational Если true, выводит подробное объяснение каждого шага
 * @return GaussResult с преобразованной матрицей и информацией о процессе
 */
GaussResult Matrix::forwardGauss(bool educational) const {
    return gaussElimination(true, false, educational);
}

/**
 * @brief Обратный ход Гаусса (приведение к систематическому виду)
 * 
 * Преобразует матрицу к систематическому виду (back substitution).
 * Обычно выполняется после прямого хода для завершения процесса элиминации.
 * Используется в методе Гаусса-Жордана.
 * 
 * @param educational Если true, выводит подробное объяснение каждого шага
 * @return GaussResult с преобразованной матрицей и информацией о процессе
 */
GaussResult Matrix::backwardGauss(bool educational) const {
    return gaussElimination(false, true, educational);
}

/**
 * @brief Полное гауссово исключение (приведение к RREF)
 * 
 * Приводит матрицу к приведённому ступенчатому виду (reduced row echelon form).
 * Это сочетание прямого и обратного ходов Гаусса.
 * 
 * RREF имеет свойства:
 *  1. На главной диагонали стоят единицы (ведущие элементы)
 *  2. Остальные элементы в столбцах с единицами - это нули
 *  3. Строки отсортированы по положению ведущих элементов
 *  4. Это уникальная форма матрицы
 * 
 * RREF используется для:
 *  - Вычисления ранга матрицы
 *  - Проверки обратимости
 *  - Нахождения обратной матрицы методом Гаусса-Жордана
 * 
 * @param educational Если true, выводит подробное объяснение каждого шага
 * @return GaussResult с преобразованно�� матрицей и информацией о процессе
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
 * 
 * Параметры:
 *  - forward: выполнять ли прямой ход (приведение к ступенчатому виду)
 *  - backward: выполнять ли обратный ход (приведение к RREF)
 *  - educational: выводить ли подробное объяснение каждого шага
 * 
 * Алгоритм прямого хода (forward):
 *  1. Для каждого столбца, начиная с первого:
 *     а) Найти ведущий (ненулевой) элемент в этом столбце
 *     б) Если не найден, п��рейти к следующему столбцу
 *     в) Переместить строку с ведущим элементом на диагональ (обмен строк)
 *     г) Нормализовать: сделать ведущий элемент равным 1 (умножение строки на обратный элемент)
 *     д) Обнулить все элементы ниже ведущего (исключение)
 * 
 * Алгоритм обратного хода (backward):
 *  1. Для каждого ведущего элемента, начиная снизу:
 *     а) Обнулить все элементы ВЫШЕ этого ведущего элемента
 *     б) Это завершает процесс приведения к RREF
 * 
 * @param forward Выполнять прямой ход
 * @param backward Выполнять обратный ход
 * @param educational Выводить подробные объяснения шагов
 * @return GaussResult с преобразованной матрицей, рангом и шагами
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
        // Проходим по каждому столбцу слева направо
        for (size_t col = 0; col < cols_ && currentRow < rows_; ++col) {
            // Ищем ведущий элемент в этом столбце, начиная с currentRow
            auto pivotRow = findPivot(result.matrix, col, currentRow);
            
            // Если ведущий элемент не найден, переходим к следующему столбцу
            if (!pivotRow.has_value()) {
                result.steps.push_back("Column " + std::to_string(col) + 
                                     ": all elements below row " + 
                                     std::to_string(currentRow) + " are zero");
                continue;
            }
            
            // Если ведущий элемент не на текущей строке, обмениваем строки
            if (pivotRow.value() != currentRow) {
                result.matrix.swapRows(currentRow, pivotRow.value());
                result.steps.push_back("Swap rows " + 
                                     std::to_string(currentRow) + " and " + 
                                     std::to_string(pivotRow.value()) +
                                     " (found pivot in column " + 
                                     std::to_string(col) + ")");
            }
            
            // Запоминаем столбец с ведущим элементом
            result.pivotCols.push_back(col);
            
            // Нормализуем строку: делаем ведущий элемент равным 1
            GF2::GF2Element pivot = result.matrix.data_[currentRow][col];
            if (pivot != result.matrix.field_.one()) {
                GF2::GF2Element pivotInv = pivot.inverse();
                result.matrix.multiplyRow(currentRow, pivotInv);
                result.steps.push_back("Multiply row " + std::to_string(currentRow) + 
                                     " by inverse of pivot (make pivot equal 1)");
            }
            
            // Обнуляем все элементы НИЖЕ ведущего
            for (size_t row = currentRow + 1; row < rows_; ++row) {
                if (!result.matrix.data_[row][col].isZero()) {
                    // Вычисляем множитель для обнуления элемента
                    GF2::GF2Element factor = -(result.matrix.data_[row][col]);
                    result.matrix.addRow(row, currentRow, factor);
                    result.steps.push_back("Add row " + std::to_string(currentRow) + 
                                         " to row " + std::to_string(row) +
                                         " (eliminate element [" + std::to_string(row) + "," + 
                                         std::to_string(col) + "])");
                }
            }
            
            currentRow++;
            result.rank++;
        }
        
        result.steps.push_back("Forward elimination complete. Matrix rank: " + 
                             std::to_string(result.rank));
    }
    
    // ========== ОБРАТНЫЙ ХОД ==========
    if (backward && result.rank > 0) {
        // Добавляем информацию о начале обратного хода
        if (forward) {
            result.steps.push_back("Starting backward elimination (RREF)");
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
                    GF2::GF2Element factor = -(result.matrix.data_[row][pivotCol]);
                    result.matrix.addRow(row, pivotRow, factor);
                    result.steps.push_back("Add row " + std::to_string(pivotRow) + 
                                         " to row " + std::to_string(row) +
                                         " (eliminate element [" + std::to_string(row) + "," + 
                                         std::to_string(pivotCol) + "])");
                }
            }
        }
        
        result.steps.push_back("Backward elimination complete. Matrix in RREF");
    }
    
    return result;
}

// ============================================================================
// БЫСТРАЯ РЕАЛИЗАЦИЯ ГАУССОВА ИСКЛЮЧЕНИЯ БЕЗ ОБЪЯСНЕНИЙ
// ============================================================================

/**
 * @brief Быстрая реализация гауссова исключения (без вывода шагов)
 * 
 * Выполняет гауссово исключение без подробного объяснения каждого шага.
 * Используется, когда нужна только скорость вычисления и не требуется образовательный вывод.
 * Принципиально алгоритм такой же, но без построения списка шагов.
 * 
 * @param forward Выполнять прямой ход
 * @param backward Выполнять обратный ход
 * @return GaussResult с преобразованной матрицей и рангом (без шагов)
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
            GF2::GF2Element pivot = result.matrix.data_[currentRow][col];
            if (pivot != result.matrix.field_.one()) {
                result.matrix.multiplyRow(currentRow, pivot.inverse());
            }
            
            // Обнуляем элементы ниже
            for (size_t row = currentRow + 1; row < rows_; ++row) {
                if (!result.matrix.data_[row][col].isZero()) {
                    result.matrix.addRow(row, currentRow, -(result.matrix.data_[row][col]));
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
                    result.matrix.addRow(row, pivotRow, -(result.matrix.data_[row][pivotCol]));
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
 * 
 * Используется для:
 *  - Определения размерности пространства строк/столбцов
 *  - Проверки обратимости матрицы
 *  - Анализа решаемости системы линейных уравнений
 * 
 * @return Ранг матрицы (целое число от 0 до min(rows, cols))
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
 *  2. Её ранг равен её размеру (ранг == количество строк == количество столбцов)
 * 
 * Эквивалентно: матрица имеет полный ранг.
 * 
 * @return true если матрица обратима, false иначе
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
 * @brief Вычисление обратной матрицы методом Гаусса-Жордана
 * 
 * Использует метод расширенной матрицы на расширенной матрице [A | I].
 * Если матрица обратима, в результате получаем [I | A^(-1)].
 * 
 * Алгоритм:
 *  1. Создаём расширенную матрицу [A | I], где I - единичная матрица
 *  2. Приводим всю матрицу к RREF методом Гаусса-Жордана
 *  3. Если слева получилась единичная матрица, то справа стоит обратная матрица
 *  4. Иначе матрица необратима (ранг < n)
 * 
 * Сложность: O(n^3)
 * 
 * @param educational Если true, выводит подробное объяснение процесса
 * @return Обратная матрица или nullopt, если матрица необратима
 */
std::optional<Matrix> Matrix::inverse(bool educational) const {
    // Проверяем, что матрица квадратная
    if (rows_ != cols_) {
        if (educational) {
            std::cout << "Matrix is not square, inverse does not exist\n";
        }
        return std::nullopt;
    }
    
    // Создаём расширенную матрицу [A | I]
    // Левая половина - исходная матрица, правая половина - единичная матрица
    Matrix augmented(rows_, 2 * cols_, field_);
    
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
                augmented.data_[i][cols_ + j] = field_.one();
            } else {
                augmented.data_[i][cols_ + j] = field_.zero();
            }
        }
    }
    
    if (educational) {
        std::cout << "Augmented matrix [A | I]:\n" << augmented << "\n\n";
    }
    
    // Приводим всю расширенную матрицу к RREF
    auto result = augmented.reducedRowEchelonForm(educational);
    
    if (educational) {
        std::cout << "\nAfter RREF:\n" << result.matrix << "\n";
        for (const auto& step : result.steps) {
            std::cout << step << "\n";
        }
    }
    
    // Проверяем, что слева получилась единичная матрица
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            bool shouldBeOne = (i == j);
            bool isOne = result.matrix.data_[i][j] == field_.one();
            bool isZero = result.matrix.data_[i][j].isZero();
            
            if (shouldBeOne && !isOne) {
                // На диагонали должна быть единица, но её нет
                if (educational) {
                    std::cout << "\nMatrix is not invertible (rank < " << rows_ << ")\n";
                }
                return std::nullopt;
            }
            if (!shouldBeOne && !isZero) {
                // Вне диагонали должны быть нули, но они не нули
                if (educational) {
                    std::cout << "\nMatrix is not invertible (cannot obtain identity matrix)\n";
                }
                return std::nullopt;
            }
        }
    }
    
    // Матрица обратима! Извлекаем правую половину (это обратная матрица)
    Matrix inv(rows_, cols_, field_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            inv.data_[i][j] = result.matrix.data_[i][cols_ + j];
        }
    }
    
    if (educational) {
        std::cout << "\nInverse matrix found!\n";
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
 * которая является обратимой (имеет полный ранг).
 * 
 * Используется перебор всех комбинаций:
 *  - Сначала ищем подматрицу максимально возможного размера: min(rows, cols)
 *  - Если не найдена, уменьшаем размер на 1 и повторяем
 *  - Процесс продолжается, пока не найдём обратимую подматрицу или не дойдём до размера 1
 * 
 * Сложность: O(C(rows, size) * C(cols, size) * size^3), где C(n, k) - биномиальный коэффициент
 * Может быть очень медленно для больших матриц!
 * 
 * @return Информация о найденной подматрице (сама подматрица, индексы строк и столбцов)
 *         или nullopt, если не найдена обратимая подматрица
 */
std::optional<SubmatrixInfo> Matrix::findInvertibleSubmatrix() const {
    // Максимальный возможный размер подматрицы
    size_t maxSize = std::min(rows_, cols_);
    
    // Ищем обратимую подматрицу, начиная с максимального размера
    for (size_t size = maxSize; size >= 1; --size) {
        std::vector<size_t> rowIndices;
        for (size_t i = 0; i < rows_; ++i) {
            rowIndices.push_back(i);
        }
        
        // Используем битовый вектор для перебора комбинаций строк
        // true значит строка выбрана
        std::vector<bool> rowSelector(rows_, false);
        // Выбираем первые 'size' строк
        std::fill(rowSelector.begin(), rowSelector.begin() + size, true);
        
        // Перебираем все комбинации строк используя std::prev_permutation
        do {
            // Собираем индексы выбранных строк
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
                // Собираем индексы выбранных столбцов
                std::vector<size_t> selectedCols;
                for (size_t j = 0; j < cols_; ++j) {
                    if (colSelector[j]) {
                        selectedCols.push_back(j);
                    }
                }
                
                // Извлекаем подматрицу с этими строками и столбцами
                Matrix sub = submatrix(selectedRows, selectedCols);
                // Проверяем подматрицу на обратимость
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
 * указанными в rowIndices и colIndices. Новая матрица имеет размер
 * (rowIndices.size() x colIndices.size()).
 * 
 * Пример: если rowIndices = {0, 2, 3}, colIndices = {1, 4}, то подматрица
 * будет содержать элементы [0,1], [0,4], [2,1], [2,4], [3,1], [3,4].
 * 
 * @param rowIndices Индексы строк для извлечения
 * @param colIndices Индексы столбцов для извлечения
 * @return Новая матрица размера (rowIndices.size() x colIndices.size())
 */
Matrix Matrix::submatrix(const std::vector<size_t>& rowIndices,
                         const std::vector<size_t>& colIndices) const {
    // Создаём результирующую матрицу нужного размера
    Matrix result(rowIndices.size(), colIndices.size(), field_);
    
    // Копируем выбранные элементы
    for (size_t i = 0; i < rowIndices.size(); ++i) {
        for (size_t j = 0; j < colIndices.size(); ++j) {
            result.data_[i][j] = data_[rowIndices[i]][colIndices[j]];
        }
    }
    return result;
}



// ============================================================================
// ВЫВОД МАТРИЦЫ
// ============================================================================

/**
 * @brief Преобразование матрицы в строку
 * 
 * Преобразует матрицу в строковое представление, которое можно вывести.
 * Использует оператор << для форматирования.
 * 
 * @return Строка, содержащая представление матрицы
 */
std::string Matrix::toString() const {
    std::ostringstream oss;
    oss << *this;
    return oss.str();
}

/**
 * @brief Оператор вывода матрицы в поток
 * 
 * Форматирует матрицу для вывода в консоль или файл.
 * Формат:
 *  - Каждая строка начинается с [
 *  - Элементы разделены пробелами, выравнены по ширине 4 символа
 *  - Каждая строка заканчивается ]
 *  - Строки выводятся одна под другой
 * 
 * Пример вывода матрицы 2x3:
 *  [   0    1    0 ]
 *  [   1    0    1 ]
 * 
 * @param os Выходной поток (cout, файл, и т.д.)
 * @param mat Матрица для вывода
 * @return Ссылка на выходной поток
 */
std::ostream& operator<<(std::ostream& os, const Matrix& mat) {
    for (size_t i = 0; i < mat.rows_; ++i) {
        os << "[";
        for (size_t j = 0; j < mat.cols_; ++j) {
            if (j > 0) os << " ";
            os << std::setw(4) << mat.data_[i][j].value();
        }
        os << " ]";
        if (i < mat.rows_ - 1) os << "\n";
    }
    return os;
}

} // namespace matrix_gf2
