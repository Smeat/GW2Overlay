#include "OptionsIcon.h"

#include <iostream>

OptionsIcon::OptionsIcon(QWidget* parent, Qt::WindowFlags f)
	: QLabel(parent, f) {
	this->m_option_window.setWindowFlags(Qt::Dialog);
}

void OptionsIcon::toggle_options() {
	std::cout << "Toggle!" << std::endl;
	if (this->m_option_window.isVisible()) {
		this->m_option_window.hide();
	} else {
		this->m_option_window.show();
	}
}
void OptionsIcon::mousePressEvent(QMouseEvent* event) { emit toggle_options(); }
