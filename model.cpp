#include <iostream>
#include <vector>
#include <random>
#include <math.h>
#define GMAX 100 // no. of topics to be added to globalList
#define UMAX 100 // no. of times a user will tweet

using namespace std;

class Graph {
public:
    int numUsers;
    int deg;
    vector< vector<int> > ngb;
    void setNumUsers(int n, int k) {
        numUsers = n;
        ngb.resize(n);
        deg = k;
    }
    void addEdge(int u, int v) {
        ngb[u].push_back(v);
        ngb[v].push_back(u);
    }
    void init() {
        int lo, hi, n = numUsers/2;
        for (int i = 0; i < numUsers; i++) {
            for (int j = 0; j < deg/2; j++) {
                lo = (numUsers + i - rand()%n + 1) % numUsers;
                hi = (i + rand()%n + 1) % numUsers;
                addEdge(i, lo); // check if edge already exists
                addEdge(i, hi);
            }
        }
        // do edge swapping
    }
};

class Model {
public:
    vector<double> globalList;
    vector<int> gtime;
    vector< vector<double> > localList;
    vector< vector<int> > ltime;
    Graph g;
    double A, B, a, b, l1, l2;

    Model(int n, int k, double AA, double BB, double aa, double bb, double ll1, double ll2) {
        g.setNumUsers(n, k);
        localList.resize(n);
        ltime.resize(n);
        A = AA;
        B = BB;
        a = aa;
        b = bb;
        l1 = ll1;
        l2 = ll2;
    }

    double nextTime(double rateParameter) {
        return -log(1. - (double) rand() / (RAND_MAX + 1.)) / rateParameter;
    }

    void timeSim() {
        // calculates the times at which (global/user) topic adoptions are going to happen
        int tmp = (int)nextTime(l1); //cout << tmp << endl;
        gtime.push_back(tmp);
        for (int i = 1; i < GMAX; i++) {
            tmp = gtime[i-1] + (int)nextTime(l1); //cout << tmp << endl;
            gtime.push_back(tmp);
        }
        int n = g.numUsers;
        for (int i = 0; i < n; i++) {
            tmp = (int)nextTime(l2);
            ltime[i].push_back(tmp);
            for (int j = 1; j < UMAX; j++) {
                tmp = ltime[i][j-1] + (int)nextTime(l2);
                ltime[i].push_back(tmp);
            }
        }        
    }

    void run() {
        g.init();
        timeSim();
    }

    ~Model() {
        cout << "deleting model" << endl;
    }
};

int main() {
    // code
    srand(time(NULL));
    Model m(10001, 20, 0.1, 0.2, 0.3, 0.1, 0.2, 0.2);
    m.run();
    return 0;
}