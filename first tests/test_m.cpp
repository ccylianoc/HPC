#include <iostream>
#include "m.hpp"
#include "v.hpp"

// to run: g++ test_m.cpp m.cpp v.cpp -o test.exe

int main() {
    float pi = 3.14159265f;

    // 1. Schaling 2, 3, 0.5 op (2, 3, 4)
    Transformatie s = Transformatie::schaling(2, 3, 0.5f);
    Vector v({2, 3, 4});
    Vector vs = s.naarMatrix() * v.homogeen();
    std::cout << vs.at(0) << " " << vs.at(1) << " " << vs.at(2) << "\n";   // 4 9 2

    // 2. Translatie (3, 4, 5) op (2, 3, 4)
    Transformatie t = Transformatie::translatie(3, 4, 5);
    Vector vt = t.naarMatrix() * v.homogeen();
    std::cout << vt.at(0) << " " << vt.at(1) << " " << vt.at(2) << "\n";   // 5 7 9

    // 3. Rotatie 90 graden om de y-as op (2, 3, 4)
    Transformatie r = Transformatie::rotatie(pi / 2.0f, Y_AS);
    Vector vr = r.naarMatrix() * v.homogeen();
    std::cout << vr.at(0) << " " << vr.at(1) << " " << vr.at(2) << "\n";   // 4 3 -2


}
