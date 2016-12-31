
#include "primitive.h"

int Primitive::nextId = 0;

Primitive::Primitive() : id(Primitive::nextId++) { }