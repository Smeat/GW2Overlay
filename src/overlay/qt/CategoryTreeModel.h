#ifndef __CATEGORYTREEMODEL_H__
#define __CATEGORYTREEMODEL_H__

#include <QAbstractItemModel>

#include "../../utils/POI.h"

class CategoryTreeModel : public QAbstractItemModel {
	Q_OBJECT

 public:
	explicit CategoryTreeModel(const std::string& data,
							   QObject* parent = nullptr);
	~CategoryTreeModel();

 private:
	MarkerCategory* m_root;
};

#endif	//  __CATEGORYTREEMODEL_H__
