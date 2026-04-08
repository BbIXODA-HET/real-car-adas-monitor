#include <iostream>
#include <iomanip>
#include "obd_parser.h"
#include "onnx_classifier.h"

int main() {
    OBDParser parser;
    if (parser.load("data/obd_data.csv") == -1) {
        std::cerr << "Failed to load CSV file." << std::endl;
        return 1;
    }

    try {
        // Загружаем нейросеть
        ONNXClassifier classifier("models/driver_classifier.onnx", "models/normalization_params.json");

        int correct_predictions = 0;
        int total_test = 20;

        std::cout << "--- Testing First 20 Records ---\n";
        std::cout << std::left << std::setw(10) << "True"
            << std::setw(12) << "Predicted"
            << "Confidence\n";
        std::cout << "--------------------------------------\n";

        for (int i = 0; i < total_test; ++i) {
            OBDRecord rec = parser.getRecord(i);

            // Превращаем структуру в массив для классификатора
            std::array<float, 6> features = {
                rec.speed_kmh, rec.engine_rpm, rec.throttle_pos,
                rec.coolant_temp, rec.fuel_level, rec.intake_air_temp
            };

            // Классифицируем
            ClassificationResult result = classifier.classify(features);

            // Считаем правильные
            if (result.label == rec.label) {
                correct_predictions++;
            }

            // Выводим таблицу
            std::cout << std::left << std::setw(10) << rec.label
                << std::setw(12) << result.label
                << std::fixed << std::setprecision(2) << (result.confidence * 100.0f) << "%\n";
        }

        // Подсчитываем точность
        float accuracy = (float)correct_predictions / total_test * 100.0f;
        std::cout << "--------------------------------------\n";
        std::cout << "Accuracy on first 20 records: " << accuracy << "%\n";

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}