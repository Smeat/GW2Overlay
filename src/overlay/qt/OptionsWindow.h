#ifndef __OPTIONSWINDOW_H__
#define __OPTIONSWINDOW_H__

#include <QMainWindow>
#include <QTreeWidgetItem>

#include <memory>
#include <vector>

#include "../../utils/POI.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class OptionsWindow;
}
QT_END_NAMESPACE

class CategoryTreeWidgetItem : public QTreeWidgetItem {
 public:
	CategoryTreeWidgetItem(QTreeWidget* p) : QTreeWidgetItem(p) {}
	CategoryTreeWidgetItem() : QTreeWidgetItem() {}
	void setCategoryMarker(std::shared_ptr<POI> d);
	std::shared_ptr<POI> getCategoryMarker();

 private:
	std::shared_ptr<POI> m_cat;
};

class OptionsWindow : public QMainWindow {
	Q_OBJECT

 public:
	OptionsWindow(QWidget* parent = nullptr);
	~OptionsWindow();

	void set_categories(const poi_container* cats, QTreeWidgetItem* parent = nullptr);
	void update_categories();
 public slots:
	void on_tree_click(QTreeWidgetItem* item, int column);
	void set_all(bool state);

 private:
	Ui::OptionsWindow* m_ui;
};

#endif	// __OPTIONSWINDOW_H__
