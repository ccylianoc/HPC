#include "matrix.hpp"
#include <cmath>


// Matrix 
Matrix:: Matrix(int rij, int kol) : rij(rij), kol(kol), m(rij * kol, 0.0f){}
Matrix:: Matrix(int rij, int kol, const std::vector<float> &waarden): rij(rij), kol(kol) {
    if ((int)waarden.size() != rij * kol) {
        throw std::invalid_argument("Aantal waarden komt niet overeen met afmetingen [Matrix]");
    }
    m = waarden;
}

int Matrix:: rijen() const {return rij;}
int Matrix:: kolommen() const{return kol;}

void Matrix::set(int r, int k, float waarde) {
    if (r < 0 || r >= rij || k < 0 || k >= kol) {
        throw std::out_of_range("Index buiten bereik [set]");
    }
    m[r * kol + k] = waarde;
}

float Matrix::at(int r, int k) const {
    if (r < 0 || r >= rij || k < 0 || k >= kol) {
        throw std::out_of_range("Index buiten bereik [at]");
    }
    return m[r * kol + k];
}


bool Matrix:: operator==(const Matrix &andere) const{
    if (rij != andere.rij || kol != andere.kol) return false;
    for (int i = 0; i < (int)m.size(); i++) {
        if (std::fabs(m[i] - andere.m[i]) > 1e-6f) return false;
    }
    return true;
}
bool Matrix:: operator!=(const Matrix &andere) const{
    return !(*this == andere);
}

Matrix Matrix:: operator+(const Matrix &andere) const{
    if (rij != andere.rij || kol != andere.kol) {
        throw std::invalid_argument("Afmetingen komen niet overeen [op+]");
    }
    Matrix resultaat(rij, kol);
    for (int i = 0; i < (int)m.size(); i++) {
        resultaat.m[i] = m[i] + andere.m[i];
    }
    return resultaat;
}

Matrix Matrix:: operator-(const Matrix &andere) const{
    if (rij != andere.rij || kol != andere.kol) {
        throw std::invalid_argument("Afmetingen komen niet overeen [op-]");
    }
    Matrix resultaat(rij, kol);
    for (int i = 0; i < (int)m.size(); i++) {
        resultaat.m[i] = m[i] - andere.m[i];
    }
    return resultaat;
}

Matrix Matrix:: operator*(const Matrix &andere) const{
    if(kol != andere.rij){
        throw std::invalid_argument("Aantal kol != aantal rij [op*]");
    }
    Matrix resultaat(rij, andere.kol);
    for (int r = 0; r < rij; r++) {
        for (int k = 0; k < andere.kol; k++){
            float som = 0.0f;
            for (int i = 0; i < kol; i++){
                som += at(r, i) * andere.at(i, k);
            }
            resultaat.set(r, k, som);
        }
    }
    return resultaat;

}
Vector Matrix:: operator*(const Vector &v) const{
    if (kol != v.size()) {
        throw std::invalid_argument("Aantal kolommen != vectorlengte [Matrix*Vector]");
    }
    Vector resultaat(rij);
    for (int r = 0; r < rij; r++) {
        float som = 0.0f;
        for (int i = 0; i < kol; i++) {
            som += at(r, i) * v.at(i);
        }
        resultaat.set(r, som);
    }
    return resultaat;
}

std::ostream &operator<<(std::ostream &os, const Matrix &mat) {
    for (int r = 0; r < mat.rijen(); r++) {
        os << "[ ";
        for (int k = 0; k < mat.kolommen(); k++) {
            os << mat.at(r, k) << " ";
        }
        os << "]\n";
    }
    return os;
}


Matrix Matrix:: getransponeerd() const{
    Matrix resultaat(kol, rij);
    for (int r = 0; r < rij; r++){
        for (int k = 0; k < kol; k++){
            resultaat.set(k, r, at(r, k));
        }
    }
    return resultaat;
}

Vector Matrix:: naarCartesisch(const Vector &v) const{
    if (v.size() < 2){
        throw std::invalid_argument("Vector te kort [naarCartesisch]");
    }
    float w = v.at(v.size() -1);
    if (w == 0.0f) {
        throw std::invalid_argument("Deling door nul [naarCartesisch]");
    }
    Vector resultaat(v.size() -1);
    for (int i = 0; i < resultaat.size(); i++){
        resultaat.set(i, v.at(i)/ w);
    }
    return resultaat;
}

float Matrix::determinant() const {
    if (rij != kol) {
        throw std::invalid_argument("Determinant alleen voor vierkante matrices.");
    }

    if (rij == 1) {
        return at(0, 0);
    }

    if (rij == 2) {
        return at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0);
    }

    if (rij == 3) {
        return at(0, 0) * (at(1, 1) * at(2, 2) - at(1, 2) * at(2, 1))
             - at(0, 1) * (at(1, 0) * at(2, 2) - at(1, 2) * at(2, 0))
             + at(0, 2) * (at(1, 0) * at(2, 1) - at(1, 1) * at(2, 0));
    }

    throw std::invalid_argument("Determinant nog niet geimplementeerd voor deze maat.");
}

