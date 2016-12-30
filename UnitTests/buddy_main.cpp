void test_buddy(int argc, char* argv[])
{
    CachedStorageArea storageArea("D:\\r2.dat", 16 * 1024 * 1024, 8192);
    auto dynamicAllocator = new BuddyDynamicAllocator(&storageArea);
    dynamicAllocator->initialize();
    cout << "ok\n";
    srand(123);
    int size_sum = 0, size_now = 0;
    uint64_t *address = new uint64_t[100000];
    int *size = new int[100000];
    for (int i = 0; i < 100000; i++)
    {
        size[i] = ((rand() % 10000) / 4) * 4;
        address[i] = dynamicAllocator->allocate(size[i]);
        size_sum += size[i];
    }
    cout << size_sum << endl;
    cout << address[100000 - 1] << endl;
    for (int i = 0; i < 100000; i++)
    {
        int s = size[i];
        for (int j = 0; j < s; j += 4)
        {
            storageArea.setDataAt(address[i] + j, (char*)&i, 4);
        }
    }
    for (int i = 0; i < 100000; i++)
    {
        int temp;
        int s = size[i];
        for (int j = 0; j < s; j += 4)
        {
            storageArea.getDataAt(address[i] + j, (char*)&temp, 4);
            if (temp != i)
                std::cout << "Error" << std::endl;
        }
    }
    for (int i = 0; i < 100000; i++)
    {
        dynamicAllocator->free(address[i], size[i]);
    }
    bitset<117448696> *p2 = new bitset<117448696>();
    *p2 = *dynamicAllocator->p;
    dynamicAllocator->initialize();
    if (*dynamicAllocator->p == *p2)
    {
        std::cout << "OK!" << std::endl;
    }
}
