#include "obd_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Конвертируем текстовую метку в число для нейросети
int OBDParser::parseLabel(const std::string& labelStr) {
    if (labelStr == "SLOW") return 0;
    if (labelStr == "NORMAL") return 1;
    if (labelStr == "AGGRESSIVE") return 2;
    return -1; // Возвращаем -1, если текст не распознан
}

int OBDParser::load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return -1; // Если файла нет, возвращаем -1 по условию задания

    records.clear(); // Очищаем старые данные перед новой загрузкой
    std::string line;

    std::getline(file, line); // Читаем и пропускаем первую строку (заголовок таблицы)

    // Читаем файл строка за строкой
    while (std::getline(file, line)) {
        if (line.empty()) continue; // Пропускаем пустые строки

        std::stringstream ss(line); // Превращаем строку в поток для удобной разбивки
        std::string token;
        OBDRecord record;

        try {
            // Читаем значения до запятой и сразу конвертируем строку в float 
            std::getline(ss, token, ','); record.speed_kmh = std::stof(token);
            std::getline(ss, token, ','); record.engine_rpm = std::stof(token);
            std::getline(ss, token, ','); record.throttle_pos = std::stof(token);
            std::getline(ss, token, ','); record.coolant_temp = std::stof(token);
            std::getline(ss, token, ','); record.fuel_level = std::stof(token);
            std::getline(ss, token, ','); record.intake_air_temp = std::stof(token);

            // Читаем последнее значение (метку текста)
            std::getline(ss, token, ',');

            // для Windows: удаляем невидимый символ возврата каретки '\r' в конце строки
            if (!token.empty() && token.back() == '\r') token.pop_back();

            record.label = parseLabel(token);
            // Если метка неизвестная, вызываем ошибку
            if (record.label == -1) throw std::invalid_argument("Invalid label");

            records.push_back(record); // Если всё ок, добавляем запись в массив
        }
        catch (const std::exception& e) {
            // Если stof упала (вместо числа был текст) - выводим предупреждение и пропускаем строку
            std::cerr << "Warning: Skipping invalid row. Reason: " << e.what() << "\n";
            continue;
        }
    }
    return records.size(); // Возвращаем итоговое количество загруженных записей
}

OBDRecord OBDParser::getRecord(int index) const {
    // Проверка на выход за границы массива
    if (index < 0 || index >= records.size()) {
        throw std::out_of_range("Index out of range"); // Бросаем исключение по условию задания
    }
    return records[index];
}