#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <ctime>
#include <string>
#include <chrono>

using namespace std;
namespace fs = std::filesystem;

string getTimestamp() {
    time_t now = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", localtime(&now));
    return string(buf);
}

int main() {
    fs::path sceneDir = "assets/scenes";
    fs::path outputDir = "outputs";
    fs::path textureDir = "assets/textures";

    // Check if scene folder exists
    if (!fs::exists(sceneDir)) {
        cerr << "[ERROR] Scene folder not found: " << sceneDir << endl;
        return 1;
    }

    // Check if texture folder exists (optional)
    if (!fs::exists(textureDir)) {
        cout << "[INFO] Texture folder not found. Continuing without it." << endl;
    }

    // Create output folder if needed
    if (!fs::exists(outputDir)) {
        cout << "[INFO] Output folder not found. Creating it..." << endl;
        fs::create_directory(outputDir);
    }

    auto startTime = chrono::high_resolution_clock::now();

    // Traverse all XML files in the scene directory
    for (const auto& entry : fs::directory_iterator(sceneDir)) {
        if (entry.path().extension() == ".xml") {
            fs::path scenePath = entry.path();
            string sceneName = scenePath.stem().string(); // filename without extension
            string timestamp = getTimestamp();
            string outputFilename = "output_scene_" + sceneName + "_" + timestamp + ".png";
            fs::path outputPath = outputDir / outputFilename;

            // Build the command
            string command = "./build/release/raytracer " + scenePath.string() + " " + outputPath.string() + " multi";

            cout << "[INFO] Rendering scene: " << scenePath.filename() << endl;
            cout << "[INFO] Command: " << command << endl;

            int result = system(command.c_str()); // execute the command

            if (result != 0) {
                cerr << "[ERROR] Failed to render: " << scenePath.filename() << endl;
            } else {
                cout << "[SUCCESS] Rendered to: " << outputPath << endl;
            }
        }
    }

    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = endTime - startTime;

    cout << "\n======================" << endl;
    cout << "[INFO] All scenes rendered." << endl;
    cout << "[TIME] Total elapsed time: " << elapsed.count() << " seconds" << endl;
    cout << "======================\n" << endl;

    return 0;
}
