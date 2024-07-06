### 项目简介

这个项目展示了一个使用 MPI（消息传递接口）进行并行计算的示例程序。项目通过 Docker 容器来配置和运行多个 MPI 进程，利用 OpenMPI 库进行通信和计算。本项目主要实现了一个并行的排序算法，并测量了整个过程中各个阶段的时间消耗。

### 依赖

- Docker
- Docker Compose

### 环境搭建

- `docker-compose.yml`：Docker Compose 文件，用于定义和配置多个 MPI 容器。
- `Dockerfile`：用于构建 MPI 容器的 Docker 镜像。
- `sshd_config` 和 `sshfiles`：用于配置 SSH 服务，以支持 MPI 进程间的通信。



### 设置和运行

1. 克隆项目

2. 在项目根目录下运行以下命令来构建 Docker 镜像：
    ``` bash
    docker-compose build -t mpi .
    ```
3. 使用 Docker Compose 启动多个 MPI 容器：
    ```bash
    docker-compose up -d
    ```
4. 进入 `mpi9` 容器并切换到 `mpiuser` 用户，然后测试 SSH 连接到其他容器。这个步骤确保所有节点之间可以通过 SSH 免密登录。如果无法完成这一步，不要进入下一步：
    ```bash
    # 进入 mpi9 容器
    user@hostmachine:$ docker exec -it mpi9 /bin/bash

    # 切换到 mpiuser 用户
    root@mpi9:$ su mpiuser

    # 测试 SSH 连接
    mpiuser@mpi9:$ ssh mpiuser@mpi1

    Are you sure you want to continue connecting (yes/no)? yes
    mpiuser@mpi1:$
    ```
5. 可以用mpi编译运行相关程序
    ```
    # 编译 MPI 程序
    mpiuser@mpi9:$ mpic++ -o mpi_sort main.cpp

    # 运行 MPI 程序
    mpiuser@mpi9:$ mpirun -np 32 --hostfile /home/mpiuser/executables/hostfile /home/mpiuser/executables/mpi_sort /path/to/your/datafile

    ```
