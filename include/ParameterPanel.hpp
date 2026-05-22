#pragma once

#include <QWidget>
#include <QString>
#include <memory>
#include "fmcw_generator.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class ParameterPanel; }
QT_END_NAMESPACE

namespace fmcw_gui {

/// Spin-box only parameter panel (left dock).
/// Sliders live in SliderBar at the bottom of the main window.
/// MainWindow keeps both in sync via syncFromSlider().
class ParameterPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ParameterPanel(QWidget* parent = nullptr);
    ~ParameterPanel() override;

    void setParams(const RadarParams& p);
    void setTarget(const Target& t);

    [[nodiscard]] RadarParams params() const;
    [[nodiscard]] Target      target() const;

    /// Called by MainWindow when a SliderBar slider moves.
    /// Updates the matching spin box and emits paramsChanged().
    void syncFromSlider(const QString& id, double value);

signals:
    void paramsChanged();

private:
    std::unique_ptr<Ui::ParameterPanel> m_ui;
    bool m_blocking{false};

    void connectAll();
    void updateDerivedLabels();
};

}  // namespace fmcw_gui
