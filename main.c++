// Filename: main.cpp
#include <iostream>
#include <limits> // Để dùng numeric_limits xóa bộ nhớ đệm
#include "Common.h"
#include "HighSpeedDB.h"
#include "DataGenerator.h" // <--- Lấy hàm sinh dữ liệu từ đây
#include "BenchMarking.h"

using namespace std;

// --- HÀM MENU GIAO DIỆN ---
void runMenu(HighSpeedDB& db) {
    int choice;
    do {
        // Vẽ giao diện menu đẹp
        cout << "\n======================================================\n";
        cout << "       HE THONG QUAN LY SINH VIEN (B-TREE + CUCKOO)\n";
        cout << "======================================================\n";
        cout << "1. Them sinh vien moi (Thu cong)\n";
        cout << "2. Tim kiem sinh vien theo ID (B-Tree - O(logN))\n";
        cout << "3. Tim kiem ID theo Username (Cuckoo Hash - O(1))\n";
        cout << "4. Tim kiem theo khoang ID (Range Query)\n";
        cout << "5. Cap nhat diem GPA (Update truc tiep)\n";
        cout << "6. Hien thi toan bo danh sach (Da sap xep)\n";
        cout << "7. Sinh them du lieu gia (Mock Data)\n";
        cout << "8. Luu du lieu ra file (Force Save)\n";
        cout << "9. Chay Benchmark (So sanh Toc do)\n";
        cout << "0. Luu va Thoat chuong trinh\n";
        cout << "======================================================\n";
        cout << "Nhap lua chon cua ban: ";
        cin >> choice;

        // Xử lý lỗi nếu người dùng nhập chữ thay vì số
        if (cin.fail()) {
            cin.clear(); // Xóa cờ lỗi
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Xóa bộ đệm
            choice = -1; // Gán giá trị sai để chạy vào default
        }

        switch (choice) {
            case 1: { // Thêm sinh viên
                int id;
                string name, user;
                float gpa;
                
                cout << "Nhap ID: "; cin >> id;
                cin.ignore(); // Xóa ký tự xuống dòng thừa
                cout << "Nhap Ho ten: "; getline(cin, name);
                cout << "Nhap Username: "; cin >> user;
                cout << "Nhap GPA: "; cin >> gpa;

                // Kiểm tra sơ bộ ID có tồn tại chưa (Optional)
                if (db.searchByID(id) != nullptr) {
                    cout << "[ERROR] ID " << id << " da ton tai!\n";
                } else {
                    db.addStudent(id, user, name, gpa);
                    cout << "[SUCCESS] Da them sinh vien moi.\n";
                }
                break;
            }
            case 2: { // Tìm theo ID
                int id;
                cout << "Nhap ID can tim: "; cin >> id;
                Student* s = db.searchByID(id);
                if (s != nullptr) {
                    cout << "\n--- KET QUA TIM KIEM ---\n";
                    s->print();
                    cout << "   Username: " << s->username << endl;
                    cout << "   GPA: " << s->gpa << endl;
                } else {
                    cout << "[INFO] Khong tim thay sinh vien co ID: " << id << endl;
                }
                break;
            }
            case 3: { // Tìm theo Username
                string user;
                cout << "Nhap Username can tim: "; cin >> user;
                int foundID = db.searchByUsername(user);
                
                if (foundID != -1) {
                    cout << "\n[FOUND] Username '" << user << "' tuong ung voi ID: " << foundID << endl;
                    // Tự động tìm thông tin chi tiết sau khi có ID
                    Student* s = db.searchByID(foundID);
                    if(s) s->print();
                } else {
                    cout << "[INFO] Khong tim thay Username: " << user << endl;
                }
                break;
            }
            case 4: { // Range Query
                int minID, maxID;
                cout << "Nhap ID bat dau: "; cin >> minID;
                cout << "Nhap ID ket thuc: "; cin >> maxID;
                if (minID > maxID) swap(minID, maxID); // Đảo nếu nhập ngược
                
                db.searchRange(minID, maxID);
                break;
            }
            case 5: { // Update GPA
                int id;
                float score;
                cout << "Nhap ID can sua diem: "; cin >> id;
                cout << "Nhap diem moi (0.0 - 4.0): "; cin >> score;
                db.updateGPA(id, score);
                break;
            }
            case 6: { // Show All
                cout << "\n--- DANH SACH SINH VIEN ---\n";
                db.showAllSorted();
                break;
            }
            case 7: { // Sinh thêm dữ liệu
                int count;
                cout << "Ban muon sinh them bao nhieu sinh vien? "; cin >> count;
                generateMockData(db, count); // Gọi hàm từ DataGenerator.h
                break;
            }
            case 8: { // Lưu file
                db.saveData();
                break;
            }
            case 9: {
                int id;
                cout << "Nhap ID de test toc do tim kiem: "; cin >> id;
                // Kiểm tra xem ID có tồn tại không trước khi test cho chính xác
                if (db.searchByID(id)) {
                    runBenchmark(db, id);
                } else {
                    cout << "[WARNING] ID nay khong ton tai. Ket qua test co the khong phan anh dung thuc te.\n";
                    runBenchmark(db, id);
                }
                break;
            }
            case 0:
                cout << "Dang luu du lieu lan cuoi truoc khi thoat...\n";
                db.saveData();
                cout << "Tam biet!\n";
                break;
            default:
                cout << "Lua chon khong hop le. Vui long chon lai!\n";
        }
        
        // Dừng màn hình một chút để xem kết quả (Optional)
        if (choice != 0) {
            cout << "\n(An Enter de tiep tuc...)";
            cin.ignore();
            cin.get();
        }

    } while (choice != 0);
}

// --- HÀM MAIN CHÍNH ---
int main() {
    // 1. Khởi tạo Database
    HighSpeedDB db;

    // 2. Load dữ liệu
    cout << "Dang khoi dong he thong...\n";
    if (!db.loadData()) {
        // Nếu chưa có file dữ liệu -> Sinh tự động 50 người
        cout << "[INFO] Lan dau chay chuong trinh. Dang tao du lieu mau...\n";
        generateMockData(db, 50); 
        db.saveData(); 
    } else {
        // Đã load xong, kiểm tra nếu file rỗng thì sinh thêm
        // (Đây là logic tùy chọn, bạn có thể bỏ qua)
        // Student* check = db.searchByID(1000); // Check thử
        // if (!check) generateMockData(db, 10);
    }

    // 3. Gọi Menu điều khiển
    runMenu(db);

    return 0;
}