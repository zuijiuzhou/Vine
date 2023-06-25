#include <iostream>
#include <array>
#include <ctime>
#include <vector>



class A{
public:
    int x;
    int y;
    int z;
    static size_t n;
    A(){
        x = 0;
        y = 0;
        z = 0;
        n++;
    }    
    A(const int& xx, const int& yy, const int& zz) : x(xx), y(yy), z(zz){
        // n++;
    }

    A(const A& a){
        x=a.x;
        y=a.y;
        z=a.z;
    }

    A& operator = (const A& a){
        x=a.x;
        y=a.y;
        z=a.z;
        return *this;
    }
};

size_t A::n = 0;

int main(int argc, char** argv){

    // std::array<A, 2> alist;
    // clock_t time_start, time_end;
    // size_t n = 119000000;




    // time_start = clock();
    // for(size_t i = 0; i < n; i++){
    //     alist[0].x=1;
    //     alist[1].x=1;
    //     alist[0].y=2;
    //     alist[1].y=2;
    //     alist[0].z=3;
    //     alist[1].z=3;
    // }
    // time_end = clock();
    // std::cout << (time_end - time_start)<< std::endl;
    // std::cout << "----" << A::n << "----\n";
   


    // time_start = clock();
    // for(size_t i = 0; i < n; i++){
    //     alist[0] = A(1,2,3);
    //     alist[1] = A(5,2,3);
    // }
    // time_end = clock(); 
    // std::cout << (time_end - time_start) << std::endl;
    // std::cout << "----" << A::n << "----\n";

    // const char* s1 = "中国";
    // const wchar_t* s2 = L"中国";
    // std::u16string s3 = u"中国";
    // std::u32string s4 = U"中国";
    // std::u8string s5 = u8"中国";

    std::vector<int> nums;

    std::cout << 1 << std::endl;
    auto i = nums.front();
    std::cout << i << std::endl;
    std::cout << 2 << std::endl;
    
    return 0;
}