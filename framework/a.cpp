#include <iostream>
#include <fstream>

using namespace std;

int main() {
    ifstream input("a.txt");

    int cnt = 6;

    while(cnt--) {
        int r, g, b;
        input >> r >> g >> b;

        float fr = r / 255.0f, fg = g / 255.0f, fb = b / 255.0f;

        printf("%.3f, %.3f, %.3f, 1\n", fr, fg, fb);
    }

    return 0;
}