#include "OptionsWindow.h"
#include <cstdlib>
#include "ui_OptionsWindow.h"

#include "../../utils/POI.h"

OptionsWindow::OptionsWindow(category_container cats, QWidget* parent)
	: QMainWindow(parent), m_ui(new Ui::OptionsWindow) {
	this->m_ui->setupUi(this);

	this->m_ui->treeWidget->setColumnCount(2);
	this->m_ui->treeWidget->setHeaderLabels({"Display Name", "Name"});

	this->set_categories(cats);
}

OptionsWindow::~OptionsWindow() { delete this->m_ui; }

void OptionsWindow::set_categories(category_container cats,
								   QTreeWidgetItem* parent) {
	for (auto iter = cats.begin(); iter != cats.end(); ++iter) {
		std::cout << "Adding " << (*iter)->m_display_name << std::endl;
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
		if ((*iter)->m_children.size()) {
			this->set_categories((*iter)->m_children, item);
		}
	}
}
