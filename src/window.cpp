#include "window.hpp"

#include <iostream>
#include <opencv2/highgui/highgui.hpp>


namespace pln {
    Window::Window()
        : m_content_type(CONTENT_TYPE::INVALID),
	  prev_banknote(0),
	  sum_money(0) {}

    Window::Window(const std::string& name, int flags)
        : m_content_type(CONTENT_TYPE::INVALID),
	  prev_banknote(0),
	  sum_money(0) {
        open(name, flags);
    }

    Window::~Window() {
        close();
    }

    void Window::open(const std::string& name, int flags) {
        m_name = name;
        cv::namedWindow(m_name, flags);
    }

    void Window::close() {
        if(!m_name.empty()) {
            cv::destroyWindow(m_name);
            m_name = "";
        }
    }

    void Window::fullscreenize() {
        cv::setWindowProperty(m_name,
                              CV_WND_PROP_FULLSCREEN,
                              CV_WINDOW_FULLSCREEN);
    }

    int Window::get_delta_time() {
        if(m_content_type == CONTENT_TYPE::VIDEO) {
            double fps = m_video_stream.get(CV_CAP_PROP_FPS);
            return (int)(1000.0 / fps);
        } else {
            return 1000;
        }
    }

    void Window::put_text(cv::Mat& mat,
                          const std::string& text,
                          cv::Point orig,
                          double scale,
                          int thickness,
                          cv::Scalar color) {
        cv::putText(mat, text, orig, CV_FONT_HERSHEY_COMPLEX,
                    scale, color, thickness, CV_AA);
    }

    cv::Size Window::get_text_size(const std::string& text,
                                   double scale,
                                   int thickness) {
        int tmp;
        return cv::getTextSize(text, CV_FONT_HERSHEY_COMPLEX,
                               scale, thickness, &tmp);
    }

    void Window::set_draw_gui(std::function<void (Window& window,
                                                  cv::Mat& image)> draw_gui) {
        m_draw_gui = std::move(draw_gui);
    }

    void Window::set_default_image(const std::string& filename) {
        m_image = cv::imread(filename);
        if(m_image.empty()) {
            std::cerr << "Error loading default image: '"
                      << filename << "'. Exiting!" << std::endl;
            exit(1);
        }

        if(m_draw_gui)
            m_draw_gui(*this, m_image);

        cv::imshow(m_name, m_image);
        m_content_type = CONTENT_TYPE::IMAGE;
        m_default_image = m_image_filename = filename;
    }

    void Window::set_image(const std::string& filename) {
        m_image = cv::imread(filename);
        if(m_image.empty()) {
            std::cerr << "Error loading image: '" << filename
                      << "'." << std::endl;
            set_default_image(m_default_image);
            return;
        }

        if(m_draw_gui)
            m_draw_gui(*this, m_image);

        cv::imshow(m_name, m_image);
        m_content_type = CONTENT_TYPE::IMAGE;
        m_image_filename = filename;
    }

    void Window::set_video(const std::string& filename) {
        m_video_stream = cv::VideoCapture(filename);
        if(!m_video_stream.isOpened()) {
            std::cerr << "Error loading video: '" << filename
                      << "'." << std::endl;
            set_default_image(m_default_image);
            return;
        }

        m_content_type = CONTENT_TYPE::VIDEO;
    }

    void Window::set_camera() {
        m_video_stream = cv::VideoCapture(0);
        if(!m_video_stream.isOpened()) {
            std::cerr << "Error opening camera." << std::endl;
            set_default_image(m_default_image);
            return;
        }

        m_content_type = CONTENT_TYPE::VIDEO;
    }

    void Window::update(cv::Mat& frame) {
        if(m_content_type == CONTENT_TYPE::IMAGE) {
            frame = m_image;
        } else if(m_content_type == CONTENT_TYPE::VIDEO) {
            m_video_stream >> frame;

            if(frame.empty()) {
                std::cout << "End of the video." << std::endl;
                set_image(m_default_image);
                return;
            }

            if(m_draw_gui)
                m_draw_gui(*this, frame);

            cv::imshow(m_name, frame);
        }
    }

    void Window::refresh_gui()
    {
        if(m_content_type == CONTENT_TYPE::IMAGE) {
            set_image(m_image_filename);
        }
    }


}
