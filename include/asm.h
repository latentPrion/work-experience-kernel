#ifndef _ASSM_H
#define _ASSM_H

#define BEGIN_GLOBAL_FUNCTION(_name) \
    .global _name; \
    BEGIN_FUNCTION(_name)

#define BEGIN_LOCAL_FUNCTION(_name) \
    .local _name; \
    BEGIN_FUNCTION(_name)

#define BEGIN_FUNCTION(_name) \
    .type _name, @function; \
    _name:; \

#define END_FUNCTION(_name) \
    .size _name, . - _name;

#endif
