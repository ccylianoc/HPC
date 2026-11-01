#include "m.hpp"
#include <iostream>

int main() {
    // 2x2: A * A^-1 moet de eenheidsmatrix geven
    Matrix A(2, 2, {4, 7, 2, 6});
    Matrix Ainv = A.inverse();
    std::cout << (A * Ainv) << "\n";

    // 3x3
    Matrix B(3, 3, {1, 2, 3, 0, 1, 4, 5, 6, 0});
    Matrix Binv = B.inverse();
    std::cout << (B * Binv) << "\n";

    // singuliere matrix moet een exception geven
    try {
        Matrix S(2, 2, {1, 2, 2, 4});
        S.inverse();
        std::cout << "FOUT: geen exception\n";
    } catch (const std::invalid_argument &e) {
        std::cout << "OK: " << e.what() << "\n";
    }

    return 0;
}
