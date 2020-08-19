#include <iostream>
#include "test.h"
using namespace std;

extern void test2();



void test3()
{
    initrd_load();
}


int main()
{
    test2();
    test3();
//    double m ,n;
//    cin >> m >> n;
//    try {
//        cout << "before dividing." << endl;
//        if( n == 0)
//            throw -1; //抛出int类型异常
//        else
//            cout << m / n << endl;
//        cout << "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  after dividing." << endl;
//    }
//    catch(double d) {
//        cout << "catch(double) " << d <<  endl;
//    }
//    catch(int e) {
//        cout << "catch(int) " << e << endl;
//    }
    cout << "finished" << endl;
    return 0;
}
