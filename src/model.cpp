#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <math.h>
#include <omp.h>
#define GMAX 500 // no. of topics to be added to globalList
#define UMAX 1000 // no. of times a user will tweet
#define TMAX 1500 // max time to simulate
#define GINIT 20 // initial no. of topics in global list
#define rewire 0.1

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

    bool checkEdge(int u, int v) {
        int size = ngb[u].size();
        for (int i = 0; i < size; i++) {
            if(ngb[u][i] == v)
                return true;
        }
        return false;
    }

    void changeEdge(int u, int v, int w) {
        int i,size;
        ngb[w].push_back(u);
        size = ngb[v].size();
        for(i = 0; i < size; i++) {
            if(ngb[v][i] == u) {
                ngb[v].erase(ngb[v].begin()+i);
                break;
            }
        }
    }

    void init() {
        int lo, hi, n = numUsers/2;
        int i,j,w;
        for (i = 0; i < numUsers; i++) {
            for (j = 0; j < deg/2; j++) {
                do {
                    lo = (numUsers + i - rand()%n + 1) % numUsers;
                } while(checkEdge(i,lo));
                addEdge(i, lo);
                do {
                    hi = (i + rand()%n + 1) % numUsers;
                } while(checkEdge(i,hi));
                addEdge(i, hi);
            }
        }
        for (i = 0; i < numUsers; i++) {
            for (j = 0; j < ngb[i].size(); j++) {
                if (rand() / (RAND_MAX + 1.) < rewire) {
                    do {
                        w = rand()%numUsers;
                    } while (checkEdge(i,w));
                    changeEdge(i,ngb[i][j],w);
                    ngb[i][j] = w;
                }
            }
        }
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

    void init(int n, int k, double AA, double BB, double aa, double bb, double ll1, double ll2, string of_name) {
        g.setNumUsers(n, k);
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
    if (argc < 2) {
        cout << "usage: " << argv[0] << " <0/1>\n";
        return -1;
    }

    if (argv[1][0] == '1') {
        #pragma omp parallel
        {
            srand(int(time(NULL)) ^ omp_get_thread_num());
            int tid = omp_get_thread_num();
            string fname = "../data/nodesVsTime" + to_string(tid) + ".dat";
            Model m;
            switch(tid) {
                    //     Model(n,    k,    A,       B,    a,   b,    l1,    l2)
                case 0: m.init(10001, 200, 10000.0, 100.0, 0.20, 0.1, 1.0/5, 1.0/3, fname); m.run(); break;
                case 1: m.init(10001, 200, 10000.0, 100.0, 0.15, 0.1, 1.0/5, 1.0/3, fname); m.run(); break;
                case 2: m.init(10001, 200, 10000.0, 100.0, 0.10, 0.1, 1.0/5, 1.0/3, fname); m.run(); break;
                case 3: m.init(10001, 200, 10000.0, 100.0, 0.05, 0.1, 1.0/5, 1.0/3, fname); m.run(); break;
                case 4: m.init(10001, 200, 10000.0, 100.0, 0.01, 0.1, 1.0/5, 1.0/3, fname); m.run(); break;
                case 5: m.init(10001, 200, 0.5,     100.0, 0.2, 0.1, 1.0/5, 1.0/3, fname); m.run(); break;
                case 6: m.init(10001, 200, 1.0,     100.0, 0.2, 0.1, 1.0/5, 1.0/3, fname); m.run(); break;
                case 7: m.init(10001, 200, 5.0,     100.0, 0.2, 0.1, 1.0/5, 1.0/3, fname); m.run(); break;
                default: ;
            }
        }
    } else {
        srand(time(NULL));
        string fname = "../data/nodesVsTime.dat";
        Model m;
        // Model(n,    k,      A,    B,     a,   b,    l1,    l2)
        m.init(10001, 200, 10000.0, 100.0, 0.2, 0.1, 1.0/5, 1.0/3, fname);
        m.run();
    }
    return 0;
}
