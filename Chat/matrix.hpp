#pragma once

#include "vector.hpp"
#include <iostream>
#include <vector>
#include <stdexcept>

class Matrix {
    protected:
        int rij, kol;
        std::vector<float> m;

    public:
        Matrix(int rij, int kol);
        Matrix(int rij, int kol, const std::vector<float> &waarden);

        int rijen() const;
        int kolommen() const;

        void set(int r, int k, float waarde);
        float at(int r, int k) const;

        bool operator==(const Matrix &andere) const;
        bool operator!=(const Matrix &andere) const;

        Matrix operator*(const Matrix &andere) const;
        Vector operator*(const Vector &v) const;
        friend std::ostream &operator<<(std::ostream &os, const Matrix &mat);

        Matrix getransponeerd() const;

        Vector naarCartesisch(const Vector &v) const;

        float determinant() const;

        Matrix inverse() const;

        void naarGL(float *doel) const;
};

class Matrix3 : public Matrix {
    public:
        Matrix3();
        Matrix3(const std::vector<float> &waarden);
        static Matrix3 eenheid();
};

class Matrix4 : public Matrix{
    public:
        Matrix4();
        Matrix4(const std::vector<float> &waarden);
        static Matrix4 eenheid();
        void naarGL(float *doel) const;
};

enum TransformatieType { ROTATIE, TRANSLATIE, SCHALING, PERSPECTIEF };
enum AsType { X_AS, Y_AS, Z_AS };

class Transformatie : public Matrix4 {
    private:
        TransformatieType type;
        AsType as;
        float waarde;
        float dx, dy, dz;
        float fovy, aspect, near, far;

    public:
        Transformatie(TransformatieType type, float waarde, AsType as = X_AS);

        static Transformatie rotatie(float hoek, AsType as);
        static Transformatie translatie(float dx, float dy, float dz);
        static Transformatie schaling(float sx, float sy, float sz);
        static Transformatie perspectief(float fovy, float aspect,
                                         float near, float far);

        Matrix4 naarMatrix() const;
};