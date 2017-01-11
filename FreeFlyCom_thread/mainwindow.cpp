#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QTime>
#include <QDate>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QGridLayout>
#include <QTextEdit>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include "mysendthread.h"
//#include "recvmythread.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>

//关于软件
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("FreeFlyComV1.0"));
    setWindowIcon(QIcon(":/new/prefix1/Image/Show.png"));
    setComboBox(true); //设置所有的combobox为显示状态
    setAction(true);    //设置所有的action为显示模式
    isPortOpen = false; //设置串口为关闭模式
    isSendChar = true;  //是否发送字符
    isRecvChar = true;  //是否字符接受
    init_timeShow();    //初始化时间显示
    init_FindPort();    //查找串口
}


MainWindow::~MainWindow()
{
    delete ui;
}

//遍历查找串口
void MainWindow::init_FindPort()
{

    qint32 cnt = 0;
//  foreach (const QSerialPortInfo &info , QSerialPortInfo::availablePorts())
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        QString str ;
        str = info.portName();
        if(str.startsWith("cu.Bluetooth-Incoming-Port",Qt::CaseSensitive) == true)
        {
            continue;
        }
        if(str.startsWith("cu.",Qt::CaseSensitive) == true)
        {
            ui->ComboBox_SerialPortName->addItem(info.portName());
            ui->statusBar->showMessage("找到串口");
            cnt++;
        }

    }
    if(cnt == 0)
    {
        ui->statusBar->showMessage("没有找到可用的串口");
    }
}

//设置串口参数
void MainWindow::init_SetPort(QSerialPort* pserial)
{
    if(pserial == NULL)
    {
       return;
    }

    //设置波特率
    pserial->setBaudRate(ui->ComboBox_BaudRate->currentText().toInt(),QSerialPort::AllDirections);
    //设置数据位
    pserial->setDataBits((QSerialPort::DataBits)ui->ComboBox_DateBit->currentText().toInt());

    //设置校验
    switch(ui->ComboBox_CheckBit->currentIndex())
    {
        case 0:
             pserial->setParity(QSerialPort::NoParity);
             break;
        case 1:
            pserial->setParity(QSerialPort::OddParity);
            break;
        case 2:
            pserial->setParity(QSerialPort::EvenParity);
            break;
        default:
            pserial->setParity(QSerialPort::NoParity);
            qDebug("set to default : PAR_NONE");
            break;
    }
    //设置停止位
    switch(ui->ComboBox_StopBit->currentIndex())
    {
        case 0:
            pserial->setStopBits(QSerialPort::OneStop);
            break;
        case 1:
            pserial->setStopBits(QSerialPort::OneAndHalfStop);
            break;
        case 2:
            pserial->setStopBits(QSerialPort::TwoStop);
            break;
        default:
            pserial->setStopBits(QSerialPort::OneStop);
            break;
    }
    //设置数据流控制
    pserial->setFlowControl(QSerialPort::NoFlowControl);
    //设置延时
    //pserial->setTimeout(TIME_OUT);


}

bool MainWindow::openPort(QSerialPort*pserial)
{
    if(pserial == NULL)
    {
       return false;
    }

    if(pserial->open(QIODevice::ReadWrite))
    {
        isPortOpen = true;
        ui->statusBar->showMessage(tr("打开串口成功"));
        setComboBox(false);
        setAction(false);
        return true;
    }
    else
    {
        isPortOpen = false;
        QMessageBox::critical(this, NULL, tr("未能打开串口 ") + currentPortName + tr("\n该串口设备不存在或已被占用"), QMessageBox::Ok);
        return false;
    }

}
void MainWindow::init_timeShow(void)
{
    systimer = new QTimer(this);
    connect(systimer, SIGNAL(timeout()), this, SLOT(qtimeSlot())); //系统时间显示
    systimer->start(1000);

    timer10ms = new QTimer(this);
    connect(timer10ms, SIGNAL(timeout()), this, SLOT(upshow_cnt()));

}

void MainWindow::upshow_cnt(void)
{
    if(isPortOpen)
    {
       readMyCom();
    }
    else
    {
       timer10ms->stop();
    }

}
void MainWindow::qtimeSlot()
{
    QTime qtimeObj = QTime::currentTime();
    QString strTime = qtimeObj.toString("hh:mm:ss");
    QDate qdateObj = QDate::currentDate();
    QString strDate = qdateObj.toString("yyyy-MM-dd-dddd "); //年、月份、天、星期、
    strDate.append(strTime);
    ui->label_systime->setText(strDate);

}
void MainWindow::init_statusBar()
{
    ui->statusBar->showMessage(tr("init_statusBar"));
}

