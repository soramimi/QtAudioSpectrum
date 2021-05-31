#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <stdint.h>

class Video;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class LevelMeterWidget;

class MainWindow : public QDialog {
	Q_OBJECT
	friend class ImageWidget;
	friend class InternalImageView;
	friend class ThumbnailCache;
private:
	Ui::MainWindow *ui;
    struct Private;
	Private *m;
	void setLevel(const int16_t *p, int n, LevelMeterWidget *w);
	void setInputLevel(const int16_t *p, int n);
	void updateSpectrum(const int16_t *data, int size);
public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
private slots:
	void onReadyRead();
	void on_comboBox_input_currentIndexChanged(int index);
};
#endif // MAINWINDOW_H
