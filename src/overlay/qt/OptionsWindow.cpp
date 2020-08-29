#include "OptionsWindow.h"
#include <cstdlib>
#include "ui_NewBuildDialog.h"
#include "ui_OptionsWindow.h"

#include <QTableWidgetItem>

#include "../../utils/CategoryManager.h"
#include "../../utils/Config.h"
#include "../../utils/POI.h"

NewBuildDialog::NewBuildDialog(QDialog* p) : QDialog(p, Qt::Popup), m_ui(new Ui::NewBuildDialog) {
	this->m_ui->setupUi(this);
	connect(this->m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(this->m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

OptionsWindow::OptionsWindow(QWidget* parent) : QMainWindow(parent), m_ui(new Ui::OptionsWindow) {
	this->m_ui->setupUi(this);

	this->m_ui->treeWidget->setColumnCount(2);
	this->m_ui->treeWidget->setHeaderLabels({"Display Name", "Name"});
	this->setAttribute(Qt::WA_TranslucentBackground, true);
	this->update_categories();
	// setStyleSheet("background-image:url(/tmp/background.png)");
	// setStyleSheet("background-color:transparent;");
	connect(this->m_ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this,
			SLOT(on_tree_click(QTreeWidgetItem*, int)));
	connect(this->m_ui->select_all_button, &QPushButton::clicked, this, [this] { this->set_all(true); });
	connect(this->m_ui->select_none_button, &QPushButton::clicked, this, [this] { this->set_all(false); });
	connect(this->m_ui->save_settings_button, &QPushButton::clicked, this, [this] { this->save_settings(); });
	connect(this->m_ui->add_build_button, &QPushButton::clicked, this, [this] { this->add_build(); });
	connect(this->m_ui->copy_build_button, &QPushButton::clicked, this, [this] { this->copy_build(); });

	this->m_options_map.insert({std::string("API_KEY"), this->m_ui->api_text});
	this->m_options_map.insert({std::string("USE_KEY"), this->m_ui->use_text});
	this->load_settings();
}

void OptionsWindow::load_settings() {
	auto conf = ConfigManager::getInstance().get_current_config();
	for (auto iter = this->m_options_map.begin(); iter != this->m_options_map.end(); ++iter) {
		auto val = conf->get_item(iter->first);
		iter->second->setText(val.c_str());
	}
}

void OptionsWindow::save_settings() {
	for (auto iter = this->m_options_map.begin(); iter != this->m_options_map.end(); ++iter) {
		ConfigManager::getInstance().get_current_config()->set_item(iter->first,
																	iter->second->displayText().toStdString());
	}
}

void set_all_children(QTreeWidgetItem* item, bool state) {
	for (int i = 0; i < item->childCount(); ++i) {
		auto child = dynamic_cast<CategoryTreeWidgetItem*>(item->child(i));
		child->setCheckState(0, state ? Qt::Checked : Qt::Unchecked);
		child->getCategoryMarker()->m_enabled = state;
		CategoryManager::getInstance().set_state_changed(true);
		set_all_children(child, state);
	}
}

void OptionsWindow::set_all(bool state) {
	for (int i = 0; i < this->m_ui->treeWidget->topLevelItemCount(); ++i) {
		auto item = dynamic_cast<CategoryTreeWidgetItem*>(this->m_ui->treeWidget->topLevelItem(i));
		item->setCheckState(0, state ? Qt::Checked : Qt::Unchecked);
		item->getCategoryMarker()->m_enabled = state;
		set_all_children(item, state);
	}
}

void OptionsWindow::on_tree_click(QTreeWidgetItem* item, int column) {
	std::shared_ptr<POI> cat_item = dynamic_cast<CategoryTreeWidgetItem*>(item)->getCategoryMarker();
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
	std::cout << "Starting setting categories" << std::endl;
	this->set_categories(CategoryManager::getInstance().get_pois());
	std::cout << "Finished setting categories" << std::endl;
}

void OptionsWindow::set_categories(const poi_container* cats, QTreeWidgetItem* parent) {
	for (auto iter = cats->begin(); iter != cats->end(); ++iter) {
		if ((*iter)->m_is_poi) continue;
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
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
		item->setCheckState(0, Qt::Checked);
		item->setCategoryMarker(*iter);
		this->set_categories((*iter)->get_children(), item);
	}
}
void CategoryTreeWidgetItem::setCategoryMarker(std::shared_ptr<POI> d) { this->m_cat = d; }
std::shared_ptr<POI> CategoryTreeWidgetItem::getCategoryMarker() { return this->m_cat; }

void OptionsWindow::add_build() {
	NewBuildDialog d;
	int res = d.exec();
	std::cout << "Pressed button " << res << std::endl;
	if (res == 1) {
		auto name = d.m_ui->build_name_input->displayText();
		auto description = d.m_ui->build_name_input->displayText();
		auto value = d.m_ui->build_name_input->displayText();
		auto build_list = this->m_ui->build_list;
		QTableWidgetItem* name_item = new QTableWidgetItem(name);
		QTableWidgetItem* description_item = new QTableWidgetItem(description);
		QTableWidgetItem* value_item = new QTableWidgetItem(value);
		build_list->insertRow(build_list->rowCount());
		int row = build_list->rowCount() - 1;
		build_list->setItem(row, 0, name_item);
		build_list->setItem(row, 1, description_item);
		build_list->setItem(row, 2, value_item);
	}
}

void OptionsWindow::copy_build() {}
