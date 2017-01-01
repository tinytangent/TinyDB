//电脑中未安装boost库，故DiskStorageArea暂未能运行
//117448696——表头所需bit数
// 83894264——1bit代表32字节的区域的起始点
//	2047、838660、25165824、33554432:分别表示长度为8bit、4bit、2bit、1bit的数量
#include <cstdint>
#include <bitset>
#include <malloc.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <algorithm>
#include "AbstractDynamicAllocator.h"
using namespace std;
class BuddyDynamicAllocator : public AbstractDynamicAllocator
{
protected:
    AbstractStorageArea* storageArea;
public:
    //"Chunk" is a large area on the file managed by the buddy allocator.		
    //A chunk is usually in size like 512MB/1GB. Note that the max possible		
    //memory can be allocated is chunkSize / 2.		
    /*const int chunkSize;

    //The minimum size to alllocate each time		
    const int minOrderSize;

    //The max order		
    const int maxOrder;

    const int chunkHeaderSize;*/
public:
    int orderTable;
    BuddyDynamicAllocator(AbstractStorageArea* storageArea);
    bitset<117448696> *p = new bitset<117448696>();
    const AbstractStorageArea* getStorageArea() const;
    //void initialize();
    uint64_t bytesTotal();//未实现——Total默认1G？
    uint64_t bytesUsed();//未实现——在allocate处修改即可，然而不知道要实现的功能
    uint64_t bytesAvailable();
    uint64_t allocate(uint64_t size) override;
    bool free(uint64_t file_loc, uint64_t size);//不知道如何从accessProxy转换到在文件中的位置，请依据free(file_loc,size)实现即可

    bool initialize();
	uint64_t find_son_left(uint64_t father);//loc2loc
	uint64_t find_son_right(uint64_t father);//loc2loc
	uint64_t find_father(uint64_t son);//loc2loc7
	uint64_t find_brother(uint64_t loc);
	uint64_t compute_size(uint64_t loc);//loc2(size-in-pow-type)
	void set_size(uint64_t loc,uint64_t size);
	uint64_t no2loc(uint64_t number);
	uint64_t loc2no(uint64_t location);
	uint64_t loc2file(uint64_t loc);//表中位置到文件中的位置
	//bool free(uint64_t file_loc,uint64_t size);//释放文件中位于file_loc、大小为size的地址
	void update_size(uint64_t loc);
	//long long allocate(uint64_t size);//分配到文件的位置 分配不了返回-1
	//bool free(uint64_t start,uint64_t size);
};
