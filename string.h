#pragma once
#include <cassert>
#include <memory>
#include <iterator>

//template<class CharT = char> class char_traits;

template< typename CharT = char, typename Allocator = ::std::allocator<CharT>>
class stringT {
public:
	// Member types
	typedef CharT value_type;
	typedef Allocator allocator_type;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef ptrdiff_t difference_type;
	typedef size_t size_type;

    // Iterators
    template<typename T>
    class Const_Iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = typename T::value_type;
        using reference = const value_type&;
        using pointer = typename T::const_pointer;
        using difference_type = typename T::difference_type;
    private:
        pointer m_ptr;
    public:

        Const_Iterator() : m_ptr(nullptr) {}
        Const_Iterator(pointer ptr) : m_ptr(ptr) {}
        Const_Iterator(const Const_Iterator& i) : m_ptr(i.m_ptr) {}

        inline reference operator*() const { return *m_ptr; }
        inline pointer operator->() const { std::pointer_traits<pointer>::pointer_to(**this); }
        inline reference operator[](difference_type rhs) { return m_ptr[rhs]; }

        inline Const_Iterator& operator+=(difference_type rhs) { m_ptr += rhs; return *this; }
        inline Const_Iterator& operator++() { m_ptr++; return *this; }
        inline Const_Iterator operator++(int) { Const_Iterator tmp(*this); ++m_ptr; return tmp; }
        inline Const_Iterator& operator-=(difference_type rhs) { m_ptr -= rhs; return *this; }
        inline Const_Iterator& operator--() { m_ptr--; return *this; }
        inline Const_Iterator operator--(int) { Const_Iterator tmp(*this); --(*this); return tmp; }

        inline difference_type operator-(const Const_Iterator& rhs) const { return m_ptr - rhs.m_ptr; }
        inline Const_Iterator operator-(difference_type rhs) const { Const_Iterator tmp(*this); return tmp -= rhs; }
        inline Const_Iterator operator+(difference_type rhs) const { Const_Iterator tmp(*this); return tmp += rhs; }
        friend inline Const_Iterator operator+(difference_type lhs, Const_Iterator<T> rhs) { return rhs += lhs; }

        inline bool operator== (const Const_Iterator& rhs) const { return m_ptr == rhs.m_ptr; };
        inline bool operator!= (const Const_Iterator& rhs) const { return m_ptr != rhs.m_ptr; };
        inline bool operator< (const Const_Iterator& rhs) const { return m_ptr < rhs.m_ptr; };
        inline bool operator<= (const Const_Iterator& rhs) const { return m_ptr <= rhs.m_ptr; };
        inline bool operator> (const Const_Iterator& rhs) const { return m_ptr > rhs.m_ptr; };
        inline bool operator>= (const Const_Iterator& rhs) const { return m_ptr >= rhs.m_ptr; };

    };

    template<typename T>
    class Iterator : public Const_Iterator<T> {
    public:
        using super = Const_Iterator<T>;
        using iterator_category = std::random_access_iterator_tag;
        using value_type = typename T::value_type;
        using reference = value_type&;
        using pointer = typename T::pointer;
        using difference_type = typename T::difference_type;

        using super::super;

        inline reference operator*() { return const_cast<reference>(super::operator*()); }
        inline pointer operator->() { std::pointer_traits<pointer>::pointer_to(**this); }
        inline reference operator[](difference_type diff) { return const_cast<reference>(super::operator[](diff)); }

        inline Iterator& operator+=(difference_type rhs) { super::operator+=(rhs); return *this; }
        inline Iterator& operator++() { super::operator++(); return *this; }
        inline Iterator operator++(int) { Iterator tmp(*this); super::operator++(); return tmp; }
        inline Iterator& operator-=(difference_type rhs) { super::operator-=(rhs); return *this; }
        inline Iterator& operator--() { super::operator--(); return *this; }
        inline Iterator operator--(int) { Iterator tmp(*this); super::operator--(); return tmp; }

        using super::operator-;
        inline Iterator operator-(difference_type rhs) const { Iterator tmp(*this); return tmp -= rhs; }
        inline Iterator operator+(difference_type rhs) const { Iterator tmp(*this); return tmp += rhs; }
        friend inline Iterator operator+(difference_type lhs, Iterator<T> rhs) { return rhs += lhs; }

        using super::operator==;
        using super::operator!=;
        using super::operator<=;
        using super::operator>=;
        using super::operator<;
        using super::operator>;
    };

    typedef Const_Iterator<stringT> const_iterator;
    typedef Iterator<stringT> iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;

