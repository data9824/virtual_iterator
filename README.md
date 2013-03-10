virtual_iterator
================

Iterators that enables runtime polymorphism

*virtual_bidirectional_iterator< T, Ref, Ptr, Dist >
*virtual_forward_iterator< T, Ref, Ptr, Dist >
*virtual_input_iterator< T, Ref, Ptr, Dist >
*virtual_output_iterator< T >
*virtual_random_access_iterator< T, Ref, Ptr, Dist >

## Example code

    // This function can accept any kind of random access iterators for the int type
    void your_function(virtual_random_access_iterator<int> first, virtual_random_access_iterator<int> last)
    {
        std::sort(first, last); // or whatever you want
    }

    int main() {
        std::vector<int> v(100);
        int *a = new int[100];
        // ...
        your_function(v.begin(), v.end());
        your_function(a, a + 100);
        // ...
    }

## Tested under

*Microsoft(R) 32-bit C/C++ Optimizing Compiler Version 16.00.40219.01 for 80x86
*Borland C++ 5.5.1 for Win32
