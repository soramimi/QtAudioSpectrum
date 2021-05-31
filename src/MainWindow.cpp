#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "WaveWidget.h"
#include <QAudioInput>
#include <QAudioOutput>
#include <QClipboard>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <utility>
#include <memory>
#include <complex>

// ref. https://github.com/tisseq/FFT-IFFT

#define PI 3.14159265359
typedef unsigned int uint;
typedef std::vector<std::complex<float>> float_complex_vector_t;

float_complex_vector_t fft(float_complex_vector_t const *a)
{
	uint s = a->size();
	if (s == 1) {
		return *a;
	}

	float_complex_vector_t val(a->size());
	for (uint i = 0; i < s; i++) {
		std::complex<float>c(cos(2.0 * PI * i / s), sin(2.0 * PI * i / s));
		val[i] = c;
	}

	float_complex_vector_t odd(s / 2), nodd(s / 2);
	for (uint i = 0; i < s/2; i++) {
		odd[i] = (*a)[i * 2];
		nodd[i] = (*a)[i * 2 + 1];
	}

	float_complex_vector_t odref = fft(&odd);
	float_complex_vector_t nodref = fft(&nodd);

	float_complex_vector_t res(s);

	for (int k = 0; k < s / 2; k++) {
		res[k] = odref[k] + (val[k] * nodref[k]);
		res[k + s / 2] = odref[k] - (val[k] * nodref[k]);
	}
	return res;
}

float_complex_vector_t ifft(float_complex_vector_t *vecz)
{
	float_complex_vector_t::iterator it;
	float_complex_vector_t test;
	for (it = vecz->begin(); it != vecz->end(); ++it) {
		*it = std::conj(*it);
	}

	test = *vecz;
	test = fft(vecz);

	for (it = test.begin(); it != test.end(); ++it) {
		*it = std::conj(*it);
	}

	for (it = test.begin(); it != test.end(); ++it) {
		*it /= test.size();
	}

	return test;
}

#define FFT_SAMPLES 1024

struct MainWindow::Private {
	QList<QAudioDeviceInfo> audio_input_devices;
	QAudioFormat audio_format;
	std::shared_ptr<QAudioInput> audio_input;
	QIODevice *input_device = nullptr;

	std::vector<uint8_t> buffer;
	int record_bytes = 0;
	int play_bytes = 0;

	int count = 0;

	int16_t samples[FFT_SAMPLES];
	int sample_count = 0;
};

MainWindow::MainWindow(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::MainWindow)
	, m(new Private)
{
	ui->setupUi(this);

	auto flags = windowFlags();
	flags &= ~Qt::WindowContextHelpButtonHint;
	setWindowFlags(flags);

	m->audio_format.setByteOrder(QAudioFormat::LittleEndian);
	m->audio_format.setChannelCount(2);
	m->audio_format.setCodec("audio/pcm");
	m->audio_format.setSampleRate(48000);
	m->audio_format.setSampleSize(16);
	m->audio_format.setSampleType(QAudioFormat::SignedInt);

	m->audio_input_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
	for (QAudioDeviceInfo const &dev : m->audio_input_devices) {
		QString name = dev.deviceName();
		ui->comboBox_input->addItem(name, name);
	}

	m->audio_input = std::shared_ptr<QAudioInput>(new QAudioInput(m->audio_format));
	m->input_device = m->audio_input->start();

	connect(m->input_device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

MainWindow::~MainWindow()
{
	delete m;
	delete ui;
}

void MainWindow::setLevel(int16_t const *p, int n, LevelMeterWidget *w)
{
	int v = 0;
	for (int i = 0; i < n; i++) {
		v = std::max(v, abs(p[i]));
	}
	float percent = v / 32768.0;
	w->setPercent(percent);
}

void MainWindow::setInputLevel(int16_t const *p, int n)
{
	setLevel(p, n, ui->widget_input_level);
}

void MainWindow::updateSpectrum(int16_t const *data, int size)
{
	float_complex_vector_t v;
	v.resize(size);
	for (int i = 0; i < size; i++) {
		v[i] = std::complex<float>(0, data[i]);
	}
	float_complex_vector_t t = fft(&v);
	std::vector<float> w;
	const int N = size / 2;
	w.resize(N);
	for (int i = 0; i < N; i++) {
		float r = t[i].real();
		float m = t[i].imag();
		w[i] = sqrt(r * r + m * m);
	}
	ui->widget_wave->setData(w);
}

void MainWindow::onReadyRead()
{
	QByteArray ba = m->input_device->readAll();
	if (ba.size() >= 2) {
		setInputLevel((int16_t const *)ba.data(), ba.size() / 2);
	}

	bool f = false;
	const int N = ba.size() / 4;
	int16_t const *p = (int16_t const *)ba.data();
	for (int i = 0; i < N; i++) {
		int16_t v = p[i * 2];
		m->samples[m->sample_count] = v;
		m->sample_count++;
		if (m->sample_count >= FFT_SAMPLES) {
			m->sample_count = 0;
			f = true;
			break;
		}
	}
	if (f) {
		updateSpectrum(m->samples, FFT_SAMPLES);
	}
}

void MainWindow::on_comboBox_input_currentIndexChanged(int index)
{
	if (m->audio_input) {
		disconnect(m->input_device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
		m->audio_input->stop();
		m->audio_input.reset();
	}

	if (index >= 0 && index < m->audio_input_devices.size()) {
		m->audio_input = std::shared_ptr<QAudioInput>(new QAudioInput(m->audio_input_devices[index], m->audio_format));
		m->input_device = m->audio_input->start();
		connect(m->input_device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	}
}

