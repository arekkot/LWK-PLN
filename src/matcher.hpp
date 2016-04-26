#ifndef PLN_MATCHER_HPP
#define PLN_MATCHER_HPP

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <string>
#include <vector>


namespace pln {
    typedef std::vector<cv::KeyPoint> KeyPoints;

    struct DataClass {
        std::string name;
        std::vector<cv::Mat> descriptors_set;
    };

    class Matcher
    {
    public:
        Matcher();
        Matcher(const Matcher&) = delete;
        Matcher(Matcher&&) = delete;
        ~Matcher();

        void add_data_class(const std::string& class_name,
                            std::vector<std::string> filenames);

        std::string match(const cv::Mat& query_descriptors);

        void compute_descriptors(const cv::Mat& image,
                                 cv::Mat& descriptors);

    private:
        std::vector<DataClass>  m_data_classes;
        cv::SiftFeatureDetector m_detector;
        cv::SIFT               m_descriptor;
        cv::BFMatcher           m_bf_matcher;
    };
}

#endif //PLN_MATCHER_HPP
