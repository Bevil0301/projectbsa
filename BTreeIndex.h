// Filename: BTreeIndex.h
#pragma once
#include "Common.h" // Để lấy Interface
#include <vector>

// Node B-Tree
template <typename RecordType, typename KeyType>
struct BTreeNode {
    bool isLeaf;
    vector<RecordType> data;
    vector<BTreeNode*> children;
    BTreeNode(bool leaf) { isLeaf = leaf; } // constructor 
};

// Class B-Tree: KẾ THỪA TỪ INTERFACE
template <typename RecordType, typename KeyType>
class BTree : public BTreeInterface<RecordType, KeyType> {
private:
    BTreeNode<RecordType, KeyType> *root;
    int t;

    // --- LOGIC RIÊNG TƯ (PRIVATE) ---
    void splitChildFixed(BTreeNode<RecordType, KeyType> *x, int i) {
        BTreeNode<RecordType, KeyType> *y = x->children[i];//vị trí node con thứ i trong mảng children của x
        BTreeNode<RecordType, KeyType> *z = new BTreeNode<RecordType, KeyType>(y->isLeaf);//tạo node z mới nếu y là lá thì z cũng là lá
        
        for (int j = 0; j < t - 1; j++) z->data.push_back(y->data[j + t]);//chuyển t-1 phần tử từ y sang z t đến 2t-2(nửa sau của node)
        if (!y->isLeaf) {
            for (int j = 0; j < t; j++) z->children.push_back(y->children[j + t]);
        }
        
        RecordType median = y->data[t - 1];
        y->data.resize(t - 1);//xóa bỏ nửa sau 
        if (!y->isLeaf) y->children.resize(t);//neu y la internal node thi 
        
        x->children.insert(x->children.begin() + i + 1, z);
        x->data.insert(x->data.begin() + i, median);
    }

    void insertNonFull(BTreeNode<RecordType, KeyType> *x, RecordType k) {//hàm tìm vị trí chính xác để chèn dữ liệu
        int i = x->data.size() - 1;
        if (x->isLeaf) {
            while (i >= 0 && k < x->data[i]) i--;//duyệt từ phải sang trái để tìm vị trí k > phan tu o vi tri i
            x->data.insert(x->data.begin() + i + 1, k);
        } else {
            while (i >= 0 && k < x->data[i]) i--;
            i++;
            if (x->children[i]->data.size() == 2 * t - 1) {//kiểm tra nếu sắp đầy thì gọi splitchild fixed nhằm luôn đảm bảo B-Tree đi theo 1 chiều
                //đảm bảo có số lượng phần tử từ t-1 đến 2t-1
                splitChildFixed(x, i);
                if (k > x->data[i]) i++;//sau khi tach ra 2 mang y va z neu k > x->data thì qua z(i++) neu k < x->data thi qua y(i)
            }
            insertNonFull(x->children[i], k);//gọi đệ quy đến khi đến nút lá
        }
    }

    RecordType* searchRecursive(BTreeNode<RecordType, KeyType> *curr, KeyType k) {
        int i = 0;
        // Logic Flex: RecordType tự so sánh với KeyType nhờ operator <
        while (i < curr->data.size() && curr->data[i] < k) i++;//tìm phần tử >= k
        if (i < curr->data.size() && curr->data[i] == k) return &curr->data[i];//nếu k == curr->data[i] thì return địa chỉ của curr->data[i]
        if (curr->isLeaf) return nullptr;//nếu tới nút lá thì không tìm thấy
        return searchRecursive(curr->children[i], k);//gọi đệ quy tiếp tục tìm kiếm đến khi tới nút lá hoặc tìm thấy
    }

    void traverseRecursive(BTreeNode<RecordType, KeyType> *curr) {
        int i;
        for (i = 0; i < curr->data.size(); i++) {
            if (!curr->isLeaf) traverseRecursive(curr->children[i]);
            curr->data[i].print(); // Flex: Gọi hàm print của RecordType
        }
        if (!curr->isLeaf) traverseRecursive(curr->children[i]);
    }

    void clear(BTreeNode<RecordType, KeyType>* node) {
        if (node) {
            if (!node->isLeaf) {
                for (auto child : node->children) clear(child);
            }
            delete node;
        }
    }

public:
    BTree(int _t) : t(_t), root(nullptr) {}

    // --- CÀI ĐẶT CÁC HÀM ĐÃ HỨA TRONG INTERFACE ---
    
    void insert(RecordType k) override {
        if (root == nullptr) {
            root = new BTreeNode<RecordType, KeyType>(true);
            root->data.push_back(k);
        } else {
            if (root->data.size() == 2 * t - 1) {
                BTreeNode<RecordType, KeyType> *s = new BTreeNode<RecordType, KeyType>(false);
                s->children.push_back(root);
                splitChildFixed(s, 0);
                int i = 0;
                if (s->data[0] < k) i++;
                insertNonFull(s->children[i], k);
                root = s;
            } else {
                insertNonFull(root, k);
            }
        }
    }

    RecordType* search(KeyType k) override {
        return (root == nullptr) ? nullptr : searchRecursive(root, k);
    }

    void traverse() override {
        if (root != nullptr) traverseRecursive(root);
    }

    // Trong BTreeIndex.h
    void searchRangeRecursive(BTreeNode<RecordType, KeyType>* node, KeyType minK, KeyType maxK) {
        int i = 0;
        // Duyệt qua các phần tử trong node hiện tại
        while (i < node->data.size()) {
            // Nếu không phải lá, hãy đi xuống con bên trái trước
            // Chỉ xuống nếu khóa của phần tử hiện tại lớn hơn minK
            if (!node->isLeaf && node->data[i] > minK) {
                searchRangeRecursive(node->children[i], minK, maxK);
            }

            // Nếu dữ liệu hiện tại nằm trong khoảng, thì in ra
            if (node->data[i] >= minK && node->data[i] <= maxK) {
                node->data[i].print();
            }

            // Nếu đã vượt quá maxK thì có thể dừng (tối ưu hóa)
            if (node->data[i] > maxK) return;

            i++;
        }

        // Đừng quên đứa con cuối cùng bên phải
        if (!node->isLeaf && i < node->children.size() && node->data[i-1] < maxK) {
            searchRangeRecursive(node->children[i], minK, maxK);
        }
    }

    // Hàm public để bên ngoài gọi
    void searchRange(KeyType minK, KeyType maxK) {
        if (root != nullptr) {
            searchRangeRecursive(root, minK, maxK);
        }
    }

    // Trong BTreeIndex.h
    void saveToStream(BTreeNode<RecordType, KeyType>* node, ofstream& fout) {
        if (node == nullptr) return;
        int i;
        for (i = 0; i < node->data.size(); i++) {
            if (!node->isLeaf) saveToStream(node->children[i], fout);
            // Ghi dữ liệu: ID, Username, FullName, GPA phân cách bằng dấu phẩy
            fout << node->data[i].id << "," 
                << node->data[i].username << "," 
                << node->data[i].fullName << "," 
                << node->data[i].gpa << "\n";
        }
        if (!node->isLeaf) saveToStream(node->children[i], fout);
    }

    void saveToFile(string filename) {
        ofstream fout(filename,ios::trunc);//ios::trunc đảm bảo xóa nội dung cũ trước khi ghi mới
        if (fout.is_open()) {
            if (root) saveToStream(root, fout);
            fout.close();
        }
    }

    ~BTree() {//destructor
        clear(root); 
    }

};