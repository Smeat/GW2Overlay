#include "QtMain.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

#include "qt/OptionsIcon.h"

#include "../utils/POI.h"

int qt_main(int argc, char** argv) {
	QApplication a(argc, argv);
	OptionsIcon options_icon;
	options_icon.resize(100, 30);
	// clang-format off
	options_icon.setWindowFlags(
			Qt::X11BypassWindowManagerHint |
			Qt::FramelessWindowHint | 
			Qt::WindowStaysOnTopHint |
			Qt::WindowDoesNotAcceptFocus
			);
	// clang-format on
	options_icon.setAttribute(Qt::WA_TranslucentBackground, true);
	options_icon.setScaledContents(true);
	options_icon.setPixmap(QPixmap("../res/img/options_icon.png"));
	options_icon.resize(32, 32);
	options_icon.move(1580, 0);

	options_icon.show();

	return a.exec();
}

// int main(int argc, char** argv) { return qt_main(argc, argv); }
