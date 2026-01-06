// Filename: DataGenerator.h
#pragma once
#include "HighSpeedDB.h" // Bắt buộc phải include để gọi được db.addStudent
#include <vector>
#include <string>
#include <random> // Thư viện random chuẩn C++11

using namespace std;

// Hàm phụ trợ: Sinh tên ngẫu nhiên
string generateRandomName() {
    static const vector<string> ho = {"Nguyen", "Tran", "Le", "Pham", "Hoang", "Huynh", "Phan", "Vu", "Vo", "Dang"};
    static const vector<string> dem = {"Van", "Thi", "Minh", "Huu", "Duc", "Thuy", "Ngoc", "Quang", "Gia", "Xuan"};
    static const vector<string> ten = {"Tuan", "Nam", "Hung", "Dung", "Hoa", "Lan", "Mai", "Cuc", "Truc", "Quynh", "An", "Binh"};
    
    // Dùng static random engine để tối ưu hiệu năng
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> disHo(0, ho.size() - 1);
    uniform_int_distribution<> disDem(0, dem.size() - 1);
    uniform_int_distribution<> disTen(0, ten.size() - 1);

    return ho[disHo(gen)] + " " + dem[disDem(gen)] + " " + ten[disTen(gen)];
}

// Hàm phụ trợ: Sinh username từ ID (cho nhất quán)
string generateUsername(int id) {
    return "user_" + to_string(id);
}

// HÀM CHÍNH: Sinh dữ liệu giả
void generateMockData(HighSpeedDB& db, int count) {
    cout << "--- DANG SINH " << count << " DU LIEU GIA (TU DATA GENERATOR) ---\n";
    
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> gpaDist(1.0, 4.0); // Random GPA từ 1.0 đến 4.0

    int startID = 1000; // Bắt đầu từ ID 1000

    for (int i = 0; i < count; ++i) {
        int id = startID + i;
        string name = generateRandomName();
        string user = generateUsername(id);
        
        // Làm tròn GPA 1 số lẻ
        float rawGPA = gpaDist(gen);
        float gpa = round(rawGPA * 10) / 10.0;

        // Gọi hàm của HighSpeedDB
        db.addStudent(id, user, name, gpa);
    }
    cout << "-> Da xong! Database hien tai co: " << count << " records.\n" << endl;
}