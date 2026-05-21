#include "RadarEngine.hpp"

#include <cmath>
#include <stdexcept>

static constexpr double C = 3.0e8;

namespace fmcw_gui {

RadarEngine::RadarEngine(QObject* parent)
    : QObject(parent)
{
    // Sensible defaults matching the engine repo's Phase 1 parameters
    m_params.f0          = 300.0e9;   // 300 GHz
    m_params.bandwidth   = 4.0e9;     //   4 GHz
    m_params.chirp_time  = 100.0e-6;  // 100 µs
    m_params.fs          = 50.0e6;    //  50 MHz  → 5000 samples/chirp

    m_target.range       = 50.0;      // m
    m_target.velocity    = 0.0;       // m/s
    m_target.vib_amp     = 2.0e-4;    // 0.2 mm
    m_target.vib_freq    = 200.0;     // Hz  (engine idle)
}

void RadarEngine::setParams(const RadarParams& p)  { m_params = p; }
void RadarEngine::setTarget(const Target& t)        { m_target = t; }

// ---------------------------------------------------------------------------
// Derived quantities
// ---------------------------------------------------------------------------
double RadarEngine::wavelengthM()    const { return C / m_params.f0; }
double RadarEngine::chirpSlopeTHzS() const { return m_params.bandwidth / m_params.chirp_time; }
double RadarEngine::beatFreqHz()     const
{
    const double tau = 2.0 * m_target.range / C;
    return chirpSlopeTHzS() * tau + 2.0 * m_params.f0 * m_target.velocity / C;
}
double RadarEngine::dopplerHz()      const { return 2.0 * m_target.velocity * m_params.f0 / C; }
double RadarEngine::rangeResM()      const { return C / (2.0 * m_params.bandwidth); }
double RadarEngine::velResMps()      const { return wavelengthM() / (2.0 * m_params.chirp_time); }
double RadarEngine::maxRangeM()      const { return m_params.fs * C / (2.0 * chirpSlopeTHzS()); }
double RadarEngine::maxVelMps()      const { return wavelengthM() / (4.0 * m_params.chirp_time); }

// ---------------------------------------------------------------------------
// Compute
// ---------------------------------------------------------------------------
void RadarEngine::compute()
{
    try {
        m_ifBuf = generate_chirp_if(m_params, m_target);
        buildSpectrum();
        emit signalReady();
    } catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

QVector<double> RadarEngine::ifSignalReal() const
{
    QVector<double> v;
    v.reserve(static_cast<int>(m_ifBuf.size()));
    for (const auto& s : m_ifBuf) v.append(s.real());
    return v;
}

QVector<double> RadarEngine::ifSignalImag() const
{
    QVector<double> v;
    v.reserve(static_cast<int>(m_ifBuf.size()));
    for (const auto& s : m_ifBuf) v.append(s.imag());
    return v;
}

QVector<double> RadarEngine::timeAxisUs() const
{
    const int N = static_cast<int>(m_ifBuf.size());
    QVector<double> t(N);
    const double dt_us = m_params.chirp_time * 1.0e6 / N;
    for (int i = 0; i < N; ++i) t[i] = i * dt_us;
    return t;
}

// ---------------------------------------------------------------------------
// Spectrum (DFT — direct, N ≤ 5000 is fast enough for real-time UI)
// ---------------------------------------------------------------------------
void RadarEngine::buildSpectrum()
{
    const int N = static_cast<int>(m_ifBuf.size());
    const int half = N / 2;
    const double mu = chirpSlopeTHzS();

    m_specDb.resize(half);
    m_rangeM.resize(half);

    const double df = 1.0 / m_params.chirp_time;

    for (int k = 0; k < half; ++k) {
        double re = 0.0, im = 0.0;
        for (int n = 0; n < N; ++n) {
            const double angle = 2.0 * M_PI * k * n / N;
            re += m_ifBuf[n].real() * std::cos(angle)
                - m_ifBuf[n].imag() * std::sin(angle);
            im += m_ifBuf[n].real() * std::sin(angle)
                + m_ifBuf[n].imag() * std::cos(angle);
        }
        const double mag = std::sqrt(re * re + im * im) / N;
        m_specDb[k] = 20.0 * std::log10(mag + 1.0e-12);
        m_rangeM[k] = k * df * C / (2.0 * mu);
    }
}

QVector<double> RadarEngine::rangeSpectrumDb() const
{
    return QVector<double>(m_specDb.begin(), m_specDb.end());
}

QVector<double> RadarEngine::rangeAxisM() const
{
    return QVector<double>(m_rangeM.begin(), m_rangeM.end());
}

}  // namespace fmcw_gui
