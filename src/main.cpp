#include <iostream>
#include <string>

#include "config/ConfigDriver.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    const string basePath = (argc >= 2) ? argv[1] : "config";

    try {
        runConfigLoaderDriver(basePath);
    } catch (const exception& ex) {
        cerr << "[CONFIG DRIVER] Failed: " << ex.what() << endl;
        cerr << "Usage: ./game [config_base_path]" << endl;
        return 1;
    }

    return 0;
}
