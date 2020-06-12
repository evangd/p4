#include <iostream>
#include <vector>
#include <iomanip>
#include <getopt.h>
using namespace std;

int main(int argc, char *argv[]) {

    // GETOPT STUFF HERE-------------------------------------------------------------------
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

    char mode;
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

    switch (mode) {
    case 'a':
        break;
    case 'b':
        break;
    case 'c':
        break;
    default:
        break;
    }


	return 0;
}