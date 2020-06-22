// Project Identifier: 5949F553E20B650AB0FB2266D3C0822B13D248B0

#include <iostream>
#include <limits>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <math.h>
#include <getopt.h>
using namespace std;

enum Terrain {Land, Sea, Coast};

struct Vertex {
    Terrain type;
    int x;
    int y;

    Vertex(int x, int y) : x(x), y(y) {
        if (x < 0 && y < 0) {
            type = Sea;
        }
        else if ((x == 0 && y <= 0) || (y == 0 && x <= 0)) {
            type = Coast;
        }
        else {
            type = Land;
        }
    }

    Vertex() {}
};

struct Node {
    Vertex *vert;
    double maxDist = numeric_limits<double>::infinity();
    int pred;
    int isVisited = 0;

    Node(Vertex *vert) : vert(vert) {}
};

struct OptiTSP {
    vector<int> currPath;
    vector<int> bestPath;
    double currDist = 0;
    double bestDist = 0;

    OptiTSP() {}

    OptiTSP(size_t numVerts) {
        currPath.reserve(numVerts);
        bestPath.reserve(numVerts);
    }

};

void mstDriver(vector<Vertex> &vertices);
void fastDriver(vector<Vertex> &vertices);
double fastTSP(vector<Vertex> &vertices, vector<int> &path);
void optiDriver(vector<Vertex> &vertices);
double distance(Vertex *left, Vertex *right);
double distance2(Vertex &left, Vertex &right);

int main(int argc, char *argv[]) {
    std::ios_base::sync_with_stdio(false);
    cout << setprecision(2);
    cout << fixed;
    int choice;
    int option_index = 0;
    option long_options[] = {
        {"mode", required_argument,       nullptr, 'm'},
        {"help", no_argument,             nullptr, 'h'},
        {nullptr, 0,                      nullptr, '\0'}
    };

    char mode = 'x';
    while ((choice = getopt_long(argc, argv, "m:h", long_options, &option_index)) != -1) {
        switch (choice) {
        case 'm':
            if (string(optarg) == "MST") {
                mode = 'a';
            }
            else if (string(optarg) == "FASTTSP") {
                mode = 'b';
            }
            else if (string(optarg) == "OPTTSP") {
                mode = 'c';
            }
            else {
                mode = 'x'; // Idk how to handle bad input
            }
            break;
        case 'h':
            cout << "Some help message \n";
            return 0;
            break;
        default:
            cerr << "Error: invalid command line argument\n";
            exit(1);
        } // switch
    } // optarg while
    // END GETOPT STUFF--------------------------------------------------------------------
    vector<Vertex> vertices;
    int vertNum;
    int count = 0;
    int x;
    int y;
    cin >> vertNum;
    vertices.reserve(vertNum);

    while (count++ < vertNum) {
        cin >> x >> y;
        vertices.push_back(Vertex(x, y));
    }


    switch (mode) {
    case 'a':
        mstDriver(vertices);
        break;
    case 'b':
        fastDriver(vertices);
        break;
    case 'c':
        optiDriver(vertices);
        break;
    default:
        break;
    }

	return 0;
}

