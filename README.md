# SimdCEP
SimdCEP complex event matching algorithm. 
Welcome to SimdCEP complex event matching algorithm. 

File:
The event streams and queries that serve as input examples are stored in the "data" and "query" folders respectively, located at the same level directory.The detailed matching results are stored in the "result" folder, which is located at the same level directory,too.
The configuration properties of SimdCEP are set in the document config.txt.The specific content is shown below:
The first line and the second line are respectively the relative addresses of the input query and event stream.
The third line is the number of times the program is executed in a loop, which facilitates measuring the average running time.
The fourth line is the size of the time slice, with a default unit of seconds. It can be adjusted based on the specific dataset.
The last line is the relative address for saving detailed matching results.

Explanation:
This version is a C++ code based on the Linux system, used to implement the complex event matching function based on 512-bit SIMD functions, and is built using the CMake.

Operating environment requirements:
1.It needs to be run in a Linux environment.
2.The CMake environment has been installed and configured.
3.The CPU must support the AVX-512 instruction set (this can be verified by the command "lscpu | grep avx512").

Operating steps:
We have provided a run.sh file to assist you in quickly running it. Before using it, you need to modify the paths in this file.
Enter in the command line:
sed -i 's/\r$//' run.sh  (If it is the first time to run)
chmod +x run.sh (Grant permissions)
./run.sh (run)
