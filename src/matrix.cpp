#include "../include/matrix_gf2/matrix.hpp"
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>

namespace matrix_gf2 {

Matrix::Matrix(size_t rows, size_t cols, uint32_t p, uint32_t m,
               const std::vector<uint32_t>& modulus)
    : rows_(rows), cols_(cols), p_(p), m_(m), modulus_(modulus) {
    data_.resize(rows);
    for (auto& row : data_) {
        row.resize(cols, GFElement(p, m, modulus));
    }
}

Matrix::Matrix(const std::vector<std::vector<uint32_t>>& data,
               uint32_t p, uint32_t m,
               const std::vector<uint32_t>& modulus)
    : rows_(data.size()), cols_(data.empty() ? 0 : data[0].size()),
      p_(p), m_(m), modulus_(modulus) {
    
    data_.resize(rows_);
    for (size_t i = 0; i < rows_; ++i) {
        data_[i].resize(cols_);
        for (size_t j = 0; j < cols_ && j < data[i].size(); ++j) {
            data_[i][j] = GFElement(data[i][j], p, m, modulus);
        }
    }
}

Matrix::Matrix(const std::vector<std::vector<GFElement>>& data)
    : rows_(data.size()), cols_(data.empty() ? 0 : data[0].size()) {
    
    if (!data.empty() && !data[0].empty()) {
        p_ = data[0][0].getP();
        m_ = data[0][0].getM();
        modulus_ = {1, 1};  // Значение по умолчанию
    }
    
    data_ = data;
}

Matrix Matrix::identity(size_t n, uint32_t p, uint32_t m,
                       const std::vector<uint32_t>& modulus) {
    Matrix result(n, n, p, m, modulus);
    for (size_t i = 0; i < n; ++i) {
        result.data_[i][i] = GFElement(1, p, m, modulus);
    }
    return result;
}

Matrix Matrix::zero(size_t rows, size_t cols, uint32_t p, uint32_t m,
                   const std::vector<uint32_t>& modulus) {
    return Matrix(rows, cols, p, m, modulus);
}

Matrix Matrix::random(size_t rows, size_t cols, uint32_t p, uint32_t m,
                     const std::vector<uint32_t>& modulus) {
    Matrix result(rows, cols, p, m, modulus);
    std::random_device rd;
    std::mt19937 gen(rd());
    
    uint32_t max_val = 1;
    for (uint32_t i = 0; i < m; ++i) {
        max_val *= p;
    }
    std::uniform_int_distribution<uint32_t> dis(0, max_val - 1);
    
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result.data_[i][j] = GFElement(dis(gen), p, m, modulus);
        }
    }
    return result;
}

GFElement& Matrix::at(size_t i, size_t j) {
    if (i >= rows_ || j >= cols_) {
        throw std::out_of_range("Индекс вне границ матрицы");
    }
    return data_[i][j];
}

const GFElement& Matrix::at(size_t i, size_t j) const {
    if (i >= rows_ || j >= cols_) {
        throw std::out_of_range("Индекс вне границ матрицы");
    }
    return data_[i][j];
}

GFElement& Matrix::operator()(size_t i, size_t j) {
    return at(i, j);
}

const GFElement& Matrix::operator()(size_t i, size_t j) const {
    return at(i, j);
}

Matrix Matrix::operator+(const Matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::invalid_argument("Размеры матриц не совпадают");
    }
    
    Matrix result(rows_, cols_, p_, m_, modulus_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result.data_[i][j] = data_[i][j] + other.data_[i][j];
        }
    }
    return result;
}

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

Matrix Matrix::operator*(const Matrix& other) const {
    if (cols_ != other.rows_) {
        throw std::invalid_argument("Несовместимые размеры для умножения матриц");
    }
    
    Matrix result(rows_, other.cols_, p_, m_, modulus_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < other.cols_; ++j) {
            GFElement sum(p_, m_, modulus_);
            for (size_t k = 0; k < cols_; ++k) {
                sum += data_[i][k] * other.data_[k][j];
            }
            result.data_[i][j] = sum;
        }
    }
    return result;
}

