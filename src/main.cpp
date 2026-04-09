#include <opencv2/opencv.hpp>
#include "dashboard.h"

int main() {
    // Создаем пустой черный кадр размером 1280x480
    cv::Mat frame = cv::Mat::zeros(480, 1280, CV_8UC3);

    // Подготавливаем тестовые данные (чтобы сработали оба предупреждения и агрессивный стиль)
    DashboardData data;
    data.speed = 110.5f;        // Красная зона спидометра
    data.rpm = 5000.0f;         // Красная зона тахометра
    data.coolant_temp = 105.0f; // >100 - выдаст WARNING
    data.fuel_level = 10.0f;    // <15 - выдаст WARNING
    data.throttle_pos = 85.0f;
    data.driving_style = 2;     // AGGRESSIVE

    // Рисуем панель
    Dashboard dashboard;
    dashboard.draw(frame, data);

    // Показываем окно
    cv::imshow("ADAS Dashboard Test", frame);

    // Ждем нажатия любой клавиши, чтобы закрыть окно
    cv::waitKey(0);

    return 0;
}