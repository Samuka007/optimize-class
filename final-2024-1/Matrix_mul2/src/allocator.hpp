#pragma once

#include <cstdlib>
// #include <iostream>
// #include <memory>
#include <limits>
#include <cstddef>
#include <new>
// #include <stdexcept>
// #include <cstdint>
#include <immintrin.h> // For _mm_malloc and _mm_free

template <typename T>
class Allocator32 {
public:
    typedef T value_type;

    constexpr static std::size_t Alignment = 32;

    Allocator32() noexcept = default;

    template <typename U>
    Allocator32(const Allocator32<U>&) noexcept {}

    [[nodiscard]] T* allocate(std::size_t n) {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
            throw std::bad_alloc();
        void* ptr = _mm_malloc(n * sizeof(T), Alignment);
        if (!ptr)
            throw std::bad_alloc();
        return static_cast<T*>(ptr);
    }

    void deallocate(T* p, std::size_t) noexcept {
        _mm_free(p);
    }
};

template <typename T, typename U, std::size_t Alignment>
bool operator==(const Allocator32<T>&, const Allocator32<U>&) noexcept {
    return true;
}

template <typename T, typename U, std::size_t Alignment>
bool operator!=(const Allocator32<T>&, const Allocator32<U>&) noexcept {
    return false;
}

// template <typename T>
// class Allocator64 {
// public:
//     typedef T value_type;

//     constexpr static std::size_t Alignment = 64;

//     Allocator64() noexcept = default;

//     template <typename U>
//     Allocator64(const Allocator32<U>&) noexcept {}

//     [[nodiscard]] T* allocate(std::size_t n) {
//         if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
//             throw std::bad_alloc();
//         void* ptr = _mm_malloc(n * sizeof(T), Alignment);
//         if (!ptr)
//             throw std::bad_alloc();
//         return static_cast<T*>(ptr);
//     }

//     void deallocate(T* p, std::size_t) noexcept {
//         _mm_free(p);
//     }
// };

// template <typename T, typename U, std::size_t Alignment>
// bool operator==(const Allocator64<T>&, const Allocator64<U>&) noexcept {
//     return true;
// }

// template <typename T, typename U, std::size_t Alignment>
// bool operator!=(const Allocator64<T>&, const Allocator64<U>&) noexcept {
//     return false;
// }

// // Function to check if a pointer is 32-byte aligned
// inline bool is_aligned(const void* ptr, std::size_t alignment) {
//     return reinterpret_cast<std::uintptr_t>(ptr) % alignment == 0;
// }

// template<class T>
// struct Mallocator
// {
//     typedef T value_type;
 
//     Mallocator() = default;
 
//     template<class U>
//     constexpr Mallocator(const Mallocator <U>&) noexcept {}
 
//     [[nodiscard]] T* allocate(std::size_t n)
//     {
//         if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
//             // throw std::bad_alloc();
//             exit(EXIT_FAILURE);
 
//         if (auto p = static_cast<T*>(std::malloc(n * sizeof(T))))
//         {
//             report(p, n);
//             return p;
//         }
 
//         // throw std::bad_alloc();
//         exit(EXIT_FAILURE);
//     }
 
//     void deallocate(T* p, std::size_t n) noexcept
//     {
//         report(p, n, 0);
//         std::free(p);
//     }
// private:
//     void report(T* p, std::size_t n, bool alloc = true) const
//     {
//         std::cout << (alloc ? "Alloc: " : "Dealloc: ") << sizeof(T) * n
//                   << " bytes at " << std::hex << std::showbase
//                   << reinterpret_cast<void*>(p) << std::dec << '\n';
//     }
// };
 
// template<class T, class U>
// bool operator==(const Mallocator <T>&, const Mallocator <U>&) { return true; }
 
// template<class T, class U>
// bool operator!=(const Mallocator <T>&, const Mallocator <U>&) { return false; }