Matrix& Matrix::operator+=(const Matrix& other) {
    *this = *this + other;
    return *this;
}

Matrix& Matrix::operator-=(const Matrix& other) {
    *this = *this - other;
    return *this;
}

Matrix Matrix::operator*(const GFElement& scalar) const {
    Matrix result(rows_, cols_, p_, m_, modulus_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result.data_[i][j] = data_[i][j] * scalar;
        }
    }
    return result;
}

Matrix& Matrix::operator*=(const GFElement& scalar) {
    *this = *this * scalar;
    return *this;
}

std::vector<GFElement> Matrix::operator*(const std::vector<GFElement>& vec) const {
    if (vec.size() != cols_) {
        throw std::invalid_argument("Размер вектора не совпадает с количеством столбцов");
    }
    
    std::vector<GFElement> result(rows_, GFElement(p_, m_, modulus_));
    for (size_t i = 0; i < rows_; ++i) {
        GFElement sum(p_, m_, modulus_);
        for (size_t j = 0; j < cols_; ++j) {
            sum += data_[i][j] * vec[j];
        }
        result[i] = sum;
    }
    return result;
}

Matrix Matrix::transpose() const {
    Matrix result(cols_, rows_, p_, m_, modulus_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result.data_[j][i] = data_[i][j];
        }
    }
    return result;
}

std::vector<GFElement> Matrix::getRow(size_t i) const {
    if (i >= rows_) {
        throw std::out_of_range("Индекс строки вне границ");
    }
    return data_[i];
}

std::vector<GFElement> Matrix::getCol(size_t j) const {
    if (j >= cols_) {
        throw std::out_of_range("Индекс столбца вне границ");
    }
    std::vector<GFElement> col(rows_);
    for (size_t i = 0; i < rows_; ++i) {
        col[i] = data_[i][j];
    }
    return col;
}

void Matrix::setRow(size_t i, const std::vector<GFElement>& row) {
    if (i >= rows_) {
        throw std::out_of_range("Индекс строки вне границ");
    }
    if (row.size() != cols_) {
        throw std::invalid_argument("Размер строки не совпадает");
    }
    data_[i] = row;
}

void Matrix::setCol(size_t j, const std::vector<GFElement>& col) {
    if (j >= cols_) {
        throw std::out_of_range("Индекс столбца вне границ");
    }
    if (col.size() != rows_) {
        throw std::invalid_argument("Размер столбца не совпадает");
    }
    for (size_t i = 0; i < rows_; ++i) {
        data_[i][j] = col[i];
    }
}

void Matrix::swapRows(size_t i, size_t j) {
    if (i >= rows_ || j >= rows_) {
        throw std::out_of_range("Индекс строки вне границ");
    }
    std::swap(data_[i], data_[j]);
}

void Matrix::multiplyRow(size_t i, const GFElement& scalar) {
    if (i >= rows_) {
        throw std::out_of_range("Индекс строки вне границ");
    }
    for (size_t j = 0; j < cols_; ++j) {
        data_[i][j] *= scalar;
    }
}

void Matrix::addRow(size_t dest, size_t src, const GFElement& scalar) {
    if (dest >= rows_ || src >= rows_) {
        throw std::out_of_range("Индекс строки вне границ");
    }
    for (size_t j = 0; j < cols_; ++j) {
        data_[dest][j] += data_[src][j] * scalar;
    }
}

std::optional<size_t> Matrix::findPivot(const Matrix& mat, size_t col, size_t startRow) const {
    for (size_t i = startRow; i < mat.rows_; ++i) {
        if (!mat.data_[i][col].isZero()) {
            return i;
        }
    }
    return std::nullopt;
}

GaussResult Matrix::forwardGauss(bool educational) const {
    return gaussElimination(true, false, educational);
}

