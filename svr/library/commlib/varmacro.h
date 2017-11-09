#pragma once

#include <cstring>
#include <string>

// param define
// 简单的变量使用宏来定义
// public:
//     const uint32_t& get_uid() const { return uid_; }
//     void set_uid(const uint32_t& newval) { uid_ = newval; }
// private:
//     uint32_t uid_;

#define GETSETVAR(type, name, default_value)                         \
public:                                                              \
    inline const type& name() const { return name##_; }              \
    inline type& name() { return name##_; }                          \
    inline type& mutable_##name() { return name##_; }                \
    inline void set_##name(const type& newval) { name##_ = newval; } \
                                                                     \
protected:                                                           \
    type name##_ = default_value;

#define STRUCT_VAR(type, name)                                       \
public:                                                              \
    inline const type& name() const { return name##_; }              \
    inline type& name() { return name##_; }                          \
    inline type& mutable_##name() { return name##_; }                \
    inline void set_##name(const type& newval) { name##_ = newval; } \
                                                                     \
protected:                                                           \
    type name##_;

#define GETSETPTR(type, name)                                     \
public:                                                           \
    inline type* name##_ptr() const { return name##_; }           \
    inline const type& name() const { return *name##_; }          \
    inline type& name() { return *name##_; }                      \
    inline bool has_##name() const { return nullptr != name##_; } \
    inline void set_##name(type* newval) { name##_ = newval; }    \
    inline void clear_##name() { name##_ = nullptr; }             \
                                                                  \
private:                                                          \
    type* name##_ = nullptr;