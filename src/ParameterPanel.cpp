#include "ParameterPanel.hpp"
#include "ui_ParameterPanel.h"

#include <cmath>

static constexpr double C = 3.0e8;

namespace fmcw_gui {

ParameterPanel::ParameterPanel(QWidget* parent)
    : QWidget(parent)
    , m_ui(std::make_unique<Ui::ParameterPanel>())
{
    m_ui->setupUi(this);
    connectAll();
}

ParameterPanel::~ParameterPanel() = default;

// ---------------------------------------------------------------------------
// Public setters — push engine defaults into controls at startup
// ---------------------------------------------------------------------------
void ParameterPanel::setParams(const RadarParams& p)
{
    m_blocking = true;
    m_ui->spinF0_GHz->setValue(p.f0 / 1.0e9);
    m_ui->spinBW_GHz->setValue(p.bandwidth / 1.0e9);
    m_ui->spinTc_us->setValue(p.chirp_time * 1.0e6);
    m_ui->spinFs_MHz->setValue(p.fs / 1.0e6);
    m_blocking = false;
    updateDerivedLabels();
}

void ParameterPanel::setTarget(const Target& t)
{
    m_blocking = true;
    m_ui->spinRange_m->setValue(t.range);
    m_ui->spinVel_mps->setValue(t.velocity);
    m_ui->spinVibAmp_mm->setValue(t.vib_amp * 1.0e3);
    m_ui->spinVibFreq_Hz->setValue(t.vib_freq);
    m_blocking = false;
}

// ---------------------------------------------------------------------------
// Public getters — read current control state back to engine structs
// ---------------------------------------------------------------------------
RadarParams ParameterPanel::params() const
{
    RadarParams p;
    p.f0         = m_ui->spinF0_GHz->value()  * 1.0e9;
    p.bandwidth  = m_ui->spinBW_GHz->value()  * 1.0e9;
    p.chirp_time = m_ui->spinTc_us->value()   * 1.0e-6;
    p.fs         = m_ui->spinFs_MHz->value()  * 1.0e6;
    p.num_samples = static_cast<int>(p.fs * p.chirp_time);
    p.num_chirps  = 256;   // fixed for Phase 1
    return p;
}

Target ParameterPanel::target() const
{
    Target t;
    t.range    = m_ui->spinRange_m->value();
    t.velocity = m_ui->spinVel_mps->value();
    t.rcs      = 1.0;   // unit reflectivity — fixed for now
    t.vib_amp  = m_ui->spinVibAmp_mm->value()  * 1.0e-3;
    t.vib_freq = m_ui->spinVibFreq_Hz->value();
    return t;
}

// ---------------------------------------------------------------------------
// Internal
// ---------------------------------------------------------------------------
void ParameterPanel::connectAll()
{
    // Connect every spin box to the same handler
    const auto spins = findChildren<QDoubleSpinBox*>();
    for (auto* s : spins)
        connect(s, &QDoubleSpinBox::valueChanged,
                this, &ParameterPanel::onAnyControlChanged);
}

void ParameterPanel::onAnyControlChanged()
{
    if (m_blocking) return;
    updateDerivedLabels();
    emit paramsChanged();
}

void ParameterPanel::updateDerivedLabels()
{
    const double f0  = m_ui->spinF0_GHz->value() * 1.0e9;
    const double B   = m_ui->spinBW_GHz->value() * 1.0e9;
    const double Tc  = m_ui->spinTc_us->value()  * 1.0e-6;
    const double lam = C / f0;
    const double dr  = C / (2.0 * B);
    const double dv  = lam / (2.0 * Tc);

    m_ui->lblLambda->setText(
        lam >= 1.0e-3
        ? QString("λ = %1 mm").arg(lam * 1000.0, 0, 'f', 2)
        : QString("λ = %1 µm").arg(lam * 1.0e6,  0, 'f', 1));

    m_ui->lblRangeRes->setText(
        QString("Δr = %1 cm").arg(dr * 100.0, 0, 'f', 2));

    m_ui->lblVelRes->setText(
        QString("Δv = %1 mm/s").arg(dv * 1000.0, 0, 'f', 2));
}

}  // namespace fmcw_gui
