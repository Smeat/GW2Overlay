#include "CategoryTreeModel.h"

CategoryTreeModel::CategoryTreeModel(const std::string& data, QObject* parent) {
	this->m_root = new MarkerCategory;
	setupModelData({"hi", "ab"}, this->m_root);
}
