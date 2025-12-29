#pragma once
#include "Common.h"
#include <vector>
#include <string>
#include <functional> // Sử dụng std::hash
#include <iostream>

using namespace std;

// Đảm bảo có hằng số giới hạn số lần đá trứng để tránh lặp vô tận
#ifndef MAX_KICK_COUNT
#define MAX_KICK_COUNT 500
#endif

template <typename SecondaryKey, typename PrimaryKey>
class CuckooHashTable : public CuckooHashInterface<SecondaryKey, PrimaryKey> {
private:
    struct Entry {
        SecondaryKey key;
        PrimaryKey value;
        bool active;

        // Constructor mặc định
        Entry() : active(false) {}

        // Constructor đầy đủ tham số để tránh lỗi gán list khởi tạo {...}
        Entry(SecondaryKey k, PrimaryKey v, bool a) 
            : key(k), value(v), active(a) {}
    };

    vector<Entry> table1;
    vector<Entry> table2;
    int currentSize;

    // Hàm băm 1
    size_t hash1(SecondaryKey key) {
        return hash<SecondaryKey>{}(key) % currentSize;
    }

    // Hàm băm 2 (biến tấu từ key gốc để tạo vị trí khác biệt)
    size_t hash2(SecondaryKey key) {
        string saltedKey = string(key) + "_alt"; 
        return hash<string>{}(saltedKey) % currentSize;
    }

    // Hàm Rehash khi bảng quá đầy hoặc bị chu trình (cycle)
    void rehash() {
        cout << "[LOG] Cuckoo Hash: Rehash dang dien ra (X2 Size)...\n";
        vector<Entry> oldTable1 = table1;
        vector<Entry> oldTable2 = table2;

        currentSize *= 2;
        table1.assign(currentSize, Entry());
        table2.assign(currentSize, Entry());

        for (auto &e : oldTable1) if (e.active) insert(e.key, e.value);
        for (auto &e : oldTable2) if (e.active) insert(e.key, e.value);
    }

public:
    CuckooHashTable(int size = 1000) : currentSize(size) {
        table1.resize(currentSize);
        table2.resize(currentSize);
    }

    // Tra cứu O(1)
    PrimaryKey lookup(SecondaryKey key) override {
        size_t h1 = hash1(key);
        if (table1[h1].active && table1[h1].key == key) return table1[h1].value;

        size_t h2 = hash2(key);
        if (table2[h2].active && table2[h2].key == key) return table2[h2].value;

        return -1; // Trả về -1 nếu không tìm thấy ID
    }

    // Thêm phần tử với cơ chế Cuckoo Kicking
    void insert(SecondaryKey key, PrimaryKey value) override {
        // Nếu key đã tồn tại, không cần chèn mới
        if (lookup(key) != -1) return;

        SecondaryKey currKey = key;
        PrimaryKey currVal = value;

        for (int count = 0; count < MAX_KICK_COUNT; count++) {
            // Thử bảng 1
            size_t h1 = hash1(currKey);
            if (!table1[h1].active) {
                table1[h1] = Entry(currKey, currVal, true);
                return;
            }

            // Đá trứng: Đổi chỗ phần tử hiện tại với phần tử trong bảng
            swap(currKey, table1[h1].key);
            swap(currVal, table1[h1].value);

            // Thử bảng 2 với phần tử vừa bị đá
            size_t h2 = hash2(currKey);
            if (!table2[h2].active) {
                table2[h2] = Entry(currKey, currVal, true);
                return;
            }

            // Tiếp tục đá ở bảng 2
            swap(currKey, table2[h2].key);
            swap(currVal, table2[h2].value);
        }

        // Nếu vượt quá giới hạn đá trứng -> Cần Rehash bảng
        rehash();
        insert(currKey, currVal);
    }
};