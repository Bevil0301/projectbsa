#pragma once
#include "Common.h"
#include "BTreeIndex.h"
#include "CuckooIndex.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

class HighSpeedDB {
public:
    // Định nghĩa kiểu cho gọn
    using BTreeIndexType = BTree<Student, int>;
    using CuckooIndexType = CuckooHashTable<string, int>;

private:
    BTreeIndexType* primaryIndex;    // B-Tree
    CuckooIndexType* secondaryIndex; // Hash
    
    // [THÊM LẠI] Vector lưu trữ tuần tự (phục vụ so sánh tốc độ)
    vector<Student> rawData; 

public:
    HighSpeedDB() {
        primaryIndex = new BTreeIndexType(BTREE_MIN_DEGREE);
        secondaryIndex = new CuckooIndexType(2000); 
    }

    ~HighSpeedDB() {
        delete primaryIndex;
        delete secondaryIndex;
        // vector tự giải phóng bộ nhớ, không cần delete
    }

    // --- 1. THÊM SINH VIÊN (Đồng bộ cả 3 nơi) ---
    void addStudent(int id, string user, string name, float gpa) {
        Student s(id, user, name, gpa);
        
        // 1. Lưu vào B-Tree (Sắp xếp, Tìm ID nhanh)
        primaryIndex->insert(s);
        
        // 2. Lưu vào Cuckoo Hash (Tìm User nhanh)
        secondaryIndex->insert(s.username, s.id);
        
        // 3. [THÊM LẠI] Lưu vào Vector (Để test Linear Search chậm chạp)
        rawData.push_back(s);
    }

    // --- [MỚI] LINEAR SEARCH (Độ phức tạp O(N)) ---
    // Dùng để chạy benchmark so sánh sự chậm chạp so với B-Tree
    bool searchLinear(int id) {
        for (const auto& s : rawData) {
            if (s.id == id) {
                return true;
            }
        }
        return false;
    }

    // --- 2. CẬP NHẬT GPA ---
    void updateGPA(int id, float newScore) {
        // Sửa trong B-Tree (Nơi lưu trữ chính)
        Student* s = primaryIndex->search(id);
        
        if (s != nullptr) {
            s->gpa = newScore; // Update qua con trỏ (nhanh)

            // [ĐỒNG BỘ DỮ LIỆU]
            // Vì ta dùng thêm rawData, nên phải sửa cả trong rawData để dữ liệu không bị lệch.
            // (Dù việc này tốn O(N) nhưng cần thiết để giữ tính nhất quán)
            for (auto& std : rawData) {
                if (std.id == id) {
                    std.gpa = newScore;
                    break;
                }
            }
            cout << "[UPDATE] Da cap nhat GPA cho ID " << id << ".\n";
        } else {
            cout << "[ERROR] Khong tim thay sinh vien co ID " << id << endl;
        }
    }

    // --- 3. CÁC HÀM TÌM KIẾM NHANH (WRAPPER) ---
    int searchByUsername(string user) {
        return secondaryIndex->lookup(user);
    }

    Student* searchByID(int id) {
        return primaryIndex->search(id);
    }

    void searchRange(int minID, int maxID) {
        primaryIndex->searchRange(minID, maxID);
    }

    void showAllSorted() {
        primaryIndex->traverse();
    }

    // --- 4. LƯU & ĐỌC FILE ---
    
    // Lưu: Dùng B-Tree để ghi file đã sắp xếp
    void saveData() {
        primaryIndex->saveToFile("database.txt");
        // cout << "[DISK] Da luu du lieu xuong file.\n";
    }

    // Đọc: Đọc từng dòng và gọi addStudent để đẩy vào cả 3 cấu trúc
    bool loadData() {
        ifstream file("database.txt");
        if (!file.is_open()) return false;

        // Kiểm tra file rỗng
        if (file.peek() == ifstream::traits_type::eof()) return false;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string segment;
            vector<string> parts;

            while (getline(ss, segment, ',')) {
                parts.push_back(segment);
            }

            if (parts.size() >= 4) {
                try {
                    int id = stoi(parts[0]);
                    string user = parts[1];
                    string name = parts[2];
                    float gpa = stof(parts[3]);

                    // Gọi hàm addStudent của class này
                    // Nó sẽ tự động thêm vào B-Tree, Hash và cả rawData
                    addStudent(id, user, name, gpa);
                } catch (...) { continue; }
            }
        }
        file.close();
        return true;
    }
};