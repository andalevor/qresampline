#include "MainWindow.hpp"
#include "./ui_MainWindow.h"
#include "ResampleDialog.hpp"
#include <QFileDialog>
#include <QGraphicsItem>
#include <QMessageBox>
#include <QTextStream>
#include <math.h>

#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , point_size(8)
    , selected(nullptr)
{
    ui->setupUi(this);
    ui->raw_table->setHorizontalHeaderLabels({ tr("Meters"), tr("Easting"), tr("Northing") });
    ui->raw_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->raw_table->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->raw_table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->processed_table->setHorizontalHeaderLabels({ tr("Meters"), tr("Easting"), tr("Northing") });
    ui->processed_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->processed_table->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->processed_table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->graphicsView->setScene(&scene);
    connect(ui->menuActionExit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
    connect(ui->menuActionOpen, SIGNAL(triggered()), this, SLOT(open_file()));
    connect(ui->raw_table, SIGNAL(itemSelectionChanged()), this, SLOT(raw_cell_selected()));
    connect(ui->processed_table, SIGNAL(itemSelectionChanged()), this, SLOT(processed_cell_selected()));
    connect(ui->actionResample_line, SIGNAL(triggered()), this, SLOT(run_dialog()));
    connect(ui->menuActionSave, SIGNAL(triggered()), this, SLOT(save_file()));
    ui->tabWidget->setTabVisible(1, false);
    // splitter hack
    QList<int> currentSizes = ui->splitter->sizes();
    currentSizes[0] += 350;
    currentSizes[1] += 250;
    ui->splitter->setSizes(currentSizes);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_file()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Choose file"));
    if (file_name.size() == 0)
        return;
    QFile in_file(file_name);
    if (!in_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, tr("Error"), tr("Can not open file"));
        return;
    }
    raw_points.clear();
    processed_points.clear();
    ui->raw_table->clearContents();
    ui->processed_table->clearContents();
    ui->tabWidget->setTabVisible(1, false);
    scene.clear();
    QTextStream in(&in_file);
    double x, y;
    char c;
    while (!in.atEnd()) {
        in >> x >> y;
        if (in.atEnd())
            return;
        raw_points.push_back(QPointF(x, y));
        in >> c;
        if (c != '\n' && c != '\r') {
            QMessageBox::critical(nullptr, tr("Error"), tr("There are more than 2 numbers per row"));
            return;
        }
    }
    if (raw_points.size() < 2) {
        QMessageBox::critical(nullptr, tr("Error"), tr("There are less than 2 points in file"));
        return;
    }
    x_min = x_max = raw_points[0].x();
    y_min = y_max = raw_points[0].y();
    for (QPointF& p : raw_points) {
        if (p.x() < x_min)
            x_min = p.x();
        if (p.x() > x_max)
            x_max = p.x();
        if (p.y() < y_min)
            y_min = p.y();
        if (p.y() > y_max)
            y_max = p.y();
    }
    QPen pen((QColor(Qt::GlobalColor::black)));
    QBrush brush(Qt::GlobalColor::black);
    for (int i = 0; i < raw_points.size(); ++i) {
        ui->raw_table->insertRow(i);
        double x = raw_points[i].x();
        double y = raw_points[i].y();
        double m = 0.0;
        if (i) {
            m = ui->raw_table->item(i - 1, 0)->data(0).toDouble();
            m += sqrt((raw_points[i - 1].x() - x) * (raw_points[i - 1].x() - x) + (raw_points[i - 1].y() - y) * (raw_points[i - 1].y() - y));
        }
        QTableWidgetItem* m_table_item = new QTableWidgetItem(QString::number(m));
        ui->raw_table->setItem(i, 0, m_table_item);
        QTableWidgetItem* x_table_item = new QTableWidgetItem(QString::number(x));
        ui->raw_table->setItem(i, 1, x_table_item);
        QTableWidgetItem* y_table_item = new QTableWidgetItem(QString::number(y));
        ui->raw_table->setItem(i, 2, y_table_item);
        QGraphicsItem* point_item = scene.addEllipse(raw_points[i].x() - x_min - point_size / 2,
            y_min - raw_points[i].y() - point_size / 2, point_size, point_size, pen, brush);
        raw_points_map[qMakePair(x_table_item, y_table_item)] = point_item;
        if (i != raw_points.size() - 1)
            scene.addLine(raw_points[i].x() - x_min, y_min - raw_points[i].y(),
                raw_points[i + 1].x() - x_min, y_min - raw_points[i + 1].y());
    }
}

