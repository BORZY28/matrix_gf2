#include "../include/matrix_gf2/gf_element.hpp"
#include <algorithm>
#include <stdexcept>
#include <numeric>

namespace matrix_gf2 {

GFElement::GFElement(uint32_t p, uint32_t m, const std::vector<uint32_t>& modulus)
    : p_(p), m_(m), modulus_(modulus), coeffs_(m, 0) {
    if (p < 2) {
        throw std::invalid_argument("Характеристика поля должна быть >= 2");
    }
    if (m < 1) {
        throw std::invalid_argument("Степень расширения должна быть >= 1");
    }
}

GFElement::GFElement(uint32_t value, uint32_t p, uint32_t m,
                     const std::vector<uint32_t>& modulus)
    : p_(p), m_(m), modulus_(modulus), coeffs_(m, 0) {
    if (p < 2) {
        throw std::invalid_argument("Характеристика поля должна быть >= 2");
    }
    if (m < 1) {
        throw std::invalid_argument("Степень расширения должна быть >= 1");
    }
    
    // Преобразование значения в коэффициенты
    if (m == 1) {
        coeffs_[0] = value % p;
    } else {
        for (size_t i = 0; i < m && value > 0; ++i) {
            coeffs_[i] = value % p;
            value /= p;
        }
    }
}

GFElement::GFElement(const std::vector<uint32_t>& coeffs, uint32_t p, uint32_t m,
                     const std::vector<uint32_t>& modulus)
    : p_(p), m_(m), modulus_(modulus) {
    if (p < 2) {
        throw std::invalid_argument("Характеристика поля должна быть >= 2");
    }
    if (m < 1) {
        throw std::invalid_argument("Степень расширения должна быть >= 1");
    }
    
    coeffs_.resize(m, 0);
    for (size_t i = 0; i < std::min(coeffs.size(), static_cast<size_t>(m)); ++i) {
        coeffs_[i] = coeffs[i] % p;
    }
    reduceModulo();
}

void GFElement::reduce() {
    for (auto& c : coeffs_) {
        c %= p_;
    }
}

void GFElement::reduceModulo() {
    if (m_ == 1) {
        reduce();
        return;
    }
    
    // Удаляем ведущие нули
    while (coeffs_.size() > m_ && coeffs_.back() == 0) {
        coeffs_.pop_back();
    }
    
    if (coeffs_.size() < m_) {
        coeffs_.resize(m_, 0);
        return;
    }
    
    // Приводим по модулю неприводимого полинома
    coeffs_ = polyMod(coeffs_, modulus_);
    coeffs_.resize(m_, 0);
}

std::vector<uint32_t> GFElement::polyMul(const std::vector<uint32_t>& a,
                                          const std::vector<uint32_t>& b) const {
    if (a.empty() || b.empty()) return {0};
    
    std::vector<uint32_t> result(a.size() + b.size() - 1, 0);
    for (size_t i = 0; i < a.size(); ++i) {
        for (size_t j = 0; j < b.size(); ++j) {
            result[i + j] = (result[i + j] + a[i] * b[j]) % p_;
        }
    }
    
    return result;
}

std::vector<uint32_t> GFElement::polyMod(const std::vector<uint32_t>& a,
                                          const std::vector<uint32_t>& b) const {
    if (b.empty() || (b.size() == 1 && b[0] == 0)) {
        throw std::invalid_argument("Деление на нулевой полином");
    }
    
    std::vector<uint32_t> remainder = a;
    
    // Удаляем ведущие нули из делителя
    auto divisor = b;
    while (divisor.size() > 1 && divisor.back() == 0) {
        divisor.pop_back();
    }
    
    if (divisor.size() == 1 && divisor[0] == 0) {
        throw std::invalid_argument("Деление на нулевой полином");
    }
    
    while (remainder.size() >= divisor.size() && 
           !std::all_of(remainder.begin(), remainder.end(), 
                       [](uint32_t x) { return x == 0; })) {
        // Удаляем ведущие нули
        while (remainder.size() > 1 && remainder.back() == 0) {
            remainder.pop_back();
        }
        
        if (remainder.size() < divisor.size()) break;
        
        // Находим обратный к старшему коэффициенту делителя
        uint32_t lead = divisor.back();
        uint32_t leadInv = 1;
        
        // Простой поиск обратного элемента в GF(p)
        for (uint32_t i = 1; i < p_; ++i) {
            if ((lead * i) % p_ == 1) {
                leadInv = i;
                break;
            }
        }
        
        uint32_t coeff = (remainder.back() * leadInv) % p_;
        
        // Вычитаем делитель, умноженный на коэффициент
        for (size_t i = 0; i < divisor.size(); ++i) {
            size_t pos = remainder.size() - divisor.size() + i;
            remainder[pos] = (remainder[pos] + p_ - (coeff * divisor[i]) % p_) % p_;
        }
    }
    
    // Удаляем ведущие нули
    while (remainder.size() > 1 && remainder.back() == 0) {
        remainder.pop_back();
    }
    
    if (remainder.empty()) {
        remainder.push_back(0);
    }
    
    return remainder;
}

GFElement GFElement::operator+(const GFElement& other) const {
    if (p_ != other.p_ || m_ != other.m_) {
        throw std::invalid_argument("Элементы из разных полей");
    }
    
    std::vector<uint32_t> result(m_);
    for (size_t i = 0; i < m_; ++i) {
        result[i] = (coeffs_[i] + other.coeffs_[i]) % p_;
    }
    
    return GFElement(result, p_, m_, modulus_);
}

GFElement GFElement::operator-(const GFElement& other) const {
    if (p_ != other.p_ || m_ != other.m_) {
        throw std::invalid_argument("Элементы из разных полей");
    }
    
    std::vector<uint32_t> result(m_);
    for (size_t i = 0; i < m_; ++i) {
        result[i] = (coeffs_[i] + p_ - other.coeffs_[i]) % p_;
    }
    
    return GFElement(result, p_, m_, modulus_);
}

GFElement GFElement::operator*(const GFElement& other) const {
    if (p_ != other.p_ || m_ != other.m_) {
        throw std::invalid_argument("Элементы из разных полей");
    }
    
    auto product = polyMul(coeffs_, other.coeffs_);
    return GFElement(product, p_, m_, modulus_);
}

GFElement GFElement::operator/(const GFElement& other) const {
    if (other.isZero()) {
        throw std::invalid_argument("Деление на ноль");
    }
    
    return (*this) * other.inverse();
}

GFElement& GFElement::operator+=(const GFElement& other) {
    *this = *this + other;
    return *this;
}

GFElement& GFElement::operator-=(const GFElement& other) {
    *this = *this - other;
    return *this;
}

GFElement& GFElement::operator*=(const GFElement& other) {
    *this = *this * other;
    return *this;
}

GFElement& GFElement::operator/=(const GFElement& other) {
    *this = *this / other;
    return *this;
}

GFElement GFElement::operator-() const {
    std::vector<uint32_t> result(m_);
    for (size_t i = 0; i < m_; ++i) {
        result[i] = (p_ - coeffs_[i]) % p_;
    }
    return GFElement(result, p_, m_, modulus_);
}

bool GFElement::operator==(const GFElement& other) const {
    if (p_ != other.p_ || m_ != other.m_) {
        return false;
    }
    return coeffs_ == other.coeffs_;
}

bool GFElement::operator!=(const GFElement& other) const {
    return !(*this == other);
}

GFElement GFElement::inverse() const {
    if (isZero()) {
        throw std::invalid_argument("Ноль не имеет обратного элемента");
    }
    
    if (m_ == 1) {
        // Простой случай GF(p)
        uint32_t val = coeffs_[0];
        for (uint32_t i = 1; i < p_; ++i) {
            if ((val * i) % p_ == 1) {
                return GFElement(i, p_, m_, modulus_);
            }
        }
        throw std::runtime_error("Обратный элемент не найден");
    }
    
    // Расширенный алгоритм Евклида для полиномов
    // a * x + b * y = НОД(a, b)
    // Находим x такой, что this * x = 1 (mod modulus_)
    
    std::vector<uint32_t> r0 = modulus_;
    std::vector<uint32_t> r1 = coeffs_;
    std::vector<uint32_t> s0(m_, 0);
    std::vector<uint32_t> s1(m_, 0);
    s1[0] = 1;
    
    while (!std::all_of(r1.begin(), r1.end(), [](uint32_t x) { return x == 0; })) {
        // Деление r0 на r1
        auto quotient_remainder = std::make_pair(
            std::vector<uint32_t>(),
            polyMod(r0, r1)
        );
        
        // Вычисляем частное
        std::vector<uint32_t> quotient;
        auto dividend = r0;
        auto divisor = r1;
        
        while (dividend.size() >= divisor.size()) {
            while (dividend.size() > 1 && dividend.back() == 0) {
                dividend.pop_back();
            }
            while (divisor.size() > 1 && divisor.back() == 0) {
                divisor.pop_back();
            }
            
            if (dividend.size() < divisor.size()) break;
            
            uint32_t lead = divisor.back();
            uint32_t leadInv = 1;
            for (uint32_t i = 1; i < p_; ++i) {
                if ((lead * i) % p_ == 1) {
                    leadInv = i;
                    break;
                }
            }
            
            uint32_t coeff = (dividend.back() * leadInv) % p_;
            size_t deg = dividend.size() - divisor.size();
            
            if (quotient.size() <= deg) {
                quotient.resize(deg + 1, 0);
            }
            quotient[deg] = coeff;
            
            for (size_t i = 0; i < divisor.size(); ++i) {
                size_t pos = dividend.size() - divisor.size() + i;
                dividend[pos] = (dividend[pos] + p_ - (coeff * divisor[i]) % p_) % p_;
            }
        }
        
        // s2 = s0 - частное * s1
        auto prod = polyMul(quotient, s1);
        std::vector<uint32_t> s2(std::max(s0.size(), prod.size()), 0);
        for (size_t i = 0; i < s0.size(); ++i) {
            s2[i] = s0[i];
        }
        for (size_t i = 0; i < prod.size(); ++i) {
            s2[i] = (s2[i] + p_ - prod[i] % p_) % p_;
        }
        
        r0 = r1;
        r1 = quotient_remainder.second;
        s0 = s1;
        s1 = s2;
    }
    
    return GFElement(s0, p_, m_, modulus_);
}

bool GFElement::isZero() const {
    return std::all_of(coeffs_.begin(), coeffs_.end(), 
                      [](uint32_t x) { return x == 0; });
}

bool GFElement::isOne() const {
    if (coeffs_[0] != 1) return false;
    for (size_t i = 1; i < coeffs_.size(); ++i) {
        if (coeffs_[i] != 0) return false;
    }
    return true;
}

uint32_t GFElement::getValue() const {
    if (m_ == 1) {
        return coeffs_[0];
    }
    
    uint32_t result = 0;
    uint32_t power = 1;
    for (size_t i = 0; i < m_; ++i) {
        result += coeffs_[i] * power;
        power *= p_;
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const GFElement& elem) {
    if (elem.m_ == 1) {
        os << elem.coeffs_[0];
    } else {
        bool first = true;
        for (int i = elem.m_ - 1; i >= 0; --i) {
            if (elem.coeffs_[i] != 0) {
                if (!first) os << " + ";
                if (elem.coeffs_[i] != 1 || i == 0) {
                    os << elem.coeffs_[i];
                }
                if (i > 0) {
                    os << "x";
                    if (i > 1) os << "^" << i;
                }
                first = false;
            }
        }
        if (first) os << "0";
    }
    return os;
}

} // namespace matrix_gf2
