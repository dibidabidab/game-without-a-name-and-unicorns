
#ifndef GAME_SFINAE_H
#define GAME_SFINAE_H

#define HAS_MEM_FUNC(func, name)                                        \
template <typename T>\
class name\
{\
    typedef char one;\
    struct two { char x[2]; };\
\
    template <typename C> static one test( decltype(&C::func) ) ;\
    template <typename C> static two test(...);\
\
  public:\
    enum { value = sizeof(test<T>(0)) == sizeof(char) };\
};

#endif //GAME_SFINAE_H