void MainWindow::setComboBox(bool status)
{
    ui->ComboBox_CheckBit->setEnabled(status);
    ui->ComboBox_BaudRate->setEnabled(status);
    ui->ComboBox_SerialPortName->setEnabled(status);
    ui->ComboBox_StopBit->setEnabled(status);
    ui->ComboBox_DateBit->setEnabled(status);
}

void MainWindow::setAction(bool status)
{
    ui->actionOpenCom ->setEnabled(status);
    ui->actionAddCom->setEnabled(status);
    ui->actionDelCom->setEnabled(status);
    ui->action_FindCom ->setEnabled(status);
    ui->actionCloseCom->setEnabled(!status);
}

//显示关于窗口
void MainWindow::on_actionAbout_triggered()
{
    aboutdlg.show();
    // 在主窗口中间显示
    int x = this->x() + (this->width() - aboutdlg.width() )/2;
    int y = this->y() + (this->height() - aboutdlg.height() )/2;
    aboutdlg.move(x,y);
    ui->statusBar->showMessage(tr("关于FreeFlyCom"));
}

//打开串口按钮
void MainWindow::on_actionOpenCom_triggered()
{
    bool res = false;
    pserial = new QSerialPort();
    currentPortName = ui->ComboBox_SerialPortName->currentText();
    ui->statusBar->showMessage(currentPortName);
    pserial->setPortName(currentPortName);
    init_SetPort(pserial);
    res = openPort(pserial);
    if(res == true)
    {
        mySendThread = new SendMyThread();
        mySendThread->initSendMyThread(pserial);
        QString obotimestr = ui->CheckBox_OBO->text();
        obotime = obotimestr.toInt();
        mySendThread->MySendThreadclose();

//        myRecvThread = new RecvMyThread();
//        myRecvThread->initRecvMyThread(pserial);
//        myRecvThread->start();

        connect(mySendThread, SIGNAL(SendMyThreadfinish(int)), this, SLOT(SendMyThreadfinish(int)));
//      timer10ms->start(1);
//      connect(myRecvThread, SIGNAL(RecvMyThreadfinish(QByteArray,int)), this, SLOT(RecvMyThreadfinish(QByteArray,int)));
        //void RecvMyThread::initSendMyThread(QSerialPort* pserial)
        connect(pserial, SIGNAL(readyRead()), this, SLOT(readMyCom()));
    }
}

void MainWindow::on_actionCloseCom_triggered()
{
    if(pserial->isOpen())
    {
        mySendThread->MySendThreadclose();
//      myRecvThread->MyRecvThreadclose();
        pserial->close();
        setComboBox(true);
        setAction(true);
        ui->PushButton_SendDate->setText(tr("发送"));
        ui->PushButton_ClearSendText->setEnabled(true);
        isPortOpen = false;
        ui->statusBar->showMessage(tr("关闭串口成功"));
        ui->PushButton_ClearSendText->setEnabled(true);
        ui->RadioButton_RecvChar->setEnabled(true);
        ui->RadioButton_RecvHex->setEnabled(true);
        ui->RadioButton_SendChar->setEnabled(true);
        ui->RadioButton_SendHex->setEnabled(true);
        ui->CheckBox_OBO->setEnabled(true);
        ui->actionClear->setEnabled(true);
        ui->action_FindCom ->setEnabled(true);
    }
}

void MainWindow::RecvMyThreadfinish(QByteArray ByteArray ,int Recvlen)
{
    QString buf;
    if(isRecvChar == false)//如果是十六进制形式接收，需要转换成字符串形式显示
    {
        QString str3 = ByteArray.toHex().data(); //以十六进制显示
        str3 = str3.toUpper ();//转换为大写
        for(int i = 0;i<str3.length ();i+=2)//填加空格
        {
            QString st = str3.mid (i,2);
            buf += st;
            buf += " ";
        }
    }
    else//如果是字符形式发送，直接显示即可
    {
        buf = QString::fromUtf8(ByteArray);
    }
    ui->TextEdit_Recv->insertPlainText(buf);
//    Recvlen = ByteArray.length();
    QTextCursor cursor = ui->TextEdit_Recv->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->TextEdit_Recv->setTextCursor(cursor);
    ui->LcdNumberRecvbytes->display(ui->LcdNumberRecvbytes->value() + Recvlen);
}

