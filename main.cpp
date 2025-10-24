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
        cout << "\tloop_counter is a positive floating-point value" << endl << endl;
        return 1;
    }

    // Use float (32-bit) as required
    float loop_bound    = stof(argv[1]);
    float loop_counter  = stof(argv[2]);

    cout << "\nLoop bound:   "  << floatToBits(loop_bound) << endl;
    cout << "Loop counter: " << floatToBits(loop_counter) << endl;
    cout << endl;

    // Compute canonical threshold T: smallest float T such that T + loop_counter == T.
    // Start at loop_counter and double until addition is lost (or we reach max).
    float threshold = loop_counter;
    const float fmax = numeric_limits<float>::max();

    // Handle degenerate cases: loop_counter == 0
    if (loop_counter == 0.0f) {
        // No meaningful threshold (adding 0 never changes), treat as no overflow.
        cout << "There is no overflow!" << endl;
        return 0;
    }

    // Doubling loop: stop if threshold gets to inf or adding stops changing it.
    while (threshold < fmax) {
        float sum = threshold + loop_counter;
        if (sum == threshold) break;    // found threshold
        // Prevent infinite loop due to denormals: use nextafter to ensure progress
        float nextThreshold = threshold * 2.0f;
        if (!isfinite(nextThreshold) || nextThreshold <= threshold) {
            // can't increase further — stop
            break;
        }
        threshold = nextThreshold;
    }

    // If loop ended without finding sum==threshold, set threshold = largest float where sum==threshold
    // (if not found, threshold may be extremely large; the check below will still be valid).
    // Now decide if the provided loop_bound would overflow (i.e., lose the increment).
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




