#ifndef COM_UNIX_H
#define COM_UNIX_H

#include <QString>
#include <QThread>

class ListenerThread : public QThread
{
    Q_OBJECT

signals:
    void byteReceived(char byte);
    void fileDescriptorInvalidated();

public:
    explicit ListenerThread(QObject* parent, int fd);
    void exit();

private:
    virtual void run() override;

    int fd;
    volatile bool isRunning;
};

class COMPort : public QObject
{
    Q_OBJECT

signals:
    void dataReceived(const char* data);

public:
    COMPort();
    ~COMPort();

    bool open(const QString& comName);
    void close();
    bool send(const char* data);

private slots:
    void onByteReceived(char data);
    void onFileDescriptorUnexpectedlyClosed();

private:
    void setPortName(const QString& portName);
    void startListenerThread();
    void stopListenerThread();
    bool openFileDescriptor();
    void closeFileDescriptor();
    bool attemptReOpen();

    ListenerThread* listenerThread;
    int fd;
    QString portName;
};

#endif // COM_UNIX_H
