#include "OptionsWindow.h"
#include <qtabwidget.h>
#include <cstdio>
#include <cstdlib>
#include "ui_NewBuildDialog.h"
#include "ui_OptionsWindow.h"

#include <QClipboard>
#include <QFileDialog>
#include <QShowEvent>
#include <QTableWidgetItem>
#include <string>

#include "TraitLineWidget.h"

#include "../../utils/CategoryManager.h"
#include "../../utils/Config.h"
#include "../../utils/GW2/GW2Builds.h"
#include "../../utils/GW2/GW2Manager.h"
#include "../../utils/POI.h"
#include "../../utils/PerformanceStats.h"
#include "../../utils/ProcessUtils.h"

#include "../../utils/json/json.hpp"

using json = nlohmann::json;

NewBuildDialog::NewBuildDialog(QDialog* p) : QDialog(p, Qt::Popup), m_ui(new Ui::NewBuildDialog) {
	this->m_ui->setupUi(this);
	connect(this->m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(this->m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

OptionsWindow::OptionsWindow(QWidget* parent) : QMainWindow(parent), m_ui(new Ui::OptionsWindow) {
	this->m_ui->setupUi(this);
	this->m_update_timer = new QTimer(this);

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
	connect(this->m_ui->build_list, &QTableWidget::itemSelectionChanged, this, [this] { this->on_build_select(); });
	connect(this->m_ui->select_helper_button, &QPushButton::clicked, this, [this] { this->select_helper_path(); });
	connect(this->m_ui->copy_api_button, &QPushButton::clicked, this, [this] { this->copy_from_api(); });
	connect(this->m_update_timer, &QTimer::timeout, this, [this] { this->update_performance(); });
	connect(this->m_ui->tabWidget, &QTabWidget::currentChanged, this,
			[this](int index) { this->on_tab_change(index); });

	this->m_options_map.insert({std::string("API_KEY"), this->m_ui->api_text});
	this->m_options_map.insert({std::string("USE_KEY"), this->m_ui->use_text});
	this->m_options_map.insert({std::string("HELPER_SCRIPT"), this->m_ui->helper_path_lineedit});
	this->m_permission_map = {{GW2Permission::BUILDS, this->m_ui->builds_label},
							  {GW2Permission::CHARACTERS, this->m_ui->characters_label},
							  {GW2Permission::GUILDS, this->m_ui->guilds_label},
							  {GW2Permission::INVENTORIES, this->m_ui->inventories_label},
							  {GW2Permission::PROGRESSION, this->m_ui->progression_label},
							  {GW2Permission::PVP, this->m_ui->pvp_label},
							  {GW2Permission::TRADINGPOST, this->m_ui->tradingpost_label},
							  {GW2Permission::UNLOCKS, this->m_ui->unlocks_label},
							  {GW2Permission::WALLET, this->m_ui->wallet_label}};
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
		if ((*iter)->get_is_poi()) continue;
		CategoryTreeWidgetItem* item = nullptr;
		if (parent) {
			item = new CategoryTreeWidgetItem();
			parent->addChild(item);
		} else {
			item = new CategoryTreeWidgetItem(this->m_ui->treeWidget);
			this->m_ui->treeWidget->addTopLevelItem(item);
		}
		item->setText(0, (*iter)->get_display_name().c_str());
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
	while (this->m_ui->build_list->rowCount() > 0) {
		this->m_ui->build_list->removeRow(0);
	}
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

void OptionsWindow::showEvent(QShowEvent* ev) {
	QMainWindow::showEvent(ev);
	// update process stuff
	bool gw2_running = GW2Manager::getInstance().is_gw2_running();
	if (gw2_running) {
		this->m_ui->gw2_running_label->setStyleSheet(STYLE_VALID);
	} else {
		this->m_ui->gw2_running_label->setStyleSheet(STYLE_INVALID);
	}
	bool helper_running = GW2Manager::getInstance().is_helper_running();
	if (helper_running) {
		this->m_ui->helper_running_label->setStyleSheet(STYLE_VALID);
	} else {
		this->m_ui->helper_running_label->setStyleSheet(STYLE_INVALID);
	}
	auto& api_instance = GW2Manager::getInstance();
	for (auto iter = this->m_permission_map.begin(); iter != this->m_permission_map.end(); ++iter) {
		if (api_instance.get_api()->has_permission(iter->first)) {
			iter->second->setStyleSheet(STYLE_VALID);
		} else {
			iter->second->setStyleSheet(STYLE_INVALID);
		}
	}
}
void OptionsWindow::select_helper_path() {
	QFileDialog dialog(this, Qt::Popup);
	std::string fileName;
	if (dialog.exec() == QDialog::Accepted) {
		fileName = dialog.selectedFiles().value(0).toStdString();
	}
	std::cout << "Selected " << fileName << std::endl;
	//	QFileDialog::getSaveFileName(this, "Select Helper script", "", "Python script (*.py);;All Files (*)")
	//		.toStdString();
	if (!fileName.empty()) {
		ConfigManager::getInstance().get_config("SETTINGS").set_item("HELPER_SCRIPT", fileName);
		this->m_ui->helper_path_lineedit->setText(QString::fromStdString(fileName));
		GW2Manager::getInstance().start_helper();
	}
}

std::vector<GW2BuildChatLink> get_builds(const std::string& char_name) {
	auto& api_instance = GW2Manager::getInstance();
	auto res = json::parse(
		api_instance.get_api()->get_value("v2/characters/" + char_name + "/buildtabs", {"tabs=all"}, false));
	std::vector<GW2BuildChatLink> links;
	for (auto iter = res.begin(); iter != res.end(); ++iter) {
		std::cout << "## Build: " << (*iter).dump() << std::endl;
		GW2BuildChatLink link((*iter)["build"].dump());
		std::cout << link.get_chat_string() << std::endl;
		links.push_back(link);
	}
	std::cout << "Got " << links.size() << " builds" << std::endl;
	return links;
}

void OptionsWindow::copy_from_api() {
	GW2Link* link = GW2Manager::getInstance().get_link();
	try {
		std::string identity = link->get_gw2_data()->get_identity();
		json identity_json = json::parse(identity);
		std::string name = identity_json["name"].get<std::string>();
		auto builds = get_builds(name);
		auto& config = ConfigManager::getInstance().get_config("BUILDS");
		auto build_map = config.get_children();
		for (auto iter = build_map->begin(); iter != build_map->end(); ++iter) {
			std::cout << "####" << iter->first << std::endl;
		}
		for (auto iter = builds.begin(); iter != builds.end(); ++iter) {
			if (build_map->find(iter->get_chat_string()) == build_map->end()) {
				config.set_items(iter->get_chat_string(), {iter->name, "Imported from API"});
				std::cout << "Importing new build " << iter->name << ":" << iter->get_chat_string() << std::endl;
			} else {
				std::cout << iter->get_chat_string() << " already exists..." << std::endl;
			}
		}
		config.save_config("./builds.json");
		this->load_builds();
	} catch (std::exception& e) {
		std::cout << "Error while getting builds from api..." << e.what() << std::endl;
	}
	std::cout << "Done" << std::endl;
}

void OptionsWindow::on_tab_change(int index) {
	// TODO: stop on close
	if (index == 2) {
		std::cout << "Starting timer!" << std::endl;
		this->m_update_timer->start(300);
	} else {
		std::cout << "Stopping timer!" << std::endl;
		this->m_update_timer->stop();
	}
}

void OptionsWindow::update_performance() {
	float gpu_time = PerformanceStats::getInstance().get_time("gpu") / 1000.0;
	float link_time = PerformanceStats::getInstance().get_time("vkfence") / 1000.0;
#ifdef PRINT_PERFORMANCE
	auto times = PerformanceStats::getInstance().get_times();
	std::cout << "[Times] ";
	for (auto iter = times.begin(); iter != times.end(); ++iter) {
		std::cout << iter->first << ": " << iter->second / 1000.0 << ", ";
	}
	std::cout << std::endl;
#endif
	this->m_ui->gpu_time_label->setText(QString::number(gpu_time));
	this->m_ui->link_time_label->setText(QString::number(link_time));
}

void OptionsWindow::on_build_select() {
	auto items = this->m_ui->build_list->selectedItems();
	std::string build_string = items[2]->text().toStdString();
	std::string name = items[0]->text().toStdString();
	std::cout << "Selected " << name << " val " << build_string << std::endl;
	GW2BuildChatLink build(build_string, name);
	this->m_ui->trait_widget->set_build(build);
}