Matrix Matrix::inverse() const {
    if (rij != kol) {
        throw std::invalid_argument("Inverse alleen voor vierkante matrices.");
    }

    float det = determinant();
    if (std::fabs(det) < 1e-6f) {
        throw std::invalid_argument("Matrix is singulier, geen inverse.");
    }
    if (rij == 2) {
        Matrix resultaat(2, 2);
        resultaat.set(0, 0,  at(1, 1) / det);
        resultaat.set(0, 1, -at(0, 1) / det);
        resultaat.set(1, 0, -at(1, 0) / det);
        resultaat.set(1, 1,  at(0, 0) / det);
        return resultaat;
    }
    if (rij == 3) {
        float a = at(0,0), b = at(0,1), c = at(0,2);
        float d = at(1,0), e = at(1,1), f = at(1,2);
        float g = at(2,0), h = at(2,1), i = at(2,2);

        Matrix resultaat(3, 3);
        resultaat.set(0, 0, (e*i - f*h) / det);
        resultaat.set(0, 1, (c*h - b*i) / det);
        resultaat.set(0, 2, (b*f - c*e) / det);
        resultaat.set(1, 0, (f*g - d*i) / det);
        resultaat.set(1, 1, (a*i - c*g) / det);
        resultaat.set(1, 2, (c*d - a*f) / det);
        resultaat.set(2, 0, (d*h - e*g) / det);
        resultaat.set(2, 1, (b*g - a*h) / det);
        resultaat.set(2, 2, (a*e - b*d) / det);
        return resultaat;
    }
    throw std::invalid_argument("Inverse nog niet geimplementeerd voor deze maat.");
}

void Matrix::naarGL(float *doel) const {
    for (int r = 0; r < 4; r++)
        for (int k = 0; k < 4; k++)
            doel[k * 4 + r] = at(r, k);
}


// Matrix 3

Matrix3::Matrix3() : Matrix(3,3) {}

Matrix3::Matrix3(const std::vector<float> &waarden) : Matrix(3,3, waarden) {}

Matrix3 Matrix3::eenheid() {
    Matrix3 resultaat;
    for (int i = 0; i < 3; i++){
        resultaat.set(i, i, 1.0f);
    }
    return resultaat;
}

// Matrix 4

Matrix4::Matrix4() : Matrix(4,4) {
    for (int i = 0; i < 4; i++) set(i, i, 1.0f);
}

Matrix4::Matrix4(const std::vector<float> &waarden) : Matrix(4,4, waarden) {}

Matrix4 Matrix4:: eenheid() {
    Matrix4 resultaat;
    for (int i = 0; i < 4; i++){
        resultaat.set(i,i, 1.0f);
    }
    return resultaat;
}

void Matrix4::naarGL(float *doel) const {
    for (int r = 0; r < 4; r++) {
        for (int k = 0; k < 4; k++) {
            doel[k * 4 + r] = at(r, k);
        }
    }
}


// Transformatie

Transformatie::Transformatie(TransformatieType type, float waarde, AsType as)
    : Matrix4(),          
      type(type), as(as), waarde(waarde),
      dx(0), dy(0), dz(0),
      fovy(0), aspect(0), near(0), far(0) {}

Transformatie Transformatie::translatie(float dx, float dy, float dz) {
    Transformatie t(TRANSLATIE, 0);
    t.dx = dx;
    t.dy = dy;
    t.dz = dz;
    return t;
}

Transformatie Transformatie::schaling(float sx, float sy, float sz) {
    Transformatie t(SCHALING, 0.0f, X_AS);
    t.dx = sx;
    t.dy = sy;
    t.dz = sz;
    return t;
}

Transformatie Transformatie::rotatie(float hoek, AsType as) {
    return Transformatie(ROTATIE, hoek, as);
}

Transformatie Transformatie::perspectief(float fovy, float aspect, float near, float far) {
    Transformatie t(PERSPECTIEF, 0.0f, X_AS);
    t.fovy = fovy;
    t.aspect = aspect;
    t.near = near;
    t.far = far;
    return t;
}

Matrix4 Transformatie::naarMatrix() const {
    Matrix4 resultaat = Matrix4::eenheid();
    switch (type) {
        case TRANSLATIE: { 
            resultaat.set(0, 3, dx);
            resultaat.set(1, 3, dy);
            resultaat.set(2, 3, dz);
            break; 
        }
        case SCHALING:   {
            resultaat.set(0, 0, dx);
            resultaat.set(1, 1, dy);
            resultaat.set(2, 2, dz);
            break;
        }
        case ROTATIE:    {
            float hoek = waarde;
            float c = std::cos(hoek);
            float s = std::sin(hoek);
            switch (as) {
                case X_AS:
                    resultaat.set(1, 1, c);
                    resultaat.set(1, 2, -s);
                    resultaat.set(2, 1, s);
                    resultaat.set(2, 2, c);
                    break;
                case Y_AS:
                    resultaat.set(0, 0, c);
                    resultaat.set(0, 2, s);
                    resultaat.set(2, 0, -s);
                    resultaat.set(2, 2, c);
                    break;
                case Z_AS:
                    resultaat.set(0, 0, c);
                    resultaat.set(0, 1, -s);
                    resultaat.set(1, 0, s);
                    resultaat.set(1, 1, c);
                    break;
            }
            break;
        }
        case PERSPECTIEF: {
            float f = 1.0f / std::tan(fovy / 2.0f);
            resultaat.set(0, 0, f / aspect);
            resultaat.set(1, 1, f);
            resultaat.set(2, 2, (far + near) / (near - far));
            resultaat.set(2, 3, (2.0f * far * near) / (near - far));
            resultaat.set(3, 2, -1.0f);
            resultaat.set(3, 3, 0.0f);
            break;
        }
        default:
            throw std::invalid_argument("Onbekend transformatietype [naarMatrix]");
    }
    return resultaat;
}
