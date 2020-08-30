#ifndef __OPTIONSWINDOW_H__
#define __OPTIONSWINDOW_H__

#include <QDialog>
#include <QLineEdit>
#include <QMainWindow>
#include <QTreeWidgetItem>

#include <memory>
#include <vector>

#include "../../utils/POI.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class OptionsWindow;
class NewBuildDialog;
}  // namespace Ui
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
	~OptionsWindow() = default;

	void set_categories(const poi_container* cats, QTreeWidgetItem* parent = nullptr);
	void update_categories();
 public slots:
	void on_tree_click(QTreeWidgetItem* item, int column);
	void set_all(bool state);

 private:
	void save_settings();
	void load_settings();
	void add_build();
	void copy_build();
	void save_builds();
	void load_builds();
	std::shared_ptr<Ui::OptionsWindow> m_ui;
	std::map<std::string, QLineEdit*> m_options_map;
};

class NewBuildDialog : public QDialog {
 public:
	NewBuildDialog(QDialog* p = nullptr);

	std::shared_ptr<Ui::NewBuildDialog> m_ui;
};

#endif	// __OPTIONSWINDOW_H__
