#include "serializable.h"

std::map<std::string, SerializableStructInfo *> *SerializableStructInfo::infos = NULL;
std::map<std::size_t , SerializableStructInfo *> *SerializableStructInfo::infosByType = NULL;
