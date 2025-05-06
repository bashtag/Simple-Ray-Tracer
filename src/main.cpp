#include "Scene.hpp"
#include "RayTracer.hpp"
#include <iostream>
#include <chrono>
#include <string>

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " scene.xml output.png (single/multithread)" << endl;
        return 1;
    }

    string sceneFilename(argv[1]);
    string outputFilename(argv[2]);
    string mode(argv[3]);

    Scene scene;
    scene.parseScene(sceneFilename);

    RayTracer rayTracer(scene);

    auto startTime = high_resolution_clock::now();

    if (mode == "single" || mode == "singlethread") {
        rayTracer.render();
    }
    else if (mode == "multi" || mode == "multithread") {
        rayTracer.renderMultithreaded();
    }
    else {
        cerr << "Invalid mode. Please use 'single' or 'multithread'." << endl;
        return 1;
    }

    auto endTime = high_resolution_clock::now();
    duration<double> elapsed = endTime - startTime;

    rayTracer.saveImage(outputFilename);

    cout << "Rendering complete. See " << outputFilename << endl;
    cout << "Elapsed time: " << elapsed.count() << " seconds." << endl;

    return 0;
}