GaussResult Matrix::backwardGauss(bool educational) const {
    return gaussElimination(false, true, educational);
}

GaussResult Matrix::reducedRowEchelonForm(bool educational) const {
    return gaussElimination(true, true, educational);
}

GaussResult Matrix::gaussElimination(bool forward, bool backward, bool educational) const {
    GaussResult result(*this);  // Use the constructor
    
    size_t currentRow = 0;
    
    if (forward) {
        // Прямой ход
        for (size_t col = 0; col < cols_ && currentRow < rows_; ++col) {
            // Поиск ведущего элемента
            auto pivotRow = findPivot(result.matrix, col, currentRow);
            
            if (!pivotRow.has_value()) {
                if (educational) {
                    result.steps.push_back("Столбец " + std::to_string(col) + 
                                         ": все элементы ниже строки " + 
                                         std::to_string(currentRow) + " равны нулю");
                }
                continue;
            }
            
            // Обмен строк
            if (pivotRow.value() != currentRow) {
                result.matrix.swapRows(currentRow, pivotRow.value());
                if (educational) {
                    result.steps.push_back("Шаг: меняем местами строки " + 
                                         std::to_string(currentRow) + " и " + 
                                         std::to_string(pivotRow.value()) +
                                         " (нашли ведущий элемент в столбце " + 
                                         std::to_string(col) + ")");
                }
            }
            
            result.pivotCols.push_back(col);
            
            // Нормализация строки
            GFElement pivot = result.matrix.data_[currentRow][col];
            if (!pivot.isOne()) {
                GFElement pivotInv = pivot.inverse();
                result.matrix.multiplyRow(currentRow, pivotInv);
                if (educational) {
                    std::ostringstream oss;
                    oss << "Шаг: умножаем строку " << currentRow 
                        << " на " << pivotInv 
                        << " (делаем ведущий элемент равным 1)";
                    result.steps.push_back(oss.str());
                }
            }
            
            // Обнуление элементов ниже ведущего
            for (size_t row = currentRow + 1; row < rows_; ++row) {
                if (!result.matrix.data_[row][col].isZero()) {
                    GFElement factor = -result.matrix.data_[row][col];
                    result.matrix.addRow(row, currentRow, factor);
                    if (educational) {
                        std::ostringstream oss;
                        oss << "Шаг: прибавляем к строке " << row 
                            << " строку " << currentRow 
                            << ", умноженную на " << factor
                            << " (обнуляем элемент [" << row << "," << col << "])";
                        result.steps.push_back(oss.str());
                    }
                }
            }
            
            currentRow++;
            result.rank++;
        }
        
        if (educational) {
            result.steps.push_back("Прямой ход завершён. Ранг матрицы: " + 
                                 std::to_string(result.rank));
        }
    }
    
    if (backward && result.rank > 0) {
        // Обратный ход
        if (educational && forward) {
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
        
        // Обратный ход: обнуляем элементы над ведущими
        for (int pivotIdx = static_cast<int>(result.pivotCols.size()) - 1; 
             pivotIdx >= 0; --pivotIdx) {
            size_t pivotRow = static_cast<size_t>(pivotIdx);
            size_t pivotCol = result.pivotCols[pivotIdx];
            
            for (int row = static_cast<int>(pivotRow) - 1; row >= 0; --row) {
                if (!result.matrix.data_[row][pivotCol].isZero()) {
                    GFElement factor = -result.matrix.data_[row][pivotCol];
                    result.matrix.addRow(row, pivotRow, factor);
                    if (educational) {
                        std::ostringstream oss;
                        oss << "Шаг: прибавляем к строке " << row 
                            << " строку " << pivotRow 
                            << ", умноженную на " << factor
                            << " (обнуляем элемент [" << row << "," << pivotCol << "])";
                        result.steps.push_back(oss.str());
                    }
                }
            }
        }
        
        if (educational) {
            result.steps.push_back("Обратный ход завершён. Матрица приведена к RREF");
        }
    }
    
    return result;
}

size_t Matrix::rank() const {
    auto result = forwardGauss(false);
    return result.rank;
}

bool Matrix::isInvertible() const {
    if (rows_ != cols_) {
        return false;
    }
    return rank() == rows_;
}

std::optional<Matrix> Matrix::inverse(bool educational) const {
    if (rows_ != cols_) {
        if (educational) {
            std::cout << "Матрица не квадратная, обратная не существует\n";
        }
        return std::nullopt;
    }
    
    // Создаём расширенную матрицу [A | I]
    Matrix augmented(rows_, 2 * cols_, p_, m_, modulus_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            augmented.data_[i][j] = data_[i][j];
        }
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
            bool shouldBeOne = (i == j);
            bool isOne = result.matrix.data_[i][j].isOne();
            bool isZero = result.matrix.data_[i][j].isZero();
            
            if (shouldBeOne && !isOne) {
                if (educational) {
                    std::cout << "\nМатрица необратима (ранг < " << rows_ << ")\n";
                }
                return std::nullopt;
            }
            if (!shouldBeOne && !isZero) {
                if (educational) {
                    std::cout << "\nМатрица необратима (не удалось получить единичную матрицу слева)\n";
                }
                return std::nullopt;
            }
        }
    }
    
    // Извлекаем правую часть (обратную матрицу)
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

