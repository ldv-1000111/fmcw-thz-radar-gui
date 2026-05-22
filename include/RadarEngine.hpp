#pragma once

#include <QObject>
#include <QVector>
#include <complex>

#include "fmcw_generator.hpp"   // RadarParams, Target, generate_chirp_if()

namespace fmcw_gui {

/// Thin Qt wrapper around the Phase 1 engine.
///
/// Call setParams() / setTarget() to update state, then compute() to
/// run generate_chirp_if(). Results are exposed as QVectors ready for
/// QtCharts series data.  All heavy computation runs on the calling
/// thread — move to a QThread if you need a responsive UI during
/// long sweeps.
class RadarEngine : public QObject
{
    Q_OBJECT

public:
    explicit RadarEngine(QObject* parent = nullptr);

    // --- configuration -------------------------------------------------------
    void setParams(const RadarParams& p);
    void setTarget(const Target& t);

    [[nodiscard]] RadarParams params() const { return m_params; }
    [[nodiscard]] Target      target() const { return m_target; }

    // --- derived scalar quantities -------------------------------------------
    [[nodiscard]] double beatFreqHz()    const;   // f_beat = (B/Tc)*tau
    [[nodiscard]] double dopplerHz()     const;   // f_d   = 2*v*f0/c
    [[nodiscard]] double rangeResM()     const;   // Δr    = c/2B
    [[nodiscard]] double velResMps()     const;   // Δv    = λ/2Tc
    [[nodiscard]] double maxRangeM()     const;   // R_max = fs*c/(2μ)
    [[nodiscard]] double maxVelMps()     const;   // v_max = λ/4Tc
    [[nodiscard]] double wavelengthM()   const;   // λ     = c/f0
    [[nodiscard]] double chirpSlopeTHzS() const;  // μ     = B/Tc  [THz/s]

    // --- signal computation --------------------------------------------------

    /// Run generate_chirp_if() and cache the result.
    /// Emits signalReady() when done.
    void compute();

    /// Time-domain IF samples (real part only, for display).
    [[nodiscard]] QVector<double> ifSignalReal()  const;
    [[nodiscard]] QVector<double> ifSignalImag()  const;
    [[nodiscard]] QVector<double> timeAxisUs()    const;

    /// Single-sided magnitude spectrum in dB, x-axis in metres.
    [[nodiscard]] QVector<double> rangeSpectrumDb() const;
    [[nodiscard]] QVector<double> rangeAxisM()      const;

signals:
    void signalReady();
    void errorOccurred(const QString& message);

private:
    RadarParams                        m_params{};
    Target                             m_target{};
    std::vector<std::complex<float>>   m_ifBuf{};

    void buildSpectrum();

    std::vector<double> m_specDb{};
    std::vector<double> m_rangeM{};
};

}  // namespace fmcw_gui
