#include "mysendthread.h"
#include <QDebug>

//#include <QMainWindow>

//#include "mainwindow.h"

SendMyThread::SendMyThread()
{
    isSend = 0;
}

void SendMyThread::initSendMyThread(QSerialPort* pserial)
{
    myserial = pserial;
}

void SendMyThread::update(QByteArray pByteArray,bool isOBO,int pdelaytime)
{
    ByteArray = pByteArray;
    isSend = true;
    isSendContinue = isOBO;
    if(pdelaytime == 0)
    {
       delaytime = 1000;
    }
    else
    {
       delaytime = pdelaytime;
    }

}

void SendMyThread::MySendThreadclose()
{
    isSend = 0;
}

void SendMyThread::run()
{
    while(isSend)
    {
        myserial->write(ByteArray);
        myserial->flush();
        emit SendMyThreadfinish(ByteArray.length());
        if(isSendContinue == true)
        {
             QThread::msleep(delaytime);//是连续发送
        }
        else
        {
            break; //不是连续发送
        }
    }
}

SendMyThread::~SendMyThread()
{

}