std::optional<SubmatrixInfo> Matrix::findInvertibleSubmatrix() const {
    size_t maxSize = std::min(rows_, cols_);
    
    // Пробуем найти обратимую подматрицу максимального размера
    for (size_t size = maxSize; size >= 1; --size) {
        // Пробуем все комбинации строк
        std::vector<size_t> rowIndices;
        for (size_t i = 0; i < rows_; ++i) {
            rowIndices.push_back(i);
        }
        
        // Генерируем комбинации строк
        std::vector<bool> rowSelector(rows_, false);
        std::fill(rowSelector.begin(), rowSelector.begin() + size, true);
        
        do {
            std::vector<size_t> selectedRows;
            for (size_t i = 0; i < rows_; ++i) {
                if (rowSelector[i]) {
                    selectedRows.push_back(i);
                }
            }
            
            // Генерируем комбинации столбцов
            std::vector<bool> colSelector(cols_, false);
            std::fill(colSelector.begin(), colSelector.begin() + size, true);
            
            do {
                std::vector<size_t> selectedCols;
                for (size_t j = 0; j < cols_; ++j) {
                    if (colSelector[j]) {
                        selectedCols.push_back(j);
                    }
                }
                
                // Проверяем подматрицу
                Matrix sub = submatrix(selectedRows, selectedCols);
                if (sub.isInvertible()) {
                    SubmatrixInfo info(sub);
                    info.rows = selectedRows;
                    info.cols = selectedCols;
                    return info;
                }
                
            } while (std::prev_permutation(colSelector.begin(), colSelector.end()));
            
        } while (std::prev_permutation(rowSelector.begin(), rowSelector.end()));
    }
    
    return std::nullopt;
}

Matrix Matrix::submatrix(const std::vector<size_t>& rowIndices,
                        const std::vector<size_t>& colIndices) const {
    Matrix result(rowIndices.size(), colIndices.size(), p_, m_, modulus_);
    for (size_t i = 0; i < rowIndices.size(); ++i) {
        for (size_t j = 0; j < colIndices.size(); ++j) {
            result.data_[i][j] = data_[rowIndices[i]][colIndices[j]];
        }
    }
    return result;
}

bool Matrix::operator==(const Matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        return false;
    }
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            if (data_[i][j] != other.data_[i][j]) {
                return false;
            }
        }
    }
    return true;
}

bool Matrix::operator!=(const Matrix& other) const {
    return !(*this == other);
}

std::string Matrix::toString() const {
    std::ostringstream oss;
    oss << *this;
    return oss.str();
}

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