//线程发送的数据
void MainWindow::SendMyThreadfinish(int cnt)
{
    ui->LcdNumberSendbytes->display(ui->LcdNumberSendbytes->value() + cnt);
}

// 线程接收数据
void MainWindow::readMyCom()
{
    QByteArray ByteArray = pserial->readAll();
    //QByteArray temp = serial.read(1000);
    QString buf;
    if(!ByteArray.isEmpty())
    {
        if(isRecvChar == false)//如果是十六进制形式接收，需要转换成字符串形式显示
        {
            QString str3 = ByteArray.toHex().data(); //以十六进制显示
            str3 = str3.toUpper ();//转换为大写
            for(int i = 0;i<str3.length ();i+=2)//填加空格
            {
                QString st = str3.mid (i,2);
                buf += st;
                buf += " ";
            }
        }
        else//如果是字符形式发送，直接显示即可
        {
            buf = QString::fromUtf8(ByteArray);
        }
        ui->TextEdit_Recv->insertPlainText(buf);
        Recvlen = ByteArray.length();
        QTextCursor cursor = ui->TextEdit_Recv->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->TextEdit_Recv->setTextCursor(cursor);
        ui->LcdNumberRecvbytes->display(ui->LcdNumberRecvbytes->value() + Recvlen);
    }
}




//发送数据按钮
void MainWindow::on_PushButton_SendDate_clicked()
{
    bool isOBO = false;
    if(isPortOpen == false)
    {
        QMessageBox::critical(this, tr("提示消息"),tr("没有打开串口"), QMessageBox::Ok);
        return;
    }

    if(ui->PushButton_SendDate->text() == tr("暂停")) //如果按下暂停键
    {
        mySendThread->MySendThreadclose();
        ui->PushButton_SendDate->setText(tr("发送"));
        ui->PushButton_ClearSendText->setEnabled(true);
        ui->RadioButton_RecvChar->setEnabled(true);
        ui->RadioButton_RecvHex->setEnabled(true);
        ui->RadioButton_SendChar->setEnabled(true);
        ui->RadioButton_SendHex->setEnabled(true);
        ui->CheckBox_OBO->setEnabled(true);
        ui->actionClear->setEnabled(true);
        mySendThread->quit();
        return;
    }
    else//如果按下发送健
    {
        //如果发送数据为空，给出提示并返回
        if(ui->TextEdit_Send->toPlainText().isEmpty())
        {
            QMessageBox::information(this, tr("提示消息"), tr("没有需要发送的数据"), QMessageBox::Ok);
            return;
        }

        if(!ui->CheckBox_OBO->isChecked())//如果不是连续发送
        {
            isOBO = false;
        }
        else    //如果是连续发送
        {
            ui->PushButton_SendDate->setText(tr("暂停"));
            ui->PushButton_ClearSendText->setEnabled(false);
            ui->RadioButton_RecvChar->setEnabled(false);
            ui->RadioButton_RecvHex->setEnabled(false);
            ui->RadioButton_SendChar->setEnabled(false);
            ui->RadioButton_SendHex->setEnabled(false);
            ui->CheckBox_OBO->setEnabled(false);
            ui->actionClear->setEnabled(false);
            QString obotimestr = ui->SpinBoxOBO->text();
            obotime = obotimestr.toInt();
            isOBO = true;

        }
        sendMsg(isOBO);
        ui->statusBar->showMessage(tr("发送数据成功"));
    }
}

//发送数据
void MainWindow::sendMsg(bool isOBO)
{
    QByteArray temp;
    QString str;
    str = ui->TextEdit_Send->toPlainText();
    temp = str.toLocal8Bit();
    if(isSendChar == false)  //如果是十六进制形式发送，需要转换成字符串形式发送
    {
        QByteArray text = QByteArray::fromHex(temp);
        str = text.data();
        temp = str.toLocal8Bit();
    }
    else
    {
        //如果是字符形式发送，直接发送即可
    }
    Sendlen = temp.length();

    //1.发送数据 2.是否连续发送 3.发送时间间隔
    mySendThread->update(temp,isOBO,obotime);
    mySendThread->start();

}


void MainWindow::on_pushButton_ReceiveClear_clicked()
{

    ui->LcdNumberRecvbytes->display(0);
}

void MainWindow::on_pushButton__SendClear_clicked()
{

    ui->LcdNumberSendbytes->display(0);
}

void MainWindow::on_PushButton_ClearReviceText_clicked()
{
    ui->TextEdit_Recv->clear();

}

