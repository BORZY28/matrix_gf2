#ifndef MATRIX_GF2_MATRIX_HPP
#define MATRIX_GF2_MATRIX_HPP

#include "gf_element.hpp"
#include <vector>
#include <string>
#include <optional>
#include <functional>

namespace matrix_gf2 {

// Предварительное объявление
class Matrix;

// Структура результата метода Гаусса
struct GaussResult;

// Информация о подматрице
struct SubmatrixInfo;

/**
 * @brief Класс для представления матрицы над полем GF(p^m)
 */
class Matrix {
public:
    /**
     * @brief Конструктор нулевой матрицы
     * @param rows Количество строк
     * @param cols Количество столбцов
     * @param p Характеристика поля
     * @param m Степень расширения
     * @param modulus Неприводимый полином
     */
    Matrix(size_t rows, size_t cols, uint32_t p = 2, uint32_t m = 1,
           const std::vector<uint32_t>& modulus = {1, 1});
    
    /**
     * @brief Конструктор из двумерного массива значений
     * @param data Данные матрицы
     * @param p Характеристика поля
     * @param m Степень расширения
     * @param modulus Неприводимый полином
     */
    Matrix(const std::vector<std::vector<uint32_t>>& data,
           uint32_t p = 2, uint32_t m = 1,
           const std::vector<uint32_t>& modulus = {1, 1});
    
    /**
     * @brief Конструктор из двумерного массива элементов GF
     * @note Используется стандартный модуль {1, 1} для простых полей GF(p)
     */
    Matrix(const std::vector<std::vector<GFElement>>& data);
    
    // Создание специальных матриц
    static Matrix identity(size_t n, uint32_t p = 2, uint32_t m = 1,
                          const std::vector<uint32_t>& modulus = {1, 1});
    static Matrix zero(size_t rows, size_t cols, uint32_t p = 2, uint32_t m = 1,
                      const std::vector<uint32_t>& modulus = {1, 1});
    static Matrix random(size_t rows, size_t cols, uint32_t p = 2, uint32_t m = 1,
                        const std::vector<uint32_t>& modulus = {1, 1});
    
    // Размеры
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }
    
    // Доступ к элементам
    GFElement& at(size_t i, size_t j);
    const GFElement& at(size_t i, size_t j) const;
    GFElement& operator()(size_t i, size_t j);
    const GFElement& operator()(size_t i, size_t j) const;
    
    // Операции с матрицами
    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    
    Matrix& operator+=(const Matrix& other);
    Matrix& operator-=(const Matrix& other);
    
    // Умножение на скаляр
    Matrix operator*(const GFElement& scalar) const;
    Matrix& operator*=(const GFElement& scalar);
    
    // Умножение матрицы на вектор
    std::vector<GFElement> operator*(const std::vector<GFElement>& vec) const;
    
    // Транспонирование
    Matrix transpose() const;
    
    // Получение строки/столбца
    std::vector<GFElement> getRow(size_t i) const;
    std::vector<GFElement> getCol(size_t j) const;
    void setRow(size_t i, const std::vector<GFElement>& row);
    void setCol(size_t j, const std::vector<GFElement>& col);
    
    // Обмен строк
    void swapRows(size_t i, size_t j);
    
    // Элементарные преобразования строк
    void multiplyRow(size_t i, const GFElement& scalar);
    void addRow(size_t dest, size_t src, const GFElement& scalar);
    
    /**
     * @brief Прямой ход Гаусса (приведение к ступенчатому виду)
     * @param educational Включить учебный режим с объяснениями
     * @return Результат с преобразованной матрицей и информацией
     */
    GaussResult forwardGauss(bool educational = false) const;
    
    /**
     * @brief Обратный ход Гаусса (приведение к систематическому виду)
     * @param educational Включить учебный режим с объяснениями
     * @return Результат с преобразованной матрицей и информацией
     */
    GaussResult backwardGauss(bool educational = false) const;
    
    /**
     * @brief Полное гауссово исключение (RREF)
     * @param educational Включить учебный режим с объяснениями
     * @return Результат с преобразованной матрицей и информацией
     */
    GaussResult reducedRowEchelonForm(bool educational = false) const;
    
    /**
     * @brief Вычисление ранга матрицы
     */
    size_t rank() const;
    
    /**
     * @brief Проверка обратимости матрицы
     */
    bool isInvertible() const;
    
    /**
     * @brief Вычисление обратной матрицы
     * @return Обратная матрица или nullopt, если матрица необратима
     */
    std::optional<Matrix> inverse(bool educational = false) const;
    
    /**
     * @brief Поиск обратимой подматрицы максимального размера
     * @return Индексы строк и столбцов обратимой подматрицы
     */
    std::optional<SubmatrixInfo> findInvertibleSubmatrix() const;
    
    /**
     * @brief Извлечение подматрицы
     * @param rowIndices Индексы строк
     * @param colIndices Индексы столбцов
     */
    Matrix submatrix(const std::vector<size_t>& rowIndices,
                    const std::vector<size_t>& colIndices) const;
    
    // Операции сравнения
    bool operator==(const Matrix& other) const;
    bool operator!=(const Matrix& other) const;
    
    // Вывод
    friend std::ostream& operator<<(std::ostream& os, const Matrix& mat);
    std::string toString() const;
    
    // Параметры поля
    uint32_t getP() const { return p_; }
    uint32_t getM() const { return m_; }
    
private:
    size_t rows_;
    size_t cols_;
    uint32_t p_;
    uint32_t m_;
    std::vector<uint32_t> modulus_;
    std::vector<std::vector<GFElement>> data_;
    
    // Вспомогательные функции для Гаусса
    GaussResult gaussElimination(bool forward, bool backward, bool educational) const;
    
    // Поиск ведущего элемента
    std::optional<size_t> findPivot(const Matrix& mat, size_t col, size_t startRow) const;
};

// Структура результата метода Гаусса
struct GaussResult {
    Matrix matrix;
    size_t rank = 0;
    std::vector<size_t> pivotCols;
    std::vector<std::string> steps;
    
    // Конструктор
    GaussResult(const Matrix& m) : matrix(m), rank(0) {}
};

// Структура с информацией о подматрице
struct SubmatrixInfo {
    std::vector<size_t> rows;
    std::vector<size_t> cols;
    Matrix submatrix;
    
    // Конструктор
    SubmatrixInfo(const Matrix& m) : submatrix(m) {}
};

} // namespace matrix_gf2

#endif // MATRIX_GF2_MATRIX_HPP
