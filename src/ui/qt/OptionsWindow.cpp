#include "OptionsWindow.h"
#include <cstdlib>
#include "ui_OptionsWindow.h"

#include "../../utils/CategoryManager.h"
#include "../../utils/POI.h"

OptionsWindow::OptionsWindow(QWidget* parent)
	: QMainWindow(parent), m_ui(new Ui::OptionsWindow) {
	this->m_ui->setupUi(this);

	this->m_ui->treeWidget->setColumnCount(2);
	this->m_ui->treeWidget->setHeaderLabels({"Display Name", "Name"});
	this->update_categories();
}

OptionsWindow::~OptionsWindow() { delete this->m_ui; }

void OptionsWindow::update_categories() {
	// TODO: clear the tree before updating?
	// this->m_ui->treeWidget.clear();
	this->set_categories(CategoryManager::getInstance().get_categories());
}

void OptionsWindow::set_categories(const category_container* cats,
								   QTreeWidgetItem* parent) {
	for (auto iter = cats->begin(); iter != cats->end(); ++iter) {
		QTreeWidgetItem* item = nullptr;
		if (parent) {
			item = new QTreeWidgetItem();
			item->setText(0, (*iter)->m_display_name.c_str());
			item->setText(1, (*iter)->m_name.c_str());
			item->setFlags(item->flags() | Qt::ItemIsUserCheckable |
						   Qt::ItemIsSelectable);
			item->setCheckState(0, Qt::Checked);
			parent->addChild(item);
		} else {
			item = new QTreeWidgetItem(this->m_ui->treeWidget);
			item->setText(0, (*iter)->m_display_name.c_str());
			item->setText(1, (*iter)->m_name.c_str());
			item->setFlags(item->flags() | Qt::ItemIsUserCheckable |
						   Qt::ItemIsSelectable);
			item->setCheckState(0, Qt::Checked);
			this->m_ui->treeWidget->addTopLevelItem(item);
		}
		this->set_categories((*iter)->get_children(), item);
	}
}
