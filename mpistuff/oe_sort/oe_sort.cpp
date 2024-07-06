#include <memory>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream> 
#include <random>
#include <string>
#include <chrono>
#include <mpi.h>

using namespace std;


template <typename T>
void print_100(vector<T>& arr) {
    for (int i = 0; i < 100; i++) {
        cout << arr[i] << ' ';
        if ((i + 1) % 10 == 0) {
            cout << endl;
        }
    }
}

void MergeHigh(vector<float>& local_data, vector<float>& recv_data) {
    // Implement the merge high function
    // This function should merge recv_data into local_data for higher half sorting
    int N = local_data.size();
    vector<float> highdata(N);
    int j = N-1, k = N-1;
    for(int i = N-1;i>=0;i--){
        if(j >= 0 && (k<0|| local_data[j] > recv_data[k])){
            highdata[i] = local_data[j];
            j--;
        } else{
            highdata[i] = recv_data[k];
            k--;
        }
    }
    local_data = highdata;
}
void MergeLow(vector<float>& local_data, vector<float>& recv_data) {
    // Implement the merge low function
    // This function should merge recv_data into local_data for lower half sorting
    int N = local_data.size();
    vector<float> lowdata(N);
    int j = 0, k = 0;
    for(int i = 0;i<N;i++){
        if(j<N && (k == N || local_data[j] < recv_data[k])){
            lowdata[i] = local_data[j];
            j++;
        } else{
            lowdata[i] = recv_data[k];
            k++;
        }
    }
    local_data = lowdata;
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

template <typename T>
void fill_random(vector<T>& vec,int seed){
    mt19937 generator(seed);
    uniform_real_distribution<T> distribution(-100.0, 100.0);

    uint64_t N = vec.size();
    for (uint64_t i = 0; i < N; i++) {
        vec[i] = distribution(generator);
    }
}
int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }
    string filename = argv[1];
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    MPI_Status status;

    // 假设float数组已经存在于文件中
    vector<float> data(128);
    int num_elements_per_proc = 0;
    
    // Rank 0 读取数据并分发
    if (world_rank == 0) {
        cout<<"====================================================="<<endl;
        cout<<"Hello from rank 0, your program started successfully!"<<endl;
        auto start = chrono::high_resolution_clock::now();
        data = read_floats_from_file(filename);
        if(data.empty()){
            cout<<"Data is empty!"<<endl;
            return 1;
        }
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> fill_time = end - start;
        // fill_random(data,42);
        num_elements_per_proc = data.size()/world_size;
        cout<<"File Loaded from "<<filename<<endl;
        cout<<"Load Time:"<<fill_time.count()<<" seconds." <<endl;
        cout<<"====================================================="<<endl;
    }

    MPI_Bcast(&num_elements_per_proc, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // 每个进程分配接收缓冲区

    vector<float> sub_data(num_elements_per_proc,0);
    vector<float> recv_data(num_elements_per_proc,0);
    // Scatter数据到所有进程

    chrono::duration<double> total_time(0);
    chrono::duration<double> scatter_time(0);
    chrono::duration<double> exchange_time(0);
    chrono::duration<double> gather_time(0);
    chrono::duration<double> communication_time(0);


    auto start = chrono::high_resolution_clock::now();
    auto scatter_start = chrono::high_resolution_clock::now();
    if(world_rank == 0){
        cout<<"start scatter data!"<<endl;
    }
    MPI_Scatter(data.data(), num_elements_per_proc, MPI_FLOAT, sub_data.data(), num_elements_per_proc, MPI_FLOAT, 0, MPI_COMM_WORLD);
    if(world_rank == 0){
        cout<<"local sort!"<<endl;
    }
    scatter_time = chrono::high_resolution_clock::now() - scatter_start;
    sort(sub_data.begin(), sub_data.end());

    vector<int> even_neighbor_choice = {1,-1};
    vector<int> odd_neighbor_choice = {-1,1};
    
    int even_neighbor = world_rank + even_neighbor_choice[world_rank % 2];
    int odd_neighbor = world_rank + odd_neighbor_choice[world_rank % 2];
    if(world_rank == 0){
        cout<<"let's merge!"<<endl;
    }
    for(int i = 0;i < world_size - 1;i++){
        // exchange data in each loop
        auto is_odd_cycle = i % 2;
        if (is_odd_cycle){
            if(world_rank == 0 || world_rank == world_size - 1){
                continue;
            }
            auto comu_start = chrono::high_resolution_clock::now();
            MPI_Sendrecv(sub_data.data(), num_elements_per_proc, MPI_FLOAT, odd_neighbor, 0,
                         recv_data.data(), num_elements_per_proc, MPI_FLOAT, odd_neighbor, 0,
                         MPI_COMM_WORLD, &status); 
            exchange_time += chrono::high_resolution_clock::now() - comu_start;
            
            if(world_rank % 2 == 0){
                MergeHigh(sub_data,recv_data);
            } else{
                MergeLow(sub_data,recv_data);
            }
        } else{
            auto comu_start = chrono::high_resolution_clock::now();
            MPI_Sendrecv(sub_data.data(), num_elements_per_proc, MPI_FLOAT, even_neighbor, 0,
                         recv_data.data(), num_elements_per_proc, MPI_FLOAT, even_neighbor, 0,
                         MPI_COMM_WORLD, &status);
            exchange_time += chrono::high_resolution_clock::now() - comu_start;
            if(world_rank % 2 == 0){
                MergeLow(sub_data,recv_data);
            } else{
                MergeHigh(sub_data,recv_data);
            }
        }
    }

    // Gather the sorted data
    if(world_rank == 0){
        cout<<"Try Gather!";
    }
    auto gather_start = chrono::high_resolution_clock::now();
    MPI_Gather(sub_data.data(), num_elements_per_proc, MPI_FLOAT, data.data(), num_elements_per_proc, MPI_FLOAT, 0, MPI_COMM_WORLD);
    auto end = chrono::high_resolution_clock::now();
    gather_time = end - gather_start;
    communication_time = gather_time + exchange_time + scatter_time;
    total_time = end - start;
    if(world_rank == 0){
        //check if the data is sorted
        for(size_t i = 0;i < data.size()-1 ;i++){
            if(data[i] > data[i+1]){
                cout<<"Data is not sorted!"<<endl;
                break;
            }
        }
        cout<<"Data is sorted!"<<endl;
        cout << "Total time: " << total_time.count() << " seconds" << endl;
        cout << "Scatter time: " << scatter_time.count() << " seconds" << endl;
        cout << "exchange time: " << exchange_time.count() << " seconds" << endl;
        cout << "Gather time: " << gather_time.count() << " seconds" << endl;
        cout << "Communication time: " << communication_time.count() << " seconds" << endl;
        cout << "Calculation time: "<< total_time.count() - communication_time.count() << " seconds" << endl;

    }

    MPI_Finalize();
    return 0;
}


