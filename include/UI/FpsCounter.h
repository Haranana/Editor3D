#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H
#include <chrono>
#include <QString>
class FpsCounter{
    public:
    FpsCounter(double alpha = defaultAlpha) : alpha(alpha){}

        void frame() {
            using clock = std::chrono::steady_clock;
            auto now = clock::now();
            if (!initialized) {
                last = now; initialized = true;
                return;
            }
            double dt_ms = std::chrono::duration<double, std::milli>(now - last).count();
            last = now;
            if (ema_ms < 0) ema_ms = dt_ms;
            else           ema_ms = alpha*dt_ms + (1.0 - alpha)*ema_ms;
        }

        double fps() const { return (ema_ms > 0) ? 1000.0 / ema_ms : 0.0; }
        double frameMs() const { return ema_ms; }

        QString text() const {
            return QStringLiteral("FPS: %1  (%2 ms)")
            .arg(fps(), 0, 'f', 1)
                .arg(frameMs(), 0, 'f', 1);
        }

    private:
        static constexpr double defaultAlpha = 0.05;
        double alpha;
        bool initialized = false;
        std::chrono::steady_clock::time_point last{};
        double ema_ms = -1.0;

};

#endif // FPSCOUNTER_H