void MainWindow::raw_cell_selected()
{
    QList<QTableWidgetItem*> list = ui->raw_table->selectedItems();
    if (selected != nullptr) {
        scene.removeItem(selected);
        delete selected;
    }
    QPen pen((QColor(Qt::GlobalColor::red)));
    QBrush brush(Qt::GlobalColor::red);
    int row = list.front()->row();
    QTableWidgetItem* x_widget = ui->raw_table->item(row, 1);
    QTableWidgetItem* y_widget = ui->raw_table->item(row, 2);
    QGraphicsItem* new_item = scene.addEllipse(x_widget->data(0).toDouble() - x_min - point_size,
        y_min - y_widget->data(0).toDouble() - point_size, point_size * 2, point_size * 2, pen, brush);
    selected = new_item;
}

void MainWindow::processed_cell_selected()
{
    QList<QTableWidgetItem*> list = ui->processed_table->selectedItems();
    if (selected != nullptr) {
        scene.removeItem(selected);
        delete selected;
    }
    QPen pen((QColor(Qt::GlobalColor::red)));
    QBrush brush(Qt::GlobalColor::red);
    int row = list.front()->row();
    QTableWidgetItem* x_widget = ui->processed_table->item(row, 1);
    QTableWidgetItem* y_widget = ui->processed_table->item(row, 2);
    QGraphicsItem* new_item = scene.addEllipse(x_widget->data(0).toDouble() - x_min - point_size,
        y_min - y_widget->data(0).toDouble() - point_size, point_size * 2, point_size * 2, pen, brush);
    selected = new_item;
}

void MainWindow::run_dialog()
{
    ResampleDialog dialog;
    int code = dialog.exec();
    if (!code)
        exit(1);
    resampline(dialog.start(), dialog.end(), dialog.step());
    start = dialog.start();
    end = dialog.end();
    step = dialog.step();
    draw_processed_points();
}

void MainWindow::resampline(double start, double end, double step)
{
    double first_x, first_y, second_x, second_y, azimuth, length = 0, dx, dy, x, y,
                                                          third_x, third_y, alpha, beta, gamma, a, b, c, total = 0, next_length;
    int steps;
    decltype(raw_points.size()) counter = 0;

    //if start is far from first point we need find line with start point
    do {
        start -= length;
        first_x = raw_points[counter].x();
        first_y = raw_points[counter].y();
        ++counter;
        second_x = raw_points[counter].x();
        second_y = raw_points[counter].y();
        length = sqrt(pow(first_x - second_x, 2) + pow(first_y - second_y, 2));
    } while (length - start <= 0 && counter + 1 < raw_points.size());
    length -= start;
    x = first_x;
    y = first_y;
    //find azimuth for first line
    azimuth = atan2(second_y - first_y, second_x - first_x);
    if (start) {
        x += cos(azimuth) * start;
        y += sin(azimuth) * start;
    }
    processed_points.push_back(QPointF(x, y));
    steps = length / step;
    dx = cos(azimuth) * step;
    dy = sin(azimuth) * step;
    for (int i = 0; i < steps; ++i) {
        x += dx;
        y += dy;
        processed_points.push_back(QPointF(x, y));
        total += step;
    }
    for (decltype(raw_points.size()) i = counter + 1; i < raw_points.size(); ++i) {
        third_x = raw_points[i].x();
        third_y = raw_points[i].y();
        //consider a triangle with sides a, b, c and angles alpha, beta, gamma
        //b is what left on first line, a -- step
        //we need to find c -- part of second line from begining
        //where we need to place next point
        a = step;
        b = length - steps * step;
        assert(b < a); //b should be never more then a
        //find angle between lines as difference of their angles
        //which we can find from their slopes
        alpha = fabs(atan2(first_y - second_y, first_x - second_x) - atan2(third_y - second_y, third_x - second_x));
        //alpha could be more than 180 degrees
        if (alpha > M_PI)
            alpha = 2 * M_PI - alpha;
        //from sines theorem
        beta = asin(b / a * sin(alpha));
        //sum of triangles angles is 180
        gamma = M_PI - alpha - beta;
        //from sines theorem
        c = a * sin(gamma) / sin(alpha);
        next_length = sqrt(pow(third_x - second_x, 2) + pow(third_y - second_y, 2));
        if (c > next_length) {
            steps = 0;
            length = sqrt(pow(third_x - x, 2) + pow(third_y - y, 2));
            first_x = x;
            first_y = y;
            second_x = third_x;
            second_y = third_y;
            continue;
        }
        //find dx and dy for first point
        azimuth = atan2(third_y - second_y, third_x - second_x);
        dx = cos(azimuth) * c;
        dy = sin(azimuth) * c;
        x = second_x + dx;
        y = second_y + dy;
        processed_points.push_back(QPointF(x, y));
        total += step;
        length = next_length - c;
        if (end && i == raw_points.size() - 1)
            length = end - start - total;
        steps = length / step;
        dx = cos(azimuth) * step;
        dy = sin(azimuth) * step;
        for (int i = 0; i < steps; ++i) {
            x += dx;
            y += dy;
            processed_points.push_back(QPointF(x, y));
            total += step;
        }
        first_x = second_x;
        first_y = second_y;
        second_x = third_x;
        second_y = third_y;
    }
    if (end) {
        while (total < end - start) {
            x += dx;
            y += dy;
            processed_points.push_back(QPointF(x, y));
            total += step;
        }
    }
}

