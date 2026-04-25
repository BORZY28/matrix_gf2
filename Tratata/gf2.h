#include <vector>
#include <string>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <bitset>
#include <map>
#include <numeric>

namespace gf2{

class Utils {
public:
    static uint64_t hammingWeight(uint64_t value) {
        return static_cast<uint64_t>(std::bitset<64>(value).count());
    }

    static uint64_t scalarProduct(uint64_t a, uint64_t b) {
        return hammingWeight(a & b) & 1;
    }

    static uint64_t randomVector(size_t length, std::mt19937_64& rng) {
        std::uniform_int_distribution<uint64_t> dist(0, 
            (length == 64) ? ~0ULL : ((1ULL << length) - 1));
        return dist(rng);
    }

    static uint64_t randomVectorWithWeight(size_t length, size_t weight, std::mt19937_64& rng) {
        if (weight > length) throw std::invalid_argument("Weight exceeds length");
        if (weight == 0) return 0;
        if (weight == length) 
            return (length == 64) ? ~0ULL : ((1ULL << length) - 1);

        std::vector<int> indices(length);
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), rng);

        uint64_t result = 0;
        for (size_t i = 0; i < weight; ++i) {
            result |= (1ULL << indices[i]);
        }
        return result;
    }

    static std::string toBinary(uint64_t value, size_t width) {
        std::string res;
        for (int i = static_cast<int>(width) - 1; i >= 0; --i) {
            res += ((value >> i) & 1) ? '1' : '0';
        }
        return res;
    }

    static std::string toHex(uint64_t value) {
        static const char hex_digits[] = "0123456789abcdef";
        char buf[20];
        int pos = 19;
        buf[pos] = '\0';

        if (value == 0) return "0x0";
    
        while (value > 0) {
            buf[--pos] = hex_digits[value & 0xF];
            value >>= 4;
        }

        buf[--pos] = 'x';
        buf[--pos] = '0';
        return std::string(buf + pos);
    }

    static std::string toPolynomial(uint64_t value, size_t maxDegree) {
        if (value == 0) return "0";
        std::string res;
        bool first = true;
        for (int i = static_cast<int>(maxDegree); i >= 0; --i) {
            if ((value >> i) & 1) {
                if (!first) res += " + ";
                if (i == 0) res += "1";
                else if (i == 1) res += "x";
                else res += "x^" + std::to_string(i);
                first = false;
            }
        }
        return res;
    }
};


class Field {
public:
    // Режим вычисления обратного элемента
    enum class InverseMode {
        LogTables,
        ExtendedEuclid
    };

    // Конструкторы
    Field(); // По умолчанию: GF(2), степень 1

    explicit Field(size_t degree, InverseMode invMode = InverseMode::LogTables);

    Field(size_t degree, uint64_t irreducible_poly, 
          InverseMode invMode = InverseMode::LogTables);

    static uint64_t getDefaultPolynomial(size_t degree);
    static std::vector<size_t> getSupportedDegrees();
    static bool hasDefaultPolynomial(size_t degree);

   //Базовые функции
    size_t degree() const { return m_degree; }
    uint64_t modulus() const { return m_modulus; }
    uint64_t elementMask() const { return m_elementMask; }
    uint64_t primitiveElement() const { return m_primitive; }
    InverseMode inverseMode() const { return m_invMode; }
    bool tablesAvailable() const { return m_tablesBuilt; }
    uint64_t order() const;


    // Операции над элементами
    uint64_t add(uint64_t a, uint64_t b) const;
    uint64_t multiply(uint64_t a, uint64_t b) const;
    uint64_t power(uint64_t base, uint64_t exp) const;
    uint64_t inverse(uint64_t value) const;
    uint64_t divide(uint64_t a, uint64_t b) const;


    // Представление в виде экспоненты
    int64_t toExponent(uint64_t value) const;
    uint64_t fromExponent(int64_t exp) const;

    // Элементы поля
    class Element;

    Element zero() const;
    Element one() const;
    Element primitive() const;
    Element fromValue(uint64_t value) const;
    Element fromExponentValue(int64_t exp) const;

    void buildTables(); // Таблицы многочленов

    // Псевдоним типа для удобства
    using GF2Element = Element;

private:

    size_t m_degree; // Степень
    uint64_t m_modulus; // Многочлен
    uint64_t m_elementMask;
    uint64_t m_primitive; // Порождающий элемент
    InverseMode m_invMode; // Как считаем обратный элемент
    bool m_tablesBuilt;
    
    std::vector<uint64_t> m_expTable;
    std::vector<int64_t> m_logTable;

    static const std::map<size_t, uint64_t>& getDefaultPolynomialMap();
    uint64_t inverseViaLog(uint64_t value) const;
    uint64_t inverseEuclid(uint64_t value) const;
    int degreeOf(uint64_t x) const;
    uint64_t findPrimitiveElement() const;
    std::vector<uint64_t> factorize(uint64_t n) const;
};


// Элемент поля

class Field::Element {
public:
   
    Element();
    Element(const Field& field, uint64_t value = 0);

    const Field& field() const { return *m_field; }
    uint64_t value() const { return m_value; }
    int64_t exponent() const;

    std::string toVector() const;
    std::string toPolynomial() const;
    std::string toHex() const;
    std::string toExponential() const;
    std::string toString() const;

    // Арифметика

    Element operator+(const Element& other) const;
    Element operator-(const Element& other) const;
    Element operator-() const;
    Element operator*(const Element& other) const;
    Element operator/(const Element& other) const;
    Element operator^(uint64_t exp) const;
    Element inverse() const;

    //Операторы сравнения

    bool operator==(const Element& other) const;
    bool operator!=(const Element& other) const;
    bool operator<(const Element& other) const;

    // Проверки значения
    bool isZero() const { return m_value == 0; }
    bool isOne() const { return m_value == 1; }

    // Операции
    size_t hammingWeight() const;
    uint64_t dotProduct(const Element& other) const;
  

private:
    void validate() const;
    void checkSameField(const Element& other) const;
    const Field* m_field;
    uint64_t m_value;
};


Field::Element randomElement(const Field& field, std::mt19937_64& rng);
Field::Element randomNonZeroElement(const Field& field, std::mt19937_64& rng);
Field::Element randomElementWithWeight(const Field& field, size_t weight, 
                                       std::mt19937_64& rng);

}