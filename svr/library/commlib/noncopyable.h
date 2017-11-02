#pragma once

class NonCopyable {
 protected:
    NonCopyable() {}
    ~NonCopyable() {}
 private:  // emphasize the following members are private
    NonCopyable(const NonCopyable&);
    const NonCopyable& operator=(const NonCopyable&);
};

