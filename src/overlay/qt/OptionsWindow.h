#ifndef __OPTIONSWINDOW_H__
#define __OPTIONSWINDOW_H__

#include <qwindowdefs.h>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QTimer>
#include <QTreeWidgetItem>

#include <memory>
#include <vector>

#include "../../utils/GW2/GW2Api.h"
#include "../../utils/POI.h"

#define STYLE_VALID "color: rgb(0, 255, 0);"
#define STYLE_INVALID "color: rgb(255, 0, 0);"

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
	void add_build_dialog();
	void add_build(const std::string& name, const std::string& desc, const std::string& val);
	void add_build(const QString& name, const QString& desc, const QString& val);
	void copy_build();
	void save_builds();
	void load_builds();
	void select_helper_path();
	std::shared_ptr<Ui::OptionsWindow> m_ui;
	std::map<std::string, QLineEdit*> m_options_map;
	std::map<GW2Permission, QLabel*> m_permission_map;
	void showEvent(QShowEvent* ev) override;
	void copy_from_api();
	void update_performance();
	void on_tab_change(int index);
	void on_build_select();

	bool m_disable_build_save = false;
	QTimer* m_update_timer = nullptr;
};

class NewBuildDialog : public QDialog {
 public:
	NewBuildDialog(QDialog* p = nullptr);

	std::shared_ptr<Ui::NewBuildDialog> m_ui;
};

#endif	// __OPTIONSWINDOW_H__
