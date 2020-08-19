#ifndef __OPTIONSWINDOW_H__
#define __OPTIONSWINDOW_H__

#include <QMainWindow>
#include <QTreeWidgetItem>

#include <memory>
#include <vector>

#include "../../utils/POI.h"
#include "../../utils/TacoLoader.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class OptionsWindow;
}
QT_END_NAMESPACE

class OptionsWindow : public QMainWindow {
	Q_OBJECT

 public:
	OptionsWindow(category_container cats, QWidget* parent = nullptr);
	~OptionsWindow();

	void set_categories(category_container cats,
						QTreeWidgetItem* parent = nullptr);

 private:
	Ui::OptionsWindow* m_ui;
};

#endif	// __OPTIONSWINDOW_H__
