#include <iostream>
#include <fstream>

using namespace std;

int main() {
	ofstream fout("temp.dat");
	int A;
	float a,b;
	int count = 0;
	for (A = 10; A < 11000; A += 1000) {
		for (a = 0.005; a < 0.5; a += 0.1) {
			for (b = 0.005; b < 0.5; b += 0.1, count = (count+1)%8) {
				if (count == 0)
					fout << "{";
				fout << "{" << A << ", " << a << ", " << b;
				if (count == 7)
					fout << "}}, ";
				else
					fout << "}, ";
			}
		}
	}
	fout.close();
	return 0;
}