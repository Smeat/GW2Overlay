#include "OptionsWindow.h"
#include <cstdlib>
#include "ui_NewBuildDialog.h"
#include "ui_OptionsWindow.h"

#include <QClipboard>
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
	connect(this->m_ui->add_build_button, &QPushButton::clicked, this, [this] { this->add_build_dialog(); });
	connect(this->m_ui->copy_build_button, &QPushButton::clicked, this, [this] { this->copy_build(); });
	connect(this->m_ui->build_list, &QTableWidget::itemChanged, this,
			[this](QTableWidgetItem* item) { this->save_builds(); });

	this->m_options_map.insert({std::string("API_KEY"), this->m_ui->api_text});
	this->m_options_map.insert({std::string("USE_KEY"), this->m_ui->use_text});
	this->load_settings();
	this->load_builds();
}

void OptionsWindow::load_settings() {
	auto& conf = ConfigManager::getInstance().get_config("SETTINGS");
	for (auto iter = this->m_options_map.begin(); iter != this->m_options_map.end(); ++iter) {
		std::cout << "Getting entry " << iter->first << std::endl;
		auto val = conf.get_entry(iter->first).get_item();
		iter->second->setText(val.c_str());
	}
	std::cout << "Done loading settings" << std::endl;
}

void OptionsWindow::save_settings() {
	for (auto iter = this->m_options_map.begin(); iter != this->m_options_map.end(); ++iter) {
		ConfigManager::getInstance()
			.get_config("SETTINGS")
			.set_item(iter->first, iter->second->displayText().toStdString());
		std::cout << "loop" << std::endl;
	}
	ConfigManager::getInstance().get_config("SETTINGS").save_config();
	std::cout << "Done saving settings" << std::endl;
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

void OptionsWindow::add_build_dialog() {
	NewBuildDialog d;
	QClipboard* clipboard = QGuiApplication::clipboard();
	d.m_ui->build_string_input->setText(clipboard->text());
	int res = d.exec();
	if (res == 1) {
		auto name = d.m_ui->build_name_input->displayText();
		auto description = d.m_ui->build_description_input->displayText();
		auto value = d.m_ui->build_string_input->displayText();
		this->add_build(name, description, value);
	}
}

void OptionsWindow::add_build(const std::string& name, const std::string& desc, const std::string& val) {
	this->add_build(QString::fromStdString(name), QString::fromStdString(desc), QString::fromStdString(val));
}
void OptionsWindow::add_build(const QString& name, const QString& desc, const QString& val) {
	auto build_list = this->m_ui->build_list;
	QTableWidgetItem* name_item = new QTableWidgetItem(name);
	QTableWidgetItem* description_item = new QTableWidgetItem(desc);
	QTableWidgetItem* value_item = new QTableWidgetItem(val);
	build_list->insertRow(build_list->rowCount());
	int row = build_list->rowCount() - 1;
	build_list->setItem(row, 0, name_item);
	build_list->setItem(row, 1, description_item);
	build_list->setItem(row, 2, value_item);
}

void OptionsWindow::copy_build() {
	auto items = this->m_ui->build_list->selectedItems();
	if (items.size() == 3) {
		auto item = items[2];
		QClipboard* clipboard = QGuiApplication::clipboard();
		clipboard->setText(item->text());
	}
}
void OptionsWindow::save_builds() {
	if (!this->m_disable_build_save) {
		auto& config = ConfigManager::getInstance().get_config("BUILDS");
		for (int row = 0; row < this->m_ui->build_list->rowCount(); ++row) {
			auto name_item = this->m_ui->build_list->item(row, 0);
			auto desc_item = this->m_ui->build_list->item(row, 1);
			auto val_item = this->m_ui->build_list->item(row, 2);
			if (name_item && desc_item && val_item) {
				std::string name = name_item->text().toStdString();
				std::string val = val_item->text().toStdString();

				std::string desc = desc_item->text().toStdString();
				std::cout << "Saving build with name " << name << " desc " << desc << " and val " << val << std::endl;
				config.set_items(val, {name, desc});
			}
		}
		// TODO: file name stuff?
		config.save_config("./builds.json");
	}
}
void OptionsWindow::load_builds() {
	this->m_disable_build_save = true;
	// this->m_ui->build_list.clear();
	auto& config = ConfigManager::getInstance().get_config("BUILDS");
	for (auto build = config.get_children()->begin(); build != config.get_children()->end(); ++build) {
		std::string val = build->second->get_name();
		auto data = build->second->get_items();
		if (data.size() == 2) {
			this->add_build(data[0], data[1], val);
		}
	}

	this->m_disable_build_save = false;
}
