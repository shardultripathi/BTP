#include <algorithm>
#include <fstream>

using namespace std;

void trim(string &str) {
    int i = 0, j = 0;
    while (str[i] == ' ') {i++;}
    while (str[i] != ' ') {str[j++] = str[i++];}
    str = str.substr(0,j);
}

int main() 
{
	int a, b;
    string str;
    ifstream fin;
    fin.open("edges");
    ofstream fout("edgelist");
    while (fin >> str) {
        replace(str.begin(), str.end(), '[', ' ');
        replace(str.begin(), str.end(), '(', ' ');
        replace(str.begin(), str.end(), ',', ' ');
        trim(str);
        a = stoi(str);
        fin >> str;
        replace(str.begin(), str.end(), ']', ' ');
        replace(str.begin(), str.end(), ')', ' ');
        replace(str.begin(), str.end(), ',', ' ');
        trim(str);
        b = stoi(str);
        fout << a << ' ' << b << " {}\n";
    }
    fin.close();
    fout.close();
}