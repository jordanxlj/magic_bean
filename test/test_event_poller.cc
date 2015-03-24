#include <assert.h>

int glob[3] = {1, 2, 3};
int* g_p = glob;
int alloc(void)
{
    return *g_p++;
}   
int release(void)
{
    return *g_p--;
}   
void test_1(void)
{
    assert(1 == alloc());
    //release();
}   
void test_2(void)
{
    assert(1 == alloc());
    //release();
}   
int main()
{
    test_1();
    test_2();
    return 0;
}
