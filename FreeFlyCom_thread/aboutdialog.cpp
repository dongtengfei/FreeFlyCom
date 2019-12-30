#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("关于软件"));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_BtnCloseAbout_clicked()
{
    this->close();
}
