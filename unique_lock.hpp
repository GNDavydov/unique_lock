// Copyright 2021 GNDavydov

#ifndef INCLUDE_UNIQUE_LOCK_HPP_
#define INCLUDE_UNIQUE_LOCK_HPP_

#include <mutex>

namespace my_unique_lock {
    template<class Mutex>
    class unique_lock {
    private:
        Mutex *mutex_;
        bool owns_;

    public:
        unique_lock(const unique_lock &) = delete;

        unique_lock &operator=(const unique_lock &) = delete;

        unique_lock() noexcept: mutex_(nullptr), owns_(false) {}

        unique_lock(unique_lock &&other) noexcept: mutex_(other.mutex_), owns_(other.owns_) {
            other.mutex_ = nullptr;
            other.owns_ = false;
        }

        explicit unique_lock(Mutex &mutex) : mutex_(&mutex), owns_(false) {
            mutex_->lock();
            owns_ = true;
        }

        unique_lock(Mutex &mutex, std::defer_lock_t) noexcept: mutex_(&mutex), owns_(false) {}

        unique_lock(Mutex &mutex, std::adopt_lock_t) noexcept: mutex_(&mutex), owns_(true) {}

        ~unique_lock() noexcept {
            if (owns_) mutex_->unlock();
        }

        unique_lock &operator=(unique_lock &&other) noexcept {
            if (this == &other) return *this;

            if (owns_) mutex_->unlock();
            mutex_ = other.mutex_;
            owns_ = other.owns_;
            other.mutex_ = nullptr;
            other.owns_ = false;
            return *this;
        }

        void lock() {
            validate();
            mutex_->lock();
            owns_ = true;
        }

        bool try_lock() {
            validate();
            owns_ = mutex_->try_lock();
            return owns_;
        }

        void unlock() {
            if (!mutex_ || !owns_) throw std::system_error();
            mutex_->unlock();
            owns_ = false;
        }

        void swap(unique_lock &other) noexcept {
            std::swap(mutex_, other.mutex_);
            std::swap(owns_, other.owns_);
        }

    private:
        void validate() const {
            if (!mutex_ || owns_) throw std::system_error();
        }
    };
}


#endif //INCLUDE_UNIQUE_LOCK_HPP_
