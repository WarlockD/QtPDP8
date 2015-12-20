#include "mainwindow.h"
#include <QApplication>
#include "wiretest.h"
#include <QMutex>
#include <QDebug>
#include <QDateTime>
#include <QString>

#include <Windows.h>
#include <mutex>
#include <deque>
#include <atomic>
#include <string>
#include <thread>

std::mutex g_m;
std::deque<std::wstring> que;
std::atomic<bool> endcond = false;
static QMutex msg_handler_mutex;
// not ideal but only way I can figure it out
void queue_debug(std::wstring msg)
{
  std::lock_guard<std::mutex> _(g_m);
  que.push_back(msg);
}
std::vector<std::wstring> &split(const std::wstring &s, wchar_t delim, std::vector<std::wstring> &elems) {
    std::wstringstream ss(s);
    std::wstring item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::wstring> split(const std::wstring &s, wchar_t delim) {
    std::vector<std::wstring> elems;
    split(s, delim, elems);
    return elems;
}

void print_from_queue()
{
  while ( !endcond )
  {
    if ( que.size() )
    {
      std::lock_guard<std::mutex> _(g_m);
      std::wstring msg = que.front();
      que.pop_front();
      auto lines = split(msg,'\n');
       OutputDebugString(reinterpret_cast<const wchar_t *>(msg.c_str()));
     // OutputDebugStringA(msg.c_str());
    }
  }
}
void test_debug(const QString& msg) {
     queue_debug(reinterpret_cast<const wchar_t *>(msg.utf16()));
}

void MessageHandler(QtMsgType type, const QMessageLogContext & context, const QString & msg)
{
    msg_handler_mutex.lock();
    // check the string if we have lines
    QStringList list = msg.split('\n');
    if(list.length() > 0) {
        for(QString s : list) OutputDebugString(reinterpret_cast<const wchar_t *>(s.utf16()));
    } else {
        OutputDebugString(reinterpret_cast<const wchar_t *>(msg.utf16()));
    }
   // queue_debug(reinterpret_cast<const wchar_t *>(msg.utf16()));
    /*
    QDateTime dateTime(QDateTime::currentDateTime());

    QString timeStr(dateTime.toString("dd-MM-yyyy HH:mm:ss:zzz"));
    QString contextString(QString("(%1, %2)").arg(context.file).arg(context.line));
   // QString data;
    QFile outFile("file.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);

    QTextStream stream(&outFile);
   // QTextStream s(&data);
    stream << timeStr << " " << contextString << ": " << msg << endl;
  //  OutputDebugString();
  */
    msg_handler_mutex.unlock();
}

int main(int argc, char *argv[])
{
   // auto thr = std::async( std::launch::async, print_from_queue ); // message pump if we need
    qInstallMessageHandler(MessageHandler);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
