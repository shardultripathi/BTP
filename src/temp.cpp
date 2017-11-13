#include <iostream>
#include <fstream>

using namespace std;

int main() {
	ofstream fout("temp.dat");
	int A;
	float a,b;
	for (A = 10; A < 11000; A += 1000) {
		for (a = 0.005; a < 0.5; a += 0.1) {
			for (b = 0.005; b < 0.5; b += 0.1) {
				fout << A << " " << a << " " << b << endl;
			}
		}
	}
	fout.close();
	return 0;
}