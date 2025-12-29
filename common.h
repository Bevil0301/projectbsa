#pragma once
#include <iostream>
#include <vector>
#include <string>
#include<iomanip>
#include <functional> // Để dùng std::function

using namespace std;

// Cấu hình chung
const int BTREE_MIN_DEGREE = 3;
const int INITIAL_HASH_SIZE = 1000;

// --- DỮ LIỆU SINH VIÊN (Để test) ---
struct Student {
    int id;
    string username;
    string fullName;
    float gpa;

    Student(int _id=0, string _user="", string _name="", float _gpa=0) 
        : id(_id), username(_user), fullName(_name), gpa(_gpa) {}

    // BẮT BUỘC: Phải có operator để B-Tree so sánh được
    bool operator<(const Student& other) const { return id < other.id; }
    bool operator>(const Student& other) const { return id > other.id; }
    bool operator==(const Student& other) const { return id == other.id; }
    
    // So sánh với Key (int)
    bool operator<(const int& key) const { return id < key; }
    bool operator>(const int& key) const { return id > key; }
    bool operator==(const int& key) const { return id == key; }

    // Thêm vào trong struct Student tại Common.h
    bool operator>=(const int& key) const { return id >= key; }
    bool operator<=(const int& key) const { return id <= key; }
    
    // Nếu cần so sánh giữa 2 Student với nhau
    bool operator>=(const Student& other) const { return id >= other.id; }
    bool operator<=(const Student& other) const { return id <= other.id; }

    void print() const {
        cout << "[ID: " << setw(5) << id << " | Name: " << fullName << "]\n";
    }
};

// ==========================================
// INTERFACES (Dạng Template)
// ==========================================

// Interface cho B-Tree: Chấp nhận kiểu Record và kiểu Key chính
template <typename RecordType, typename PrimaryKey>
class BTreeInterface {
public:
    virtual void insert(RecordType record) = 0;
    virtual RecordType* search(PrimaryKey key) = 0;
    virtual void traverse() = 0;
};

// Interface cho Hash: Chấp nhận kiểu Key phụ và kiểu Key chính (Value)
template <typename SecondaryKey, typename PrimaryKey>
class CuckooHashInterface {
public:
    virtual void insert(SecondaryKey key, PrimaryKey value) = 0;
    virtual PrimaryKey lookup(SecondaryKey key) = 0;
};