void MainWindow::on_PushButton_ClearSendText_clicked()
{
    ui->TextEdit_Send->clear();
}

void MainWindow::on_actionClear_triggered()
{
    ui->LcdNumberRecvbytes->display(0);
    ui->LcdNumberSendbytes->display(0);
    ui->TextEdit_Recv->clear();
    ui->TextEdit_Send->clear();
}



void MainWindow::on_RadioButton_SendChar_clicked()
{
    QByteArray temp;
    QString str;
    if(isSendChar == true)
    {
        return;
    }
    QTextCursor cursor = ui->TextEdit_Send->textCursor();
    str = ui->TextEdit_Send->toPlainText();
    temp = str.toLocal8Bit();
    QByteArray text = QByteArray::fromHex(temp);
    ui->TextEdit_Send->setText(text.data());
    cursor.movePosition(QTextCursor::End);
    ui->TextEdit_Send->setTextCursor(cursor);
    isSendChar= true;
}

//QByteArray text = QByteArray::fromHex("517420697320677265617421");
//text.data();            // returns "Qt is great!"
void MainWindow::on_RadioButton_SendHex_clicked()
{
    QString strDis;
    QByteArray temp;
    QString str;
    if(isSendChar == false)
    {
        return;
    }
    QTextCursor cursor = ui->TextEdit_Send->textCursor();
    str = ui->TextEdit_Send->toPlainText();
    temp = str.toLocal8Bit();
     QString str3 = temp.toHex().data(); //以十六进制显示
     str3 = str3.toUpper ();//转换为大写
     for(int i = 0;i<str3.length ();i+=2)//填加空格
     {
            QString st = str3.mid (i,2);
            strDis += st;
            strDis += " ";
      }
      ui->TextEdit_Send->setText(strDis);
      cursor.movePosition(QTextCursor::End);
      ui->TextEdit_Send->setTextCursor(cursor);
      isSendChar = false;
}


void MainWindow::on_RadioButton_RecvChar_clicked()
{
    QByteArray temp;
    QString str;
    if(isRecvChar == true)
    {
        return;
    }
    QTextCursor cursor = ui->TextEdit_Recv->textCursor();
    str = ui->TextEdit_Recv->toPlainText();
    temp = str.toLocal8Bit();
    QByteArray text = QByteArray::fromHex(temp);
    ui->TextEdit_Recv->setText(text.data());
    cursor.movePosition(QTextCursor::End);
    ui->TextEdit_Recv->setTextCursor(cursor);
    isRecvChar= true;
}

void MainWindow::on_RadioButton_RecvHex_clicked()
{

    QString strDis;
    QByteArray temp;
    QString str;
    if(isRecvChar == false)
    {
        return;
    }
    QTextCursor cursor = ui->TextEdit_Recv->textCursor();
    str = ui->TextEdit_Recv->toPlainText();
    temp = str.toLocal8Bit();
    QString str3 = temp.toHex().data(); //以十六进制显示
    str3 = str3.toUpper ();//转换为大写
    for(int i = 0;i<str3.length ();i+=2)//填加空格
    {
        QString st = str3.mid (i,2);
        strDis += st;
        strDis += " ";
    }
    ui->TextEdit_Recv->setText(strDis);
    cursor.movePosition(QTextCursor::End);
    ui->TextEdit_Recv->setTextCursor(cursor);
    isRecvChar= false;
}



void MainWindow::on_actionAddCom_triggered()
{
    bool ok = false;
    QString portname;

    portname = QInputDialog::getText(this, tr("添加串口"), tr("设备文件名"), QLineEdit::Normal, 0, &ok);
    if(ok && !portname.isEmpty())
    {
        ui->ComboBox_SerialPortName ->addItem(portname);
        ui->statusBar->showMessage(tr("添加串口成功"));
    }
}

void MainWindow::on_actionDelCom_triggered()
{
    qint16 index =0;
    index = ui->ComboBox_SerialPortName ->currentIndex() ;
    ui->ComboBox_SerialPortName ->removeItem(index);
}

void MainWindow::on_action_FindCom_triggered()
{
    ui->ComboBox_SerialPortName->clear();
    init_FindPort();
}

void MainWindow::on_CheckBox_OBO_clicked()
{
    if(ui->CheckBox_OBO->isChecked())
    {
        ui->SpinBoxOBO->setEnabled(false);
    }
    else
    {
        ui->SpinBoxOBO->setEnabled(true);
    }
}
