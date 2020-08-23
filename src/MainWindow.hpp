#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QGraphicsScene>
#include <QMainWindow>
#include <QPointF>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    void resampline(double start, double end, double step);
    void draw_processed_points();
    Ui::MainWindow* ui;
    QGraphicsScene scene;
    QVector<QPointF> raw_points;
    QVector<QPointF> processed_points;
    QMap<QPair<QTableWidgetItem*, QTableWidgetItem*>, QGraphicsItem*> raw_points_map;
    QMap<QPair<QTableWidgetItem*, QTableWidgetItem*>, QGraphicsItem*> processed_points_map;
    double x_min, x_max, y_min, y_max, start, end, step;
    int point_size;
    QGraphicsItem* selected;

public slots:
    void open_file();
    void raw_cell_selected();
    void processed_cell_selected();
    void run_dialog();
    void save_file();
};
#endif // MAINWINDOW_HPP
