#pragma once
#include "Common.h"
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

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
        Entry() : active(false) {}
        Entry(SecondaryKey k, PrimaryKey v, bool a) : key(k), value(v), active(a) {}
    };

    vector<Entry> table1;
    vector<Entry> table2;
    int currentSize;
    int numElements;

    int polynomialHash(SecondaryKey key, int seed) {
        string sKey = (string)key; 
        long long hashValue = 0;
        long long p = 53;
        long long m = 1e9 + 9;
        long long p_pow = 1;

        for (unsigned char c : sKey) { 
            hashValue = (hashValue + c * p_pow) % m;
            p_pow = (p_pow * p) % m;
        }

        long long finalHash = (hashValue + seed) % currentSize;
        if (finalHash < 0) finalHash += currentSize;
        return (int)finalHash;
    }

    int hash1(SecondaryKey key) {
        return polynomialHash(key, 0);
    }

    int hash2(SecondaryKey key) {
        return polynomialHash(key, 1234567);
    }

    void rehash() {
        vector<Entry> oldTable1 = table1;
        vector<Entry> oldTable2 = table2;

        currentSize *= 2;
        table1.assign(currentSize, Entry());
        table2.assign(currentSize, Entry());
        numElements = 0;

        for (auto &e : oldTable1) if (e.active) insert(e.key, e.value);
        for (auto &e : oldTable2) if (e.active) insert(e.key, e.value);
    }

    void insertHelper(SecondaryKey key, PrimaryKey value) {
        for (int count = 0; count < MAX_KICK_COUNT; count++) {
            int h1 = hash1(key);
            if (!table1[h1].active) {
                table1[h1] = Entry(key, value, true);
                numElements++;
                return;
            }
            swap(key, table1[h1].key);
            swap(value, table1[h1].value);

            int h2 = hash2(key);
            if (!table2[h2].active) {
                table2[h2] = Entry(key, value, true);
                numElements++;
                return;
            }
            swap(key, table2[h2].key);
            swap(value, table2[h2].value);
        }
        rehash();
        insertHelper(key, value);
    }

public:
    CuckooHashTable(int size = 1000) : currentSize(size), numElements(0) {
        table1.resize(currentSize);
        table2.resize(currentSize);
    }

    PrimaryKey lookup(SecondaryKey key) override {
        int h1 = hash1(key);
        if (table1[h1].active && table1[h1].key == key) return table1[h1].value;

        int h2 = hash2(key);
        if (table2[h2].active && table2[h2].key == key) return table2[h2].value;

        return -1; 
    }

    void insert(SecondaryKey key, PrimaryKey value) override {
        int h1 = hash1(key);
        if (table1[h1].active && table1[h1].key == key) { table1[h1].value = value; return; }
        
        int h2 = hash2(key);
        if (table2[h2].active && table2[h2].key == key) { table2[h2].value = value; return; }

        if ((float)numElements / (currentSize * 2) > 0.7) {
            rehash();
        }
        insertHelper(key, value);
    }
    
    void remove(SecondaryKey key) {
        int h1 = hash1(key);
        if (table1[h1].active && table1[h1].key == key) {
            table1[h1].active = false;
            numElements--;
            return;
        }
        int h2 = hash2(key);
        if (table2[h2].active && table2[h2].key == key) {
            table2[h2].active = false;
            numElements--;
            return;
        }
    }

void printTable() {
        // 1. In Thống kê tổng quan trước
        cout << "\n" << string(80, '=') << endl;
        cout << " [CUCKOO HASH STATUS] " << endl;
        cout << " + Capacity (Suc chua): " << currentSize << " slots" << endl;
        cout << " + Active Items (Da dung): " << numElements << " sinh vien" << endl;
        
        // Tính % lấp đầy
        float loadFactor = (float)numElements / (currentSize * 2) * 100.0f;
        cout << " + Load Factor (Ty le day): " << fixed << setprecision(2) << loadFactor << "%" << endl;
        
        if (loadFactor > 50.0) cout << "   (Canh bao: Bang sap day, chuan bi Rehash...)" << endl;
        
        cout << string(80, '-') << endl;
        cout << left << setw(10) << "Index" << " | "
             << left << setw(32) << "TABLE 1 (Primary)" << " | "
             << left << setw(32) << "TABLE 2 (Backup)" << endl;
        cout << string(80, '-') << endl;

        // 2. Duyệt toàn bộ bảng, KHÔNG GIỚI HẠN LIMIT
        bool isEmpty = true;
        int countPrinted = 0;

        for (int i = 0; i < currentSize; i++) {
            // Logic lọc: Chỉ in nếu dòng đó có dữ liệu
            bool hasRowData = false;
            if (table1[i].active || table2[i].active) hasRowData = true;

            if (hasRowData) {
                isEmpty = false;
                countPrinted++;

                // In chỉ số Index
                cout << left << setw(10) << i << " | ";

                // In dữ liệu Bảng 1
                if (table1[i].active) {
                    stringstream ss;
                    ss << table1[i].key << " (" << table1[i].value << ")";
                    cout << setw(32) << ss.str();
                } else {
                    cout << setw(32) << " ."; 
                }

                cout << " | ";

                // In dữ liệu Bảng 2
                if (table2[i].active) {
                    stringstream ss;
                    ss << table2[i].key << " (" << table2[i].value << ")";
                    cout << setw(32) << ss.str();
                } else {
                    cout << setw(32) << " .";
                }
                
                cout << endl;
            }
        }

        // 3. Xử lý trường hợp đặc biệt
        if (isEmpty) {
            cout << "       (Bang hien tai dang trong rong)" << endl;
        } else {
            cout << string(80, '-') << endl;
            cout << "-> Da hien thi toan bo " << countPrinted << " dong co du lieu." << endl;
        }
        cout << string(80, '=') << endl;
    }
};