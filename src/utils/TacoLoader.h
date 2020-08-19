#ifndef __TACOLOADER_H__
#define __TACOLOADER_H__

#include <functional>
#include <memory>
#include <set>
#include <vector>

#include "xml/pugixml.hpp"

#include "POI.h"

void load_xml_types(const std::string& filename, poi_container* poi_vec,
					category_container* category_vec);

#endif
