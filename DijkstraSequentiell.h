//
// Created by svenpete@id.hsos.de on 17.11.22.
//

#ifndef DIJKSTRA_DIJKSTRASEQUENTIELL_H
#define DIJKSTRA_DIJKSTRASEQUENTIELL_H


#include <fstream>
#include <sstream>
#include <tbb/tbb.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <chrono>
#include <iterator>
#include <vector>
#include <algorithm>


using namespace std;

typedef struct
{
    unsigned dest;
    unsigned dist;
} dest_to_dist_Seq;

class DijkstraSequentiell
{
public:
    string filename;
    vector<vector<dest_to_dist_Seq>> graph;
    vector<unsigned> dist;
    vector<unsigned> prev;
    vector<bool> mrkd;
    string tag;


    DijkstraSequentiell(string filename):
            filename(filename){}



    void readGraph()
    {
        ifstream f(filename);
        string line;
        unsigned nodeCount = 0;

        while(getline(f, line)) {
            stringstream ss(line);
            string id;
            ss >> id;
            if(id == "a") {
                unsigned srcs, dest, arc;
                ss >> srcs >> dest >> arc;
                nodeCount = std::max(srcs, nodeCount);
                srcs--;
                dest--;
                if(nodeCount > graph.size())
                    graph.resize(nodeCount);
                graph[srcs].push_back({
                                              .dest = dest,
                                              .dist = arc
                                      });
            }
        }
        cout << "ANz nodes: " << std::to_string(graph.size()) << endl;
    }

    void createGraph()
    {
        dist.resize(graph.size());
        prev.resize(graph.size());
        mrkd.resize(graph.size());
                              for(size_t i = 0; i < graph.size(); i++) {
                                  dist[i] = UINT_MAX;
                                  prev[i] = 0;
                                  mrkd[i] = false;
                              }


        if(dist.size() > 0)
            dist[0] = 0;
    }

    void execGraph()
    {
        for(unsigned i = 0; i < graph.size(); i++) {
            int minDistIdx = getMinDist();
            std::vector<dest_to_dist_Seq> targetNode = graph[minDistIdx];
            mrkd[minDistIdx] = true;
            for(size_t j = 0; j < targetNode.size(); j++) {
                unsigned destIdx = targetNode[j].dest;
                unsigned relDist = targetNode[j].dist;
                if(!mrkd[destIdx]) {
                    unsigned absDist = dist[minDistIdx] + relDist;
                    if(absDist < dist[destIdx]) {
                        dist[destIdx] = absDist;
                        prev[destIdx] = minDistIdx;
                    }
                }
            }
        }
    }

    int getMinDist()
    {
        unsigned min = UINT_MAX;
        int idx = -1;
        for(size_t i = 0; i < dist.size(); i++) {
            if(!mrkd[i] && dist[i] < min) {
                min = dist[i];
                idx = i;
            }
        }
        return idx;
    }

    vector<unsigned> getHops(unsigned dest)
    {
        vector<unsigned> hops;
        if(dest < graph.size() && dest > 0) {
            unsigned dist, prev = dest;
            do {
                prev = this->prev[prev];
                dist = this->dist[prev];
                hops.push_back(prev + 1);
            } while(dist > 0);
            reverse(hops.begin(), hops.end());
        }
        return hops;
    }


    void printHops(unsigned dest)
    {
        vector<unsigned> hops = getHops(dest);
        cout << tag << left << ": " << "1-->" << dest << ": " << dist[dest] << " { ";
        copy(hops.begin(), hops.end(), ostream_iterator<unsigned>(cout, " "));
        cout << "}\n\n";
    }
};


#endif //DIJKSTRA_DIJKSTRASEQUENTIELL_H