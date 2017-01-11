#ifndef SENDMYTHREAD_H
#define SENDMYTHREAD_H

#include <QThread>
#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>

class SendMyThread : public QThread
{
    Q_OBJECT
public:
    SendMyThread();
    ~SendMyThread();

    void initSendMyThread(QSerialPort* pserial);
    void update(QByteArray pByteArray,bool isOBO,int pdelaytime);
    void MySendThreadclose();

private:
    QSerialPort* myserial;
    QByteArray ByteArray;
    int delaytime;
    bool isSend;
    bool isSendContinue;

protected:
    void run();
signals:
    void SendMyThreadfinish(int);

};

#endif // MYTHREAD_H
