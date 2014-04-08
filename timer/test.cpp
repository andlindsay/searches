#include <ctime>
#include <cstdio>
#include "unistd.h"
#include <iostream>

using namespace std;

int main() {
	clock_t start, stop;

    // first, use std clock() function to measure elapsed time ////////////////
    start = stop = clock(); // start timer and remember initial ticks

    while(stop-start < CLOCKS_PER_SEC * 1.5)
        stop = clock();

	cout << (double)((double)(stop - start) / CLOCKS_PER_SEC) << "s." << endl;
	cout << CLOCKS_PER_SEC << endl;
	return 0;
}