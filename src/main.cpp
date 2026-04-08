#include <iostream>
#include "obd_parser.h"

int main() {
    OBDParser parser;

    // Пытаемся загрузить сгенерированный датасет
    int loaded = parser.load("data/obd_data.csv");

    if (loaded == -1) {
        std::cerr << "Failed to load CSV file. Check data/obd_data.csv path." << std::endl;
        return 1;
    }

    std::cout << "Successfully loaded " << loaded << " records.\n\n";
    std::cout << "--- First 5 records ---\n";

    // Массив для подсчета статистики классов: 0-SLOW, 1-NORMAL, 2-AGGRESSIVE
    int stats[3] = { 0, 0, 0 };

    // Проходимся по всем загруженным записям
    for (int i = 0; i < loaded; ++i) {
        OBDRecord rec = parser.getRecord(i);
        stats[rec.label]++; // Увеличиваем счетчик для соответствующего класса вождения

        // Выводим в консоль только первые 5 записей
        if (i < 5) {
            std::cout << "Speed: " << rec.speed_kmh
                << " km/h, RPM: " << rec.engine_rpm
                << ", Label: " << rec.label << "\n";
        }
    }

    // Выводим итоговую статистику
    std::cout << "\n--- Statistics ---\n";
    std::cout << "SLOW: " << stats[0] << "\n";
    std::cout << "NORMAL: " << stats[1] << "\n";
    std::cout << "AGGRESSIVE: " << stats[2] << "\n";

    return 0;
}