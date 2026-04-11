#include "dms_monitor.h"

DMSMonitor::DMSMonitor(const std::string& face_prototxt, const std::string& face_caffe, const std::string& eye_cascade_path) {
    face_net = cv::dnn::readNetFromCaffe(face_prototxt, face_caffe);
    if (!eye_cascade.load(eye_cascade_path)) {
        throw std::runtime_error("Error loading eye cascade");
    }
}

cv::Rect DMSMonitor::detectFace(const cv::Mat& frame) {
    // Подготовка кадра для нейросети
    cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0), false, false);
    face_net.setInput(blob);
    cv::Mat detections = face_net.forward();

    cv::Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());

    float max_confidence = 0.0;
    cv::Rect best_face(0, 0, 0, 0);

    for (int i = 0; i < detectionMat.rows; i++) {
        float confidence = detectionMat.at<float>(i, 2);
        if (confidence > 0.5 && confidence > max_confidence) {
            max_confidence = confidence;
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

            // Защита от выхода за границы
            x1 = std::max(0, x1); y1 = std::max(0, y1);
            x2 = std::min(frame.cols - 1, x2); y2 = std::min(frame.rows - 1, y2);
            best_face = cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));
        }
    }
    return best_face;
}

void DMSMonitor::estimateEyeOpenness(const cv::Mat& frame, const cv::Rect& face_rect, DriverState& state) {
    // Ищем глаза только в верхней половине лица
    cv::Rect eyes_roi(face_rect.x, face_rect.y, face_rect.width, face_rect.height / 2);
    cv::Mat face_img = frame(eyes_roi);
    cv::Mat gray;
    cv::cvtColor(face_img, gray, cv::COLOR_BGR2GRAY);

    std::vector<cv::Rect> eyes;
    eye_cascade.detectMultiScale(gray, eyes, 1.1, 3, 0, cv::Size(15, 15));

    state.eyes_open = (eyes.size() > 0);
    state.eye_openness = state.eyes_open ? 1.0f : 0.0f;
}

void DMSMonitor::estimateHeadTurn(const cv::Mat& frame, const cv::Rect& face_rect, DriverState& state) {
    int frame_center = frame.cols / 2;
    int face_center = face_rect.x + face_rect.width / 2;

    float offset = face_center - frame_center;
    state.head_turn_deg = offset * 0.2f;

    // Если угол больше 20 градусов в любую сторону - отвлечение
    state.looking_forward = (std::abs(state.head_turn_deg) < 20.0f);
}

DriverState DMSMonitor::analyze(const cv::Mat& frame) {
    DriverState state;
    if (frame.empty()) return state;

    state.face_rect = detectFace(frame);

    if (state.face_rect.area() > 0) {
        state.face_detected = true;
        estimateEyeOpenness(frame, state.face_rect, state);
        estimateHeadTurn(frame, state.face_rect, state);
    }
    else {
        state.eyes_open = false;
        state.looking_forward = false;
    }

    // История усталости (15 кадров)
    eyes_history.push_back(!state.eyes_open);
    if (eyes_history.size() > 15) eyes_history.pop_front();

    int closed_count = 0;
    for (bool closed : eyes_history) if (closed) closed_count++;

    // Алерты
    state.alert_drowsy = (closed_count >= 10);
    state.alert_distracted = !state.looking_forward;

    return state;
}