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
    bool isVisited = false;

    Node(Vertex *vert) : vert(vert) {}
};

void mstDriver(vector<Vertex> &vertices);
void fastDriver(vector<Vertex> &vertices);
double distance(Node &left, Node &right);

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
        break;
    default:
        break;
    }

	return 0;
}

double distance(Node &left, Node &right) {
    if ((left.vert->type == Sea && right.vert->type == Land) || (right.vert->type == Sea && left.vert->type == Land))
        return numeric_limits<double>::infinity();
    return pow(static_cast<double>(left.vert->x - right.vert->x), 2)
        + pow(static_cast<double>(left.vert->y - right.vert->y), 2);
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
                double dist = distance(adjmat[shortest], adjmat[i]); // Since I'm doing it with a variable, maybe sqrt is okay?
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

}