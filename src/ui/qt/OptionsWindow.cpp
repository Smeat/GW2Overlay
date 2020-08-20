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
	this->setAttribute(Qt::WA_TranslucentBackground, true);
	this->update_categories();
	// setStyleSheet("background-image:url(/tmp/background.png)");
	// setStyleSheet("background-color:transparent;");
	connect(this->m_ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
			this, SLOT(on_tree_click(QTreeWidgetItem*, int)));
}

OptionsWindow::~OptionsWindow() { delete this->m_ui; }

void set_all_children(QTreeWidgetItem* item, bool state) {
	for (int i = 0; i < item->childCount(); ++i) {
		auto child = dynamic_cast<CategoryTreeWidgetItem*>(item->child(i));
		child->setCheckState(0, state ? Qt::Checked : Qt::Unchecked);
		child->getCategoryMarker()->m_enabled = state;
		set_all_children(child, state);
	}
}

void OptionsWindow::on_tree_click(QTreeWidgetItem* item, int column) {
	std::shared_ptr<MarkerCategory> cat_item =
		dynamic_cast<CategoryTreeWidgetItem*>(item)->getCategoryMarker();
	bool enabled = item->checkState(0) == 2;
	if (enabled != cat_item->m_enabled) {
		cat_item->m_enabled = !cat_item->m_enabled;
		// apply to all children
		set_all_children(item, cat_item->m_enabled);
		CategoryManager::getInstance().set_state_changed(true);
	}
}

void OptionsWindow::update_categories() {
	// TODO: clear the tree before updating?
	// this->m_ui->treeWidget.clear();
	this->set_categories(CategoryManager::getInstance().get_categories());
}

void OptionsWindow::set_categories(const category_container* cats,
								   QTreeWidgetItem* parent) {
	for (auto iter = cats->begin(); iter != cats->end(); ++iter) {
		CategoryTreeWidgetItem* item = nullptr;
		if (parent) {
			item = new CategoryTreeWidgetItem();
			parent->addChild(item);
		} else {
			item = new CategoryTreeWidgetItem(this->m_ui->treeWidget);
			this->m_ui->treeWidget->addTopLevelItem(item);
		}
		item->setText(0, (*iter)->m_display_name.c_str());
		item->setText(1, (*iter)->m_name.c_str());
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable |
					   Qt::ItemIsSelectable);
		item->setCheckState(0, Qt::Checked);
		item->setCategoryMarker(*iter);
		this->set_categories((*iter)->get_children(), item);
	}
}
void CategoryTreeWidgetItem::setCategoryMarker(
	std::shared_ptr<MarkerCategory> d) {
	this->m_cat = d;
}
std::shared_ptr<MarkerCategory> CategoryTreeWidgetItem::getCategoryMarker() {
	return this->m_cat;
}