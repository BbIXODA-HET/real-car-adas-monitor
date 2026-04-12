#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <sstream> 

#include "obd_parser.h"
#include "onnx_classifier.h"
#include "dashboard.h"
#include "dms_monitor.h"
#include "dms_hud.h"
#include "shared_state.h"

// поток 1: Обработка данных OBD
void obdThreadFunc(SharedState& state) {
    OBDParser parser;
    if (parser.load("data/obd_data.csv") == -1) {
        std::cerr << "OBD Thread: Failed to load CSV." << std::endl;
        return;
    }

    ONNXClassifier classifier("models/driver_classifier.onnx", "models/normalization_params.json");
    int record_idx = 0;
    int total_records = 5000;

    while (state.running) {
        OBDRecord rec = parser.getRecord(record_idx);

        std::array<float, 6> features = {
            rec.speed_kmh, rec.engine_rpm, rec.throttle_pos,
            rec.coolant_temp, rec.fuel_level, rec.intake_air_temp
        };
        ClassificationResult res = classifier.classify(features);

        // Блокируем мьютекс и обновляем общие данные
        {
            std::lock_guard<std::mutex> lock(state.mtx);
            state.current_obd = rec;
            state.drive_style = res;
            if (res.label == 2) state.alert_aggressive_count++; // 2 - Aggressive
        }

        record_idx = (record_idx + 1) % total_records;
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Задержка 100 мс
    }
}

// поток 2: Камера, Отрисовка и Логи
int main() {
    SharedState state;

    // Запускаем OBD поток
    std::thread obd_thread(obdThreadFunc, std::ref(state));
    
    cv::VideoCapture cap(0, cv::CAP_DSHOW);

    if (!cap.isOpened()) {
        std::cerr << "Main Thread: Cannot open camera!" << std::endl;
        state.running = false;
        obd_thread.join();
        return -1;
    }

    DMSMonitor dms("models/deploy.prototxt", "models/res10_300x300_ssd_iter_140000.caffemodel", "models/haarcascade_eye.xml");
    DMSHUD dms_hud;
    Dashboard dashboard;

    cv::Mat display_frame = cv::Mat::zeros(480, 1280, CV_8UC3);

    // Настройка записи видео и логов
    cv::VideoWriter video_writer("output/result_situation2.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), 10.0, cv::Size(1280, 480));
    std::ofstream alert_log("output/dms_alerts.log", std::ios::app);

    auto start_time = std::chrono::steady_clock::now();
    int processed_obd_count = 0;
    bool is_paused = false;
    int screenshot_counter = 0;

    std::cout << "System started. Controls: [Q]-Quit, [SPACE]-Pause, [S]-Screenshot\n";

    while (state.running) {
        // Обработка клавиатуры
        char key = (char)cv::waitKey(30);
        if (key == 'q' || key == 'Q') break;
        if (key == ' ') is_paused = !is_paused;
        if (key == 's' || key == 'S') {
            screenshot_counter++; 
            std::stringstream ss;
            ss << "output/screenshot_" << screenshot_counter << ".png";
            std::string filename = ss.str();

            cv::imwrite(filename, display_frame);
            std::cout << "Screenshot saved to " << filename << "\n";
        }

        if (is_paused) continue;

        cv::Mat cam_frame;
        cap >> cam_frame;
        if (cam_frame.empty()) break;
        cv::flip(cam_frame, cam_frame, 1);
        
        cv::resize(cam_frame, cam_frame, cv::Size(640, 480));
        // Анализируем водителя
        DriverState dms_state = dms.analyze(cam_frame);

        // Читаем данные из OBD-потока (копируем под мьютексом)
        DashboardData dash_data;
        {
            std::lock_guard<std::mutex> lock(state.mtx);
            dash_data.speed = state.current_obd.speed_kmh;
            dash_data.rpm = state.current_obd.engine_rpm;
            dash_data.coolant_temp = state.current_obd.coolant_temp;
            dash_data.fuel_level = state.current_obd.fuel_level;
            dash_data.throttle_pos = state.current_obd.throttle_pos;
            dash_data.driving_style = state.drive_style.label;

            if (dms_state.alert_drowsy) state.alert_drowsy_count++;
            if (dms_state.alert_distracted) state.alert_distracted_count++;
            processed_obd_count++;
        }

        // Логирование алертов в файл
        if (dms_state.alert_drowsy) alert_log << "ALERT: Drowsiness detected!\n";
        if (dms_state.alert_distracted) alert_log << "ALERT: Distraction detected!\n";

        // трисовка
        display_frame.setTo(cv::Scalar(0, 0, 0));
        dashboard.draw(display_frame, dash_data);
        dms_hud.draw(display_frame, cam_frame, dms_state);

        // Пишем кадр в видео и выводим на экран
        video_writer.write(display_frame);
        cv::imshow("Real-Car ADAS Monitor", display_frame);
    }

    // Правильное завершение потоков
    state.running = false;
    if (obd_thread.joinable()) obd_thread.join();
    alert_log.close();
    video_writer.release();

    // Вывод итоговой статистики в консоль
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    std::cout << "\n=== FINAL STATISTICS ===\n";
    std::cout << "Runtime: " << duration.count() << " seconds\n";
    std::cout << "Processed OBD records: " << processed_obd_count << "\n";
    int total_alerts = state.alert_drowsy_count + state.alert_distracted_count + state.alert_aggressive_count;
    std::cout << "Total Alerts: " << total_alerts << "\n";
    std::cout << " - Drowsy: " << state.alert_drowsy_count << "\n";
    std::cout << " - Distracted: " << state.alert_distracted_count << "\n";
    std::cout << " - Aggressive Driving: " << state.alert_aggressive_count << "\n";

    return 0;
}