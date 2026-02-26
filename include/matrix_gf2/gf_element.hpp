#ifndef MATRIX_GF2_GF_ELEMENT_HPP
#define MATRIX_GF2_GF_ELEMENT_HPP

#include <cstdint>
#include <vector>
#include <iostream>

namespace matrix_gf2 {

/**
 * @brief Класс для представления элемента конечного поля GF(p^m)
 * 
 * Представляет элементы поля Галуа GF(p^m), где p - простое число,
 * m - степень расширения. Элементы хранятся как полиномы степени < m
 * с коэффициентами из GF(p).
 */
class GFElement {
public:
    /**
     * @brief Конструктор по умолчанию (нулевой элемент)
     * @param p Характеристика поля (простое число)
     * @param m Степень расширения
     * @param modulus Неприводимый полином (коэффициенты)
     */
    GFElement(uint32_t p = 2, uint32_t m = 1, 
              const std::vector<uint32_t>& modulus = {1, 1});
    
    /**
     * @brief Конструктор из значения
     * @param value Целочисленное значение элемента (для GF(p) или как полином)
     * @param p Характеристика поля
     * @param m Степень расширения
     * @param modulus Неприводимый полином
     */
    GFElement(uint32_t value, uint32_t p, uint32_t m,
              const std::vector<uint32_t>& modulus = {1, 1});
    
    /**
     * @brief Конструктор из вектора коэффициентов
     * @param coeffs Коэффициенты полинома
     * @param p Характеристика поля
     * @param m Степень расширения
     * @param modulus Неприводимый полином
     */
    GFElement(const std::vector<uint32_t>& coeffs, uint32_t p, uint32_t m,
              const std::vector<uint32_t>& modulus);
    
    // Арифметические операции
    GFElement operator+(const GFElement& other) const;
    GFElement operator-(const GFElement& other) const;
    GFElement operator*(const GFElement& other) const;
    GFElement operator/(const GFElement& other) const;
    
    GFElement& operator+=(const GFElement& other);
    GFElement& operator-=(const GFElement& other);
    GFElement& operator*=(const GFElement& other);
    GFElement& operator/=(const GFElement& other);
    
    // Унарные операции
    GFElement operator-() const;
    
    // Операции сравнения
    bool operator==(const GFElement& other) const;
    bool operator!=(const GFElement& other) const;
    
    // Получение обратного элемента
    GFElement inverse() const;
    
    // Проверка на ноль
    bool isZero() const;
    bool isOne() const;
    
    // Получение характеристики и степени
    uint32_t getP() const { return p_; }
    uint32_t getM() const { return m_; }
    const std::vector<uint32_t>& getModulus() const { return modulus_; }
    
    // Получение коэффициентов
    const std::vector<uint32_t>& getCoeffs() const { return coeffs_; }
    
    // Получение значения (для простых полей)
    uint32_t getValue() const;
    
    // Вывод
    friend std::ostream& operator<<(std::ostream& os, const GFElement& elem);
    
private:
    uint32_t p_;  // Характеристика поля
    uint32_t m_;  // Степень расширения
    std::vector<uint32_t> modulus_; // Неприводимый полином
    std::vector<uint32_t> coeffs_;  // Коэффициенты полинома
    
    // Приведение по модулю p
    void reduce();
    
    // Приведение по модулю неприводимого полинома
    void reduceModulo();
    
    // Умножение полиномов
    std::vector<uint32_t> polyMul(const std::vector<uint32_t>& a,
                                   const std::vector<uint32_t>& b) const;
    
    // Деление полиномов с остатком
    std::vector<uint32_t> polyMod(const std::vector<uint32_t>& a,
                                   const std::vector<uint32_t>& b) const;
    

};

} // namespace matrix_gf2

#endif // MATRIX_GF2_GF_ELEMENT_HPP
