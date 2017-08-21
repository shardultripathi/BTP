#include <iostream>
#include <vector>
using namespace std;

class Model {
public:
    vector<double> globalList;
    Model(int n) {

    }
    ~Model() {
        cout << "deleting model" << endl;
    }
};

int main() {
    //code
    Model m(1);
    return 0;
}