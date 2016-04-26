#include "matcher.hpp"
#include "window.hpp"

#include <iostream>
#include <opencv2/core/core.hpp>
#include <pthread.h>
#include <signal.h>
#include <QApplication>
#include <QDesktopWidget>


void draw_gui(pln::Window& window, cv::Mat& image) {
    QRect screen_size = QApplication::desktop()->availableGeometry();

    double scale_x = double(image.cols) / double(screen_size.width());
    double scale_y = double(image.rows) / double(screen_size.height());
    double scale = 0.7 * std::max<>(scale_x, scale_y);

    rectangle(image, cv::Rect(0, 0, 380 * scale, 210 * scale),
              cv::Scalar(50, 50, 50), CV_FILLED);

    cv::Size sz = window.get_text_size("ESC", scale, 1);
    int x = 20 * scale_x;
    int y = 30 * scale_y;
    int w = sz.width;
    int h = sz.height + 5 * scale_y;
    window.put_text(image, "ESC",            cv::Point(x,   y), scale, 1);
    window.put_text(image, ": Exit",         cv::Point(x+w, y), scale, 1);
    window.put_text(image, "Q",              cv::Point(x,   y+h), scale, 1);
    window.put_text(image, ": Select image", cv::Point(x+w, y+h), scale, 1);
    window.put_text(image, "W",              cv::Point(x,   y+2*h), scale, 1);
    window.put_text(image, ": Select video", cv::Point(x+w, y+2*h), scale, 1);
    window.put_text(image, "E",              cv::Point(x,   y+3*h), scale, 1);
    window.put_text(image, ": Open camera",  cv::Point(x+w, y+3*h), scale, 1);
    window.put_text(image, "Pln",            cv::Point(x,   y+4*h), scale, 1);
    window.put_text(image, ": " + window.recognized_class,
                    cv::Point(x+w, y+4*h), scale, 1);
    window.put_text(image, "Sum",            cv::Point(x,   y+5*h), scale, 1);
    window.put_text(image, ": " + std::to_string(window.sum_money),
                    cv::Point(x+w, y+5*h), scale, 1);
}

void generate_training_feats(pln::Matcher& matcher) {
    std::cout << "Generating training features." << std::endl;

    std::vector<std::string> classes = {"10", "20", "50", "100", "200"};
    for(std::string& data_class : classes) {
        std::vector<std::string> images = {
            "data/train/" + data_class + "/front1.jpg",
            "data/train/" + data_class + "/front2.jpg",
            "data/train/" + data_class + "/front3.jpg",
            "data/train/" + data_class + "/back1.jpg",
            "data/train/" + data_class + "/back2.jpg",
            "data/train/" + data_class + "/back3.jpg",
        };
        matcher.add_data_class(data_class, images);
    }
}

struct match_worker_data
{
    cv::Mat&      frame;
    pln::Window&  window;
    pln::Matcher& matcher;
    bool&         worker_ended;
};

void* match_async(void* arg)
{
    match_worker_data* data = (match_worker_data*)arg;
    pln::Window&  window  = data->window;
    pln::Matcher& matcher = data->matcher;
    cv::Mat       frame   = data->frame;
    bool&         worker_ended = data->worker_ended;

    window.recognized_class = matcher.match(frame);

    int new_banknote = stoi(window.recognized_class);
    if(window.prev_banknote != new_banknote) {
        window.sum_money += new_banknote;
	window.prev_banknote = new_banknote;
    }

    window.refresh_gui();
    worker_ended = true;
    pthread_exit(0);
}

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    pln::Matcher matcher;
    generate_training_feats(matcher);

    std::string window_name = "main-window";
    pln::Window window(window_name, 0);
    window.fullscreenize();
    window.set_draw_gui(draw_gui);
    window.set_default_image("data/welcome.png");

    while(true) {
        cv::Mat frame;
        window.update(frame);

        static bool worker_ended = true;

        pthread_t thread_id;
        if(worker_ended) {
            worker_ended = false;
            match_worker_data data = {frame, window, matcher, worker_ended};
            pthread_create(&thread_id, nullptr, match_async, &data);
        }

        int dt = window.get_delta_time();

        //Bitwise operator makes key-code system independent
        int key = cv::waitKey(dt) & 0xEFFFFF;
        //std::cout << "Key code: '" << key << "'";

        std::string filename;

        switch(key)
        {
        case 27: //ESC
            std::cout << "Exiting!" << std::endl;
            return 0;

        case 113: //Q
            std::cout << "Selecting new image." << std::endl;
            filename =
                QFileDialog::getOpenFileName(nullptr,
                                             "Select new image",
                                             "",
                                             "Image Files (*.png *.jpg *.bmp)")
                .toStdString();

            pthread_cancel(thread_id);
            worker_ended = true;
            window.recognized_class = "thinking...";

            std::cout << "New image: '" << filename << "'." << std::endl;
            if(!filename.empty())
                window.set_image(filename);

            break;

        case 119: //W
            std::cout << "Selecting new video." << std::endl;
            filename =
                QFileDialog::getOpenFileName(nullptr,
                                             "Select new video",
                                             "",
                                             "Video Files (*.avi *.webm)")
                .toStdString();

            pthread_cancel(thread_id);
            worker_ended = true;
            window.recognized_class = "thinking...";

            std::cout << "New video: '" << filename << "'." << std::endl;
            if(!filename.empty())
                window.set_video(filename);

            break;

        case 101: //E
            pthread_cancel(thread_id);
            worker_ended = true;
            window.recognized_class = "thinking...";

            std::cout << "Streaming from camera." << std::endl;
            window.set_camera();

            break;
        }
    }

    return 0;
}
