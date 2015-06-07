#include <iostream>
#include <set>
#include <sstream>
#include <fstream>
using namespace std;

struct Edge {
    Edge(int a_, int b_):
            a(a_), b(b_)
    {       
        if (a > b) {
            swap(a, b);
        }
    }
    
    bool operator<(const Edge& rhs) const {
        if (a < rhs.a) return true;
        if (a > rhs.a) return false;
        if (b < rhs.b) return true;
        if (b > rhs.b) return false;
        return false;
    }
    
    bool operator==(const Edge& rhs) const {
        return (a == rhs.a && b == rhs.b);
    }
    int a, b;
};

class Graph {
public:
    Graph(): node_count(0) { }

    void addEdge(int a, int b) {
        if (edges.find(Edge(a, b)) == edges.end()) {
            edges.insert(Edge(a, b));
        }
        if(a >= node_count)
            node_count = a+1;
        if(b >= node_count)
            node_count = b+1;
    }
    
    set<Edge> getEdges(int v) {
        set<Edge> ret;
        for (Edge e : edges) {
            if (e.a == v || e.b == v)
                ret.insert(e);
        }
        return ret;
    }
    
    set<int> getNeighbours(int v) {
        set<int> ret;
        for(Edge e : edges) {
            if(e.a == v)
                ret.insert(e.b);
            if(e.b == v)
                ret.insert(e.a);
        }
        return ret;
    }
    
    void parse(string filename) {
        ifstream file(filename);
        string row;
        char buffer[256];
        while (file.good()) {
            file.getline(buffer,256);
            row = buffer;
            istringstream iss(row);
        
            char c;                 
            iss >> c;
        
            if (c == 'e') {
                int n1, n2;
                iss >> n1;
                iss >> n2;
                addEdge(n1-1,n2-1);
            }
        }
    }
    int getVarName(int v, int m) {
        m = m%node_count;
        
        return v*node_count + m + 1;
    }
    
    string build_function() {
        ostringstream oss;
        //ascending neighbour markings
        for(int v = 0; v < node_count; v++) {
            for(int m = 0; m < node_count; m++) {
                set<int> neighbours = getNeighbours(v);
                for(int n : neighbours) {
                    oss << getVarName(n, m+1) << " ";
                }
                oss << -getVarName(v, m) << " 0" << endl;
            }
        }
        // >0 markings per vertex
        for(int v = 0; v < node_count; v++) {
            for(int m = 0; m < node_count; m++) {
                oss << getVarName(v, m) << " ";
            }
            oss << " 0" << endl;
        }
        // <2 markings per vertex
        for(int v = 0; v < node_count; v++) {
            for(int j = 0; j < node_count; j++) {
                for(int k = j+1; k < node_count; k++) {
                    oss << -getVarName(v, j) << " " << -getVarName(v,k) << " 0" << endl;
                }
            }
        }
        return oss.str();
    }
private:
    set<Edge> edges;
    int node_count;
};

int main(int argc, char** argv) {
    Graph g;
    if(argc >= 2) {
        g.parse(string(argv[1]));
    } else {
        return 0;
    }
    cout << "p cnf 1 1" << endl;
    cout << g.build_function() << endl;
    return 0;
}
