//#include "myrecvthread.h"

//RecvMyThread::RecvMyThread()
//{

//}

//void RecvMyThread::MyRecvThreadclose()
//{
//    runflag = 0 ;
//}

//void RecvMyThread::initRecvMyThread(QSerialPort* pserial)
//{
//    myserial = pserial;
//    runflag =1;
//    //connect(myserial, SIGNAL(readyRead()), this, SLOT(readMyCom()));
//}

//void RecvMyThread::run()
//{
//    qDebug("RecvMyThread::run");
//    while(runflag)
//    {
//       sendCnt = myserial->bytesAvailable();
//       if(sendCnt > 0 )
//       {
//            QByteArray ByteArray = myserial->readAll();
//            qDebug("length = %d",ByteArray.length());
//            emit RecvMyThreadfinish(ByteArray ,ByteArray.length());
//        }

//        QThread::msleep(1);
//    }
//    qDebug("RecvMyThread::out");
//}

//RecvMyThread::~RecvMyThread()
//{


//}
