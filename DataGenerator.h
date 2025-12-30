string generateRandomName() {
    string first[] = {"Nguyen", "Tran", "Le", "Pham", "Hoang", "Huynh", "Phan", "Vu", "Vo", "Dang"};
    string middle[] = {"Van", "Thi", "Huu", "Duc", "Minh", "Ngoc", "Thanh", "Quoc", "Gia", "Xuan"};
    string last[] = {"An", "Binh", "Cuong", "Dung", "Giang", "Huy", "Khoa", "Lam", "Nam", "Tuan"};
    
    return first[rand() % 10] + " " + middle[rand() % 10] + " " + last[rand() % 10];
}

void generateMockData(HighSpeedDB& db, int count) {
    cout << "Dang sinh " << count << " sinh vien gia lap..." << endl;
    
    // Sử dụng random engine hiện đại cho ID unique tốt hơn
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> idDist(1000000, 9999999); // ID 7 chữ số
    uniform_real_distribution<> gpaDist(0.0, 4.0);

    // Dùng set để đảm bảo ID không trùng khi generate
    vector<int> uniqueIDs;
    while(uniqueIDs.size() < count) {
        uniqueIDs.push_back(idDist(gen)); 
    }

    for (int i = 0; i < count; ++i) {
        Student s(uniqueIDs[i], generateRandomName(), gpaDist(gen));
        db.addStudent(s);
    }
    cout << "-> Da xong! Database hien tai co: " << db.getDataSize() << " records.\n" << endl;
}
