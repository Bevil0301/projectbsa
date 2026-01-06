// Filename: BenchMarking.h
#pragma once
#include <iostream>
#include <chrono>
#include <iomanip>
#include <string>
#include "HighSpeedDB.h"

using namespace std;
using namespace std::chrono;

void runBenchmark(HighSpeedDB& db, int searchID) {
    string searchUser = "user_" + to_string(searchID);
    
    // Số lần lặp: Giảm xuống 1 chút vì Linear Search chạy chậm
    // Nếu chạy 100,000 lần với Linear Search trên tập dữ liệu lớn sẽ rất lâu.
    int iterations = 5000; 

    cout << "\n================= BENCHMARK (3-WAY) =================" << endl;
    cout << "Muc tieu: ID " << searchID << endl;
    cout << "So lan lap: " << iterations << " lan" << endl;
    cout << "-----------------------------------------------------" << endl;

    // --- 1. LINEAR SEARCH (O(N)) ---
    auto start = high_resolution_clock::now();
    for(int i = 0; i < iterations; ++i) {
        volatile bool found = db.searchLinear(searchID);
    }
    auto end = high_resolution_clock::now();
    double avgLinear = duration_cast<nanoseconds>(end - start).count() / (double)iterations;

    // --- 2. B-TREE SEARCH (O(log N)) ---
    start = high_resolution_clock::now();
    for(int i = 0; i < iterations; ++i) {
        volatile auto ptr = db.searchByID(searchID);
    }
    end = high_resolution_clock::now();
    double avgBTree = duration_cast<nanoseconds>(end - start).count() / (double)iterations;

    // --- 3. CUCKOO HASH (O(1)) ---
    start = high_resolution_clock::now();
    for(int i = 0; i < iterations; ++i) {
        volatile int id = db.searchByUsername(searchUser);
    }
    end = high_resolution_clock::now();
    double avgHash = duration_cast<nanoseconds>(end - start).count() / (double)iterations;

    // --- KẾT QUẢ ---
    cout << left << setw(20) << "Phuong phap" << setw(15) << "Time (ns)" << "Do phuc tap" << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << left << setw(20) << "Linear Search" << setw(15) << avgLinear << "O(N) - Cham" << endl;
    cout << left << setw(20) << "B-Tree Search" << setw(15) << avgBTree << "O(log N)" << endl;
    cout << left << setw(20) << "Cuckoo Hash" << setw(15) << avgHash << "O(1) - Sieu nhanh" << endl;
    cout << "-----------------------------------------------------" << endl;

    // So sánh vui
    if (avgHash > 0 && avgBTree > 0) {
        cout << "=> Cuckoo Hash nhanh hon Linear: " << (long)(avgLinear / avgHash) << " lan." << endl;
        cout << "=> B-Tree nhanh hon Linear:      " << (long)(avgLinear / avgBTree) << " lan." << endl;
    } else {
        cout << "=> Toc do qua nhanh (gan 0ns) nen khong the so sanh chinh xac!" << endl;
    }
    cout << "=====================================================\n";
}