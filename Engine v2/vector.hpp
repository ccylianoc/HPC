#pragma once

#include <vector>
#include <stdexcept>
#include <iosfwd>

class Vector {
    private:
        std::vector<float> v;

    public:
        Vector(int n);
        Vector(const std::vector<float> &data);

        int size() const;
        float at(int i) const;
        void set(int i, float waarde);

        float length() const;
        Vector normalized() const;

        Vector operator+(const Vector &other) const;
        Vector operator-(const Vector &other) const;
        Vector operator*(float s) const;
        Vector operator/(float s) const;

        float dot(const Vector &other) const;
        Vector cross(const Vector &other) const;

        bool operator==(const Vector &other) const;
        bool operator!=(const Vector &other) const;

        friend std::ostream &operator<<(std::ostream &os, const Vector &vec);

        Vector homogeen() const;
};
