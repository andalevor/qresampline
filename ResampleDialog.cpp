#include "ResampleDialog.hpp"
#include "ui_ResampleDialog.h"

ResampleDialog::ResampleDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ResampleDialog)
{
    ui->setupUi(this);
}

ResampleDialog::~ResampleDialog()
{
    delete ui;
}

double ResampleDialog::start()
{
    return ui->startSpin->value();
}

double ResampleDialog::end()
{
    return ui->endSpin->value();
}

double ResampleDialog::step()
{
    return ui->stepSpin->value();
}
