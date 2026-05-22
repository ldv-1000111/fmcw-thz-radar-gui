#pragma once

#include <QMainWindow>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace fmcw_gui {
class RadarEngine;
class ParameterPanel;
class PlotWidget;
class SliderBar;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onParamsChanged();
    void onSignalReady();
    void onEngineError(const QString& msg);
    void onSliderChanged(const QString& id, double value);

private:
    std::unique_ptr<Ui::MainWindow>         m_ui;
    std::unique_ptr<fmcw_gui::RadarEngine>  m_engine;
    fmcw_gui::ParameterPanel*               m_paramPanel{nullptr};
    fmcw_gui::PlotWidget*                   m_plotWidget{nullptr};
    fmcw_gui::SliderBar*                    m_sliderBar{nullptr};

    void setupLayout();
    void connectSignals();
    void updateMetrics();
};
