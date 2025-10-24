#include <iostream>
#include <iomanip>
#include <bitset>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <limits>
using namespace std;

string floatToBits(float f) {
    uint32_t bits;
    memcpy(&bits, &f, sizeof(bits));
    string s = bitset<32>(bits).to_string();
    return s.substr(0,1) + " " + s.substr(1,8) + " " + s.substr(9);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "usage:\n\t./fp_overflow_checker loop_bound loop_counter" << endl;
        cout << "\n\tloop_bound is a positive floating-point value" << endl;
        cout << "\tloop_counter is a positive floating-point value" << endl;
        return 1;
    }

    float loop_bound   = stof(argv[1]);
    float loop_counter = stof(argv[2]);

    cout << "\nLoop bound:\t"  << floatToBits(loop_bound) << endl;
    cout << "Loop counter:\t" << floatToBits(loop_counter) << endl << endl;

    if (!isfinite(loop_counter) || loop_counter <= 0.0f) {
        cout << "There is no overflow!" << endl;
        return 0;
    }

    float two_inc = 2.0f * loop_counter;
    float lg = log2f(two_inc);        
    int k = static_cast<int>(ceil(lg));
    int e = 23 + k;                   

    const float fmax = numeric_limits<float>::max();
    float threshold;

    if (e > 127) {
        threshold = fmax;
    } else if (e < -150) {
        threshold = powf(2.0f, static_cast<float>(e)); 
        if (threshold <= 0.0f) threshold = numeric_limits<float>::denorm_min();
    } else {
        threshold = powf(2.0f, static_cast<float>(e));
        if (!isfinite(threshold) || threshold > fmax) threshold = fmax;
    }

    bool willOverflow = (loop_bound >= threshold);

    if (!willOverflow) {
        cout << "There is no overflow!" << endl;
    } else {
        cout << "Warning: Possible overflow!" << endl;
        cout << "Overflow threshold:" << endl;
        cout << "\t" << scientific << setprecision(5) << threshold << endl;
        cout << "\t" << floatToBits(threshold) << endl;
    }
    return 0;
}







