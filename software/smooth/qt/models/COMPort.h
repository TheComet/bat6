#ifndef COM_UNIX_H
#define COM_UNIX_H

#include <QString>
#include <QThread>

class ListenerThread : public QThread
{
    Q_OBJECT

signals:
    void byteReceived(char byte);

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
    bool send(const char* data) const;

private slots:
    void onByteReceived(char data);

private:

    ListenerThread* listenerThread;
    int fd;
};

#endif // COM_UNIX_H
