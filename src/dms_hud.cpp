#include "dms_hud.h"

void DMSHUD::draw(cv::Mat& display_frame, const cv::Mat& camera_frame, const DriverState& state) {
    // Вставляем камеру в правую половину кадра (от x=640)
    cv::Mat right_half = display_frame(cv::Rect(640, 0, 640, 480));
    cv::Mat resized_cam;
    cv::resize(camera_frame, resized_cam, cv::Size(640, 480));
    resized_cam.copyTo(right_half);

    int offset_x = 640;

    if (state.face_detected) {
        cv::Scalar box_color = state.alert_drowsy ? cv::Scalar(0, 165, 255) : cv::Scalar(0, 255, 0);
        // Адаптируем координаты лица к размеру 640x480 (коэффициент масштабирования)
        float scale_x = 640.0f / camera_frame.cols;
        float scale_y = 480.0f / camera_frame.rows;

        int fx = offset_x + static_cast<int>(state.face_rect.x * scale_x);
        int fy = static_cast<int>(state.face_rect.y * scale_y);
        int fw = static_cast<int>(state.face_rect.width * scale_x);
        int fh = static_cast<int>(state.face_rect.height * scale_y);
        int len = 20;

        // Рисуем угловые рамки
        cv::line(display_frame, cv::Point(fx, fy), cv::Point(fx + len, fy), box_color, 3);
        cv::line(display_frame, cv::Point(fx, fy), cv::Point(fx, fy + len), box_color, 3);
        cv::line(display_frame, cv::Point(fx + fw, fy), cv::Point(fx + fw - len, fy), box_color, 3);
        cv::line(display_frame, cv::Point(fx + fw, fy), cv::Point(fx + fw, fy + len), box_color, 3);
        cv::line(display_frame, cv::Point(fx, fy + fh), cv::Point(fx + len, fy + fh), box_color, 3);
        cv::line(display_frame, cv::Point(fx, fy + fh), cv::Point(fx, fy + fh - len), box_color, 3);
        cv::line(display_frame, cv::Point(fx + fw, fy + fh), cv::Point(fx + fw - len, fy + fh), box_color, 3);
        cv::line(display_frame, cv::Point(fx + fw, fy + fh), cv::Point(fx + fw, fy + fh - len), box_color, 3);
    }

    // Индикаторы состояния
    cv::circle(display_frame, cv::Point(offset_x + 30, 30), 8, state.eyes_open ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), cv::FILLED);
    cv::putText(display_frame, "EYES", cv::Point(offset_x + 50, 35), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);

    cv::circle(display_frame, cv::Point(offset_x + 130, 30), 8, state.looking_forward ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), cv::FILLED);
    cv::putText(display_frame, "FOCUS", cv::Point(offset_x + 150, 35), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);

    if (state.alert_drowsy) {
        cv::rectangle(display_frame, cv::Point(offset_x + 120, 200), cv::Point(offset_x + 520, 280), cv::Scalar(0, 165, 255), cv::FILLED);
        cv::putText(display_frame, "DROWSINESS ALERT!", cv::Point(offset_x + 140, 250), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 3);
    }

    if (state.alert_distracted) {
        cv::rectangle(display_frame, cv::Point(offset_x, 430), cv::Point(offset_x + 640, 480), cv::Scalar(0, 0, 255), cv::FILLED);
        cv::putText(display_frame, "DISTRACTION: KEEP EYES ON ROAD", cv::Point(offset_x + 100, 465), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 2);
    }
}