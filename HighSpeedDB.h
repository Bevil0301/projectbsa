#pragma once
#include "Common.h"
#include "BTreeIndex.h"
#include "CuckooIndex.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class HighSpeedDB {
public:
    // Định nghĩa kiểu rõ ràng để code ngắn gọn
    using BTreeIndexType = BTree<Student, int>;
    using CuckooIndexType = CuckooHashTable<string, int>;

private:
    BTreeIndexType* primaryIndex;    // Module 1: B-Tree (Lưu trữ chính, Sort ID)
    CuckooIndexType* secondaryIndex; // Module 2: Cuckoo (Tra cứu User cực nhanh)
    vector<Student> rawData;         // Module 3: Backup & Duyệt tuần tự

public:
    HighSpeedDB() {
        // Khởi tạo các cấu trúc dữ liệu
        primaryIndex = new BTreeIndexType(BTREE_MIN_DEGREE);
        secondaryIndex = new CuckooIndexType(2000); // Size lớn chút cho ít va chạm
    }

    ~HighSpeedDB() {
        delete primaryIndex;
        delete secondaryIndex;
    }

    // --- 1. THÊM SINH VIÊN (Kết hợp cả 3 nơi) ---
    void addStudent(int id, string user, string name, float gpa) {
        Student s(id, user, name, gpa);
        
        // Thêm vào B-Tree (Để tìm theo ID)
        primaryIndex->insert(s);
        
        // Thêm vào Cuckoo (Để tìm theo User -> Trả về ID)
        secondaryIndex->insert(s.username, s.id);
        
        // Thêm vào Vector (Để lưu file cho dễ)
        rawData.push_back(s);
    }

    // --- 2. CẬP NHẬT ĐIỂM (TÍNH NĂNG BẠN VỪA THÊM) ---
    void updateGPA(int id, float newScore) {
        // Bước 1: Tìm trong B-Tree để sửa
        Student* s = primaryIndex->search(id);
        if (s != nullptr) {
            s->gpa = newScore;
            cout << "[UPDATE] Da cap nhat GPA cua ID " << id << " thanh " << newScore << endl;
            
            // Bước 2: Quan trọng! Phải sửa cả trong rawData để khi Save không bị mất
            for (auto& st : rawData) {
                if (st.id == id) {
                    st.gpa = newScore;
                    break;
                }
            }
        } else {
            cout << "[ERROR] Khong tim thay sinh vien co ID " << id << " de update!" << endl;
        }
    }

    // --- 3. TÌM KIẾM (Search) ---
    
    // Tìm ID dựa trên Username (Dùng Cuckoo) - Tốc độ O(1)
    int searchByUsername(string user) {
        return secondaryIndex->lookup(user);
    }

    // Tìm Student Object dựa trên ID (Dùng B-Tree) - Tốc độ O(log N)
    Student* searchByID(int id) {
        return primaryIndex->search(id);
    }

    // Tìm kiếm khoảng (Range Search)
    void searchRange(int minID, int maxID) {
        cout << "\n[RANGE] Danh sach tu ID " << minID << " den " << maxID << ":\n";
        primaryIndex->searchRange(minID, maxID);
    }

    // --- 4. CÁC TÍNH NĂNG QUẢN LÝ KHÁC ---

    // Hiển thị tất cả (đã sort)
    void showAllSorted() {
        primaryIndex->traverse();
    }

    // Visualizer: Xem cấu trúc Hash bên trong
    void showCuckooStructure() {
        secondaryIndex->printTable();
    }

    // Xóa User (Remove)
    void removeUser(string user) {
        secondaryIndex->remove(user);
        // Lưu ý: Tạm thời chỉ xóa trong Hash để demo thuật toán
    }

    // --- 5. LƯU & ĐỌC FILE (Dùng dấu phẩy như code của bạn) ---
    void saveData() {
        ofstream file("database.txt");
        if (!file.is_open()) return;

        // Ghi format: ID,Username,Name,GPA
        for (const auto& s : rawData) {
            file << s.id << "," << s.username << "," << s.fullName << "," << s.gpa << endl;
        }
        file.close();
        cout << "[DISK] Da luu " << rawData.size() << " sinh vien vao file.\n";
    }

    bool loadData() {
        ifstream file("database.txt");
        if (!file.is_open()) return false;
        if (file.peek() == ifstream::traits_type::eof()) return false; // File rỗng

        rawData.clear();
        string line;
        int count = 0;

        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string segment;
            vector<string> parts;

            // Tách chuỗi bằng dấu phẩy ','
            while (getline(ss, segment, ',')) {
                parts.push_back(segment);
            }

            if (parts.size() >= 4) {
                try {
                    int id = stoi(parts[0]);
                    string user = parts[1];
                    string name = parts[2];
                    float gpa = stof(parts[3]);
                    
                    // Gọi hàm addStudent của chính class này để nó tự đẩy vào BTree/Cuckoo
                    addStudent(id, user, name, gpa);
                    count++;
                } catch (...) { continue; }
            }
        }
        file.close();
        cout << "[DISK] Da tai thanh cong " << count << " ban ghi.\n";
        return true;
    }
};