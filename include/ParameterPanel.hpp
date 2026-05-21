#pragma once

#include <QWidget>
#include <memory>
#include "fmcw_generator.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class ParameterPanel; }
QT_END_NAMESPACE

namespace fmcw_gui {

/// Slider + spin-box panel for all RadarParams and Target fields.
/// Emits paramsChanged() whenever any control is modified.
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

signals:
    void paramsChanged();

private slots:
    void onAnyControlChanged();

private:
    std::unique_ptr<Ui::ParameterPanel> m_ui;
    bool m_blocking{false};

    void connectAll();
    void updateDerivedLabels();
};

}  // namespace fmcw_gui
