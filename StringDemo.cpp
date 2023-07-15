// StringDemo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iomanip>
#include <iostream>
#include <algorithm>
#include "string.h"

void print(int id, string::size_type n, string const& s)
{
    std::cout << id << ") ";
    if (string::npos == n)
        std::cout << "not found! n == npos\n";
    else
        std::cout << "found @ n = " << n << ", substr(" << n << ") = "
        << std::quoted(s.substr(n).c_str()) << '\n';
}

int main()
{
    string *strHeap = new string("hello2");
    std::cout << strHeap->c_str() << std::endl;
    delete(strHeap);

    string str("hello1");
    const char& back = str.back();
    char& front = str.front();
    std::cout << back << std::endl;
    std::cout << front << std::endl;
    std::cout << str.c_str() << std::endl;

    string moveStr = std::move(str);
    std::cout << moveStr.c_str() << std::endl;
    std::cout << str.size() << std::endl;

    string str1;
    string str2{ "alpha" };

    // (1) operator=(const string&);
    str1 = str2;
    std::cout << std::quoted(str1.c_str()) << ' '   // "alpha"
        << std::quoted(str2.c_str()) << '\n'; // "alpha"

    // (2) operator=(string&&);
    str1 = std::move(str2);
    std::cout << std::quoted(str1.c_str()) << ' '   // "alpha"
        << std::quoted(str2.c_str()) << '\n'; // "" or "alpha" (unspecified)

    // (3) operator=(const CharT*);
    str1 = "beta";
    std::cout << std::quoted(str1.c_str()) << '\n'; // "beta"

    // (4) operator=(CharT);
    str1 = '!';
    std::cout << std::quoted(str1.c_str()) << '\n'; // "!"

    // (5) operator=(const T&);
    str1 = 35U; // equivalent to str1 = static_cast<char>(35U);
    std::cout << std::quoted(str1.c_str()) << '\n'; // "#" (ASCII = 35)

    string s;
    string const c("Exemplary");
    // assign(const string& str)
    s.assign(c);
    std::cout << c.c_str() << " == " << s.c_str() << '\n'; // "Exemplary == Exemplary"

    // assign(const string& str, size_type pos, size_type count)
    s.assign(c, 0, c.length() - 1);
    std::cout << s.c_str() << '\n'; // "Exemplar";

    // assign(string&& str)
    s.assign(string("C++ by ") + string("example"));
    std::cout << s.c_str() << '\n'; // "C++ by example"

    // assign(const CharT* s, size_type count)
    s.assign("C-style string", 7);
    std::cout << s.c_str() << '\n'; // "C-style"

    // assign(const CharT* s)
    s.assign("C-style\0string");
    std::cout << s.c_str() << '\n'; // "C-style"

    s = string("message"); // for capacity

    s = "abc";
    s.at(2) = 'x'; // OK
    std::cout << s.c_str() << '\n';

    std::cout << "string size = " << s.size() << '\n';
    std::cout << "string capacity = " << s.capacity() << '\n';

    //try
    //{
    //    // This will throw since the requested offset is greater than the current size.
    //    s.at(3) = 'x';
    //}
    //catch (std::out_of_range const& exc)
    //{
    //    std::cout << exc.what() << '\n';
    //}

    const string e("Exemplar");
    for (unsigned i = e.length() - 1; i != 0; i /= 2)
        std::cout << e[i];
    std::cout << '\n';

    const char* cc = &e[0];
    std::cout << cc << '\n'; // print as a C string

    // Change the last character of s into a 'y'
    string ss("Exemplar ");
    ss[ss.size() - 1] = 'y'; // equivalent to s.back() = 'y';
    std::cout << ss.c_str() << '\n';

    string sExemp("Exemplary");
    char& f1 = sExemp.front();
    f1 = 'e';
    std::cout << sExemp.c_str() << '\n'; // "exemplary"

    string const cExemp("Exemplary");
    char const& f2 = cExemp.front();
    std::cout << &f2 << '\n'; // "Exemplary"

    string sExempB("Exemplary");
    char& f1B = sExempB.back();
    f1B = 'e';
    std::cout << sExempB.c_str() << '\n'; // "exemplary"

    string const cExempB("Exemplary");
    char const& f2B = cExempB.back();
    std::cout << &f2B << '\n'; // "Exemplary"

    string const sData("Emplary");
    assert(sData.size() == std::strlen(sData.data()));
    assert('\0' == *(sData.data() + sData.size()));

    string sItr("Exemplar");
    *sItr.begin() = 'e';
    std::cout << sItr.c_str() << '\n';

    auto i = sItr.cbegin();
    std::cout << *i << '\n';
    //  *i = 'E'; // error: i is a constant iterator

    string sEnd("Exemparl");
    std::next_permutation(sEnd.begin(), sEnd.end());

    for (auto itr = sEnd.begin(); itr != sEnd.end(); ++itr)
    {
        std::cout << *itr << '\n'; // "Exemplar"
    }
    
    string sFit;
    std::cout << "Size of string is " << sizeof sFit << " bytes\n"
        << "Default-constructed capacity is " << sFit.capacity()
        << " and size is " << sFit.size() << '\n';

    for (int i = 0; i < 42; i++)
        sFit.append(" 42 ");
    std::cout << "Capacity after 42 appends is " << sFit.capacity()
        << " and size is " << sFit.size() << '\n';

    sFit.clear();
    std::cout << "Capacity after clear() is " << sFit.capacity()
        << " and size is " << sFit.size() << '\n';

    string sErase = "This Is An Example";
    std::cout << "1) " << sErase.c_str() << '\n';

    sErase.erase(7, 3); // erases " An" using overload (1)
    std::cout << "2) " << sErase.c_str() << '\n';

    sErase.erase(std::find(sErase.begin(), sErase.end(), ' ')); // erases first ' '; overload (2)
    std::cout << "3) " << sErase.c_str() << '\n';

    string strPushBack{ "Short string" };
    std::cout << "1) " << std::quoted(strPushBack.c_str()) << ", size: " << strPushBack.size() << '\n';

    strPushBack.push_back('!');
    std::cout << "2) " << std::quoted(strPushBack.c_str()) << ", size: " << strPushBack.size() << '\n';

    string strPushBackAnother;
    strPushBackAnother.push_back('!');
    std::cout << "3) " << std::quoted(strPushBackAnother.c_str()) << ", size: " << strPushBackAnother.size() << '\n';

    string a = "0123456789abcdefghij";

    // count is npos, returns [pos, size())
    string sub1 = a.substr(10);
    std::cout << sub1.c_str() << '\n';

    // both pos and pos + count are within bounds, returns [pos, pos + count)
    string sub2 = a.substr(5, 3);
    std::cout << sub2.c_str() << '\n';

    // pos is within bounds, pos + count is not, returns [pos, size())
    string sub4 = a.substr(a.size() - 3, 50);
    // this is effectively equivalent to
    // string sub4 = a.substr(17, 3);
    // since a.size() == 20, pos == a.size() - 3 == 17, and a.size() - pos == 3

    std::cout << sub4.c_str() << '\n';

    //try
    //{
    //    // pos is out of bounds, throws
    //    string sub5 = a.substr(a.size() + 3, 50);
    //    std::cout << sub5.c_str() << '\n';
    //}
    //catch (const std::out_of_range& e)
    //{
    //    std::cout << "pos exceeds string size\n";
    //}


    const unsigned desired_length{ 8 };
    string long_string("Where is the end?");
    string short_string("H");

    std::cout << "Basic functionality:\n"
        << "Shorten:\n"
        << "1. Before: " << std::quoted(long_string.c_str()) << '\n';
    long_string.resize(desired_length);
    std::cout << "2. After:  " << std::quoted(long_string.c_str()) << '\n';

    std::cout << "Lengthen with a given value 'a':\n"
        << "3. Before: " << std::quoted(short_string.c_str()) << '\n';
    short_string.resize(desired_length, 'a');
    std::cout << "4. After:  " << std::quoted(short_string.c_str()) << '\n';

    std::cout << "Lengthen with char() == " << static_cast<int>(char()) << '\n'
        << "5. Before: " << std::quoted(short_string.c_str()) << '\n';
    short_string.resize(desired_length + 3);
    std::cout << "6. After:  \"";
    for (char c : short_string)
        std::cout << (c == char() ? '@' : c);
    std::cout << "\"\n\n";

    string::size_type n;
    string const sFind = "This is a string"; /*
                             ^  ^  ^
                             1  2  3          */

                             // search from beginning of string
    n = sFind.find("is");
    print(1, n, sFind);

    // search from position 5
    n = sFind.find("is", 5);
    print(2, n, sFind);

    // find a single character
    n = sFind.find("a");
    print(3, n, sFind);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
