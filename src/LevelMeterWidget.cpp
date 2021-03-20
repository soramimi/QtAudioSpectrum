#include "LevelMeterWidget.h"

#include <QPainter>

LevelMeterWidget::LevelMeterWidget(QWidget *parent)
	: QWidget(parent)
{

}

void LevelMeterWidget::paintEvent(QPaintEvent *event)
{
	QPainter pr(this);
	QRect r = rect();
	pr.fillRect(r, Qt::black);
	r.adjust(1, 1, -1, -1);
	r.setWidth(r.width() * percent_);
	pr.fillRect(r, Qt::green);
}

void LevelMeterWidget::setPercent(float percent)
{
	percent_ = percent;
	update();
}
