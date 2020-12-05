#ifndef __TRAITLINEWIDGET_H_
#define __TRAITLINEWIDGET_H_

#include <qboxlayout.h>
#include <qgraphicseffect.h>
#include <qguiapplication.h>
#include <qlayoutitem.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qwidget.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QFormLayout>
#include <QGraphicsColorizeEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QScreen>
#include <QWidget>

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "../../utils/GW2/GW2Api.h"
#include "../../utils/GW2/GW2Builds.h"
#include "../../utils/GW2/GW2Manager.h"

#define MAJOR_TRAIT_NUM 9

class TraitTooltip : public QWidget {
 public:
	TraitTooltip(QWidget* parent, const GW2Trait* trait) : QWidget(parent) {
		this->setWindowFlags(Qt::ToolTip);
		this->m_title = new QLabel(this);
		this->m_description = new QLabel(this);
		this->m_vertical_base = new QVBoxLayout;
		this->m_form_facts = new QFormLayout;
		this->m_form_facts_widget = new QWidget(this);
		this->m_recharge_layout = new QHBoxLayout;
		// this->m_recharge_layout_widget = new QWidget(this);

		this->m_title->setText(trait->get_name().c_str());
		this->m_description->setText(trait->get_description().c_str());

		this->m_recharge_layout->addWidget(this->m_title);
		this->m_vertical_base->addLayout(this->m_recharge_layout);
		// this->m_vertical_base->addWidget(this->m_recharge_layout_widget);
		this->m_vertical_base->addWidget(this->m_description);
		this->m_vertical_base->addWidget(this->m_form_facts_widget);

		auto facts = trait->get_facts();
		for (auto iter = facts.begin(); iter != facts.end(); ++iter) {
			QLabel* label_icon = new QLabel(this);
			auto icon_data = iter->get_icon_data();
			QPixmap icon;
			icon.loadFromData((uchar*)icon_data.data(), icon_data.size());
			label_icon->setPixmap(icon);
			label_icon->setScaledContents(true);
			// recharge has a special place at the top
			if (iter->type == "Recharge") {
				label_icon->setMaximumSize(16, 16);
				QLabel* time = new QLabel(this);
				time->setText(QString::number(iter->value));
				QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
				this->m_recharge_layout->addItem(spacer);
				this->m_recharge_layout->addWidget(time, 0, Qt::AlignRight);
				this->m_recharge_layout->addWidget(label_icon, 0, Qt::AlignRight);

			} else {
				QLabel* label_text = new QLabel(this);
				label_icon->setMaximumSize(32, 32);
				std::string text = iter->text;
				if (iter->type == "Percent") {
					text += ": " + std::to_string(iter->percent) + "%";
				} else if (iter->type == "Buff") {
					text = iter->status + "(" + std::to_string(iter->duration) + "s): " + iter->description;
				} else if (iter->type == "Number" || iter->type == "AttributeAdjust" || iter->type == "Distance") {
					text += ": " + std::to_string(iter->value);
				} else if (iter->type == "Time") {
					text += ": " + std::to_string(iter->duration) + " seconds";
				} else if (iter->type == "ComboFinisher") {
					text += ": " + iter->finisher_type;
					if (iter->percent < 100) {
						text += "(" + std::to_string(iter->percent) + " chance)";
					}
				} else if (iter->type == "BuffConversion") {
					text = "Gain " + iter->target + " based on a percentage of " + iter->source + ": " +
						   std::to_string(iter->percent) + "%";
				}
				label_text->setText(text.c_str());
				this->m_form_facts->addRow(label_icon, label_text);
			}
		}
		this->setLayout(this->m_vertical_base);
		this->m_form_facts_widget->setLayout(this->m_form_facts);
	}

 private:
	QVBoxLayout* m_vertical_base;
	QLabel* m_title;
	QLabel* m_description;
	QFormLayout* m_form_facts;
	QWidget* m_form_facts_widget;
	QHBoxLayout* m_recharge_layout;
};

class QLabelGW2Tooltip : public QLabel {
 public:
	QLabelGW2Tooltip(QWidget* parent, const GW2Trait* trait = nullptr) : QLabel(parent) {
		this->setMouseTracking(true);
		if (trait) {
			this->m_tooltip = new TraitTooltip(this, trait);
		}
	}

	virtual void mouseMoveEvent(QMouseEvent* event) {
		if (this->m_tooltip) {
			this->m_tooltip->show();
			int y_offset = this->m_tooltip->rect().height() + 10;
			int total_x = 0;
			for (auto iter = QGuiApplication::screens().begin(); iter != QGuiApplication::screens().end(); ++iter) {
				total_x += (*iter)->geometry().width();
			}
			int x_offset = std::min(0, total_x - (event->globalX() + this->m_tooltip->rect().width()));
			this->m_tooltip->move(event->globalX() + x_offset, event->globalY() - y_offset);
		}
	}
	virtual void leaveEvent(QEvent* event) {
		if (this->m_tooltip) {
			this->m_tooltip->hide();
		}
	}

