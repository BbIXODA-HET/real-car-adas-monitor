#include <opencv2/opencv.hpp>
#include "dms_monitor.h"
#include "dms_hud.h"

int main() {
    cv::VideoCapture cap(0, cv::CAP_DSHOW);
    if (!cap.isOpened()) {
        std::cerr << "Cannot open camera" << std::endl;
        return -1;
    }

    // Загружаем модели
    DMSMonitor monitor("models/deploy.prototxt", "models/res10_300x300_ssd_iter_140000.caffemodel", "models/haarcascade_eye.xml");
    DMSHUD hud;

    cv::Mat display_frame = cv::Mat::zeros(480, 1280, CV_8UC3);
    std::cout << "Press 'Q' to quit." << std::endl;

    while (true) {
        cv::Mat cam_frame;
        cap >> cam_frame;
        if (cam_frame.empty()) break;

        cv::flip(cam_frame, cam_frame, 1); // Зеркальное отражение
        DriverState state = monitor.analyze(cam_frame);

        display_frame.setTo(cv::Scalar(0, 0, 0)); // Очистка кадра
        hud.draw(display_frame, cam_frame, state);

        cv::imshow("DMS Test", display_frame);

        if (cv::waitKey(30) == 'q' || cv::waitKey(30) == 'Q') {
            break;
        }
    }
    return 0;
}