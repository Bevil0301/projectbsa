void runBenchmark(HighSpeedDB& db, int searchID) {
    Student result;
    int iterations = 1000; // Chạy 1000 lần để lấy trung bình cho chính xác

    cout << "=== BENCHMARK REPORT ===" << endl;
    cout << "Target ID: " << searchID << endl;
    cout << "So lan chay thu nghiem: " << iterations << endl;
    cout << "---------------------------------------------------" << endl;

    // 1. Đo Linear Search (Tìm thường)
    auto start = chrono::high_resolution_clock::now();
    for(int i=0; i<iterations; ++i) {
        volatile bool found = db.searchSlow(searchID, result);
    }
    auto end = chrono::high_resolution_clock::now();
    auto durationLinear = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    double avgLinear = durationLinear / (double)iterations;

    // 2. Đo Cuckoo Hash Search (Index)
    start = chrono::high_resolution_clock::now();
    for(int i=0; i<iterations; ++i) {
        volatile bool found = db.searchFast(searchID, result);
    }
    end = chrono::high_resolution_clock::now();
    auto durationHash = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    double avgHash = durationHash / (double)iterations;

    // In kết quả
    cout << left << setw(20) << "Phuong phap" << setw(20) << "Thoi gian (ns)" << "Ghi chu" << endl;
    cout << "---------------------------------------------------" << endl;
    cout << left << setw(20) << "Linear Search" << setw(20) << avgLinear << "O(N) - Cham" << endl;
    cout << left << setw(20) << "Cuckoo Index" << setw(20) << avgHash << "O(1) - Sieu nhanh" << endl;
    cout << "---------------------------------------------------" << endl;
    
    if (avgHash > 0)
        cout << "=> Cuckoo Hash nhanh hon gap " << (long)(avgLinear / avgHash) << " lan!" << endl;
    else 
        cout << "=> Cuckoo Hash qua nhanh (gan nhu 0ns)!" << endl;
}
