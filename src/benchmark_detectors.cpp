#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <memory>
#include <numeric>
#include <algorithm>

#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>


const std::string DATASET_PATH = "data/training/R_01_easy/asl_folder/aria/cam0/data";
const std::string OUTPUT_CSV = "results/benchmark_results.csv";
const int MAX_IMAGES_TO_TEST = 100;

std::vector<std::filesystem::path> load_image_paths(const std::filesystem::path& dir) {

    std::vector<std::filesystem::path> paths;

    if (!std::filesystem::exists(dir)) {
        std::cerr << "Directory not found: " << dir << "\n";
        return paths;
    }

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.path().extension() == ".png") {
            paths.push_back(entry.path());
        }
    }

    std::sort(paths.begin(), paths.end());

    if (paths.size() > MAX_IMAGES_TO_TEST) {
        paths.resize(MAX_IMAGES_TO_TEST);
    }
    return paths;
}

struct Result {
    std::string name;
    double avg_time_ms;
    double avg_keypoints;
};

Result run_test(const std::string& name, cv::Ptr<cv::Feature2D> detector, const std::vector<std::filesystem::path>& images) {
    std::cout << "Testing " << name << "..." << std::flush;

    std::vector<double> times;
    std::vector<size_t> kp_counts;

    times.reserve(images.size());

    for (const auto& path : images) {
        cv::Mat img = cv::imread(path.string(), cv::IMREAD_GRAYSCALE);
        if (img.empty()) continue;

        std::vector<cv::KeyPoint> keypoints;

        auto start = std::chrono::high_resolution_clock::now();

        detector->detect(img, keypoints);

        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> duration = end - start;

        times.push_back(duration.count());
        kp_counts.push_back(keypoints.size());
    }

    double total_time = std::accumulate(times.begin(), times.end(), 0.0);
    double total_kp = std::accumulate(kp_counts.begin(), kp_counts.end(), 0.0);

    double avg_time = total_time / times.size();
    double avg_kp = total_kp / kp_counts.size();

    std::cout << " Done. (" << avg_time << " ms/frame, " << avg_kp << " feats)\n";
    return {name, avg_time, avg_kp};
}

// -------------------------------------------------------------------------
// Main
// -------------------------------------------------------------------------
int main() {
    // [vector] A list of pairs (Name, DetectorPointer)
    // Note on <memory>: cv::Ptr is similar to std::shared_ptr found in <memory>
    std::vector<std::pair<std::string, cv::Ptr<cv::Feature2D>>> detectors;

    detectors.push_back({"FAST", cv::FastFeatureDetector::create(30, true, cv::FastFeatureDetector::TYPE_9_16)});
    detectors.push_back({"ORB", cv::ORB::create(500)});
    detectors.push_back({"GFTT", cv::GFTTDetector::create(500, 0.01, 10)});
    detectors.push_back({"SIFT", cv::SIFT::create(500)});

    // [iostream] Logging
    std::cout << "Loading images from: " << DATASET_PATH << "\n";
    auto images = load_image_paths(DATASET_PATH);

    if (images.empty()) {
        std::cerr << "No images found! Check path.\n";
        return -1;
    }
    std::cout << "Loaded " << images.size() << " images for benchmarking.\n";

    // [fstream] Opening a file for writing the CSV
    std::ofstream csv_file(OUTPUT_CSV);
    
    // [fstream] Writing the header row
    csv_file << "Detector,AvgTime_ms,FPS_Est,AvgKeypoints\n";

    for (const auto& [name, detector] : detectors) {
        if (!images.empty()) {
            cv::Mat temp = cv::imread(images[0].string(), cv::IMREAD_GRAYSCALE);
            std::vector<cv::KeyPoint> dummy;
            detector->detect(temp, dummy);
        }

        Result res = run_test(name, detector, images);
        
        // [fstream] Writing formatted data to the file
        csv_file << res.name << "," 
                 << res.avg_time_ms << "," 
                 << (1000.0 / res.avg_time_ms) << "," 
                 << res.avg_keypoints << "\n";
    }

    std::cout << "Benchmark saved to " << OUTPUT_CSV << "\n";
    return 0;
}