 private:
	TraitTooltip* m_tooltip = nullptr;
};

class TraitLineWidget : public QWidget {
 public:
	TraitLineWidget(QWidget* parent, specialization_t spec) : QWidget(parent) {
		for (int i = 0; i < MAJOR_TRAIT_NUM; ++i) {
			this->m_unused_effect[i] = new QGraphicsColorizeEffect;
			this->m_unused_effect[i]->setStrength(1);
			this->m_unused_effect[i]->setColor(QColor(255, 255, 255, 0));
		}
		this->m_spec = spec;
		this->m_api = GW2Manager::getInstance().get_api();

		GW2Specialization gw_spec(spec.specialization);
		gw_spec.load_data();

		// get background
		QPixmap background_image;
		QPixmap main_icon_image;
		auto background_data = gw_spec.get_background();
		auto main_icon_data = gw_spec.get_icon();
		background_image.loadFromData((uchar*)background_data.data(), background_data.size());
		main_icon_image.loadFromData((uchar*)main_icon_data.data(), main_icon_data.size());

		this->m_background_label = new QLabel(this);
		this->m_background_label->setPixmap(background_image);
		this->m_background_label->setAlignment(Qt::AlignBottom | Qt::AlignLeading | Qt::AlignLeft);
		this->m_background_label->setGeometry(QRect(-40, 0, 651, 141));

		this->m_main_icon_label = new QLabelGW2Tooltip(this);
		this->m_main_icon_label->setPixmap(main_icon_image);
		this->m_main_icon_label->setGeometry(QRect(30, 10, 101, 101));
		this->m_main_icon_label->setScaledContents(true);

		auto minor_traits = gw_spec.get_minor_traits();
		for (int i = 0; i < 3; ++i) {
			QPixmap minor_trait_image;
			auto minor_trait_data = minor_traits[i].get_icon();
			minor_trait_image.loadFromData((uchar*)minor_trait_data.data(), minor_trait_data.size());

			this->m_minor_trait_labels[i] = new QLabelGW2Tooltip(this, &minor_traits[i]);
			this->m_minor_trait_labels[i]->setPixmap(minor_trait_image);
			this->m_minor_trait_labels[i]->setScaledContents(true);
			this->m_minor_trait_labels[i]->setGeometry(QRect(160 + 150 * i, 50, 32, 32));
		}
		auto major_traits = gw_spec.get_major_traits();
		for (int x = 0; x < 3; ++x) {
			for (int y = 0; y < 3; ++y) {
				int i = x * 3 + y;
				QPixmap major_trait_image;
				auto major_trait_data = major_traits[i].get_icon();
				major_trait_image.loadFromData((uchar*)major_trait_data.data(), major_trait_data.size());
				QPainter pixmapPainter(&major_trait_image);
				pixmapPainter.setCompositionMode(QPainter::CompositionMode_Overlay);
				pixmapPainter.fillRect(major_trait_image.rect(), QColor(255, 255, 255, 0));
				pixmapPainter.end();

				this->m_major_trait_labels[i] = new QLabelGW2Tooltip(this, &major_traits[i]);
				this->m_major_trait_labels[i]->setPixmap(major_trait_image);
				this->m_major_trait_labels[i]->setScaledContents(true);
				this->m_major_trait_labels[i]->setGeometry(QRect(230 + 150 * x, 15 + 35 * y, 32, 32));

				if ((x == 0 && y + 1 != spec.trait_adept) || (x == 1 && y + 1 != spec.trait_master) ||
					(x == 2 && y + 1 != spec.trait_grandmaster))
					this->m_major_trait_labels[i]->setGraphicsEffect(this->m_unused_effect[i]);
			}
		}
	}

 private:
	QLabel* m_background_label;
	QLabel* m_main_icon_label;
	QLabel* m_minor_trait_labels[3];
	QLabel* m_major_trait_labels[MAJOR_TRAIT_NUM];
	QGraphicsColorizeEffect* m_unused_effect[MAJOR_TRAIT_NUM];

	specialization_t m_spec;
	GW2Api* m_api;
};

class TraitWidget : public QWidget {
 public:
	TraitWidget(QWidget* parent) : QWidget(parent) {
		this->m_layout = new QVBoxLayout;
		this->setLayout(this->m_layout);
	}

	void set_build(GW2BuildChatLink build) {
		QLayoutItem* item = nullptr;
		while ((item = this->m_layout->takeAt(0)) != nullptr) {
			delete item->widget();
			delete item;
		}
		for (int i = 0; i < 3; ++i) {
			this->m_traits[i] = new TraitLineWidget(this, build.chat_data.specialization[i]);
			this->m_layout->addWidget(this->m_traits[i]);
		}
	}

 private:
	TraitLineWidget* m_traits[3];
	QVBoxLayout* m_layout;
};

#endif	// __TRAITLINEWIDGET_H_
