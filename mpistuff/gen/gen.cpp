#include <memory>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream> 
#include <random>
#include <string>
#include <chrono>
using namespace std;

template<typename T>
void fill_random(vector<T>& vec,int seed){
    mt19937 generator(seed);
    uniform_real_distribution<T> distribution(-100.0, 100.0);

    uint64_t N = vec.size();
    for (uint64_t i = 0; i < N; i++) {
        vec[i] = distribution(generator);
    }
}

template <typename T>
void print_100(vector<T>& arr) {
    for (int i = 0; i < 100; i++) {
        cout << arr[i] << ' ';
        if ((i + 1) % 10 == 0) {
            cout << endl;
        }
    }
}
void save_to_file(const vector<float>& data, const string& filename) {
    ofstream ofs(filename, ios::binary); // 直接使用构造函数初始化
    if (!ofs) {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }

    ofs.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(float));
    if (!ofs) {
        cerr << "Error writing data to file: " << filename << endl;
    }

    ofs.close();
}

// 从文件中读取数据
vector<float> read_floats_from_file(const string& filename) {
    ifstream ifs(filename, ios::binary); // 直接使用构造函数初始化
    if (!ifs) {
        cerr << "Error opening file for reading: " << filename << endl;
        return {};
    }
    
    ifs.seekg(0, ios::end);
    streampos file_size = ifs.tellg();
    ifs.seekg(0, ios::beg);
    size_t num_floats = file_size / sizeof(float);
    vector<float> data(num_floats);
    ifs.read(reinterpret_cast<char*>(data.data()), file_size);
    ifs.close();


    return data;
}
int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <0,1,2> for 256M 1G 16G and filesname\n", argv[0]);
        return 1;
    }

    int seed = 42;
    int size_idx = atoi(argv[1]);
    string filename = argv[2];

    vector<unsigned long long> sizes(3,1);
    vector<int> shift_bits = {28,30,32};
    for(int i = 0;i<3;i++){
        sizes[i]= sizes[i]<<shift_bits[i];
    }
    
    
    
    vector<float> all_floats(sizes[size_idx]);
    fill_random(all_floats, seed);


    save_to_file(all_floats, filename);

    // vector<float> read_floats = read_floats_from_file("floats.bin");

    // auto start = chrono::high_resolution_clock::now();
    // auto end = chrono::high_resolution_clock::now();
    // chrono::duration<double> fill_time = end - start;
    // cout << "Time to fill random numbers: " << fill_time.count() << " seconds" << endl;
    return 0;
}
