#ifndef WAVEWIDGET_H
#define WAVEWIDGET_H

#include <QWidget>

class WaveWidget : public QWidget {
	Q_OBJECT
private:
	std::vector<float> data_;
public:
	explicit WaveWidget(QWidget *parent = nullptr);
	void setData(const std::vector<float> &v);
protected:
	void paintEvent(QPaintEvent *event);
};

#endif // WAVEWIDGET_H
