# 使用官方 Ubuntu 镜像作为基础镜像
FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

# 安装所需的包，确保无需人工干预
RUN apt-get update && \
    apt-get install -y --no-install-recommends openmpi-bin openmpi-common libopenmpi-dev
# 编译 MPI 程序
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    openmpi-bin \
    openmpi-common \
    libopenmpi-dev \
    openssh-server \
    gcc \
    g++ \
    make \
    build-essential \
    vim 

# 设置 SSH 服务
ENV DEBIAN_FRONTEND=

# 创建 SSH 目录并生成 SSH 密钥
RUN mkdir /var/run/sshd 

# 允许 SSH 密钥登录
RUN echo 'PermitRootLogin yes' >> /etc/ssh/sshd_config && \
    echo 'PasswordAuthentication no' >> /etc/ssh/sshd_config

# 创建 MPI 用户和 SSH 文件夹
RUN useradd -m -s /bin/bash mpiuser && \
    mkdir /home/mpiuser/.ssh && \
    chown mpiuser:mpiuser /home/mpiuser/.ssh

RUN sed -i 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' /etc/pam.d/sshd

# 为 mpiuser 设置权限
COPY sshd_config /etc/ssh/sshd_config
RUN chown root:root /etc/ssh/sshd_config && \
chmod 644 /etc/ssh/sshd_config

COPY sshfiles /home/mpiuser/.ssh 
RUN chown mpiuser:mpiuser /home/mpiuser/.ssh -R && \
chmod 700 /home/mpiuser/.ssh && \
chmod 600 /home/mpiuser/.ssh/*

# 清除 DEBIAN_FRONTEND 环境变量
ENV DEBIAN_FRONTEND=

# 默认命令，启动 SSH 服务

ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]