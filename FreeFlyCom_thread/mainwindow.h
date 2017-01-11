#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include "aboutdialog.h"
#include "mysendthread.h"
#include "myrecvthread.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_actionAbout_triggered();
    void qtimeSlot(void);
    void upshow_cnt(void);
    void SendMyThreadfinish(int);
    void RecvMyThreadfinish(QByteArray ,int);
    void on_actionOpenCom_triggered();
    void on_actionCloseCom_triggered();
    void readMyCom();
    void on_PushButton_SendDate_clicked();
    void on_pushButton_ReceiveClear_clicked();
    void on_pushButton__SendClear_clicked();

    void on_PushButton_ClearReviceText_clicked();

    void on_PushButton_ClearSendText_clicked();

    void on_RadioButton_SendChar_clicked();
    void on_RadioButton_SendHex_clicked();


    void on_RadioButton_RecvChar_clicked();

    void on_RadioButton_RecvHex_clicked();

    void on_actionClear_triggered();

    void on_actionAddCom_triggered();

    void on_actionDelCom_triggered();

    void on_action_FindCom_triggered();

    void on_CheckBox_OBO_clicked();

private:
    Ui::MainWindow *ui;
    AboutDialog aboutdlg;
    void init_statusBar();
    QTimer *timer10ms;
    QTimer *systimer;
    void init_timeShow();
    void init_FindPort();
    void init_SetPort(QSerialPort *pserial);
    bool openPort(QSerialPort *pserial);
    QString currentPortName ;
    QSerialPort *pserial;
    qint32 obotime ;
    qint32  Sendlen;
    qint32  Recvlen;
    bool isPortOpen;
    bool isRecvChar;
    bool isSendChar;
    void sendMsg(bool);
//  void readMyCom();
    void setComboBox(bool status);
    void setAction(bool status);
    SendMyThread* mySendThread;
//    RecvMyThread* myRecvThread;
};

#endif // MAINWINDOW_H
