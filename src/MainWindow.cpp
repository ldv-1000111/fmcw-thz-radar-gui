#include "MainWindow.hpp"
#include "RadarEngine.hpp"
#include "ParameterPanel.hpp"
#include "PlotWidget.hpp"
#include "ui_MainWindow.h"

#include <QDockWidget>
#include <QLabel>
#include <QMessageBox>
#include <QStatusBar>
#include <QString>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(std::make_unique<Ui::MainWindow>())
    , m_engine(std::make_unique<fmcw_gui::RadarEngine>())
{
    m_ui->setupUi(this);
    setWindowTitle("FMCW THz Radar Simulator");
    resize(1280, 800);

    setupLayout();
    connectSignals();

    // Trigger initial render
    m_engine->compute();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupLayout()
{
    // --- Parameter panel in a left dock ---
    m_paramPanel = new fmcw_gui::ParameterPanel(this);
    auto* dock = new QDockWidget("Parameters", this);
    dock->setWidget(m_paramPanel);
    dock->setFeatures(QDockWidget::DockWidgetMovable |
                      QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    // --- Plot area as central widget ---
    m_plotWidget = new fmcw_gui::PlotWidget(this);
    setCentralWidget(m_plotWidget);

    // Seed the panel with engine defaults
    m_paramPanel->setParams(m_engine->params());
    m_paramPanel->setTarget(m_engine->target());
}

void MainWindow::connectSignals()
{
    connect(m_paramPanel, &fmcw_gui::ParameterPanel::paramsChanged,
            this, &MainWindow::onParamsChanged);

    connect(m_engine.get(), &fmcw_gui::RadarEngine::signalReady,
            this, &MainWindow::onSignalReady);

    connect(m_engine.get(), &fmcw_gui::RadarEngine::errorOccurred,
            this, &MainWindow::onEngineError);
}

void MainWindow::onParamsChanged()
{
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
    statusBar()->showMessage(
        QString("f₀ %1 GHz  |  Δr %2 cm  |  f_beat %3 kHz  |  f_D %4 Hz  |  SNR est. —")
            .arg(m_engine->params().f0 / 1.0e9, 0, 'f', 1)
            .arg(m_engine->rangeResM() * 100.0,  0, 'f', 2)
            .arg(m_engine->beatFreqHz() / 1.0e3, 0, 'f', 2)
            .arg(m_engine->dopplerHz(),           0, 'f', 1)
    );
}
