#ifndef LEVELMETERWIDGET_H
#define LEVELMETERWIDGET_H

#include <QWidget>

class LevelMeterWidget : public QWidget {
	Q_OBJECT
private:
	float percent_ = 0;
protected:
	void paintEvent(QPaintEvent *event);
public:
	explicit LevelMeterWidget(QWidget *parent = nullptr);
	void setPercent(float percent);
};

#endif // LEVELMETERWIDGET_H
