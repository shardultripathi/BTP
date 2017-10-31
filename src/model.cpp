#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <algorithm>
#include <math.h>
#include <omp.h>
#define GMAX 500 // no. of topics to be added to globalList
#define UMAX 1000 // no. of times a user will tweet
#define TMAX 1500 // max time to simulate
#define GINIT 20 // initial no. of topics in global list
#define rewire 0.1

using namespace std;

void trim(string &str) {
    int i = 0, j = 0;
    while (str[i] == ' ') {i++;}
    while (str[i] != ' ') {str[j++] = str[i++];}
    str = str.substr(0,j);
}

class Graph {
public:
    int numUsers;
    vector< vector<int> > ngb;
    void setNumUsers(int n) {
        numUsers = n;
        ngb.resize(n);
    }

    void addEdge(int u, int v) {
        ngb[u].push_back(v);
        ngb[v].push_back(u);
    }

    void init() {
        int a, b;
        string str;
        ifstream fin;
        fin.open("edges");
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
            addEdge(a,b);
        }
        fin.close();
        cout << "graph initialised" << endl;
    }
};

class Model {
public:
    vector<double> globalList;
    vector<int> gtime;
    vector< vector<double> > localList;
    vector< vector<int> > localTopics;
    vector< vector<int> > ltime;
    Graph g;
    double A, B, a, b, l1, l2;
    ofstream outfile;

    void init(int n, double AA, double BB, double aa, double bb, double ll1, double ll2, string of_name) {
        g.setNumUsers(n);
        localList.resize(n);
        localTopics.resize(n);
        ltime.resize(n);
        A = AA;
        B = BB;
        a = aa;
        b = bb;
        l1 = ll1;
        l2 = ll2;
        outfile.open(of_name);
        cout << "model initialised" << endl;
    }

    inline double nextTime(double rateParameter) {
        return -log(1. - (double) rand() / (RAND_MAX + 1.)) / rateParameter;
    }

    void getTimeSim() {
        // calculates the times at which (global/user) topic adoptions are going to happen
        int tmp = (int)nextTime(l1);
        int tmp1;
        gtime.push_back(tmp);
        for (int i = 1; i < GMAX; i++) {
            do {
                tmp1 = (int)nextTime(l1);
            } while (tmp1 == 0);
            tmp = gtime[i-1] + tmp1;
            gtime.push_back(tmp);
        }
        int n = g.numUsers;
        for (int i = 0; i < n; i++) {
            tmp = gtime[GINIT-1] + (int)nextTime(l2);
            ltime[i].push_back(tmp);
            for (int j = 1; j < UMAX; j++) {
                do {
                    tmp1 = (int)nextTime(l1);
                } while (tmp1 == 0);
                tmp = ltime[i][j-1] + tmp1;
                ltime[i].push_back(tmp);
                // cout << tmp << " ";
            }
            // cout << endl;
        }
    }

    void timeSim() {
        cout << "simulating time" << endl;
        int n = g.numUsers, size;
        int gc = 0;
        vector<int> lc(n,0);
        vector<double> topic_weight(GMAX);
        vector<int> topic_num(GMAX);
        int i,j,k,l,topic_id;
        for (int t = 0; t < TMAX; t++) {
            // update weights
            size = globalList.size();
            for (i = 0; i < size; i++) {
                globalList[i] /= exp(a);
            }
            for (i = 0; i < n; i++) {
                size = localList[i].size();
                for (j = 0; j < size; j++) {
                    localList[i][j] /= exp(b);
                }
            }
            // global topic addition
            if (gtime[gc] == t) {
                gc++;
                globalList.push_back(A);
            }
            if (!gc)
                continue;
            // for all users topic adoption
            for (i = 0;i < gc; i++) {
                topic_num[i] = 0;
            }
            for (i = 0; i < n; i++) {
                if (ltime[i][lc[i]] == t) {
                    // cout << i << " ";
                    lc[i]++;
                    for(j = 0; j < gc; j++) {
                        topic_weight[j] = globalList[j];
                    }
                    for(j = 0; j < g.ngb[i].size(); j++) {
                        k = g.ngb[i][j];
                        for(l = 0; l < localTopics[k].size(); l++) {
                            topic_weight[localTopics[k][l]] += localList[k][l];
                        }
                    }
                    for(j = 1; j < gc; j++) {
                        topic_weight[j] += topic_weight[j-1];
                    }
                    auto it = lower_bound(topic_weight.begin(), topic_weight.begin()+gc, ((double) rand() / (RAND_MAX + 1.))*topic_weight[gc-1]);
                    topic_id = (int) (it - topic_weight.begin());
                    localTopics[i].push_back(topic_id);
                    topic_num[topic_id]++;
                    localList[i].push_back(B);
                    // outfile << i << " " << topic_id << " ";
                }
            }
            for (j = 0; j < gc; j++) {
                outfile << topic_num[j] << " ";
            }
            outfile << endl;
            // cout << endl;
        }
        // cout << "gc: " << gc << endl;
        outfile.close();
    }

    void run() {
        g.init();
        getTimeSim();
        timeSim();
    }

    ~Model() {
        cout << "deleting model" << endl;
    }
};

int main(int argc, char const *argv[]) {
    // code
    if (argc < 3) {
        cout << "usage: " << argv[0] << " <0/1> <number of users>\n";
        return -1;
    }

    int n = stoi(argv[2]);

    if (argv[1][0] == '1') {
        #pragma omp parallel
        {
            srand(int(time(NULL)) ^ omp_get_thread_num());
            int tid = omp_get_thread_num();
            string fname = "../data/nodesVsTime" + to_string(tid) + ".dat";
            Model m;
            switch(tid) {
                    //     Model(n, A,       B,    a,   b,    l1,    l2)
                case 0: m.init(n, 10000.0, 100.0, 0.2, 0.5, 1.0/5, 1.0/3, fname); m.run(); break;
                case 1: m.init(n, 10000.0, 100.0, 0.2, 0.2, 1.0/5, 1.0/3, fname); m.run(); break;
                case 2: m.init(n, 10000.0, 100.0, 0.2, 0.01, 1.0/5, 1.0/3, fname); m.run(); break;
                case 3: m.init(n, 10000.0, 100.0, 0.2, 0.005, 1.0/5, 1.0/3, fname); m.run(); break;
                case 4: m.init(n, 100.0, 100.0, 0.2, 0.5, 1.0/5, 1.0/3, fname); m.run(); break;
                case 5: m.init(n, 100.0, 100.0, 0.2, 0.2, 1.0/5, 1.0/3, fname); m.run(); break;
                case 6: m.init(n, 100.0, 100.0, 0.2, 0.01, 1.0/5, 1.0/3, fname); m.run(); break;
                case 7: m.init(n, 100.0, 100.0, 0.2, 0.005, 1.0/5, 1.0/3, fname); m.run(); break;
                default: ;
            }
        }
    } else {
        srand(time(NULL));
        string fname = "../data/nodesVsTime0.dat";
        Model m;
        // Model(n,   A,    B,     a,   b,    l1,    l2)
        m.init(n, 1.0, 100.0, 0.2, 0.1, 1.0/5, 1.0/3, fname);
        m.run();
    }
    return 0;
}