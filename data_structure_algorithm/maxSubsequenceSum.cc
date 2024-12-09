#include<iostream>

/*
 * 最大连续子序列和的问题。给定一个序列，寻找并标识最大的连续子序列。
 * 如果所有的整数都是负的，那么最大的连续子序列和是0.  
*/

// 暴力解法 复杂度为 n^3
int maxSubsequenceSum1(int a[], int size)
{
    int maxSum = 0;
    // 确定边界
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            // 对边界内的序列进行求和
            int tmpSum = 0;
            for(int k = i; k < j; k++)
            {
                tmpSum += a[k];
            }
            if(tmpSum > maxSum)
            {
                maxSum = tmpSum;
            }
        }
    }
    return maxSum;
}

int main()
{
    int a[] = {-1, 12, -5, 13, -6, 3};
    int result = 0;
    int size = sizeof(a) / sizeof(a[0]);
    result = maxSubsequenceSum1(a, size);
    std::cout << "最大的连续子串和为：" << result << std::endl;
    return 0;
}