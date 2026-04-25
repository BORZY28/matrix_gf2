
#include "gf2.h"
#include <vector>
#include <string>
#include <optional>
#include <functional>

namespace matrix_gf2 {

// Псевдоним типа для поля GF(2^m) из пространства имён gf2
using GF2 = gf2::Field;

// Forward declaration
class Matrix;

// Result structure for Gaussian elimination
struct GaussResult;

// Submatrix information  
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
     * @param field Объект поля
     */
    Matrix(size_t rows, size_t cols, const GF2& field); //!!! заменила на (p, m, modulus) на объект поля 
    
    /**
     * @brief Конструктор из двумерного массива значений
     * @param data Данные матрицы
     * @param p Характеристика поля
     * @param m Степень расширения
     * @param modulus Неприводимый полином
     */
    Matrix(const std::vector<std::vector<uint64_t>>& data,
           const GF2& field); //!!! uint64_t чтобы с элементами поля совместимость была
    
    /**
     * @brief Конструктор из двумерного массива элементов GF
     * @note Используется стандартный модуль {1, 1} для простых полей GF(p)
     */
    Matrix(const std::vector<std::vector<GF2::GF2Element>>& data); ///!!! элементы поля
    
    // Создание специальных матриц
    static Matrix identity(size_t n, const GF2& field);
    static Matrix zero(size_t rows, size_t cols, const GF2& field);
    static Matrix random(size_t rows, size_t cols, const GF2& field);
    
    // Размеры
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }
    
    // Доступ к элементам !!! замена на элементы поля
    GF2::GF2Element& at(size_t i, size_t j);
    const GF2::GF2Element& at(size_t i, size_t j) const;
    GF2::GF2Element& operator()(size_t i, size_t j);
    const GF2::GF2Element& operator()(size_t i, size_t j) const;
    
    // Операции с матрицами
    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;

    Matrix& operator+=(const Matrix& other);
    Matrix& operator-=(const Matrix& other);
    
    // Операции сравнения
    bool operator==(const Matrix& other) const;
    bool operator!=(const Matrix& other) const;

    // Умножение на скаляр !!! Замена на элементы поля
    Matrix operator*(const GF2::GF2Element& scalar) const;
    Matrix& operator*=(const GF2::GF2Element& scalar);
    
    // Умножение матрицы на вектор !!!
    std::vector<GF2::GF2Element> operator*(const std::vector<GF2::GF2Element>& vec) const;
    
    // Транспонирование
    Matrix transpose() const;
    
    // Получение строки/столбца !!!
    std::vector<GF2::GF2Element> getRow(size_t i) const;
    std::vector<GF2::GF2Element> getCol(size_t j) const;
    void setRow(size_t i, const std::vector<GF2::GF2Element>& row);
    void setCol(size_t j, const std::vector<GF2::GF2Element>& col);
    
    // Обмен строк
    void swapRows(size_t i, size_t j);
    
    // Элементарные преобразования строк
    void multiplyRow(size_t i, const GF2::GF2Element& scalar);
    void addRow(size_t dest, size_t src, const GF2::GF2Element& scalar);
    
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
    
    
    
    // Вывод
    friend std::ostream& operator<<(std::ostream& os, const Matrix& mat);
    std::string toString() const;
    
    // Параметры поля
    //uint32_t getP() const { return field_.p; } Надо дописать метод в GF2
    //uint32_t getM() const { return m_; }
    
private:
    size_t rows_;
    size_t cols_;
    GF2 field_;
    std::vector<std::vector<GF2::GF2Element>> data_;
    
    // Вспомогательные функции для Гаусса
    GaussResult gaussEliminationCore(bool forward, bool backward) const;
    GaussResult gaussElimination(bool forward, bool backward, bool educational) const;
    
    // Поиск ведущего элемента
    std::optional<size_t> findPivot(const Matrix& mat, size_t col, size_t startRow) const;
};

// Gaussian elimination result structure
struct GaussResult {
    Matrix matrix;
    size_t rank = 0;
    std::vector<size_t> pivotCols;
    std::vector<std::string> steps;
    
    // Constructor
    GaussResult(const Matrix& m) : matrix(m), rank(0) {}
};

// Submatrix information structure
struct SubmatrixInfo {
    std::vector<size_t> rows;
    std::vector<size_t> cols;
    Matrix submatrix;
    
    // Constructor
    SubmatrixInfo(const Matrix& m) : submatrix(m) {}
};

} // namespace matrix_gf2