private:
    // not including '\0'
    size_type m_size = 0;
    pointer m_ptr;
    allocator_type m_allocator;
    size_type m_allocated = 0;

    constexpr inline void alloc(size_type b) {
        // TODO:use a separate buffer array on stack
        //      and not allocate memory on heap. Read
        //      from stack is much faster and this will
        //      help improve performance of string
        //      intensive applications.
        
        // Allocation is done more than twice the requested
        // size to keep performance of append to ammortized O(1)
        m_allocated = (size_t)(2 * b + 1);
        m_ptr = m_allocator.allocate(m_allocated);
    }

    constexpr void free(pointer p, size_type oldSize) {
        m_allocator.deallocate(p, oldSize);
        p = nullptr;
        m_allocated = 0;
    }

    constexpr void free(pointer p, size_type oldSize) const {
        m_allocator.deallocate(p, oldSize);
        m_allocated = 0;
    }

    constexpr inline void allocateIfNeeded(size_type newSize) {
        // Don't waste an allocation if the memory already allocated is large enough to hold the new data
        if (m_allocated > newSize) {
            return;
        }
        // Free old allocated memory
        free(m_ptr, m_allocated);
        // Allocate memory. Plus 1 for null terminator
        alloc(newSize + 1);
    }

    constexpr inline int _compare(const_pointer a, size_type alen, const_pointer b, size_type blen) const noexcept {
        const size_type count = std::min(alen, blen);
        int res = memcmp(a, b, count);
        return res ? res : (int)(alen - blen);
    }

