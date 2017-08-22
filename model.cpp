#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <math.h>
#define GMAX 100 // no. of topics to be added to globalList
#define UMAX 1000 // no. of times a user will tweet
#define TMAX 1000 // max time to simulate
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
        /*size = ngb[u].size();
        for(i=0;i<size;i++) {
            if(ngb[u][i] == v) {
                ngb[u][i] = w;
                break;
            }
        }*/
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
        // TODO: do edge swapping
        //double rewiringProb = 0.1;
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
        cout << "graph initiated" << endl;
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

    Model(int n, int k, double AA, double BB, double aa, double bb, double ll1, double ll2, string of_name) {
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
        cout << "model created" << endl;
    }

    double nextTime(double rateParameter) {
        return -log(1. - (double) rand() / (RAND_MAX + 1.)) / rateParameter;
    }

    void getTimeSim() {
        // calculates the times at which (global/user) topic adoptions are going to happen
        // TODO: intial set of topics
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
            tmp = (int)nextTime(l2);
            ltime[i].push_back(tmp);
            for (int j = 1; j < UMAX; j++) {
                do {
                    tmp1 = (int)nextTime(l1);
                } while (tmp1 == 0);
                tmp = ltime[i][j-1] + tmp1;
                ltime[i].push_back(tmp);
                // cout<<tmp<<" ";
            }
            // cout<<endl;
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
                    // cout<<i<<" ";
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
                    // outfile<<i<<" "<<topic_id<<" ";
                }
            }
            for (j = 0; j < gc; j++) {
                outfile << topic_num[j] << " ";
            }
            outfile << endl;
            // cout<<endl;
        }
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

int main() {
    // code
    srand(time(NULL));
    // Model(int n, int k, double AA, double BB, double aa, double bb, double ll1, double ll2)
    Model m(10001, 200, 1.0, 0.4, 0.5, 0.3, 1.0/4, 1.0/2, "nodesVsTime.dat");
    m.run();
    return 0;
}
