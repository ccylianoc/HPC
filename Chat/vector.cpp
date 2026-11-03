#include "vector.hpp"
#include <cmath>
#include <iostream>

Vector::Vector(int n) : v(n) {}
Vector::Vector(const std::vector<float> &data) : v(data) {}

int Vector::size() const { return v.size(); }
float Vector::at(int i) const { return v.at(i); }
void Vector::set(int i, float waarde) { v.at(i) = waarde; }

float Vector::length() const {
    float som = 0.0f;
    for (float c : v) som += c * c;
    return std::sqrt(som);
}

Vector Vector::normalized() const {
    float len = length();
    if (len == 0.0f) return Vector(v.size());
    Vector result(v.size());
    for (int i = 0; i < size(); i++) result.v[i] = v[i] / len;
    return result;
}

Vector Vector::operator+(const Vector &other) const {
    if (other.size() != size()) {
        throw std::invalid_argument("Vectorgroottes komen niet overeen [op+]");
    }
    Vector result(size());
    for (int i = 0; i < size(); i++) result.v[i] = v[i] + other.v[i];
    return result;
}

Vector Vector::operator-(const Vector &other) const {
    if (other.size() != size()) {
        throw std::invalid_argument("Vectorgroottes komen niet overeen [op-]");
    }
    Vector result(size());
    for (int i = 0; i < size(); i++) result.v[i] = v[i] - other.v[i];
    return result;
}

Vector Vector::operator*(float s) const {
    Vector result(size());
    for (int i = 0; i < size(); i++) result.v[i] = v[i] * s;
    return result;
}

Vector Vector::operator/(float s) const {
    if (s == 0.0f) {
        throw std::invalid_argument("Deling door nul [op/]");
    }
    Vector result(size());
    for (int i = 0; i < size(); i++) result.v[i] = v[i] / s;
    return result;
}

float Vector::dot(const Vector &other) const {
    if (other.size() != size()) {
        throw std::invalid_argument("Vectorgroottes komen niet overeen [dot]");
    }
    float som = 0.0f;
    for (int i = 0; i < size(); i++) som += v[i] * other.v[i];
    return som;
}

Vector Vector::cross(const Vector &other) const {
    if (size() != 3 || other.size() != 3) {
        throw std::invalid_argument("Kruisproduct alleen gedefinieerd voor 3D [cross]");
    }
    return Vector({v[1] * other.v[2] - v[2] * other.v[1],
                   v[2] * other.v[0] - v[0] * other.v[2],
                   v[0] * other.v[1] - v[1] * other.v[0]});
}

bool Vector::operator==(const Vector &other) const {
    if (size() != other.size()) return false;
    for (int i = 0; i < size(); i++) {
        if (std::fabs(v[i] - other.v[i]) > 1e-6f) return false;
    }
    return true;
}

bool Vector::operator!=(const Vector &other) const {
    return !(*this == other);
}

std::ostream &operator<<(std::ostream &os, const Vector &vec) {
    os << "(";
    for (int i = 0; i < vec.size(); i++) {
        os << vec.v[i];
        if (i < vec.size() - 1) os << ", ";
    }
    os << ")";
    return os;
}

Vector Vector::homogeen() const{
    if (size() != 2 && size() != 3) {
        throw std::invalid_argument("Homogene coordinaten alleen voor 2D of 3D [homogeen]");
    }
    Vector resultaat(size() + 1);
    for (int i = 0; i < size(); i++) {
        resultaat.set(i, at(i));
    }
    resultaat.set(size(), 1.0f);
    return resultaat;
}