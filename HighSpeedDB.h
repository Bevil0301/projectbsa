class HighSpeedDB {
private:
    BTreeIndex* primaryIndex;   
    CuckooIndex* secondaryIndex; 
    vector<Student> rawData;    

public:
    HighSpeedDB() {
        primaryIndex = new BTreeIndex();
        secondaryIndex = new CuckooIndex();
    }

    ~HighSpeedDB() {
        delete primaryIndex;
        delete secondaryIndex;
    }

  
    void addStudent(const Student& s) {
        primaryIndex->insert(s);   
        secondaryIndex->insert(s); 
        rawData.push_back(s);      
    }

  
    bool searchFast(int id, Student& result) {
        return secondaryIndex->search(id, result);
    }

   
    bool searchSlow(int id, Student& result) {
        for (const auto& s : rawData) {
            if (s.id == id) {
                result = s;
                return true;
            }
        }
        return false;
    }
    
    // Hàm lấy kích thước dữ liệu hiện tại
    size_t getDataSize() const {
        return rawData.size();
    }
};
