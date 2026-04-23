#include <iostream>
#include <string>
#include <vector>
#include <memory>

// #include "config/ConfigDriver.hpp"
#include "core/GameEngine.hpp"

using namespace std;

int main() {
    // const string basePath = (argc >= 2) ? argv[1] : "config";

    // try {
    //     runConfigLoaderDriver(basePath);
    // } catch (const exception& ex) {
    //     cerr << "[CONFIG DRIVER] Failed: " << ex.what() << endl;
    //     cerr << "Usage: ./game [config_base_path]" << endl;
    //     return 1;
    // }

    // return 0;
    GameEngine ge(10);
    ge.run();
}
