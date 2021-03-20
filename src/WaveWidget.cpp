#include "WaveWidget.h"

#include <QPainter>

WaveWidget::WaveWidget(QWidget *parent)
	: QWidget(parent)
{

}

void WaveWidget::setData(std::vector<float> const &v)
{
	data_ = v;
	update();
}

void WaveWidget::paintEvent(QPaintEvent *event)
{
	QPainter pr(this);
	pr.fillRect(rect(), Qt::black);

	pr.setPen(Qt::green);

	int n = data_.size();
	int x0 = 0;
	int y0 = height() - 1;
	for (int i = 0; i < n; i++) {
		int x1 = width() * (i + 1) / n;
		int y1 = height() - 1 - data_[i] / 100.0;
		pr.drawLine(x0, y0, x1, y1);
		x0 = x1;
		y0 = y1;
	}
}
