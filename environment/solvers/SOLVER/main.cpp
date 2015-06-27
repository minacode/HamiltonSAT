#include <iostream>
#include <set>
#include <sstream>
#include <fstream>
#include <cmath>
#include <vector>
#include <map>
#include "minisat/core/Solver.h"
#include "minisat/core/SolverTypes.h"
using namespace std;
using namespace Minisat;

struct Edge {
    Edge(int a_, int b_): a(a_), b(b_) {}
    
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

    int getNodeCount() {
        return node_count;
    }

    void addEdge(int a, int b) {
        if (edges.find(Edge(a, b)) == edges.end()) {
            edges.insert(Edge(a, b));
        }
        if(a > node_count)
            node_count = a;
        if(b > node_count)
            node_count = b;
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
                addEdge(n1,n2);
            }
        }
    }
    Var getVarName(int v, int m, Solver& solver) {
        pair<int,int> vm = pair<int,int>(v,m);
            
        auto it = vm_to_var.find(vm); 
        if(it != vm_to_var.end()) {
            return it->second;
        } else {
            Var n = solver.newVar();
            vm_to_var.insert(pair<pair<int,int>, Var>(vm,n) );
            var_to_vm.insert(pair<Var, pair<int,int> >(n, vm) );
            return n;
        }
    }
    
    void buildFunction(Solver& solver) {
        vec<Lit> clause;
        //ascending neighbour markings
        for(int v = 1; v <= node_count; v++) {
            for(int m = 0; m < node_count; m++) {
                set<int> neighbours = getNeighbours(v);
                for(int n : neighbours) {
                    clause.push(mkLit(getVarName(n, (m+1)%node_count,solver), false) );
                }
                clause.push(mkLit(getVarName(v, m, solver), true) );
                solver.addClause(clause);
                clause.clear();
            }
        }

        // >0 markings per vertex
        for(int v = 1; v <= node_count; v++) {
            for(int m = 0; m < node_count; m++) {
                clause.push(mkLit(getVarName(v, m, solver), false) );
            }
            solver.addClause(clause);
            clause.clear();
        }

        // <2 markings per vertex
        for(int v = 1; v <= node_count; v++) {
            for(int j = 0; j < node_count; j++) {
                for(int k = j+1; k < node_count; k++) {
                    clause.push(mkLit(getVarName(v, j, solver), true) );
                    clause.push(mkLit(getVarName(v, k, solver), true) );
                    solver.addClause(clause);
                    clause.clear();
                }
            }
        }
    }
    
    void printCycle(Solver& solver) {
        for(int m = 0; m < node_count; m++) {
            for(int v = 1; v <= node_count; v++) {
                Var var = vm_to_var.find(pair<int,int>(v,m))->second;
                if(solver.modelValue(var) == l_True) {
                    cout << v << " ";
                    break;
                }
            }
        }
        cout << endl;   
    }
private:
    set<Edge> edges;
    map<Var, pair<int,int> > var_to_vm;
    map<pair<int,int>, Var> vm_to_var;
    int node_count;
};


int main(int argc, char** argv) {
    Graph g;
    if(argc >= 2) {
        g.parse(string(argv[1]));
    } else {
        return 0;
    }
    
    Solver solver;
    g.buildFunction(solver);
    if(!solver.solve() ) {
        cout << "s UNSATISFIABLE" << endl;
        return 20;
    } else {
        cout << "s SATISFIABLE" << endl;
        g.printCycle(solver);
        return 10;
    }
}
