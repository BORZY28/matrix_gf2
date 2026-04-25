#include "../include/matrix_gf2/gf2.h"

namespace gf2 {

Field::Field() : Field(1) {}

Field::Field(size_t degree, InverseMode invMode)
    : Field(degree, getDefaultPolynomial(degree), invMode) {}

Field::Field(size_t degree, uint64_t irreducible_poly, InverseMode invMode)
    : m_degree(degree), m_modulus(irreducible_poly), 
      m_invMode(invMode), m_tablesBuilt(false), m_primitive(0) {
    
    if (degree < 1 || degree > 64) {
        throw std::invalid_argument("Degree must be in [1, 64]");
    }
    
    m_modulus |= (1ULL << degree);
    m_elementMask = (degree == 64) ? ~0ULL : ((1ULL << degree) - 1);
    
    m_primitive = findPrimitiveElement();
    
    if (degree <= 20 && invMode == InverseMode::LogTables) {
        buildTables();
    } else if (invMode == InverseMode::LogTables) {
        m_invMode = InverseMode::ExtendedEuclid;
    }
}

uint64_t Field::getDefaultPolynomial(size_t degree) {
    const auto& table = getDefaultPolynomialMap();
    auto it = table.find(degree);
    if (it != table.end()) return it->second;
    throw std::invalid_argument(
        "No default polynomial for degree " + std::to_string(degree));
}

std::vector<size_t> Field::getSupportedDegrees() {
    std::vector<size_t> result;
    for (const auto& p : getDefaultPolynomialMap()) {
        result.push_back(p.first);
    }
    return result;
}

bool Field::hasDefaultPolynomial(size_t degree) {
    return getDefaultPolynomialMap().count(degree) > 0;
}

const std::map<size_t, uint64_t>& Field::getDefaultPolynomialMap() {
    static const std::map<size_t, uint64_t> table =  { 
    {1,   0x3},           // x + 1  
    {2,   0x7},           // x^2 + x + 1
    {3,   0xB},           // x^3 + x + 1
    {4,   0x13},          // x^4 + x + 1
    {5,   0x25},          // x^5 + x^2 + 1
    {6,   0x43},          // x^6 + x + 1
    {7,   0x83},          // x^7 + x^3 + 1
    {8,   0x11B},         // x^8 + x^4 + x^3 + x + 1 (AES)
    {9,   0x211},         // x^9 + x^4 + 1
    {10,  0x409},         // x^10 + x^3 + 1
    {11,  0x805},         // x^11 + x^2 + 1
    {12,  0x1053},        // x^12 + x^6 + x^4 + x + 1
    {13,  0x201B},        // x^13 + x^4 + x^3 + x + 1
    {14,  0x402B},        // x^14 + x^5 + x^3 + x + 1
    {15,  0x8003},        // x^15 + x + 1
    {16,  0x1100B},       // x^16 + x^5 + x^3 + x + 1
    {20,  0x100005},      // x^20 + x^3 + 1
    {24,  0x1000023},     // x^24 + x^7 + x^2 + x + 1
    {32,  0x10000001B}}; // x^32 + x^7 + x^3 + x^2 + 1
    return table;
}


uint64_t Field::order() const {
    return (m_degree == 64) ? ~0ULL : ((1ULL << m_degree) - 1);
}

uint64_t Field::add(uint64_t a, uint64_t b) const {
    return a ^ b;
}

uint64_t Field::multiply(uint64_t a, uint64_t b) const {
    uint64_t result = 0;
    a &= m_elementMask;
    b &= m_elementMask;
    while (b) {
        if (b & 1) result ^= a;
        b >>= 1;
        if (a & (1ULL << (m_degree - 1))) {
            a = (a << 1) ^ m_modulus;
        } else {
            a <<= 1;
        }
    }
    return result & m_elementMask;
}

uint64_t Field::power(uint64_t base, uint64_t exp) const {
    uint64_t result = 1;
    base &= m_elementMask;
    while (exp) {
        if (exp & 1) result = multiply(result, base);
        base = multiply(base, base);
        exp >>= 1;
    }
    return result;
}

uint64_t Field::inverse(uint64_t value) const {
    if (value == 0) throw std::invalid_argument("Inverse of zero undefined");
    if (m_invMode == InverseMode::LogTables && m_tablesBuilt) {
        return inverseViaLog(value);
    }
    return inverseEuclid(value);
}

uint64_t Field::divide(uint64_t a, uint64_t b) const {
    return multiply(a, inverse(b));
}

int64_t Field::toExponent(uint64_t value) const {
    if (value == 0) return -1;
    if (!m_tablesBuilt) {
        uint64_t cur = 1;
        for (uint64_t i = 0; i < order(); ++i) {
            if (cur == value) return static_cast<int64_t>(i);
            cur = multiply(cur, m_primitive);
        }
        throw std::logic_error("Element not in field cycle");
    }
    return m_logTable[value];
}

uint64_t Field::fromExponent(int64_t exp) const {
    if (exp < 0) return 0;
    if (!m_tablesBuilt) {
        return power(m_primitive, static_cast<uint64_t>(exp));
    }
    return m_expTable[static_cast<size_t>(exp % order())];
}


Field::Element Field::zero() const { return Element(*this, 0); }
Field::Element Field::one() const { return Element(*this, 1); }
Field::Element Field::primitive() const { return Element(*this, m_primitive); }
Field::Element Field::fromValue(uint64_t value) const { 
    return Element(*this, value); 
}
Field::Element Field::fromExponentValue(int64_t exp) const { 
    return Element(*this, fromExponent(exp)); 
}


void Field::buildTables() {
    if (m_degree > 20) {
        throw std::runtime_error(
            "Log/Exp tables only supported for m <= 20");
    }
    size_t size = static_cast<size_t>(1ULL) << m_degree;
    m_expTable.resize(size);
    m_logTable.assign(size, -1);

    uint64_t x = 1;
    for (size_t i = 0; i < order(); ++i) {
        m_expTable[i] = x;
        m_logTable[x] = static_cast<int64_t>(i);
        x = multiply(x, m_primitive);
    }
    m_expTable[order()] = 1;
    m_tablesBuilt = true;
}

uint64_t Field::inverseViaLog(uint64_t value) const {
    int64_t logVal = m_logTable[value];
    if (logVal < 0) throw std::invalid_argument("Log of zero undefined");
    size_t invLog = (order() - static_cast<uint64_t>(logVal)) % order();
    return m_expTable[invLog];
}

uint64_t Field::inverseEuclid(uint64_t value) const {
    uint64_t u = value, v = m_modulus;
    uint64_t g1 = 1, g2 = 0;

    while (u != 1 && u != 0) {
        int degU = degreeOf(u);
        int degV = degreeOf(v);
        int j = degU - degV;
        if (j < 0) {
            std::swap(u, v);
            std::swap(g1, g2);
            j = -j;
        }
        u ^= (v << j);
        g1 ^= multiply(g2, 1ULL << j);
    }
    return (u == 1) ? (g1 & m_elementMask) : 0;
}

int Field::degreeOf(uint64_t x) const {
    if (x == 0) return -1;
    int deg = 0;
    while ((x >>= 1) != 0) ++deg;
    return deg;
}

uint64_t Field::findPrimitiveElement() const { //Алгоритм поиска образующего элемента
    uint64_t ord = order();
    auto factors = factorize(ord);

    for (uint64_t g = 2; g <= 256; ++g) {
        if ((g & m_elementMask) == 0) continue;
        bool isPrim = true;
        for (uint64_t p : factors) {
            if (power(g, ord / p) == 1) {
                isPrim = false;
                break;
            }
        }
        if (isPrim && power(g, ord) == 1) return g;
    }
    throw std::runtime_error("Failed to find primitive element");
}

std::vector<uint64_t> Field::factorize(uint64_t n) const {
    std::vector<uint64_t> factors;
    for (uint64_t i = 2; i * i <= n && i < 1000000; ++i) {
        if (n % i == 0) {
            factors.push_back(i);
            while (n % i == 0) n /= i;
        }
    }
    if (n > 1) factors.push_back(n);
    return factors;
}

Field::Element::Element() : m_field(nullptr), m_value(0) {}

Field::Element::Element(const Field& field, uint64_t value) 
    : m_field(&field), m_value(value & field.elementMask()) {
    validate();
}

int64_t Field::Element::exponent() const {
    return m_value ? m_field->toExponent(m_value) : -1;
}


std::string Field::Element::toVector() const {
    return Utils::toBinary(m_value, m_field->degree());
}

std::string Field::Element::toPolynomial() const {
    return Utils::toPolynomial(m_value, m_field->degree());
}

std::string Field::Element::toHex() const {
    return Utils::toHex(m_value);
}

std::string Field::Element::toExponential() const {
    if (m_value == 0) return "0";
    return "g^" + std::to_string(exponent());
}

std::string Field::Element::toString() const {
    return toPolynomial() + " (" + toHex() + ")";
}

Field::Element Field::Element::operator+(const Element& other) const {
    checkSameField(other);
    return Element(*m_field, m_field->add(m_value, other.m_value));
}

Field::Element Field::Element::operator-(const Element& other) const {
    return *this + other;
}

Field::Element Field::Element::operator-() const {
    // В GF(2^m) характеристика равна 2, поэтому -a = a
    return *this;
}

Field::Element Field::Element::operator*(const Element& other) const {
    checkSameField(other);
    return Element(*m_field, m_field->multiply(m_value, other.m_value));
}

Field::Element Field::Element::operator/(const Element& other) const {
    checkSameField(other);
    return Element(*m_field, m_field->divide(m_value, other.m_value));
}

Field::Element Field::Element::operator^(uint64_t exp) const {
    return Element(*m_field, m_field->power(m_value, exp));
}

Field::Element Field::Element::inverse() const {
    return Element(*m_field, m_field->inverse(m_value));
}

bool Field::Element::operator==(const Element& other) const {
    return m_field == other.m_field && m_value == other.m_value;
}

bool Field::Element::operator!=(const Element& other) const {
    return !(*this == other);
}

bool Field::Element::operator<(const Element& other) const {
    return m_value < other.m_value;
}

size_t Field::Element::hammingWeight() const {
    return Utils::hammingWeight(m_value);
}

uint64_t Field::Element::dotProduct(const Element& other) const {
    checkSameField(other);
    return Utils::scalarProduct(m_value, other.m_value);
}

void Field::Element::validate() const {
    if (!m_field) throw std::runtime_error("Element not associated with field");
    if (m_value > m_field->elementMask()) {
        throw std::out_of_range("Element value exceeds field range");
    }
}

void Field::Element::checkSameField(const Element& other) const {
    if (m_field != other.m_field) {
        throw std::invalid_argument("Elements from different fields");
    }
}

Field::Element randomElement(const Field& field, std::mt19937_64& rng) {
    return field.fromValue(Utils::randomVector(field.degree(), rng));
}

Field::Element randomNonZeroElement(const Field& field, std::mt19937_64& rng) {
    Field::Element elem;
    do {
        elem = randomElement(field, rng);
    } while (elem.value() == 0);
    return elem;
}

Field::Element randomElementWithWeight(const Field& field, size_t weight, 
                                       std::mt19937_64& rng) {
    return field.fromValue(
        Utils::randomVectorWithWeight(field.degree(), weight, rng));
}

}