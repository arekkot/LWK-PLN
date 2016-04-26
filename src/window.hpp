#ifndef PLN_WINDOW_HPP
#define PLN_WINDOW_HPP

#include <opencv2/highgui/highgui.hpp>
#include <functional>
#include <string>
#include <vector>
#include <QFileDialog>


namespace pln {
    class Window;
    typedef std::function<void
                          (Window& window,
                           cv::Mat& image)> draw_gui_callback_t;

    enum class CONTENT_TYPE {
        INVALID,
        IMAGE,
        VIDEO
    };

    class Window
    {
    public:
        Window();
        Window(const std::string& name,
               int flags = CV_WINDOW_AUTOSIZE
                         | CV_WINDOW_KEEPRATIO
                         | CV_GUI_EXPANDED);
        Window(const Window&) = delete;
        Window(Window&&) = delete;
        ~Window();

        void open(const std::string& name,
                  int flags = CV_WINDOW_AUTOSIZE
                            | CV_WINDOW_KEEPRATIO
                            | CV_GUI_EXPANDED);
        void close();

        void fullscreenize();

        int get_delta_time();

        void put_text(cv::Mat& mat,
                      const std::string& text,
                      cv::Point orig,
                      double scale = 1.0,
                      int thickness = 1,
                      cv::Scalar color = cvScalar(255,255,255));
        cv::Size get_text_size(const std::string& text,
                               double scale = 1.0,
                               int thickness = 1);

        void set_draw_gui(draw_gui_callback_t draw_gui);
        void set_default_image(const std::string& filename);
        void set_image(const std::string& filename);
        void set_video(const std::string& filename);
        void set_camera();
        void update(cv::Mat& frame);
        void refresh_gui();

        std::string recognized_class;
        int prev_banknote;
        int sum_money;

    private:
        CONTENT_TYPE        m_content_type;
        std::string         m_default_image;
        draw_gui_callback_t m_draw_gui;
        std::string         m_name;
        cv::VideoCapture    m_video_stream;
        cv::Mat             m_image;
        cv::string          m_image_filename;
    };
}

#endif //PLN_WINDOW_HPP
