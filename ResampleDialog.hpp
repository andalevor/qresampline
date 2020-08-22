#ifndef RESAMPLEDIALOG_HPP
#define RESAMPLEDIALOG_HPP

#include <QDialog>

namespace Ui {
class ResampleDialog;
}

class ResampleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResampleDialog(QWidget *parent = nullptr);
    ~ResampleDialog();
    double start();
    double end();
    double step();

private:
    Ui::ResampleDialog *ui;
};

#endif // RESAMPLEDIALOG_HPP
