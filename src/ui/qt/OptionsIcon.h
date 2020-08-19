#ifndef __OPTIONSICON_H__
#define __OPTIONSICON_H__

#include <QLabel>

#include <memory>
#include <vector>
#include "OptionsWindow.h"

#include "../../utils/POI.h"

class OptionsIcon : public QLabel {
	Q_OBJECT

 public:
	OptionsIcon(category_container cats, QWidget* parent = nullptr,
				Qt::WindowFlags f = Qt::WindowFlags());

 public slots:
	void toggle_options();

 protected:
	void mousePressEvent(QMouseEvent* event);

 private:
	OptionsWindow m_option_window;
};

#endif	//  __OPTIONSICON_H__
