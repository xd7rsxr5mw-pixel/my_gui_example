#include "calculator.hpp"
#include <QtTest>
#include <limits>
#include <stdexcept>

class BackendTest : public QObject {
  Q_OBJECT
private slots:
  void additionRejectsInvalidNumbers() {
    const showcase::Calculator calculator;
    QCOMPARE(calculator.add(2, 3), 5.0);
    QCOMPARE(calculator.add(-2.5, 0.5), -2.0);
    QVERIFY_EXCEPTION_THROWN(
        calculator.add(std::numeric_limits<double>::infinity(), 1),
        std::invalid_argument);
    QVERIFY_EXCEPTION_THROWN(calculator.add(std::numeric_limits<double>::max(),
                                            std::numeric_limits<double>::max()),
                             std::overflow_error);
  }
  void reproducibleSignalAndWorkerCalculation() {
    const showcase::Calculator calculator;
    const auto points = calculator.curve(1, 0.5, 1001);
    QCOMPARE(points.size(), std::size_t(1001));
    QCOMPARE(points.front().x, 0.0);
    QCOMPARE(points.back().x, 10.0);
    QVERIFY(std::abs(calculator.sample(0.5).y - 1) < 1e-12);
    QVERIFY(std::abs(calculator.meanSquare({}, 10000) - 0.5) < 1e-9);
    std::stop_source stopped;
    stopped.request_stop();
    QCOMPARE(calculator.meanSquare(stopped.get_token()), 0.0);
    QVERIFY_EXCEPTION_THROWN(calculator.meanSquare({}, 0),
                             std::invalid_argument);
  }
};
QTEST_APPLESS_MAIN(BackendTest)
#include "tst_Backend.moc"
