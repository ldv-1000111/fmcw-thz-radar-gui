#include "MainWindow.hpp"
#include "RadarEngine.hpp"
#include "ParameterPanel.hpp"
#include "PlotWidget.hpp"
#include "SliderBar.hpp"
#include "ui_MainWindow.h"

#include <QDockWidget>
#include <QMessageBox>
#include <QFrame>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
// Log scale helpers (same mapping as SliderBar / ParameterPanel)
// ---------------------------------------------------------------------------
static int gHzToLogInt(double ghz)
{
    const double t = (std::log10(ghz) - std::log10(10.0))
                   / (std::log10(10000.0) - std::log10(10.0));
    return qBound(1, static_cast<int>(std::round(1.0 + t * 999.0)), 1000);
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(std::make_unique<Ui::MainWindow>())
    , m_engine(std::make_unique<fmcw_gui::RadarEngine>())
{
    m_ui->setupUi(this);
    setWindowTitle("FMCW THz Radar Simulator");
    resize(1280, 820);

    setupLayout();
    connectSignals();
    m_engine->compute();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupLayout()
{
    // ── Left dock: parameter spin-boxes ─────────────────────────────────
    m_paramPanel = new fmcw_gui::ParameterPanel(this);
    auto* dock = new QDockWidget("Parameters", this);
    dock->setWidget(m_paramPanel);
    dock->setFeatures(QDockWidget::DockWidgetMovable |
                      QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    // ── Central: plot widget ─────────────────────────────────────────────
    m_plotWidget = new fmcw_gui::PlotWidget(this);

    // ── Bottom: slider bar ───────────────────────────────────────────────
    m_sliderBar = new fmcw_gui::SliderBar(this);

    // Add all sliders — radar system
    m_sliderBar->addSlider({"f0",      "f₀",      "GHz",  1,    1000, gHzToLogInt(300.0), 1.0,  1, true});
    m_sliderBar->addSlider({"bw",      "B",       "GHz",  1,    500,  4,                  1.0,  1});
    m_sliderBar->addSlider({"tc",      "Tc",      "µs",   1,    500,  100,                1.0,  0});
    m_sliderBar->addSlider({"fs",      "fs",      "MHz",  1,    500,  50,                 1.0,  0});
    // Target
    m_sliderBar->addSlider({"range",   "Range",   "m",    1,    300,  50,                 1.0,  1});
    m_sliderBar->addSlider({"vel",     "Vel",     "m/s",  -300, 300,  0,                  0.1,  1});
    m_sliderBar->addSlider({"vibamp",  "Vib amp", "mm",   0,    500,  2,                  0.1,  2});
    m_sliderBar->addSlider({"vibfreq", "Vib Hz",  "Hz",   0,    500,  200,                1.0,  0});

    // ADAS band on f0
    m_sliderBar->setAdasBand(gHzToLogInt(100.0), gHzToLogInt(330.0),
                             QColor("#1D9E75"));

    // Wrap plot + slider bar in a vertical layout as central widget
    auto* central = new QWidget(this);
    auto* vbox    = new QVBoxLayout(central);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(0);
    vbox->addWidget(m_plotWidget, 1);

    // Separator line above slider bar
    auto* sep = new QFrame(central);
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);
    vbox->addWidget(sep);
    vbox->addWidget(m_sliderBar, 0);

    setCentralWidget(central);

    // Seed panel with engine defaults
    m_paramPanel->setParams(m_engine->params());
    m_paramPanel->setTarget(m_engine->target());
}

void MainWindow::connectSignals()
{
    connect(m_paramPanel, &fmcw_gui::ParameterPanel::paramsChanged,
            this, &MainWindow::onParamsChanged);

    connect(m_sliderBar, &fmcw_gui::SliderBar::valueChanged,
            this, &MainWindow::onSliderChanged);

    connect(m_engine.get(), &fmcw_gui::RadarEngine::signalReady,
            this, &MainWindow::onSignalReady);

    connect(m_engine.get(), &fmcw_gui::RadarEngine::errorOccurred,
            this, &MainWindow::onEngineError);
}

void MainWindow::onParamsChanged()
{
    m_engine->setParams(m_paramPanel->params());
    m_engine->setTarget(m_paramPanel->target());

    // Keep sliders in sync with spinbox edits
    const auto p = m_paramPanel->params();
    const auto t = m_paramPanel->target();
    m_sliderBar->setValue("f0",      p.f0 / 1.0e9);
    m_sliderBar->setValue("bw",      p.bandwidth / 1.0e9);
    m_sliderBar->setValue("tc",      p.chirp_time * 1.0e6);
    m_sliderBar->setValue("fs",      p.fs / 1.0e6);
    m_sliderBar->setValue("range",   t.range);
    m_sliderBar->setValue("vel",     t.velocity);
    m_sliderBar->setValue("vibamp",  t.vib_amp * 1.0e3);
    m_sliderBar->setValue("vibfreq", t.vib_freq);

    m_engine->compute();
}

void MainWindow::onSliderChanged(const QString& id, double value)
{
    // Sync spinbox, which will emit paramsChanged → onParamsChanged
    // but we must not double-trigger compute, so sync directly here
    m_paramPanel->syncFromSlider(id, value);
    m_engine->setParams(m_paramPanel->params());
    m_engine->setTarget(m_paramPanel->target());
    m_engine->compute();
}

void MainWindow::onSignalReady()
{
    m_plotWidget->updateIfSignal(m_engine->timeAxisUs(),
                                  m_engine->ifSignalReal());
    m_plotWidget->updateSpectrum(m_engine->rangeAxisM(),
                                  m_engine->rangeSpectrumDb());
    updateMetrics();
}

void MainWindow::onEngineError(const QString& msg)
{
    QMessageBox::critical(this, "Engine error", msg);
}

void MainWindow::updateMetrics()
{
    const auto p = m_engine->params();
    statusBar()->showMessage(
        QString("f₀ %1 GHz  |  Δr %2 cm  |  f_beat %3 kHz  |  f_D %4 Hz  |  SNR est. —")
            .arg(p.f0 / 1.0e9,              0, 'f', 1)
            .arg(m_engine->rangeResM()*100,  0, 'f', 2)
            .arg(m_engine->beatFreqHz()/1e3, 0, 'f', 2)
            .arg(m_engine->dopplerHz(),      0, 'f', 1));
}
