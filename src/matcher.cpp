#include "matcher.hpp"

#include <cstdlib>
#include <ctime>


namespace pln {
    Matcher::Matcher()
        : m_bf_matcher(cv::NORM_L2) {
        srand(time(0));
    }

    Matcher::~Matcher() {}

    void Matcher::add_data_class(const std::string& class_name,
                                 std::vector<std::string> filenames) {
        DataClass data_class;
        data_class.name = class_name;

        for(std::string& filename : filenames) {
            std::cout << "Class '" << class_name
                      << "': processing '" << filename << "'...";

            cv::Mat image = cv::imread(filename);
            cv::Mat descriptors;

            if(!image.empty()) {
                std::cout << "Ok" << std::endl;

                compute_descriptors(image, descriptors);
                data_class.descriptors_set.push_back(std::move(descriptors));
            } else {
                std::cout << "Failure" << std::endl;
            }
        }

        unsigned num_processed = data_class.descriptors_set.size();
        unsigned num_total     = filenames.size();
        std::cout << "Processed " << num_processed << "/"
                  << num_total << " images for class '"
                  << class_name << "'." << std::endl;

        if(!num_processed) {
            std::cout << "Exiting!" << std::endl;
            exit(1);
        }

        m_data_classes.push_back(std::move(data_class));
    }

    std::string Matcher::match(const cv::Mat& query_image) {
        static std::vector<std::vector<cv::DMatch>> matches;
        static unsigned num_best = 2;

        double best_class_confidence = 0.0;
        std::string best_class_name = "0";

        cv::Mat query_descriptors;
        compute_descriptors(query_image, query_descriptors);

        for(const DataClass& data_class : m_data_classes) {
            double best_confidence = 0.0;
            double total_weight = 0.0;

            for(const cv::Mat& descriptors : data_class.descriptors_set) {
                total_weight += descriptors.rows;
            }

            for(const cv::Mat& descriptors : data_class.descriptors_set) {
                matches.clear();
                unsigned num_matches = 0;

                m_bf_matcher.knnMatch(query_descriptors, descriptors,
                                      matches, num_best);

                for(auto& match : matches) {
                    if(match.size() == 2
                       && match[0].distance < 0.65 * match[1].distance)
                        num_matches++;
                }

                double confidence = double(num_matches)
                    * double(descriptors.rows)
                    / double(query_descriptors.rows)
                    / total_weight;
                std::cout << confidence << " | ";
                best_confidence += confidence;
		std::cout.flush();
            }

            if(best_confidence > best_class_confidence) {
                best_class_name = "0";
                if(best_confidence > 0.012)
                    best_class_name = data_class.name;
                best_class_confidence = best_confidence;
            }

            std::cout << "Class '" << data_class.name
                      << "', confidence: "
                      << best_confidence << std::endl;
        }

        return best_class_name;
    }

    void Matcher::compute_descriptors(const cv::Mat& image,
                                      cv::Mat& descriptors) {
        KeyPoints key_points;
        m_detector.detect(image, key_points);
        m_descriptor.compute(image, key_points, descriptors);
    }
}
