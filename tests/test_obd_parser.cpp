#include <gtest/gtest.h>
#include "../src/obd_parser.h"
#include <fstream>

// 1. Тест: проверяем правильность конвертации текста в числа
TEST(OBDParserTest, LabelConversion) {
    EXPECT_EQ(OBDParser::parseLabel("SLOW"), 0);
    EXPECT_EQ(OBDParser::parseLabel("NORMAL"), 1);
    EXPECT_EQ(OBDParser::parseLabel("AGGRESSIVE"), 2);
}

// 2. Тест: если попытаться загрузить файл, которого нет, метод должен вернуть -1
TEST(OBDParserTest, FileNotFoundReturnsMinusOne) {
    OBDParser parser;
    EXPECT_EQ(parser.load("non_existent_file.csv"), -1);
}

// 3. Тест: если запросить запись по индексу 0, но массив пустой - должно выброситься исключение
TEST(OBDParserTest, GetRecordThrowsOutOfRange) {
    OBDParser parser;
    EXPECT_THROW(parser.getRecord(0), std::out_of_range);
}

// 4. Тест: проверяем чтение нормального файла
TEST(OBDParserTest, ParseCorrectCSV) {
    // Создаем временный файл прямо во время теста
    std::ofstream temp("temp_correct.csv");
    temp << "speed_kmh,engine_rpm,throttle_pos,coolant_temp,fuel_level,intake_air_temp,label\n";
    temp << "50.5,2000,30,90,50,25,NORMAL\n";
    temp.close();

    OBDParser parser;
    // Ожидаем, что загрузится ровно 1 строка
    EXPECT_EQ(parser.load("temp_correct.csv"), 1);
    // Проверяем, что метка NORMAL правильно превратилась в 1
    EXPECT_EQ(parser.getRecord(0).label, 1);

    std::remove("temp_correct.csv"); // Убираем за собой временный файл
}

// 5. Тест: проверяем поведение, если в CSV попала строка с битыми данными (текст вместо числа)
TEST(OBDParserTest, ParseIncorrectRowIsSkipped) {
    std::ofstream temp("temp_incorrect.csv");
    temp << "speed,rpm,throttle,coolant,fuel,intake,label\n";
    temp << "invalid_speed,2000,30,90,50,25,NORMAL\n"; // Эта строка битая ("invalid_speed")
    temp << "100.0,3000,50,95,40,20,AGGRESSIVE\n";      // А эта нормальная
    temp.close();

    OBDParser parser;
    // Ожидаем, что загрузится только 1 нормальная строка (битая пропустится)
    EXPECT_EQ(parser.load("temp_incorrect.csv"), 1);
    // Загруженная строка должна быть AGGRESSIVE (индекс 2)
    EXPECT_EQ(parser.getRecord(0).label, 2);

    std::remove("temp_incorrect.csv");
}