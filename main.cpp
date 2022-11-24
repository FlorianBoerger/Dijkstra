#include <iostream>
#include <thread>
#include "DijkstraParrallel.h"
#include "DijkstraSequentiell.h"

using namespace std;
int main(int argc, char *argv[])
{
    cout << "Hello Dijkstra's World!" << endl;

    auto filename = "/home/student/Schreibtisch/dijkstra/USA-road-d.NY.gr";


    auto par = DijkstraParrallel(filename);
    auto seq = DijkstraSequentiell(filename);

   // Creating parallel Dijkstra-graph
    auto start_read_graph = chrono::high_resolution_clock::now();
    par.readGraph();
    auto end_read_graph = chrono::high_resolution_clock::now() - start_read_graph;

    long long microseconds_read_graph = chrono::duration_cast<chrono::microseconds>(end_read_graph).count();
    cout <<"Reading parallel graph: " << microseconds_read_graph << " us" << endl;


    auto start_create_graph = chrono::high_resolution_clock::now();
    par.createGraph();
    auto end_create_graph = chrono::high_resolution_clock::now() - start_read_graph;
    long long microseconds_create_graph = chrono::duration_cast<chrono::microseconds>(end_create_graph).count();
    cout <<"Creating parallel graph: " << microseconds_create_graph <<  " us" << endl;


    auto start_exec_graph = chrono::high_resolution_clock::now();
    par.execGraph();
    auto end_exec_graph = chrono::high_resolution_clock::now() - start_read_graph;
    long long microseconds_exec_graph = chrono::duration_cast<chrono::microseconds>(end_exec_graph).count();
    cout <<"Creating parallel graph: " << microseconds_exec_graph <<  " us" << endl;


    // Creating Sequentiell Dijkstra-graph
    auto start_read_graph_seq = chrono::high_resolution_clock::now();
    seq.readGraph();
    auto end_read_graph_seq = chrono::high_resolution_clock::now() - start_read_graph_seq;

    long long microseconds_read_graph_seq = chrono::duration_cast<chrono::microseconds>(end_read_graph_seq).count();
    cout <<"Reading Sequentiell graph: " << microseconds_read_graph_seq << " us" << endl;


    auto start_create_graph_seq = chrono::high_resolution_clock::now();
    seq.createGraph();
    auto end_create_graph_seq = chrono::high_resolution_clock::now() - start_read_graph_seq;
    long long microseconds_create_graph_seq = chrono::duration_cast<chrono::microseconds>(end_create_graph_seq).count();
    cout <<"Creating Sequentiell graph: " << microseconds_create_graph_seq <<  " us" << endl;


    auto start_exec_graph_seq = chrono::high_resolution_clock::now();
    seq.execGraph();
    auto end_exec_graph_seq = chrono::high_resolution_clock::now() - start_read_graph_seq;
    long long microseconds_exec_graph_seq = chrono::duration_cast<chrono::microseconds>(end_exec_graph_seq).count();
    cout <<"Creating Sequentiell graph: " << microseconds_exec_graph_seq <<  " us" << endl;

    string input;
    do {
        cout << "Print hops to destination: ";
        getline(cin, input);
        cout << endl;
        int dest = input == "max" ? par.graph.size() - 1 : atoi(input.c_str());


        if(dest > 0){
            auto start_hop_graph = chrono::high_resolution_clock::now();
            seq.printHops(dest);
            auto end_hop_graph = chrono::high_resolution_clock::now() - start_hop_graph;
            long long microseconds_hop_graph = chrono::duration_cast<chrono::microseconds>(end_hop_graph).count();
            cout <<"Printing parallel graph: " << microseconds_hop_graph <<  " us" << endl;

        }

    } while(input != "exit");

    return 0;
}