public:
    static constexpr size_type npos = size_type(-1);

    stringT(std::nullptr_t): m_size(0) {
        m_ptr = nullptr;
    }

    constexpr inline stringT() : m_size(0) {
        m_ptr = nullptr;
    }

    constexpr stringT(size_type count, value_type c) : m_size(count) {
        alloc(m_size + 1);
        ::memset((void*)m_ptr, c, m_size);
        m_ptr[m_size] = '\0';
    }

    explicit constexpr inline stringT(const value_type c) : m_size(1) {
        stringT(m_size, c);
    }

    constexpr stringT(const stringT& str, size_type pos = 0) {
        m_size = str.m_size - pos;
        alloc(m_size);
        memcpy(m_ptr, str.m_ptr + pos, m_size + 1);
    }

    constexpr stringT(const stringT& str, size_type pos, size_type count) {
        m_size = (count == npos || pos + count >= str.size()) ? str.size() - pos : count;
        alloc(m_size + 1);
        memcpy(m_ptr, str.m_ptr + pos, m_size + 1);
        m_ptr[m_size] = '\0';
    }

    constexpr stringT(const_pointer s) : m_size(::strlen(s)) {
        alloc(m_size + 1);
        memcpy(m_ptr, s, m_size + 1);
        m_ptr[m_size] = '\0';
    }

    constexpr stringT(const_pointer s, size_type count) : m_size(count) {
        alloc(m_size + 1);
        memcpy(m_ptr, s, m_size);
        m_ptr[m_size] = '\0';
    }

    constexpr stringT(const_pointer s, size_type pos, size_type count) : m_size(count)
    {
        alloc(m_size + 1);
        memcpy(m_ptr, s + pos, m_size);
        m_ptr[m_size] = '\0';
    }

    // Move constructor
    constexpr stringT(stringT&& str) noexcept {
        swap(str);
    }

    ~stringT() {
        if(m_ptr != nullptr && m_allocated > 0)
            free(m_ptr, m_allocated);
    }

    constexpr stringT& operator=(const stringT& str) {
        // If copying from same string just return this.
        // No need to perform copy.
        if (&str == this) {
            return *this;
        }
        // TODO:Need to spend more time here
        //      should be enough for now
        m_size = str.size();
        allocateIfNeeded(m_size);
        memcpy(m_ptr, str.m_ptr, m_size + 1);
        return *this;
    }

    constexpr stringT& operator=(stringT&& str) {
        swap(str);
        str.clear();
        return *this;
    }

    constexpr stringT& operator=(const_pointer s) {
        m_size = ::strlen(s);
        allocateIfNeeded(m_size);
        // Clone the other value
        memcpy(m_ptr, s, m_size + 1);
        return *this;
    }

    constexpr stringT& operator=(const value_type c) {
        free(m_ptr, m_allocated);
        m_size = 1;
        alloc(m_size + 1);
        m_ptr[0] = c;
        m_ptr[1] = '\0';
        return *this;
    }

    constexpr const_pointer c_str() const noexcept {
        return m_ptr;
    }

    constexpr reference operator[](size_type x) {
        // Check for index out of bounds
        assert(x < m_size && x >= 0);
        return m_ptr[x];
    }

    constexpr const_reference operator[](size_type x) const {
        // Check for index out of bounds
        assert(x < m_size && x >= 0);
        return m_ptr[x];
    }

    constexpr const_reference at(size_type x) const {
        // Check for index out of bounds
        assert(x < m_size && x >= 0);
        return m_ptr[x];
    }

    constexpr reference at(size_type x) {
        // Check for index out of bounds
        assert(x < m_size && x >= 0);
        return m_ptr[x];
    }

    constexpr const_reference front() const {
        // Assert if empty string
        assert(m_ptr);
        return m_ptr[0];
    }

    constexpr reference front() {
        // Assert if empty string
        assert(m_ptr);
        return m_ptr[0];
    }

    constexpr const_reference back() const {
        // Assert if empty string
        assert(m_ptr);
        return m_ptr[m_size - 1];
    }

    constexpr reference back() {
        // Assert if empty string
        assert(m_ptr);
        return m_ptr[m_size - 1];
    }

    constexpr void clear() {
        m_size = 0;
        *m_ptr = '\0';
    }

    constexpr stringT& assign(const stringT& str, size_type pos = 0, size_type count = npos) {
        size_type copy_len = (count == npos || pos + count >= str.size()) ? str.size() - pos : count;
        if (pos == 0 && copy_len == str.size()) {
            return (*this) = str;
        }
        m_size = copy_len;
        // free and/or allocate memory if necessary. 
        allocateIfNeeded(m_size);
        memcpy(m_ptr, str.m_ptr + pos, m_size);
        data()[m_size] = '\0';
        return *this;
    }

    constexpr stringT& assign(const_pointer s, size_type count) {
        m_size = count;
        // free and/or allocate memory if necessary. 
        allocateIfNeeded(m_size);
        memcpy(m_ptr, s, m_size);
        m_ptr[m_size] = '\0';
        return *this;
    }

    constexpr stringT& assign(const_pointer s) {
        return (*this = s);
    }

    constexpr stringT& assign(const stringT&& str) {
        return (*this) = str;
    }

    constexpr allocator_type get_allocator() {
        return m_allocator;
    }

    constexpr const_pointer data() const noexcept {
        return m_ptr;
    }

    constexpr pointer data() noexcept {
        return m_ptr;
    }

    constexpr iterator begin() {
        return iterator(m_ptr);
    }

    constexpr iterator end() {
        return iterator(m_ptr + m_size);
    }

    constexpr const_iterator cbegin() {
        return const_iterator(m_ptr);
    }

    constexpr const_iterator cend() {
        return const_iterator(m_ptr + m_size);
    }

    // Not making reverse iterators constexpr for 
    // backward compatibility with std::reverse_iterator
    reverse_iterator rbegin() {
        return reverse_iterator(m_ptr + m_size);
    }

    reverse_iterator rend() {
        return reverse_iterator(m_ptr);
    }

    const_reverse_iterator  crbegin() {
        return const_reverse_iterator(m_ptr + m_size);
    }

    const_reverse_iterator  crend() {
        return const_reverse_iterator(m_ptr);
    }

    constexpr size_type size() const {
        return m_size;
    }

    constexpr size_type length() const {
        return m_size;
    }

    constexpr size_type capacity() const {
        return m_allocated;
    }

    constexpr bool empty() const {
        return (m_size == 0);
    }

    constexpr void reserve(size_type newSize = 0) {
        if (newSize > capacity()) {
            pointer old = m_ptr;
            size_t oldSize = capacity();
            pointer newPtr = m_allocator.allocate(newSize + 1);
            memcpy(newPtr, old, m_size + 1);
            m_ptr = newPtr;
            free(old, oldSize);
        }
    }

    constexpr stringT& insert(size_type pos, const stringT& str) {
        if (pos == m_size) {
            return append(str);
        }
        return replace(pos, 0, str);
    }

    constexpr stringT& replace(size_type pos, size_type count, const stringT& str) {
        if (pos == m_size) {
            return append(str.data(), str.m_size);
        }
        return replace(pos, count, str.data(), str.m_size);
    }

    constexpr size_type max_size() const {
        return size_type(-1);
    }

    constexpr stringT& replace(size_type pos, size_type count, const_pointer s, size_type count2) {
        if (pos == m_size) {
            return append(s, count2);
        }

        // clamp count 
        if (pos + count > m_size) count = m_size - pos;
        long sizeDiff = (long)(count2 - count);

        assert(pos <= m_size && max_size() >= m_size + sizeDiff);
        if (sizeDiff > 0) { // count < count2 -> insert
            size_type newSize = m_size + sizeDiff + 1;
            allocateIfNeeded(newSize);
            memcpy(data() + pos, s, count2);
            m_size += sizeDiff;
        }
        else if (sizeDiff < 0) { // count > count2 
            memcpy(data() + pos, s, count2);
            memmove(data() + pos + count2, data() + pos + count, m_size - pos - count + 1);
            m_size += sizeDiff;
        }
        else {
            memcpy(data() + pos, s, count2);
        }
        return (*this);
    }

    constexpr void resize(size_type count, value_type c = '\0') {
        if (count < m_size) {
            m_ptr[m_size = count] = '\0';
        }
        else if (count > m_size) {
            append(count - m_size, c);
        }
    }

    constexpr stringT& append(size_type count, value_type c) {
        allocateIfNeeded(m_size + count + 1);
        ::memset(m_ptr + m_size, c, count);
        m_ptr[m_size += count] = '\0';
        return *this;
    }

    constexpr stringT& append(const stringT& str) {
        return (*this += str);
    }

    constexpr size_type copy(pointer dest, size_type count, size_type pos = 0) const {
        size_type cpyCount = pos + count > m_size ? m_size - pos : count;
        // the resulting string of copy is not null terminated
        memcpy(dest, m_ptr + pos, cpyCount);
        return cpyCount;
    }

    constexpr stringT& erase(size_type pos = 0, size_type count = npos) {
        if ((count == npos) || (pos + count > m_size)) {
            count = m_size - pos;
        }

        if ((pos == 0) && (count == m_size)) {
            // Optimize erasing the entire string
            clear();
        }
        else if (count > 0) {
            // move [old + pos + count, old + m_length] up by count
            memmove(data() + pos, data() + pos + count, m_size - (pos + count) + 1);
            m_size -= count;
        }

        return *this;
    }

    constexpr iterator erase(iterator pos) {
        size_type n = pos - data();
        erase(n, 1);
        return iterator(data() + n);
    }

    constexpr stringT operator+(const stringT& rhs) {
        stringT result;
        result.m_size = (*this).m_size + rhs.m_size;
        result.alloc(result.m_size + 1);
        // Copy lhs first
        memcpy(result.data(), (*this).m_ptr, (*this).m_size);
        // Add rhs
        memcpy(result.data() + (*this).m_size, rhs.data(), rhs.m_size + 1);
        return result;
    }

    constexpr stringT& operator+=(const stringT& str) {
        allocateIfNeeded(m_size + str.m_size + 1);
        memcpy(m_ptr + m_size, str.data(), str.m_size + 1);
        m_size += str.m_size;
        return *this;
    }

    constexpr stringT& operator+=(const value_type c) {
        // +1 for c, +1 for null operator
        allocateIfNeeded(m_size + 2);
        m_ptr[m_size] = c;
        m_ptr[++m_size] = '\0';
        return *this;
    }

    constexpr void push_back(value_type c) {
        *this += c;
    }

    constexpr void pop_back() {
        m_ptr[--m_size] = '\0';
    }

    constexpr void swap(stringT& str) {
        std::swap<Allocator>(m_allocator, str.m_allocator);
        std::swap<size_type>(m_size, str.m_size);
        std::swap<pointer>(m_ptr, str.m_ptr);
    }

    constexpr stringT substr(size_type pos, size_type count = npos) const {
        // Assert if index out of bound
        assert(pos < m_size);
        const size_type slen = std::min(m_size - pos, count);

        if (slen == 0) return stringT();
        else return stringT(m_ptr + pos, slen);
    }

    constexpr size_type find(const stringT& str, size_type pos = 0) const {
        return find(str.data(), pos, str.m_size);
    }

    constexpr size_type find(const_pointer s, size_type pos, size_type count) const
    {
        if (pos + count > m_size) return npos;
        if (count == 0) return pos;

        const_pointer pFound = static_cast<const_pointer>(memchr(m_ptr + pos, *s, m_size - pos));
        size_type i = static_cast<size_type>(pFound - m_ptr);

        while (pFound && (i + count) <= m_size) {
            if (memcmp(pFound, s, count) == 0) {
                return i;
            }
            pFound = static_cast<const_pointer>(memchr(pFound + 1, *s, m_size - i - 1));
            i = static_cast<size_type>(pFound - m_ptr);
        }
        return npos;
    }

    constexpr int compare(const stringT& str) const {
        if (m_ptr == str.data() && m_size == str.m_size) {
            return 0;
        }
        else {
            return _compare(m_ptr, m_size, str.data(), str.m_size);
        }
    }

    constexpr inline bool operator==(const stringT& str) const {
        return ((m_size == str.m_size) && (m_ptr == str.data())) || !_compare(m_ptr, m_size, str.data(), str.m_size);
    }

    constexpr inline bool operator!=(const stringT& s) const {
        return !(*this == s);
    }

    constexpr bool operator>(const stringT& s) const {
        return compare(s) > 0;
    }

    constexpr bool operator<(const stringT& s) const {
        return compare(s) < 0;
    }

    constexpr bool operator>=(const stringT& s) const {
        return compare(s) >= 0;
    }

    constexpr bool operator<=(const stringT& s) const {
        return compare(s) <= 0;
    }

};

// This might not be needed for class type variables post C++17.
// However, pointer class type would need it.
typedef stringT<> string;