bool promising(vector<Vertex> &vertices, OptiTSP &sol, size_t permLength) {

    if (vertices.size() - permLength <= 4) return true; // Supposed to save work or whatever

    vector<Node> adjmat;
    // Need to figure out which nodes to put in here
    for (size_t i = permLength; i < vertices.size(); ++i) {
        adjmat.push_back(Node(&vertices[sol.currPath[i]]));
    }

    size_t numTrue = 0;
    adjmat[0].maxDist = 0;

    while (numTrue++ < vertices.size() - permLength) {
        size_t shortest = 0;
        for (size_t i = 0; i < vertices.size() - permLength; ++i) {
            if (adjmat[i].isVisited == false) {
                if (adjmat[shortest].maxDist > adjmat[i].maxDist ||
                    (shortest == 0 && adjmat[i].maxDist != numeric_limits<double>::infinity()))
                    shortest = i;
            }
        } // first for

        adjmat[shortest].isVisited = true;

        for (size_t i = 0; i < vertices.size() - permLength; ++i) {
            if (adjmat[i].isVisited == false) {
                double dist = distance2(*adjmat[shortest].vert, *adjmat[i].vert);
                if (adjmat[i].maxDist > dist) {
                    adjmat[i].maxDist = dist;
                    adjmat[i].pred = static_cast<int>(shortest);
                }
            }
        } // second for
    } // while

    double totDist = 0;

    for (size_t i = 0; i < adjmat.size(); ++i) {
        totDist += sqrt(adjmat[i].maxDist);
    }
    
    /* Here's a little debugging thing
    cout << totDist << "\n";

    for (int i = 1; i < static_cast<int>(adjmat.size()); ++i) {
        cout << min(i, adjmat[i].pred) << " " << max(i, adjmat[i].pred) << "\n";
    }*/

    // ad le arms
    double dist1 = numeric_limits<double>::infinity();
    for (size_t i = 0; i < adjmat.size(); ++i) {
        double newDist = distance2(vertices[0], *adjmat[i].vert);
        if (newDist < dist1) dist1 = newDist;
    }
    totDist += sqrt(dist1);

    double dist2 = numeric_limits<double>::infinity();
    for (size_t i = 0; i < adjmat.size(); ++i) {
        double newDist = distance2(vertices[sol.currPath[permLength - 1]], *adjmat[i].vert);
        if (newDist < dist2) dist2 = newDist;
    }
    totDist += sqrt(dist2);

    //cout << setw(5) << permLength << setw(10) << sol.currPath.size() << setw(10) << sqrt(dist1) << setw(10) << sqrt(dist2) << setw(10) << totDist - sqrt(dist1) - sqrt(dist2) << '\n';

    if (totDist + sol.currDist < sol.bestDist) {
        return true;
    }

    return false;
} // PROMISING

void genPerms(vector<Vertex> &vertices, OptiTSP &sol, size_t permLength) {
    if (permLength == sol.currPath.size()) {
        double lastLink = sqrt(distance2(vertices[sol.currPath.front()], vertices[sol.currPath.back()]));
        sol.currDist += lastLink;
        if (sol.currDist < sol.bestDist) {
            sol.bestPath = sol.currPath;
            sol.bestDist = sol.currDist;
        }
        sol.currDist -= lastLink;
        return;
    } // if
    if (!promising(vertices, sol, permLength)) return;
    for (size_t i = permLength; i < sol.currPath.size(); ++i) {
        swap(sol.currPath[permLength], sol.currPath[i]);
        sol.currDist += sqrt(distance2(vertices[sol.currPath[permLength]], vertices[sol.currPath[permLength - 1]]));
        genPerms(vertices, sol, permLength + 1);
        sol.currDist -= sqrt(distance2(vertices[sol.currPath[permLength]], vertices[sol.currPath[permLength - 1]]));
        swap(sol.currPath[permLength], sol.currPath[i]);
    } // for
} // genPerms()

double distance(Vertex *left, Vertex *right) {
    if ((left->type == Sea && right->type == Land) || (right->type == Sea && left->type == Land))
        return numeric_limits<double>::infinity();
    return pow(static_cast<double>(left->x - right->x), 2)
        + pow(static_cast<double>(left->y - right->y), 2);
}

double distance2(Vertex &left, Vertex &right) {
    return pow(static_cast<double>(left.x - right.x), 2)
        + pow(static_cast<double>(left.y - right.y), 2);
}

