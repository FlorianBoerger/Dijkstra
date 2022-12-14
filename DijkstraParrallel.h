//
// Created by svenpete@id.hsos.de on 17.11.22.
//
#ifndef __DIJKSTRA_PAR_H
#define __DIJKSTRA_PAR_H

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
} dest_to_dist_Par;

class DijkstraParrallel
{
public:
    string filename;
    vector<vector<dest_to_dist_Par>> graph;
    vector<unsigned> dist;
    vector<unsigned> prev;
    vector<bool> mrkd;
    string tag;
    double tsum;

    DijkstraParrallel(string filename):
    filename(filename),
    tsum(0) {}



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
        tbb::parallel_for(tbb::blocked_range<size_t>(0, graph.size()),
                          [this](tbb::blocked_range<size_t> &r) {
                              for(size_t i = r.begin(); i != r.end(); i++) {
                                  dist[i] = UINT_MAX;
                                  prev[i] = 0;
                                  mrkd[i] = false;
                              }
                          }
        );
        if(dist.size() > 0)
            dist[0] = 0;
    }

    void execGraph()
    {
        //F??r alle Knoten im Graph das folgenden verhalten ausf??hren
        for(unsigned i = 0; i < graph.size(); i++) {
            //Als n??chstes wird unter den nicht besuchten Knoten nach dem mit der geringsten Distanz zum Knoten 0 gesucht
            int minDistIdx = getMinDist();
            //Die Daten des jeweiligen Knotens werden in Target Node geladen
            vector<dest_to_dist_Par> targetNode = graph[minDistIdx];
            //in diesem Vector wurde f??r jeden Knoten der Wert false hinterlegt. Nach dem Besuch eines Knotens wird dieser hier nun auf True gesetzt.
            //So k??nnen wir wissen das der Knoten schon ber??cksichtigt wurde
            mrkd[minDistIdx] = true;

            tbb::parallel_for(tbb::blocked_range<size_t>(0, targetNode.size(), 1e4),
                              [this, &targetNode, &minDistIdx](tbb::blocked_range<size_t> &r) {
                                  for(size_t j = r.begin(); j != r.end(); j++) {
                                      //Hier wird f??r jedes Ziel das von dem ausgew??hlten Knoten erreicht werden kann der Name des Knotens
                                      // und die Distanz zu diesem festgelegt
                                      unsigned destIdx = targetNode[j].dest;
                                      unsigned relDist = targetNode[j].dist;
                                      //Wenn der Knoten noch nicht besucht wurde, wird gepr??ft ob die absolute Distanz
                                      //??ber den gerade betrachteten Knoten k??rzer ist als der aktuelle Weg zum Knoten
                                      if(!mrkd[destIdx]) {
                                          unsigned absDist = dist[minDistIdx] + relDist;
                                          if(absDist < dist[destIdx]) {
                                              dist[destIdx] = absDist;
                                              prev[destIdx] = minDistIdx;
                                          }
                                      }
                                  }
                              }
            );
        }
    }

    int getMinDist()
    {
        unsigned min = UINT_MAX;
        int idx = -1;
        tbb::concurrent_vector<pair<int, unsigned>> localMins;
        tbb::parallel_for(tbb::blocked_range<size_t>(0, dist.size(), 1e3),
                          [this, &localMins](tbb::blocked_range<size_t> &r) {
                              unsigned _min = UINT_MAX;
                              int _idx = -1;
                              for(size_t i = r.begin(); i < r.end(); i++) {
                                  if(!mrkd[i] && dist[i] < _min) {
                                      _min = dist[i];
                                      _idx = i;
                                  }
                              }
                              if(_idx >= 0)
                                  localMins.push_back(std::make_pair(_idx, _min));
                          }
        );
        for(size_t i = 0; i < localMins.size(); i++) {
            if(localMins[i].second < min) {
                min = localMins[i].second;
                idx = localMins[i].first;
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


    void printHops_Par(unsigned dest)
    {
        vector<unsigned> hops = getHops(dest);
        cout << tag << left << ": " << "1-->" << dest << ": " << dist[dest] << " { ";
        copy(hops.begin(), hops.end(), ostream_iterator<unsigned>(cout, " "));
        cout << "}\n\n";
    }
};

#endif // __DIJKSTRA_PAR_H