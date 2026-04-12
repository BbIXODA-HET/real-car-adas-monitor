#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <deque>

/**
 * @struct DriverState
 * @brief Содержит полную информацию о текущем состоянии водителя.
 */
struct DriverState {
    bool face_detected = false;
    bool eyes_open = true;
    bool looking_forward = true;
    float eye_openness = 1.0f; // 0.0 - закрыты, 1.0 - открыты
    float head_turn_deg = 0.0f;
    bool alert_drowsy = false;
    bool alert_distracted = false;
    cv::Rect face_rect;
};

/**
 * @class DMSMonitor
 * @brief Модуль системы мониторинга водителя (Driver Monitoring System).
 *
 * Класс отвечает за распознавание лица и глаз водителя на видеопотоке,
 * а также за вычисление уровня усталости и степени отвлечения от дороги.
 */
class DMSMonitor {
public:
    /**
     * @brief Конструктор, загружающий DNN-модель для лица и каскад для глаз.
     * @param[in] face_prototxt Путь к .prototxt файлу.
     * @param[in] face_caffe Путь к .caffemodel файлу.
     * @param[in] eye_cascade Путь к .xml файлу каскада глаз.
     * @throw std::runtime_error Если не удалось загрузить одну из моделей.
     */
    DMSMonitor(const std::string& face_prototxt, const std::string& face_caffe, const std::string& eye_cascade);

    /**
     * @brief Анализирует один кадр с камеры и возвращает состояние водителя.
     * @param[in] frame Кадр с веб-камеры.
     * @return Структура DriverState с результатами анализа.
     */
    DriverState analyze(const cv::Mat& frame);

private:
    cv::Rect detectFace(const cv::Mat& frame);
    void estimateEyeOpenness(const cv::Mat& frame, const cv::Rect& face_rect, DriverState& state);
    void estimateHeadTurn(const cv::Mat& frame, const cv::Rect& face_rect, DriverState& state);

    cv::dnn::Net face_net;
    cv::CascadeClassifier eye_cascade;
    std::deque<bool> eyes_history; // Хранит последние 15 кадров
};