void MainWindow::draw_processed_points()
{
    QPen pen((QColor(Qt::GlobalColor::yellow)));
    QBrush brush(Qt::GlobalColor::yellow);
    for (int i = 0; i < processed_points.size(); ++i) {
        ui->processed_table->insertRow(i);
        double x = processed_points[i].x();
        double y = processed_points[i].y();
        double m = 0.0;
        if (i) {
            m = ui->processed_table->item(i - 1, 0)->data(0).toDouble();
            m += sqrt((processed_points[i - 1].x() - x) * (processed_points[i - 1].x() - x) + (processed_points[i - 1].y() - y) * (processed_points[i - 1].y() - y));
        }
        QTableWidgetItem* m_table_item = new QTableWidgetItem(QString::number(m));
        ui->processed_table->setItem(i, 0, m_table_item);
        QTableWidgetItem* x_table_item = new QTableWidgetItem(QString::number(x));
        ui->processed_table->setItem(i, 1, x_table_item);
        QTableWidgetItem* y_table_item = new QTableWidgetItem(QString::number(y));
        ui->processed_table->setItem(i, 2, y_table_item);
        QGraphicsItem* point_item = scene.addEllipse(processed_points[i].x() - x_min - point_size / 4,
            y_min - processed_points[i].y() - point_size / 4, point_size / 2, point_size / 2, pen, brush);
        processed_points_map[qMakePair(x_table_item, y_table_item)] = point_item;
    }
    ui->tabWidget->setTabVisible(1, true);
}

void MainWindow::save_file()
{
    if (processed_points.empty()) {
        QMessageBox::information(nullptr, tr("Information"), tr("Nothing to save"));
        return;
    }
    QString file_name = QFileDialog::getSaveFileName(this, tr("Choose file"));
    if (file_name.size() == 0)
        return;
    QFile out_file(file_name);
    if (!out_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, tr("Error"), tr("Can not open file"));
        return;
    }
    QTextStream out(&out_file);
    out.setRealNumberPrecision(1);
    out.setRealNumberNotation(QTextStream::FixedNotation);
    QVector<int> field_length;
    field_length.push_back(QString::number(processed_points.size()).length());
    field_length.push_back(QString::number(static_cast<long>((processed_points.size() - 1) * step)).length() + 3);
    field_length.push_back(QString::number(static_cast<long>(processed_points.back().x())).length() + 3);
    field_length.push_back(QString::number(static_cast<long>(processed_points.back().y())).length() + 3);
    decltype(processed_points.size()) counter = 0;
    decltype(start) curr = start;
    for (const auto& v : processed_points) {
        out.setFieldWidth(field_length[0]);
        out << " " << ++counter;
        out.setFieldWidth(field_length[1]);
        out << " " << curr;
        out.setFieldWidth(field_length[2]);
        out << " " << v.x();
        out.setFieldWidth(field_length[3]);
        out << " " << v.y() << Qt::endl;
        curr += step;
    }
}
