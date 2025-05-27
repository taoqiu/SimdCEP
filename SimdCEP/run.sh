#!/bin/bash

# BitCEP 项目自动化构建和运行脚本

# 项目配置
PROJECT_DIR="/home/dell/jsw/SimdCEP"
BUILD_DIR="${PROJECT_DIR}/build"
EXECUTABLE="${BUILD_DIR}/src"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 检查并创建构建目录
prepare_build() {
    echo -e "${YELLOW}===== 准备构建环境 =====${NC}"
    if [ ! -d "${PROJECT_DIR}" ]; then
        echo -e "${RED}错误: 项目目录不存在: ${PROJECT_DIR}${NC}"
        exit 1
    fi
    
    mkdir -p "${BUILD_DIR}"
    echo -e "${GREEN}构建目录已准备: ${BUILD_DIR}${NC}"
}

# 构建项目
build_project() {
    echo -e "\n${YELLOW}===== 开始构建项目 =====${NC}"
    cd "${BUILD_DIR}" || exit 1
    
    echo -e "${GREEN}生成构建系统...${NC}"
    cmake .. || {
        echo -e "${RED}CMake 配置失败!${NC}"
        exit 1
    }
    
    echo -e "\n${GREEN}编译项目 (使用 $(nproc) 线程)...${NC}"
    make -j$(nproc) || {
        echo -e "${RED}编译失败!${NC}"
        exit 1
    }
    
    if [ ! -f "${EXECUTABLE}" ]; then
        echo -e "${RED}错误: 可执行文件未生成: ${EXECUTABLE}${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}项目构建成功!${NC}"
}

# 运行程序
run_program() {
    echo -e "\n${YELLOW}===== 运行程序 =====${NC}"
    cd "${PROJECT_DIR}" || exit 1
    
    if [ ! -f "${EXECUTABLE}" ]; then
        echo -e "${RED}错误: 可执行文件不存在: ${EXECUTABLE}${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}执行: ${EXECUTABLE}${NC}"
    "${EXECUTABLE}"
    
    # 检查程序退出状态
    if [ $? -ne 0 ]; then
        echo -e "${RED}程序执行失败!${NC}"
        exit 1
    else
        echo -e "${GREEN}程序执行完成!${NC}"
    fi
}

# 主函数
main() {
    prepare_build
    build_project
    run_program
}

# 执行主函数
main