void mstDriver(vector<Vertex> &vertices) {
    vector<Node> adjmat;
    int coastCount = 0, seaCount = 0, landCount = 0;
    
    for (size_t i = 0; i < vertices.size(); ++i) {
        if (vertices[i].type == Land) ++landCount; // Is there a faster way to do this?
        else if (vertices[i].type == Sea) ++seaCount;
        else ++coastCount;
        adjmat.push_back(Node(&vertices[i]));
    }

    if (coastCount == 0 && landCount > 0 && seaCount > 0) {
        cerr << "Cannot construct MST!";
        exit(1);
    }

    size_t numTrue = 0;
    adjmat[0].maxDist = 0;

    while (numTrue++ < vertices.size()) {
        size_t shortest = 0;
        for (size_t i = 0; i < vertices.size(); ++i) {
            if (adjmat[i].isVisited == false) {
                if (adjmat[shortest].maxDist > adjmat[i].maxDist || 
                    (shortest == 0 && adjmat[i].maxDist != numeric_limits<double>::infinity()))
                    shortest = i;
            }
        } // first for

        adjmat[shortest].isVisited = true;

        for (size_t i = 0; i < vertices.size(); ++i) {
            if (adjmat[i].isVisited == false) {
                double dist = distance(adjmat[shortest].vert, adjmat[i].vert); // Since I'm doing it with a variable, maybe sqrt is okay?
                if (adjmat[i].maxDist > dist) {
                    adjmat[i].maxDist = dist;
                    adjmat[i].pred = static_cast<int>(shortest);
                }
            }
        } // second for
    } // while
    
    // Print MST out
    double totalDist = 0;
    for (size_t i = 0; i < adjmat.size(); ++i) {
        totalDist += sqrt(adjmat[i].maxDist);
    }

    cout << totalDist << "\n";

    for (int i = 1; i < static_cast<int>(adjmat.size()); ++i) {
        cout << min(i, adjmat[i].pred) << " " << max(i, adjmat[i].pred) << "\n";
    }
}

void fastDriver(vector<Vertex> &vertices) {
    vector<int> path;
    path.reserve(vertices.size());
    double totDist = fastTSP(vertices, path);

    cout << totDist << "\n";
    for (size_t i = 0; i < path.size(); ++i) {
        cout << path[i] << " ";
    }
}

void optiDriver(vector<Vertex> &vertices) {
    OptiTSP sol(vertices.size());
     
    // Hit em with the FASTTSP real quick
    sol.bestDist = fastTSP(vertices, sol.bestPath);
    /*for (size_t i = 0; i < vertices.size(); ++i) {
        sol.currPath.push_back(static_cast<int>(i));
    }*/
    sol.currPath = sol.bestPath;

    genPerms(vertices, sol, 1);

    cout << sol.bestDist << "\n";
    for (size_t i = 0; i < sol.bestPath.size(); ++i) {
        cout << sol.bestPath[i] << " ";
    }
}

double fastTSP(vector<Vertex> &vertices, vector<int> &path) {
    path.push_back(0);
    path.push_back(1);

    vector<int> outs;
    outs.reserve(vertices.size());

    for (size_t i = 2; i < vertices.size(); ++i) {
        outs.push_back(static_cast<int>(i));
    }

    while (path.size() < vertices.size()) {
        size_t addSpot;
        double minDist = numeric_limits<double>::infinity();
        int newBoi;
        for (size_t k = 0; k < path.size() - 1; ++k) {
            double newDist = distance2(vertices[path[k]], vertices[outs.back()]) + distance2(vertices[path[k + 1]], vertices[outs.back()])
                - distance2(vertices[path[k]], vertices[path[k + 1]]);
            if (newDist < minDist) {
                minDist = newDist;
                addSpot = k + 1;
                newBoi = outs.back();
            }
        }
        path.insert(path.begin() + addSpot, newBoi);
        outs.pop_back();
    }

    double totDist = 0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        totDist += sqrt(distance2(vertices[path[i]], vertices[path[i + 1]]));
    }

    totDist += sqrt(distance2(vertices[path.front()], vertices[path.back()]));

    return totDist;
}